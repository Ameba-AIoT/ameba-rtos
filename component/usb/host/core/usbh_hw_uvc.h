/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_HW_UVC_H
#define USBH_HW_UVC_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"

/* Exported defines ----------------------------------------------------------*/

#if CONFIG_SUPPORT_USBH_UVC_HW_DEC
#define USBH_HW_UAC_MAX_BUF_NUM		3

/* Exported types ------------------------------------------------------------*/

typedef struct {
	u32 buf_start_addr;
	u32 buf_size;
	u32 is_used;
} usbh_hw_uvc_dec_buf;

typedef struct {
	usbh_hw_uvc_dec_buf buf[USBH_HW_UAC_MAX_BUF_NUM];
	void *priv;
	u32 ch;
	u32 frame_done_num;
	u32 frame_done_size;
	u32 free_buf_cnt;

	/* cmd reg related */
	u32 xfer_len;
	u32 ep_mps;
	rtos_sema_t dec_sema;

	u8 ep_num;
	u8 pipe_num;
	u8 interval;
	u8 dev_addr;
} usbh_hw_uvc_dec;

typedef struct {
	usbh_pipe_t *pipe;
	u8 dev_addr;
	u8 isr_priority;
} usbh_hw_uvc_dec_ctx;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

void usbh_hw_uvc_init(usbh_hw_uvc_dec *uvc_dec, const usbh_hw_uvc_dec_ctx *ctx);
void usbh_hw_uvc_deinit(usbh_hw_uvc_dec *uvc_dec);
usbh_hw_uvc_dec *usbh_hw_uvc_alloc_channel(void);
void usbh_hw_uvc_free_channel(usbh_hw_uvc_dec *uvc_dec);

#endif
#endif