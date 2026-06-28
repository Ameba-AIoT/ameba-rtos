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

#if 0	/* TODO_AMSDU */
void whc_host_hal_amsdu_priv_reset(struct amsdu_priv_t *pamsdu)
{
	if (pamsdu->pending_skb) {
		dev_kfree_skb_any(pamsdu->pending_skb);
	}
	memset(pamsdu, 0, sizeof(struct amsdu_priv_t));
}

int whc_host_hal_rx_amsdu_check(u8 *rxbd, u8 *pdata, u8 *first_seg, u8 *last_seg)
{
	u8 *pframe = NULL;
	struct amsdu_priv_t	*pamsdu = global_idev.whchpriv.recvpriv.amsdu_priv;
	*first_seg = (u8)GET_RX_BUFFER_DESC_FS_92E(rxbd);
	*last_seg = (u8)GET_RX_BUFFER_DESC_LS_92E(rxbd);

	// Case 1: A-MSDU seg middle/last
	if (pamsdu && pamsdu->wait_sub_seg) {
		return 1;
	}

	pframe = pdata + RXDESC_SIZE + (u8)GET_RX_STATUS_DESC_DRV_INFO_SIZE(pdata) * 8 + (u8)GET_RX_STATUS_DESC_SHIFT(pdata);

	/* Case 2: A-MSDU seg none/first. */
	if ((*first_seg == 1) && (!GET_RX_STATUS_DESC_C2H(pdata)) &&
		(GET_RX_STATUS_DESC_PKT_INDICATE(pdata) == NORMAL_RX) &&
		((u8)GET_RX_STATUS_DESC_QOS(pdata) == 1) &&
		(GetAMsdu((pframe + (get_tofr_ds(pframe) == 3 ? 30 : 24))) == 1)) {	/*https://jira.realtek.com/browse/RSWLANDIOT-13108*/

		if (get_tofr_ds(pframe) == 3) {
			return -2; /* Not consider to_fr_ds = 3 case */
		}
		return 2;
	} else if (*first_seg == 0 || *last_seg == 0) { /* Case 3: none A-MSDU middle/last seg. */
		return 3;
	} else { /* Case 4: Normal MSDU. */
		return 4;
	}
}

