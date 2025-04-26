#ifndef _AMEBA_WIFICFG_COMMON_H_
#define _AMEBA_WIFICFG_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  The structure is used to describe the wifi user configuration, can be configured in ameba_wificfg.c.
  */
struct wifi_user_conf {
	/*!	This effects EDCCA threshold, wifi won't TX if detected energy exceeds threshold.
		- \b RTW_EDCCA_NORM: Adjust EDCCA threshold dynamically;
		- \b RTW_EDCCA_ADAPT: For ESTI or SRRC;
		- \b RTW_EDCCA_CS: For japan;
		- \b RTW_EDCCA_DISABLE: Ingore EDCCA. */
	u8 rtw_edcca_mode;

	/*!	For power limit, see Ameba Wi-Fi TX power and Country code Setup Guideline.pdf.\n
		0: Disable, 1: Enable, 2: Depend on efuse(flash). */
	u8 rtw_tx_pwr_lmt_enable;

	/*!	For power by rate, see Ameba Wi-Fi TX power and Country code Setup Guideline.pdf.\n
		0: Disable, 1: Enable, 2: Depend on efuse(flash). */
	u8 rtw_tx_pwr_by_rate;

	/*!	Enabled during TRP TIS certification. */
	u8 rtw_trp_tis_cert_en;

	/*!	Force wpa mode:
		- \b RTW_WPA_AUTO_MODE: Auto mode, follow AP;
		- \b RTW_WPA_ONLY_MODE: Wpa only;
		- \b RTW_WPA2_ONLY_MODE: Wpa2 only;
		- \b RTW_WPA3_ONLY_MODE: Wpa3 only;
		- \b RTW_WPA_WPA2_MIXED_MODE: Wpa and wpa2 mixed;
		- \b RTW_WPA2_WPA3_MIXED_MODE: Wpa2 and wpa3 mixed.*/
	u8 wifi_wpa_mode_force;

	/*!	TDMA DIG affects the range of RX, when enabled, both near and distant devices can be received.\n
		0: TDMA DIG off, 1: TDMA DIG on. */
	u8 tdma_dig_enable;

	/*!	Antdiv mode:
		- \b RTW_ANTDIV_AUTO: Auto antdiv;
		- \b RTW_ANTDIV_FIX_MAIN: Fix main ant;
		- \b RTW_ANTDIV_FIX_AUX: Fix aux ant;
		- \b RTW_ANTDIV_DISABLE: Antdiv disable. */
	u8 antdiv_mode;

	/*!	The maximum number of STAs connected to the softap should not exceed the num specified in notes of func wifi_set_user_config(). */
	u8 ap_sta_num;

	/*!	IPS(Inactive power save), If disconnected for more than 2 seconds, WIFI will be powered off. */
	u8 ips_enable;

	/*!	Power save status:
		- \b RTW_IPS_WIFI_OFF: The WIFI is powered off during the IPS;
		- \b RTW_IPS_WIFI_PG: The WIFI enters the PG state during the IPS, and therefore it exits the IPS faster. Only dplus support this status. */
	u8 ips_level;

	/*!	The driver does not enter the IPS due to 2s disconnection. Instead, API wifi_set_ips_internal() controls the IPS.\n
		0: API wifi_set_ips_internal() control ips enable/disable, 1: Control ips enter/leave. */
	u8 ips_ctrl_by_usr;

	/*!	LPS(Legacy power save). After connection, with low traffic, part of WIFI can be powered off and woken up upon packet interaction.\n
		There are two power save modes: legacy power save and uapsd mode. Can only enable one of them.\n
		0: Disable legacy power save, 1: Enable. */
	u8 lps_enable;

	/*!	In LPS, the sta wakes up every lps_listen_interval * 102.4ms to receive beacon.*/
	u8 lps_listen_interval;

	/*! Enable or disable rx broadcast of LPS. LPS is the abbreviation of Leisure Power Save mode.
		1 means disable rx broadcast of LPS, 0 means enable(default) rx broadcast of LPS.*/
	u8 wowlan_rx_bcmc_dis;

	/*!	U-APSD WMM power save mode. After connection, with low traffic, part of WIFI can be powered off and woken up upon packet interaction.\n
		There are two power save modes: legacy power save and uapsd mode. Can only enable one of them.
		0: Disable uapsd mode, 1: Enable. */
	u8 uapsd_enable;

