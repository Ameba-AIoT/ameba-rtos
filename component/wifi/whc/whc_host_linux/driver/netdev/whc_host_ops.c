#include <whc_host_linux.h>

int whc_host_scan_abort(void)
{
	int ret = 0;
	struct internal_block_param *block_param = NULL;

	block_param = (struct internal_block_param *)kzalloc(sizeof(struct internal_block_param), GFP_KERNEL);
	if (!block_param) {
		ret = -ENOMEM;
		goto exit;
	}
	/* initialize scan_abort_sema. */
	init_completion(&block_param->sema);

	ret = whc_host_send_scan_abort();

	if (ret < 0) {
		ret = 0;
		goto exit;  /* there is no scan ongoing, just return SUCCESS*/
	}

	global_idev.mlme_priv.scan_abort_block_param = block_param;

	if (wait_for_completion_interruptible_timeout(&block_param->sema, RTW_SCAN_ABORT_TIMEOUT) == 0) {
		dev_err(global_idev.pwhc_dev, "%s: Scan abort wait timeout!\n", __func__);
		ret = -EINVAL;
	}
	global_idev.mlme_priv.scan_abort_block_param = NULL;

exit:
	if (block_param) {
		complete_release(&block_param->sema);
		kfree((u8 *)block_param);
	}

	return ret;
}

int whc_host_scan(struct rtw_scan_param *scan_param, u32 ssid_length, u8 block)
{
	int ret = 0;
	struct internal_block_param *block_param = NULL;

	if (block) {
		block_param = (struct internal_block_param *)kzalloc(sizeof(struct internal_block_param), GFP_KERNEL);
		if (!block_param) {
			ret = -ENOMEM;
			goto error;
		}
		/* initialize scan_sema. */
		init_completion(&block_param->sema);
	}

	ret = whc_host_send_scan(scan_param, ssid_length);

	if (ret < 0) {
		goto error;
	}

	if (block) {
		global_idev.mlme_priv.scan_block_param = block_param;

		if (wait_for_completion_interruptible_timeout(&block_param->sema, RTW_SCAN_TIMEOUT) == 0) {
			dev_err(global_idev.pwhc_dev, "%s: Scan timeout!\n", __func__);
			ret = -EINVAL;
		}
		global_idev.mlme_priv.scan_block_param = NULL;
	}

error:
	if (block_param) {
		complete_release(&block_param->sema);
		kfree((u8 *)block_param);
	}

	return ret;
}

int whc_host_connect(struct rtw_network_info *connect_param, unsigned char block)
{
	int ret = 0;
	struct internal_block_param *block_param = NULL;

#ifdef CONFIG_SUPPLICANT_SME
	/* step1: check if auth is finished */
	if ((global_idev.mlme_priv.rtw_join_status > RTW_JOINSTATUS_UNKNOWN)
		&& (global_idev.mlme_priv.rtw_join_status < RTW_JOINSTATUS_SUCCESS)
		&& (global_idev.mlme_priv.rtw_join_status != RTW_JOINSTATUS_AUTHENTICATED)) {
		dev_err(global_idev.pwhc_dev, "[whc]: auth is not finished! rtw_join_status=%d\n", global_idev.mlme_priv.rtw_join_status);
		return -EBUSY;
	}
#else
	/* step1: check if there's ongoing connect*/
	if ((global_idev.mlme_priv.rtw_join_status > RTW_JOINSTATUS_UNKNOWN)
		&& (global_idev.mlme_priv.rtw_join_status < RTW_JOINSTATUS_SUCCESS)) {
		dev_err(global_idev.pwhc_dev, "[whc]: there is ongoing wifi connect!rtw_join_status=%d\n", global_idev.mlme_priv.rtw_join_status);
		return -EBUSY;
	}
#endif

	/*clear for last connect status */
	global_idev.mlme_priv.rtw_join_status = RTW_JOINSTATUS_STARTING;
	whc_host_connect_indicate(RTW_JOINSTATUS_STARTING, NULL);

	/* step2: malloc and set synchronous connection related variables*/
	if (block) {
		block_param = (struct internal_block_param *)kzalloc(sizeof(struct internal_block_param), GFP_KERNEL);
		if (!block_param) {
			ret = -ENOMEM;
			global_idev.mlme_priv.rtw_join_status = RTW_JOINSTATUS_FAIL;
			goto error;
		}
		/* initialize join_sema. */
		init_completion(&block_param->sema);
	}

	/* step3: set connect cmd to driver*/
	ret = whc_host_send_connect(connect_param);

	if (ret != 0) {
		ret = -EINVAL;
		global_idev.mlme_priv.rtw_join_status = RTW_JOINSTATUS_FAIL;
		goto error;
	}

	/* step4: wait connect finished for synchronous connection*/
	if (block) {
		global_idev.mlme_priv.join_block_param = block_param;

		if (wait_for_completion_interruptible_timeout(&block_param->sema, RTW_JOIN_TIMEOUT) == 0) {
			dev_err(global_idev.pwhc_dev, "%s: Join bss timeout!\n", __func__);
			global_idev.mlme_priv.rtw_join_status = RTW_JOINSTATUS_FAIL;
			ret = -EINVAL;
			goto error;
		} else {
			if (global_idev.mlme_priv.rtw_join_status != RTW_JOINSTATUS_SUCCESS) {
				ret = -EINVAL;
				global_idev.mlme_priv.rtw_join_status = RTW_JOINSTATUS_FAIL;
				goto error;
			}
		}
	}

error:
	if (block_param) {
		complete_release(&block_param->sema);
		kfree((u8 *)block_param);
		global_idev.mlme_priv.join_block_param = NULL;
	}

	if (global_idev.mlme_priv.rtw_join_status == RTW_JOINSTATUS_FAIL) {
		whc_host_connect_indicate(RTW_JOINSTATUS_FAIL, NULL);
	}

	return ret;
}

