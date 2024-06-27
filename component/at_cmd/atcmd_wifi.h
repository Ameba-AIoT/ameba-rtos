/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_WIFI_H__
#define __ATCMD_WIFI_H__
#include "main.h"
#include "rtw_wifi_constants.h"
#include "FreeRTOS.h"

#ifndef CONFIG_MP_SHRINK
void print_wifi_at(void);
void at_wifi_init(void);
#endif

#endif
