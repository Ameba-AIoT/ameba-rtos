#include <whc_host_linux.h>

#ifdef CONFIG_P2P
#define rtw_p2p_frame_is_registered(p2p_role, frame_type) global_idev.p2p_global.mgmt_register[p2p_role - 1] & BIT(frame_type >> 4)
#endif

static void whc_fullmac_host_event_scan_report_indicate(struct event_priv_t *event_priv, u32 *param_buf)
{
	u32 channel = param_buf[0];
	u32 frame_is_bcn = param_buf[1];
	s32 rssi = (s32)param_buf[2];
	u32 ie_len = param_buf[3];
	unsigned char *mac_addr = (u8 *) &param_buf[4];
	unsigned char *IEs = mac_addr + ETH_ALEN;

	whc_fullmac_host_inform_bss(channel, frame_is_bcn, rssi, mac_addr, IEs, ie_len);
	return;
}

static void whc_fullmac_host_event_update_regd_indicate(struct event_priv_t *event_priv, u32 *param_buf)
{
	struct rtw_country_code_table *ptab = (struct rtw_country_code_table *)&param_buf[0];
	(void)event_priv;

	whc_fullmac_host_regd_update(ptab);
	return;
}

static void whc_fullmac_host_event_set_acs_info(u32 *param_buf)
{
	u8 idx = 0;
	struct rtw_acs_mntr_rpt *acs_rpt = (struct rtw_acs_mntr_rpt *)&param_buf[0];

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


static void whc_fullmac_host_event_join_status_indicate(struct event_priv_t *event_priv, u32 *param_buf)
{
	struct mlme_priv_t *mlme_priv = &global_idev.mlme_priv;
	u32 event = (u32)param_buf[0];
	char *evt_info = (char *) &param_buf[2];
	u16 disassoc_reason;
	int channel = 6;/*channel need get, force 6 seems ok temporary*/
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

	if (event == RTW_EVENT_JOIN_STATUS) {
		join_status_info = (struct rtw_event_join_status_info *)evt_info;
		join_status = join_status_info->status;
		whc_fullmac_host_connect_indicate(join_status, evt_info);
	}

	if ((event == RTW_EVENT_JOIN_STATUS) && ((join_status == RTW_JOINSTATUS_FAIL) || (join_status == RTW_JOINSTATUS_DISCONNECT))) {
		if (join_status == RTW_JOINSTATUS_DISCONNECT) {
			disconnect = &join_status_info->priv.disconnect;
			disassoc_reason = (u16)(disconnect->disconn_reason && 0xffff);
			dev_dbg(global_idev.fullmac_dev, "%s: disassoc_reason=%d \n", __func__, disassoc_reason);
			whc_fullmac_host_disconnect_indicate(disassoc_reason, 1, join_status_info->bssid);
		}
		if (mlme_priv->b_in_disconnect) {
			complete(&mlme_priv->disconnect_done_sema);
			mlme_priv->b_in_disconnect = false;
		}
	}

	if (event == RTW_EVENT_AP_STA_ASSOC) {
		dev_dbg(global_idev.fullmac_dev, "%s: sta assoc \n", __func__);
		sta_assoc_info = (struct rtw_event_ap_sta_assoc *)evt_info;
		whc_fullmac_host_sta_assoc_indicate(sta_assoc_info->frame, sta_assoc_info->frame_len);
	}

	if (event == RTW_EVENT_AP_STA_DISASSOC) {
		dev_dbg(global_idev.fullmac_dev, "%s: sta disassoc \n", __func__);
		sta_disassoc_info = (struct rtw_event_ap_sta_disassoc *)evt_info;
		cfg80211_del_sta(global_idev.pndev[1], sta_disassoc_info->sta_mac, GFP_ATOMIC);
	}

	if (event == RTW_EVENT_EXTERNAL_AUTH_REQ) {
		dev_dbg(global_idev.fullmac_dev, "%s: auth req \n", __func__);
		whc_fullmac_host_external_auth_request(evt_info);
	}

	if (event == RTW_EVENT_RX_MGNT) {
		dev_dbg(global_idev.fullmac_dev, "%s: rx mgnt \n", __func__);
		rx_mgnt_info = (struct rtw_event_rx_mgnt *)evt_info;
#ifdef CONFIG_P2P
		channel = rx_mgnt_info->channel;
		frame_type = rx_mgnt_info->frame_type;
		if (frame_type == IEEE80211_STYPE_PROBE_REQ) {
			if (global_idev.p2p_global.pd_pwdev && (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_PROBE_REQ))) {//P2P_DEV intf registered probe_req
				wdev =	global_idev.p2p_global.pd_pwdev;
			}
		} else if (frame_type == IEEE80211_STYPE_ACTION) {
			if (global_idev.p2p_global.pd_pwdev && (memcmp((rx_mgnt_info->frame + 4), global_idev.p2p_global.pd_pwdev->address, 6) == 0)) {
				if (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_ACTION)) {
					wdev =	global_idev.p2p_global.pd_pwdev; //DA match P2P_DEV intf, need use P2P_DEV intf to indicate
				}
			} else if (rtw_p2p_frame_is_registered(P2P_ROLE_CLIENT, IEEE80211_STYPE_ACTION)) { //GC intf has registered action report
				wdev = global_idev.pwdev_global[1];
			}
		} else {
			if (global_idev.p2p_global.pd_pwdev && memcmp((rx_mgnt_info->frame + 4), global_idev.p2p_global.pd_pwdev->address, 6) == 0) {
				wdev =	global_idev.p2p_global.pd_pwdev;
			} else if (global_idev.p2p_global.p2p_role == P2P_ROLE_CLIENT) { /* port0 is used by GC */
				wdev = global_idev.pwdev_global[1];
			} else {
				wdev = global_idev.pwdev_global[0];							/* port0 is used by STA */
			}
		}
#endif
		/*channel need get, force 6 seems ok temporary*/
		cfg80211_rx_mgmt(wdev, rtw_ch2freq(channel), 0, rx_mgnt_info->frame, rx_mgnt_info->frame_len, 0);
	}

