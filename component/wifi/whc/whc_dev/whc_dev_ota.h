// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_DEV_OTA_H__
#define __WHC_DEV_OTA_H__

#include "whc_dev.h"

#define WHC_OTA_START		0
#define WHC_OTA_READ		1
#define WHC_OTA_WRITE		2
#define WHC_OTA_END			3

/* ota type */
#define OTA_FOR_NORMAL		1
#define OTA_FOR_RMESH		2

struct whc_dev_ota_cmd {
	u8 type;
	u8 subtype;
	u8 rsvd[2];
	u32 port;
	char host[16];
	/* fix less than 20B now */
	char resource[20];
};

struct whc_dev_ota_hdr {
	/* fix WHC_WIFI_TEST_OTA*/
	uint8_t type;
	uint8_t subtype;
	int16_t len;
};

struct whc_dev_ota_info {
	uint32_t port;
	char host[16];
	/* fix less than 20B now */
	char resource[20];
	uint8_t ota_type;
	uint8_t rmesh_ota_type;
	uint8_t rsvd[2];
};

struct whc_dev_ota_readbuf {
	struct whc_dev_ota_hdr ota_hdr;
	u8 *buf;
};

void whc_dev_api_ota_process(u8 *buf);
int whc_dev_ota_read(u8 *data, int data_len);
int whc_dev_ota_close(void);

#endif
