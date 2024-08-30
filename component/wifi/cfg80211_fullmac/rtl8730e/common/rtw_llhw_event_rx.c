#include <rtw_cfg80211_fullmac.h>

#ifdef CONFIG_P2P
#define rtw_p2p_frame_is_registered(p2p_role, frame_type) global_idev.p2p_global.mgmt_register[p2p_role - 1] & BIT(frame_type >> 4)
#endif

static void llhw_event_scan_report_indicate(struct event_priv_t *event_priv, u32 *param_buf)
{
#ifndef CONFIG_SDIO_BRIDGE
	u32 channel = param_buf[0];
	u32 frame_is_bcn = param_buf[1];
	s32 rssi = (s32)param_buf[2];
	u32 ie_len = param_buf[3];
	unsigned char *mac_addr = (u8 *) &param_buf[4];
	unsigned char *IEs = mac_addr + ETH_ALEN;

	cfg80211_rtw_inform_bss(channel, frame_is_bcn, rssi, mac_addr, IEs, ie_len);
#endif
	return;
}

static void cfg80211_rtw_set_acs_info(u32 *param_buf)
{
#ifndef CONFIG_SDIO_BRIDGE
	extern u8 chanel_idx_max;
	extern u8 rtw_chnl_tbl[MAX_CHANNEL_NUM];
	extern struct acs_mntr_rpt acs_mntr_rpt_tbl[MAX_CHANNEL_NUM];

	u8 idx = 0;
	struct acs_mntr_rpt *acs_rpt = (struct acs_mntr_rpt *)&param_buf[0];

	if (acs_rpt->channel == 0) {
		memset(acs_mntr_rpt_tbl, 0, sizeof(struct acs_mntr_rpt)*MAX_CHANNEL_NUM);
		return;
	}

	for (idx = 0; idx < MAX_CHANNEL_NUM; idx++) {
		if (acs_rpt->channel == rtw_chnl_tbl[idx]) {
			memcpy(&acs_mntr_rpt_tbl[idx], acs_rpt, sizeof(struct acs_mntr_rpt));
			chanel_idx_max = idx;
			break;
		}
	}
#endif
}