#ifdef CONFIG_IEEE80211R
	if ((event == RTW_EVENT_FT_AUTH_START) || (event == RTW_EVENT_FT_RX_MGNT) || (event == RTW_EVENT_JOIN_STATUS)) {
		whc_fullmac_host_ft_event(event, evt_info, join_status);
	}
#endif

	if (event == RTW_EVENT_RX_MGNT_AP) {
		dev_dbg(global_idev.fullmac_dev, "%s: rx mgnt \n", __func__);
		wdev = ndev_to_wdev(global_idev.pndev[1]);
		rx_mgnt_info = (struct rtw_event_rx_mgnt *)evt_info;
#ifdef CONFIG_P2P
		channel = rx_mgnt_info->channel;
		frame_type = rx_mgnt_info->frame_type;
		if (frame_type == IEEE80211_STYPE_PROBE_REQ) {
			if (global_idev.p2p_global.pd_pwdev && (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_PROBE_REQ))) {//P2P_DEV intf registered probe_req
				cfg80211_rx_mgmt(global_idev.p2p_global.pd_pwdev, rtw_ch2freq(channel), 0, rx_mgnt_info->frame, rx_mgnt_info->frame_len, 0);
			}
			if (rtw_p2p_frame_is_registered(P2P_ROLE_GO, IEEE80211_STYPE_PROBE_REQ)) {//P2P_GO intf registered probe_req
				cfg80211_rx_mgmt(wdev, rtw_ch2freq(channel), 0, rx_mgnt_info->frame, rx_mgnt_info->frame_len, 0);
			}
			return;
		} else if ((frame_type == IEEE80211_STYPE_ACTION)) {
			if (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_ACTION)) {
				if (global_idev.p2p_global.pd_pwdev && (memcmp((rx_mgnt_info->frame + 4), global_idev.p2p_global.pd_pwdev->address, 6)) == 0) {
					wdev =	global_idev.p2p_global.pd_pwdev; //DA match P2P_DEV intf, need use P2P_DEV intf to indicate
				}
			}
		}
