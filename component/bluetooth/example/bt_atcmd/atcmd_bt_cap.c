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
#include <osif.h>
#include <log_service.h>
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
		AT_PRINTK("[ATBC] mcp media send fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] mcp media send successfully \r\n");

	return 0;
}

static const cmd_table_t cap_mcp_server_cmd_table[] = {
	{"send",    atcmd_bt_mcp_media_send,       2, 3},
	{NULL,},
};

static int atcmd_bt_mcp_server_act(int argc, char **argv)
{
	atcmd_bt_excute(argc, &argv[0], cap_mcp_server_cmd_table, "[ATBC][cap_cmd][initiator][mcp]");
	return 0;
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
		AT_PRINTK("[ATBC] mcp media discover fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] mcp media discover successfully \r\n");

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
		AT_PRINTK("[ATBC] mcp media write fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] mcp media write successfully \r\n");

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
		AT_PRINTK("[ATBC] mcp media read fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] mcp media read successfully \r\n");

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
		AT_PRINTK("[ATBC] mcp media cccd fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] mcp media cccd successfully \r\n");

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
	atcmd_bt_excute(argc, &argv[0], cap_mcp_client_cmd_table, "[ATBC][cap_cmd][acceptor/commander][mcp]");
	return 0;
}
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
static int atcmd_bt_vcp_server_get(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	rtk_bt_le_audio_vcs_param_t vcs_param = {0};

	if (rtk_bt_vcp_server_get(&vcs_param)) {
		AT_PRINTK("[ATBC] vcp server get fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] vcp server get successfully \r\n");

	return 0;
}

static const cmd_table_t cap_vcp_server_cmd_table[] = {
	{"get",    atcmd_bt_vcp_server_get,   1, 1},
	{NULL,},
};

static int atcmd_bt_vcp_server_act(int argc, char **argv)
{
	atcmd_bt_excute(argc, &argv[0], cap_vcp_server_cmd_table, "[ATBC][cap_cmd][acceptor][vcp]");
	return 0;
}
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
static int atcmd_bt_micp_server_get(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	rtk_bt_le_audio_mics_param_t mics_param = {0};

	if (rtk_bt_micp_server_get(&mics_param)) {
		AT_PRINTK("[ATBC] micp server get fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] micp server get successfully \r\n");

	return 0;
}

static const cmd_table_t cap_micp_server_cmd_table[] = {
	{"get",    atcmd_bt_micp_server_get,   1, 1},
	{NULL,},
};

static int atcmd_bt_micp_server_act(int argc, char **argv)
{
	atcmd_bt_excute(argc, &argv[0], cap_micp_server_cmd_table, "[ATBC][cap_cmd][acceptor][micp]");
	return 0;
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
		AT_PRINTK("%s: osif_mem_alloc len %d fail\r\n", __func__, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
		return -1;
	}
	if (rtk_bt_vocs_server_get(&vocs_param)) {
		AT_PRINTK("[ATBC] vocs server get fail \r\n");
		osif_mem_free((void *)vocs_param.output_des.p_output_des);
		return -1;
	}
	AT_PRINTK("[ATBC] vocs server get successfully \r\n");
	osif_mem_free((void *)vocs_param.output_des.p_output_des);

	return 0;
}

static const cmd_table_t cap_vocs_server_cmd_table[] = {
	{"get",    atcmd_bt_vocs_server_get,   1, 1},
	{NULL,},
};

static int atcmd_bt_vocs_server_act(int argc, char **argv)
{
	atcmd_bt_excute(argc, &argv[0], cap_vocs_server_cmd_table, "[ATBC][cap_cmd][acceptor][vocs]");
	return 0;
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
		AT_PRINTK("%s: wrong argc:%d \r\n", __func__, argc);
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
		AT_PRINTK("%s: not support aics_param_type %d for get \r\n", __func__, aics_param_type);
		return -1;
	}

	p_value = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, value_len);
	if (p_value == NULL) {
		AT_PRINTK("%s: osif_mem_alloc len %d fail\r\n", __func__, value_len);
		return RTK_BT_ERR_NO_RESOURCE;
	}

	if (rtk_bt_aics_server_get(srv_instance_id, aics_param_type, value_len, p_value)) {
		AT_PRINTK("[ATBC] aics server get fail \r\n");
		return -1;
	}
	switch (aics_param_type) {
	case RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATE: {
		rtk_bt_le_audio_aics_input_state_t *p_input_state = (rtk_bt_le_audio_aics_input_state_t *)p_value;
		AT_PRINTK("aics param: srv_instance_id %d, gain_setting %d, mute %d, gain_mode %d,change_counter %d\r\n",
				  srv_instance_id, p_input_state->gain_setting, p_input_state->mute, p_input_state->gain_mode, p_input_state->change_counter);
		break;
	}
	case RTK_BT_LE_AUDIO_AICS_PARAM_GAIN_SETTING_PROP: {
		rtk_bt_le_audio_aics_gain_setting_prop_t *p_setting_prop = (rtk_bt_le_audio_aics_gain_setting_prop_t *)p_value;
		AT_PRINTK("aics param: srv_instance_id %d, gain_setting_units %d, gain_setting_min %d, gain_setting_max %d\r\n",
				  srv_instance_id, p_setting_prop->gain_setting_units, p_setting_prop->gain_setting_min, p_setting_prop->gain_setting_max);
		break;
	}
	case RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_TYPE:
		AT_PRINTK("aics param: srv_instance_id %d, input type %d\r\n", srv_instance_id, *(uint8_t *)p_value);
		break;
	case RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATUS:
		AT_PRINTK("aics param: srv_instance_id %d, input status %d\r\n", srv_instance_id, *(uint8_t *)p_value);
		break;
	default:
		break;
	}
	if (p_value) {
		osif_mem_free((void *)p_value);
	}

	AT_PRINTK("[ATBC] aics server get successfully \r\n");

	return 0;
}

