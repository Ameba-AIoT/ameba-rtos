// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __INIC_SDIO_DEV_BRIDGE_H__
#define __INIC_SDIO_DEV_BRIDGE_H__

u8 inic_dev_recv_bridge_process(u8 *idx, struct sk_buff **skb_send);
void inic_event_bridge_DHCP(u32 api_id, u32 *param_buf);
void inic_event_bridge_get_ip(u32 api_id, u32 *param_buf);
void inic_event_bridge_get_scan_res(u32 api_id, u32 *param_buf);
void inic_event_bridge_get_dev_mac(u32 api_id, u32 *param_buf);
#endif
