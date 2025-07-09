// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between host linux and firmware in dpluc. The
*   commnunication between driver and firmware is SPI or SDIO.
*
* Copyright (C) 2025, Realtek Corporation. All rights reserved.
*/

#ifndef _WHC_HOST_CMD_PATH_API_H_
#define _WHC_HOST_CMD_PATH_API_H_

#include <whc_host_linux.h>

/* pkt from device */
int whc_host_buf_rx_to_user(u8 *buf, u16 size);
/* send pkt to device */
void whc_host_send_data_to_dev(u8 *pbuf, u32 len, u32 with_txdesc);

/* for pkt from user space to dev by cmd path */
void whc_host_send_cmd_data(u8 *buf, u32 len);
int whc_host_cmd_data_rx_to_user(struct sk_buff *pskb);

#endif
