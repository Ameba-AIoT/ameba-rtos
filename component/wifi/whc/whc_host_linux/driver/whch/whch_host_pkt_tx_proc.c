#include <whc_host_linux.h>

#ifdef CONFIG_WHCH
int	whc_host_xmit_priv_init(void)
{
	struct xmit_frame *pxframe;
	struct whch_xmit_priv *pxmitpriv = &global_idev.whchpriv.xmitpriv;
	u32 i;
	int	res = 0;

	spin_lock_init(&pxmitpriv->mutex);

	INIT_LIST_HEAD(&pxmitpriv->free_xmit_queue);
	spin_lock_init(&pxmitpriv->free_xmit_lock);

	/* init pxmitframe */
	pxmitpriv->pallocated_frame_buf = kmalloc(nr_xmitframe * sizeof(struct xmit_frame) + 4, GFP_KERNEL);
	if (pxmitpriv->pallocated_frame_buf  == NULL) {
		pxmitpriv->pxmit_frame_buf = NULL;
		res = -1;
		goto exit;
	}
	pxmitpriv->pxmit_frame_buf = (u8 *)N_BYTE_ALIGMENT((size_t)(pxmitpriv->pallocated_frame_buf), 4);
	pxframe = (struct xmit_frame *) pxmitpriv->pxmit_frame_buf;

	for (i = 0; i < nr_xmitframe; i++) {
		INIT_LIST_HEAD(&(pxframe->list));
		pxframe->iface_type = WHC_STA_PORT;
		pxframe->frame_tag = NULL_FRAMETAG;
		pxframe->pkt = NULL;
		list_add_tail(&(pxframe->list), &(pxmitpriv->free_xmit_queue));
		pxframe++;
	}

	atomic_set(&pxmitpriv->free_xmitframe_cnt, nr_xmitframe);

	/* init hal_xmit_thread */
	sema_init(&pxmitpriv->hal_tx_sema, 0);

	pxmitpriv->hal_tx_thread = kthread_run(whc_host_hal_xmit_thread, pxmitpriv, "WHCH_HAL_TX_THREAD");
	if (IS_ERR(pxmitpriv->hal_tx_thread)) {
		dev_err(global_idev.pwhc_dev, "FAIL to create hal_tx_thread!\n");
		pxmitpriv->hal_tx_thread = NULL;
		kfree(pxmitpriv->pallocated_frame_buf);
		return -EINVAL;
	}

exit:
	return res;
}

void whc_host_xmit_priv_free(void)
{
	struct whch_xmit_priv *pxmitpriv = &global_idev.whchpriv.xmitpriv;
	struct xmit_frame	*pxmitframe = (struct xmit_frame *)pxmitpriv->pxmit_frame_buf;
	int i;

	dev_dbg(global_idev.pwhc_dev, "[whc] %s.", __func__);

	/* stop hal_xmit_thread */
	if (pxmitpriv->hal_tx_thread) {
		up(&pxmitpriv->hal_tx_sema);
		kthread_stop(pxmitpriv->hal_tx_thread);
		pxmitpriv->hal_tx_thread = NULL;
	}

	/* free pxmitframe */
	if (pxmitpriv->pxmit_frame_buf == NULL) {
		goto out;
	}

	for (i = 0; i < nr_xmitframe; i++) {
		if (pxmitframe->pkt) {
			dev_kfree_skb_any(pxmitframe->pkt);
		}
		pxmitframe->pkt = NULL;
		pxmitframe++;
	}

	if (pxmitpriv->pallocated_frame_buf) {
		kfree(pxmitpriv->pallocated_frame_buf);
	}


out:
	return;
}

struct xmit_frame *whc_host_xmitframe_alloc(u8 iface_type)
{
	struct xmit_frame *pxframe = NULL;
	struct whch_xmit_priv *pxmitpriv = &global_idev.whchpriv.xmitpriv;
	struct list_head *plist, *phead;

	spin_lock_bh(&(pxmitpriv->free_xmit_lock));

	if (list_empty(&(pxmitpriv->free_xmit_queue)) == true) {
		pxframe = NULL;
		dev_err(global_idev.pwhc_dev, "[whc]: %s xmitframe not enough", __func__);
	} else {
		phead = &(pxmitpriv->free_xmit_queue);
		plist = phead->next;
		pxframe = list_entry(plist, struct xmit_frame, list);
		list_del(&(pxframe->list));
	}

	if (pxframe != NULL) { //default value setting
		atomic_dec(&pxmitpriv->free_xmitframe_cnt);
		memset(&pxframe->attrib, 0, sizeof(struct pkt_attrib));
		pxframe->frame_tag = DATA_FRAMETAG;
		pxframe->iface_type = iface_type;
	}

	spin_unlock_bh(&(pxmitpriv->free_xmit_lock));

	return pxframe;
}

