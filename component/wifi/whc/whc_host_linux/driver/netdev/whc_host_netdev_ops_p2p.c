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

#ifdef CONFIG_P2P

/*
 * whc_p2p_ndev_unregister - safely tear down a netdev/wdev pair while
 * wiphy_mutex is already held (from add_virtual_intf / del_virtual_intf).
 *
 * cfg80211_unregister_netdevice cannot be used here: it ultimately calls
 * unregister_netdevice which fires the NETDEV_GOING_DOWN notifier; the
 * cfg80211 notifier handler tries to re-acquire wiphy_mutex → deadlock.
 *
 * Safe sequence:
 *  1. Sever both ndev↔wdev links BEFORE any kernel call.
 *  2. cfg80211_unregister_wdev(): with wdev->netdev == NULL it takes the
 *     "no-netdev" code path (same as P2P_DEVICE/NAN), removing the wdev
 *     from the wiphy list without invoking unregister_netdevice.
 *  3. kfree(wdev): now safe because it is no longer on any list.
 *  4. unregister_netdevice(ndev): the cfg80211 notifier checks
 *     ndev->ieee80211_ptr first; NULL → NOTIFY_DONE, wiphy_mutex never
 *     touched.
 *  5. free_netdev(ndev).
 */
static void whc_p2p_ndev_unregister(struct net_device *ndev,
									struct wireless_dev *wdev)
{
	/* Step 1: sever both links so neither direction can reach the other */
	wdev->netdev = NULL;
	ndev->ieee80211_ptr = NULL;

	/* Step 2+3: remove from wiphy list (non-netdev path) and free */
	cfg80211_unregister_wdev(wdev);
	kfree((u8 *)wdev);

	/* Step 4+5: unregister and free the orphaned netdev */
	unregister_netdevice(ndev);
	free_netdev(ndev);
}

static int whc_host_p2p_ndev_open(struct net_device *pnetdev)
{
	struct wireless_dev *wdev;

	dev_info(global_idev.pwhc_dev, "[whc]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));

	wdev = ndev_to_wdev(pnetdev);
	rtw_netdev_priv_is_on(pnetdev) = true;

	if (wdev->iftype == NL80211_IFTYPE_P2P_GO) {
		whc_host_init_ap();
	}

	netif_tx_start_all_queues(pnetdev);
	netif_tx_wake_all_queues(pnetdev);

	return 0;
}

static int whc_host_p2p_ndev_close(struct net_device *pnetdev)
{
	struct wireless_dev *wdev;
	struct cfg80211_scan_info info;
	int ret = 0;

	dev_dbg(global_idev.pwhc_dev, "[whc]: %s %d\n", __func__, rtw_netdev_idx(pnetdev));

	ret = whc_host_scan_abort();
	if (ret) {
		dev_err(global_idev.pwhc_dev, "[whc]: %s abort wifi scan failed!\n", __func__);
		return -EPERM;
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

	wdev = ndev_to_wdev(pnetdev);
	if (wdev->iftype == NL80211_IFTYPE_P2P_CLIENT) {
		if (global_idev.p2p_global.pd_wlan_idx == 1) {//GC is active; P2P Device is on port1
			whc_host_p2p_gc_intf_revert(1);
		}
	} else if (wdev->iftype == NL80211_IFTYPE_P2P_GO) {
		/* if2 deinit (SW) */
		whc_host_deinit_ap();
	}
	rtw_netdev_priv_is_on(pnetdev) = false;

	whc_host_set_p2p_role(P2P_ROLE_DISABLE);
	global_idev.p2p_global.p2p_role = P2P_ROLE_DISABLE;

	return 0;
}

const struct net_device_ops whc_host_ndev_ops_p2p = {
	.ndo_init = rtw_ndev_init,
	.ndo_uninit = rtw_ndev_uninit,
	.ndo_open = whc_host_p2p_ndev_open,
	.ndo_stop = whc_host_p2p_ndev_close,
	.ndo_start_xmit = rtw_xmit_entry,
	.ndo_select_queue = rtw_ndev_select_queue,
	.ndo_set_mac_address = rtw_ndev_set_mac_address,
	.ndo_get_stats = rtw_ndev_get_stats,
};

int whc_host_ndev_p2p_register(enum nl80211_iftype type, const char *name, u8 wlan_idx)
{
	int ret = false;
	struct net_device *ndev = NULL;
	struct wireless_dev *wdev;
	u8 dev_addr[ETH_ALEN] = {0};

	/*step1: alloc netdev*/
	ndev = alloc_etherdev_mq(sizeof(struct netdev_priv_t), 1);
	if (!ndev) {
		goto dev_fail;
	}
	rtw_netdev_idx(ndev) = wlan_idx;
	ndev->netdev_ops = &whc_host_ndev_ops_p2p;
	ndev->watchdog_timeo = HZ * 3; /* 3 second timeout */
#ifndef CONFIG_WHC_HCI_IPC
	ndev->needed_headroom = max(SIZE_RX_DESC, SIZE_TX_DESC) + sizeof(struct whc_msg_info) + 4;
#endif
	SET_NETDEV_DEV(ndev, global_idev.pwhc_dev);
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
		/* set p2p mac address, otherwise the intend interface addr in GO neg will be zero. */
		memcpy(dev_addr, global_idev.p2p_global.pd_pwdev->address, ETH_ALEN);
		dev_addr[1] = global_idev.p2p_global.pd_pwdev->address[1] + 1;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0))
		memcpy((void *)global_idev.pndev[wlan_idx]->dev_addr, dev_addr, ETH_ALEN);
#else
		eth_hw_addr_set(global_idev.pndev[wlan_idx], dev_addr);
#endif
	} else if (type == NL80211_IFTYPE_P2P_CLIENT) {
		/* init ap to prepare softap adapter in device, otherwise configure mac for port 1 below will fail. */
		whc_host_init_ap();
		global_idev.p2p_global.pd_wlan_idx = 1;

		/* switch port0 and port1 mac address: port0 mac = efuse_mac+1, port1 mac = efuse_mac */
		whc_host_p2p_driver_macaddr_switch();
	}

	/* step4: register netdev */
	rtw_ethtool_ops_init();
	netdev_set_default_ethtool_ops(global_idev.pndev[wlan_idx], &global_idev.rtw_ethtool_ops);
	if (dev_alloc_name(global_idev.pndev[wlan_idx], name) < 0) {
		dev_err(global_idev.pwhc_dev, "dev_alloc_name, fail!\n");
	}
	netif_carrier_off(global_idev.pndev[wlan_idx]);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 12, 0))
	ret = (cfg80211_register_netdevice(global_idev.pndev[wlan_idx]) == 0) ? true : false;
