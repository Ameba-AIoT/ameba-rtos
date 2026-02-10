#include "rtw_whc_common.h"
#include "whc_sdio_host_app.h"

struct inic_sdio inic_sdio_priv = {0};
extern struct event_priv_t event_priv;

#define inic_host_rx_req_task_prio	2

#define WIFI_STACK_SIZE_RX_REQ_TASK (4096)

K_THREAD_STACK_DEFINE(inic_sdio_rx_req_stack, WIFI_STACK_SIZE_RX_REQ_TASK);

static struct k_thread ameba_host_rx_req_thread;

void (*tx_read_pkt_ptr)(void *pkt_addr, void *data, size_t length);
int (*rx_callback_ptr)(uint8_t idx, void *buffer, uint16_t len);

static void whc_bridge_sdio_host_init_drv(void)
{
	k_sem_init(&inic_sdio_priv.host_send, 1, SEMA_MAX_COUNT);
	k_sem_init(&(inic_sdio_priv.host_recv_wake), 0, SEMA_MAX_COUNT);
	k_sem_init(&(inic_sdio_priv.host_recv_done), 1, 1);
	k_sem_init(&(inic_sdio_priv.txbd_wq), 0, SEMA_MAX_COUNT);
	k_mutex_init(&inic_sdio_priv.lock);

	/* init event priv */
	k_sem_init(&(event_priv.api_ret_sema), 0, SEMA_MAX_COUNT);
	k_mutex_init(&(event_priv.send_mutex));


	k_thread_create(&ameba_host_rx_req_thread, inic_sdio_rx_req_stack,
					WIFI_STACK_SIZE_RX_REQ_TASK,
					(k_thread_entry_t)whc_bridge_sdio_host_recv_data_process, NULL, NULL, NULL,
					inic_host_rx_req_task_prio, K_USER,
					K_NO_WAIT);
}

/**
 * @brief  to initialize the inic device.
 * @param  none.
 * @return none.
 */
void whc_bridge_sdio_host_init(void)
{
	struct inic_sdio *priv = &inic_sdio_priv;

	if (rtw_sdio_init(priv) != TRUE) {
		printf("%s: initialize SDIO Failed!\n", __FUNCTION__);
		return;
	}

	/* init sdio */
	whc_bridge_sdio_host_init_drv();
}

