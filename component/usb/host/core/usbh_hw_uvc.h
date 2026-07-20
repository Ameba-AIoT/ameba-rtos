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

#define USBH_HW_UVC_CH0_BUF0_OVERSIZE   BIT0
#define USBH_HW_UVC_CH0_BUF1_OVERSIZE   BIT1
#define USBH_HW_UVC_CH0_BUF0_HEADER     BIT2
#define USBH_HW_UVC_CH0_BUF1_HEADER     BIT3
#define USBH_HW_UVC_CH1_BUF0_OVERSIZE   BIT4
#define USBH_HW_UVC_CH1_BUF1_OVERSIZE   BIT5
#define USBH_HW_UVC_CH1_BUF0_HEADER     BIT6
#define USBH_HW_UVC_CH1_BUF1_HEADER     BIT7

#define USBH_HW_UVC_CH_IDX_STREAM0             (0U)      /**< Channel index for stream 0. */
#define USBH_HW_UVC_CH_IDX_STREAM1             (1U)      /**< Channel index for stream 1. */
#define USBH_HW_UVC_CH_INVALID                 (0xFU)    /**< Invalid channel return value. */
#define USBH_HW_UVC_BUF_INVALID                (0xEU)    /**< Invalid buf addr return value. */
#define USBH_HW_UVC_STOP_DELAY_MS              (1U)      /**< Delay before disabling concat after stop. */
/* Exported types ------------------------------------------------------------*/

typedef struct {
	u32 buf_start_addr;
	u32 buf_size;
} usbh_hw_uvc_dec_buf;

typedef struct {
	usbh_hw_uvc_dec_buf buf[USBH_HW_UVC_MAX_BUF_NUM];
	void *priv;
	/**
	 * @brief Called when the HW UVC decoder reports an error condition.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] err: Bitmask of HW UVC error flags (USBH_HW_UVC_CHx_BUFx_OVERSIZE / _HEADER).
	 */
	void (*err_cb)(u32 err);

	__IO u32 frame_done_idx;   /**< Buffer index (0..MAX_BUF_NUM-1) of the last completed frame. */
	__IO u32 frame_done_size;  /**< Byte size of the last completed frame. */
	__IO u32 frame_rx_cnt;     /**< Total frames the HW decoder completed (produced). */
	__IO u32 frame_drop_cnt;   /**< Frames overwritten before usbh_uvc_get_frame() consumed them. */
	__IO u8  buf_locked[USBH_HW_UVC_MAX_BUF_NUM]; /**< 1: buffer held by the app between get_frame() and put_frame(); ISR must not recycle it. */
	u32 free_buf_cnt;

	/* cmd reg related */
	rtos_sema_t dec_sema;

	u8 dev_addr;
	u8 ch;
	__IO u8 is_active;
} usbh_hw_uvc_dec_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

void usbh_hw_uvc_init(usbh_hw_uvc_dec_t *uvc_dec);
void usbh_hw_uvc_prepare(usbh_hw_uvc_dec_t *uvc_dec, usbh_pipe_t *pipe);
void usbh_hw_uvc_deinit(usbh_hw_uvc_dec_t *uvc_dec);
usbh_hw_uvc_dec_t *usbh_hw_uvc_alloc_channel(void);
void usbh_hw_uvc_free_channel(usbh_hw_uvc_dec_t *uvc_dec);
void usbh_hw_uvc_start(usbh_hw_uvc_dec_t *uvc_dec);
void usbh_hw_uvc_stop(usbh_hw_uvc_dec_t *uvc_dec);
void usbh_hw_uvc_release_buf(usbh_hw_uvc_dec_t *uvc_dec, u32 buf_idx);
void usbh_hw_uvc_flush(usbh_hw_uvc_dec_t *uvc_dec);
void usbh_hw_uvc_irq_en(u8 irq_pri);
void usbh_hw_uvc_irq_dis(void);
#endif
#endif
