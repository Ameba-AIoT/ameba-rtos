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

void llhw_recv_task_from_msg_recv_pkts(int idx_wlan, struct dev_sk_buff *skb_phy)
{
	struct sk_buff *pskb = NULL;
	struct inic_ipc_ex_msg ipc_msg = {0};
	struct device *pdev = global_idev.ipc_dev;
	struct inic_device *idev = &global_idev;
	struct dev_sk_buff *skb = NULL;
	u8 *data_addr = NULL;
	struct net_device_stats *pstats = &global_idev.stats[idx_wlan];

#ifdef CONFIG_P2P
	if (global_idev.p2p_global.pd_wlan_idx == 1) {
		idx_wlan = idx_wlan ^ 1; /*GC intf is up, linux netdev idx is oppsite to driver wlan_idx*/
		pstats = &global_idev.stats[idx_wlan];
	}
#endif

	/* get the rx queue. */
	if (!pdev) {
		dev_err(global_idev.fullmac_dev, "%s: device or inic device is NULL!\n", __func__);
		goto func_exit;
	}

	if (!skb_phy) {
		dev_err(global_idev.fullmac_dev, "%s: skb_phy is NULL", __func__);
		goto func_exit;
	}

	skb = llhw_ipc_fw_phy_to_virt((unsigned long)skb_phy);
	data_addr = llhw_ipc_fw_phy_to_virt((unsigned long)skb->data);
	/* allocate pbuf to store ethernet data from IPC. */
	pskb = netdev_alloc_skb(idev->pndev[idx_wlan], skb->len);
	if (pskb == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: Alloc skb rx buf Err, alloc_sz %d!!\n\r",
				__func__, skb->len);
		goto recv_done;
	}

	/* cpoy data from skb(ipc data) to local skb */
	memcpy(pskb->data, data_addr, skb->len);
	skb_put(pskb, skb->len);

	pskb->dev = global_idev.pndev[idx_wlan];
	pskb->protocol = eth_type_trans(pskb, global_idev.pndev[idx_wlan]);
	pskb->ip_summed = CHECKSUM_NONE;

	if (netif_rx(pskb) == NET_RX_SUCCESS) {
		pstats->rx_bytes += pskb->len;
	}

recv_done:
	ipc_msg.event_num = IPC_WIFI_MSG_RECV_DONE;
	ipc_msg.msg_addr = (u32)skb_phy;

	llhw_send_packet(&ipc_msg);

func_exit:
	return;
}

void llhw_recv_task_from_msg(u8 event_num, u32 msg_addr, u8 wlan_idx)
{
	switch (event_num) {
	/* receive the data from device */
	case IPC_WIFI_EVT_RECV_PKTS:
		llhw_recv_task_from_msg_recv_pkts(wlan_idx, (struct dev_sk_buff *)(msg_addr));
		break;
	default:
		dev_err(global_idev.fullmac_dev, "Host Unknown event(%d)!\n\r", event_num);
		break;
	}
}

static unsigned int llhw_recv_interrupt(aipc_ch_t *ch, ipc_msg_struct_t *pmsg)
{
	struct inic_ipc_ex_msg *p_ipc_msg = (struct inic_ipc_ex_msg *)llhw_ipc_fw_phy_to_virt(pmsg->msg);
	struct device *pdev = NULL;
	int  ret = 0;

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		ret = -ENODEV;
		dev_err(global_idev.fullmac_dev, "%s:%s ipc device is null (%d).\n", "ipc recv", __func__, ret);
		goto func_exit;
	}

	if (p_ipc_msg->event_num == IPC_WIFI_EVT_TX_DONE) {
		llhw_xmit_done(p_ipc_msg->wlan_idx);
		ret = 0;
	} else {
		/* put the ipc message to the queue */
		ret = inic_msg_enqueue(p_ipc_msg);
	}

	/* enqueuing message is seccussful, send acknowledgement to another
	 * port. */
	p_ipc_msg->event_num = IPC_WIFI_MSG_READ_DONE;
	if (ret == 0) {
		p_ipc_msg->msg_queue_status = 0;
	} else {
		p_ipc_msg->msg_queue_status = IPC_WIFI_MSG_MEMORY_NOT_ENOUGH;
	}

func_exit:
	return ret;
}

struct aipc_ch_ops llhw_ipc_recv_ops = {
	.channel_recv = llhw_recv_interrupt,
};
