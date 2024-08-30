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
#ifdef CONFIG_P2P
#define rtw_p2p_frame_is_registered(p2p_role, frame_type) global_idev.p2p_global.mgmt_register[p2p_role - 1] & BIT(frame_type >> 4)
#endif

static void llhw_event_scan_report_indicate(struct event_priv_t *event_priv, struct inic_ipc_dev_req_msg *p_ipc_msg)
{
	struct device *pdev = NULL;
	dma_addr_t dma_addr = 0;
	dma_addr_t dma_ie = 0;
	u32 channel = p_ipc_msg->param_buf[0];
	u32 frame_is_bcn = p_ipc_msg->param_buf[1];
	s32 rssi = (s32)p_ipc_msg->param_buf[2];
	unsigned char *mac_addr = phys_to_virt(p_ipc_msg->param_buf[3]);
	unsigned char *IEs = phys_to_virt(p_ipc_msg->param_buf[4]);
	u32 ie_len = p_ipc_msg->param_buf[5];

	if (!global_idev.event_ch) {
		dev_err(global_idev.fullmac_dev, "%s,%s: event_priv_t is NULL in!\n", "event", __func__);
		goto func_exit;
	}

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		dev_err(global_idev.fullmac_dev, "%s,%s: device is NULL in scan!\n", "event", __func__);
		goto func_exit;
	}

	dma_addr = dma_map_single(pdev, mac_addr, ETH_ALEN, DMA_FROM_DEVICE);
	if (dma_mapping_error(pdev, dma_addr)) {
		dev_err(global_idev.fullmac_dev, "%s: mapping dma error!\n", __func__);
		goto func_exit;
	}

	dma_ie = dma_map_single(pdev, IEs, ie_len, DMA_FROM_DEVICE);
	if (dma_mapping_error(pdev, dma_ie)) {
		dev_err(global_idev.fullmac_dev, "%s: mapping dma error!\n", __func__);
		goto func_exit;
	}

	cfg80211_rtw_inform_bss(channel, frame_is_bcn, rssi, mac_addr, IEs, ie_len);

	dma_unmap_single(pdev, dma_addr, ETH_ALEN, DMA_FROM_DEVICE);
	dma_unmap_single(pdev, dma_ie, ie_len, DMA_FROM_DEVICE);

func_exit:
	return;
}

static void cfg80211_rtw_set_acs_info(struct inic_ipc_dev_req_msg *p_ipc_msg)
{
	extern u8 chanel_idx_max;
	extern u8 rtw_chnl_tbl[MAX_CHANNEL_NUM];
	extern struct acs_mntr_rpt acs_mntr_rpt_tbl[MAX_CHANNEL_NUM];

	u8 idx = 0;
	struct device *pdev = NULL;
	dma_addr_t dma_acs_rpt = 0;
	struct acs_mntr_rpt *acs_rpt = phys_to_virt(p_ipc_msg->param_buf[0]);


	if (!global_idev.event_ch) {
		dev_err(global_idev.fullmac_dev, "%s,%s: event_priv_t is NULL in!\n", "event", __func__);
		goto func_exit;
	}

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		dev_err(global_idev.fullmac_dev, "%s,%s: device is NULL in scan!\n", "event", __func__);
		goto func_exit;
	}

	dma_acs_rpt = dma_map_single(pdev, acs_rpt, sizeof(struct acs_mntr_rpt), DMA_FROM_DEVICE);
	if (dma_mapping_error(pdev, dma_acs_rpt)) {
		dev_err(global_idev.fullmac_dev, "%s: mapping dma error!\n", __func__);
		goto func_exit;
	}

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

	dma_unmap_single(pdev, dma_acs_rpt, sizeof(struct acs_mntr_rpt), DMA_FROM_DEVICE);

func_exit:
	return;
}