#else
	ret = (register_netdevice(global_idev.pndev[wlan_idx]) == 0) ? true : false;
#endif
	if (ret != true) {
		dev_err(global_idev.pwhc_dev, "netdevice register fail!\n");
		goto dev_fail;
	}

	if (type == NL80211_IFTYPE_P2P_GO) {
		whc_host_set_p2p_role(P2P_ROLE_GO);
		global_idev.p2p_global.p2p_role = P2P_ROLE_GO;
	} else if (type == NL80211_IFTYPE_P2P_CLIENT) {
		whc_host_set_p2p_role(P2P_ROLE_CLIENT);
		global_idev.p2p_global.p2p_role = P2P_ROLE_CLIENT;
	}
	return true;

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
		whc_host_p2p_gc_intf_revert(1);
	}

	return -ENODEV;

}

/*
            net_device      wireless_dev	   MAC	        port
GO          pndev[1]        pwdev[1]	       mac+1        Port1, wlan_idx = 1
P2P DEV     -               pd_pwdev           mac          Port0, pd_wlan_idx = 0

GC          pndev[0]        pwdev[0]           mac+1        Port0,  wlan_idx = 0
P2P DEV	    -               pd_pwdev           mac          Port1, pd_wlan_idx= 1
*/
int whc_host_p2p_iface_alloc(struct wiphy *wiphy, const char *name,
							 struct wireless_dev **p2p_wdev, enum nl80211_iftype type)
{
	struct wireless_dev *wdev = NULL;
	int ret = 0;

	if (type == NL80211_IFTYPE_P2P_DEVICE) {
		memset(&(global_idev.p2p_global), 0, sizeof(struct p2p_priv_t));

		/* check whether old wdev exits(normally already freed in del intf)*/
		if (global_idev.p2p_global.pd_pwdev) {
			dev_info(global_idev.pwhc_dev, "%s: wdev already exists", __func__);
			ret = -EBUSY;
			return ret;
		}

		/* Release softap ndev (pndev[1]/wlan1) to make room for future P2P GO.
		 * Use whc_p2p_ndev_unregister() to avoid re-acquiring wiphy_mutex. */
		if (global_idev.pwdev_global[1]) {
			whc_p2p_ndev_unregister(global_idev.pndev[1], global_idev.pwdev_global[1]);
			global_idev.pwdev_global[1] = NULL;
			global_idev.pndev[1] = NULL;
		}

		/* P2P DEVICE has no netdev*/
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

	} else if (type == NL80211_IFTYPE_P2P_GO) {
		/* pndev[1] was already released in the P2P_DEVICE step above;
		 * register the new GO ndev directly at slot 1. */
		whc_host_ndev_p2p_register(type, name, 1);
		*p2p_wdev = global_idev.pwdev_global[1];

	} else if (type == NL80211_IFTYPE_P2P_CLIENT) {
		/* GC will occupy slot 0 of pndev and pwdev.  As pndev[0] is the active STA ndev (wlan0)
		 * which is RUNNING (P2P discovery scan), so move wlan0 ndev and wdev to slot 1, then
		 * alloc net ndev and wdev for GC to slot 0. */
		global_idev.pndev[1] = global_idev.pndev[0];
		global_idev.pwdev_global[1] = global_idev.pwdev_global[0];
		global_idev.pndev[0] = NULL;
		global_idev.pwdev_global[0] = NULL;
		rtw_netdev_idx(global_idev.pndev[1]) = 1;

		whc_host_ndev_p2p_register(type, name, 0);
		*p2p_wdev = global_idev.pwdev_global[0];
	}
	return ret;
}

