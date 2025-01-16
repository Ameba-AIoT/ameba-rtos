#include "inic_dev.h"

struct inic_sdio_priv_t sdio_priv = {0};

void rtw_pending_q_resume(void);

static char inic_sdio_dev_rpwm_cb(void *priv, u16 value)
{
	UNUSED(priv);

	if (value & RPWM2_CG_BIT) {
		SDIO_SetReady(SDIO_WIFI, DISABLE);
		pmu_release_wakelock(PMU_WLAN_DEVICE);
	}

	if (value & RPWM2_ACT_BIT) {
		pmu_acquire_wakelock(PMU_WLAN_DEVICE);
		SDIO_SetReady(SDIO_WIFI, ENABLE);
	}

	return 0;
}

/*spdio tx done callback(Device->HOST), manage buffer*/
// this API will be called after package have been moved to HOST
static char inic_sdio_dev_tx_done_cb(void *priv, void *pbuf)
{
	UNUSED(priv);
	struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)pbuf;
	struct inic_txbuf_info_t *inic_tx = container_of(tx_buf, struct inic_txbuf_info_t, txbuf_info);

	if (inic_tx->is_skb) {
		dev_kfree_skb_any((struct sk_buff *) inic_tx->ptr);
	} else {
		rtos_mem_free((u8 *)inic_tx->ptr);
	}

	rtos_mem_free((u8 *)inic_tx);

	rtos_sema_give(sdio_priv.rxbd_release_sema);

	return SUCCESS;
}

/*spdio rx done callback (HOST->Device), manage your package and buffer*/
static char inic_sdio_dev_rx_done_cb(void *priv, void *pbuf, u8 *pdata, u16 size, u8 type)
{
	struct spdio_buf_t *rx_buf = (struct spdio_buf_t *)pbuf;
	u32 event = *(u32 *)pdata;
	u8 *buf = NULL;
	struct sk_buff *new_skb = NULL, *rx_skb;
	struct inic_msg_info *p_msg_info;

	(void) priv;
	(void) type;

	if (event == INIC_WIFI_EVT_XIMT_PKTS) {
		/* SPDIO receives XMIT_PKTS */
		rx_skb = (struct sk_buff *)rx_buf->priv;

		if (((skbpriv.skb_buff_num - skbpriv.skb_buff_used) < 5) ||
			((new_skb = dev_alloc_skb(SPDIO_RX_BUFSZ, SPDIO_SKB_RSVD_LEN)) == NULL)) {

			/* resume pending queue to release skb */
			rtw_pending_q_resume();

			return FAIL;
		}

		/* assign new buffer for SPDIO RX ring */
		rx_buf->buf_allocated = rx_buf->buf_addr = (u32) new_skb->data;
		rx_buf->size_allocated = sdio_priv.dev.rx_bd_bufsz;
		rx_buf->priv = new_skb;

		/* handle buf data */
		p_msg_info = (struct inic_msg_info *)(rx_skb->data + sizeof(INIC_TX_DESC));

		skb_reserve(rx_skb, sizeof(INIC_TX_DESC) + sizeof(struct inic_msg_info) + p_msg_info->pad_len);
		skb_put(rx_skb, size - sizeof(struct inic_msg_info) - p_msg_info->pad_len);

		/* save wlan_idx temporaries*/
		rx_skb->dev = (void *) p_msg_info->wlan_idx;

		inic_dev_event_int_hdl(pdata, rx_skb);
	} else if (event == INIC_CUST_EVT) {
		inic_dev_recv_cust_evt(pdata);
	} else {
		/* SPDIO receives EVENTS */
		buf = rtos_mem_zmalloc(size);

		if (buf == NULL) {
			RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
			return FAIL;
		}

		memcpy(buf, pdata, size);

		inic_dev_event_int_hdl(buf, NULL);

		/* free buf later, sdio ring buffer no need to modify. */
	}

	return SUCCESS;
}

