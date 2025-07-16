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
	struct rtw_event_join_status_info *evt_info = (struct rtw_event_join_status_info *)buf;

	if (evt_info != NULL) {
		dev_dbg(global_idev.fullmac_dev, "Auth req: ch %d, bssid %02x:%02x:%02x:%02x:%02x:%02x, rssi %d\n", evt_info->channel, evt_info->bssid[0], evt_info->bssid[1], evt_info->bssid[2], evt_info->bssid[3], evt_info->bssid[4], evt_info->bssid[5], evt_info->rssi);
	}
}

void whc_fullmac_host_event_auth_resp(u8 *buf, s32 buf_len)
{
	struct rtw_event_join_status_info *evt_info = (struct rtw_event_join_status_info *)buf;

	if (evt_info != NULL) {
		dev_dbg(global_idev.fullmac_dev, "Auth resp: bssid %02x:%02x:%02x:%02x:%02x:%02x, status %d\n", evt_info->bssid[0], evt_info->bssid[1], evt_info->bssid[2], evt_info->bssid[3], evt_info->bssid[4], evt_info->bssid[5], evt_info->private.authenticated.reason_or_status_code);
	}
}

void whc_fullmac_host_event_assoc_req(u8 *buf, s32 buf_len)
{
	struct rtw_event_join_status_info *evt_info = (struct rtw_event_join_status_info *)buf;

	if (evt_info != NULL) {
		dev_dbg(global_idev.fullmac_dev, "Assoc req: is_reassoc %d, ch %d, bssid %02x:%02x:%02x:%02x:%02x:%02x, rssi %d\n", evt_info->private.associating.is_reassoc, evt_info->channel, evt_info->bssid[0], evt_info->bssid[1], evt_info->bssid[2], evt_info->bssid[3], evt_info->bssid[4], evt_info->bssid[5], evt_info->rssi);
	}
}

void whc_fullmac_host_event_assoc_resp(u8 *buf, s32 buf_len)
{
	struct rtw_event_join_status_info *evt_info = (struct rtw_event_join_status_info *)buf;

	if (evt_info != NULL) {
		dev_dbg(global_idev.fullmac_dev, "Assoc resp: is_reassoc %d, bssid %02x:%02x:%02x:%02x:%02x:%02x, status %d\n", evt_info->private.associated.is_reassoc, evt_info->bssid[0], evt_info->bssid[1], evt_info->bssid[2], evt_info->bssid[3], evt_info->bssid[4], evt_info->bssid[5], evt_info->private.associated.reason_or_status_code);
	}
}

void whc_fullmac_host_event_join_fail(u8 *buf, s32 buf_len)
{
	struct rtw_event_join_status_info *evt_info = (struct rtw_event_join_status_info *)buf;

	if (evt_info != NULL) {
		dev_dbg(global_idev.fullmac_dev, "Join fail: ch %d, bssid %02x:%02x:%02x:%02x:%02x:%02x, rssi %d, reason %d, status %d\n", evt_info->channel, evt_info->bssid[0], evt_info->bssid[1], evt_info->bssid[2], evt_info->bssid[3], evt_info->bssid[4], evt_info->bssid[5], evt_info->rssi, evt_info->private.fail.fail_reason, evt_info->private.fail.reason_or_status_code);
	}
}

void whc_fullmac_host_event_disconnect_hdl(u8 *buf, s32 buf_len)
{
	struct rtw_event_join_status_info *evt_info = (struct rtw_event_join_status_info *)buf;

	if (evt_info != NULL) {
		dev_dbg(global_idev.fullmac_dev, "Disconnect: ch %d, bssid %02x:%02x:%02x:%02x:%02x:%02x, rssi %d, reason %d\n", evt_info->channel, evt_info->bssid[0], evt_info->bssid[1], evt_info->bssid[2], evt_info->bssid[3], evt_info->bssid[4], evt_info->bssid[5], evt_info->rssi, evt_info->private.disconnect.disconn_reason);
	}
}

/* Join status event contains all events related to join, unified into one handle func for processing */
void whc_fullmac_host_join_status_hdl(u8 *buf, s32 buf_len)
{
	struct rtw_event_join_status_info *evt_info = (struct rtw_event_join_status_info *)buf;

	if (evt_info == NULL) {
		dev_err(global_idev.fullmac_dev, "Event join status err!\n");
		return;
	}

	switch (evt_info->status) {
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
	case RTW_JOINSTATUS_DISCONNECT:
		whc_fullmac_host_event_disconnect_hdl(buf, buf_len);
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

