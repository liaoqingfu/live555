#ifndef _NU_PUSHER_HANDLER_H_
#define _NU_PUSHER_HANDLER_H_

#include "nupusher_rtsp_api.h"
#include "nutypes.h"

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

class NuPusherHandler : public IDarwinInjectorCallBack {
public:
    NuPusherHandler();
    ~NuPusherHandler();
	void    releaseOurselves();
	NU_U32 startStream(char* serverAddr,
        NU_U16 port,
        char* streamName,
        int rtpOverTcp, /*1-tcp, 2-udp*/
        char *username,
        char *password,
        NU_MEDIA_INFO_T*  pstruStreamInfo,
        NU_U32 bufferKSize,
        NU_Bool createlogfile);

    void    registerCallBack(NuPusherRTSP_Callback cb);
    void    setUserPointer(void *ptr);
    void*   getUserPointer();
    void    setId(int id);
    int     getId();
	virtual void connStateCallBack(int state, char* resultStr);

private:
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
    NU_MEDIA_INFO_T*  fPstruStreamInfo;

    UsageEnvironment *fEnv;	    //live555 global environment
    TaskScheduler* fScheduler;
    char fEventLoopWatchVariable = 0;

    DarwinInjector* fInjector;  //DarwinInjector
    FramedSource* fVideoSource;
    FramedSource* fAudioSource;

    RTPSink* fVideoSink;
    RTPSink* fAudioSink;

    Groupsock* fVideoRtpGroupsock; //Video Socket
    Groupsock* fAudioRtpGroupsock; //Video Socket
};

#endif // _NU_PUSHER_HANDLER_H_