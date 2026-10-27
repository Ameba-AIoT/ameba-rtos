/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_HW_UVC_H
#define USBH_HW_UVC_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_SUPPORT_USBH_UVC_HW_DEC

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Host_Constants USB Host Constants
 *  @{
 */
/** @defgroup Host_HW_UVC_Constants Host HW UVC Constants
 *  @brief Buffer sizing, channel indices and error flags of the UVC hardware decoder.
 *  @{
 */

#define USBH_HW_UVC_MAX_BUF_NUM     3     /**< Number of frame buffers rotated per channel. */

#define USBH_HW_UVC_ISOC_MPS        1024  /**< Isochronous IN maximum packet size the decoder expects. */

#define USBH_HW_UVC_HCDMA_TAG_ADDR                     0x20060000U  /**< HCDMA placeholder the decoder matches against HCDMAi, never written: not a buffer. */

#define USBH_HW_UVC_CH0_BUF0_OVERSIZE   BIT0  /**< Channel 0 buffer 0 frame exceeded the buffer size. */
#define USBH_HW_UVC_CH0_BUF1_OVERSIZE   BIT1  /**< Channel 0 buffer 1 frame exceeded the buffer size. */
#define USBH_HW_UVC_CH0_BUF0_HEADER     BIT2  /**< Channel 0 buffer 0 payload header error. */
#define USBH_HW_UVC_CH0_BUF1_HEADER     BIT3  /**< Channel 0 buffer 1 payload header error. */
#define USBH_HW_UVC_CH1_BUF0_OVERSIZE   BIT4  /**< Channel 1 buffer 0 frame exceeded the buffer size. */
#define USBH_HW_UVC_CH1_BUF1_OVERSIZE   BIT5  /**< Channel 1 buffer 1 frame exceeded the buffer size. */
#define USBH_HW_UVC_CH1_BUF0_HEADER     BIT6  /**< Channel 1 buffer 0 payload header error. */
#define USBH_HW_UVC_CH1_BUF1_HEADER     BIT7  /**< Channel 1 buffer 1 payload header error. */

#define USBH_HW_UVC_CH_IDX_STREAM0             (0U)      /**< Channel index for stream 0. */
#define USBH_HW_UVC_CH_IDX_STREAM1             (1U)      /**< Channel index for stream 1. */
#define USBH_HW_UVC_BUF_INVALID                (0xEU)    /**< Invalid buf addr return value. */
#define USBH_HW_UVC_STOP_DELAY_MS              (1U)      /**< Delay before disabling concat after stop. */

/** @} End of Host_HW_UVC_Constants group */
/** @} End of USB_Host_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Host_Types USB Host Types
 *  @{
 */
/** @defgroup Host_HW_UVC_Types Host HW UVC Types
 *  @brief Frame buffer and decoder context types of the UVC hardware decoder.
 *  @{
 */

/** @brief One frame buffer slot of a hardware decoder channel. */
typedef struct {
	u32 buf_start_addr;   /**< Base address of this frame buffer. */
	u32 buf_size;         /**< Size of this frame buffer in bytes. */
} usbh_hw_uvc_dec_buf;

/** @brief Hardware UVC decoder channel context. */
typedef struct {
	void *priv;                /**< Opaque context of the owning class layer. */
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

	/* frame hand-off */
	usb_os_sema_t dec_sema;    /**< Frame-done signal: given by the decoder ISR, taken by usbh_uvc_get_frame(). */

	usbh_hw_uvc_dec_buf buf[USBH_HW_UVC_MAX_BUF_NUM];  /**< Frame buffer slots rotated by the decoder. */
	__IO u8  buf_locked[USBH_HW_UVC_MAX_BUF_NUM]; /**< 1: buffer held by the app between get_frame() and put_frame(); ISR must not recycle it. */
	u8 dev_addr;               /**< Address of the device being decoded. */
	u8 ch;                     /**< Hardware decoder channel index. */
	__IO u8 is_active;         /**< 1 while the channel is streaming; gates the ISR. */
} usbh_hw_uvc_dec_t;

/** @} End of Host_HW_UVC_Types group */
/** @} End of USB_Host_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 *  @{
 */
/** @defgroup Host_HW_UVC_Functions Host HW UVC Functions
 *  @brief Channel lifecycle and frame buffer operations of the UVC hardware decoder.
 *  @{
 */

/**
 * @brief  Initialize a UVC hardware decoder channel.
 * @param  uvc_dec: Decoder channel context.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_hw_uvc_init(usbh_hw_uvc_dec_t *uvc_dec);

/**
 * @brief  Prepare the decoder with the negotiated pipe.
 * @note   Idempotent.
 * @param  uvc_dec: Decoder channel context.
 * @param  pipe: Pipe handle; pipe_num and ep_mps must be valid, i.e. after usbh_open_pipe().
 * @return None
 */
void usbh_hw_uvc_prepare(usbh_hw_uvc_dec_t *uvc_dec, usbh_pipe_t *pipe);

/**
 * @brief  De-initialize a UVC hardware decoder channel.
 * @param  uvc_dec: Decoder channel context.
 * @return None
 */
void usbh_hw_uvc_deinit(usbh_hw_uvc_dec_t *uvc_dec);

/**
 * @brief  Allocate a free UVC hardware decoder channel.
 * @return The allocated decoder channel context, or NULL when none is free.
 */
usbh_hw_uvc_dec_t *usbh_hw_uvc_alloc_channel(void);

/**
 * @brief  Free a UVC hardware decoder channel and release its resources.
 * @param  uvc_dec: Decoder channel context.
 * @return None
 */
void usbh_hw_uvc_free_channel(usbh_hw_uvc_dec_t *uvc_dec);

/**
 * @brief  Start decoding on a channel.
 * @param  uvc_dec: Decoder channel context.
 * @return None
 */
void usbh_hw_uvc_start(usbh_hw_uvc_dec_t *uvc_dec);

/**
 * @brief  Stop decoding on a channel.
 * @param  uvc_dec: Decoder channel context.
 * @return None
 */
void usbh_hw_uvc_stop(usbh_hw_uvc_dec_t *uvc_dec);

/**
 * @brief  Release a buffer previously handed to the application by usbh_uvc_get_frame().
 * @note   Called by the class layer from usbh_uvc_put_frame(). Clears the per-buffer lock
 *         so the ISR may recycle it for hardware capture again.
 * @param  uvc_dec: Decoder channel context.
 * @param  buf_idx: Buffer index to release, 0 to USBH_HW_UVC_MAX_BUF_NUM-1.
 * @return None
 */
void usbh_hw_uvc_release_buf(usbh_hw_uvc_dec_t *uvc_dec, u32 buf_idx);

/**
 * @brief  Clear the hardware-side residual state between two streaming rounds.
 * @note   Called from usbh_uvc_stream_flush(). Discards any frame-done or error status
 *         latched during the previous round and resets the buffer rotation and
 *         last-completion hand-off state. is_active is 0 here, so the ISR is gated.
 * @param  uvc_dec: Decoder channel context.
 * @return None
 */
void usbh_hw_uvc_flush(usbh_hw_uvc_dec_t *uvc_dec);

/**
 * @brief  Register and enable the UVC hardware decoder interrupt.
 * @param  irq_pri: Interrupt priority level.
 * @return None
 */
void usbh_hw_uvc_irq_en(u8 irq_pri);

/**
 * @brief  Disable and unregister the UVC hardware decoder interrupt.
 * @return None
 */
void usbh_hw_uvc_irq_dis(void);

/** @} End of Host_HW_UVC_Functions group */
/** @} End of USB_Host_Functions group */

/** @} End of USB_Host_API group */

#endif /* CONFIG_SUPPORT_USBH_UVC_HW_DEC */

#ifdef __cplusplus
}
#endif

#endif /* USBH_HW_UVC_H */
