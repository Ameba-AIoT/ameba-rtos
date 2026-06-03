#include <whc_host_linux.h>

#ifdef CONFIG_WHCH
struct sta_info bcmc_stainfo; /* for primary padapter */

void whc_host_sta_init(u8 iface_type)
{
	struct sta_priv	*pstapriv = &global_idev.stapriv[iface_type];

	dev_dbg(global_idev.pwhc_dev, "[whc] %s iface_type=%d.", __func__, iface_type);

	INIT_LIST_HEAD(&pstapriv->sta_list);
	spin_lock_init(&pstapriv->sta_list_mutex);
}

struct pending_sta_t *whc_host_sta_get_sta_pending(struct sta_xmit_priv *psta_xmitpriv, int hw_queue)
{
	struct pending_sta_t *pending_sta = NULL;

	/* 0(VO), 1(VI), 2(BE), 3(BK) */
	switch (hw_queue) {
	case 3:
		pending_sta = &(psta_xmitpriv->sta_pending_be_q);
		break;
	case 2:
		pending_sta = &(psta_xmitpriv->sta_pending_bk_q);
		break;
	case 1:
		pending_sta = &(psta_xmitpriv->sta_pending_vi_q);
		break;
	case 0:
		pending_sta = &(psta_xmitpriv->sta_pending_vo_q);
		break;
	default:
		pending_sta = &(psta_xmitpriv->sta_pending_be_q);
		break;
	}

	return pending_sta;
}

struct sta_info *whc_host_sta_get_stainfo(u8 iface_type, u8 *hwaddr)
{
	struct sta_priv     *pstapriv = &global_idev.stapriv[iface_type];
	struct list_head    *plist, *phead;
	struct sta_info     *psta = NULL;
	u8 *addr;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	if (hwaddr == NULL) {
		return NULL;
	}

	if (IS_MCAST(hwaddr)) {
		addr = bc_addr;
	} else {
		addr = hwaddr;
	}

	spin_lock(&pstapriv->sta_list_mutex);

	phead = &pstapriv->sta_list;
	plist = phead->next;

	while (plist != phead) {
		psta = list_entry(plist, struct sta_info, list);
		if ((!memcmp(psta->sta_mlmepriv.stainfo_mac_addr, addr, ETH_ALEN))) {
			// found the matched address
			break;
		}
		psta = NULL;
		plist = plist->next;
	}

	spin_unlock(&pstapriv->sta_list_mutex);
	return psta;
}

static void _whc_host_sta_init_pending_sta_q(struct pending_sta_t *pending_sta)
{
	INIT_LIST_HEAD(&pending_sta->list);
	INIT_LIST_HEAD(&pending_sta->frame_queue);
	spin_lock_init(&pending_sta->frame_queue_lock);
	atomic_set(&pending_sta->qcnt, 0);
}

void _whc_host_sta_init_stainfo(struct sta_info *psta)
{
	struct sta_xmit_priv *psta_xmitpriv = &psta->sta_xmitpriv;
	struct sta_recv_priv *psta_recvpriv = &psta->sta_recvpriv;
	int i = 0;
	u16 wRxSeqInitialValue = 0xffff;

	memset((u8 *)psta, 0, sizeof(struct sta_info));

	INIT_LIST_HEAD(&psta->list);

	memset((unsigned char *)psta_xmitpriv, 0, sizeof(struct sta_xmit_priv));
	_whc_host_sta_init_pending_sta_q(&psta_xmitpriv->sta_pending_be_q);
	_whc_host_sta_init_pending_sta_q(&psta_xmitpriv->sta_pending_bk_q);
	_whc_host_sta_init_pending_sta_q(&psta_xmitpriv->sta_pending_vi_q);
	_whc_host_sta_init_pending_sta_q(&psta_xmitpriv->sta_pending_vo_q);

	psta->sta_mlmepriv.tx_agg_num = 0x1f;

	for (i = 0; i < 16; i++) {
		memcpy(&psta_recvpriv->tid_rxseq[i], &wRxSeqInitialValue, 2);
	}

	timer_setup(&psta_recvpriv->defrag_ctrl.defrag_timer, whc_host_defrag_timer_handler, 0);

	//for A-MPDU Rx reordering buffer control
	for (i = 0; i < MAXTID; i++) {
		psta_recvpriv->recvreorder_ctrl[i] = NULL;
	}
}

struct sta_info *whc_host_sta_alloc_stainfo(u8 iface_type, u8 *hwaddr)
{
	struct mlme_info	*pmlmeinfo = &global_idev.mlmeinfo[iface_type];
	struct sta_priv		*pstapriv = &global_idev.stapriv[iface_type];
	struct sta_info		*psta = NULL;
	struct sta_mlme_priv *psta_mlmepriv = NULL;
	u8 is_bcmc = 0;
	u8 bc_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	if (!memcmp(bc_addr, hwaddr, ETH_ALEN)) {
		is_bcmc = 1;
	}

	if ((iface_type == WHC_STA_PORT) && is_bcmc) {
		/*bcmc stainfo for primary will be allocated during wifi on, thus use global variable directly*/
		psta = &bcmc_stainfo;
	} else {
		psta = kmalloc(sizeof(struct sta_info), GFP_KERNEL);
		if (!psta) {
			dev_err(global_idev.pwhc_dev, "%s: malloc failed.", __func__);
			goto exit;
		}
	}

