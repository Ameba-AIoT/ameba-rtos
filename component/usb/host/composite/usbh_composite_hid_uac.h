/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_COMPOSITE_HID_UAC_H
#define USBH_COMPOSITE_HID_UAC_H

/* Includes ------------------------------------------------------------------*/
#include "usbh.h"
#include "usb_ch9.h"
#include "os_wrapper.h"
#if defined(CONFIG_USBH_COMPOSITE_HID_UAC1)
#include "usbh_composite_uac1.h"
#endif
#include "usbh_composite_hid.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */
/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_Composite_HID_UAC_Functions Host Composite HID UAC Functions
 * @{
 */
/**
 * @brief  Init composite class
 * @param[in] hid_cb: HID user callback
 * @param[in] uac_cb: UAC user callback
 * @param[in] frame_cnt: Configuration for ring buffer size (frame count).
 * @return 0 on success, non-zero on failure.
 */
int usbh_composite_init(usbh_composite_hid_usr_cb_t *hid_cb, usbh_composite_uac_usr_cb_t *uac_cb, int frame_cnt);
/**
 * @brief  DeInit composite class
 * @return None
 */
int usbh_composite_deinit(void);
/** @} End of Host_Composite_HID_UAC_Functions group */
/** @} End of USB_Host_Functions group */
/** @} End of USB_Host_API group */

#endif  /* USBH_COMPOSITE_HID_UAC_H */
