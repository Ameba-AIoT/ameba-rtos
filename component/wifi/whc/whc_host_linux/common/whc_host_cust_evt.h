// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2025, Realtek Corporation. All rights reserved.
*/

#ifndef __RTW_LLHW_CUST_EVT_H__
#define __RTW_LLHW_CUST_EVT_H__

#include <whc_host_linux.h>

void whc_host_recv_cust_evt(u8 *buf);
void whc_host_send_cust_evt(u8 *buf, u32 len);

#endif //__RTW_LLHW_CUST_EVT_H__