void whc_host_p2p_iface_free(struct wiphy *wiphy, struct wireless_dev *wdev)
{
	enum nl80211_iftype iftype = wdev->iftype;

	if (iftype == NL80211_IFTYPE_P2P_DEVICE) {
		whc_host_p2p_pdwdev_free();
	} else if (iftype == NL80211_IFTYPE_P2P_CLIENT) {
		/* Restore port0 and port1 mac address: port0 mac = efuse_mac, port1 mac = efuse_mac+1 */
		whc_host_p2p_gc_intf_revert(1);

		/* Tear down the GC ndev at slot 0 */
		if (global_idev.pndev[0]) {
			whc_p2p_ndev_unregister(global_idev.pndev[0], global_idev.pwdev_global[0]);
			global_idev.pwdev_global[0] = NULL;
			global_idev.pndev[0] = NULL;
		}

		/* Move the STA ndev back from slot 1 to slot 0 */
		if (global_idev.pndev[1]) {
			global_idev.pndev[0] = global_idev.pndev[1];
			global_idev.pwdev_global[0] = global_idev.pwdev_global[1];
			global_idev.pndev[1] = NULL;
			global_idev.pwdev_global[1] = NULL;
			rtw_netdev_idx(global_idev.pndev[0]) = 0;
		}
	} else if (iftype == NL80211_IFTYPE_P2P_GO) {
		/* Tear down the GO ndev at slot 1. */
		if (global_idev.pndev[1]) {
			whc_p2p_ndev_unregister(global_idev.pndev[1], global_idev.pwdev_global[1]);
			global_idev.pwdev_global[1] = NULL;
			global_idev.pndev[1] = NULL;
		}
	}

	whc_host_set_p2p_role(P2P_ROLE_DISABLE);
	global_idev.p2p_global.p2p_role = P2P_ROLE_DISABLE;

	return;
}

/* GC (pndev[0], port0) gets efuse_mac+1; P2P Device moves to port1 with efuse_mac */
void whc_host_p2p_driver_macaddr_switch(void)
{
	int softap_addr_offset_idx = global_idev.wifi_user_config.softap_addr_offset_idx;
	unsigned char gc_mac[ETH_ALEN];

	/* Derive GC MAC from P2P Device MAC (efuse_mac) using the same offset as AP MAC */
	memcpy(gc_mac, global_idev.p2p_global.pd_pwdev->address, ETH_ALEN);
	if (softap_addr_offset_idx == 0) {
		gc_mac[softap_addr_offset_idx] += (1 << 1);
	} else {
		gc_mac[softap_addr_offset_idx] += 1;
	}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0))
	memcpy((void *)global_idev.pndev[0]->dev_addr, gc_mac, ETH_ALEN);
#else
	eth_hw_addr_set(global_idev.pndev[0], gc_mac);
#endif
	whc_host_set_mac_addr(0, gc_mac);
	whc_host_set_mac_addr(1, global_idev.p2p_global.pd_pwdev->address);
}

void whc_host_p2p_gc_intf_revert(u8 need_if2_deinit)
{
	int softap_addr_offset_idx = global_idev.wifi_user_config.softap_addr_offset_idx;
	unsigned char ap_mac[ETH_ALEN];

	if (!global_idev.p2p_global.pd_wlan_idx) {
		return;
	}

	global_idev.p2p_global.pd_wlan_idx = 0;

	/* Restore driver port0 to efuse_mac (P2P Device MAC) */
	whc_host_set_mac_addr(0, global_idev.p2p_global.pd_pwdev->address);

	/* Restore driver port1 to efuse_mac+1 (AP MAC) */
	memcpy(ap_mac, global_idev.p2p_global.pd_pwdev->address, ETH_ALEN);
	if (softap_addr_offset_idx == 0) {
		ap_mac[softap_addr_offset_idx] += (1 << 1);
	} else {
		ap_mac[softap_addr_offset_idx] += 1;
	}
	whc_host_set_mac_addr(1, ap_mac);

	if (need_if2_deinit) {
		whc_host_deinit_ap();
	}
}

int whc_host_p2p_get_wdex_idx(struct wireless_dev *wdev)
{
	int idx = 0;
	struct net_device *pnetdev = NULL;

	if (wdev->iftype == NL80211_IFTYPE_P2P_DEVICE) {
		idx = 2; /* used idx=2 to specialize p2p device, since it cannot refered as global_idev.pwdev_global[x]*/
	} else if (wdev->iftype == NL80211_IFTYPE_P2P_CLIENT) {
		idx = 0; /* GC uses pndev[0]/pwdev_global[0] */
	} else if (wdev->iftype == NL80211_IFTYPE_P2P_GO) {
		idx = 1; /* GO uses pndev[1]/pwdev_global[1] */
	} else { /* other non P2P cases*/
		pnetdev = wdev_to_ndev(wdev);
		if (pnetdev) {
			idx = rtw_netdev_idx(pnetdev);
		}
	}

	return idx;
}

void whc_host_p2p_pdwdev_free(void)
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

