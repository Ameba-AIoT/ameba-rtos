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

	pbuf = (uint8_t *)whc_malloc(allocsize);
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
				whc_free(pbuf);
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
		whc_host_sdio_rx_default(pbuf);
		break;
	}

	return 0;
}

void whc_host_sdio_recv_data_process(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;
	uint8_t *pbuf;
	uint32_t rx_len_rdy;
	uint32_t SdioRxFIFOSize;
	uint8_t retry = 0;
	uint32_t himr;

	for (;;) {
		retry = 0;
		whc_sem_take_timeout(priv->host_recv_wake, MUTEX_WAIT_TIMEOUT);

		/* wakeup device if it's sleep */
		if (priv->dev_state == PWR_STATE_SLEEP) {
			//TODO resume
		}

		/* no need to close data1 irq if using gpio as int src */
#if defined(WHC_SDIO_INT_MODE) && !defined(WHC_SDIO_INT_GPIO)
		himr = priv->sdio_himr & (~SDIO_HIMR_RX_REQUEST_MSK);
		rtw_write32(priv, SDIO_REG_HIMR, himr);
#endif

		do {
			/* validate RX_LEN_RDY before reading RX0_REQ_LEN */
			rx_len_rdy = rtw_read8(priv, SDIO_REG_RX0_REQ_LEN + 3) & BIT(7);

			if (rx_len_rdy) {
				SdioRxFIFOSize = rtw_read32(priv, SDIO_REG_RX0_REQ_LEN) & SDIO_RX_REQ_LEN_MSK;

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
#if defined(WHC_SDIO_INT_MODE) && !defined(WHC_SDIO_INT_GPIO)
		himr = priv->sdio_himr;
		rtw_write32(priv, SDIO_REG_HIMR, himr);
#endif
	}
}

/* one pass of HISR handling, shared by all ports' interrupt handlers */
void whc_host_sdio_isr_process(struct whc_sdio *priv)
{
	u32 value;
	/* read HISR */
	priv->sdio_hisr = rtw_read32(priv, SDIO_REG_HISR);

	if (priv->sdio_hisr & priv->sdio_himr) {
		priv->sdio_hisr &= priv->sdio_himr;

#if defined(WHC_SDIO_INT_MODE) && !defined(WHC_SDIO_INT_GPIO)
		value = priv->sdio_hisr & MASK_SDIO_HISR_CLEAR;
		if (value) {
			rtw_write32(priv, SDIO_REG_HISR, value);
		}
#endif

#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		if (priv->sdio_hisr & SDIO_HISR_AVAL_INT) {
			uint32_t freepage;
			/* for DP bug, read txbd to clear aval int */
			freepage = rtw_read32(priv, SDIO_REG_FREE_TXBD_NUM);
			/* wakeup tx task if waiting */
			if (priv->tx_avail_int_triggered == 0) {
				priv->tx_avail_int_triggered = 1;
				whc_sem_give(priv->txbd_wq);
			}
		}
#endif
		if (priv->sdio_hisr & SDIO_HISR_RX_REQUEST) {
			priv->sdio_hisr ^= SDIO_HISR_RX_REQUEST;
			whc_sem_give(priv->host_recv_wake);
		}
	}
}

/* Shared SDIO host TX: wait for a free TXBD, fill the TX descriptor and push
 * the buffer into the device TX FIFO. Additional required port macros:
 *   - whc_mutex_take(m, t) / whc_mutex_give(m)
 *   - whc_sem_take_timeout(s, t)   : timed wait on the TXBD-avail sema object
 *   - whc_msleep(ms)
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

	whc_mutex_take(priv->lock, MUTEX_WAIT_TIMEOUT);
	// check if hardware tx fifo page is enough
	while (priv->SdioTxBDFreeNum < 1) {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		//TODO
		priv->tx_avail_int_triggered = 0;
		rtw_sdio_query_txbd_status(priv);
		if (priv->SdioTxBDFreeNum >= 1) {
			break;
		}
		if (whc_sem_take_timeout(priv->txbd_wq, 1000) != 0) {
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
			whc_msleep(1);
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
	whc_mutex_give(priv->lock);

	return;
}
