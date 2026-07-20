/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UVC_PARSE_H
#define USBH_UVC_PARSE_H

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"

/* Exported defines ----------------------------------------------------------*/

#define USBH_UVC_FRAME_INTERVAL_UNIT_NS              (10000000U) /**< UVC frame interval base unit: 100ns (10^7 * 100ns = 1s). */

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int usbh_uvc_parse_cfgdesc(usb_host_t *host);
int usbh_uvc_desc_find_format_frame(usbh_uvc_stream_t *stream, usbh_uvc_s_ctx_t *context, u32 *format_idx, u32 *frame_idx);
int usbh_uvc_desc_find_frame_rate(usbh_uvc_stream_t *stream, usbh_uvc_s_ctx_t *context, u32 *interval, u32 *format_idx, u32 *frame_idx);
int usbh_uvc_desc_init(void);
void usbh_uvc_desc_deinit(void);
u16 usbh_uvc_get_ctrl_len_by_version(u16 bcdUVC);

#endif /* USBH_UVC_PARSE_H */
