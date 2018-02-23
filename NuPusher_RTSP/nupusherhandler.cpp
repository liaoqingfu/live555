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
#include <windows.h>  

#define LOG_TAG     "NUPUSHER_HANDLER"
#include "utils/Log.h"

NuPusherHandler::NuPusherHandler()
{
    ALOGDTRACE();
    fScheduler = BasicTaskScheduler::createNew();
    fEnv = BasicUsageEnvironment::createNew(*fScheduler); 
    fPushThread = new THREAD_OBJ();
    fPushThread->flag = __THREAD_STATE::THREAD_STOP;
    fAvQueue = new SS_QUEUE_OBJ_T(); // ֡�������
    SSQ_Init(fAvQueue, 0x00, fId, TEXT(""), MAX_AVQUEUE_SIZE, 2, 0x01);
    SSQ_Clear(fAvQueue);
}

NuPusherHandler::~NuPusherHandler()
{
    releaseOurselves();
    if (fPushThread)
        delete fPushThread;

    if (fAvQueue != NULL) {
        SSQ_Deinit(fAvQueue);
        delete fAvQueue;
        fAvQueue = NULL;
    }
}

// �ͷ��Լ���������Դ
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

    if (fAvQueue != NULL) {
        SSQ_Clear(fAvQueue);
    }

    delete fVideoRtpGroupsock;
    delete fAudioRtpGroupsock;
    fVideoRtpGroupsock = NULL;
    fAudioRtpGroupsock = NULL;
}


// ֹͣ���ͣ��ͷ����б���
static void afterPlaying(void* handle)
{
    if (handle == NULL)
        return;

    NuPusherHandler *handler = (NuPusherHandler *)handle;
    handler->releaseOurselves();
}

// ע��ص�
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
LPTHREAD_START_ROUTINE NuPusherHandler::startPushThreadFunc(LPVOID _pParam)
{
    ALOGD("%s ---------------------------------------------start\n", __FUNCTION__);
    NuPusherHandler *handle = (NuPusherHandler*)_pParam;
    handle->fPushThread->flag = __THREAD_STATE::THREAD_START;
    handle->fEventLoopWatchVariable = 0;
    while (handle->fAvQueue->pQueHeader->videoframes <= 0) { // �ȴ�֡����䵽�����������
        ALOGD("%s ---------------------------------------------start000000\n", __FUNCTION__);
        Sleep(100);
    }
    // ��ʼת����ƵRTP����
    if ((handle->fVideoSink != NULL) && (handle->fVideoSource != NULL)) {
        ALOGE("fVideoSink->startPlaying\n");
        handle->fVideoSink->startPlaying(*handle->fVideoSource, afterPlaying, handle);
    }
    ALOGD("%s ---------------------------------------------start111111\n", __FUNCTION__);
    handle->fEnv->taskScheduler().doEventLoop(&handle->fEventLoopWatchVariable);
    ALOGD("%s ---------------------------------------------end\n", __FUNCTION__);
    return 0;
}