int whc_host_xmitframe_free(struct xmit_frame *pxmitframe)
{
	struct sk_buff *pndis_pkt = NULL;
	struct whch_xmit_priv *pxmitpriv = &global_idev.whchpriv.xmitpriv;

	if (pxmitframe == NULL) {
		goto exit;
	}

	spin_lock_bh(&(pxmitpriv->free_xmit_lock));
	if (pxmitframe->pkt) {
		pndis_pkt = pxmitframe->pkt;
		pxmitframe->pkt = NULL;
	}

	list_add_tail(&pxmitframe->list, &(pxmitpriv->free_xmit_queue));
	atomic_inc(&pxmitpriv->free_xmitframe_cnt);
	spin_unlock_bh(&(pxmitpriv->free_xmit_lock));

	if (pndis_pkt) {
		dev_kfree_skb_any(pndis_pkt);
	}

exit:
	return 0;
}

void whc_host_xmit_open_pktfile(struct sk_buff *pktptr, struct pkt_file *pfile)
{
	pfile->pkt = pktptr;
	pfile->cur_addr = pfile->buf_start = pktptr->data;
	pfile->pkt_len = pfile->buf_len = pktptr->len;
	pfile->cur_buffer = pfile->buf_start;
}

u32 whc_host_xmit_read_pktfile(struct pkt_file *pfile, u8 *rmem, u32 rlen)
{
	u32	remainder_len = 0;

	remainder_len =  pfile->buf_len - ((size_t)(pfile->cur_addr) - (size_t)(pfile->buf_start));
	remainder_len = (rlen > remainder_len) ? remainder_len : rlen;

	if (rmem) {
		memcpy(rmem, pfile->cur_addr, remainder_len);
	}

	pfile->cur_addr += remainder_len;
	pfile->pkt_len -= remainder_len;

	return remainder_len;
}

