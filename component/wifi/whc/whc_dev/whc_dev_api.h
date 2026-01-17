// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_DEV_API_H__
#define __WHC_DEV_API_H__

#include "whc_dev.h"

#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
#include "whc_dev_tcpip.h"
#endif

#define WHC_HOST_READY    1
#define WHC_HOST_UNREADY  0

struct whc_dev_network_info {
	u8 ip[4];
	u8 gw[4];
	u8 gw_mask[4];
};

void whc_dev_init_cmd_path_task(void);

#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
u8 whc_dev_api_get_host_rdy(void);

void whc_dev_api_set_default_direction(u8 dir);
u8 whc_dev_api_get_default_direction(void);

void whc_dev_api_add_filter_node(struct whc_dev_pkt_filter *filter);
void whc_dev_api_delete_filter_node(u32_t identity);
void whc_dev_api_get_filter_node(struct whc_dev_pkt_filter *filter, u32_t identity);

void whc_dev_pktfilter_init(void);
#endif

void whc_dev_api_set_host_state(u8 state);

/* pkt rx: pkt from host to device */
/* weak func in rtk code, Reimplement the function if needed */
void whc_dev_pkt_rx_to_user(u8 *rxbuf, u8 *real_buf, u16 size);

/* pkt tx: pkt send to host from dev */
void whc_dev_api_send_to_host(u8 *data, u32 len);

u8 whc_dev_api_bus_is_idle(void);
#endif
