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
#include <rtk_bt_cap.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_gattc.h>

/* initiator */
//rtk_bt_le_audio_mcp_server_send_data_param_t app_mcp_svr_info app_bt_le_audio_mcp_server_info_t
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
static int atcmd_bt_mcp_media_send(int argc, char **argv)
{
	uint16_t char_uuid = 0;
	int32_t track_value = 0;

	char_uuid = (uint16_t)str_to_int(argv[0]);
	if (argc == 2) {
		track_value = (int32_t)str_to_int(argv[1]);
	}

	if (rtk_bt_le_audio_mcs_server_send_data(char_uuid, &track_value)) {
		BT_LOGE("mcp media send fail\r\n");
		return -1;
	}
	BT_LOGA("mcp media send successfully\r\n");

	return 0;
}

static const cmd_table_t cap_mcp_server_cmd_table[] = {
	{"send",    atcmd_bt_mcp_media_send,       2, 3},
	{NULL,},
};

static int atcmd_bt_mcp_server_act(int argc, char **argv)
{
	return atcmd_bt_excute(argc, &argv[0], cap_mcp_server_cmd_table, "[AT+BLECAP][initiator][mcp]");
}
#endif

/* acceptor */

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
static int atcmd_bt_mcp_media_discover(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	if (rtk_bt_gattc_discover_all(conn_handle)) {
		BT_LOGE("mcp media discover fail\r\n");
		return -1;
	}
	BT_LOGA("mcp media discover successfully\r\n");

	return 0;
}

static int atcmd_bt_mcp_media_write(int argc, char **argv)
{
	uint16_t conn_handle;
	uint8_t opcode;
	int32_t op_param;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	opcode = (uint8_t)str_to_int(argv[1]);
	if (argc == 3) {
		op_param = (int32_t)str_to_int(argv[2]);
	}

	if (rtk_bt_le_audio_mcs_client_write_media_cp(conn_handle, 0, true, true, opcode, &op_param)) {
		BT_LOGE("mcp media write fail\r\n");
		return -1;
	}
	BT_LOGA("mcp media write successfully\r\n");

	return 0;
}

static int atcmd_bt_mcp_media_read(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	uint16_t char_uuid;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	char_uuid = (uint16_t)str_to_int(argv[1]);
	if (rtk_bt_le_audio_mcs_client_read_char_value(conn_handle, 0, true, char_uuid)) {
		BT_LOGE("mcp media read fail\r\n");
		return -1;
	}
	BT_LOGA("mcp media read successfully\r\n");

	return 0;
}

static int atcmd_bt_mcp_media_cccd(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	uint32_t cfg_cccd;
	bool enable;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	cfg_cccd = (uint32_t)str_to_int(argv[1]);
	enable = (bool)str_to_int(argv[2]);
	if (rtk_bt_le_audio_mcs_client_cfg_cccd(conn_handle, 0, true, cfg_cccd, enable)) {
		BT_LOGE("mcp media cccd fail\r\n");
		return -1;
	}
	BT_LOGA("mcp media cccd successfully\r\n");

	return 0;
}

static const cmd_table_t cap_mcp_client_cmd_table[] = {
	{"dis",    atcmd_bt_mcp_media_discover,   2, 2},
	{"write",  atcmd_bt_mcp_media_write,      3, 4},
	{"read",   atcmd_bt_mcp_media_read,       3, 3},
	{"cccd",   atcmd_bt_mcp_media_cccd,       4, 4},
	{NULL,},
};

static int atcmd_bt_mcp_client_act(int argc, char **argv)
{
	return atcmd_bt_excute(argc, &argv[0], cap_mcp_client_cmd_table, "[AT+BLECAP][acceptor/commander][mcp]");
}
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
static int atcmd_bt_vcp_server_get(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	rtk_bt_le_audio_vcs_param_t vcs_param = {0};

	if (rtk_bt_le_audio_vcs_get_param(&vcs_param)) {
		BT_LOGE("vcp server get fail\r\n");
		return -1;
	}
	BT_LOGA("vcp server get successfully\r\n");
	BT_AT_PRINT("+BLECAP:acceptor,vcp,get,%u,%u,%u,%u,%u\r\n",
				vcs_param.volume_setting, vcs_param.mute, vcs_param.change_counter,
				vcs_param.volume_flags, vcs_param.step_size);

	return 0;
}

