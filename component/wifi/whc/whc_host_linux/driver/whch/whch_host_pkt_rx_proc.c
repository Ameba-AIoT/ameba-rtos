#include <whc_host_linux.h>

#ifdef CONFIG_WHCH

static const u8 SNAP_ETH_TYPE_IPX[] = {0x81, 0x37};
static const u8 SNAP_ETH_TYPE_APPLETALK_AARP[] = {0x80, 0xF3};

int whc_host_recv_priv_init(void)
{
	int	i;
	int	res = 0;
	union recv_frame	*precvframe;
	u8					*precv_frame_buf = NULL;
	struct whch_recv_priv 	*precvpriv = &global_idev.whchpriv.recvpriv;

	INIT_LIST_HEAD(&precvpriv->free_recv_queue);
	spin_lock_init(&precvpriv->free_recv_lock);

	precvpriv->free_recvframe_cnt = 8;

	whc_host_hal_deseg_priv_reset(&precvpriv->deseg_priv);

	precvpriv->pallocated_frame_buf = kmalloc(precvpriv->free_recvframe_cnt * sizeof(union recv_frame) + RXFRAME_ALIGN_SZ, GFP_KERNEL);
	if (precvpriv->pallocated_frame_buf == NULL) {
		res = -1;
		goto exit;
	}

	precv_frame_buf = (u8 *)N_BYTE_ALIGMENT((size_t)(precvpriv->pallocated_frame_buf), RXFRAME_ALIGN_SZ);
	precvframe = (union recv_frame *)precv_frame_buf;

	for (i = 0; i < precvpriv->free_recvframe_cnt; i++) {
		INIT_LIST_HEAD(&(precvframe->u.list));
		list_add_tail(&(precvframe->u.list), &(precvpriv->free_recv_queue));
		precvframe->u.hdr.pkt = NULL;
		precvframe->u.hdr.len = 0;
		precvframe->u.hdr.iface_type = WHC_STA_PORT;
		precvframe++;
	}
exit:
	return res;
}

void whc_host_recv_priv_free(void)
{
	if (global_idev.whchpriv.recvpriv.pallocated_frame_buf) {
		kfree(global_idev.whchpriv.recvpriv.pallocated_frame_buf);
	}
}

union recv_frame *whc_host_recv_alloc_frame(void)
{
	struct whch_recv_priv 	*precvpriv = &global_idev.whchpriv.recvpriv;
	struct list_head    *plist, *phead;
	union recv_frame	*precvframe;

	spin_lock_bh(&(precvpriv->free_recv_lock));

	if (list_empty(&precvpriv->free_recv_queue) == true) {
		dev_err(global_idev.pwhc_dev, "[whc]: rcvfrm no enough\n");
		precvframe = NULL;
	} else {
		phead = &precvpriv->free_recv_queue;
		plist = phead->next;
		precvframe = list_entry(plist, union recv_frame, u.hdr.list);
		list_del_init(&precvframe->u.hdr.list);

		precvpriv->free_recvframe_cnt--;
	}
	spin_unlock_bh(&(precvpriv->free_recv_lock));
	return precvframe;
}

int whc_host_recv_free_frame(union recv_frame *precvframe)
{
	struct whch_recv_priv 	*precvpriv = &global_idev.whchpriv.recvpriv;

	if (precvframe == NULL) {
		return 0;
	}

	if (precvframe->u.hdr.pkt) {
		dev_kfree_skb(precvframe->u.hdr.pkt);
		precvframe->u.hdr.pkt = NULL;
	}

	if (precvframe->u.hdr.attrib.b_ap_bcmc_frame) { /*will free when softap off*/
		return 0;
	}

	spin_lock_bh(&(precvpriv->free_recv_lock));
	list_del_init(&(precvframe->u.hdr.list));
	precvframe->u.hdr.len = 0;
	list_add_tail(&(precvframe->u.hdr.list), &precvpriv->free_recv_queue);

	precvpriv->free_recvframe_cnt++;

	spin_unlock_bh(&(precvpriv->free_recv_lock));

	return 0;
}

u8 *whc_host_recv_recvframe_put(union recv_frame *precvframe, signed int sz)
{
	/* used for append sz bytes from ptr to rx_tail, update rx_tail and return the updated rx_tail to the caller
	 * after putting, rx_tail must be still larger than rx_end.
	*/
	if (precvframe == NULL) {
		return NULL;
	}

	precvframe->u.hdr.rx_tail += sz;

	if (precvframe->u.hdr.rx_tail > precvframe->u.hdr.rx_end) {
		precvframe->u.hdr.rx_tail -= sz;
		return NULL;
	}

	precvframe->u.hdr.len += sz;

	return precvframe->u.hdr.rx_tail;
}

u8 *whc_host_recv_recvframe_pull(union recv_frame *precvframe, signed int sz)
{
	// rx_data += sz; move rx_data sz bytes  hereafter
	//used for extract sz bytes from rx_data, update rx_data and return the updated rx_data to the caller
	if (precvframe == NULL) {
		return NULL;
	}

	precvframe->u.hdr.rx_data += sz;

	if (precvframe->u.hdr.rx_data > precvframe->u.hdr.rx_tail) {
		precvframe->u.hdr.rx_data -= sz;
		return NULL;
	}

	precvframe->u.hdr.len -= sz;

	return precvframe->u.hdr.rx_data;
}

u8 *whc_host_recv_recvframe_pull_tail(union recv_frame *precvframe, signed int sz)
{
	/* used for append sz bytes from ptr to rx_tail, update rx_tail and return the updated rx_tail to the caller
	 * after putting, rx_tail must be still larger than rx_end.
	*/

	if (precvframe == NULL) {
		return NULL;
	}

	precvframe->u.hdr.rx_tail -= sz;
	if (precvframe->u.hdr.rx_tail < precvframe->u.hdr.rx_data) {
		precvframe->u.hdr.rx_tail += sz;
		return NULL;
	}

	precvframe->u.hdr.len -= sz;
	return precvframe->u.hdr.rx_tail;
}

void whc_host_recv_count_rx_stats(struct whch_rx_stats *pstats, union recv_frame *prframe, struct sta_mlme_priv *psta_mlmepriv)
{
	struct whch_mlme_priv	*pmlmepriv = &global_idev.whchpriv.mlmepriv;
	struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;
	u8 is_ra_bmc = IS_MCAST(pattrib->ra);
	int	sz;

	sz = prframe->u.hdr.len;
	pstats->rx_bytes_in2s += sz;

	pmlmepriv->NumRxOkInPeriod++;
	if (!IS_MCAST(pattrib->dst)) {
		pmlmepriv->NumRxUnicastOkInPeriod++;
	}

	if (psta_mlmepriv) {
		psta_mlmepriv->stainfo_rx_data_pkts_in2s++;
		if (!is_ra_bmc) {
			psta_mlmepriv->stainfo_rx_byte_uni_in2s += sz;
		}
	}

	if (!is_ra_bmc) {
		pstats->rx_byte_uni_in2s += sz;
	}
}

struct sta_info *whc_host_recv_sta2sta_data_frame(u8 iface_type, union recv_frame *precv_frame)
{
	struct sta_info *psta = NULL;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 *ptr = precv_frame->u.hdr.rx_data;
	int ret = 0;
	u8 *sta_addr = NULL;
	signed int bmcast = IS_MCAST(pattrib->dst);

	if (iface_type == WHC_STA_PORT) {
		// For Station mode, sa and bssid should always be BSSID, and DA is my mac-address
		if (memcmp(pattrib->bssid, pattrib->src, ETH_ALEN) != 0) {
			ret = -1;
			goto exit;
		}
		sta_addr = pattrib->bssid;
	} else if (iface_type == WHC_AP_PORT) {
		if (bmcast) {
			// For AP mode, if DA == MCAST, then BSSID should be also MCAST
			if (!IS_MCAST(pattrib->bssid)) {
				ret = -1;
				goto exit;
			}
		} else { // not mc-frame
			// For AP mode, if DA is non-MCAST, then it must be BSSID, and bssid == BSSID
			if (memcmp(pattrib->bssid, pattrib->dst, ETH_ALEN) != 0) {
				ret = -1;
				goto exit;
			}
			sta_addr = pattrib->src;
		}

	} else {
		ret = -1;
		goto exit;
	}

	if (bmcast) {
		psta = whc_host_sta_get_stainfo(iface_type, bc_addr);
	} else {
		psta = whc_host_sta_get_stainfo(iface_type, sta_addr);
	}

