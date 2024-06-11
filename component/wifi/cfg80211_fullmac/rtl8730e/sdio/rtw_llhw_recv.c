#include <rtw_cfg80211_fullmac.h>

static void llhw_recv_pkts(struct sk_buff *pskb)
{
	struct inic_msg_info *msg = (struct inic_msg_info *)(pskb->data + SIZE_RX_DESC);
	u8 wlan_idx = msg->wlan_idx;
	u32 pkt_len = msg->data_len;
	struct net_device_stats *pstats = &global_idev.stats[wlan_idx];

#ifdef CONFIG_P2P
	if (global_idev.p2p_global.pd_wlan_idx == 1) {
		wlan_idx = wlan_idx ^ 1; /*GC intf is up, linux netdev idx is oppsite to driver wlan_idx*/
		pstats = &global_idev.stats[wlan_idx];
	}
#endif

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

static int llhw_recv_handler(struct sk_buff *pskb)
{
	int ret = 0;
	u32 event = *(u32 *)(pskb->data + SIZE_RX_DESC);
	struct event_priv_t *event_priv = &global_idev.event_priv;

	switch (event) {
	case INIC_WIFI_EVT_RECV_PKTS:
		llhw_recv_pkts(pskb);
		break;
	case INIC_WIFI_EVT_API_CALL:
		event_priv->rx_api_msg = pskb;
		schedule_work(&(event_priv->api_work));
		break;
	case INIC_WIFI_EVT_API_RETURN:
		event_priv->rx_api_ret_msg = pskb;

		/* unblock API calling func */
		complete(&event_priv->api_ret_sema);
		break;
	default:
		dev_err(global_idev.fullmac_dev, "%s: unknown event:%d\n", __func__, event);
	}

	return ret;
}

static struct sk_buff *llhw_recv_rxfifo(struct inic_sdio *priv, u32 size)
{
	u32 allocsize, ret;
	u32 retry = 0;
	struct sk_buff *pskb = NULL;

	allocsize = _RND(size, priv->func->cur_blksize);

	pskb = netdev_alloc_skb(NULL, allocsize);
	if (pskb == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: Alloc skb rx buf Err, alloc_sz %d!!\n\r",
				__func__, allocsize);
		return NULL;
	}

	while (1) {
		ret = sdio_read_port(priv, SDIO_RX_FIFO_DOMAIN_ID, size, pskb->data);
		if (ret == true) {
			break;
		} else {
			/* retry to reduce impact of bus err */
			if (retry++ > 10) {
				kfree_skb(pskb);
				dev_err(&priv->func->dev, "%s: read port FAIL!\n", __FUNCTION__);
				return NULL;
			};
		}
	}

	//print_hex_dump_bytes("llhw_recv_rxfifo: ", DUMP_PREFIX_NONE, rx_buf, size);

	return pskb;
}

static int llhw_recv_thread(void *data)
{
	u8 tmp[4];
	struct sk_buff *pskb;
	u32 rx_len_rdy, himr;
	u16 SdioRxFIFOSize;
	u8 retry = 0;
	int ret = 0;

	struct recv_priv_t *recv_priv = &global_idev.recv_priv;
	struct inic_sdio *sdio_priv = global_idev.sdio_priv;

	while (!kthread_should_stop()) {

		/* wait for sema*/
		down_interruptible(&recv_priv->sdio_rx_sema);

		/* wakeup device if it's sleep */
		if (sdio_priv->dev_state == PWR_STATE_SLEEP) {
			dev_dbg(&sdio_priv->func->dev, "%s: wakeup device", __func__);
			if (rtw_resume_common(sdio_priv)) {
				dev_err(&sdio_priv->func->dev, "%s: fail to wakeup device, stop read rxfifo", __func__);
				continue;
			}
		}

		/* disable RX_REQ interrupt */
		himr = cpu_to_le32(sdio_priv->sdio_himr & (~SDIO_HIMR_RX_REQUEST_MSK));
		sdio_local_write(sdio_priv, SDIO_REG_HIMR, 4, (u8 *)&himr);

		do {
			/* validate RX_LEN_RDY before reading RX0_REQ_LEN */
			rx_len_rdy = sdio_read8(sdio_priv, SDIO_REG_RX0_REQ_LEN + 3) & BIT(7);

			if (rx_len_rdy) {
				sdio_local_read(sdio_priv, SDIO_REG_RX0_REQ_LEN, 4, tmp);
				SdioRxFIFOSize = le16_to_cpu(*(u16 *)tmp);

				if (SdioRxFIFOSize == 0) {
					if (retry ++ < 3) {
						continue;
					} else {
						break;
					}
				} else {
					retry = 0;
					pskb = llhw_recv_rxfifo(sdio_priv, SdioRxFIFOSize);
					if (pskb) {
						/* skip RX_DESC */
						llhw_recv_handler(pskb);
					} else {
						break;
					}
				}
			} else {
				break;
			}
		} while (1);

		/* restore RX_REQ interrupt*/
		himr = cpu_to_le32(sdio_priv->sdio_himr);
		sdio_local_write(sdio_priv, SDIO_REG_HIMR, 4, (u8 *)&himr);
	}

	return ret;
}

void llhw_recv_notify(void)
{
	up(&global_idev.recv_priv.sdio_rx_sema);
}

void llhw_recv_init(void)
{
	struct recv_priv_t *recv_priv = &global_idev.recv_priv;

	/* Create Rx thread */
	sema_init(&recv_priv->sdio_rx_sema, 0);

	recv_priv->sdio_rx_thread = kthread_run(llhw_recv_thread, recv_priv, "RTW_RX_THREAD");
	if (IS_ERR(recv_priv->sdio_rx_thread)) {
		dev_err(global_idev.fullmac_dev, "FAIL to create llhw_recv_thread!\n");
		recv_priv->sdio_rx_thread = NULL;
	}
}

void llhw_recv_deinit(void)
{
	struct recv_priv_t *recv_priv = &global_idev.recv_priv;

	if (recv_priv->sdio_rx_thread) {
		up(&recv_priv->sdio_rx_sema);
		kthread_stop(recv_priv->sdio_rx_thread);
		recv_priv->sdio_rx_thread = NULL;
	}
}

