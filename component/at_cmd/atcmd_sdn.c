/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"
#if defined(CONFIG_SDN) && CONFIG_SDN

#include "atcmd_service.h"

void atcmd_sdn_cmd(void *arg);

ATCMD_TABLE_DATA_SECTION
const log_item_t at_sdn_cmd = {"+BTSDN",    atcmd_sdn_cmd};

#endif /* CONFIG_SDN */