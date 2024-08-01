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

#ifdef CONFIG_P2P
static int rtw_p2p_ndev_open(struct net_device *pnetdev)
{
	struct wireless_dev *wdev;

	dev_info(global_idev.fullmac_dev, "[fullmac]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));

	wdev = ndev_to_wdev(pnetdev);
	rtw_netdev_priv_is_on(pnetdev) = true;

	if (wdev->iftype == NL80211_IFTYPE_P2P_GO) {
		llhw_wifi_init_ap();
	}

	netif_tx_start_all_queues(pnetdev);
	netif_tx_wake_all_queues(pnetdev);

	return 0;
}

static int rtw_p2p_ndev_close(struct net_device *pnetdev)
{
	struct wireless_dev *wdev;
	struct cfg80211_scan_info info;
	int ret = 0;

	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));

	ret = llhw_wifi_scan_abort(0);
	if (ret) {
		dev_err(global_idev.fullmac_dev, "[fullmac]: %s abort wifi scan failed!\n", __func__);
		return -EPERM;
	}
	if (global_idev.mlme_priv.pscan_req_global) {
		memset(&info, 0, sizeof(info));
		info.aborted = 1;
		cfg80211_scan_done(global_idev.mlme_priv.pscan_req_global, &info);
	}

	netif_tx_stop_all_queues(pnetdev);
	netif_carrier_off(pnetdev);

	wdev = ndev_to_wdev(pnetdev);
	if (wdev->iftype == NL80211_IFTYPE_P2P_CLIENT) {
		if (global_idev.p2p_global.pd_wlan_idx == 1) {//GC is up and use netdev1 with port0
			rtw_p2p_gc_intf_revert(1);
		}
	} else if (wdev->iftype == NL80211_IFTYPE_P2P_GO) {
		/* if2 deinit (SW) */
		llhw_wifi_deinit_ap();
	}
	rtw_netdev_priv_is_on(pnetdev) = false;

	llhw_wifi_set_p2p_role(P2P_ROLE_DISABLE);
	global_idev.p2p_global.p2p_role = P2P_ROLE_DISABLE;

	return 0;
}

const struct net_device_ops rtw_ndev_ops_p2p = {
	.ndo_init = rtw_ndev_init,
	.ndo_uninit = rtw_ndev_uninit,
	.ndo_open = rtw_p2p_ndev_open,
	.ndo_stop = rtw_p2p_ndev_close,
	.ndo_start_xmit = rtw_xmit_entry,
	.ndo_select_queue = rtw_ndev_select_queue,
	.ndo_set_mac_address = rtw_ndev_set_mac_address,
	.ndo_get_stats = rtw_ndev_get_stats,
	.ndo_do_ioctl = rtw_ndev_ioctl,
};


