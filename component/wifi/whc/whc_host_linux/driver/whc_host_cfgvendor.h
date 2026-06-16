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

#ifdef NAN_CUSTOMER_NANDOW
/* NAN event IDs */
enum rtw_nan_event_id {
	RTW_NAN_EVT_NAN_CLUSTER_CHANGED                = 1,   // NAN cluster changed
	RTW_NAN_EVT_DISC_RESULT                        = 2,   // Discovery result received
	RTW_NAN_EVT_PUBLISH_REPLIED                    = 3,   // Publish replied by subscriber
	RTW_NAN_EVT_DATAPATH_REQ_RECV                  = 4,   // Datapath request received
	RTW_NAN_EVT_DATAPATH_RSP_RECV                  = 5,   // Datapath response received
	RTW_NAN_EVT_DATAPATH_CONFIRM_RECV              = 6,   // Datapath confirm received
	RTW_NAN_EVT_DATAPATH_ESTABLISHED               = 7,   // Datapath established
	RTW_NAN_EVT_DATAPATH_END                       = 8,   // Datapath ended
	RTW_NAN_EVT_DATAPATH_HOST_ASSIST               = 9,   // Host assist request
	RTW_NAN_EVT_SUBSCRIBE_TERMINATED               = 10,  // Subscribe terminated
	RTW_NAN_EVT_PUBLISH_TERMINATED                 = 11,  // Publish terminated
	RTW_NAN_EVT_FOLLOWUP_RECV                      = 12,  // Follow-up received
	RTW_NAN_EVT_FOLLOWUP_TX_COMPLETE               = 13,  // Follow-up tx complete
	RTW_NAN_EVT_BEACON                             = 14,  // Beacon received
	RTW_NAN_EVT_OOB_AF_TX_STATUS                   = 15,  // Transmit an out of bound action frame (OOB AF)
	RTW_NAN_EVT_OOB_AF_RX                          = 16,  // Receive an out of bound action frame (OOB AF)
	RTW_NAN_EVT_RESET_INTERFACE                    = 17,  // Chip reset
	RTW_NAN_EVT_COUNTRY_CODE_CHANGED               = 18,  // Country code changed
	RTW_NAN_EVT_INFRA_ASSOC_START                  = 19,  // Infra assoc start
	RTW_NAN_EVT_ASSOC_DONE                         = 20,  // Association done
	RTW_NAN_EVT_ASSOC_READY                        = 21,  // Association ready
	RTW_NAN_EVT_AUTH                               = 22,  // Authentication
	RTW_NAN_EVT_ASSOC                              = 23,  // Association
	RTW_NAN_EVT_INFRA_SCAN_START                   = 24,  // Infra scan start
	RTW_NAN_EVT_INFRA_SCAN_COMPLETE                = 25,  // Infra scan complete
	RTW_NAN_EVT_SET_INFRA_CHANNEL                  = 26,  // Set infra channel
};
#endif

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
	NAN_EVENT_NANDOW_MAX    =   RTW_NAN_EVT_SET_INFRA_CHANNEL,
#endif
	NAN_EVENT_DATA_TERMINATION,
	NAN_EVENT_DATA_INDICATION,
	NAN_EVENT_PASN_START,
	NAN_EVENT_PASN_RX,
	NAN_EVENT_PASN_UPD_PMKID,
	NAN_EVENT_BSTRAP_RESULT,
	NAN_EVENT_PASN_RESULT,
};

#ifdef NAN_CUSTOMER_NANDOW
#define NANDOW_EVENTID_OFFSET 0
#define GET_OVERALL_EVENT_ID(nandow_id) (nandow_id+NANDOW_EVENTID_OFFSET)
#endif

void whc_host_cfgvendor_attach(struct wiphy *wiphy);
void whc_host_cfgvendor_detach(struct wiphy *wiphy);

void whc_host_cfgvendor_nan_event_indication(u8 event_id, void *event, int size);
void whc_host_cfgvendor_send_cmd_reply(void  *data, int len);
#endif