int whc_host_xmit_make_wlanhdr(u8 iface_type, u8 *hdr, struct pkt_attrib *pattrib)
{
	struct whch_qos_priv *pqospriv = &global_idev.whchpriv.qospriv[iface_type];
	struct rtw_ieee80211_hdr_3addr	*pwlanhdr = (struct rtw_ieee80211_hdr_3addr *)hdr;
	struct sta_info *psta = NULL;
	struct sta_mlme_priv *psta_mlmepriv = NULL;
	struct sta_security_priv *psta_securitypriv = NULL;
	struct sta_xmit_priv *psta_xmitpriv = NULL;
	struct sta_ht_priv *psta_htpriv = NULL;
	u16	*qc = NULL;
	u16	fctrl = 0;
	u8	*mybssid = global_idev.bssid;
	u8	qos_option = false;
	u8	htc_option = false;
#ifdef CONFIG_80211AX_HE
	u8	*htc = NULL;
#endif
	int	res = 0;

	psta = whc_host_sta_get_stainfo(iface_type, pattrib->ra);

	if (psta) {
		psta_mlmepriv = &psta->sta_mlmepriv;
		psta_securitypriv = &psta->sta_security;
		psta_xmitpriv = &psta->sta_xmitpriv;
		psta_htpriv = &psta->sta_htpriv;
	}

	/* For wifi cast */
	if (pattrib->b_tx_raw == true) { /* TODO_raw */
		if (pqospriv->qos_option) {
			qos_option = true;
		}
		goto skip_wlanhdr;
	} else {
		memset(hdr, 0, sizeof(struct rtw_ieee80211_hdr_3addr));
		fctrl = pwlanhdr->frame_ctl;
		set_frame_sub_type(&fctrl, pattrib->type_subtype);
		pwlanhdr->frame_ctl = fctrl;
	}
#ifdef CONFIG_NAN
	if (pattrib->nan_pkt_type == NAN_PKT_TYPE_DATA) {
		rtw_xmit_make_wlanhdr_nanaddr(pwlanhdr, pattrib); //TODO_NAN
	} else
#endif
	{
		if (iface_type == WHC_STA_PORT) {
			//to_ds = 1, fr_ds = 0;
			//Data transfer to AP
			fctrl = pwlanhdr->frame_ctl;
			SetToDs(&fctrl);
			pwlanhdr->frame_ctl = fctrl;
			memcpy(pwlanhdr->addr1, mybssid, ETH_ALEN);
			memcpy(pwlanhdr->addr2, pattrib->src, ETH_ALEN);
			memcpy(pwlanhdr->addr3, pattrib->dst, ETH_ALEN);

			if (pqospriv->qos_option) {
				qos_option = true;
			}
		} else if (iface_type == WHC_AP_PORT) {
			//to_ds = 0, fr_ds = 1;
			fctrl = pwlanhdr->frame_ctl;
			SetFrDs(&fctrl);
			pwlanhdr->frame_ctl = fctrl;
			memcpy(pwlanhdr->addr1, pattrib->dst, ETH_ALEN);
			memcpy(pwlanhdr->addr2, mybssid, ETH_ALEN);
			memcpy(pwlanhdr->addr3, pattrib->src, ETH_ALEN);
			if (psta_mlmepriv && psta_mlmepriv->b_sta_qos_option) {
				qos_option = true;
			}
		} else {
			return -1;
		}
	}

	if (pattrib->b_mdata) {
		fctrl = pwlanhdr->frame_ctl;
		SetMData(&fctrl);
		pwlanhdr->frame_ctl = fctrl;
	}

	if (pattrib->encrypt) {
		fctrl = pwlanhdr->frame_ctl;
		SetPrivacy(&fctrl);
		pwlanhdr->frame_ctl = fctrl;
	}

skip_wlanhdr:
	if (qos_option) {
		if (pattrib->b_tx_order) {  // htc only support QoS Data/Mgnt/Qos Null
			htc_option = true;
			fctrl = pwlanhdr->frame_ctl;
			SetOrder(&fctrl);
			pwlanhdr->frame_ctl = fctrl;
#ifdef CONFIG_80211AX_HE
			/* fill ht control filed */
			htc = (u8 *)(void *)(hdr + pattrib->hdrlen - 4 * htc_option);
			rtw_he_fill_htc(htc); /* TODO_AX */
#endif
		}
		qc = (unsigned short *)(void *)(hdr + pattrib->hdrlen - 4 * htc_option - 2);
		memset(qc, 0, 2);

		if (pattrib->priority) {
			SetPriority(qc, pattrib->priority);
		}

		SetEOSP(qc, pattrib->b_tx_eosp);
		SetAckpolicy(qc, pattrib->ack_policy);
	}

#if 0
	//Update Seq Num will be handled by f/w
	if (psta_xmitpriv) {
		psta_xmitpriv->txseq_tid[pattrib->priority]++;
		psta_xmitpriv->txseq_tid[pattrib->priority] &= 0xFFF;
		pattrib->seqnum = psta_xmitpriv->txseq_tid[pattrib->priority];
		SetSeqNum(hdr, pattrib->seqnum);

		//check if enable ampdu
		if (pattrib->b_ht_en && psta_htpriv->ampdu_enable) {
			if (psta_htpriv->addba_success_bitmap & BIT(pattrib->priority)) {
				pattrib->b_ampdu_en = true;
			}
		}

		//re-check if enable ampdu by BA_starting_seqctrl
		if (pattrib->b_ampdu_en == true) {
			u16 tx_seq;

			tx_seq = psta_xmitpriv->ba_starting_seqctrl[pattrib->priority & 0x0f];

			//check BA_starting_seqctrl
			if (SN_LESS(pattrib->seqnum, tx_seq)) {
				pattrib->b_ampdu_en = false;//AGG BK
			} else if (SN_EQUAL(pattrib->seqnum, tx_seq)) {
				psta_xmitpriv->ba_starting_seqctrl[pattrib->priority & 0x0f] = (tx_seq + 1) & 0xfff;
				pattrib->b_ampdu_en = true;//AGG EN
			} else {
				psta_xmitpriv->ba_starting_seqctrl[pattrib->priority & 0x0f] = (pattrib->seqnum + 1) & 0xfff;
				pattrib->b_ampdu_en = true;//AGG EN
			}
		}
	}
#endif

	return res;
}

/* Logical Link Control(LLC) SubNetwork Attachment Point(SNAP) header
 * IEEE LLC/SNAP header contains 8 octets
 * First 3 octets comprise the LLC portion
 * SNAP portion, 5 octets, is divided into two fields:
 *	Organizationally Unique Identifier(OUI), 3 octets,
 *	type, defined by that organization, 2 octets.
 */
s32 whc_host_xmit_put_snap(u8 *data, u16 h_proto)
{
	struct ieee80211_snap_hdr *snap;

	snap = (struct ieee80211_snap_hdr *)data;
	snap->dsap = 0xaa;
	snap->ssap = 0xaa;
	snap->ctrl = 0x03;

	if (h_proto == 0x8137 || h_proto == 0x80f3) {
		snap->oui[0] = 0x00;
		snap->oui[1] = 0x00;
		snap->oui[2] = 0xF8;
	} else {
		snap->oui[0] = 0x00;
		snap->oui[1] = 0x00;
		snap->oui[2] = 0x00;
	}
	*(u16 *)(data + sizeof(struct ieee80211_snap_hdr)) = htons(h_proto);
	return sizeof(struct ieee80211_snap_hdr) + SNAP_PROTOCOL_ID_SIZE;
}

