#include "whc_dev.h"

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
	struct whc_bridge_hdr *hdr;

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
		hdr = (struct whc_bridge_hdr *)rxbuf;
		whc_dev_pkt_rx_to_user(rxbuf + sizeof(struct whc_bridge_hdr), rxbuf, hdr->len);
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

	whc_bridge_dev_pktfilter_init();

	/* initialize the dev priv */
	whc_dev_init_priv();
}