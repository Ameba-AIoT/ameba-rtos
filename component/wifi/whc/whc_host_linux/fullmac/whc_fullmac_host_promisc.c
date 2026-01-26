// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2024, Realtek Corporation. All rights reserved.
*/

#include <whc_host_linux.h>

static int rtw_promisc_rx_80211_radiotap(struct rtw_rx_pkt_info *ppktinfo)
{
	struct sk_buff *pskb = NULL;
	struct ieee80211_radiotap_header *rtap_hdr = NULL;
	int rtap_len = 0, present_len = 0;
	uint8_t *pos = NULL;
	uint32_t present = 0 ;
	int freq = 0;

	dev_dbg(global_idev.pwhc_dev, "[fullmac]: %s enter\n", __func__);
	freq = rtw_ch2freq(ppktinfo->channel);
	present_len = sizeof(uint8_t); /* Data rate */
	present |= (1 << IEEE80211_RADIOTAP_RATE);
	if (freq != 0) {
		present_len += sizeof(uint16_t) + sizeof(uint16_t); /* Channel frequency and Channel flags */
		present |= (1 << IEEE80211_RADIOTAP_CHANNEL);
	}
	if (ppktinfo->recv_signal_power != 0) {
		present_len += sizeof(int8_t); /* RSSI */
		present |= (1 << IEEE80211_RADIOTAP_DBM_ANTSIGNAL);
	}
	rtap_len = (sizeof(struct ieee80211_radiotap_header) + present_len);

	/* allocate skb to store ethernet data from IPC. */
	pskb = netdev_alloc_skb(global_idev.pndev[0], ppktinfo->len + rtap_len);
	if (pskb == NULL) {
		dev_err(global_idev.pwhc_dev, "%s: Alloc skb rx buf Err, alloc_sz %d!!\n\r",
				__func__, ppktinfo->len);
		return -ENOMEM;
	}

	skb_reserve(pskb, rtap_len); /* reserve the buffer for Radiotap Header */
	skb_put(pskb, ppktinfo->len);
	/* cpoy data from skb(ipc data) to local skb */
	memcpy(pskb->data, ppktinfo->buf, ppktinfo->len);

	/* get the radiotap header from skb buffer */
	rtap_hdr = (struct ieee80211_radiotap_header *)skb_push(pskb, rtap_len);
	pos = (uint8_t *)rtap_hdr + sizeof(struct ieee80211_radiotap_header);

	/* fill the radiotap header */
	rtap_hdr->it_version = 0;
	rtap_hdr->it_pad = 0;
	rtap_hdr->it_len = cpu_to_le16((uint32_t)rtap_len);
	rtap_hdr->it_present = cpu_to_le32(present);

	/* fill data rate info */
	*pos = ppktinfo->data_rate;
	pos += sizeof(uint8_t);

	if (freq != 0) {
		/* fill the channel info */
		*((uint16_t *)pos) = cpu_to_le16((uint16_t)freq);
		pos += sizeof(uint16_t);
		*((uint16_t *)pos) = 0;
		if (ppktinfo->channel > 14) {
			*((uint16_t *)pos) |= cpu_to_le16((IEEE80211_CHAN_5GHZ));
		} else {
			*((uint16_t *)pos) |= cpu_to_le16((IEEE80211_CHAN_2GHZ));
		}
		if ((ppktinfo->data_rate == RTW_RATE_1M) || \
			(ppktinfo->data_rate == RTW_RATE_2M) || \
			(ppktinfo->data_rate == RTW_RATE_5_5M) || \
			(ppktinfo->data_rate == RTW_RATE_11M)) {
			*((uint16_t *)pos) |= cpu_to_le16((IEEE80211_CHAN_CCK));
		} else {
			*((uint16_t *)pos) |= cpu_to_le16((IEEE80211_CHAN_OFDM));
		}
		pos += sizeof(uint16_t);
	}

	if (ppktinfo->recv_signal_power != 0) {
		/* fill rssi info */
		*((int8_t *)pos) = ppktinfo->recv_signal_power;
	}

	skb_reset_mac_header(pskb);
	pskb->dev = global_idev.pndev[0];
	pskb->pkt_type = PACKET_OTHERHOST;
	pskb->protocol = htons(0x0019); /* ETH_P_80211_RAW */;
	pskb->ip_summed = CHECKSUM_NONE;
	netif_rx(pskb);

	return 0;
}

static int rtw_promisc_rx_ether(struct rtw_rx_pkt_info *ppktinfo)
{
	struct sk_buff *pskb = NULL;

	dev_dbg(global_idev.pwhc_dev, "[fullmac]: %s enter\n", __func__);
	/* allocate skb to store ethernet data from IPC. */
	pskb = netdev_alloc_skb(global_idev.pndev[0], ppktinfo->len);
	if (pskb == NULL) {
		dev_err(global_idev.pwhc_dev, "%s: Alloc skb rx buf Err, alloc_sz %d!!\n\r",
				__func__, ppktinfo->len);
		return -ENOMEM;
	}

	/* cpoy data from skb(ipc data) to local skb */
	memcpy(pskb->data, ppktinfo->buf, ppktinfo->len);
	skb_put(pskb, ppktinfo->len);
	skb_reset_mac_header(pskb);
	pskb->dev = global_idev.pndev[0];
	pskb->pkt_type = PACKET_OTHERHOST;
	pskb->protocol = htons(0x0019);
	pskb->ip_summed = CHECKSUM_NONE;
	netif_rx(pskb);

	return 0;
}

int rtw_promisc_rx(struct rtw_rx_pkt_info *ppktinfo)
{
	int ret = 0;
	struct net_device *ndev = global_idev.pndev[0];

	if (ndev == NULL) {
		dev_err(global_idev.pwhc_dev, "%s: ndev is NULL!\n\r",
				__func__);
		return -EINVAL;
	}

	if (ndev->type == ARPHRD_IEEE80211_RADIOTAP) {
		ret = rtw_promisc_rx_80211_radiotap(ppktinfo);
	} else {
		ret = rtw_promisc_rx_ether(ppktinfo);
	}

	return ret;
}
