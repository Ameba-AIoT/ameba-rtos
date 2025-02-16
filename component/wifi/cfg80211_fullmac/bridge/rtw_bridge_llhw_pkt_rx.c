#include <rtw_cfg80211_fullmac.h>

static void llhw_recv_pkts(struct sk_buff *pskb)
{
	struct inic_msg_info *msg = (struct inic_msg_info *)(pskb->data + SIZE_RX_DESC);
	u8 wlan_idx = msg->wlan_idx;
	u32 pkt_len = msg->data_len;
	u32 total_len;
	struct net_device_stats *pstats = &global_idev.stats[wlan_idx];

#ifdef CONFIG_P2P
	if (global_idev.p2p_global.pd_wlan_idx == 1) {
		wlan_idx = wlan_idx ^ 1; /*GC intf is up, linux netdev idx is oppsite to driver wlan_idx*/
		pstats = &global_idev.stats[wlan_idx];
	}
#endif

	total_len = SIZE_RX_DESC + sizeof(struct inic_msg_info) + msg->pad_len + msg->data_len;
	if (total_len > skb_end_offset(pskb)) {
		dev_warn(global_idev.fullmac_dev, "receive err pkt, rx pkt len:%d, skb buffer len:%d\n", total_len, skb_end_offset(pskb));

		/* free skb */
		kfree_skb(pskb);
		pstats->rx_dropped++;

		return;
	}

	/* adjust skb pointers */
	skb_reserve(pskb, SIZE_RX_DESC + sizeof(struct inic_msg_info) + msg->pad_len);
	skb_put(pskb, pkt_len);

	pskb->dev = global_idev.pndev[wlan_idx];
	pskb->protocol = eth_type_trans(pskb, global_idev.pndev[wlan_idx]);
	pskb->ip_summed = CHECKSUM_NONE;

	if (netif_rx(pskb) == NET_RX_SUCCESS) {
		pstats->rx_packets++;
		pstats->rx_bytes += pskb->len;
	} else {
		pstats->rx_dropped++;
	}

	return;
}

extern void bridge_llhw_recv_to_user(u8 *rx_buf);

int llhw_recv_process(struct sk_buff *pskb)
{
	int ret = 0;
	u32 event = *(u32 *)(pskb->data + SIZE_RX_DESC);

	switch (event) {
	case INIC_WIFI_EVT_RECV_PKTS:
		llhw_recv_pkts(pskb);
		break;
	default:
		bridge_llhw_recv_to_user((u8 *)pskb->data + SIZE_RX_DESC);
		kfree_skb(pskb);
		break;
	}

	return ret;
}

static int llhw_recv_thread(void *data)
{
	int ret = 0;

	struct recv_priv_t *recv_priv = &global_idev.recv_priv;

	while (!kthread_should_stop()) {

		/* wait for sema*/
		down_interruptible(&recv_priv->rx_sema);

		llhw_recv_data_process(global_idev.intf_priv);
	}

	return ret;
}

void llhw_recv_notify(void)
{
	up(&global_idev.recv_priv.rx_sema);
}

void llhw_recv_init(void)
{
	struct recv_priv_t *recv_priv = &global_idev.recv_priv;

	/* Create Rx thread */
	sema_init(&recv_priv->rx_sema, 0);

	recv_priv->rx_thread = kthread_run(llhw_recv_thread, recv_priv, "RTW_RX_THREAD");
	if (IS_ERR(recv_priv->rx_thread)) {
		dev_err(global_idev.fullmac_dev, "FAIL to create llhw_recv_thread!\n");
		recv_priv->rx_thread = NULL;
	}

	recv_priv->initialized = true;
}

void llhw_recv_deinit(void)
{
	struct recv_priv_t *recv_priv = &global_idev.recv_priv;

	if (recv_priv->rx_thread) {
		up(&recv_priv->rx_sema);
		kthread_stop(recv_priv->rx_thread);
		recv_priv->rx_thread = NULL;
	}
}