	if (psta == NULL) {
		ret = -1;
		goto exit;
	}

exit:
	return (ret == -1) ? NULL : psta;
}

struct sta_info *whc_host_recv_ap2sta_data_frame(u8 iface_type, union recv_frame *precv_frame)
{
	struct mlme_priv_t *mlme_priv = &global_idev.mlme_priv;
	struct sta_info *psta = NULL;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 *ptr = precv_frame->u.hdr.rx_data;
	int ret = 0;
	u8 *mybssid  = global_idev.bssid;
	u8 *myhwaddr = global_idev.pndev[iface_type]->dev_addr;
	u8 subtype = 0;
	signed int bmcast = IS_MCAST(pattrib->dst);

	if (iface_type != WHC_STA_PORT) {
		ret = -1;
		goto exit;
	}

	if (whc_host_check_sta_associated_to_ap() || (mlme_priv->b_in_linking == 1)) {//check underlinking or already associated
		subtype = GetFrameSubType(ptr);
		// if NULL-frame, drop packet
		if (subtype == RTW_DATA_NULL) {
			ret = -1;
			goto exit;
		}

		//drop QoS-SubType Data, including QoS NULL, excluding QoS-Data
		if ((subtype & RTW_QOS_DATA_TYPE) == RTW_QOS_DATA_TYPE) {
			if (subtype & (BIT(4) | BIT(5) | BIT(6))) {
				ret = -1;
				goto exit;
			}
		}

		// filter packets that SA is myself or multicast or broadcast
		if (!memcmp(myhwaddr, pattrib->src, ETH_ALEN)) {
			ret = -1;
			goto exit;
		}

		// da should be for me
		if ((memcmp(myhwaddr, pattrib->dst, ETH_ALEN) != 0) && (!bmcast)) {
			ret = -1;
			goto exit;
		}

		// check BSSID
		if (!memcmp(pattrib->bssid, "\x0\x0\x0\x0\x0\x0", ETH_ALEN) ||
			!memcmp(mybssid, "\x0\x0\x0\x0\x0\x0", ETH_ALEN) ||
			(memcmp(pattrib->bssid, mybssid, ETH_ALEN) != 0)) {

			ret = -1;
			goto exit;
		}

		if (bmcast) {
			psta = whc_host_sta_get_stainfo(iface_type, bc_addr);
		} else {
			psta = whc_host_sta_get_stainfo(iface_type, pattrib->bssid);
		}

		if (psta == NULL) {
			ret = -1;
		}
	}

exit:
	return (ret == -1) ? NULL : psta;

}

struct sta_info *whc_host_recv_sta2ap_data_frame(u8 iface_type, union recv_frame *precv_frame)
{
	struct sta_info *psta = NULL;
	struct whch_rx_stats *pstats = &global_idev.whchpriv.rx_stats[iface_type];
	u8 *ptr = precv_frame->u.hdr.rx_data;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	unsigned char *mybssid  = global_idev.pndev[iface_type]->dev_addr;
	int ret = 0;
	unsigned char pwrbit;

	if (iface_type == WHC_AP_PORT) {
		//For AP mode, RA=BSSID, TX=STA(SRC_ADDR), A3=DST_ADDR
		if (memcmp(pattrib->bssid, mybssid, ETH_ALEN) != 0) {
			ret = -1;
			goto exit;
		}
		psta = whc_host_sta_get_stainfo(iface_type, pattrib->src);
#if 0   /* TODO_softap */
		if (psta == NULL) {
			/* TODO_softap */
			rtw_issue_deauth(iface_type, pattrib->src, RTW_DISCONN_RSN_80211_CLASS3_FRAME_FROM_NONASSOC_STA);
			ret = -1;
			goto exit;
		}

		// process_pwrbit_data
		pwrbit = GetPwrMgt(ptr);
		if (pwrbit) {
			if ((g_apmlmepriv.sta_dz_bitmap & BIT(psta_mlmepriv->stainfo_aid)) == 0) { /*sta not in ps mode*/
				rtw_ap_stop_sta_xmit(psta_mlmepriv, &psta->sta_xmitpriv);
			}
		} else {
			if (g_apmlmepriv.sta_dz_bitmap & BIT(psta_mlmepriv->stainfo_aid)) {/*sta in ps mode*/
				rtw_ap_wakeup_sta_to_xmit(wifi_rom_singlethread_wakeup_sema, psta_mlmepriv, &psta->sta_xmitpriv);
			}
		}

		// if NULL-frame, drop packet
		if ((GetFrameSubType(ptr)) == RTW_DATA_NULL) {
			//temporily count it here
			whc_host_recv_count_rx_stats(pstats, precv_frame, psta_mlmepriv);
			ret = -1;
			goto exit;
		}

		//drop QoS-SubType Data, including QoS NULL, excluding QoS-Data
		if ((GetFrameSubType(ptr) & RTW_QOS_DATA_TYPE) == RTW_QOS_DATA_TYPE) {

			if (GetFrameSubType(ptr) == RTW_QOS_DATA_NULL) {
				//temporily count it here
				whc_host_recv_count_rx_stats(pstats, precv_frame, psta_mlmepriv);
				ret = -1;
				goto exit;
			}
		}
#endif
	}

exit:
	return (ret == -1) ? NULL : psta;
}

int whc_host_recv_pn_check(u8 iface_type, union recv_frame *precv_frame)
{
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct sta_info *sta = precv_frame->u.hdr.psta;
	struct whch_security_priv *psecuritypriv = &global_idev.whchpriv.securitypriv[iface_type];
	int tid = precv_frame->u.hdr.attrib.priority;
	u8 *ptr = precv_frame->u.hdr.rx_data;
	u64 tmp_iv_hdr = 0;
	u64 curr_pn = 0, pkt_pn = 0;
	u8 key_id;

	if (iface_type == WHC_STA_PORT) {
		tmp_iv_hdr = le64_to_cpu(*(u64 *)(ptr + pattrib->hdrlen));
		key_id = CCMPH_2_KEYID(tmp_iv_hdr);
		pkt_pn = CCMPH_2_PN(tmp_iv_hdr);

		curr_pn = le64_to_cpu(*(u64 *)psecuritypriv->iv_seq[key_id]);
		curr_pn &= 0x0000ffffffffffff;

		if (!VALID_PN_CHK(pkt_pn, curr_pn)) {
			return -1;
		}

		*(u64 *)psecuritypriv->iv_seq[key_id] = cpu_to_le64(pkt_pn);
	}

	return 0;
}