static const cmd_table_t cap_vcp_server_cmd_table[] = {
	{"get",    atcmd_bt_vcp_server_get,   1, 1},
	{NULL,},
};

static int atcmd_bt_vcp_server_act(int argc, char **argv)
{
	return atcmd_bt_excute(argc, &argv[0], cap_vcp_server_cmd_table, "[AT+BLECAP][acceptor][vcp]");
}
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
static int atcmd_bt_micp_server_get(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	rtk_bt_le_audio_mics_param_t mics_param = {0};

	if (rtk_bt_le_audio_mics_get_param(&mics_param)) {
		BT_LOGE("micp server get fail\r\n");
		return -1;
	}
	BT_LOGA("micp server get successfully\r\n");
	BT_AT_PRINT("+BLECAP:acceptor,micp,get,%u\r\n", mics_param.mic_mute);

	return 0;
}

static const cmd_table_t cap_micp_server_cmd_table[] = {
	{"get",    atcmd_bt_micp_server_get,   1, 1},
	{NULL,},
};

static int atcmd_bt_micp_server_act(int argc, char **argv)
{
	return atcmd_bt_excute(argc, &argv[0], cap_micp_server_cmd_table, "[AT+BLECAP][acceptor][micp]");
}
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
static int atcmd_bt_vocs_server_get(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	rtk_bt_le_audio_vocs_param_t vocs_param = {0};
	uint16_t ret = RTK_BT_FAIL;

	vocs_param.output_des.p_output_des = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
	if (!vocs_param.output_des.p_output_des) {
		BT_LOGE("%s: osif_mem_alloc len %d fail\r\n", __func__, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
		return -1;
	}
	for (uint8_t srv_instance_id = 0; srv_instance_id < RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM; srv_instance_id++) {
		ret = rtk_bt_le_audio_vocs_get_param(srv_instance_id, &vocs_param);
		if (ret == RTK_BT_OK) {
			BT_LOGA("vocs param: srv_instance_id %d, volume_offset %d, change_counter %d, audio_location 0x%x,output_des_len %d, output_des %s\r\n",
					srv_instance_id, vocs_param.volume_offset, vocs_param.change_counter,
					(unsigned int)vocs_param.audio_location, vocs_param.output_des.output_des_len, vocs_param.output_des.p_output_des);
		}
	}
	BT_LOGA("vocs server get successfully\r\n");
	BT_AT_PRINT("+BLECAP:acceptor,vocs,get,%d,%u,%u,%u,%s\r\n",
				vocs_param.volume_offset, vocs_param.change_counter, vocs_param.audio_location,
				vocs_param.output_des.output_des_len, vocs_param.output_des.p_output_des);
	osif_mem_free((void *)vocs_param.output_des.p_output_des);

	return 0;
}

static const cmd_table_t cap_vocs_server_cmd_table[] = {
	{"get",    atcmd_bt_vocs_server_get,   1, 1},
	{NULL,},
};

static int atcmd_bt_vocs_server_act(int argc, char **argv)
{
	return atcmd_bt_excute(argc, &argv[0], cap_vocs_server_cmd_table, "[AT+BLECAP][acceptor][vocs]");
}
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
static int atcmd_bt_aics_server_get(int argc, char **argv)
{
	uint8_t value_len = 0;
	uint8_t *p_value = NULL;

	if (argc != 2) {
		BT_LOGE("%s: wrong argc:%d\r\n", __func__, argc);
		return -1;
	}

	uint8_t srv_instance_id = (uint8_t)str_to_int(argv[0]);
	uint8_t aics_param_type = (uint8_t)str_to_int(argv[1]);

	switch (aics_param_type) {
	case RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATE:
		value_len = sizeof(rtk_bt_le_audio_aics_input_state_t);
		break;
	case RTK_BT_LE_AUDIO_AICS_PARAM_GAIN_SETTING_PROP:
		value_len = sizeof(rtk_bt_le_audio_aics_gain_setting_prop_t);
		break;
	case RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_TYPE:
		value_len = 1;
		break;
	case RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATUS:
		value_len = 1;
		break;
	default:
		BT_LOGE("%s: not support aics_param_type %d for get\r\n", __func__, aics_param_type);
		return -1;
	}
	p_value = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, value_len);
	if (p_value == NULL) {
		BT_LOGE("%s: osif_mem_alloc len %d fail\r\n", __func__, value_len);
		return RTK_BT_ERR_NO_RESOURCE;
	}
	if (rtk_bt_le_audio_aics_get_param(srv_instance_id, aics_param_type, value_len, p_value)) {
		BT_LOGE("aics server get fail\r\n");
		return -1;
	}
	switch (aics_param_type) {
	case RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATE: {
		rtk_bt_le_audio_aics_input_state_t *p_input_state = (rtk_bt_le_audio_aics_input_state_t *)p_value;
		BT_LOGA("aics param: srv_instance_id %d, gain_setting %d, mute %d, gain_mode %d,change_counter %d\r\n",
				srv_instance_id, p_input_state->gain_setting, p_input_state->mute, p_input_state->gain_mode, p_input_state->change_counter);
		BT_AT_PRINT("+BLECAP:acceptor,aics,get,%u,%d,%d,%d,%d,%d\r\n",
					aics_param_type, srv_instance_id, p_input_state->gain_setting,
					p_input_state->mute, p_input_state->gain_mode, p_input_state->change_counter);
		break;
	}
	case RTK_BT_LE_AUDIO_AICS_PARAM_GAIN_SETTING_PROP: {
		rtk_bt_le_audio_aics_gain_setting_prop_t *p_setting_prop = (rtk_bt_le_audio_aics_gain_setting_prop_t *)p_value;
		BT_LOGA("aics param: srv_instance_id %d, gain_setting_units %d, gain_setting_min %d, gain_setting_max %d\r\n",
				srv_instance_id, p_setting_prop->gain_setting_units, p_setting_prop->gain_setting_min, p_setting_prop->gain_setting_max);
		BT_AT_PRINT("+BLECAP:acceptor,aics,get,%u,%d,%d,%d,%d\r\n",
					aics_param_type, srv_instance_id, p_setting_prop->gain_setting_units,
					p_setting_prop->gain_setting_min, p_setting_prop->gain_setting_max);
		break;
	}
	case RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_TYPE:
		BT_LOGA("aics param: srv_instance_id %d, input type %d\r\n", srv_instance_id, *(uint8_t *)p_value);
		BT_AT_PRINT("+BLECAP:acceptor,aics,get,%u,%d,%d\r\n",
					aics_param_type, srv_instance_id, *(uint8_t *)p_value);
		break;
	case RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATUS:
		BT_LOGA("aics param: srv_instance_id %d, input status %d\r\n", srv_instance_id, *(uint8_t *)p_value);
		BT_AT_PRINT("+BLECAP:acceptor,aics,get,%u,%d,%d\r\n",
					aics_param_type, srv_instance_id, *(uint8_t *)p_value);
		break;
	default:
		break;
	}
	if (p_value) {
		osif_mem_free((void *)p_value);
	}
	BT_LOGA("aics server get successfully\r\n");

	return 0;
}