	/*!	0: RTW_UAPSD_NO_LIMIT, 1: RTW_UAPSD_TWO_MSDU, 2: RTW_UAPSD_FOUR_MSDU, 3: RTW_UAPSD_SIX_MSDU. */
	u8 uapsd_max_sp_len;

	/*!	BIT0: AC_VO UAPSD, BIT1: AC_VI UAPSD, BIT2: AC_BK UAPSD, BIT3: AC_BE UAPSD. */
	u8 uapsd_ac_enable;

	/*!	0: Disable ampdu rx, 1: Enable ampdu rx. */
	u8 ampdu_rx_enable;

	/*!	0: Disable ampdu tx, 1: Enable ampdu tx. */
	u8 ampdu_tx_enable;

	/*!	0: If the pkt's destination address doesn't match, it won't be dropped, 1: If the pkt's destination address doesn't match, it will be dropped. */
	u8 check_dest_address_en;

	/*!	The ap_compatibilty_enabled is used to configure the wlan settings, each bit controls one aspect.
		- <b>bit 0:</b> 0: follow 802.11 spec, do not issue deauth, 1(default): issue deauth in 1st REAUTH_TO to be compatible with ap;
		- <b>bit 1:</b> 0: do not check beacon info to connect with AP with multiple SSID, 1(default): check beacon info;
		- <b>bit 2:</b> 0(default): do not issue deauth at start of auth, 1: issue deauth at start of auth;
		- <b>bit 3:</b> 0: do not switch WEP auth algo unless WLAN_STATUS_NOT_SUPPORTED_AUTH_ALG, 1(default): switch WEP auth algo from shared key to open system in 1st REAUTH_TO;
		- <b>other bits:</b> reserved */
	u8 ap_compatibilty_enabled;

	/*!	0: API wifi_set_channel() does not trigger RFK;
		1: API wifi_set_channel() triggers RFK. */
	u8 set_channel_api_do_rfk;

	/*!	0: Do not limit the peak current of DPD;
		1: Limit the peak current of DPD. */
	u8 dpk_peak_limit;

	/*!	RF calibration is triggered during WIFI initialization and channel switch to calibrate TRX to optimal performance,\n
		but it takes a long time (around 100ms), so some applications can sacrifice performance so that WIFI initializes and switches faster.\n
		- \b Bit0: RTW_RFK_DIS_DPK;
		- <b>Other bits:</b> reserved.*/
	u16 rf_calibration_disable;

	/*! The maximum number of roaming attempts triggered by BTM.*/
	u8 max_roaming_times;

	/*!	AP periodically sends null pkt to check whether the STA is offline, not support right now.*/
	u8 ap_polling_sta;

	/*!	Refer to ameba_wifi_country_code_table_usrcfg.c, e.g. China: country_code[0] = 'C', country_code[1] = 'N'.\n
		Specical value: country_code[0]~[1] = 0x0000: follow efuse; country_code[0]='0', country_code[1]='0' : WORLDWIDE. */
	u8 country_code[2];

	/*!	Bandwidth 40MHz, some IC hardware does not support.*/
	u8 bw_40_enable;

	/*!	Refe to 802.11d spec, obtain the country code information from beacon, and set the pwr limit and channel plan.*/
	u8 rtw_802_11d_en;

	/*!	When booting the STA, it automatically reconnects to previously connected AP*/
	u8 fast_reconnect_en;

	/*!	When disconnection, STA automatically reconnects.*/
	u8 auto_reconnect_en;

	/*!	When disconnection, STA automatically reconnects, and auto_reconnect_count is the number of attempts.
		Specical value: 0xff means infinite retry count*/
	u8 auto_reconnect_count;

	/*!	auto_reconnect_interval is Automatic reconnection interval, unit s.*/
	u8 auto_reconnect_interval;

	/*!	no_beacon_disconnect_time is the disconnect time when no beacon occurs, unit 2s.*/
	u8 no_beacon_disconnect_time;

	/*!	skb_num_np is wifi driver's trx buffer number, each buffer occupies about 1.8K bytes of heap, a little difference between different chips.\n
		These buffer are used for all traffics except tx data in INIC mode, and all traffics in single core mode.\n
		For higher throughput or a large number of STAs connected to softap, skb_num_np can be increased.\n
		Minimum: 7 (3 for Ameba lite). Default: 10.*/
	s32 skb_num_np;