int whc_host_recv_validate_data_frame(u8 iface_type, union recv_frame *precv_frame)
{
	struct whch_security_priv *psecuritypriv = &global_idev.whchpriv.securitypriv[iface_type];
	struct sta_info *psta = NULL;
	struct sta_mlme_priv *psta_mlmepriv = NULL;
	struct rx_pkt_attrib	*pattrib = &precv_frame->u.hdr.attrib;
	struct whch_rx_stats *pstats = &global_idev.whchpriv.rx_stats[iface_type];
	u8	*psa, *pda, *pbssid;
	u8	*ptr = precv_frame->u.hdr.rx_data;
	int	ret = 0;
	u16 seq_ctrl;
	signed int tid;
	u8 is_nan_data = false;

#ifdef CONFIG_NAN
	if (rtw_is_nan_frame) {
		is_nan_data = rtw_is_nan_frame(precv_frame);
	}
#endif
	pda = get_da(ptr);
	psa = get_sa(ptr);
	pbssid = get_hdr_bssid(ptr);
	if (pbssid == NULL) {
		ret = -1;
		goto exit;
	}

	memcpy(pattrib->dst, pda, ETH_ALEN);
	memcpy(pattrib->src, psa, ETH_ALEN);
	memcpy(pattrib->bssid, pbssid, ETH_ALEN);

	switch (pattrib->to_fr_ds) {
	case 0:
		memcpy(pattrib->ra, pda, ETH_ALEN);
		memcpy(pattrib->ta, psa, ETH_ALEN);
		psta = whc_host_recv_sta2sta_data_frame(iface_type, precv_frame);
		break;
	case 1:
		memcpy(pattrib->ra, pda, ETH_ALEN);
		memcpy(pattrib->ta, pbssid, ETH_ALEN);
		psta = whc_host_recv_ap2sta_data_frame(iface_type, precv_frame);
		break;
	case 2:
		memcpy(pattrib->ra, pbssid, ETH_ALEN);
		memcpy(pattrib->ta, psa, ETH_ALEN);
		psta = whc_host_recv_sta2ap_data_frame(iface_type, precv_frame);
		break;
	case 3:
		memcpy(pattrib->ra, GetAddr1Ptr(ptr), ETH_ALEN);
		memcpy(pattrib->ta, GetAddr2Ptr(ptr), ETH_ALEN);
		ret = -1;
		break;
	default:
		ret = -1;
		break;
	}

#ifdef CONFIG_NAN
	ret = (is_nan_data) ? (0) : (ret);
	if (is_nan_data == true) {
		psta = whc_host_sta_get_stainfo(iface_type, pattrib->src);
	}
#endif

	if (ret == -1) {
		goto exit;
	}

	if (psta) {
		psta_mlmepriv = &psta->sta_mlmepriv;
	} else {
		ret = -1;
		goto exit;
	}

	precv_frame->u.hdr.psta = psta;
	pattrib->b_amsdu = 0;
	//pattrib->ack_policy = 0;
	//parsing QC field
	if (pattrib->b_qos == 1) {
		pattrib->priority = GetPriority((ptr + 24));
		//pattrib->ack_policy = GetAckpolicy((ptr + 24));
		pattrib->b_amsdu = GetAMsdu((ptr + 24));
		pattrib->hdrlen = pattrib->to_fr_ds == 3 ? 32 : 26;
		if (pattrib->priority != 0 && pattrib->priority != 3) {
			pstats->b_is_any_non_be_pkts = true;
		}
	} else {
		pattrib->priority = 0;
		pattrib->hdrlen = pattrib->to_fr_ds == 3 ? 30 : 24;
	}

	if (pattrib->b_order) { //HT-CTRL 11n
		pattrib->hdrlen += 4;
	}

	/* decache, drop duplicate recv packets */
	tid = precv_frame->u.hdr.attrib.priority;
	seq_ctrl = ((precv_frame->u.hdr.attrib.seq_num & 0xffff) << 4) |
			   (precv_frame->u.hdr.attrib.frag_num & 0xf);
	if (tid > 15) {
		ret = -1;
		goto exit;
	}
	if (seq_ctrl == psta->sta_recvpriv.tid_rxseq[tid]) {
		if (pattrib->b_amsdu == 0) {
			/* Do not drop re-build A-MSDU pkts. */
			ret = -1;
			goto exit;
		}
	}
	psta->sta_recvpriv.tid_rxseq[tid] = seq_ctrl;

	if (pattrib->b_privacy) {
		GET_ENCRY_ALGO(psecuritypriv, &psta->sta_security, pattrib->encrypt, IS_MCAST(pattrib->ra));
#ifdef CONFIG_NAN
		if (is_nan_data == true && psta_mlmepriv) {
			pattrib->encrypt = psta->sta_security.dot11_security_privacy;
		}
#endif
		SET_ICE_IV_LEN(pattrib->iv_len, pattrib->icv_len, pattrib->encrypt);
	} else {
		pattrib->encrypt = 0;
		pattrib->iv_len = pattrib->icv_len = 0;
	}

	if ((pattrib->encrypt == _AES_ || pattrib->encrypt == _GCMP_256_) && IS_MCAST(pattrib->ra)) {
		if (whc_host_recv_pn_check(iface_type, precv_frame) == -1) {
			dev_warn(global_idev.pwhc_dev, "broadcast pn check fail!\n");
			ret = -1;
			goto exit;
		}
	}
exit:
	return ret;
}

int whc_host_recv_validate_frame(u8 iface_type, union recv_frame **pprecv_frame)
{
	union recv_frame	*precv_frame = *pprecv_frame;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	u8 type, subtype;
	int ret = 0;
	u8 *ptr = precv_frame->u.hdr.rx_data;
	u8  ver = (unsigned char)(*ptr) & 0x3;
	//add version chk
	if (ver != 0) {
		ret = -1;
		goto exit;
	}

	type =  GetFrameType(ptr);
	subtype = GetFrameSubType(ptr); //bit(7)~bit(2)

	pattrib->to_fr_ds = get_tofr_ds(ptr);
	pattrib->frag_num = GetFragNum(ptr);
	pattrib->seq_num = GetSequence(ptr);
	pattrib->b_mfrag = GetMFrag(ptr);
	pattrib->b_mdata = GetMData(ptr);
	pattrib->b_privacy = GetPrivacy(ptr);
	pattrib->b_order = GetOrder(ptr);

	switch (type) {
	case RTW_DATA_TYPE: //data
		pattrib->b_qos = (subtype & BIT(7)) ? 1 : 0;
		ret = whc_host_recv_validate_data_frame(iface_type, *pprecv_frame);
		break;
	default:
		ret = -1;
		break;
	}

exit:
	return ret;
}

int whc_host_recv_func_prehandle(u8 iface_type, union recv_frame *rframe)
{
	int ret = 0;

	ret = whc_host_recv_validate_frame(iface_type, &rframe);
	if (ret != 0 && rframe) {
		whc_host_recv_free_frame(rframe);//free this recv_frame
		goto exit;
	}

exit:
	return ret;
}

void whc_host_defrag_ctrl_init(struct recv_defrag_ctrl	*pdefrag_ctrl)
{
	if (pdefrag_ctrl->pfragbuf) {
		memset(pdefrag_ctrl->pfragbuf, 0, MAX_SKB_BUF_SIZE);
	} else {
		/* Total fragments size is expected to not exceed a skb size */
		pdefrag_ctrl->pfragbuf = kmalloc(MAX_SKB_BUF_SIZE, GFP_KERNEL);
		if (pdefrag_ctrl->pfragbuf == NULL) {
			return;
		}
	}
	pdefrag_ctrl->latest_frag_num = 0;
	pdefrag_ctrl->len = 0;
	mod_timer(&pdefrag_ctrl->defrag_timer, jiffies + msecs_to_jiffies(DEFRAG_TO));
}

void whc_host_defrag_ctrl_deinit(struct recv_defrag_ctrl	*pdefrag_ctrl)
{
	if (pdefrag_ctrl->pfragbuf) {
		kfree(pdefrag_ctrl->pfragbuf);
		pdefrag_ctrl->pfragbuf = NULL;
	}
	pdefrag_ctrl->latest_frag_num = 0;
	pdefrag_ctrl->len = 0;
	del_timer(&pdefrag_ctrl->defrag_timer);
}

void whc_host_defrag_timer_handler(struct timer_list *t)
{
	struct recv_defrag_ctrl *pdefrag_ctrl = from_timer(pdefrag_ctrl, t, defrag_timer);

	whc_host_defrag_ctrl_deinit(pdefrag_ctrl);
}

int whc_host_recv_check_indicate_seq(u16 *indicate_seq, u16 wsize, u16 wend, u16 seq_num)
{
	// Rx Reorder initialize condition.
	if (*indicate_seq == 0xFFFF) {
		*indicate_seq = seq_num;
	}

	// Drop out the packet which SeqNum is smaller than WinStart
	if (SN_LESS(seq_num, *indicate_seq)) {
		return false;
	}

	/*
	*  Sliding window manipulation. Conditions includes:
	*  1. Incoming SeqNum is equal to WinStart =>Window shift 1
	*  2. Incoming SeqNum is larger than the WinEnd => Window shift N
	*/
	if (SN_EQUAL(seq_num, *indicate_seq)) {
		*indicate_seq = (*indicate_seq + 1) & 0xFFF;

	} else if (SN_LESS(wend, seq_num)) {
		// boundary situation, when seq_num cross 0xFFF
		if (seq_num >= (wsize - 1)) {
			*indicate_seq = seq_num + 1 - wsize;
		} else {
			*indicate_seq = 0xFFF - (wsize - (seq_num + 1)) + 1;
		}

	}
	return true;
}

