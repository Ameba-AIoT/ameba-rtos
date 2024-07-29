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

static const u32 rtw_cipher_suites[] = {
	WLAN_CIPHER_SUITE_WEP40,
	WLAN_CIPHER_SUITE_WEP104,
	WLAN_CIPHER_SUITE_TKIP,
	WLAN_CIPHER_SUITE_CCMP,
	WLAN_CIPHER_SUITE_AES_CMAC,
	WIFI_CIPHER_SUITE_GCMP,
	WIFI_CIPHER_SUITE_GCMP_256,
	WIFI_CIPHER_SUITE_CCMP_256,
	WIFI_CIPHER_SUITE_BIP_GMAC_128,
	WIFI_CIPHER_SUITE_BIP_GMAC_256,
	WIFI_CIPHER_SUITE_BIP_CMAC_256,
};

const struct ieee80211_channel rtw_2ghz_channels[MAX_CHANNEL_NUM_2G] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
	CHAN2G(12, 2467, 0),
	CHAN2G(13, 2472, 0),
	CHAN2G(14, 2484, 0),
};

const struct ieee80211_channel rtw_5ghz_a_channels[MAX_CHANNEL_NUM_5G] = {
	CHAN5G(36, 0),	CHAN5G(40, 0),	CHAN5G(44, 0),	CHAN5G(48, 0),
	CHAN5G(52, 0),	CHAN5G(56, 0),	CHAN5G(60, 0),	CHAN5G(64, 0),
	CHAN5G(100, 0),	CHAN5G(104, 0),	CHAN5G(108, 0),	CHAN5G(112, 0),
	CHAN5G(116, 0),	CHAN5G(120, 0),	CHAN5G(124, 0),	CHAN5G(128, 0),
	CHAN5G(132, 0),	CHAN5G(136, 0),	CHAN5G(140, 0),	CHAN5G(144, 0),
	CHAN5G(149, 0),	CHAN5G(153, 0),	CHAN5G(157, 0),	CHAN5G(161, 0),
	CHAN5G(165, 0),	CHAN5G(169, 0),	CHAN5G(173, 0),	CHAN5G(177, 0),
};

static const struct ieee80211_rate rtw_rates[] = {
	RATETAB_ENT(10,  0x1,   0),
	RATETAB_ENT(20,  0x2,   0),
	RATETAB_ENT(55,  0x4,   0),
	RATETAB_ENT(110, 0x8,   0),
	RATETAB_ENT(60,  0x10,  0),
	RATETAB_ENT(90,  0x20,  0),
	RATETAB_ENT(120, 0x40,  0),
	RATETAB_ENT(180, 0x80,  0),
	RATETAB_ENT(240, 0x100, 0),
	RATETAB_ENT(360, 0x200, 0),
	RATETAB_ENT(480, 0x400, 0),
	RATETAB_ENT(540, 0x800, 0),
};

static const struct ieee80211_txrx_stypes
	rtw_cfg80211_default_mgmt_stypes[NUM_NL80211_IFTYPES] = {
	[NL80211_IFTYPE_STATION] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_AUTH >> 4) |
		BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
	},
	[NL80211_IFTYPE_AP] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
		BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
		BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
		BIT(IEEE80211_STYPE_DISASSOC >> 4) |
		BIT(IEEE80211_STYPE_AUTH >> 4) |
		BIT(IEEE80211_STYPE_DEAUTH >> 4) |
		BIT(IEEE80211_STYPE_ACTION >> 4)
	},
	[NL80211_IFTYPE_AP_VLAN] = {
		/* copy AP */
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
		BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
		BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
		BIT(IEEE80211_STYPE_DISASSOC >> 4) |
		BIT(IEEE80211_STYPE_AUTH >> 4) |
		BIT(IEEE80211_STYPE_DEAUTH >> 4) |
		BIT(IEEE80211_STYPE_ACTION >> 4)
	},
