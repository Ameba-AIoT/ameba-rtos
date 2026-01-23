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
#define USBH_UVC_USE_HW						1	/* use uvc hw decoder */
#else
#define USBH_UVC_USE_HW						0
#endif
#define USBH_UVC_GET_FRAME_TIMEOUT				2000   /* unit:ms */

/*Supported type*/
#define USBH_UVC_FORMAT_MJPEG				0x6
#define USBH_UVC_FORMAT_YUV				0x4
#define USBH_UVC_FORMAT_H264				0x10

#define USBH_UVC_URB_NUMS				4
#define USBH_UVC_URB_SIZE				(3072 + 32) * 2   /* bytes */

#define USBH_UVC_VIDEO_MAX_FRAME			3  /* if using UVC HW decoder, frame buffer number should fix to 3 */
#define USBH_UVC_VIDEO_FRAME_SIZE			150*1024    /* bytes */

#define USBH_UVC_DECODE_TASK_STACK			512*4    /* bytes */
#define USBH_UVC_DECODE_TASK_PRIORITY		5

#define USBH_UVC_USE_SOF					0  /* if set to 0, sof interrupt can be disabled */

#define UVC_IRQ_PRIORITY					INT_PRI_LOWEST

/* Exported types ------------------------------------------------------------*/

enum streaming_state {
	STREAMING_OFF = 0,
	STREAMING_ON = 1,
};

typedef enum  {
	UVC_FRAME_INIT = 0,
	UVC_FRAME_FLYING,  // memcpying
	UVC_FRAME_READY,   // ready to commit frame_chain
	UVC_FRAME_INUSE    // in using
} usbh_uvc_frame_state_t;

typedef struct  {
	int fmt_type;    		 //video format type
	int width;               //video frame width
	int height;              //video frame height
	int frame_rate;          //video frame rate
} uvc_config_t;

typedef struct {
	int(* init)(void);
	int(* deinit)(void);
	int(* attach)(void);
	int(* detach)(void);
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
int usbh_uvc_init(usbh_uvc_cb_t *cb);

void usbh_uvc_deinit(void);

int usbh_uvc_stream_on(u32 if_num);

int usbh_uvc_stream_off(u32 if_num);

int usbh_uvc_stream_state(u32 if_num);

int usbh_uvc_set_param(uvc_config_t *para, u32 if_num);

usbh_uvc_frame_t *usbh_uvc_get_frame(u32 if_num);

void usbh_uvc_put_frame(usbh_uvc_frame_t *frame, u32 if_num);

#endif