#endif
		cfg80211_rx_mgmt(wdev, rtw_ch2freq(channel), 0, rx_mgnt_info->frame, rx_mgnt_info->frame_len, 0);
	}

	if (event == RTW_EVENT_OWE_PEER_KEY_RECV) {
		dev_dbg(global_idev.fullmac_dev, "%s: owe update \n", __func__);
		whc_fullmac_host_update_owe_info_event(evt_info);
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
		dev_dbg(global_idev.fullmac_dev, "%s: RTW_EVENT_SME_AUTH_TIMEOUT \n", __func__);
		auth_timeout_info = (struct rtw_event_sme_auth_timeout *)evt_info;
		cfg80211_auth_timeout(pndev, auth_timeout_info->bssid);

	} else if (event == RTW_EVENT_SME_ASSOC_TIMEOUT) {
		dev_dbg(global_idev.fullmac_dev, "%s: RTW_EVENT_SME_ASSOC_TIMEOUT \n", __func__);

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
			dev_dbg(global_idev.fullmac_dev, "%s(): association timeout but no corresponding bss recorded\n", __FUNCTION__);
		}

	} else if (event == RTW_EVENT_SME_RX_MLME_MGNT) {
		dev_dbg(global_idev.fullmac_dev, "%s: RTW_EVENT_SME_RX_MLME_MGNT\n", __func__);
		evt_rpt_frm = (struct rtw_event_report_frame *)evt_info;
		cfg80211_rx_mlme_mgmt(pndev, evt_rpt_frm->frame, evt_rpt_frm->frame_len);

	} else if (event == RTW_EVENT_SME_TX_MLME_MGNT) {
		dev_dbg(global_idev.fullmac_dev, "%s: RTW_EVENT_SME_TX_MLME_MGNT \n", __func__);
		evt_rpt_frm = (struct rtw_event_report_frame *)evt_info;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 11, 0))
		cfg80211_tx_mlme_mgmt(pndev, evt_rpt_frm->frame, evt_rpt_frm->frame_len, false);
#else
		cfg80211_tx_mlme_mgmt(pndev, evt_rpt_frm->frame, evt_rpt_frm->frame_len);
#endif
	} else if (event == RTW_EVENT_SME_RX_ASSOC_RESP) {
		dev_dbg(global_idev.fullmac_dev, "%s: RTW_EVENT_SME_RX_ASSOC_RESP \n", __func__);

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
			dev_dbg(global_idev.fullmac_dev, "%s(): RX assocrsp but no corresponding bss recorded\n", __FUNCTION__);
		}
	} else if (event == RTW_EVENT_SME_RX_UNPROT_MLME_MGMT) {
		rx_mgnt_info = (struct rtw_event_rx_mgnt *)evt_info;

		dev_dbg(global_idev.fullmac_dev, "%s: rx unprot mlme mgnt \n", __func__);
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
	return;

}

static void whc_fullmac_host_event_set_netif_info(struct event_priv_t *event_priv, u32 *param_buf)
{
	int idx = (int)param_buf[0];
	unsigned char *dev_addr = (u8 *)&param_buf[1];
	int softap_addr_offset_idx = global_idev.wifi_user_config.softap_addr_offset_idx;
	unsigned char softap_mac[ETH_ALEN];

	dev_dbg(global_idev.fullmac_dev, "[fullmac]: set netif info.");

	if (idx >= TOTAL_IFACE_NUM) {
		dev_dbg(global_idev.fullmac_dev, "%s: interface %d not exist!\n", __func__, idx);
		goto func_exit;
	}

	if (!dev_addr) {
		dev_dbg(global_idev.fullmac_dev, "%s: mac address is NULL!\n", __func__);
		goto func_exit;
	}

#ifdef CONFIG_P2P
	if (global_idev.p2p_global.pd_wlan_idx == 1) {
		idx = idx ^ 1; /*GC intf is up, linux netdev idx is oppsite to driver wlan_idx*/
	}
	if (!global_idev.pndev[idx]) {
		/*when GC netdev close, need revert mac address in driver, but netdev0 may already be closed*/
		goto func_exit;
	}
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0))
	memcpy((void *)global_idev.pndev[idx]->dev_addr, dev_addr, ETH_ALEN);