#ifdef CONFIG_P2P
	[NL80211_IFTYPE_P2P_CLIENT] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_AUTH >> 4) |
		BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
	},
	[NL80211_IFTYPE_P2P_GO] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
		BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
		BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
		BIT(IEEE80211_STYPE_DISASSOC >> 4) |
		BIT(IEEE80211_STYPE_AUTH >> 4) |
		BIT(IEEE80211_STYPE_DEAUTH >> 4) |
		BIT(IEEE80211_STYPE_ACTION >> 4)
	},
	[NL80211_IFTYPE_P2P_DEVICE] = {
		.tx = 0xffff,
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
	},
#endif

};

/* if wowlan is not supported, kernel generate a disconnect at each suspend
 * cf: /net/wireless/sysfs.c, so register a stub wowlan.
 * Moreover wowlan has to be enabled via a the nl80211_set_wowlan callback.
 * (from user space, e.g. iw phy0 wowlan enable)
 */
static const struct wiphy_wowlan_support rtw_wowlan_stub = {
	.flags = WIPHY_WOWLAN_ANY,
	.n_patterns = 0,
	.pattern_max_len = 0,
	.pattern_min_len = 0,
	.max_pkt_offset = 0,
};

#ifdef CONFIG_P2P /*for wpa_supplicant set WPA_DRIVER_FLAGS_P2P_CONCURRENT and P2P_MGMT_AND_NON_P2P*/
struct ieee80211_iface_limit rtw_limits[] = {
	{
		.max = 1,
		.types = BIT(NL80211_IFTYPE_STATION)
		| BIT(NL80211_IFTYPE_P2P_CLIENT)
	},
	{
		.max = 1,
		.types = BIT(NL80211_IFTYPE_AP)
		| BIT(NL80211_IFTYPE_P2P_GO)
	},
	{
		.max = 1,
		.types = BIT(NL80211_IFTYPE_P2P_DEVICE)
	},
};

struct ieee80211_iface_combination rtw_combinations[] = {
	{
		.limits = rtw_limits,
		.n_limits = ARRAY_SIZE(rtw_limits),
		.max_interfaces = 2,
		.num_different_channels = 1,
	},
};
#endif

int rtw_wiphy_band_init(struct wiphy *pwiphy, u32 band_type)
{
	int n_channels = 0, n_bitrates = 0;
	struct ieee80211_supported_band *band = NULL;

	if (band_type == NL80211_BAND_2GHZ) {
		n_channels = MAX_CHANNEL_NUM_2G;
		n_bitrates = RTW_G_RATES_NUM;
	} else if (band_type == NL80211_BAND_5GHZ) {
		n_channels = MAX_CHANNEL_NUM_5G;
		n_bitrates = RTW_A_RATES_NUM;
	}

	band = (struct ieee80211_supported_band *)kzalloc(
			   sizeof(struct ieee80211_supported_band)
			   + sizeof(struct ieee80211_channel) * n_channels
			   + sizeof(struct ieee80211_rate) * n_bitrates
			   + sizeof(struct ieee80211_sband_iftype_data) * 2,
			   in_interrupt() ? GFP_ATOMIC : GFP_KERNEL
		   );

	if (!band) {
		dev_err(global_idev.fullmac_dev, "init wiphy band failed\n");
		return false;
	}

	band->channels = (struct ieee80211_channel *)(((u8 *)band) + sizeof(struct ieee80211_supported_band));
	band->bitrates = (struct ieee80211_rate *)(((u8 *)band->channels) + sizeof(struct ieee80211_channel) * n_channels);
	band->band = NL80211_BAND_2GHZ;
	band->n_channels = n_channels;
	band->n_bitrates = n_bitrates;
	band->iftype_data = (struct ieee80211_sband_iftype_data *)(((u8 *)band->bitrates)
						+ sizeof(struct ieee80211_rate) * n_bitrates);
	band->n_iftype_data = 0;

	if (band_type == NL80211_BAND_2GHZ) {
		memcpy((void *)band->channels, (void *)rtw_2ghz_channels, sizeof(rtw_2ghz_channels));
		memcpy(band->bitrates, rtw_g_rates, sizeof(struct ieee80211_rate) * RTW_G_RATES_NUM);
	} else if (band_type == NL80211_BAND_5GHZ) {
		memcpy((void *)band->channels, (void *)rtw_5ghz_a_channels, sizeof(rtw_5ghz_a_channels));
		memcpy(band->bitrates, rtw_a_rates, sizeof(struct ieee80211_rate) * RTW_A_RATES_NUM);
	}

	pwiphy->bands[band_type] = band;
	return true;

}

