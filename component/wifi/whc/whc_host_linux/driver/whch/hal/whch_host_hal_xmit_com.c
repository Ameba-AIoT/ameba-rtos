#include <whc_host_linux.h>

#ifdef CONFIG_WHCH
void whc_host_hal_pending_q_init(u8 iface_type)
{
	struct hw_xmit *ppending_q = global_idev.pending_queue[iface_type].hwxmits;
	int i = 0;

	for (i = 0; i < 4; i++) {
		INIT_LIST_HEAD(&ppending_q[i].sta_queue);
		spin_lock_init(&ppending_q[i].sta_queue_lock);
		atomic_set(&ppending_q[i].accnt, 0);
	}
}

int whc_host_hal_pending_q_check(struct hw_xmit *ppending_q)
{
	int val = 0;

	val = atomic_read(&ppending_q->accnt);
	return val;
}

int whc_host_hal_pending_q_status(u8 iface_type)
{
	struct hw_xmit *ppending_q = global_idev.pending_queue[iface_type].hwxmits;
	int val = 0, i = 0;

	for (i = 0; i < 4; i++) {
		val += atomic_read(&ppending_q[i].accnt);
	}

	return val;
}

void whc_host_hal_pending_q_enqueue(struct hw_xmit *pending_top, struct sta_xmit_priv *psta_xmitpriv, struct sk_buff *pkt, u32 hw_queue)
{
	struct pending_sta_t	*pending_sta = whc_host_sta_get_sta_pending(psta_xmitpriv, hw_queue);

	spin_lock(&(pending_sta->frame_queue_lock));
	if (list_empty(&pending_sta->list)) {
		list_add_tail(&pending_sta->list, &pending_top->sta_queue);
	}
	list_add_tail(&pkt->list, &pending_sta->frame_queue);
	atomic_inc(&pending_sta->qcnt);
	atomic_inc(&pending_top->accnt);
	spin_unlock(&(pending_sta->frame_queue_lock));
}

void whc_host_hal_pending_q_dequeue(int iface_type)
{
	struct whch_xmit_priv 	*pxmitpriv = &global_idev.xmitpriv;
	struct hw_xmit			*pending_top = NULL;
	struct pending_sta_t	*pending_sta = NULL;
	struct sk_buff 			*pskb = NULL;
	struct list_head 		*plist, *phead;
	struct list_head 		*sta_plist, *sta_phead;
	int idx;

	spin_lock(&pxmitpriv->mutex);
	/* 0(VO), 1(VI), 2(BE), 3(BK) */
	for (idx = 0; idx < 4; idx++) {
		pending_top = &global_idev.pending_queue[iface_type].hwxmits[idx];
		sta_phead = &pending_top->sta_queue;
		sta_plist = sta_phead->next;
		while (sta_plist != sta_phead) {
			/*get a pending sta*/
			pending_sta = list_entry(sta_plist, struct pending_sta_t, list);
			spin_lock(&(pending_sta->frame_queue_lock));
			phead = &pending_sta->frame_queue;
			plist = phead->next;
			while (plist != phead) {
				/*get a pending frame of this sta*/
				pskb = list_entry(plist, struct sk_buff, list);
				list_del_init(plist);
				plist = phead->next;
				atomic_dec(&pending_sta->qcnt);
				atomic_dec(&pending_top->accnt);

				whc_host_xmit_entry_posthandle(iface_type, pskb, idx);
			}
			list_del_init(&pending_sta->list);
			spin_unlock(&(pending_sta->frame_queue_lock));
			sta_plist = sta_phead->next;
		}
	}
	spin_unlock(&pxmitpriv->mutex);
}

void whc_host_hal_pending_q_resume(void)
{
	u8 tx_pending_status = 0;

	if (global_idev.mlme_priv.b_in_scan) {
		return;
	}
	if (whc_host_hal_txbd_enough_check() == 0) {
		return;
	}

	tx_pending_status = whc_host_hal_pending_q_status(WHC_STA_PORT);

	if (global_idev.pndev[1] && rtw_netdev_priv_is_on(global_idev.pndev[1])) {
		tx_pending_status += whc_host_hal_pending_q_status(WHC_AP_PORT);
	}

#ifdef CONFIG_NAN
	if (global_idev.pndev[2] && rtw_netdev_priv_is_on(global_idev.pndev[2])) {
		tx_pending_status += (whc_host_hal_pending_q_status(WHC_NAN_PORT) > 0);
	}
#endif

	if (tx_pending_status) {
		whc_host_hal_pending_q_dequeue(0);
		if (global_idev.pndev[1] && rtw_netdev_priv_is_on(global_idev.pndev[1])) {
			whc_host_hal_pending_q_dequeue(1);
		}
#ifdef CONFIG_NAN
		if (global_idev.pndev[2] && rtw_netdev_priv_is_on(global_idev.pndev[2])) {
			whc_host_hal_pending_q_dequeue(2);
		}
#endif
	}
}

