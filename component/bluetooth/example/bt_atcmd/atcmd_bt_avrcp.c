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

static int atcmd_bt_avrcp_element_attr_get(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};
	uint8_t attr;

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	attr = (uint8_t)str_to_int(argv[1]);
	if (rtk_bt_avrcp_get_element_attr(bd_addr, 1, &attr)) {
		BT_LOGE("AVRCP element attr get fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP element attr get %s ...\r\n", addr_str);

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

static int atcmd_bt_avrcp_cover_art_connect(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_cover_art_connect(bd_addr)) {
		BT_LOGE("AVRCP cover art connect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP cover art connecting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_app_setting_attrs_list(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_app_setting_attrs_list(bd_addr)) {
		BT_LOGE("AVRCP list app setting attrs fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP litsing app setting attrs to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_app_setting_values_list(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t attr_id = 0;
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	attr_id = (uint8_t)str_to_int(argv[1]);
	if (rtk_bt_avrcp_app_setting_values_list(bd_addr, attr_id)) {
		BT_LOGE("AVRCP list app setting values fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP listing app setting values to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_get_app_setting(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t attr_id = 0;
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	attr_id = (uint8_t)str_to_int(argv[1]);
	if (rtk_bt_avrcp_app_setting_value_get(bd_addr, 1, &attr_id)) {
		BT_LOGE("AVRCP get app setting fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP get app setting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_avrcp_set_app_setting(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t attr_id = 0;
	uint8_t setting = 0;
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	attr_id = (uint8_t)str_to_int(argv[1]);
	setting = (uint8_t)str_to_int(argv[2]);
	if (rtk_bt_avrcp_app_setting_value_set(bd_addr, attr_id, setting)) {
		BT_LOGE("AVRCP set app setting fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP set app setting to device %s ...\r\n", addr_str);

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

static int atcmd_bt_avrcp_get_play_status(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_avrcp_get_play_status_req(bd_addr)) {
		BT_LOGE("AVRCP get play status fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("AVRCP sending get play status request to device %s ...\r\n", addr_str);

	return 0;
}

static const cmd_table_t avrcp_cmd_table[] = {
	{"play",                    atcmd_bt_avrcp_play,                    2, 2},
	{"pause",                   atcmd_bt_avrcp_pause,                   2, 2},
	{"stop",                    atcmd_bt_avrcp_stop,                    2, 2},
	{"forward",                 atcmd_bt_avrcp_forward,                 2, 2},
	{"backward",                atcmd_bt_avrcp_backward,                2, 2},
	{"rewind_start",            atcmd_bt_avrcp_rewind_start,            2, 2},
	{"rewind_stop",             atcmd_bt_avrcp_rewind_stop,             2, 2},
	{"fast_forward_start",      atcmd_bt_avrcp_fast_forward_start,      2, 2},
	{"fast_forward_stop",       atcmd_bt_avrcp_fast_forward_stop,       2, 2},
	{"volume_set",              atcmd_bt_avrcp_volume_set,              3, 3},
	{"volume_change_req",       atcmd_bt_avrcp_volume_change_req,       3, 3},
	{"element_attr",            atcmd_bt_avrcp_element_attr_get,        3, 3},
	{"conn",                    atcmd_bt_avrcp_connect,                 2, 2},
	{"cover_art_conn",          atcmd_bt_avrcp_cover_art_connect,       2, 2},
	{"app_setting_attrs_list",  atcmd_bt_avrcp_app_setting_attrs_list,  2, 2},
	{"app_setting_values_list", atcmd_bt_avrcp_app_setting_values_list, 3, 3},
	{"get_app_setting",         atcmd_bt_avrcp_get_app_setting,         3, 3},
	{"set_app_setting",         atcmd_bt_avrcp_set_app_setting,         4, 4},
	{"get_play_status",         atcmd_bt_avrcp_get_play_status,         2, 2},
	{"disconn",                 atcmd_bt_avrcp_disconnect,              2, 2},
	{NULL,},
};

int atcmd_bt_avrcp_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, avrcp_cmd_table, "[AT+BTAVRCP]");
}