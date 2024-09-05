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
#include <rtk_bt_le_audio.h>

static int atcmd_bt_bap_broadcast_source_start(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	if (rtk_bt_bap_broadcast_source_start()) {
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

	if (rtk_bt_bap_broadcast_source_stop()) {
		BT_LOGE("BAP broadcast source stop fail\r\n");
		return -1;
	}
	BT_LOGA("BAP stop successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_broadcast_source_qos_cfg(int argc, char **argv)
{
	(void)argc;
	rtk_bt_le_audio_qos_cfg_type_t qos_type;

	qos_type = str_to_int(argv[0]);

	if (qos_type != 2 && qos_type != 3) {
		BT_LOGE("BIS qos type %d , should be 2~3\r\n", qos_type);
		return -1;
	}

	bap_broadcast_source_cfg.cfg_qos_type = qos_type;

	if (qos_type == RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY) {
		BT_LOGA("BAP broadcast source prefer QoS: BIS LOW LATENCY\r\n");
	} else if (qos_type == RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY) {
		BT_LOGA("BAP broadcast source prefer QoS: BIS HIG RELIABILITY\r\n");
	}

	return 0;
}

static int atcmd_bt_bap_ext_scan(int argc, char **argv)
{
	(void)argc;
	bool enable = false;

	enable = (bool)str_to_int(argv[0]);
	if (rtk_bt_bap_ext_scan(enable)) {
		BT_LOGE("BAP ext scan fail\r\n");
		return -1;
	}
	BT_LOGA("BAP ext scan successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_broadcast_sink_sync_start(int argc, char **argv)
{
	(void)argc;
	rtk_bt_bap_bass_scan_dev_info_t info = {0};

	info.adv_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)info.adv_addr.addr_val, RTK_BD_ADDR_LEN);
	if (rtk_bt_bap_broadcast_sync_start(&info)) {
		BT_LOGE("BAP broadcast sink sync fail\r\n");
		return -1;
	}
	BT_LOGA("BAP broadcast sink sync successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_broadcast_sink_sync_term(int argc, char **argv)
{
	(void)argc;
	rtk_bt_le_addr_t addr = {0};

	addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)addr.addr_val, RTK_BD_ADDR_LEN);
	if (rtk_bt_bap_broadcast_sync_term(&addr)) {
		BT_LOGE("BAP broadcast sink sync term fail\r\n");
		return -1;
	}
	BT_LOGA("BAP broadcast sink sync term successfully\r\n");

	return 0;
}

#if defined(CONFIG_BT_BAP_SUPPORT) && CONFIG_BT_BAP_SUPPORT
static int atcmd_bt_bap_broadcast_sink_cfg(int argc, char **argv)
{
	(void)argc;
	uint8_t channel = 0;

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
	if (rtk_bt_bap_broadcast_sink_cfg(channel)) {
		BT_LOGE("BAP broadcast sink config channel fail\r\n");
		return -1;
	}
	BT_LOGA("BAP broadcast sink config channel successfully\r\n");

	return 0;
}
#endif

static int atcmd_bt_bap_bass_scan(int argc, char **argv)
{
	(void)argc;
	bool enable = false;
	bool remote_scan_enable = false;
	uint8_t group_idx = 0;

	enable = (bool)str_to_int(argv[0]);
	remote_scan_enable = (bool)str_to_int(argv[1]);
	group_idx = (uint8_t)str_to_int(argv[2]);
	if (rtk_bt_bap_bass_scan(enable, remote_scan_enable, group_idx)) {
		BT_LOGE("BAP bass scan fail\r\n");
		return -1;
	}
	BT_LOGA("BAP bass scan successfully\r\n");

	return 0;
}

static int atcmd_bt_broadcast_assistant_sync_start(int argc, char **argv)
{
	(void)argc;
	rtk_bt_bap_bass_scan_dev_info_t info = {0};

	info.adv_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)info.adv_addr.addr_val, RTK_BD_ADDR_LEN);
	if (rtk_bt_bap_broadcast_assistant_sync_start(&info)) {
		BT_LOGE("BAP broadcast assistant sync fail\r\n");
		return -1;
	}
	BT_LOGA("BAP broadcast assistant sync successfully\r\n");

	return 0;
}

static int atcmd_bt_broadcast_assistant_sync_term(int argc, char **argv)
{
	(void)argc;
	rtk_bt_le_addr_t addr = {0};

	addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)addr.addr_val, RTK_BD_ADDR_LEN);
	if (rtk_bt_bap_broadcast_assistant_sync_term(&addr)) {
		BT_LOGE("BAP broadcast assistant sync term fail\r\n");
		return -1;
	}
	BT_LOGA("BAP broadcast assistant sync term successfully\r\n");

	return 0;
}

