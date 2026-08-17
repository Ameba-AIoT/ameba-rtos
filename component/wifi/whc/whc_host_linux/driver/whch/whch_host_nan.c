/******************************************************************************
 *
 * whch host-MAC NAN peer table (Block B).
 *
 * The device (NP) runs NAN NDP negotiation and PASN/pairing, then pushes each
 * established / terminated NDP to the host via WHC_API_NAN_NDP_STATUS. This file
 * turns those events into host data-path state, mirroring the STA join/disconnect
 * path: a per-NDI peer table (macid + NDL state) plus a host sta_info carrying
 * the negotiated QoS/HT caps and the PTK/GTK keys used by the whch crypto path.
 *
 * The peer table is keyed by peer NDI. Multiple NDPs to the same NDI share one
 * entry, refcounted by ndp_count (matching the NP, which counts established NDPs
 * per NDI). The host sta_info and keys are (re)installed on the 0->1 transition
 * and freed on the 1->0 transition.
 *
 ******************************************************************************/
#include <whc_host_linux.h>

#if	defined(CONFIG_NAN) && defined(CONFIG_WHCH)

struct whch_host_nan_priv g_nanpriv = {
	.peer_list = LIST_HEAD_INIT(g_nanpriv.peer_list),
	.peer_lock = __SPIN_LOCK_UNLOCKED(g_nanpriv.peer_lock),
};

static struct nan_peer_info_t *whch_host_nan_peer_find(u8 *ndi)
{
	struct nan_peer_info_t *ppeer;

	list_for_each_entry(ppeer, &g_nanpriv.peer_list, list) {
		if (memcmp(ppeer->peer_ndi, ndi, ETH_ALEN) == 0) {
			return ppeer;
		}
	}
	return NULL;
}

u8 whch_host_nan_is_nan_frame(union recv_frame *precvframe)
{
	struct nan_peer_info_t *ppeer;
	unsigned long flags;
	u8 *pframe, *bssid, *ta;
	u8 is_nan = 0;

	if (!g_nanpriv.enabled || (g_nanpriv.datalink_cnt == 0)) {
		return 0;
	}

	pframe = precvframe->u.hdr.rx_data;
	bssid = GetAddr3Ptr(pframe);
	ta = GetAddr2Ptr(pframe);

	/* Definitive marker: addr3 is the NAN cluster BSSID. */
	if (memcmp(bssid, g_nanpriv.nmi.bssid, ETH_ALEN) == 0) {
		return 1;
	}

	/* Fallback: transmitter is a known established NDI peer. */
	spin_lock_irqsave(&g_nanpriv.peer_lock, flags);
	ppeer = whch_host_nan_peer_find(ta);
	if (ppeer && ppeer->peer_info_ndl_establish) {
		is_nan = 1;
	}
	spin_unlock_irqrestore(&g_nanpriv.peer_lock, flags);

	return is_nan;
}

u8 whch_host_nan_check_datalink_exist(void)
{
	return (g_nanpriv.datalink_cnt > 0) ? 1 : 0;
}

struct nan_peer_info_t *whch_host_nan_peer_info_get_by_ndi(u8 *ndi)
{
	struct nan_peer_info_t *ppeer;
	unsigned long flags;

	spin_lock_irqsave(&g_nanpriv.peer_lock, flags);
	ppeer = whch_host_nan_peer_find(ndi);
	spin_unlock_irqrestore(&g_nanpriv.peer_lock, flags);
	return ppeer;
}

struct nan_peer_info_t *whch_host_nan_peer_info_first_entry_get(u8 only_established)
{
	struct nan_peer_info_t *ppeer, *found = NULL;
	unsigned long flags;

	spin_lock_irqsave(&g_nanpriv.peer_lock, flags);
	list_for_each_entry(ppeer, &g_nanpriv.peer_list, list) {
		if (only_established && !ppeer->peer_info_ndl_establish) {
			continue;
		}
		found = ppeer;
		break;
	}
	spin_unlock_irqrestore(&g_nanpriv.peer_lock, flags);
	return found;
}

