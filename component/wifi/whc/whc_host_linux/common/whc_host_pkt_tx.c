#include <whc_host_linux.h>

static int whc_host_enqueue_tx_packet(struct xmit_priv_t *xmit_priv, struct whc_msg_node *p_node)
{
	/* enqueue msg */
	spin_lock(&(xmit_priv->lock));
	list_add_tail(&(p_node->list), &(xmit_priv->queue_head));
	atomic_inc(&xmit_priv->msg_num);
	spin_unlock(&(xmit_priv->lock));

	return 0;
}

struct whc_msg_node *whc_host_dequeue_tx_packet(struct xmit_priv_t *xmit_priv)
{
	struct whc_msg_node *p_node;
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
		p_node = list_entry(plist, struct whc_msg_node, list);
		list_del(&(p_node->list));
		atomic_dec(&xmit_priv->msg_num);
	}

	spin_unlock_irq(&(xmit_priv->lock));

	return p_node;
}

int whc_host_xmit_pending_q_num(void)
{
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;

	return atomic_read(&xmit_priv->msg_num);
}

void whc_host_xmit_wakeup_thread(void)
{
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;

	up(&xmit_priv->tx_sema);
}

int whc_host_xmit_thread(void *data)
{
	struct xmit_priv_t *xmit_priv = (struct xmit_priv_t *)data;
	struct whc_msg_node *p_node = NULL;
	struct sk_buff *pskb;
	int ret = 0;
	int i = 0;

	while (!kthread_should_stop()) {

		/* wait for smea */
		ret = down_interruptible(&xmit_priv->tx_sema);

		/* dequeue msg node */
		while ((!global_idev.mlme_priv.b_in_scan) && ((p_node = whc_host_dequeue_tx_packet(xmit_priv)) != NULL)) {

			pskb = p_node->msg;

			/* send to NP*/
			whc_host_send_data(pskb->data, pskb->len, pskb);

			/* wake tx queue if need */
			if (whc_host_xmit_pending_q_num() < QUEUE_WAKE_THRES) {
				for (i = 0; i < WHC_MAX_NET_PORT_NUM; i++) {
					if (global_idev.pndev[i]) {
						netif_tx_wake_all_queues(global_idev.pndev[i]);
					}
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

int whc_host_xmit_entry(int idx, struct sk_buff *pskb)
{
	int ret = NETDEV_TX_OK;
	bool b_dropped = false;
	struct whc_msg_info *msg = NULL;
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;
	struct net_device_stats *pstats = &global_idev.stats[idx];
	struct net_device *pndev = global_idev.pndev[idx];
	struct whc_msg_node *p_node = NULL;
	u32 need_headroom, pad_len;

	if (!global_idev.host_init_done) {
		dev_err(global_idev.pwhc_dev, "Host xmit err: wifi not init\n");
		return -1;
	}

	if (whc_host_xmit_pending_q_num() >= QUEUE_STOP_THRES) {
		netif_tx_stop_all_queues(pndev);
		if (whc_host_xmit_pending_q_num() >= PKT_DROP_THRES) {
			dev_warn(global_idev.pwhc_dev, "buffered too much pkts, drop!\n");
			b_dropped = true;
			goto exit;
		}
	}

	need_headroom = SIZE_TX_DESC + sizeof(struct whc_msg_info);

	/* buf addr should be 4-byte aligned, because Laptop PCIE to SDIO converter driver don't support non 4byte-aligned transfer */
	pad_len = BUF_ALIGN_SZ ? ((uintptr_t)(pskb->data - need_headroom) % BUF_ALIGN_SZ) : 0;
	need_headroom += pad_len;

	if (skb_headroom(pskb) >= need_headroom) {
		skb_push(pskb, need_headroom);
	} else {
		if (pskb_expand_head(pskb, need_headroom, 0, GFP_ATOMIC)) {
			dev_warn(global_idev.pwhc_dev, "expand headroom FAIL!\n");
			b_dropped = true;
			goto exit;
		}
		skb_push(pskb, need_headroom);
	}

	msg = (struct whc_msg_info *)(pskb->data + SIZE_TX_DESC);
	msg->event = WHC_WIFI_EVT_XIMT_PKTS;
	msg->wlan_idx = idx;
	msg->data_len = pskb->len - need_headroom;
	msg->pad_len = pad_len;

	/* enqueue pkt */
	p_node = kzalloc(sizeof(struct whc_msg_node), GFP_KERNEL);
	p_node ->msg = pskb;

	whc_host_enqueue_tx_packet(xmit_priv, p_node);

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

int whc_host_xmit_init(void)
{
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;

	sema_init(&xmit_priv->tx_sema, 0);
	spin_lock_init(&xmit_priv->lock);

	INIT_LIST_HEAD(&xmit_priv->queue_head);
	atomic_set(&xmit_priv->msg_num, 0);

	xmit_priv->tx_thread = kthread_run(whc_host_xmit_thread, xmit_priv, "RTW_TX_THREAD");
	if (IS_ERR(xmit_priv->tx_thread)) {
		dev_err(global_idev.pwhc_dev, "FAIL to create sdio_tx_thread!\n");
		xmit_priv->tx_thread = NULL;
		return -EINVAL;
	}

	xmit_priv->initialized = 1;

	return 0;
}

int whc_host_xmit_deinit(void)
{
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;
	struct whc_msg_node *p_node = NULL;

	/* stop xmit_buf_thread */
	if (xmit_priv->tx_thread) {
		up(&xmit_priv->tx_sema);
		kthread_stop(xmit_priv->tx_thread);
		xmit_priv->tx_thread = NULL;
	}

	/* de initialize queue */
	while ((p_node = whc_host_dequeue_tx_packet(xmit_priv)) != NULL) {
		/* release the memory */
		kfree(p_node);
		kfree(p_node->msg);
	}

	return 0;
}

