#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
// #include <GroupsockHelper.hh>
// #include "RTSPCommon.hh"
// #include "..MatroskaFileServerMediaSubsession.hh"

char* server = "192.168.22.124"; //RTSP流媒体转发服务器地址，<请修改为自己搭建的流媒体服务器地址>
int port = 11554; //RTSP流媒体转发服务器端口，<请修改为自己搭建的流媒体服务器端口>
char* streamName = "av_stream.sdp"; //流名称，推送到Darwin的流名称必须以.sdp结尾
char* filename = "Robotica_1080.mkv";

UsageEnvironment* env;
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

MatroskaFile* matroskaFile;
MatroskaDemux* matroskaDemux;

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
// To make the second and subsequent client for each stream reuse the same
// input stream as the first client (rather than playing the file from the
// start for each client), change the following "False" to "True":
Boolean reuseFirstSource = False;

static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
			   char const* streamName); // fwd

int main(int argc, char** argv) {
    // Begin by setting up our usage environment:
    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
    env = BasicUsageEnvironment::createNew(*scheduler);

    UserAuthenticationDatabase* authDB = NULL;
    // Create the RTSP server.  Try first with the default port number (554),
    // and then with the alternative port number (8554):
    RTSPServer* rtspServer;
    portNumBits rtspServerPortNum = 554;
    rtspServer = RTSPServer::createNew(*env, rtspServerPortNum, authDB);
    if (rtspServer == NULL) {
        rtspServerPortNum = 8554;
        rtspServer = RTSPServer::createNew(*env, rtspServerPortNum, authDB);
    }
    if (rtspServer == NULL) {
        *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
        exit(1);
    }

    // Set up each of the possible streams that can be served by the
    // RTSP server.  Each such stream is implemented using a
    // "ServerMediaSession" object, plus one or more
    // "ServerMediaSubsession" objects for each audio/video substream.
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, filename,
				      "Matroska video+audio+(optional)subtitles");
    sms->addSubsession(MatroskaFileServerMediaSubsession::createNew(*env, reuseFirstSource));
    rtspServer->addServerMediaSession(sms);

    char* url = rtspServer->rtspURL(sms);
    *env << "url = " << url << "\n\t" ;


    env->taskScheduler().doEventLoop(); // does not return

    return 0; // only to prevent compiler warning

}

