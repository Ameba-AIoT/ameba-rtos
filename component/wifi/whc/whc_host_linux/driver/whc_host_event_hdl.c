// SPDX-License-Identifier: GPL-2.0-only
/*
 * Realtek wireless local area network IC driver.
 *   Shared WiFi event handler called by all transport paths after parameter
 *   extraction.  Each transport (IPC / SDIO / SPI / USB) decodes its own
 *   message format then calls whc_host_join_status_hdl() with plain values.
 *
 * Copyright (C) 2023, Realtek Corporation. All rights reserved.
 */

#include <whc_host_linux.h>

const struct whc_event_entry whc_host_event_table[] = {
	{ WHC_API_SCAN_USER_CALLBACK,				whc_host_scan_user_callback_hdl },
	{ WHC_API_IP_ACS,							whc_host_event_set_acs_info },
	{ WHC_API_WIFI_EVENT,						whc_host_event_join_status_indicate },
#ifdef CONFIG_WHC_HCI_IPC
	{ WHC_API_PROMISC_CALLBACK, 				whc_host_event_promisc_pkt_hdl },
#else
	{ WHC_API_PROMISC_CALLBACK, 				NULL },
#endif
	{ WHC_API_GET_LWIP_INFO,					whc_host_event_get_network_info },
	{ WHC_API_SET_NETIF_INFO,					whc_host_event_set_netif_info },
	{ WHC_API_CFG80211_SCAN_REPORT, 			whc_host_event_scan_report_indicate },
	{ WHC_API_UPDATE_REGD_EVENT,				whc_host_event_update_regd_indicate },
	{ WHC_API_AP_CH_SWITCH,						NULL},
	{ WHC_API_SCAN_EACH_REPORT_USER_CALLBACK,	NULL},
#ifdef CONFIG_NAN
	{ WHC_API_CFG80211_NAN_REPORT_MATCH_EVENT,	whc_host_event_nan_match_indicate },
	{ WHC_API_CFG80211_NAN_DEL_FUNC,			whc_host_nan_func_free },
	{ WHC_API_CFG80211_NAN_CFGVENDOR_EVENT, 	whc_host_event_nan_cfgvendor_event_indicate },
	{ WHC_API_CFG80211_NAN_CFGVENDOR_CMD_REPLY, whc_host_event_nan_cfgvendor_cmd_reply },
#endif
	{ 0, NULL },
};

u8 whc_host_internal_event_handle(u32 api_id, u32 *param_buf)
{
	u32 i;

	for (i = 0; whc_host_event_table[i].api_id != 0; i++) {
		if (whc_host_event_table[i].api_id == api_id) {
			if (whc_host_event_table[i].handler) {
				return whc_host_event_table[i].handler(param_buf);
			} else {
				return 0;
			}
		}
	}

	dev_err(global_idev.pwhc_dev, "%s: Unknown Device event(%x)!\n\r", __func__, api_id);
	return 0;
}

