// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2025, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_WPAS_STD_APP_H__
#define __WHC_WPAS_STD_APP_H__


int whc_wpa_ops_get_status(char *ptr, u8 *buf);
int whc_dev_rtw_cli_wpas_test(char *ptr, u8 *buf, int msg_len);
int whc_dev_rtw_cli_remove_network(char *ptr, u8 *buf);


#endif
