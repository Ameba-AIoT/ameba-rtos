// SPDX-License-Identifier: GPL-2.0-only
#include <linux/types.h>
#include <linux/if_ether.h>
#include <linux/timer.h>
#include <net/cfg80211.h>
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC (Inter Process
*   Communication) bus, SPI, SDIO and USB.
*
* Copyright (C) 2025, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_HOST_FT_H__
#define __WHC_HOST_FT_H__

#ifdef CONFIG_IEEE80211R
void whc_host_ft_init(void);
void whc_host_ft_set_unassociated(void);
int whc_host_ft_set_bssid(const u8 *target_bssid);
int whc_host_ft_event(u32 event, char *evt_info, unsigned int join_status);
int whc_host_update_ft_ies(struct wiphy *wiphy, struct net_device *dev, struct cfg80211_update_ft_ies_params *ftie);
#endif /* CONFIG_IEEE80211R */

#endif //__WHC_HOST_FT_H__
