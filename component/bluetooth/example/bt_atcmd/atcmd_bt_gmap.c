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
#include <rtk_bt_gmap.h>
#include <atcmd_bt_impl.h>

_WEAK uint16_t rtk_bt_gmap_broadcast_game_receiver_cfg(uint8_t channel, uint8_t *p_neighbor_addr)
{
	(void)channel;
	(void)p_neighbor_addr;
	AT_PRINTK("[ATBE] gmap broadcast game receiver not support cfg");
	return -1;
}

_WEAK uint16_t rtk_bt_gmap_unicast_game_terminal_cfg(uint8_t channel, uint8_t *p_neighbor_addr)
{
	(void)channel;
	(void)p_neighbor_addr;
	AT_PRINTK("[ATBE] gmap unicast game terminal not support cfg");
	return -1;
}

static int atcmd_bt_gmap_broadcast_game_receiver_cfg(int argc, char **argv)
{
	uint8_t channel = 0;
	uint8_t neighbor_addr[6] = {0};

	if ((argc != 1) && (argc != 2)) {
		AT_PRINTK("[ATBC] GMAP broadcast game receiver cfg op failed! wrong args num!");
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

	if (argc == 2) {
		hexdata_str_to_bd_addr(argv[1], neighbor_addr, 6);
		if (rtk_bt_gmap_broadcast_game_receiver_cfg(channel, neighbor_addr)) {
			AT_PRINTK("[ATBC] GMAP broadcast game receiver config channel neighbor address fail \r\n");
			return -1;
		}
		AT_PRINTK("[ATBC] GMAP broadcast game receiver config channel and neighbor address successfully \r\n");
	} else {
		if (rtk_bt_gmap_broadcast_game_receiver_cfg(channel, NULL)) {
			AT_PRINTK("[ATBC] GMAP broadcast game receiver config channel fail \r\n");
			return -1;
		}
		AT_PRINTK("[ATBC] GMAP broadcast game receiver config channel successfully \r\n");
	}

	return 0;
}

static int atcmd_bt_gmap_unicast_game_terminal_cfg(int argc, char **argv)
{
	uint8_t channel = 0;
	uint8_t neighbor_addr[6] = {0};

	if ((argc != 1) && (argc != 2)) {
		AT_PRINTK("[ATBC] GMAP unicast game terminal cfg op failed! wrong args num!");
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
	if (argc == 2) {
		hexdata_str_to_bd_addr(argv[1], neighbor_addr, 6);
		if (rtk_bt_gmap_unicast_game_terminal_cfg(channel, neighbor_addr)) {
			AT_PRINTK("[ATBC] GMAP unicast game terminal config channel neighbor address fail \r\n");
			return -1;
		}
		AT_PRINTK("[ATBC] GMAP unicast game terminal config channel and neighbor address successfully \r\n");
	} else {
		if (rtk_bt_gmap_unicast_game_terminal_cfg(channel, NULL)) {
			AT_PRINTK("[ATBC] GMAP unicast game terminal config channel fail \r\n");
			return -1;
		}
		AT_PRINTK("[ATBC] GMAP unicast game terminal config channel successfully \r\n");
	}

	return 0;
}

static const cmd_table_t gmap_broadcast_game_receiver_cmd_table[] = {
	{"cfg",       atcmd_bt_gmap_broadcast_game_receiver_cfg,        1, 3},
	{NULL,},
};

static const cmd_table_t gmap_unicast_game_terminal_cmd_table[] = {
	{"cfg",       atcmd_bt_gmap_unicast_game_terminal_cfg,          1, 3},
	{NULL,},
};

int atcmd_bt_gmap_cmd(int argc, char *argv[])
{
	if (argc < 2) {
		AT_PRINTK("[%s]Error: gmap atcmd should have at least 2 parameters !!!\r\n", __func__);
		return -1;
	}
	if (strcmp(argv[0], "bgr") == 0) {
		AT_PRINTK("[ATBC] Set gmap broadcast game receiver cmd");
		atcmd_bt_excute(argc - 1, &argv[1], gmap_broadcast_game_receiver_cmd_table, "[ATBC][gmap][bgr]");
	} else if (strcmp(argv[0], "ugt") == 0) {
		AT_PRINTK("[ATBC] Set gmap unicast game terminal cmd");
		atcmd_bt_excute(argc - 1, &argv[1], gmap_unicast_game_terminal_cmd_table, "[ATBC][gmap][ugt]");
	} else {
		AT_PRINTK("[%s]Error: gmap do not support %s \r\n", __func__, argv[0]);
		return -1;
	}

	return 0;
}
#endif