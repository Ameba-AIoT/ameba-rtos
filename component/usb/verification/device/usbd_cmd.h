/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_CMD_H
#define USBD_CMD_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int cmd_usbd_verify_test_entry(u16 argc, u8  *argv[]);

int cmd_usbd_cdc_acm(u16 argc, u8 *argv[]);

#endif /* USBD_CMD_H */

