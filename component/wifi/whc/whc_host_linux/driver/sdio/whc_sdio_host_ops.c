// SPDX-License-Identifier: GPL-2.0-only
/*
 * Realtek wireless local area network IC driver.
 *
 * Copyright(c) 2024 Realtek Corporation. All rights reserved.
 */
#include <whc_host_linux.h>

#ifndef WHC_RX_AGG
static struct sk_buff *whc_sdio_host_read_rxfifo(struct whc_sdio *priv, u32 size)
{
	u32 allocsize, ret;
	struct sk_buff *pskb = NULL;

	allocsize = _RND(size, priv->func->cur_blksize);

	pskb = netdev_alloc_skb(NULL, allocsize);
	if (pskb == NULL) {
		dev_err(global_idev.pwhc_dev, "%s: Alloc skb rx buf Err, alloc_sz %d!!\n\r",
				__func__, allocsize);
		return NULL;
	}

	ret = rtw_read_port(priv, SDIO_RX_FIFO_DOMAIN_ID, size, pskb->data);
	if (ret == false) {
		kfree_skb(pskb);
		dev_err(&priv->func->dev, "%s: read port FAIL!\n", __FUNCTION__);
		return NULL;
	}

	//print_hex_dump_bytes("whc_sdio_host_read_rxfifo: ", DUMP_PREFIX_NONE, pskb->data, size);

	return pskb;
}
#else

static u8 *whc_sdio_host_read_rxfifo_agg(struct whc_sdio *priv, u32 size)
{
	u32 allocsize, ret;
	u8 *pbuf = NULL;

	allocsize = _RND(size, priv->func->cur_blksize);

	pbuf = kmalloc(allocsize, GFP_ATOMIC);
	if (pbuf == NULL) {
		dev_err(&priv->func->dev, "%s: Alloc rx buf Err, alloc_sz %d!\n", __func__, allocsize);
		return NULL;
	}

	ret = rtw_read_port(priv, SDIO_RX_FIFO_DOMAIN_ID, size, pbuf);
	if (ret == false) {
		kfree(pbuf);
		dev_err(&priv->func->dev, "%s: read port FAIL!\n", __FUNCTION__);
		return NULL;
	}
	return pbuf;
}

/*
 * size (4B aligned, incl. RX_DESC) of the aggregated RX segment  at @ptr,
 * derived from its header via union whc_hdr. Returns 0 when the segment can't
 * be sized (unknown event, or BT which is not yet de-aggregated) so the caller
 * stops walking the buffer instead of misparsing.
 */
static u32 whc_sdio_host_rxagg_segment_size(u8 *ptr)
{
	union whc_hdr *hdr = (union whc_hdr *)(ptr + SIZE_RX_DESC);
	u32 body;

	switch (hdr->msg.event) {
	case WHC_WIFI_EVT_RECV_PKTS:
	case WHC_WIFI_EVT_FLOWCTRL:
		/* whc_msg_info: header + pad + payload */
		body = sizeof(struct whc_msg_info) + hdr->msg.data_len + hdr->msg.pad_len;
		break;
	case WHC_WIFI_EVT_API_CALL:
	case WHC_WIFI_EVT_API_RETURN:
		/* whc_api_info: header + payload (no pad; @12 is a reserved placeholder) */
		body = sizeof(struct whc_api_info) + hdr->api.data_len;
		break;
	case WHC_WIFI_EVT_CMD:
		/* whc_cmd_path_hdr: header + payload (no pad field) */
		body = sizeof(struct whc_cmd_path_hdr) + hdr->cmd.len;
		break;
	default:
		/* BT INIC (WHC_BT_EVT_BASE..MAX) reserved, or corrupt header */
		return 0;
	}

	body += SIZE_RX_DESC;
	return _RND4(body);
}
#endif /* WHC_RX_AGG */

/* Block until at least one TXBD is free. Returns the free count, or 0 on timeout.
 * Caller must hold priv->lock. */
static u16 whc_sdio_host_wait_txbd(struct whc_sdio *priv)
{
	u32 polling_num = 0;

	(void) polling_num;

	// check if hardware tx fifo page is enough
	while (priv->SdioTxBDFreeNum < 1) {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		/* Reset flag before query so any ISR firing after this point is
		 * guaranteed to be caught by wait_event_timeout's condition check.
		 * Should clear tx_avail_int_triggered each loop: jira: https://jira.realtek.com/browse/RSWLANDIOT-11649,
		 * but can't put it after rtw_sdio_query_txbd_status considering lost-wakeup race */
		priv->tx_avail_int_triggered = 0;
		rtw_sdio_query_txbd_status(priv);
		if (priv->SdioTxBDFreeNum >= 1) {
			break;
		}
		if (!wait_event_timeout(priv->txbd_wq, priv->tx_avail_int_triggered == 1, msecs_to_jiffies(100))) {
			dev_err(&priv->func->dev, "%s: TXBD unavailable, TX FAIL\n", __FUNCTION__);
			return 0;
		}
#else
		polling_num++;
		if ((polling_num % 60) == 0) {
			msleep(1);
		}

		// Total number of TXBD is NOT available, so update current TXBD status
		rtw_sdio_query_txbd_status(priv);
#endif
	}

	return priv->SdioTxBDFreeNum;
}

