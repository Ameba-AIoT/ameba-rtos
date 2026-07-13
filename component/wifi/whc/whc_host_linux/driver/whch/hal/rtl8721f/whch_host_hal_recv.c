#include <whc_host_linux.h>

#ifdef CONFIG_WHCH
void whc_host_hal_rxdesc_query(struct rx_pkt_attrib	*pattrib, u8 *pdesc)
{
	memset(pattrib, 0, sizeof(struct rx_pkt_attrib));

	if (GET_RX_STATUS_DESC_C2H(pdesc)) {
		pattrib->pkt_rpt_type = C2H_PACKET;
	} else {
		pattrib->pkt_rpt_type = GET_RX_STATUS_DESC_PKT_INDICATE(pdesc);
	}

	if (pattrib->pkt_rpt_type != CH_INFO_NDPA && pattrib->pkt_rpt_type != CH_INFO_RSP) {
		//Offset 0
		pattrib->b_crc_err = (u8)GET_RX_STATUS_DESC_CRC32(pdesc);//((le32_to_cpu(pdesc->rxdw0) >> 14) & 0x1);
		pattrib->b_icv_err = (u8)GET_RX_STATUS_DESC_ICV(pdesc);//((le32_to_cpu(pdesc->rxdw0) >> 15) & 0x1);
		pattrib->encrypt = (u8)GET_RX_STATUS_DESC_SECURITY(pdesc);//((le32_to_cpu(pdesc->rxdw0) >> 20) & 0x7);
		pattrib->priority = (u8)GET_RX_STATUS_DESC_TID(pdesc);
		pattrib->shift_sz = (u8)GET_RX_STATUS_DESC_SHIFT(pdesc);//((le32_to_cpu(pdesc->rxdw0) >> 24) & 0x3);
		pattrib->b_decrypted = !GET_RX_STATUS_DESC_SWDEC(pdesc);//(le32_to_cpu(pdesc->rxdw0) & BIT(27))? 0:1;
		pattrib->drvinfo_sz = (u8)GET_RX_STATUS_DESC_DRV_INFO_SIZE(pdesc) * 8;
	}

	pattrib->b_qos = (u8)GET_RX_STATUS_DESC_QOS(pdesc);

	//Offset 4
	pattrib->b_mdata = (u8)GET_RX_STATUS_DESC_MORE_DATA(pdesc);//((le32_to_cpu(pdesc->rxdw1) >> 26) & 0x1);
	pattrib->b_mfrag = (u8)GET_RX_STATUS_DESC_MORE_FRAG(pdesc);//((le32_to_cpu(pdesc->rxdw1) >> 27) & 0x1);//more fragment bit
	pattrib->A1_matched = (u8)GET_RX_STATUS_DESC_PAM(pdesc);
	// frame_type not in rx_pkt_attrib
	pattrib->b_bc = (u8)GET_RX_STATUS_DESC_BC(pdesc);

	//Offset 8
	pattrib->seq_num = (u16)GET_RX_STATUS_DESC_SEQ(pdesc);//(le32_to_cpu(pdesc->rxdw2) & 0x00000fff);
	pattrib->frag_num = (u8)GET_RX_STATUS_DESC_FRAG(pdesc);//((le32_to_cpu(pdesc->rxdw2) >> 12) & 0xf);//fragmentation number
	pattrib->sgi = (u8)GET_RX_STATUS_DESC_GI_LTF(pdesc);

	switch (pattrib->pkt_rpt_type) {
	case NORMAL_RX:
	case C2H_PACKET:
		pattrib->pkt_len = (u16)GET_RX_STATUS_DESC_PKT_LEN(pdesc);
		break;
	case CH_INFO_NDPA:
	case CH_INFO_RSP:
		pattrib->pkt_len = (u16)GET_RX_STATUS_DESC_CH_INFO_CSI_LEN(pdesc) * 8;
		break;
	case PPDU_STATUS:
		pattrib->pkt_len = (u16)GET_RX_STATUS_DESC_PHYSTS_LEN(pdesc) * 8;
		break;
	default:
		break;
	}

	//Offset 12
	pattrib->data_rate = (u16)GET_RX_STATUS_DESC_RX_RATE(pdesc); //((le32_to_cpu(pdesc->rxdw3))&0x7f);

	//Offset 16
	// macid_from_rxdesc / phy_info not in rx_pkt_attrib

}

