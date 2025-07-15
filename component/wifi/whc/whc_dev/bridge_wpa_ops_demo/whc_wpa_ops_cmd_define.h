// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2025, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_WPA_OPS_CMD_DEFINE_H__
#define __WHC_WPA_OPS_CMD_DEFINE_H__

// For Utility
#define WHC_WPA_OPS_UTIL 0xffa5a5a5
#define WHC_WPA_OPS_UTIL_GET_MAC_ADDR 0x1
#define WHC_WPA_OPS_UTIL_SET_NETWORK  0x2
#define WHC_WPA_OPS_UTIL_LIST_NETWORK 0x3
#define WHC_WPA_OPS_UTIL_SELECT_NETWORK 0x4

// For Custom API
#define WHC_WPA_OPS_CUSTOM_API 0xff112255

#define WHC_WPA_OPS_CUSTOM_API_SCAN          0x1


// For CB
#define WHC_WPA_OPS_SOC_CB 0xff112233

#define WHC_WPA_OPS_SOC_CB_SCAN_RESULT   0x1


// For Event
#define WHC_WPA_OPS_EVENT 0xff000000

#define WHC_WPA_OPS_EVENT_SCANING       0x1
#define WHC_WPA_OPS_EVENT_SCAN_COMPLETE   0x2



#endif
