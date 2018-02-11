#include "nupusherhandler.h"
#include "BasicUsageEnvironment.hh"
#include "RTSPCommon.hh"
#include "liveMedia.hh"
#include "MatroskaFileServerDemux.hh"
#include "../liveMedia/MatroskaFileServerMediaSubsession.hh"
#include "../liveMedia/include/MP3AudioFileServerMediaSubsession.hh"
#include "../liveMedia/MP3AudioMatroskaFileServerMediaSubsession.hh"
#include "../liveMedia/include/NuVideoServerMediaSubsession.hh"
#include "../liveMedia/include/NuH264VideoStreamFramer.hh"

#define LOG_TAG     "NUPUSHER_HANDLER"
#include "utils/Log.h"

NuPusherHandler::NuPusherHandler()
{
    ALOGDTRACE();
    fScheduler = BasicTaskScheduler::createNew();
    fEnv = BasicUsageEnvironment::createNew(*fScheduler); 
    fAvQueue = NULL;
    //fAvQueue = new SS_QUEUE_OBJ_T(); // 帧缓冲队列
    //SSQ_Init(fAvQueue, 0x00, 0, TEXT(""), MAX_AVQUEUE_SIZE, 2, 0x01);
    //SSQ_Clear(fAvQueue);
}

NuPusherHandler::~NuPusherHandler()
{
    releaseOurselves();
}

// 释放自己创建的资源
void
NuPusherHandler::releaseOurselves()
{
    if (fIsPlaying) {
        if (fVideoSink != NULL) {
            fVideoSink->stopPlaying();
            Medium::close(fVideoSink);
        }
        if (fAudioSink != NULL) {
            fAudioSink->stopPlaying();
            Medium::close(fAudioSink);
        }
    }

    if (fInjector != NULL) {
        Medium::close(*fEnv, fInjector->name());
        fInjector = NULL;
    }

    if (fVideoSource != NULL)
        Medium::close(fVideoSource);

    if (fAudioSource != NULL)
        Medium::close(fAudioSource);

    delete fVideoRtpGroupsock;
    delete fAudioRtpGroupsock;
    fVideoRtpGroupsock = NULL;
    fAudioRtpGroupsock = NULL;
}


// 停止推送，释放所有变量
static void afterPlaying(void* clientData)
{
    if (clientData == NULL)
        return;

    NuPusherHandler *handler = (NuPusherHandler *)clientData;
    handler->releaseOurselves();
}

// 注册回调
void
NuPusherHandler::registerCallBack(NuPusherRTSP_Callback cb)
{
    ALOGDTRACE();
    fUserCallBack = cb;
}

void
NuPusherHandler::setUserPointer(void* ptr)
{
    fUserPtr = ptr;
}

void*
NuPusherHandler::getUserPointer()
{
    return fUserPtr;
}

void
NuPusherHandler::setId(int id)
{
    fId = id;
}

