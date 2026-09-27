// SPDX-License-Identifier: GPL-2.0-only
/*
 * Realtek wireless local area network IC driver.
 *
 * Copyright(c) 2024 Realtek Corporation. All rights reserved.
 *
 */
#include <whc_host_linux.h>

#ifndef WHC_RX_AGG
static struct sk_buff *whc_gspi_host_read_rxfifo(struct whc_gspi *priv, u32 rx_len)
{
	struct sk_buff *pskb;
	u32 frame_len = GSPI_CMD_PHASE_LEN + GSPI_ALIGN4(rx_len) + GSPI_STATUS_PHASE_LEN;

	pskb = netdev_alloc_skb(NULL, frame_len);
	if (pskb == NULL) {
		dev_err(global_idev.pwhc_dev, "%s: alloc rx skb (%d) failed\n", __func__, frame_len);
		return NULL;
	}

	/* read into the raw head (which still holds the CMD-dummy slot) */
	if (gspi_read_port(priv, pskb->data, rx_len) == false) {
		kfree_skb(pskb);
		dev_err(&priv->spi_dev->dev, "%s: read port FAIL\n", __func__);
		return NULL;
	}

	/* drop the leading 4-byte CMD dummy; ->data now points at the payload,
	 * ->len is still 0, matching the SDIO contract */
	skb_reserve(pskb, GSPI_CMD_PHASE_LEN);

	return pskb;
}
#else

static u8 *whc_gspi_host_read_rxfifo_agg(struct whc_gspi *priv, u32 rx_len)
{
	u32 frame_len = GSPI_CMD_PHASE_LEN + GSPI_ALIGN4(rx_len) + GSPI_STATUS_PHASE_LEN;
	u8 *buf;

	buf = kmalloc(frame_len, GFP_ATOMIC);
	if (buf == NULL) {
		dev_err(&priv->spi_dev->dev, "%s: alloc rx buf (%d) failed\n", __func__, frame_len);
		return NULL;
	}

	if (gspi_read_port(priv, buf, rx_len) == false) {
		kfree(buf);
		dev_err(&priv->spi_dev->dev, "%s: read port FAIL\n", __func__);
		return NULL;
	}

	return buf;
}

/*
 * 4B-aligned size (incl. RX_DESC) of the aggregated RX segment at @ptr, derived
 * from its header via union whc_hdr. Returns 0 when the segment can't be sized
 * (unknown event, or BT which is not de-aggregated) so the caller stops walking
 * instead of misparsing.
 */
static u32 whc_gspi_host_rxagg_segment_size(u8 *ptr)
{
	union whc_hdr *hdr = (union whc_hdr *)(ptr + SIZE_RX_DESC);
	u32 body;

	switch (hdr->msg.event) {
	case WHC_WIFI_EVT_RECV_PKTS:
		body = sizeof(struct whc_msg_info) + hdr->msg.data_len + hdr->msg.pad_len;
		break;
	case WHC_WIFI_EVT_API_CALL:
	case WHC_WIFI_EVT_API_RETURN:
		body = sizeof(struct whc_api_info) + hdr->api.data_len;
		break;
	case WHC_WIFI_EVT_CMD:
		body = sizeof(struct whc_cmd_path_hdr) + hdr->cmd.len;
		break;
	default:
		return 0;
	}

	body += SIZE_RX_DESC;
	return _RND4(body);
}

/*
 * Walk one hardware-aggregated rx buffer: split it into per-packet segments by
 * header, copy each into its own skb and dispatch it, then free the buffer.
 */