static void inic_sdio_init(void)
{
	u32 i;
	struct sk_buff *skb = NULL;
	struct spdio_t *dev = &sdio_priv.dev;

	dev->priv = NULL;
	dev->rx_bd_num = SPDIO_RX_BD_NUM;
	dev->tx_bd_num = SPDIO_TX_BD_NUM;
	dev->rx_bd_bufsz = SPDIO_RX_BUFSZ;

	dev->rx_buf = (struct spdio_buf_t *)rtos_mem_zmalloc(dev->rx_bd_num * sizeof(struct spdio_buf_t));
	if (!dev->rx_buf) {
		RTK_LOGE(TAG_WLAN_INIC, "malloc failed for spdio buffer structure!\n");
		return;
	}

	for (i = 0; i < dev->rx_bd_num; i++) {
		skb = dev_alloc_skb(SPDIO_RX_BUFSZ, SPDIO_SKB_RSVD_LEN);

		dev->rx_buf[i].buf_allocated = dev->rx_buf[i].buf_addr = (u32) skb->data;
		dev->rx_buf[i].size_allocated = dev->rx_bd_bufsz;
		dev->rx_buf[i].priv = skb;

		// this buffer must be 4 byte alignment
		if (dev->rx_buf[i].buf_addr % 4) {
			RTK_LOGE(TAG_WLAN_INIC, "rx buffer address not 4-byte aligned!\n");
			return;
		}
	}

	dev->rx_done_cb = inic_sdio_dev_rx_done_cb;
	dev->tx_done_cb = inic_sdio_dev_tx_done_cb;
	dev->rpwm_cb = inic_sdio_dev_rpwm_cb;

	rtos_mutex_create_static(&sdio_priv.tx_lock);
	rtos_sema_create_static(&sdio_priv.rxbd_release_sema, 0, 0xFFFFFFFF);

	spdio_init(dev);

	RTK_LOGI(TAG_WLAN_INIC, "SDIO device init done!\n");

}

void inic_dev_trigger_rx_handle(void)
{
	spdio_trigger_rx_handle();
}

u8 inic_dev_tx_path_avail(void)
{
	u32 delay_cnt = 0;
	u8 ret = FALSE;

	while (delay_cnt++ < 100) {
		/* wait for TXBD empty and skb allocatable */
		if ((SDIO_TXBD_WPTR_Get(SDIO_WIFI) == SDIO_TXBD_RPTR_Get(SDIO_WIFI)) &&
			(skbpriv.skb_buff_num - skbpriv.skb_buff_used) > 3) {
			ret = TRUE;
			break;
		} else {
			rtos_time_delay_ms(1);
		}
	}

	return ret;
}

/**
 * @brief  to haddle the inic message interrupt. If the message queue is
 * 	initialized, it will enqueue the message and wake up the message
 * 	task to haddle the message. If last send message cannot be done, I will
 * 	set pending for next sending message.
 * @param  rxbuf: rx data.
 * @return none.
 */
void inic_dev_event_int_hdl(u8 *rxbuf, struct sk_buff *skb)
{
	u32 event = *(u32 *)rxbuf;
	struct inic_api_info *ret_msg;

	switch (event) {
	case INIC_WIFI_EVT_API_CALL:
		event_priv.rx_api_msg = rxbuf;
		rtos_sema_give(event_priv.task_wake_sema);

		break;
	case INIC_WIFI_EVT_API_RETURN:
		if (event_priv.b_waiting_for_ret) {
			event_priv.rx_ret_msg = rxbuf;
			rtos_sema_give(event_priv.api_ret_sema);
		} else {
			ret_msg = (struct inic_api_info *)rxbuf;
			RTK_LOGW(TAG_WLAN_INIC, "too late to receive API ret, ID: 0x%x!\n", ret_msg->api_id);

			/* free rx buffer */
			rtos_mem_free((u8 *)ret_msg);
		}

		break;
	case INIC_WIFI_EVT_XIMT_PKTS:
		/* put the inic message to the queue */
		if (inic_msg_enqueue(skb, &dev_xmit_priv.xmit_queue) == FAIL) {
			break;
		}
		/* wakeup task */
		rtos_sema_give(dev_xmit_priv.xmit_sema);

		break;
	default:
		RTK_LOGE(TAG_WLAN_INIC, "Event(%ld) unknown!\n", event);
		break;
	}

}

void inic_dev_send(struct spdio_buf_t *pbuf)
{
	/* Call this function when receive pkt, call API or send API return value.
	Use locks to ensure exclusive execution. */
	rtos_mutex_take(sdio_priv.tx_lock, MUTEX_WAIT_TIMEOUT);

	while (spdio_tx(&sdio_priv.dev, pbuf) == FALSE) {
		/* wait for RXBD release */
		rtos_sema_take(sdio_priv.rxbd_release_sema, 0xFFFFFFFF);
	}

	rtos_mutex_give(sdio_priv.tx_lock);

	return;
}

/**
 * @brief  to initialize the inic device.
 * @param  none.
 * @return none.
 */
void inic_dev_init(void)
{
	rtk_log_level_set(TAG_WLAN_INIC, RTK_LOG_DEBUG);
	rtk_log_level_set("SPDIO", RTK_LOG_ERROR);

	wifi_set_user_config();
	init_skb_pool(wifi_user_config.skb_num_np, wifi_user_config.skb_buf_size ? wifi_user_config.skb_buf_size : MAX_SKB_BUF_SIZE, SKB_CACHE_SZ);

	inic_sdio_init();

	/* initialize the dev priv */
	inic_dev_init_priv();

	inic_api_init_dev();
}