void whc_host_connect_indicate(unsigned int join_status, void *evt_info)
{
	struct mlme_priv_t *mlme_priv = &global_idev.mlme_priv;
	u8 wlan_idx = 0;
	unsigned int rtw_join_status_last = mlme_priv->rtw_join_status;
	struct rtw_event_join_status_info *join_status_info = (struct rtw_event_join_status_info *)evt_info;
#ifdef CONFIG_IEEE80211R
	struct cfg80211_roam_info roam_info = {0};
#endif
	struct ieee80211_mgmt *mgmt = NULL;
	struct rtw_wpa_4way_status	rpt_4way = {0};

	mlme_priv->rtw_join_status = join_status;

	if (join_status >= RTW_JOINSTATUS_ASSOCIATED) {
		if (global_idev.mlme_priv.b_in_linking) {
			global_idev.mlme_priv.b_in_linking = false;
		}
	}

	/* Merge from wifi_join_status_indicate. */
	if (join_status == RTW_JOINSTATUS_SUCCESS) {
		/* if Synchronous connection, up sema when connect success */
		if (mlme_priv->join_block_param) {
			complete(&mlme_priv->join_block_param->sema);
		}
		memcpy(global_idev.bssid, join_status_info->bssid, ETH_ALEN);
	}

	/* step 1: internal process for different status*/
	if (join_status == RTW_JOINSTATUS_STARTING) {
		dev_dbg(global_idev.pwhc_dev, "[whc] --- %s --- join starting", __func__);
		memset(mlme_priv->assoc_req_ie, 0, ASSOC_IE_MAX_LEN);
		memset(mlme_priv->assoc_rsp_ie, 0, ASSOC_IE_MAX_LEN);
		mlme_priv->assoc_req_ie_len = 0;
		mlme_priv->assoc_rsp_ie_len = 0;
		return;
	}

	if (join_status == RTW_JOINSTATUS_ASSOCIATING) {
		if (join_status_info->frame_len > 0) {
			memcpy(mlme_priv->assoc_req_ie, (u8 *)(join_status_info->frame), join_status_info->frame_len);
			mlme_priv->assoc_req_ie_len = join_status_info->frame_len;
		}
		return;
	}

	if (join_status == RTW_JOINSTATUS_ASSOCIATED) {
		if (join_status_info->frame_len > 0) {
			memcpy(mlme_priv->assoc_rsp_ie, (u8 *)(join_status_info->frame), join_status_info->frame_len);
			mlme_priv->assoc_rsp_ie_len = join_status_info->frame_len;
			dev_dbg(global_idev.pwhc_dev, "[whc] --- %s --- %s success.", __func__, (*(u8 *)mlme_priv->assoc_req_ie == 0 ? "association" : "re-association"));
#ifdef CONFIG_P2P
			if ((global_idev.p2p_global.pd_wlan_idx == 1) && (memcmp(mlme_priv->assoc_rsp_ie + 4, global_idev.pndev[1]->dev_addr, 6) == 0)) {
				wlan_idx = 1; //address match GC, then use GC's netdev
			}
#endif
#ifdef CONFIG_IEEE80211R
			if (global_idev.b_in_roaming) {

				dev_dbg(global_idev.pwhc_dev, "[whc] %s roaming success.", __func__);
				roam_info.links[0].bssid = mlme_priv->assoc_rsp_ie + 16;
				roam_info.links[0].addr = mlme_priv->assoc_rsp_ie + 8;
				roam_info.req_ie = mlme_priv->assoc_req_ie + WLAN_HDR_A3_LEN + 4 + (*(u8 *)mlme_priv->assoc_req_ie == 0 ? 0 : 6);
				roam_info.req_ie_len = mlme_priv->assoc_req_ie_len - WLAN_HDR_A3_LEN - 4 - (*(u8 *)mlme_priv->assoc_req_ie == 0 ? 0 : 6);
				roam_info.resp_ie = mlme_priv->assoc_rsp_ie + WLAN_HDR_A3_LEN + 6;
				roam_info.resp_ie_len = mlme_priv->assoc_rsp_ie_len -  WLAN_HDR_A3_LEN - 6;
				cfg80211_roamed(global_idev.pndev[wlan_idx], &roam_info, GFP_ATOMIC);
				global_idev.b_in_roaming = false;
			} else
#endif
			{
				dev_dbg(global_idev.pwhc_dev, "[whc] %s connectting success.", __func__);
#ifndef CONFIG_SUPPLICANT_SME /* no need to report connect_result when using userspace sme */
				/* Inform connect information. */
				/* Different between cfg80211_connect_result and cfg80211_connect_bss are described in net/cfg80211.h. */
				/* if connect_result warning, that means get_bss fail (need check), one reason is WPA_S calls disconnect ops, which resulting in wdev->ssid_len = 0 */
				cfg80211_connect_result(global_idev.pndev[wlan_idx], mlme_priv->assoc_rsp_ie + 16,
										mlme_priv->assoc_req_ie + WLAN_HDR_A3_LEN + 4/* fixed parameters */
										+ (*(u8 *)mlme_priv->assoc_req_ie == 0 ? 0 : 6)/* re-assoc: current ap */,
										mlme_priv->assoc_req_ie_len - WLAN_HDR_A3_LEN - 4 - (*(u8 *)mlme_priv->assoc_req_ie == 0 ? 0 : 6),
										mlme_priv->assoc_rsp_ie + WLAN_HDR_A3_LEN + 6, mlme_priv->assoc_rsp_ie_len -  WLAN_HDR_A3_LEN - 6,
										WLAN_STATUS_SUCCESS, GFP_ATOMIC);
#endif
			}
#ifdef CONFIG_WHCH
			/* assign some parameters trx use */
			global_idev.whchpriv.qospriv[wlan_idx].qos_option = join_status_info->qos_option;
			global_idev.whchpriv.mlmepriv.acm_mask = join_status_info->acm_mask;
			global_idev.whchpriv.mlmepriv.user_tx_rate = join_status_info->user_tx_rate;
			global_idev.whchpriv.mlmeinfo[wlan_idx].cur_channel = join_status_info->channel;
			global_idev.whchpriv.mlmeinfo[wlan_idx].b_preamble_mode = join_status_info->preamble_mode;
			global_idev.whchpriv.htpriv[wlan_idx].HT_protection = join_status_info->HT_protection;
			/* update stainfo for trx use */
			whc_host_sta_update_stainfo(wlan_idx, join_status_info->bssid, &join_status_info->stainfo, &join_status_info->sec_priv);
#endif

			netif_carrier_on(global_idev.pndev[wlan_idx]);
			if (wlan_idx == WHC_STA_PORT) {
				mgmt = (struct ieee80211_mgmt *)mlme_priv->assoc_rsp_ie;
				if (!(mgmt->u.assoc_resp.capab_info & WLAN_CAPABILITY_PRIVACY)) {
					netif_dormant_off(global_idev.pndev[wlan_idx]);
				}
			}

			dev_dbg(global_idev.pwhc_dev, "[whc]: wlan_idx = %d, is_need_4wway= %d, is_4way_ongoing =%d", wlan_idx, global_idev.is_need_4way[wlan_idx],
					global_idev.is_4way_ongoing[wlan_idx]);
			//send message notify NP coex, 4-way handshake start
			if (global_idev.is_need_4way[wlan_idx]) {
				rpt_4way.is_start = true;//4-way start
				rpt_4way.is_success = false;
				rpt_4way.wlan_idx = wlan_idx;
				whc_host_wpa_4way_status_indicate(&rpt_4way);
				global_idev.is_4way_ongoing[wlan_idx] = 1;
				dev_dbg(global_idev.pwhc_dev, "WHC host indicate 4-way start \n");
			}
		}
		return;
	}

	if (join_status == RTW_JOINSTATUS_FAIL) {
#ifdef CONFIG_P2P
		if (global_idev.p2p_global.pd_wlan_idx == 1) {
			wlan_idx = 1;// GC intf is up, then use GC's netdev
		}
#endif
		dev_dbg(global_idev.pwhc_dev, "[whc] --- %s --- join failed up sema.", __func__);
		/* merge from wifi_join_status_indicate if synchronous connection, up sema when connect fail*/
		if (mlme_priv->join_block_param) {
			complete(&mlme_priv->join_block_param->sema);
		}
		dev_dbg(global_idev.pwhc_dev, "[whc] --- %s --- join failed inform cfg80211.", __func__);

		dev_dbg(global_idev.pwhc_dev, "[whc]: wlan_idx = %d, is_need_4wway= %d, is_4way_ongoing =%d", wlan_idx, global_idev.is_need_4way[wlan_idx],
				global_idev.is_4way_ongoing[wlan_idx]);
		//send message notify NP coex, 4-way handshake end
		if (global_idev.is_need_4way[wlan_idx] && global_idev.is_4way_ongoing[wlan_idx]) {
			rpt_4way.is_start = false;
			rpt_4way.is_success = true;
			rpt_4way.wlan_idx = wlan_idx;
			whc_host_wpa_4way_status_indicate(&rpt_4way);
			global_idev.is_4way_ongoing[wlan_idx] = 0;
			dev_dbg(global_idev.pwhc_dev, "WHC host indicate 4-way end \n");
		}

		if (rtw_join_status_last >= RTW_JOINSTATUS_ASSOCIATED) {
			cfg80211_disconnected(global_idev.pndev[wlan_idx], 0, NULL, 0, 1, GFP_ATOMIC);
		} else {
			cfg80211_connect_result(global_idev.pndev[wlan_idx], NULL, NULL, 0, NULL, 0, WLAN_STATUS_UNSPECIFIED_FAILURE, GFP_ATOMIC);
		}
		if ((wlan_idx == WHC_STA_PORT) && !netif_dormant(global_idev.pndev[wlan_idx])) {
			netif_dormant_on(global_idev.pndev[wlan_idx]);
#ifdef CONFIG_WHCH
			whc_host_sta_free_resource(wlan_idx);
#endif
		}
		netif_carrier_off(global_idev.pndev[wlan_idx]);
		return;
	}
}

