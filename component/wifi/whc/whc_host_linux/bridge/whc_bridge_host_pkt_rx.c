#include <whc_host_linux.h>
#include <whc_bridge_host_intf.h>

static void whc_bridge_host_recv_pkts(struct sk_buff *pskb)
{
	struct whc_msg_info *msg = (struct whc_msg_info *)(pskb->data + SIZE_RX_DESC);
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

	total_len = SIZE_RX_DESC + sizeof(struct whc_msg_info) + msg->pad_len + msg->data_len;
	if (total_len > skb_end_offset(pskb)) {
		dev_warn(global_idev.fullmac_dev, "receive err pkt, rx pkt len:%d, skb buffer len:%d\n", total_len, skb_end_offset(pskb));

		/* free skb */
		kfree_skb(pskb);
		pstats->rx_dropped++;

		return;
	}

	/* adjust skb pointers */
	skb_reserve(pskb, SIZE_RX_DESC + sizeof(struct whc_msg_info) + msg->pad_len);
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

int whc_bridge_host_recv_process(struct sk_buff *pskb)
{
	int ret = 0;
	u32 event = *(u32 *)(pskb->data + SIZE_RX_DESC);

	switch (event) {
	case WHC_WIFI_EVT_RECV_PKTS:
		whc_bridge_host_recv_pkts(pskb);
		break;
	default:
		whc_bridge_host_pkt_rx_to_user(pskb);
		kfree_skb(pskb);
		break;
	}

	return ret;
}
