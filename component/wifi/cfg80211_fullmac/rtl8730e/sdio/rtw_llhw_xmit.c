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

static struct inic_msg_node *dequeue_tx_packet(struct xmit_priv_t *xmit_priv)
{
	struct inic_msg_node *p_node;
	struct list_head *plist, *phead;

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


int llhw_xmit_thread(void *data)
{
	struct xmit_priv_t *xmit_priv = (struct xmit_priv_t *)data;
	struct inic_msg_node *p_node = NULL;
	struct inic_msg_info *msg;
	u8 *buf;
	int ret = 0;

	while (!kthread_should_stop()) {

		/* wait for smea */
		down_interruptible(&xmit_priv->sdio_tx_sema);

		/* dequeue msg node */
		while ((p_node = dequeue_tx_packet(xmit_priv)) != NULL) {

			buf = p_node->msg;
			msg = (struct inic_msg_info *)((u8 *)buf + SIZE_TX_DESC);

			/* send to NP*/
			llhw_host_send((u8 *)buf, SIZE_TX_DESC + sizeof(struct inic_msg_info) + msg->pad_len + msg->data_len);

			/* wake tx queue if need */
			if (atomic_read(&xmit_priv->msg_num) < QUEUE_WAKE_THRES) {
				netif_tx_wake_all_queues(global_idev.pndev[0]);
				if (global_idev.pndev[1]) {
					netif_tx_wake_all_queues(global_idev.pndev[1]);
				}
			}

			/* release the memory for this message. */
			kfree(p_node);
			kfree(buf);
		}
	}

	return ret;
}

int llhw_xmit_entry(int idx, struct sk_buff *pskb)
{
	int ret = NETDEV_TX_OK;
	bool b_dropped = false;
	u8 *buf = NULL;
	struct inic_msg_info *msg = NULL;
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;
	struct net_device_stats *pstats = &global_idev.stats[idx];
	struct net_device *pndev = global_idev.pndev[idx];
	struct inic_msg_node *p_node = NULL;

	if (atomic_read(&xmit_priv->msg_num) >= QUEUE_STOP_THRES) {
		netif_tx_stop_all_queues(pndev);
		if (atomic_read(&xmit_priv->msg_num) >= PKT_DROP_THRES) {
			dev_warn(global_idev.fullmac_dev, "buffered too much pkts, drop!\n");
			b_dropped = true;
			goto exit;
		}
	}

	/* allocate memory and copy data. */
	buf = kzalloc(SIZE_TX_DESC + sizeof(struct inic_msg_info) + pskb->len, GFP_KERNEL);
	if (!buf) {
		b_dropped = true;
		goto exit;
	}

	msg = (struct inic_msg_info *)(buf + SIZE_TX_DESC);
	msg->event = INIC_WIFI_EVT_XIMT_PKTS;
	msg->wlan_idx = idx;
	msg->data_len = pskb->len;
	msg->pad_len = 0;

	memcpy((void *)(msg + 1), pskb->data, pskb->len);

	/* enqueue pkt */
	p_node = kzalloc(sizeof(struct inic_msg_node), GFP_KERNEL);
	p_node ->msg = buf;

	enqueue_tx_packet(xmit_priv, p_node);

	/* up sema to notify xmit thread */
	up(&xmit_priv->sdio_tx_sema);

	pstats->tx_packets++;
	pstats->tx_bytes += msg->data_len;

exit:
	skb_tx_timestamp(pskb);
	if (b_dropped) {
		pstats->tx_dropped++;
		ret = NETDEV_TX_BUSY;
		/* requeue or free this skb in netdevice, not here. */
	} else {
		dev_kfree_skb(pskb);
	}

	return ret;
}

int llhw_xmit_init(void)
{
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;

	sema_init(&xmit_priv->sdio_tx_sema, 0);
	spin_lock_init(&xmit_priv->lock);

	INIT_LIST_HEAD(&xmit_priv->queue_head);
	atomic_set(&xmit_priv->msg_num, 0);

	xmit_priv->sdio_tx_thread = kthread_run(llhw_xmit_thread, xmit_priv, "RTW_TX_THREAD");
	if (IS_ERR(xmit_priv->sdio_tx_thread)) {
		dev_err(global_idev.fullmac_dev, "FAIL to create sdio_tx_thread!\n");
		xmit_priv->sdio_tx_thread = NULL;
		return -EINVAL;
	}

	return 0;
}

int llhw_xmit_deinit(void)
{
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;
	struct inic_msg_node *p_node = NULL;

	/* stop xmit_buf_thread */
	if (xmit_priv->sdio_tx_thread) {
		up(&xmit_priv->sdio_tx_sema);
		kthread_stop(xmit_priv->sdio_tx_thread);
		xmit_priv->sdio_tx_thread = NULL;
	}

	/* de initialize queue */
	while ((p_node = dequeue_tx_packet(xmit_priv)) != NULL) {
		/* release the memory */
		kfree(p_node);
		kfree(p_node->msg);
	}

	return 0;
}