static const cmd_table_t cap_aics_server_cmd_table[] = {
	{"get",    atcmd_bt_aics_server_get,   3, 3},
	{NULL,},
};

static int atcmd_bt_aics_server_act(int argc, char **argv)
{
	return atcmd_bt_excute(argc, &argv[0], cap_aics_server_cmd_table, "[AT+BLECAP][acceptor][aics]");
}
#endif

/* commander */
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
static int atcmd_bt_vcp_client_gmute(int argc, char **argv)
{
	(void)argc;
	uint8_t group_idx;
	rtk_bt_le_audio_vcs_mute_state_t vcs_mute;

	group_idx = (uint8_t)str_to_int(argv[0]);
	vcs_mute = (rtk_bt_le_audio_vcs_mute_state_t)str_to_int(argv[1]);
	if (vcs_mute > 1) {
		BT_LOGE("%s: vcs_mute value (%d) error\r\n", __func__, vcs_mute);
		return RTK_BT_FAIL;
	}
	if (rtk_bt_le_audio_vcs_change_mute(group_idx, vcs_mute)) {
		BT_LOGE("vcp client gmute fail\r\n");
		return -1;
	}
	BT_LOGA("vcp client gmute successfully\r\n");

	return 0;
}

static int atcmd_bt_vcp_client_gvolume(int argc, char **argv)
{
	(void)argc;
	uint8_t group_idx;
	uint8_t volume_setting;

	group_idx = (uint8_t)str_to_int(argv[0]);
	volume_setting = (uint8_t)str_to_int(argv[1]);
	if (rtk_bt_le_audio_vcs_change_volume(group_idx, volume_setting)) {
		BT_LOGE("vcp client gvolume fail\r\n");
		return -1;
	}
	BT_LOGA("vcp client gvolume successfully\r\n");

	return 0;
}

