#include "whc_dev.h"

/**
 * @brief  to haddle the inic message interrupt. If the message queue is
 * 	initialized, it will enqueue the message and wake up the message
 * 	task to haddle the message. If last send message cannot be done, I will
 * 	set pending for next sending message.
 * @param  rxbuf: rx data.
 * @return none.
 */
void whc_fullmac_sdio_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb)
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

/**
 * @brief  to initialize the inic device.
 * @param  none.
 * @return none.
 */
void whc_fullmac_sdio_dev_init(void)
{
	rtk_log_level_set(TAG_WLAN_INIC, RTK_LOG_DEBUG);
	rtk_log_level_set("SPDIO", RTK_LOG_ERROR);

	wifi_set_user_config();
	init_skb_pool(wifi_user_config.skb_num_np, wifi_user_config.skb_buf_size ? wifi_user_config.skb_buf_size : MAX_SKB_BUF_SIZE, SKB_CACHE_SZ);

	whc_sdio_dev_init();

	/* initialize the dev priv */
	inic_dev_init_priv();

	inic_api_init_dev();
}

