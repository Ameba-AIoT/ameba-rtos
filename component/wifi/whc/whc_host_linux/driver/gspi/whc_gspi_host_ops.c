// SPDX-License-Identifier: GPL-2.0-only
/*
 * Realtek wireless local area network IC driver.
 *
 * Copyright(c) 2024 Realtek Corporation. All rights reserved.
 *
 * WHC data path over GSPI. Mirrors the SDIO ops: flow control by free TX BD
 * on send, RX-length-then-read on receive. The framing (CMD/data/Status) lives
 * in whc_gspi_host_drvio.c.
 */
#include <whc_host_linux.h>

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

/* Block until at least one TXBD is free. Returns the free count, or 0 on timeout.
 * Caller must hold priv->lock. */
static u8 whc_gspi_host_wait_txbd(struct whc_gspi *priv)
{
	u32 polling_num = 0;

	(void) polling_num;

	// check if hardware tx fifo page is enough
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

	(void)pskb;
	INIC_TX_DESC *ptxdesc;

	mutex_lock(&priv->lock);

	/* block until at least one TXBD is free */
	if (whc_gspi_host_wait_txbd(priv) == 0) {
		goto exit;
	}

	if (len > priv->GspiTxMaxSZ) {
		dev_err(&priv->spi_dev->dev, "%s: PKT SIZE ERROR, total size: %d\n", __func__, len);
		goto exit;
	}

	ptxdesc = (INIC_TX_DESC *)buf;
	ptxdesc->txpktsize = len;
	ptxdesc->offset = SIZE_TX_DESC;
	ptxdesc->type = TX_PACKET_802_3;
	ptxdesc->bus_agg_num = 1;

	gspi_write_port(priv, buf, len + SIZE_TX_DESC);

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
	struct sk_buff *pskb;
	u32 rx_len;

	do {
		rx_len = rtw_gspi_get_rx_len(priv);
		if (rx_len == 0) {
			break;
		}
		pskb = whc_gspi_host_read_rxfifo(priv, rx_len);
		if (pskb == NULL) {
			break;
		}

		whc_host_recv_dispatch(pskb);
	} while (1);

	/* re-enable the RX_REQUEST interrupt the ISR masked (full shadow rewrite) */
	gspi_write32(priv, GSPI_REG_HIMR, priv->gspi_himr);
}

struct hci_ops_t whc_gspi_host_intf_ops = {
	.send_data = whc_gspi_host_send_data,
	.recv_data = whc_gspi_host_recv_data,
};