void whc_host_join_status_hdl(u32 event, char *evt_info)
{
	struct mlme_priv_t *mlme_priv = &global_idev.mlme_priv;
	u16 disassoc_reason;
	int channel = 6; /*channel need get, force 6 seems ok temporary*/
	struct wireless_dev *wdev = global_idev.pwdev_global[0];
#ifdef CONFIG_SUPPLICANT_SME
	struct sme_priv_t *sme_priv = &global_idev.sme_priv;
	struct rtw_event_sme_auth_timeout *auth_timeout_info;
	struct rtw_event_sme_rx_assoc_resp *rx_assoc_rsp_info;
	struct net_device *pndev = global_idev.pndev[0];
	struct rtw_event_report_frame *evt_rpt_frm;
#endif
	struct rtw_event_ap_sta_disassoc *sta_disassoc_info;
	struct rtw_event_join_status_info *join_status_info;
	struct rtw_event_disconnect *disconnect;
	unsigned int join_status = 0;
	struct rtw_event_rx_mgnt *rx_mgnt_info;
	struct rtw_event_ap_sta_assoc *sta_assoc_info;
#ifdef CONFIG_P2P
	u16 frame_type;
#endif
	u8 wlan_idx;
	struct rtw_wpa_4way_status rpt_4way = {0};

	if (event == RTW_EVENT_JOIN_STATUS) {
		join_status_info = (struct rtw_event_join_status_info *)evt_info;
		join_status = join_status_info->status;
		whc_host_connect_indicate(join_status, evt_info);
	}

	if ((event == RTW_EVENT_JOIN_STATUS) && ((join_status == RTW_JOINSTATUS_FAIL) || (join_status == RTW_JOINSTATUS_DISCONNECT))) {
		if (join_status == RTW_JOINSTATUS_DISCONNECT) {
			disconnect = &join_status_info->priv.disconnect;
			disassoc_reason = (u16)(disconnect->disconn_reason & 0xffff);
			dev_dbg(global_idev.pwhc_dev, "%s: disassoc_reason=%d \n", __func__, disassoc_reason);
			whc_host_disconnect_indicate(disassoc_reason, 1, join_status_info->bssid);
		}
		if (mlme_priv->b_in_disconnect) {
			complete(&mlme_priv->disconnect_done_sema);
			mlme_priv->b_in_disconnect = false;
		}
	}

	if (event == RTW_EVENT_AP_STA_ASSOC) {
		dev_dbg(global_idev.pwhc_dev, "%s: sta assoc \n", __func__);
		sta_assoc_info = (struct rtw_event_ap_sta_assoc *)evt_info;
		whc_host_sta_assoc_indicate(sta_assoc_info->frame, sta_assoc_info->frame_len);
	}

	if (event == RTW_EVENT_AP_STA_DISASSOC) {
		dev_dbg(global_idev.pwhc_dev, "%s: sta disassoc \n", __func__);

		wlan_idx = 1;
		dev_dbg(global_idev.pwhc_dev, "[whc]: wlan_idx = %d, is_need_4way= %d, is_4way_ongoing =%d", wlan_idx,
				global_idev.is_need_4way[wlan_idx], global_idev.is_4way_ongoing[wlan_idx]);
		if (global_idev.is_need_4way[wlan_idx] && global_idev.is_4way_ongoing[wlan_idx] > 0) {
			global_idev.is_4way_ongoing[wlan_idx]--;
			if (0 == global_idev.is_4way_ongoing[wlan_idx]) {
				rpt_4way.is_start = false;
				rpt_4way.is_success = true;
				rpt_4way.wlan_idx = wlan_idx;
				whc_host_wpa_4way_status_indicate(&rpt_4way);
				dev_dbg(global_idev.pwhc_dev, "llhw indicate 4-way end\n");
			}
		}

		sta_disassoc_info = (struct rtw_event_ap_sta_disassoc *)evt_info;
		cfg80211_del_sta(global_idev.pndev[1], sta_disassoc_info->sta_mac, GFP_ATOMIC);
	}

	if (event == RTW_EVENT_EXTERNAL_AUTH_REQ) {
		dev_dbg(global_idev.pwhc_dev, "%s: auth req \n", __func__);
		whc_host_external_auth_request(evt_info);
	}

	if (event == RTW_EVENT_RX_MGNT) {
		rx_mgnt_info = (struct rtw_event_rx_mgnt *)evt_info;
#ifdef CONFIG_P2P
		channel = rx_mgnt_info->channel;
		frame_type = rx_mgnt_info->frame_type;
		if (frame_type == IEEE80211_STYPE_PROBE_REQ) {
			if (global_idev.p2p_global.pd_pwdev && (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_PROBE_REQ))) {
				wdev = global_idev.p2p_global.pd_pwdev;
			}
		} else if (frame_type == IEEE80211_STYPE_ACTION) {
			if (global_idev.p2p_global.pd_pwdev && memcmp((rx_mgnt_info->frame + 4), global_idev.p2p_global.pd_pwdev->address, 6) == 0) {
				if (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_ACTION)) {
					wdev = global_idev.p2p_global.pd_pwdev; /*DA match P2P_DEV intf*/
				}
			} else if (rtw_p2p_frame_is_registered(P2P_ROLE_CLIENT, IEEE80211_STYPE_ACTION)) {
				wdev = global_idev.pwdev_global[1];
			}
		} else {
			if (global_idev.p2p_global.pd_pwdev && memcmp((rx_mgnt_info->frame + 4), global_idev.p2p_global.pd_pwdev->address, 6) == 0) {
				wdev = global_idev.p2p_global.pd_pwdev;
			} else if (global_idev.p2p_global.p2p_role == P2P_ROLE_CLIENT) { /* port0 is used by GC */
				wdev = global_idev.pwdev_global[1];
			} else {
				wdev = global_idev.pwdev_global[0]; /* port0 is used by STA */
			}
		}
