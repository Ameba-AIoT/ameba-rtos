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
	usbh_hw_uvc_dec_buf uvc_dec_buf[USBH_HW_UAC_MAX_BUF_NUM];
	u32 ch;
	u32 frame_done_num;
	u32 frame_done_size;
	u32 free_buf_num;
	u32 ep_num;
	u32 pipe_num;
	u32 binterval;
	u32 dev_addr;
	void *priv;

	/* cmd reg related */
	u32 xfer_len;
	u32 ep_mps;
	rtos_sema_t dec_sema;
} usbh_hw_uvc_dec;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

usbh_hw_uvc_dec *usbh_hw_uvc_dec_alloc_channel(void);
void usbh_hw_uvc_dec_free_channel(usbh_hw_uvc_dec *uvc_dec);
void usbh_hw_uvc_dec_init(usbh_hw_uvc_dec *uvc_dec, u8 irq_pri);
void usbh_hw_uvc_dec_deinit(usbh_hw_uvc_dec *uvc_dec);
void usbh_hw_uvc_dec_start(usbh_hw_uvc_dec *uvc_dec);
void usbh_hw_uvc_dec_stop(usbh_hw_uvc_dec *uvc_dec);
void usbh_hw_uvc_dec_dump_reg(void);
#endif
#endif