	/*!	These buffer are used for tx data packtes in INIC mode, not used in single core mode.\n
		For higher throughput or a large number of STAs connected to softap, skb_num_ap can be increased. */
	s32 skb_num_ap;

	/*!	Specify the trx buffer size of each skb.\n
		Cache size(32 for amebadplus&amebalite and 64 for amebasmart)alignment will be applied to the input size.\n
		Considering the length field of L-SIG is 12 bits, the max PSDU size is 4095 bytes, so skb_buf_size is suggested not exceed 4k.\n
		0: use default size. */
	u32 skb_buf_size;

	/*!	Every data tx is forced to start with cts2self. */
	u8 force_cts2self;

	/*!	Multi Channel Concurrent mode, STA and Softap can work on different channels via TDMA.
		@note Mcc performance has limitations, please contact Realtek before use to clarify your requirements. */
	u8 en_mcc;

	u8 tx_shortcut_enable;

	u8 rx_shortcut_enable;

	/*! For concurrent mode:
	    - \b 0: STA or SoftAP only at any time, The MAC address of TA or Softap is the MAC address of chip;
		- \b 1: STA and SoftAP may run at the same time, Softap's mac address depends on softap_addr_offset_idx. */
	u8 concurrent_enabled;

	/*!	It is valid only when concurrent_enabled =1. The range is 0~5.The lowest bit of mac[0] is 1, which represents the multicast address,
		therefore, the mac[0] of softap is incremented by 2, others is incremented by 1.
		- e.g. softap_addr_offset_idx = 0, chip's mac = 00:e0:4c:01:02:03, softap's mac = 02:e0:4c:01:02:03;
		- e.g. softap_addr_offset_idx = 1, chip's mac = 00:e0:4c:01:02:03, softap's mac = 00:e1:4c:01:02:03;
		- e.g. softap_addr_offset_idx = 5, chip's mac = 00:e0:4c:01:02:03, softap's mac = 00:e0:4c:01:02:04.*/
	u8 softap_addr_offset_idx;

	/*!	The number of ampdu that Recipient claims to Originator for RX, it can be any value less than 64.\n
		skb_num_np needs to be adjusted simultaneously.*/
	u8 rx_ampdu_num;

	/*!	Linux Fullmac architecture, ignore in RTOS.*/
	u8 cfg80211;

	/*!	WPS. */
	u8 wps_retry_count;

	/*!	Unit: ms. */
	u32 wps_retry_interval;

	/*!	For wifi speaker configuration.\n
		BIT0: Main switch, BIT1: Enable tsf interrupt, BIT2: Enable audio tsf. */
	u8 wifi_speaker_feature;

	/*! STA mode will periodically send null packet to AP to keepalive, unit: second. */
	u8 keepalive_interval;

	/*! Automatic channel selection.*/
	u8 acs_en;

	/*! 0: Disable R-mesh function, 1: Enable R-mesh function.*/
	u8 wtn_en;

	/*! R-mesh AP strong RSSI thresh, when AP rssi larger than this thresh, will try to switch to AP.*/
	s8 wtn_strong_rssi_thresh;

	/*! R-mesh father refresh timeout, when not receive beacon from father for this timeout, will switch to other node, unit: millisecond.*/
	u16 wtn_father_refresh_timeout;

	/*! R-mesh child refresh timeout, when not receive beacon from child for this timeout, will delete this child, unit: millisecond.*/
	u16 wtn_child_refresh_timeout;

	/*! 0: Disable R-mesh NAT feature, 1: Enable R-mesh NAT feature.*/
	u8 wtn_rnat_en;

	/*! 0: Determine whether to become RNAT node based on the rssi to AP, 1: Become RNAT node regardless of the rssi to AP.*/
	u8 wtn_fixed_rnat_node;

	/*! 0: This device can connect to R-Mesh group or R-NAT group, 1: this device will only connect to R-NAT group.*/
	u8 wtn_connect_only_to_rnat;

	/*! Max node number in R-mesh network, this is used for decide each node's beacon window.*/
	u16 wtn_max_node_num;
};

extern struct wifi_user_conf wifi_user_config __attribute__((aligned(64)));

void wifi_set_user_config(void);

#ifdef __cplusplus
}
#endif

#endif

