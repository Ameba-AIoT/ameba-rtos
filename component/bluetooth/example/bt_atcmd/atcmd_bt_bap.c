/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <atcmd_service.h>
#include <bt_utils.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_bap.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_le_gap.h>

static int atcmd_bt_bap_ext_scan(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	bool enable = false;
	const char *act[] = {"stop", "start"};

	if (str_to_int(argv[0]) != 0 && str_to_int(argv[0]) != 1) {
		BT_LOGE("BAP ext scan fail, illegal input param: %d \r\n", str_to_int(argv[0]));
		return -1;
	}

	enable = (bool)str_to_int(argv[0]);
	if (rtk_bt_le_audio_ext_scan_act(enable)) {
		BT_LOGE("BAP ext scan %s fail\r\n", act[str_to_int(argv[0])]);
		return -1;
	}
	BT_LOGA("BAP ext scan %s successfully\r\n", act[str_to_int(argv[0])]);

	return 0;
}

static int atcmd_bt_bap_client_start(int argc, char **argv)
{
	(void)argc;
	uint8_t group_idx;
	uint16_t sink_context;
	uint16_t source_context;

	group_idx = (uint8_t)str_to_int(argv[0]);
	sink_context = (uint16_t)str_to_int(argv[1]);
	source_context = (uint16_t)str_to_int(argv[2]);
	if (rtk_bt_bap_unicast_client_start(group_idx, sink_context, source_context)) {
		BT_LOGE("BAP client start fail\r\n");
		return -1;
	}
	BT_LOGA("BAP client start successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_client_stop(int argc, char **argv)
{
	(void)argc;
	uint8_t group_idx;
	uint32_t time_out;

	group_idx = (uint8_t)str_to_int(argv[0]);
	time_out = (uint32_t)str_to_int(argv[1]);
	if (rtk_bt_bap_unicast_client_stop(group_idx, time_out)) {
		BT_LOGE("BAP client stop fail\r\n");
		return -1;
	}
	BT_LOGA("BAP client stop successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_client_release(int argc, char **argv)
{
	(void)argc;
	uint8_t group_idx;

	group_idx = (uint8_t)str_to_int(argv[0]);
	if (rtk_bt_bap_unicast_client_release(group_idx)) {
		BT_LOGE("BAP client release fail\r\n");
		return -1;
	}
	BT_LOGA("BAP client release successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_broadcast_source_start(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	if (rtk_bt_le_audio_broadcast_source_start()) {
		BT_LOGE("BAP broadcast source start fail\r\n");
		return -1;
	}
	BT_LOGA("BAP start successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_broadcast_source_stop(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	if (rtk_bt_le_audio_broadcast_source_stop()) {
		BT_LOGE("BAP broadcast source stop fail\r\n");
		return -1;
	}
	BT_LOGA("BAP stop successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_broadcast_pa_sync_create(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	rtk_bt_le_addr_t adv_addr = {0};
	uint8_t adv_sid = 0;
	uint8_t broadcast_id[RTK_BT_LE_AUDIO_BROADCAST_ID_LEN] = {0};

	adv_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)adv_addr.addr_val, RTK_BD_ADDR_LEN);
	adv_sid = (uint8_t)str_to_int(argv[2]);
	hexdata_str_to_array((char *)argv[3], broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);

	if (rtk_bt_le_audio_broadcast_pa_sync_create(adv_addr, adv_sid, broadcast_id)) {
		BT_LOGE("BAP broadcast pa sync create fail\r\n");
		return -1;
	}

	return 0;
}

static int atcmd_bt_bap_broadcast_pa_sync_remove(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	rtk_bt_le_addr_t adv_addr = {0};
	adv_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)adv_addr.addr_val, RTK_BD_ADDR_LEN);

	if (rtk_bt_le_audio_broadcast_pa_sync_terminate(adv_addr)) {
		BT_LOGE("BAP broadcast pa sync terminate fail\r\n");
		return -1;
	}
	BT_LOGA("BAP broadcast pa sync terminate successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_broadcast_big_sync_create(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	rtk_bt_le_addr_t adv_addr = {0};
	adv_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)adv_addr.addr_val, RTK_BD_ADDR_LEN);

	if (rtk_bt_le_audio_broadcast_big_sync_create(adv_addr)) {
		BT_LOGE("BAP broadcast big sync create fail\r\n");
		return -1;
	}
	BT_LOGA("BAP broadcast big sync create successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_broadcast_big_sync_remove(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	rtk_bt_le_addr_t adv_addr = {0};
	adv_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)adv_addr.addr_val, RTK_BD_ADDR_LEN);
	if (rtk_bt_le_audio_broadcast_big_sync_terminate(adv_addr)) {
		BT_LOGE("BAP broadcast big sync terminate fail\r\n");
		return -1;
	}
	BT_LOGA("BAP broadcast big sync terminate successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_broadcast_assistant_remote_ext_scan(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	bool enable = false;
	uint8_t group_idx = 0;
	const char *act[] = {"stop", "start"};

	if (str_to_int(argv[0]) != 0 && str_to_int(argv[0]) != 1) {
		BT_LOGE("BAP broadcast assistant remote ext scan fail, illegal input param: %d \r\n", str_to_int(argv[0]));
		return -1;
	}

	enable = (bool)str_to_int(argv[0]);
	group_idx = (uint8_t)str_to_int(argv[1]);

	if (rtk_bt_le_audio_broadcast_assistant_remote_ext_scan_act(enable, group_idx)) {
		BT_LOGE("Broadcast assistant remote ext scan %s fail\r\n", act[str_to_int(argv[0])]);
		return -1;
	}
	BT_LOGA("Broadcast assistant remote ext scan %s successfully\r\n", act[str_to_int(argv[0])]);

	return 0;
}