static void llhw_event_join_status_indicate(struct event_priv_t *event_priv, struct inic_ipc_dev_req_msg *p_ipc_msg)
{
	enum rtw_event_indicate event = (enum rtw_event_indicate)p_ipc_msg->param_buf[0];
	char *buf = phys_to_virt(p_ipc_msg->param_buf[1]);
	int buf_len = (int)p_ipc_msg->param_buf[2];
	int flags = (int)p_ipc_msg->param_buf[3];
	struct device *pdev = NULL;
	dma_addr_t dma_buf = 0;
	u16 disassoc_reason;
	int channel = 6;/*channel need get, force 6 seems ok temporary*/
	struct wireless_dev *wdev = global_idev.pwdev_global[0];
#ifdef CONFIG_P2P
	u16 frame_type;
#endif

	if (!global_idev.event_ch) {
		dev_err(global_idev.fullmac_dev, "%s,%s: event_priv_t is NULL in!\n", "event", __func__);
		goto func_exit;
	}

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		dev_err(global_idev.fullmac_dev, "%s,%s: device is NULL in scan!\n", "event", __func__);
		goto func_exit;
	}

	if (buf_len > 0) {
		dma_buf = dma_map_single(pdev, buf, buf_len, DMA_FROM_DEVICE);
		if (dma_mapping_error(pdev, dma_buf)) {
			dev_err(global_idev.fullmac_dev, "%s: mapping dma error!\n", __func__);
			goto func_exit;
		}
	}

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
#ifdef CONFIG_P2P
		channel = (flags > 0) ? (flags & 0x0000FFFF) : 6;
		frame_type = (u16)(flags >> 16);

		if (frame_type == IEEE80211_STYPE_PROBE_REQ) {
			if (global_idev.p2p_global.pd_pwdev && (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_PROBE_REQ))) {//P2P_DEV intf registered probe_req
				wdev =	global_idev.p2p_global.pd_pwdev;
			}
		} else if (frame_type == IEEE80211_STYPE_ACTION) {
			if (global_idev.p2p_global.pd_pwdev && memcmp((buf + 4), global_idev.p2p_global.pd_pwdev->address, 6) == 0) {
				if (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_ACTION)) {
					wdev =	global_idev.p2p_global.pd_pwdev; //DA match P2P_DEV intf, need use P2P_DEV intf to indicate
				}
			} else if (rtw_p2p_frame_is_registered(P2P_ROLE_CLIENT, IEEE80211_STYPE_ACTION)) { //GC intf has registered action report
				wdev = global_idev.pwdev_global[1];
			}
		}
#endif
		dev_dbg(global_idev.fullmac_dev, "%s: rx mgnt \n", __func__);
		/*channel need get, force 6 seems ok temporary*/
		cfg80211_rx_mgmt(wdev, rtw_ch2freq(channel), 0, buf, buf_len, 0);
	}

	if (event == WIFI_EVENT_RX_MGNT_AP) {
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
		} else if ((frame_type == IEEE80211_STYPE_ACTION) && (rtw_p2p_frame_is_registered(P2P_ROLE_DEVICE, IEEE80211_STYPE_ACTION))) {
			if (global_idev.p2p_global.pd_pwdev && (memcmp((buf + 4), global_idev.p2p_global.pd_pwdev->address, 6)) == 0) {
				wdev =  global_idev.p2p_global.pd_pwdev; //DA match P2P_DEV intf, need use P2P_DEV intf to indicate
			}
		}
#endif
		dev_dbg(global_idev.fullmac_dev, "%s: rx mgnt \n", __func__);
		cfg80211_rx_mgmt(wdev, rtw_ch2freq(channel), 0, buf, buf_len, 0);
	}

	if (event == WIFI_EVENT_OWE_PEER_KEY_RECV) {
		dev_dbg(global_idev.fullmac_dev, "%s: owe update \n", __func__);
		cfg80211_rtw_update_owe_info_event(buf, buf_len);
	}

	if (buf_len > 0) {
		dma_unmap_single(pdev, dma_buf, buf_len, DMA_FROM_DEVICE);
	}

func_exit:
	return;
}

