#include "rtw_whc_common.h"

struct whc_sdio whc_sdio_priv = {0};
int whc_host_init_done = 0;
#ifdef CONFIG_WHC_WIFI_API_PATH
extern struct event_priv_t event_priv;
#endif
#define WIFI_STACK_SIZE_API_TASK (4096)
#define WIFI_STACK_SIZE_RX_REQ_TASK (4096)
#define SDIO_POLLING_STACK_SIZE 1024

void dump_buf(char *info, uint8_t *buf, uint32_t len)
{
	printf("%s", info);
	for (uint32_t i = 0; i < len; i++) {
		printf("%s0x%02X%s", i % 16 == 0 ? "\n     " : ",", buf[i], i == len - 1 ? "\n" : "");
	}
}

/* compile in fullhan sdk now */
#ifdef TODO
void whc_sdio_host_rx_handler(uint8_t *buf)
{
	uint8_t *ptr = buf + SIZE_RX_DESC;
	struct whc_msg_info *msg_info = (struct whc_msg_info *)ptr;
	char *data = (char *)(ptr + sizeof(struct whc_msg_info) + msg_info->pad_len);
	//uint32_t len = msg_info->data_len;
	struct pbuf *temp_buf = 0;
	struct pbuf *p_buf;

retry:
	p_buf = pbuf_alloc(PBUF_RAW, msg_info->data_len, PBUF_POOL);

	if (p_buf == NULL) {
		buf_counter++;
		if (buf_counter >= 1000) {
			printf("%s: Alloc skb rx buf Err conuter %d \n", __func__, buf_counter);
			buf_counter = 0;
			rt_free(buf);
			return;
		}
		printf("%s: Alloc skb rx buf Err conuter %d \n", __func__, buf_counter);
		rt_thread_mdelay(1);
		goto retry;
	}

	/* cpoy data from skb(ipc data) to pbuf(ether net data) */
	temp_buf = p_buf;
	while (temp_buf) {
		/* If tot_len > PBUF_POOL_BUFSIZE_ALIGNED, the skb will be
		 * divided into several pbufs. Therefore, there is a while to
		 * use to assigne data to pbufs.
		 */

		memcpy(temp_buf->payload, data, temp_buf->len);
		data = data + temp_buf->len;
		temp_buf = temp_buf->next;
	}

	if (p_buf != NULL) {
		LwIP_ethernetif_recv_inic(msg_info->wlan_idx, p_buf);
	}

	rt_free(buf);
}
#endif

int sdio_recv_process(uint8_t *pbuf)
{
	int ret = 0;
	uint32_t event = *(uint32_t *)(pbuf + SIZE_RX_DESC);
	//dump_buf("sdio recv", pbuf + 16, 32);

#ifdef CONFIG_WHC_WIFI_API_PATH
	struct whc_api_info *ret_msg;
	int counter = 0;
#endif

	switch (event) {
	case WHC_WIFI_EVT_RECV_PKTS:
		whc_sdio_host_rx_handler(pbuf);
		break;
#ifdef CONFIG_WHC_WIFI_API_PATH
	case WHC_WIFI_EVT_API_CALL:
		while (event_priv.rx_api_msg) {
			vTaskDelay(1);
			counter ++;
			if (counter == 500) {
				counter = 0;
				printf("%s: waiting for last event \n", __func__);
			}
		}
		event_priv.rx_api_msg = pbuf;
		rtos_sema_give(event_priv.task_wake_sema);
		break;
	case WHC_WIFI_EVT_API_RETURN:
		if (event_priv.b_waiting_for_ret) {
			while (event_priv.rx_ret_msg) {
				vTaskDelay(1);
				counter ++;
				if (counter == 500) {
					counter = 0;
					printf("%s: waiting for last event \n", __func__);
				}
			}
			event_priv.rx_ret_msg = pbuf;

			/* unblock API calling func */
			rtos_sema_give(event_priv.api_ret_sema);
		} else {
			ret_msg = (struct whc_api_info *)(pbuf + SIZE_RX_DESC);
			printf("too late to receive API ret, ID: 0x%x!\n", ret_msg->api_id);

			/* free rx buffer */
			rt_free(pbuf);
		}
		break;
#endif

#ifdef CONFIG_WHC_CMD_PATH
	default:
		whc_host_pkt_rx_to_user(pbuf + SIZE_RX_DESC);
		rt_free(pbuf);
		break;
#else
	default:
		printf("%s: unknown event:%d\n", __func__, event);
		rt_free(pbuf);
		break;
#endif
	}
	return ret;
}

