// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <rtw_cfg80211_fullmac.h>
#ifdef CONFIG_NAN

/*---------------------------events--------------------------*/
void rtw_cfgvendor_nan_event_indication(u8 event_id, void *event, int size)
{
	struct wiphy *wiphy = global_idev.pwiphy_global;
	struct wireless_dev *wdev = global_idev.pwdev_global[2];
	struct sk_buff *skb;
	u32 tot_len = NLMSG_DEFAULT_SIZE;
	gfp_t kflags;

	kflags = in_atomic() ? GFP_ATOMIC : GFP_KERNEL;

	/* Alloc the SKB for vendor_event */
	skb = cfg80211_vendor_event_alloc(wiphy, wdev, tot_len, event_id, kflags);
	if (!skb) {
		goto exit;
	}

	nla_append(skb, size, event);

	cfg80211_vendor_event(skb, kflags);
exit:
	return;
}

void rtw_cfgvendor_send_cmd_reply(void  *data, int len)
{
	struct wiphy *wiphy = global_idev.pwiphy_global;
	struct sk_buff *skb;

	/* Alloc the SKB for vendor_event */
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len);
	if (unlikely(!skb)) {
		goto exit;
	}

	/* Push the data to the skb */
	nla_put_nohdr(skb, len, data);

	cfg80211_vendor_cmd_reply(skb);
exit:
	return;
}

/*---------------------------cmds--------------------------*/
static int rtw_cfgvendor_set_nan_srvc_extinfo(struct wiphy *wiphy,
		struct wireless_dev *wdev, const void *data, int len)
{
	llhw_wifi_nan_cfgvendor_cmd(NAN_SUBCMD_SRVC_EXT_INFO, data, len);
	return 0;
}

static int rtw_cfgvendor_set_nan_data_request(struct wiphy *wiphy,
		struct wireless_dev *wdev, const void *data, int len)
{
	llhw_wifi_nan_cfgvendor_cmd(NAN_SUBCMD_DATA_REQ, data, len);

	return 0;
}

static int rtw_cfgvendor_set_nan_data_response(struct wiphy *wiphy,
		struct wireless_dev *wdev, const void *data, int len)
{
	llhw_wifi_nan_cfgvendor_cmd(NAN_SUBCMD_DATA_RSP, data, len);

	return 0;
}

static int rtw_cfgvendor_set_nan_data_end(struct wiphy *wiphy,
		struct wireless_dev *wdev, const void *data, int len)
{
	llhw_wifi_nan_cfgvendor_cmd(NAN_SUBCMD_DATA_END, data, len);

	return 0;
}

#ifdef CONFIG_NAN_PAIRING
static int rtw_cfgvendor_set_nan_follow_up(struct wiphy *wiphy,
		struct wireless_dev *wdev, const void *data, int len)
{
	llhw_wifi_nan_cfgvendor_cmd(NAN_SUBCMD_FOLLOWUP, data, len);

	return 0;
}

static int rtw_cfgvendor_nan_pairing_set_pw(struct wiphy *wiphy,
		struct wireless_dev *wdev, const void *data, int len)
{
	llhw_wifi_nan_cfgvendor_cmd(NAN_SUBCMD_PAIRING_SET_PW, data, len);

	return 0;
}

static int rtw_cfgvendor_nan_pasn_start(struct wiphy *wiphy,
										struct wireless_dev *wdev, const void *data, int len)
{
	llhw_wifi_nan_cfgvendor_cmd(NAN_SUBCMD_PASN_START, data, len);

	return 0;
}

static int rtw_cfgvendor_nan_pasn_setkeys(struct wiphy *wiphy,
		struct wireless_dev *wdev, const void *data, int len)
{
	llhw_wifi_nan_cfgvendor_cmd(NAN_SUBCMD_PASN_SET_KEY, data, len);

	return 0;
}
#endif /* CONFIG_NAN_PAIRING */

#ifdef NAN_CUSTOMER_NANDOW
static int rtw_cfgvendor_nandow_entry(struct wiphy *wiphy,
									  struct wireless_dev *wdev, const void *data, int len)
{
	llhw_wifi_nan_cfgvendor_cmd(NAN_SUBCMD_CUSTOMER_NANDOW, data, len);

	return 0;
}
#endif

