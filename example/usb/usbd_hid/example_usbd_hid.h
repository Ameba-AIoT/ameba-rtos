/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EXAMPLE_USBD_HID_H
#define EXAMPLE_USBD_HID_H

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
extern const usb_app_cmd_t example_usbd_hid_cmds[];
#endif

/* Exported functions --------------------------------------------------------*/

void example_usbd_hid(void);
int example_usbd_hid_start(void);
int example_usbd_hid_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* EXAMPLE_USBD_HID_H */
