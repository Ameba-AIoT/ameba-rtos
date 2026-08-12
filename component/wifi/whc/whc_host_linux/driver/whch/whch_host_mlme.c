
#include <whc_host_linux.h>

#ifdef CONFIG_WHCH
#define WHC_HOST_DYNAMIC_PERIOD_MS	2000

void whc_host_mlme_priv_init(void)
{
	struct whch_mlme_priv	*pmlmepriv = &global_idev.whchpriv.mlmepriv;

	INIT_DELAYED_WORK(&pmlmepriv->dynamic_work, whc_host_dynamic_timer_hdl);
	schedule_delayed_work(&pmlmepriv->dynamic_work, msecs_to_jiffies(WHC_HOST_DYNAMIC_PERIOD_MS));
}

void whc_host_mlme_priv_deinit(void)
{
	struct whch_mlme_priv	*pmlmepriv = &global_idev.whchpriv.mlmepriv;

	cancel_delayed_work_sync(&pmlmepriv->dynamic_work);
}

int whc_host_init_default_value(u8 iface_type)
{
	struct whch_mlme_info	*pmlmeinfo = &global_idev.whchpriv.mlmeinfo[iface_type];
	struct whch_security_priv *psecuritypriv = &global_idev.whchpriv.securitypriv[iface_type];

	//mlme_info

	//security_priv
	psecuritypriv->b_installGrpkey = false;
	psecuritypriv->b_sw_encrypt = 0;
	psecuritypriv->b_hw_decrypted = 1;

	psecuritypriv->dot11PrivacyAlgrthm = _NO_PRIVACY_;
	psecuritypriv->dot11_wep_key_index = 0;
	psecuritypriv->dot118021XGrpPrivacy = _NO_PRIVACY_;
	psecuritypriv->dot11_wpa_mode = 0;

	return true;
}

int whc_host_state_check_ap_client_assoc_success(struct sta_mlme_priv *psta_mlmepriv)
{
	return (psta_mlmepriv->sta_state == 1) ? true : false;
}

int whc_host_check_sta_associated_to_ap(void)
{
	struct mlme_priv_t *mlme_priv = &global_idev.mlme_priv;

	if ((mlme_priv->rtw_join_status >= RTW_JOINSTATUS_ASSOCIATED) && (mlme_priv->rtw_join_status <= RTW_JOINSTATUS_FAIL)) {
		return true;
	} else {
		return false;
	}
}

