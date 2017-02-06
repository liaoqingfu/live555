/*
	功能描述：	一个简单的RTSP/RTP对接功能，从RTSP源通过基本的RTSPClient流程，获取到RTP流媒体数据
				再通过标准RTSP推送过程（ANNOUNCE/SETUP/PLAY），将获取到RTP数据推送给Darwin流媒体
				分发服务器。
				此Demo只演示了单个源的转换、推送功能!
				
	Author：	sunpany@qq.com
	时间：		2014/06/25
*/

#include "BasicUsageEnvironment.hh"
#include "RTSPCommon.hh"
#include "liveMedia.hh"
#include "MatroskaFileServerDemux.hh"
#include "../liveMedia/MatroskaFileServerMediaSubsession.hh"
#include "../liveMedia/include/MP3AudioFileServerMediaSubsession.hh"
#include "../liveMedia/MP3AudioMatroskaFileServerMediaSubsession.hh"

char* server = "192.168.22.124"; //RTSP流媒体转发服务器地址，<请修改为自己搭建的流媒体服务器地址>
int port = 11554; //RTSP流媒体转发服务器端口，<请修改为自己搭建的流媒体服务器端口>
char* streamName = "media.sdp"; //流名称，推送到Darwin的流名称必须以.sdp结尾
//char* src = "rtsp://218.204.223.237:554/live/1/66251FC11353191F/e7ooqwcfbqjoo80j.sdp";	//源端URL
char* src = "rtsp://admin:admin123@172.16.34.214/av_stream.sdp"; //源端URL

UsageEnvironment* env = NULL; //live555 global environment
TaskScheduler* scheduler = NULL;
char eventLoopWatchVariable = 0;

DarwinInjector* injector = NULL; //DarwinInjector
FramedSource* vSource = NULL; //Video Source
FramedSource* aSource = NULL; //Audio Source

RTPSink* vSink = NULL; //Video Sink
RTPSink* aSink = NULL; //Audio Sink

Groupsock* rtpGroupsockVideo = NULL; //Video Socket
Groupsock* rtpGroupsockAudio = NULL; //Video Socket

ProxyServerMediaSession* sms = NULL; //proxy session

// 流转发过程
bool RedirectStream(char const* ip, unsigned port, const char *fileName);

// 流转发结束后处理回调
void afterPlaying(void* clientData);

// 实现等待功能
void sleep(void* clientSession)
{
    char* var = (char*)clientSession;
    *var = ~0;
}

// Special code for handling Matroska files:
struct MatroskaDemuxCreationState {
    MatroskaFileServerDemux* demux;
    char watchVariable;
};

static void onMatroskaDemuxCreation(MatroskaFileServerDemux* newDemux, void* clientData)
{
    MatroskaDemuxCreationState* creationState = (MatroskaDemuxCreationState*)clientData;
    creationState->demux = newDemux;
    creationState->watchVariable = 1;
}

// Main
int main(int argc, char** argv)
{
    // 初始化基本的live555环境
    scheduler = BasicTaskScheduler::createNew();
    env = BasicUsageEnvironment::createNew(*scheduler);

    // 开始转发流程
    RedirectStream(server, port, argv[1]);
    *env << "after RedirectStream\n";

    env->taskScheduler().doEventLoop(&eventLoopWatchVariable);
    *env << "after doEventLoop\n";

    return 0;
}

