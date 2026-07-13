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
			if (priv->tx_avail_int_triggered == 1) {
				WHC_SEM_GIVE(priv->txbd_wq);
				priv->tx_avail_int_triggered = 0;
			}
		}
#endif
		if (priv->sdio_hisr & SDIO_HISR_RX_REQUEST) {
			priv->sdio_hisr ^= SDIO_HISR_RX_REQUEST;
			WHC_SEM_GIVE(priv->host_recv_wake);
		}
	}
}