#else
	eth_hw_addr_set(global_idev.pndev[idx], dev_addr);
#endif
	dev_dbg(global_idev.fullmac_dev, "MAC ADDR [%02x:%02x:%02x:%02x:%02x:%02x]", *global_idev.pndev[idx]->dev_addr,
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

func_exit:
	return;
}

static u8 whc_fullmac_host_event_get_network_info(struct event_priv_t *event_priv, u32 *param_buf)
{
	uint32_t type = (uint32_t)param_buf[0];
	int idx = param_buf[1];
	/* input is used for WHC_WLAN_IS_VALID_IP, not used now. */
	/* uint8_t *input = (uint8_t *)(&param_buf[2]); */
	uint32_t *rsp_ptr = NULL;
	uint32_t rsp_len = 0;
	struct in_ifaddr *ifa = NULL;
	uint32_t inic_ip_addr[WHC_MAX_NET_PORT_NUM] = {0};
	uint32_t inic_ip_mask[WHC_MAX_NET_PORT_NUM] = {0};
	struct whc_api_info *ret_msg;
	u8 *buf;
	u32 buf_len;

#ifdef CONFIG_P2P
	if (global_idev.p2p_global.pd_wlan_idx == 1) {
		idx = idx ^ 1; /*GC intf is up, linux netdev idx is oppsite to driver wlan_idx*/
	}
#endif

	switch (type) {
	case WHC_WLAN_GET_IP:
		rcu_read_lock();
		in_dev_for_each_ifa_rcu(ifa, global_idev.pndev[idx]->ip_ptr)
		memcpy(&inic_ip_addr[idx], &ifa->ifa_address, 4);
		rcu_read_unlock();
		rsp_ptr = &inic_ip_addr[idx];
		rsp_len = 4;
		break;
	case WHC_WLAN_GET_GW:
		dev_warn(global_idev.fullmac_dev, "WHC_WLAN_GET_GW is not supported. Add into global_idev if needed.");
		break;
	case WHC_WLAN_GET_GWMSK:
		rcu_read_lock();
		in_dev_for_each_ifa_rcu(ifa, global_idev.pndev[idx]->ip_ptr)
		memcpy(&inic_ip_mask[idx], &ifa->ifa_mask, 4);
		rcu_read_unlock();
		rsp_ptr = &inic_ip_mask[idx];
		rsp_len = 4;
		break;
	case WHC_WLAN_GET_HW_ADDR:
		rsp_ptr = (uint32_t *)global_idev.pndev[idx]->dev_addr;
		rsp_len = ETH_ALEN;
		break;
	case WHC_WLAN_IS_VALID_IP:
		return 0;
	}

	/* free rx_event_msg */
	kfree_skb(event_priv->rx_api_msg);

	buf_len = SIZE_TX_DESC + sizeof(struct whc_api_info) + rsp_len;
	buf = kzalloc(buf_len, GFP_KERNEL);
	if (buf) {
		/* fill whc_api_info_t */
		ret_msg = (struct whc_api_info *)(buf + SIZE_TX_DESC);
		ret_msg->event = WHC_WIFI_EVT_API_RETURN;
		ret_msg->api_id = WHC_API_GET_LWIP_INFO;

		/* copy data */
		memcpy((u8 *)(ret_msg + 1), (u8 *)rsp_ptr, rsp_len);

		/* send */
		whc_host_send_data(buf, buf_len, NULL);
#ifndef CONFIG_INIC_USB_ASYNC_SEND
		kfree(buf);
#endif
	}

	return 1;
}

