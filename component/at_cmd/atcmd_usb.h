/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ATCMD_USB_H
#define ATCMD_USB_H

/* Includes ------------------------------------------------------------------*/

#include "basic_types.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Print available USB AT commands.
 * @note   Called from help/command listing, matching print_wifi_at() pattern.
 */
void print_usb_at(void);

#endif /* ATCMD_USB_H */
