/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "atcmd_bt_mp.h"

#if defined(CONFIG_BT) && CONFIG_BT
#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED

static bool open_flag = 0;

static int mp_ext2_uart_bridge(void **argv, int argc)
{
	MP_EXT2_PRINTF("_AT_MP_BRIDGE_\n");

	if (argc == 1) {
		if (strncmp(argv[0], "close", 5) == 0) {
			MP_EXT2_PRINTF("close UART bridge.\n");
			if (open_flag == 0) {
				return 0;
			}

			bt_uart_bridge_close();
			open_flag = 0;
			return 0;
		}
	}

	MP_EXT2_PRINTF("open UART bridge.\n");
	bt_uart_bridge_open();
	open_flag = 1;

	return 0;
}

static int mp_ext2_bt_power(void **argv, int argc)
{
	(void)argc;

	if (strcmp(argv[0], "on") == 0) {
		MP_EXT2_PRINTF("BT power on.\n\r");
		wifi_btcoex_set_pta(PTA_BT, PTA_HOST_BT, COMMON_ACTION);
		rtk_bt_mp_power_on();
	} else if (strcmp(argv[0], "off") == 0) {
		MP_EXT2_PRINTF("BT power off.\n\r");
		rtk_bt_mp_power_off();
		wifi_btcoex_set_pta(PTA_WIFI, PTA_HOST_BT, COMMON_ACTION);
	}

	return 0;
}

static int mp_ext2_gnt_bt(void **argv, int argc)
{
	(void)argc;

	if (strcmp(argv[0], "wifi") == 0) {
		MP_EXT2_PRINTF("Switch GNT_BT to WIFI.\n\r");
		wifi_btcoex_set_pta(PTA_WIFI, PTA_HOST_BT, COMMON_ACTION);
	} else if (strcmp(argv[0], "bt") == 0) {
		MP_EXT2_PRINTF("Switch GNT_BT to BT.\n\r");
		wifi_btcoex_set_pta(PTA_BT, PTA_HOST_BT, COMMON_ACTION);
	}

	return 0;
}

static int mp_ext2_ant(void **argv, int argc)
{
	(void)argc;

	if (strcmp(argv[0], "s0") == 0) {
		MP_EXT2_PRINTF("BT use dedicated RF s0.\n\r");
		wifi_btcoex_set_bt_ant(0);
		rtk_bt_set_bt_antenna(0);
	} else if (strcmp(argv[0], "s1") == 0) {
		MP_EXT2_PRINTF("BT use share RF s1.\n\r");
		wifi_btcoex_set_bt_ant(1);
		rtk_bt_set_bt_antenna(1);
	}

	return 0;
}

at_mp_ext_item_t at_mp_ext2_items[] = {
	{"bridge",		mp_ext2_uart_bridge,		UART_BRIDGE_USAGE},
	{"bt_power",	mp_ext2_bt_power,			BT_POWER_USAGE},
	{"gnt_bt",		mp_ext2_gnt_bt,				GNT_BT_USAGE},
	{"ant",			mp_ext2_ant,				SELECTION_BT_ANTENNA},
};

void fATM2(void *arg)
{
	int argc = 0, idx, cmd_cnt;
	char *argv[MAX_ARGC] = {0};

	cmd_cnt = sizeof(at_mp_ext2_items) / sizeof(at_mp_ext2_items[0]);
	argc = parse_param(arg, argv);
	if (argc == 1) {
		_AT_PRINTK("\n");
		MP_EXT2_PRINTF("Command usage :\n");
		for (idx = 0; idx < cmd_cnt; idx++) {
			_AT_PRINTK("%s", at_mp_ext2_items[idx].mp_ext_usage);
		}
	} else {
		for (idx = 0; idx < cmd_cnt; idx++) {
			if (strcmp(argv[1], at_mp_ext2_items[idx].mp_ext_cmd) == 0) {
				int (*fun)(void **argv, int argc) = at_mp_ext2_items[idx].mp_ext_fun;
				fun((void **)&argv[2], argc - 2);
				return;
			}
		}
		MP_EXT2_PRINTF("unknown command %s.\n", argv[1]);
	}
}

//-------- AT MP commands ---------------------------------------------------------------
log_item_t at_mp_items[] = {
	{"ATM2", fATM2, {NULL, NULL}},	// MP ext2 AT command
};

void at_mp_init(void)
{
	log_service_add_table(at_mp_items, sizeof(at_mp_items) / sizeof(at_mp_items[0]));
}

#ifdef SUPPORT_LOG_SERVICE
log_module_init(at_mp_init);
#endif

#endif /* #if CONFIG_ATCMD_MP */
#endif /* #if CONFIG_BT */

