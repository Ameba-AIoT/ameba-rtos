/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if defined(CONFIG_BT_GMAP_SUPPORT) && CONFIG_BT_GMAP_SUPPORT
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <atcmd_service.h>
#include <bt_utils.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_gmap.h>
#include <atcmd_bt_impl.h>

static int atcmd_bt_gmap_broadcast_game_receiver_cfg(int argc, char **argv)
{
	uint8_t channel = 0;
	uint8_t neighbor_addr[6] = {0};

	if ((argc != 1) && (argc != 2)) {
		BT_LOGE("GMAP broadcast game receiver cfg op failed! wrong args num!\r\n");
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
		BT_LOGE("Unknown channel allocation\r\n");
		return -1;
	}

	if (argc == 2) {
		hexdata_str_to_bd_addr(argv[1], neighbor_addr, 6);
		if (rtk_bt_gmap_broadcast_game_receiver_cfg(channel, neighbor_addr)) {
			BT_LOGE("GMAP broadcast game receiver config channel neighbor address fail\r\n");
			return -1;
		}
		BT_LOGA("GMAP broadcast game receiver config channel and neighbor address successfully\r\n");
	} else {
		if (rtk_bt_gmap_broadcast_game_receiver_cfg(channel, NULL)) {
			BT_LOGE("GMAP broadcast game receiver config channel fail\r\n");
			return -1;
		}
		BT_LOGA("GMAP broadcast game receiver config channel successfully\r\n");
	}

	return 0;
}

static int atcmd_bt_gmap_unicast_game_terminal_cfg(int argc, char **argv)
{
	uint8_t channel = 0;
	uint8_t neighbor_addr[6] = {0};

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
	if (argc == 2) {
		hexdata_str_to_bd_addr(argv[1], neighbor_addr, 6);
		if (rtk_bt_gmap_unicast_game_terminal_cfg(channel, neighbor_addr)) {
			BT_LOGE("GMAP unicast game terminal config channel neighbor address fail\r\n");
			return -1;
		}
		BT_LOGA("GMAP unicast game terminal config channel and neighbor address successfully\r\n");
	} else {
		if (rtk_bt_gmap_unicast_game_terminal_cfg(channel, NULL)) {
			BT_LOGE("GMAP unicast game terminal config channel fail\r\n");
			return -1;
		}
		BT_LOGA("GMAP unicast game terminal config channel successfully\r\n");
	}

	return 0;
}

static const cmd_table_t gmap_broadcast_game_receiver_cmd_table[] = {
	{"cfg",       atcmd_bt_gmap_broadcast_game_receiver_cfg,        2, 3},
	{NULL,},
};

static const cmd_table_t gmap_unicast_game_terminal_cmd_table[] = {
	{"cfg",       atcmd_bt_gmap_unicast_game_terminal_cfg,          2, 3},
	{NULL,},
};

int atcmd_bt_gmap_cmd(int argc, char *argv[])
{
	int ret = 0;
	char tag[80] = "[AT+BLEGMAP]";
	if (strcmp(argv[0], "bgr") == 0) {
		BT_LOGA("Set gmap broadcast game receiver cmd\r\n");
		strcat(tag, "[bgr]");
		ret = atcmd_bt_excute(argc - 1, &argv[1], gmap_broadcast_game_receiver_cmd_table, tag);
	} else if (strcmp(argv[0], "ugt") == 0) {
		BT_LOGA("Set gmap unicast game terminal cmd\r\n");
		strcat(tag, "[ugt]");
		ret = atcmd_bt_excute(argc - 1, &argv[1], gmap_unicast_game_terminal_cmd_table, tag);
	} else {
		BT_LOGE("[%s]Error: gmap do not support %s\r\n", __func__, argv[0]);
		ret = -1;
	}

	return ret;
}
#endif