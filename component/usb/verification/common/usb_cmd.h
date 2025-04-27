/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_CMD_H
#define USB_CMD_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#ifndef CONFIG_SUPPORT_USB_FS_ONLY

int cmd_usb_phydw(u16 argc, u8  *argv[]);

int cmd_usb_phyew(u16 argc, u8  *argv[]);

#endif

int cmd_usb_loopback_test(u16 argc, u8 *argv[]);

#endif /* USB_CMD_H */