static void whch_host_nan_install_keys(struct sta_info *psta, struct rtw_event_nan_ndp_status_info *info)
{
	struct sta_security_priv *psta_security = &psta->sta_security;
	struct whch_security_priv *psecuritypriv = &global_idev.whchpriv.securitypriv[WHC_NAN_PORT];

	/* Pairwise (PTK): unicast TX/RX key lives in the sta_info. */
	if (info->ptk.algorithm && info->ptk.key_len) {
		memset(&psta_security->dot118021x_UncstKey, 0, sizeof(union Keytype_32));
		memcpy(psta_security->dot118021x_UncstKey.skey, info->ptk.key,
			   min_t(u8, info->ptk.key_len, sizeof(psta_security->dot118021x_UncstKey.skey)));
		psta_security->dot11_security_privacy = info->ptk.algorithm;
		psta_security->b_pairwise_key_installed = true;
		psta_security->b_ieee8021x_blocked = false;
	}

	/* Peer group key (GTK): used to decrypt the peer's NAN multicast frames.
	 * The whch RX path picks this per-STA key when gtk_bmp is non-zero. */
	if (info->peer_gtk.algorithm && info->peer_gtk.key_len) {
		memset(&psta_security->gtk, 0, sizeof(union Keytype_32));
		memcpy(psta_security->gtk.skey, info->peer_gtk.key,
			   min_t(u8, info->peer_gtk.key_len, sizeof(psta_security->gtk.skey)));
		psta_security->gtk_bmp = BIT(info->peer_gtk.key_idx & 0x03);
	}

	/* Local group key (self GTK): used to encrypt our NAN multicast frames.
	 * This is a per-port key (same for every peer), stored in securitypriv. */
	if (info->self_gtk.algorithm && info->self_gtk.key_len) {
		memset(&psecuritypriv->dot11_wpa_grpkey[0], 0, sizeof(union Keytype_32));
		memcpy(psecuritypriv->dot11_wpa_grpkey[0].skey, info->self_gtk.key,
			   min_t(u8, info->self_gtk.key_len, sizeof(psecuritypriv->dot11_wpa_grpkey[0].skey)));
		psecuritypriv->dot11_wpa_grpkey_index = info->self_gtk.key_idx;
		psecuritypriv->dot118021XGrpPrivacy = info->self_gtk.algorithm;
		psecuritypriv->b_installGrpkey = true;
	}
}

static void whch_host_nan_update_stainfo(struct rtw_event_nan_ndp_status_info *info)
{
	struct sta_info *psta;
	struct sta_mlme_priv *psta_mlmepriv;
	struct sta_ht_priv *psta_htpriv;
	struct whch_security_priv *psecuritypriv = &global_idev.whchpriv.securitypriv[WHC_NAN_PORT];

	psta = whc_host_sta_get_stainfo(WHC_NAN_PORT, info->peer_ndi);
	if (!psta) {
		psta = whc_host_sta_alloc_stainfo(WHC_NAN_PORT, info->peer_ndi);
	}
	if (!psta) {
		dev_warn(global_idev.pwhc_dev, "[whc] %s alloc stainfo fail\n", __func__);
		return;
	}

	psta_mlmepriv = &psta->sta_mlmepriv;
	psta_htpriv = &psta->sta_htpriv;

	psta_mlmepriv->stainfo_macid = info->stainfo.stainfo_macid;
	psta_mlmepriv->tx_ampdu_density = info->stainfo.tx_ampdu_density;
	psta_mlmepriv->asoc_cap.htc_rx = info->stainfo.htc_rx;
	psta_mlmepriv->b_erp_protection = info->stainfo.bcnupd_info.erp_protection;

	psta_htpriv->bwmode = info->stainfo.bcnupd_info.bwmode;
	psta_htpriv->ch_offset = info->stainfo.bcnupd_info.ch_offset;
	psta_htpriv->sgi_20m = info->stainfo.sgi_20m;
	psta_htpriv->sgi_40m = info->stainfo.sgi_40m;
	psta_htpriv->ampdu_enable = info->stainfo.ampdu_enable;
	psta_htpriv->addba_success_bitmap = 0;
	psta_htpriv->ht_option = info->stainfo.ht_option;
	psta_htpriv->he_option = info->stainfo.he_option;

	/* NAN data frames are encrypted in software on the host. */
	psecuritypriv->dot11_wpa_mode = 1;
	psecuritypriv->b_sw_encrypt = 1;
	psecuritypriv->dot11PrivacyAlgrthm = info->ptk.algorithm;

	whch_host_nan_install_keys(psta, info);
}

