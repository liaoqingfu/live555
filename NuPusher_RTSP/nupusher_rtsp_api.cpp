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

// �������;��  ����Ϊ���ֵ
NU_API NU_Pusher_Handle NU_APICALL NuPusherRTSP_Create()
{
    ALOGDTRACE();
    NuPusherHandler *handler = new NuPusherHandler();
    return (void*)handler;
}

// �ͷ����;��
NU_API NU_U32 NU_APICALL NuPusherRTSP_Release(NU_Pusher_Handle handle)
{
    ALOGDTRACE();
    if (handle)
        delete handle;
    return 0;
}

// �����������¼��ص� userptr�����Զ������ָ��
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

// ��ʼ������ 
//
// serverAddr: ��ý���������ַ��
// port: ��ý��˿ڡ�
// streamName: ������<xxx.sdp>��
// username/password: ����Я�����û������롢
// pstruStreamInfo: ���͵�ý�嶨�塢
// bufferKSize: ��kΪ��λ�Ļ�������С<512~2048֮��,Ĭ��512> 
// bool createlogfile: ������־�ļ�
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

// ֹͣ������
NU_API NU_U32 NU_APICALL NuPusherRTSP_StopStream(NU_Pusher_Handle handle)
{
    ALOGDTRACE();
    return NU_PUSH_STATE_ERROR;
}

// ���� frame:�������͵���ý��֡
NU_API NU_U32 NU_APICALL NuPusherRTSP_PushFrame(NU_Pusher_Handle handle, NU_AV_Frame* frame)
{
    ALOGDTRACE();
    return NU_PUSH_STATE_ERROR;
}