#endif
		dev_dbg(global_idev.pwhc_dev, "%s: rx mgnt \n", __func__);
		/*channel need get, force 6 seems ok temporary*/
		cfg80211_rx_mgmt(wdev, rtw_ch2freq(channel), 0, rx_mgnt_info->frame, rx_mgnt_info->frame_len, 0);
	}

#ifdef CONFIG_IEEE80211R
	if ((event == RTW_EVENT_FT_AUTH_START) || (event == RTW_EVENT_FT_RX_MGNT) || (event == RTW_EVENT_JOIN_STATUS)) {
		whc_host_ft_event(event, evt_info, join_status);
	}
#endif

	if (event == RTW_EVENT_RX_MGNT_AP) {
		wdev = ndev_to_wdev(global_idev.pndev[1]);
		rx_mgnt_info = (struct rtw_event_rx_mgnt *)evt_info;
#ifdef CONFIG_P2P
		channel = rx_mgnt_info->channel;
		frame_type = rx_mgnt_info->frame_type;
		if (frame_type == IEEE80211_STYPE_PROBE_REQ) {
			if (global_idev.p2p_global.pd_pwdev && (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_PROBE_REQ))) {
				cfg80211_rx_mgmt(global_idev.p2p_global.pd_pwdev, rtw_ch2freq(channel), 0, rx_mgnt_info->frame, rx_mgnt_info->frame_len, 0);
			}
			if (rtw_p2p_frame_is_registered(P2P_ROLE_GO, IEEE80211_STYPE_PROBE_REQ)) {
				cfg80211_rx_mgmt(wdev, rtw_ch2freq(channel), 0, rx_mgnt_info->frame, rx_mgnt_info->frame_len, 0);
			}
			return;
		} else if ((frame_type == IEEE80211_STYPE_ACTION) && (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_ACTION))) {
			if (global_idev.p2p_global.pd_pwdev && (memcmp((rx_mgnt_info->frame + 4), global_idev.p2p_global.pd_pwdev->address, 6)) == 0) {
				wdev = global_idev.p2p_global.pd_pwdev; /*DA match P2P_DEV intf*/
			}
		}
#endif
		dev_dbg(global_idev.pwhc_dev, "%s: rx mgnt \n", __func__);
		cfg80211_rx_mgmt(wdev, rtw_ch2freq(channel), 0, rx_mgnt_info->frame, rx_mgnt_info->frame_len, 0);
	}

	if (event == RTW_EVENT_OWE_PEER_KEY_RECV) {
		dev_dbg(global_idev.pwhc_dev, "%s: owe update \n", __func__);
		whc_host_update_owe_info_event(evt_info);
	}

#ifdef CONFIG_SUPPLICANT_SME
#ifdef CONFIG_P2P
	if (global_idev.p2p_global.p2p_role == P2P_ROLE_CLIENT) {
		pndev = global_idev.pndev[1];	/* GC connect using SME */
	} else {
		pndev = global_idev.pndev[0];	/* STA connect using SME */
	}
