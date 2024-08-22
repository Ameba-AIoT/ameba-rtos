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

#define DRIVERVERSION "v1.15.12-27-g7f6d5a49a.20220627"

#define RTW_PRIV_DGB_CMD (SIOCDEVPRIVATE)
#define RTW_PRIV_MP_CMD (SIOCDEVPRIVATE + 1)
#define WIFI_MP_MSG_BUF_SIZE (4096)

struct rtw_priv_ioctl {
	unsigned char __user *data;
	unsigned short len;
};

/*
 * Set wlan0 mac address: 1) ip link set dev wlan0 down, 2) ip link set dev wlan0 address 86:EF:EB:92:A2:FF, 3) ip link set dev wlan0 up
 * Set wlan1 mac address: 1) ifconfig wlan1 up, 2) ip link set dev wlan0 address 86:EF:EB:92:A2:F0, 3) hostapd /etc/hostapd.conf -B -i wlan1
 * NOTE for Soft AP: when wlan1 down, cannot set mac address. but if mac address changed, need restart hostapd. Recomment to change bssid inside hostapd.conf.
*/
int rtw_ndev_set_mac_address(struct net_device *pnetdev, void *p)
{
	int ret = 0;
	struct sockaddr *addr = p;

	if (rtw_netdev_idx(pnetdev) >= INIC_MAX_NET_PORT_NUM) {
		dev_err(global_idev.fullmac_dev, "Netdevice index err.");
		return -1;
	}

	dev_dbg(global_idev.fullmac_dev,
			"[netdev] %s: set wlan-%d mac addr to [%02x:%02x:%02x:%02x:%02x:%02x]", __func__, rtw_netdev_idx(pnetdev),
			*(u8 *)(addr->sa_data), *(u8 *)(addr->sa_data + 1), *(u8 *)(addr->sa_data + 2), *(u8 *)(addr->sa_data + 3), *(u8 *)(addr->sa_data + 4),
			*(u8 *)(addr->sa_data + 5));

	ret = llhw_wifi_set_mac_addr(rtw_netdev_idx(pnetdev), addr->sa_data);

	if (ret == 0) {
		/* Set mac address success, then change the dev_addr inside net_device. */
		memset((void *)global_idev.pndev[rtw_netdev_idx(pnetdev)]->dev_addr, 0, ETH_ALEN);
		memcpy((void *)global_idev.pndev[rtw_netdev_idx(pnetdev)]->dev_addr, addr->sa_data, ETH_ALEN);
	}

	return ret;
}

struct net_device_stats *rtw_ndev_get_stats(struct net_device *pnetdev)
{
	dev_dbg(global_idev.fullmac_dev, "[netdev]: %s", __func__);
	return &global_idev.stats[rtw_netdev_idx(pnetdev)];
}

/* Given a data frame determine the 802.1p/1d tag to use. */
unsigned int rtw_classify8021d(struct sk_buff *skb)
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
	case htons(ETH_P_IP):
		dscp = ip_hdr(skb)->tos & 0xfc;
		break;
	default:
		return 0;
	}

	return dscp >> 5;
}

u8 qos_acm(u8 acm_mask, u8 priority)
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
		dev_warn(global_idev.fullmac_dev, "qos_acm(): invalid pattrib->priority: %d!!!\n", priority);
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

int rtw_ndev_ioctl(struct net_device *ndev, struct ifreq *rq, int cmd_id)
{
	struct rtw_priv_ioctl *wrq_data = rq->ifr_data;
	struct rtw_priv_ioctl cmd = {NULL, 0};
	unsigned char *cmd_buf = NULL, *user_buf = NULL;
	static dma_addr_t cmd_buf_phy = 0, user_buf_phy = 0;
	int ret = 0;

	if (copy_from_user(&cmd, rq->ifr_data, sizeof(struct rtw_priv_ioctl))) {
		dev_err(global_idev.fullmac_dev, "[fullmac]: %s copy_from_user failed\n", __func__);
		ret = -EFAULT;
		goto out;
	}

	if (!cmd.data || (cmd.len <= 0) || (cmd.len > WIFI_MP_MSG_BUF_SIZE)) {
		ret = -EINVAL;
		goto out;
	}

	cmd_buf = rtw_malloc(cmd.len, &cmd_buf_phy);
	if (!cmd_buf) {
		dev_err(global_idev.fullmac_dev, "%s: allloc cmd buffer failed.\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	user_buf = rtw_malloc(WIFI_MP_MSG_BUF_SIZE, &user_buf_phy);
	if (!user_buf) {
		dev_err(global_idev.fullmac_dev, "%s: allloc user buffer failed.\n", __func__);
		ret = -ENOMEM;
		goto free_cmd_buf;
	}

	if (copy_from_user(cmd_buf, cmd.data, cmd.len)) {
		dev_err(global_idev.fullmac_dev, "[fullmac]: %s copy_from_user failed\n", __func__);
		ret = -EFAULT;
		goto free_user_buf;
	}

	switch (cmd_id) {
	case RTW_PRIV_DGB_CMD:
		ret = llhw_wifi_iwpriv_cmd(cmd_buf_phy, cmd.len, user_buf_phy);
		break;
	case RTW_PRIV_MP_CMD:
		ret = llhw_wifi_mp_cmd(cmd_buf_phy, cmd.len, user_buf_phy);
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}

	cmd.len = strlen(user_buf);
	user_buf[cmd.len] = '\0';
	if (cmd.len > 0) {
		if (copy_to_user(&wrq_data->len, &cmd.len, sizeof(unsigned short))) {
			ret = -EFAULT;
			goto free_user_buf;
		}

		if (copy_to_user(cmd.data, user_buf, cmd.len + 1)) {
			ret = -EFAULT;
		}
	}

free_user_buf:
	if (user_buf) {
		rtw_mfree(WIFI_MP_MSG_BUF_SIZE, user_buf, user_buf_phy);
		cmd_buf = NULL;
	}


free_cmd_buf:
	if (cmd_buf) {
		rtw_mfree(cmd.len, cmd_buf, cmd_buf_phy);
		cmd_buf = NULL;
	}
out:
	return ret;
}


int rtw_ndev_init(struct net_device *pnetdev)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));
	return 0;
}