//TODO  check RD desc
static uint8_t *sdio_read_rxfifo(struct whc_sdio *priv, uint32_t size)
{
	uint32_t allocsize, ret;
	uint32_t retry = 0;
	uint8_t *pbuf = NULL;

	allocsize = _RND(size, SDIO_BLOCK_SIZE);

	pbuf = (uint8_t *)rt_malloc(allocsize);
	if (pbuf == NULL) {
		printf("%s: Alloc skb rx buf Err, alloc_sz %d !!\n\r", __func__, (int)allocsize);
		return NULL;
	}

	while (1) {
		ret = sdio_read_port(priv, SDIO_RX_FIFO_DOMAIN_ID, size, pbuf);
		if (ret == TRUE) {
			break;
		} else {
			/* retry to reduce impact of bus err */
			if (retry++ > 10) {
				rt_free(pbuf);
				printf("%s: read port FAIL!\n", __FUNCTION__);
				return NULL;
			};
		}
	}

	return pbuf;
}

void rtw_sdio_recv_data_process(void)
{
	struct whc_sdio *sdio_priv = &whc_sdio_priv;
	uint8_t tmp[4];
	uint8_t *pbuf;
	uint32_t rx_len_rdy, himr;
	uint16_t SdioRxFIFOSize;
	uint8_t retry = 0;
	(void)himr;

	for (;;)  {
		rtos_sema_take(whc_sdio_priv.host_recv_wake, MUTEX_WAIT_TIMEOUT);

		/* wakeup device if it's sleep */
		if (sdio_priv->dev_state == PWR_STATE_SLEEP) {
			printf("%s: wakeup device", __func__);
			//TODO resume
		}
#if defined(WHC_SDIO_INT_MODE) && !defined(WHC_SDIO_INT_GPIO)
		/* TODO disable RX_REQ interrupt */
		himr = sdio_priv->sdio_himr & (~SDIO_HIMR_RX_REQUEST_MSK);
		sdio_local_write(sdio_priv, SDIO_REG_HIMR, 4, (uint8_t *)&himr);
#endif

		do {
			/* validate RX_LEN_RDY before reading RX0_REQ_LEN */
			rx_len_rdy = sdio_read8(sdio_priv, SDIO_REG_RX0_REQ_LEN + 3) & BIT(7);

			if (rx_len_rdy) {
				sdio_local_read(sdio_priv, SDIO_REG_RX0_REQ_LEN, 4, tmp);
				SdioRxFIFOSize = (*(uint16_t *)tmp);
				if (SdioRxFIFOSize == 0) {
					if (retry ++ < 3) {
						continue;
					} else {
						break;
					}
				} else {
					retry = 0;
					pbuf = sdio_read_rxfifo(sdio_priv, SdioRxFIFOSize);
					if (pbuf) {
						/* skip RX_DESC */
						sdio_recv_process(pbuf);
					} else {
						break;
					}
				}
			} else {
				break;
			}
		} while (1);
#if defined(WHC_SDIO_INT_MODE) && !defined(WHC_SDIO_INT_GPIO)
		/* restore RX_REQ interrupt*/
		himr = (sdio_priv->sdio_himr);
		sdio_local_write(sdio_priv, SDIO_REG_HIMR, 4, (uint8_t *)&himr);
#endif
	}

}
extern void rtw_sdio_interrupt_handler(void);
extern void sdio_polling_task(void *arg1, void *arg2, void *arg3);

