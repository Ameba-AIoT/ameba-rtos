#include "whc_dev.h"

struct whc_sdio_priv_t sdio_priv = {0};

void (*bt_inic_sdio_recv_ptr)(uint8_t *buffer, uint16_t len);

static u32 whc_sdio_dev_suspend(u32 expected_idle_time, void *param)
{
	UNUSED(expected_idle_time);
	UNUSED(param);

	if (pmu_get_sleep_type() == SLEEP_PG) {
		struct spdio_t *dev = &sdio_priv.dev;
		spdio_deinit(dev);
	}

	return TRUE;
}

static u32 whc_sdio_dev_resume(u32 expected_idle_time, void *param)
{
	UNUSED(expected_idle_time);
	UNUSED(param);
	if (pmu_get_sleep_type() == SLEEP_PG) {
		struct spdio_t *dev = &sdio_priv.dev;
		spdio_init(dev);
	}
	return TRUE;
}

static char whc_sdio_dev_rpwm_cb(void *priv, u16 value)
{
	UNUSED(priv);

	if (value & RPWM2_CG_BIT) {
		SDIO_SetReady(SDIO_WIFI, DISABLE);
		whc_dev_ps_set_tickps_cmd(WHC_CMD_TICKPS_R);
	}

	if (value & RPWM2_ACT_BIT) {
		whc_dev_ps_resume_cb();
		SDIO_SetReady(SDIO_WIFI, ENABLE);
	}

	return 0;
}

/*spdio tx done callback(Device->HOST), manage buffer*/
// this API will be called after package have been moved to HOST
static char whc_sdio_dev_tx_done_cb(void *priv, void *pbuf)
{
	UNUSED(priv);
	struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)pbuf;
	struct whc_txbuf_info_t *whc_tx = container_of(tx_buf, struct whc_txbuf_info_t, txbuf_info);

	whc_dev_free_txbuf(whc_tx);

	rtos_sema_give(sdio_priv.rxbd_release_sema);

	return RTK_SUCCESS;
}

/*spdio rx done callback (HOST->Device), manage your package and buffer*/
static char whc_sdio_dev_rx_done_cb(void *priv, void *pbuf, u8 *pdata, u16 size, u8 type)
{
	struct spdio_buf_t *rx_buf = (struct spdio_buf_t *)pbuf;
	u32 event = *(u32 *)pdata;
	struct sk_buff *new_skb = NULL, *rx_skb;
	struct whc_msg_info *p_msg_info;

	(void) priv;
	(void) type;

	if (event == WHC_WIFI_EVT_XIMT_PKTS) {
		/* SPDIO receives XMIT_PKTS */
		rx_skb = (struct sk_buff *)rx_buf->priv;

		/* reserved 3 skb for rx */
		if (((skbpriv.skb_buff_num - skbpriv.skb_buff_used) < 3) ||
			((new_skb = dev_alloc_skb(SPDIO_DEVICE_RX_BUFSZ, SPDIO_SKB_RSVD_LEN)) == NULL)) {
			goto drop_pkt;
		}

		/* assign new buffer for SPDIO RX ring */
		rx_buf->buf_allocated = rx_buf->buf_addr = (u32) new_skb->data;
		rx_buf->size_allocated = rx_buf->buf_size = sdio_priv.dev.device_rx_bufsz;
		rx_buf->priv = new_skb;

		/* handle buf data */
		p_msg_info = (struct whc_msg_info *)(rx_skb->data + sizeof(INIC_TX_DESC));
		if (!wifi_is_running(p_msg_info->wlan_idx)) {
			/*free skb and return*/
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Port %d is down, drop!\n", p_msg_info->wlan_idx);
			dev_kfree_skb_any(rx_skb);
			goto drop_pkt;
		}

		skb_reserve(rx_skb, sizeof(INIC_TX_DESC) + sizeof(struct whc_msg_info) + p_msg_info->pad_len);
		skb_put(rx_skb, size - sizeof(struct whc_msg_info) - p_msg_info->pad_len);

		/* save wlan_idx temporaries*/
		rx_skb->dev = (void *)((u32)p_msg_info->wlan_idx);

		whc_dev_event_int_hdl(pdata, rx_skb);

#ifdef CONFIG_WHC_WIFI_API_PATH
	} else if (event == WHC_CUST_EVT) {
		whc_dev_recv_cust_evt(pdata);
#endif
	} else if (event >= WHC_BT_EVT_BASE && event <= WHC_BT_EVT_MAX) {
		/* copy by bt, skb no change */
		if (bt_inic_sdio_recv_ptr) {
			bt_inic_sdio_recv_ptr(pdata, SPDIO_DEVICE_RX_BUFSZ);
		}
	} else {
		/* SPDIO receives EVENTS, free buf later, sdio ring buffer no need to modify */
		whc_dev_dispatch_event_copy(pdata, size);
	}

drop_pkt:
	return RTK_SUCCESS;
}

