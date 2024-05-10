/**
  ******************************************************************************
  * The header file for uvc class
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usbh_uvc_desc.h"
#include "usbh_uvc_intf.h"


/* Exported defines ----------------------------------------------------------*/


#define UVC_MAX_NUM_IN_TERMINAL			5
#define UVC_MAX_NUM_OUT_TERMINAL		2
#define UVC_MAX_NUM_CAMERA_TERMINAL		2
#define UVC_MAX_NUM_PROCESSING_UNIT		2
#define UVC_MAX_NUM_EXTENSION_UNIT		2
#define UVC_MAX_NUM_ENCODING_UNIT		2
#define UVC_MAX_NUM_SELECTOR_UNIT		2


#define USBH_MAX_NUM_MJPEG_FORMAT		30
#define USBH_MAX_NUM_UNCOMP_FRAME		30

#define USBH_MAX_NUM_VS_DESC			3
#define USBH_MAX_NUM_VS_ALTS			30

#define UVC_DETECT_EOF					0

#define UVC_USE_SOF						1  //if set to 0, sof interrupt can be disabled

#define HFNUM_MAX_FRNUM					0x3fff


/* Exported types ------------------------------------------------------------*/

typedef struct {
	u8  bFrameIndex;
	u8  bmCapabilities;
	u16 wWidth;
	u16 wHeight;
	u32 dwMinBitRate;
	u32 dwMaxBitRate;
	u32 dwMaxVideoFrameBufferSize;   //for mjpeg & uncompressed payload type descriptor
	u32 dwDefaultFrameInterval;
	u8  bFrameIntervalType;
	u32 *dwFrameInterval;
	u32 dwBytesPerLine;   //for frame based payload frame type descriptor
} uvc_vs_frame_t;


typedef struct {
	u8 type;
	u8 index;
	u8 bpp;
	u8 colorspace;
	u32 fcc;
	u32 flags;
	char name[32];
	u32 nframes;
	uvc_vs_frame_t *frame;
} uvc_vs_format_t;


typedef struct {
	struct list_head stream_list;
	struct list_head format_list;
	u32 intf_num;
	u32 format_num;
	u32 type;
} uvc_vs_stream_t;


typedef struct {
	void *p;
	u8 bInterfaceNumber;
	uvc_vc_header_desc_t *vcheader;
	struct list_head entity_chain;
	u8 entity_num;
	u8 *intr_ep;
	u8 *cs_intr_desc;
} uvc_vc_t;


typedef struct {
	void *p;
	usbh_ep_desc_t *endpoint;
} uvc_alt_t;


typedef struct {
	void *p;
	u8 bInterfaceNumber;
	u8 alt_num;
	u8 has_h264;
	uvc_alt_t altsetting[USBH_MAX_NUM_VS_ALTS];
	uvc_vs_input_header_desc_t *InputHeader;
	u32 nformat;
	uvc_vs_format_t *format;
} uvc_vs_t;


typedef struct {
	uvc_vc_t vc_intf;
	uvc_vs_t vs_intf[USBH_MAX_NUM_VS_DESC];
	u8 vs_num;
} uvc_cfg_t;


typedef struct {
	uvc_alt_t *altsetting;
	u8 bAlternateSetting;
	uvc_vs_t *cur_vs_intf;
	u8 bInterfaceNumber;
	u8 pipe;
	u8 ep_addr;
	u8 interval;
	u16 ep_size;
	u16 mps;
	u8 ep_type;
	u8 valid;
	u32 last_frame;
	u8 is_processing;
} uvc_setting_t;


typedef struct {
	u32 offset;
	u32 length;
} uvc_packet_desc_t;


typedef struct {
	u8 *addr;
	u32 index;
	u32 packet_num;
	u32 packet_length;
	uvc_packet_desc_t packet_info[0];
} uvc_urb_t;


typedef struct {
	uvc_setting_t cur_setting;
	uvc_vs_t *vs_intf;
	u8 stream_state;
	u8 stream_data_state;

	uvc_frame_t frame_buffer[UVC_VIDEO_MAX_FRAME];
	u8 *frame_buf;
	u32 frame_buffer_size;
	struct list_head frame_empty;
	struct list_head frame_chain;
	usb_os_sema_t frame_sema;
	u32 frame_cnt;
	u32 err_frame_cnt;

	uvc_urb_t *urb[UVC_URB_NUMS];
	u8 *uvc_buffer;
	u32 urb_buffer_size;
	u32 cur_urb;
	u32 cur_packet;
	volatile u32 cur_packet_state;

	usb_os_queue_t urb_wait_queue;
	usb_os_queue_t urb_giveback_queue;
	u8 last_fid;

	uvc_stream_control_t stream_ctrl;

	uvc_vs_format_t *def_format;
	uvc_vs_format_t *cur_format;
	uvc_vs_frame_t *cur_frame;

	usb_os_task_t decode_task;
} uvc_stream_t;


typedef struct {
	usb_host_t *host;

	uvc_cfg_t uvc_desc;
	struct list_head entity_list;
	struct list_head video_chain;

	uvc_stream_t stream[USBH_MAX_NUM_VS_DESC];

	usbh_uvc_cb_t *cb;

	u32 sof_cnt;
} usbh_uvc_host_t;


/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/*class module*/
int usbh_uvc_class_init(void);

void usbh_uvc_class_deinit(void);

/*stream module*/
int usbh_uvc_process_rx(uvc_stream_t *stream);

uvc_urb_t *usbh_uvc_urb_complete(uvc_stream_t *stream, uvc_urb_t *urb);

int usbh_uvc_set_video(uvc_stream_t *stream, int probe);

int usbh_uvc_get_video(uvc_stream_t *stream, int probe, u16 request);

int usbh_uvc_probe_video(uvc_stream_t *stream);

int usbh_uvc_commit_video(uvc_stream_t *stream);

int usbh_uvc_video_init(uvc_stream_t *stream);

int usbh_uvc_stream_init(uvc_stream_t *stream);

void usbh_uvc_stream_deinit(uvc_stream_t *stream);

void usbh_uvc_process_sof(usb_host_t *host);


/*parse module*/
int usbh_uvc_parse_cfgdesc(usb_host_t *host);

void usbh_uvc_desc_init(void);

void usbh_uvc_desc_free(void);



