// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <whc_host_linux.h>
#ifdef CONFIG_NAN
void whc_fullmac_host_nan_func_filter(u8 num,
									  struct cfg80211_nan_func_filter *nan_func_filter)
{
	u8 i;
	printk("=======================\n");
	printk("whc_fullmac_host_nan_func_filter:\n");
	for (i = 0; i < num; i++) {
		printk("filer %d:\n", i);
		rtw_dump_buf("filter content:", nan_func_filter[i].filter, nan_func_filter[i].len);
	}
	printk("=======================\n");
}

void whc_fullmac_host_dump_nan_func(struct cfg80211_nan_func *func)
{
	printk("=======================\n");
	printk("%s:\n", __func__);
	switch (func->type) {
	case NL80211_NAN_FUNC_PUBLISH:
		printk("type: publish\n");
		if (func->publish_type & NL80211_NAN_SOLICITED_PUBLISH) {
			printk("publish_type: solicited\n");
			printk("publish_bcast: %s\n", (func->publish_bcast) ? "true" : "false");
		}
		if (func->publish_type & NL80211_NAN_UNSOLICITED_PUBLISH) {
			printk("publish_type: unsolicited\n");
		}
		break;
	case NL80211_NAN_FUNC_SUBSCRIBE:
		printk("type: subscribe\n");
		printk("subscribe_active: %s\n", (func->subscribe_active) ? "true" : "false");
		break;
	case NL80211_NAN_FUNC_FOLLOW_UP:
		printk("type: follow up\n");
		printk("followup_id: %d\n", func->followup_id);
		printk("followup_reqid: %d\n", func->followup_reqid);
		break;
	default:
		printk("type: unknown\n");

	}
	rtw_dump_buf("service_id: ", func->service_id, NL80211_NAN_FUNC_SERVICE_ID_LEN);
	printk("close_range: %s\n", (func->close_range) ? "true" : "false");

	if (func->type == NL80211_NAN_FUNC_FOLLOW_UP) {
		goto end;
	}

	printk("ttl: %d\n", func->ttl);
	if (func->serv_spec_info_len > 0) {
		rtw_dump_buf("serv_spec_info: ", func->serv_spec_info, func->serv_spec_info_len);
	}
	printk("srf_include: %s\n", (func->srf_include) ? "true" : "false");
	if (func->srf_bf_len > 0) {
		rtw_dump_buf("srf_bf: ", func->srf_bf, func->srf_bf_len);
		printk("srf_bf_idx: %d\n", func->srf_bf_idx);
	}
	if (func->srf_num_macs > 0) {
		printk("srf_num_macs: %d\n", func->srf_num_macs);
	}
	if (func->num_rx_filters > 0) {
		printk("num_rx_filters: %d\n", func->num_rx_filters);
		whc_fullmac_host_nan_func_filter(func->num_rx_filters, func->rx_filters);
	}
	if (func->num_tx_filters > 0) {
		printk("num_tx_filters: %d\n", func->num_tx_filters);
		whc_fullmac_host_nan_func_filter(func->num_tx_filters, func->tx_filters);
	}

end:
	printk("instance_id: %d\n", func->instance_id);
	printk("cookie: %llu\n", func->cookie);
	printk("=======================\n");
}


static int whc_fullmac_host_start_nan(struct wiphy *wiphy,
									  struct wireless_dev *wdev,
									  struct cfg80211_nan_conf *conf)
{
	int ret = 0;
	u8 band_support;

	printk(" => %s: master_pref = %d, bands = %d\n", __func__, conf->master_pref, conf->bands);

	/* prepare neccessary parameters */
	band_support = ((conf->bands & BIT(NL80211_BAND_2GHZ)) ? RTW_SUPPORT_BAND_2_4G : 0)
				   | ((conf->bands & BIT(NL80211_BAND_5GHZ)) ? RTW_SUPPORT_BAND_5G : 0);

	if (whc_fullmac_host_start_nan(conf->master_pref, band_support) == -1) {
		ret = -ENOTCONN;
	}

	return ret;
}

void whc_fullmac_host_stop_nan(struct wiphy *wiphy, struct wireless_dev *wdev)
{
	printk(" => %s\n", __func__);

	whc_fullmac_host_stop_nan();
}

static int whc_fullmac_host_add_nan_func(struct wiphy *wiphy,
		struct wireless_dev *wdev,
		struct cfg80211_nan_func *func)
{
	int ret = 0;
	struct rtw_nan_func_info_t nan_param = {0};
	u8 *serv_spec_info_vir = NULL, *srf_bf_vir = NULL, *srf_macs_vir = NULL;
	dma_addr_t serv_spec_info_phy, srf_bf_phy, srf_macs_phy;
	u8 *rx_filters_vir = NULL, *tx_filters_vir = NULL;
	dma_addr_t rx_filters_phy, tx_filters_phy;

	printk(" => %s\n", __func__);

	memcpy(&nan_param, func, sizeof(struct rtw_nan_func_info_t));

	if (func->serv_spec_info) {
		serv_spec_info_vir = rtw_malloc(func->serv_spec_info_len, &serv_spec_info_phy);
		if (!serv_spec_info_vir) {
			dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
			return -ENOMEM;
			goto exit;
		}
		memcpy(serv_spec_info_vir, func->serv_spec_info, func->serv_spec_info_len);
		nan_param.serv_spec_info = (const u8 *)serv_spec_info_phy;
	}