int whc_host_xmit_tkip_addmic(struct whch_security_priv *psecuritypriv, struct sk_buff *pkt, struct sta_security_priv *psta_security, u16 pkt_len)
{
	u8 *pframe, *payload, mic[8];
	struct mic_data micdata;
	u8 priority[4] = {0x0, 0x0, 0x0, 0x0};
	signed int bmcst = IS_MCAST(get_ra(pkt->data));
	u8 hdrlen = 0, iv_len = 8;
	u16 length = 0;

	//encode mic code
	u8 null_key[16] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	pframe = pkt->data;
	if (bmcst) {
		if (!memcmp(psecuritypriv->dot11_tkip_grpmickey_tx[0].skey, null_key, 16)) {
			return -1;
		}
		//start to calculate the mic code
		whc_host_secmicsetkey(&micdata, psecuritypriv->dot11_tkip_grpmickey_tx[0].skey);
	} else {
		if (!memcmp(&psta_security->dot11tkiptxmickey.skey[0], null_key, 16)) {
			return -1;
		}
		//start to calculate the mic code
		whc_host_secmicsetkey(&micdata, &psta_security->dot11tkiptxmickey.skey[0]);
	}

	if (pframe[1] & 1) { //ToDS==1
		whc_host_secmicappend(&micdata, &pframe[16], 6);  //DA
		if (pframe[1] & 2) { //From Ds==1
			whc_host_secmicappend(&micdata, &pframe[24], 6);
		} else {
			whc_host_secmicappend(&micdata, &pframe[10], 6);
		}
	} else {	//ToDS==0
		whc_host_secmicappend(&micdata, &pframe[4], 6);   //DA
		if (pframe[1] & 2) { //From Ds==1
			whc_host_secmicappend(&micdata, &pframe[16], 6);
		} else {
			whc_host_secmicappend(&micdata, &pframe[10], 6);
		}
	}

	if (is_qos(pframe)) {
		hdrlen = WLAN_HDR_A3_QOS_LEN; //ignore get_tofr_ds=3
		if (GetOrder(pframe)) {
			hdrlen += 4;
		}
		priority[0] = GetPriority((pframe + hdrlen - 2));
	} else {
		hdrlen = WLAN_HDR_A3_LEN;
	}
	whc_host_secmicappend(&micdata, &priority[0], 4);

	if (pkt_len > hdrlen + iv_len) {
		payload = pframe + hdrlen + iv_len;
		length = pkt_len - hdrlen - iv_len;
	} else {
		return -1;
	}
	whc_host_secmicappend(&micdata, payload, length);
	whc_host_secgetmic(&micdata, &(mic[0]));

	payload = payload + length;
	memcpy(payload, &(mic[0]), 8);

	return 0;
}

int whc_host_xmit_enc_software(u8 iface_type, struct whch_security_priv *psecuritypriv, struct xmit_frame *pxmitframe)
{
	struct pkt_attrib	*pattrib = &((struct xmit_frame *)pxmitframe)->attrib;
	struct sta_info *psta = NULL;
	struct sta_security_priv *psta_securitypriv = NULL;
	u8			*pframe, *prwskey = NULL;

	if (pattrib->b_swenc) {
		pframe = ((struct xmit_frame *)pxmitframe)->pkt->data;
		if ((pattrib->encrypt == _TKIP_) || (pattrib->encrypt == _AES_) || (pattrib->encrypt == _GCMP_256_)) {
			psta = whc_host_sta_get_stainfo(iface_type, pattrib->ra);
			if (psta) {
				psta_securitypriv = &psta->sta_security;
			} else {
				return -1;
			}

			if (IS_MCAST(pattrib->ra)) {
				prwskey = psecuritypriv->dot11_wpa_grpkey[0].skey;
			} else {
				prwskey = &psta_securitypriv->dot118021x_UncstKey.skey[0];
			}
		}

		switch (pattrib->encrypt) {
		case _WEP40_:
		case _WEP104_:
			wep_80211_encrypt(
				pframe, // Frame offset. Early mode and USB aggregation is not considered
				pattrib->hdrlen,	// WLAN header length
				pattrib->iv_len,	// IV length
				pattrib->last_txcmdsz - pattrib->hdrlen - pattrib->iv_len - pattrib->icv_len,	// Data length
				&psecuritypriv->dot11_wep_key[0].skey[0],// WEP key
				psecuritypriv->dot11_wep_keylen[0]//WEP key_len,
			);
			break;

		case _TKIP_:
			tkip_80211_encrypt(
				pframe, // Frame offset. Early mode and USB aggregation is not considered
				pattrib->hdrlen,	// WLAN header length
				pattrib->iv_len,	// IV length
				pattrib->last_txcmdsz - pattrib->hdrlen - pattrib->iv_len - pattrib->icv_len, // Payload Length
				prwskey, 			// tkip key
				16,				// tkip key length
				&pattrib->ta[0]	// transmit address for ken generation
			);
			break;
		case _AES_:
			aes_80211_encrypt(
				pframe,  // Frame offset. Early mode and USB aggregation is not considered
				pattrib->hdrlen,			// WLAN header length
				pattrib->last_txcmdsz - pattrib->hdrlen - pattrib->iv_len - pattrib->icv_len, 	// payload length
				prwskey, 					// AES key
				get_frame_sub_type(pframe)	// Frame Type
			);
			break;
		case _GCMP_256_:
			gcmp_80211_encrypt(
				pframe,  // Frame offset. Early mode and USB aggregation is not considered
				pattrib->hdrlen,			// WLAN header length
				pattrib->last_txcmdsz - pattrib->hdrlen - pattrib->iv_len - pattrib->icv_len, 	// payload length
				prwskey, 					// gcmp-256 key
				(pattrib->encrypt == _GCMP_256_) ? 32 : 16
			);
			break;
		default:
			dev_err(global_idev.pwhc_dev, "SW ENC: NOT SUPPORTED ALG %x \r\n", pattrib->encrypt);
			break;
		}
	}

	return 0;
}

