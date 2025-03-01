// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_SDIO_DEV_BRIDGE_H__
#define __WHC_SDIO_DEV_BRIDGE_H__

#include "lwip/prot/ethernet.h"

#define PORT_TO_BOTH	0x0
#define PORT_TO_UP		0x1
#define PORT_TO_HOST	0x2
#define PORT_TO_UNKNOWN 0xFF

#define IP_ICMP_REQUEST 0x08
#ifndef DHCP_SERVER_PORT
#define DHCP_SERVER_PORT 67
#endif

// Mask definitions (These values can depend on your specific mask setup)
#define MASK_SRC_IP   0x1
#define MASK_PORT     0x2
#define MASK_TYPE     0x4
#define MASK_IDX      0x8

struct bridge_pkt_attrib {
	u16_t protocol;
	u16_t src_port;
	u16_t dst_port;
	u8_t src_ip[4];
	u8_t dst_ip[4];
	u8_t flags;
	u8_t type;
	struct eth_addr src_mac;
	struct eth_addr dst_mac;
	u8_t port_idx;
};

struct whc_bridge_dev_pkt_filter {
	u32_t identity;
	u8_t src_ip[4];
	u16_t port_num; // dst port num
	u8_t index; // STA_WLAN_INDEX/SOFTAP_WLAN_INDEX
	u8_t type;  //tcp or udp or icmp
	u8_t mask;  //filter mask
	u8_t direction; //PORT_TO_BOTH/PORT_TO_UP(DEV LWIP)/PORT_TO_HOST(HOST)
	u8_t state; //entry enable or not
	u8_t rsvd;
};

extern u8(*whc_bridge_sdio_dev_pkt_redir_cusptr)(struct sk_buff *skb, struct bridge_pkt_attrib *pattrib);

u8 whc_bridge_dev_recv_pkt_process(u8 *idx, struct sk_buff **skb_send);

#endif
