/*
* Realtek wireless local area network IC driver.
*   Implemented interface : ops->dump_survey
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2024, Realtek Corporation. All rights reserved.
*/

#include "rtw_cfg80211_fullmac.h"

extern const struct ieee80211_channel rtw_2ghz_channels[MAX_CHANNEL_NUM_2G];
extern const struct ieee80211_channel rtw_5ghz_a_channels[MAX_CHANNEL_NUM_5G];
u8 chanel_idx_max = MAX_CHANNEL_NUM - 1;

u8 rtw_chnl_tbl[MAX_CHANNEL_NUM] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	36, 40, 44, 48, 52, 56, 60, 64,
	100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144,
	149, 153, 157, 161, 165, 169, 173, 177
};

struct acs_mntr_rpt acs_mntr_rpt_tbl[MAX_CHANNEL_NUM] = {0};

int cfg80211_rtw_dump_survey_params(struct wiphy *wiphy, struct net_device *netdev, int idx, struct survey_info *info)
{
	if (!global_idev.wifi_user_config.acs_en) {
		return -ENOENT;
	}

	if (global_idev.mp_fw) {
		return -ENOENT;
	}

	if (rtw_netdev_idx(netdev) == 0) {
		return -ENOENT;
	}

	if (!info) {
		dev_err(global_idev.fullmac_dev, "%s: invial parameters!\n", __func__);
		return -ENOENT;
	}

	if (idx > chanel_idx_max) {
		return -ENOENT;
	}

	memset(info, 0, sizeof(struct survey_info));

	if (acs_mntr_rpt_tbl[idx].channel != 0) {

		info->channel = kmalloc(sizeof(struct ieee80211_channel), GFP_KERNEL);
		if (idx < MAX_CHANNEL_NUM_2G) {
			memcpy(info->channel, &rtw_2ghz_channels[idx], sizeof(struct ieee80211_channel));
		} else {
			memcpy(info->channel, &rtw_5ghz_a_channels[idx - MAX_CHANNEL_NUM_2G], sizeof(struct ieee80211_channel));
		}

		info->time = (u64)acs_mntr_rpt_tbl[idx].meas_time;
		info->time_busy = (u64)acs_mntr_rpt_tbl[idx].busy_time;
		info->time_tx = (u64)acs_mntr_rpt_tbl[idx].tx_time;
		info->noise = (u64)acs_mntr_rpt_tbl[idx].noise;
		info->filled = SURVEY_INFO_TIME | SURVEY_INFO_TIME_BUSY | SURVEY_INFO_TIME_TX | SURVEY_INFO_NOISE_DBM;
	} else {
		info->channel = NULL;
		dev_err(global_idev.fullmac_dev, "%s: idx：%d, acs_info == NULL\n", __func__,idx);
	}

	return 0;
}