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

#if 1
char* server = "192.168.22.124"; //RTSP流媒体转发服务器地址，<请修改为自己搭建的流媒体服务器地址>
int port = 11554; //RTSP流媒体转发服务器端口，<请修改为自己搭建的流媒体服务器端口>
char* streamName = "av_stream.sdp"; //流名称，推送到Darwin的流名称必须以.sdp结尾
//char* src = "rtsp://218.204.223.237:554/live/1/66251FC11353191F/e7ooqwcfbqjoo80j.sdp";	//源端URL
char* src = "rtsp://admin:admin123@172.16.34.214/av_stream.sdp"; //源端URL
#else 
char* server = "192.168.2.243"; //RTSP流媒体转发服务器地址，<请修改为自己搭建的流媒体服务器地址>
int port = 10554; //RTSP流媒体转发服务器端口，<请修改为自己搭建的流媒体服务器端口>
char* streamName = "av_stream.sdp"; //流名称，推送到Darwin的流名称必须以.sdp结尾
//char* src = "rtsp://218.204.223.237:554/live/1/66251FC11353191F/e7ooqwcfbqjoo80j.sdp";	//源端URL
char* src = "rtsp://113.108.146.203:10554/679114.sdp"; //源端URL
#endif

UsageEnvironment* env = NULL; //live555 global environment
TaskScheduler* scheduler = NULL;
char eventLoopWatchVariable = 0;

DarwinInjector* injector = NULL; //DarwinInjector
FramedSource* vSource = NULL; //Video Source
FramedSource* aSource = NULL; //Audio Source

RTPSink* vSink = NULL; //Video Sink
RTPSink* aSink = NULL; //Audio Sink

Groupsock* rtpGroupsockVideo = NULL; //Video Socket
Groupsock* rtpGroupsockAudio = NULL; //Audio Socket

ProxyServerMediaSession* sms = NULL; //proxy session

// 流转发过程
bool RedirectStream(char const* ip, unsigned port);

// 流转发结束后处理回调
void afterPlaying(void* clientData);

// 实现等待功能
void sleep(void* clientSession)
{
    char* var = (char*)clientSession;
    *var = ~0;
}

// Main
int main(int argc, char** argv)
{
    // 初始化基本的live555环境
    scheduler = BasicTaskScheduler::createNew();
    env = BasicUsageEnvironment::createNew(*scheduler);

    // 新建转发SESSION
    if (argc != 2) {
        *env << "pls input your source sdp" << "\n";
        return -1;
    }
    char *newsrc = argv[1];
    sms = ProxyServerMediaSession::createNew(*env, NULL, newsrc);

    // 循环等待转接程序与源端连接成功
    while (sms->numSubsessions() <= 0) {
        char fWatchVariable = 0;
        env->taskScheduler().scheduleDelayedTask(2 * 1000000, (TaskFunc*)sleep, &fWatchVariable);
        *env << "before doEventLoop\n";
        env->taskScheduler().doEventLoop(&fWatchVariable);
        *env << "after doEventLoop\n";
    }

    *env << "before RedirectStream\n";
    // 开始转发流程
    RedirectStream(server, port);
    *env << "after RedirectStream\n";

    env->taskScheduler().doEventLoop(&eventLoopWatchVariable);
    *env << "after doEventLoop\n";

    return 0;
}

// 推送视频到流媒体服务器
bool RedirectStream(char const* ip, unsigned port)
{
    // 转发SESSION必须保证存在
    if (sms == NULL)
        return false;

    // 判断sms是否已经连接上源端
    if (sms->numSubsessions() <= 0) {
        *env << "sms numSubsessions() == 0\n";
        return false;
    }

    // DarwinInjector主要用于向Darwin推送RTSP/RTP数据
    injector = DarwinInjector::createNew(*env);

    struct in_addr dummyDestAddress;
    dummyDestAddress.s_addr = 0;
    rtpGroupsockVideo = new Groupsock(*env, dummyDestAddress, 0, 0);

    struct in_addr dummyDestAddressAudio;
    dummyDestAddressAudio.s_addr = 0;
    rtpGroupsockAudio = new Groupsock(*env, dummyDestAddressAudio, 0, 0);

    ServerMediaSubsession* subsession = NULL;
    ServerMediaSubsessionIterator iter(*sms);
    while ((subsession = iter.next()) != NULL) {
        ProxyServerMediaSubsession* proxySubsession = (ProxyServerMediaSubsession*)subsession;

        unsigned streamBitrate;
        FramedSource* source = proxySubsession->createNewStreamSource(1, streamBitrate);

        if (strcmp(proxySubsession->mediumName(), "video") == 0) {
            *env << " proxySubsession->mediumName() video\n";
            // 用ProxyServerMediaSubsession建立Video的RTPSource
            vSource = source;
            unsigned char rtpPayloadType = proxySubsession->rtpPayloadFormat();
            *env << "video: rtpPayloadType = " << rtpPayloadType << "\n";
            // 建立Video的RTPSink
            vSink = proxySubsession->createNewRTPSink(rtpGroupsockVideo, rtpPayloadType, source);
            // 将Video的RTPSink赋值给DarwinInjector，推送视频RTP给Darwin
            injector->addStream(vSink, NULL);
        } else {
            *env << " proxySubsession->mediumName() audio\n";
            // 用ProxyServerMediaSubsession建立Audio的RTPSource
            aSource = source;
            unsigned char rtpPayloadType = proxySubsession->rtpPayloadFormat();
            *env << "audio: rtpPayloadType = " << rtpPayloadType << "\n";
            // 建立Audio的RTPSink
            aSink = proxySubsession->createNewRTPSink(rtpGroupsockVideo, rtpPayloadType, source);
            // 将Audio的RTPSink赋值给DarwinInjector，推送音频RTP给Darwin
            injector->addStream(aSink, NULL);
        }
    }

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

    if (aSink != NULL)
        aSink->stopPlaying();

    if (injector != NULL) {
        Medium::close(*env, injector->name());
        injector = NULL;
    }

    ServerMediaSubsession* subsession = NULL;
    ServerMediaSubsessionIterator iter(*sms);
    while ((subsession = iter.next()) != NULL) {
        ProxyServerMediaSubsession* proxySubsession = (ProxyServerMediaSubsession*)subsession;
        if (strcmp(proxySubsession->mediumName(), "video") == 0)
            proxySubsession->closeStreamSource(vSource);

        else
            proxySubsession->closeStreamSource(aSource);
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
    rtpGroupsockVideo = NULL;
    delete rtpGroupsockAudio;
    rtpGroupsockAudio = NULL;
}