#ifdef CONFIG_NAN
static void whc_fullmac_host_event_nan_match_indicate(struct event_priv_t *event_priv, u32 *param_buf)
{
	u8 type = param_buf[0];
	u8 inst_id = param_buf[1];
	u8 peer_inst_id = param_buf[2];
	u32 info_len = param_buf[3];
	u64 cookie = ((u64)param_buf[5] << 32) | param_buf[4];
	unsigned char *mac_addr = (u8 *)&param_buf[6];
	unsigned char *IEs = mac_addr + ETH_ALEN;

	whc_fullmac_host_nan_handle_sdf(type, inst_id, peer_inst_id, mac_addr, info_len, IEs, cookie);

	return;
}

static void whc_fullmac_host_event_nan_cfgvendor_event_indicate(struct event_priv_t *event_priv, u32 *param_buf)
{
	u8 event_id = param_buf[0];
	u32 size = param_buf[1];
	unsigned char *event_addr = (u8 *)&param_buf[2];

	whc_fullmac_host_cfgvendor_nan_event_indication(event_id, event_addr, size);

	return;
}

static void whc_fullmac_host_event_nan_cfgvendor_cmd_reply(struct event_priv_t *event_priv, u32 *param_buf)
{
	u32 size = param_buf[0];
	unsigned char *data_addr = (u8 *)&param_buf[1];

	whc_fullmac_host_cfgvendor_send_cmd_reply(data_addr, size);

	return;
}

#endif

