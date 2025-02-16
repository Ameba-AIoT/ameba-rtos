#include "whc_dev.h"

/**
 * @brief  to haddle the inic message interrupt. If the message queue is
 * 	initialized, it will enqueue the message and wake up the message
 * 	task to haddle the message. If last send message cannot be done, I will
 * 	set pending for next sending message.
 * @param  rxbuf: rx data.
 * @return none.
 */
void bridge_spi_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb)
{
	u32 event = *(u32 *)rxbuf;

	switch (event) {
	case INIC_WIFI_EVT_XIMT_PKTS:
		/* put the inic message to the queue */
		if (inic_msg_enqueue(skb, &dev_xmit_priv.xmit_queue) == FAIL) {
			break;
		}
		/* wakeup task */
		rtos_sema_give(dev_xmit_priv.xmit_sema);

		break;
	default:
		//whc_bridge_dev_pkt_rx_to_user(rxbuf);
		break;
	}

}

/**
 * @brief  to initialize the bridge device.
 * @param  none.
 * @return none.
 */
void bridge_spi_dev_init(void)
{
	rtk_log_level_set(TAG_WLAN_INIC, RTK_LOG_DEBUG);

	inic_spi_init();

	/* initialize the dev priv */
	inic_dev_init_priv();
}