#include <rtw_cfg80211_fullmac.h>

void llhw_recv_pkts(void *buf)
{
	struct sk_buff *pskb = NULL;
	struct inic_device *idev = &global_idev;
	struct inic_msg_info *msg = (struct inic_msg_info *) buf;
	u8 wlan_idx = msg->wlan_idx;
	u32 pkt_len;
	struct net_device_stats *pstats = &global_idev.stats[wlan_idx];

#ifdef CONFIG_P2P
	if (global_idev.p2p_global.pd_wlan_idx == 1) {
		wlan_idx = wlan_idx ^ 1; /*GC intf is up, linux netdev idx is oppsite to driver wlan_idx*/
		pstats = &global_idev.stats[wlan_idx];
	}
#endif

	/* allocate skb to store ethernet data from sdio. */
	pkt_len = msg->data_len;
	pskb = netdev_alloc_skb(idev->pndev[wlan_idx], pkt_len);
	if (pskb == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: Alloc skb rx buf Err, alloc_sz %d!!\n\r",
				__func__, pkt_len);
		goto exit;
	}

	/* copy data from msg to local skb */
	memcpy(pskb->data, (u8 *)(msg + 1) + msg->pad_len, pkt_len);
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

exit:
	if (msg) {
		llhw_free_rxbuf((u8 *)msg);
	}

	return;
}

unsigned int llhw_recv_handler(u8 *rxbuf)
{
	int  ret = 0;
	u32 event = *(u32 *)rxbuf;
	struct event_priv_t *event_priv = &global_idev.event_priv;
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;

	if (!event_priv || !xmit_priv) {
		dev_err(global_idev.fullmac_dev, "%s: event_priv or xmit_priv is NULL in interrupt!\n", "event");
		goto func_exit;
	}

	switch (event) {
	case INIC_WIFI_EVT_RECV_PKTS:
		/* put the message to the queue */
		ret = inic_msg_enqueue(&global_idev.msg_priv, (void *)rxbuf);
		break;
	case INIC_WIFI_EVT_API_CALL:
		event_priv->rx_api_msg = rxbuf;
		schedule_work(&(event_priv->api_work));
		break;
	case INIC_WIFI_EVT_API_RETURN:
		event_priv->rx_api_ret_msg = rxbuf;

		/* unblock API calling func */
		complete(&event_priv->api_ret_sema);
		break;
	default:
		dev_err(global_idev.fullmac_dev, "%s: unknown event:%d\n", __func__, event);
	}

func_exit:
	return ret;
}

