/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UVC_STREAM_H
#define USBH_UVC_STREAM_H

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 *  @{
 */
/** @defgroup Host_UVC_Stream_Functions Host UVC Stream Functions
 *  @brief Stream negotiation, lifecycle and data path of the UVC class, internal to it.
 *  @{
 */

/**
 * @brief  Send a SET_CUR request on the Probe or Commit control.
 * @param  stream: UVC streaming interface.
 * @param  probe: 1 to address the Probe control, 0 for the Commit control.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_uvc_stream_ctrl_set_video(usbh_uvc_stream_t *stream, u8 probe);

/**
 * @brief  Send a GET request, such as GET_CUR or GET_DEF, on the Probe or Commit control.
 * @param  stream: UVC streaming interface.
 * @param  probe: 1 to address the Probe control, 0 for the Commit control.
 * @param  request: Request code to issue.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_uvc_stream_ctrl_get_video(usbh_uvc_stream_t *stream, u8 probe, u16 request);

/**
 * @brief  Allocate and initialize all runtime resources of a UVC stream.
 * @param  stream: UVC streaming interface.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_uvc_stream_open(usbh_uvc_stream_t *stream);

/**
 * @brief  Release all runtime resources of a UVC stream.
 * @param  stream: UVC streaming interface.
 * @return None
 */
void usbh_uvc_stream_close(usbh_uvc_stream_t *stream);

/**
 * @brief  Start the data path of a UVC stream, moving it from READY to ACTIVE.
 * @param  stream: UVC streaming interface.
 * @return HAL_OK on success or if the stream is already active, a HAL error code otherwise.
 */
int usbh_uvc_stream_start(usbh_uvc_stream_t *stream);

/**
 * @brief  Stop the data path of a UVC stream, moving it from ACTIVE to READY.
 * @param  stream: UVC streaming interface.
 * @return HAL_OK on success or if the stream is not currently active, a HAL error code otherwise.
 */
int usbh_uvc_stream_stop(usbh_uvc_stream_t *stream);

/**
 * @brief  Synchronously flush all per-round runtime state of a UVC stream.
 * @param  stream: UVC streaming interface.
 * @return None
 */
void usbh_uvc_stream_flush(usbh_uvc_stream_t *stream);

#if (USBH_UVC_USE_HW == 0)

/**
 * @brief  Free the URB buffers, which are sized for the previous alternate setting MPS.
 * @param  stream: UVC streaming interface.
 * @return None
 */
void usbh_uvc_stream_free_urb_buffer(usbh_uvc_stream_t *stream);

/**
 * @brief  Allocate the URB buffers sized to the pipe's current transfer length.
 * @param  stream: UVC streaming interface.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_uvc_stream_alloc_urb_buffer(usbh_uvc_stream_t *stream);

/**
 * @brief  Process a Start Of Frame event and resubmit the isochronous IN URBs.
 * @note   ISR context: must not block or allocate.
 * @param  host: Host handle.
 * @return None
 */
void usbh_uvc_stream_process_sof(usb_host_t *host);

/**
 * @brief  Process a completed isochronous IN transfer and assemble the frame payload.
 * @note   ISR context: must not block or allocate.
 * @param  host: Host handle.
 * @param  pipe_num: Pipe number that completed.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_uvc_stream_process_completed(usb_host_t *host, u8 pipe_num);

#endif /* USBH_UVC_USE_HW == 0 */

#if USBH_UVC_DEBUG
#if (USBH_UVC_USE_HW == 0)

/**
 * @brief  Software UVC status dump thread entry.
 * @param  param: Thread argument, unused.
 * @return None
 */
void usbh_uvc_sw_status_dump_thread(void *param);

#else

/**
 * @brief  Hardware UVC status dump thread entry.
 * @param  param: Thread argument, unused.
 * @return None
 */
void usbh_uvc_hw_status_dump_thread(void *param);

#endif
#endif /* USBH_UVC_DEBUG */

/** @} End of Host_UVC_Stream_Functions group */
/** @} End of USB_Host_Functions group */

/** @} End of USB_Host_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBH_UVC_STREAM_H */