static void llhw_event_join_status_indicate(struct event_priv_t *event_priv, u32 *param_buf)
{
#ifndef CONFIG_SDIO_BRIDGE
	enum rtw_event_indicate event = (enum rtw_event_indicate)param_buf[0];
	int flags = (int)param_buf[1];
	int buf_len = (int)param_buf[2];
	char *buf = (char *) &param_buf[3];
	u16 disassoc_reason;
	int channel = 6;/*channel need get, force 6 seems ok temporary*/
	struct wireless_dev *wdev = global_idev.pwdev_global[0];
#ifdef CONFIG_P2P
	u16 frame_type;
#endif

	if (event == WIFI_EVENT_JOIN_STATUS) {
		cfg80211_rtw_connect_indicate(flags, buf, buf_len);
	}

	if ((event == WIFI_EVENT_JOIN_STATUS) && ((flags == RTW_JOINSTATUS_FAIL) || (flags == RTW_JOINSTATUS_DISCONNECT))) {
		if (flags == RTW_JOINSTATUS_DISCONNECT) {
			disassoc_reason = (u16)(((struct rtw_event_disconn_info_t *)buf)->disconn_reason && 0xffff);
			dev_dbg(global_idev.fullmac_dev, "%s: disassoc_reason=%d \n", __func__, disassoc_reason);
			cfg80211_rtw_disconnect_indicate(disassoc_reason, 1);
		}
		if (global_idev.mlme_priv.b_in_disconnect) {
			complete(&global_idev.mlme_priv.disconnect_done_sema);
			global_idev.mlme_priv.b_in_disconnect = false;
		}
	}

	if (event == WIFI_EVENT_STA_ASSOC) {
		dev_dbg(global_idev.fullmac_dev, "%s: sta assoc \n", __func__);
		cfg80211_rtw_sta_assoc_indicate(buf, buf_len);
	}

	if (event == WIFI_EVENT_STA_DISASSOC) {
		dev_dbg(global_idev.fullmac_dev, "%s: sta disassoc \n", __func__);
		cfg80211_del_sta(global_idev.pndev[1], buf, GFP_ATOMIC);
	}

	if (event == WIFI_EVENT_EXTERNAL_AUTH_REQ) {
		dev_dbg(global_idev.fullmac_dev, "%s: auth req \n", __func__);
		cfg80211_rtw_external_auth_request(buf, buf_len);
	}

	if (event == WIFI_EVENT_RX_MGNT) {
		dev_dbg(global_idev.fullmac_dev, "%s: rx mgnt \n", __func__);
#ifdef CONFIG_P2P
		channel = (flags > 0) ? (flags & 0x0000FFFF) : 6;
		frame_type = (u16)(flags >> 16);
		if (frame_type == IEEE80211_STYPE_PROBE_REQ) {
			if (global_idev.p2p_global.pd_pwdev && (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_PROBE_REQ))) {//P2P_DEV intf registered probe_req
				wdev =	global_idev.p2p_global.pd_pwdev;
			}
		} else if (frame_type == IEEE80211_STYPE_ACTION) {
			if (global_idev.p2p_global.pd_pwdev && (memcmp((buf + 4), global_idev.p2p_global.pd_pwdev->address, 6) == 0)) {
				if (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_ACTION)) {
					wdev =	global_idev.p2p_global.pd_pwdev; //DA match P2P_DEV intf, need use P2P_DEV intf to indicate
				}
			} else if (rtw_p2p_frame_is_registered(P2P_ROLE_CLIENT, IEEE80211_STYPE_ACTION)) { //GC intf has registered action report
				wdev = global_idev.pwdev_global[1];
			}
		}
#endif
		/*channel need get, force 6 seems ok temporary*/
		cfg80211_rx_mgmt(wdev, rtw_ch2freq(channel), 0, buf, buf_len, 0);
	}

	if (event == WIFI_EVENT_RX_MGNT_AP) {
		dev_dbg(global_idev.fullmac_dev, "%s: rx mgnt \n", __func__);
		wdev = ndev_to_wdev(global_idev.pndev[1]);
#ifdef CONFIG_P2P
		channel = (flags > 0) ? (flags & 0x0000FFFF) : 6;
		frame_type = (u16)(flags >> 16);
		if (frame_type == IEEE80211_STYPE_PROBE_REQ) {
			if (global_idev.p2p_global.pd_pwdev && (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_PROBE_REQ))) {//P2P_DEV intf registered probe_req
				cfg80211_rx_mgmt(global_idev.p2p_global.pd_pwdev, rtw_ch2freq(channel), 0, buf, buf_len, 0);
			}
			if (rtw_p2p_frame_is_registered(P2P_ROLE_GO, IEEE80211_STYPE_PROBE_REQ)) {//P2P_GO intf registered probe_req
				cfg80211_rx_mgmt(wdev, rtw_ch2freq(channel), 0, buf, buf_len, 0);
			}
			return;
		} else if ((frame_type == IEEE80211_STYPE_ACTION)) {
			if (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_ACTION)) {
				if (global_idev.p2p_global.pd_pwdev && (memcmp((buf + 4), global_idev.p2p_global.pd_pwdev->address, 6)) == 0) {
					wdev =	global_idev.p2p_global.pd_pwdev; //DA match P2P_DEV intf, need use P2P_DEV intf to indicate
				}
			}
		}
#endif
		cfg80211_rx_mgmt(wdev, rtw_ch2freq(channel), 0, buf, buf_len, 0);
	}

	if (event == WIFI_EVENT_OWE_PEER_KEY_RECV) {
		dev_dbg(global_idev.fullmac_dev, "%s: owe update \n", __func__);
		cfg80211_rtw_update_owe_info_event(buf, buf_len);
	}

