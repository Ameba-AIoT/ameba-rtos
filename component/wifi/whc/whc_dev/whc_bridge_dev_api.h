// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_BRIDGE_DEV_API_H__
#define __WHC_BRIDGE_DEV_API_H__

#define WHC_BRIDGE_HOST_READY    1
#define WHC_BRIDGE_HOST_UNREADY  0


struct PktFilterNode {
	struct list_head list;
	struct whc_bridge_dev_pkt_filter filter;
};

void whc_bridge_dev_api_set_host_state(u8 state);
void whc_bridge_dev_api_set_tickps_cmd(u8 subtype);
u8 whc_bridge_dev_api_get_host_rdy(void);

void whc_bridge_dev_api_set_default_direction(u8 dir);
u8 whc_bridge_dev_api_get_default_direction(void);

void whc_bridge_dev_api_add_filter_node(struct whc_bridge_dev_pkt_filter *filter);
void whc_bridge_dev_api_delete_filter_node(u32_t identity);
void whc_bridge_dev_api_get_filter_node(struct whc_bridge_dev_pkt_filter *filter, u32_t identity);

void whc_bridge_dev_api_send_to_host(u8 *data, u32 len);
void whc_bridge_dev_pktfilter_init(void);

void whc_dev_pkt_rx_to_user(u8 *rxbuf, u8 *real_buf, u16 size);
void whc_dev_init_cmd_path_task(void);

#endif