void whc_host_disconnect_indicate(u16 reason, u8 locally_generated, u8 *bssid)
{
	u8 wlan_idx = 0;
	struct rtw_wpa_4way_status	rpt_4way = {0};
#ifdef CONFIG_P2P
	if (global_idev.p2p_global.p2p_role == P2P_ROLE_CLIENT) {
		wlan_idx = 1;// GC intf is up, then use GC's netdev
	}
#endif

#ifdef CONFIG_SUPPLICANT_SME
	struct sme_priv_t *sme_priv = &global_idev.sme_priv;

	/* deauth from device driver */
	if (sme_priv->deauth_from_wpas == 0) {
		u8 deauth_buf[sizeof(struct rtw_ieee80211_hdr_3addr) + 2] = {0};
		struct rtw_ieee80211_hdr_3addr *hdr;
		u8 *pos;

		pos = deauth_buf;
		hdr = (struct rtw_ieee80211_hdr_3addr *)pos;
		set_frame_sub_type(hdr, RTW_DEAUTH);
		memcpy(hdr->addr1, global_idev.pndev[0]->dev_addr, ETH_ALEN);
		memcpy(hdr->addr2, bssid, ETH_ALEN);
		memcpy(hdr->addr3, bssid, ETH_ALEN);
		pos += sizeof(struct rtw_ieee80211_hdr_3addr);
		*(u16 *)pos = cpu_to_le16(reason);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 11, 0))
		cfg80211_tx_mlme_mgmt(global_idev.pndev[0], deauth_buf, sizeof(deauth_buf), false);
