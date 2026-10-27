/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UVC_PARSE_H
#define USBH_UVC_PARSE_H

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Host_Constants USB Host Constants
 *  @{
 */
/** @defgroup Host_UVC_Parse_Constants Host UVC Parse Constants
 *  @brief Unit conversion constants of the UVC descriptor parser.
 *  @{
 */

#define USBH_UVC_FRAME_INTERVAL_UNIT_NS              (10000000U) /**< UVC frame interval base unit: 100ns (10^7 * 100ns = 1s). */

/** @} End of Host_UVC_Parse_Constants group */
/** @} End of USB_Host_Constants group */

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 *  @{
 */
/** @defgroup Host_UVC_Parse_Functions Host UVC Parse Functions
 *  @brief UVC descriptor parsing and format lookup, internal to the UVC class.
 *  @{
 */

/**
 * @brief  Parse the configuration descriptor into the UVC descriptor tree.
 * @param  host: Host handle.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_uvc_parse_cfgdesc(usb_host_t *host);

/**
 * @brief  Find the format and frame index matching a requested resolution and format type.
 * @param  stream: UVC streaming interface to search.
 * @param  context: Requested configuration, carrying width, height and format type.
 * @param  format_idx: Output, receives the matching format index.
 * @param  frame_idx: Output, receives the matching frame index.
 * @return HAL_OK on success, a HAL error code when nothing matches.
 */
int usbh_uvc_desc_find_format_frame(usbh_uvc_stream_t *stream, usbh_uvc_s_ctx_t *context, u8 *format_idx, u8 *frame_idx);

/**
 * @brief  Find the frame interval matching a requested frame rate.
 * @param  stream: UVC streaming interface to search.
 * @param  context: Requested configuration, carrying the frame rate.
 * @param  interval: Output, receives the matching frame interval.
 * @param  format_idx: Output, receives the matching format index.
 * @param  frame_idx: Output, receives the matching frame index.
 * @return HAL_OK on success, a HAL error code when nothing matches.
 */
int usbh_uvc_desc_find_frame_rate(usbh_uvc_stream_t *stream, usbh_uvc_s_ctx_t *context, u32 *interval, u8 *format_idx, u8 *frame_idx);

/**
 * @brief  Allocate the UVC descriptor parser state.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_uvc_desc_init(void);

/**
 * @brief  Free the UVC descriptor parser state.
 * @return None
 */
void usbh_uvc_desc_deinit(void);

/**
 * @brief  Get the Probe/Commit control wLength for a UVC release number.
 * @note   Ref UVC 1.5 Table 4-75.
 * @param  bcdUVC: UVC device release number, such as 0x0100, 0x0110 or 0x0150.
 * @return 26 for UVC 1.0, 34 for UVC 1.1, 48 for UVC 1.5.
 */
u16 usbh_uvc_get_ctrl_len_by_version(u16 bcdUVC);

/** @} End of Host_UVC_Parse_Functions group */
/** @} End of USB_Host_Functions group */

/** @} End of USB_Host_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBH_UVC_PARSE_H */