void whc_host_xmit_update_attrib_vcs_info(u8 iface_type, struct xmit_frame *pxmitframe, struct sta_mlme_priv *psta_mlmepriv, u8 f_cts2self)
{
	struct pkt_attrib	*pattrib = &pxmitframe->attrib;
	struct whch_ht_priv *phtpriv = &global_idev.whchpriv.htpriv[iface_type];

	/* RTS/CTS is determined by HW by default, except for the following situations
	  1. Force sending RTS/CTS if need ERP protection since the presence of 11b STAs
	  2. Force sending RTS/CTS if not need ERP protection but need HT protection since the presence of 11g/11bg STAs
	  3. Force sending CTS2SELF if needed by user config
	*/
	pattrib->vcs_mode = RTS_CTS_BY_HW;
	if (psta_mlmepriv->b_erp_protection) {   //1. check ERP protection
		pattrib->vcs_mode = RTS_CTS_FORCE;
	} else if (pattrib->b_ht_en && phtpriv->HT_protection == 3) {  //2. check HT protection
		pattrib->vcs_mode = RTS_CTS_FORCE;
	} else if (f_cts2self) {  //3. check user config
		pattrib->vcs_mode = CTS_TO_SELF_FORCE;
	}
}

void whc_host_xmit_update_attrib_set_qos(struct pkt_file *ppktfile, struct pkt_attrib *pattrib)
{
	struct ethhdr etherhdr;
	struct iphdr ip_hdr;
	struct ipv6_hdr ipv6_hdr;
	s32 UserPriority = 0;
	u8 traffic_class = 0;
	u32 v_tc_fl = 0;

	whc_host_xmit_open_pktfile(ppktfile->pkt, ppktfile);
	whc_host_xmit_read_pktfile(ppktfile, (unsigned char *)&etherhdr, ETH_HLEN);

	// get UserPriority from IP hdr
	if (pattrib->ether_type == ETH_P_IP) {
		whc_host_xmit_read_pktfile(ppktfile, (u8 *)&ip_hdr, sizeof(ip_hdr));
		UserPriority = ip_hdr.tos >> 5;
	} else if (pattrib->ether_type == ETH_P_IPV6) {//ipv6
		whc_host_xmit_read_pktfile(ppktfile, (u8 *)&ipv6_hdr, sizeof(ipv6_hdr));
		v_tc_fl = ipv6_hdr.v_tc_fl;
		v_tc_fl = ((v_tc_fl & 0x000000ffUL) << 24) | ((v_tc_fl & 0x0000ff00UL) <<  8) |
				  ((v_tc_fl & 0x00ff0000UL) >>  8) | ((v_tc_fl & 0xff000000UL) >> 24);
		traffic_class = (v_tc_fl >> 20) & 0xFF;
		UserPriority = traffic_class >> 5;
	} else if (pattrib->ether_type == ETH_P_PAE) {
		// "When priority processing of data frames is supported,
		// a STA's SME should send EAPOL-Key frames at the highest priority."
		UserPriority = 7;
	}

	pattrib->priority = UserPriority;
	pattrib->hdrlen = WLAN_HDR_A3_QOS_LEN;
	pattrib->type_subtype = RTW_QOS_DATA_TYPE;
}

u8	whc_host_xmit_update_attrib_qos_acm(u8 acm_mask, u8 priority)
{
	u8	change_priority = priority;

	switch (priority) {
	case 0:
	case 3:
		if (acm_mask & BIT(1)) {
			change_priority = 1;
		}
		break;
	case 1:
	case 2:
		break;
	case 4:
	case 5:
		if (acm_mask & BIT(2)) {
			change_priority = 0;
		}
		break;
	case 6:
	case 7:
		if (acm_mask & BIT(3)) {
			change_priority = 5;
		}
		break;
	default:
		dev_warn(global_idev.pwhc_dev, "Invalid priority: %d!\n", priority);

		break;
	}

	return change_priority;
}

