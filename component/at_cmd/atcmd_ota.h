/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_OTA_H__
#define __ATCMD_OTA_H__
#include "ameba_ota.h"
#include "platform_autoconf.h"

#define OTA_TYPE_MIN  OTA_HTTP
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBASMART)
#define OTA_TYPE_MAX  OTA_VFS
#else
#define OTA_TYPE_MAX  OTA_HTTPS
#endif

void at_ota_init(void);

#endif /* #ifndef __ATCMD_OTA_H__ */