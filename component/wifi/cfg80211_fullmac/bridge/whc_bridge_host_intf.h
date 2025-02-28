// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between host linux and firmware in dpluc. The
*   commnunication between driver and firmware is SPI or SDIO.
*
* Copyright (C) 2025, Realtek Corporation. All rights reserved.
*/

#include <whc_host_linux.h>

#ifndef _WHC_BRIDGE_HOST_INTF_H_
#define _WHC_BRIDGE_HOST_INTF_H_

void whc_bridge_host_send_data(u8 *buf, u32 len);
int whc_bridge_host_pkt_rx_to_user(struct sk_buff *pskb);

#endif
