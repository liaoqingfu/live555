#ifndef _NU_VIDEO_SERVER_MEDIA_SUBSESSION
#define _NU_VIDEO_SERVER_MEDIA_SUBSESSION

#ifndef _ON_DEMAND_SERVER_MEDIA_SUBSESSION_HH
#include "OnDemandServerMediaSubsession.hh"
#endif

class NuVideoServerMediaSubsession : public OnDemandServerMediaSubsession {
public:
    static NuVideoServerMediaSubsession* createNew(UsageEnvironment& env, Boolean reuseFirstSource);

    // redefined virtual functions
    virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
    virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);

private:
    NuVideoServerMediaSubsession(UsageEnvironment& env, Boolean reuseFirstSource);
    virtual ~NuVideoServerMediaSubsession();

protected:
    // virtual char const* sdpLines();
};

#endif // _NU_VIDEO_SERVER_MEDIA_SUBSESSION