NU_U32
NuPusherHandler::startStream(char* serverAddr, NU_U16 port, char* streamName, int rtpOverTcp, char *username,
    char *password, NU_MEDIA_INFO_T*  pstruStreamInfo, NU_U32 bufferKSize, NU_Bool createlogfile)
{
    ALOGDTRACE();

    fPort = port;
    fServerAddr = serverAddr;
    fUsername = username;
    fPassword = password;
    fRtpOverTcp = rtpOverTcp;
    fStreamName = streamName;
    fBufferKSize = bufferKSize;
    fCreatelogfile = createlogfile;
    fPstruStreamInfo = pstruStreamInfo;

    fInjector = DarwinInjector::createNew(*fEnv);
    fInjector->registerConnectStateCallBack(this);
    struct in_addr dummyDestAddress;
    dummyDestAddress.s_addr = 0;
    fVideoRtpGroupsock = new Groupsock(*fEnv, dummyDestAddress, 0, 0);
    struct in_addr dummyDestAddressAudio;
    dummyDestAddressAudio.s_addr = 0;
    fAudioRtpGroupsock = new Groupsock(*fEnv, dummyDestAddressAudio, 0, 0);

    unsigned char testRtpPayloadType = 96;
    NuVideoServerMediaSubsession *videoss = NuVideoServerMediaSubsession::createNew(*fEnv, this, false);
    unsigned estBitrate = 87543408;
    fVideoSource = videoss->createNewStreamSource(0, estBitrate);
    if (fVideoSource == NULL) {
        ALOGE("%s createNewStreamSource failed\n", __FUNCTION__);
        return -1;
    }
    fVideoSink = videoss->createNewRTPSink(fVideoRtpGroupsock, testRtpPayloadType, NULL);
    if (fVideoSink == NULL) {
        ALOGE("%s createNewRTPSink failed\n", __FUNCTION__);
        return -1;
    }

    // ��Video��RTPSink��ֵ��DarwinInjector��������ƵRTP��Darwin
    fInjector->addStream(fVideoSink, NULL);

    // RTSP ANNOUNCE/SETUP/PLAY���͹���
    if (!fInjector->setDestination(fServerAddr, fStreamName, "live555", "LIVE555", fPort)) {
        ALOGE("fInjector->setDestination() failed: %s\n", fEnv->getResultMsg());
        return -1;
    }

    // ��ʼת����ƵRTP����
    //if ((fVideoSink != NULL) && (fVideoSource != NULL)) {
    //    ALOGE("fVideoSink->startPlaying\n");
    //    fVideoSink->startPlaying(*fVideoSource, afterPlaying, this);
    //}

    // �����߳�������
    fPushThread->tHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startPushThreadFunc, (LPVOID)this, 0, NULL); 
    while (fPushThread->flag != __THREAD_STATE::THREAD_START) {
        Sleep(100);
    }

    if (NULL != fPushThread->tHandle)
        SetThreadPriority(fPushThread->tHandle, THREAD_PRIORITY_HIGHEST);
    return 0;
}

NU_U32
NuPusherHandler::addFrame(NU_AV_Frame* frame)
{
    ALOGDTRACE();
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

    return SSQ_AddData(fAvQueue, fId, MEDIA_TYPE_VIDEO, &frameInfo, (char *)frame->pBuffer);
    if (frame->u32AVFrameFlag == 1) {
        ALOGE("%s add MEDIA_TYPE_VIDEO frame to Queue\n", __FUNCTION__);
        int ret = SSQ_AddData(fAvQueue, fId, MEDIA_TYPE_VIDEO, &frameInfo, (char *)frame->pBuffer);
        ALOGD("%s, SSQ_AddData ret = %d\n", __FUNCTION__, ret);
        return ret;
    } else {
        ALOGE("%s MEDIA_TYPE = [%d] not add to Queue\n", __FUNCTION__, frame->u32AVFrameFlag);
    }
    return -1;
}

int
NuPusherHandler::getFrame(unsigned int *channelid, unsigned int *mediatype, MEDIA_FRAME_INFO *frameinfo, char* pbuf)
{
    ALOGDTRACE();
    if (fAvQueue == NULL)
        return -1;

    int ret = SSQ_GetData(fAvQueue, channelid, mediatype, frameinfo, pbuf);
    //ALOGD("%s---->framesize = %d\n", fAvQueue->pQueHeader->videoframes);
    //if (fAvQueue->pQueHeader->videoframes > 30) {
    //    ALOGD("[ch%d]����֡��[%d]>�趨֡��[%d].  ��ն��в��ȴ���һ��Key frame.\n", channelid, fAvQueue->pQueHeader->videoframes, 30);
    //    SSQ_Clear(fAvQueue);
    //}
    ALOGD("%s, SSQ_GetData------- ret = %d\n", __FUNCTION__, ret);
    return ret;
}

NU_U32
NuPusherHandler::stopStream()
{
    fEventLoopWatchVariable = -1; // exit doEventLoop
    CloseHandle(fPushThread->tHandle);
    fPushThread->flag = __THREAD_STATE::THREAD_STOP;
    releaseOurselves();
    return 0;
}