int whc_host_recv_reorder(void *preorder_per_tid)
{
	struct recv_reorder_ctrl *preorder_ctrl = (struct recv_reorder_ctrl *)preorder_per_tid;
	struct reorder_node *new_node = preorder_ctrl->new_node;
	struct reorder_node *cur_node = NULL;
	struct list_head	*phead, *plist;
	u16	wend;

	/*s1. special pkt, indicate directly, but eapol won't get here, dhcp seems no need.*/

	/*s2. check if winstart_b(indicate_seq) needs to been updated*/
	spin_lock_bh(&preorder_ctrl->pending_recvframe_lock);
	wend = (preorder_ctrl->indicate_seq + preorder_ctrl->wsize_b - 1) & 0xFFF; //% 4096;
	if (!whc_host_recv_check_indicate_seq(&preorder_ctrl->indicate_seq, preorder_ctrl->wsize_b, wend, new_node->seq_num)) {
		goto _err_exit;
	}

	/*s3. Enqueue*/
	phead = &preorder_ctrl->pending_recvframe_queue;
	plist = phead->next;
	while (plist != phead) {
		cur_node = list_entry(plist, struct reorder_node, list);
		if (SN_LESS(cur_node->seq_num, new_node->seq_num)) {
			plist = plist->next;
		} else if (SN_EQUAL(cur_node->seq_num, new_node->seq_num)) {
			goto _err_exit;
		} else {
			break;
		}
	}
	list_del_init(&(new_node->list));
	list_add_tail(&(new_node->list), plist);
	preorder_ctrl->new_node = NULL;

	/*s4. Dequeue*/
	if (whc_host_recv_reorder_dequeue(preorder_ctrl, false) == true) {
		/*still has pkts in queue*/
		if (!preorder_ctrl->bReorderWaiting) {
			preorder_ctrl->bReorderWaiting = true;
			mod_timer(&preorder_ctrl->reordering_ctrl_timer, jiffies + msecs_to_jiffies(REORDER_WAIT_TIME));
		}
	} else {
		/*queue empty*/
		if (preorder_ctrl->bReorderWaiting) {
			preorder_ctrl->bReorderWaiting = false;
			del_timer(&preorder_ctrl->reordering_ctrl_timer);
		}
	}
	spin_unlock_bh(&preorder_ctrl->pending_recvframe_lock);

	return 0;

_err_exit:
	/*enqueue fail*/
	dev_kfree_skb_any(preorder_ctrl->new_node->skb);
	INIT_LIST_HEAD(&(preorder_ctrl->new_node->list));
	list_add_tail(&(preorder_ctrl->new_node->list), &preorder_ctrl->node_free_list);
	preorder_ctrl->new_node = NULL;
	spin_unlock_bh(&preorder_ctrl->pending_recvframe_lock);
	return -1;
}

int whc_host_recv_reorder_dequeue(struct recv_reorder_ctrl *preorder_ctrl, int bforced)
{
	struct list_head	*phead, *plist;
	struct reorder_node *cur_node;
	int	bPktInBuf = false;

	phead = &preorder_ctrl->pending_recvframe_queue;
	plist = phead->next;

	if (bforced == true) {
		if (list_empty(phead)) {
			return bPktInBuf;
		}
		cur_node = list_entry(plist, struct reorder_node, list);
		preorder_ctrl->indicate_seq = cur_node->seq_num;
	}

	while (!list_empty(phead)) {
		cur_node = list_entry(plist, struct reorder_node, list);
		if (!SN_LESS(preorder_ctrl->indicate_seq, cur_node->seq_num)) {
			plist = plist->next;
			list_del_init(&(cur_node->list));
			if (SN_EQUAL(preorder_ctrl->indicate_seq, cur_node->seq_num)) {
				preorder_ctrl->indicate_seq = (preorder_ctrl->indicate_seq + 1) & 0xFFF;
			}
			//indicate this recv_frame
			whc_host_if_netif_rx(cur_node->skb, NULL);
			cur_node->skb = NULL;
			INIT_LIST_HEAD(&(cur_node->list));
			list_add_tail(&cur_node->list, &preorder_ctrl->node_free_list);
			//Update local variables.
			bPktInBuf = false;
		} else {
			bPktInBuf = true;
			break;
		}
	}
	return bPktInBuf;
}

void whc_host_recv_reorder_timer_hdl(struct timer_list *t)
{
	int ret;
	struct recv_reorder_ctrl *preorder_ctrl = from_timer(preorder_ctrl, t, reordering_ctrl_timer);
	struct whch_mlme_info *pmlmeinfo = &global_idev.whchpriv.mlmeinfo[preorder_ctrl->iface_type];

	spin_lock_bh(&preorder_ctrl->pending_recvframe_lock);
	ret = whc_host_recv_reorder_dequeue(preorder_ctrl, true);

	if ((preorder_ctrl->ap_compatibilty_enabled & BIT(4))
		&& (preorder_ctrl->bReorderWaiting & BIT(7))) {
		preorder_ctrl->bReorderWaiting &= ~BIT(7);
		preorder_ctrl->indicate_seq = 0xffff;
		preorder_ctrl->enable = true;
	}

	preorder_ctrl->bReorderWaiting = false;
	if (ret == true) {
		preorder_ctrl->bReorderWaiting = true;
		mod_timer(&preorder_ctrl->reordering_ctrl_timer, jiffies + msecs_to_jiffies(REORDER_WAIT_TIME));
	}

	spin_unlock_bh(&preorder_ctrl->pending_recvframe_lock);
}

void whc_host_recv_reorder_init(struct recv_reorder_ctrl *preorder_ctrl)
{
	memset(preorder_ctrl, 0, sizeof(*preorder_ctrl));
	preorder_ctrl->enable = false;
	preorder_ctrl->indicate_seq = 0xffff;
	preorder_ctrl->wend_b = 0xffff;
	preorder_ctrl->wsize_b = 64;
	INIT_LIST_HEAD(&preorder_ctrl->pending_recvframe_queue);
	spin_lock_init(&preorder_ctrl->pending_recvframe_lock);
	timer_setup(&preorder_ctrl->reordering_ctrl_timer, whc_host_recv_reorder_timer_hdl, 0);
	INIT_LIST_HEAD(&preorder_ctrl->node_free_list);
	preorder_ctrl->node_pool = NULL;
	preorder_ctrl->pool_size = 0;
}

void whc_host_recv_reorder_setup(struct recv_reorder_ctrl *preorder_ctrl, u8 ap_compatibilty, u8 iface_type)
{
	u16 pool_sz = preorder_ctrl->wsize_b;
	int i;

	preorder_ctrl->iface_type = iface_type;
	preorder_ctrl->enable = false;
	preorder_ctrl->indicate_seq = 0xffff;
	preorder_ctrl->wend_b = 0xffff;
	preorder_ctrl->wsize_b = 64;
	preorder_ctrl->ap_compatibilty_enabled = ap_compatibilty;

	if (preorder_ctrl->node_pool == NULL || preorder_ctrl->pool_size < pool_sz) {
		if (preorder_ctrl->node_pool) {
			kfree(preorder_ctrl->node_pool);
		}
		preorder_ctrl->node_pool = (struct reorder_node *)kmalloc_array(pool_sz, sizeof(struct reorder_node), GFP_KERNEL);
		if (preorder_ctrl->node_pool == NULL) {
			dev_err(global_idev.pwhc_dev, "[whc] reorder node_pool alloc fail (sz=%d)\n", pool_sz);
			preorder_ctrl->pool_size = 0;
			return;
		}
		preorder_ctrl->pool_size = pool_sz;
	}

	INIT_LIST_HEAD(&preorder_ctrl->node_free_list);
	for (i = 0; i < preorder_ctrl->pool_size; i++) {
		INIT_LIST_HEAD(&preorder_ctrl->node_pool[i].list);
		list_add_tail(&preorder_ctrl->node_pool[i].list, &preorder_ctrl->node_free_list);
	}
}

void whc_host_recv_reorder_free(struct recv_reorder_ctrl *preorder_ctrl)
{
	struct reorder_node *cur_node;
	struct list_head	*phead, *plist;

	if (preorder_ctrl == NULL) {
		return;
	}

	del_timer_sync(&preorder_ctrl->reordering_ctrl_timer);

	spin_lock_bh(&preorder_ctrl->pending_recvframe_lock);
	phead = &preorder_ctrl->pending_recvframe_queue;
	plist = phead->next;

	while (!list_empty(phead)) {
		cur_node = list_entry(plist, struct reorder_node, list);
		plist = plist->next;
		list_del_init(&(cur_node->list));
		if (cur_node->skb) {
			dev_kfree_skb_any(cur_node->skb);
		}
	}

	if (preorder_ctrl->new_node) {
		if (preorder_ctrl->new_node->skb) {
			dev_kfree_skb_any(preorder_ctrl->new_node->skb);
		}
		preorder_ctrl->new_node = NULL;
	}

	preorder_ctrl->bReorderWaiting = 0;
	preorder_ctrl->enable = false;
	spin_unlock_bh(&preorder_ctrl->pending_recvframe_lock);

	if (preorder_ctrl->node_pool) {
		kfree(preorder_ctrl->node_pool);
		preorder_ctrl->node_pool = NULL;
		preorder_ctrl->pool_size = 0;
	}
}