// 推送视频到流媒体服务器
bool RedirectStream(char const* ip, unsigned port, const char *fileName)
{
    // DarwinInjector主要用于向Darwin推送RTSP/RTP数据
    injector = DarwinInjector::createNew(*env);

    struct in_addr dummyDestAddress;
    dummyDestAddress.s_addr = 0;
    rtpGroupsockVideo = new Groupsock(*env, dummyDestAddress, 6700, 255);

    struct in_addr dummyDestAddressAudio;
    dummyDestAddressAudio.s_addr = 0;
    rtpGroupsockAudio = new Groupsock(*env, dummyDestAddressAudio, 6702, 255);

    *env << "beforeCreateMediaSource, fileName = " << fileName << "\n";
    MatroskaDemuxCreationState creationState;
    creationState.watchVariable = 0;

    // 创建MatroskaFileServerDemux并parse it
    MatroskaFileServerDemux::createNew(*env, fileName, onMatroskaDemuxCreation, &creationState);
    env->taskScheduler().doEventLoop(&creationState.watchVariable);
    MatroskaFileServerMediaSubsession* vSubsession;
    if ((vSubsession = (MatroskaFileServerMediaSubsession *)creationState.demux->newServerMediaSubsession()) == NULL) {
        *env << "MatroskaFileServerMediaSubsession creationState.demux->newServerMediaSubsession() failed! " << "\n";
        exit(-1);
    }

    MP3AudioMatroskaFileServerMediaSubsession* aSubsession;
    if ((aSubsession = (MP3AudioMatroskaFileServerMediaSubsession *)creationState.demux->newServerMediaSubsession()) == NULL) {
        *env << "MP3AudioMatroskaFileServerMediaSubsession creationState.demux->newServerMediaSubsession() failed! " << "\n";
        exit(-1);
    }

    unsigned estBitrate=87543408;
    vSource = vSubsession->createNewStreamSource(0, estBitrate);
    if (vSource == NULL) {
        *env << "MatroskaFileServerMediaSubsession subsession->createNewStreamSource(vSource) failed! exit... " << "\n";
        exit(-1);
    }

    estBitrate = 500;
    aSource = aSubsession->createNewStreamSource(0, estBitrate);
    if (aSource == NULL) {
        *env << "MP3AudioMatroskaFileServerMediaSubsession subsession->createNewStreamSource(aSource) failed! exit... " << "\n";
        exit(-1);
    }

    unsigned char rtpPayloadType = 96;  // video
    vSink = vSubsession->createNewRTPSink(rtpGroupsockVideo, rtpPayloadType, NULL);
    if (vSink == NULL) {
        *env << "subsession->createNewRTPSink(vSink) failed! exit... " << "\n";
        exit(-1);
    }

    aSink = aSubsession->createNewRTPSink(rtpGroupsockAudio, rtpPayloadType + 1, NULL);
    if (aSink == NULL) {
        *env << "subsession->createNewRTPSink(aSink) failed! exit... " << "\n";
        exit(-1);
    }

    // 将Video的RTPSink赋值给DarwinInjector，推送视频RTP给Darwin
    injector->addStream(vSink, NULL);
    injector->addStream(aSink, NULL);

    // RTSP ANNOUNCE/SETUP/PLAY推送过程
    if (!injector->setDestination(ip, streamName, "live555", "LIVE555", port)) {
        *env << "injector->setDestination() failed: " << env->getResultMsg() << "\n";
        return false;
    }

    // 开始转发视频RTP数据
    if ((vSink != NULL) && (vSource != NULL)) {
        *env << "vSink->startPlaying\n";
        vSink->startPlaying(*vSource, afterPlaying, vSink);
    }

    // 开始转发音频RTP数据
    if ((aSink != NULL) && (aSource != NULL)) {
        *env << "aSink->startPlaying\n";
        aSink->startPlaying(*aSource, afterPlaying, aSink);
    }

    *env << "\nBeginning to get camera video...\n";
    return true;
}

// 停止推送，释放所有变量
void afterPlaying(void* clientData)
{
    *env << "afterPlaying\n";
    if (clientData == NULL)
        return;

    if (vSink != NULL)
        vSink->stopPlaying();

    if (vSink != NULL)
        aSink->stopPlaying();

    if (injector != NULL) {
        Medium::close(*env, injector->name());
        injector = NULL;
    }

    if (vSink != NULL)
        Medium::close(vSink);

    if (aSink != NULL)
        Medium::close(aSink);

    if (vSource != NULL)
        Medium::close(vSource);

    if (aSource != NULL)
        Medium::close(aSource);

    delete rtpGroupsockVideo;
    delete rtpGroupsockAudio;
    rtpGroupsockVideo = NULL;
    rtpGroupsockAudio = NULL;
}
