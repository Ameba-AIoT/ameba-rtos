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


void whc_host_event_auth_req(u8 *evt_info)
{
	struct rtw_event_join_status_info *join_status_info = (struct rtw_event_join_status_info *)evt_info;

	if (join_status_info != NULL) {
		dev_dbg(global_idev.pwhc_dev, "Auth req: ch %d, bssid %02x:%02x:%02x:%02x:%02x:%02x, rssi %d\n", join_status_info->channel, join_status_info->bssid[0],
				join_status_info->bssid[1],
				join_status_info->bssid[2], join_status_info->bssid[3], join_status_info->bssid[4], join_status_info->bssid[5], join_status_info->rssi);
	}
}

void whc_host_event_auth_resp(u8 *evt_info)
{
	struct rtw_event_join_status_info *join_status_info = (struct rtw_event_join_status_info *)evt_info;

	if (join_status_info != NULL) {
		dev_dbg(global_idev.pwhc_dev, "Auth resp: bssid %02x:%02x:%02x:%02x:%02x:%02x, status %d\n", join_status_info->bssid[0], join_status_info->bssid[1],
				join_status_info->bssid[2],
				join_status_info->bssid[3], join_status_info->bssid[4], join_status_info->bssid[5], join_status_info->priv.authenticated.reason_or_status_code);
	}
}

void whc_host_event_assoc_req(u8 *evt_info)
{
	struct rtw_event_join_status_info *join_status_info = (struct rtw_event_join_status_info *)evt_info;

	if (join_status_info != NULL) {
		dev_dbg(global_idev.pwhc_dev, "Assoc req: is_reassoc %d, ch %d, bssid %02x:%02x:%02x:%02x:%02x:%02x, rssi %d\n",
				join_status_info->priv.associating.is_reassoc,
				join_status_info->channel, join_status_info->bssid[0], join_status_info->bssid[1], join_status_info->bssid[2], join_status_info->bssid[3],
				join_status_info->bssid[4], join_status_info->bssid[5], join_status_info->rssi);
	}
}

void whc_host_event_assoc_resp(u8 *evt_info)
{
	struct rtw_event_join_status_info *join_status_info = (struct rtw_event_join_status_info *)evt_info;

	if (join_status_info != NULL) {
		dev_dbg(global_idev.pwhc_dev, "Assoc resp: is_reassoc %d, bssid %02x:%02x:%02x:%02x:%02x:%02x, status %d\n", join_status_info->priv.associated.is_reassoc,
				join_status_info->bssid[0], join_status_info->bssid[1], join_status_info->bssid[2], join_status_info->bssid[3], join_status_info->bssid[4],
				join_status_info->bssid[5],
				join_status_info->priv.associated.reason_or_status_code);
	}
}

void whc_host_event_join_fail(u8 *evt_info)
{
	struct rtw_event_join_status_info *join_status_info = (struct rtw_event_join_status_info *)evt_info;

	if (join_status_info != NULL) {
		dev_dbg(global_idev.pwhc_dev, "Join fail: ch %d, bssid %02x:%02x:%02x:%02x:%02x:%02x, rssi %d, reason %d, status %d\n", join_status_info->channel,
				join_status_info->bssid[0], join_status_info->bssid[1], join_status_info->bssid[2], join_status_info->bssid[3], join_status_info->bssid[4],
				join_status_info->bssid[5], join_status_info->rssi,
				join_status_info->priv.fail.fail_reason, join_status_info->priv.fail.reason_or_status_code);
	}
}

void whc_host_event_disconnect_hdl(u8 *evt_info)
{
	struct rtw_event_join_status_info *join_status_info = (struct rtw_event_join_status_info *)evt_info;

	if (join_status_info != NULL) {
		dev_dbg(global_idev.pwhc_dev, "Disconnect: ch %d, bssid %02x:%02x:%02x:%02x:%02x:%02x, rssi %d, reason %d\n", join_status_info->channel,
				join_status_info->bssid[0],
				join_status_info->bssid[1], join_status_info->bssid[2], join_status_info->bssid[3], join_status_info->bssid[4], join_status_info->bssid[5],
				join_status_info->rssi,
				join_status_info->priv.disconnect.disconn_reason);
	}
}

/* Join status event contains all events related to join, unified into one handle func for processing */
void whc_host_join_status_hdl(u8 *evt_info)
{
	struct rtw_event_join_status_info *join_status_info = (struct rtw_event_join_status_info *)evt_info;

	if (join_status_info == NULL) {
		dev_err(global_idev.pwhc_dev, "Event join status err!\n");
		return;
	}

	switch (join_status_info->status) {
	case RTW_JOINSTATUS_AUTHENTICATING:
		whc_host_event_auth_req(evt_info);
		break;
	case RTW_JOINSTATUS_AUTHENTICATED:
		whc_host_event_auth_resp(evt_info);
		break;
	case RTW_JOINSTATUS_ASSOCIATING:
		whc_host_event_assoc_req(evt_info);
		break;
	case RTW_JOINSTATUS_ASSOCIATED:
		whc_host_event_assoc_resp(evt_info);
		break;
	case RTW_JOINSTATUS_FAIL:
		whc_host_event_join_fail(evt_info);
		break;
	case RTW_JOINSTATUS_DISCONNECT:
		whc_host_event_disconnect_hdl(evt_info);
		break;
	default:
		break;
	}
}

/**
 * @brief external event handle
* Customs can add their own handle functions here
 */
struct rtw_event_hdl_func_t event_external_hdl[1] = {
	{RTW_EVENT_JOIN_STATUS,			whc_host_join_status_hdl},
};
u16 array_len_of_event_external_hdl = sizeof(event_external_hdl) / sizeof(struct rtw_event_hdl_func_t);