void whc_fullmac_host_event_task(struct work_struct *data)
{
	struct event_priv_t *event_priv = &global_idev.event_priv;
	u8 already_ret = 0;
	struct whc_api_info *p_recv_msg = (struct whc_api_info *)(event_priv->rx_api_msg->data + SIZE_RX_DESC);
	u32 *param_buf = (u32 *)(p_recv_msg + 1);
	struct whc_api_info *ret_msg;
	u8 *buf;
	u32 buf_len;
#ifdef CONFIG_P2P
	struct wireless_dev *wdev = NULL;
#endif
#ifdef CONFIG_NAN
	u64 os_dep_data = 0;
#endif

	dev_dbg(global_idev.fullmac_dev, "-----DEVICE CALLING API %x START\n", p_recv_msg->api_id);

	switch (p_recv_msg->api_id) {

	/* receive callback indication */
	case WHC_API_SCAN_USER_CALLBACK:
#ifdef CONFIG_P2P
		if (global_idev.p2p_global.roch_onging) {
			global_idev.p2p_global.roch_onging = 0;
			if (param_buf[1] == 2) {/*scan_userdata=2 means using P2P Device intf*/
				wdev = global_idev.p2p_global.pd_pwdev;
			} else {
				wdev = global_idev.pwdev_global[param_buf[1]];
			}
			cfg80211_remain_on_channel_expired(wdev, global_idev.p2p_global.roch_cookie, &global_idev.p2p_global.roch,
											   GFP_KERNEL);
			break;
		}
#endif
		/* If user callback provided as NULL, param_buf[1] appears NULL here. Do not make ptr. */
		/* https://jira.realtek.com/browse/AMEBAD2-1543 */
		whc_fullmac_host_scan_done_indicate(param_buf[0], NULL);

		/* if Synchronous scan/scan abort, up sema when scan done */
		if (global_idev.mlme_priv.scan_block_param) {
			complete(&global_idev.mlme_priv.scan_block_param->sema);
		}
		if (global_idev.mlme_priv.scan_abort_block_param) {
			complete(&global_idev.mlme_priv.scan_abort_block_param->sema);
		}
		break;
	case WHC_API_IP_ACS:
		whc_fullmac_host_event_set_acs_info(param_buf);
		break;
	case WHC_API_SCAN_EACH_REPORT_USER_CALLBACK:
		//iiha_scan_each_report_cb_hdl(event_priv, p_recv_msg);
		break;
	case WHC_API_WIFI_AP_CH_SWITCH:
		//iiha_ap_ch_switch_hdl(event_priv, p_recv_msg);
		break;
	case WHC_API_WIFI_EVENT:
		whc_fullmac_host_event_join_status_indicate(event_priv, param_buf);
		break;
	case WHC_API_PROMISC_CALLBACK:
		//iiha_wifi_promisc_hdl(event_priv, p_recv_msg);
		break;
	case WHC_API_GET_LWIP_INFO:
		already_ret = whc_fullmac_host_event_get_network_info(event_priv, param_buf);
		break;
	case WHC_API_SET_NETIF_INFO:
		whc_fullmac_host_event_set_netif_info(event_priv, param_buf);
		break;
	case WHC_API_CFG80211_SCAN_REPORT:
		whc_fullmac_host_event_scan_report_indicate(event_priv, param_buf);
		break;
#ifdef CONFIG_NAN
	case WHC_API_CFG80211_NAN_REPORT_MATCH_EVENT:
		whc_fullmac_host_event_nan_match_indicate(event_priv, param_buf);
		break;
	case WHC_API_CFG80211_NAN_DEL_FUNC:
		os_dep_data = ((u64)param_buf[1] << 32) | param_buf[0];
		whc_fullmac_host_nan_func_free(os_dep_data);
		break;
	case WHC_API_CFG80211_NAN_CFGVENDOR_EVENT:
		whc_fullmac_host_event_nan_cfgvendor_event_indicate(event_priv, param_buf);
		break;
	case WHC_API_CFG80211_NAN_CFGVENDOR_CMD_REPLY:
		whc_fullmac_host_event_nan_cfgvendor_cmd_reply(event_priv, param_buf);
		break;
#endif
	case WHC_API_UPDATE_REGD_EVENT:
		whc_fullmac_host_event_update_regd_indicate(event_priv, param_buf);
	break;

	default:
		dev_err(global_idev.fullmac_dev, "%s: Unknown Device event(%x)!\n\r", "event", p_recv_msg->event);
		break;
	}

	if (already_ret == 0) {
		buf_len = SIZE_TX_DESC + sizeof(struct whc_api_info);
		buf = kzalloc(buf_len, GFP_KERNEL);
		if (buf) {
			/* fill and send ret_msg */
			ret_msg = (struct whc_api_info *)(buf + SIZE_TX_DESC);
			ret_msg->event = WHC_WIFI_EVT_API_RETURN;
			ret_msg->api_id = p_recv_msg->api_id;

			/* free rx_event_msg */
			kfree_skb(event_priv->rx_api_msg);

			whc_host_send_data(buf, buf_len, NULL);
#ifndef CONFIG_INIC_USB_ASYNC_SEND
			kfree(buf);
#endif
		}
	}

	dev_dbg(global_idev.fullmac_dev, "-----DEVICE CALLING API %x END\n", p_recv_msg->api_id);

	return;
}

int whc_fullmac_host_event_init(struct whc_device *idev)
{
	struct event_priv_t	*event_priv = &global_idev.event_priv;

	/* initialize the mutex to send event_priv message. */
	mutex_init(&(event_priv->send_mutex));
	init_completion(&event_priv->api_ret_sema);
	event_priv->b_waiting_for_ret = 0;

	/* initialize event tasklet */
	INIT_WORK(&(event_priv->api_work), whc_fullmac_host_event_task);

	return 0;
}

void whc_fullmac_host_event_deinit(void)
{
	struct event_priv_t *event_priv = &global_idev.event_priv;

	/* deinitialize the mutex to send event_priv message. */
	mutex_destroy(&(event_priv->send_mutex));

	complete_release(&event_priv->api_ret_sema);

	return;
}