static int whc_host_hal_seg_reassemble(u8 *pdata, struct rx_deseg_priv *pdeseg, u8 first_seg, u8 last_seg, int seg_len)
{
	u8 *src = NULL;
	int len = 0;

	if (first_seg == 1) {
		/* First (or only) segment: copy from the rxbd start to keep the prefix + mac header. */
		src = pdata - RX_BUFFER_DESC_SIZE;
		len = (last_seg == 1) ? pdeseg->reasm_total : seg_len;
	} else {
		/* Continuation segment: append the body (no rxdesc on follow-up segments). */
		src = pdata;
		len = (last_seg == 1) ? (pdeseg->reasm_total - pdeseg->reasm_len) : seg_len;
	}

	if (len < 0 || (pdeseg->reasm_len + len) > pdeseg->reasm_total) {
		dev_warn(global_idev.pwhc_dev, "[whc]: deseg: reasm overflow!\n");
		return -1;
	}

	memcpy(pdeseg->deseg_skb->data + pdeseg->reasm_len, src, len);
	pdeseg->reasm_len += len;
	return 0;
}

int whc_host_hal_rx_mpdu_deseg(u8 *rxbd, struct sk_buff **pskb, u8 *pdata)
{
	struct sk_buff *skb = *pskb;
	struct rx_deseg_priv	*pdeseg = &global_idev.whchpriv.recvpriv.deseg_priv;
	struct rx_pkt_attrib *pattrib = NULL;
	u8 *pframe = NULL;
	u8 first_seg = (u8)GET_RX_BUFFER_DESC_FS_92E(rxbd);
	u8 last_seg = (u8)GET_RX_BUFFER_DESC_LS_92E(rxbd);
	u8 is_normal_qos_data;

	if (pdeseg->wait_sub_seg) {
		/* Case A: reassembly in progress, expecting a continuation (FS == 0) segment. */
		if (first_seg == 1) {
			/* A new frame starts before the previous one finished: drop the stale
			 * accumulation and let this segment restart on the normal flow. */
			whc_host_hal_deseg_priv_reset(pdeseg);
			return 0;
		}
	} else {
		/* Case B: no reassembly in progress. Classify this segment by FS/LS:
		 *   FS == 0           -> orphan middle/last segment, no frame to attach it to: drop.
		 *   FS == 1, LS == 1  -> single complete segment.
		 *   FS == 1, LS == 0  -> first segment of a multi-segment frame.
		 */
		if (first_seg == 0) {
			return -1; /* Orphan middle/last segment: drop. */
		}

		/* FS == 1 from here. Only a NORMAL_RX QoS data frame is a reassembly candidate. */
		is_normal_qos_data = (!GET_RX_STATUS_DESC_C2H(pdata)) &&
							 (GET_RX_STATUS_DESC_PKT_INDICATE(pdata) == NORMAL_RX) &&
							 ((u8)GET_RX_STATUS_DESC_QOS(pdata) == 1);
		if (!is_normal_qos_data) {
			/* Not a candidate: a single complete segment (LS == 1) goes the normal mpdu path;
			 * the first segment of a frame we cannot reassemble is dropped. */
			return (last_seg == 1) ? 0 : -1;
		}

		pframe = pdata + RXDESC_SIZE + (u8)GET_RX_STATUS_DESC_DRV_INFO_SIZE(pdata) * 8 + (u8)GET_RX_STATUS_DESC_SHIFT(pdata);
		if (get_tofr_ds(pframe) == 3) {
			return -1;
		}
		if (last_seg == 1 && GetAMsdu(pframe + 24) == 0) {
			return 0; /* Single complete non-AMSDU segment: normal mpdu path. */
		}

		/* Take over and start reassembly:
		 *   FS == 1, LS == 0          -> first segment of a multi-segment frame, or
		 *   FS == 1, LS == 1, A-MSDU  -> single-segment A-MSDU. */
		pattrib = &pdeseg->attrib;

		whc_host_hal_rxdesc_query(pattrib, pdata);

		if (pattrib->b_crc_err) {
			return -1;
		}

		pattrib->hdrlen = 26;	/* Not consider to_fr_ds = 3 case. */
		if (pattrib->b_order) { //HT-CTRL 11n
			pattrib->hdrlen += 4;
		}
		pattrib->iv_len = pattrib->encrypt ? 8 : 0;

		/* Allocate the raw-layout reassembly skb: [rxbd + rxdesc + drvinfo + shift][mac header][body].
		 *    The mac header is kept inside deseg_skb at prefix_len, so no separate backup is needed. */
		pdeseg->prefix_len = RX_BUFFER_DESC_SIZE + RXDESC_SIZE + pattrib->drvinfo_sz + pattrib->shift_sz;
		pdeseg->reasm_total = pdeseg->prefix_len + pattrib->pkt_len;
		pdeseg->reasm_len = 0;
		if (pattrib->pkt_len <= (pattrib->hdrlen + pattrib->iv_len)) {
			whc_host_hal_deseg_priv_reset(pdeseg);
			return -1;
		}
		pdeseg->deseg_skb = netdev_alloc_skb(global_idev.pndev[0], SKB_DATA_ALIGN(pdeseg->reasm_total));
		if (pdeseg->deseg_skb == NULL) {
			dev_warn(global_idev.pwhc_dev, "[whc]: deseg: reasm skb NULL!\n");
			whc_host_hal_deseg_priv_reset(pdeseg);
			return -1;
		}
		skb_put(pdeseg->deseg_skb, pdeseg->reasm_total);
	}

	pdeseg->wait_sub_seg = (last_seg == 1) ? 0 : 1;

	if (whc_host_hal_seg_reassemble(pdata, pdeseg, first_seg, last_seg, skb->len - RX_BUFFER_DESC_SIZE) != 0) {
		whc_host_hal_deseg_priv_reset(pdeseg);
		return -1;
	}

	if (skb) {
		dev_kfree_skb_any(skb);
		*pskb = NULL;
	}

	if (last_seg == 1) {
		struct sk_buff *skb_new = pdeseg->deseg_skb;

		pdeseg->deseg_skb = NULL;
		whc_host_hal_deseg_priv_reset(pdeseg);
		*pskb = skb_new;
		return 0;
	}

	return -1;
}

