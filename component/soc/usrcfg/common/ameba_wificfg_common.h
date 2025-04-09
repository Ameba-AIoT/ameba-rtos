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
	unsigned char rtw_edcca_mode;

	/*!	For power limit, see Ameba Wi-Fi TX power and Country code Setup Guideline.pdf.\n
		0: Disable, 1: Enable, 2: Depend on efuse(flash). */
	unsigned char rtw_tx_pwr_lmt_enable;

	/*!	For power by rate, see Ameba Wi-Fi TX power and Country code Setup Guideline.pdf.\n
		0: Disable, 1: Enable, 2: Depend on efuse(flash). */
	unsigned char rtw_tx_pwr_by_rate;

	/*!	Enabled during TRP TIS certification. */
	unsigned char rtw_trp_tis_cert_en;

	/*!	Force wpa mode:
		- \b RTW_WPA_AUTO_MODE: Auto mode, follow AP;
		- \b RTW_WPA_ONLY_MODE: Wpa only;
		- \b RTW_WPA2_ONLY_MODE: Wpa2 only;
		- \b RTW_WPA3_ONLY_MODE: Wpa3 only;
		- \b RTW_WPA_WPA2_MIXED_MODE: Wpa and wpa2 mixed;
		- \b RTW_WPA2_WPA3_MIXED_MODE: Wpa2 and wpa3 mixed.*/
	unsigned char wifi_wpa_mode_force;

	/*!	TDMA DIG affects the range of RX, when enabled, both near and distant devices can be received.\n
		0: TDMA DIG off, 1: TDMA DIG on. */
	unsigned char tdma_dig_enable;

	/*!	Antdiv mode:
		- \b RTW_ANTDIV_AUTO: Auto antdiv;
		- \b RTW_ANTDIV_FIX_MAIN: Fix main ant;
		- \b RTW_ANTDIV_FIX_AUX: Fix aux ant;
		- \b RTW_ANTDIV_DISABLE: Antdiv disable. */
	unsigned char antdiv_mode;

	/*!	The maximum number of STAs connected to the softap should not exceed the num specified in notes of func wifi_set_user_config(). */
	unsigned char ap_sta_num;

	/*!	IPS(Inactive power save), If disconnected for more than 2 seconds, WIFI will be powered off. */
	unsigned char ips_enable;

	/*!	Power save status:
		- \b RTW_IPS_WIFI_OFF: The WIFI is powered off during the IPS;
		- \b RTW_IPS_WIFI_PG: The WIFI enters the PG state during the IPS, and therefore it exits the IPS faster. Only dplus support this status. */
	unsigned char ips_level;

	/*!	The driver does not enter the IPS due to 2s disconnection. Instead, API wifi_set_ips_internal() controls the IPS.\n
		0: API wifi_set_ips_internal() control ips enable/disable, 1: Control ips enter/leave. */
	unsigned char ips_ctrl_by_usr;

	/*!	LPS(leisure power save), After connection, with low traffic, part of WIFI can be powered off and woken up upon packet interaction.\n
		0: Disable power save when wifi connected, 1: Enable. */
	unsigned char lps_enable;

	/*!	Power management mode:
		- \b RTW_PS_MODE_LEGACY: During TBTT, wake up to receive beacon; otherwise, WIFI remains in power-save mode;\n
		- \b RTW_PS_MODE_UAPSD_WMM: Not support right now. */
	unsigned char lps_mode;

	/*!	In LPS, the sta wakes up every legacy_ps_listen_interval* 102.4ms to receive beacon.*/
	unsigned char legacy_ps_listen_interval;

	/*!	0: RTW_UAPSD_NO_LIMIT, 1: RTW_UAPSD_TWO_MSDU, 2: RTW_UAPSD_FOUR_MSDU, 3: RTW_UAPSD_SIX_MSDU. */
	unsigned char uapsd_max_sp_len;

	/*!	BIT0: AC_VO UAPSD, BIT1: AC_VI UAPSD, BIT2: AC_BK UAPSD, BIT3: AC_BE UAPSD. */
	unsigned char uapsd_ac_enable;

	/*!	0: Disable ampdu rx, 1: Enable ampdu rx. */
	unsigned char ampdu_rx_enable;

	/*!	0: Disable ampdu tx, 1: Enable ampdu tx. */
	unsigned char ampdu_tx_enable;

	/*!	0: If the pkt's destination address doesn't match, it won't be dropped, 1: If the pkt's destination address doesn't match, it will be dropped. */
	unsigned char bCheckDestAddress;

	/*!	The ap_compatibilty_enabled is used to configure the wlan settings, each bit controls one aspect.
		- <b>bit 0:</b> 0: follow 802.11 spec, do not issue deauth, 1(default): issue deauth in 1st REAUTH_TO to be compatible with ap;
		- <b>bit 1:</b> 0: do not check beacon info to connect with AP with multiple SSID, 1(default): check beacon info;
		- <b>bit 2:</b> 0(default): do not issue deauth at start of auth, 1: issue deauth at start of auth;
		- <b>bit 3:</b> 0: do not switch WEP auth algo unless WLAN_STATUS_NOT_SUPPORTED_AUTH_ALG, 1(default): switch WEP auth algo from shared key to open system in 1st REAUTH_TO;
		- <b>other bits:</b> reserved */
	unsigned char ap_compatibilty_enabled;

	/*!	0: API wifi_set_channel() does not trigger RFK;
		1: API wifi_set_channel() triggers RFK. */
	unsigned char set_channel_api_do_rfk;

	/*!	0: Do not limit the peak current of DPD;
		1: Limit the peak current of DPD. */
	unsigned char dpk_peak_limit;

	/*!	RF calibration is triggered during WIFI initialization and channel switch to calibrate TRX to optimal performance,\n
		but it takes a long time (around 100ms), so some applications can sacrifice performance so that WIFI initializes and switches faster.\n
		- \b Bit0: RTW_DIS_DPK;
		- <b>Other bits:</b> reserved.*/
	u16 rf_calibration_disable;

	/*! The maximum number of roaming attempts triggered by BTM.*/
	unsigned char max_roaming_times;

	/*!	AP periodically sends null pkt to check whether the STA is offline, not support right now.*/
	unsigned char ap_polling_sta;

	/*!	Refer to ameba_wifi_country_code_table_usrcfg.c, e.g. China: country_code[0] = 'C', country_code[1] = 'N'.\n
		Specical value: country_code[0]~[1] = 0x0000: follow efuse; country_code[0]='0', country_code[1]='0' : WORLDWIDE. */
	s8 country_code[2];

	/*!	Bandwidth 40MHz, some IC hardware does not support.*/
	unsigned char bw_40_enable;

	/*!	Refe to 802.11d spec, obtain the country code information from beacon, and set the pwr limit and channel plan.*/
	unsigned char rtw_802_11d_en;

	/*!	When booting the STA, it automatically reconnects to previously connected AP*/
	unsigned char fast_reconnect_en;

	/*!	When disconnection, STA automatically reconnects.*/
	unsigned char auto_reconnect_en;

	/*!	When disconnection, STA automatically reconnects, and auto_reconnect_count is the number of attempts.*/
	unsigned char auto_reconnect_count;

	/*!	auto_reconnect_interval is Automatic reconnection interval, unit s.*/
	unsigned char auto_reconnect_interval;

	/*!	no_beacon_disconnect_time is the disconnect time when no beacon occurs, unit 2s.*/
	unsigned char no_beacon_disconnect_time;

	/*!	skb_num_np is wifi driver's trx buffer number, each buffer occupies about 1.8K bytes of heap, a little difference between different chips.\n
		These buffer are used for all traffics except tx data in INIC mode, and all traffics in single core mode.\n
		For higher throughput or a large number of STAs connected to softap, skb_num_np can be increased.\n
		Minimum: 7 (3 for Ameba lite). Default: 10.*/
	int skb_num_np;

	/*!	These buffer are used for tx data packtes in INIC mode, not used in single core mode.\n
		For higher throughput or a large number of STAs connected to softap, skb_num_ap can be increased. */
	int skb_num_ap;

	/*!	Specify the trx buffer size of each skb.\n
		Cache size(32 for amebadplus&amebalite and 64 for amebasmart)alignment will be applied to the input size.\n
		Considering the length field of L-SIG is 12 bits, the max PSDU size is 4095 bytes, so skb_buf_size is suggested not exceed 4k.\n
		0: use default size. */
	unsigned int skb_buf_size;

	/*!	Every data tx is forced to start with cts2self. */
	unsigned char force_cts2self;

	/*!	Multi Channel Concurrent mode, STA and Softap can work on different channels via TDMA.
		@note Mcc performance has limitations, please contact Realtek before use to clarify your requirements. */
	unsigned char en_mcc;

	unsigned char tx_shortcut_enable;

	unsigned char rx_shortcut_enable;

	/*! For concurrent mode:
	    - \b 0: STA or SoftAP only at any time, The MAC address of TA or Softap is the MAC address of chip;
		- \b 1: STA and SoftAP may run at the same time, Softap's mac address depends on softap_addr_offset_idx. */
	unsigned char concurrent_enabled;

	/*!	It is valid only when concurrent_enabled =1. The range is 0~5.The lowest bit of mac[0] is 1, which represents the multicast address,
		therefore, the mac[0] of softap is incremented by 2, others is incremented by 1.
		- e.g. softap_addr_offset_idx = 0, chip's mac = 00:e0:4c:01:02:03, softap's mac = 02:e0:4c:01:02:03;
		- e.g. softap_addr_offset_idx = 1, chip's mac = 00:e0:4c:01:02:03, softap's mac = 00:e1:4c:01:02:03;
		- e.g. softap_addr_offset_idx = 5, chip's mac = 00:e0:4c:01:02:03, softap's mac = 00:e0:4c:01:02:04.*/
	unsigned char softap_addr_offset_idx;

	/*!	The number of ampdu that Recipient claims to Originator for RX, it can be any value less than 64.\n
		skb_num_np needs to be adjusted simultaneously.*/
	unsigned char rx_ampdu_num;

	/*!	Linux Fullmac architecture, ignore in RTOS.*/
	unsigned char cfg80211;

	/*!	WPS. */
	unsigned char wps_retry_count;

	/*!	Unit: ms. */
	unsigned int wps_retry_interval;

	/*!	For wifi speaker configuration.\n
		BIT0: Main switch, BIT1: Enable tsf interrupt, BIT2: Enable audio tsf. */
	unsigned char wifi_speaker_feature;

	/*! STA mode will periodically send null packet to AP to keepalive, unit: second. */
	unsigned char keepalive_interval;

	/*! Automatic channel selection.*/
	unsigned char acs_en;

	/*! 0: Disable R-mesh function, 1: Enable R-mesh function.*/
	unsigned char wtn_en;

	/*! R-mesh AP strong RSSI thresh, when AP rssi larger than this thresh, will try to switch to AP.*/
	signed char wtn_strong_rssi_thresh;

	/*! R-mesh father refresh timeout, when not receive beacon from father for this timeout, will switch to other node, unit: millisecond.*/
	u16 wtn_father_refresh_timeout;

	/*! R-mesh child refresh timeout, when not receive beacon from child for this timeout, will delete this child, unit: millisecond.*/
	u16 wtn_child_refresh_timeout;

	/*! 0: Disable R-mesh NAT feature, 1: Enable R-mesh NAT feature.*/
	unsigned char wtn_rnat_en;

	/*! 0: Determine whether to become RNAT node based on the rssi to AP, 1: Become RNAT node regardless of the rssi to AP.*/
	unsigned char wtn_fixed_rnat_node;

	/*! Max node number in R-mesh network, this is used for decide each node's beacon window.*/
	u16 wtn_max_node_num;
};

extern struct wifi_user_conf wifi_user_config __attribute__((aligned(64)));

void wifi_set_user_config(void);

#ifdef __cplusplus
}
#endif

#endif

