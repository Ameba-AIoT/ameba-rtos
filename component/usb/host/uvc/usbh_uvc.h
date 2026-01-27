/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UVC_H
#define USBH_UVC_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usbh_uvc_desc.h"
#include "usbh_uvc_intf.h"

/* Exported defines ----------------------------------------------------------*/

#define USBH_UVC_IN_TERMINAL_MAX_NUM            5
#define USBH_UVC_OUT_TERMINAL_MAX_NUM           2
#define USBH_UVC_CAMERA_TERMINAL_MAX_NUM        2
#define USBH_UVC_PROCESSING_UNIT_MAX_NUM        2
#define USBH_UVC_EXTENSION_UNIT_MAX_NUM         2
#define USBH_UVC_ENCODING_UNIT_MAX_NUM          2
#define USBH_UVC_SELECTOR_UNIT_MAX_NUM          2

#define USBH_UVC_VS_DESC_MAX_NUM                2
#define USBH_UVC_VS_ALTS_MAX_NUM                30

/* Exported types ------------------------------------------------------------*/
typedef enum {
	UVC_STATE_IDLE = 0,
	UVC_STATE_CTRL,
	UVC_STATE_TRANSFER,
	UVC_STATE_ERROR
} usbh_uvc_state_t;

typedef enum {
	STREAM_STATE_CTRL_IDLE = 0,
	STREAM_STATE_SET_PARA,        // Set Param: Probe - Commit - Set alt
	STREAM_STATE_PROBE_NEGOTIATE, // Set Video (Probe1)
	STREAM_STATE_PROBE_UPDATE,    // Get Video (Probe2)
	STREAM_STATE_PROBE_FINAL,     // Set Video (Probe3)
	STREAM_STATE_COMMIT,          // Set Video (Commit)
	STREAM_STATE_FIND_ALT,        // Find Interface/Altsetting for transfer
	STREAM_STATE_SET_ALT,         // Set Interface/Altsetting for transfer
	STREAM_STATE_SET_CTRL,        // Set Interface/0 for ctrl
	STREAM_STATE_ERROR
} usbh_stream_state_t;

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
	usbh_uvc_alt_t altsetting[USBH_UVC_VS_ALTS_MAX_NUM];
	usbh_uvc_vs_input_header_desc_t *InputHeader;
	usbh_uvc_vs_format_t *format;
	u32 nformat;
	void *p;
	u8 bInterfaceNumber;
	u8 alt_num;
	u8 has_h264;
} usbh_uvc_vs_t;

typedef struct {
	usbh_uvc_vs_t vs_intf[USBH_UVC_VS_DESC_MAX_NUM];
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
	usbh_uvc_frame_t frame_buffer[USBH_UVC_VIDEO_FRAME_NUMS];
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
	volatile usbh_stream_state_t state;
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
	u8 *request_buf;

#if USBH_UVC_USE_HW
	usbh_hw_uvc_dec *uvc_dec;
	u8 isr_priority;
#endif

	volatile u8 set_alt;
	u8 stream_state;
	u8 stream_data_state;
	u8 stream_idx;
	u8 last_fid;
} usbh_uvc_stream_t;

typedef struct {
	usbh_uvc_stream_t stream[USBH_UVC_VS_DESC_MAX_NUM];
	usbh_uvc_cfg_t uvc_desc;
	struct list_head entity_list;
	struct list_head video_chain;
	usbh_setup_req_t setup_req;
	usb_host_t *host;
	usbh_uvc_cb_t *cb;
	volatile usbh_uvc_state_t state;
	u32 sof_cnt;
	volatile u8 stream_in_ctrl;// record stream idx for ctrl process
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

int usbh_uvc_video_init(usbh_uvc_stream_t *stream);
int usbh_uvc_stream_init(usbh_uvc_stream_t *stream);
void usbh_uvc_stream_deinit(usbh_uvc_stream_t *stream);
void usbh_uvc_process_sof(usb_host_t *host);

int usbh_uvc_parse_cfgdesc(usb_host_t *host);
void usbh_uvc_desc_init(void);
void usbh_uvc_desc_deinit(void);

#endif
