#ifndef __QCAP_COMMON_H__
#define __QCAP_COMMON_H__

#include "qcap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

// COLOR SPACE TYPE
enum {
	QCAP_COLORSPACE_TYPE_BGRP = 0x50524742,
	// QCAP_COLORSPACE_TYPE_YV21 = QCAP_COLORSPACE_TYPE_I420,
	QCAP_COLORSPACE_TYPE_RGBP_FLOAT = 0x50464752,
	QCAP_COLORSPACE_TYPE_UNDEFINED = 0xFFFFFFFF,
};

// INPUT VIDEO SOURCE
//
enum {
	QCAP_INPUT_TYPE_TVI	 = 100,
	QCAP_INPUT_TYPE_SENSOR,
	QCAP_INPUT_TYPE_UNDEFINED = 0xFFFFFFFF,
};

// INPUT AUDIO SOURCE
//
enum {
	QCAP_INPUT_TYPE_MICROPHONE	= 0x1002,
};

// ENCODER TYPE (RECORD PROPERTY)
//
enum {

	QCAP_ENCODER_TYPE_ZZNVCODEC		= 1005,
	QCAP_ENCODER_TYPE_GST_X264		= 1006,
	QCAP_ENCODER_TYPE_GST_VAAPI		= 1007,
	QCAP_ENCODER_TYPE_GST_NVENC		= 1008,
	QCAP_ENCODER_TYPE_CCIMS			= 1009,

};

// DECODER TYPE (PLAYBACK PROPERTY)
//
enum {

	QCAP_DECODER_TYPE_ZZNVCODEC		= 1005,

};

// AUDIO SAMPLE FORMAT
//
enum {
	QCAP_SAMPLE_FMT_NONE		= -1,
	QCAP_SAMPLE_FMT_U8			= 0,
	QCAP_SAMPLE_FMT_S16,
	QCAP_SAMPLE_FMT_S32,
	QCAP_SAMPLE_FMT_FLT,
	QCAP_SAMPLE_FMT_DBL,
	QCAP_SAMPLE_FMT_U8P,
	QCAP_SAMPLE_FMT_S16P,
	QCAP_SAMPLE_FMT_S32P,
	QCAP_SAMPLE_FMT_FLTP,
	QCAP_SAMPLE_FMT_DBLP,
};

// ENCODER VIDEO FORMAT (RECORD PROPERTY)
//
enum {
	QCAP_ENCODER_FORMAT_MJPEG		= 0x1000,
	QCAP_ENCODER_FORMAT_JPEG		= 0x1001,
	QCAP_ENCODER_FORMAT_PCM_BE		= 0x1002,
	QCAP_ENCODER_FORMAT_PCM_LE		= QCAP_ENCODER_FORMAT_PCM,
	QCAP_ENCODER_FORMAT_S302M		= 0x1003,

	QCAP_DECODER_FORMAT_MJPEG		= 0x1000,

	QCAP_ENCODER_FORMAT_UNDEFINED	= 0xFFFFFFFF,
	QCAP_DECODER_FORMAT_UNDEFINED	= 0xFFFFFFFF,
};

// RECORD MODE (RECORD PROPERTY)
//
enum {
	QCAP_RECORD_MODE_FIXQP	= 0x1000,
};

// device custom property
enum {
	// compatible with QCAP windows
	QCAP_PROPERTY_HARDWARE_VERSION = 15,			// [out, ULONG]
	QCAP_PROPERTY_HARDWARE_DEINTERLACE = 200,		// [in, ULONG] 0:enabled, 1:disabled
	QCAP_PROPERTY_720_OUTPUT = 214,					// [in, ULONG] 0:960, 1:720
	QCAP_PROPERTY_CAMERA_TYPE = 226,				// [in, ULONG] 0:TVI, 1:AHD, 2:CVI
	QCAP_PROPERTY_AUDIO_CHANNEL = 257,				// [in, ULONG] 0:normal, 1:8ch
	QCAP_PROPERTY_MEDICAL_MODE = 276,				// [in, ULONG] 0:normal, 1:medical
	QCAP_PROPERTY_MUTE_LOOPTHROUGH = 334,			// [in, ULONG] 0:unmute, 1:mute
	QCAP_PROPERTY_COLORSPACE = 370,					// [out, ULONG]
	QCAP_PROPERTY_BITRATE = 403,					// [in, ULONG]
	QCAP_PROPERTY_SURROUND_MODE = 610,				// [in/out, ULONG] 0:normal, 2:two-span-mode, 3:three-span-mode 4:quad-mode
	QCAP_PROPERTY_SDI12G_MODE = 613,				// [in, ULONG] 0:QUAD-Link, 1:2SI
	QCAP_PROPERTY_OSD_X = 915,						// [in, LONG]
	QCAP_PROPERTY_OSD_Y = 916,						// [in, LONG]
	QCAP_PROPERTY_OSD_FONT_SIZE = 917,				// [in, LONG]
	QCAP_PROPERTY_OSD_NUMBER = 920,					// [in, LONG]
	QCAP_PROPERTY_OSD_TEXT = 921,					// [in, char*, NULL-terminated]
	QCAP_PROPERTY_OSD_FONT_STYLE = 929,				// [in, LONG]
	QCAP_PROPERTY_BOARD_MEMORY = 958,				// [in/out: BYTE*, <= 16 bytes]
	QCAP_PROPERTY_BOARD_MEMORY_ACCESS = 959,		// [in, LONG]
	QCAP_PROPERTY_VBI_LINES_ENABLE = 236,			// [in, ULONG] 0:disable, 1:enable
	QCAP_PROPERTY_VBI_LINES = 237,					// [in, ULONG] 0~84
	QCAP_PROPERTY_VBI_MIX_MODE = 229,				// [in, ULONG] 0:VANC, 1:HANC

