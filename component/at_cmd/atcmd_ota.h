/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_OTA_H__
#define __ATCMD_OTA_H__

#define OTA_MAX_HOSTNAME_LEN    128
#define OTA_MAX_PATH_LEN        256

#define OTA_STATUS_RUNNING      BIT(0)
#define OTA_STATUS_SYSRST       BIT(1)

enum OTA_CONN_TYPE {
	OTA_HTTP_OVER_TCP = 1,
	OTA_HTTPS_VERIFY_NONE,
	OTA_HTTPS_VERIFY_SERVER,
	OTA_HTTPS_VERIFY_CLIENT,
	OTA_HTTPS_VERIFY_BOTH,
};

void at_ota_init(void);

#endif /* #ifndef __ATCMD_OTA_H__ */