int whc_host_recv_frame_chkmic(u8 iface_type,  struct whch_security_priv *psecuritypriv, union recv_frame *precvframe)
{
	int	i, res = 0;
	u32	datalen;
	u8	miccode[8];
	u8	bmic_err = false;
	u8	*pframe, *payload, *pframemic;
	u8	*mickey;
	struct sta_info *psta = NULL;
	struct sta_security_priv *psta_security = NULL;
	struct	rx_pkt_attrib	*prxattrib = &precvframe->u.hdr.attrib;

	psta = precvframe->u.hdr.psta;
	psta_security = psta ? &psta->sta_security : NULL;

	if (prxattrib->encrypt == _TKIP_) {
		//calculate mic code
		if (psta_security != NULL) {
			if (IS_MCAST(prxattrib->ra)) {
				mickey = &psecuritypriv->dot11_tkip_grpmickey_rx[0].skey[0];
				if (psecuritypriv->b_installGrpkey == false) {
					res = -1;
					dev_warn(global_idev.pwhc_dev, "chkmic: no group key install!\n");
					goto exit;
				}
			} else {
				mickey = &psta_security->dot11tkiprxmickey.skey[0];
			}

			datalen = precvframe->u.hdr.len - prxattrib->hdrlen - prxattrib->iv_len - prxattrib->icv_len - 8; //icv_len included the mic code
			pframe = precvframe->u.hdr.rx_data;
			payload = pframe + prxattrib->hdrlen + prxattrib->iv_len;
			whc_host_seccalctkipmic(mickey, pframe, payload, datalen, &miccode[0], (unsigned char)prxattrib->priority); //care the length of the data
			pframemic = payload + datalen;
			bmic_err = false;

			for (i = 0; i < 8; i++) {
				if (miccode[i] != *(pframemic + i)) {
					bmic_err = true;
				}
			}

			if (bmic_err == true) {
				dev_warn(global_idev.pwhc_dev, " mic err, %d\n", prxattrib->b_decrypted);
				res = -1;
			}

		} else {
			dev_warn(global_idev.pwhc_dev, "chkmic: NULL stainfo!\n");
		}

		whc_host_recv_recvframe_pull_tail(precvframe, 8);
	}

exit:
	return res;
}

int whc_host_recv_frame_check_frag_pn(struct recv_defrag_ctrl *pdefrag_ctrl, union recv_frame *precvframe, u8 fragcase)
{
	struct rx_pkt_attrib	*prxattrib = &precvframe->u.hdr.attrib;
	u8 *pframe;
	u8 idx;
	u64 tmp_iv_hdr, cur_pn;

	if (prxattrib->encrypt == _AES_ || prxattrib->encrypt == _GCMP_256_) {
		/*only check be: https://jira.realtek.com/browse/RSWLANDIOT-11204*/
		if (prxattrib->priority == 0 || prxattrib->priority == 3) {
			pframe = precvframe->u.hdr.rx_data;
			tmp_iv_hdr = le64_to_cpu(*(u64 *)(pframe + prxattrib->hdrlen));
			/* get the first frame's PN. */
			cur_pn = CCMPH_2_PN(tmp_iv_hdr);
			if (fragcase == 0) {	/* get the first frame's PN. */
				pdefrag_ctrl->latest_pn = cur_pn;
			} else if (fragcase == 1) {	/* 1~ n-1 frag */
				if (cur_pn != pdefrag_ctrl->latest_pn + 1) {
					dev_dbg(global_idev.pwhc_dev, "%s non-consective PN! old:%llu, new:%llu\n", __func__, pdefrag_ctrl->latest_pn, cur_pn);
					/* PN must be consecutive */
					return -1;
				} else {
					pdefrag_ctrl->latest_pn = cur_pn;
				}
			}
		}
	}
	return 0;
}

union recv_frame *whc_host_recv_frame_check_defrag(u8 iface_type, struct whch_security_priv *psecuritypriv, union recv_frame *precv_frame)
{
	struct recv_frame_hdr	*pfhdr = &precv_frame->u.hdr;
	struct sta_info *psta = NULL;
	struct sta_recv_priv *psta_recvpriv = NULL;
	struct recv_defrag_ctrl	*pdefrag_ctrl;
	struct sk_buff	 *skb = pfhdr->pkt;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 ismfrag = pfhdr->attrib.b_mfrag;
	u8 fragnum = pfhdr->attrib.frag_num;
	u32 datalen, curr_rxlen, buffersize = 8192;
	u8 *payload;

	if ((ismfrag == 0) && (fragnum == 0)) {
		return precv_frame;     //isn't a fragment frame
	}

	psta = whc_host_sta_get_stainfo(iface_type, GetAddr2Ptr(pfhdr->rx_data));
	if (psta == NULL) {
		goto exit;
	}
	psta_recvpriv = &psta->sta_recvpriv;
	pdefrag_ctrl = &psta_recvpriv->defrag_ctrl;

	if (ismfrag == 1) {  //0~(n-1) fragment frame
		if (fragnum == 0) {
			whc_host_defrag_ctrl_init(pdefrag_ctrl);
			whc_host_recv_frame_check_frag_pn(pdefrag_ctrl, precv_frame, 0);
		} else {
			if ((pdefrag_ctrl->pfragbuf == NULL) || (pdefrag_ctrl->latest_frag_num + 1 != fragnum)) {
				goto exit;
			}
			if (whc_host_recv_frame_check_frag_pn(pdefrag_ctrl, precv_frame, 1) == -1) {
				whc_host_defrag_ctrl_deinit(pdefrag_ctrl);
				goto exit;
			}
		}
		datalen = pfhdr->len - pfhdr->attrib.hdrlen - pfhdr->attrib.iv_len - pfhdr->attrib.icv_len;
		if (datalen > (buffersize - pdefrag_ctrl->len)) {
			/* The remaining buffer is not sufficient to store this fragment, drop all fragments */
			whc_host_defrag_ctrl_deinit(pdefrag_ctrl);
			goto exit;
		}
		payload = pfhdr->rx_data + pfhdr->attrib.hdrlen + pfhdr->attrib.iv_len;
		memcpy(pdefrag_ctrl->pfragbuf + pdefrag_ctrl->len, payload, datalen);
		pdefrag_ctrl->len += datalen;
		pdefrag_ctrl->latest_frag_num = fragnum;
		whc_host_recv_free_frame(precv_frame);
		return NULL;
	}

	if ((ismfrag == 0) && (fragnum != 0)) {   //the last fragment frame
		if ((pdefrag_ctrl->pfragbuf == NULL) || (pdefrag_ctrl->latest_frag_num + 1 != fragnum)) {
			goto exit;
		}
		curr_rxlen = pfhdr->len + (RXDESC_SIZE + pfhdr->attrib.drvinfo_sz + pfhdr->attrib.shift_sz);
		if ((curr_rxlen + pdefrag_ctrl->len) > buffersize) {
			/* Total fragments size is exceed a skb size, drop all fragments */
			whc_host_defrag_ctrl_deinit(pdefrag_ctrl);
			goto exit;
		}

		if (whc_host_recv_frame_check_frag_pn(pdefrag_ctrl, precv_frame, 1) == -1) {
			whc_host_defrag_ctrl_deinit(pdefrag_ctrl);
			goto exit;
		}

		/* Insert previous fragments before the data of the last fragment */
		datalen = pfhdr->len - pfhdr->attrib.hdrlen - pfhdr->attrib.iv_len;
		payload = pfhdr->rx_data + pfhdr->attrib.hdrlen + pfhdr->attrib.iv_len;
		memmove(payload + pdefrag_ctrl->len, payload, datalen);
		memcpy(payload, pdefrag_ctrl->pfragbuf, pdefrag_ctrl->len);
		skb->end += pdefrag_ctrl->len;
		pfhdr->rx_end = skb_end_pointer(skb);
		whc_host_recv_recvframe_put(precv_frame, pdefrag_ctrl->len);
		whc_host_defrag_ctrl_deinit(pdefrag_ctrl);
	}

	return precv_frame;

exit:
	dev_dbg(global_idev.pwhc_dev, "[whc]: defrag err, drop pkt\n");
	whc_host_recv_free_frame(precv_frame);
	return NULL;
}