static void llhw_event_set_netif_info(struct event_priv_t *event_priv, struct inic_ipc_dev_req_msg *p_ipc_msg)
{
	struct device *pdev = NULL;
	dma_addr_t dma_addr = 0;
	int idx = (u32)p_ipc_msg->param_buf[0];
	unsigned char *dev_addr = phys_to_virt(p_ipc_msg->param_buf[1]);
	int softap_addr_offset_idx = global_idev.wifi_user_config.softap_addr_offset_idx;

	dev_dbg(global_idev.fullmac_dev, "[fullmac]: set netif info.");

	if (!global_idev.event_ch) {
		dev_err(global_idev.fullmac_dev, "%s,%s: event_priv_t is NULL in!\n", "event", __func__);
		goto func_exit;
	}

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		dev_err(global_idev.fullmac_dev, "%s,%s: device is NULL in scan!\n", "event", __func__);
		goto func_exit;
	}

	if (idx >= INIC_MAX_NET_PORT_NUM) {
		dev_dbg(global_idev.fullmac_dev, "%s: interface %d not exist!\n", __func__, idx);
		goto func_exit;
	}

	if (!dev_addr) {
		dev_dbg(global_idev.fullmac_dev, "%s: mac address is NULL!\n", __func__);
		goto func_exit;
	}

	dma_addr = dma_map_single(pdev, dev_addr, ETH_ALEN, DMA_FROM_DEVICE);
	if (dma_mapping_error(pdev, dma_addr)) {
		dev_err(global_idev.fullmac_dev, "%s,%s: mapping dma error!\n", "event", __func__);
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

	memcpy(global_idev.pndev[idx]->dev_addr, dev_addr, ETH_ALEN);
	dev_dbg(global_idev.fullmac_dev, "MAC ADDR [%02x:%02x:%02x:%02x:%02x:%02x]", *global_idev.pndev[idx]->dev_addr,
			*(global_idev.pndev[idx]->dev_addr + 1), *(global_idev.pndev[idx]->dev_addr + 2),
			*(global_idev.pndev[idx]->dev_addr + 3), *(global_idev.pndev[idx]->dev_addr + 4),
			*(global_idev.pndev[idx]->dev_addr + 5));

	if (!global_idev.pndev[0]) {
		/*set ap port mac address*/
		memcpy(global_idev.pndev[1]->dev_addr, global_idev.pndev[0]->dev_addr, ETH_ALEN);
		global_idev.pndev[1]->dev_addr[softap_addr_offset_idx] = global_idev.pndev[0]->dev_addr[softap_addr_offset_idx] + 1;
	}
	dma_unmap_single(pdev, dma_addr, ETH_ALEN, DMA_FROM_DEVICE);

func_exit:
	return;
}

static void llhw_event_get_network_info(struct event_priv_t *event_priv, struct inic_ipc_dev_req_msg *p_ipc_msg)
{
	struct device *pdev = NULL;
	uint32_t type = (uint32_t)p_ipc_msg->param_buf[0];
	/* input is used for INIC_WLAN_IS_VALID_IP, not used now. */
	/* uint8_t *input = (uint8_t *)phys_to_virt(p_ipc_msg->param_buf[1]); */
	int idx = p_ipc_msg->param_buf[2];
	uint32_t *rsp_ptr = NULL;
	uint32_t rsp_len = 0;
	struct in_ifaddr *ifa = NULL;
	uint32_t inic_ip_addr[INIC_MAX_NET_PORT_NUM] = {0};
	uint32_t inic_ip_mask[INIC_MAX_NET_PORT_NUM] = {0};

	if (!global_idev.event_ch) {
		dev_err(global_idev.fullmac_dev, "%s: event_priv_t is NULL!\n", "event");
		goto func_exit;
	}

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		dev_err(global_idev.fullmac_dev, "%s: device is NULL!\n", "event");
		goto func_exit;
	}

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
		return;
	}

	memcpy(global_idev.event_priv.dev_req_network_info, rsp_ptr, rsp_len);
	p_ipc_msg->ret = (u32)global_idev.event_priv.dev_req_network_info_phy;

func_exit:
	return;
}

#ifdef CONFIG_NAN
static void llhw_event_nan_match_indicate(struct event_priv_t *event_priv, struct inic_ipc_dev_req_msg *p_ipc_msg)
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

static void llhw_event_nan_cfgvendor_event_indicate(struct event_priv_t *event_priv, struct inic_ipc_dev_req_msg *p_ipc_msg)
{
	struct device *pdev = NULL;
	dma_addr_t dma_event = 0;
	u8 event_id = p_ipc_msg->param_buf[0];
	unsigned char *event_addr = phys_to_virt(p_ipc_msg->param_buf[1]);
	u32 size = p_ipc_msg->param_buf[2];

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		dev_err(global_idev.fullmac_dev, "%s,%s: device is NULL in scan!\n", "event", __func__);
		goto func_exit;
	}

	dma_event = dma_map_single(pdev, event_addr, size, DMA_FROM_DEVICE);
	if (dma_mapping_error(pdev, dma_event)) {
		dev_err(global_idev.fullmac_dev, "%s: mapping dma error!\n", __func__);
		goto func_exit;
	}

	rtw_cfgvendor_nan_event_indication(event_id, event_addr, size);

	dma_unmap_single(pdev, dma_event, size, DMA_FROM_DEVICE);

func_exit:
	return;
}

