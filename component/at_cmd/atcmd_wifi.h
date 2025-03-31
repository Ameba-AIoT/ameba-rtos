/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_WIFI_H__
#define __ATCMD_WIFI_H__
#include "main.h"
#include "wifi_api_types.h"
#include "FreeRTOS.h"

#ifndef CONFIG_MP_SHRINK
void print_wifi_at(void);
void at_wifi_init(void);
#endif

#endif