union recv_frame *whc_host_recv_decryptor(u8 iface_type, struct whch_security_priv *psecuritypriv, union recv_frame *precv_frame)
{
	struct rx_pkt_attrib	*prxattrib = &precv_frame->u.hdr.attrib;
	union recv_frame	*return_packet = precv_frame;
	struct sta_info *psta = NULL;
	struct sta_security_priv *psta_security = NULL;
	union u_crc		crc = {0};
	int			res = 0;
	u8			*pframe, *prwskey = NULL;
	int			payload_len;
	u8 			decrypted = false;

	if (prxattrib->encrypt > 0) {
		u8 *iv = precv_frame->u.hdr.rx_data + prxattrib->hdrlen;
		prxattrib->key_index = (((iv[3]) >> 6) & 0x3);

		if (prxattrib->key_index > 4) {
			switch (prxattrib->encrypt) {
			case _WEP40_:
			case _WEP104_:
				prxattrib->key_index = psecuritypriv->dot11_wep_key_index;
				break;
			case _TKIP_:
			case _AES_:
			case _GCMP_256_:
			default:
				prxattrib->key_index = psecuritypriv->dot11_wpa_grpkey_index;
				break;
			}
		}
	}

	if ((prxattrib->encrypt > 0) && ((prxattrib->b_decrypted == 0))) {
		if (prxattrib->b_amsdu) {
			dev_err(global_idev.pwhc_dev, "[whc]: amsdu not support sw dec\n");
			res = -1;
			goto exit;
		}

		psecuritypriv->b_hw_decrypted = false;

		pframe = (unsigned char *)((union recv_frame *)precv_frame)->u.hdr.rx_data;
		payload_len = ((union recv_frame *)precv_frame)->u.hdr.len - prxattrib->hdrlen - prxattrib->iv_len;
		if ((prxattrib->encrypt == _TKIP_) || (prxattrib->encrypt == _AES_) || (prxattrib->encrypt == _GCMP_256_)) {
			psta = precv_frame->u.hdr.psta;
			psta_security = psta ? &psta->sta_security : NULL;
			if (psta_security == NULL) {
				res = -1;
				goto exit;
			}
			if (IS_MCAST(prxattrib->ra)) {
				if (psecuritypriv->b_installGrpkey == false) {
					res = -1;
					goto exit;
				}
				prwskey = psecuritypriv->dot11_wpa_grpkey[0].skey;
#ifdef CONFIG_NAN
				if (iface_type == WHC_NAN_PORT && rtw_is_nan_frame && rtw_is_nan_frame(precv_frame)) {
					/* for NAN, group rx key is stored in stainfo */
					if (psta_security->gtk_bmp) {
						prwskey = psta_security->gtk.skey;
					}
				}
#endif
			} else {
				prwskey = &psta_security->dot118021x_UncstKey.skey[0];
			}
		}

		switch (prxattrib->encrypt) {
		case _WEP40_:
		case _WEP104_:
			decrypted = wep_80211_decrypt(
							pframe,
							prxattrib->hdrlen,								//Wlan Header Length
							prxattrib->iv_len,								// IV length
							payload_len, // Payload length
							&psecuritypriv->dot11_wep_key[0].skey[0],	//WEP Key
							psecuritypriv->dot11_wep_keylen[0],		//WEP Key Length
							&crc	//CRC32 value filed
						);
			res = (decrypted == true) ? 0 : -1;
			if (res == -1) {
				dev_warn(global_idev.pwhc_dev, "[whc]: wep_decrypt:icv err\n");
			}
			break;

		case _TKIP_:
			decrypted = tkip_80211_decrypt(
							pframe, // start of 802.11 frame
							prxattrib->hdrlen, 	// WLAN header length
							prxattrib->iv_len, 	// IV length
							payload_len, // Payload length
							prwskey, 			// tkip key
							16, 				// tkip ley length
							&prxattrib->ta[0],	// transmit address for ken generation
							&crc				//CRC32 value filed
						);
			res = (decrypted == true) ? 0 : -1;
			break;
		case _AES_:
			decrypted = aes_80211_decrypt(
							pframe,  // Frame offset. Early mode and USB aggregation is not considered
							prxattrib->hdrlen,			// WLAN header length
							payload_len,	// payload length
							prwskey, 					// AES key
							get_frame_sub_type(pframe)	// Frame Type
						);
			res = (decrypted == true) ? 0 : -1;
			break;
		case _GCMP_256_:
			decrypted = gcmp_80211_decrypt(
							pframe,  // Frame offset. Early mode and USB aggregation is not considered
							prxattrib->hdrlen,			// WLAN header length
							((union recv_frame *)precv_frame)->u.hdr.len,	// frame length
							prwskey, 					// GCMP key
							(prxattrib->encrypt == _GCMP_256_) ? 32 : 16
						);
			res = (decrypted == true) ? 0 : -1;
			break;
		default:
			dev_err(global_idev.pwhc_dev, "SwDec: NOT SPT ALG %x\n", prxattrib->encrypt);
			break;
		}
	} else if (prxattrib->b_decrypted == 1
			   && prxattrib->encrypt > 0
			   && (psecuritypriv->b_usetkipkey == 1 || prxattrib->encrypt != _TKIP_)) {
		psecuritypriv->b_hw_decrypted = true;
	}

exit:
	if (res == -1) {
		whc_host_recv_free_frame(return_packet);
		return_packet = NULL;
	}
	return return_packet;
}

void whc_host_recv_wlanhdr_to_ethhdr(union recv_frame *precvframe)
{
	struct rx_pkt_attrib	*pattrib = &precvframe->u.hdr.attrib;
	struct ieee80211_snap_hdr *psnap;
	u16	eth_type, len;
	u8	bsnaphdr;
	u8	*psnap_type;
	u8	*ptr = precvframe->u.hdr.rx_data; // point to frame_ctrl field
	signed int rmv_len;

	if (pattrib->encrypt) {
		whc_host_recv_recvframe_pull_tail(precvframe, pattrib->icv_len);
	}

	psnap = (struct ieee80211_snap_hdr *)(ptr + pattrib->hdrlen + pattrib->iv_len);
	psnap_type = ptr + pattrib->hdrlen + pattrib->iv_len + sizeof(struct ieee80211_snap_hdr);
	/* convert hdr + possible LLC headers into Ethernet header */
	if ((!memcmp(psnap, (void *)rfc1042_header, sizeof(struct ieee80211_snap_hdr)) &&
		 (memcmp(psnap_type, (void *)SNAP_ETH_TYPE_IPX, 2) != 0) &&
		 (memcmp(psnap_type, (void *)SNAP_ETH_TYPE_APPLETALK_AARP, 2) != 0)) ||
		!memcmp(psnap, (void *)bridge_tunnel_header, sizeof(struct ieee80211_snap_hdr))) {
		/* remove RFC1042 or Bridge-Tunnel encapsulation and replace EtherType */
		bsnaphdr = true;
	} else {
		/* Leave Ethernet header part of hdr and full payload */
		bsnaphdr = false;
	}

	rmv_len = pattrib->hdrlen + pattrib->iv_len + (bsnaphdr ? sizeof(struct ieee80211_snap_hdr) : 0);
	len = precvframe->u.hdr.len - rmv_len;

	memcpy(&eth_type, ptr + rmv_len, 2);
	eth_type = htons((unsigned short)eth_type); //pattrib->ether_type
	pattrib->eth_type = eth_type;
#ifdef CONFIG_MP_INCLUDED
	if (rtw_halphy_shareinfo.phl_drv_mode == RTW_DRV_MODE_MP) {
		ptr += rmv_len;
		*ptr = 0x87;
		*(ptr + 1) = 0x12;

		eth_type = 0x8712;
		// append rx status for mp test packets
		ptr = whc_host_recv_recvframe_pull(precvframe, (rmv_len - sizeof(struct ethhdr) + 2) - 24);
		memcpy(ptr, precvframe->u.hdr.rx_head, 24);
		ptr += 24;
	} else
#endif
	{
		ptr = whc_host_recv_recvframe_pull(precvframe, (rmv_len - sizeof(struct ethhdr) + (bsnaphdr ? 2 : 0)));
	}

	memcpy(ptr, pattrib->dst, ETH_ALEN);

#ifdef CONFIG_NAN
	if (!memcmp(pattrib->dst, g_nanpriv.nmi.mac, ETH_ALEN)) {
		memcpy(ptr, rtw_get_adapter_mac_addr(precvframe->u.hdr.iface_type), ETH_ALEN);
	}
#endif

	memcpy(ptr + ETH_ALEN, pattrib->src, ETH_ALEN);

	if (!bsnaphdr) {
		len = htons(len);
		memcpy(ptr + 12, &len, 2);
	}
}

