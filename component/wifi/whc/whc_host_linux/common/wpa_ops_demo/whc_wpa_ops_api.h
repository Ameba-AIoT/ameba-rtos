// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between host linux and firmware in dpluc. The
*   commnunication between driver and firmware is SPI or SDIO.
*
* Copyright (C) 2025, Realtek Corporation. All rights reserved.
*/

#ifndef _WHC_WPA_OPS_API_H_
#define _WHC_WPA_OPS_API_H_


#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    (!FALSE)
#endif


// For Utility
#define WHC_WPA_OPS_UTIL 0xffa5a5a5

#define WHC_WPA_OPS_UTIL_GET_MAC_ADDR 0x1
#define WHC_WPA_OPS_UTIL_SET_NETWORK 0x2
#define WHC_WPA_OPS_UTIL_LIST_NETWORK 0x3
#define WHC_WPA_OPS_UTIL_SELECT_NETWORK 0x4
#define WHC_WPA_OPS_UTIL_GET_STATUS 0x5

// For Custom API
#define WHC_WPA_OPS_CUSTOM_API 0xff112255

#define WHC_WPA_OPS_CUSTOM_API_SCAN          0x1
#define WHC_WPA_OPS_CUSTOM_API_INIT_WPAS_STD 0x2
#define WHC_WPA_OPS_CUSTOM_API_WIFION        0x3


// For CB
#define WHC_WPA_OPS_SOC_CB 0xff112233

#define WHC_WPA_OPS_SOC_CB_SCAN_RESULT   0x1


// For Event
#define WHC_WPA_OPS_EVENT 0xff000000

#define WHC_WPA_OPS_EVENT_SCANING       0x1
#define WHC_WPA_OPS_EVENT_SCAN_COMPLETE   0x2



#endif