int whc_host_xmit_update_attrib_sec_info(struct whch_security_priv *psecuritypriv, struct pkt_attrib *pattrib, struct sta_security_priv *psta_security)
{
	int res = 0;
	signed int bmcast = IS_MCAST(pattrib->ra);
#ifdef CONFIG_NAN
	if (pattrib->nan_pkt_type == NAN_PKT_TYPE_DATA) {
		if (psta_security->dot11_security_privacy == _AES_) {
			if (bmcast) {
				pattrib->key_idx = (u8)psecuritypriv->dot11_wpa_grpkey_index;
				pattrib->encrypt = _AES_;
			} else {
				pattrib->key_idx = 0;
				pattrib->encrypt = _AES_;
			}
		} else {
			pattrib->encrypt = _NO_PRIVACY_;
		}
	} else if (psta_security->b_ieee8021x_blocked == true)
#else
	if (psta_security->b_ieee8021x_blocked == true)
#endif
	{
		pattrib->encrypt = 0;
		if (pattrib->ether_type != ETH_P_PAE) {
			res = -1;
			goto exit;
		}
	} else {
		if (psecuritypriv->dot11_wpa_mode) { /* WPA/WPA2/WPA3 */
			if (bmcast) {
				pattrib->encrypt = psecuritypriv->dot118021XGrpPrivacy;
				pattrib->key_idx = (u8)psecuritypriv->dot11_wpa_grpkey_index;
			} else {
				pattrib->encrypt = psta_security->dot11_security_privacy;
				pattrib->key_idx = 0;
			}
		} else { /* OPEN/WEP */
			pattrib->encrypt = psecuritypriv->dot11PrivacyAlgrthm;
			pattrib->key_idx = (u8)psecuritypriv->dot11_wep_key_index;
		}
	}

	switch (pattrib->encrypt) {
	case _WEP40_:
	case _WEP104_:
		pattrib->iv_len = 4;
		pattrib->icv_len = 4;
		break;
	case _TKIP_:
		pattrib->iv_len = 8;
		pattrib->icv_len = 4;
		if (psecuritypriv->b_usetkipkey == false) {
			res = -1;
			goto exit;
		}
		break;
	case _AES_:
		pattrib->iv_len = 8;
		pattrib->icv_len = 8;
		break;
	case _GCMP_:
	case _GCMP_256_:
		pattrib->iv_len = 8;
		pattrib->icv_len = 16;
		break;
	case _CCMP_256_:
		pattrib->iv_len = 8;
		pattrib->icv_len = 16;
		break;
	default:
		pattrib->iv_len = 0;
		pattrib->icv_len = 0;
		break;
	}

	if (pattrib->encrypt && (psecuritypriv->b_sw_encrypt == true)) {
		pattrib->b_swenc = true;
	} else {
		pattrib->b_swenc = false;
	}

exit:
	return res;
}

void whc_host_xmit_update_attrib_phy_info(struct pkt_attrib *pattrib, struct sta_mlme_priv *psta_mlmepriv, struct sta_ht_priv *psta_htpriv, u8 bw_40_en)
{
	pattrib->b_mdata = 0;
	pattrib->b_tx_eosp = 0;

	if (psta_mlmepriv) {
		//qos_en, ht_en, init rate, ,bw, ch_offset, sgi
		pattrib->b_qos_en = psta_mlmepriv->b_sta_qos_option;
		pattrib->b_ht_en = psta_htpriv->ht_option;
		if (pattrib->b_tx_raw) {
			pattrib->bwmode = bw_40_en ? CHANNEL_WIDTH_40 : CHANNEL_WIDTH_20;
		} else {
			pattrib->bwmode = psta_htpriv->bwmode;
		}
		pattrib->ch_offset = psta_htpriv->ch_offset;
		pattrib->b_tx_sgi = (pattrib->bwmode == CHANNEL_WIDTH_40) ? psta_htpriv->sgi_40m : psta_htpriv->sgi_20m;
	} else if (bw_40_en) {
		// for no link tx TODO_raw, check whether need
		pattrib->b_ht_en = 1;
		pattrib->bwmode = CHANNEL_WIDTH_40;
	}
}