#endif
	if (event == RTW_EVENT_SME_AUTH_TIMEOUT) {
		dev_dbg(global_idev.pwhc_dev, "%s: RTW_EVENT_SME_AUTH_TIMEOUT \n", __func__);
		auth_timeout_info = (struct rtw_event_sme_auth_timeout *)evt_info;
		cfg80211_auth_timeout(pndev, auth_timeout_info->bssid);

	} else if (event == RTW_EVENT_SME_ASSOC_TIMEOUT) {
		dev_dbg(global_idev.pwhc_dev, "%s: RTW_EVENT_SME_ASSOC_TIMEOUT \n", __func__);

		if (sme_priv->cfg80211_assoc_bss) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 0, 0))
			struct cfg80211_assoc_failure *passocfail_data = (struct cfg80211_assoc_failure *)kzalloc(sizeof(struct cfg80211_assoc_failure), GFP_KERNEL);
			passocfail_data->bss[0] = sme_priv->cfg80211_assoc_bss;
			passocfail_data->timeout = 1;

			cfg80211_assoc_failure(pndev, passocfail_data);
			kfree(passocfail_data);
#else
			cfg80211_assoc_timeout(pndev, sme_priv->cfg80211_assoc_bss);
#endif
			sme_priv->cfg80211_assoc_bss = NULL;
		} else {
			dev_dbg(global_idev.pwhc_dev, "%s(): association timeout but no corresponding bss recorded\n", __FUNCTION__);
		}

	} else if (event == RTW_EVENT_SME_RX_MLME_MGNT) {
		dev_dbg(global_idev.pwhc_dev, "%s: RTW_EVENT_SME_RX_MLME_MGNT\n", __func__);
		evt_rpt_frm = (struct rtw_event_report_frame *)evt_info;
		cfg80211_rx_mlme_mgmt(pndev, evt_rpt_frm->frame, evt_rpt_frm->frame_len);

	} else if (event == RTW_EVENT_SME_TX_MLME_MGNT) {
		dev_dbg(global_idev.pwhc_dev, "%s: RTW_EVENT_SME_TX_MLME_MGNT \n", __func__);
		evt_rpt_frm = (struct rtw_event_report_frame *)evt_info;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 11, 0))
		cfg80211_tx_mlme_mgmt(pndev, evt_rpt_frm->frame, evt_rpt_frm->frame_len, false);
#else
		cfg80211_tx_mlme_mgmt(pndev, evt_rpt_frm->frame, evt_rpt_frm->frame_len);
#endif
	} else if (event == RTW_EVENT_SME_RX_ASSOC_RESP) {
		dev_dbg(global_idev.pwhc_dev, "%s: RTW_EVENT_SME_RX_ASSOC_RESP \n", __func__);

		if (sme_priv->cfg80211_assoc_bss) {
			rx_assoc_rsp_info = (struct rtw_event_sme_rx_assoc_resp *)evt_info;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 0, 0))
			struct cfg80211_rx_assoc_resp *passocrsp_data = (struct cfg80211_rx_assoc_resp *)kzalloc(sizeof(struct cfg80211_rx_assoc_resp), GFP_KERNEL);

			passocrsp_data->buf = rx_assoc_rsp_info->frame;
			passocrsp_data->len = rx_assoc_rsp_info->frame_len;
			passocrsp_data->req_ies = mlme_priv->assoc_req_ie + WLAN_HDR_A3_LEN + 4 + (*(u8 *)mlme_priv->assoc_req_ie == 0 ? 0 : 6);/* re-assoc: current ap */
			passocrsp_data->req_ies_len = mlme_priv->assoc_req_ie_len - WLAN_HDR_A3_LEN - 4 - (*(u8 *)mlme_priv->assoc_req_ie == 0 ? 0 : 6);
			passocrsp_data->uapsd_queues = rx_assoc_rsp_info->uapsd_ac_enable;
			passocrsp_data->links[0].bss = sme_priv->cfg80211_assoc_bss;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 2, 0))
			passocrsp_data->links[0].status = *(u16 *)(passocrsp_data->buf + WLAN_HDR_A3_LEN + 2);
