// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2025, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_WPA_LITE_APP_H__
#define __WHC_WPA_LITE_APP_H__


int whc_wpa_ops_disconnect(char *ptr, u8 *buf);
int whc_wpa_ops_select_network(char *ptr, u8 *buf);
int whc_wpa_ops_list_network(char *ptr, u8 *buf);
int whc_wpa_ops_set_network(char *ptr, u8 *buf, int msg_len);
int whc_wpa_ops_get_macaddr(u8 *ptr, u8 *buf);
int whc_wpa_ops_do_scan(u8 *input, u8 *buf);
int whc_wpa_ops_get_status(char *ptr, u8 *buf);
int whc_dev_rtw_cli_wpas_test(char *ptr, u8 *buf, int msg_len);


#endif
