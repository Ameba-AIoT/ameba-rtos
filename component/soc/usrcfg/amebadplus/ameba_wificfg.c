/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#ifdef CONFIG_FULLMAC
#include <rtw_cfg80211_fullmac.h>
#else
#include "platform_autoconf.h"
#include <wifi_api.h>
#endif

struct wifi_user_conf wifi_user_config __attribute__((aligned(64)));

_WEAK void wifi_set_user_config(void)
{
	int skb_num_np_rsvd;
	_memset(&wifi_user_config, 0, sizeof(struct wifi_user_conf));

	/* below items for user config, for details, see wifi_user_conf in ameba_wificfg_common.h */
	wifi_user_config.concurrent_enabled = 1;
	wifi_user_config.softap_addr_offset_idx = 1;
	wifi_user_config.fast_reconnect_en = 1;
	wifi_user_config.auto_reconnect_en = 1;
	wifi_user_config.auto_reconnect_count = 10;
	wifi_user_config.auto_reconnect_interval = 5;
	wifi_user_config.no_beacon_disconnect_time = 9;  /* unit 2s, default 18s */

#if (defined(CONFIG_WHC_INTF_SDIO) || defined(CONFIG_WHC_INTF_USB)) || (defined(CONFIG_WHC_BRIDGE))
	skb_num_np_rsvd = 10; /*4 for rx_ring_buffer + 2 for mgnt trx + 4 for spido rx_ring_buffer */
	wifi_user_config.skb_num_np = 20;  /* skb_num_np should >= rx_ampdu_num + skb_num_np_rsvd */
	wifi_user_config.skb_num_ap = 0;
	wifi_user_config.rx_ampdu_num = 8;
#elif defined(CONFIG_WHC_INTF_SPI)
	skb_num_np_rsvd = 7; /*4 for rx_ring_buffer + 2 for mgnt trx + 1 for spi rx_dma_buffer */
	wifi_user_config.skb_num_np = 14;  /* skb_num_np should >= rx_ampdu_num + skb_num_np_rsvd */
	wifi_user_config.skb_num_ap = 0;
	wifi_user_config.rx_ampdu_num = 4;
#else
	skb_num_np_rsvd = 6; /*4 for rx_ring_buffer + 2 for mgnt trx*/
#ifdef CONFIG_HIGH_TP_TEST /*enable high tp in make menuconfig*/
	wifi_user_config.skb_num_np = 14;
	wifi_user_config.skb_num_ap = 5;
	wifi_user_config.rx_ampdu_num = 8;
#else
	wifi_user_config.skb_num_np = 10; /* skb_num_np should >= rx_ampdu_num + skb_num_np_rsvd */
	wifi_user_config.skb_num_ap = 4;
	wifi_user_config.rx_ampdu_num = 4;
#endif
#if WIFI_LOGO_CERTIFICATION
	wifi_user_config.skb_num_ap = 16;
#endif
#ifdef CONFIG_SINGLE_CORE_WIFI
	wifi_user_config.skb_num_ap = 0;
#endif
#endif
	wifi_user_config.skb_buf_size = 0;
	wifi_user_config.wifi_wpa_mode_force = RTW_WPA_AUTO_MODE;

	/*Regulatory related*/
	wifi_user_config.country_code[0] = 0;
	wifi_user_config.country_code[1] = 0;
	wifi_user_config.bw_40_enable = 0;
	wifi_user_config.rtw_tx_pwr_lmt_enable = 2;
	wifi_user_config.rtw_tx_pwr_by_rate	= 2;
	wifi_user_config.rtw_802_11d_en = 0;
	wifi_user_config.rtw_trp_tis_cert_en = RTW_TRP_TIS_DISABLE;
	wifi_user_config.rtw_edcca_mode = RTW_EDCCA_NORM;
	wifi_user_config.antdiv_mode = RTW_ANTDIV_DISABLE;

	/* IPS(Inactive Power Save), power save when wifi unconnected */
	wifi_user_config.ips_enable = 1;
	wifi_user_config.ips_level = RTW_IPS_WIFI_OFF;
	wifi_user_config.ips_ctrl_by_usr = 0;

	/* LPS(Leisure Power Save), power save when wifi connected, has 2 mode - legacy and uapsd, uapsd not support right now */
	wifi_user_config.lps_enable = 1;
	wifi_user_config.lps_mode = RTW_PS_MODE_LEGACY;
	wifi_user_config.legacy_ps_listen_interval = 0;
	wifi_user_config.wowlan_rx_bcmc_dis = 0;

	/* Softap related */
	wifi_user_config.ap_sta_num = 5;	/*should not exceed 14 */
	wifi_user_config.ap_polling_sta = 0;

	/* MISC */
	wifi_user_config.en_mcc = 0;  /* must select ENABLE_MCC in menuconfig when wifi_user_config.en_mcc=1 */
	wifi_user_config.max_roaming_times = 2;
	wifi_user_config.ampdu_rx_enable = 1;
	wifi_user_config.ampdu_tx_enable = 1;
	wifi_user_config.check_dest_address_en = 1;
	wifi_user_config.ap_compatibilty_enabled = 0x0B;
	wifi_user_config.set_channel_api_do_rfk = 1;
	wifi_user_config.rf_calibration_disable = 0;
	wifi_user_config.tx_shortcut_enable = 1;
	wifi_user_config.rx_shortcut_enable = 1;
	wifi_user_config.keepalive_interval = 20;

	/* WPS */
	wifi_user_config.wps_retry_count = 4;
	wifi_user_config.wps_retry_interval = 5000;

#if defined(CONFIG_FULLMAC_DEV)
	/* Linux wifi supports cfg80211 ops. */
	wifi_user_config.cfg80211 = 1;
#else
	wifi_user_config.cfg80211 = 0;
#endif

	/* wifi speaker */
	wifi_user_config.wifi_speaker_feature = 0;

	/*Automatic channel selection*/
	wifi_user_config.acs_en = 0;

	/*R-mesh*/
	wifi_user_config.wtn_en = 0;
	wifi_user_config.wtn_strong_rssi_thresh = -50;
	wifi_user_config.wtn_father_refresh_timeout = 3000;
	wifi_user_config.wtn_child_refresh_timeout = 4000;
	wifi_user_config.wtn_rnat_en = 0;
	wifi_user_config.wtn_fixed_rnat_node = 0;
	wifi_user_config.wtn_connect_only_to_rnat = 0;
	wifi_user_config.wtn_max_node_num = 15;
	if (wifi_user_config.wtn_en) {
		skb_num_np_rsvd = 16; /*4 for rx_ring_buffer + 2 for mgnt trx + 10 for tunnel tx */
		wifi_user_config.skb_num_np = 20; /* skb_num_np should >= rx_ampdu_num + skb_num_np_rsvd */
	}

	/* ensure skb_num_np >= rx_ampdu_num + skb_num_np_rsvd */
	if (wifi_user_config.skb_num_np < wifi_user_config.rx_ampdu_num + skb_num_np_rsvd) {
		wifi_user_config.skb_num_np = wifi_user_config.rx_ampdu_num + skb_num_np_rsvd;
		RTK_LOGW(TAG_WLAN_DRV, "change skb_num_np to %d\n", wifi_user_config.skb_num_np);
	}

	/* ensure ap_sta_num not exceed 14*/
	if (wifi_user_config.ap_sta_num > 14) {
		wifi_user_config.ap_sta_num = 14;
		RTK_LOGW(TAG_WLAN_DRV, "change ap_sta_num to 14\n");
	}
}

