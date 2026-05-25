/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include "whc_dev.h"

struct xmit_priv_t dev_xmit_priv;

/**
 * @brief  xmit handler to get frames from xmit queue and forward to WIFI stack
 * @param  none.
 * @return none.
 */
void whc_dev_xmit_tasklet(void)
{
	struct whc_msg_node *p_node = NULL;
	struct __queue *p_xmit_queue = NULL;
	u8  continus_handle;
	struct sk_buff *skb;

	p_xmit_queue = &dev_xmit_priv.xmit_queue;
	continus_handle = 0;

	/* get the data from tx queue. */
	p_node = whc_msg_dequeue(p_xmit_queue);
	while (p_node) {
		skb = (struct sk_buff *) p_node->msg;
		wifi_if_send_skb((int)skb->dev, skb);

		/* release node */
		rtos_mem_free((u8 *)p_node);
		continus_handle++;
		if (continus_handle > (wifi_user_config.skb_num_np / 2)) {
			break;
		}

		/* get next item */
		p_node = whc_msg_dequeue(p_xmit_queue);
	}
}

/* ---------------------------- Public Functions ---------------------------- */

/**
 * @brief  to initialize the parameters of xmit.
 * @param  none.
 * @return none.
 */
void whc_dev_xmit_init(void)
{
	/* initialize queue. */
	rtw_init_queue(&(dev_xmit_priv.xmit_queue));
}

/**
 * @brief  receiving function to send the received data to host.
 * @param  idx_wlan[in]: which port of wifi to set.
 * @return none.
 */
void whc_dev_netif_rx(int idx)
{
	struct sk_buff *skb = NULL;
	u8 *ptr;
	u8 pad_len, tmp = 0;
	struct whc_msg_info *msg_info = NULL;

#if defined(CONFIG_WHC_DEV_TCPIP_KEEPALIVE)
	if (whc_dev_recv_pkt_process((u8 *)&idx, &skb) == 0) {
		return;
	}
#else
	skb = wifi_if_get_recv_skb(idx);
#endif
	if (!skb) {
		return;
	}

	/* proxy process */
#if defined(CONFIG_WAR_OFFLOAD)
	if (WAROffloadParm.offload_en) {
		switch (ParseProtocol(idx, skb)) {

		case Offload_Service_mDnsv4:
			if (WAROffloadParm.offload_ctrl & (WAR_MDNS_V4_RSP_EN | WAR_MDNS_V4_WAKEUP_EN)) {
				OnMDNSv4(idx, skb);
			}
			break;
		case Offload_Service_mDnsv6:
			if (WAROffloadParm.offload_ctrl & (WAR_MDNS_V6_RSP_EN | WAR_MDNS_V6_WAKEUP_EN)) {
				OnMDNSv6(idx, skb);
			}
			break;

		default:
			break;
		}

		/* TODO: pattern match check */

		/* free skb */
		dev_kfree_skb_any(skb);
		return;
	}
#endif

	/* padding to make the msg_info address 4-byte aligned */
	ptr = skb->data;
	pad_len = ((u32)ptr - sizeof(struct whc_msg_info)) % DEV_DMA_ALIGN;

	if (pad_len + sizeof(struct whc_msg_info) > (u32)(skb->data - skb->head)) {
		RTK_LOGE(TAG_WLAN_INIC, "can't reserve struct whc_msg_info in front of skb->data!\n");
		return;
	}

	msg_info = (struct whc_msg_info *)(ptr - (pad_len + sizeof(struct whc_msg_info)));
	if ((u32)msg_info % DEV_DMA_ALIGN) {
		RTK_LOGE(TAG_WLAN_INIC, "msg_info not 4-bytes aligned!\n");
		return;
	}

	msg_info->event = WHC_WIFI_EVT_RECV_PKTS;
	msg_info->wlan_idx = idx;
	msg_info->data_len = skb->len;
	msg_info->pad_len = pad_len;

	whc_dev_flowctrl(&tmp, 0);
	msg_info->flow_ctrl_en = tmp;

	/* send msg_info + pad + rx_pkt_data(skb->data, skb->len) */
	whc_dev_send((u8 *)msg_info, sizeof(struct whc_msg_info) + pad_len + skb->len, skb, 1);
}

