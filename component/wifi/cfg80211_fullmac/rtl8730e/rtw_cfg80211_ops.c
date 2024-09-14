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

static struct wps_str wps_info;

static int cfg80211_rtw_get_station(struct wiphy *wiphy, struct net_device *ndev, const u8 *mac, struct station_info *sinfo)
{
	int ret = 0;
	struct _rtw_phy_statistics_t *statistic_vir = NULL;
	dma_addr_t statistic_phy;

	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);

	if (rtw_netdev_idx(ndev) != 0) {
		dev_dbg(global_idev.fullmac_dev, "Only net device-0 is used for STA.");
	}

	statistic_vir = rtw_malloc(sizeof(struct _rtw_phy_statistics_t), &statistic_phy);
	if (!statistic_vir) {
		dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		return -ENOMEM;
	}

	ret = llhw_wifi_get_statistics(statistic_phy);

	sinfo->filled |= BIT(NL80211_STA_INFO_SIGNAL);
	sinfo->signal = statistic_vir->rssi;

	sinfo->filled |= BIT(NL80211_STA_INFO_TX_BITRATE);
	sinfo->txrate.legacy = statistic_vir->supported_max_rate;

	rtw_mfree(sizeof(struct _rtw_phy_statistics_t), statistic_vir, statistic_phy);

	return ret;
}

static int cfg80211_rtw_change_iface(struct wiphy *wiphy, struct net_device *ndev, enum nl80211_iftype type, struct vif_params *params)
{
	u32 widx = rtw_netdev_idx(ndev);

	/* The port0 workes as sation only in fullmac firmware and port1 workes
	 * as AP mode only. So to add this conditon to stop to change the mode
	 * of interface. */
	if ((widx == 0) && ((type == NL80211_IFTYPE_AP) || (type == NL80211_IFTYPE_AP_VLAN))) {
		dev_err(global_idev.fullmac_dev, "Port0 no AP mode!\n");
		return -EPERM;
	} else if ((widx == 1) && (type == NL80211_IFTYPE_STATION)) {
		dev_err(global_idev.fullmac_dev, "Port1 no STA mode!\n");
		return -EPERM;
	}
#ifdef CONFIG_NAN
	if (ndev->ieee80211_ptr->iftype == NL80211_IFTYPE_NAN) {
		return 0;
	}
#endif
#ifdef CONFIG_P2P
	if (type == NL80211_IFTYPE_P2P_GO) {
		if (ndev_to_wdev(ndev)->iftype == NL80211_IFTYPE_P2P_CLIENT) { /*change from GC to GO*/
			rtw_p2p_gc_intf_revert(0);
		}
		global_idev.p2p_global.p2p_role = P2P_ROLE_GO;
		llhw_wifi_set_p2p_role(P2P_ROLE_GO);
	} else if (type == NL80211_IFTYPE_P2P_CLIENT) {
		if (ndev_to_wdev(ndev)->iftype == NL80211_IFTYPE_P2P_GO) {
			/*change from GO to GC: below setting not suitable when STA port connected)*/
			global_idev.p2p_global.pd_wlan_idx = 1;
			rtw_p2p_driver_macaddr_switch();
		}
		global_idev.p2p_global.p2p_role = P2P_ROLE_CLIENT;
		llhw_wifi_set_p2p_role(P2P_ROLE_CLIENT);
	}
#endif

	ndev->ieee80211_ptr->iftype = type;
	return 0;
}

int cfg80211_rtw_scan_done_indicate(unsigned int scanned_AP_num, void *user_data)
{
	struct cfg80211_scan_info info;

	if (!rtw_netdev_priv_is_on(global_idev.pndev[0]) && !rtw_netdev_priv_is_on(global_idev.pndev[1])) {
		dev_dbg(global_idev.fullmac_dev, "sta is down, finish scan.");
		return -1;
	}

	if (!global_idev.mlme_priv.pscan_req_global) {
		dev_dbg(global_idev.fullmac_dev, "Last scan req has been finished. Wait for next. ");
		return -1;
	}

	memset(&info, 0, sizeof(info));
	info.aborted = 0;
	dev_dbg(global_idev.fullmac_dev, "%s: scan request(%p) done.", __FUNCTION__, global_idev.mlme_priv.pscan_req_global);
	cfg80211_scan_done(global_idev.mlme_priv.pscan_req_global, &info);

	/* cfg80211_scan_done will clear last request. Clean global scan request as well. */
	global_idev.mlme_priv.pscan_req_global = NULL;
	global_idev.mlme_priv.b_in_scan = false;

#ifndef CONFIG_FULLMAC_HCI_IPC
	/* wakeup xmit thread if there are pending packets */
	if (llhw_xmit_pending_q_num() > 0) {
		llhw_xmit_wakeup_thread();
	}
#endif

	return 0;
}