void whc_host_if_netif_rx(struct sk_buff *skb, void *preorder_ctrl)
{
	int idx = 0;

	if (preorder_ctrl != NULL) {
		whc_host_recv_reorder(preorder_ctrl);
		return;
	}

	if (skb == NULL) {
		dev_err(global_idev.pwhc_dev, "[whc] netifrx skb NULL\n");
		return;
	}

	for (idx = 0; idx < WHC_MAX_NET_PORT_NUM; idx++) {
		if (skb->dev == global_idev.pndev[idx]) {
			break;
		}
	}

	if (idx == WHC_MAX_NET_PORT_NUM) {
		dev_err(global_idev.pwhc_dev, "[whc] netifrx get idx fail\n");
		return;
	}

	whc_host_netif_rx(skb, idx);
}

int whc_host_recv_indicatepkt(u8 iface_type, union recv_frame *precv_frame)
{
	struct whch_security_priv *psecuritypriv = &global_idev.whchpriv.securitypriv[iface_type];
	struct sk_buff *skb;
	struct whch_mlme_info *pmlmeinfo = (&global_idev.whchpriv.mlmeinfo[iface_type]);
	struct rx_pkt_attrib *pattrib;
	struct sta_info *psta = NULL;
	struct sta_mlme_priv *psta_mlmepriv = NULL;
	struct recv_reorder_ctrl *preorder_ctrl = NULL;
	u16 ether_type, start_seq = 0;
	u8 dst_ip, ip_gate;
	int ip_valid = 1;

	if (precv_frame == NULL) {
		dev_warn(global_idev.pwhc_dev, "[whc]: NULL precv_frame!\n");
		goto _recv_indicatepkt_drop;
	}
	pattrib = &precv_frame->u.hdr.attrib;
	skb = precv_frame->u.hdr.pkt;
	if (skb == NULL) {
		dev_warn(global_idev.pwhc_dev, "[whc]: NULL skb!\n");
		goto _recv_indicatepkt_drop;
	}

	skb->data = precv_frame->u.hdr.rx_data;
	skb_set_tail_pointer(skb, precv_frame->u.hdr.len);
	skb->len = precv_frame->u.hdr.len;

#if 0 /* TODO_softap */
	if (iface_type == WHC_AP_PORT) {
		struct sk_buff *pskb2 = NULL;

		if (memcmp(pattrib->dst, rtw_get_adapter_mac_addr(iface_type), ETH_ALEN) != 0) {
			//Add station authenticated check to fix FragAttacks vulnerabilities CVE-2020-26139
			psta = whc_host_sta_get_stainfo(iface_type, pattrib->ta);
			if (psta) {
				if ((psecuritypriv->dot11_wpa_mode == Ndis802_11AuthModeWPAPSK) || (psecuritypriv->dot11_wpa_mode == Ndis802_11AuthModeWPA2PSK) ||
					(psecuritypriv->dot11_wpa_mode == Ndis802_11AuthModeWPA3PSK)) {
					//if station is not authenticated, then bypass forwarding
					if (psta->sta_security.b_pairwise_key_installed != true) {
						goto bypass_forwarding;
					}
				}
			}

			// not forwarding if only one sta or ap bypass forwarding enabled(RSWLANDIOT-14240)
			if ((wifi_user_config.ap_bypass_forwarding == 1) || (rtw_ap_get_assoc_sta_num() <= 1)) {
				// indicate directly
				goto bypass_forwarding;
			}
			if (IS_MCAST(pattrib->dst)) {//broadcast and multicast frames
				u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
				psta = whc_host_sta_get_stainfo(iface_type, bc_addr);
				ether_type = htons(*(unsigned short *)(skb->data + 12));
				//1208 iphone5 connection issue, need to check psta exist first
				if (psta) {
					//skb  rx->tx,reserve tx desc
					pskb2 = skb_copy(skb, GFP_KERNEL, SKB_WLAN_TX_EXTRA_LEN);
					if (pskb2 != NULL) {	//only pskb2 is not null, transmit pskb2 to rtw_xmit_entry
						pskb2->dev = global_idev.pndev[iface_type];
						rtw_xmit_entry(pskb2, pskb2->dev);//tx pskb2
					}
					//next , whether pskb2 is null or not null , skb will alwalys be transmitted to rltk_netif_rx.
				}
			} else {//unicast
				psta = whc_host_sta_get_stainfo(iface_type, pattrib->dst);
				if (psta) {
					//skb  rx->tx,reserve tx desc
					pskb2 = skb_copy(skb, GFP_KERNEL, SKB_WLAN_TX_EXTRA_LEN);
					dev_kfree_skb_any(skb);//free skb as it is not used anymore

					if (pskb2 != NULL) {	//only pskb2 is not null, transmit pskb2 to rtw_xmit_entry
						pskb2->dev = global_idev.pndev[iface_type];
						rtw_xmit_entry(pskb2, pskb2->dev);//pskb2 will be free by xmit flow
					}
					goto _recv_indicatepkt_end;
				}
			}
bypass_forwarding:
			;
		}
	}
#endif

	skb->dev = global_idev.pndev[iface_type];

	preorder_ctrl = NULL;
	/*reorder is needed if addba of this STA's TID has been setup */
	psta = whc_host_sta_get_stainfo(iface_type, pattrib->ta);
	if (pattrib->b_qos && psta && (!IS_MCAST(pattrib->ra)) && (!pattrib->b_amsdu)) {
		preorder_ctrl = &psta->sta_recvpriv.recvreorder_ctrl[pattrib->priority & 0x0F];
		if (preorder_ctrl->enable) { /*addba has setted*/
			if ((preorder_ctrl->indicate_seq == 0xffff) && (preorder_ctrl->iface_type == WHC_STA_PORT)) {
				start_seq = le16_to_cpu(pmlmeinfo->paddba_req.BA_starting_seqctrl) >> 4;
				if (((preorder_ctrl->ap_compatibilty_enabled & 0xF) < 10) &&
					(((4096 + pattrib->seq_num - start_seq) & 0xFFF) >= preorder_ctrl->wsize_b)) { /* https://jira.realtek.com/browse/RSWLANDIOT-11971 */
					preorder_ctrl->ap_compatibilty_enabled = (preorder_ctrl->ap_compatibilty_enabled & 0xF0) | ((preorder_ctrl->ap_compatibilty_enabled + 1) & 0xF);
					preorder_ctrl = NULL; /*wait reorder start*/
				} else {
					preorder_ctrl->ap_compatibilty_enabled &= 0xF0;
				}
			}

			if (preorder_ctrl) {
				if (list_empty(&preorder_ctrl->node_free_list)) {
					dev_err(global_idev.pwhc_dev, "[whc]: %s reorder node pool empty!\n", __func__);
					goto _recv_indicatepkt_drop;
				}
				preorder_ctrl->new_node = list_first_entry(&preorder_ctrl->node_free_list, struct reorder_node, list);
				list_del_init(&preorder_ctrl->new_node->list);
				if (preorder_ctrl->new_node == NULL) {
					dev_err(global_idev.pwhc_dev, "[whc]: %s get reorder_node fail!\n", __func__);
					goto _recv_indicatepkt_drop;
				}
				preorder_ctrl->new_node->skb = skb;
				preorder_ctrl->new_node->seq_num = pattrib->seq_num;
				INIT_LIST_HEAD(&(preorder_ctrl->new_node->list));
			}
		}
	}

	whc_host_if_netif_rx(skb, preorder_ctrl);

_recv_indicatepkt_end:

	precv_frame->u.hdr.pkt = NULL; // pointers to NULL before whc_host_recv_free_frame()

	whc_host_recv_free_frame(precv_frame);

	return 0;

_recv_indicatepkt_drop:
	//enqueue back to free_recv_queue
	if (precv_frame) {
		whc_host_recv_free_frame(precv_frame);
	}

	return -1;
}

