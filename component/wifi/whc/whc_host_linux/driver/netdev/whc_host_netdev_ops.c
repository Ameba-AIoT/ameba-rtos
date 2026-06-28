// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <whc_host_linux.h>

/*
 * Set wlan0 mac address: 1) ip link set dev wlan0 down, 2) ip link set dev wlan0 address 86:EF:EB:92:A2:FF, 3) ip link set dev wlan0 up
 * Set wlan1 mac address: 1) ifconfig wlan1 up, 2) ip link set dev wlan0 address 86:EF:EB:92:A2:F0, 3) hostapd /etc/hostapd.conf -B -i wlan1
 * NOTE for Soft AP: when wlan1 down, cannot set mac address. but if mac address changed, need restart hostapd. Recomment to change bssid inside hostapd.conf.
*/
int rtw_ndev_set_mac_address(struct net_device *pnetdev, void *p)
{
	int ret = 0;
#if defined(CONFIG_WHC_WIFI_API_PATH)
	struct sockaddr *addr = p;

	if (rtw_netdev_idx(pnetdev) >= WHC_MAX_NET_PORT_NUM) {
		dev_err(global_idev.pwhc_dev, "Netdevice index err.");
		return -1;
	}

	dev_dbg(global_idev.pwhc_dev,
			"[netdev] %s: set wlan-%d mac addr to [%02x:%02x:%02x:%02x:%02x:%02x]", __func__, rtw_netdev_idx(pnetdev),
			*(u8 *)(addr->sa_data), *(u8 *)(addr->sa_data + 1), *(u8 *)(addr->sa_data + 2), *(u8 *)(addr->sa_data + 3), *(u8 *)(addr->sa_data + 4),
			*(u8 *)(addr->sa_data + 5));

	ret = whc_host_set_mac_addr(rtw_netdev_idx(pnetdev), addr->sa_data);

	if (ret == 0) {
		/* Set mac address success, then change the dev_addr inside net_device. */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0))
		memcpy((void *)global_idev.pndev[rtw_netdev_idx(pnetdev)]->dev_addr, addr->sa_data, ETH_ALEN);
#else
		eth_hw_addr_set(global_idev.pndev[rtw_netdev_idx(pnetdev)], addr->sa_data);
#endif
	}
#endif
	return ret;
}

struct net_device_stats *rtw_ndev_get_stats(struct net_device *pnetdev)
{
	dev_dbg(global_idev.pwhc_dev, "[netdev]: %s", __func__);
	return &global_idev.stats[rtw_netdev_idx(pnetdev)];
}

/* Given a data frame determine the 802.1p/1d tag to use. */
static unsigned int rtw_classify8021d(struct sk_buff *skb)
{
	unsigned int dscp;

	/* skb->priority values from 256->263 are magic values to
	 * directly indicate a specific 802.1d priority.  This is used
	 * to allow 802.1d priority to be passed directly in from VLAN
	 * tags, etc.
	 */
	if (skb->priority >= 256 && skb->priority <= 263) {
		return skb->priority - 256;
	}

	switch (skb->protocol) {
	case htons(0x0800):
		dscp = ip_hdr(skb)->tos & 0xfc;
		break;
	default:
		return 0;
	}

	return dscp >> 5;
}

static u8 qos_acm(u8 acm_mask, u8 priority)
{
	u8 change_priority = priority;

	switch (priority) {
	case 0:
	case 3:
		if (acm_mask & BIT(1)) {
			change_priority = 1;
		}
		break;
	case 1:
	case 2:
		break;
	case 4:
	case 5:
		if (acm_mask & BIT(2)) {
			change_priority = 0;
		}
		break;
	case 6:
	case 7:
		if (acm_mask & BIT(3)) {
			change_priority = 5;
		}
		break;
	default:
		dev_warn(global_idev.pwhc_dev, "qos_acm(): invalid pattrib->priority: %d!!!\n", priority);
		break;
	}

	return change_priority;
}

/*
 * AC to queue mapping
 *
 * AC_VO -> queue 0
 * AC_VI -> queue 1
 * AC_BE -> queue 2
 * AC_BK -> queue 3
 */
