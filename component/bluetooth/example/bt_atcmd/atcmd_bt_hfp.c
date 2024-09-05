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
#include <rtk_bt_hfp.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_br_gap.h>

static int atcmd_bt_hfp_connect(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_hfp_connect(bd_addr)) {
		BT_LOGE("HFP connect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HFP connecting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_hfp_disconnect(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_hfp_disconnect(bd_addr)) {
		BT_LOGE("HFP disconnect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HFP disconnecting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_hfp_sco_connect(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_hfp_sco_connect(bd_addr)) {
		BT_LOGE("HFP sco connect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HFP sco connecting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_hfp_sco_disconnect(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_hfp_sco_disconnect(bd_addr)) {
		BT_LOGE("HFP sco disconnect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HFP sco disconnecting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_hfp_call_income(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (strlen(argv[1]) > 20) {
		BT_LOGE("HFP call income phone illegal\r\n");
		return -1;
	}
	if (rtk_bt_hfp_call_incoming(bd_addr, argv[1], strlen(argv[1]) + 1, (uint8_t)str_to_int(argv[2]))) {
		BT_LOGE("HFP call income fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HFP call income to device %s ... number is %s length is %d type is %s\r\n", addr_str, argv[1], strlen(argv[1]), argv[2]);

	return 0;
}

static int atcmd_bt_hfp_call_answer_req(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_hfp_call_answer(bd_addr)) {
		BT_LOGE("HFP call answer req fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HFP call answer req to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_hfp_call_terminate_req(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_hfp_call_terminate(bd_addr)) {
		BT_LOGE("HFP call terminate req fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HFP call terminate req to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_hfp_batt_level_report(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};
	uint8_t power_level = 0;

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	power_level = (uint8_t)str_to_int(argv[1]);
	if (rtk_bt_hfp_batt_level_report(bd_addr, power_level)) {
		BT_LOGE("HFP batt level report fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HFP batt level report to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_hfp_speaker_gain_report(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};
	uint8_t gain = 0;

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	gain = (uint8_t)str_to_int(argv[1]);

	if (rtk_bt_hfp_speaker_gain_report(bd_addr, gain)) {
		BT_LOGE("HFP speaker gain report fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HFP speaker gain report to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_hfp_microphone_gain_report(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};
	uint8_t gain = 0;

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	gain = (uint8_t)str_to_int(argv[1]);
	if (rtk_bt_hfp_microphone_gain_report(bd_addr, gain)) {
		BT_LOGE("HFP microphone gain report fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HFP microphone gain report to device %s ...\r\n", addr_str);

	return 0;
}

static const cmd_table_t hfp_cmd_table[] = {
	{"conn",                    atcmd_bt_hfp_connect,                  2, 2},
	{"disconn",                 atcmd_bt_hfp_disconnect,               2, 2},
	{"sco_conn",                atcmd_bt_hfp_sco_connect,              2, 2},
	{"sco_disconn",             atcmd_bt_hfp_sco_disconnect,           2, 2},
	{"call_income",             atcmd_bt_hfp_call_income,              4, 4},
	{"call_answer",             atcmd_bt_hfp_call_answer_req,          2, 2},
	{"call_terminate",          atcmd_bt_hfp_call_terminate_req,       2, 2},
	{"batt_level_report",       atcmd_bt_hfp_batt_level_report,        3, 3},
	{"speaker_gain_report",     atcmd_bt_hfp_speaker_gain_report,      3, 3},
	{"microphone_gain_report",  atcmd_bt_hfp_microphone_gain_report,   3, 3},
	{NULL,},
};

int atcmd_bt_hfp_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, hfp_cmd_table, "[AT+BTHFP]");
}