static void llhw_event_nan_cfgvendor_cmd_reply(struct event_priv_t *event_priv, struct inic_ipc_dev_req_msg *p_ipc_msg)
{
	u32 size = param_buf[0];
	unsigned char *data_addr = (u8 *)&param_buf[1];

	rtw_cfgvendor_send_cmd_reply(data_addr, size);

	return;
}

#endif

void llhw_event_task(unsigned long data)
{
	dma_addr_t dma_addr = 0;
	struct event_priv_t *event_priv = &global_idev.event_priv;
	struct device *pdev = NULL;
	struct inic_ipc_dev_req_msg *p_recv_msg = NULL;
	int msg_len = 0;
#ifdef CONFIG_P2P
	struct wireless_dev *wdev = NULL;
#endif

	if (!global_idev.event_ch) {
		dev_err(global_idev.fullmac_dev, "%s: event_priv_t is NULL!\n", "event");
		goto func_exit;
	}

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		dev_err(global_idev.fullmac_dev, "%s: device is NULL!\n", "event");
		goto func_exit;
	}

	msg_len = event_priv->recv_ipc_msg.msg_len;
	if (!event_priv->recv_ipc_msg.msg || !msg_len) {
		dev_err(global_idev.fullmac_dev, "%s: Invalid device message!\n", "event");
		goto func_exit;
	}
	p_recv_msg = phys_to_virt(event_priv->recv_ipc_msg.msg);
	dma_addr = dma_map_single(pdev, p_recv_msg, sizeof(struct inic_ipc_dev_req_msg), DMA_FROM_DEVICE);
	if (dma_mapping_error(pdev, dma_addr)) {
		dev_err(global_idev.fullmac_dev, "%s: device is NULL!\n", "event");
		goto func_exit;
	}

	switch (p_recv_msg->enevt_id) {
	/* receive callback indication */
	case INIC_API_SCAN_USER_CALLBACK:
#ifdef CONFIG_P2P
		if (global_idev.p2p_global.roch_onging) {
			global_idev.p2p_global.roch_onging = 0;
			if (p_recv_msg->param_buf[1] == 2) {/*scan_userdata=2 means using P2P Device intf*/
				wdev = global_idev.p2p_global.pd_pwdev;
			} else {
				wdev = global_idev.pwdev_global[p_recv_msg->param_buf[1]];
			}
			cfg80211_remain_on_channel_expired(wdev, global_idev.p2p_global.roch_cookie, &global_idev.p2p_global.roch,
											   GFP_KERNEL);
			break;
		}
#endif
		/* If user callback provided as NULL, param_buf[1] appears NULL here. Do not make ptr. */
		/* https://jira.realtek.com/browse/AMEBAD2-1543 */
		cfg80211_rtw_scan_done_indicate(p_recv_msg->param_buf[0], NULL);
		break;
	case INIC_API_IP_ACS:
		cfg80211_rtw_set_acs_info(p_recv_msg);
		break;
	case INIC_API_SCAN_EACH_REPORT_USER_CALLBACK:
		//iiha_scan_each_report_cb_hdl(event_priv, p_recv_msg);
		break;
	case INIC_API_AP_CH_SWITCH:
		//iiha_ap_ch_switch_hdl(event_priv, p_recv_msg);
		break;
	case INIC_API_HDL:
		llhw_event_join_status_indicate(event_priv, p_recv_msg);
		break;
	case INIC_API_PROMISC_CALLBACK:
		//iiha_wifi_promisc_hdl(event_priv, p_recv_msg);
		break;
	case INIC_API_GET_LWIP_INFO:
		llhw_event_get_network_info(event_priv, p_recv_msg);
		break;
	case INIC_API_SET_NETIF_INFO:
		llhw_event_set_netif_info(event_priv, p_recv_msg);
		break;
	case INIC_API_CFG80211_SCAN_REPORT:
		llhw_event_scan_report_indicate(event_priv, p_recv_msg);
		break;
#ifdef CONFIG_NAN
	case INIC_API_CFG80211_NAN_REPORT_MATCH_EVENT:
		llhw_event_nan_match_indicate(event_priv, p_recv_msg);
		break;
	case INIC_API_CFG80211_NAN_DEL_FUNC:
		cfg80211_rtw_nan_func_free(p_recv_msg->param_buf[0]);
		break;
	case INIC_API_CFG80211_NAN_CFGVENDOR_EVENT:
		llhw_event_nan_cfgvendor_event_indicate(event_priv, p_recv_msg);
		break;
	case INIC_API_CFG80211_NAN_CFGVENDOR_CMD_REPLY:
		llhw_event_nan_cfgvendor_event_indicate(event_priv, p_recv_msg);
		break;
#endif
#ifdef CONFIG_P2P
	case INIC_API_CFG80211_P2P_CH_RDY:
		if (p_recv_msg->param_buf[0] == 2) {/*scan_userdata=2 means using P2P Device intf*/
			wdev = global_idev.p2p_global.pd_pwdev;
		} else {
			wdev = global_idev.pwdev_global[p_recv_msg->param_buf[0]];
		}
		cfg80211_ready_on_channel(wdev, global_idev.p2p_global.roch_cookie, &global_idev.p2p_global.roch,
								  global_idev.p2p_global.roch_duration, GFP_KERNEL);
		break;
#endif
	default:
		dev_err(global_idev.fullmac_dev, "%s: Unknown Device event(%d)!\n\r", "event", p_recv_msg->enevt_id);
		break;
	}

	/*set enevt_id to 0 to notify NP that event is finished*/
	p_recv_msg->enevt_id = INIC_API_PROCESS_DONE;
	dma_sync_single_for_device(pdev, dma_addr, sizeof(struct inic_ipc_dev_req_msg), DMA_TO_DEVICE);
	dma_unmap_single(pdev, dma_addr, sizeof(struct inic_ipc_dev_req_msg), DMA_TO_DEVICE);