#endif
			passocrsp_data->links[0].addr = mlme_priv->assoc_req_ie + 4;

			cfg80211_rx_assoc_resp(pndev, passocrsp_data);

			kfree(passocrsp_data);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
			cfg80211_rx_assoc_resp(pndev, sme_priv->cfg80211_assoc_bss, rx_assoc_rsp_info->frame, rx_assoc_rsp_info->frame_len, rx_assoc_rsp_info->uapsd_ac_enable,
								   mlme_priv->assoc_req_ie + WLAN_HDR_A3_LEN + 4 + (*(u8 *)mlme_priv->assoc_req_ie == 0 ? 0 : 6),
								   mlme_priv->assoc_req_ie_len - WLAN_HDR_A3_LEN - 4 - (*(u8 *)mlme_priv->assoc_req_ie == 0 ? 0 : 6));
#else
			cfg80211_rx_assoc_resp(pndev, sme_priv->cfg80211_assoc_bss, rx_assoc_rsp_info->frame, rx_assoc_rsp_info->frame_len, rx_assoc_rsp_info->uapsd_ac_enable);
#endif
			sme_priv->cfg80211_assoc_bss = NULL;
		} else {
			dev_dbg(global_idev.pwhc_dev, "%s(): RX assocrsp but no corresponding bss recorded\n", __FUNCTION__);
		}
	} else if (event == RTW_EVENT_SME_RX_UNPROT_MLME_MGMT) {
		rx_mgnt_info = (struct rtw_event_rx_mgnt *)evt_info;

		dev_dbg(global_idev.pwhc_dev, "%s: rx unprot mlme mgnt \n", __func__);
		cfg80211_rx_unprot_mlme_mgmt(pndev, rx_mgnt_info->frame, rx_mgnt_info->frame_len);
	}
#endif

#ifdef CONFIG_P2P
	if (event == RTW_EVENT_WPA_P2P_CHANNEL_RDY) {
		if (evt_info[0] == 2) {/*scan_userdata=2 means using P2P Device intf*/
			wdev = global_idev.p2p_global.pd_pwdev;
		} else {
			wdev = global_idev.pwdev_global[evt_info[0]];
		}
		cfg80211_ready_on_channel(wdev, global_idev.p2p_global.roch_cookie, &global_idev.p2p_global.roch,
								  global_idev.p2p_global.roch_duration, GFP_KERNEL);
	}
#endif
}

void whc_host_acs_info_hdl(struct rtw_acs_mntr_rpt *acs_rpt)
{
	u8 idx;

	if (acs_rpt->channel == 0) {
		return;
	}

	for (idx = 0; idx < MAX_CHANNEL_NUM; idx++) {
		if (acs_rpt->channel == rtw_chnl_tbl[idx]) {
			memcpy(&acs_mntr_rpt_tbl[idx], acs_rpt, sizeof(struct rtw_acs_mntr_rpt));
			chanel_idx_max = idx;
			break;
		}
	}
}

u8 whc_host_get_network_info_hdl(uint32_t type, int idx, void *out_buf, uint32_t *rsp_len)
{
	struct in_ifaddr *ifa = NULL;

#ifdef CONFIG_P2P
	if (global_idev.p2p_global.pd_wlan_idx == 1) {
		idx = idx ^ 1;    /*GC intf is up, linux netdev idx is oppsite to driver wlan_idx*/
	}
#endif

	*rsp_len = 0;

	switch (type) {
	case WHC_WLAN_GET_IP:
		rcu_read_lock();
		in_dev_for_each_ifa_rcu(ifa, global_idev.pndev[idx]->ip_ptr)
		memcpy(out_buf, &ifa->ifa_address, 4);
		rcu_read_unlock();
		*rsp_len = 4;
		break;
	case WHC_WLAN_GET_GW:
		dev_warn(global_idev.pwhc_dev, "WHC_WLAN_GET_GW is not supported. Add into global_idev if needed.");
		break;
	case WHC_WLAN_GET_GWMSK:
		rcu_read_lock();
		in_dev_for_each_ifa_rcu(ifa, global_idev.pndev[idx]->ip_ptr)
		memcpy(out_buf, &ifa->ifa_mask, 4);
		rcu_read_unlock();
		*rsp_len = 4;
		break;
	case WHC_WLAN_GET_HW_ADDR:
		memcpy(out_buf, global_idev.pndev[idx]->dev_addr, ETH_ALEN);
		*rsp_len = ETH_ALEN;
		break;
	case WHC_WLAN_IS_VALID_IP:
		return 0;
	}

	return 1;
}

