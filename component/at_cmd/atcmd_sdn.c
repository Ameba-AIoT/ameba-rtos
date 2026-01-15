/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "atcmd_service.h"

void atcmd_sdn_cmd(void *arg);

ATCMD_TABLE_DATA_SECTION
static const log_item_t at_sdn_cmd = {"+BTSDN",    atcmd_sdn_cmd,         {NULL, NULL}};

void at_sdn_init(void)
{
#if (defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK)
	(void)atcmd_sdn_cmd;
#else
	atcmd_service_add_table((log_item_t *)&at_sdn_cmd, 1);
#endif
}