static int atcmd_bt_bap_broadcast_assistant_reception_start(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	rtk_bt_le_addr_t adv_addr = {0};
	uint8_t group_idx = 0;

	adv_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)adv_addr.addr_val, RTK_BD_ADDR_LEN);
	group_idx = (uint8_t)str_to_int(argv[2]);

	if (rtk_bt_le_audio_broadcast_assistant_reception_start(adv_addr, group_idx)) {
		BT_LOGE("Broadcast assistant reception start fail\r\n");
		return -1;
	}
	BT_LOGA("Broadcast assistant reception start successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_broadcast_assistant_reception_stop(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	rtk_bt_le_addr_t adv_addr = {0};
	uint8_t group_idx = 0;

	adv_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)adv_addr.addr_val, RTK_BD_ADDR_LEN);
	group_idx = (uint8_t)str_to_int(argv[2]);

	if (rtk_bt_le_audio_broadcast_assistant_reception_stop(adv_addr, group_idx)) {
		BT_LOGE("Broadcast assistant reception stop fail\r\n");
		return -1;
	}
	BT_LOGA("Broadcast assistant reception stop successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_broadcast_assistant_reception_remove(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	rtk_bt_le_addr_t adv_addr = {0};
	uint8_t group_idx = 0;

	adv_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)adv_addr.addr_val, RTK_BD_ADDR_LEN);
	group_idx = (uint8_t)str_to_int(argv[2]);

	if (rtk_bt_le_audio_broadcast_assistant_reception_remove(adv_addr, group_idx)) {
		BT_LOGE("Broadcast assistant reception remove fail\r\n");
		return -1;
	}
	BT_LOGA("Broadcast assistant reception remove successfully\r\n");

	return 0;
}

static const cmd_table_t bap_cmd_table[] = {
	{"escan",                atcmd_bt_bap_ext_scan,                               2, 2},
	{"client_start",         atcmd_bt_bap_client_start,                           4, 4},
	{"client_stop",          atcmd_bt_bap_client_stop,                            3, 3},
	{"client_release",       atcmd_bt_bap_client_release,                         2, 2},
	{"broadcast_start",      atcmd_bt_bap_broadcast_source_start,                 1, 1},
	{"broadcast_stop",       atcmd_bt_bap_broadcast_source_stop,                  1, 1},
	{"pa_sync_create",       atcmd_bt_bap_broadcast_pa_sync_create,               5, 5},
	{"pa_sync_remove",       atcmd_bt_bap_broadcast_pa_sync_remove,               3, 3},
	{"big_sync_create",      atcmd_bt_bap_broadcast_big_sync_create,              3, 3},
	{"big_sync_remove",      atcmd_bt_bap_broadcast_big_sync_remove,              3, 3},
	{"remote_scan",          atcmd_bt_bap_broadcast_assistant_remote_ext_scan,    3, 3},
	{"reception_start",      atcmd_bt_bap_broadcast_assistant_reception_start,    4, 4},
	{"reception_stop",       atcmd_bt_bap_broadcast_assistant_reception_stop,     4, 4},
	{"reception_remove",     atcmd_bt_bap_broadcast_assistant_reception_remove,   4, 4},
	{NULL,},
};

int atcmd_bt_bap_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, bap_cmd_table, "[AT+BLEBAP]");
}
#endif