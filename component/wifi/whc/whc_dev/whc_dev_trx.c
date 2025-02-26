/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include "whc_dev.h"

struct xmit_priv_t dev_xmit_priv;

/**
 * @brief  to handle the inic message from the host. They incude tx data and
 *	command.
 * @param  pxmitbuf[inout]: the xmit buffer from xmit queue.
 * @return none.
 */
static void whc_dev_xmit_tasklet_handler(void *msg)
{
	struct sk_buff *skb = (struct sk_buff *) msg;

	dev_xmit_priv.tx_bytes += skb->len;
	dev_xmit_priv.tx_pkts++;
	wifi_if_send_skb((int)skb->dev, skb);

	return;
}

/**
 * @brief  rx task to handle the rx data, get the data from the rx queue and
 * 	send to upper layer.
 * @param  none.
 * @return none.
 */
static void whc_dev_xmit_tasklet(void)
{
	struct whc_msg_node *p_node = NULL;
	struct __queue *p_xmit_queue = NULL;

	p_xmit_queue = &dev_xmit_priv.xmit_queue;
	do {
		rtos_sema_take(dev_xmit_priv.xmit_sema, 0xFFFFFFFF);

		/* get the data from tx queue. */
		p_node = whc_msg_dequeue(p_xmit_queue);
		while (p_node) {
			whc_dev_xmit_tasklet_handler(p_node->msg);

			/* release node */
			rtos_mem_free((u8 *)p_node);

			/* get next item */
			p_node = whc_msg_dequeue(p_xmit_queue);
		}
	} while (1);

	rtos_task_delete(NULL);
}

/* ---------------------------- Public Functions ---------------------------- */

/**
 * @brief  to initialize the parameters of xmit.
 * @param  none.
 * @return none.
 */
void whc_dev_init_priv(void)
{
	/* initialize the sema of tx. */
	rtos_sema_create_static(&dev_xmit_priv.xmit_sema, 0, 0xFFFFFFFF);

	/* initialize queue. */
	rtw_init_queue(&(dev_xmit_priv.xmit_queue));

	dev_xmit_priv.tx_bytes = 0;
	dev_xmit_priv.tx_pkts = 0;

	/* Initialize the TX task */
	/*modify single thread task's priority lower than INIC XMIT, https://jira.realtek.com/browse/AMEBALITE-434*/
	if (SUCCESS != rtos_task_create(NULL, (const char *const)"inic_xmit_tasklet", (rtos_task_function_t)whc_dev_xmit_tasklet, NULL, 1024 * 4,
									2)) {
		RTK_LOGI(NOTAG, "Create inic_xmit_tasklet Err!!\n");
	}
}

/**
 * @brief  receiving function to send the received data to host.
 * @param  idx_wlan[in]: which port of wifi to set.
 * @return none.
 */
void whc_dev_recv(int idx)
{
	struct sk_buff *skb = NULL;
	u8 *ptr;
	u8 pad_len;
	struct whc_msg_info *msg_info = NULL;
	struct whc_txbuf_info_t *inic_tx;

#if defined(CONFIG_FULLMAC_BRIDGEB) || defined(CONFIG_FULLMAC_BRIDGE)
	if (whc_bridge_dev_recv_pkt_process((u8 *)&idx, &skb) == 0) {
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

	/* construct struct whc_buf_info & whc_buf_info_t */
	inic_tx = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));
	if (!inic_tx) {
		RTK_LOGE(TAG_WLAN_INIC, "fail to alloc struct whc_txbuf_info_t!\n");
		return;
	}

	inic_tx->txbuf_info.buf_allocated = inic_tx->txbuf_info.buf_addr = (u32)msg_info;
	inic_tx->txbuf_info.size_allocated = inic_tx->txbuf_info.buf_size = sizeof(struct whc_msg_info) + pad_len + skb->len;

	inic_tx->ptr = skb;
	inic_tx->is_skb = 1;

	/* send msg_info + pad + rx_pkt_data(skb->data, skb->len) */
	whc_dev_send(&inic_tx->txbuf_info);
}

void whc_dev_tx_done(int idx)
{
	(void)idx;
}

