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
#include <rtk_bt_avrcp.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_br_gap.h>

static int atcmd_bt_avrcp_play(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_play(bd_addr)) {
		BT_LOGE("AVRCP play fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP play %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_pause(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_pause(bd_addr)) {
		BT_LOGE("AVRCP pause fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP pause %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_stop(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_stop(bd_addr)) {
		BT_LOGE("AVRCP stop fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP stop %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_forward(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_forward(bd_addr)) {
		BT_LOGE("AVRCP forward fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP forward %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_backward(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_backward(bd_addr)) {
		BT_LOGE("AVRCP backward fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP backward %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_rewind_start(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_rewind_start(bd_addr)) {
		BT_LOGE("AVRCP rewind start fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP rewind start %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_rewind_stop(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_rewind_stop(bd_addr)) {
		BT_LOGE("AVRCP rewind stop fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP rewind stop %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_fast_forward_start(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_fast_forward_start(bd_addr)) {
		BT_LOGE("AVRCP fast forward start fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP fast forward start %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_fast_forward_stop(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_fast_forward_stop(bd_addr)) {
		BT_LOGE("AVRCP fast forward stop fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP fast forward stop %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_volume_set(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};
	uint8_t volume = 0;

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	volume = (uint8_t)str_to_int(argv[1]);
	if (rtk_bt_avrcp_absolute_volume_set(bd_addr, volume)) {
		BT_LOGE("AVRCP volume set fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP volume set %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_volume_change_req(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};
	uint8_t volume = 0;

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	volume = (uint8_t)str_to_int(argv[1]);
	if (rtk_bt_avrcp_volume_change_req(bd_addr, volume)) {
		BT_LOGE("AVRCP volume set fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP volume set %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_connect(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_connect(bd_addr)) {
		BT_LOGE("AVRCP connect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP connecting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_disconnect(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_disconnect(bd_addr)) {
		BT_LOGE("AVRCP disconnect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP disconnecting to device %s ...\r\n", addr_str);

	return 0;
}

static const cmd_table_t avrcp_cmd_table[] = {
	{"play",                 atcmd_bt_avrcp_play,                  2, 2},
	{"pause",                atcmd_bt_avrcp_pause,                 2, 2},
	{"stop",                 atcmd_bt_avrcp_stop,                  2, 2},
	{"forward",              atcmd_bt_avrcp_forward,               2, 2},
	{"backward",             atcmd_bt_avrcp_backward,              2, 2},
	{"rewind_start",         atcmd_bt_avrcp_rewind_start,          2, 2},
	{"rewind_stop",          atcmd_bt_avrcp_rewind_stop,           2, 2},
	{"fast_forward_start",   atcmd_bt_avrcp_fast_forward_start,    2, 2},
	{"fast_forward_stop",    atcmd_bt_avrcp_fast_forward_stop,     2, 2},
	{"volume_set",           atcmd_bt_avrcp_volume_set,            3, 3},
	{"volume_change_req",    atcmd_bt_avrcp_volume_change_req,     3, 3},
	{"conn",                 atcmd_bt_avrcp_connect,               2, 2},
	{"disconn",              atcmd_bt_avrcp_disconnect,            2, 2},
	{NULL,},
};

int atcmd_bt_avrcp_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, avrcp_cmd_table, "[AT+BTAVRCP]");
}