void rtw_ndev_uninit(struct net_device *pnetdev)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));
}

int rtw_ndev_open(struct net_device *pnetdev)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));
	rtw_netdev_priv_is_on(pnetdev) = true;
	netif_tx_start_all_queues(pnetdev);
	netif_tx_wake_all_queues(pnetdev);

	return 0;
}

static int rtw_ndev_close(struct net_device *pnetdev)
{
	struct cfg80211_scan_info info;
	int ret = 0;

	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));

	if (!global_idev.mp_fw) {
		ret = llhw_wifi_scan_abort(0);
		if (ret) {
			dev_err(global_idev.fullmac_dev, "[fullmac]: %s abort wifi scan failed!\n", __func__);
			return -EPERM;
		}
	}

	if (global_idev.mlme_priv.pscan_req_global) {
		memset(&info, 0, sizeof(info));;
		info.aborted = 1;
		cfg80211_scan_done(global_idev.mlme_priv.pscan_req_global, &info);
	}
#ifdef CONFIG_SDIO_BRIDGE
	llhw_wifi_disconnect();
	/* sdio device will report WIFI_EVENT_DISCONNECT event to linux, after disconnect done */
	global_idev.mlme_priv.b_in_disconnect = true;
	wait_for_completion_interruptible(&global_idev.mlme_priv.disconnect_done_sema);
#endif
	netif_tx_stop_all_queues(pnetdev);
	netif_carrier_off(pnetdev);
	rtw_netdev_priv_is_on(pnetdev) = false;

	return 0;
}

int rtw_ndev_open_ap(struct net_device *pnetdev)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));

	rtw_netdev_priv_is_on(pnetdev) = true;

	if (!global_idev.mp_fw) {
		/* if2 init(SW + part of HW) */
		llhw_wifi_init_ap();
	}

	netif_tx_start_all_queues(pnetdev);
	netif_tx_wake_all_queues(pnetdev);

	return 0;
}

static int rtw_ndev_close_ap(struct net_device *pnetdev)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));

	netif_tx_stop_all_queues(pnetdev);
	netif_carrier_off(pnetdev);

	if (!global_idev.mp_fw) {
		/* if2 deinit (SW) */
		llhw_wifi_deinit_ap();
	}
	rtw_netdev_priv_is_on(pnetdev) = false;

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

	ret = llhw_xmit_entry(wlan_idx, skb);