void cfg80211_rtw_inform_bss(u32 channel, u32 frame_is_bcn, s32 rssi, u8 *mac_addr, u8 *IEs, u32 ie_len)
{
	struct ieee80211_channel *notify_channel = NULL;
	struct cfg80211_bss *bss = NULL;
	s32 notify_signal;
	u8 *pbuf;
	size_t buf_size = 1000;
	size_t len, bssinf_len = 0;
	struct rtw_ieee80211_hdr_3addr *pwlanhdr;
	unsigned short *fctrl;
	u8  bc_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	struct wiphy *wiphy = global_idev.pwiphy_global;
	struct ieee80211_supported_band *sband;
	int i, j;

	pbuf = kzalloc(buf_size, in_interrupt() ? GFP_ATOMIC : GFP_KERNEL);
	if (pbuf == NULL) {
		dev_dbg(global_idev.fullmac_dev, "%s pbuf allocate failed	!!\n", __FUNCTION__);
		return;
	}

	bssinf_len = ie_len + sizeof(struct rtw_ieee80211_hdr_3addr);
	if (bssinf_len > buf_size) {
		dev_dbg(global_idev.fullmac_dev, "%s IE Length too long > %zu byte\n", __FUNCTION__, buf_size);
		goto exit;
	}

	for (i = 0; i < 2; i++) {
		sband = wiphy->bands[i];
		if (!sband) {
			continue;
		}

		for (j = 0; j < sband->n_channels; j++) {
			if (sband->channels[j].hw_value == channel) {
				notify_channel = &sband->channels[j];
				break;
			}
		}
		if (notify_channel) {
			break;
		}
	}

	notify_signal = 100 * rssi;

	pwlanhdr = (struct rtw_ieee80211_hdr_3addr *)pbuf;
	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	if (frame_is_bcn) { /* WIFI_BEACON */
		memcpy(pwlanhdr->addr1, bc_addr, ETH_ALEN);
		set_frame_sub_type(pbuf, BIT(7));
	} else {
		memcpy(pwlanhdr->addr1, global_idev.pndev[0]->dev_addr, ETH_ALEN);
		set_frame_sub_type(pbuf, BIT(6) | BIT(4));
	}

	memcpy(pwlanhdr->addr2, mac_addr, ETH_ALEN);
	memcpy(pwlanhdr->addr3, mac_addr, ETH_ALEN);

	len = sizeof(struct rtw_ieee80211_hdr_3addr);
	memcpy((pbuf + len), IEs, ie_len);
	len += ie_len;

	bss = cfg80211_inform_bss_frame(wiphy, notify_channel, (struct ieee80211_mgmt *)pbuf, len, notify_signal, GFP_ATOMIC);

	if (!bss) {
		goto exit;
	}

	cfg80211_put_bss(wiphy, bss);

exit:
	if (pbuf) {
		kfree(pbuf);
	}
}

static int cfg80211_rtw_scan(struct wiphy *wiphy, struct cfg80211_scan_request *request)
{
	int i;
	int ret = 0;
	struct cfg80211_ssid *ssids = request->ssids;
	struct wireless_dev *wdev;
	struct element *target_ptr = NULL;
	u32 wlan_idx = 0;
	struct net_device *pnetdev = NULL;
	struct _rtw_scan_param_t scan_param = {0};
	struct cfg80211_scan_info info;

	/* coherent alloc: revise vir addr will be mapped to phy addr. Send phy addr to rtos by ipc. */
	u8 *channel_list_vir = NULL;
	dma_addr_t channel_list_phy;

	/* coherent alloc: revise vir addr will be mapped to phy addr. Send phy addr to rtos by ipc. */
	u8 *ssid_vir = NULL;
	dma_addr_t ssid_phy;

	u32 ssid_len = 0;
	u32 channel_num = 0;

	if (global_idev.mp_fw) {
		return -EPERM;
	}

	wdev = request->wdev;
	pnetdev = wdev_to_ndev(wdev);
#ifdef CONFIG_P2P
	if (wdev == global_idev.p2p_global.pd_pwdev) {
		wlan_idx = global_idev.p2p_global.pd_wlan_idx;
	} else
#endif
		if (pnetdev) {
			wlan_idx = rtw_netdev_idx(pnetdev);
		} else {
			ret = -EINVAL;
			goto exit;
		}

	dev_dbg(global_idev.fullmac_dev, "cfg80211_rtw_scan enter\n");

	memset(&scan_param, 0, sizeof(struct _rtw_scan_param_t));

	/* Add fake callback to inform rots give scan indicate when scan done. */
	scan_param.scan_user_callback = (enum _rtw_result_t (*)(unsigned int,  void *))0xffffffff;
	scan_param.ssid = NULL;
#ifdef CONFIG_P2P
	scan_param.scan_user_data = (void *)(uintptr_t)rtw_p2p_get_wdex_idx(wdev); /*for later cfg80211 indicate*/
	if ((wdev == global_idev.p2p_global.pd_pwdev) && (wlan_idx == 1)) {
		scan_param.options |= RTW_SCAN_WITH_P2P;
	}
#endif
	for (i = 0; i < request->n_ssids && ssids && i < RTW_SSID_SCAN_AMOUNT; i++) {
		if (ssids[i].ssid_len) {
			ssid_vir = rtw_malloc(ssids[0].ssid_len, &ssid_phy);
			if (!ssid_vir) {
				dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
				return -ENOMEM;
			}
			memcpy(ssid_vir, ssids[0].ssid, ssids[0].ssid_len);
			ssid_len = ssids[0].ssid_len;
			/* Only hidden ssid scan has ssid param. */
			scan_param.ssid = (char *)ssid_phy;
#ifdef CONFIG_P2P
			if (memcmp(ssids[i].ssid, "DIRECT-", 7) == 0) {
				target_ptr = (struct element *)cfg80211_find_vendor_ie(WLAN_OUI_WFA, WLAN_OUI_TYPE_WFA_P2P, request->ie, request->ie_len);
				if (target_ptr) {
					if (global_idev.p2p_global.p2p_role == P2P_ROLE_DISABLE) { /* enable P2P role first */
						global_idev.p2p_global.p2p_role = P2P_ROLE_DEVICE;
						llhw_wifi_set_p2p_role(P2P_ROLE_DEVICE);
					}
					llhw_wifi_set_gen_ie(wlan_idx, (u8 *)target_ptr, ((u16)target_ptr->datalen + 2), P2PWPS_PROBE_REQ_IE);
				}
			}
#endif
			/* Multi ssid need other operation here. Only support one here. */
			break;
		}
	}

	/* no ssid entry, set the scan type as passive */
	if (request->n_ssids == 0) {
		scan_param.options = RTW_SCAN_PASSIVE;
	}

	if (request->n_channels) {
		channel_list_vir = rtw_malloc(request->n_channels, &channel_list_phy);
		if (!channel_list_vir) {
			dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
			if (ssid_vir) {
				rtw_mfree(ssids[0].ssid_len, ssid_vir, ssid_phy);
			}
			return -ENOMEM;
		}
	}

	for (i = 0; i < request->n_channels && i < RTW_CHANNEL_SCAN_AMOUNT; i++) {
		if (request->channels[i]->flags & IEEE80211_CHAN_DISABLED) {
			continue;
		}
		*(channel_list_vir + channel_num) = request->channels[i]->hw_value;
		channel_num++;
	}
	scan_param.channel_list_num = channel_num;
	if (request->n_channels) {
		/* If channel list exists. */
		scan_param.channel_list = (unsigned char *)channel_list_phy;
	}

	target_ptr = (struct element *)cfg80211_find_vendor_ie(WLAN_OUI_MICROSOFT, WLAN_OUI_TYPE_MICROSOFT_WPS, request->ie, request->ie_len);
	if (target_ptr) {
		wps_info.wps_probe_ielen = (u16)target_ptr->datalen + 2;
		memcpy(wps_info.wps_probe_ie, target_ptr, wps_info.wps_probe_ielen);
		llhw_wifi_set_gen_ie(wlan_idx, wps_info.wps_probe_ie, wps_info.wps_probe_ielen, P2PWPS_PROBE_REQ_IE);
	}

	if (global_idev.mlme_priv.b_in_scan) {
		memset(&info, 0, sizeof(info));
		info.aborted = 0;
		cfg80211_scan_done(request, &info);
		dev_dbg(global_idev.fullmac_dev, "%s: scan is in progress..", __FUNCTION__);
	} else {
		global_idev.mlme_priv.b_in_scan = true;

		ret = llhw_wifi_scan(&scan_param, ssid_len, 0);
		if (ret < 0) {
			memset(&info, 0, sizeof(info));
			info.aborted = 0;
			cfg80211_scan_done(request, &info);
			dev_dbg(global_idev.fullmac_dev, "%s: scan request(%p) fail.", __FUNCTION__, request);

			global_idev.mlme_priv.b_in_scan = false;
			global_idev.mlme_priv.pscan_req_global = NULL;

#ifndef CONFIG_FULLMAC_HCI_IPC
			/* wakeup xmit thread if there are pending packets */
			if (llhw_xmit_pending_q_num() > 0) {
				llhw_xmit_wakeup_thread();
			}
#endif
		} else {
			global_idev.mlme_priv.pscan_req_global = request;
			dev_dbg(global_idev.fullmac_dev, "%s: scan request(%p) start.", __FUNCTION__, request);
		}
	}

	if (ssid_vir) {
		rtw_mfree(ssids[0].ssid_len, ssid_vir, ssid_phy);
	}
	if (channel_list_vir) {
		rtw_mfree(request->n_channels, channel_list_vir, channel_list_phy);
	}

exit:
	return ret;
}


