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
int usbh_composite_init(usbh_composite_hid_usr_cb_t *hid_cb, usbh_composite_uac_usr_cb_t *uac_cb, int frame_cnt);
int usbh_composite_deinit(void);

#endif  /* USBH_COMPOSITE_HID_UAC_H */