static int atcmd_bt_broadcast_assistant_reception_start(int argc, char **argv)
{
	(void)argc;
	rtk_bt_le_addr_t addr = {0};
	uint8_t group_idx = 0;

	addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)addr.addr_val, RTK_BD_ADDR_LEN);
	group_idx = (uint8_t)str_to_int(argv[2]);
	if (rtk_bt_bap_broadcast_assistant_reception_start(&addr, group_idx)) {
		BT_LOGE("BAP broadcast assistant reception start fail\r\n");
		return -1;
	}
	BT_LOGA("BAP broadcast assistant reception start successfully\r\n");

	return 0;
}

static int atcmd_bt_broadcast_assistant_reception_stop(int argc, char **argv)
{
	(void)argc;
	rtk_bt_le_addr_t addr = {0};
	uint8_t group_idx = 0;

	addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)addr.addr_val, RTK_BD_ADDR_LEN);
	group_idx = (uint8_t)str_to_int(argv[2]);
	if (rtk_bt_bap_broadcast_assistant_reception_stop(&addr, group_idx)) {
		BT_LOGE("BAP broadcast assistant reception stop fail\r\n");
		return -1;
	}
	BT_LOGA("BAP broadcast assistant reception stop successfully\r\n");

	return 0;
}

static int atcmd_bt_broadcast_assistant_reception_remove(int argc, char **argv)
{
	(void)argc;
	rtk_bt_le_addr_t addr = {0};
	uint8_t group_idx = 0;

	addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	hexdata_str_to_bd_addr(argv[1], (uint8_t *)addr.addr_val, RTK_BD_ADDR_LEN);
	group_idx = (uint8_t)str_to_int(argv[2]);
	if (rtk_bt_bap_broadcast_assistant_reception_remove(&addr, group_idx)) {
		BT_LOGE("BAP broadcast assistant reception remove fail\r\n");
		return -1;
	}
	BT_LOGA("BAP broadcast assistant reception remove successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_unicast_client_start(int argc, char **argv)
{
	(void)argc;
	uint8_t group_idx = 0;
	uint8_t play_mode = 0;

	group_idx = (uint8_t)str_to_int(argv[0]);
	play_mode = (uint8_t)str_to_int(argv[1]);
	if (rtk_bt_bap_unicast_client_start(group_idx, (rtk_bt_le_audio_play_mode_t)play_mode)) {
		BT_LOGE("BAP unicast client start fail\r\n");
		return -1;
	}
	BT_LOGA("BAP unicast client start successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_unicast_client_stop(int argc, char **argv)
{
	(void)argc;
	uint8_t group_idx = 0;

	group_idx = (uint8_t)str_to_int(argv[0]);
	if (rtk_bt_bap_unicast_client_stop(group_idx)) {
		BT_LOGE("BAP unicast client stop fail\r\n");
		return -1;
	}
	BT_LOGA("BAP unicast client stop successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_unicast_client_release(int argc, char **argv)
{
	(void)argc;
	uint8_t group_idx = 0;

	group_idx = (uint8_t)str_to_int(argv[0]);
	if (rtk_bt_bap_unicast_client_release(group_idx)) {
		BT_LOGE("BAP unicast client release fail\r\n");
		return -1;
	}
	BT_LOGA("BAP unicast client release successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_unicast_client_stream_session_release(int argc, char **argv)
{
	(void)argc;
	uint8_t group_idx = 0;

	group_idx = (uint8_t)str_to_int(argv[0]);
	if (rtk_bt_bap_unicast_client_stream_session_release(group_idx)) {
		BT_LOGE("BAP unicast client stream session release fail\r\n");
		return -1;
	}
	BT_LOGA("BAP unicast client release stream session successfully\r\n");

	return 0;
}

static int atcmd_bt_bap_unicast_client_prefer_qos_cfg(int argc, char **argv)
{
	(void)argc;
	rtk_bt_le_audio_qos_cfg_type_t prefer_qos_type;

	prefer_qos_type = (rtk_bt_le_audio_qos_cfg_type_t)str_to_int(argv[0]);

	if (prefer_qos_type != 0 && prefer_qos_type != 1) {
		BT_LOGE("wrong CIS Qos type: %d , should be 0~1\r\n", prefer_qos_type);
		return -1;
	}

	bap_unicast_client_cfg.cfg_qos_type = prefer_qos_type;

	if (prefer_qos_type == RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY) {
		BT_LOGA("BAP unicast client prefer session Qos: CIS LOW LATENCY\r\n");
	} else if (prefer_qos_type == RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY) {
		BT_LOGA("BAP unicast client prefer session QoS: CIS HIG RELIABILITY\r\n");
	}

	return 0;
}

static int atcmd_bt_bap_unicast_client_ase_target_qos_cfg(int argc, char **argv)
{
	(void)argc;
	rtk_bt_le_audio_ascs_ase_target_latency_t target_qos;

	target_qos = (rtk_bt_le_audio_ascs_ase_target_latency_t)str_to_int(argv[0]);

	if (target_qos < 1 || target_qos > 3) {
		BT_LOGE("wrong ase target QoS: %d , should be 1~3\r\n", target_qos);
		return -1;
	}

	app_lea_default_prefer_codec.target_latency = target_qos;
	if (target_qos == RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY) {
		BT_LOGA("BAP unicast client ase target QoS: 0x%x Low Latency\r\n", target_qos);
	} else if (target_qos == RTK_BLE_AUDIO_ASCS_ASE_TARGET_BALANCED) {
		BT_LOGA("BAP unicast client ase target QoS: 0x%x Balanced\r\n", target_qos);
	} else if (target_qos == RTK_BLE_AUDIO_ASCS_ASE_TARGET_HIGHER_RELIABILITY) {
		BT_LOGA("BAP unicast client ase target QoS: 0x%x High Reliability\r\n", target_qos);
	}

	return 0;
}

#if defined(CONFIG_BT_BAP_SUPPORT) && CONFIG_BT_BAP_SUPPORT
static int atcmd_bt_bap_unicast_server_cfg(int argc, char **argv)
{
	(void)argc;
	uint8_t channel = 0;

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
	if (rtk_bt_bap_unicast_server_cfg(channel)) {
		BT_LOGE("BAP unicast server config channel fail\r\n");
		return -1;
	}
	BT_LOGA("BAP unicast server config channel successfully\r\n");

	return 0;
}
#endif

static const cmd_table_t bap_broadcast_source_cmd_table[] = {
	{"start",       atcmd_bt_bap_broadcast_source_start,       1, 1},
	{"stop",        atcmd_bt_bap_broadcast_source_stop,        1, 1},
	{"qos_cfg",     atcmd_bt_bap_broadcast_source_qos_cfg,     2, 2},
	{NULL,},
};

static const cmd_table_t bap_broadcast_sink_cmd_table[] = {
	{"escan",       atcmd_bt_bap_ext_scan,                      2, 2},
	{"sync_start",  atcmd_bt_bap_broadcast_sink_sync_start,     3, 3},
	{"sync_term",   atcmd_bt_bap_broadcast_sink_sync_term,      3, 3},
#if defined(CONFIG_BT_BAP_SUPPORT) && CONFIG_BT_BAP_SUPPORT
	{"cfg",         atcmd_bt_bap_broadcast_sink_cfg,            2, 2},
#endif
	{NULL,},
};

static const cmd_table_t bap_broadcast_assistant_cmd_table[] = {
	{"bass_scan",         atcmd_bt_bap_bass_scan,                        4, 4},
	{"sync_start",        atcmd_bt_broadcast_assistant_sync_start,       3, 3},
	{"sync_term",         atcmd_bt_broadcast_assistant_sync_term,        3, 3},
	{"reception_start",   atcmd_bt_broadcast_assistant_reception_start,  4, 4},
	{"reception_stop",    atcmd_bt_broadcast_assistant_reception_stop,   4, 4},
	{"reception_remove",  atcmd_bt_broadcast_assistant_reception_remove, 4, 4},
	{NULL,},
};

static const cmd_table_t bap_scan_delegate_cmd_table[] = {
	{NULL,},
};

static const cmd_table_t bap_unicast_client_cmd_table[] = {
	{"start",             atcmd_bt_bap_unicast_client_start,                    3, 3},
	{"escan",             atcmd_bt_bap_ext_scan,                                2, 2},
	{"stop",              atcmd_bt_bap_unicast_client_stop,                     2, 2},
	{"release",           atcmd_bt_bap_unicast_client_release,                  2, 2},
	{"stream_release",    atcmd_bt_bap_unicast_client_stream_session_release,   2, 2},
	{"session_qos",       atcmd_bt_bap_unicast_client_prefer_qos_cfg,           2, 2},
	{"ase_qos",           atcmd_bt_bap_unicast_client_ase_target_qos_cfg,       2, 2},
	{NULL,},
};

static const cmd_table_t bap_unicast_server_cmd_table[] = {
#if defined(CONFIG_BT_BAP_SUPPORT) && CONFIG_BT_BAP_SUPPORT
	{"cfg",         atcmd_bt_bap_unicast_server_cfg,          2, 2},
#endif
	{NULL,},
};

int atcmd_bt_bap_cmd(int argc, char *argv[])
{
	int ret = 0;
	char tag[80] = "[AT+BLEBAP]";
	if (strcmp(argv[0], "broadcast") == 0) {
		if (strcmp(argv[1], "source") == 0) {
			BT_LOGA("Set bap broadcast source\r\n");
			strcat(tag, "[broadcast][source]");
			ret = atcmd_bt_excute(argc - 2, &argv[2], bap_broadcast_source_cmd_table, tag);
		} else if (strcmp(argv[1], "sink") == 0) {
			BT_LOGA("Set bap broadcast sink\r\n");
			strcat(tag, "[broadcast][sink]");
			ret = atcmd_bt_excute(argc - 2, &argv[2], bap_broadcast_sink_cmd_table, tag);
		} else if (strcmp(argv[1], "assistant") == 0) {
			BT_LOGA("Set bap broadcast assistant\r\n");
			strcat(tag, "[broadcast][assistant]");
			ret = atcmd_bt_excute(argc - 2, &argv[2], bap_broadcast_assistant_cmd_table, tag);
		} else if (strcmp(argv[1], "delegate") == 0) {
			BT_LOGA("Set bap scan delegate\r\n");
			strcat(tag, "[broadcast][delegate]");
			ret = atcmd_bt_excute(argc - 2, &argv[2], bap_scan_delegate_cmd_table, tag);
		} else {
			BT_LOGE("[%s]Error: bap broadcast has no role %s\r\n", __func__, argv[1]);
			ret = -1;
		}
	} else if (strcmp(argv[0], "unicast") == 0) {
		if (strcmp(argv[1], "client") == 0) {
			BT_LOGA("Set bap unicast client\r\n");
			strcat(tag, "[unicast][client]");
			ret = atcmd_bt_excute(argc - 2, &argv[2], bap_unicast_client_cmd_table, tag);
		} else if (strcmp(argv[1], "server") == 0) {
			BT_LOGA("Set bap unicast server\r\n");
			strcat(tag, "[unicast][server]");
			ret = atcmd_bt_excute(argc - 2, &argv[2], bap_unicast_server_cmd_table, tag);
		} else {
			BT_LOGE("[%s]Error: bap unicast has no role %s\r\n", __func__, argv[1]);
			ret = -1;
		}
	} else {
		BT_LOGE("[%s]Error: bap do not support %s\r\n", __func__, argv[0]);
		ret = -1;
	}
	return ret;
}
#endif