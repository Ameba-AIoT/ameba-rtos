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
 * @param  idx[in]: wlan interface index.
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

void whc_dev_send_flowctrl_cmd(u8 fc_state)
{
	struct whc_msg_info *msg_info = NULL;

	msg_info = rtos_mem_zmalloc(sizeof(struct whc_msg_info));
	if ((u32)msg_info % DEV_DMA_ALIGN) {
		RTK_LOGE(TAG_WLAN_INIC, "msg_info not 4-bytes aligned!\n");
		return;
	}

	msg_info->event = WHC_WIFI_EVT_FLOWCTRL;
	msg_info->wlan_idx = 0;
	msg_info->data_len = 0;
	msg_info->pad_len = 0;
	msg_info->flow_ctrl_en = fc_state;

	/* send msg_info + pad + rx_pkt_data(skb->data, skb->len) */
	whc_dev_send((u8 *)msg_info, sizeof(struct whc_msg_info), msg_info, 0);
}

void whc_dev_trigger_rx(void)
{
	whc_dev_trigger_rx_handle();
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
#ifdef CONFIG_WHCH
		rtw_recv_interface_dma_ok((struct sk_buff *)buf_info->ptr);
#else
		dev_kfree_skb_any((struct sk_buff *)buf_info->ptr);
#endif
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
 * @brief  to handle the whc message interrupt. If the message queue is
 * 	initialized, it will enqueue the message and wake up the message
 * 	task to handle the message.
 * @param  rxbuf[in]: rx data.
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
#ifndef WHCH_TXAGG
		/* wakeup task */
		rtw_single_thread_wakeup();
#endif
		break;
#ifdef CONFIG_WHC_WIFI_API_PATH
	case WHC_WIFI_EVT_API_CALL:
		event_priv.rx_api_msg = rxbuf;
		rtos_sema_give(event_priv.task_wake_sema);

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

#ifdef CONFIG_WHCH
void whch_dev_netif_rx(struct sk_buff *skb)
{
	rltk_wlan_info[0].skb = (void *)skb;

	whc_dev_netif_rx(0);
}

#ifdef WHCH_RXAGG
/**
 * @brief  send an aggregated run of RX units to the host as one USB transfer.
 * @param  head_skb: skb of the first (contiguous) unit; msg_info is built in its headroom.
 * @param  agg_num:  number of units in the run (1..WHCH_RXAGG_NUM).
 * @param  stride:   fixed byte stride between unit starts (host parses units at this stride).
 * @param  content_len: total wire bytes after msg_info+pad = (agg_num-1)*stride + last_unit_len.
 * @return none.
 */
void whch_dev_rxagg_dispatch(struct sk_buff *head_skb, u8 agg_num, u16 stride, u32 content_len)
{
	u8 *ptr;
	u8 pad_len, tmp = 0;
	struct whc_msg_info *msg_info = NULL;

	/* head_skb->data points at the first unit's rx_buffer_desc (rxbd pushed by caller) */
	ptr = head_skb->data;
	pad_len = ((u32)ptr - sizeof(struct whc_msg_info)) % DEV_DMA_ALIGN;

	if (pad_len + sizeof(struct whc_msg_info) > (u32)(head_skb->data - head_skb->head)) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "rxagg: no room for msg_info!\n");
		/* fixed-pool skb: never free it, just mark the run DMA-done so the RXBD is released */
		head_skb->tx_raw.device_id = agg_num;
		rtw_recv_interface_dma_ok(head_skb);
		return;
	}

	msg_info = (struct whc_msg_info *)(ptr - (pad_len + sizeof(struct whc_msg_info)));
	if ((u32)msg_info % DEV_DMA_ALIGN) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "rxagg: msg_info not aligned!\n");
		head_skb->tx_raw.device_id = agg_num;
		rtw_recv_interface_dma_ok(head_skb);
		return;
	}

	msg_info->event = WHC_WIFI_EVT_RECV_PKTS;
	msg_info->wlan_idx = 0;
	msg_info->agg_num = agg_num;
	msg_info->agg_stride = stride;
	msg_info->data_len = content_len;
	msg_info->pad_len = pad_len;

	whc_dev_flowctrl(&tmp, 0);
	msg_info->flow_ctrl_en = tmp;

	/* run length carried on the head skb so tx-done can release every slot */
	head_skb->tx_raw.device_id = agg_num;

	whc_dev_send((u8 *)msg_info, (u16)(sizeof(struct whc_msg_info) + pad_len + content_len), head_skb, 1);
}
#endif /* WHCH_RXAGG */

#ifdef WHCH_TXAGG
void whch_dev_txagg_dispatch(struct whch_buff *buff, u32 buffidx, u8 *msg_hdr, u32 rx_len, u8 agg_num)
{
	u8 delivered = 0;

	if ((agg_num == 0) || (agg_num > WHCH_TXAGG_NUM)) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "txagg bad agg_num=%d buff=%x\n", agg_num, buff);
		buff->agg_num = 0;
		buff->status = 0;
		goto exit;
	}

	RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_DEBUG, "agg_num=%d\n", agg_num);

	buff->agg_num = agg_num;
	buff->status = (agg_num >= 32) ? 0xFFFFFFFFU : ((1U << agg_num) - 1U);

	delivered = rtw_xmit_txagg_parse(buffidx, msg_hdr, sizeof(struct whc_msg_info), rx_len, agg_num);

exit:
	whch_usb_dev_txagg_buf_busy(buff, delivered, buff->agg_num);

	if (buff->status) {
		/* wakeup task */
		rtw_single_thread_wakeup();
	}
}
#endif /* WHCH_TXAGG */
#endif /* CONFIG_WHCH */

/**
 * @brief  Refresh flow_ctrl_en in the packet header before a TX retry.
 *
 * Only RECV_PKTS packets carry a whc_msg_info header with flow_ctrl_en.
 * All other packet types (EVT_CMD, EVT_API_*, …) use different headers
 * whose byte-4 must not be touched.
 *
 * @param  buf: the same buffer pointer passed to whc_dev_send().
 */
void whc_dev_update_flowctrl(u8 *buf)
{
	struct whc_msg_info *msg_info = (struct whc_msg_info *)buf;
	u8 tmp = 0;

	/* Only RECV_PKTS or EVT_FLOWCTRL packets carry a whc_msg_info header with flow_ctrl_en.
	 * All other packet types (EVT_CMD, EVT_API_*, …) use different headers
	 * whose byte-4 must not be touched. */
	if ((msg_info->event != WHC_WIFI_EVT_RECV_PKTS) && (msg_info->event != WHC_WIFI_EVT_FLOWCTRL)) {
		return;
	}

	whc_dev_flowctrl(&tmp, 0);
	msg_info->flow_ctrl_en = tmp;
}
