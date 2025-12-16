/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "atcmd_service.h"

void atcmd_sdn_cmd(void *arg);

static log_item_t at_sdn_cmd = {"+BTSDN",    atcmd_sdn_cmd,         {NULL, NULL}};

void at_sdn_init(void)
{
#if (defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK)
	(void)atcmd_sdn_cmd;
#else
	atcmd_service_add_table(&at_sdn_cmd, 1);
#endif
}