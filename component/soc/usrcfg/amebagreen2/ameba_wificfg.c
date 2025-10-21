/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"
#include <wifi_api.h>
#if defined(CONFIG_WHC_HOST)
#include "whc_ipc.h"
#endif

struct wifi_user_conf wifi_user_config __attribute__((aligned(64)));

_WEAK void wifi_set_user_config(void)
{
	int skb_num_np_rsvd;
	_memset(&wifi_user_config, 0, sizeof(struct wifi_user_conf));

	/* below items for user config, for details, see wifi_user_conf in ameba_wificfg_common.h */
	wifi_user_config.concurrent_enabled = (u8)TRUE;
	wifi_user_config.softap_addr_offset_idx = 1;
	wifi_user_config.fast_reconnect_en = 1;
	wifi_user_config.auto_reconnect_en = 1;
	wifi_user_config.auto_reconnect_count = 10;
	wifi_user_config.auto_reconnect_interval = 5;
	wifi_user_config.no_beacon_disconnect_time = 9; /* unit 2s, default 18s */

#if (defined(CONFIG_WHC_INTF_SDIO) || defined(CONFIG_WHC_INTF_USB))
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
	skb_num_np_rsvd = 6; /* 4 for rx_ring_buffer + 2 for mgnt trx */
#ifdef CONFIG_HIGH_TP_TEST /*enable high tp in make menuconfig*/
	wifi_user_config.skb_num_np = 14; /* skb_num_np should >= rx_ampdu_num + skb_num_np_rsvd */
	wifi_user_config.skb_num_ap = 5;
	wifi_user_config.rx_ampdu_num = 8;
#else
	wifi_user_config.skb_num_np = 10; /* skb_num_np should >= rx_ampdu_num + skb_num_np_rsvd */
	wifi_user_config.skb_num_ap = 4;
	wifi_user_config.rx_ampdu_num = 4;
#endif
#ifdef CONFIG_WHC_NONE
	wifi_user_config.skb_num_ap = 0;
#endif
#endif
	wifi_user_config.tx_ampdu_num = 0; /* 0: default 20, 1: equivalent to wifi_user_config.ampdu_tx_enable = 0, Otherwise: max aggregation number, up to 0x3F*/
	wifi_user_config.skb_buf_size = 0;
	wifi_user_config.wifi_wpa_mode_force = RTW_WPA_AUTO_MODE;

	/*Regulatory related*/
	wifi_user_config.country_code[0] = 0;
	wifi_user_config.country_code[1] = 0;
	wifi_user_config.freq_band_support = RTW_SUPPORT_BAND_MAX;
	wifi_user_config.tx_pwr_table_selection = 2;
	wifi_user_config.rtw_802_11d_en = DISABLE;
	wifi_user_config.rtw_trp_tis_cert_en = RTW_TRP_TIS_DISABLE;
	wifi_user_config.rtw_edcca_mode = RTW_EDCCA_NORM;
	wifi_user_config.tdma_dig_enable = 0; /*add for customer ctrl tdma dig on/off*/
	wifi_user_config.antdiv_mode = RTW_ANTDIV_DISABLE;
	wifi_user_config.probe_hidden_ap_on_passive_ch = 1;

	/* IPS(Inactive Power Save), power save when wifi unconnected */
	wifi_user_config.ips_enable = 1;
	wifi_user_config.ips_level = RTW_IPS_WIFI_OFF;

	/* LPS(Legacy Power Save), power save when wifi connected */
	wifi_user_config.lps_enable = 1;
	wifi_user_config.lps_listen_interval = 0;
	wifi_user_config.wowlan_rx_bcmc_dis = 0;

	/* Softap related */
	wifi_user_config.ap_sta_num = 5;	/*should not exceed 5 */
	wifi_user_config.ap_polling_sta = 0;

	/* MISC */
	wifi_user_config.en_mcc = (u8) DISABLE;  /* must select ENABLE_MCC in menuconfig when wifi_user_config.en_mcc=1 */
	wifi_user_config.mcc_force_p1_slot_ratio = 44;
	wifi_user_config.max_roaming_times = 2;
	wifi_user_config.ampdu_rx_enable = (u8)TRUE;
	wifi_user_config.ampdu_tx_enable = (u8)TRUE;
	wifi_user_config.ap_compatibilty_enabled = 0x07;
	wifi_user_config.set_channel_api_do_rfk = 1;
	wifi_user_config.dpk_peak_limit = 0;
	wifi_user_config.rf_calibration_disable = 0;
	wifi_user_config.tx_shortcut_enable = 1;
	wifi_user_config.rx_shortcut_enable = 1;
	wifi_user_config.keepalive_interval = 20;
	wifi_user_config.rx_cca_thresh = 0;
	wifi_user_config.rate_mask_cck = 0x0;

#if defined(CONFIG_FULLMAC_DEV) &&  !defined(CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
	/* Linux wifi supports cfg80211 ops. */
	wifi_user_config.cfg80211 = 1;
#else
	wifi_user_config.cfg80211 = 0;
#endif

	/*R-Mesh*/
	wifi_user_config.wtn_en = 0;
	wifi_user_config.wtn_strong_rssi_thresh = -50;
	wifi_user_config.wtn_father_refresh_timeout = 3000;
	wifi_user_config.wtn_child_refresh_timeout = 4000;
	wifi_user_config.wtn_max_node_num = 32;

	/* ensure skb_num_np >= rx_ampdu_num + skb_num_np_rsvd */
	if (wifi_user_config.skb_num_np < wifi_user_config.rx_ampdu_num + skb_num_np_rsvd) {
		wifi_user_config.skb_num_np = wifi_user_config.rx_ampdu_num + skb_num_np_rsvd;
		RTK_LOGW(TAG_WLAN_DRV, "change skb_num_np to %d\n", wifi_user_config.skb_num_np);
	}

	/* ensure ap_sta_num not exceed 5*/
	if (wifi_user_config.ap_sta_num > 5) {
		wifi_user_config.ap_sta_num = 5;
		RTK_LOGW(TAG_WLAN_DRV, "change ap_sta_num to 5\n");
	}
}

/**
 * @brief external event handle, customer can add handle functions for wifi events @ref rtw_event_id
 */
__weak struct rtw_event_hdl_func_t event_external_hdl[1] = {
	{RTW_EVENT_MAX,			NULL},
};
__weak u16 array_len_of_event_external_hdl = sizeof(event_external_hdl) / sizeof(struct rtw_event_hdl_func_t);