	// for linux_base
	QCAP_PROPERTY_TIME_ZONE = 10000,			// [in, LONG]
	QCAP_PROPERTY_NO_SIGNAL_NO_OUTPUT,			// [in, ULONG] 0:no-signal image, 1:freeze
	QCAP_PROPERTY_NO_SIGNAL_YUV_PATTERN,		// [in, ULONG] 0x00YYVVUU
	QCAP_PROPERTY_IDEAL_PTS,					// [in/out, ULONG] boolean value
	QCAP_PROPERTY_DOUBLE_FRAME_RATE,			// [in/out, ULONG] boolean value
	QCAP_PROPERTY_IO_METHOD,					// [in/out, ULONG] 0:mmap, 1:userptr
	QCAP_PROPERTY_VO_BACKEND,					// [in/out, ULONG] 0:sdl, 1:gstreamer, 2:davmf
	QCAP_PROPERTY_PSF,							// [out, ULONG] 0:progressive, 1:PSF

	// for hisiv_base
	QCAP_PROPERTY_LINEIN_SOURCE = 20000,		// in,out: ULONG
	QCAP_PROPERTY_CLONE_WINDOW,					// in,out: HWND
	QCAP_PROPERTY_ROTATE,						// in,out: ULONG (in degrees, 0~360)
	QCAP_PROPERTY_COMPOSITE_SOURCE,				// in,out: ULONG
	QCAP_PROPERTY_PIPELINE_MODE,				// in,out: ULONG
	QCAP_PROPERTY_LOWDELAY_MODE,				// in,out: ULONG
	QCAP_PROPERTY_BIND_BS,						// in,out: PVOID (broadcast server)
	QCAP_PROPERTY_BIND_BS_0 = QCAP_PROPERTY_BIND_BS,					// in,out: PVOID (broadcast server)
	QCAP_PROPERTY_BIND_BS_1,					// in,out: PVOID (broadcast server)
	QCAP_PROPERTY_BIND_BS_2,					// in,out: PVOID (broadcast server)
	QCAP_PROPERTY_BIND_BS_3,					// in,out: PVOID (broadcast server)
	QCAP_PROPERTY_BIND_SR,						// in,out: UINT (share record number)
	QCAP_PROPERTY_BIND_SR_0 = QCAP_PROPERTY_BIND_SR,					// in,out: UINT (share record number)
	QCAP_PROPERTY_BIND_SR_1,					// in,out: UINT (share record number)
	QCAP_PROPERTY_BIND_SR_2,					// in,out: UINT (share record number)
	QCAP_PROPERTY_BIND_SR_3,					// in,out: UINT (share record number)
	QCAP_PROPERTY_AI_TRACK_MODE,				// in,out: ULONG (0:normal, 1:both_left, 2:both_right)
	QCAP_PROPERTY_CROP_RATIO,					// in: [x:LONG, y:LONG, w:LONG, h:LONG] (0~999)
	QCAP_PROPERTY_BIND_WND_0,					// in,out: HWND
	QCAP_PROPERTY_BIND_WND_1,					// in,out: HWND
	QCAP_PROPERTY_BIND_WND_2,					// in,out: HWND
	QCAP_PROPERTY_BIND_WND_3,					// in,out: HWND

	QCAP_PROPERTY_AUDIO_RENDERER_VOLUME,		// in,out: ULONG