static const cmd_table_t cap_aics_server_cmd_table[] = {
	{"get",    atcmd_bt_aics_server_get,   3, 3},
	{NULL,},
};

static int atcmd_bt_aics_server_act(int argc, char **argv)
{
	atcmd_bt_excute(argc, &argv[0], cap_aics_server_cmd_table, "[ATBC][cap_cmd][acceptor][aics]");
	return 0;
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
		AT_PRINTK("[ATBC] Unknown channel allocation");
		return -1;
	}
	if (argc == 2) {
		hexdata_str_to_bd_addr(argv[1], neighbor_addr, 6);
		if (rtk_bt_cap_acceptor_cfg(channel, neighbor_addr)) {
			AT_PRINTK("[ATBC] CAP acceptor config channel neighbor address fail \r\n");
			return -1;
		}
		AT_PRINTK("[ATBC] CAP acceptor config channel neighbor address successfully \r\n");
	} else {
		if (rtk_bt_cap_acceptor_cfg(channel, NULL)) {
			AT_PRINTK("[ATBC] CAP acceptor config channel fail \r\n");
			return -1;
		}
		AT_PRINTK("[ATBC] CAP acceptor config channel successfully \r\n");
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
		AT_PRINTK("[ATBC] vcp client gmute fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] vcp client gmute successfully \r\n");

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
		AT_PRINTK("[ATBC] vcp client gvolume fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] vcp client gvolume successfully \r\n");

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
		AT_PRINTK("[ATBC] vcp client write fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] vcp client write successfully \r\n");

	return 0;
}