	if (func->srf_bf) {
		srf_bf_vir = rtw_malloc(func->srf_bf_len, &srf_bf_phy);
		if (!srf_bf_vir) {
			dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
			ret = -ENOMEM;
			goto exit;
		}
		memcpy(srf_bf_vir, func->srf_bf, func->srf_bf_len);
		nan_param.srf_bf = (const u8 *)srf_bf_phy;
	}

	if (func->srf_macs) {
		srf_macs_vir = rtw_malloc(func->srf_num_macs * sizeof(struct mac_address), &srf_macs_phy);
		if (!srf_macs_vir) {
			dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
			ret = -ENOMEM;
			goto exit;
		}
		memcpy(srf_macs_vir, func->srf_macs, func->srf_num_macs * sizeof(struct mac_address));
		nan_param.srf_macs = (struct mac_address *)srf_macs_phy;
	}

	if (func->rx_filters) {
		rx_filters_vir = rtw_malloc(func->num_rx_filters * sizeof(struct cfg80211_nan_func_filter), &rx_filters_phy);
		if (!rx_filters_vir) {
			dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
			ret = -ENOMEM;
			goto exit;
		}
		memcpy(rx_filters_vir, func->rx_filters, func->num_rx_filters * sizeof(struct cfg80211_nan_func_filter));
		nan_param.rx_filters = (struct cfg80211_nan_func_filter *)rx_filters_phy;
	}

	if (func->tx_filters) {
		tx_filters_vir = rtw_malloc(func->num_tx_filters * sizeof(struct cfg80211_nan_func_filter), &tx_filters_phy);
		if (!tx_filters_vir) {
			dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
			ret = -ENOMEM;
			goto exit;
		}
		memcpy(tx_filters_vir, func->tx_filters, func->num_tx_filters * sizeof(struct cfg80211_nan_func_filter));
		nan_param.tx_filters = (struct cfg80211_nan_func_filter *)tx_filters_phy;
	}

	func->instance_id = whc_fullmac_host_add_nan_func(&nan_param, func);

	whc_fullmac_host_dump_nan_func(func);

exit:
	if (serv_spec_info_vir) {
		rtw_mfree(func->serv_spec_info_len, serv_spec_info_vir, serv_spec_info_phy);
	}
	if (srf_bf_vir) {
		rtw_mfree(func->srf_bf_len, srf_bf_vir, srf_bf_phy);
	}
	if (srf_macs_vir) {
		rtw_mfree(func->srf_num_macs, srf_macs_vir, srf_macs_phy);
	}
	if (rx_filters_vir) {
		rtw_mfree(func->num_rx_filters, rx_filters_vir, rx_filters_phy);
	}
	if (tx_filters_vir) {
		rtw_mfree(func->num_tx_filters, tx_filters_vir, tx_filters_phy);
	}

	return ret;
}

void whc_fullmac_host_del_nan_func(struct wiphy *wiphy,
								   struct wireless_dev *wdev, u64 cookie)
{
	printk("%s =>\n", __func__);

	whc_fullmac_host_del_nan_func(cookie);
}

static int whc_fullmac_host_nan_change_conf(struct wiphy *wiphy,
		struct wireless_dev *wdev,
		struct cfg80211_nan_conf *conf,
		u32 changes)
{
	int ret = 0;

	printk(" => %s: master_pref = %d, bands = %d changes=%d\n", __func__, conf->master_pref, conf->bands, changes);

	//rtw_update_nan_conf(conf,changes);

	return ret;
}

void whc_fullmac_host_nan_handle_sdf(u8 type, u8 inst_id, u8 peer_inst_id, u8 *addr, u32 info_len, u8 *info, u64 cookie)
{
	struct cfg80211_nan_match_params match_param;

	gfp_t kflags;

	kflags = in_atomic() ? GFP_ATOMIC : GFP_KERNEL;
	memset(&match_param, 0, sizeof(match_param));
	match_param.type = type;
	match_param.inst_id = inst_id;
	match_param.peer_inst_id = peer_inst_id;
	match_param.addr = addr;
	match_param.info_len = info_len;
	match_param.info = info;
	match_param.cookie = cookie;
	cfg80211_nan_match(global_idev.pwdev_global[1], &match_param, kflags);

}

void whc_fullmac_host_nan_func_free(u64 os_dep_data)
{
	struct wireless_dev *wdev = global_idev.pwdev_global[2];
	gfp_t kflags;
	struct cfg80211_nan_func *func = (struct cfg80211_nan_func *)os_dep_data;

	if (wdev == NULL) {
		return;
	}

	printk("%s => os_dep_data=%llx\n", __func__, os_dep_data);

	kflags = in_atomic() ? GFP_ATOMIC : GFP_KERNEL;

	if (func != NULL) {
		cfg80211_nan_func_terminated(wdev, func->instance_id, NL80211_NAN_FUNC_TERM_REASON_USER_REQUEST, func->cookie, kflags);
		cfg80211_free_nan_func(func);
	}
}

void whc_fullmac_host_nan_init(void)
{
	struct cfg80211_ops *ops = &global_idev.rtw_cfg80211_ops;
	ops->start_nan = whc_fullmac_host_start_nan;
	ops->stop_nan = whc_fullmac_host_stop_nan;
	ops->add_nan_func = whc_fullmac_host_add_nan_func;
	ops->del_nan_func = whc_fullmac_host_del_nan_func;
	ops->nan_change_conf = whc_fullmac_host_nan_change_conf;
}
#endif