void whc_bridge_sdio_host_send_data(uint8_t *buf, uint32_t len, void *pskb)
{
	uint32_t polling_num = 0;
	struct inic_sdio *priv = &inic_sdio_priv;
	struct INIC_TX_DESC *ptxdesc;

	(void) polling_num;
	(void) pskb;

	/* wakeup device if it's in power save mode before send msg */
	if (priv->dev_state == PWR_STATE_SLEEP) {
		printf("%s: wakeup device", __func__);
		//TODO wake
	}

	k_mutex_lock(&priv->lock, MUTEX_WAIT_TIMEOUT);

	// check if hardware tx fifo page is enough
	while (priv->SdioTxBDFreeNum < 1) {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		//TODO
		priv->tx_avail_int_triggered = 1;
		if (rtos_sema_take(&(priv->txbd_wq), 1000) != 0) {
			LOG_INF("take sem fail %s %d\r\n", __func__, __LINE__);
		} else {
			priv->tx_avail_int_triggered = 0;
		}
		rtw_sdio_query_txbd_status(priv);
#else
		polling_num++;
		if ((polling_num % 60) == 0) {
			k_sleep(K_MSEC(1));
		}

		// Total number of TXBD is NOT available, so update current TXBD status
		rtw_sdio_query_txbd_status(priv);
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

	rtw_write_port(priv, SDIO_TX_FIFO_DOMAIN_ID, len, buf);

	if (priv->SdioTxBDFreeNum > 0) {
		priv->SdioTxBDFreeNum -= 1;
	}

exit:
	k_mutex_unlock(&priv->lock);

	return;


}

void whc_bridge_sdio_host_recv_pkts(uint8_t *buf)
{
	uint8_t *ptr = buf + SIZE_RX_DESC;
	struct whc_msg_info *msg_info = (struct whc_msg_info *)ptr;
	char *data = (char *)(ptr + sizeof(struct whc_msg_info) + msg_info->pad_len);
	uint32_t len = msg_info->data_len;

	// direct up to zephyr network, free after done
	if (rx_callback_ptr) {
		rx_callback_ptr(msg_info->wlan_idx, data, len);
	}

	k_free(buf);
}

int whc_bridge_sdio_host_recv_process(uint8_t *pbuf)
{
	int ret = 0;
	uint32_t event = *(uint32_t *)(pbuf + SIZE_RX_DESC);

	switch (event) {
	case INIC_WIFI_EVT_RECV_PKTS:
		whc_bridge_sdio_host_recv_pkts(pbuf);
		break;

	default:
		whc_host_pkt_rx_to_user(pbuf);
		break;

	}

	return ret;
}

//TODO  check RD desc
static uint8_t *whc_bridge_sdio_host_read_rxfifo(struct inic_sdio *priv, uint32_t size)
{
	uint32_t allocsize, ret;
	uint32_t retry = 0;
	uint8_t *pbuf = NULL;

	allocsize = _RND(size, SDIO_BLOCK_SIZE);

	pbuf = k_malloc(allocsize);
	if (pbuf == NULL) {
		printf("%s: Alloc skb rx buf Err, alloc_sz %d!!\n\r", __func__, allocsize);
		return NULL;
	}

	while (1) {
		ret = sdio_read_port(priv, SDIO_RX_FIFO_DOMAIN_ID, size, pbuf);
		if (ret == TRUE) {
			break;
		} else {
			/* retry to reduce impact of bus err */
			if (retry++ > 10) {
				k_free(pbuf);
				printf("%s: read port FAIL!\n", __FUNCTION__);
				return NULL;
			};
		}
	}

	//print_hex_dump_bytes("whc_bridge_sdio_host_read_rxfifo: ", DUMP_PREFIX_NONE, rx_buf, size);

	return pbuf;
}

void whc_bridge_sdio_host_recv_data_process(void)
{
	struct inic_sdio *sdio_priv = &inic_sdio_priv;
	uint8_t tmp[4];
	uint8_t *pbuf;
	uint32_t rx_len_rdy, himr;
	uint16_t SdioRxFIFOSize;
	uint8_t retry = 0;

	for (;;)  {
		k_sem_take(&(inic_sdio_priv.host_recv_wake), MUTEX_WAIT_TIMEOUT);

		/* wakeup device if it's sleep */
		if (sdio_priv->dev_state == PWR_STATE_SLEEP) {
			printf("%s: wakeup device", __func__);
			//TODO resume
		}

		/* TODO disable RX_REQ interrupt */
		himr = sdio_priv->sdio_himr & (~SDIO_HIMR_RX_REQUEST_MSK);
		sdio_local_write(sdio_priv, SDIO_REG_HIMR, 4, (uint8_t *)&himr);

		do {
			/* validate RX_LEN_RDY before reading RX0_REQ_LEN */
			rx_len_rdy = sdio_read8(sdio_priv, SDIO_REG_RX0_REQ_LEN + 3) & BIT(7);

			if (rx_len_rdy) {
				sdio_local_read(sdio_priv, SDIO_REG_RX0_REQ_LEN, 4, tmp);
				SdioRxFIFOSize = (*(u16 *)tmp);

				if (SdioRxFIFOSize == 0) {
					if (retry ++ < 3) {
						continue;
					} else {
						break;
					}
				} else {
					retry = 0;
					pbuf = whc_bridge_sdio_host_read_rxfifo(sdio_priv, SdioRxFIFOSize);
					if (pbuf) {
						/* skip RX_DESC */
						whc_bridge_sdio_host_recv_process(pbuf);
					} else {
						break;
					}
				}
			} else {
				break;
			}
		} while (1);

		/* restore RX_REQ interrupt*/
		himr = (sdio_priv->sdio_himr);
		sdio_local_write(sdio_priv, SDIO_REG_HIMR, 4, (u8 *)&himr);
	}

}