void whc_sdio_host_init_drv(void)
{
	int ret;
	ret = rtos_sema_create(&whc_sdio_priv.host_send, 1, SEMA_MAX_COUNT);
	ret += rtos_sema_create(&(whc_sdio_priv.host_irq), 0, SEMA_MAX_COUNT);
	ret += rtos_sema_create(&whc_sdio_priv.host_send_block_sema, 0, SEMA_MAX_COUNT);
	ret += rtos_sema_create(&(whc_sdio_priv.host_recv_wake), 0, SEMA_MAX_COUNT);

	/* should higher than polling, polling 7 */
	if (rtos_task_create(NULL, ((const char *)"rtw_sdio_recv_data_process"), (rtos_task_function_t)rtw_sdio_recv_data_process, NULL, WIFI_STACK_SIZE_RX_REQ_TASK,
						 0 + 6) != 0) {
		printf("create rtw_sdio_recv_data_process fail \n");
	}

#ifdef CONFIG_WHC_WIFI_API_PATH
	rtos_sema_create(&(event_priv.task_wake_sema), 0, SEMA_MAX_COUNT);

	if (rtos_task_create(NULL, ((const char *)"whc_sdio_api_task"), (rtos_task_function_t)whc_host_api_task, NULL, WIFI_STACK_SIZE_API_TASK, 3) != 0) {
		printf("create whc_host_api_task fail \n");
	}

#endif

	if (rtos_task_create(NULL, ((const char *)"sdioPollingTask"), (rtos_task_function_t)sdio_polling_task, NULL, SDIO_POLLING_STACK_SIZE, 7) != 0) {
		printf("%s(), fail to create sdioPollingTask \r\n", __func__);
	}

}

/**
 * @brief  to initialize the inic device.
 * @param  none.
 * @return none.
 */
void whc_host_init(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;

	if (whc_host_init_done == 1) {
		return;
	}

	if (rtw_sdio_init(priv) != TRUE) {
		printf("%s: initialize SDIO Failed!\n", __FUNCTION__);
		return;
	}

	/* init sdio */
	whc_sdio_host_init_drv();

	LwIP_Init();

	whc_host_init_done = 1;

	/* tell dev host type rtos */
	whc_host_set_host();
	/* init wifi when sdio done */
	whc_host_wifi_on();
}

void rtw_sdio_send_data(uint8_t *buf, uint32_t len, void *pskb)
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

	rtos_mutex_take(priv->lock, MUTEX_WAIT_TIMEOUT);
	// check if hardware tx fifo page is enough
	while (priv->SdioTxBDFreeNum < 1) {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		//TODO
		if (polling_num ++ > 0) {
			priv->tx_avail_int_triggered = 1;
			if (rtos_sema_take(priv->txbd_wq, 1000) != 0) {
				goto exit;
			}
		}

		rtw_sdio_query_txbd_status(priv);
#else
		rtw_sdio_query_txbd_status(priv);

		polling_num++;
		if ((polling_num % 60) == 0) {

			rt_thread_mdelay(1);
		}

		// Total number of TXBD is NOT available, so update current TXBD status
		rtw_sdio_query_txbd_status(priv);
#endif
	}

	if (len > priv->SdioTxMaxSZ) {
		printf("%s: PKT SIZE ERROR, total size: %d\n", __FUNCTION__, (int)len);
		goto exit;
	}

	ptxdesc = (struct INIC_TX_DESC *)buf;
	ptxdesc->txpktsize = len - SIZE_TX_DESC;
	ptxdesc->offset = SIZE_TX_DESC;
	ptxdesc->type = TX_PACKET_802_3;
	ptxdesc->bus_agg_num = 1;

	//dump_buf("sdio send", buf + SIZE_TX_DESC, 32);
	//printf("send counter %d size %d buf %x\r\n", ++counter, len - SIZE_TX_DESC - sizeof(struct whc_msg_info), buf);

	rtw_write_port(priv, SDIO_TX_FIFO_DOMAIN_ID, len, buf);

	if (priv->SdioTxBDFreeNum > 0) {
		priv->SdioTxBDFreeNum -= 1;
	}

	priv->txbd_wptr = (priv->txbd_wptr + 1) % priv->txbd_size;
exit:
	rtos_mutex_give(priv->lock);

	return;


}