	QCAP_PROPERTY_ISP_PUB,						// in,out: ISP_PUB_ATTR_S
	QCAP_PROPERTY_ISP_STATISTICS_CFG,			// in,out: ISP_STATISTICS_CFG_S
	QCAP_PROPERTY_ISP_VDTIMEOUT,				// out: ULONG[2] { ISP_VD_TYPE_E, HI_U32 }
	QCAP_PROPERTY_ISP_FOCUS_STATISTICS,			// out: ISP_AF_STATISTICS_S
	QCAP_PROPERTY_ISP_EXPOSURE,					// in,out: ISP_EXPOSURE_ATTR_S
	QCAP_PROPERTY_ISP_WB,						// in,out: ISP_WB_ATTR_S
	QCAP_PROPERTY_ISP_LIGHTBOXGAIN,				// out: ISP_AWB_Calibration_Gain_S
	QCAP_PROPERTY_ISP_EXPOSUREINFO,				// out: ISP_EXP_INFO_S
	QCAP_PROPERTY_VPSS_NR_ATTR,					// in: [HI_BOOL, VPSS_NR_ATTR_S]
	QCAP_PROPERTY_ISP_CSC,						// in,out: ISP_CSC_ATTR_S
	QCAP_PROPERTY_ISP_SATURATION,				// in: ISP_SATURATION_ATTR_S
	QCAP_PROPERTY_VI_LDC,						// in,out: VI_LDC_ATTR_S
	QCAP_PROPERTY_ISP_AWB_EX,					// in,out: ISP_AWB_ATTR_EX_S
	QCAP_PROPERTY_ISP_WB_INFO,					// out,ISP_WB_INFO_S
	QCAP_PROPERTY_VPSS_MIRROR_ATTR,				// in:[HI_BOOL]
	QCAP_PROPERTY_VPSS_FLIP_ATTR,				// in:[HI_BOOL]
	QCAP_PROPERTY_VPSS_LDC_ATTR,				// in: [HI_BOOL, LDC_ATTR_S]
	QCAP_PROPERTY_ISP_DRC_ATTR,					// in,out:ISP_DRC_ATTR_S
	QCAP_PROPERTY_ISP_LDCI_ATTR, 				// in,out:ISP_LDCI_ATTR_S
	QCAP_PROPERTY_ISP_DEHAZE_ATTR, 				// in,out:ISP_DEHAZE_ATTR_S
	QCAP_PROPERTY_ISP_GAMMA_ATTR, 				// in,out:ISP_GAMMA_ATTR_S
	QCAP_PROPERTY_ISP_COLORMATRIX_ATTR, 		// in,out:ISP_COLORMATRIX_ATTR_S
	QCAP_PROPERTY_ISP_NR_ATTR, 					// in,out:ISP_NR_ATTR_S
	QCAP_PROPERTY_ISP_SHARPEN_ATTR,				// in,out:ISP_SHARPEN_ATTR_S
	QCAP_PROPERTY_ISP_SHADING_ATTR,				// in,out:ISP_SHADING_ATTR_S
	QCAP_PROPERTY_ISP_SHADING_LUT_ATTR,	    	// in,out:ISP_SHADING_LUT_ATTR_S
	QCAP_PROPERTY_ISP_DEMOSAIC_ATTR,    	   	// in,out:ISP_DEMOSAIC_ATTR_S
	QCAP_PROPERTY_ISP_DP_DYNAMIC_ATTR,			// in,out:ISP_DP_DYNAMIC_ATTR_S
	QCAP_PROPERTY_VPSS_NRX_PARAM,				// in: [VPSS_GRP_NRX_PARAM_S]

	QCAP_PROPERTY_VPSS_CHN_ATTR_0,				// in,out: VPSS_CHN_ATTR_S
	QCAP_PROPERTY_VPSS_CHN_ATTR_1,				// in,out: VPSS_CHN_ATTR_S
	QCAP_PROPERTY_VPSS_CHN_ATTR_2,				// in,out: VPSS_CHN_ATTR_S
	QCAP_PROPERTY_VPSS_CHN_ATTR_3,				// in,out: VPSS_CHN_ATTR_S
	QCAP_PROPERTY_VPSS_CHN_CROP_0,				// in,out: VPSS_CROP_INFO_S
	QCAP_PROPERTY_VPSS_CHN_CROP_1,				// in,out: VPSS_CROP_INFO_S
	QCAP_PROPERTY_VPSS_CHN_CROP_2,				// in,out: VPSS_CROP_INFO_S
	QCAP_PROPERTY_VPSS_CHN_CROP_3,				// in,out: VPSS_CROP_INFO_S
};

// share record custom property
enum {
	QCAP_SHARE_RECORD_PROPERTY_SOURCE_FRAME_RATE = 100, // in: double, DEPRECATED!

	QCAP_SRPROP_SOURCE_FRAME_RATE = 100,	// in: double
	QCAP_SRPROP_ROTATE,						// in,out: ULONG (in degrees, 0~360)
	QCAP_SRPROP_VO_BACKEND,					// [in/out, ULONG] 0:sdl, 1:gstreamer, 2:davmf

	// private use only
	QCAP_SRPROP_BINDING_VPSS = 10800,		// in: [ULONG, ULONG]
	QCAP_SRPROP_BINDING_AVS,				// in: [ULONG, ULONG]
};

// broadcast server custom property
enum {
	QCAP_PROPERTY_NDI_CONFIG				= 100, // in,out: char*, DEPRECATED!

	QCAP_BSPROP_NDI_CONFIG = 100,			// in,out: char*
	QCAP_BSPROP_SENDING_IP,					// in,out: char*
	QCAP_BSPROP_SOURCE_FRAME_RATE,			// in: double
	QCAP_BSPROP_RECEIVING_IP,				// in,out: char*
	QCAP_BSPROP_MULTICAST_IP,				// in,out: char*
	QCAP_BSPROP_FRAME_RATE,					// in: [double, double]

	// private use only
	QCAP_BSPROP_BINDING_VPSS = 10800,		// in: [ULONG, ULONG]
	QCAP_BSPROP_BINDING_AVS,				// in: [ULONG, ULONG]
};