static void cfg80211_rtw_abort_scan(struct wiphy *wiphy, struct wireless_dev *wdev)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);
}

static int cfg80211_rtw_set_wiphy_params(struct wiphy *wiphy, u32 changed)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);
	return 0;
}

static int cfg80211_rtw_join_ibss(struct wiphy *wiphy, struct net_device *ndev,
								  struct cfg80211_ibss_params *params)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);
	return 0;
}

static int cfg80211_rtw_leave_ibss(struct wiphy *wiphy, struct net_device *ndev)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);
	return 0;
}

void cfg80211_rtw_connect_indicate(unsigned int join_status, void *user_data, size_t user_data_len)
{
	struct mlme_priv_t *mlme_priv = &global_idev.mlme_priv;
	u8 wlan_idx = 0;

	mlme_priv->rtw_join_status = join_status;

	/* Merge from wifi_join_status_indicate. */
	if (join_status == RTW_JOINSTATUS_SUCCESS) {
		/* if Synchronous connection, up sema when connect success */
		if (mlme_priv->join_block_param && mlme_priv->join_block_param->block) {
			complete(&mlme_priv->join_block_param->join_sema);
		}
	}

	/* step 1: internal process for different status*/
	if (join_status == RTW_JOINSTATUS_STARTING) {
		dev_dbg(global_idev.fullmac_dev, "[fullmac] --- %s --- join starting", __func__);
		memset(mlme_priv->assoc_req_ie, 0, ASSOC_IE_MAX_LEN);
		memset(mlme_priv->assoc_rsp_ie, 0, ASSOC_IE_MAX_LEN);
		mlme_priv->assoc_req_ie_len = 0;
		mlme_priv->assoc_rsp_ie_len = 0;
		return;
	}

	if (join_status == RTW_JOINSTATUS_ASSOCIATING) {
		if (user_data_len > 0) {
			memcpy(mlme_priv->assoc_req_ie, (u8 *)user_data, user_data_len);
			mlme_priv->assoc_req_ie_len = user_data_len;
		}
		return;
	}

	if (join_status == RTW_JOINSTATUS_ASSOCIATED) {
		if (user_data_len > 0) {
			memcpy(mlme_priv->assoc_rsp_ie, (u8 *)user_data, user_data_len);
			mlme_priv->assoc_rsp_ie_len = user_data_len;
			dev_dbg(global_idev.fullmac_dev, "[fullmac] --- %s --- %s success.", __func__, (*(u8 *)mlme_priv->assoc_req_ie == 0 ? "association" : "re-association"));
#ifdef CONFIG_P2P
			if ((global_idev.p2p_global.pd_wlan_idx == 1) && (memcmp(mlme_priv->assoc_rsp_ie + 4, global_idev.pndev[1]->dev_addr, 6) == 0)) {
				wlan_idx = 1; //address match GC, then use GC's netdev
			}
#endif
			/* Inform connect information. */
			/* Different between cfg80211_connect_result and cfg80211_connect_bss are described in net/cfg80211.h. */
			/* if connect_result warning, that means get_bss fail (need check), one reason is WPA_S calls disconnect ops, which resulting in wdev->ssid_len = 0 */
			cfg80211_connect_result(global_idev.pndev[wlan_idx], mlme_priv->assoc_rsp_ie + 16,
									mlme_priv->assoc_req_ie + WLAN_HDR_A3_LEN + 4/* fixed parameters */
									+ (*(u8 *)mlme_priv->assoc_req_ie == 0 ? 0 : 6)/* re-assoc: current ap */,
									mlme_priv->assoc_req_ie_len - WLAN_HDR_A3_LEN - 4 - (*(u8 *)mlme_priv->assoc_req_ie == 0 ? 0 : 6),
									mlme_priv->assoc_rsp_ie + WLAN_HDR_A3_LEN + 6, mlme_priv->assoc_rsp_ie_len -  WLAN_HDR_A3_LEN - 6,
									WLAN_STATUS_SUCCESS, GFP_ATOMIC);
			netif_carrier_on(global_idev.pndev[wlan_idx]);
		}
		return;
	}

	if (join_status == RTW_JOINSTATUS_FAIL) {
#ifdef CONFIG_P2P
		if (global_idev.p2p_global.pd_wlan_idx == 1) {
			wlan_idx = 1;// GC intf is up, then use GC's netdev
		}
#endif
		dev_dbg(global_idev.fullmac_dev, "[fullmac] --- %s --- join failed up sema.", __func__);
		/* merge from wifi_join_status_indicate if synchronous connection, up sema when connect fail*/
		if (mlme_priv->join_block_param && mlme_priv->join_block_param->block) {
			complete(&mlme_priv->join_block_param->join_sema);
		}
		dev_dbg(global_idev.fullmac_dev, "[fullmac] --- %s --- join failed inform cfg80211.", __func__);
		cfg80211_connect_result(global_idev.pndev[wlan_idx], NULL, NULL, 0, NULL, 0, WLAN_STATUS_UNSPECIFIED_FAILURE, GFP_ATOMIC);
		return;
	}
}

