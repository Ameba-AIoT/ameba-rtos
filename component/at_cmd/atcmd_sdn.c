/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"
#if defined(CONFIG_SDN_DEV) && CONFIG_SDN_DEV

#include "atcmd_service.h"
extern void atcmd_sdn_cmd(u16 argc, char **argv);

ATCMD_TABLE_DATA_SECTION
const log_item_t at_sdn_cmd = {"+SDN",    atcmd_sdn_cmd};

#endif /* CONFIG_SDN_DEV */