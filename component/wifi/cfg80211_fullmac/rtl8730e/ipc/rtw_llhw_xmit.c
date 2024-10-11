// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <rtw_cfg80211_fullmac.h>

void llhw_xmit_done(int idx_wlan)
{
	atomic_inc(&global_idev.xmit_priv.skb_free_num);
	if (atomic_read(&global_idev.xmit_priv.skb_free_num) >= QUEUE_WAKE_THRES) {
		//dev_dbg(global_idev.fullmac_dev, "wq %d\n", free_num);
		netif_tx_wake_all_queues(global_idev.pndev[0]);
		netif_tx_wake_all_queues(global_idev.pndev[1]);
	}
}

static struct dev_sk_buff *llhw_find_one_free_skb(u32 *skb_index, bool *b_dropped)
{
	struct dev_sk_buff *skb;
	u32 start_idx = *skb_index;
	int skb_num_ap = global_idev.wifi_user_config.skb_num_ap;

	do {
		*skb_index = (*skb_index + 1) % skb_num_ap;
		skb = &global_idev.xmit_priv.host_skb_buff[*skb_index];
		if (skb->busy == 0) {
			return skb;
		}
	} while (*skb_index != start_idx);

	*b_dropped = true;
	dev_warn(global_idev.fullmac_dev, "[ERROR] not found free skb. free_skb_num=%d\n", atomic_read(&global_idev.xmit_priv.skb_free_num));
	return NULL;
}

int llhw_xmit_entry(int idx, struct sk_buff *pskb)
{
	struct dev_sk_buff *skb = NULL;
	dma_addr_t skb_data_phy = 0, skb_phy = 0;
	int ret = NETDEV_TX_OK;
	u32 skb_index = 0;
	struct net_device *pndev = global_idev.pndev[idx];
	struct net_device_stats *pstats = &global_idev.stats[idx];
	bool b_dropped = false;
	int size = 0;
	struct inic_ipc_ex_msg ipc_msg = {0};
	int skb_num_ap = global_idev.wifi_user_config.skb_num_ap;

	/*s1. check free skb num*/
	if (atomic_read(&global_idev.xmit_priv.skb_free_num) < QUEUE_STOP_THRES) {
		netif_tx_stop_all_queues(pndev);//, skb_get_queue_mapping(pskb));
		if (atomic_read(&global_idev.xmit_priv.skb_free_num) == 0) {
			dev_warn(global_idev.fullmac_dev, "[DEBUG] free_num is zero\n");
			b_dropped = true;
			goto func_exit;
		}
	}

	/*s2. get a skb, use spinklock to prevent more than one threads get a same skb*/
	spin_lock(&(global_idev.xmit_priv.skb_lock));
	skb_index = global_idev.xmit_priv.skb_idx;
	skb = &global_idev.xmit_priv.host_skb_buff[skb_index];
	/*next skb may be busy if NP release skb out of order when tx in different ACs or under softap mode*/
	if ((skb->busy) && ((skb = llhw_find_one_free_skb(&skb_index, &b_dropped)) == NULL)) {
		spin_unlock(&(global_idev.xmit_priv.skb_lock));
		goto func_exit;
	}
	skb->busy = 1;
	global_idev.xmit_priv.skb_idx = (skb_index + 1) % skb_num_ap;/*next skb, for next tx*/
	atomic_dec(&global_idev.xmit_priv.skb_free_num);
	spin_unlock(&(global_idev.xmit_priv.skb_lock));

	/*s3. config skb*/
	skb_phy = global_idev.xmit_priv.host_skb_buff_phy + sizeof(struct dev_sk_buff) * skb_index;
	size = SKB_DATA_ALIGN(pskb->len + SKB_DATA_ALIGN(SKB_WLAN_TX_EXTRA_LEN));
	skb_data_phy = global_idev.xmit_priv.host_skb_buff_phy + sizeof(struct dev_sk_buff) * skb_index + ((u32)(skb->buf) - (u32)skb);
	memcpy(skb->buf + SKB_DATA_ALIGN(SKB_WLAN_TX_EXTRA_LEN), pskb->data, pskb->len);
	skb->head = (unsigned char *)skb_data_phy;
	skb->end = (unsigned char *)(skb_data_phy + size);
	skb->data = (unsigned char *)(skb_data_phy + SKB_DATA_ALIGN(SKB_WLAN_TX_EXTRA_LEN));
	skb->tail = (unsigned char *)(skb_data_phy + SKB_DATA_ALIGN(SKB_WLAN_TX_EXTRA_LEN));
	skb->no_free = 1;
	skb->len = 0;
	skb->list.prev = skb->list.next = skb_phy + offsetof(struct dev_sk_buff, list);
	atomic_set(&skb->ref, 1);
	dev_skb_put(skb, pskb->len);

	/*s4 send to NP*/
	ipc_msg.event_num = IPC_WIFI_CMD_XIMT_PKTS;
	ipc_msg.msg_addr = (u32)skb_phy;
	ipc_msg.wlan_idx = idx;
	llhw_send_packet(&ipc_msg);
	pstats->tx_bytes += pskb->len;

func_exit:
	skb_tx_timestamp(pskb);
	if (b_dropped) {
		ret = NETDEV_TX_BUSY;
		/* requeue or free this skb in netdevice, not here. */
	} else {
		dev_kfree_skb(pskb);
	}

	return ret;
}

int llhw_xmit_init(void)
{
	struct device *pdev = global_idev.ipc_dev;
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;
	int skb_num_ap = global_idev.wifi_user_config.skb_num_ap;

	if (xmit_priv->host_skb_buff) {
		dev_err(global_idev.fullmac_dev, "host_skb_buff not mfree|\n");
		return -ENOMEM;
	}

	xmit_priv->host_skb_buff = (struct dev_sk_buff *)dmam_alloc_coherent(pdev, sizeof(struct dev_sk_buff) * skb_num_ap, &xmit_priv->host_skb_buff_phy, GFP_KERNEL);
	if (!xmit_priv->host_skb_buff) {
		dev_err(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		return -ENOMEM;
	}

	memset(xmit_priv->host_skb_buff, 0, sizeof(struct dev_sk_buff) * skb_num_ap);
	spin_lock_init(&(xmit_priv->skb_lock));
	atomic_set(&global_idev.xmit_priv.skb_free_num, skb_num_ap);

	return 0;
}

void llhw_xmit_deinit(void)
{
	struct device *pdev = global_idev.ipc_dev;
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;
	int skb_num_ap = global_idev.wifi_user_config.skb_num_ap;

	if (xmit_priv->host_skb_buff) {
		dma_free_coherent(pdev, sizeof(struct dev_sk_buff) * skb_num_ap, xmit_priv->host_skb_buff, xmit_priv->host_skb_buff_phy);
		memset(&global_idev.xmit_priv, 0, sizeof(struct xmit_priv_t));
	}
}