static const struct  nl80211_vendor_cmd_info rtw_vendor_events[] = {
	{ OUI_REALTEK, NAN_EVENT_DATA_COMFIRM},
#ifdef NAN_CUSTOMER_NANDOW
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_CLUSTER_CHANGED)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_DISCOVERY_RESULT_RECEIVED)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_PUBLISH_REPLIED_PRIVATE)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_DATAPATH_REQUEST_RECEIVED)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_DATAPATH_RESPONSE_RECEIVED)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_DATAPATH_CONFIRM_RECEIVED)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_DATAPATH_ESTABLISHED)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_DATAPATH_END)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_DATAPATH_HOST_ASSIST_REQUEST)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_SUBSCRIBE_TERMINATED)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_PUBLISH_TERMINATED)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_FOLLOW_UP_RECEIVED)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_FOLLOW_UP_TX_COMPLETE)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_BEACON)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_OUT_OF_BOUND_ACTION_FRAME_TX_STATUS)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_NAN_OUT_OF_BOUND_ACTION_FRAME_RX)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_RESET_INTERFACE)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_COUNTRY_CODE_CHANGED)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_INFRA_ASSOC_START)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_ASSOC_DONE)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_ASSOC_READY)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_AUTH)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_ASSOC)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_INFRA_SCAN_START)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_INFRA_SCAN_COMPLETE)},
	{ OUI_REALTEK, GET_OVERALL_EVENT_ID(WFPAL_M_SET_INFRA_CHANNEL)},
#endif
	{ OUI_REALTEK, NAN_EVENT_DATA_TERMINATION},
	{ OUI_REALTEK, NAN_EVENT_DATA_INDICATION},
	{ OUI_REALTEK, NAN_EVENT_PASN_START},
	{ OUI_REALTEK, NAN_EVENT_PASN_RX},
	{ OUI_REALTEK, NAN_EVENT_PASN_UPD_PMKID},
};


static const struct wiphy_vendor_command rtw_vendor_cmds[] = {
	{
		{
			.vendor_id = OUI_REALTEK,
			.subcmd = NAN_SUBCMD_SRVC_EXT_INFO
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.policy = VENDOR_CMD_RAW_DATA,
		.doit = rtw_cfgvendor_set_nan_srvc_extinfo
	},
	{
		{
			.vendor_id = OUI_REALTEK,
			.subcmd = NAN_SUBCMD_DATA_REQ
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.policy = VENDOR_CMD_RAW_DATA,
		.doit = rtw_cfgvendor_set_nan_data_request
	},
	{
		{
			.vendor_id = OUI_REALTEK,
			.subcmd = NAN_SUBCMD_DATA_RSP
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.policy = VENDOR_CMD_RAW_DATA,
		.doit = rtw_cfgvendor_set_nan_data_response
	},
	{
		{
			.vendor_id = OUI_REALTEK,
			.subcmd = NAN_SUBCMD_DATA_END
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.policy = VENDOR_CMD_RAW_DATA,
		.doit = rtw_cfgvendor_set_nan_data_end
	},
#ifdef CONFIG_NAN_PAIRING
	{
		{
			.vendor_id = OUI_REALTEK,
			.subcmd = NAN_SUBCMD_FOLLOWUP
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.policy = VENDOR_CMD_RAW_DATA,
		.doit = rtw_cfgvendor_set_nan_follow_up
	},
#endif
#ifdef NAN_CUSTOMER_NANDOW
	{
		{
			.vendor_id = OUI_REALTEK,
			.subcmd = NAN_SUBCMD_CUSTOMER_NANDOW
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.policy = VENDOR_CMD_RAW_DATA,
		.doit = rtw_cfgvendor_nandow_entry
	},
#endif /* NAN_CUSTOMER_NANDOW */
#ifdef CONFIG_NAN_PAIRING
	{
		{
			.vendor_id = OUI_REALTEK,
			.subcmd = NAN_SUBCMD_PAIRING_SET_PW
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.policy = VENDOR_CMD_RAW_DATA,
		.doit = rtw_cfgvendor_nan_pairing_set_pw
	},
	{
		{
			.vendor_id = OUI_REALTEK,
			.subcmd = NAN_SUBCMD_PASN_START
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.policy = VENDOR_CMD_RAW_DATA,
		.doit = rtw_cfgvendor_nan_pasn_start
	},
	{
		{
			.vendor_id = OUI_REALTEK,
			.subcmd = NAN_SUBCMD_PASN_SET_KEY
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.policy = VENDOR_CMD_RAW_DATA,
		.doit = rtw_cfgvendor_nan_pasn_setkeys
	}
#endif /* CONFIG_NAN_PAIRING */
};

void rtw_cfgvendor_attach(struct wiphy *wiphy)
{
	wiphy->vendor_commands	= rtw_vendor_cmds;
	wiphy->n_vendor_commands = ARRAY_SIZE(rtw_vendor_cmds);
	wiphy->vendor_events	= rtw_vendor_events;
	wiphy->n_vendor_events	= ARRAY_SIZE(rtw_vendor_events);
}

void rtw_cfgvendor_detach(struct wiphy *wiphy)
{
	wiphy->vendor_commands  = NULL;
	wiphy->vendor_events    = NULL;
	wiphy->n_vendor_commands = 0;
	wiphy->n_vendor_events  = 0;
}
#endif /* CONFIG_NAN */