static int atcmd_bt_vcp_client_write(int argc, char **argv)
{
	rtk_bt_le_audio_vcs_cp_op_t cp_op;
	uint16_t conn_handle;
	rtk_bt_le_audio_vcs_cp_param_t cp_param = {
		.volume_setting = RTK_BT_LE_AUDIO_DEFAULT_VCS_VOL_SETTING
	};

	conn_handle = (uint16_t)str_to_int(argv[0]);
	cp_op = (rtk_bt_le_audio_vcs_cp_op_t)str_to_int(argv[1]);
	if (cp_op >= RTK_BT_LE_AUDIO_VCS_CP_MAX) {
		BT_LOGE("%s: cp_op value (%d) error for write\r\n", __func__, cp_op);
		return RTK_BT_FAIL;
	}
	if (argc == 3 && cp_op == RTK_BT_LE_AUDIO_VCS_CP_SET_ABSOLUTE_VOLUME) {
		cp_param.volume_setting = (uint8_t)str_to_int(argv[2]);
	}
	if (rtk_bt_le_audio_vcs_write_cp(conn_handle, cp_op, &cp_param)) {
		BT_LOGE("vcp client write fail\r\n");
		return -1;
	}
	BT_LOGA("vcp client write successfully\r\n");

	return 0;
}

static int atcmd_bt_vcp_client_get(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	rtk_bt_le_audio_vcs_volume_state_t volume_state = {0};

	conn_handle = (uint16_t)str_to_int(argv[0]);
	if (rtk_bt_le_audio_vcs_get_volume_state(conn_handle, &volume_state)) {
		BT_LOGE("vcp client get fail\r\n");
		return -1;
	}
	BT_LOGA("vcp client get successfully\r\n");
	BT_LOGA("volume setting %x, mute %x, change_counter %x \r\n", volume_state.volume_setting, volume_state.mute, volume_state.change_counter);
	BT_AT_PRINT("+BLECAP:commander,vcp,get,%u,%u,%u,%u\r\n",
				conn_handle, volume_state.volume_setting,
				volume_state.mute, volume_state.change_counter);

	return 0;
}

static const cmd_table_t cap_vcp_client_cmd_table[] = {
	{"gmute",    atcmd_bt_vcp_client_gmute,    3, 3},
	{"gvolume",  atcmd_bt_vcp_client_gvolume,  3, 3},
	{"write",    atcmd_bt_vcp_client_write,    3, 4},
	{"get",      atcmd_bt_vcp_client_get,      2, 2},
	{NULL,},
};

static int atcmd_bt_vcp_client_act(int argc, char **argv)
{
	return atcmd_bt_excute(argc, &argv[0], cap_vcp_client_cmd_table, "[AT+BLECAP][commander][vcp]");
}
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
static int atcmd_bt_micp_client_gmute(int argc, char **argv)
{
	(void)argc;
	uint8_t group_idx;
	rtk_bt_le_audio_mics_mute_state_t mic_mute;

	group_idx = (uint8_t)str_to_int(argv[0]);
	mic_mute = (rtk_bt_le_audio_mics_mute_state_t)str_to_int(argv[1]);
	if (rtk_bt_le_audio_mics_change_mute(group_idx, mic_mute)) {
		BT_LOGE("micp client gmute fail\r\n");
		return -1;
	}
	BT_LOGA("micp client gmute successfully\r\n");

	return 0;
}

static int atcmd_bt_micp_client_set_mute(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	rtk_bt_le_audio_mics_mute_state_t mic_mute;

	conn_handle = (uint8_t)str_to_int(argv[0]);
	mic_mute = (rtk_bt_le_audio_mics_mute_state_t)str_to_int(argv[1]);
	if (mic_mute > 2) {
		BT_LOGE("%s: mic_mute value (%d) error\r\n", __func__, mic_mute);
		return RTK_BT_FAIL;
	}
	if (rtk_bt_le_audio_mics_set_mute_value(conn_handle, mic_mute)) {
		BT_LOGE("micp client set mute fail\r\n");
		return -1;
	}
	BT_LOGA("micp client set mute successfully\r\n");

	return 0;
}

