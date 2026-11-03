/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EXAMPLE_USBD_UAC_H
#define EXAMPLE_USBD_UAC_H

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>

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
extern const usb_app_cmd_t example_usbd_uac_cmds[];
#endif

/* Exported functions --------------------------------------------------------*/

void example_usbd_uac(void);
int example_usbd_uac_start(void);
int example_usbd_uac_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* EXAMPLE_USBD_UAC_H */
