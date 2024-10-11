/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if defined(CONFIG_BT_PBP_SUPPORT) && CONFIG_BT_PBP_SUPPORT
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <atcmd_service.h>
#include <bt_utils.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_pbp.h>
#include <atcmd_bt_impl.h>

static int atcmd_bt_pbp_broadcast_sink_cfg(int argc, char **argv)
{
	(void)argc;
	uint8_t channel = 0;

	if (strcmp(argv[0], "left") == 0) {
		/* RTK_BT_LE_AUDIO_LEFT */
		channel = 1;
	} else if (strcmp(argv[0], "right") == 0) {
		/* RTK_BT_LE_AUDIO_RIGHT */
		channel = 2;
	} else if (strcmp(argv[0], "stereo") == 0) {
		/* RTK_BT_LE_AUDIO_STEREO */
		channel = 3;
	} else {
		BT_LOGE("Unknown channel allocation\r\n");
		return -1;
	}
	if (rtk_bt_pbp_broadcast_sink_cfg(channel)) {
		BT_LOGE("PBP broadcast sink config channel fail\r\n");
		return -1;
	}
	BT_LOGA("PBP broadcast sink config channel 0x%x successfully\r\n", channel);

	return 0;
}

static const cmd_table_t pbp_broadcast_sink_cmd_table[] = {
	{"cfg",       atcmd_bt_pbp_broadcast_sink_cfg,        2, 2},
	{NULL,},
};

int atcmd_bt_pbp_cmd(int argc, char *argv[])
{
	int ret = 0;
	char tag[80] = "[AT+BLEPBP][sink]";
	if (strcmp(argv[0], "sink") == 0) {
		BT_LOGA("Set pbp broadcast sink cmd\r\n");
		ret = atcmd_bt_excute(argc - 1, &argv[1], pbp_broadcast_sink_cmd_table, tag);
	} else {
		BT_LOGE("[%s]Error: pbp do not support %s\r\n", __func__, argv[0]);
		ret = -1;
	}

	return ret;
}
#endif