int whc_host_hal_rx_mpdu(struct sk_buff *pskb)
{
	union recv_frame	*precvframe = NULL;
	struct rx_pkt_attrib	*pattrib;
	u8 *rx_desc, *rxbd, *pphy_info = NULL;
	u32 skb_len;
	int not_free_recvframe = 0;

	/* pskb->data pointer to rxbd */
	rxbd = pskb->data;
	rx_desc = pskb->data + RX_BUFFER_DESC_SIZE;
	if (whc_host_hal_rx_mpdu_deseg(rxbd, &pskb, rx_desc) != 0) {
		if (pskb) {
			dev_kfree_skb_any(pskb);
		}
		not_free_recvframe = 1;
		goto done;
	}
	/* pskb may have been replaced by a reassembled single MSDU; re-derive rxdesc. */
	rx_desc = pskb->data + RX_BUFFER_DESC_SIZE;

	/* adjust skb pointers to rxdesc */
	skb_pull(pskb, RX_BUFFER_DESC_SIZE);

	precvframe = whc_host_recv_alloc_frame();
	if (precvframe == NULL) {
		dev_warn(global_idev.pwhc_dev, "[whc]: precvframe NULL\n");
		goto done;
	}

	INIT_LIST_HEAD(&precvframe->u.hdr.list);
	precvframe->u.hdr.len = 0;

	whc_host_hal_rxdesc_query(&precvframe->u.hdr.attrib, rx_desc);
	pattrib = &precvframe->u.hdr.attrib;

	if (pattrib->pkt_len == 0 || pattrib->b_crc_err) {
		dev_err(global_idev.pwhc_dev, "[whc]: Drop pkt[%d,%d,%d]\n", pattrib->b_crc_err, pattrib->b_icv_err, pattrib->pkt_len);
		goto done;
	}

	if (pattrib->b_physt_valid && pattrib->drvinfo_sz) {
		pphy_info = (u8 *)(rx_desc + RXDESC_SIZE);
	}

	skb_len = pattrib->pkt_len;

	/* adjust skb pointers */
	pskb->dev = global_idev.pndev[0];
	precvframe->u.hdr.pkt = pskb;
	skb_pull(pskb, RXDESC_SIZE + pattrib->drvinfo_sz + pattrib->shift_sz);
	pskb->len = skb_len;
	precvframe->u.hdr.rx_head = pskb->head;
	precvframe->u.hdr.rx_data = precvframe->u.hdr.rx_tail = pskb->data;
	precvframe->u.hdr.rx_end = skb_end_pointer(pskb);

	whc_host_recv_recvframe_put(precvframe, skb_len);
	switch (pattrib->pkt_rpt_type) {
	case NORMAL_RX: {
#if 0	/* TODO_promisc */
		promisc_ret = wifi_hal_rx_promisc(precvframe, pphy_info);
		if (g_promiscpriv.promisc_enabled && (promisc_ret == RTW_PROMISC_BYPASS_DRV_HDL)) {
			goto done;
		} else
#endif
		{
#if 0	/* TODO_mp */
			if (rtw_halphy_shareinfo.phl_drv_mode == RTW_DRV_MODE_MP) {
				wifi_hal_mp_rx_process(precvframe, pphy_info);
			} else
#endif
			{
				whc_host_hal_normal_rx_process(precvframe, pphy_info);
				not_free_recvframe = 1;/*may handle fragment frame and not free recvframe*/
			}
		}
		break;
	}
	default:
		break;
	}

done:
	if (not_free_recvframe == 0) {
		whc_host_recv_free_frame(precvframe);
	}
	return 0;
}

#endif