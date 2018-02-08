#ifndef _NU_PUSHER_RTSP_API_H_
#define _NU_PUSHER_RTSP_API_H_

#include "nutypes.h"

#define RTSP_CLIENT_NAME	"NuPusherRTSP v0.01"

typedef struct __NU_AV_Frame
{
	NU_U32 u32AVFrameFlag;		/* ֡��־  ��Ƶ or ��Ƶ */
	NU_U32 u32AVFrameLen;		/* ֡�ĳ��� */
	NU_U32 u32VFrameType;		/* ��Ƶ�����ͣ�I֡��P֡ */
	NU_U8 *pBuffer;				/* ���� */
	NU_U32 u32TimestampSec;		/* ʱ���(��)*/
	NU_U32 u32TimestampUsec;	/* ʱ���(΢��) */
} NU_AV_Frame;

/* �����¼����Ͷ��� */
typedef enum __NU_PUSH_STATE_T
{
	NU_PUSH_STATE_CONNECTING = 1,   /* ������ */
	NU_PUSH_STATE_CONNECTED,        /* ���ӳɹ� */
	NU_PUSH_STATE_CONNECT_FAILED,   /* ����ʧ�� */
	NU_PUSH_STATE_CONNECT_ABORT,    /* �����쳣�ж� */
	NU_PUSH_STATE_PUSHING,          /* ������ */
	NU_PUSH_STATE_DISCONNECTED,     /* �Ͽ����� */
	NU_PUSH_STATE_ERROR				/* ���� */
} NU_PUSH_STATE_T;

/* ���ͻص��������� _userptr��ʾ�û��Զ������� */
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

	/* �������;��  ����Ϊ���ֵ */
	NU_API NU_Pusher_Handle NU_APICALL NuPusherRTSP_Create();

	/* �ͷ����;�� */
	NU_API NU_U32 NU_APICALL NuPusherRTSP_Release(NU_Pusher_Handle handle);

	/* �����������¼��ص� userptr�����Զ������ָ��*/
	NU_API NU_U32 NU_APICALL NuPusherRTSP_SetEventCallback(NU_Pusher_Handle handle, NuPusherRTSP_Callback callback, int id, void *userptr);

	/* ��ʼ������ serverAddr:��ý���������ַ��port:��ý��˿ڡ�streamName:������<xxx.sdp>��username/password:����Я�����û������롢pstruStreamInfo:���͵�ý�嶨�塢bufferKSize:��kΪ��λ�Ļ�������С<512~2048֮��,Ĭ��512> bool createlogfile:������־�ļ�*/
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

	/* ֹͣ������ */
	NU_API NU_U32 NU_APICALL NuPusherRTSP_StopStream(NU_Pusher_Handle handle);

	/* ���� frame:�������͵���ý��֡ */
	NU_API NU_U32 NU_APICALL NuPusherRTSP_PushFrame(NU_Pusher_Handle handle, NU_AV_Frame* frame);

#ifdef __cplusplus
}
#endif

#endif // _NU_PUSHER_RTSP_API_H_
