#include "NuVideoServerMediaSubsession.hh"
#include "NuH264VideoStreamFramer.hh"
#include <GroupsockHelper.hh>
#include <H264VideoRTPSink.hh>

NuVideoServerMediaSubsession*
NuVideoServerMediaSubsession::createNew(UsageEnvironment& env, Boolean reuseFirstSource)
{
    return new NuVideoServerMediaSubsession(env, reuseFirstSource);
}

NuVideoServerMediaSubsession::NuVideoServerMediaSubsession(UsageEnvironment& env, Boolean reuseFirstSource)
    : OnDemandServerMediaSubsession(env, reuseFirstSource)
{
}

NuVideoServerMediaSubsession::~NuVideoServerMediaSubsession()
{
}

FramedSource*
NuVideoServerMediaSubsession::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate)
{
    estBitrate = 96; // video
    return NuH264VideoStreamFramer::createNew(envir(), NULL);
}

RTPSink*
NuVideoServerMediaSubsession::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource)
{
    return H264VideoRTPSink::createNew(envir(), rtpGroupsock, 96); // 96 for video
}