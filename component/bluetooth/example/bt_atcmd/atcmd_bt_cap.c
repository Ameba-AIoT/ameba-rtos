/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if defined(CONFIG_BT_CAP_SUPPORT) && CONFIG_BT_CAP_SUPPORT
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
#include <rtk_bt_le_audio.h>

/* initiator */

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
static int atcmd_bt_mcp_media_send(int argc, char **argv)
{
	uint16_t char_uuid = 0;
	int32_t track_value = 0;

	char_uuid = (uint16_t)str_to_int(argv[0]);
	if (argc == 2) {
		track_value = (int32_t)str_to_int(argv[1]);
	}

	if (rtk_bt_mcp_media_send(char_uuid, track_value)) {
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
	if (rtk_bt_mcp_media_discover(conn_handle)) {
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

	if (rtk_bt_mcp_media_write(conn_handle, opcode, op_param)) {
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
	if (rtk_bt_mcp_media_read(conn_handle, char_uuid)) {
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
	if (rtk_bt_mcp_media_cccd(conn_handle, cfg_cccd, enable)) {
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

	if (rtk_bt_vcp_server_get(&vcs_param)) {
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

	if (rtk_bt_micp_server_get(&mics_param)) {
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

	vocs_param.output_des.p_output_des = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
	if (!vocs_param.output_des.p_output_des) {
		BT_LOGE("%s: osif_mem_alloc len %d fail\r\n", __func__, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
		return -1;
	}
	if (rtk_bt_vocs_server_get(&vocs_param)) {
		BT_LOGE("vocs server get fail\r\n");
		osif_mem_free((void *)vocs_param.output_des.p_output_des);
		return -1;
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
	(void)argc;
	(void)argv;
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

	if (rtk_bt_aics_server_get(srv_instance_id, aics_param_type, value_len, p_value)) {
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

static int atcmd_bt_cap_acceptor_cfg(int argc, char **argv)
{
	uint8_t channel = 0;
	uint8_t neighbor_addr[6] = {0};

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
	if (argc == 2) {
		hexdata_str_to_bd_addr(argv[1], neighbor_addr, 6);
		if (rtk_bt_cap_acceptor_cfg(channel, neighbor_addr)) {
			BT_LOGE("CAP acceptor config channel neighbor address fail\r\n");
			return -1;
		}
		BT_LOGA("CAP acceptor config channel neighbor address successfully\r\n");
	} else {
		if (rtk_bt_cap_acceptor_cfg(channel, NULL)) {
			BT_LOGE("CAP acceptor config channel fail\r\n");
			return -1;
		}
		BT_LOGA("CAP acceptor config channel successfully\r\n");
	}

	return 0;
}

/* commander */

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
static int atcmd_bt_vcp_client_gmute(int argc, char **argv)
{
	(void)argc;
	uint8_t group_idx;
	rtk_bt_le_audio_vcs_mute_state_t vcs_mute;

	group_idx = (uint8_t)str_to_int(argv[0]);
	vcs_mute = (rtk_bt_le_audio_vcs_mute_state_t)str_to_int(argv[1]);
	if (rtk_bt_vcp_client_gmute(group_idx, vcs_mute)) {
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
	if (rtk_bt_vcp_client_gvolume(group_idx, volume_setting)) {
		BT_LOGE("vcp client gvolume fail\r\n");
		return -1;
	}
	BT_LOGA("vcp client gvolume successfully\r\n");

	return 0;
}

static int atcmd_bt_vcp_client_write(int argc, char **argv)
{
	uint16_t conn_handle;
	rtk_bt_le_audio_vcs_cp_op_t cp_op;
	uint8_t volume_setting;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	cp_op = (rtk_bt_le_audio_vcs_cp_op_t)str_to_int(argv[1]);
	if (argc == 3 && cp_op == RTK_BT_LE_AUDIO_VCS_CP_SET_ABSOLUTE_VOLUME) {
		volume_setting = (uint8_t)str_to_int(argv[2]);
	}

	if (rtk_bt_vcp_client_write(conn_handle, cp_op, volume_setting)) {
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
	if (rtk_bt_vcp_client_get(conn_handle, &volume_state)) {
		BT_LOGE("vcp client get fail\r\n");
		return -1;
	}
	BT_LOGA("vcp client get successfully\r\n");
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
	if (rtk_bt_micp_client_gmute(group_idx, mic_mute)) {
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
	if (rtk_bt_micp_client_set_mute(conn_handle, mic_mute)) {
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
	if (rtk_bt_micp_client_get_mute(conn_handle, &mic_mute)) {
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
	int16_t volume_offset;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	cp_op = (rtk_bt_le_audio_vocs_cp_op_t)str_to_int(argv[1]);
	volume_offset = (int16_t)str_to_int(argv[2]);
	if (rtk_bt_vocs_client_write(conn_handle, cp_op, volume_offset)) {
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
	int16_t volume_offset;

	group_idx = (uint8_t)str_to_int(argv[0]);
	cp_op = (rtk_bt_le_audio_vocs_cp_op_t)str_to_int(argv[1]);
	volume_offset = (int16_t)str_to_int(argv[2]);
	if (rtk_bt_vocs_client_gwrite(group_idx, cp_op, volume_offset)) {
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
	if (rtk_bt_vocs_client_wdes(conn_handle, srv_instance_id, des_str)) {
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
	rtk_bt_le_audio_vocs_srv_data_t srv_data = {0};

	conn_handle = (uint16_t)str_to_int(argv[0]);
	srv_data.output_des.p_output_des = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
	if (!srv_data.output_des.p_output_des) {
		BT_LOGE("%s: conn_handle (%d) osif_mem_alloc len %d fail\r\n", __func__, conn_handle, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
		return -1;
	}
	if (rtk_bt_vocs_client_get_srv(conn_handle, &srv_data)) {
		BT_LOGE("vocs client get srv fail\r\n");
		osif_mem_free((void *)srv_data.output_des.p_output_des);
		return -1;
	}
	BT_LOGA("vocs client get srv successfully\r\n");
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

	conn_handle = (uint16_t)str_to_int(argv[0]);
	if (rtk_bt_vocs_client_get_char(conn_handle)) {
		BT_LOGE("vocs client get char fail\r\n");
		return -1;
	}
	BT_LOGA("vocs client get char successfully\r\n");

	return 0;
}

static const cmd_table_t cap_vocs_client_cmd_table[] = {
	{"write",    atcmd_bt_vocs_client_write,    4, 4},
	{"gwrite",   atcmd_bt_vocs_client_gwrite,   4, 4},
	{"wdes",     atcmd_bt_vocs_client_wdes,     4, 4},
	{"get_srv",  atcmd_bt_vocs_client_get_srv,  2, 2},
	{"get_char", atcmd_bt_vocs_client_get_char, 2, 2},
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
	rtk_bt_le_audio_aics_cp_op_t cp_op;
	int8_t gaining_setting;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	cp_op = (rtk_bt_le_audio_aics_cp_op_t)str_to_int(argv[1]);
	if (argc == 3 && cp_op == RTK_BT_LE_AUDIO_AICS_CP_SET_GAIN_SETTING) {
		gaining_setting = (int8_t)str_to_int(argv[2]);
	}
	if (rtk_bt_aics_client_write(conn_handle, cp_op, gaining_setting)) {
		BT_LOGE("aics client write fail\r\n");
		return -1;
	}
	BT_LOGA("aics client write successfully\r\n");

	return 0;
}

static int atcmd_bt_aics_client_gwrite(int argc, char **argv)
{
	uint8_t group_idx;
	rtk_bt_le_audio_aics_cp_op_t cp_op;
	int8_t gaining_setting;

	group_idx = (uint8_t)str_to_int(argv[0]);
	cp_op = (rtk_bt_le_audio_aics_cp_op_t)str_to_int(argv[1]);
	if (argc == 3 && cp_op == RTK_BT_LE_AUDIO_AICS_CP_SET_GAIN_SETTING) {
		gaining_setting = (int8_t)str_to_int(argv[2]);
	}
	if (rtk_bt_aics_client_gwrite(group_idx, cp_op, gaining_setting)) {
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
	if (rtk_bt_aics_client_wdes(conn_handle, srv_instance_id, des_str)) {
		BT_LOGE("aics client wdes fail\r\n");
		return -1;
	}
	BT_LOGA("aics client wdes successfully\r\n");

	return 0;
}

static int atcmd_bt_aics_client_get_srv(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	rtk_bt_le_audio_aics_srv_data_t srv_data = {0};

	conn_handle = (uint16_t)str_to_int(argv[0]);
	srv_data.input_des.p_input_des = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
	if (srv_data.input_des.p_input_des == NULL) {
		BT_LOGE("%s: conn_handle (%d) osif_mem_alloc len %d fail\r\n", __func__, conn_handle, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
		return -1;
	}
	if (rtk_bt_aics_client_get_srv(conn_handle, &srv_data)) {
		BT_LOGE("aics client get srv fail\r\n");
		osif_mem_free((void *)srv_data.input_des.p_input_des);
		return -1;
	}
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

	conn_handle = (uint16_t)str_to_int(argv[0]);
	if (rtk_bt_aics_client_get_char(conn_handle)) {
		BT_LOGE("aics client get char fail\r\n");
		return -1;
	}
	BT_LOGA("aics client get char successfully\r\n");

	return 0;
}

static const cmd_table_t cap_aics_client_cmd_table[] = {
	{"write",    atcmd_bt_aics_client_write,    3, 4},
	{"gwrite",   atcmd_bt_aics_client_gwrite,   3, 4},
	{"wdes",     atcmd_bt_aics_client_wdes,     4, 4},
	{"get_srv",  atcmd_bt_aics_client_get_srv,  2, 2},
	{"get_char", atcmd_bt_aics_client_get_char, 2, 2},
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
	{"cfg",         atcmd_bt_cap_acceptor_cfg,              2, 3},
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
	{"vocs",        atcmd_bt_vocs_client_act,               2, 5},
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