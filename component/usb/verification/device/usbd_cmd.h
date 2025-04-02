/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_CMD_H
#define USBH_CMD_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#ifndef CONFIG_USB_FS

int cmd_usbd_cts_phydw(u16 argc, u8  *argv[]);

int cmd_usbd_cts_phyew(u16 argc, u8  *argv[]);

#endif

int cmd_usbd_verify_test_entry(u16 argc, u8  *argv[]);

int cmd_usbd_cdc_acm(u16 argc, u8 *argv[]);

int cmd_usbd_bus_cmd(u16 argc, u8 *argv[]);

#endif /* USBH_CMD_H */