int rtw_ndev_p2p_register(enum nl80211_iftype type, const char *name, u8 wlan_idx)
{
	int ret = false;
	struct net_device *ndev = NULL;
	struct wireless_dev *wdev;

	/*step1: alloc netdev*/
	ndev = alloc_etherdev_mq(sizeof(struct netdev_priv_t), 1);
	if (!ndev) {
		goto dev_fail;
	}
	rtw_netdev_idx(ndev) = (type == NL80211_IFTYPE_P2P_GO) ? 1 : 0;
	ndev->netdev_ops = &rtw_ndev_ops_p2p;
	ndev->watchdog_timeo = HZ * 3; /* 3 second timeout */
#ifndef CONFIG_FULLMAC_HCI_IPC
	ndev->needed_headroom = max(SIZE_RX_DESC, SIZE_TX_DESC) + sizeof(struct inic_msg_info) + 4;
#endif
	SET_NETDEV_DEV(ndev, global_idev.fullmac_dev);
	global_idev.pndev[wlan_idx] = ndev;

	/* step2: alloc wireless_dev */
	wdev = (struct wireless_dev *)kzalloc(sizeof(struct wireless_dev), in_interrupt() ? GFP_ATOMIC : GFP_KERNEL);
	if (!wdev) {
		goto dev_fail;
	}
	wdev->wiphy = global_idev.pwiphy_global;
	wdev->netdev = ndev;
	wdev->iftype = type;
	ndev->ieee80211_ptr = wdev;
	global_idev.pwdev_global[wlan_idx] = wdev;

	/* step3: special setting for netdev */
	if (type == NL80211_IFTYPE_P2P_GO) {
		/* set p2p mac address, otherwise the intend interface addr in GO neg will be zero*/
		memcpy(global_idev.pndev[wlan_idx]->dev_addr, global_idev.pndev[0]->dev_addr, ETH_ALEN);
		global_idev.pndev[wlan_idx]->dev_addr[1] = global_idev.pndev[0]->dev_addr[1] + 1;
	} else if (type == NL80211_IFTYPE_P2P_CLIENT) {
		llhw_wifi_init_ap();
		global_idev.p2p_global.pd_wlan_idx = 1;
		rtw_p2p_driver_macaddr_switch();//switch port0 and port1 MAC
	}

	/* step4: register netdev */
	rtw_ethtool_ops_init();
	netdev_set_default_ethtool_ops(global_idev.pndev[wlan_idx], &global_idev.rtw_ethtool_ops);
	if (dev_alloc_name(global_idev.pndev[wlan_idx], name) < 0) {
		dev_err(global_idev.fullmac_dev, "dev_alloc_name, fail!\n");
	}
	netif_carrier_off(global_idev.pndev[wlan_idx]);
	ret = (register_netdevice(global_idev.pndev[wlan_idx]) == 0) ? true : false;

	if (ret != true) {
		dev_err(global_idev.fullmac_dev, "netdevice register fail!\n");
		goto dev_fail;
	}

	if (type == NL80211_IFTYPE_P2P_GO) {
		llhw_wifi_set_p2p_role(P2P_ROLE_GO);
		global_idev.p2p_global.p2p_role = P2P_ROLE_GO;
	} else if (type == NL80211_IFTYPE_P2P_CLIENT) {
		llhw_wifi_set_p2p_role(P2P_ROLE_CLIENT);
		global_idev.p2p_global.p2p_role = P2P_ROLE_CLIENT;
	}

	return ret;

dev_fail:
	if (global_idev.pwdev_global[wlan_idx]) { //wdev
		kfree((u8 *)global_idev.pwdev_global[wlan_idx]);
		global_idev.pwdev_global[wlan_idx] = NULL;
	}

	if (global_idev.pndev[wlan_idx]) {
		free_netdev(global_idev.pndev[wlan_idx]);
		global_idev.pndev[wlan_idx] = NULL;
	}

	if (global_idev.p2p_global.pd_wlan_idx) {
		rtw_p2p_gc_intf_revert(1);
	}

	return -ENODEV;

}

int rtw_p2p_iface_alloc(struct wiphy *wiphy, const char *name,
						struct wireless_dev **p2p_wdev, enum nl80211_iftype type)
{
	struct wireless_dev *wdev = NULL;
	int ret = 0;
	u8 wlan_idx = 0;
	struct wireless_dev *old_wdev = NULL;

	if (type == NL80211_IFTYPE_P2P_GO || type == NL80211_IFTYPE_P2P_CLIENT) {
		wlan_idx = 1;
		old_wdev = global_idev.pwdev_global[wlan_idx];
	} else if (type == NL80211_IFTYPE_P2P_DEVICE) {
		memset(&(global_idev.p2p_global), 0, sizeof(struct p2p_priv_t));
		/*step0: unregister original softap netdev and wdev for later P2P GO usage*/
		if (global_idev.pwdev_global[1]) {
			unregister_netdevice(global_idev.pndev[1]);
			kfree((u8 *)global_idev.pwdev_global[1]);
			global_idev.pwdev_global[1] = NULL;
			/* remove wireless_dev in ndev. */
			global_idev.pndev[1]->ieee80211_ptr = NULL;
		}
		old_wdev = global_idev.p2p_global.pd_pwdev;
	}

	if (old_wdev) {/*step1: check whether old wdev exits(normally already freed in del intf)*/
		dev_info(global_idev.fullmac_dev, "%s: wdev already exists", __func__);
		ret = -EBUSY;
		return ret;
	}

	if (type == NL80211_IFTYPE_P2P_GO || type == NL80211_IFTYPE_P2P_CLIENT) {
		if (global_idev.pndev[wlan_idx]) {/*step2: free old netdev*/
			free_netdev(global_idev.pndev[wlan_idx]);
			global_idev.pndev[wlan_idx] = NULL;
		}

		rtw_ndev_p2p_register(type, name, wlan_idx); /*step3: alloc/register new netdev and wdev*/

		*p2p_wdev = global_idev.pwdev_global[wlan_idx];
	} else if (type == NL80211_IFTYPE_P2P_DEVICE) {/* P2P DEVICE has no netdev*/
		wdev = (struct wireless_dev *)kzalloc(sizeof(struct wireless_dev), in_interrupt() ? GFP_ATOMIC : GFP_KERNEL);
		if (!wdev) {
			ret = -ENOMEM;
			return ret;
		}

		wdev->wiphy = wiphy;
		wdev->iftype = NL80211_IFTYPE_P2P_DEVICE;
		memcpy(wdev->address, global_idev.pndev[0]->dev_addr, ETH_ALEN);
		global_idev.p2p_global.pd_pwdev = wdev;
		global_idev.p2p_global.pd_wlan_idx = 0;
		*p2p_wdev = wdev;
	}