void whc_host_hal_pending_q_free(int iface_type, struct sta_xmit_priv *psta_xmitpriv)
{
	struct hw_xmit 			*pending_top = NULL;
	struct pending_sta_t	*pending_sta = NULL;
	struct txdesc_priv		*ptxdesc;
	struct sk_buff			*skb;
	struct list_head		*plist, *phead;
	int hw_queue = 0;

	for (hw_queue = 0; hw_queue < 4; hw_queue++) {
		pending_sta = whc_host_sta_get_sta_pending(psta_xmitpriv, hw_queue);
		phead = &pending_sta->frame_queue;
		dev_dbg(global_idev.pwhc_dev, "[whc] %s hw_queue=%d.", __func__, hw_queue);
		while (!list_empty(phead)) {
			plist = phead->next;
			skb = list_entry(plist, struct sk_buff, list);
			list_del_init(plist);
			dev_dbg(global_idev.pwhc_dev, "[whc] %s hw_queue=%d skb=%x.", __func__, hw_queue, skb);
			ptxdesc = (struct txdesc_priv *)skb->data;
			pending_top = &global_idev.pending_queue[iface_type].hwxmits[hw_queue];
			atomic_dec(&pending_sta->qcnt);
			atomic_dec(&pending_top->accnt);
			dev_kfree_skb_any(skb);
		}
		/*delete pending_sta from pending_top when pending_sta is empty*/
		list_del_init(&pending_sta->list);
	}
}

int whc_host_hal_xmitframe_coalesce(u8 iface_type, struct sk_buff *pkt, struct xmit_frame *pxmitframe, u8 force_cts2self)
{
	struct security_priv *psecuritypriv = &global_idev.securitypriv[iface_type];
	struct pkt_file pktfile;
	s32 llc_sz;
	u8 *pframe, *mem_start;
	struct sta_info *psta = NULL;
	struct sta_mlme_priv *psta_mlmepriv = NULL;
	struct sta_security_priv *psta_securitypriv = NULL;
	struct pkt_attrib	*pattrib = &pxmitframe->attrib;
	s32 bmcst = IS_MCAST(pattrib->ra);
	int res = 0;
	u8 *wlan_overhead_head = NULL;
	int wlan_overhead_head_len = 0;

	psta = whc_host_sta_get_stainfo(iface_type, pattrib->ra);
	if (psta == NULL) {
		return -1;
	}

	psta_mlmepriv = &psta->sta_mlmepriv;
	psta_securitypriv = &psta->sta_security;

	wlan_overhead_head = kmalloc(WLAN_HDR_A4_QOS_LEN + WLAN_MAX_IV_LEN + WLAN_SNAP_HEADER, GFP_KERNEL);
	if (!wlan_overhead_head) {
		dev_warn(global_idev.pwhc_dev, "%s: malloc failed", __func__);
		res = -1;
		goto exit;
	}

	mem_start = wlan_overhead_head;
	if (whc_host_xmit_make_wlanhdr(iface_type, mem_start, pattrib) == -1) {
		dev_warn(global_idev.pwhc_dev, "make_wlanhdr fail, drop\n");
		res = -1;
		goto exit;
	}

	whc_host_xmit_open_pktfile(pkt, &pktfile);
	whc_host_xmit_read_pktfile(&pktfile, NULL, pattrib->pkt_hdrlen);

	pframe = mem_start + pattrib->hdrlen;
	//adding iv, if necessary...
	if (pattrib->iv_len) {
		if (psta_securitypriv != NULL) {
			switch (pattrib->encrypt) {
			case _WEP40_:
			case _WEP104_:
				WEP_IV(pattrib->iv, psta_securitypriv->dot11txpn, pattrib->key_idx);
				break;
			case _TKIP_:
				if (bmcst) {
					TKIP_IV(pattrib->iv, psta_securitypriv->dot11txpn, pattrib->key_idx);
				} else {
					TKIP_IV(pattrib->iv, psta_securitypriv->dot11txpn, 0);
				}
				break;
			case _AES_:
				if (bmcst) {
					AES_IV(pattrib->iv, psta_securitypriv->dot11txpn, pattrib->key_idx);
				} else {
					AES_IV(pattrib->iv, psta_securitypriv->dot11txpn, 0);
				}
				break;
			case _GCMP_256_:
				if (bmcst) {
					GCMP_IV(pattrib->iv, psta_securitypriv->dot11txpn, pattrib->key_idx);
				} else {
					GCMP_IV(pattrib->iv, psta_securitypriv->dot11txpn, 0);
				}
				break;
			}
		}
		memcpy(pframe, pattrib->iv, pattrib->iv_len);
		pframe += pattrib->iv_len;
	}

	llc_sz = whc_host_xmit_put_snap(pframe, pattrib->ether_type);
	pframe += llc_sz;

	wlan_overhead_head_len = pframe - wlan_overhead_head;
	pframe = pkt->data + pattrib->pkt_hdrlen;
	mem_start = pframe - wlan_overhead_head_len;
	memcpy(mem_start, wlan_overhead_head, wlan_overhead_head_len);

	pkt->data = pframe - wlan_overhead_head_len;
	pframe += pattrib->pktlen;

	pattrib->last_txcmdsz = pattrib->hdrlen + pattrib->iv_len +  llc_sz + pattrib->pktlen;

	if (pattrib->encrypt == _TKIP_) {
		if (psta_securitypriv && whc_host_xmit_tkip_addmic(psecuritypriv, pxmitframe->pkt, psta_securitypriv, pattrib->last_txcmdsz) == -1) {
			dev_warn(global_idev.pwhc_dev, "addmic FAIL\n");
			res = -1;
			goto exit;
		}
		pframe += 8;
		pattrib->last_txcmdsz += 8;
	}

	if ((pattrib->icv_len > 0) && (pattrib->b_swenc)) {
		memset(pframe, 0, pattrib->icv_len);
		pframe += pattrib->icv_len;
		pattrib->last_txcmdsz += pattrib->icv_len;
	}

	/* TODO */
	whc_host_xmit_enc_software(iface_type, psecuritypriv, pxmitframe);

	if (bmcst == false) {
		whc_host_xmit_update_attrib_vcs_info(iface_type, pxmitframe, psta_mlmepriv, force_cts2self);
	} else {
		pattrib->vcs_mode = NONE_VCS;
	}

exit:

	if (wlan_overhead_head) {
		kfree(wlan_overhead_head);
	}
	return res;
}

