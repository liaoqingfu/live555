#include "NuH264VideoStreamFramer.hh"
#include "H264VideoRTPSource.hh"
#include <GroupsockHelper.hh>

NuH264VideoStreamParser::NuH264VideoStreamParser(NuH264VideoStreamFramer* usingSource, FramedSource* inputSource)
    : MPEGVideoStreamParser(usingSource, inputSource)
{
}

NuH264VideoStreamParser::~NuH264VideoStreamParser()
{
}

unsigned int
NuH264VideoStreamParser::parse()
{
    return 0;
}

NuH264VideoStreamFramer*
NuH264VideoStreamFramer::createNew(UsageEnvironment& env, FramedSource* inputSource, Boolean includeStartCodeInOutput)
{
    return new NuH264VideoStreamFramer(env, inputSource, includeStartCodeInOutput);
}

NuH264VideoStreamFramer ::NuH264VideoStreamFramer(UsageEnvironment& env, FramedSource* inputSource, Boolean createParser)
    : MPEGVideoStreamFramer(env, inputSource)
{
    fParser = createParser
        ? new NuH264VideoStreamParser(this, inputSource)
        : NULL;
    fFrameRate = 25.0; // We assume a frame rate of 25 fps, unless we learn otherwise (from parsing a Sequence Parameter Set NAL unit)
}

NuH264VideoStreamFramer::~NuH264VideoStreamFramer()
{
}

void NuH264VideoStreamFramer::doGetNextFrame()
{
    fprintf(stderr, "lcp-debug NuH264VideoStreamFramer::doGetNextFrame");
    afterGetting(this);
}