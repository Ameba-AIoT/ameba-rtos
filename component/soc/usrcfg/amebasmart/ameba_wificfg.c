/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#ifdef CONFIG_FULLMAC
#include <whc_host_linux.h>
#else
#include "platform_autoconf.h"
#include <wifi_api.h>
#endif

struct wifi_user_conf wifi_user_config __attribute__((aligned(64)));

_WEAK void wifi_set_user_config(void)
{
	int skb_num_np_rsvd = 6; /* 4 for rx_ring_buffer + 2 for mgnt trx */
	_memset(&wifi_user_config, 0, sizeof(struct wifi_user_conf));

	/* below items for user config, for details, see wifi_user_conf in ameba_wificfg_common.h */
	wifi_user_config.concurrent_enabled = 1;
	wifi_user_config.softap_addr_offset_idx = 1;
	wifi_user_config.fast_reconnect_en = 1;
	wifi_user_config.auto_reconnect_en = 1;
	wifi_user_config.auto_reconnect_count = 10;
	wifi_user_config.auto_reconnect_interval = 5;
	wifi_user_config.no_beacon_disconnect_time = 9; /* unit 2s, default 18s */
#ifdef CONFIG_HIGH_TP_TEST /*enable high tp in make menuconfig*/
	wifi_user_config.skb_num_np = 22; /* skb_num_np should >= rx_ampdu_num + skb_num_np_rsvd */
	wifi_user_config.skb_num_ap = 8;
#else
#ifdef CONFIG_FULLMAC
	wifi_user_config.skb_num_ap = 10;
#else
	wifi_user_config.skb_num_ap = 8;	/*adjust to 8 for ping 10k*/
#endif
	wifi_user_config.skb_num_np = 22;  /* skb_num_np should >= rx_ampdu_num + skb_num_np_rsvd */
#endif
	wifi_user_config.rx_ampdu_num = 16;

#ifdef CONFIG_SINGLE_CORE_WIFI
	wifi_user_config.skb_num_ap = 0;
#endif
	wifi_user_config.skb_buf_size = 0;
	wifi_user_config.wifi_wpa_mode_force = RTW_WPA_AUTO_MODE;

	/*Regulatory related*/
	wifi_user_config.country_code[0] = 0;
	wifi_user_config.country_code[1] = 0;
	wifi_user_config.tx_pwr_table_selection = 2;
	wifi_user_config.rtw_802_11d_en = 0;
	wifi_user_config.rtw_trp_tis_cert_en = RTW_TRP_TIS_DISABLE;
	wifi_user_config.rtw_edcca_mode = RTW_EDCCA_NORM;
	wifi_user_config.tdma_dig_enable = 0; /*add for customer ctrl tdma dig on/off*/
	wifi_user_config.antdiv_mode = RTW_ANTDIV_DISABLE;
	wifi_user_config.probe_hidden_ap_on_passive_ch = 1;


	/* IPS(Inactive Power Save), power save when wifi unconnected */
	wifi_user_config.ips_enable = 1;
	wifi_user_config.ips_level = RTW_IPS_WIFI_OFF;

	/* LPS(Legacy Power Save), the legacy power save when wifi connected. */
	wifi_user_config.lps_enable = 1;
	wifi_user_config.lps_listen_interval = 0;
	wifi_user_config.wowlan_rx_bcmc_dis = 0;

	/* U-APSD WMM power save when wifi connected. Only one of the lps mode or uapsd mode can be enabled */
	wifi_user_config.uapsd_enable = 0;
	wifi_user_config.uapsd_max_sp_len = 0;
	wifi_user_config.uapsd_ac_enable = 0;

	/* Softap related */
	wifi_user_config.ap_sta_num = 12;	/*should not exceed 12 */
	wifi_user_config.ap_polling_sta = 0;

	/* MISC */
	wifi_user_config.en_mcc = 0;  /* must select ENABLE_MCC in menuconfig when wifi_user_config.en_mcc=1 */
	wifi_user_config.mcc_force_p1_slot_ratio = 44;
	wifi_user_config.max_roaming_times = 2;
	wifi_user_config.ampdu_rx_enable = 1;
	wifi_user_config.ampdu_tx_enable = 1;
	wifi_user_config.check_dest_address_en = 1;
	wifi_user_config.ap_compatibilty_enabled = 0x0B;
	wifi_user_config.set_channel_api_do_rfk = 1;
	wifi_user_config.dpk_peak_limit = 0;
	wifi_user_config.rf_calibration_disable = 0;
	wifi_user_config.tx_shortcut_enable = 1;
	wifi_user_config.rx_shortcut_enable = 1;
	wifi_user_config.keepalive_interval = 20;

#ifdef CONFIG_FULLMAC
	/* Linux wifi supports cfg80211 ops. */
	wifi_user_config.cfg80211 = 1;
#endif

	/* WPS */
	wifi_user_config.wps_retry_count = 4;
	wifi_user_config.wps_retry_interval = 5000;

	/* wifi speaker */
	wifi_user_config.wifi_speaker_feature = 0;

	/* ensure skb_num_np >= rx_ampdu_num + skb_num_np_rsvd */
	if (wifi_user_config.skb_num_np < wifi_user_config.rx_ampdu_num + skb_num_np_rsvd) {
		wifi_user_config.skb_num_np = wifi_user_config.rx_ampdu_num + skb_num_np_rsvd;
#ifndef CONFIG_FULLMAC
		RTK_LOGW(TAG_WLAN_DRV, "change skb_num_np to %d\n", wifi_user_config.skb_num_np);
#endif
	}

	/* ensure ap_sta_num not exceed 12*/
	if (wifi_user_config.ap_sta_num > 12) {
		wifi_user_config.ap_sta_num = 12;
#ifndef CONFIG_FULLMAC
		RTK_LOGW(TAG_WLAN_DRV, "change ap_sta_num to 12\n");
#endif
	}

	if (wifi_user_config.lps_enable && wifi_user_config.uapsd_enable) {
		wifi_user_config.lps_enable = 0;
#ifndef CONFIG_FULLMAC
		RTK_LOGW(TAG_WLAN_DRV, "only enable uspsd mode\n");
#endif
	}
}