void whch_host_nan_ndp_established(struct rtw_event_nan_ndp_status_info *info)
{
	struct nan_peer_info_t *ppeer;
	unsigned long flags;
	u8 first_ndp = 0;

	memcpy(g_nanpriv.nmi.mac, info->self_ndi, ETH_ALEN);
	memcpy(g_nanpriv.nmi.bssid, info->self_bssid, ETH_ALEN);
	g_nanpriv.enabled = 1;

	spin_lock_irqsave(&g_nanpriv.peer_lock, flags);
	ppeer = whch_host_nan_peer_find(info->peer_ndi);
	if (!ppeer) {
		ppeer = kzalloc(sizeof(*ppeer), GFP_ATOMIC);
		if (!ppeer) {
			spin_unlock_irqrestore(&g_nanpriv.peer_lock, flags);
			dev_warn(global_idev.pwhc_dev, "[whc] %s alloc peer fail\n", __func__);
			return;
		}
		memcpy(ppeer->peer_ndi, info->peer_ndi, ETH_ALEN);
		list_add_tail(&ppeer->list, &g_nanpriv.peer_list);
		g_nanpriv.datalink_cnt++;
		first_ndp = 1;
	}
	ppeer->peer_ndl_mac_id = info->stainfo.stainfo_macid;
	ppeer->peer_info_ndl_establish = 1;
	ppeer->ndp_count++;
	spin_unlock_irqrestore(&g_nanpriv.peer_lock, flags);

	whch_host_nan_update_stainfo(info);

	dev_dbg(global_idev.pwhc_dev, "[whc] NAN NDP up ndi=%pM macid=%d first=%d\n",
			info->peer_ndi, info->stainfo.stainfo_macid, first_ndp);
}

void whch_host_nan_ndp_terminated(struct rtw_event_nan_ndp_status_info *info)
{
	struct nan_peer_info_t *ppeer;
	unsigned long flags;
	u8 freed = 0;

	spin_lock_irqsave(&g_nanpriv.peer_lock, flags);
	ppeer = whch_host_nan_peer_find(info->peer_ndi);
	if (ppeer) {
		if (ppeer->ndp_count) {
			ppeer->ndp_count--;
		}
		if (ppeer->ndp_count == 0) {
			list_del(&ppeer->list);
			if (g_nanpriv.datalink_cnt) {
				g_nanpriv.datalink_cnt--;
			}
			freed = 1;
		}
	}
	spin_unlock_irqrestore(&g_nanpriv.peer_lock, flags);

	if (freed) {
		whc_host_sta_free_stainfo(WHC_NAN_PORT, info->peer_ndi);
		kfree(ppeer);
		dev_dbg(global_idev.pwhc_dev, "[whc] NAN NDP down ndi=%pM reason=%d\n", info->peer_ndi, info->reason);
	}
}

void whch_host_nan_free_all(void)
{
	struct nan_peer_info_t *ppeer, *tmp;
	unsigned long flags;
	LIST_HEAD(dead);

	spin_lock_irqsave(&g_nanpriv.peer_lock, flags);
	list_splice_init(&g_nanpriv.peer_list, &dead);
	g_nanpriv.datalink_cnt = 0;
	g_nanpriv.enabled = 0;
	spin_unlock_irqrestore(&g_nanpriv.peer_lock, flags);

	list_for_each_entry_safe(ppeer, tmp, &dead, list) {
		list_del(&ppeer->list);
		whc_host_sta_free_stainfo(WHC_NAN_PORT, ppeer->peer_ndi);
		kfree(ppeer);
	}
}

void whch_host_nan_update_txdesc(struct xmit_frame *pxmitframe, u8 *pbuf)
{
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	struct txdesc_priv *ptxdesc = (struct txdesc_priv *)pbuf;

	if (pattrib->nan_pkt_type != NAN_PKT_TYPE_DATA) {
		return;
	}

	if (!pattrib->b_qos_en) {
		ptxdesc->en_hwseq = 1;
		ptxdesc->hw_ssn_sel = 2;
	}

	ptxdesc->retry_limit = 15;
	ptxdesc->port_id = WHC_NAN_PORT;
}

#endif /* CONFIG_NAN */