u8 whc_host_scan_user_callback_hdl(u32 *param_buf)
{
	u32 scan_result = param_buf[0];
	u32 scan_userdata = param_buf[1];
#ifdef CONFIG_P2P
	struct wireless_dev *wdev;

	if (global_idev.p2p_global.roch_onging) {
		global_idev.p2p_global.roch_onging = 0;
		if (scan_userdata == 2) { /*scan_userdata=2 means using P2P Device intf*/
			wdev = global_idev.p2p_global.pd_pwdev;
		} else {
			wdev = global_idev.pwdev_global[scan_userdata];
		}
		cfg80211_remain_on_channel_expired(wdev, global_idev.p2p_global.roch_cookie, &global_idev.p2p_global.roch,
										   GFP_KERNEL);
		return 0;
	}
#endif
	/* If user callback provided as NULL, scan_userdata appears NULL here. Do not make ptr. */
	/* https://jira.realtek.com/browse/AMEBAD2-1543 */
	whc_host_scan_done_indicate(scan_result, NULL);

	if (global_idev.mlme_priv.scan_block_param) {
		complete(&global_idev.mlme_priv.scan_block_param->sema);
	}
	if (global_idev.mlme_priv.scan_abort_block_param) {
		complete(&global_idev.mlme_priv.scan_abort_block_param->sema);
	}
	return 0;
}

void whc_host_set_netif_info_hdl(int idx, unsigned char *dev_addr)
{
	int softap_addr_offset_idx = global_idev.wifi_user_config.softap_addr_offset_idx;
	unsigned char softap_mac[ETH_ALEN];

	dev_dbg(global_idev.pwhc_dev, "[whc]: set netif info.");

	if (idx >= WHC_MAX_NET_PORT_NUM) {
		dev_dbg(global_idev.pwhc_dev, "%s: interface %d not exist!\n", __func__, idx);
		return;
	}

	if (!dev_addr) {
		dev_dbg(global_idev.pwhc_dev, "%s: mac address is NULL!\n", __func__);
		return;
	}

#ifdef CONFIG_P2P
	if (global_idev.p2p_global.pd_wlan_idx == 1) {
		idx = idx ^ 1; /*GC intf is up, linux netdev idx is oppsite to driver wlan_idx*/
	}
	if (!global_idev.pndev[idx]) {
		/*when GC netdev close, need revert mac address in driver, but netdev0 may already be closed*/
		return;
	}
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0))
	memcpy((void *)global_idev.pndev[idx]->dev_addr, dev_addr, ETH_ALEN);
#else
	eth_hw_addr_set(global_idev.pndev[idx], dev_addr);
#endif

	dev_dbg(global_idev.pwhc_dev, "MAC ADDR [%02x:%02x:%02x:%02x:%02x:%02x]", *global_idev.pndev[idx]->dev_addr,
			*(global_idev.pndev[idx]->dev_addr + 1), *(global_idev.pndev[idx]->dev_addr + 2),
			*(global_idev.pndev[idx]->dev_addr + 3), *(global_idev.pndev[idx]->dev_addr + 4),
			*(global_idev.pndev[idx]->dev_addr + 5));

	if (global_idev.pndev[0]) {
		/*set ap port mac address*/
		memcpy(softap_mac, global_idev.pndev[0]->dev_addr, ETH_ALEN);
		if (softap_addr_offset_idx == 0) {
			softap_mac[softap_addr_offset_idx] = global_idev.pndev[0]->dev_addr[softap_addr_offset_idx] + (1 << 1);
		} else {
			softap_mac[softap_addr_offset_idx] = global_idev.pndev[0]->dev_addr[softap_addr_offset_idx] + 1;
		}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0))
		memcpy((void *)global_idev.pndev[1]->dev_addr, softap_mac, ETH_ALEN);
#else
		eth_hw_addr_set(global_idev.pndev[1], softap_mac);
#endif
	}
}

