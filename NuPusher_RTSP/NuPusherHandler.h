#ifndef _NU_PUSHER_HANDLER_H_
#define _NU_PUSHER_HANDLER_H_

#include "nupusher_rtsp_api.h"
#include "nutypes.h"
#include "ssqueue.h"
#include "NuH264VideoStreamFramer.hh"

#ifndef _DARWIN_INJECTOR_HH
#include "DarwinInjector.hh"
#endif

// forward
class UsageEnvironment;
class TaskScheduler;
class DarwinInjector;
class FramedSource;
class RTPSink;
class Groupsock;

typedef enum __THREAD_STATE {
    THREAD_STOP = 0,
    THREAD_START
};

typedef struct __THREAD_OBJ
{
    __THREAD_STATE  flag;
    HANDLE          tHandle;
} THREAD_OBJ;

class NuPusherHandler : public IDarwinInjectorCallBack, public IVideoStreamFramerCallBack {
public:
    NuPusherHandler();
    ~NuPusherHandler();
	void    releaseOurselves();
	NU_U32  startStream(char* serverAddr,
        NU_U16 port,
        char* streamName,
        int rtpOverTcp, /*1-tcp, 2-udp*/
        char *username,
        char *password,
        NU_MEDIA_INFO_T*  pstruStreamInfo,
        NU_U32 bufferKSize,
        NU_Bool createlogfile);
    NU_U32  stopStream();
    // 添加一帧数据
    NU_U32  addFrame(NU_AV_Frame* frame);
    // 获取一帧数据

    void    registerCallBack(NuPusherRTSP_Callback cb);
    void    setUserPointer(void *ptr);
    void*   getUserPointer();
    void    setId(int id);
    int     getId();
	virtual void connStateCallBack(int state, char* resultStr);
    virtual int getFrame(unsigned int *channelid, unsigned int *mediatype, MEDIA_FRAME_INFO *frameinfo, char* pbuf);

    static LPTHREAD_START_ROUTINE __stdcall startPushThreadFunc(LPVOID _pParam);

    UsageEnvironment *fEnv;	    //live555 global environment
    char fEventLoopWatchVariable;

private:
    THREAD_OBJ* fPushThread;
    NuPusherRTSP_Callback fUserCallBack;
    NU_Bool fIsPlaying;

    void*   fUserPtr;
    int     fId;

    int     fRtpOverTcp;
    char*   fStreamName;
    char*   fUsername;
    char*   fPassword;
    char*   fServerAddr;
    NU_U16  fPort;
    NU_U32  fBufferKSize;
    NU_Bool fCreatelogfile;
    NU_MEDIA_INFO_T*    fPstruStreamInfo;
    SS_QUEUE_OBJ_T*     fAvQueue;

    TaskScheduler* fScheduler;

    DarwinInjector* fInjector;  //DarwinInjector
    FramedSource* fVideoSource;
    FramedSource* fAudioSource;

    RTPSink* fVideoSink;
    RTPSink* fAudioSink;

    Groupsock* fVideoRtpGroupsock; //Video Socket
    Groupsock* fAudioRtpGroupsock; //Video Socket
};

#endif // _NU_PUSHER_HANDLER_H_