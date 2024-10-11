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
#include <rtk_bt_rfc.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_br_gap.h>

static int atcmd_bt_rfc_connect(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_rfc_connect(bd_addr)) {
		BT_LOGE("RFC connect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("RFC connecting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_rfc_disconnect(int argc, char **argv)
{
	(void)argc;
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};
	char addr_str[30] = {0};

	if (hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN) == false) {
		BT_LOGE("RFC send data op failed! wrong bd addr!\r\n");
		return -1;
	}
	if (rtk_bt_rfc_disconnect(bd_addr)) {
		BT_LOGE("RFC disconnect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("RFC send disconnect request to device %s \r\n", addr_str);

	return 0;
}

static int atcmd_bt_rfc_send_data(int argc, char **argv)
{
	(void)argc;
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};
	uint8_t server_chann = 0;
	uint16_t data_send_len = 0;
	uint8_t data_send_buf[128] = {0};
	char addr_str[30] = {0};
	rtk_bt_rfc_data_send_t send_data_t = {0};

	if (hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN) == false) {
		BT_LOGE("RFC send data op failed! wrong bd addr!\r\n");
		return -1;
	}
	data_send_len = strlen(argv[2]) / 2;
	if (hexdata_str_to_array(argv[2], data_send_buf, data_send_len) == false) {
		BT_LOGE("RFC send data op failed! wrong hex data!\r\n");
		return -1;
	}
	server_chann = (uint8_t)(str_to_int(argv[1]));
	memcpy((void *)send_data_t.bd_addr, bd_addr, 6);
	send_data_t.server_chann = server_chann;
	send_data_t.p_data = data_send_buf;
	send_data_t.data_len = data_send_len;
	if (rtk_bt_rfc_data_send(&send_data_t)) {
		BT_LOGE("RFC send data fail\r\n");
		return -1;
	}

	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("RFC send data to device %s ,server channel 0x%x success!\r\n", addr_str, server_chann);

	return 0;
}

static int atcmd_bt_rfc_give_credits(int argc, char **argv)
{
	(void)argc;
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};
	uint8_t server_chann = 0;
	uint8_t credits = 0;
	char addr_str[30] = {0};

	if (hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN) == false) {
		BT_LOGE("RFC give credits op failed! wrong bd addr!\r\n");
		return -1;
	}

	server_chann = (uint8_t)(str_to_int(argv[1]));
	credits = (uint8_t)(str_to_int(argv[2]));

	rtk_bt_rfc_credits_give_t give_credits_t = {0};
	memcpy((void *)give_credits_t.bd_addr, bd_addr, 6);
	give_credits_t.local_server_chann = server_chann;
	give_credits_t.credits = credits;

	if (rtk_bt_rfc_credits_give(&give_credits_t)) {
		BT_LOGE("RFC credits give fail\r\n");
		return -1;
	}

	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("RFC give %d credits to device %s ,server channel 0x%x success!\r\n", credits, addr_str, server_chann);

	return 0;
}

static const cmd_table_t rfc_cmd_table[] = {
	{"conn",         atcmd_bt_rfc_connect,            2, 2},
	{"disconn",      atcmd_bt_rfc_disconnect,         2, 2},
	{"send_data",    atcmd_bt_rfc_send_data,          4, 4},
	{"give_credits", atcmd_bt_rfc_give_credits,       4, 4},
	{NULL,},
};

int atcmd_bt_rfc_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, rfc_cmd_table, "[AT+BTRFC]");
}