#else
	llhw_sdio_bridge_event_join_status_indicate(event_priv, param_buf);
#endif
	return;

}

static void llhw_event_set_netif_info(struct event_priv_t *event_priv, u32 *param_buf)
{
	int idx = (int)param_buf[0];
	unsigned char *dev_addr = (u8 *)&param_buf[1];
	unsigned char last;
	int softap_addr_offset_idx = global_idev.wifi_user_config.softap_addr_offset_idx;

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

	memcpy((void *)global_idev.pndev[idx]->dev_addr, dev_addr, ETH_ALEN);
	dev_dbg(global_idev.fullmac_dev, "MAC ADDR [%02x:%02x:%02x:%02x:%02x:%02x]", *global_idev.pndev[idx]->dev_addr,
			*(global_idev.pndev[idx]->dev_addr + 1), *(global_idev.pndev[idx]->dev_addr + 2),
			*(global_idev.pndev[idx]->dev_addr + 3), *(global_idev.pndev[idx]->dev_addr + 4),
			*(global_idev.pndev[idx]->dev_addr + 5));

	if (global_idev.pndev[0]) {
		/*set ap port mac address*/
		memcpy((void *)global_idev.pndev[1]->dev_addr, global_idev.pndev[0]->dev_addr, ETH_ALEN);

		last = global_idev.pndev[0]->dev_addr[softap_addr_offset_idx] + 1;
		memcpy((void *)&global_idev.pndev[1]->dev_addr[softap_addr_offset_idx], &last, 1);
	}

func_exit:
	return;
}

static u8 llhw_event_get_network_info(struct event_priv_t *event_priv, u32 *param_buf)
{
	uint32_t type = (uint32_t)param_buf[0];
	int idx = param_buf[1];
	/* input is used for INIC_WLAN_IS_VALID_IP, not used now. */
	/* uint8_t *input = (uint8_t *)(&param_buf[2]); */
	uint32_t *rsp_ptr = NULL;
	uint32_t rsp_len = 0;
	struct in_ifaddr *ifa = NULL;
	uint32_t inic_ip_addr[INIC_MAX_NET_PORT_NUM] = {0};
	uint32_t inic_ip_mask[INIC_MAX_NET_PORT_NUM] = {0};
	struct inic_api_info *ret_msg;
	u8 *buf;
	u32 buf_len;

#ifdef CONFIG_P2P
	if (global_idev.p2p_global.pd_wlan_idx == 1) {
		idx = idx ^ 1; /*GC intf is up, linux netdev idx is oppsite to driver wlan_idx*/
	}
#endif

	switch (type) {
	case INIC_WLAN_GET_IP:
		rcu_read_lock();
		in_dev_for_each_ifa_rcu(ifa, global_idev.pndev[idx]->ip_ptr)
		memcpy(&inic_ip_addr[idx], &ifa->ifa_address, 4);
		rcu_read_unlock();
		rsp_ptr = &inic_ip_addr[idx];
		rsp_len = 4;
		break;
	case INIC_WLAN_GET_GW:
		dev_warn(global_idev.fullmac_dev, "INIC_WLAN_GET_GW is not supported. Add into global_idev if needed.");
		break;
	case INIC_WLAN_GET_GWMSK:
		rcu_read_lock();
		in_dev_for_each_ifa_rcu(ifa, global_idev.pndev[idx]->ip_ptr)
		memcpy(&inic_ip_mask[idx], &ifa->ifa_mask, 4);
		rcu_read_unlock();
		rsp_ptr = &inic_ip_mask[idx];
		rsp_len = 4;
		break;
	case INIC_WLAN_GET_HW_ADDR:
		rsp_ptr = (uint32_t *)global_idev.pndev[idx]->dev_addr;
		rsp_len = ETH_ALEN;
		break;
	case INIC_WLAN_IS_VALID_IP:
		return 0;
	}

	buf_len = SIZE_TX_DESC + sizeof(struct inic_api_info) + rsp_len;
	buf = kzalloc(buf_len, GFP_KERNEL);
	if (buf) {
		/* fill inic_api_info_t */
		ret_msg = (struct inic_api_info *)(buf + SIZE_TX_DESC);
		ret_msg->event = INIC_WIFI_EVT_API_RETURN;
		ret_msg->api_id = INIC_API_GET_LWIP_INFO;

		/* copy data */
		memcpy((u8 *)(ret_msg + 1), (u8 *)rsp_ptr, rsp_len);

		/* send */
		llhw_send_data(buf, buf_len, NULL);
#ifndef CONFIG_INIC_USB_ASYNC_SEND
		kfree(buf);
#endif
	}

	return 1;
}