int rtw_wiphy_init_params(struct wiphy *pwiphy)
{
	int ret = true;

	pwiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;
	pwiphy->max_scan_ssids = RTW_SSID_SCAN_AMOUNT;
	pwiphy->max_scan_ie_len = RTW_SCAN_IE_LEN_MAX;
	pwiphy->max_num_pmkids = RTW_MAX_NUM_PMKIDS;
	pwiphy->interface_modes = BIT(NL80211_IFTYPE_STATION)
							  | BIT(NL80211_IFTYPE_AP)
#ifdef CONFIG_NAN
							  | BIT(NL80211_IFTYPE_NAN)
#endif
#ifdef CONFIG_P2P
							  | BIT(NL80211_IFTYPE_P2P_CLIENT)
							  | BIT(NL80211_IFTYPE_P2P_GO)
							  | BIT(NL80211_IFTYPE_P2P_DEVICE)
#endif
							  ;
	pwiphy->cipher_suites = rtw_cipher_suites;
	pwiphy->n_cipher_suites = sizeof(rtw_cipher_suites) / sizeof((rtw_cipher_suites)[0]);
	pwiphy->features |= NL80211_FEATURE_SAE;
	pwiphy->support_mbssid = true;
	pwiphy->support_only_he_mbssid = false;
	pwiphy->max_num_csa_counters = 5;

	wiphy_ext_feature_set(pwiphy, NL80211_EXT_FEATURE_MFP_OPTIONAL);

#ifdef CONFIG_PM
	pwiphy->wowlan = &rtw_wowlan_stub;
#endif

#ifdef CONFIG_NAN
	pwiphy->nan_supported_bands |= BIT(NL80211_BAND_2GHZ);
	pwiphy->nan_supported_bands |= BIT(NL80211_BAND_5GHZ);
#endif
#ifdef CONFIG_P2P
	pwiphy->iface_combinations = rtw_combinations;
	pwiphy->n_iface_combinations = ARRAY_SIZE(rtw_combinations);
	pwiphy->flags |= WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL;
	pwiphy->max_remain_on_channel_duration = RTW_MAX_REMAIN_ON_CHANNEL_DURATION;
#endif

	/* Support for AP mode. */
	pwiphy->flags |= (WIPHY_FLAG_HAVE_AP_SME | WIPHY_FLAG_HAS_CHANNEL_SWITCH);
	pwiphy->mgmt_stypes = rtw_cfg80211_default_mgmt_stypes;

	ret = rtw_wiphy_band_init(pwiphy, NL80211_BAND_2GHZ);
	if (ret == false) {
		return ret;
	}
	ret = rtw_wiphy_band_init(pwiphy, NL80211_BAND_5GHZ);
	if (ret == false) {
		return ret;
	}

	//HT cap-2.4G
	pwiphy->bands[NL80211_BAND_2GHZ]->ht_cap.ht_supported = 1;
	pwiphy->bands[NL80211_BAND_2GHZ]->ht_cap.cap = IEEE80211_HT_CAP_SUP_WIDTH_20_40 |
			IEEE80211_HT_CAP_SGI_40 | IEEE80211_HT_CAP_SGI_20 |
			IEEE80211_HT_CAP_DSSSCCK40 | IEEE80211_HT_CAP_MAX_AMSDU;
	//pwiphy->bands[NL80211_BAND_2GHZ]->ht_cap->cap |= IEEE80211_HT_CAP_RX_STBC_1R;
	pwiphy->bands[NL80211_BAND_2GHZ]->ht_cap.ampdu_factor = IEEE80211_HT_MAX_AMPDU_16K;
	pwiphy->bands[NL80211_BAND_2GHZ]->ht_cap.ampdu_density = IEEE80211_HT_MPDU_DENSITY_16;
	pwiphy->bands[NL80211_BAND_2GHZ]->ht_cap.mcs.tx_params = IEEE80211_HT_MCS_TX_DEFINED;
	pwiphy->bands[NL80211_BAND_2GHZ]->ht_cap.mcs.rx_mask[0] = 0xFF;
	pwiphy->bands[NL80211_BAND_2GHZ]->ht_cap.mcs.rx_highest = 72;

	//HT cap-5G
	pwiphy->bands[NL80211_BAND_5GHZ]->ht_cap.ht_supported = 1;
	pwiphy->bands[NL80211_BAND_5GHZ]->ht_cap.cap = IEEE80211_HT_CAP_SUP_WIDTH_20_40 |
			IEEE80211_HT_CAP_SGI_40 | IEEE80211_HT_CAP_SGI_20 |
			IEEE80211_HT_CAP_DSSSCCK40 | IEEE80211_HT_CAP_MAX_AMSDU;
	//pwiphy->bands[NL80211_BAND_5GHZ]->ht_cap->cap |= IEEE80211_HT_CAP_RX_STBC_1R;
	pwiphy->bands[NL80211_BAND_5GHZ]->ht_cap.ampdu_factor = IEEE80211_HT_MAX_AMPDU_16K;
	pwiphy->bands[NL80211_BAND_5GHZ]->ht_cap.ampdu_density = IEEE80211_HT_MPDU_DENSITY_16;
	pwiphy->bands[NL80211_BAND_5GHZ]->ht_cap.mcs.tx_params = IEEE80211_HT_MCS_TX_DEFINED;
	pwiphy->bands[NL80211_BAND_5GHZ]->ht_cap.mcs.rx_mask[0] = 0xFF;
	pwiphy->bands[NL80211_BAND_5GHZ]->ht_cap.mcs.rx_highest = 72;

	return ret;
}

