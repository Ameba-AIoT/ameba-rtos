/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EXAMPLE_USBH_COMPOSITE_HID_UAC_H
#define EXAMPLE_USBH_COMPOSITE_HID_UAC_H

/* Includes ------------------------------------------------------------------*/

#ifdef USB_APP_AGGREGATE
#include "usb_app.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

#ifdef USB_APP_AGGREGATE
extern const usb_app_cmd_t example_usbh_composite_hid_uac_cmds[];
#endif

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Run the USB host composite example driving a HID and a UAC function.
 * @return None
 */
void example_usbh_composite_hid_uac(void);
int example_usbh_composite_hid_uac_start(void);
int example_usbh_composite_hid_uac_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* EXAMPLE_USBH_COMPOSITE_HID_UAC_H */