#ifdef CONFIG_NAN
static void llhw_event_nan_match_indicate(struct event_priv_t *event_priv, u32 *param_buf)
{
	u8 type = param_buf[0];
	u8 inst_id = param_buf[1];
	u8 peer_inst_id = param_buf[2];
	u32 info_len = param_buf[3];
	u64 cookie = ((u64)param_buf[5] << 32) | param_buf[4];
	unsigned char *mac_addr = (u8 *)&param_buf[6];
	unsigned char *IEs = mac_addr + ETH_ALEN;

	cfg80211_rtw_nan_handle_sdf(type, inst_id, peer_inst_id, mac_addr, info_len, IEs, cookie);

	return;
}

static void llhw_event_nan_cfgvendor_event_indicate(struct event_priv_t *event_priv, u32 *param_buf)
{
	u8 event_id = param_buf[0];
	u32 size = param_buf[1];
	unsigned char *event_addr = (u8 *)&param_buf[2];

	rtw_cfgvendor_nan_event_indication(event_id, event_addr, size);

	return;
}

static void llhw_event_nan_cfgvendor_cmd_reply(struct event_priv_t *event_priv, u32 *param_buf)
{
	u32 size = param_buf[0];
	unsigned char *data_addr = (u8 *)&param_buf[1];

	rtw_cfgvendor_send_cmd_reply(data_addr, size);

	return;
}

#endif