void cfg80211_rtw_disconnect_indicate(u16 reason, u8 locally_generated)
{
	u8 wlan_idx = 0;
#ifdef CONFIG_P2P
	if (global_idev.p2p_global.p2p_role == P2P_ROLE_CLIENT) {
		wlan_idx = 1;// GC intf is up, then use GC's netdev
	}
#endif

	if (global_idev.pndev[wlan_idx] != NULL) {
		/* Do it first for tx broadcast pkt after disconnection issue! */
		netif_carrier_off(global_idev.pndev[wlan_idx]);
		dev_dbg(global_idev.fullmac_dev, "%s reason:%d\n", __func__, reason);
		cfg80211_disconnected(global_idev.pndev[wlan_idx], reason, NULL, 0, locally_generated, GFP_ATOMIC);
	}
}

void cfg80211_rtw_external_auth_request(char *buf, int buf_len)
{
	struct cfg80211_external_auth_params *auth_ext_para = &global_idev.mlme_priv.auth_ext_para;
	struct wpa_sae_param_t *wpa_sae_param = (struct wpa_sae_param_t *)buf;

	auth_ext_para->action = NL80211_EXTERNAL_AUTH_START;
	memcpy(auth_ext_para->bssid, wpa_sae_param->peer_mac, ETH_ALEN);
	auth_ext_para->key_mgmt_suite = 0x8ac0f00;

	/*ap mode doesn't need call this ops, sta will trigger auth*/
	cfg80211_external_auth_request(global_idev.pndev[0], auth_ext_para, GFP_ATOMIC);
	dev_dbg(global_idev.fullmac_dev, "%s, ssid=%s, len=%d\n", __func__, auth_ext_para->ssid.ssid, auth_ext_para->ssid.ssid_len);
}

void cfg80211_rtw_update_owe_info_event(char *buf, int buf_len)
{
	struct cfg80211_update_owe_info owe_info;
	struct rtw_owe_param_t *owe_param = (struct rtw_owe_param_t *)buf;
	char owe_ie[RTW_OWE_KEY_LEN + 5];

	owe_ie[0] = WLAN_EID_EXTENSION;
	owe_ie[1] = RTW_OWE_KEY_LEN + 3; /*WLAN_EID_EXT_OWE_DH_PARAM + group*/
	owe_ie[2] = WLAN_EID_EXT_OWE_DH_PARAM;
	memcpy(&owe_ie[3], &owe_param->group, owe_param->pub_key_len + 2);
	owe_info.ie = (u8 *)&owe_ie;
	memcpy(owe_info.peer,  owe_param->peer_mac, ETH_ALEN);
	owe_info.status = WLAN_STATUS_SUCCESS;
	owe_info.ie_len = RTW_OWE_KEY_LEN + 5;
	cfg80211_update_owe_info_event(global_idev.pndev[0], &owe_info, GFP_ATOMIC);
}

