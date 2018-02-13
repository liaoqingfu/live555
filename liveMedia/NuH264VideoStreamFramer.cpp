#include "NuH264VideoStreamFramer.hh"
#include "H264VideoRTPSource.hh"
#include <GroupsockHelper.hh>

#define LOG_TAG     "NU_H264_VIDEO_STREAM_FRAMER"
#include "utils/Log.h"

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
NuH264VideoStreamFramer::createNew(UsageEnvironment& env, FramedSource* inputSource, IVideoStreamFramerCallBack* getFrameCb, Boolean includeStartCodeInOutput)
{
    return new NuH264VideoStreamFramer(env, inputSource, getFrameCb, includeStartCodeInOutput);
}

NuH264VideoStreamFramer ::NuH264VideoStreamFramer(UsageEnvironment& env, FramedSource* inputSource, IVideoStreamFramerCallBack* getFrameCb, Boolean createParser)
    : MPEGVideoStreamFramer(env, inputSource)
{
    fParser = createParser
        ? new NuH264VideoStreamParser(this, inputSource)
        : NULL;
    fFrameRate = 25.0; // We assume a frame rate of 25 fps, unless we learn otherwise (from parsing a Sequence Parameter Set NAL unit)
    fGetFrameCb = getFrameCb;
}

NuH264VideoStreamFramer::~NuH264VideoStreamFramer()
{
}

// redefined virtual functions, which called by sourceIsCompatibleWithUs
Boolean
NuH264VideoStreamFramer::isH264VideoStreamFramer() const
{
    return True;
}

void NuH264VideoStreamFramer::doGetNextFrame()
{
    ALOGD("%s enter!!!!!!!!!!!!!!!!!!!!!!\n", __FUNCTION__);
    ALOGDTRACE();
    if (fGetFrameCb == NULL) {
        ALOGE("%s Error, fGetFrameCb is NULL\n", __FUNCTION__);
        return;
    }

    unsigned int channelid = 0;
    unsigned int mediatype = 0;
    MEDIA_FRAME_INFO frameinfo;
    int buf_size = 1024 * 1024;
    char *pbuf = new char[buf_size];
    int ret = fGetFrameCb->getFrame(&channelid, &mediatype, &frameinfo, pbuf);
    if (ret < 0) {
        ALOGE("%s Error, getFrame ret[%d] < 0\n", __FUNCTION__, ret);
        delete[]pbuf;
        pbuf = NULL;
        afterGetting(this);
        return;
    }

    if (frameinfo.length < fMaxSize) {
        memcpy(fTo, pbuf, frameinfo.length);
    } else {
        memcpy(fTo, pbuf, fMaxSize);
        fNumTruncatedBytes = frameinfo.length - fMaxSize;
    }

    fDurationInMicroseconds = 40000;
    fFrameSize = frameinfo.length;

    delete[]pbuf;
    pbuf = NULL;
    afterGetting(this);
}