#else
		cfg80211_tx_mlme_mgmt(global_idev.pndev[0], deauth_buf, sizeof(deauth_buf));
#endif
		return;
	}
#endif

	if (global_idev.pndev[wlan_idx] != NULL) {
		/* Do it first for tx broadcast pkt after disconnection issue! */
		if (wlan_idx == WHC_STA_PORT) {
			netif_dormant_on(global_idev.pndev[wlan_idx]);
		}
		netif_carrier_off(global_idev.pndev[wlan_idx]);

		dev_dbg(global_idev.pwhc_dev, "[whc]: wlan_idx = %d, is_need_4wway= %d, is_4way_ongoing =%d", wlan_idx, global_idev.is_need_4way[wlan_idx],
				global_idev.is_4way_ongoing[wlan_idx]);
		//send message notify NP coex, 4-way handshake end
		if (global_idev.is_need_4way[wlan_idx] && global_idev.is_4way_ongoing[wlan_idx]) {
			rpt_4way.is_start = false;
			rpt_4way.is_success = true;
			rpt_4way.wlan_idx = wlan_idx;
			whc_host_wpa_4way_status_indicate(&rpt_4way);
			global_idev.is_4way_ongoing[wlan_idx] = 0;
			dev_dbg(global_idev.pwhc_dev, "WHC host indicate 4-way end \n");
		}

		dev_dbg(global_idev.pwhc_dev, "%s reason:%d\n", __func__, reason);
		cfg80211_disconnected(global_idev.pndev[wlan_idx], reason, NULL, 0, locally_generated, GFP_ATOMIC);
	}
}

