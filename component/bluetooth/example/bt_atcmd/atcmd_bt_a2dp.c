/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <osif.h>

#include "platform_autoconf.h"
#include <atcmd_service.h>
#include <bt_utils.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_a2dp.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_br_gap.h>

static int atcmd_bt_a2dp_connect(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_a2dp_connect(bd_addr)) {
		BT_LOGE("A2DP connect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("A2DP connecting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_a2dp_disconnect(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_a2dp_disconnect(bd_addr)) {
		BT_LOGE("A2DP disconnect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("A2DP disconnecting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_a2dp_start(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_a2dp_start(bd_addr)) {
		BT_LOGE("A2DP start fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("A2DP starting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_a2dp_suspend(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_a2dp_suspend(bd_addr)) {
		BT_LOGE("A2DP suspend fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("A2DP suspending to device %s ...\r\n", addr_str);

	return 0;
}

static const cmd_table_t a2dp_cmd_table[] = {
	{"conn",       atcmd_bt_a2dp_connect,        2, 2},
	{"disconn",    atcmd_bt_a2dp_disconnect,     2, 2},
	{"start",      atcmd_bt_a2dp_start,          2, 2},
	{"suspend",    atcmd_bt_a2dp_suspend,        2, 2},
	{NULL,},
};

int atcmd_bt_a2dp_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, a2dp_cmd_table, "[AT+BTA2DP]");
}