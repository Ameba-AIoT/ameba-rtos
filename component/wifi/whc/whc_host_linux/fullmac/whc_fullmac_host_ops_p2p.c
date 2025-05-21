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
static int whc_host_start_p2p_device(struct wiphy *wiphy, struct wireless_dev *wdev)
{
	if (global_idev.p2p_global.p2p_role == P2P_ROLE_DISABLE) {
		global_idev.p2p_global.p2p_role = P2P_ROLE_DEVICE;
		whc_fullmac_host_set_p2p_role(P2P_ROLE_DEVICE);
	}

	return 0;
}

static void whc_host_stop_p2p_device(struct wiphy *wiphy, struct wireless_dev *wdev)
{
	whc_fullmac_host_scan_abort();
	global_idev.p2p_global.p2p_role = P2P_ROLE_DISABLE;
	whc_fullmac_host_set_p2p_role(P2P_ROLE_DISABLE);
}

void whc_host_ops_p2p_init(void)
{
	struct cfg80211_ops *ops = &global_idev.rtw_cfg80211_ops;
	ops->start_p2p_device = whc_host_start_p2p_device;
	ops->stop_p2p_device = whc_host_stop_p2p_device;
}
#endif