void whc_dev_flowctrl(u8 *status, u8 send_cmd)
{
	struct whc_msg_info *msg_info = NULL;
	u8 status_change = 0;

	if (skbpriv.skb_buff_num - skbpriv.skb_buff_used < WHC_FLOWCTRL_LOW_THRESHOLD) {
		if (!dev_xmit_priv.flowctrl_en) {
			dev_xmit_priv.flowctrl_en = 1;
			status_change = 1;
		}
	} else if (skbpriv.skb_buff_num - skbpriv.skb_buff_used > WHC_FLOWCTRL_HIGH_THRESHOLD) {
		if (dev_xmit_priv.flowctrl_en) {
			dev_xmit_priv.flowctrl_en = 0;
			status_change = 1;
		}
	}

	if (status) {
		*status = dev_xmit_priv.flowctrl_en;
	}

	if (send_cmd && status_change) {
		msg_info = rtos_mem_zmalloc(sizeof(struct whc_msg_info));
		msg_info->event = WHC_WIFI_EVT_FLOWCTRL;
		msg_info->wlan_idx = 0;
		msg_info->data_len = 0;
		msg_info->pad_len = 0;
		msg_info->flow_ctrl_en = dev_xmit_priv.flowctrl_en;

		/* send msg_info + pad + rx_pkt_data(skb->data, skb->len) */
		whc_dev_send((u8 *)msg_info, sizeof(struct whc_msg_info), msg_info, 0);
	}
}

void whc_dev_dispatch_event_copy(const u8 *src, u32 size)
{
	u8 *buf = rtos_mem_zmalloc(size);

	if (!buf) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: no mem\n", __func__);
		return;
	}
	memcpy(buf, src, size);
	whc_dev_event_int_hdl(buf, NULL);
}

void whc_dev_free_txbuf(struct whc_txbuf_info_t *buf_info)
{
	if (buf_info->is_skb) {
		dev_kfree_skb_any((struct sk_buff *)buf_info->ptr);
	} else {
		rtos_mem_free((u8 *)buf_info->ptr);
	}
	rtos_mem_free((u8 *)buf_info);
}

struct whc_txbuf_info_t *whc_dev_alloc_buf_info(u8 *buf, u16 len, void *alloc_buf, u8 is_skb)
{
	struct whc_txbuf_info_t *buf_info = NULL;

	/* construct struct whc_buf_info & whc_buf_info_t */
	buf_info = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));
	if (!buf_info) {
		return NULL;
	}

	buf_info->txbuf_info.buf_allocated = buf_info->txbuf_info.buf_addr = (u32)buf;
	buf_info->txbuf_info.size_allocated = buf_info->txbuf_info.buf_size = len;

	buf_info->ptr = alloc_buf;
	buf_info->is_skb = is_skb;

	return buf_info;
}

/**
 * @brief  to haddle the inic message interrupt. If the message queue is
 * 	initialized, it will enqueue the message and wake up the message
 * 	task to haddle the message. If last send message cannot be done, I will
 * 	set pending for next sending message.
 * @param  rxbuf: rx data.
 * @return none.
 */
void whc_dev_event_int_hdl(u8 *rxbuf, struct sk_buff *skb)
{
	u32 event = *(u32 *)rxbuf;
	struct whc_api_info *ret_msg;

	(void) ret_msg;

	switch (event) {
	case WHC_WIFI_EVT_XIMT_PKTS:
		/* put the inic message to the queue */
		if (whc_msg_enqueue(skb, &dev_xmit_priv.xmit_queue) == RTK_FAIL) {
			break;
		}
		/* wakeup task */
		rtw_single_thread_wakeup();
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
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_WARN, "API ret TO, ID: 0x%x!\n", ret_msg->api_id);

			/* free rx buffer */
			rtos_mem_free((u8 *)ret_msg);
		}

		break;
#endif
#ifdef CONFIG_WHC_CMD_PATH
	case WHC_WIFI_EVT_CMD:
		whc_dev_cmd_rx_to_user(rxbuf);
		break;
#endif
	default:
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Event(%x) unknown!\n", event);
	}

}

