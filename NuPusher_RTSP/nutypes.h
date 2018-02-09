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
    NU_ACTIVATE_INVALID_KEY = -1,			/* 无效Key */
    NU_ACTIVATE_TIME_ERR = -2,			/* 时间错误 */
    NU_ACTIVATE_PROCESS_NAME_LEN_ERR = -3,			/* 进程名称长度不匹配 */
    NU_ACTIVATE_PROCESS_NAME_ERR = -4,			/* 进程名称不匹配 */
    NU_ACTIVATE_VALIDITY_PERIOD_ERR = -5,			/* 有效期校验不一致 */
    NU_ACTIVATE_PLATFORM_ERR = -6,			/* 平台不匹配 */
    NU_ACTIVATE_COMPANY_ID_LEN_ERR = -7,			/* 授权使用商不匹配 */
    NU_ACTIVATE_SUCCESS = 0,			/* 激活成功 */

} NU_ACTIVATE_ERR_CODE_ENUM;


/* 视频编码 */
#define NU_SDK_VIDEO_CODEC_H264		0x1C		/* H264  */
#define NU_SDK_VIDEO_CODEC_H265		0x48323635	/* 1211250229 */
#define	NU_SDK_VIDEO_CODEC_MJPEG	0x08		/* MJPEG */
#define	NU_SDK_VIDEO_CODEC_MPEG4	0x0D		/* MPEG4 */

/* 音频编码 */
#define NU_SDK_AUDIO_CODEC_AAC		0x15002		/* AAC */
#define NU_SDK_AUDIO_CODEC_G711U	0x10006		/* G711 ulaw*/
#define NU_SDK_AUDIO_CODEC_G711A	0x10007		/* G711 alaw*/
#define NU_SDK_AUDIO_CODEC_G726		0x1100B		/* G726 */

#define NU_SDK_EVENT_CODEC_ERROR	0x63657272	/* ERROR */
#define NU_SDK_EVENT_CODEC_EXIT		0x65786974	/* EXIT */

/* 音视频帧标识 */
#define NU_SDK_VIDEO_FRAME_FLAG		0x00000001		/* 视频帧标志 */
#define NU_SDK_AUDIO_FRAME_FLAG		0x00000002		/* 音频帧标志 */
#define NU_SDK_EVENT_FRAME_FLAG		0x00000004		/* 事件帧标志 */
#define NU_SDK_RTP_FRAME_FLAG		0x00000008		/* RTP帧标志 */
#define NU_SDK_SDP_FRAME_FLAG		0x00000010		/* SDP帧标志 */
#define NU_SDK_MEDIA_INFO_FLAG		0x00000020		/* 媒体类型标志*/
#define NU_SDK_VIDEO_RAW_RGB		0x00000040			/* RGB*/

/* 视频关键字标识 */
#define NU_SDK_VIDEO_FRAME_I		0x01		/* I帧 */
#define NU_SDK_VIDEO_FRAME_P		0x02		/* P帧 */
#define NU_SDK_VIDEO_FRAME_B		0x03		/* B帧 */
#define NU_SDK_VIDEO_FRAME_J		0x04		/* JPEG */

/* 连接类型 */
typedef enum __NU_RTP_CONNECT_TYPE
{
    NU_RTP_OVER_TCP = 0x01,			/* RTP Over TCP */
    NU_RTP_OVER_UDP					/* RTP Over UDP */
} NU_RTP_CONNECT_TYPE;

/* 媒体信息 */
typedef struct __NU_MEDIA_INFO_T
{
    NU_U32 u32VideoCodec;			/* 视频编码类型 */
    NU_U32 u32VideoFps;				/* 视频帧率 */

    NU_U32 u32AudioCodec;	        /* 音频编码类型 */
    NU_U32 u32AudioSamplerate;		/* 音频采样率 */
    NU_U32 u32AudioChannel;			/* 音频通道数 */
    NU_U32 u32AudioBitsPerSample;   /* 音频采样精度 */

    NU_U32 u32VpsLength;			/* 脢脫脝碌vps脰隆鲁陇露脠 */
    NU_U32 u32SpsLength;			/* 脢脫脝碌sps脰隆鲁陇露脠 */
    NU_U32 u32PpsLength;			/* 脢脫脝碌pps脰隆鲁陇露脠 */
    NU_U32 u32SeiLength;			/* 脢脫脝碌sei脰隆鲁陇露脠 */
    NU_U8	 u8Vps[255];			/* 脢脫脝碌vps脰隆脛脷脠脻 */
    NU_U8	 u8Sps[255];			/* 脢脫脝碌sps脰隆脛脷脠脻 */
    NU_U8	 u8Pps[128];		    /* 脢脫脝碌sps脰隆脛脷脠脻 */
    NU_U8	 u8Sei[128];            /* 脢脫脝碌sei脰隆脛脷脠脻 */
} NU_MEDIA_INFO_T;

/* 帧信息 */
typedef struct
{
    unsigned int	codec;				/* 音视频格式 */

    unsigned int	type;				/* 视频帧类型 */
    unsigned char	fps;				/* 视频帧率 */
    unsigned short	width;				/* 视频宽 */
    unsigned short  height;				/* 视频高 */

    unsigned int	reserved1;			/* 保留参数1 */
    unsigned int	reserved2;			/* 保留参数2 */

    unsigned int	sample_rate;		/* 音频采样率 */
    unsigned int	channels;			/* 音频声道数 */
    unsigned int	bits_per_sample;	/* 音频采样精度 */

    unsigned int	length;				/* 音视频帧大小 */
    unsigned int    timestamp_usec;		/* 时间戳,微妙 */
    unsigned int	timestamp_sec;		/* 时间戳 秒 */

    float			bitrate;			/* 比特率 */
    float			losspacket;			/* 丢包率 */
} NU_RTSP_FRAME_INFO;

#endif // _NU_TYPES_H
