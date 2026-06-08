// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2025, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_DEV_RTW_CLI_CMD_DEFINE_H__
#define __WHC_DEV_RTW_CLI_CMD_DEFINE_H__

#define REPORT_EVENT_JOIN_STATUS_LEN    256

struct whc_rtw_cli_wifi_status {
	u8 		ssid[33];
	u8		bssid[6];
	u8		channel;
};

#endif //__WHC_DEV_RTW_CLI_CMD_DEFINE_H__
