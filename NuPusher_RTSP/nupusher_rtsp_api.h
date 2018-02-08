#ifndef _NU_PUSHER_RTSP_API_H_
#define _NU_PUSHER_RTSP_API_H_

#include "nutypes.h"

#define RTSP_CLIENT_NAME	"NuPusherRTSP v0.01"

typedef struct __NU_AV_Frame
{
	NU_U32 u32AVFrameFlag;		/* 帧标志  视频 or 音频 */
	NU_U32 u32AVFrameLen;		/* 帧的长度 */
	NU_U32 u32VFrameType;		/* 视频的类型，I帧或P帧 */
	NU_U8 *pBuffer;				/* 数据 */
	NU_U32 u32TimestampSec;		/* 时间戳(秒)*/
	NU_U32 u32TimestampUsec;	/* 时间戳(微秒) */
} NU_AV_Frame;

/* 推送事件类型定义 */
typedef enum __NU_PUSH_STATE_T
{
	NU_PUSH_STATE_CONNECTING = 1,   /* 连接中 */
	NU_PUSH_STATE_CONNECTED,        /* 连接成功 */
	NU_PUSH_STATE_CONNECT_FAILED,   /* 连接失败 */
	NU_PUSH_STATE_CONNECT_ABORT,    /* 连接异常中断 */
	NU_PUSH_STATE_PUSHING,          /* 推流中 */
	NU_PUSH_STATE_DISCONNECTED,     /* 断开连接 */
	NU_PUSH_STATE_ERROR				/* 出错 */
} NU_PUSH_STATE_T;

/* 推送回调函数定义 _userptr表示用户自定义数据 */
typedef int(*NuPusherRTSP_Callback)(int _id, NU_PUSH_STATE_T _state, NU_AV_Frame *_frame, void *_userptr);

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef ANDROID
	NU_API NU_I32 NU_APICALL NuPusherRTSP_Activate(char *license, char* userPtr);
#else
	NU_API NU_I32 NU_APICALL NuPusherRTSP_Activate(char *license);
#endif

	/* 创建推送句柄  返回为句柄值 */
	NU_API NU_Pusher_Handle NU_APICALL NuPusherRTSP_Create();

	/* 释放推送句柄 */
	NU_API NU_U32 NU_APICALL NuPusherRTSP_Release(NU_Pusher_Handle handle);

	/* 设置流传输事件回调 userptr传输自定义对象指针*/
	NU_API NU_U32 NU_APICALL NuPusherRTSP_SetEventCallback(NU_Pusher_Handle handle, NuPusherRTSP_Callback callback, int id, void *userptr);

	/* 开始流传输 serverAddr:流媒体服务器地址、port:流媒体端口、streamName:流名称<xxx.sdp>、username/password:推送携带的用户名密码、pstruStreamInfo:推送的媒体定义、bufferKSize:以k为单位的缓冲区大小<512~2048之间,默认512> bool createlogfile:创建日志文件*/
	NU_API NU_U32 NU_APICALL NuPusherRTSP_StartStream(NU_Pusher_Handle handle,
		char* serverAddr,
		NU_U16 port,
		char* streamName,
		int rtpOverTcp, /*1-tcp, 2-udp*/
		char *username,
		char *password,
		NU_MEDIA_INFO_T*  pstruStreamInfo,
		NU_U32 bufferKSize,
		NU_Bool createlogfile);

	/* 停止流传输 */
	NU_API NU_U32 NU_APICALL NuPusherRTSP_StopStream(NU_Pusher_Handle handle);

	/* 推流 frame:具体推送的流媒体帧 */
	NU_API NU_U32 NU_APICALL NuPusherRTSP_PushFrame(NU_Pusher_Handle handle, NU_AV_Frame* frame);

#ifdef __cplusplus
}
#endif

#endif // _NU_PUSHER_RTSP_API_H_