int whc_host_hal_xmitframe_dump(u8 iface_type, struct xmit_frame *pxmitframe, u8 *wlan_hw_queue)
{
	struct whch_xmit_priv *pxmitpriv = &global_idev.xmitpriv;
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	struct sta_info *psta = NULL;
	struct sta_mlme_priv *psta_mlmepriv = NULL;
	struct sta_xmit_priv *psta_xmitpriv = NULL;
	struct hw_xmit *ppending_q = NULL;

	u32	hw_queue;
	u8 *ptxdesc;
	u8 enqueued = 0, offset = 0;

	psta = whc_host_sta_get_stainfo(iface_type, pattrib->ra);
	if (psta == NULL) {
		if (pattrib->b_tx_raw) {
			u8 raw_sta_addr[ETH_ALEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
			psta = whc_host_sta_get_stainfo(iface_type, raw_sta_addr);
		}
	}

	if (psta) {
		psta_mlmepriv = &psta->sta_mlmepriv;
		psta_xmitpriv = &psta->sta_xmitpriv;
	}

	if ((pxmitframe->frame_tag == DATA_FRAMETAG) &&
		(pattrib->ether_type != ETH_P_ARP) &&
		(pattrib->ether_type != ETH_P_PAE) &&
		(pattrib->pkt_type != PACKET_DHCP) &&
		(pattrib->b_tx_raw == false) &&
		psta_mlmepriv && global_idev.wifi_user_config.ampdu_tx_enable) {
		pattrib->issue_addba = 1; /* ADDBA move to dev */
	}

	hw_queue = whc_host_hal_hwqueue_get(pattrib->qsel);
	ppending_q = &global_idev.pending_queue[iface_type].hwxmits[hw_queue];
	*wlan_hw_queue = hw_queue;
	/*step1: set txdesc*/
	if (pxmitframe->pkt == NULL) {
		dev_err(global_idev.pwhc_dev, "xmitdump fail1\n");
		goto drop_pkt;
	}

	skb_push(pxmitframe->pkt, TXDESC_SIZE);
	ptxdesc = pxmitframe->pkt->data;
	//offset = ((u32)ptxdesc & 3);
	//ptxdesc -= offset;	//force ptxdesc 4-bytes aligned
	whc_host_hal_txdesc_fill(pxmitframe, ptxdesc);
	pxmitframe->pkt->len = pattrib->last_txcmdsz + TXDESC_SIZE + offset;
	dev_dbg(global_idev.pwhc_dev, "[whc] %s len=%d.", __func__, pxmitframe->pkt->len);

	/*step2: check enqueue*/
	/*step2.1: check if need move to sleep_q*/
	/* ap mode TODO */
#if 0
	if ((iface_type == WHC_AP_PORT) && rtw_ap_enqueue_for_sleeping_sta(psta_mlmepriv, pxmitframe->pkt) == true) {
		enqueued = 1;
	} else
#endif
	{
		/*step2.2: check if need move to pending_q*/
		spin_lock(&pxmitpriv->mutex);
		if ((whc_host_hal_pending_q_check(ppending_q) > 0) ||
			(whc_host_hal_txbd_enough_check() == 0) ||	//TODO: maybe check usb/sdio resource?
			global_idev.mlme_priv.b_in_scan || global_idev.mlme_priv.b_in_connect) {
			if (psta_xmitpriv) {
				//dev_dbg(global_idev.pwhc_dev, "[whc] %s %d enqueue.", __func__, __LINE__);
				whc_host_hal_pending_q_enqueue(ppending_q, psta_xmitpriv, pxmitframe->pkt, hw_queue);
				up(&pxmitpriv->hal_tx_sema);
				enqueued = 1;
			} else {
				spin_unlock(&pxmitpriv->mutex);
				goto drop_pkt;
			}
		}
		spin_unlock(&pxmitpriv->mutex);
	}

	pxmitframe->pkt = NULL;/*not free skb*/
	whc_host_xmitframe_free(pxmitframe);

	if (enqueued) {
		return RTK_TX_ENQUEUE;
	}
	return RTK_TX_DIRECT;

drop_pkt:
	whc_host_xmitframe_free(pxmitframe);
	return RTK_TX_DROP;
}

void whc_host_hal_xmit_check_eapol4(u8 iface_type, struct xmit_frame *pxmitframe)
{
	struct pkt_attrib	*pattrib = &pxmitframe->attrib;
	u8 eapol_type = 0;
	u8 eapol_keyinfo_bytes[2] = {0}, eapol_key_nonce[32] = {0};
	u16	eapol_keyinfo = 0;
	s32 llc_sz = sizeof(struct ieee80211_snap_hdr) + SNAP_PROTOCOL_ID_SIZE;

	if ((iface_type == WHC_STA_PORT) && (pattrib->ether_type == 0x888e)) {
		memcpy(&eapol_type, pxmitframe->pkt->data + pattrib->hdrlen + pattrib->iv_len + llc_sz + 1, 1);  // 3 means eapol key
		memcpy(eapol_keyinfo_bytes, pxmitframe->pkt->data + pattrib->hdrlen + pattrib->iv_len + llc_sz + 5, 2);
		eapol_keyinfo = (eapol_keyinfo_bytes[0] << 8) | eapol_keyinfo_bytes[1];

		if ((eapol_type == 0x03) && /* check if eapol key message 4*/
			(((~eapol_keyinfo & (BIT(3) | BIT(8))) == 0) && ((eapol_keyinfo & (BIT(6) | BIT(7) | BIT(12))) == 0)) &&
			/* bit3(keytype), bit8(key MIC) = 1; bit6(install), bit7(key ACK), bit12(encrypted key data) = 0 */
			(memcmp(pxmitframe->pkt->data + pattrib->hdrlen + pattrib->iv_len + llc_sz + 17, eapol_key_nonce, 32) == 0)) {
			/* msg4 key nonce 32 bytes = 0 */

			pattrib->tx_done_id = 1;	// eapol4, need polling tx_done
		}
	}
}

int whc_host_hal_xmit(u8 iface_type, struct xmit_frame *pxmitframe, u8 *wlan_hw_queue)
{
	int ret = RTK_TX_DROP;

	if (whc_host_hal_xmitframe_coalesce(iface_type, pxmitframe->pkt, pxmitframe, global_idev.wifi_user_config.force_cts2self) == 0) {
		whc_host_hal_xmit_check_eapol4(iface_type, pxmitframe);
		ret = whc_host_hal_xmitframe_dump(iface_type, pxmitframe, wlan_hw_queue);
	} else {
		whc_host_xmitframe_free(pxmitframe);
	}
	return ret;
}

int whc_host_hal_xmit_thread(void *data)
{
	struct whch_xmit_priv *pxmitpriv = &global_idev.xmitpriv;
	int ret = 0;

	while (!kthread_should_stop()) {

		/* wait for smea */
		ret = down_interruptible(&pxmitpriv->hal_tx_sema);

		/* dequeue sta_pending queue */
		whc_host_hal_pending_q_resume();
	}

	return ret;
}
#endif