// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __RTW_DRV_PROBE_H__
#define __RTW_DRV_PROBE_H__

int __init rtw_drv_entry(void);
void __exit rtw_drv_halt(void);

void rtw_inetaddr_notifier_register(void);
void rtw_inetaddr_notifier_unregister(void);

int rtw_netdev_probe(struct device *pdev);
int rtw_netdev_remove(struct device *pdev);

#endif // __RTW_DRV_PROBE_H__