static void whc_gspi_host_rxagg_dispatch_buf(struct whc_gspi *priv, u8 *buf, u32 total)
{
	struct sk_buff *pskb;
	u8 *payload = buf + GSPI_CMD_PHASE_LEN;
	u8 *ptr = payload;
	u32 size;

	while (1) {
		size = whc_gspi_host_rxagg_segment_size(ptr);
		if (size == 0) {
			dev_dbg(&priv->spi_dev->dev, "agg evt err %x", *(u32 *)(ptr + SIZE_RX_DESC));
			break;
		}

		/* guard against a corrupt hdr: the segment must fit in the rx data */
		if ((u32)(ptr - payload) + size > total) {
			dev_err(&priv->spi_dev->dev, "agg size err: size %d off %d total %d",
					size, (int)(ptr - payload), total);
			break;
		}

		pskb = netdev_alloc_skb(NULL, size);
		if (pskb == NULL) {
			dev_err(&priv->spi_dev->dev, "agg alloc rx skb fail, size %d", size);
			break;
		}
		memcpy(pskb->data, ptr, size);

		whc_host_recv_dispatch(pskb);
		ptr += size;

		/* stop once too little rx data remains to hold even the smallest
		 * header (event + len): the rest is trailing alignment padding */
		if ((u32)(ptr - payload) + 8 > total) {
			break;
		}
	}

	kfree(buf);
}
#endif /* WHC_RX_AGG */

/* Block until at least one TXBD is free. Returns the free count, or 0 on timeout.
 * Caller must hold priv->lock. */
static u8 whc_gspi_host_wait_txbd(struct whc_gspi *priv)
{
	u32 polling_num = 0;

	(void) polling_num;

	while (priv->GspiTxBDFreeNum < 1) {
#ifdef CONFIG_GSPI_TX_ENABLE_AVAL_INT
		/* Reset flag before query so any ISR firing after this point is
		 * guaranteed to be caught by wait_event_timeout's condition check.
		 * Should clear tx_avail_int_triggered each loop: jira: https://jira.realtek.com/browse/RSWLANDIOT-11649,
		 * but can't put it after rtw_sdio_query_txbd_status considering lost-wakeup race */
		priv->tx_avail_int_triggered = 0;
		rtw_gspi_query_txbd_status(priv);
		if (priv->GspiTxBDFreeNum >= 1) {
			break;
		}
		if (!wait_event_timeout(priv->txbd_wq, priv->tx_avail_int_triggered == 1, msecs_to_jiffies(100))) {
			dev_err(&priv->spi_dev->dev, "%s: TXBD unavailable, TX FAIL\n", __func__);
			return 0;
		}
#else
		polling_num++;
		if ((polling_num % 60) == 0) {
			msleep(1);
		}

		// Total number of TXBD is NOT available, so update current TXBD status
		rtw_gspi_query_txbd_status(priv);
#endif
	}

	return priv->GspiTxBDFreeNum;
}

void whc_gspi_host_send_data(u8 *buf, u32 len, struct sk_buff *pskb)
{
	struct whc_gspi *priv = &whc_gspi_priv;
	INIC_TX_DESC *ptxdesc;

	(void)pskb;

	/* wake the device if it is clock-gated before sending */
	if (priv->dev_state == PWR_STATE_SLEEP) {
		if (whc_gspi_host_resume_common(priv)) {
			dev_err(&priv->spi_dev->dev, "%s: fail to wakeup device, stop send\n", __func__);
			return;
		}
	}

	mutex_lock(&priv->lock);

	if (whc_gspi_host_wait_txbd(priv) == 0) {
		goto exit;
	}

	if (len > priv->GspiTxMaxSZ) {
		dev_err(&priv->spi_dev->dev, "%s: PKT SIZE ERROR, total size: %d\n", __func__, len);
		goto exit;
	}

	ptxdesc = (INIC_TX_DESC *)buf;
	ptxdesc->txpktsize = len - SIZE_TX_DESC;
	ptxdesc->offset = SIZE_TX_DESC;
	ptxdesc->type = TX_PACKET_802_3;
	ptxdesc->bus_agg_num = 1;

	gspi_write_port(priv, buf, len);

	if (priv->GspiTxBDFreeNum > 0) {
		priv->GspiTxBDFreeNum -= 1;
	}

	/* update txbd num in sw after send one pkt */
	priv->txbd_wptr = (priv->txbd_wptr + 1) % priv->txbd_size;

exit:
	mutex_unlock(&priv->lock);
}