// broadcast client custom property
enum {
	QCAP_BCPROP_SENDING_IP = 100,				// in,out: char*
	QCAP_BCPROP_PIPELINE_MODE,					// in,out: ULONG
	QCAP_BCPROP_VIDEO_DECODE,					// in,out: BOOL
	QCAP_BCPROP_AUDIO_DECODE,					// in,out: BOOL
	QCAP_BCPROP_NDI_PTZ_IS_SUPPORTED,			// out: BOOL
	QCAP_BCPROP_NDI_PTZ_ZOOM,			 		// in: float 0.0 (zoomed in) ... 1.0 (zoomed out)
	QCAP_BCPROP_NDI_PTZ_ZOOM_SPEED,			 	// in: float -1.0 (zoom outwards) ... +1.0 (zoom inwards)
	QCAP_BCPROP_NDI_PTZ_PAN_TILT,			 	// in: [float -1.0 (left) ... 0.0 (centered) ... +1.0 (right), float -1.0 (bottom) ... 0.0 (centered) ... +1.0 (top)]
	QCAP_BCPROP_NDI_PTZ_PAN_TILT_SPEED,			// in: [float -1.0 (moving right) ... 0.0 (stopped) ... +1.0 (moving left), float -1.0 (down) ... 0.0 (stopped) ... +1.0 (moving up)]
	QCAP_BCPROP_NDI_PTZ_STORE_PRESET,			// in: int 0 ... 99
	QCAP_BCPROP_NDI_PTZ_RECALL_PRESET,			// in: [int 0 ... 99, float 0.0(as slow as possible) ... 1.0(as fast as possible) The speed at which to move to the new preset]
	QCAP_BCPROP_NDI_PTZ_AUTO_FOCUS,				// in: NULL
	QCAP_BCPROP_NDI_PTZ_FOCUS,					// in: float 0.0 (focused to infinity) ... 1.0 (focused as close as possible)
	QCAP_BCPROP_NDI_PTZ_FOCUS_SPEED,			// in: float -1.0 (focus outwards) ... +1.0 (focus inwards)
	QCAP_BCPROP_NDI_PTZ_WHITE_BALANCE_AUTO,		// in: NULL
	QCAP_BCPROP_NDI_PTZ_WHITE_BALANCE_INDOOR,	// in: NULL
	QCAP_BCPROP_NDI_PTZ_WHITE_BALANCE_OUTDOOR,	// in: NULL
	QCAP_BCPROP_NDI_PTZ_WHITE_BALANCE_ONESHOT,	// in: NULL
	QCAP_BCPROP_NDI_PTZ_WHITE_BALANCE_MANUAL,	// in: [float 0.0(not red) ... 1.0(very red), float 0.0(not blue) ... 1.0(very blue)]
	QCAP_BCPROP_NDI_PTZ_EXPOSURE_AUTO,			// in: NULL
	QCAP_BCPROP_NDI_PTZ_EXPOSURE_MANUAL,		// in: float 0.0(dark) ... 1.0(light)
	QCAP_BCPROP_NDI_KVM_IS_SUPPORTED,			// out: BOOL
	QCAP_BCPROP_NDI_KVM_SEND_LEFT_MOUSE,		// in: BOOL 0 (release), 1 (click)
	QCAP_BCPROP_NDI_KVM_SEND_MIDDLE_MOUSE,		// in: BOOL 0 (release), 1 (click)
	QCAP_BCPROP_NDI_KVM_SEND_RIGHT_MOUSE,		// in: BOOL 0 (release), 1 (click)
	QCAP_BCPROP_NDI_KVM_SEND_WHEEL_MOUSE,		// in: int 0 (vertical), 1 (horizontal)
	QCAP_BCPROP_NDI_KVM_SEND_MOUSE_POSITION,	// in: [float 0.0 (left of the screen) ... 1.0(right of the screen), float 0.0(top of the screen) ... 1.0(botton of the screen)]
	QCAP_BCPROP_NDI_KVM_SEND_KEYBOARD,			// in: BOOL 0 (release), 1 (click)   in: int (key_sym_value)
};

// custom property
enum {
	// for hisiv_base
	QCAP_PROPERTY_FILE_PIPELINE_MODE = 20000,		// in,out: ULONG
	QCAP_PROPERTY_TSENSOR_DETECT,					// in,out: ULONG
	QCAP_PROPERTY_HDMI_VO_PARAM,
	QCAP_PROPERTY_COLOR_RANGE,
	QCAP_PROPERTY_VO_PARAM,
	QCAP_PROPERTY_VO_CSC_PARAM,
	QCAP_PROPERTY_MCU_VERSION,
	QCAP_PROPERTY_COPY_PROTECT_KEY,
	QCAP_PROPERTY_COPY_PROTECT_CHECK,
	QCAP_PROPERTY_GPIO_OUTPUT,
	QCAP_PROPERTY_AUDIO_MIXER_SOURCE_VOLUME,
	QCAP_PROPERTY_LED,								// in: QCAP_PROPERTY_LED_VALUE
	QCAP_PROPERTY_LED_EX,							// in: QCAP_PROPERTY_LED_EX_VALUE
	QCAP_PROPERTY_BUTTON,
	QCAP_PROPERTY_VOLUME,
	QCAP_PROPERTY_NDI_MODE,							// out: QCAP_PROPERTY_NDI_MODE_RESPONSE
	QCAP_PROPERTY_NOISE_GATE,
	QCAP_PROPERTY_NDI_KEY,							// in/out: QCAP_PROPERTY_NDI_KEY_VALUE
	QCAP_PROPERTY_BUTTON_FD,						// out: int
	QCAP_PROPERTY_BUTTON_EX,						// out: QCAP_PROPERTY_BUTTON_EX_RESPONSE
	QCAP_PROPERTY_OLED,
};