	return ret;
}

void rtw_p2p_iface_free(struct wiphy *wiphy, struct wireless_dev *wdev)
{
	enum nl80211_iftype iftype = wdev->iftype;

	if (iftype == NL80211_IFTYPE_P2P_DEVICE) {
		rtw_p2p_pdwdev_free();
	} else if (iftype == NL80211_IFTYPE_P2P_CLIENT || iftype == NL80211_IFTYPE_P2P_GO) {
		if (iftype == NL80211_IFTYPE_P2P_CLIENT) {
			rtw_p2p_gc_intf_revert(1); /* switch back port0 and port1's mac addr*/
		}

		if (global_idev.pndev[1]) {
			unregister_netdevice(global_idev.pndev[1]);
			if (global_idev.pwdev_global[1]) { //wdev
				kfree((u8 *)global_idev.pwdev_global[1]);
				global_idev.pwdev_global[1] = NULL;
				/* remove wireless_dev in ndev. */
				global_idev.pndev[1]->ieee80211_ptr = NULL;
			}
		}
	}

	llhw_wifi_set_p2p_role(P2P_ROLE_DISABLE);
	global_idev.p2p_global.p2p_role = P2P_ROLE_DISABLE;

	return;
}

/*netdev0: driver_wlanidx_1, port1_MAC=efuse_mac
  netdev1: driver_wlanidx_0, port0_MAC=efuse_mac+1 */
void rtw_p2p_driver_macaddr_switch(void)
{
	llhw_wifi_set_mac_addr(1, global_idev.pndev[0]->dev_addr);
	llhw_wifi_set_mac_addr(0, global_idev.pndev[1]->dev_addr);
	rtw_netdev_idx(global_idev.pndev[0]) = 1;
	rtw_netdev_idx(global_idev.pndev[1]) = 0;
}

void rtw_p2p_gc_intf_revert(u8 need_if2_deinit)
{
	int softap_addr_offset_idx = global_idev.wifi_user_config.softap_addr_offset_idx;
	unsigned char port0_macaddr[6];
	unsigned char last;

	if (global_idev.p2p_global.pd_wlan_idx) {
		global_idev.p2p_global.pd_wlan_idx = 0;

		memcpy((void *)port0_macaddr, global_idev.pndev[1]->dev_addr, ETH_ALEN);
		last = global_idev.pndev[1]->dev_addr[softap_addr_offset_idx] - 1;
		memcpy((void *)&port0_macaddr[softap_addr_offset_idx], &last, 1);
		llhw_wifi_set_mac_addr(0, port0_macaddr);
		llhw_wifi_set_mac_addr(1, global_idev.pndev[1]->dev_addr);
		if (global_idev.pndev[0]) {
			rtw_netdev_idx(global_idev.pndev[0]) = 0;
		}
		rtw_netdev_idx(global_idev.pndev[1]) = 1;
		if (need_if2_deinit) {
			llhw_wifi_deinit_ap();
		}
	}
}

int rtw_p2p_get_wdex_idx(struct wireless_dev *wdev)
{
	int idx = 0;
	struct net_device *pnetdev = NULL;

	if (wdev->iftype == NL80211_IFTYPE_P2P_DEVICE) {
		idx = 2; /* used idx=2 to specialize p2p device, since it cannot refered as global_idev.pwdev_global[x]*/
	} else if (wdev->iftype == NL80211_IFTYPE_P2P_CLIENT || wdev->iftype == NL80211_IFTYPE_P2P_GO) {
		idx = 1; /* GC and GO both refered as global_idev.pwdev_global[1] */
	} else { /* other non P2P cases*/
		pnetdev = wdev_to_ndev(wdev);
		if (pnetdev) {
			idx = rtw_netdev_idx(pnetdev);
		}
	}

	return idx;
}

void rtw_p2p_pdwdev_free(void)
{
	bool rtnl_lock_need = 0;

	if (global_idev.p2p_global.pd_pwdev) {
		rtnl_lock_need = !rtnl_is_locked();
		if (rtnl_lock_need) {
			rtnl_lock();
		}
		cfg80211_unregister_wdev(global_idev.p2p_global.pd_pwdev);
		kfree((u8 *)global_idev.p2p_global.pd_pwdev);
		global_idev.p2p_global.pd_pwdev = NULL;
		if (rtnl_lock_need) {
			rtnl_unlock();
		}
	}
}

#endif

