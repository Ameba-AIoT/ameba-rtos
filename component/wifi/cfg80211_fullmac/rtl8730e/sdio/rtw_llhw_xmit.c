#include <rtw_cfg80211_fullmac.h>

void llhw_xmit_task_handler(void *buf)
{
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;
	struct inic_msg_info *msg = (struct inic_msg_info *)((u8 *)buf + SIZE_TX_DESC);

	/* send to NP*/
	llhw_host_send((u8 *)buf, SIZE_TX_DESC + sizeof(struct inic_msg_info) + msg->pad_len + msg->data_len);

	/* free buffer */
	kfree(buf);

	if (atomic_read(&xmit_priv->tx_msg_priv.msg_num) < QUEUE_WAKE_THRES) {
		netif_tx_wake_all_queues(global_idev.pndev[0]);
		netif_tx_wake_all_queues(global_idev.pndev[1]);
	}
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

	if (atomic_read(&xmit_priv->tx_msg_priv.msg_num) >= QUEUE_STOP_THRES) {
		netif_tx_stop_all_queues(pndev);
		if (atomic_read(&xmit_priv->tx_msg_priv.msg_num) >= PKT_DROP_THRES) {
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

	inic_msg_enqueue(&xmit_priv->tx_msg_priv, (void *)buf);

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

	inic_msg_q_init(&xmit_priv->tx_msg_priv, llhw_xmit_task_handler);

	return 0;
}

int llhw_xmit_deinit(void)
{
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;

	inic_msg_q_deinit(&xmit_priv->tx_msg_priv);

	return 0;
}