static void whc_gspi_host_recv_data(void *intf_priv)
{
	struct whc_gspi *priv = (struct whc_gspi *)intf_priv;
	u32 rx_len;
#ifdef WHC_RX_AGG
	u8 *buf;
#else
	struct sk_buff *pskb;
#endif

	/* wake the device if it is clock-gated before reading */
	if (priv->dev_state == PWR_STATE_SLEEP) {
		if (whc_gspi_host_resume_common(priv)) {
			dev_err(&priv->spi_dev->dev, "%s: fail to wakeup device, stop recv\n", __func__);
			return;
		}
	}

	do {
		rx_len = rtw_gspi_get_rx_len(priv);
		if (rx_len == 0) {
			break;
		}
#ifdef WHC_RX_AGG
		buf = whc_gspi_host_read_rxfifo_agg(priv, rx_len);
		if (buf == NULL) {
			break;
		}
		whc_gspi_host_rxagg_dispatch_buf(priv, buf, rx_len);
#else
		pskb = whc_gspi_host_read_rxfifo(priv, rx_len);
		if (pskb == NULL) {
			break;
		}

		whc_host_recv_dispatch(pskb);
#endif /* WHC_RX_AGG */
	} while (1);

	/* re-enable the RX_REQUEST interrupt the ISR masked (full shadow rewrite) */
	gspi_write32(priv, GSPI_REG_HIMR, priv->gspi_himr);
}

/*
 * Drain the tx queue: each round coalesces queued frames into one bus transfer.
 * The current free TXBD count bounds how many frames go out per round (each
 * aggregated frame consumes one TXBD on the device). Every packed unit is
 * 4-byte aligned; gspi_write_port() wraps the whole aggregate in one CMD/Status
 * frame.
 */
#ifdef WHC_TX_AGG
void whc_host_txagg_xmit(struct xmit_priv_t *xmit_priv)
{
	struct whc_gspi *priv = &whc_gspi_priv;
	struct whc_msg_node *p_node = NULL;	/* carried across rounds when the buffer fills mid-batch */
	struct sk_buff *pskb;
	u32 off, unit;
	u8 free_bd;
	u8 agg_num;
	INIC_TX_DESC *ptxdesc;

	/* wake the device if it is clock-gated before sending */
	if (priv->dev_state == PWR_STATE_SLEEP) {
		if (whc_gspi_host_resume_common(priv)) {
			dev_err(&priv->spi_dev->dev, "%s: fail to wakeup device, stop send\n", __func__);
			return;
		}
	}

	mutex_lock(&priv->lock);

	while ((!global_idev.mlme_priv.b_in_scan) &&
		   ((p_node != NULL) || (atomic_read(&xmit_priv->msg_num) > 0))) {

		free_bd = whc_gspi_host_wait_txbd(priv);
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
			/* each packed unit must be 4B aligned; hw drops the pad per txpktsize */
			unit = _RND(pskb->len, BUF_ALIGN_SZ);

			/* single frame larger than one bus transfer can never be sent: drop it */
			if (unit > priv->GspiTxMaxSZ) {
				dev_err(&priv->spi_dev->dev, "txagg: frame too large (%u), drop!\n", pskb->len);
				dev_kfree_skb(pskb);
				kfree(p_node);
				p_node = NULL;
				continue;
			}

			/* agg buffer can't fit this frame: flush what we have, keep p_node
			 * for the next round (off resets to 0, so it will fit then) */
			if (off + unit > priv->GspiTxMaxSZ) {
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

			gspi_write_port(priv, priv->agg_buf, off);

			/* each aggregated frame consumes one TXBD on the device */
			priv->GspiTxBDFreeNum = (priv->GspiTxBDFreeNum > agg_num) ? (priv->GspiTxBDFreeNum - agg_num) : 0;
			priv->txbd_wptr = (priv->txbd_wptr + agg_num) % priv->txbd_size;
		}
	}

	mutex_unlock(&priv->lock);

	/* If we stopped on scan / no-TXBD with a frame still in hand, put it back
	 * at the head of the queue rather than dropping it. */
	if (p_node) {
		whc_host_requeue_tx_packet_head(xmit_priv, p_node);
	}

	whc_host_xmit_wake_tx_queue();
}
#endif /* WHC_TX_AGG */

struct hci_ops_t whc_gspi_host_intf_ops = {
	.send_data = whc_gspi_host_send_data,
	.recv_data = whc_gspi_host_recv_data,
};
