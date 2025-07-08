// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <whc_host_linux.h>


/* Customs shall define their own handle functions here */
void whc_fullmac_host_event_auth_req(u8 *buf, s32 buf_len)
{
	union rtw_event_info *evt_info = (union rtw_event_info *)buf;

	dev_dbg(global_idev.fullmac_dev, "Auth req: ch %d, bssid %02x:%02x:%02x:%02x:%02x:%02x\n", evt_info->join_status.channel, evt_info->join_status.bssid[0], evt_info->join_status.bssid[1], evt_info->join_status.bssid[2], evt_info->join_status.bssid[3], evt_info->join_status.bssid[4], evt_info->join_status.bssid[5]);
}

void whc_fullmac_host_event_auth_resp(u8 *buf, s32 buf_len)
{
	union rtw_event_info *evt_info = (union rtw_event_info *)buf;

	dev_dbg(global_idev.fullmac_dev, "Auth resp: status %d\n", evt_info->join_status.reason_or_status_code);
}

void whc_fullmac_host_event_assoc_req(u8 *buf, s32 buf_len)
{
	union rtw_event_info *evt_info = (union rtw_event_info *)buf;

	dev_dbg(global_idev.fullmac_dev, "Assoc req: ch %d, bssid %02x:%02x:%02x:%02x:%02x:%02x, rssi %d\n", evt_info->join_status.channel, evt_info->join_status.bssid[0], evt_info->join_status.bssid[1], evt_info->join_status.bssid[2], evt_info->join_status.bssid[3], evt_info->join_status.bssid[4], evt_info->join_status.bssid[5], evt_info->join_status.rssi);
}

void whc_fullmac_host_event_assoc_resp(u8 *buf, s32 buf_len)
{
	union rtw_event_info *evt_info = (union rtw_event_info *)buf;

	dev_dbg(global_idev.fullmac_dev, "Assoc resp: status %d, bssid %02x:%02x:%02x:%02x:%02x:%02x\n", evt_info->join_status.reason_or_status_code, evt_info->join_status.bssid[0], evt_info->join_status.bssid[1], evt_info->join_status.bssid[2], evt_info->join_status.bssid[3], evt_info->join_status.bssid[4], evt_info->join_status.bssid[5]);
}

