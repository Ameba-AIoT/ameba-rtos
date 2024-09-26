#include <rtw_cfg80211_fullmac.h>

static int enqueue_tx_packet(struct xmit_priv_t *xmit_priv, struct inic_msg_node *p_node)
{
	/* enqueue msg */
	spin_lock(&(xmit_priv->lock));
	list_add_tail(&(p_node->list), &(xmit_priv->queue_head));
	atomic_inc(&xmit_priv->msg_num);
	spin_unlock(&(xmit_priv->lock));

	return 0;
}

struct inic_msg_node *dequeue_tx_packet(struct xmit_priv_t *xmit_priv)
{
	struct inic_msg_node *p_node;
	struct list_head *plist, *phead;

	if (xmit_priv->initialized == 0) {
		return NULL;
	}

	/* stop interrupt interrupting this process to cause dead lock. */
	spin_lock_irq(&(xmit_priv->lock));

	if (list_empty(&(xmit_priv->queue_head)) == true) {
		p_node = NULL;
	} else {
		phead = &(xmit_priv->queue_head);
		plist = phead->next;
		p_node = list_entry(plist, struct inic_msg_node, list);
		list_del(&(p_node->list));
		atomic_dec(&xmit_priv->msg_num);
	}

	spin_unlock_irq(&(xmit_priv->lock));

	return p_node;
}

int llhw_xmit_pending_q_num(void)
{
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;

	return atomic_read(&xmit_priv->msg_num);
}

void llhw_xmit_wakeup_thread(void)
{
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;

	up(&xmit_priv->tx_sema);
}

int llhw_xmit_thread(void *data)
{
	struct xmit_priv_t *xmit_priv = (struct xmit_priv_t *)data;
	struct inic_msg_node *p_node = NULL;
	struct sk_buff *pskb;
	int ret = 0;

	while (!kthread_should_stop()) {

		/* wait for smea */
		down_interruptible(&xmit_priv->tx_sema);

		/* dequeue msg node */
		while ((!global_idev.mlme_priv.b_in_scan) && ((p_node = dequeue_tx_packet(xmit_priv)) != NULL)) {

			pskb = p_node->msg;

			/* send to NP*/
			llhw_send_data(pskb->data, pskb->len, pskb);

			/* wake tx queue if need */
			if (llhw_xmit_pending_q_num() < QUEUE_WAKE_THRES) {
				netif_tx_wake_all_queues(global_idev.pndev[0]);
				if (global_idev.pndev[1]) {
					netif_tx_wake_all_queues(global_idev.pndev[1]);
				}
			}
#ifndef CONFIG_INIC_USB_ASYNC_SEND
			/* release the memory for this message. */
			dev_kfree_skb(pskb);
#endif
			kfree(p_node);
		}
	}

	return ret;
}

int llhw_xmit_entry(int idx, struct sk_buff *pskb)
{
	int ret = NETDEV_TX_OK;
	bool b_dropped = false;
	struct inic_msg_info *msg = NULL;
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;
	struct net_device_stats *pstats = &global_idev.stats[idx];
	struct net_device *pndev = global_idev.pndev[idx];
	struct inic_msg_node *p_node = NULL;
	u32 need_headroom, pad_len;

	if (llhw_xmit_pending_q_num() >= QUEUE_STOP_THRES) {
		netif_tx_stop_all_queues(pndev);
		if (llhw_xmit_pending_q_num() >= PKT_DROP_THRES) {
			dev_warn(global_idev.fullmac_dev, "buffered too much pkts, drop!\n");
			b_dropped = true;
			goto exit;
		}
	}

	need_headroom = SIZE_TX_DESC + sizeof(struct inic_msg_info);

	/* buf addr should be 4-byte aligned, because Laptop PCIE to SDIO converter driver don't support non 4byte-aligned transfer */
	if (BUF_ALIGN_SZ != 0) {
		pad_len = (uintptr_t)(pskb->data - need_headroom) % BUF_ALIGN_SZ;
	} else {
		pad_len = 0;
	}
	need_headroom += pad_len;

	if (skb_headroom(pskb) >= need_headroom) {
		skb_push(pskb, need_headroom);
	} else {
		if (pskb_expand_head(pskb, need_headroom, 0, GFP_ATOMIC)) {
			dev_warn(global_idev.fullmac_dev, "expand headroom FAIL!\n");
			b_dropped = true;
			goto exit;
		}
	}

	msg = (struct inic_msg_info *)(pskb->data + SIZE_TX_DESC);
	msg->event = INIC_WIFI_EVT_XIMT_PKTS;
	msg->wlan_idx = idx;
	msg->data_len = pskb->len - need_headroom;
	msg->pad_len = pad_len;

	/* enqueue pkt */
	p_node = kzalloc(sizeof(struct inic_msg_node), GFP_KERNEL);
	p_node ->msg = pskb;

	enqueue_tx_packet(xmit_priv, p_node);

	/* up sema to notify xmit thread */
	up(&xmit_priv->tx_sema);

	pstats->tx_packets++;
	pstats->tx_bytes += msg->data_len;

exit:
	skb_tx_timestamp(pskb);
	if (b_dropped) {
		pstats->tx_dropped++;
		ret = NETDEV_TX_BUSY;
		/* requeue or free this skb in netdevice, not here. */
	}

	return ret;
}

int llhw_xmit_init(void)
{
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;

	sema_init(&xmit_priv->tx_sema, 0);
	spin_lock_init(&xmit_priv->lock);

	INIT_LIST_HEAD(&xmit_priv->queue_head);
	atomic_set(&xmit_priv->msg_num, 0);

	xmit_priv->tx_thread = kthread_run(llhw_xmit_thread, xmit_priv, "RTW_TX_THREAD");
	if (IS_ERR(xmit_priv->tx_thread)) {
		dev_err(global_idev.fullmac_dev, "FAIL to create sdio_tx_thread!\n");
		xmit_priv->tx_thread = NULL;
		return -EINVAL;
	}

	xmit_priv->initialized = 1;

	return 0;
}

int llhw_xmit_deinit(void)
{
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;
	struct inic_msg_node *p_node = NULL;

	/* stop xmit_buf_thread */
	if (xmit_priv->tx_thread) {
		up(&xmit_priv->tx_sema);
		kthread_stop(xmit_priv->tx_thread);
		xmit_priv->tx_thread = NULL;
	}

	/* de initialize queue */
	while ((p_node = dequeue_tx_packet(xmit_priv)) != NULL) {
		/* release the memory */
		kfree(p_node);
		kfree(p_node->msg);
	}

	return 0;
}