static int atcmd_bt_micp_client_get_mute(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	rtk_bt_le_audio_mics_mute_state_t mic_mute;

	conn_handle = (uint8_t)str_to_int(argv[0]);
	if (rtk_bt_le_audio_mics_get_mute_value(conn_handle, &mic_mute)) {
		BT_LOGE("micp client get mute fail\r\n");
		return -1;
	}
	BT_LOGA("micp client get mute successfully\r\n");
	BT_AT_PRINT("+BLECAP:commander,micp,get_mute,%u,%d\r\n",
				conn_handle, mic_mute);

	return 0;
}

static const cmd_table_t cap_micp_client_cmd_table[] = {
	{"gmute",     atcmd_bt_micp_client_gmute,     3, 3},
	{"set_mute",  atcmd_bt_micp_client_set_mute,  3, 3},
	{"get_mute",  atcmd_bt_micp_client_get_mute,  2, 2},
	{NULL,},
};

static int atcmd_bt_micp_client_act(int argc, char **argv)
{
	return atcmd_bt_excute(argc, &argv[0], cap_micp_client_cmd_table, "[AT+BLECAP][commander][micp]");
}
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
static int atcmd_bt_vocs_client_write(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	rtk_bt_le_audio_vocs_cp_op_t cp_op;
	uint8_t srv_instance_id = 0;
	rtk_bt_le_audio_vocs_cp_param_t cp_param = {
		.counter_used = false,
		.change_counter = 0,
		.volume_offset = 0,
	};

	conn_handle = (uint16_t)str_to_int(argv[0]);
	srv_instance_id = (uint8_t)str_to_int(argv[1]);
	cp_op = (rtk_bt_le_audio_vocs_cp_op_t)str_to_int(argv[2]);
	cp_param.volume_offset = (int16_t)str_to_int(argv[3]);
	if (rtk_bt_le_audio_vocs_write_cp(conn_handle, srv_instance_id, cp_op, cp_param)) {
		BT_LOGE("vocs client write fail\r\n");
		return -1;
	}
	BT_LOGA("vocs client write successfully\r\n");

	return 0;
}

