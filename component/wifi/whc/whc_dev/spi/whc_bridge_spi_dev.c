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
	(void) hdr;
	struct whc_api_info *ret_msg;
	(void) ret_msg;

	switch (event) {
	case WHC_WIFI_EVT_XIMT_PKTS:
		/* put the inic message to the queue */
		if (whc_msg_enqueue(skb, &dev_xmit_priv.xmit_queue) == RTK_FAIL) {
			break;
		}
		/* wakeup task */
		rtos_sema_give(dev_xmit_priv.xmit_sema);

		break;
#ifdef CONFIG_WHC_WIFI_API_PATH
	case WHC_WIFI_EVT_API_CALL:
		event_priv.rx_api_msg = rxbuf;
		rtos_sema_give(event_priv.task_wake_sema);

		break;
	case WHC_WIFI_EVT_API_RETURN:
		if (event_priv.b_waiting_for_ret) {
			event_priv.rx_ret_msg = rxbuf;
			rtos_sema_give(event_priv.api_ret_sema);
		} else {
			ret_msg = (struct whc_api_info *)rxbuf;
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_WARN, "too late to receive API ret, ID: 0x%x!\n", ret_msg->api_id);

			/* free rx buffer */
			rtos_mem_free((u8 *)ret_msg);
		}

		break;
#endif
	default:
#ifdef CONFIG_WHC_CMD_PATH
		hdr = (struct whc_bridge_hdr *)rxbuf;
		whc_dev_pkt_rx_to_user(rxbuf + sizeof(struct whc_bridge_hdr), rxbuf, hdr->len);
#endif
		break;
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

	wifi_set_user_config();
	init_skb_pool(wifi_user_config.skb_num_np, wifi_user_config.skb_buf_size ? wifi_user_config.skb_buf_size : MAX_SKB_BUF_SIZE, SKB_CACHE_SZ);

	whc_spi_dev_init();

	whc_bridge_dev_pktfilter_init();

	/* initialize the dev priv */
	whc_dev_init_priv();

#ifdef CONFIG_WHC_WIFI_API_PATH
	whc_dev_api_init();
#endif
}