void whc_host_set_key(struct rtw_crypt_info *crypt)
{
	u8 iface_type = crypt->wlan_idx;
	struct sta_info *psta = NULL;
	struct sta_security_priv *psta_security = NULL;
	struct whch_security_priv *psecuritypriv = &global_idev.whchpriv.securitypriv[iface_type];

	if (crypt->pairwise) {
		/*Case1: PTK---------------------------------------------------------------------------------------*/
		psta = whc_host_sta_get_stainfo(iface_type, crypt->mac_addr);
		if (psta) {
			psta_security = &psta->sta_security;
		} else {
			dev_warn(global_idev.pwhc_dev, "[whc] %s psta is null\n", __func__);
			return;
		}

		memcpy(psta_security->dot118021x_UncstKey.skey, crypt->key, crypt->key_len);
		psta_security->dot11_security_privacy = crypt->driver_cipher;
		if ((u8)psta_security->dot11_security_privacy == _TKIP_) {	/*tkip needs sw do mic*/
			memcpy(psta_security->dot11tkiptxmickey.skey, crypt->key + 16, 8);
			memcpy(psta_security->dot11tkiprxmickey.skey, crypt->key + 24, 8);
			psecuritypriv->b_usetkipkey = true;
		}

		psta_security->b_pairwise_key_installed = true;
		psta_security->b_ieee8021x_blocked = false;
	} else if (crypt->driver_cipher == _BIP_ || crypt->driver_cipher == _BIP_GMAC_256) {
#ifdef CONFIG_IEEE80211W
		/*Case2: IGTK---------------------------------------------------------------------------------------*/
#endif
	} else if ((crypt->driver_cipher == _TKIP_) || (crypt->driver_cipher == _AES_) || (crypt->driver_cipher == _GCMP_256_)) {
		/*Case3: GTK----------------------------------------------------------------------------------------*/
		memcpy(psecuritypriv->dot11_wpa_grpkey[0].skey, crypt->key, crypt->key_len);
		psecuritypriv->b_installGrpkey = true;
		psecuritypriv->dot11_wpa_grpkey_index = crypt->key_idx;
		psecuritypriv->dot118021XGrpPrivacy = crypt->driver_cipher;
		if (psecuritypriv->dot118021XGrpPrivacy == _TKIP_) {
			memcpy(psecuritypriv->dot11_tkip_grpmickey_tx[0].skey, crypt->key + 16, 8);
			memcpy(psecuritypriv->dot11_tkip_grpmickey_rx[0].skey, crypt->key + 24, 8);
			psecuritypriv->b_usetkipkey = true;
		} else if (psecuritypriv->dot118021XGrpPrivacy == _AES_ || psecuritypriv->dot118021XGrpPrivacy == _GCMP_256_) {
			if (crypt->key_idx < 4) {
				memset(psecuritypriv->iv_seq[crypt->key_idx], 0, 8);
			}
		}
	}

	return;
}
void whc_host_dynamic_timer_hdl(struct work_struct *work)
{
	struct whch_rx_stats *pcount = &global_idev.whchpriv.rx_stats[WHC_STA_PORT];
	struct rtw_stats_info *pstats;
	struct whch_mlme_priv	*pmlmepriv = &global_idev.whchpriv.mlmepriv;
	struct sta_info	*psta_info;
	u32 size;
	u32 *param;

	size = sizeof(struct rtw_stats_info);
	param = (u32 *)kzalloc(size, GFP_KERNEL);
	if (param == NULL) {
		goto rearm;
	}

	pstats = (struct rtw_stats_info *)param;

	/* The device accumulates (+=) every counter it receives and zeroes its own
	 * per-period accumulators each expire tick, so the host must report the delta
	 * since the previous sync and then reset its own period counters below. */
	pstats->NumRxOkInPeriod = pmlmepriv->NumRxOkInPeriod;
	pstats->NumRxUnicastOkInPeriod = pmlmepriv->NumRxUnicastOkInPeriod;
	pmlmepriv->NumRxOkInPeriod = 0;
	pmlmepriv->NumRxUnicastOkInPeriod = 0;

	pstats->port_stats_info[0].rx_bytes_in2s = pcount->rx_bytes_in2s;
	pstats->port_stats_info[0].rx_byte_uni_in2s = pcount->rx_byte_uni_in2s;
	pstats->port_stats_info[0].rx_packets = pcount->rx_packets;
	pcount->rx_bytes_in2s = 0;
	pcount->rx_byte_uni_in2s = 0;
	pcount->rx_packets = 0;

	if (whc_host_check_sta_associated_to_ap()) {
		pstats->sta_num = 1;
		psta_info = whc_host_sta_get_stainfo(WHC_STA_PORT, global_idev.bssid);
		if (psta_info) {
			pstats->sta_stats_info[0].macid = 0;
			pstats->sta_stats_info[0].stainfo_rx_data_pkts_in2s = psta_info->sta_mlmepriv.stainfo_rx_data_pkts_in2s;
			pstats->sta_stats_info[0].stainfo_rx_byte_uni_in2s = psta_info->sta_mlmepriv.stainfo_rx_byte_uni_in2s;
			memcpy(pstats->sta_stats_info[0].mac_addr, psta_info->sta_mlmepriv.stainfo_mac_addr, ETH_ALEN);
			psta_info->sta_mlmepriv.stainfo_rx_data_pkts_in2s = 0;
			psta_info->sta_mlmepriv.stainfo_rx_byte_uni_in2s = 0;
		}
	}

	if (global_idev.pndev[1] && rtw_netdev_priv_is_on(global_idev.pndev[1])) {
		struct whch_sta_priv *pstapriv = &global_idev.whchpriv.stapriv[WHC_AP_PORT];
		struct list_head *plist, *phead;
		u8 idx = pstats->sta_num;

		pcount = &global_idev.whchpriv.rx_stats[WHC_AP_PORT];
		pstats->port_stats_info[1].rx_bytes_in2s = pcount->rx_bytes_in2s;
		pstats->port_stats_info[1].rx_byte_uni_in2s = pcount->rx_byte_uni_in2s;
		pstats->port_stats_info[1].rx_packets = pcount->rx_packets;
		pcount->rx_bytes_in2s = 0;
		pcount->rx_byte_uni_in2s = 0;
		pcount->rx_packets = 0;

		spin_lock_bh(&pstapriv->sta_list_mutex);
		phead = &pstapriv->sta_list;
		plist = phead->next;
		while ((plist != phead) && (idx < ARRAY_SIZE(pstats->sta_stats_info))) {
			psta_info = list_entry(plist, struct sta_info, list);
			plist = plist->next;
			/* skip the bcmc stainfo entry */
			if (IS_MCAST(psta_info->sta_mlmepriv.stainfo_mac_addr)) {
				continue;
			}
			pstats->sta_stats_info[idx].macid = psta_info->sta_mlmepriv.stainfo_macid;
			pstats->sta_stats_info[idx].stainfo_rx_data_pkts_in2s = psta_info->sta_mlmepriv.stainfo_rx_data_pkts_in2s;
			pstats->sta_stats_info[idx].stainfo_rx_byte_uni_in2s = psta_info->sta_mlmepriv.stainfo_rx_byte_uni_in2s;
			memcpy(pstats->sta_stats_info[idx].mac_addr, psta_info->sta_mlmepriv.stainfo_mac_addr, ETH_ALEN);
			psta_info->sta_mlmepriv.stainfo_rx_data_pkts_in2s = 0;
			psta_info->sta_mlmepriv.stainfo_rx_byte_uni_in2s = 0;
			idx++;
		}
		spin_unlock_bh(&pstapriv->sta_list_mutex);
		pstats->sta_num = idx;
	}

	whc_host_send_event(WHC_API_WIFI_WHCH_STATES_SYNC, (u8 *)param, size, NULL, 0);

	kfree((void *)param);

rearm:
	/* self-rearm: delayed_work is one-shot */
	schedule_delayed_work(&pmlmepriv->dynamic_work, msecs_to_jiffies(WHC_HOST_DYNAMIC_PERIOD_MS));
}
#endif