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

/* pkt rx: pkt from device to host kernel space, default to user space by netlink */
/* weak func in rtk code, Reimplement the function if needed */
int whc_host_buf_rx_to_user(u8 *buf, u16 size);

/* pkt tx: send pkt to device from host kernel space */
/* pkt to dev need an extra size for txdesc,set 1 if size have reserved already */
void whc_host_send_data_to_dev(u8 *pbuf, u32 len, u32 with_txdesc);

#endif