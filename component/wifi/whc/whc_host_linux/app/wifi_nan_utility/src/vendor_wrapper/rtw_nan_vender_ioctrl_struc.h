#ifndef _RTW_NAN_VENDOR_IOCTRL_STRUC_H_
#define _RTW_NAN_VENDOR_IOCTRL_STRUC_H_

/*
   To correctly exchange info bwteen wifi driver in kernel and
   nan application(cfg80211) in user-space,
   the definitions here have to be the same in both sides.

   This header contains these common definitions
   in ioctrl(osdep) and core level,
   meaning they do not be exposed to phl layer.

   Notice that nan application(cfg80211) could be
   nan utility, hostapd or any user-space apps via cfg80211/iw

   for nan utility:
    - Please make sure this headers within them are identical.

   for hostapd (to support pairing/pasn nan)
    - Please make sure "pasn_nan(hostapd)" section is identical
      in here and realtek_vendor.h
*/


/* for application(cfg80211) */
#if !defined(CONFIG_IFACE_NUMBER) && !defined(MAX_WIFI_ROLE_NUMBER)
#define CONFIG_NAN_APP
#endif

#ifdef CONFIG_NAN_APP
/* the following variables are defined in non-nan headers in driver */
#include "rtw_nan_vender_osdep.h"
#include "phl_nan_def_ioctl.h"
#define NAN_CUSTOMER_NANDOW

#define MAX_SRVC_NAME_LEN 256
#define MAX_SRVC_INST_LEN 64
#define MAX_SRVC_TEXT_LEN 256
#define NAN_MAX_HOSTNAME_LEN 255
#define MAX_INTERFACE_ID_LEN (8)

#define OUI_REALTEK 0x00E04C
#ifndef WFA_OUI
static unsigned char WFA_OUI[] = {0x50, 0x6F, 0x9A};
#endif

#ifdef NAN_CUSTOMER_NANDOW
#ifndef __STDINT_H_
#ifndef _STDINT_H
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int  uint32_t;
//typedef unsigned long uint64_t;
#endif
#endif
#endif /* NAN_CUSTOMER_NANDOW */
#endif /* CONFIG_NAN_APP */

#if defined(CONFIG_NAN) || defined(CONFIG_NAN_APP)

/*
   ### pasn_nan(hostapd) section ###

   these macroes and event structures are
   exposed to hostapd/wpa_supplicant,
   so please make sure that any change are synced
   with realtek_vendor.h in wpa_supplicant

 */

#ifdef CONFIG_NAN_PAIRING
/* (hostapd) struct rtk_nan_pasn_start */
struct nan_event_pasn_start {
	u8 deinit;
	u8 role;
	u8 pairing_type;
	u8 peer_nmi[ETH_ALEN];
	u8 bssid[ETH_ALEN];
	u8 setup_para_auth_method;
	u8 setup_para_nik_chahing;
	u8 setup_para_passphrase_len;
	char setup_para_passphrase[MAX_PASN_PW_LEN];
	u16 auth_frame_len;
	u8 auth_frame[MAX_PASN_RX_LEN];
	u8 append_nan_attr_len;
	u8 append_nan_attr[MAX_PASN_NAN_APPEND_IE_LEN];
	u8 own_pmkid[MAX_PMKID_LEN];
	u8 peer_pmkid[MAX_PMKID_LEN];
} __attribute__((packed));

/* (hostapd) struct rtk_nan_pasn_rx */
struct nan_event_pasn_rx {
	u8 seq;
	u8 pairing_type;
	u8 passphrase_len;
	char passphrase[MAX_PASN_PW_LEN];
	u16 auth_frame_len;
	u8 auth_frame[MAX_PASN_RX_LEN];
	u8 append_nan_attr_len;
	u8 append_nan_attr[MAX_PASN_NAN_APPEND_IE_LEN];
} __attribute__((packed));

/* (hostapd) struct rtk_nan_pasn_upd_pmkid */
struct nan_event_pasn_upd_pmkid {
	u8 role;
	u8 status;
	u8 peer_nmi[ETH_ALEN];
	u8 peer_pmkid[MAX_PMKID_LEN];
	u8 prev_pmkid[MAX_PMKID_LEN];
	u8 auth_method;
} __attribute__((packed));
#endif /* CONFIG_NAN_PAIRING */