int rtw_wiphy_init(void)
{
	int ret = false;
	struct wiphy *pwiphy = NULL;

	dev_dbg(global_idev.fullmac_dev, "--- %s ---", __func__);

	cfg80211_rtw_ops_sta_init();
	cfg80211_rtw_ops_ap_init();
	cfg80211_rtw_ops_key_init();
#ifdef CONFIG_NAN
	cfg80211_rtw_ops_nan_init();
#endif
#ifdef CONFIG_P2P
	cfg80211_rtw_ops_p2p_init();
#endif
	pwiphy = wiphy_new(&global_idev.rtw_cfg80211_ops, 0);
	if (!pwiphy) {
		return ret;
	}
	global_idev.pwiphy_global = pwiphy;
	set_wiphy_dev(pwiphy, global_idev.fullmac_dev);
#ifdef CONFIG_NAN
	rtw_cfgvendor_attach(global_idev.pwiphy_global);
#endif

	ret = rtw_wiphy_init_params(pwiphy);
	if (ret != true) {
		goto wiphy_fail;
	}

	return true;

wiphy_fail:
	if (pwiphy->bands[NL80211_BAND_2GHZ]) {
		kfree(pwiphy->bands[NL80211_BAND_2GHZ]);
		pwiphy->bands[NL80211_BAND_2GHZ] = NULL;
	}
	if (pwiphy->bands[NL80211_BAND_5GHZ]) {
		kfree(pwiphy->bands[NL80211_BAND_5GHZ]);
		pwiphy->bands[NL80211_BAND_5GHZ] = NULL;
	}
	wiphy_free(pwiphy);
	global_idev.pwiphy_global = NULL;

	return ret;
}

void rtw_wiphy_deinit(void)
{
#ifdef CONFIG_NAN
	rtw_cfgvendor_detach(global_idev.pwiphy_global);
#endif

	if (global_idev.pwiphy_global->bands[NL80211_BAND_2GHZ]) {
		kfree(global_idev.pwiphy_global->bands[NL80211_BAND_2GHZ]);
		global_idev.pwiphy_global->bands[NL80211_BAND_2GHZ] = NULL;
	}
	if (global_idev.pwiphy_global->bands[NL80211_BAND_5GHZ]) {
		kfree(global_idev.pwiphy_global->bands[NL80211_BAND_5GHZ]);
		global_idev.pwiphy_global->bands[NL80211_BAND_5GHZ] = NULL;
	}

	if (global_idev.pwiphy_global) {
		wiphy_free(global_idev.pwiphy_global);
		dev_dbg(global_idev.fullmac_dev, "free wiphy done.");
		global_idev.pwiphy_global = NULL;
	}
}
