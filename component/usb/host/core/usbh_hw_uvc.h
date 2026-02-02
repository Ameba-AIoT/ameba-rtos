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
#define USBH_HW_UVC_MAX_BUF_NUM     3

#define USBH_HW_UVC_ISOC_MPS        1024

#define USBH_HW_UVC_FRAME_BUF_START_ADDR               0x20060000U

#define USBH_HW_UVC_CH0_BUF0_OVERSIZE   (1 << 0)
#define USBH_HW_UVC_CH0_BUF1_OVERSIZE   (1 << 1)
#define USBH_HW_UVC_CH0_BUF0_HEADER     (1 << 2)
#define USBH_HW_UVC_CH0_BUF1_HEADER     (1 << 3)
#define USBH_HW_UVC_CH1_BUF0_OVERSIZE   (1 << 4)
#define USBH_HW_UVC_CH1_BUF1_OVERSIZE   (1 << 5)
#define USBH_HW_UVC_CH1_BUF0_HEADER     (1 << 6)
#define USBH_HW_UVC_CH1_BUF1_HEADER     (1 << 7)
/* Exported types ------------------------------------------------------------*/

/**
 * @brief UVC Hardware Error Codes
 * Defined to distinguish error type and buffer location.
 */
typedef enum {
	USBH_HW_UVC_ERR_NONE = 0,
	USBH_HW_UVC_ERR_BUF0_OVERSIZE,  /* Frame data exceeded buffer size in BUF0 */
	USBH_HW_UVC_ERR_BUF0_HEADER,    /* Payload header parsing error in BUF0 */
	USBH_HW_UVC_ERR_BUF1_OVERSIZE,  /* Frame data exceeded buffer size in BUF1 */
	USBH_HW_UVC_ERR_BUF1_HEADER,    /* Payload header parsing error in BUF1 */
} usbh_hw_uvc_err_status_t;

typedef struct {
	u32 buf_start_addr;
	u32 buf_size;
	u32 is_used;
} usbh_hw_uvc_dec_buf;

typedef struct {
	usbh_hw_uvc_dec_buf buf[USBH_HW_UVC_MAX_BUF_NUM];
	void *priv;
	void (*err_cb)(usbh_hw_uvc_err_status_t err);
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
	__IO u8 is_active;
} usbh_hw_uvc_dec_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

void usbh_hw_uvc_init(usbh_hw_uvc_dec_t *uvc_dec, usbh_pipe_t *pipe);
void usbh_hw_uvc_deinit(usbh_hw_uvc_dec_t *uvc_dec);
usbh_hw_uvc_dec_t *usbh_hw_uvc_alloc_channel(void);
void usbh_hw_uvc_free_channel(usbh_hw_uvc_dec_t *uvc_dec);
void usbh_hw_uvc_start(usbh_hw_uvc_dec_t *uvc_dec);
void usbh_hw_uvc_stop(usbh_hw_uvc_dec_t *uvc_dec);
void usbh_hw_uvc_irq_en(u8 irq_pri);
void usbh_hw_uvc_irq_dis(void);
#endif
#endif