void whc_sdio_host_send_data(u8 *buf, u32 len, struct sk_buff *pskb)
{
	struct whc_sdio *priv = &whc_sdio_priv;
	INIC_TX_DESC *ptxdesc;

	(void) pskb;

	/* wakeup device if it's in power save mode before send msg */
	if (priv->dev_state == PWR_STATE_SLEEP) {
		dev_dbg(&priv->func->dev, "%s: wakeup device", __func__);
		if (whc_sdio_host_resume_common(priv)) {
			dev_err(&priv->func->dev, "%s: fail to wakeup device, stop send", __func__);
			return;
		}
	}

	mutex_lock(&priv->lock);

	/* block until at least one TXBD is free */
	if (whc_sdio_host_wait_txbd(priv) == 0) {
		goto exit;
	}

	if (len > priv->SdioTxMaxSZ) {
		dev_err(&priv->func->dev, "%s: PKT SIZE ERROR, total size: %d\n", __FUNCTION__, len);
		goto exit;
	}

	ptxdesc = (INIC_TX_DESC *)buf;
	ptxdesc->txpktsize = len - SIZE_TX_DESC;
	ptxdesc->offset = SIZE_TX_DESC;
	ptxdesc->type = TX_PACKET_802_3;
	ptxdesc->bus_agg_num = 1;

	//print_hex_dump_bytes("whc_sdio_host_send_data: ", DUMP_PREFIX_NONE, buf, len);

	rtw_write_port(priv, SDIO_TX_FIFO_DOMAIN_ID, len, buf);

	if (priv->SdioTxBDFreeNum > 0) {
		priv->SdioTxBDFreeNum -= 1;
	}

	/* update txbd num in sw after send one pkt */
	priv->txbd_wptr = (priv->txbd_wptr + 1) % priv->txbd_size;
exit:
	mutex_unlock(&priv->lock);

	return;
}

#ifdef WHC_RX_AGG
/*
 * Walk one hardware-aggregated rx buffer: split it into per-packet segments by
 * header, copy each into its own skb and dispatch it, then free the buffer.
 */
static void whc_sdio_host_rxagg_dispatch_buf(struct whc_sdio *priv, u8 *buf, u32 total)
{
	struct sk_buff *pskb;
	u8 *ptr = buf;
	u32 size;

	while (1) {
		/* size the next segment from its header (RX_DESC + hdr + pad + payload) */
		size = whc_sdio_host_rxagg_segment_size(ptr);
		if (size == 0) {
			dev_dbg(&priv->func->dev, "agg evt err %x", *(u32 *)(ptr + SIZE_RX_DESC));
			//print_hex_dump_bytes("err data: ", DUMP_PREFIX_NONE, ptr, sizeof(union whc_hdr));
			break;
		}

		/* guard against a corrupt hdr: the segment must fit in the remaining rx data */
		if ((u32)(ptr - buf) + size > total) {
			dev_err(&priv->func->dev, "agg size err: size %d off %d total %d", size, (int)(ptr - buf), total);
			//print_hex_dump_bytes("err data: ", DUMP_PREFIX_NONE, ptr, sizeof(union whc_hdr));
			break;
		}

		pskb = netdev_alloc_skb(NULL, size);
		if (pskb == NULL) {
			dev_err(&priv->func->dev, "agg alloc rx skb fail, size %d", size);
			break;
		}
		memcpy(pskb->data, ptr, size);

		/* skip RX_DESC */
		whc_host_recv_dispatch(pskb);
		ptr += size;
		/* stop once too little rx data remains to hold even the
		 * smallest header (id + len): the rest is trailing
		 * alignment padding, not another segment */
		if ((u32)(ptr - buf) + 8 > total) {
			break;
		}
	}

	kfree(buf);
}
#endif /* WHC_RX_AGG */

static void whc_sdio_host_recv_data(void *intf_priv)
{
	struct whc_sdio *sdio_priv = (struct whc_sdio *)intf_priv;
	u32 himr;
	u32 SdioRxFIFOSize;
#ifdef WHC_RX_AGG
	u8 *buf;
#else
	struct sk_buff *pskb;
#endif

	/* wakeup device if it's sleep */
	if (sdio_priv->dev_state == PWR_STATE_SLEEP) {
		dev_dbg(&sdio_priv->func->dev, "%s: wakeup device", __func__);
		if (whc_sdio_host_resume_common(sdio_priv)) {
			dev_err(&sdio_priv->func->dev, "%s: fail to wakeup device, stop read rxfifo", __func__);
			return;
		}
	}

	do {
		SdioRxFIFOSize = rtw_sdio_get_rx_len(sdio_priv);
		if (SdioRxFIFOSize == 0) {
			break;
		} else {
#ifdef WHC_RX_AGG
			buf = whc_sdio_host_read_rxfifo_agg(sdio_priv, SdioRxFIFOSize);
			if (buf) {
				whc_sdio_host_rxagg_dispatch_buf(sdio_priv, buf, SdioRxFIFOSize);
			} else {
				break;
			}
#else
			pskb = whc_sdio_host_read_rxfifo(sdio_priv, SdioRxFIFOSize);
			if (pskb) {
				/* skip RX_DESC */
				whc_host_recv_dispatch(pskb);
			} else {
				break;
			}
#endif /* WHC_RX_AGG */
		}
	} while (1);

	/* restore RX_REQ interrupt*/
	himr = cpu_to_le32(sdio_priv->sdio_himr);
	rtw_write32(sdio_priv, SDIO_REG_HIMR, himr);
}