static int cfg80211_rtw_connect(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_connect_params *sme)
{
	int ret = 0;
	struct _rtw_network_info_t connect_param = {0};
	u32 wlan_idx = 0;
	/* coherent alloc: revise vir addr will be mapped to phy addr. Send phy addr to rtos by ipc. */
	u8 *vir_pwd = NULL;
	u8 pwd[] = "12345678";
	dma_addr_t phy_pwd;
	int rsnx_ielen = 0, rsn_ielen = 0;
	u8 pmf_mode = 0, spp_opt = 0;
	u8 *prsnx, *prsn;
	struct element *target_ptr;
	struct rtw_owe_param_t owe_info;
	struct cfg80211_external_auth_params *auth_ext_para = &global_idev.mlme_priv.auth_ext_para;

	if (global_idev.mp_fw) {
		return -EPERM;
	}

	if (rtw_netdev_idx(ndev) == 1) {
		return -EPERM;
	}

	if (sme->bssid == NULL) {
		dev_err(global_idev.fullmac_dev, "=> bssid is NULL!\n");
		ret = -EINVAL;
		goto exit;
	}

	if (ndev) {
		wlan_idx = rtw_netdev_idx(ndev);
	} else {
		ret = -EINVAL;
		goto exit;
	}

	dev_dbg(global_idev.fullmac_dev, "=>"FUNC_NDEV_FMT" - Start to Connection\n", FUNC_NDEV_ARG(ndev));
	dev_dbg(global_idev.fullmac_dev,
			"ssid=%s, ssid_len=%ld, freq=%d, bssid=[0x%x:0x%x:0x%x:0x%x:0x%x:0x%x], privacy=%d, key=%p, key_len=%d, key_idx=%d, auth_type=%d\n",
			sme->ssid, sme->ssid_len, sme->channel->center_freq,
			sme->bssid[0], sme->bssid[1], sme->bssid[2], sme->bssid[3], sme->bssid[4], sme->bssid[5],
			sme->privacy, sme->key, sme->key_len, sme->key_idx, sme->auth_type);
	dev_dbg(global_idev.fullmac_dev, "wpa_versions=0x%x, ciphers_pairwise=0x%x, cipher_group=0x%x,, akm_suites=0x%x\n",
			sme->crypto.wpa_versions, sme->crypto.ciphers_pairwise[0], sme->crypto.cipher_group, sme->crypto.akm_suites[0]);

	memset(&connect_param, 0, sizeof(struct _rtw_network_info_t));
	connect_param.security_type = 0;
	if (sme->crypto.wpa_versions & NL80211_WPA_VERSION_3) {
		connect_param.security_type |= WPA3_SECURITY;
	}
	if (sme->crypto.wpa_versions & NL80211_WPA_VERSION_2) {
		connect_param.security_type |= WPA2_SECURITY;
		llhw_wifi_set_wpa_mode(WPA2_ONLY_MODE);
	}
	if (sme->crypto.wpa_versions & NL80211_WPA_VERSION_1) {
		connect_param.security_type |= WPA_SECURITY;
		llhw_wifi_set_wpa_mode(WPA_ONLY_MODE);
	}

	if (sme->auth_type == NL80211_AUTHTYPE_SHARED_KEY) {
		connect_param.security_type |= SHARED_ENABLED;
	}

	if ((sme->crypto.ciphers_pairwise[0] == WLAN_CIPHER_SUITE_WEP40)
		|| (sme->crypto.ciphers_pairwise[0] == WLAN_CIPHER_SUITE_WEP104)) {
		connect_param.security_type |= WEP_ENABLED;
	} else if ((sme->crypto.ciphers_pairwise[0] == WLAN_CIPHER_SUITE_CCMP)
			   || (sme->crypto.ciphers_pairwise[0] == WLAN_CIPHER_SUITE_CCMP_256)) {
		connect_param.security_type |= AES_ENABLED;
	} else if (sme->crypto.ciphers_pairwise[0] == WLAN_CIPHER_SUITE_TKIP) {
		connect_param.security_type |= TKIP_ENABLED;
	} else if (sme->crypto.ciphers_pairwise[0] == WLAN_CIPHER_SUITE_AES_CMAC) {
		connect_param.security_type |= AES_CMAC_ENABLED;
	}

	if (sme->want_1x || (sme->auth_type == NL80211_AUTHTYPE_NETWORK_EAP)) {
		connect_param.security_type |= ENTERPRISE_ENABLED;
	}

	/* Do not change open and WEP password from upper layer. */
	if (connect_param.security_type && !(connect_param.security_type & WEP_ENABLED)) {
		sme->key_len = 8;
		sme->key = pwd;
	}

	memcpy(connect_param.ssid.val, (u8 *)sme->ssid, sme->ssid_len);
	connect_param.ssid.len = sme->ssid_len;

	memset(connect_param.prev_bssid.octet, 0, ETH_ALEN);
	if (sme->prev_bssid) {
		dev_dbg(global_idev.fullmac_dev,
				"Former AP [0x%x:0x%x:0x%x:0x%x:0x%x:0x%x], Re-Association Request to [0x%x:0x%x:0x%x:0x%x:0x%x:0x%x].\n",
				sme->prev_bssid[0], sme->prev_bssid[1], sme->prev_bssid[2], sme->prev_bssid[3], sme->prev_bssid[4], sme->prev_bssid[5],
				sme->bssid[0], sme->bssid[1], sme->bssid[2], sme->bssid[3], sme->bssid[4], sme->bssid[5]);
		memcpy(connect_param.prev_bssid.octet, sme->prev_bssid, ETH_ALEN);
		/* Stop upper-layer-data-queue because of re-association. */
		netif_carrier_off(global_idev.pndev[0]);
	}

	if (sme->crypto.akm_suites[0] ==  WIFI_AKM_SUITE_OWE) {
		connect_param.security_type = 0;
		target_ptr = (struct element *)cfg80211_find_ext_elem(WLAN_EID_EXT_OWE_DH_PARAM, sme->ie, sme->ie_len);
		if (target_ptr) {
			memcpy(&owe_info, target_ptr->data + 1, target_ptr->datalen - 1);
			owe_info.pub_key_len = target_ptr->datalen - 3;
			if (owe_info.pub_key_len == RTW_OWE_KEY_LEN) {
				llhw_wifi_set_owe_param(&owe_info);
			} else {
				/* disconent when ask for group 20, 21 */
				cfg80211_connect_result(ndev, NULL, NULL, 0, NULL, 0, WLAN_STATUS_UNSPECIFIED_FAILURE, GFP_ATOMIC);
				return -EINVAL;
			}
		}
	}

	if (sme->key_len) {
		vir_pwd = rtw_malloc(sme->key_len, &phy_pwd);
		if (!vir_pwd) {
			dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
			return -ENOMEM;
		}
		memcpy(vir_pwd, (u8 *)sme->key, sme->key_len);
		connect_param.password = (unsigned char *)phy_pwd;
	} else {
		connect_param.password = NULL;
	}

	if (sme->crypto.akm_suites[0] ==  WIFI_AKM_SUITE_SAE) {
		/*SAE need request wpa_suppilcant to auth*/
		memcpy(auth_ext_para->ssid.ssid, (u8 *)sme->ssid, sme->ssid_len);
		auth_ext_para->ssid.ssid_len = sme->ssid_len;
		llhw_wifi_set_wpa_mode(WPA3_ONLY_MODE);
	}

	connect_param.password_len = sme->key_len;
	connect_param.key_id = sme->key_idx;
	/* set channel to let low level do scan on specific channel */
	connect_param.channel = rtw_freq2ch(sme->channel->center_freq);
	connect_param.pscan_option = 0x2;

	/* do connect by bssid */
	memset(connect_param.bssid.octet, 0, ETH_ALEN);
	if (sme->bssid) {
		memcpy(connect_param.bssid.octet, sme->bssid, ETH_ALEN);
	}

	/* read rsn*/
	prsn = rtw_get_ie((u8 *)sme->ie, WLAN_EID_RSN, &rsn_ielen, sme->ie_len);
	if (prsn && (rsn_ielen > 0)) {
		spp_opt = rtw_get_pmf_option(prsn, rsn_ielen);
		/*no RSN Capability in RSN*/
		if (spp_opt == 0xff) {
			spp_opt = sme->mfp;
		}
	}

	/* convert nl80211_mfp to pmf_mode */
	if (spp_opt & MFPR_BIT) {
		pmf_mode = 2;
	} else if (spp_opt & MFPC_BIT) {
		pmf_mode = 1;
	} else {
		pmf_mode = 0;
	}

	ret = llhw_wifi_set_pmf_mode(pmf_mode);
	if (ret < 0) {
		/* set pmf failed */
		dev_err(global_idev.fullmac_dev, "%s: set mfp mode failed (%d).", __func__, ret);
	}

	/* set rsnxe*/
	prsnx = rtw_get_ie((u8 *)sme->ie, WLAN_EID_RSNX, &rsnx_ielen, sme->ie_len);
	if (prsnx && (rsnx_ielen > 0)) {
		if ((rsnx_ielen + 2) <= RSNXE_MAX_LEN) {
			memcpy(connect_param.wpa_supp.rsnxe_ie, prsnx, rsnx_ielen + 2);
		} else {
			printk("%s:no more buf to save RSNX Cap!rsnx_ielen=%d\n", __func__, rsnx_ielen + 2);
		}
	}

	/* set wps phase */
	target_ptr = (struct element *)cfg80211_find_vendor_ie(WLAN_OUI_MICROSOFT, WLAN_OUI_TYPE_MICROSOFT_WPS, sme->ie, sme->ie_len);
	if (target_ptr) {
		u8 wps_ie[258];
		u16 wps_ielen = 0;
		wps_info.wps_phase = 1;
		wps_ielen = (u16)target_ptr->datalen + 2;
		memcpy(wps_ie, target_ptr, wps_ielen);
		llhw_wifi_set_gen_ie(wlan_idx, wps_info.wps_probe_ie, wps_info.wps_probe_ielen, P2PWPS_PROBE_REQ_IE);
		llhw_wifi_set_gen_ie(wlan_idx, wps_ie, wps_ielen, P2PWPS_ASSOC_REQ_IE);
	} else if (wps_info.wps_phase) {
		memset(&wps_info, 0, sizeof(struct wps_str));
		connect_param.is_wps_trigger = 1;
	}
	llhw_wifi_set_wps_phase(wps_info.wps_phase);

#ifdef CONFIG_P2P
	target_ptr = (struct element *)cfg80211_find_vendor_ie(WLAN_OUI_WFA, WLAN_OUI_TYPE_WFA_P2P, sme->ie, sme->ie_len);
	if (target_ptr) {
		llhw_wifi_set_gen_ie(wlan_idx, (u8 *)target_ptr, ((u16)target_ptr->datalen + 2), P2PWPS_ASSOC_REQ_IE);
	}
#endif

	ret = llhw_wifi_connect(&connect_param, 0);
	if (ret < 0) {
		/* KM4 connect failed */
		cfg80211_connect_result(ndev, NULL, NULL, 0, NULL, 0, WLAN_STATUS_UNSPECIFIED_FAILURE, GFP_ATOMIC);
	}

	if (sme->key_len) {
		rtw_mfree(strlen(vir_pwd), vir_pwd, phy_pwd);
	}

exit:
	return ret;
}

