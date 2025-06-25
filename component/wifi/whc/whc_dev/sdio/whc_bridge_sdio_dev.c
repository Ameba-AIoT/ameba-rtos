#include "whc_dev.h"

/**
 * @brief  to haddle the inic message interrupt. If the message queue is
 * 	initialized, it will enqueue the message and wake up the message
 * 	task to haddle the message. If last send message cannot be done, I will
 * 	set pending for next sending message.
 * @param  rxbuf: rx data.
 * @return none.
 */
void whc_bridge_sdio_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb, u16 size)
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
		whc_dev_pkt_rx_to_user(rxbuf, rxbuf, size);
		break;
	}

}



/**
 * @brief  to initialize the inic device.
 * @param  none.
 * @return none.
 */
void whc_bridge_sdio_dev_init(void)
{
	rtk_log_level_set("SPDIO", RTK_LOG_ERROR);

	whc_sdio_dev_init();

	whc_bridge_dev_pktfilter_init();

	/* initialize the dev priv, for pkt trx */
	whc_dev_init_priv();
}

