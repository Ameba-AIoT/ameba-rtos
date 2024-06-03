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
/* TODO */
#define ENABLE_SET_MAC_ADDRESS                 0

#ifndef CONFIG_MP_SHRINK
extern void print_wifi_at(void);
#endif

#endif
