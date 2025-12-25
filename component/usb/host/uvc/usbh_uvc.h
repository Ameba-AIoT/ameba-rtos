/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UVC_H
#define USBH_UVC_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usbh_uvc_dec.h"
#include "usbh_uvc_desc.h"
#include "usbh_uvc_intf.h"

/* Exported defines ----------------------------------------------------------*/

#define USBH_UVC_MAX_NUM_IN_TERMINAL			5
#define USBH_UVC_MAX_NUM_OUT_TERMINAL			2
#define USBH_UVC_MAX_NUM_CAMERA_TERMINAL		2
#define USBH_UVC_MAX_NUM_PROCESSING_UNIT		2
#define USBH_UVC_MAX_NUM_EXTENSION_UNIT			2
#define USBH_UVC_MAX_NUM_ENCODING_UNIT			2
#define USBH_UVC_MAX_NUM_SELECTOR_UNIT			2

#define USBH_MAX_NUM_MJPEG_FORMAT		30
#define USBH_MAX_NUM_UNCOMP_FRAME		30

#define USBH_MAX_NUM_VS_DESC			2
#define USBH_MAX_NUM_VS_ALTS			30

#define HFNUM_MAX_FRNUM					0x3fff

/* Exported types ------------------------------------------------------------*/

typedef struct {
	u32 dwMinBitRate;
	u32 dwMaxBitRate;
	u32 dwMaxVideoFrameBufferSize;   //for mjpeg & uncompressed payload type descriptor
	u32 dwDefaultFrameInterval;
	u32 *dwFrameInterval;
	u32 dwBytesPerLine;   //for frame based payload frame type descriptor
	u16 wWidth;
	u16 wHeight;
	u8  bFrameIndex;
	u8  bmCapabilities;
	u8  bFrameIntervalType;
} __PACKED usbh_uvc_vs_frame_t;

typedef struct {
	char name[32];
	usbh_uvc_vs_frame_t *frame;
	u32 nframes;
	u32 fcc;
	u32 flags;
	u8 type;
	u8 index;
	u8 bpp;
	u8 colorspace;
} usbh_uvc_vs_format_t;

typedef struct {
	struct list_head stream_list;
	struct list_head format_list;
	u32 intf_num;
	u32 format_num;
	u32 type;
} usbh_uvc_vs_stream_t;

typedef struct {
	struct list_head entity_chain;
	usbh_uvc_vc_header_desc_t *vcheader;
	void *p;
	u8 *intr_ep;
	u8 *cs_intr_desc;
	u8 bInterfaceNumber;
	u8 entity_num;
} usbh_uvc_vc_t;

typedef struct {
	void *p;
	usbh_ep_desc_t *endpoint;
} usbh_uvc_alt_t;

typedef struct {
	usbh_uvc_alt_t altsetting[USBH_MAX_NUM_VS_ALTS];
	usbh_uvc_vs_input_header_desc_t *InputHeader;
	usbh_uvc_vs_format_t *format;
	u32 nformat;
	void *p;
	u8 bInterfaceNumber;
	u8 alt_num;
	u8 has_h264;
} usbh_uvc_vs_t;

typedef struct {
	usbh_uvc_vs_t vs_intf[USBH_MAX_NUM_VS_DESC];
	usbh_uvc_vc_t vc_intf;
	u8 vs_num;
} usbh_uvc_cfg_t;

typedef struct {
	usbh_pipe_t pipe;
	usbh_uvc_alt_t *altsetting;
	usbh_uvc_vs_t *cur_vs_intf;
	u32 last_frame;
	u8 bAlternateSetting;
	u8 bInterfaceNumber;
	u8 valid;
	u8 is_processing;
} usbh_uvc_setting_t;

typedef struct {
	u32 offset;
	u32 length;
} usbh_uvc_packet_desc_t;

typedef struct {
	u8 *addr;
	u32 index;
	u32 packet_num;
	u32 packet_length;
	usbh_uvc_packet_desc_t packet_info[];
} usbh_uvc_urb_t;

typedef struct {
	usbh_uvc_setting_t cur_setting;
	usbh_uvc_frame_t frame_buffer[USBH_UVC_VIDEO_MAX_FRAME];
	usbh_uvc_stream_control_t stream_ctrl;
	struct list_head frame_empty;
	struct list_head frame_chain;
	usbh_uvc_vs_t *vs_intf;
	usbh_uvc_urb_t *urb[USBH_UVC_URB_NUMS];
	usbh_uvc_vs_format_t *def_format;
	usbh_uvc_vs_format_t *cur_format;
	usbh_uvc_vs_frame_t *cur_frame;
	usb_os_sema_t frame_sema;
	usb_os_queue_t urb_wait_queue;
	usb_os_queue_t urb_giveback_queue;
	usb_os_task_t decode_task;
	volatile u32 cur_packet_state;
	volatile u32 complete_flag;
	volatile u32 complete_on;
	u32 frame_buffer_size;
	u32 frame_cnt;
	u32 err_frame_cnt;
	u32 drop_cnt;
	u32 urb_buffer_size;
	u32 cur_urb;
	u32 cur_packet;
	usbh_uvc_frame_t *cur_frame_buf;
	u8 *urb_buffer;
	u8 *frame_buf;

#if USBH_UVC_USE_HW
	usbh_uvc_dec *uvc_dec;
#endif

	u8 stream_state;
	u8 stream_data_state;
	u8 stream_num;
	u8 last_fid;
} usbh_uvc_stream_t;

typedef struct {
	usbh_uvc_stream_t stream[USBH_MAX_NUM_VS_DESC];
	usbh_uvc_cfg_t uvc_desc;
	struct list_head entity_list;
	struct list_head video_chain;
	usb_host_t *host;
	usbh_uvc_cb_t *cb;
	u32 sof_cnt;
} usbh_uvc_host_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/*class module*/
int usbh_uvc_class_init(void);
void usbh_uvc_class_deinit(void);

/*stream module*/
int usbh_uvc_process_rx(usbh_uvc_stream_t *stream);
usbh_uvc_urb_t *usbh_uvc_urb_complete(usbh_uvc_stream_t *stream, usbh_uvc_urb_t *urb);

int usbh_uvc_set_video(usbh_uvc_stream_t *stream, int probe);
int usbh_uvc_get_video(usbh_uvc_stream_t *stream, int probe, u16 request);
int usbh_uvc_probe_video(usbh_uvc_stream_t *stream);
int usbh_uvc_commit_video(usbh_uvc_stream_t *stream);

int usbh_uvc_video_init(usbh_uvc_stream_t *stream);
int usbh_uvc_stream_init(usbh_uvc_stream_t *stream);
void usbh_uvc_stream_deinit(usbh_uvc_stream_t *stream);
void usbh_uvc_process_sof(usb_host_t *host);

int usbh_uvc_parse_cfgdesc(usb_host_t *host);
void usbh_uvc_desc_init(void);
void usbh_uvc_desc_deinit(void);

#endif