int whc_host_hal_rx_amsdu(u8 *rxbd, struct sk_buff *skb, u8 *pdata, void *dev_addr)
{
	struct __queue *pfree_recv_queue = &global_idev.whchpriv.recvpriv.free_recv_queue;
	union recv_frame *precvframe = NULL;
	struct sk_buff *skb_new = NULL, *pkt_copy = NULL;
	struct amsdu_priv_t	*pamsdu = global_idev.whchpriv.recvpriv.amsdu_priv;
	struct rx_pkt_attrib *pattrib = NULL;
	u8 first_seg = 0;
	u8 last_seg = 0;
	int seg_len = 0, current_seg_copy_len = 0, padding_len = 0, amsdu_case = 0;
	u16 msdu_len = 0;

	amsdu_case = whc_host_hal_rx_amsdu_check(rxbd, pdata, &first_seg, &last_seg);

	switch (amsdu_case) {
	case 1: // Case 1: A-MSDU seg middle/last.
		if (first_seg == 1) {
			/* None SEG: Process as normal packets. */
			/* First SEG: Wrong SEG and cannot be processed as normal packets. Return success and drop by pkt_len too large. */
			whc_host_hal_amsdu_priv_reset(pamsdu);
			return RTK_SUCCESS;
		} else if (last_seg == 1) {
			pamsdu->wait_sub_seg = 0;
		}
		pattrib = &pamsdu->attrib;
		goto process_current_seg;
	case 2: // Case 2: A-MSDU seg none/first.
		if (!pamsdu) {
			dev_dbg(global_idev.pwhc_dev, "[whc]: amsdu appear\n");
			global_idev.whchpriv.recvpriv.amsdu_priv = (struct amsdu_priv_t *)wifi_rom_zmalloc(sizeof(struct amsdu_priv_t));
			if (!global_idev.whchpriv.recvpriv.amsdu_priv) {
				dev_err(global_idev.pwhc_dev, "[whc]: No memeory for amsdu_priv.\n");
				return RTK_FAIL;
			}
			pamsdu = global_idev.whchpriv.recvpriv.amsdu_priv;
		}
		pattrib = &pamsdu->attrib;

		/* 1. Set attrib in amsdu_priv. */
		whc_host_hal_rxdesc_query(pattrib, pdata);

		if (pattrib->b_crc_err) {
			return RTK_FAIL;
		}

		/* 2. Caculate mac header length. Set iv/icv len in amsdu_priv. Simplified by rtw_recv_validate_data_frame. */
		pattrib->hdrlen = 26;	/* Not consider to_fr_ds = 3 case. */
		if (pattrib->b_order) { //HT-CTRL 11n
			pattrib->hdrlen += 4;
		}

		if (pattrib->encrypt) {
			/* WEP icv len? */
			pattrib->iv_len = pattrib->icv_len = 8;
		} else {
			pattrib->iv_len = pattrib->icv_len = 0;
		}

		pamsdu->mac_hdr_len = pattrib->hdrlen + pattrib->iv_len;
		pamsdu->icv_len = pattrib->icv_len;

		if (pamsdu->mac_hdr_len > MAC_HEADER_RESEARVED_LEN) {
			dev_warn(global_idev.pwhc_dev, "[whc]: mac hdr too long!");
			return RTK_FAIL;
		}

		/* 3. Move pdata to data position and backup mac header. */
		pdata += (RXDESC_SIZE + pattrib->drvinfo_sz + pattrib->shift_sz);
		memcpy(pamsdu->ma c_hdr, pdata, pamsdu->mac_hdr_len);

		if (last_seg == 0) {
			pamsdu->wait_sub_seg = 1;
		} else {
			pamsdu->wait_sub_seg = 0;
		}

		goto process_current_seg;
	case 3: // Case 3: none A-MSDU middle/last seg.
		return RTK_FAIL;
	case 4: // Case 4: Normal MSDU.
		return RTK_SUCCESS;
	default: // Error case.
		return RTK_FAIL;
	}

process_current_seg:

	/* Move pdata to sub-amsdu position and caculate the sub-amsdu total length. */
	if (first_seg == 1 && last_seg == 1) {
		seg_len = pattrib->pkt_len - pamsdu->mac_hdr_len;
		pdata += pamsdu->mac_hdr_len;
	} else if (first_seg == 1 && last_seg == 0) {
		seg_len = skbpriv.skb_buf_max_size - (RXDESC_SIZE + pattrib->drvinfo_sz + pattrib->shift_sz) - pamsdu->mac_hdr_len;
		pamsdu->remain_len = pattrib->pkt_len - pamsdu->mac_hdr_len - seg_len;
		pdata += pamsdu->mac_hdr_len;
	} else if (first_seg == 0 && last_seg == 0) {
		seg_len = skbpriv.skb_buf_max_size;
		pamsdu->remain_len -= skbpriv.skb_buf_max_size;
	} else {
		seg_len = pamsdu->remain_len;
	}

	while ((last_seg == 0 && seg_len > 0) || (last_seg == 1 && seg_len > pattrib->icv_len)) {
		if (pamsdu->pending_skb) {
			/* Get pending skb and copy more. */
			skb_new = pamsdu->pending_skb;
			/* Check AMSDU ETH_HLEN complete or not. */
			if (pamsdu->pending_skb_len < ETH_HLEN) {
				memcpy(skb_new->data + pamsdu->mac_hdr_len - ETH_HLEN + pamsdu->pending_skb_len, pdata, ETH_HLEN - pamsdu->pending_skb_len);
				pdata += (ETH_HLEN - pamsdu->pending_skb_len);
				seg_len -= (ETH_HLEN - pamsdu->pending_skb_len);
				pamsdu->pending_skb_len = 0;
			}
		} else {
			/* Malloc new, copy. If complete, submit. */
			skb_new = wifi_rom_dev_alloc_skb(SKB_DATA_ALIGN(skbpriv.skb_buf_max_size), 0);
			if (skb_new == NULL) {
				dev_warn(global_idev.pwhc_dev, "[whc]: amsdu: skb NULL!\n");
				return RTK_FAIL;
			}

			skb_assign_buf(skb_new, skb_new->data, SKB_DATA_ALIGN(skbpriv.skb_buf_max_size));
			skb_new->len = SKB_DATA_ALIGN(skbpriv.skb_buf_max_size);
			skb_new->dev = dev_addr;
			skb_new->tail = skb_new->data + pamsdu->mac_hdr_len;

			if (seg_len >= ETH_HLEN) {
				memcpy(skb_new->data + pamsdu->mac_hdr_len - ETH_HLEN, pdata, ETH_HLEN);
				seg_len -= ETH_HLEN;
				pdata += ETH_HLEN;
			} else {
				memcpy(skb_new->data + pamsdu->mac_hdr_len - ETH_HLEN, pdata, seg_len);
				pamsdu->pending_skb = skb_new;
				pamsdu->pending_skb_len = seg_len;
				return RTK_FAIL;
			}
		}

		msdu_len = ReadEF2Byte(skb_new->data + pamsdu->mac_hdr_len - 2);
		msdu_len = htons((unsigned short)msdu_len);

		if ((seg_len + pamsdu->pending_skb_len) < msdu_len) {
			if (last_seg == 1) {
				wifi_rom_dev_kfree_skb_any(skb_new);
				whc_host_hal_amsdu_priv_reset(pamsdu);
				return RTK_FAIL;
			} else {
				/* Copy some of next msdu. */
				memcpy(skb_new->data + pamsdu->mac_hdr_len, pdata, seg_len);
				skb_new->tail = skb_new->data + pamsdu->mac_hdr_len + seg_len;
				pamsdu->pending_skb = skb_new;
				pamsdu->pending_skb_len = seg_len;
				return RTK_FAIL;
			}
		}

		/* Allocate a new precvframe to put split mpdu+msdu. */
		precvframe = whc_host_recv_alloc_frame();
		if (precvframe == NULL) {
			dev_warn(global_idev.pwhc_dev, "[whc]: precvframe NULL\n");
			wifi_rom_dev_kfree_skb_any(skb_new);
			return RTK_FAIL;
		}
		rtw_init_listhead(&precvframe->u.hdr.list);
		precvframe->u.hdr.len = 0;
		memcpy(&precvframe->u.hdr.attrib, pattrib, sizeof(struct rx_pkt_attrib));

		memcpy(precvframe->u.hdr.attrib.dst, skb_new->data + pamsdu->mac_hdr_len - ETH_HLEN, ETH_ALEN);
		memcpy(precvframe->u.hdr.attrib.src, skb_new->data + pamsdu->mac_hdr_len - ETH_HLEN + ETH_ALEN, ETH_ALEN);
		//Add AMSDU frame check if dst equals to rfc1042 header to fix FragAttacks vulnerabilities CVE-2020-24588
		if (precvframe->u.hdr.attrib.dst[0] == 0xaa && precvframe->u.hdr.attrib.dst[1] == 0xaa && precvframe->u.hdr.attrib.dst[2] == 0x03 &&
			precvframe->u.hdr.attrib.dst[3] == 0x00 && precvframe->u.hdr.attrib.dst[4] == 0x00 && precvframe->u.hdr.attrib.dst[5] == 0x00) {
			goto err_exit;
		}

		current_seg_copy_len = msdu_len - pamsdu->pending_skb_len;
		memcpy(skb_new->data, pamsdu->mac_hdr, pamsdu->mac_hdr_len);
		memcpy(skb_new->tail, pdata, current_seg_copy_len);
		seg_len -= current_seg_copy_len;
		pdata += current_seg_copy_len;

		skb_new->len = SKB_DATA_ALIGN(pamsdu->mac_hdr_len + msdu_len + pamsdu->icv_len);
		skb_new->tail = skb_new->data + pamsdu->mac_hdr_len + msdu_len + pamsdu->icv_len;
		precvframe->u.hdr.pkt = skb_new;
		precvframe->u.hdr.rx_head = skb_new->head;
		precvframe->u.hdr.rx_data = precvframe->u.hdr.rx_tail = skb_new->data;
		precvframe->u.hdr.rx_end = skb_end_pointer(skb_new);
		precvframe->u.hdr.attrib.pkt_len = pamsdu->mac_hdr_len + msdu_len + pamsdu->icv_len;

		whc_host_recv_recvframe_put(precvframe, pamsdu->mac_hdr_len + msdu_len + pamsdu->icv_len);
		whc_host_hal_normal_rx_process(precvframe);

		pamsdu->pending_skb = NULL;
		pamsdu->pending_skb_len = 0;

		/* Caculate padding bytes. */
		if (((u32)(msdu_len + ETH_HLEN) & ((u32)0x3)) == 0) {
			padding_len = 0;
		} else {
			padding_len = 4 - ((u32)(msdu_len + ETH_HLEN) & ((u32)0x3));
		}
		pdata += padding_len;
		seg_len -= padding_len;
	}

	if (skb) {
		pkt_copy = wifi_rom_dev_alloc_skb(skbpriv.skb_buf_max_size, 0);
		if (pkt_copy) {
			global_idev.whchpriv.recvpriv.rx_ring[0].rx_buf[global_idev.whchpriv.recvpriv.rx_ring[0].idx] = pkt_copy;
			SET_RX_BUFFER_PHYSICAL_LOW_92E(rxbd, (u32)pkt_copy->data);
			/*In case of cache auto write back when cache entry full*/
			DCache_Invalidate((u32)(pkt_copy->data), skbpriv.skb_buf_max_size);
			wifi_rom_dev_kfree_skb_any(skb);
		} else {
			dev_warn(global_idev.pwhc_dev, "[whc]: skb not enough!\n");
			return RTK_FAIL;
		}
	}

	if (last_seg == 1) {
		whc_host_hal_amsdu_priv_reset(pamsdu);
	}

	/* Jump normal mpdu processing flow, update rxbd ring directly. */
	return RTK_FAIL;

err_exit:
	whc_host_recv_free_frame(precvframe);
	wifi_rom_dev_kfree_skb_any(skb_new);
	whc_host_hal_amsdu_priv_reset(pamsdu);
	return RTK_FAIL;
}
#endif

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
	//whc_host_hal_rx_amsdu(rxbd, pskb, rx_desc);	/* TODO_AMSDU */

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


/* TODO_deseg */
int whc_host_hal_rx_mpdu_deseg(u8 *rxbd, struct sk_buff *skb, u8 *pdata)
{
	u8 first_seg = (u8)GET_RX_BUFFER_DESC_FS_92E(rxbd);
	u8 last_seg = (u8)GET_RX_BUFFER_DESC_LS_92E(rxbd);

	if (first_seg == 1 && last_seg == 1) {
		/* MPDU occupy one seg */
		return 0;
	}

	/* MPDU occupy multiple segs */
	if (first_seg == 1) {
		/* first seg */

	}
	return -1;
}

#endif