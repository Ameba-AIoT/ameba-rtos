
#include <whc_host_linux.h>

#ifdef CONFIG_WHCH
void whc_host_mlme_priv_init(void)
{
	struct whch_mlme_priv	*pmlmepriv = &global_idev.whchpriv.mlmepriv;

	timer_setup(&pmlmepriv->dynamic_timer, whc_host_dynamic_timer_hdl, 0);
}

void whc_host_mlme_priv_deinit(void)
{
	struct whch_mlme_priv	*pmlmepriv = &global_idev.whchpriv.mlmepriv;

	del_timer_sync(&pmlmepriv->dynamic_timer);
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
void whc_host_dynamic_timer_hdl(struct timer_list *t)
{
	struct whch_rx_stats *pcount = &global_idev.whchpriv.rx_stats[WHC_STA_PORT];
	struct rtw_stats_info *pstats;
	struct whch_mlme_priv	*pmlmepriv = &global_idev.whchpriv.mlmepriv;
	struct sta_info	*psta_info;
	u32 size;
	u32 *param;

	size = sizeof(struct rtw_stats_info);
	param = (u32 *)kzalloc(size, GFP_ATOMIC);
	if (param == NULL) {
		return;
	}

	pstats = (struct rtw_stats_info *)param;

	pstats->NumRxOkInPeriod = pmlmepriv->NumRxOkInPeriod;
	pstats->NumRxUnicastOkInPeriod = pmlmepriv->NumRxUnicastOkInPeriod;

	pstats->port_stats_info[0].rx_bytes_in2s = pcount->rx_bytes_in2s;
	pstats->port_stats_info[0].rx_byte_uni_in2s = pcount->rx_byte_uni_in2s;
	pstats->port_stats_info[0].rx_packets = pcount->rx_packets;

	if (whc_host_check_sta_associated_to_ap()) {
		pstats->sta_num = 1;
		psta_info = whc_host_sta_get_stainfo(WHC_STA_PORT, global_idev.bssid);
		if (psta_info) {
			pstats->sta_stats_info[0].macid = 0;
			pstats->sta_stats_info[0].stainfo_rx_data_pkts_in2s = psta_info->sta_mlmepriv.stainfo_rx_data_pkts_in2s;
			pstats->sta_stats_info[0].stainfo_rx_byte_uni_in2s = psta_info->sta_mlmepriv.stainfo_rx_byte_uni_in2s;
			memcpy(pstats->sta_stats_info[0].mac_addr, psta_info->sta_mlmepriv.stainfo_mac_addr, ETH_ALEN);
		}
	}

	if (global_idev.pndev[1] && rtw_netdev_priv_is_on(global_idev.pndev[1])) {
		pcount = &global_idev.whchpriv.rx_stats[WHC_AP_PORT];
		pstats->port_stats_info[1].rx_bytes_in2s = pcount->rx_bytes_in2s;
		pstats->port_stats_info[1].rx_byte_uni_in2s = pcount->rx_byte_uni_in2s;
		pstats->port_stats_info[1].rx_packets = pcount->rx_packets;

		/* TODO_softap stainfo rx counter */
	}

	whc_host_send_event(WHC_API_WIFI_WHCH_STATES_SYNC, (u8 *)param, size, NULL, 0);

	kfree((void *)param);
}
#endif