	_whc_host_sta_init_stainfo(psta);
	psta_mlmepriv = &psta->sta_mlmepriv;
	memcpy(psta_mlmepriv->stainfo_mac_addr, hwaddr, 6);

	spin_lock(&pstapriv->sta_list_mutex);
	list_add_tail(&psta->list, &pstapriv->sta_list);
	pmlmeinfo->total_sta_count_by_port++;
	spin_unlock(&pstapriv->sta_list_mutex);

exit:
	return psta;
}

int whc_host_sta_free_stainfo(u8 iface_type, u8 *hwaddr)
{
	struct whch_xmit_priv	*pxmitpriv = &global_idev.xmitpriv;
	struct mlme_info	*pmlmeinfo = &global_idev.mlmeinfo[iface_type];
	struct sta_priv		*pstapriv = &global_idev.stapriv[iface_type];
	struct sta_info		*psta = NULL;
	//struct sta_xmit_priv	*psta_xmitpriv = NULL;
	int				ret = 0;
#ifndef CONFIG_MP_SHRINK
	int				i;
#endif

	dev_dbg(global_idev.pwhc_dev, "[whc] %s iface_type=%d hwaddr=[0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x].",
			__func__, iface_type, hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);

	psta = whc_host_sta_get_stainfo(iface_type, hwaddr);

	if (!psta) {
		ret = -1;
		goto exit;
	}

	spin_lock(&pstapriv->sta_list_mutex);
	list_del(&psta->list);
	pmlmeinfo->total_sta_count_by_port--;
	spin_unlock(&pstapriv->sta_list_mutex);

	/* free pendingq */
	spin_lock(&pxmitpriv->mutex);
	whc_host_hal_pending_q_free(iface_type, &psta->sta_xmitpriv);
	spin_unlock(&pxmitpriv->mutex);

	whc_host_defrag_ctrl_deinit(&psta->sta_recvpriv.defrag_ctrl);
	del_timer_sync(&psta->sta_recvpriv.defrag_ctrl.defrag_timer);

#ifndef CONFIG_MP_SHRINK
	//for A-MPDU Rx reordering buffer control, cancel reordering_ctrl_timer
	for (i = 0; i < MAXTID; i++) {
		whc_host_recv_reorder_free(&psta->sta_recvpriv.recvreorder_ctrl[i]);
	}
#endif

	/* softap TODO */
	if (iface_type == WHC_AP_PORT) {
		//g_apmlmepriv.sta_dz_bitmap &= ~BIT(psta_mlmepriv->stainfo_aid);
		//g_apmlmepriv.tim_bitmap &= ~BIT(psta_mlmepriv->stainfo_aid);
		//g_apmlmepriv.aid_bitmap &= ~BIT(psta_mlmepriv->stainfo_aid);
	}

	if (psta != (&bcmc_stainfo)) {
		kfree((u8 *)psta);
	}
exit:
	return ret;
}

void whc_host_sta_update_stainfo(u8 iface_type, u8 *hwaddr, struct rtw_event_sta_info *pstainfo)
{
	struct sta_info     *psta = NULL;
	struct sta_mlme_priv *psta_mlmepriv = NULL;
	struct sta_security_priv *psta_securitypriv = NULL;
	struct sta_xmit_priv *psta_xmitpriv = NULL;
	struct sta_ht_priv *psta_htpriv = NULL;

	dev_dbg(global_idev.pwhc_dev, "[whc] %s.", __func__);

	psta = whc_host_sta_get_stainfo(iface_type, hwaddr);

	if (!psta) {
		psta = whc_host_sta_alloc_stainfo(iface_type, hwaddr);
	}

	if (psta) {		/* update stainfo */
		psta_mlmepriv = &psta->sta_mlmepriv;
		psta_securitypriv = &psta->sta_security;
		psta_xmitpriv = &psta->sta_xmitpriv;
		psta_htpriv = &psta->sta_htpriv;

		psta_mlmepriv->stainfo_macid = pstainfo->stainfo_macid;
		psta_mlmepriv->tx_ampdu_density = pstainfo->tx_ampdu_density;
		psta_mlmepriv->asoc_cap.htc_rx = pstainfo->htc_rx;
		psta_mlmepriv->b_erp_protection = pstainfo->bcnupd_info.erp_protection;

		psta_htpriv->bwmode = pstainfo->bcnupd_info.bwmode;
		psta_htpriv->ch_offset = pstainfo->bcnupd_info.ch_offset;
		psta_htpriv->sgi_20m = pstainfo->sgi_20m;
		psta_htpriv->sgi_40m = pstainfo->sgi_40m;
		psta_htpriv->ampdu_enable = pstainfo->ampdu_enable;
		psta_htpriv->addba_success_bitmap = 0;
		psta_htpriv->ht_option = pstainfo->ht_option;
		psta_htpriv->he_option = pstainfo->he_option;
	}
}

void whc_host_sta_free_resource(u8 wlan_idx)
{
	if (wlan_idx == WHC_STA_PORT) {
		whc_host_sta_free_stainfo(wlan_idx, global_idev.bssid);
	}
}
#endif