static int atcmd_bt_vocs_client_gwrite(int argc, char **argv)
{
	(void)argc;
	uint8_t group_idx;
	rtk_bt_le_audio_vocs_cp_op_t cp_op;
	uint8_t srv_instance_id = 0;
	rtk_bt_le_audio_vocs_cp_param_t cp_param = {
		.counter_used = false,
		.change_counter = 0,
		.volume_offset = 0,
	};

	group_idx = (uint8_t)str_to_int(argv[0]);
	srv_instance_id = (uint8_t)str_to_int(argv[1]);
	cp_op = (rtk_bt_le_audio_vocs_cp_op_t)str_to_int(argv[2]);
	if (cp_op >= RTK_BT_LE_AUDIO_VOCS_CP_MAX) {
		BT_LOGE("%s: cp_op value (%d) error for write\r\n", __func__, cp_op);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (cp_op == RTK_BT_LE_AUDIO_VOCS_CP_SET_VOLUME_OFFSET) {
		cp_param.volume_offset = (int16_t)str_to_int(argv[3]);
	}
	if (rtk_bt_le_audio_vocs_write_cp_by_group(group_idx, srv_instance_id, cp_op, cp_param)) {
		BT_LOGE("vocs client gwrite fail\r\n");
		return -1;
	}
	BT_LOGA("vocs client gwrite successfully\r\n");

	return 0;
}

static int atcmd_bt_vocs_client_wdes(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	uint8_t srv_instance_id;
	uint8_t *des_str;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	srv_instance_id = (uint8_t)str_to_int(argv[1]);
	des_str = (uint8_t *)argv[2];
	if (rtk_bt_le_audio_vocs_write_output_des(conn_handle, srv_instance_id, strlen((char *)des_str) + 1, des_str)) {
		BT_LOGE("vocs client wdes fail\r\n");
		return -1;
	}
	BT_LOGA("vocs client wdes successfully\r\n");

	return 0;
}

static int atcmd_bt_vocs_client_get_srv(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	uint8_t srv_instance_id;
	rtk_bt_le_audio_vocs_srv_data_t srv_data = {0};

	conn_handle = (uint16_t)str_to_int(argv[0]);
	srv_instance_id = (uint8_t)str_to_int(argv[1]);
	srv_data.output_des.p_output_des = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
	if (!srv_data.output_des.p_output_des) {
		BT_LOGE("%s: conn_handle (%d) osif_mem_alloc len %d fail\r\n", __func__, conn_handle, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
		return -1;
	}
	if (rtk_bt_le_audio_vocs_get_srv_data(conn_handle, srv_instance_id,  &srv_data)) {
		BT_LOGE("vocs client get srv fail\r\n");
		osif_mem_free((void *)srv_data.output_des.p_output_des);
		return -1;
	}
	BT_LOGA("vocs client get srv successfully\r\n");
	BT_LOGA("srv_instance_id 0x%x, type_exist 0x%x, volume_offset 0x%x, change_counter 0x%x, audio_location 0x%x, output_des_len 0x%x, p_output_des %s \r\n",
			srv_data.srv_instance_id, srv_data.type_exist, srv_data.volume_offset.volume_offset,
			srv_data.volume_offset.change_counter, srv_data.audio_location, srv_data.output_des.output_des_len,
			srv_data.output_des.p_output_des);
	BT_AT_PRINT("+BLECAP:commander,vocs,get_srv,%u,%u,%d,%u,%u,%u,%s\r\n",
				srv_data.srv_instance_id, srv_data.type_exist, srv_data.volume_offset.volume_offset,
				srv_data.volume_offset.change_counter, srv_data.audio_location, srv_data.output_des.output_des_len,
				srv_data.output_des.p_output_des);
	osif_mem_free((void *)srv_data.output_des.p_output_des);

	return 0;
}

static int atcmd_bt_vocs_client_get_char(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	uint8_t srv_instance_id;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	srv_instance_id = (uint8_t)str_to_int(argv[1]);
	for (uint8_t vocs_char_type = RTK_BT_LE_AUDIO_VOCS_CHAR_OFFSET_STATE; vocs_char_type <= RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_OUTPUT_DESC; vocs_char_type++) {
		if (rtk_bt_le_audio_vocs_read_char_value(conn_handle, srv_instance_id, vocs_char_type)) {
			BT_LOGE("vocs client get char fail\r\n");
			return -1;
		}
	}
	BT_LOGA("vocs client get char successfully\r\n");

	return 0;
}

static const cmd_table_t cap_vocs_client_cmd_table[] = {
	{"write",    atcmd_bt_vocs_client_write,    4, 5},
	{"gwrite",   atcmd_bt_vocs_client_gwrite,   5, 5},
	{"wdes",     atcmd_bt_vocs_client_wdes,     4, 4},
	{"get_srv",  atcmd_bt_vocs_client_get_srv,  3, 3},
	{"get_char", atcmd_bt_vocs_client_get_char, 3, 3},
	{NULL,},
};

static int atcmd_bt_vocs_client_act(int argc, char **argv)
{
	return atcmd_bt_excute(argc, &argv[0], cap_vocs_client_cmd_table, "[AT+BLECAP][commander][vocs]");
}
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
static int atcmd_bt_aics_client_write(int argc, char **argv)
{
	uint16_t conn_handle;
	uint8_t srv_instance_id;
	rtk_bt_le_audio_aics_cp_op_t cp_op;
	rtk_bt_le_audio_aics_cp_param_t cp_param = {
		.gaining_setting = 0,
	};

	conn_handle = (uint16_t)str_to_int(argv[0]);
	srv_instance_id = (uint8_t)str_to_int(argv[1]);
	cp_op = (rtk_bt_le_audio_aics_cp_op_t)str_to_int(argv[2]);
	if (argc == 4 && cp_op == RTK_BT_LE_AUDIO_AICS_CP_SET_GAIN_SETTING) {
		cp_param.gaining_setting = (int8_t)str_to_int(argv[3]);
	}
	if (rtk_bt_le_audio_aics_write_cp(conn_handle, srv_instance_id, cp_op, cp_param)) {
		BT_LOGE("aics client write fail\r\n");
		return -1;
	}
	BT_LOGA("aics client write successfully\r\n");

	return 0;
}

static int atcmd_bt_aics_client_gwrite(int argc, char **argv)
{
	uint8_t group_idx;
	uint8_t srv_instance_id;
	rtk_bt_le_audio_aics_cp_op_t cp_op;
	rtk_bt_le_audio_aics_cp_param_t cp_param = {
		.gaining_setting = 0,
	};

	group_idx = (uint8_t)str_to_int(argv[0]);
	srv_instance_id = (uint8_t)str_to_int(argv[1]);
	cp_op = (rtk_bt_le_audio_aics_cp_op_t)str_to_int(argv[2]);
	if (argc == 4 && cp_op == RTK_BT_LE_AUDIO_AICS_CP_SET_GAIN_SETTING) {
		cp_param.gaining_setting = (int8_t)str_to_int(argv[3]);
	}
	if (rtk_bt_le_audio_aics_write_cp_by_group(group_idx, srv_instance_id, cp_op, cp_param)) {
		BT_LOGE("aics client gwrite fail\r\n");
		return -1;
	}
	BT_LOGA("aics client gwrite successfully\r\n");

	return 0;
}

static int atcmd_bt_aics_client_wdes(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	uint8_t srv_instance_id;
	uint8_t *des_str;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	srv_instance_id = (uint8_t)str_to_int(argv[1]);
	des_str = (uint8_t *)argv[2];
	if (srv_instance_id >= RTK_BT_LE_AUDIO_DEFAULT_AICS_NUM) {
		BT_LOGE("%s srv_instance_id (%d) > max (%d) for wdes\r\n", __func__, srv_instance_id, RTK_BT_LE_AUDIO_DEFAULT_AICS_NUM);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (srv_instance_id == 0) {
		des_str = (uint8_t *)RTK_BT_LE_AUDIO_BLUETOOTH_DES;
	} else if (srv_instance_id == 1) {
		des_str = (uint8_t *)RTK_BT_LE_AUDIO_MIC_DES;
	} else {
		if (!des_str) {
			BT_LOGE("%s des_str is NULL for wdes\r\n", __func__);
			return RTK_BT_ERR_PARAM_INVALID;
		}
	}
	if (rtk_bt_le_audio_aics_write_input_des(conn_handle, srv_instance_id, strlen((char *)des_str) + 1, des_str)) {
		BT_LOGE("aics client wdes fail\r\n");
		return -1;
	}
	BT_LOGA("%s: srv_instance_id %d aics_write_input_des %s OK for conn_handle=%d \r\n", __func__, srv_instance_id, des_str, conn_handle);
	BT_LOGA("aics client wdes successfully\r\n");

	return 0;
}

static int atcmd_bt_aics_client_get_srv(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	uint8_t srv_instance_id;
	rtk_bt_le_audio_aics_srv_data_t srv_data = {0};

	conn_handle = (uint16_t)str_to_int(argv[0]);
	srv_instance_id = (uint8_t)str_to_int(argv[1]);
	srv_data.input_des.p_input_des = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
	if (srv_data.input_des.p_input_des == NULL) {
		BT_LOGE("%s: conn_handle (%d) osif_mem_alloc len %d fail\r\n", __func__, conn_handle, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
		return -1;
	}
	if (rtk_bt_le_audio_aics_get_srv_data(conn_handle, srv_instance_id, &srv_data)) {
		BT_LOGE("aics client get srv fail\r\n");
		osif_mem_free((void *)srv_data.input_des.p_input_des);
		return -1;
	}
	BT_LOGA("aics srv data: srv_instance_id %d, type_exist 0x%x,gain_setting %d, mute %d, gain_mode %d,change_counter %d, gain_setting_units 0x%x,gain_setting_min %d,gain_setting_max %d,input_type %d,input_status %d,input_des_len %d, input_des %s\r\n",
			srv_data.srv_instance_id, srv_data.type_exist,
			srv_data.input_state.gain_setting, srv_data.input_state.mute, srv_data.input_state.gain_mode, srv_data.input_state.change_counter,
			srv_data.setting_prop.gain_setting_units, srv_data.setting_prop.gain_setting_min, srv_data.setting_prop.gain_setting_max,
			srv_data.input_type, srv_data.input_status,
			srv_data.input_des.input_des_len, srv_data.input_des.p_input_des);
	BT_LOGA("aics client get srv successfully\r\n");
	BT_AT_PRINT("+BLECAP:commander,aics,get_srv,%u,%u,%d,%u,%u,%u,%u,%d,%d,%u,%u,%u,%s\r\n",
				srv_data.srv_instance_id, srv_data.type_exist, srv_data.input_state.gain_setting,
				srv_data.input_state.mute, srv_data.input_state.gain_mode, srv_data.input_state.change_counter,
				srv_data.setting_prop.gain_setting_units, srv_data.setting_prop.gain_setting_min, srv_data.setting_prop.gain_setting_max,
				srv_data.input_type, srv_data.input_status, srv_data.input_des.input_des_len,
				srv_data.input_des.p_input_des);
	osif_mem_free((void *)srv_data.input_des.p_input_des);

	return 0;
}

static int atcmd_bt_aics_client_get_char(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	uint8_t srv_instance_id;
	uint8_t aics_char_type;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	srv_instance_id = (uint8_t)str_to_int(argv[1]);
	for (aics_char_type = RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_STATE; aics_char_type <= RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_DES; aics_char_type++) {
		if (rtk_bt_le_audio_aics_read_char_value(conn_handle, srv_instance_id, aics_char_type)) {
			BT_LOGE("aics client get char fail\r\n");
			return -1;
		}
	}
	BT_LOGA("aics client get char successfully\r\n");

	return 0;
}

static const cmd_table_t cap_aics_client_cmd_table[] = {
	{"write",    atcmd_bt_aics_client_write,    4, 5},
	{"gwrite",   atcmd_bt_aics_client_gwrite,   4, 5},
	{"wdes",     atcmd_bt_aics_client_wdes,     4, 4},
	{"get_srv",  atcmd_bt_aics_client_get_srv,  3, 3},
	{"get_char", atcmd_bt_aics_client_get_char, 3, 3},
	{NULL,},
};

static int atcmd_bt_aics_client_act(int argc, char **argv)
{
	return atcmd_bt_excute(argc, &argv[0], cap_aics_client_cmd_table, "[AT+BLECAP][commander][aics]");
}
#endif

static const cmd_table_t cap_initiator_cmd_table[] = {
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
	{"mcp",         atcmd_bt_mcp_server_act,                2, 4},
#endif
	{NULL,},
};

static const cmd_table_t cap_acceptor_cmd_table[] = {
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
	{"mcp",         atcmd_bt_mcp_client_act,                2, 5},
#endif
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
	{"vcp",         atcmd_bt_vcp_server_act,                2, 2},
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
	{"micp",        atcmd_bt_micp_server_act,               2, 2},
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	{"vocs",        atcmd_bt_vocs_server_act,               2, 2},
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	{"aics",        atcmd_bt_aics_server_act,               4, 4},
#endif
	{NULL,},
};

static const cmd_table_t cap_commander_cmd_table[] = {
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
	{"mcp",         atcmd_bt_mcp_client_act,                2, 4},
#endif
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
	{"vcp",         atcmd_bt_vcp_client_act,                2, 5},
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
	{"micp",        atcmd_bt_micp_client_act,               2, 4},
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	{"vocs",        atcmd_bt_vocs_client_act,               2, 6},
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	{"aics",        atcmd_bt_aics_client_act,               2, 5},
#endif
	{NULL,},
};

int atcmd_bt_cap_cmd(int argc, char *argv[])
{
	int ret = 0;
	char tag[80] = "[AT+BLECAP]";
	if (strcmp(argv[0], "initiator") == 0) {
		strcat(tag, "[initiator]");
		ret = atcmd_bt_excute(argc - 1, &argv[1], cap_initiator_cmd_table, tag);
	} else if (strcmp(argv[0], "acceptor") == 0) {
		strcat(tag, "[acceptor]");
		ret = atcmd_bt_excute(argc - 1, &argv[1], cap_acceptor_cmd_table, tag);
	} else if (strcmp(argv[0], "commander") == 0) {
		strcat(tag, "[commander]");
		ret = atcmd_bt_excute(argc - 1, &argv[1], cap_commander_cmd_table, tag);
	} else {
		BT_LOGE("[%s]Error: cap do not support %s\r\n", __func__, argv[0]);
		ret = -1;
	}
	return ret;
}
#endif