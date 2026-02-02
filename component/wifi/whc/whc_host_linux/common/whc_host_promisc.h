// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2024, Realtek Corporation. All rights reserved.
*/

#ifndef __RTW_PROMISC_H__
#define __RTW_PROMISC_H__

#include <whc_host_linux.h>

int rtw_promisc_rx(struct rtw_rx_pkt_info *ppktinfo);

#endif //__RTW_PROMISC_H__
