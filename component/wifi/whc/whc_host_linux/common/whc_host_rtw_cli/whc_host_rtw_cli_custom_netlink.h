// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between host linux and firmware in dpluc. The
*   commnunication between driver and firmware is SPI or SDIO.
*
* Copyright (C) 2025, Realtek Corporation. All rights reserved.
*/

#ifndef _WHC_HOST_RTW_CLI_CUSTOM_NETLINK_H_
#define _WHC_HOST_RTW_CLI_CUSTOM_NETLINK_H_


/* pkt from device */
int whc_bridge_host_send_to_user(u8 *buf, u16 size, u16 api_id);
int whc_bridge_host_send_to_user_multi(u8 *buf, u16 size, u16 api_id, u32 chuck_index, u8 last_chuck);
/* send pkt to device */
void whc_host_send_data_to_dev(u8 *pbuf, u32 len, u32 with_txdesc);

int whc_bridge_host_do_scan(struct sk_buff *skb, struct genl_info *info);
int whc_bridge_host_scan_result(void);
#if defined(CONFIG_WHC_WIFI_API_PATH)
int whc_host_do_dbg_cmd(struct sk_buff *skb, struct genl_info *info);
int whc_host_do_mp_cmd(struct sk_buff *skb, struct genl_info *info);
#endif
int pre_process_buf_data(u8 *buf, u16 size);
int whc_host_get_join_status(struct sk_buff *skb, struct genl_info *info);

#endif //_WHC_HOST_RTW_CLI_CUSTOM_NETLINK_H_
