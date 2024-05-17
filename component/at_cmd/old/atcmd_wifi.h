/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_WIFI_H__
#define __ATCMD_WIFI_H__
#include "main.h"
#include "rtw_wifi_constants.h"
#ifdef CONFIG_LWIP_LAYER
//#include "lwip_netconf.h"
#endif
#include "platform_autoconf.h"

#ifndef CONFIG_NEW_ATCMD
#define at_printf(fmt, args...) do{DiagPrintf(fmt, ##args);}while(0)
#define at_print_data(data, size) do{__rtl_memDump(data, size, NULL);}while(0)
#endif

#endif