enum nan_pmk_setting {
	NAN_PMK_SET_BY_USER_PMK	= 1,
	NAN_PMK_SET_BY_USER_PASSPRHRAE,
	NAN_PMK_SET_BY_PAIRING
};

/**
 * nan_vendor_tx_followup_info
 * @follow_up_type: enum nan_tx_follow_up_type
 * @remote_inst_id: the instance id of the peer's service
 * @local_inst_id: the instance id
 * @remote_mac: the peer's mac
 * @bstrap_type_status:
 *  [3:0] type: 1: request 2: response
 *  [7:4] status: 0: accepted (only applicable with type = 2)
 *                1: rejected (only applicable with type = 2)
 *                2: comeback
 * @bootstrap_method:
 */
struct nan_vendor_tx_followup_info {
	u8 follow_up_type;
	u8 remote_inst_id;
	u8 local_inst_id;
	u8 remote_mac[ETH_ALEN];
	/* general */
	// todo: buf (check iw r1/r2)
	/* bootstrapping param */
	u8 bstrap_type_status;
	u8 bstrap_reason;
	u16 bstrap_method;
	u8 sec_key_type;
};

struct nan_pairing_setpw {
	u8 peer_nmi[ETH_ALEN];
	char pw[MAX_PASN_PW_LEN + 1];
	u8 pw_len;
};

struct nan_pairing_start {
	u8 role;
	u8 type;
	u8 peer_nmi[ETH_ALEN];
	u8 auth_method;
	u8 nik_caching;
};

#ifdef NAN_CUSTOMER_NANDOW

#define MAX_NANDOW_PARA_LEN 2600
#define MAX_NANDOW_REPLY_LEN 512

#include "rtw_nan_vendor_def.h"

typedef enum nan_customer_nandow_cmd_type {
	NANDOW_CMD_TYPE_GET     = (1 << 0),
	NANDOW_CMD_TYPE_SET     = (1 << 1),
} nan_customer_nandow_cmd_type_t;

union nandow_para {
	struct rtw_nan_device_capability dev_cap;
	struct rtw_nan_driver_capabilities drv_cap;
	struct rtw_nan_enable nan_enabled;
	struct rtw_nan_cluster_id cluster_id;
	struct rtw_nan_master_preference master_pref;
	struct rtw_nan_random_factor random_factor;
	struct rtw_nan_discovery_window_awake_period dw_awake_period;
	struct rtw_nan_primary_master_channel pri_master_ch;
	struct rtw_nan_secondary_master_channel second_master_ch;
	struct rtw_nan_discovery_beacon_period disc_bcn_period;
	struct rtw_nan_publish_data publish_srv;
	struct rtw_nan_cancel_publish_data cancel_publish_srv;
	struct rtw_nan_subscribe_data subscribe_srv;
	struct rtw_nan_cancel_subscribe_data cancel_sub_srv;
	struct rtw_nan_follow_up_transmit_data followup_srv;
	struct rtw_nan_datapath_request datapath_req;
	struct rtw_nan_datapath_end datapath_end;
	struct rtw_nan_datapath_response datapath_rsp;
	struct rtw_nan_datapath_confirm datapath_confirm;
	struct rtw_nan_committed_availability avail_cmt;
	struct rtw_nan_potential_availability avail_pot;
	struct rtw_nan_data_cluster_availability avail_ndc;
	struct rtw_nan_forced_discovery_beacon_transmission forced_disc_bcn;
	struct rtw_nan_set_scan_control scan_ctl;
	struct rtw_nan_country_code_data country_code;
};

struct nan_customer_nandow {
	u8 cmd_id;
	u8 cmd_type; // nan_customer_nandow_cmd_type_t
	u32 para_len;
	union nandow_para para;
};
#endif /* NAN_CUSTOMER_NANDOW */
#endif /* CONFIG_NAN || CONFIG_NAN_APP */
#endif /* _RTW_NAN_VENDOR_IOCTRL_STRUC_H_ */
