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

static void rtw_ethtool_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{

}

static int rtw_ethtool_get_sset_count(struct net_device *dev, int sset)
{
	return 0;
}

static void rtw_ethtool_get_strings(struct net_device *dev, u32 sset, u8 *data)
{

}

static void rtw_ethtool_get_stats(struct net_device *dev,
								  struct ethtool_stats *stats,
								  u64 *data)
{

}

void rtw_ethtool_ops_init(void)
{
	struct ethtool_ops *ops = &global_idev.rtw_ethtool_ops;

	ops->get_drvinfo = rtw_ethtool_get_drvinfo;
	ops->get_link = ethtool_op_get_link;
	ops->get_strings = rtw_ethtool_get_strings;
	ops->get_ethtool_stats = rtw_ethtool_get_stats;
	ops->get_sset_count = rtw_ethtool_get_sset_count;
}