void whc_host_external_auth_request(char *evt_info)
{
	struct cfg80211_external_auth_params *auth_ext_para = &global_idev.mlme_priv.auth_ext_para;
	struct wpa_sae_param_t *wpa_sae_param = (struct wpa_sae_param_t *)evt_info;

	auth_ext_para->action = NL80211_EXTERNAL_AUTH_START;
	memcpy(auth_ext_para->bssid, wpa_sae_param->peer_mac, ETH_ALEN);
	auth_ext_para->key_mgmt_suite = le32_to_cpu(*(__le32 *)wpa_sae_param->rsn_auth_key_mgmt);

	/*ap mode doesn't need call this ops, sta will trigger auth*/
	cfg80211_external_auth_request(global_idev.pndev[0], auth_ext_para, GFP_ATOMIC);
	dev_dbg(global_idev.pwhc_dev, "%s, ssid=%s, len=%d\n", __func__, auth_ext_para->ssid.ssid, auth_ext_para->ssid.ssid_len);
}

void whc_host_update_owe_info_event(char *evt_info)
{
	struct cfg80211_update_owe_info owe_info;
	struct rtw_owe_param_t *owe_param = (struct rtw_owe_param_t *)evt_info;
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

int whc_host_scan_done_indicate(unsigned int scanned_AP_num, void *user_data)
{
	struct cfg80211_scan_info info;

	if (!rtw_netdev_priv_is_on(global_idev.pndev[0]) && !rtw_netdev_priv_is_on(global_idev.pndev[1])) {
		dev_dbg(global_idev.pwhc_dev, "sta is down, finish scan.");
		return -1;
	}

	if (!global_idev.mlme_priv.pscan_req_global || !global_idev.mlme_priv.b_in_scan) {
		dev_dbg(global_idev.pwhc_dev, "Last scan req has been finished. Wait for next. ");
		return -1;
	}

	memset(&info, 0, sizeof(info));
	info.aborted = 0;
	dev_dbg(global_idev.pwhc_dev, "%s: scan request(%p) done.", __FUNCTION__, global_idev.mlme_priv.pscan_req_global);
	cfg80211_scan_done(global_idev.mlme_priv.pscan_req_global, &info);

	/* cfg80211_scan_done will clear last request. Clean global scan request as well. */
	global_idev.mlme_priv.pscan_req_global = NULL;
	global_idev.mlme_priv.b_in_scan = false;

#ifndef CONFIG_WHC_HCI_IPC
	/* wakeup xmit thread if there are pending packets */
	if (whc_host_xmit_pending_q_num() > 0) {
		whc_host_xmit_wakeup_thread();
	}
#endif

	return 0;
}

void whc_host_inform_bss(u32 channel, u32 frame_is_bcn, s32 rssi, u8 *mac_addr, u8 *IEs, u32 ie_len)
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
		dev_dbg(global_idev.pwhc_dev, "%s pbuf allocate failed	!!\n", __FUNCTION__);
		return;
	}

	bssinf_len = ie_len + sizeof(struct rtw_ieee80211_hdr_3addr);
	if (bssinf_len > buf_size) {
		dev_dbg(global_idev.pwhc_dev, "%s IE Length too long > %zu byte\n", __FUNCTION__, buf_size);
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

	if (frame_is_bcn) { /* RTW_BEACON */
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

