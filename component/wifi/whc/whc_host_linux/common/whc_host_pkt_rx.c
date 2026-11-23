#include <whc_host_linux.h>
#ifdef CONFIG_WHC_CMD_PATH
#include <whc_host_cmd_path_api.h>
#include <whc_host_netlink.h>
#endif
#ifdef CONFIG_WHC_HOST_LOG_FWD
#include "whc_host_log_fwd.h"
#endif
/* Guarded include: with the feature off, common/tsf_sync/Kbuild adds no -I path. */
#if defined(CONFIG_WHC_TSF_SYNC)
#include "whc_host_tsf_sync.h"
#endif

void whc_host_recv_notify(void)
{
	up(&global_idev.recv_priv.rx_sema);
}

static void whc_host_recv_pkts(struct sk_buff *pskb)
{
	struct whc_msg_info *msg = (struct whc_msg_info *)(pskb->data + SIZE_RX_DESC);
	u8 wlan_idx = msg->wlan_idx;
	u32 pkt_len = msg->data_len;
	u32 total_len;
	struct net_device_stats *pstats = &global_idev.stats[wlan_idx];
	u8 tmp;

#ifdef CONFIG_P2P
	if (global_idev.p2p_global.pd_wlan_idx == 1) {
		wlan_idx = wlan_idx ^ 1; /*GC intf is up, linux netdev idx is oppsite to driver wlan_idx*/
		pstats = &global_idev.stats[wlan_idx];
	}
#endif

	tmp = global_idev.xmit_priv.flowctrl_en;
	global_idev.xmit_priv.flowctrl_en = msg->flow_ctrl_en;

	if (tmp && msg->flow_ctrl_en == 0) {
		whc_host_xmit_wakeup_thread();
	}

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

#ifdef CONFIG_WHC_CMD_PATH
int whc_host_cmd_data_process(struct sk_buff *pskb)
{
	u32 event = *(u32 *)(pskb->data + SIZE_RX_DESC);
	struct whc_cmd_path_hdr *hdr = NULL;
	u8 *rxbuf = NULL;
	int log_len;
	const char *log_buf;

	switch (event) {
	case WHC_WIFI_EVT_CMD:
		hdr = (struct whc_cmd_path_hdr *)(pskb->data + SIZE_RX_DESC);
		rxbuf = (u8 *)pskb->data + SIZE_RX_DESC + sizeof(struct whc_cmd_path_hdr);
		if (hdr->len >= sizeof(u32) && *(u32 *)rxbuf == WHC_LOG_EVENT) {
			log_len = (int)(hdr->len - sizeof(u32));
			log_buf = (const char *)(rxbuf + sizeof(u32));
			if (log_len > 0) {
				if (log_buf[log_len - 1] == '\n') {
					pr_info("[FW] %.*s", log_len, log_buf);
				} else {
					pr_info("[FW] %.*s\n", log_len, log_buf);
				}
			}
#ifdef CONFIG_WHC_HOST_LOG_FWD
		} else if (hdr->len >= sizeof(u32) + 2 &&
				   *(u32 *)rxbuf == WHC_WIFI_TEST &&
				   rxbuf[4] == WHC_WIFI_TEST_LOG_ACK) {
			whc_host_log_forward_ack(rxbuf[5]);
#endif
#if defined(CONFIG_WHC_TSF_SYNC)
		} else if (hdr->len >= TSF_HDR_LEN + sizeof(struct tsf_sync_sample) &&
				   *(u32 *)rxbuf == WHC_WIFI_TEST &&
				   rxbuf[4] == WHC_WIFI_TEST_TSF_SAMPLE) {
			/* handled entirely in kernel; never forwarded to user space */
			whc_host_tsf_sync_push(rxbuf + TSF_HDR_LEN, sizeof(struct tsf_sync_sample));
#endif
		} else {
			whc_host_deliver_rxbuf_to_user(rxbuf, hdr->len);
		}
		break;
	default:
		break;
	}
	return 0;
}
#endif

int whc_host_recv_dispatch(struct sk_buff *pskb)
{
	int ret = 0;
	u32 event = *(u32 *)(pskb->data + SIZE_RX_DESC);
	struct event_priv_t *event_priv = &global_idev.event_priv;
	struct whc_api_info *ret_msg;
	struct whc_msg_info *msg;

	switch (event) {
	case WHC_WIFI_EVT_RECV_PKTS:
		whc_host_recv_pkts(pskb);
		break;
	case WHC_WIFI_EVT_API_CALL:
		/* queue instead of waiting for the previous API to finish: the handler
		 * may call a device API whose API_RETURN is behind this msg on the bus */
		skb_queue_tail(&event_priv->rx_api_queue, pskb);
		schedule_work(&(event_priv->api_work));
		break;
	case WHC_WIFI_EVT_API_RETURN:
		/* overwriting an occupied slot would drop that msg and still raise a
		 * second completion, leaving a count with no msg behind it */
		spin_lock(&event_priv->api_ret_lock);
		if (event_priv->b_waiting_for_ret && !event_priv->rx_api_ret_msg) {
			event_priv->rx_api_ret_msg = pskb;

			/* unblock API calling func. Must stay inside the lock: releasing it
			 * first lets the caller claim the msg and reinit the completion
			 * before this runs, putting the count out of step again */
			complete(&event_priv->api_ret_sema);
			spin_unlock(&event_priv->api_ret_lock);
		} else {
			spin_unlock(&event_priv->api_ret_lock);

			ret_msg = (struct whc_api_info *)(pskb->data + SIZE_RX_DESC);
			dev_warn(global_idev.pwhc_dev, "too late to receive API ret, ID: 0x%x!\n", ret_msg->api_id);

			/* free rx buffer */
			kfree_skb(pskb);
		}
		break;
	case WHC_WIFI_EVT_FLOWCTRL:
		msg = (struct whc_msg_info *)(pskb->data + SIZE_RX_DESC);
		global_idev.xmit_priv.flowctrl_en = msg->flow_ctrl_en;
		if (!msg->flow_ctrl_en) {
			whc_host_xmit_wakeup_thread();
		}
		/* free rx buffer */
		kfree_skb(pskb);
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
		whc_host_cmd_data_process(pskb);
#elif !defined(CONFIG_WHC_HCI_SPI)
		dev_err(global_idev.pwhc_dev, "%s: unknown event:%d\n", __func__, event);
#endif
		kfree_skb(pskb);
		break;
	}

	return ret;
}

static int whc_host_recv_thread(void *data)
{
	int ret = 0;

	struct recv_priv_t *recv_priv = &global_idev.recv_priv;

	while (!kthread_should_stop()) {

		/* wait for sema*/
		ret = down_interruptible(&recv_priv->rx_sema);

		whc_host_recv_data_process(global_idev.intf_priv);
	}

	return ret;
}

void whc_host_recv_init(void)
{
	struct recv_priv_t *recv_priv = &global_idev.recv_priv;

	/* Create Rx thread */
	sema_init(&recv_priv->rx_sema, 0);

	recv_priv->rx_thread = kthread_run(whc_host_recv_thread, recv_priv, "RTW_RX_THREAD");
	if (IS_ERR(recv_priv->rx_thread)) {
		dev_err(global_idev.pwhc_dev, "FAIL to create llhw_recv_thread!\n");
		recv_priv->rx_thread = NULL;
	}

	recv_priv->initialized = true;
}

void whc_host_recv_deinit(void)
{
	struct recv_priv_t *recv_priv = &global_idev.recv_priv;

	if (recv_priv->rx_thread) {
		up(&recv_priv->rx_sema);
		kthread_stop(recv_priv->rx_thread);
		recv_priv->rx_thread = NULL;
	}
}