// file custom property
enum {
	QCAP_FILEPROP_PIPELINE_MODE				= 804, // in,out: ULONG, DEPRECATED!
};

#pragma pack(push)
#pragma pack(1)

struct QCAP_PROPERTY_HDMI_VO_PARAM_VALUE
{
	int width;
	int height;
	int fps;
};

struct QCAP_PROPERTY_COLOR_RANGE_VALUE
{
	ULONG color_range; // QCAP_COLORRANGE_TYPE_xxx
};

struct QCAP_PROPERTY_COLOR_RANGE_RESPONSE
{
	ULONG color_range; // QCAP_COLORRANGE_TYPE_xxx
};

struct QCAP_PROPERTY_VO_PARAM_VALUE
{
	int id;
	int width;
	int height;
	int fps;
	int flags; // Bit0: progressive / interlaced, Bit1: 1000 / 1001
};

struct QCAP_PROPERTY_VO_PARAM_RESPONSE
{
	int id; // in

	// output fields
	int width;
	int height;
	int fps;
	int flags; // Bit0: progressive / interlaced, Bit1: 1000 / 1001
};

struct QCAP_PROPERTY_VO_CSC_PARAM_VALUE
{
	int id;
	uint32_t luma;
	uint32_t contrast;
	uint32_t hue;
	uint32_t saturation;
};

struct QCAP_PROPERTY_MCU_VERSION_RESPONSE
{
	BYTE ver[4];
};

struct QCAP_PROPERTY_COPY_PROTECT_KEY_VALUE
{
	uint32_t key;
};

struct QCAP_PROPERTY_COPY_PROTECT_CHECK_RESPONSE
{
	int result;
};

struct QCAP_PROPERTY_GPIO_OUTPUT_VALUE
{
	uint32_t port; // 0x90 = PORT A, 0x91 = PORT B, 0x92 = PORT C
	uint32_t pin; // 0x00 ~ 0x1F, pin number
	uint32_t value; // 0X00 = LOW, 0X01 = HIGH
};

struct QCAP_PROPERTY_LED_VALUE
{
	int index;

	unsigned char period;
	unsigned char level;
	unsigned char start;
	unsigned char end;
};

struct QCAP_PROPERTY_LED_EX_VALUE
{
	int index;

	int argc;
	unsigned char args[8];

	int async_mode;
};

struct QCAP_PROPERTY_BUTTON_RESPONSE
{
	int size;

	uint32_t state[8];
};

struct QCAP_PROPERTY_VOLUME_VALUE
{
	int index;
	int volume;

	int async_mode;
};

struct QCAP_PROPERTY_VOLUME_RESPONSE
{
	int index; // in
	int volume[8]; // out
};

struct QCAP_PROPERTY_NDI_MODE_RESPONSE
{
	int mode; // 0: no key; 1: yuan key; 2: custom key
};

struct QCAP_PROPERTY_NOISE_GATE_VALUE
{
	// 0: LineIn1; 1: LineIn2; 2: LineIn3; 3: LineIn4; 4: LineOut1
	int index;

	// 0: Off
	// 1: -34; 2: -37; 3: -40; 4: -43; 5: -46; 6: -52; 7: -58dB
	// 8: -64; 9: -67; 10: -70; 11: -73; 12: -76; 13: -82dB
	int value;

	// 0: 50; 1: 100; 2: 150; 3: 200ms
	int delay;
};

struct QCAP_PROPERTY_NDI_KEY_VALUE
{
	char* pszNDIKey;
};

struct QCAP_PROPERTY_BUTTON_EX_RESPONSE
{
	uint32_t state[4]; // 128 button state
};

struct QCAP_PROPERTY_TSENSOR_DETECT_RESPONSE
{
	double value;
};

struct QCAP_PROPERTY_OLED_VALUE
{
	int index;
	int channel;
	int size;
	unsigned char data[64];
};

struct QCAP_BCPROP_NDI_KVM_SEND_KEYBOARD_VALUE
{
	int type; // 0: release, 1: press
	int key;
};

#pragma pack(pop)

typedef QRETURN (QCAP_EXPORT *PF_MEDIA_BROADCAST_SERVER_CALLBACK)( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, double dSampleTime /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, PVOID pUserData /*IN*/ );