func_exit:
	return;
}

static u32 llhw_ipc_event_interrupt(aipc_ch_t *ch, ipc_msg_struct_t *pmsg)
{
	struct event_priv_t *event_priv = &global_idev.event_priv;
	u32 ret = 0;

	if (!event_priv) {
		dev_err(global_idev.fullmac_dev, "%s: event_priv_t is NULL in interrupt!\n", "event");
		goto func_exit;
	}

	/* copy ipc_msg from temp memory in ipc interrupt. */
	memcpy((u8 *) & (event_priv->recv_ipc_msg), (u8 *)pmsg, sizeof(ipc_msg_struct_t));
	tasklet_schedule(&(event_priv->api_tasklet));

func_exit:
	return ret;
}

int llhw_event_init(struct inic_device *idev)
{
	struct event_priv_t	*event_priv = &global_idev.event_priv;
	aipc_ch_t		*event_ch = global_idev.event_ch;

	/* initialize the mutex to send event_priv message. */
	mutex_init(&(event_priv->iiha_send_mutex));

	event_priv->preq_msg = dmam_alloc_coherent(event_ch->pdev, sizeof(struct inic_ipc_host_req_msg), &event_priv->req_msg_phy_addr, GFP_KERNEL);
	if (!event_priv->preq_msg) {
		dev_err(global_idev.fullmac_dev, "%s: allloc req_msg error.\n", "event");
		return -ENOMEM;
	}

	/* coherent alloc some non-cache memory for transmit network_info to NP */
	event_priv->dev_req_network_info = dmam_alloc_coherent(event_ch->pdev, DEV_REQ_NETWORK_INFO_MAX_LEN, &event_priv->dev_req_network_info_phy, GFP_KERNEL);
	if (!event_priv->dev_req_network_info) {
		dev_err(global_idev.fullmac_dev, "%s: allloc dev_req_network_info error.\n", "event");
		return -ENOMEM;
	}

	/* initialize event tasklet */
	tasklet_init(&(event_priv->api_tasklet), llhw_event_task, (unsigned long)event_priv);

	return 0;
}

void llhw_event_deinit(void)
{
	struct event_priv_t *event_priv = &global_idev.event_priv;

	/* free sema to wakeup the message queue task */
	tasklet_kill(&(event_priv->api_tasklet));

	dma_free_coherent(global_idev.ipc_dev, DEV_REQ_NETWORK_INFO_MAX_LEN,
					  event_priv->dev_req_network_info, event_priv->dev_req_network_info_phy);
	dma_free_coherent(global_idev.ipc_dev, sizeof(struct inic_ipc_host_req_msg), event_priv->preq_msg, event_priv->req_msg_phy_addr);

	/* deinitialize the mutex to send event_priv message. */
	mutex_destroy(&(event_priv->iiha_send_mutex));

	return;
}

struct aipc_ch_ops llhw_ipc_event_ops = {
	.channel_recv = llhw_ipc_event_interrupt,
};
