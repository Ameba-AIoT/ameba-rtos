#include "whc_dev.h"

struct whc_sdio_priv_t sdio_priv = {0};

#ifndef CONFIG_WHC_BRIDGE
void rtw_pending_q_resume(void);
#endif

static char whc_sdio_dev_rpwm_cb(void *priv, u16 value)
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
static char whc_sdio_dev_tx_done_cb(void *priv, void *pbuf)
{
	UNUSED(priv);
	struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)pbuf;
	struct whc_txbuf_info_t *inic_tx = container_of(tx_buf, struct whc_txbuf_info_t, txbuf_info);

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
static char whc_sdio_dev_rx_done_cb(void *priv, void *pbuf, u8 *pdata, u16 size, u8 type)
{
	struct spdio_buf_t *rx_buf = (struct spdio_buf_t *)pbuf;
	u32 event = *(u32 *)pdata;
	u8 *buf = NULL;
	struct sk_buff *new_skb = NULL, *rx_skb;
	struct whc_msg_info *p_msg_info;

	(void) priv;
	(void) type;

	if (event == WHC_WIFI_EVT_XIMT_PKTS) {
		/* SPDIO receives XMIT_PKTS */
		rx_skb = (struct sk_buff *)rx_buf->priv;

		if (((skbpriv.skb_buff_num - skbpriv.skb_buff_used) < 5) ||
			((new_skb = dev_alloc_skb(SPDIO_RX_BUFSZ, SPDIO_SKB_RSVD_LEN)) == NULL)) {
#ifndef CONFIG_WHC_BRIDGE
			/* resume pending queue to release skb */
			rtw_pending_q_resume();
#endif
			return FAIL;
		}

		/* assign new buffer for SPDIO RX ring */
		rx_buf->buf_allocated = rx_buf->buf_addr = (u32) new_skb->data;
		rx_buf->size_allocated = sdio_priv.dev.rx_bd_bufsz;
		rx_buf->priv = new_skb;

		/* handle buf data */
		p_msg_info = (struct whc_msg_info *)(rx_skb->data + sizeof(INIC_TX_DESC));

		skb_reserve(rx_skb, sizeof(INIC_TX_DESC) + sizeof(struct whc_msg_info) + p_msg_info->pad_len);
		skb_put(rx_skb, size - sizeof(struct whc_msg_info) - p_msg_info->pad_len);

		/* save wlan_idx temporaries*/
		rx_skb->dev = (void *) p_msg_info->wlan_idx;

		whc_sdio_dev_event_int_hdl(pdata, rx_skb, size);

#ifndef  CONFIG_WHC_BRIDGE
	} else if (event == WHC_CUST_EVT) {
		whc_dev_recv_cust_evt(pdata);
#endif

	} else {
		/* SPDIO receives EVENTS */
		buf = rtos_mem_zmalloc(size);

		if (buf == NULL) {
			RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
			return FAIL;
		}

		memcpy(buf, pdata, size);

		whc_sdio_dev_event_int_hdl(buf, NULL, size);

		/* free buf later, sdio ring buffer no need to modify. */
	}

	return SUCCESS;
}

void whc_sdio_dev_init(void)
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
	//DiagPrintf("%s %d %d %d dev->rx_bd_num: %d\r\n", __func__, __LINE__, SPDIO_RX_BUFSZ, SPDIO_SKB_RSVD_LEN, dev->rx_bd_num);

	for (i = 0; i < dev->rx_bd_num; i++) {
		skb = dev_alloc_skb(SPDIO_RX_BUFSZ, SPDIO_SKB_RSVD_LEN);

		dev->rx_buf[i].buf_allocated = dev->rx_buf[i].buf_addr = (u32) skb->data;
		dev->rx_buf[i].size_allocated = dev->rx_bd_bufsz;
		dev->rx_buf[i].priv = skb;

		// this buffer must be 4 byte alignment
		if (dev->rx_buf[i].buf_addr % 4) {
			RTK_LOGE(TAG_WLAN_INIC, "rx buffer address not 4-byte aligned! %x \n", dev->rx_buf[i].buf_addr);
			return;
		}
	}

	dev->rx_done_cb = whc_sdio_dev_rx_done_cb;
	dev->tx_done_cb = whc_sdio_dev_tx_done_cb;
	dev->rpwm_cb = whc_sdio_dev_rpwm_cb;

	rtos_mutex_create_static(&sdio_priv.tx_lock);
	rtos_sema_create_static(&sdio_priv.rxbd_release_sema, 0, 0xFFFFFFFF);

	spdio_init(dev);

	RTK_LOGI(TAG_WLAN_INIC, "SDIO device init done!\n");

}

void whc_sdio_dev_trigger_rx_handle(void)
{
	spdio_trigger_rx_handle();
}

u8 whc_sdio_dev_tx_path_avail(void)
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

void whc_sdio_dev_send_data(u8 *data, u32 len)
{
	struct whc_txbuf_info_t *inic_tx = NULL;
	u8 *buf = NULL;

	buf = rtos_mem_zmalloc(len);

	if (!buf) {
		RTK_LOGE(TAG_WLAN_INIC, "%s Send Error !!", __func__);
		return;
	}

	memcpy(buf, data, len);

	inic_tx = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));
	if (!inic_tx) {
		return;
	}

	inic_tx->txbuf_info.buf_allocated = inic_tx->txbuf_info.buf_addr = (u32)buf;
	inic_tx->txbuf_info.size_allocated = inic_tx->txbuf_info.buf_size = len;

	inic_tx->ptr = buf;
	inic_tx->is_skb = 0;

	/* buf free in sdio send done callback */
	whc_sdio_dev_send(&inic_tx->txbuf_info);

	return;

}

void whc_sdio_dev_send(struct spdio_buf_t *pbuf)
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

