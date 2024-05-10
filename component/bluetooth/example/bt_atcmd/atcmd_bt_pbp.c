/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#include <stdlib.h>
#include <stdio.h>
#include <osif.h>
#include <log_service.h>
#include <bt_utils.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_pbp.h>
#include <atcmd_bt_impl.h>

_WEAK uint16_t rtk_bt_pbp_broadcast_sink_cfg(uint8_t channel)
{
	(void)channel;
	AT_PRINTK("[ATBE] pbp broadcast sink not support cfg");
	return -1;
}

static int atcmd_bt_pbp_broadcast_sink_cfg(int argc, char **argv)
{
	uint8_t channel = 0;

	if (argc != 1) {
		AT_PRINTK("[ATBC] PBP broadcast sink op failed! wrong args num!");
		return -1;
	}
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
		AT_PRINTK("[ATBC] Unknown channel allocation");
		return -1;
	}
	if (rtk_bt_pbp_broadcast_sink_cfg(channel)) {
		AT_PRINTK("[ATBC] PBP broadcast sink config channel fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] PBP broadcast sink config channel 0x%x successfully \r\n", channel);

	return 0;
}

static const cmd_table_t pbp_broadcast_sink_cmd_table[] = {
	{"cfg",       atcmd_bt_pbp_broadcast_sink_cfg,        1, 2},
	{NULL,},
};

int atcmd_bt_pbp_cmd(int argc, char *argv[])
{
	if (argc < 2) {
		AT_PRINTK("[%s]Error: pbp atcmd should have at least 2 parameters !!!\r\n", __func__);
		return -1;
	}
	if (strcmp(argv[0], "sink") == 0) {
		AT_PRINTK("[ATBC] Set pbp broadcast sink cmd");
		atcmd_bt_excute(argc - 1, &argv[1], pbp_broadcast_sink_cmd_table, "[ATBC][pbp][sink]");
	} else {
		AT_PRINTK("[%s]Error: pbp do not support  %s \r\n", __func__, argv[0]);
		return -1;
	}

	return 0;
}
#endif