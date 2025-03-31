/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_CMD_H
#define USBH_CMD_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int cmd_usbh_emc_test(void);

int cmd_usbh_cts_phydw(u16 argc, u8  *argv[]);

int cmd_usbh_cts_phyew(u16 argc, u8  *argv[]);

int cmd_usbh_tmod_test(u16 argc, u8  *argv[]);

int cmd_usbh_resume_test(void);
int cmd_usbh_suspend_test(void);

int cmd_usbh_xfer_test(u16 argc, u8  *argv[]);

int cmd_usbh_verify_test_entry(u16 argc, u8  *argv[]);

#endif /* USBH_CMD_H */