static int atcmd_bt_vcp_client_get(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	rtk_bt_le_audio_vcs_volume_state_t volume_state = {0};

	conn_handle = (uint16_t)str_to_int(argv[0]);
	if (rtk_bt_vcp_client_get(conn_handle, &volume_state)) {
		AT_PRINTK("[ATBC] vcp client get fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] vcp client get successfully \r\n");

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
	atcmd_bt_excute(argc, &argv[0], cap_vcp_client_cmd_table, "[ATBC][cap_cmd][commander][vcp]");
	return 0;
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
		AT_PRINTK("[ATBC] micp client gmute fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] micp client gmute successfully \r\n");

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
		AT_PRINTK("[ATBC] micp client set mute fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] micp client set mute successfully \r\n");

	return 0;
}

static int atcmd_bt_micp_client_get_mute(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	rtk_bt_le_audio_mics_mute_state_t mic_mute;

	conn_handle = (uint8_t)str_to_int(argv[0]);
	if (rtk_bt_micp_client_get_mute(conn_handle, &mic_mute)) {
		AT_PRINTK("[ATBC] micp client get mute fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] micp client get mute successfully \r\n");

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
	atcmd_bt_excute(argc, &argv[0], cap_micp_client_cmd_table, "[ATBC][cap_cmd][commander][micp]");
	return 0;
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
		AT_PRINTK("[ATBC] vocs client write fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] vocs client write successfully \r\n");

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
		AT_PRINTK("[ATBC] vocs client gwrite fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] vocs client gwrite successfully \r\n");

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
		AT_PRINTK("[ATBC] vocs client wdes fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] vocs client wdes successfully \r\n");

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
		AT_PRINTK("%s: conn_handle (%d) osif_mem_alloc len %d fail\r\n", __func__, conn_handle, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
		return -1;
	}
	if (rtk_bt_vocs_client_get_srv(conn_handle, &srv_data)) {
		AT_PRINTK("[ATBC] vocs client get srv fail \r\n");
		osif_mem_free((void *)srv_data.output_des.p_output_des);
		return -1;
	}
	AT_PRINTK("[ATBC] vocs client get srv successfully \r\n");
	osif_mem_free((void *)srv_data.output_des.p_output_des);

	return 0;
}

static int atcmd_bt_vocs_client_get_char(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	if (rtk_bt_vocs_client_get_char(conn_handle)) {
		AT_PRINTK("[ATBC] vocs client get char fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] vocs client get char successfully \r\n");

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
	atcmd_bt_excute(argc, &argv[0], cap_vocs_client_cmd_table, "[ATBC][cap_cmd][commander][vocs]");
	return 0;
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
		AT_PRINTK("[ATBC] aics client write fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] aics client write successfully \r\n");

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
		AT_PRINTK("[ATBC] aics client gwrite fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] aics client gwrite successfully \r\n");

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
		AT_PRINTK("[ATBC] aics client wdes fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] aics client wdes successfully \r\n");

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
		AT_PRINTK("%s: conn_handle (%d) osif_mem_alloc len %d fail\r\n", __func__, conn_handle, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
		return -1;
	}
	if (rtk_bt_aics_client_get_srv(conn_handle, &srv_data)) {
		AT_PRINTK("[ATBC] aics client get srv fail \r\n");
		osif_mem_free((void *)srv_data.input_des.p_input_des);
		return -1;
	}
	AT_PRINTK("[ATBC] aics client get srv successfully \r\n");
	osif_mem_free((void *)srv_data.input_des.p_input_des);

	return 0;
}

static int atcmd_bt_aics_client_get_char(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	if (rtk_bt_aics_client_get_char(conn_handle)) {
		AT_PRINTK("[ATBC] aics client get char fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] aics client get char successfully \r\n");

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
	atcmd_bt_excute(argc, &argv[0], cap_aics_client_cmd_table, "[ATBC][cap_cmd][commander][aics]");
	return 0;
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
	if (strcmp(argv[0], "initiator") == 0) {
		atcmd_bt_excute(argc - 1, &argv[1], cap_initiator_cmd_table, "[ATBC][cap_cmd][initiator]");
	} else if (strcmp(argv[0], "acceptor") == 0) {
		atcmd_bt_excute(argc - 1, &argv[1], cap_acceptor_cmd_table, "[ATBC][cap_cmd][acceptor]");
	} else if (strcmp(argv[0], "commander") == 0) {
		atcmd_bt_excute(argc - 1, &argv[1], cap_commander_cmd_table, "[ATBC][cap_cmd][commander]");
	} else {
		AT_PRINTK("[%s]Error: cap do not support %s \r\n", __func__, argv[0]);
		return -1;
	}
	return 0;
}
#endif