static int cfg80211_rtw_disconnect(struct wiphy *wiphy, struct net_device *ndev, u16 reason_code)
{
	int ret = 0;

	dev_dbg(global_idev.fullmac_dev, "[fullmac] --- %s ---", __func__);

	if (global_idev.mp_fw) {
		return -EPERM;
	}

	if (rtw_netdev_idx(ndev) == 1) {
		return -EPERM;
	}

	if (wps_info.wps_phase) {
		/* KM4 disable wps */
		llhw_wifi_set_wps_phase(0);
	}

	ret = llhw_wifi_disconnect();

	/* KM4 will report WIFI_EVENT_DISCONNECT event to linux, after disconnect done */
	global_idev.mlme_priv.b_in_disconnect = true;
	wait_for_completion_interruptible(&global_idev.mlme_priv.disconnect_done_sema);

	return ret;
}

static int cfg80211_rtw_set_txpower(struct wiphy *wiphy, struct wireless_dev *wdev, enum nl80211_tx_power_setting type, int mbm)
{
	dev_dbg(global_idev.fullmac_dev, "%s set %d %d", __func__, type, mbm);

	/* Operation not permitted */
	return -EPERM;
}

static int cfg80211_rtw_get_txpower(struct wiphy *wiphy, struct wireless_dev *wdev, int *dbm)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);

	/* Operation not permitted */
	return -EPERM;
}

static int cfg80211_rtw_set_power_mgmt(struct wiphy *wiphy, struct net_device *ndev, bool enabled, int timeout)
{
	dev_dbg(global_idev.fullmac_dev, "%s: enable = %d, timeout = %d", __func__, enabled, timeout);

	if (global_idev.mp_fw) {
		return -EPERM;
	}

	/* A timeout value of -1 allows the driver to adjust the dynamic ps timeout value. Power save 2s timeout. */
	llhw_wifi_set_lps_enable(enabled);
	return 0;
}

