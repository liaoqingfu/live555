#ifndef _NU_TYPES_H
#define _NU_TYPES_H

#if (defined _WIN32) && (defined _USE_SHARED_LIB)
#define NU_API  __declspec(dllexport)
#define NU_APICALL  __stdcall
#define WIN32_LEAN_AND_MEAN
#else
#define NU_API
#define NU_APICALL 
#endif

// Handle Type
#define NU_RTSP_Handle void*
#define NU_Pusher_Handle void*
#define NU_HLS_Handle void*

typedef int						NU_I32;

typedef unsigned char           NU_U8;
typedef unsigned char           NU_UChar;
typedef unsigned short          NU_U16;
typedef unsigned int            NU_U32;
typedef unsigned char			NU_Bool;

enum
{
    NU_NoErr = 0,
    NU_RequestFailed = -1,
    NU_Unimplemented = -2,
    NU_RequestArrived = -3,
    NU_OutOfState = -4,
    NU_NotAModule = -5,
    NU_WrongVersion = -6,
    NU_IllegalService = -7,
    NU_BadIndex = -8,
    NU_ValueNotFound = -9,
    NU_BadArgument = -10,
    NU_ReadOnly = -11,
    NU_NotPreemptiveSafe = -12,
    NU_NotEnoughSpace = -13,
    NU_WouldBlock = -14,
    NU_NotConnected = -15,
    NU_FileNotFound = -16,
    NU_NoMoreData = -17,
    NU_AttrDoesntExist = -18,
    NU_AttrNameExists = -19,
    NU_InstanceAttrsNotAllowed = -20,
    NU_InvalidSocket = -21,
    NU_MallocError = -22,
    NU_ConnectError = -23,
    NU_SendError = -24
};
typedef int NU_Error;


typedef enum __NU_ACTIVATE_ERR_CODE_ENUM
{
    NU_ACTIVATE_INVALID_KEY = -1,			/* ��ЧKey */
    NU_ACTIVATE_TIME_ERR = -2,			/* ʱ����� */
    NU_ACTIVATE_PROCESS_NAME_LEN_ERR = -3,			/* �������Ƴ��Ȳ�ƥ�� */
    NU_ACTIVATE_PROCESS_NAME_ERR = -4,			/* �������Ʋ�ƥ�� */
    NU_ACTIVATE_VALIDITY_PERIOD_ERR = -5,			/* ��Ч��У�鲻һ�� */
    NU_ACTIVATE_PLATFORM_ERR = -6,			/* ƽ̨��ƥ�� */
    NU_ACTIVATE_COMPANY_ID_LEN_ERR = -7,			/* ��Ȩʹ���̲�ƥ�� */
    NU_ACTIVATE_SUCCESS = 0,			/* ����ɹ� */

} NU_ACTIVATE_ERR_CODE_ENUM;


/* ��Ƶ���� */
#define NU_SDK_VIDEO_CODEC_H264		0x1C		/* H264  */
#define NU_SDK_VIDEO_CODEC_H265		0x48323635	/* 1211250229 */
#define	NU_SDK_VIDEO_CODEC_MJPEG	0x08		/* MJPEG */
#define	NU_SDK_VIDEO_CODEC_MPEG4	0x0D		/* MPEG4 */

/* ��Ƶ���� */
#define NU_SDK_AUDIO_CODEC_AAC		0x15002		/* AAC */
#define NU_SDK_AUDIO_CODEC_G711U	0x10006		/* G711 ulaw*/
#define NU_SDK_AUDIO_CODEC_G711A	0x10007		/* G711 alaw*/
#define NU_SDK_AUDIO_CODEC_G726		0x1100B		/* G726 */

#define NU_SDK_EVENT_CODEC_ERROR	0x63657272	/* ERROR */
#define NU_SDK_EVENT_CODEC_EXIT		0x65786974	/* EXIT */