int whc_host_xmit_update_attrib(u8 iface_type, struct sk_buff *pkt, struct pkt_attrib *pattrib)
{
	struct whch_security_priv *psecuritypriv = &global_idev.whchpriv.securitypriv[iface_type];
	struct whch_qos_priv *pqospriv = &global_idev.whchpriv.qospriv[iface_type];
	struct pkt_file	pktfile;
	struct sta_info *psta = NULL;
	struct sta_mlme_priv *psta_mlmepriv = NULL;
	struct sta_ht_priv *psta_htpriv = NULL;
	struct sta_security_priv *psta_securitypriv = NULL;
	struct ethhdr	etherhdr;
	signed int	bmcast;
	int			res = 0;
	u8			*mybssid = global_idev.bssid;
	u8			arp_type = 0;
#ifdef CONFIG_NAN /* TODO_NAN */
	struct nan_peer_info_t *ppeer_info = NULL;
	u8 is_nan_bcmc = 0;
#endif

	whc_host_xmit_open_pktfile(pkt, &pktfile);
	whc_host_xmit_read_pktfile(&pktfile, (u8 *)&etherhdr, ETH_HLEN);

	pattrib->ether_type = htons(etherhdr.h_proto);

	//dev_dbg(global_idev.pwhc_dev, "[whc]: %s ether_type=%x %x", __func__, etherhdr.h_proto, pattrib->ether_type);

	memcpy(pattrib->dst, &etherhdr.h_dest, ETH_ALEN);
	memcpy(pattrib->src, &etherhdr.h_source, ETH_ALEN);

	if (iface_type == WHC_STA_PORT) {
		memcpy(pattrib->ra, mybssid, ETH_ALEN);
		memcpy(pattrib->ta, pattrib->src, ETH_ALEN);
	} else if (iface_type == WHC_AP_PORT) {
		memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);
		memcpy(pattrib->ta, mybssid, ETH_ALEN);
	}

	pattrib->pktlen = pktfile.pkt_len;
	pattrib->pkt_type = PACKET_NORMAL;

	/* parse pkt_type for lps&btcoex use */
	if (pattrib->ether_type == ETH_P_IP) {
		// The following is for DHCP, we use OFDM6M to tx these packets and let LPS awake some time
		// to prevent DHCP protocol fail
		u8 tmp[24];
		whc_host_xmit_read_pktfile(&pktfile, &tmp[0], 24);
		if (pktfile.pkt_len > 282) {//MINIMUM_DHCP_PACKET_SIZE
			if (((tmp[21] == 68) && (tmp[23] == 67)) ||
				((tmp[21] == 67) && (tmp[23] == 68))) {
				// 68 : UDP BOOTP client
				// 67 : UDP BOOTP server
				pattrib->pkt_type = PACKET_DHCP;
			}
		}
	} else if (pattrib->ether_type == ETH_P_ARP) {
		whc_host_xmit_read_pktfile(&pktfile, NULL, 7);
		whc_host_xmit_read_pktfile(&pktfile, &arp_type, 1);//arp_type : 1 arp request  2 arp response
		if (arp_type == 1) {
			pattrib->pkt_type = PACKET_ARP;
		}
	} else if (pattrib->ether_type == ETH_P_PAE) {
		pattrib->pkt_type = PACKET_EAPOL;
	}

#ifdef CONFIG_NAN  /* TODO_NAN */
	/* Get dst IP address from IPv6 header to decide is NAN frame or not */
	if ((iface_type == NAN_PORT) && rtw_check_nan_enabled()) {
		ppeer_info = rtw_nan_peer_info_get_by_ndi(pattrib->dst);
		if (ppeer_info && (ppeer_info->peer_info_ndl_establish || ppeer_info->peer_info_ndl_renego)) {
			memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);
			memcpy(pattrib->ta, g_nanpriv.nmi.mac, ETH_ALEN);
			pattrib->nan_pkt_type = NAN_PKT_TYPE_DATA;
#ifdef CONFIG_NAN_PAIRING
		} else if (IS_IPV6_MCAST(pattrib->dst)) {
			ppeer_info = rtw_nan_peer_info_first_entry_get(true);
			if (ppeer_info) {
				memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);
				memcpy(pattrib->ta, g_nanpriv.nmi.mac, ETH_ALEN);
				pattrib->nan_pkt_type = NAN_PKT_TYPE_DATA;
				is_nan_bcmc = 1;
				dev_dbg(global_idev.pwhc_dev, "is_nan_bcmc\n");
			}
#endif
		}
	}
#endif

	bmcast = IS_MCAST(pattrib->ra);

	// get sta_info
	psta = whc_host_sta_get_stainfo(iface_type, pattrib->ra);

	if (psta) {
		psta_mlmepriv = &psta->sta_mlmepriv;
		psta_securitypriv = &psta->sta_security;
		psta_htpriv = &psta->sta_htpriv;
		if (!bmcast &&
			((iface_type == WHC_AP_PORT) && (!whc_host_state_check_ap_client_assoc_success(psta_mlmepriv)))
#ifdef CONFIG_NAN
			|| ((iface_type == WHC_NAN_PORT) && rtw_check_nan_enabled() && !rtw_check_nan_datalink_exist())
#endif
		   ) { // for softap unassociated_sta and nan unlinked_NDL => drop the pkt
			res = -1;
			dev_warn(global_idev.pwhc_dev, "sta not associated to softap\n");
			goto exit;
		}
		pattrib->mac_id = psta_mlmepriv->stainfo_macid;
		pattrib->agg_num = psta_mlmepriv->tx_agg_num;
		pattrib->ampdu_density = psta_mlmepriv->tx_ampdu_density;
	} else {
		// if we cannot get psta => drop the pkt
		dev_warn(global_idev.pwhc_dev, "NULL psta_mlmepriv\n");
		res = -1;
		goto exit;
	}

