/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/

#ifndef __RTW_NL_H__
#define __RTW_NL_H__

#include <stddef.h>
#include <stdint.h>

/*
 * rtw_nl_send_cmd - Send an "iwpriv-style" MP command (Netlink path).
 *
 * Replicates whc_host_wifi_mp() from whc_cust_path_demo: the command string is
 * sent via CMD_WIFI_SEND_BUF as [WHC_WIFI_TEST][WHC_WIFI_TEST_MP][show_msg][cmd]
 * over the "whc_demo" Netlink family. The driver replies with fragmented
 * WHC_WIFI_TEST_MP events that are reassembled and printed.
 *
 * Returns 0 on success, -1 on error.
 */
int rtw_nl_send_cmd(const char *cmd);

/*
 * rtw_nl_send_cmd_buf - Send an "iwpriv-style" MP command (Netlink path)
 * and capture the reassembled MP reply into outbuf.
 */
int rtw_nl_send_cmd_buf(const char *cmd, char *outbuf, size_t outlen);

/*
 * expect_reply modes for rtw_nl_send_buf().
 */
#define RTW_NL_NO_REPLY		0	/* fire-and-forget */
#define RTW_NL_REPLY_ONCE	1	/* read a single WHC_CMD_EVENT reply */
#define RTW_NL_REPLY_SCAN	2	/* loop reading SCAN_RESULT until idx==0 */
#define RTW_NL_REPLY_MP		3	/* loop reassembling MP result fragments */

/*
 * rtw_nl_send_buf - Send a raw buffer via CMD_WIFI_SEND_BUF (Pattern A).
 *
 * Used by commands that send WHC_WIFI_TEST + sub-command payloads
 * (getip, getmac, setrdy, tickps, wifion, scan, dhcp, connect, logon, logoff).
 *
 * expect_reply selects the reply-handling mode (see RTW_NL_* above).
 */
int rtw_nl_send_buf(const uint8_t *payload, uint32_t payload_len, int expect_reply);

/*
 * rtw_nl_send_dbg - Send debug command via CMD_WIFI_DBG (Pattern B).
 */
int rtw_nl_send_dbg(const char *dbg_cmd);

/*
 * rtw_nl_set_mac - Set MAC address via CMD_WIFI_SET_MAC (Pattern B).
 */
int rtw_nl_set_mac(uint8_t idx, const char *mac_str);

/*
 * rtw_nl_netif_on - Set network interface up via CMD_WIFI_NETIF_ON (Pattern B).
 */
int rtw_nl_netif_on(uint8_t idx);

#endif /* __RTW_NL_H__ */
