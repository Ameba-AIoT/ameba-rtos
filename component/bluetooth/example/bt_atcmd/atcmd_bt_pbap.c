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
#include <rtk_bt_pbap.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_br_gap.h>

static int atcmd_bt_pbap_connect(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_pbap_connect(bd_addr)) {
		BT_LOGE("PBAP connect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("PBAP connecting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_pbap_disconnect(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_pbap_disconnect(bd_addr)) {
		BT_LOGE("PBAP disconnect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("PBAP disconnecting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_pbap_set_phone_book(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	rtk_bt_pbap_set_pb_t set_pb_t = {0};

	hexdata_str_to_bd_addr(argv[0], set_pb_t.bd_addr, RTK_BD_ADDR_LEN);
	set_pb_t.repos = (rtk_bt_pbap_repository)str_to_int(argv[1]);
	set_pb_t.path = (rtk_bt_pbap_path)str_to_int(argv[2]);

	if (rtk_bt_pbap_set_phone_book(&set_pb_t)) {
		BT_LOGE("PBAP set phone book fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(set_pb_t.bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("PBAP set phone book to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_pbap_phone_book_pull(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	rtk_bt_pbap_pb_pull_t pb_pull_t = {0};

	hexdata_str_to_bd_addr(argv[0], pb_pull_t.bd_addr, RTK_BD_ADDR_LEN);
	pb_pull_t.repos = (rtk_bt_pbap_repository)str_to_int(argv[1]);
	pb_pull_t.phone_book = (rtk_bt_pbap_phone_book)str_to_int(argv[2]);
	pb_pull_t.start_offset = (uint16_t)str_to_int(argv[3]);
	pb_pull_t.max_list_count = (uint16_t)str_to_int(argv[4]);
	pb_pull_t.filter = (uint64_t)str_to_int(argv[5]);

	if (rtk_bt_pbap_phone_book_pull(&pb_pull_t)) {
		BT_LOGE("PBAP phone book pull fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(pb_pull_t.bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("PBAP phone book pull to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_pbap_phone_book_pull_continue(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_pbap_phone_book_pull_continue(bd_addr)) {
		BT_LOGE("PBAP phone book pull continue fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("PBAP phone book pull continue to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_pbap_phone_book_size_get(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	rtk_bt_pbap_pb_size_get_t pb_size_t = {0};

	hexdata_str_to_bd_addr(argv[0], pb_size_t.bd_addr, RTK_BD_ADDR_LEN);
	pb_size_t.repos = (rtk_bt_pbap_repository)str_to_int(argv[1]);
	pb_size_t.phone_book = (rtk_bt_pbap_phone_book)str_to_int(argv[2]);

	if (rtk_bt_pbap_phone_book_size_get(&pb_size_t)) {
		BT_LOGE("PBAP phone book size get fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(pb_size_t.bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("PBAP phone book size get from device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_pbap_pull_caller_id_name(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	rtk_bt_pbap_pull_caller_id_name_t caller_id = {0};

	if (strlen(argv[1]) > 20) {
		BT_LOGE("PBAP pull caller id number is overflow\r\n");
		return -1;
	}
	hexdata_str_to_bd_addr(argv[0], caller_id.bd_addr, RTK_BD_ADDR_LEN);
	memcpy((void *)caller_id.number, (void *)argv[1], strlen(argv[1]));
	if (rtk_bt_pbap_pull_caller_id_name(&caller_id)) {
		BT_LOGE("PBAP pull caller id name fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(caller_id.bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("PBAP pull caller id name to device %s ...\r\n", addr_str);

	return 0;
}

static const cmd_table_t pbap_cmd_table[] = {
	{"conn",                    atcmd_bt_pbap_connect,                  2, 2},
	{"disconn",                 atcmd_bt_pbap_disconnect,               2, 2},
	{"set_pb",                  atcmd_bt_pbap_set_phone_book,           2, 4},
	{"pull_pb",                 atcmd_bt_pbap_phone_book_pull,          2, 7},
	{"pull_pb_continue",        atcmd_bt_pbap_phone_book_pull_continue, 2, 2},
	{"get_pb_size",             atcmd_bt_pbap_phone_book_size_get,      2, 4},
	{"pull_id",                 atcmd_bt_pbap_pull_caller_id_name,      2, 3},
	{NULL,},
};

int atcmd_bt_pbap_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, pbap_cmd_table, "[AT+BTPBAP]");
}