// extension function
QCAP_EXT_API double QCAP_EXPORT QCAP_GET_TIME();
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_SET_CUSTOM_PROPERTY(ULONG nProperty, BYTE* pValue, ULONG nBytes);
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GET_CUSTOM_PROPERTY(ULONG nProperty, BYTE* pValue, ULONG nBytes);
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_SYNCHRONIZED_SNAPSHOT_BMP( ULONG nFileArgs /*IN*/, CHAR * pszFilePathName1 /*IN*/, PVOID pDevice1 /*IN*/, CHAR * pszFilePathName2 /*IN*/, PVOID pDevice2 /*IN*/, ... );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_SYNCHRONIZED_SNAPSHOT_JPG( ULONG nFileArgs /*IN*/, CHAR * pszFilePathName1 /*IN*/, ULONG nQuality1 /*IN*/, PVOID pDevice1 /*IN*/, CHAR * pszFilePathName2 /*IN*/, ULONG nQuality2 /*IN*/, PVOID pDevice2 /*IN*/, ... );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_COLORSPACE_YUY2_TO_UYVY(   BYTE * pSrcFrameBuffer /*IN*/, ULONG nSrcWidth /*IN*/, ULONG nSrcHeight /*IN*/, ULONG nSrcPitch /*IN*/, BYTE * pDstFrameBuffer /*OUT*/, ULONG nDstWidth /*IN*/, ULONG nDstHeight /*IN*/, ULONG nDstPitch /*IN*/, BOOL bHorizontalMirror, BOOL bVerticalMirror );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_COLORSPACE_YV12_TO_UYVY(   BYTE * pSrcFrameBuffer /*IN*/, ULONG nSrcWidth /*IN*/, ULONG nSrcHeight /*IN*/, ULONG nSrcPitch /*IN*/, BYTE * pDstFrameBuffer /*OUT*/, ULONG nDstWidth /*IN*/, ULONG nDstHeight /*IN*/, ULONG nDstPitch /*IN*/, BOOL bHorizontalMirror, BOOL bVerticalMirror );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_COLORSPACE_NV12_TO_UYVY(   BYTE * pSrcFrameBuffer /*IN*/, ULONG nSrcWidth /*IN*/, ULONG nSrcHeight /*IN*/, ULONG nSrcPitch /*IN*/, BYTE * pDstFrameBuffer /*OUT*/, ULONG nDstWidth /*IN*/, ULONG nDstHeight /*IN*/, ULONG nDstPitch /*IN*/, BOOL bHorizontalMirror, BOOL bVerticalMirror );

// reference counted buffer
QCAP_EXT_API PVOID QCAP_EXPORT QCAP_BUFFER_GET_RCBUFFER( BYTE * pBuffer /*IN*/, ULONG nBufferLen /*IN*/ );
QCAP_EXT_API PVOID QCAP_EXPORT QCAP_RCBUFFER_LOCK_DATA( PVOID pRCBuffer /*IN*/ );
QCAP_EXT_API void QCAP_EXPORT QCAP_RCBUFFER_UNLOCK_DATA( PVOID pRCBuffer /*IN*/ );
QCAP_EXT_API void QCAP_EXPORT QCAP_RCBUFFER_ADD_REF( PVOID pRCBuffer /*IN*/ );
QCAP_EXT_API void QCAP_EXPORT QCAP_RCBUFFER_RELEASE( PVOID pRCBuffer /*IN*/ );

// code-block lock
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_BLOCK_LOCK( PVOID* ppBlockLock );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_DESTROY_BLOCK_LOCK( PVOID pBlockLock );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GRANT_BLOCK_LOCK( PVOID pBlockLock, BOOL bGrant );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_ENTER_BLOCK_LOCK( PVOID pBlockLock );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_LEAVE_BLOCK_LOCK( PVOID pBlockLock );

// A/V frame pool
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_VIDEO_FRAME_POOL( ULONG nColorSpaceType /*IN*/, ULONG nFrameWidth /*IN*/, ULONG nFrameHeight /*IN*/, ULONG nFrames, PVOID* ppFramePool );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_VIDEO_FRAME_POOL_EX( ULONG nColorSpaceType /*IN*/, ULONG nFrameWidth /*IN*/, ULONG nFrameHeight /*IN*/, ULONG nFrames, BOOL b,PVOID* ppFramePool );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_AUDIO_FRAME_POOL( ULONG nChannels /*IN*/, ULONG nBitsPerSample /*IN*/, ULONG nSampleFrequency, ULONG nFrameSize, ULONG nFrames, PVOID* ppFramePool );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_AUDIO_FRAME_POOL_EX( ULONG nChannels /*IN*/, ULONG nSampleFmt /*IN*/, ULONG nSampleFrequency, ULONG nFrameSize, ULONG nFrames, PVOID* ppFramePool );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GET_FRAME_BUFFER( PVOID pFramePool, BYTE** ppFrameBuffer /*IN*/, ULONG* pFrameBufferSize );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_DESTROY_FRAME_POOL( PVOID pFramePool );

// audio resampler
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_AUDIO_RESAMPLER( ULONG nSrcChannels /*IN*/, ULONG nSrcBitsPerSample /*IN*/, ULONG nSrcSampleFrequency, ULONG nDstChannels /*IN*/, ULONG nDstBitsPerSample /*IN*/, ULONG nDstSampleFrequency, PVOID* ppAudioResampler );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_DESTROY_AUDIO_RESAMPLER( PVOID pAudioResampler );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_AUDIO_RESAMPLER_SET_AUDIO_BUFFER( PVOID pAudioResampler, BYTE * pSrcFrameBuffer /*IN*/, ULONG nSrcFrameBufferLen /*IN*/, double dSampleTime);
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_AUDIO_RESAMPLER_GET_RESAMPLED_AUDIO_BUFFER( PVOID pAudioResampler, BYTE * pDstFrameBuffer /*IN*/, ULONG nDstFrameBufferLen /*IN*/, double * pSampleTime);