int
NuPusherHandler::getId()
{
    return fId;
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

void
NuPusherHandler::connStateCallBack(int state, char* resultStr)
{
    if (!fUserCallBack)
        return;

    ALOGDTRACE();
    ALOGD("%s: %s[%d]\n", __FUNCTION__, resultStr, state);
    switch (state) {
    case 0:
        fUserCallBack(fId, NU_PUSH_STATE_CONNECTED, NULL, fUserPtr);
        break;
    case -10057:
        fUserCallBack(fId, NU_PUSH_STATE_CONNECT_FAILED, NULL, fUserPtr);
        break;
    default:
        fUserCallBack(fId, NU_PUSH_STATE_ERROR, NULL, fUserPtr);
        break;
    }
}

NU_U32
NuPusherHandler::startStream(char* serverAddr, NU_U16 port, char* streamName, int rtpOverTcp, char *username,
    char *password, NU_MEDIA_INFO_T*  pstruStreamInfo, NU_U32 bufferKSize, NU_Bool createlogfile)
{
    fPort = port;
    fServerAddr = serverAddr;
    fUsername = username;
    fPassword = password;
    fRtpOverTcp = rtpOverTcp;
    fStreamName = streamName;
    fBufferKSize = bufferKSize;
    fCreatelogfile = createlogfile;
    fPstruStreamInfo = pstruStreamInfo;

    ALOGDTRACE();
    fInjector = DarwinInjector::createNew(*fEnv);
    fInjector->registerConnectStateCallBack(this);
    struct in_addr dummyDestAddress;
    dummyDestAddress.s_addr = 0;
    fVideoRtpGroupsock = new Groupsock(*fEnv, dummyDestAddress, 0, 0);
    struct in_addr dummyDestAddressAudio;
    dummyDestAddressAudio.s_addr = 0;
    fAudioRtpGroupsock = new Groupsock(*fEnv, dummyDestAddressAudio, 0, 0);

    unsigned char testRtpPayloadType = 96;
    unsigned testEstBitrate = 0;
    NuVideoServerMediaSubsession* nuVSubsession = NuVideoServerMediaSubsession::createNew(*fEnv, false);
    RTPSink* nuRtpSink = nuVSubsession->createNewRTPSink(fVideoRtpGroupsock, testRtpPayloadType, NULL);
    FramedSource* nuSource = nuVSubsession->createNewStreamSource(0, testEstBitrate);

    const char* fileName = "Robotica_1080.mkv";
    ALOGD("beforeCreateMediaSource, fileName = %s\n", fileName);
    MatroskaDemuxCreationState creationState;
    creationState.watchVariable = 0;

    // 创建MatroskaFileServerDemux并parse it
    MatroskaFileServerDemux::createNew(*fEnv, fileName, onMatroskaDemuxCreation, &creationState);
    fEnv->taskScheduler().doEventLoop(&creationState.watchVariable);
    MatroskaFileServerMediaSubsession* vSubsession;
    if ((vSubsession = (MatroskaFileServerMediaSubsession *)creationState.demux->newServerMediaSubsession()) == NULL) {
        ALOGD("MatroskaFileServerMediaSubsession creationState.demux->newServerMediaSubsession() failed!\n");
        return -1;
    }

    MP3AudioMatroskaFileServerMediaSubsession* aSubsession;
    if ((aSubsession = (MP3AudioMatroskaFileServerMediaSubsession *)creationState.demux->newServerMediaSubsession()) == NULL) {
        ALOGD("MP3AudioMatroskaFileServerMediaSubsession creationState.demux->newServerMediaSubsession() failed! \n");
        return -1;
    }

    unsigned estBitrate = 87543408;
    FramedSource* vSource = vSubsession->createNewStreamSource(0, estBitrate);
    if (vSource == NULL) {
        ALOGD("MatroskaFileServerMediaSubsession subsession->createNewStreamSource(vSource) failed! exit...\n");
        return -1;
    }

    estBitrate = 500;
    FramedSource* aSource = aSubsession->createNewStreamSource(0, estBitrate);
    if (aSource == NULL) {
        ALOGD("MP3AudioMatroskaFileServerMediaSubsession subsession->createNewStreamSource(aSource) failed! exit...\n");
        return -1;
    }

    unsigned char rtpPayloadType = 96;  // video
    fVideoSink = vSubsession->createNewRTPSink(fVideoRtpGroupsock, rtpPayloadType, NULL);
    if (fVideoSink == NULL) {
        ALOGD("subsession->createNewRTPSink(vSink) failed! exit...\n");
        return -1;
    }

    fAudioSink = aSubsession->createNewRTPSink(fAudioRtpGroupsock, rtpPayloadType + 1, NULL);
    if (fAudioSink == NULL) {
        ALOGD("subsession->createNewRTPSink(aSink) failed! exit... \n");
        return -1;
    }

    // 将Video的RTPSink赋值给DarwinInjector，推送视频RTP给Darwin
    fInjector->addStream(fVideoSink, NULL);
    fInjector->addStream(fAudioSink, NULL);

    // RTSP ANNOUNCE/SETUP/PLAY推送过程
    if (!fInjector->setDestination("192.168.22.124", fStreamName, "live555", "LIVE555", 11554)) {
        ALOGD("fInjector->setDestination() failed: %s\n", fEnv->getResultMsg());
        return -1;
    }

    // 开始转发视频RTP数据
    if ((fVideoSink != NULL) && (vSource != NULL)) {
        ALOGD("fVideoSink->startPlaying\n");
        fVideoSink->startPlaying(*vSource, afterPlaying, this);
    }

    // 开始转发音频RTP数据
    if ((fAudioSink != NULL) && (aSource != NULL)) {
        ALOGD("fAudioSink->startPlaying\n");
        fAudioSink->startPlaying(*aSource, afterPlaying, this);
    }

    ALOGD("Beginning to get camera video...\n");
    return 0;
}

NU_U32
NuPusherHandler::addFrame(NU_AV_Frame* frame)
{
    ALOGDTRACE();
    if (fAvQueue == NULL) {
        ALOGD("%s, -------------SSQ_Init CREATE fAvQueue\n", __FUNCTION__);
        fAvQueue = new SS_QUEUE_OBJ_T(); // 帧缓冲队列
        SSQ_Init(fAvQueue, 0x00, fId, TEXT(""), MAX_AVQUEUE_SIZE, 2, 0x01);
        SSQ_Clear(fAvQueue);
    }
    MEDIA_FRAME_INFO frameInfo;
    frameInfo.codec = frame->u32AVFrameFlag;
    frameInfo.type = frame->u32VFrameType;
    frameInfo.timestamp_sec = frame->u32TimestampSec;
    frameInfo.timestamp_usec = frame->u32TimestampUsec;
    frameInfo.length = frame->u32AVFrameLen;
    frameInfo.channels = fPstruStreamInfo->u32AudioChannel;
    frameInfo.sample_rate = fPstruStreamInfo->u32AudioSamplerate;
    frameInfo.bits_per_sample = fPstruStreamInfo->u32AudioBitsPerSample;
    frameInfo.channels = fPstruStreamInfo->u32AudioChannel;

    ALOGD("%s, -------------0000\n", __FUNCTION__);
    int ret = SSQ_AddData(fAvQueue, fId, MEDIA_TYPE_VIDEO, &frameInfo, (char *)frame->pBuffer);
    ALOGD("%s, SSQ_AddData = %d\n", __FUNCTION__, ret);
    return ret;
}

NU_U32
NuPusherHandler::stopStream()
{
    releaseOurselves();
    return 0;
}