void whc_sdio_dev_init(void)
{
	u32 i;
	struct sk_buff *skb = NULL;
	struct spdio_t *dev = &sdio_priv.dev;
#ifdef WHC_SDIO_USE_GPIO_INT
	GPIO_InitTypeDef GPIO_InitStruct;
#endif

	rtk_log_level_set("SPDIO", RTK_LOG_ERROR);

	dev->priv = NULL;
	dev->host_tx_bd_num = SPDIO_HOST_TX_BD_NUM;
	dev->host_rx_bd_num = SPDIO_HOST_RX_BD_NUM;
	dev->device_rx_bufsz = ((wifi_user_config.skb_buf_size - SPDIO_SKB_RSVD_LEN) >> 6) << 6;

	dev->rx_buf = (struct spdio_buf_t *)rtos_mem_zmalloc(dev->host_tx_bd_num * sizeof(struct spdio_buf_t));
	if (!dev->rx_buf) {
		RTK_LOGE(TAG_WLAN_INIC, "malloc fail for spdio buf!\n");
		return;
	}

	for (i = 0; i < dev->host_tx_bd_num; i++) {
		skb = dev_alloc_skb(SPDIO_DEVICE_RX_BUFSZ, SPDIO_SKB_RSVD_LEN);

		// this buffer must be 4 byte alignment
		if (skb == NULL || (u32)skb->data % 4) {
			RTK_LOGE(TAG_WLAN_INIC, "alloc rx buf fail!\n");
			goto fail;
		}

		dev->rx_buf[i].buf_allocated = dev->rx_buf[i].buf_addr = (u32) skb->data;
		dev->rx_buf[i].size_allocated = dev->rx_buf[i].buf_size = dev->device_rx_bufsz;
		dev->rx_buf[i].priv = skb;
	}

	dev->pSDIO = SDIO_WIFI;
	dev->device_rx_done_cb = whc_sdio_dev_rx_done_cb;
	dev->device_tx_done_cb = whc_sdio_dev_tx_done_cb;
	dev->rpwm_cb = whc_sdio_dev_rpwm_cb;

	rtos_mutex_create_static(&sdio_priv.tx_lock);
	rtos_sema_create_static(&sdio_priv.rxbd_release_sema, 0, 0xFFFFFFFF);

	spdio_init(dev);

	/* take lock after host ready */
	pmu_acquire_wakelock(PMU_WHC_WIFI);

	pmu_register_sleep_callback(PMU_WHC_WIFI, (PSM_HOOK_FUN)whc_sdio_dev_suspend, NULL, (PSM_HOOK_FUN)whc_sdio_dev_resume, NULL);

#ifdef WHC_SDIO_USE_GPIO_INT
	/* Initialize GPIO */
	GPIO_InitStruct.GPIO_Pin = RX_REQ_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct);
	whc_sdio_dev_set_rxreq_pin(DEV_RX_IDLE);
#endif

	RTK_LOGI(TAG_WLAN_INIC, "SDIO device init done!\n");
	return;

fail:
	/* free allocated skb and buffer */
	for (i = 0; i < dev->host_tx_bd_num; i++) {
		if (dev->rx_buf[i].priv) {
			dev_kfree_skb_any(dev->rx_buf[i].priv);
		}
	}

	rtos_mem_free(dev->rx_buf);
	return;
}

void whc_sdio_dev_send(u8 *buf, u16 len, void *buf_alloc, u8 is_skb)
{
	struct whc_txbuf_info_t *buf_info = NULL;
	struct spdio_buf_t *pbuf;

	buf_info = whc_dev_alloc_buf_info(buf, len, buf_alloc, is_skb);
	if (!buf_info) {
		goto drop;
	}
	pbuf = &buf_info->txbuf_info;

	/* Call this function when receive pkt, call API or send API return value.
	Use locks to ensure exclusive execution. */
	rtos_mutex_take(sdio_priv.tx_lock, MUTEX_WAIT_TIMEOUT);

	while (spdio_tx(&sdio_priv.dev, pbuf) == FALSE) {
		/* wait for RXBD release */
		rtos_sema_take(sdio_priv.rxbd_release_sema, 0xFFFFFFFF);
	}

#ifdef WHC_SDIO_USE_GPIO_INT
	// Regardless of the previous state of the GPIO, generate a rising edge here.
	whc_sdio_dev_set_rxreq_pin(DEV_RX_IDLE);
	whc_sdio_dev_set_rxreq_pin(DEV_RX_REQ);
#endif

	rtos_mutex_give(sdio_priv.tx_lock);

	return;

drop:
	/* Dev drop tx, free tx skb or buffer */
	if (is_skb) {
		dev_kfree_skb_any((struct sk_buff *) buf_alloc);
	} else {
		rtos_mem_free((u8 *)buf_alloc);
	}
}

u8 whc_sdio_dev_bus_is_idle(void)
{
	/*Not yet implemented*/
	return TRUE;
}

