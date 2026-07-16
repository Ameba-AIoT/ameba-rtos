/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UVC_STREAM_H
#define USBH_UVC_STREAM_H

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int usbh_uvc_stream_ctrl_set_video(usbh_uvc_stream_t *stream, u8 probe);
int usbh_uvc_stream_ctrl_get_video(usbh_uvc_stream_t *stream, u8 probe, u16 request);
int usbh_uvc_stream_ctrl_apply(usbh_uvc_stream_t *stream);
int usbh_uvc_stream_open(usbh_uvc_stream_t *stream);
void usbh_uvc_stream_close(usbh_uvc_stream_t *stream);
int usbh_uvc_stream_start(usbh_uvc_stream_t *stream);
int usbh_uvc_stream_stop(usbh_uvc_stream_t *stream);
void usbh_uvc_stream_flush(usbh_uvc_stream_t *stream);

#if (USBH_UVC_USE_HW == 0)
void usbh_uvc_stream_free_urb_buffer(usbh_uvc_stream_t *stream);
int usbh_uvc_stream_alloc_urb_buffer(usbh_uvc_stream_t *stream);
void usbh_uvc_stream_process_sof(usb_host_t *host);
int usbh_uvc_stream_process_completed(usb_host_t *host, u8 pipe_num);
#if USBH_UVC_DEBUG
void usbh_uvc_sw_status_dump_thread(void *param);
#endif
#endif

#if USBH_UVC_USE_HW && USBH_UVC_DEBUG
void usbh_uvc_hw_status_dump_thread(void *param);
#endif

#endif /* USBH_UVC_STREAM_H */