// stream buffer pool
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_STREAM_BUFFER_POOL( ULONG nCount, PVOID* ppStreamBufferPool );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GET_STREAM_BUFFER( PVOID pStreamBufferPool, BYTE** ppStreamBuffer /*IN*/, ULONG* pStreamBufferLen );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_DESTROY_STREAM_BUFFER_POOL( PVOID pStreamBufferPool );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_COPY_NATIVE_STREAM_BUFFER( BYTE* pDstStreamBuffer /*IN*/, ULONG nDstStreamBufferLen, BYTE* pSrcStreamBuffer /*IN*/, ULONG nSrcStreamBufferLen, BOOL bIsKeyFrame, double dSrcSampleTime );

// audio delay queue
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_AUDIO_DELAY_QUEUE( PVOID* ppFrameQueue );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_SET_AUDIO_DELAY( PVOID pFrameQueue, ULONG nMaxDelayInMS );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_PUSH_AUDIO_FRAME_BUFFER( PVOID pFrameQueue, BYTE* pSrcFrameBuffer, ULONG nSrcFrameBufferSize, BYTE** ppDstFrameBuffer, ULONG* pDstFrameBufferSize);
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_DESTROY_AUDIO_DELAY_QUEUE( PVOID pFrameQueue );

// animation extension
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_STEP_ANIMATION_CLIP2( PVOID pClip /*IN*/, UINT * pFrameNum /*OUT*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/);
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GET_ANIMATION_CLIP_BKCOLOR_PROPERTY( PVOID pClip /*IN*/, float* pR, float* pG, float* pB );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_SET_ANIMATION_CLIP_BKCOLOR_PROPERTY( PVOID pClip /*IN*/, float r, float g, float b );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GET_ANIMATION_CLIP_SPRITE_SCALE_STYLE( PVOID pClip /*IN*/, UINT iSpriteNum /*IN*/, ULONG* pScaleStyle ); // QCAP_SCALE_STYLE_STRETCH / QCAP_SCALE_STYLE_FIT / QCAP_SCALE_STYLE_FILL
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_SET_ANIMATION_CLIP_SPRITE_SCALE_STYLE( PVOID pClip /*IN*/, UINT iSpriteNum /*IN*/, ULONG nScaleStyle ); // QCAP_SCALE_STYLE_STRETCH / QCAP_SCALE_STYLE_FIT / QCAP_SCALE_STYLE_FILL
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GET_ANIMATION_CLIP_SPRITE_BKCOLOR( PVOID pClip /*IN*/, UINT iSpriteNum /*IN*/, float* pR, float* pG, float* pB );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_SET_ANIMATION_CLIP_SPRITE_BKCOLOR( PVOID pClip /*IN*/, UINT iSpriteNum /*IN*/, float r, float g, float b );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GET_ANIMATION_CLIP_SPRITE_STATIC( PVOID pClip /*IN*/, UINT iSpriteNum /*IN*/, BOOL* pStatic );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_SET_ANIMATION_CLIP_SPRITE_STATIC( PVOID pClip /*IN*/, UINT iSpriteNum /*IN*/, BOOL bStatic );

