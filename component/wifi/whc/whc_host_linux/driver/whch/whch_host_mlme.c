
#include <whc_host_linux.h>

#ifdef CONFIG_WHCH
void whc_host_mlme_priv_init(void)
{
	struct mlme_priv	*pmlmepriv = &global_idev.mlmepriv;

	timer_setup(&pmlmepriv->dynamic_timer, whc_host_dynamic_timer_hdl, 0);
}

void whc_host_mlme_priv_deinit(void)
{
	struct mlme_priv	*pmlmepriv = &global_idev.mlmepriv;

	del_timer_sync(&pmlmepriv->dynamic_timer);
}

int whc_host_mlme_init(u8 iface_type)
{
	struct mlme_info	*pmlmeinfo = &global_idev.mlmeinfo[iface_type];

	pmlmeinfo->b_accept_addba_req = global_idev.wifi_user_config.ampdu_rx_enable;

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

void whc_host_dynamic_timer_hdl(struct timer_list *t)
{
	struct whch_rx_stats *pcount = &global_idev.rx_stats[WHC_STA_PORT];
	struct rtw_stats_info *pstats;
	struct mlme_priv	*pmlmepriv = &global_idev.mlmepriv;
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
		pcount = &global_idev.rx_stats[WHC_AP_PORT];
		pstats->port_stats_info[1].rx_bytes_in2s = pcount->rx_bytes_in2s;
		pstats->port_stats_info[1].rx_byte_uni_in2s = pcount->rx_byte_uni_in2s;
		pstats->port_stats_info[1].rx_packets = pcount->rx_packets;

		/* TODO, softap stainfo rx counter */
	}

	whc_host_send_event(WHC_API_WIFI_WHCH_STATES_SYNC, (u8 *)param, size, NULL, 0);

	kfree((void *)param);
}
#endif