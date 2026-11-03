/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_APP_MGR_H
#define USB_APP_MGR_H

/* Includes ------------------------------------------------------------------*/

#include "usb_app.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/

/* Default wait between stopping one app and starting the next in a switch, long enough
   for the peer to see the disconnect before it is offered the next function */
#define USB_APP_SWITCH_DELAY_MS             500U

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern const usb_app_t usb_app_table[];
extern const u32 usb_app_table_num;

/* Exported functions --------------------------------------------------------*/

int usb_app_mgr_init(void);
void usb_app_mgr_dispatch(const char *name, u16 argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* USB_APP_MGR_H */
