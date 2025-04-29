// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between host linux and firmware in dpluc. The
*   commnunication between driver and firmware is SPI or SDIO.
*
* Copyright (C) 2025, Realtek Corporation. All rights reserved.
*/

#ifndef _RTW_SDIO_BRIDGE_KERNEL_API_H_
#define _RTW_SDIO_BRIDGE_KERNEL_API_H_

/* pkt from device */
int whc_bridge_host_buf_rx_to_user(u8 *buf, u16 size);
/* send pkt to device */
void whc_bridge_host_send_data_to_dev(u8 *pbuf, u32 len, u32 with_txdesc);

#endif
