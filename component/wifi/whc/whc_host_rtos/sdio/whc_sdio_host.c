#include "whc_host.h"

struct whc_sdio whc_sdio_priv = {0};

extern int whc_host_init_done;
extern rtos_mutex_t sdio_lock;

void rtw_sdio_interrupt_handler(void);
void rtw_sdio_polling_task(void *arg1, void *arg2, void *arg3);

//TODO  check RD desc
static uint8_t *sdio_read_rxfifo(struct whc_sdio *priv, uint32_t size)
{
	uint32_t allocsize, ret;
	uint32_t retry = 0;
	uint8_t *pbuf = NULL;

	allocsize = _RND(size, SDIO_BLOCK_SIZE);

	pbuf = (uint8_t *)rtos_mem_zmalloc(allocsize);
	if (pbuf == NULL) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: Alloc skb rx buf Errs!!\n\r", __func__);
		return NULL;
	}

	while (1) {
		ret = sdio_read_port(priv, SDIO_RX_FIFO_DOMAIN_ID, size, pbuf);
		if (ret == RTK_SUCCESS) {
			break;
		} else {
			/* retry to reduce impact of bus err */
			if (retry++ > 10) {
				rtos_mem_free(pbuf);
				RTK_LOGE(TAG_WLAN_INIC, "%s: read port FAIL!\n", __func__);
				return NULL;
			};
		}
	}

	return pbuf;
}

void whc_sdio_recv_data_process(void)
{
	struct whc_sdio *sdio_priv = &whc_sdio_priv;
	uint8_t *pbuf;
	uint32_t rx_len_rdy, himr;
	uint32_t SdioRxFIFOSize;
	uint8_t retry = 0;


	for (;;)  {
		rtos_sema_take(whc_sdio_priv.host_recv_wake, MUTEX_WAIT_TIMEOUT);

		/* wakeup device if it's sleep */
		if (sdio_priv->dev_state == PWR_STATE_SLEEP) {
			RTK_LOGE(TAG_WLAN_INIC, "%s: wakeup device", __func__);
			//TODO resume
		}

		/* TODO disable RX_REQ interrupt */
		himr = sdio_priv->sdio_himr & (~SDIO_HIMR_RX_REQUEST_MSK);
		rtw_write32(sdio_priv, SDIO_REG_HIMR, himr);

		do {
			/* validate RX_LEN_RDY before reading RX0_REQ_LEN */
			rx_len_rdy = rtw_read8(sdio_priv, SDIO_REG_RX0_REQ_LEN + 3) & BIT(7);

			if (rx_len_rdy) {
				SdioRxFIFOSize = rtw_read32(sdio_priv, SDIO_REG_RX0_REQ_LEN) & SDIO_RX_REQ_LEN_MSK;
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
						/* SDIO carries pkt_len (payload after RX_DESC) in the RX descriptor */
						whc_host_recv_dispatch(pbuf, *(u16 *)pbuf);
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
		rtw_write32(sdio_priv, SDIO_REG_HIMR, himr);
	}

}

void whc_sdio_host_init_drv(void)
{
	//RTK_LOGE(TAG_WLAN_INIC,"init sdio sema \r\n");
	rtos_mutex_create(&whc_sdio_priv.host_send);
	rtos_sema_create(&whc_sdio_priv.host_irq, 0, 0xFFFFFFFF);
	rtos_sema_create(&(whc_sdio_priv.host_recv_wake), 0, 0xFFFFFFFF);
	rtos_sema_create(&(whc_sdio_priv.host_recv_done), 1, 1);
	rtos_sema_create(&(whc_sdio_priv.txbd_wq), 0, 0xFFFFFFFF);
	rtos_mutex_create(&whc_sdio_priv.lock);

	/* should be higher than polling, polling 7 */
	if (rtos_task_create(NULL, ((const char *)"whc_sdio_rx"), (rtos_task_function_t)whc_sdio_recv_data_process, NULL, WIFI_STACK_SIZE_RX_REQ_TASK,
						 0 + 6) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "create whc_sdio_rx fail \n");
	}

#ifdef CONFIG_WHC_WIFI_API_PATH
	whc_host_api_init();
#endif

#ifndef SDIO_INT_MODE
	if (rtos_task_create(NULL, ((const char *)"sdioPollingTask"), (rtos_task_function_t)rtw_sdio_polling_task, NULL, SDIO_POLLING_STACK_SIZE,
						 0 + 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "create sdioPollingTask fail \n");
	}
#else
	if (rtos_task_create(NULL, ((const char *)"sdio_int_hal_task"), (rtos_task_function_t)rtw_sdio_interrupt_handler, NULL, SDIO_POLLING_STACK_SIZE,
						 0 + 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "create sdio_int_hal_task fail \n");
	}
#endif

}

/**
 * @brief  to initialize the inic device.
 * @param  none.
 * @return none.
 */
void whc_sdio_host_init(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;
	if (RTK_SUCCESS != rtos_mutex_create(&sdio_lock)) {
		RTK_LOGE(TAG_WLAN_INIC, "create sdio lock fail \r\n");
	}

	if (rtw_sdio_init(priv) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: initialize SDIO Failed!\n", __FUNCTION__);
		return;
	}

	/* init sdio */
	whc_sdio_host_init_drv();

	whc_host_init_done = 1;
}


/* Synchronous API: blocks until the transfer completes. Caller is responsible
 * for freeing buf_alloc after this function returns. */
void whc_sdio_host_send(u8 *buf, u16 len, void *buf_alloc, u8 is_skb)
{
	uint32_t polling_num = 0;
	struct whc_sdio *priv = &whc_sdio_priv;
	struct INIC_TX_DESC *ptxdesc;

	(void) buf_alloc;
	(void) is_skb;

	/* wakeup device if it's in power save mode before send msg */
	if (priv->dev_state == PWR_STATE_SLEEP) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: wakeup device", __func__);
		//TODO wake
	}

	rtos_mutex_take(priv->lock, MUTEX_WAIT_TIMEOUT);
	// check if hardware tx fifo page is enough
	while (priv->SdioTxBDFreeNum < 1) {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		//TODO
		if (polling_num ++ > 0) {
			priv->tx_avail_int_triggered = 1;
			if (rtos_sema_take(priv->txbd_wq, 1000) != RTK_SUCCESS) {
				goto exit;
			}
		}

		rtw_sdio_query_txbd_status(priv);
#else
		rtw_sdio_query_txbd_status(priv);

		polling_num++;
		if ((polling_num % 60) == 0) {
			vTaskDelay(1);
		}

		// Total number of TXBD is NOT available, so update current TXBD status
		rtw_sdio_query_txbd_status(priv);
#endif
	}

	if (len > priv->SdioTxMaxSZ) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: PKT SIZE ERROR, total size: %x\n", __FUNCTION__, len);
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

exit:
	rtos_mutex_give(priv->lock);
	return;
}

