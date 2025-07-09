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


void whc_fullmac_host_event_auth_req(u8 *buf, s32 buf_len)
{
	union rtw_event_join_status_info *evt_info = (union rtw_event_join_status_info *)buf;

	if (evt_info != NULL) {
		dev_dbg(global_idev.fullmac_dev, "Auth req: ch %d, bssid %02x:%02x:%02x:%02x:%02x:%02x\n", evt_info->authenticating.channel, evt_info->authenticating.bssid[0], evt_info->authenticating.bssid[1], evt_info->authenticating.bssid[2], evt_info->authenticating.bssid[3], evt_info->authenticating.bssid[4], evt_info->authenticating.bssid[5]);
	}
}

void whc_fullmac_host_event_auth_resp(u8 *buf, s32 buf_len)
{
	union rtw_event_join_status_info *evt_info = (union rtw_event_join_status_info *)buf;

	if (evt_info != NULL) {
		dev_dbg(global_idev.fullmac_dev, "Auth resp: status %d\n", evt_info->authenticated.reason_or_status_code);
	}
}

void whc_fullmac_host_event_assoc_req(u8 *buf, s32 buf_len)
{
	union rtw_event_join_status_info *evt_info = (union rtw_event_join_status_info *)buf;

	if (evt_info != NULL) {
		dev_dbg(global_idev.fullmac_dev, "Assoc req: ch %d, bssid %02x:%02x:%02x:%02x:%02x:%02x, rssi %d\n", evt_info->associating.channel, evt_info->associating.bssid[0], evt_info->associating.bssid[1], evt_info->associating.bssid[2], evt_info->associating.bssid[3], evt_info->associating.bssid[4], evt_info->associating.bssid[5], evt_info->associating.rssi);
	}
}

void whc_fullmac_host_event_assoc_resp(u8 *buf, s32 buf_len)
{
	union rtw_event_join_status_info *evt_info = (union rtw_event_join_status_info *)buf;

	if (evt_info != NULL) {
		dev_dbg(global_idev.fullmac_dev, "Assoc resp: status %d, bssid %02x:%02x:%02x:%02x:%02x:%02x\n", evt_info->associated.reason_or_status_code, evt_info->associated.bssid[0], evt_info->associated.bssid[1], evt_info->associated.bssid[2], evt_info->associated.bssid[3], evt_info->associated.bssid[4], evt_info->associated.bssid[5]);
	}
}

void whc_fullmac_host_event_join_fail(u8 *buf, s32 buf_len)
{
	union rtw_event_join_status_info *evt_info = (union rtw_event_join_status_info *)buf;

	if (evt_info != NULL) {
		dev_dbg(global_idev.fullmac_dev, "Join fail: reason 0x%x, status %d, bssid %02x:%02x:%02x:%02x:%02x:%02x\n", evt_info->fail.fail_reason, evt_info->fail.reason_or_status_code, evt_info->fail.bssid[0], evt_info->fail.bssid[1], evt_info->fail.bssid[2], evt_info->fail.bssid[3], evt_info->fail.bssid[4], evt_info->fail.bssid[5]);
	}
}

/* Join status event contains all events related to join, unified into one handle func for processing */
void whc_fullmac_host_join_status_hdl(u8 *buf, s32 buf_len, s32 flags)
{
	switch (flags) {
	case RTW_JOINSTATUS_AUTHENTICATING:
		whc_fullmac_host_event_auth_req(buf, buf_len);
		break;
	case RTW_JOINSTATUS_AUTHENTICATED:
		whc_fullmac_host_event_auth_resp(buf, buf_len);
		break;
	case RTW_JOINSTATUS_ASSOCIATING:
		whc_fullmac_host_event_assoc_req(buf, buf_len);
		break;
	case RTW_JOINSTATUS_ASSOCIATED:
		whc_fullmac_host_event_assoc_resp(buf, buf_len);
		break;
	case RTW_JOINSTATUS_FAIL:
		whc_fullmac_host_event_join_fail(buf, buf_len);
		break;
	default:
		break;
	}
}

/**
 * @brief external event handle
* Customs can add their own handle functions here
 */
struct rtw_event_hdl_func_t whc_fullmac_host_event_internal_hdl[1] = {
	{RTW_EVENT_JOIN_STATUS,			whc_fullmac_host_join_status_hdl},
};
u16 array_len_of_event_external_hdl = sizeof(whc_fullmac_host_event_internal_hdl) / sizeof(struct rtw_event_hdl_func_t);

