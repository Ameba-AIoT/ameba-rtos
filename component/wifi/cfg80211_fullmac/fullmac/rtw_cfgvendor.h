// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifdef CONFIG_NAN
#define OUI_REALTEK 0x00E04C

enum ANDROID_VENDOR_SUB_COMMAND {
	/* don't use 0 as a valid subcommand */
	VENDOR_NL80211_SUBCMD_UNSPECIFIED,

	/* define all NAN related commands between 0x1900 and 0x19FF */
	NL80211_SUBCMD_NAN_RANGE_START = 0x1900,
	NL80211_SUBCMD_NAN_RANGE_END   = 0x19FF,

	/* This is reserved for future usage */
};

enum rtw_vendor_subcmd {
	NAN_SUBCMD_SRVC_EXT_INFO = NL80211_SUBCMD_NAN_RANGE_START,	/* 0x1900 */
	NAN_SUBCMD_DATA_REQ,						/* 0x1901 */
	NAN_SUBCMD_DATA_RSP,						/* 0x1902 */
	NAN_SUBCMD_DATA_END,						/* 0x1903 */
	NAN_SUBCMD_DATA_UPDATE,					/* 0x1904 */
	NAN_SUBCMD_CUSTOMER_NANDOW,			/* 0x1905 */
	NAN_SUBCMD_FOLLOWUP,						/* 0x1906 */
	NAN_SUBCMD_PAIRING_SET_PW,          /* 0x1907 */
	NAN_SUBCMD_PASN_START,              /* 0x1908 */
	NAN_SUBCMD_PASN_SET_KEY,            /* 0x1909 */

	VENDOR_SUBCMD_MAX
};

enum rtw_vendor_event {
	NAN_EVENT_DATA_COMFIRM,
#ifdef NAN_CUSTOMER_NANDOW
	NAN_EVENT_NANDOW_MAX    =   WFPAL_M_SET_INFRA_CHANNEL,
#endif
	NAN_EVENT_DATA_TERMINATION,
	NAN_EVENT_DATA_INDICATION,
	NAN_EVENT_PASN_START,
	NAN_EVENT_PASN_RX,
	NAN_EVENT_PASN_UPD_PMKID,
};

void rtw_cfgvendor_attach(struct wiphy *wiphy);
void rtw_cfgvendor_detach(struct wiphy *wiphy);

void rtw_cfgvendor_nan_event_indication(u8 event_id, void *event, int size);
void rtw_cfgvendor_send_cmd_reply(void  *data, int len);
#endif

