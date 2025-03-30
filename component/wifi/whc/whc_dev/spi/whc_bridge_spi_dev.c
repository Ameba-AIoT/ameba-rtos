#include "whc_dev.h"

struct whc_bridge_spi_priv {
	u8 *rxbuf;
};


static rtos_sema_t bridge_spi_sema;
static struct whc_bridge_spi_priv bridge_priv = {0};

static void whc_bridge_spi_dev_task(void *pData)
{
	struct whc_bridge_spi_priv *p = pData;
	struct whc_bridge_hdr *hdr = NULL;

	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(bridge_spi_sema, RTOS_MAX_TIMEOUT);

		if (p->rxbuf) {
			hdr = (struct whc_bridge_hdr *)p->rxbuf;
			whc_bridge_dev_pkt_rx_to_user(p->rxbuf + sizeof(struct whc_bridge_hdr), hdr->len);
			rtos_mem_free(p->rxbuf);
			p->rxbuf = NULL;
		}
	}
}

/**
 * @brief  to haddle the inic message interrupt. If the message queue is
 * 	initialized, it will enqueue the message and wake up the message
 * 	task to haddle the message. If last send message cannot be done, I will
 * 	set pending for next sending message.
 * @param  rxbuf: rx data.
 * @return none.
 */
void whc_bridge_spi_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb)
{
	u32 event = *(u32 *)rxbuf;

	switch (event) {
	case WHC_WIFI_EVT_XIMT_PKTS:
		/* put the inic message to the queue */
		if (whc_msg_enqueue(skb, &dev_xmit_priv.xmit_queue) == RTK_FAIL) {
			break;
		}
		/* wakeup task */
		rtos_sema_give(dev_xmit_priv.xmit_sema);

		break;
	default:
		/* wait for last handdle */
		while (bridge_priv.rxbuf != NULL) {
			rtos_time_delay_ms(1);
		}
		bridge_priv.rxbuf = rxbuf;
		rtos_sema_give(bridge_spi_sema);
		break;
	}

}

/**
* @brief  send buf bridge device
* @param  buf: data buf to be sent, must 4B aligned.
* @param  len: data len to be sent.
* @return none.
*/
void whc_bridge_spi_send_data(u8 *buf, u32 len)
{
	u8 *txbuf = NULL;
	u32 event = *(u32 *)buf;
	u32 txsize = len + sizeof(struct whc_bridge_hdr);
	struct whc_bridge_hdr *hdr = NULL;

	if (event != WHC_WIFI_EVT_RECV_PKTS) {
		txbuf = rtos_mem_zmalloc(txsize);
		if (!txbuf) {
			RTK_LOGE(TAG_WLAN_INIC, "allocate buffer failed when to send in spi bridge\n");
			return;
		}
		hdr = (struct whc_bridge_hdr *)txbuf;
		hdr->event = WHC_WIFI_EVT_BRIDGE;
		hdr->len = len;
		memcpy(txbuf + sizeof(struct whc_bridge_hdr), buf, len);
		whc_spi_dev_send_data(txbuf, txsize);
	} else {
		whc_spi_dev_send_data(buf, len);
	}
}

/**
 * @brief  to initialize the bridge device.
 * @param  none.
 * @return none.
 */
void whc_bridge_spi_dev_init(void)
{
	rtk_log_level_set(TAG_WLAN_INIC, RTK_LOG_DEBUG);

	whc_spi_dev_init();

	bridge_priv.rxbuf = NULL;
	rtos_sema_create(&(bridge_spi_sema), 0, RTOS_SEMA_MAX_COUNT);
	if (rtos_task_create(NULL, "WHC_BRIDGE_SPI_DEV_TASK", whc_bridge_spi_dev_task, (void *)&bridge_priv, 1024 * 4, 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create WHC_BRIDGE_SPI_DEV_TASK Err!!\n");
		return;
	}


	whc_bridge_dev_pktfilter_init();

	/* initialize the dev priv */
	whc_dev_init_priv();
}