#ifdef CONFIG_NAN_PAIRING
	if (is_nan_bcmc) {
		/* for nan bcmc data, maybe assgin macid as one peerinfo, to decide which FAW to tx */
		pattrib->mac_id = ppeer_info->peer_ndl_mac_id;
		/* currently, use sw encrypt for nan bcmc */
		pattrib->b_swenc = 1;
	}
#endif

	pattrib->ack_policy = 0;

	pattrib->pkt_hdrlen = ETH_HLEN;
	pattrib->hdrlen = WLAN_HDR_A3_LEN;
	pattrib->type_subtype = RTW_DATA_TYPE;
	pattrib->priority = 0;

	if ((iface_type == WHC_AP_PORT)
#ifdef CONFIG_NAN
		|| ((iface_type == WHC_NAN_PORT) && rtw_check_nan_enabled() && rtw_check_nan_datalink_exist())
#endif
	   ) {
		if (psta_mlmepriv->b_sta_qos_option) {
			whc_host_xmit_update_attrib_set_qos(&pktfile, pattrib);
		}
	} else if (iface_type == WHC_STA_PORT) {
		if (pqospriv->qos_option) {
			whc_host_xmit_update_attrib_set_qos(&pktfile, pattrib);

			if (global_idev.whchpriv.mlmepriv.acm_mask != 0) {
				pattrib->priority = whc_host_xmit_update_attrib_qos_acm(global_idev.whchpriv.mlmepriv.acm_mask, pattrib->priority);
			}

#ifdef WIFI_LOGO_CERTIFICATION //wifi logo require tb ppdu with uph
			/* htc only support QoS Data/Mgnt/Qos Null */
			if (psta_htpriv->he_option == true) {
				/* by test, eapol & arp & dhcp pkt can not append ht control */
				if ((pattrib->pkt_type == PACKET_EAPOL) || (pattrib->ether_type == ETH_P_ARP) || (pattrib->pkt_type == PACKET_DHCP)) {
					pattrib->b_tx_order = 0;
				} else {
					pattrib->b_tx_order = psta_mlmepriv->asoc_cap.htc_rx;
				}
			}
#endif
			if (pattrib->b_tx_order) {
				pattrib->hdrlen = WLAN_HDR_A3_QOS_HTC_LEN;
			}

		}
	}

	if (whc_host_xmit_update_attrib_sec_info(psecuritypriv, pattrib, psta_securitypriv) == -1) {
		res = -1;
		dev_err(global_idev.pwhc_dev, "xmit_update_attrib_sec failed\n");
		goto exit;
	}
	whc_host_xmit_update_attrib_phy_info(pattrib, psta_mlmepriv, psta_htpriv, global_idev.wifi_user_config.bw_40_enable);

	pattrib->qsel = pattrib->priority;

exit:
	return res;
}

int whc_host_xmit_prehandle(int idx, struct sk_buff *pskb)
{
	struct xmit_frame *pxmitframe = NULL;
	struct pkt_attrib *pattrib;
	int ret = RTK_TX_DROP;
	u32 need_headroom, pad_len;

	/* reserve headroom for macheader and mac txdesc */
	need_headroom = WLAN_MAX_PROTOCOL_OVERHEAD + TXDESC_SIZE - 8;
	pad_len = BUF_ALIGN_SZ ? ((uintptr_t)(pskb->data - need_headroom) % BUF_ALIGN_SZ) : 0;
	need_headroom += pad_len;

	if (skb_headroom(pskb) < need_headroom) {
		if (pskb_expand_head(pskb, need_headroom, 0, GFP_ATOMIC)) {
			dev_warn(global_idev.pwhc_dev, "expand headroom FAIL!\n");
			goto drop_packet;
		}
	}

	pxmitframe = whc_host_xmitframe_alloc(idx);
	if (pxmitframe == NULL) {
		dev_warn(global_idev.pwhc_dev, "[whc]: %s xmit: no more pxmitframe", __func__);
		goto drop_packet;
	}

	if (whc_host_xmit_update_attrib(idx, pskb, &pxmitframe->attrib) == -1) {
		whc_host_xmitframe_free(pxmitframe);
		goto drop_packet;
	}

	pxmitframe->pkt = pskb;
	/* 802.3 to 802.11 */
	/* add mac txdesc. and for some ic, txdesc should be 4-bytes aligned */
	ret = whc_host_hal_xmit(idx, pxmitframe); /* after whc_host_hal_xmit success,the pxmitframe is freed*/

	goto exit;

drop_packet:
	dev_kfree_skb_any(pskb);
exit:
	return ret;
}

#endif