void llhw_event_task(struct work_struct *data)
{
	struct event_priv_t *event_priv = &global_idev.event_priv;
	u8 already_ret = 0;
	struct inic_api_info *p_recv_msg = (struct inic_api_info *)(event_priv->rx_api_msg->data + SIZE_RX_DESC);
	u32 *param_buf = (u32 *)(p_recv_msg + 1);
	struct inic_api_info *ret_msg;
	u8 *buf;
	u32 buf_len;
#ifdef CONFIG_P2P
	struct wireless_dev *wdev = NULL;
#endif

	dev_dbg(global_idev.fullmac_dev, "-----DEVICE CALLING API %x START\n", p_recv_msg->api_id);

	switch (p_recv_msg->api_id) {

	/* receive callback indication */
	case INIC_API_SCAN_USER_CALLBACK:
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
#ifdef CONFIG_SDIO_BRIDGE
		llhw_sdio_bridge_get_scan_result(param_buf[0]);
#else
		/* If user callback provided as NULL, param_buf[1] appears NULL here. Do not make ptr. */
		/* https://jira.realtek.com/browse/AMEBAD2-1543 */
		cfg80211_rtw_scan_done_indicate(param_buf[0], NULL);
#endif
		break;
	case INIC_API_IP_ACS:
		cfg80211_rtw_set_acs_info(param_buf);
		break;
	case INIC_API_SCAN_EACH_REPORT_USER_CALLBACK:
		//iiha_scan_each_report_cb_hdl(event_priv, p_recv_msg);
		break;
	case INIC_API_WIFI_AP_CH_SWITCH:
		//iiha_ap_ch_switch_hdl(event_priv, p_recv_msg);
		break;
	case INIC_API_HDL:
		llhw_event_join_status_indicate(event_priv, param_buf);
		break;
	case INIC_API_PROMISC_CALLBACK:
		//iiha_wifi_promisc_hdl(event_priv, p_recv_msg);
		break;
	case INIC_API_GET_LWIP_INFO:
		already_ret = llhw_event_get_network_info(event_priv, param_buf);
		break;
	case INIC_API_SET_NETIF_INFO:
		llhw_event_set_netif_info(event_priv, param_buf);
		break;
	case INIC_API_CFG80211_SCAN_REPORT:
		llhw_event_scan_report_indicate(event_priv, param_buf);
		break;
#ifdef CONFIG_NAN
	case INIC_API_CFG80211_NAN_REPORT_MATCH_EVENT:
		llhw_event_nan_match_indicate(event_priv, param_buf);
		break;
	case INIC_API_CFG80211_NAN_DEL_FUNC:
		cfg80211_rtw_nan_func_free(param_buf[0]);
		break;
	case INIC_API_CFG80211_NAN_CFGVENDOR_EVENT:
		llhw_event_nan_cfgvendor_event_indicate(event_priv, param_buf);
		break;
	case INIC_API_CFG80211_NAN_CFGVENDOR_CMD_REPLY:
		llhw_event_nan_cfgvendor_cmd_reply(event_priv, param_buf);
		break;
#endif
#ifdef CONFIG_P2P
	case INIC_API_CFG80211_P2P_CH_RDY:
		if (param_buf[0] == 2) {/*scan_userdata=2 means using P2P Device intf*/
			wdev = global_idev.p2p_global.pd_pwdev;
		} else {
			wdev = global_idev.pwdev_global[param_buf[0]];
		}
		cfg80211_ready_on_channel(wdev, global_idev.p2p_global.roch_cookie, &global_idev.p2p_global.roch,
								  global_idev.p2p_global.roch_duration, GFP_KERNEL);
		break;
#endif

	default:
		dev_err(global_idev.fullmac_dev, "%s: Unknown Device event(%x)!\n\r", "event", p_recv_msg->event);
		break;
	}

	if (already_ret == 0) {
		buf_len = SIZE_TX_DESC + sizeof(struct inic_api_info);
		buf = kzalloc(buf_len, GFP_KERNEL);
		if (buf) {
			/* fill and send ret_msg */
			ret_msg = (struct inic_api_info *)(buf + SIZE_TX_DESC);
			ret_msg->event = INIC_WIFI_EVT_API_RETURN;
			ret_msg->api_id = p_recv_msg->api_id;

			llhw_send_data(buf, buf_len, NULL);
#ifndef CONFIG_INIC_USB_ASYNC_SEND
			kfree(buf);
#endif
		}
	}

	dev_dbg(global_idev.fullmac_dev, "-----DEVICE CALLING API %x END\n", p_recv_msg->api_id);

	/* free rx_event_msg */
	kfree_skb(event_priv->rx_api_msg);

	return;
}

int llhw_event_init(struct inic_device *idev)
{
	struct event_priv_t	*event_priv = &global_idev.event_priv;

	/* initialize the mutex to send event_priv message. */
	mutex_init(&(event_priv->send_mutex));
	init_completion(&event_priv->api_ret_sema);
	event_priv->b_waiting_for_ret = 0;

#ifdef CONFIG_SDIO_BRIDGE
	init_completion(&event_priv->bridge_scan_done_sema);
#endif
	/* initialize event tasklet */
	INIT_WORK(&(event_priv->api_work), llhw_event_task);

	return 0;
}

void llhw_event_deinit(void)
{
	struct event_priv_t *event_priv = &global_idev.event_priv;

	/* deinitialize the mutex to send event_priv message. */
	mutex_destroy(&(event_priv->send_mutex));

	complete_release(&event_priv->api_ret_sema);
#ifdef CONFIG_SDIO_BRIDGE
	complete_release(&event_priv->bridge_scan_done_sema);
#endif
	return;
}

