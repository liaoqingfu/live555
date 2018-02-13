#ifndef _NU_H264_VIDEO_STREAM_FRAMER_HH
#define _NU_H264_VIDEO_STREAM_FRAMER_HH

#ifndef _MPEG_VIDEO_STREAM_FRAMER_HH
#include "MPEGVideoStreamFramer.hh"
#endif

#include "ssqueue.h"


class IVideoStreamFramerCallBack {
public:
    // 连接状态回调
    virtual int getFrame(unsigned int *channelid, unsigned int *mediatype, MEDIA_FRAME_INFO *frameinfo, char* pbuf) = 0;
};

class NuH264VideoStreamFramer : public MPEGVideoStreamFramer {
public:
    static NuH264VideoStreamFramer* createNew(UsageEnvironment& env, FramedSource* inputSource, IVideoStreamFramerCallBack* getFrameCb,
        Boolean includeStartCodeInOutput = False);

protected:
    NuH264VideoStreamFramer(UsageEnvironment& env, FramedSource* inputSource, IVideoStreamFramerCallBack* getFrameCb, Boolean includeStartCodeInOutput);
    virtual ~NuH264VideoStreamFramer();

    // redefined virtual functions:
    virtual Boolean isH264VideoStreamFramer() const;
    virtual void doGetNextFrame();

private:
    void setPresentationTime() { fPresentationTime = fNextPresentationTime; }

private:
    IVideoStreamFramerCallBack* fGetFrameCb;
    struct timeval fNextPresentationTime; // the presentation time to be used for the next NAL unit to be parsed/delivered after this
    friend class NuH264VideoStreamParser; // hack
};

#ifndef _MPEG_VIDEO_STREAM_PARSER_HH
#include "../MPEGVideoStreamParser.hh"
#endif

class NuH264VideoStreamParser : public MPEGVideoStreamParser {
public:
    NuH264VideoStreamParser(NuH264VideoStreamFramer* usingSource, FramedSource* inputSource);
    virtual ~NuH264VideoStreamParser();

private: // redefined virtual functions:
    virtual unsigned int parse();

private:
};

#endif // _NU_H264_VIDEO_STREAM_FRAMER_HH