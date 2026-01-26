#include <whc_host_linux.h>
#include <whc_host_cmd_path_api.h>

static void whc_host_recv_pkts(struct sk_buff *pskb)
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
		dev_warn(global_idev.pwhc_dev, "receive err pkt, rx pkt len:%d, skb buffer len:%d\n", total_len, skb_end_offset(pskb));

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

int whc_fullmac_host_recv_process(struct sk_buff *pskb)
{
	int ret = 0;
	u32 event = *(u32 *)(pskb->data + SIZE_RX_DESC);
	struct event_priv_t *event_priv = &global_idev.event_priv;
	struct whc_api_info *ret_msg;

	switch (event) {
	case WHC_WIFI_EVT_RECV_PKTS:
		whc_host_recv_pkts(pskb);
		break;
	case WHC_WIFI_EVT_API_CALL:
		event_priv->rx_api_msg = pskb;
		schedule_work(&(event_priv->api_work));
		break;
	case WHC_WIFI_EVT_API_RETURN:
		if (event_priv->b_waiting_for_ret) {
			event_priv->rx_api_ret_msg = pskb;

			/* unblock API calling func */
			complete(&event_priv->api_ret_sema);
		} else {
			ret_msg = (struct whc_api_info *)(pskb->data + SIZE_RX_DESC);
			dev_warn(global_idev.pwhc_dev, "too late to receive API ret, ID: 0x%x!\n", ret_msg->api_id);

			/* free rx buffer */
			kfree_skb(pskb);
		}
		break;
	case WHC_CUST_EVT:
		whc_host_recv_cust_evt((u8 *)pskb->data + SIZE_RX_DESC);
		break;
#ifdef CONFIG_BT_INIC
	case INIC_BT_HOST_RX:
		bt_recv_pkts(pskb);
		break;
#endif
	default:
#if defined(CONFIG_WHC_CMD_PATH)
		whc_host_cmd_data_rx_to_user(pskb);
#elif !defined(CONFIG_FULLMAC_HCI_SPI)
		dev_err(global_idev.pwhc_dev, "%s: unknown event:%d\n", __func__, event);
#endif
		kfree_skb(pskb);
		break;
	}

	return ret;
}

