/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UVC_INTF_H
#define USBH_UVC_INTF_H

/* Includes ------------------------------------------------------------------*/

#include "dlist.h"

/* Exported defines ----------------------------------------------------------*/
#ifdef CONFIG_SUPPORT_USBH_UVC_HW_DEC
#define USBH_UVC_USE_HW                               1 /* use uvc hw decoder */
#else
#define USBH_UVC_USE_HW                               0
#endif

#if USBH_UVC_USE_HW
#include "usbh_hw_uvc.h"
#endif

#define USBH_UVC_GET_FRAME_TIMEOUT                    2000   /* unit:ms */

/*Supported type*/
#define USBH_UVC_FORMAT_MJPEG                         0x6
#define USBH_UVC_FORMAT_YUV                           0x4
#define USBH_UVC_FORMAT_H264                          0x10

#define USBH_UVC_URB_NUMS                             4
#define USBH_UVC_URB_SIZE                             ((3072 + 32) * 2)   /* bytes */

#define USBH_UVC_VIDEO_FRAME_NUMS                     3  /* if using UVC HW decoder, frame buffer number should fix to 3 */

#define USBH_UVC_DECODE_TASK_STACK                    (512 * 4)   /* bytes */
#define USBH_UVC_DECODE_TASK_PRIORITY                 5

#define USBH_UVC_USE_SOF                              0  /* if set to 0, sof interrupt can be disabled */

#define UBSH_UVC_REQUEST_BUF_LEN                      64

#if (USBH_UVC_USE_HW == 1)
#define USBH_HW_UVC_DUMP_ERR    0
#else
#define USBH_HW_UVC_DUMP_ERR    0
#endif

#if (USBH_UVC_USE_HW != 1) && (USBH_HW_UVC_DUMP_ERR == 1)
#error "USBH_HW_UVC_DUMP_ERR cannot be enabled when USBH_UVC_USE_HW is disabled"
#endif

/* Exported types ------------------------------------------------------------*/

typedef enum {
	STREAMING_OFF = 0,
	STREAMING_ON
} usbh_uvc_streaming_state_t;

typedef enum {
	STREAM_STATE_IDLE = 1,
	STREAM_DATA_IN,
} usbh_uvc_stream_data_state_t;

typedef enum  {
	UVC_FRAME_INIT = 0,
	UVC_FRAME_FLYING,  // memcpying
	UVC_FRAME_READY,   // ready to commit frame_chain
	UVC_FRAME_INUSE    // in using
} usbh_uvc_frame_state_t;

typedef struct  {
	int fmt_type;    //video format type
	int width;       //video frame width
	int height;      //video frame height
	int frame_rate;  //video frame rate
	u32 frame_buf_size;  //video frame size
} usbh_uvc_s_ctx_t;

typedef struct  {
#if USBH_UVC_USE_HW
	u8 hw_isr_pri; //uvc hw isr priorigy
#endif
} usbh_uvc_ctx_t;
typedef struct {
	int(* init)(void);
	int(* deinit)(void);
	int(* attach)(void);
	int(* detach)(void);
	int(* setup)(void);
	int(* setparam)(void);
} usbh_uvc_cb_t;

typedef struct {
	struct list_head list;
	u8 *buf;
	u32 index;
	u32 byteused;
	u32 err;
	u32 timestamp;
	usbh_uvc_frame_state_t state;
} usbh_uvc_frame_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
int usbh_uvc_init(usbh_uvc_ctx_t *cfg, usbh_uvc_cb_t *cb);
void usbh_uvc_deinit(void);
int usbh_uvc_stream_on(usbh_uvc_s_ctx_t *para, u32 itf_num);
int usbh_uvc_stream_off(u32 itf_num);
int usbh_uvc_stream_state(u32 itf_num);
int usbh_uvc_set_param(usbh_uvc_s_ctx_t *para, u32 itf_num);
usbh_uvc_frame_t *usbh_uvc_get_frame(u32 itf_num);
void usbh_uvc_put_frame(usbh_uvc_frame_t *frame, u32 itf_num);

#endif
