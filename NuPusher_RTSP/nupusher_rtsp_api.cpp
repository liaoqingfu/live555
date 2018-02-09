#include "nupusher_rtsp_api.h"
#include "nutypes.h"
#include <stdio.h>
#include "NuPusherHandler.h"

#define LOG_TAG                         "NUPUSHER_RTSP_API"
#include "utils/Log.h"

#ifdef ANDROID
NU_API NU_I32 NU_APICALL NuPusherRTSP_Activate(char *license, char* userPtr)
{
    return 0;
}
#else
NU_API NU_I32 NU_APICALL NuPusherRTSP_Activate(char *license)
{
    ALOGDTRACE();
    return 0;
}
#endif

// 创建推送句柄  返回为句柄值
NU_API NU_Pusher_Handle NU_APICALL NuPusherRTSP_Create()
{
    ALOGDTRACE();
    NuPusherHandler *handler = new NuPusherHandler();
    return (void*)handler;
}

// 释放推送句柄
NU_API NU_U32 NU_APICALL NuPusherRTSP_Release(NU_Pusher_Handle handle)
{
    ALOGDTRACE();
    if (handle)
        delete handle;
    return 0;
}

// 设置流传输事件回调 userptr传输自定义对象指针
NU_API NU_U32 NU_APICALL NuPusherRTSP_SetEventCallback(NU_Pusher_Handle handle, NuPusherRTSP_Callback callback, int id, void *userptr)
{
    ALOGDTRACE();
    if (handle) {
        ((NuPusherHandler *)handle)->registerCallBack(callback);
        ((NuPusherHandler *)handle)->setId(id);
        ((NuPusherHandler *)handle)->setUserPointer(userptr);
        return 0;
    }
    return -1;
}

// 开始流传输 
//
// serverAddr: 流媒体服务器地址、
// port: 流媒体端口、
// streamName: 流名称<xxx.sdp>、
// username/password: 推送携带的用户名密码、
// pstruStreamInfo: 推送的媒体定义、
// bufferKSize: 以k为单位的缓冲区大小<512~2048之间,默认512> 
// bool createlogfile: 创建日志文件
NU_API NU_U32 NU_APICALL NuPusherRTSP_StartStream(NU_Pusher_Handle handle,
    char* serverAddr,
    NU_U16 port,
    char* streamName,
    int rtpOverTcp, /*1-tcp, 2-udp*/
    char *username,
    char *password,
    NU_MEDIA_INFO_T*  pstruStreamInfo,
    NU_U32 bufferKSize,
    NU_Bool createlogfile)
{
    ALOGDTRACE();
    if (handle)
        return ((NuPusherHandler *)handle)->startStream(serverAddr, port, streamName, rtpOverTcp, username, 
            password, pstruStreamInfo, bufferKSize, createlogfile);
    return NU_PUSH_STATE_ERROR;
}

// 停止流传输
NU_API NU_U32 NU_APICALL NuPusherRTSP_StopStream(NU_Pusher_Handle handle)
{
    ALOGDTRACE();
    return NU_PUSH_STATE_ERROR;
}

// 推流 frame:具体推送的流媒体帧
NU_API NU_U32 NU_APICALL NuPusherRTSP_PushFrame(NU_Pusher_Handle handle, NU_AV_Frame* frame)
{
    ALOGDTRACE();
    return NU_PUSH_STATE_ERROR;
}
