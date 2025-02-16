// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __RTW_ACS_H__
#define __RTW_ACS_H__

int cfg80211_rtw_dump_survey_params(struct wiphy *wiphy, struct net_device *netdev, int idx, struct survey_info *info);

#endif