func_exit:
	return ret;
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
	.ndo_do_ioctl = rtw_ndev_ioctl,
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
	.ndo_do_ioctl = rtw_ndev_ioctl,
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

	if (global_idev.pwdev_global[2]) {
		dev_info(global_idev.fullmac_dev, "%s: nan_wdev already exists", __func__);
		ret = -EBUSY;
		goto exit;
	}

	/* alloc and init netdev */
	ndev = alloc_etherdev_mq(sizeof(struct netdev_priv_t), 1);
	if (!ndev) {
		goto exit;
	}
	global_idev.pndev[2] = ndev;
	rtw_netdev_idx(ndev) = 2;
	ndev->netdev_ops = &rtw_ndev_ops_nan;
	ndev->watchdog_timeo = HZ * 3; /* 3 second timeout */
	SET_NETDEV_DEV(ndev, global_idev.fullmac_dev);

	netdev_set_default_ethtool_ops(global_idev.pndev[2], &global_idev.rtw_ethtool_ops);
	if (dev_alloc_name(global_idev.pndev[2], "nan0") < 0) {
		dev_err(global_idev.fullmac_dev, "dev_alloc_name, fail!\n");
	}

	netif_carrier_off(global_idev.pndev[2]);
	/* set nan port mac address */
	memcpy(global_idev.pndev[2]->dev_addr, global_idev.pndev[0]->dev_addr, ETH_ALEN);
	global_idev.pndev[2]->dev_addr[softap_addr_offset_idx] = global_idev.pndev[0]->dev_addr[softap_addr_offset_idx] + 2;

	ret = (register_netdevice(global_idev.pndev[2]) == 0) ? true : false;
	if (ret != true) {
		dev_err(global_idev.fullmac_dev, "netdevice register fail!\n");
		goto exit;
	}

	/* alloc and init wireless_dev */
	wdev = (struct wireless_dev *)kzalloc(sizeof(struct wireless_dev), in_interrupt() ? GFP_ATOMIC : GFP_KERNEL);
	if (!wdev) {
		dev_err(global_idev.fullmac_dev, "%s: allocate wdev fail", __func__);
		ret = -ENOMEM;
		goto exit;
	}
	wdev->wiphy = global_idev.pwiphy_global;
	wdev->netdev = ndev;
	wdev->iftype = NL80211_IFTYPE_NAN;
	ndev->ieee80211_ptr = wdev;
	global_idev.pwdev_global[2] = wdev;

	*nan_wdev = wdev;
	netif_carrier_on(global_idev.pndev[2]);
	netif_tx_start_all_queues(global_idev.pndev[2]);

	llhw_wifi_init_nan();
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
	if (global_idev.pndev[2]) {
		/* hold rtnl_lock in unregister_netdev. */
		unregister_netdevice(global_idev.pndev[2]);
	} else {
		goto exit;
	}

	dev_dbg(global_idev.fullmac_dev, "remove netdev done for interface NAN.");

	if (global_idev.pwdev_global[2]) {
		//rtnl_lock();
		cfg80211_unregister_wdev(global_idev.pwdev_global[2]);
		//rtnl_unlock();

		dev_dbg(global_idev.fullmac_dev, "remove wdev done for interface NAN.");

		kfree((u8 *)global_idev.pwdev_global[2]);
		global_idev.pwdev_global[2] = NULL;
		/* remove wireless_dev in ndev. */
		global_idev.pndev[2]->ieee80211_ptr = NULL;
	}

	if (global_idev.pndev[2]) {
		free_netdev(global_idev.pndev[2]);
		global_idev.pndev[2] = NULL;
	}

	llhw_wifi_deinit_nan();

exit:
	return;
}

#endif

int rtw_ndev_alloc(void)
{
	int i, ret = false;
	struct net_device *ndev = NULL;
#ifndef CONFIG_SDIO_BRIDGE
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
		ndev->netdev_ops = (i ? &rtw_ndev_ops_ap : &rtw_ndev_ops);
		ndev->watchdog_timeo = HZ * 3; /* 3 second timeout */
#ifndef CONFIG_FULLMAC_HCI_IPC
		ndev->needed_headroom = max(SIZE_RX_DESC, SIZE_TX_DESC) + sizeof(struct inic_msg_info) + 4;
#ifndef CONFIG_SDIO_BRIDGE
#ifdef CONFIG_WIRELESS_EXT
		if (i == 0) {
			ndev->wireless_handlers = (struct iw_handler_def *)&rtw_handlers_def;
		}
#endif
#endif
#endif
		SET_NETDEV_DEV(ndev, global_idev.fullmac_dev);

#ifndef CONFIG_SDIO_BRIDGE
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

	init_completion(&global_idev.mlme_priv.disconnect_done_sema);

	return ret;

fail:
	for (i = 0; i < TOTAL_IFACE_NUM; i++) {
#ifndef CONFIG_SDIO_BRIDGE
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
#ifdef CONFIG_SDIO_BRIDGE
	char *wlan_name = "eth_sta%d";
#else
	char *wlan_name = "wlan%d";
#endif
	for (i = 0; i < TOTAL_IFACE_NUM; i++) {
		rtw_ethtool_ops_init();
		netdev_set_default_ethtool_ops(global_idev.pndev[i], &global_idev.rtw_ethtool_ops);
		if (dev_alloc_name(global_idev.pndev[i], wlan_name) < 0) {
			dev_err(global_idev.fullmac_dev, "dev_alloc_name, fail!\n");
		}
		netif_carrier_off(global_idev.pndev[i]);
		if (register_netdev(global_idev.pndev[i]) != 0) {
			dev_err(global_idev.fullmac_dev, "netdevice register fail!\n");
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
		dev_dbg(global_idev.fullmac_dev, "remove netdev done for interface %d.", i);

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
		dev_dbg(global_idev.fullmac_dev, "free netdev %d ok.", i);
	}
#ifdef CONFIG_P2P
	rtw_p2p_pdwdev_free();
#endif
}

MODULE_DESCRIPTION("Realtek Wireless Lan Driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Realtek Corporation");
MODULE_VERSION(DRIVERVERSION);