static int cfg80211_rtw_set_monitor_channel(struct wiphy *wiphy, struct cfg80211_chan_def *chandef)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);
	return 0;
}

static int cfg80211_rtw_external_auth_status(struct wiphy *wiphy, struct net_device *dev, struct cfg80211_external_auth_params *params)
{
	if (global_idev.mp_fw) {
		return -EPERM;
	}

	if (rtw_netdev_idx(dev) == 0) {
		dev_dbg(global_idev.fullmac_dev, "[STA]: %s: auth_status=%d\n", __func__, params->status);
		/* interface change later. */
		if (params->status == WLAN_STATUS_SUCCESS) {
			llhw_wifi_sae_status_indicate(rtw_netdev_idx(dev), params->status, NULL);
		}
	} else {
		dev_dbg(global_idev.fullmac_dev, "[SoftAP]: %s: auth_status=%d\n", __func__, params->status);
		/* SoftAP is supposed to go into this interface instead of private one. */
		llhw_wifi_sae_status_indicate(rtw_netdev_idx(dev), params->status, params->bssid);
	}

	return 0;
}

#ifdef CONFIG_CFG80211_SME_OFFLOAD
static int cfg80211_rtw_auth(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_auth_request *req)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);
	return 0;
}

static int cfg80211_rtw_assoc(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_assoc_request *req)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);
	return 0;
}

static int cfg80211_rtw_deauth(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_deauth_request *req)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);
	return 0;
}

static int cfg80211_rtw_disassoc(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_disassoc_request *req)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);
	return 0;
}
#endif	/* CONFIG_CFG80211_SME_OFFLOAD */

static s32 cfg80211_rtw_remain_on_channel(struct wiphy *wiphy, struct wireless_dev *wdev, struct ieee80211_channel *channel, unsigned int duration, u64 *cookie)
{
#ifdef CONFIG_P2P
	int ret = 0;
	u32 wlan_idx = 0;
	struct net_device *pnetdev = NULL;
	struct _rtw_scan_param_t scan_param = {0};

	/* coherent alloc: revise vir addr will be mapped to phy addr. Send phy addr to rtos by ipc. */
	u8 *channel_list_vir = NULL;
	dma_addr_t channel_list_phy;

	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);

	pnetdev = wdev_to_ndev(wdev);
	if (wdev == global_idev.p2p_global.pd_pwdev) {
		wlan_idx = global_idev.p2p_global.pd_wlan_idx;
	} else if (pnetdev) {
		wlan_idx = rtw_netdev_idx(pnetdev);
	} else {
		ret = -EINVAL;
		goto exit;
	}

	llhw_wifi_scan_abort(1);

	memset(&scan_param, 0, sizeof(struct _rtw_scan_param_t));
	/* Add fake callback to inform rots give scan indicate when scan done. */
	scan_param.scan_user_callback = (enum _rtw_result_t (*)(unsigned int,  void *))0xffffffff;
	scan_param.ssid = NULL;
	scan_param.options = RTW_SCAN_PASSIVE;
	scan_param.chan_scan_time.passive_scan_time = duration;
	scan_param.scan_user_data = (void *)(uintptr_t)rtw_p2p_get_wdex_idx(wdev); /*for later cfg80211 indicate*/
	if (wlan_idx == 1) {
		scan_param.options |= RTW_SCAN_WITH_P2P;
	}

	if (channel) {
		channel_list_vir = rtw_malloc(1, &channel_list_phy);
		if (!channel_list_vir) {
			dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
			return -ENOMEM;
		}

		*(channel_list_vir) = channel->hw_value;
		scan_param.channel_list_num = 1;
		scan_param.channel_list = (unsigned char *)channel_list_phy;
	}

	global_idev.p2p_global.roch_onging = 1;
	global_idev.p2p_global.roch_duration = duration;
	memcpy(&global_idev.p2p_global.roch, channel, sizeof(struct ieee80211_channel));
	global_idev.p2p_global.roch_cookie++;
	*cookie = global_idev.p2p_global.roch_cookie;

	if (global_idev.p2p_global.p2p_role == P2P_ROLE_DISABLE) { /* enable P2P role first */
		global_idev.p2p_global.p2p_role = P2P_ROLE_DEVICE;
		llhw_wifi_set_p2p_role(P2P_ROLE_DEVICE);
	}

	llhw_wifi_set_p2p_remain_on_ch(wlan_idx, 1);
	ret = llhw_wifi_scan(&scan_param, 0, 0);
	if (ret < 0) {
		return -EBUSY;
	}


	if (channel_list_vir) {
		rtw_mfree(1, channel_list_vir, channel_list_phy);
	}

exit:
	return ret;
#else
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);
	return 0;
#endif

}

static s32 cfg80211_rtw_cancel_remain_on_channel(struct wiphy *wiphy, struct wireless_dev *wdev, u64 cookie)
{
#ifdef CONFIG_P2P
	if (global_idev.p2p_global.roch_onging) {
		llhw_wifi_scan_abort(1);
	}
	return 0;
#else
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);
	return 0;
#endif
}

static int cfg80211_rtw_mgmt_tx(struct wiphy *wiphy, struct wireless_dev *wdev, struct cfg80211_mgmt_tx_params *params, u64 *cookie)
{
	struct net_device *ndev = wdev_to_ndev(wdev);
	u8 wlan_idx = 0;
	struct ieee80211_channel *chan = params->chan;
	const u8 *buf = params->buf;
	size_t len = params->len;
	int ret = 0;
	bool ack = true;
	u8 tx_ch = 0;
	u8 frame_styp;
	static u32 mgmt_tx_cookie = 0;
	u8 need_wait_ack = 0;

	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);

	if (global_idev.mp_fw) {
		return -EPERM;
	}

#ifdef CONFIG_P2P
	if (wdev == global_idev.p2p_global.pd_pwdev) {/*P2P Device intf not have ndev, wlanidx need fetch from other way*/
		wlan_idx = global_idev.p2p_global.pd_wlan_idx;
	} else