// broadcast server extension
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_BROADCAST_TS_OVER_UDP_SERVER_EX( UINT iSvrNum /*IN*/ /*0 ~ 63*/, CHAR * pszURL /*IN*/, PVOID * ppServer /*OUT*/, CHAR * pszNetworkAdapterIP DEFVAL( NULL /*IN*/), ULONG nServiceID DEFVAL( 1 /*IN*/), CHAR * pszServiceName DEFVAL( NULL /*IN*/), CHAR * pszServiceProviderName DEFVAL( NULL /*IN*/), BOOL bEnableCBRMode DEFVAL( TRUE /*IN*/), ULONG nTransferBitRate DEFVAL( 0 /*IN*/ /*0 = FREE*/), ULONG nOriginalNetworkID DEFVAL( 1 /*IN*/), ULONG nTransportStreamID DEFVAL( 1 /*IN*/), ULONG nPMT_PID DEFVAL( 4096 /*IN*/), ULONG nPCR_PID DEFVAL( 4097 /*IN*/), ULONG nVideo_PID DEFVAL( 256 /*IN*/), ULONG nAudio_PID DEFVAL( 257 /*IN*/), ULONG nVideo_CodecID DEFVAL( 0 /*IN*/ /*0 = AUTO*/), ULONG nAudio_CodecID DEFVAL( 0 /*IN*/ /*0 = AUTO*/), ULONG nPCRInterval DEFVAL( 0 /*IN*/ /*ms*/ ), ULONG nTTL DEFVAL( 7 /*IN*/ )); // TOTOAL SESSION = 1
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_BROADCAST_TS_OVER_SRT_SERVER_EX( UINT iSvrNum /*IN*/ /*0 ~ 63*/, PVOID * ppServer /*OUT*/, ULONG nNetworkPort_SRT = 1234 , CHAR * pszPassPhrase = NULL, ULONG nPBKeyLen = 16,  CHAR * pszNetworkAdapterIP =  NULL , ULONG nServiceID = 1 , CHAR * pszServiceName = NULL , CHAR * pszServiceProviderName = NULL , ULONG nTransferBitRate = 0 , ULONG nOriginalNetworkID = 1 , ULONG nTransportStreamID = 1, ULONG nPMT_PID = 4096 , ULONG nPCR_PID = 4097 , ULONG nVideo_PID = 256, ULONG nAudio_PID = 257, ULONG nVideo_CodecID = 0 /*IN*/ /*0 = AUTO*/, ULONG nAudio_CodecID = 0 /*IN*/ /*0 = AUTO*/, ULONG nPCRInterval = 0 /*IN*/ /*ms*/, ULONG nSRTLatency = 120 /*IN*/ /*ms*/, ULONG nSRTBandwidth = 25 /*IN*/ /*100%*/ );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_BROADCAST_TS_OVER_SRT_WEB_PORTAL_SERVER_EX( UINT iSvrNum /*IN*/ /*0 ~ 63*/, CHAR * pszURL /*IN*/, PVOID * ppServer /*OUT*/, CHAR * pszPassPhrase = NULL /*IN*/,  ULONG nPBKeyLen = 16, ULONG nServiceID = 1 /*IN*/, CHAR * pszServiceName = NULL /*IN*/, CHAR * pszServiceProviderName = NULL /*IN*/, ULONG nTransferBitRate = 0 /*IN*/ /*0 = FREE*/, ULONG nOriginalNetworkID = 1 /*IN*/, ULONG nTransportStreamID = 1 /*IN*/, ULONG nPMT_PID = 4096 /*IN*/, ULONG nPCR_PID = 4097 /*IN*/, ULONG nVideo_PID = 256 /*IN*/, ULONG nAudio_PID = 257 /*IN*/, ULONG nVideo_CodecID = 0 /*IN*/ /*0 = AUTO*/, ULONG nAudio_CodecID = 0 /*IN*/ /*0 = AUTO*/, ULONG nPCRInterval = 0 /*IN*/ /*ms*/, ULONG nSRTLatency = 120 /*IN*/ /*ms*/, ULONG nSRTOverheadBandwidth = 25 /*IN*/ /*100%*/ ); // TOTOAL SESSION = 1
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_MEDIA_BROADCAST_SERVER_CALLBACK( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, PF_MEDIA_BROADCAST_SERVER_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GET_BROADCAST_SERVER_INFO( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, ULONG * pConnections /*OUT*/ );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_SET_BROADCAST_SERVER_CUSTOM_PROPERTY( PVOID pServer /*IN*/, ULONG nProperty, BYTE* pValue, ULONG nBytes);
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GET_BROADCAST_SERVER_CUSTOM_PROPERTY( PVOID pServer /*IN*/, ULONG nProperty, BYTE* pValue, ULONG* pBytes);

// broadcast client extension
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_BROADCAST_SRT_CLIENT_EX( UINT iCliNum /*IN*/ /*0 ~ 63*/, CHAR * pszURL /*IN*/, PVOID * ppClient /*OUT*/, ULONG nSRTLatency = 120 , ULONG nSRTOverheadBandwidth = 25, ULONG nDecoderType = QCAP_DECODER_TYPE_SOFTWARE , HWND hAttachedWindow = NULL, BOOL bThumbDraw = FALSE, BOOL bMaintainAspectRatio = FALSE /*IN*/, CHAR * pszPassPhrase = NULL /*IN*/,  ULONG nPBKeyLen = 16);
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_SET_BROADCAST_CLIENT_CUSTOM_PROPERTY_EX( PVOID pClient /*IN*/, ULONG nProperty, BYTE* pValue, ULONG nBytes);
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GET_BROADCAST_CLIENT_CUSTOM_PROPERTY_EX( PVOID pClient /*IN*/, ULONG nProperty, BYTE* pValue, ULONG* pBytes);

// share record extension
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_SHARE_RECORD_CUSTOM_PROPERTY( UINT iRecNum /*IN*/, ULONG nProperty, BYTE* pValue, ULONG nBytes);
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_SHARE_RECORD_CUSTOM_PROPERTY( UINT iRecNum /*IN*/, ULONG nProperty, BYTE* pValue, ULONG* pBytes);
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_SET_AUDIO_MX_SHARE_RECORD_VOLUME(UINT iRecNum /*IN*/, ULONG nTracks /*IN*/, UINT iMixNum /*IN*/, ULONG  nVolume /*IN*/);
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GET_AUDIO_MX_SHARE_RECORD_VOLUME(UINT iRecNum /*IN*/, ULONG nTracks /*IN*/, UINT iMixNum /*IN*/, ULONG* pVolume /*OUT*/);

// file extension
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_GET_FILE_CUSTOM_PROPERTY_EX( PVOID pFile /*IN*/, ULONG nProperty /*IN*/, BYTE * pValue /*OUT*/, ULONG* pBytes /*IN*/ );
QCAP_EXT_API QRESULT QCAP_EXPORT QCAP_SET_FILE_CUSTOM_PROPERTY_EX( PVOID pFile /*IN*/, ULONG nProperty /*IN*/, BYTE * pValue /*IN*/, ULONG nBytes /*IN*/ );

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