/* ����Ƶ֡��ʶ */
#define NU_SDK_VIDEO_FRAME_FLAG		0x00000001		/* ��Ƶ֡��־ */
#define NU_SDK_AUDIO_FRAME_FLAG		0x00000002		/* ��Ƶ֡��־ */
#define NU_SDK_EVENT_FRAME_FLAG		0x00000004		/* �¼�֡��־ */
#define NU_SDK_RTP_FRAME_FLAG		0x00000008		/* RTP֡��־ */
#define NU_SDK_SDP_FRAME_FLAG		0x00000010		/* SDP֡��־ */
#define NU_SDK_MEDIA_INFO_FLAG		0x00000020		/* ý�����ͱ�־*/
#define NU_SDK_VIDEO_RAW_RGB		0x00000040			/* RGB*/

/* ��Ƶ�ؼ��ֱ�ʶ */
#define NU_SDK_VIDEO_FRAME_I		0x01		/* I֡ */
#define NU_SDK_VIDEO_FRAME_P		0x02		/* P֡ */
#define NU_SDK_VIDEO_FRAME_B		0x03		/* B֡ */
#define NU_SDK_VIDEO_FRAME_J		0x04		/* JPEG */

/* �������� */
typedef enum __NU_RTP_CONNECT_TYPE
{
    NU_RTP_OVER_TCP = 0x01,			/* RTP Over TCP */
    NU_RTP_OVER_UDP					/* RTP Over UDP */
} NU_RTP_CONNECT_TYPE;

/* ý����Ϣ */
typedef struct __NU_MEDIA_INFO_T
{
    NU_U32 u32VideoCodec;			/* ��Ƶ�������� */
    NU_U32 u32VideoFps;				/* ��Ƶ֡�� */

    NU_U32 u32AudioCodec;	        /* ��Ƶ�������� */
    NU_U32 u32AudioSamplerate;		/* ��Ƶ������ */
    NU_U32 u32AudioChannel;			/* ��Ƶͨ���� */
    NU_U32 u32AudioBitsPerSample;   /* ��Ƶ�������� */

    NU_U32 u32VpsLength;			/* ÊÓÆµvpsÖ¡³¤¶È */
    NU_U32 u32SpsLength;			/* ÊÓÆµspsÖ¡³¤¶È */
    NU_U32 u32PpsLength;			/* ÊÓÆµppsÖ¡³¤¶È */
    NU_U32 u32SeiLength;			/* ÊÓÆµseiÖ¡³¤¶È */
    NU_U8	 u8Vps[255];			/* ÊÓÆµvpsÖ¡ÄÚÈÝ */
    NU_U8	 u8Sps[255];			/* ÊÓÆµspsÖ¡ÄÚÈÝ */
    NU_U8	 u8Pps[128];		    /* ÊÓÆµspsÖ¡ÄÚÈÝ */
    NU_U8	 u8Sei[128];            /* ÊÓÆµseiÖ¡ÄÚÈÝ */
} NU_MEDIA_INFO_T;

/* ֡��Ϣ */
typedef struct
{
    unsigned int	codec;				/* ����Ƶ��ʽ */

    unsigned int	type;				/* ��Ƶ֡���� */
    unsigned char	fps;				/* ��Ƶ֡�� */
    unsigned short	width;				/* ��Ƶ�� */
    unsigned short  height;				/* ��Ƶ�� */

    unsigned int	reserved1;			/* ��������1 */
    unsigned int	reserved2;			/* ��������2 */

    unsigned int	sample_rate;		/* ��Ƶ������ */
    unsigned int	channels;			/* ��Ƶ������ */
    unsigned int	bits_per_sample;	/* ��Ƶ�������� */

    unsigned int	length;				/* ����Ƶ֡��С */
    unsigned int    timestamp_usec;		/* ʱ���,΢�� */
    unsigned int	timestamp_sec;		/* ʱ��� �� */

    float			bitrate;			/* ������ */
    float			losspacket;			/* ������ */
} NU_RTSP_FRAME_INFO;

#endif // _NU_TYPES_H
