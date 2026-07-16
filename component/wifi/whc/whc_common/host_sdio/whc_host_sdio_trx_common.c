#include "rtw_whc_common.h"
#include "whc_host_sdio_trx_common.h"

extern struct whc_sdio whc_sdio_priv;

/* read one packet out of the device RX FIFO, buffer returned to caller */
static uint8_t *whc_host_sdio_read_rxfifo(struct whc_sdio *priv, uint32_t size)
{
	uint32_t allocsize, ret;
	uint32_t retry = 0;
	uint8_t *pbuf = NULL;

	allocsize = _RND(size, SDIO_BLOCK_SIZE);

	pbuf = (uint8_t *)WHC_MALLOC(allocsize);
	if (pbuf == NULL) {
		return NULL;
	}

	while (1) {
		ret = sdio_read_port(priv, SDIO_RX_FIFO_DOMAIN_ID, size, pbuf);
		if (ret == TRUE) {
			break;
		} else {
			/* retry to reduce impact of bus err */
			if (retry++ > 10) {
				WHC_FREE(pbuf);
				return NULL;
			}
		}
	}

	return pbuf;
}

/* dispatch one received buffer by event type (per-port macros decide the
 * handler, RX-desc offset and buffer ownership) */
int whc_host_sdio_recv_process(uint8_t *pbuf)
{
	uint32_t event = *(uint32_t *)(pbuf + SIZE_RX_DESC);

	switch (event) {
	case WHC_WIFI_EVT_RECV_PKTS:
		whc_host_sdio_recv_pkts(pbuf);
		break;

	default:
		WHC_HOST_SDIO_RX_DEFAULT(pbuf);
		break;
	}

	return 0;
}

void whc_host_sdio_recv_data_process(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;
	uint8_t tmp[4];
	uint8_t *pbuf;
	uint32_t rx_len_rdy;
	uint16_t SdioRxFIFOSize;
	uint8_t retry = 0;

	for (;;) {
		WHC_SEM_TAKE(priv->host_recv_wake);

		/* wakeup device if it's sleep */
		if (priv->dev_state == PWR_STATE_SLEEP) {
			//TODO resume
		}

		WHC_HOST_SDIO_RX_INT_DISABLE(priv);

		do {
			/* validate RX_LEN_RDY before reading RX0_REQ_LEN */
			rx_len_rdy = sdio_read8(priv, SDIO_REG_RX0_REQ_LEN + 3) & BIT(7);

			if (rx_len_rdy) {
				sdio_local_read(priv, SDIO_REG_RX0_REQ_LEN, 4, tmp);
				SdioRxFIFOSize = (*(uint16_t *)tmp);

				if (SdioRxFIFOSize == 0) {
					if (retry++ < 3) {
						continue;
					} else {
						break;
					}
				} else {
					retry = 0;
					pbuf = whc_host_sdio_read_rxfifo(priv, SdioRxFIFOSize);
					if (pbuf) {
						/* buffer starts with RX_DESC */
						whc_host_sdio_recv_process(pbuf);
					} else {
						break;
					}
				}
			} else {
				break;
			}
		} while (1);

		WHC_HOST_SDIO_RX_INT_RESTORE(priv);
	}
}

/* one pass of HISR handling, shared by all ports' interrupt handlers */
void whc_host_sdio_isr_process(struct whc_sdio *priv)
{
	uint8_t data[4];

	/* read HISR */
	sdio_local_read(priv, SDIO_REG_HISR, 4, data);
	priv->sdio_hisr = (*(uint32_t *)data);

	if (priv->sdio_hisr & priv->sdio_himr) {
		priv->sdio_hisr &= priv->sdio_himr;

		/* clear serviced HISR bits (port decides, may be a no-op) */
		WHC_HOST_SDIO_HISR_CLEAR(priv);

#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		if (priv->sdio_hisr & SDIO_HISR_AVAL_INT) {
			uint32_t freepage;
			/* for DP bug, read txbd to clear aval int */
			sdio_local_read(priv, SDIO_REG_FREE_TXBD_NUM, 4, (uint8_t *)&freepage);
			(void)freepage;
			/* wakeup tx task if waiting */
			if (priv->tx_avail_int_triggered == 0) {
				priv->tx_avail_int_triggered = 1;
				WHC_SEM_GIVE(priv->txbd_wq);
			}
		}
#endif
		if (priv->sdio_hisr & SDIO_HISR_RX_REQUEST) {
			priv->sdio_hisr ^= SDIO_HISR_RX_REQUEST;
			WHC_SEM_GIVE(priv->host_recv_wake);
		}
	}
}

/* Shared SDIO host TX: wait for a free TXBD, fill the TX descriptor and push
 * the buffer into the device TX FIFO. Additional required port macros:
 *   - WHC_MUTEX_TAKE(m, t) / WHC_MUTEX_GIVE(m)
 *   - WHC_SEM_TAKE_TIMEOUT(s, t)   : timed wait on the TXBD-avail sema object
 *   - WHC_MSLEEP(ms)
 * plus rtw_sdio_query_txbd_status() and hal rtw_write_port(). */
void whc_host_sdio_send_data(uint8_t *buf, uint32_t len, void *pskb)
{
	uint32_t polling_num = 0;
	struct whc_sdio *priv = &whc_sdio_priv;
	struct INIC_TX_DESC *ptxdesc;

	(void) polling_num;
	(void) pskb;

	/* wakeup device if it's in power save mode before send msg */
	if (priv->dev_state == PWR_STATE_SLEEP) {
		printf("%s: wakeup device", __func__);
		//TODO wake
	}

	WHC_MUTEX_TAKE(priv->lock, MUTEX_WAIT_TIMEOUT);
	// check if hardware tx fifo page is enough
	while (priv->SdioTxBDFreeNum < 1) {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		//TODO
		priv->tx_avail_int_triggered = 0;
		rtw_sdio_query_txbd_status(priv);
		if (priv->SdioTxBDFreeNum >= 1) {
			break;
		}
		if (WHC_SEM_TAKE_TIMEOUT(priv->txbd_wq, 1000) != 0) {
			printf("txbd unavaliable \n");
		}
#else
		// Total number of TXBD is NOT available, so update current TXBD status
		rtw_sdio_query_txbd_status(priv);
		if (priv->SdioTxBDFreeNum >= 1) {
			break;
		}
		polling_num++;
		if ((polling_num % 60) == 0) {
			WHC_MSLEEP(1);
		}
#endif
	}

	if (len > priv->SdioTxMaxSZ) {
		printf("%s: PKT SIZE ERROR, total size: %d\n", __FUNCTION__, len);
		goto exit;
	}

	ptxdesc = (struct INIC_TX_DESC *)buf;
	ptxdesc->txpktsize = len - SIZE_TX_DESC;
	ptxdesc->offset = SIZE_TX_DESC;
	ptxdesc->type = TX_PACKET_802_3;
	ptxdesc->bus_agg_num = 1;

	//dump_buf("sdio send", buf, 32);

	rtw_write_port(priv, SDIO_TX_FIFO_DOMAIN_ID, len, buf);

	if (priv->SdioTxBDFreeNum > 0) {
		priv->SdioTxBDFreeNum -= 1;
	}
	priv->txbd_wptr = (priv->txbd_wptr + 1) % priv->txbd_size;

exit:
	WHC_MUTEX_GIVE(priv->lock);

	return;
}