u16 rtw_ndev_select_queue(struct net_device *pnetdev, struct sk_buff *skb, struct net_device *sb_dev)
{
	/* int acm_mask = 0;*/
	u16 rtw_1d_to_queue[8] = { 2, 3, 3, 2, 1, 1, 0, 0 };

	skb->priority = rtw_classify8021d(skb);

	/* acm_mask != 0 is qos packet. tmp. */
	/*if (acm_mask != 0) {
		skb->priority = qos_acm(acm_mask, skb->priority);
	}*/

	return rtw_1d_to_queue[skb->priority];
}

int rtw_ndev_init(struct net_device *pnetdev)
{
	dev_dbg(global_idev.pwhc_dev, "[whc]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));
#ifdef CONFIG_WHCH
	whc_host_sta_init(rtw_netdev_idx(pnetdev));
	whc_host_hal_pending_q_init(rtw_netdev_idx(pnetdev));
	whc_host_init_default_value(rtw_netdev_idx(pnetdev));
	sema_init(&global_idev.whchpriv.netif_rx_sema[rtw_netdev_idx(pnetdev)], 1);
#endif
	return 0;
}

void rtw_ndev_uninit(struct net_device *pnetdev)
{
	dev_dbg(global_idev.pwhc_dev, "[whc]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));
}

static int rtw_ndev_open(struct net_device *pnetdev)
{
	dev_dbg(global_idev.pwhc_dev, "[whc]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));
	rtw_netdev_priv_is_on(pnetdev) = true;
#ifdef CONFIG_NAN
	if (rtw_netdev_idx(pnetdev) == 2) {
		netif_carrier_on(pnetdev);
	}
#endif
	netif_tx_start_all_queues(pnetdev);
	netif_tx_wake_all_queues(pnetdev);

	return 0;
}

static int rtw_ndev_close(struct net_device *pnetdev)
{
#if defined(CONFIG_WHC_WIFI_API_PATH)
	struct cfg80211_scan_info info;
	int ret = 0;
	u8 is_mp = 0;
#endif

	dev_dbg(global_idev.pwhc_dev, "[whc]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));
#if defined(CONFIG_WHC_WIFI_API_PATH)
	whc_host_dev_driver_is_mp(&is_mp);
	if (is_mp != 1) {
		ret = whc_host_scan_abort();
		if (ret) {
			dev_err(global_idev.pwhc_dev, "[whc]: %s abort wifi scan failed!\n", __func__);
			return -EPERM;
		}
	}

	if (global_idev.mlme_priv.pscan_req_global) {
		memset(&info, 0, sizeof(info));
		info.aborted = 1;
		cfg80211_scan_done(global_idev.mlme_priv.pscan_req_global, &info);
		global_idev.mlme_priv.pscan_req_global = NULL;
		global_idev.mlme_priv.b_in_scan = false;
	}
#endif
	netif_tx_stop_all_queues(pnetdev);
	netif_dormant_on(pnetdev);
	netif_carrier_off(pnetdev);
	rtw_netdev_priv_is_on(pnetdev) = false;

	return 0;
}

static int rtw_ndev_open_ap(struct net_device *pnetdev)
{
#if defined(CONFIG_WHC_WIFI_API_PATH)
	u8 is_mp = 0;

	dev_dbg(global_idev.pwhc_dev, "[whc]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));

	rtw_netdev_priv_is_on(pnetdev) = true;

	whc_host_dev_driver_is_mp(&is_mp);
	if (is_mp != 1) {
		/* if2 init(SW + part of HW) */
		whc_host_init_ap();
	}

	netif_tx_start_all_queues(pnetdev);
	netif_tx_wake_all_queues(pnetdev);
#elif defined(CONFIG_WHC_HOST_RTW_CLI)
	dev_dbg(global_idev.pwhc_dev, "[whc]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));

	rtw_netdev_priv_is_on(pnetdev) = true;
	netif_carrier_off(pnetdev);
	netif_tx_stop_all_queues(pnetdev);
#endif

	return 0;
}

static int rtw_ndev_close_ap(struct net_device *pnetdev)
{
#if defined(CONFIG_WHC_WIFI_API_PATH)
	struct cfg80211_scan_info info;
	int ret = 0;
	u8 is_mp = 0;

	dev_dbg(global_idev.pwhc_dev, "[whc]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));

	whc_host_dev_driver_is_mp(&is_mp);
	if (is_mp != 1) {
		ret = whc_host_scan_abort();
		if (ret) {
			dev_err(global_idev.pwhc_dev, "[whc]: %s abort wifi scan failed!\n", __func__);
			return -EPERM;
		}
	}

	if (global_idev.mlme_priv.pscan_req_global) {
		memset(&info, 0, sizeof(info));
		info.aborted = 1;
		cfg80211_scan_done(global_idev.mlme_priv.pscan_req_global, &info);
		global_idev.mlme_priv.pscan_req_global = NULL;
		global_idev.mlme_priv.b_in_scan = false;
	}

	netif_tx_stop_all_queues(pnetdev);
	netif_carrier_off(pnetdev);

	if (is_mp != 1) {
		/* if2 deinit (SW) */
		whc_host_deinit_ap();
	}
	rtw_netdev_priv_is_on(pnetdev) = false;
#elif defined(CONFIG_WHC_HOST_RTW_CLI)
	dev_dbg(global_idev.pwhc_dev, "[whc]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));

	rtw_netdev_priv_is_on(pnetdev) = false;
	netif_carrier_off(pnetdev);
	netif_tx_stop_all_queues(pnetdev);
#endif
	return 0;
}

enum netdev_tx rtw_xmit_entry(struct sk_buff *skb, struct net_device *pnetdev)
{
	int ret = 0;
	int wlan_idx = 0;

	if (pnetdev) {
		wlan_idx = rtw_netdev_idx(pnetdev);
	} else {
		ret = -EINVAL;
		goto func_exit;
	}

	ret = whc_host_xmit_entry(wlan_idx, skb);

func_exit:
	return ret;
}

static void rtw_set_rx_mode(struct net_device *dev)
{
#if defined(CONFIG_WHC_WIFI_API_PATH)
	if ((rtw_netdev_flags(dev) ^ dev->flags) & IFF_PROMISC) {
		if (dev->flags & IFF_PROMISC) {
			dev_dbg(global_idev.pwhc_dev, "[whc]: %s enable promisc mode!\n", __func__);
			whc_host_set_promisc_enable(1, RTW_PROMISC_FILTER_ALL_PKT);
		} else {
			dev_dbg(global_idev.pwhc_dev, "[whc]: %s disable promisc mode!\n", __func__);
			whc_host_set_promisc_enable(0, RTW_PROMISC_FILTER_ALL_PKT);
		}
	}

	if (rtw_netdev_flags(dev) ^ dev->flags) {
		rtw_netdev_flags(dev) = dev->flags;
	}
#endif
}

static const struct net_device_ops rtw_ndev_ops = {
	.ndo_init = rtw_ndev_init,
	.ndo_uninit = rtw_ndev_uninit,
	.ndo_open = rtw_ndev_open,
	.ndo_stop = rtw_ndev_close,
	.ndo_start_xmit = rtw_xmit_entry,
	.ndo_select_queue = rtw_ndev_select_queue,
	.ndo_set_mac_address = rtw_ndev_set_mac_address,
	.ndo_get_stats = rtw_ndev_get_stats,
	.ndo_set_rx_mode = rtw_set_rx_mode,
};

static const struct net_device_ops rtw_ndev_ops_ap = {
	.ndo_init = rtw_ndev_init,
	.ndo_uninit = rtw_ndev_uninit,
	.ndo_open = rtw_ndev_open_ap,
	.ndo_stop = rtw_ndev_close_ap,
	.ndo_start_xmit = rtw_xmit_entry,
	.ndo_select_queue = rtw_ndev_select_queue,
	.ndo_set_mac_address = rtw_ndev_set_mac_address,
	.ndo_get_stats = rtw_ndev_get_stats,
	.ndo_do_ioctl = NULL,
};

#ifdef CONFIG_NAN
const struct net_device_ops rtw_ndev_ops_nan = {
	.ndo_init = rtw_ndev_init,
	.ndo_uninit = rtw_ndev_uninit,
	.ndo_open = rtw_ndev_open,
	.ndo_stop = rtw_ndev_close,
	.ndo_start_xmit = rtw_xmit_entry,
	.ndo_select_queue = rtw_ndev_select_queue,
	.ndo_set_mac_address = rtw_ndev_set_mac_address,
	.ndo_get_stats = rtw_ndev_get_stats,
};

int rtw_nan_iface_alloc(struct wiphy *wiphy,
						const char *name,
						struct wireless_dev **nan_wdev,
						struct vif_params *params)
{
	struct wireless_dev *wdev = NULL;
	struct net_device *ndev = NULL;
	int ret = 0;
	int softap_addr_offset_idx = global_idev.wifi_user_config.softap_addr_offset_idx;
	unsigned char nan_mac[ETH_ALEN];

	if (global_idev.pwdev_global[2]) {
		dev_info(global_idev.pwhc_dev, "%s: nan_wdev already exists", __func__);
		ret = -EBUSY;
		goto exit;
	}

	if (global_idev.pndev[2]) {
		dev_info(global_idev.pwhc_dev, "free old global_idev.pndev[2]=%x\n", global_idev.pndev[2]);
		free_netdev(global_idev.pndev[2]);
		global_idev.pndev[2] = NULL;
	}

	/* alloc and init netdev */
	ndev = alloc_etherdev_mq(sizeof(struct netdev_priv_t), 1);
	if (!ndev) {
		goto exit;
	}
	global_idev.pndev[2] = ndev;
	rtw_netdev_idx(ndev) = 2;
	rtw_netdev_label(ndev) = WIFI_FULLMAC_LABEL;
	ndev->netdev_ops = &rtw_ndev_ops_nan;
	ndev->watchdog_timeo = HZ * 3; /* 3 second timeout */
#ifndef CONFIG_WHC_HCI_IPC
	ndev->needed_headroom = max(SIZE_RX_DESC, SIZE_TX_DESC) + sizeof(struct whc_msg_info) + 4;
#endif
	SET_NETDEV_DEV(ndev, global_idev.pwhc_dev);

	netdev_set_default_ethtool_ops(global_idev.pndev[2], &global_idev.rtw_ethtool_ops);
	if (dev_alloc_name(global_idev.pndev[2], "nan0") < 0) {
		dev_err(global_idev.pwhc_dev, "dev_alloc_name, fail!\n");
	}

	netif_carrier_off(global_idev.pndev[2]);
	/* set nan port mac address */
	memcpy(nan_mac, global_idev.pndev[0]->dev_addr, ETH_ALEN);

	if (softap_addr_offset_idx == 0) {
		nan_mac[softap_addr_offset_idx] = global_idev.pndev[0]->dev_addr[softap_addr_offset_idx] + (2 << 1);
	} else {
		nan_mac[softap_addr_offset_idx] = global_idev.pndev[0]->dev_addr[softap_addr_offset_idx] + 2;
	}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0))
	memcpy((void *)global_idev.pndev[2]->dev_addr, nan_mac, ETH_ALEN);
#else
	eth_hw_addr_set(global_idev.pndev[2], nan_mac);
#endif

	ret = (register_netdevice(global_idev.pndev[2]) == 0) ? true : false;
	if (ret != true) {
		dev_err(global_idev.pwhc_dev, "netdevice register fail!\n");
		goto exit;
	}

	/* alloc and init wireless_dev */
	wdev = (struct wireless_dev *)kzalloc(sizeof(struct wireless_dev), in_interrupt() ? GFP_ATOMIC : GFP_KERNEL);
	if (!wdev) {
		dev_err(global_idev.pwhc_dev, "%s: allocate wdev fail", __func__);
		ret = -ENOMEM;
		goto exit;
	}
	wdev->wiphy = global_idev.pwiphy_global;
	wdev->netdev = ndev;
	wdev->iftype = NL80211_IFTYPE_NAN;
	ndev->ieee80211_ptr = wdev;
	global_idev.pwdev_global[2] = wdev;

	*nan_wdev = wdev;

	whc_host_init_nan();
	return ret;

exit:
	if (global_idev.pwdev_global[2]) { //wdev
		kfree((u8 *)global_idev.pwdev_global[2]);
		global_idev.pwdev_global[2] = NULL;
	}

	if (global_idev.pndev[2]) {
		free_netdev(global_idev.pndev[2]);
		global_idev.pndev[2] = NULL;
	}
	return -ENODEV;

}

void rtw_nan_iface_free(struct wiphy *wiphy)
{
	bool rtnl_lock_need = !rtnl_is_locked();

	if (!global_idev.pwdev_global[2] && !global_idev.pndev[2]) {
		return;
	}

	if (global_idev.pwdev_global[2]) {
		if (rtnl_lock_need) {
			rtnl_lock();
		}
		global_idev.pwdev_global[2]->netdev = NULL; // set to null for kernel to stop NAN
		cfg80211_unregister_wdev(global_idev.pwdev_global[2]);
		if (rtnl_lock_need) {
			rtnl_unlock();
		}
		dev_dbg(global_idev.pwhc_dev, "unregister wdev done for NAN, global_idev.pndev[2]->reg_state=%d.", global_idev.pndev[2]->reg_state);
	}

	if (global_idev.pndev[2] && (global_idev.pndev[2]->reg_state == NETREG_REGISTERED)) {
		global_idev.pwdev_global[2]->netdev = global_idev.pndev[2];
		if (rtnl_lock_need) {
			/* hold rtnl_lock in unregister_netdev. */
			unregister_netdev(global_idev.pndev[2]);
		} else {
			unregister_netdevice(global_idev.pndev[2]);
		}
	}

	dev_info(global_idev.pwhc_dev, "unregister netdev done for NAN.");

	if (global_idev.pwdev_global[2]) {
		kfree((u8 *)global_idev.pwdev_global[2]);
		global_idev.pwdev_global[2] = NULL;
		dev_info(global_idev.pwhc_dev, "remove wdev done for NAN.");
		/* remove wireless_dev in ndev. */
		global_idev.pndev[2]->ieee80211_ptr = NULL;
	}
	if (global_idev.pndev[2]) {
		free_netdev(global_idev.pndev[2]);
		global_idev.pndev[2] = NULL;
	}

	whc_host_deinit_nan();

	return;
}

#endif

int rtw_ndev_alloc(void)
{
	int i, ret = false;
	struct net_device *ndev = NULL;
#if defined(CONFIG_WHC_WIFI_API_PATH)
	struct wireless_dev *wdev;
#endif

	for (i = 0; i < TOTAL_IFACE_NUM; i++) {
		/* alloc and init netdev */
		ndev = alloc_etherdev_mq(sizeof(struct netdev_priv_t), 1);
		if (!ndev) {
			goto fail;
		}
		global_idev.pndev[i] = ndev;
		rtw_netdev_idx(ndev) = i;
		rtw_netdev_label(ndev) = WIFI_FULLMAC_LABEL;
		rtw_netdev_flags(ndev) = ndev->flags;

		ndev->netdev_ops = (i ? &rtw_ndev_ops_ap : &rtw_ndev_ops);
		ndev->watchdog_timeo = HZ * 3; /* 3 second timeout */
#ifndef CONFIG_WHC_HCI_IPC
		ndev->needed_headroom = max(SIZE_RX_DESC, SIZE_TX_DESC) + sizeof(struct whc_msg_info) + 4;
#endif
		SET_NETDEV_DEV(ndev, global_idev.pwhc_dev);

#if defined(CONFIG_WHC_WIFI_API_PATH)
		/* alloc and init wireless_dev */
		wdev = (struct wireless_dev *)kzalloc(sizeof(struct wireless_dev), in_interrupt() ? GFP_ATOMIC : GFP_KERNEL);
		if (!wdev) {
			goto fail;
		}
		wdev->wiphy = global_idev.pwiphy_global;
		wdev->netdev = ndev;
		wdev->iftype = (i ? NL80211_IFTYPE_AP : NL80211_IFTYPE_STATION);
		ndev->ieee80211_ptr = wdev;
		global_idev.pwdev_global[i] = wdev;
#endif
	}
	global_idev.mlme_priv.b_in_scan = false;
	global_idev.mlme_priv.b_in_disconnect = false;
	global_idev.mlme_priv.b_in_linking = false;

	init_completion(&global_idev.mlme_priv.disconnect_done_sema);

	return ret;

fail:
	for (i = 0; i < TOTAL_IFACE_NUM; i++) {
#if defined(CONFIG_WHC_WIFI_API_PATH)
		if (global_idev.pwdev_global[i]) { //wdev
			kfree((u8 *)global_idev.pwdev_global[i]);
			global_idev.pwdev_global[i] = NULL;
		}
#endif
		if (global_idev.pndev[i]) {
			free_netdev(global_idev.pndev[i]);
			global_idev.pndev[i] = NULL;
		}
	}
	return -ENODEV;

}

int rtw_ndev_register(void)
{
	int i, ret = false;
	char *wlan_name = WHC_HOST_PORT_NAME;

	for (i = 0; i < TOTAL_IFACE_NUM; i++) {
		rtw_ethtool_ops_init();
		netdev_set_default_ethtool_ops(global_idev.pndev[i], &global_idev.rtw_ethtool_ops);
		if (dev_alloc_name(global_idev.pndev[i], wlan_name) < 0) {
			dev_err(global_idev.pwhc_dev, "dev_alloc_name, fail!\n");
		}
		if (i == WHC_STA_PORT) {
			netif_dormant_on(global_idev.pndev[i]);
		}
		netif_carrier_off(global_idev.pndev[i]);
		if (register_netdev(global_idev.pndev[i]) != 0) {
			dev_err(global_idev.pwhc_dev, "netdevice register fail!\n");
			return -ENODEV;
		}
	}

	return ret;
}

void rtw_ndev_unregister(void)
{
	int i;

	for (i = 0; i < TOTAL_IFACE_NUM; i++) {
		if (global_idev.pndev[i] && (global_idev.pndev[i]->reg_state != NETREG_UNREGISTERED)) {
			/* hold rtnl_lock in unregister_netdev. */
			unregister_netdev(global_idev.pndev[i]);
		}
		dev_dbg(global_idev.pwhc_dev, "remove netdev done for interface %d.", i);

		if (global_idev.pwdev_global[i]) { //wdev
			kfree((u8 *)global_idev.pwdev_global[i]);
			global_idev.pwdev_global[i] = NULL;
			/* remove wireless_dev in ndev. */
			global_idev.pndev[i]->ieee80211_ptr = NULL;
		}

		if (global_idev.pndev[i]) {
			free_netdev(global_idev.pndev[i]);
			global_idev.pndev[i] = NULL;
		}
		dev_dbg(global_idev.pwhc_dev, "free netdev %d ok.", i);
	}
#ifdef CONFIG_P2P
	whc_host_p2p_pdwdev_free();
#endif
#ifdef CONFIG_NAN
	rtw_nan_iface_free(NULL);
#endif
}

/* IPv4, IPv6 IP addr notifier */
static int rtw_inetaddr_notifier_call(struct notifier_block *nb, unsigned long action, void *data)
{
	struct in_ifaddr *ifa = (struct in_ifaddr *)data;
	struct net_device *ndev;

	if (!ifa || !ifa->ifa_dev || !ifa->ifa_dev->dev) {
		return NOTIFY_DONE;
	}

	ndev = ifa->ifa_dev->dev;
	if (rtw_netdev_label(ndev) != WIFI_FULLMAC_LABEL) {
		dev_dbg(global_idev.pwhc_dev, "%s is not whc dev\n", ifa->ifa_label);
		return NOTIFY_DONE;
	}

	switch (action) {
	case NETDEV_UP:
		memcpy(global_idev.ip_addr, &ifa->ifa_address, RTW_IP_ADDR_LEN);
		dev_dbg(global_idev.pwhc_dev, "%s[%s]: up IP: [%pI4]\n", __func__, ifa->ifa_label, global_idev.ip_addr);
		break;
	case NETDEV_DOWN:
		memset(global_idev.ip_addr, 0, RTW_IP_ADDR_LEN);
		dev_dbg(global_idev.pwhc_dev, "%s[%s]: down IP: [%pI4]\n", __func__, ifa->ifa_label, global_idev.ip_addr);
		break;
	default:
		dev_dbg(global_idev.pwhc_dev, "%s: default action\n", __func__);
		break;
	}
	return NOTIFY_DONE;
}

#if IS_ENABLED(CONFIG_IPV6)
static int rtw_inet6addr_notifier_call(struct notifier_block *nb, unsigned long action, void *data)
{
	struct inet6_ifaddr *inet6_ifa = (struct inet6_ifaddr *)data;
	struct net_device *ndev;

	if (!inet6_ifa || !inet6_ifa->idev || !inet6_ifa->idev->dev) {
		return NOTIFY_DONE;
	}

	ndev = inet6_ifa->idev->dev;
	if (rtw_netdev_label(ndev) != WIFI_FULLMAC_LABEL) {
		dev_dbg(global_idev.pwhc_dev, "Not whc dev\n");
		return NOTIFY_DONE;
	}

	switch (action) {
	case NETDEV_UP:
		memcpy(global_idev.ipv6_addr, &inet6_ifa->addr, RTW_IPv6_ADDR_LEN);
		global_idev.ipv6_addr_updated = 1;
		dev_dbg(global_idev.pwhc_dev, "%s: up IP: [%pI6]\n", __func__, global_idev.ipv6_addr);
		break;
	case NETDEV_DOWN:
		memset(global_idev.ipv6_addr, 0, RTW_IPv6_ADDR_LEN);
		dev_dbg(global_idev.pwhc_dev, "%s: down IP: [%pI6]\n", __func__, global_idev.ipv6_addr);
		break;
	default:
		dev_dbg(global_idev.pwhc_dev, "%s: default action\n", __func__);
		break;
	}
	return NOTIFY_DONE;
}
#endif

static struct notifier_block rtw_inetaddr_notifier = {
	.notifier_call = rtw_inetaddr_notifier_call
};

#if IS_ENABLED(CONFIG_IPV6)
static struct notifier_block rtw_inet6addr_notifier = {
	.notifier_call = rtw_inet6addr_notifier_call
};
#endif

void rtw_inetaddr_notifier_register(void)
{
	register_inetaddr_notifier(&rtw_inetaddr_notifier);
#if IS_ENABLED(CONFIG_IPV6)
	register_inet6addr_notifier(&rtw_inet6addr_notifier);
#endif
}

void rtw_inetaddr_notifier_unregister(void)
{
	unregister_inetaddr_notifier(&rtw_inetaddr_notifier);
#if IS_ENABLED(CONFIG_IPV6)
	unregister_inet6addr_notifier(&rtw_inet6addr_notifier);
#endif
}

int rtw_netdev_probe(struct device *pdev)
{
	int ret = false;

	memset(&global_idev, 0, sizeof(struct whc_device));

	/* Initialize axi_priv */
	global_idev.pwhc_dev = pdev;

	dev_dbg(global_idev.pwhc_dev, "rtw_dev_probe start\n");

#if defined(CONFIG_WHC_WIFI_API_PATH)
	/*step1: alloc and init wiphy */
	ret = rtw_wiphy_init();
	if (ret == false) {
		dev_err(global_idev.pwhc_dev, "wiphy init fail");
		goto exit;
	}

	/*step3: register wiphy */
	if (wiphy_register(global_idev.pwiphy_global) != 0) {
		dev_err(global_idev.pwhc_dev, "wiphy register fail");
		goto os_ndevs_deinit;
	}
#endif
	/*step4: register netdev */
	ret = rtw_ndev_alloc();
	if (ret < 0) {
		dev_err(global_idev.pwhc_dev, "ndev alloc fail");
		goto os_ndevs_deinit;
	}

	ret = whc_host_init();
	if (ret < 0) {
		dev_err(global_idev.pwhc_dev, "llhw init fail");
		goto os_ndevs_deinit;
	}

	ret = rtw_ndev_register();
	if (ret < 0) {
		dev_err(global_idev.pwhc_dev, "ndev register fail");
		goto os_ndevs_deinit;
	}

#if defined(CONFIG_WHC_WIFI_API_PATH)
	whc_host_regd_init();
	rtw_drv_proc_init();
#ifdef CONFIG_IEEE80211R
	whc_host_ft_init();
#endif

#ifdef CONFIG_WAR_OFFLOAD
	rtw_proxy_init();
#endif
#endif

#if defined(CONFIG_WHC_CMD_PATH)
	whc_host_register_genl_family();
#endif

	return 0; /* probe success */

os_ndevs_deinit:
	rtw_ndev_unregister();
#if defined(CONFIG_WHC_WIFI_API_PATH)
	rtw_wiphy_deinit();

exit:
#endif
	return -ENODEV;
}

int rtw_netdev_remove(struct device *pdev)
{
	dev_dbg(global_idev.pwhc_dev, "%s start.", __func__);

#if defined(CONFIG_WHC_WIFI_API_PATH)
	rtw_drv_proc_deinit();
#endif
	rtw_ndev_unregister();
	dev_dbg(global_idev.pwhc_dev, "unregister netdev done.");
#if defined(CONFIG_WHC_WIFI_API_PATH)
	wiphy_unregister(global_idev.pwiphy_global);

	rtw_wiphy_deinit();
#endif
	dev_dbg(global_idev.pwhc_dev, "unregister and deinit wiphy done.");

	whc_host_deinit();
	dev_dbg(global_idev.pwhc_dev, "remove llhw done.");

	pr_info("%s done\n", __func__);
	memset(&global_idev, 0, sizeof(struct whc_device));
#if defined(CONFIG_WHC_CMD_PATH)
	whc_host_unregister_genl_family();
#endif
	return 0;
}

MODULE_DESCRIPTION("Realtek Wireless Lan Driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Realtek Corporation");
MODULE_VERSION(DRIVERVERSION);