#endif
		if (ndev) {
			wlan_idx = rtw_netdev_idx(ndev);
		}

	if (chan != NULL) {
		tx_ch = (u8)ieee80211_frequency_to_channel(chan->center_freq);
	}

	/* cookie generation */
	*cookie = mgmt_tx_cookie++;

	/* indicate ack before issue frame to avoid racing with rsp frame */
	cfg80211_mgmt_tx_status(wdev, *cookie, buf, len, ack, GFP_KERNEL);
	frame_styp = le16_to_cpu(((struct rtw_ieee80211_hdr_3addr *)buf)->frame_ctl) & IEEE80211_FCTL_STYPE;

	if (frame_styp == IEEE80211_STYPE_AUTH) {
		dev_dbg(global_idev.fullmac_dev, "wpa_s tx auth\n");
		//dev_dbg(global_idev.fullmac_dev, "tx_ch=%d, no_cck=%u, da="MAC_FMT"\n", tx_ch, no_cck, MAC_ARG(GetAddr1Ptr(buf)));
		goto dump;
	} else if (frame_styp == IEEE80211_STYPE_ACTION) {
		dev_dbg(global_idev.fullmac_dev, "issue action.\n");
		goto dump;
	}
#ifdef CONFIG_P2P
	else if (frame_styp == IEEE80211_STYPE_PROBE_RESP) {
		goto dump;
	}
#endif
	else {
		dev_dbg(global_idev.fullmac_dev, "mgmt tx frame_type:0x%x\n", frame_styp);
		return ret;
	}
dump:
#ifdef CONFIG_P2P
	if (params->dont_wait_for_ack == 0) {
		need_wait_ack = 1;
	}
#endif

	/*ignore tx_ch/ no_cck/ wait_ack temporary*/
	llhw_wifi_tx_mgnt(wlan_idx, buf, len, need_wait_ack);

	return ret;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0))
void cfg80211_rtw_mgmt_frame_register(struct wiphy *wiphy, struct wireless_dev *wdev, u16 frame_type, bool reg)
{
	//dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s reg = %d", __func__, reg);
#ifdef CONFIG_P2P
	int p2p_idx = -1;
	if (wdev->iftype == NL80211_IFTYPE_P2P_DEVICE) {
		p2p_idx = 0;
	} else if (wdev->iftype == NL80211_IFTYPE_P2P_CLIENT) {
		p2p_idx = 1;
	} else if (wdev->iftype == NL80211_IFTYPE_P2P_GO) {
		p2p_idx = 2;
	}

	if ((p2p_idx >= 0) && (frame_type == IEEE80211_STYPE_PROBE_REQ || frame_type == IEEE80211_STYPE_ACTION)) {
		if (reg > 0) {
			global_idev.p2p_global.mgmt_register[p2p_idx] |= BIT(frame_type >> 4);
		} else {
			global_idev.p2p_global.mgmt_register[p2p_idx] &= ~(BIT(frame_type >> 4));
		}
	}
#endif
}
#else
void cfg80211_rtw_update_mgmt_frame_register(struct wiphy *wiphy, struct wireless_dev *wdev, struct mgmt_frame_regs *upd)
{
#ifdef CONFIG_P2P
	u16 frame_type_mask = 0;
	int p2p_idx = -1;

	if (wdev->iftype == NL80211_IFTYPE_P2P_DEVICE) {
		p2p_idx = 0;
	} else if (wdev->iftype == NL80211_IFTYPE_P2P_CLIENT) {
		p2p_idx = 1;
	} else if (wdev->iftype == NL80211_IFTYPE_P2P_GO) {
		p2p_idx = 2;
	}

	frame_type_mask = (u16)(upd->interface_stypes & (BIT(IEEE80211_STYPE_PROBE_REQ >> 4) | BIT(IEEE80211_STYPE_ACTION >> 4)));
	if ((p2p_idx >= 0) && (global_idev.p2p_global.mgmt_register[p2p_idx] != frame_type_mask)) {
		global_idev.p2p_global.mgmt_register[p2p_idx] = frame_type_mask;
	}
#endif
}
#endif

void cfg80211_rtw_ops_sta_init(void)
{
	struct cfg80211_ops *ops = &global_idev.rtw_cfg80211_ops;

	ops->change_virtual_intf = cfg80211_rtw_change_iface;
	ops->get_station = cfg80211_rtw_get_station;
	ops->scan = cfg80211_rtw_scan;
	ops->abort_scan = cfg80211_rtw_abort_scan;
	ops->set_wiphy_params = cfg80211_rtw_set_wiphy_params;
	ops->connect = cfg80211_rtw_connect;
	ops->disconnect = cfg80211_rtw_disconnect;
	ops->join_ibss = cfg80211_rtw_join_ibss;
	ops->leave_ibss = cfg80211_rtw_leave_ibss;
	ops->set_tx_power = cfg80211_rtw_set_txpower;
	ops->get_tx_power = cfg80211_rtw_get_txpower;
	ops->set_power_mgmt = cfg80211_rtw_set_power_mgmt;
	ops->set_monitor_channel = cfg80211_rtw_set_monitor_channel;
	ops->remain_on_channel = cfg80211_rtw_remain_on_channel;
	ops->cancel_remain_on_channel = cfg80211_rtw_cancel_remain_on_channel;
	ops->mgmt_tx = cfg80211_rtw_mgmt_tx;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 8, 0))
	ops->mgmt_frame_register = cfg80211_rtw_mgmt_frame_register;
#else
	ops->update_mgmt_frame_registrations = cfg80211_rtw_update_mgmt_frame_register;
#endif
	ops->external_auth = cfg80211_rtw_external_auth_status;
#ifdef CONFIG_CFG80211_SME_OFFLOAD
	ops->auth = cfg80211_rtw_auth;
	ops->deauth = cfg80211_rtw_deauth;
	ops->assoc = cfg80211_rtw_assoc;
	ops->disassoc = cfg80211_rtw_disassoc;
#endif	/* CONFIG_CFG80211_SME_OFFLOAD */
}

