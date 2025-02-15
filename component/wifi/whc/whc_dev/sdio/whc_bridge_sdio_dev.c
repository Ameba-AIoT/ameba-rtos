#include "whc_dev.h"

/**
 * @brief  to haddle the inic message interrupt. If the message queue is
 * 	initialized, it will enqueue the message and wake up the message
 * 	task to haddle the message. If last send message cannot be done, I will
 * 	set pending for next sending message.
 * @param  rxbuf: rx data.
 * @return none.
 */
void whc_bridge_sdio_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb)
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
		whc_bridge_dev_pkt_rx_to_user(rxbuf);
		break;
	}

}

/**
 * @brief  to send data to host
 * @param  data: data buf to be sent, data shold always 4Bytes aligned.
 * @param  len: data len to be sent.
 * @param  need_free: data buf need free or not after move to host. set 1 if need free
 * @return none.
 * @note: data buf free after data move to host, static data buf is not allowed.
 */
void whc_bridge_sdio_dev_send_to_host(u8 *data, u32 len, u8 need_free)
{
	whc_sdio_dev_send_data(data, len, need_free);
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

	/* initialize the dev priv, for pkt trx */
	inic_dev_init_priv();
}