static int whc_host_recv_amsdu_to_msdu(u8 iface_type, union recv_frame *prframe)
{
	struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;
	u8 *buf = prframe->u.hdr.rx_data + pattrib->hdrlen + pattrib->iv_len; /* sub-MSDU region start. */
	int payload_total = prframe->u.hdr.len - pattrib->hdrlen - pattrib->iv_len - pattrib->icv_len;
	int off = 0;
	int ret = 0;

	while ((off + ETH_HLEN) <= payload_total) {
		struct ieee80211_snap_hdr *psnap;
		u8 *psnap_type;
		struct sk_buff *sub_skb;
		u8 *pdata;
		u16 msdu_len, eth_type;
		u8 bsnaphdr;
		int hdr_len, payload_len;

		msdu_len = get_unaligned_be16(buf + off + ETH_HLEN - 2);
		if ((off + ETH_HLEN + msdu_len) > payload_total) {
			dev_warn(global_idev.pwhc_dev, "[whc]: amsdu: sub-msdu len bad!\n");
			break;
		}

		//Add AMSDU frame check if dst equals to rfc1042 header to fix FragAttacks vulnerabilities CVE-2020-24588
		if (buf[off] == 0xaa && buf[off + 1] == 0xaa && buf[off + 2] == 0x03 &&
			buf[off + 3] == 0x00 && buf[off + 4] == 0x00 && buf[off + 5] == 0x00) {
			ret = -1;
			break;
		}

		/* Strip the RFC1042 / bridge-tunnel SNAP after the sub-MSDU DA/SA/Length header. */
		psnap = (struct ieee80211_snap_hdr *)(buf + off + ETH_HLEN);
		psnap_type = (u8 *)psnap + sizeof(struct ieee80211_snap_hdr);
		if ((msdu_len >= sizeof(struct ieee80211_snap_hdr) + 2) &&
			((!memcmp(psnap, (void *)rfc1042_header, sizeof(struct ieee80211_snap_hdr)) &&
			  (memcmp(psnap_type, (void *)SNAP_ETH_TYPE_IPX, 2) != 0) &&
			  (memcmp(psnap_type, (void *)SNAP_ETH_TYPE_APPLETALK_AARP, 2) != 0)) ||
			 !memcmp(psnap, (void *)bridge_tunnel_header, sizeof(struct ieee80211_snap_hdr)))) {
			bsnaphdr = true;
		} else {
			bsnaphdr = false;
		}

		hdr_len = bsnaphdr ? (sizeof(struct ieee80211_snap_hdr) + 2) : 0;
		payload_len = msdu_len - hdr_len;

		sub_skb = netdev_alloc_skb(global_idev.pndev[iface_type], ETH_HLEN + payload_len);
		if (sub_skb == NULL) {
			dev_warn(global_idev.pwhc_dev, "[whc]: amsdu: sub skb NULL!\n");
			ret = -1;
			break;
		}

		/* Build the Ethernet frame: DA(6) SA(6) EtherType/Length(2) | payload. */
		pdata = skb_put(sub_skb, ETH_HLEN + payload_len);
		memcpy(pdata, buf + off, ETH_ALEN * 2); /* DA + SA */
		if (bsnaphdr) {
			memcpy(pdata + ETH_ALEN * 2, psnap_type, 2); /* EtherType from SNAP. */
		} else {
			eth_type = htons((unsigned short)payload_len);
			memcpy(pdata + ETH_ALEN * 2, &eth_type, 2); /* 802.3 length. */
		}
		memcpy(pdata + ETH_HLEN, buf + off + ETH_HLEN + hdr_len, payload_len);

		whc_host_if_netif_rx(sub_skb, NULL);

		/* Advance past this sub-MSDU and its 4-byte alignment padding. */
		off += ETH_HLEN + msdu_len;
		off = (off + 3) & ~3;
	}

	whc_host_recv_free_frame(prframe);
	return ret;
}

int whc_host_recv_process_indicatepkts(u8 iface_type, union recv_frame *prframe)
{
	if (prframe->u.hdr.attrib.b_amsdu) {
		return whc_host_recv_amsdu_to_msdu(iface_type, prframe);
	}

	whc_host_recv_wlanhdr_to_ethhdr(prframe);

	return whc_host_recv_indicatepkt(iface_type, prframe);
}

int whc_host_recv_func_posthandle(u8 iface_type, union recv_frame *prframe)
{
	struct rx_pkt_attrib    *pattrib = &prframe->u.hdr.attrib;
	struct whch_security_priv    *psecuritypriv = &global_idev.whchpriv.securitypriv[iface_type];
	struct sta_info         *psta;
	struct whch_rx_stats 	*pstats = &global_idev.whchpriv.rx_stats[iface_type];
	u8 *ptr = prframe->u.hdr.rx_data;
	u16 ether_type = 0;
	int ret = 0;

	//Add plaintext check to fix FragAttacks vulnerabilities CVE-2020-26140, CVE-2020-26143, CVE-2020-26145, CVE-2020-26147
	psta = prframe->u.hdr.psta;
	if ((psecuritypriv->dot11_wpa_mode && pattrib->encrypt == 0) && psta && (psta->sta_security.b_pairwise_key_installed == true)) {
		whc_host_recv_free_frame(prframe);
		ret = -1;
		goto _recv_data_drop;
	}

	prframe = whc_host_recv_decryptor(iface_type, psecuritypriv, prframe);
	if (prframe == NULL) {
		ret = -1;
		goto _recv_data_drop;
	}

	prframe = whc_host_recv_frame_check_defrag(iface_type, psecuritypriv, prframe);
	if (prframe == NULL) {
		ret = -1;
		goto _recv_data_drop;
	}

	if (prframe->u.hdr.attrib.b_privacy) {
		//check tkip mic code
		if (whc_host_recv_frame_chkmic(iface_type, psecuritypriv, prframe) == -1) {
			whc_host_recv_free_frame(prframe);
			prframe = NULL;
			ret = -1;
			goto _recv_data_drop;
		}
	}

	psta = prframe->u.hdr.psta;
	/* For A-MSDU the bytes after hdrlen+iv are the first sub-MSDU's DA/SA/Length, not an LLC
	 * header, so the ether_type read does not apply; an A-MSDU is never an EAPOL frame. */
	if (!pattrib->b_amsdu) {
		ptr += pattrib->hdrlen + pattrib->iv_len + LLC_HEADER_SIZE;
		memcpy(&ether_type, ptr, 2);
		ether_type = htons((unsigned short)ether_type);
	}

	if (psecuritypriv->dot11_wpa_mode) {
		if ((psta != NULL) && (psta->sta_security.b_ieee8021x_blocked)) {
			//blocked
			//only accept EAPOL frame
			if (pattrib->b_amsdu || ether_type != ETH_P_PAE || prframe->u.hdr.attrib.b_privacy) {
				whc_host_recv_free_frame(prframe);
				ret = -1;
				goto _recv_data_drop;
			}
		}
	}

	whc_host_recv_count_rx_stats(pstats, prframe, psta ? &psta->sta_mlmepriv : NULL);

	ret = whc_host_recv_process_indicatepkts(iface_type, prframe);

_recv_data_drop:
	return ret;
}

int  whc_host_recv_entry(union recv_frame *precvframe)
{
	int			ret = 0;
	u8 iface_type = precvframe->u.hdr.iface_type;
	struct rx_pkt_attrib *pattrib = &precvframe->u.hdr.attrib;
	struct whch_rx_stats *pstats = &global_idev.whchpriv.rx_stats[iface_type];
	struct whch_security_priv *psecuritypriv = &global_idev.whchpriv.securitypriv[iface_type];

#if 0   /* TODO_softap */
	if (iface_type == WHC_AP_PORT) {
		rtos_mutex_take(g_apmlmepriv.expire_mutex, MUTEX_WAIT_TIMEOUT);
	}
#endif

	if (GetFrameType(precvframe->u.hdr.rx_data) == RTW_DATA_TYPE) {
		pstats->cur_rx_data_rate = pattrib->data_rate;
	}

	/* Statistic rx pkts on each port */
	pstats->rx_packets++;

	/* if REG_SECCFG bit1=0, MAC will not search keycam, and rxdesc.rx_sw_dec will always set to 0, no matter this packet is decrypted or not */
	/* so, under SW encrypt, need set rx_attrib.b_decrypted by SW */
	if (psecuritypriv->b_sw_encrypt == 1) {
		precvframe->u.hdr.attrib.b_decrypted = 0;
	}

#ifdef CONFIG_SUPPLICANT_SME
	//ret = rtw_sme_recv_func_prehandle(iface_type, rtw_get_stats(iface_type), precvframe, true);
#else
	ret = whc_host_recv_func_prehandle(iface_type, precvframe);
#endif
	if (ret == 0) {
		ret = whc_host_recv_func_posthandle(iface_type, precvframe);
	}
#if 0   /* TODO_softap */
	if (iface_type == WHC_AP_PORT) {
		rtos_mutex_give(g_apmlmepriv.expire_mutex);
	}
#endif
	if (ret == -1) {
		goto _recv_entry_drop;
	}

	return ret;

_recv_entry_drop:
	return ret;
}
#endif