/*
 * Drain the tx queue: each round coalesces queued frames into one bus transfer.
 * The current free TXBD count bounds how many frames go out per round (each
 * aggregated frame consumes one TXBD on the device).
 */
#ifdef WHC_TX_AGG
void whc_host_txagg_xmit(struct xmit_priv_t *xmit_priv)
{
	struct whc_sdio *priv = &whc_sdio_priv;
	struct whc_msg_node *p_node = NULL;	/* carried across rounds when the buffer fills mid-batch */
	struct sk_buff *pskb;
	u32 off, unit;
	u16 free_bd;
	u8 agg_num;
	INIC_TX_DESC *ptxdesc;

	/* wakeup device if it's in power save mode before send msg */
	if (priv->dev_state == PWR_STATE_SLEEP) {
		dev_dbg(&priv->func->dev, "%s: wakeup device", __func__);
		if (whc_sdio_host_resume_common(priv)) {
			dev_err(&priv->func->dev, "%s: fail to wakeup device, stop send", __func__);
			return;
		}
	}

	mutex_lock(&priv->lock);

	while ((!global_idev.mlme_priv.b_in_scan) &&
		   (!xmit_priv->flowctrl_en) &&
		   ((p_node != NULL) || (atomic_read(&xmit_priv->msg_num) > 0))) {

		/* the current free TXBD count bounds how many frames we aggregate this round,
		 * capped by the 8-bit bus_agg_num field the leading desc can carry */
		//rtw_sdio_query_txbd_status(priv);
		free_bd = whc_sdio_host_wait_txbd(priv);
		if (free_bd == 0) {
			break;	/* no TXBD: keep any held p_node, re-queue it below */
		}

		off = 0;
		agg_num = 0;

		while (agg_num < free_bd) {
			if (p_node == NULL) {
				p_node = whc_host_dequeue_tx_packet(xmit_priv);
				if (p_node == NULL) {
					break;	/* queue drained */
				}
			}
			pskb = p_node->msg;
			/* buf should always 4B aligned, hw drops useless data according to ptxdesc->txpktsize */
			unit = _RND(pskb->len, BUF_ALIGN_SZ);

			/* single frame larger than one bus transfer can never be sent: drop it */
			if (unit > priv->SdioTxMaxSZ) {
				dev_err(&priv->func->dev, "txagg: frame too large (%u), drop!\n", pskb->len);
				dev_kfree_skb(pskb);
				kfree(p_node);
				p_node = NULL;
				continue;
			}

			/* agg buffer can't fit this frame: flush what we have, keep p_node
			 * for the next round (off resets to 0, so it will fit then) */
			if (off + unit > priv->SdioTxMaxSZ) {
				break;
			}

			ptxdesc = (INIC_TX_DESC *)pskb->data;
			ptxdesc->txpktsize = pskb->len - SIZE_TX_DESC;
			ptxdesc->offset = SIZE_TX_DESC;
			ptxdesc->type = TX_PACKET_802_3;
			ptxdesc->bus_agg_num = 1;

			memcpy(priv->agg_buf + off, pskb->data, pskb->len);
			off += unit;
			agg_num++;

			dev_kfree_skb(pskb);
			kfree(p_node);
			p_node = NULL;
		}

		if (agg_num > 0) {
			/* the leading unit's desc carries the whole aggregate count */
			ptxdesc = (INIC_TX_DESC *)priv->agg_buf;
			ptxdesc->bus_agg_num = agg_num;

			rtw_write_port(priv, SDIO_TX_FIFO_DOMAIN_ID, off, priv->agg_buf);

			/* each aggregated frame consumes one TXBD on the device */
			priv->SdioTxBDFreeNum = (priv->SdioTxBDFreeNum > agg_num) ? (priv->SdioTxBDFreeNum - agg_num) : 0;
			priv->txbd_wptr = (priv->txbd_wptr + agg_num) % priv->txbd_size;
		}
	}

	mutex_unlock(&priv->lock);

	/* If we stopped on scan / flowctrl / no-TXBD with a frame still in hand,
	 * put it back at the head of the queue rather than dropping it. It will be
	 * resent when the tx thread is woken again (e.g. once scan completes). */
	if (p_node) {
		whc_host_requeue_tx_packet_head(xmit_priv, p_node);
	}

	whc_host_xmit_wake_tx_queue();
}
#endif /* WHC_TX_AGG */

struct hci_ops_t whc_sdio_host_intf_ops = {
	.send_data = whc_sdio_host_send_data,
	.recv_data = whc_sdio_host_recv_data,
};

