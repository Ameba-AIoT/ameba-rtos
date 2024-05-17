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
#include <osif.h>
#include <log_service.h>
#include <bt_utils.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_cap.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_le_audio.h>

_WEAK uint16_t rtk_bt_mcp_media_send(uint16_t char_uuid, int32_t track_value)
{
	(void)char_uuid;
	(void)track_value;
	AT_PRINTK("[ATBE] mcp not support media send");
	return -1;
}

_WEAK uint16_t rtk_bt_mcp_media_discover(uint16_t conn_handle)
{
	(void)conn_handle;
	AT_PRINTK("[ATBE] mcp not support media discover");
	return -1;
}

_WEAK uint16_t rtk_bt_mcp_media_write(uint16_t conn_handle, uint8_t opcode, int32_t op_param)
{
	(void)conn_handle;
	(void)opcode;
	(void)op_param;
	AT_PRINTK("[ATBE] mcp not support media write");
	return -1;
}

_WEAK uint16_t rtk_bt_mcp_media_read(uint16_t conn_handle, uint16_t char_uuid)
{
	(void)conn_handle;
	(void)char_uuid;
	AT_PRINTK("[ATBE] mcp not support media read");
	return -1;
}

_WEAK uint16_t rtk_bt_mcp_media_cccd(uint16_t conn_handle, uint32_t cfg_cccd, bool enable)
{
	(void)conn_handle;
	(void)cfg_cccd;
	(void)enable;
	AT_PRINTK("[ATBE] mcp not support media cccd");
	return -1;
}

_WEAK uint16_t rtk_bt_vcp_server_get(rtk_bt_le_audio_vcs_param_t *p_vcs_param)
{
	(void)p_vcs_param;
	AT_PRINTK("[ATBE] vcp server not support get");
	return -1;
}

_WEAK uint16_t rtk_bt_micp_server_get(rtk_bt_le_audio_mics_param_t *p_mics_param)
{
	(void)p_mics_param;
	AT_PRINTK("[ATBE] micp server not support get");
	return -1;
}

_WEAK uint16_t rtk_bt_vocs_server_get(rtk_bt_le_audio_vocs_param_t *p_vocs_param)
{
	(void)p_vocs_param;
	AT_PRINTK("[ATBE] vocs server not support get");
	return -1;
}

_WEAK uint16_t rtk_bt_aics_server_get(void)
{
	AT_PRINTK("[ATBE] aics server not support get");
	return -1;
}

_WEAK uint16_t rtk_bt_vcp_client_gmute(uint8_t group_idx, rtk_bt_le_audio_vcs_mute_state_t vcs_mute)
{
	(void)group_idx;
	(void)vcs_mute;
	AT_PRINTK("[ATBE] vcp client not support group mute");
	return -1;
}

_WEAK uint16_t rtk_bt_vcp_client_gvolume(uint8_t group_idx, uint8_t volume_setting)
{
	(void)group_idx;
	(void)volume_setting;
	AT_PRINTK("[ATBE] vcp client not support group volume");
	return -1;
}

_WEAK uint16_t rtk_bt_vcp_client_write(uint16_t conn_handle, rtk_bt_le_audio_vcs_cp_op_t cp_op, uint8_t volume_setting)
{
	(void)conn_handle;
	(void)cp_op;
	(void)volume_setting;
	AT_PRINTK("[ATBE] vcp client not support write");
	return -1;
}

_WEAK uint16_t rtk_bt_vcp_client_get(uint16_t conn_handle, rtk_bt_le_audio_vcs_volume_state_t *p_volume_state)
{
	(void)conn_handle;
	(void)p_volume_state;
	AT_PRINTK("[ATBE] vcp client not support get");
	return -1;
}

_WEAK uint16_t rtk_bt_micp_client_gmute(uint8_t group_idx, rtk_bt_le_audio_mics_mute_state_t mic_mute)
{
	(void)group_idx;
	(void)mic_mute;
	AT_PRINTK("[ATBE] micp client not support group mute");
	return -1;
}

_WEAK uint16_t rtk_bt_micp_client_set_mute(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t mic_mute)
{
	(void)conn_handle;
	(void)mic_mute;
	AT_PRINTK("[ATBE] micp client not support mute");
	return -1;
}

_WEAK uint16_t rtk_bt_micp_client_get_mute(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t *p_mic_mute)
{
	(void)conn_handle;
	(void)p_mic_mute;
	AT_PRINTK("[ATBE] micp client not support get mute");
	return -1;
}

_WEAK uint16_t rtk_bt_vocs_client_write(uint16_t conn_handle, rtk_bt_le_audio_vocs_cp_op_t cp_op, int16_t volume_offset)
{
	(void)conn_handle;
	(void)cp_op;
	(void)volume_offset;
	AT_PRINTK("[ATBE] vocs client not support write");
	return -1;
}

_WEAK uint16_t rtk_bt_vocs_client_gwrite(uint8_t group_idx, rtk_bt_le_audio_vocs_cp_op_t cp_op, int16_t volume_offset)
{
	(void)group_idx;
	(void)cp_op;
	(void)volume_offset;
	AT_PRINTK("[ATBE] vocs client not support group write");
	return -1;
}

_WEAK uint16_t rtk_bt_vocs_client_wdes(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t *des_str)
{
	(void)conn_handle;
	(void)srv_instance_id;
	(void)des_str;
	AT_PRINTK("[ATBE] vocs client not support wdes");
	return -1;
}

_WEAK uint16_t rtk_bt_vocs_client_get_srv(uint16_t conn_handle, rtk_bt_le_audio_vocs_srv_data_t *p_srv_data)
{
	(void)conn_handle;
	(void)p_srv_data;
	AT_PRINTK("[ATBE] vocs client not support get srv");
	return -1;
}

_WEAK uint16_t rtk_bt_vocs_client_get_char(uint16_t conn_handle)
{
	(void)conn_handle;
	AT_PRINTK("[ATBE] vocs client not support get char");
	return -1;
}

_WEAK uint16_t rtk_bt_aics_client_write(uint16_t conn_handle, rtk_bt_le_audio_aics_cp_op_t cp_op, int8_t gaining_setting)
{
	(void)conn_handle;
	(void)cp_op;
	(void)gaining_setting;
	AT_PRINTK("[ATBE] aics client not support write");
	return -1;
}

_WEAK uint16_t rtk_bt_aics_client_gwrite(uint8_t group_idx, rtk_bt_le_audio_aics_cp_op_t cp_op, int8_t gaining_setting)
{
	(void)group_idx;
	(void)cp_op;
	(void)gaining_setting;
	AT_PRINTK("[ATBE] aics client not support group write");
	return -1;
}

_WEAK uint16_t rtk_bt_aics_client_wdes(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t *des_str)
{
	(void)conn_handle;
	(void)srv_instance_id;
	(void)des_str;
	AT_PRINTK("[ATBE] aics client not support wdes");
	return -1;
}

_WEAK uint16_t rtk_bt_aics_client_get_srv(uint16_t conn_handle, rtk_bt_le_audio_aics_srv_data_t *p_srv_data)
{
	(void)conn_handle;
	(void)p_srv_data;
	AT_PRINTK("[ATBE] aics client not support get srv");
	return -1;
}

_WEAK uint16_t rtk_bt_aics_client_get_char(uint16_t conn_handle)
{
	(void)conn_handle;
	AT_PRINTK("[ATBE] aics client not support get char");
	return -1;
}

_WEAK uint16_t rtk_bt_cap_acceptor_cfg(uint8_t channel, uint8_t *p_neighbor_addr)
{
	(void)channel;
	(void)p_neighbor_addr;
	AT_PRINTK("[ATBE] cap acceptor not support cfg");
	return -1;
}

/* initiator */

static int atcmd_bt_mcp_media_send(int argc, char **argv)
{
	uint16_t char_uuid = 0;
	int32_t track_value = 0;

	if (argc < 1 || argc > 2) {
		AT_PRINTK("[ATBC] mcp media send op failed! wrong args num!");
		return -1;
	}
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

static int atcmd_bt_mcp_server_act(int argc, char **argv)
{
	if (argc < 1) {
		AT_PRINTK("[ATBC] MCP server op failed! wrong args num!");
		return -1;
	}
	if (strcmp(argv[0], "send") == 0) {
		return atcmd_bt_mcp_media_send(argc - 1, &argv[1]);
	} else {
		AT_PRINTK("[%s]Error: mcp server do not support %s \r\n", __func__, argv[0]);
		return -1;
	}
}

/* acceptor */

static int atcmd_bt_mcp_media_discover(int argc, char **argv)
{
	uint16_t conn_handle;

	if (argc != 1) {
		AT_PRINTK("[ATBC] mcp media discover op failed! wrong args num!");
		return -1;
	}
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

	if (argc > 3 || argc < 2) {
		AT_PRINTK("[ATBC] mcp media write op failed! wrong args num!");
		return -1;
	}

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
	uint16_t conn_handle;
	uint16_t char_uuid;

	if (argc != 2) {
		AT_PRINTK("[ATBC] mcp media read op failed! wrong args num!");
		return -1;
	}
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
	uint16_t conn_handle;
	uint32_t cfg_cccd;
	bool enable;

	if (argc != 3) {
		AT_PRINTK("[ATBC] mcp media cccd op failed! wrong args num!");
		return -1;
	}
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

static int atcmd_bt_mcp_client_act(int argc, char **argv)
{
	if (argc < 1) {
		AT_PRINTK("[ATBC] MCP client op failed! wrong args num!");
		return -1;
	}
	if (strcmp(argv[0], "dis") == 0) {
		return atcmd_bt_mcp_media_discover(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "write") == 0) {
		return atcmd_bt_mcp_media_write(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "read") == 0) {
		return atcmd_bt_mcp_media_read(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "cccd") == 0) {
		return atcmd_bt_mcp_media_cccd(argc - 1, &argv[1]);
	} else {
		AT_PRINTK("[%s]Error: mcp client do not support %s \r\n", __func__, argv[0]);
		return -1;
	}
}

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

static int atcmd_bt_vcp_server_act(int argc, char **argv)
{
	if (argc < 1) {
		AT_PRINTK("[ATBC] VCP server op failed! wrong args num!");
		return -1;
	}
	if (strcmp(argv[0], "get") == 0) {
		return atcmd_bt_vcp_server_get(argc - 1, &argv[1]);
	} else {
		AT_PRINTK("[%s]Error: vcp server do not support %s \r\n", __func__, argv[0]);
		return -1;
	}
}

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

static int atcmd_bt_micp_server_act(int argc, char **argv)
{
	if (argc < 1) {
		AT_PRINTK("[ATBC] MICP server op failed! wrong args num!");
		return -1;
	}
	if (strcmp(argv[0], "get") == 0) {
		return atcmd_bt_micp_server_get(argc - 1, &argv[1]);
	} else {
		AT_PRINTK("[%s]Error: micp server do not support %s \r\n", __func__, argv[0]);
		return -1;
	}
}

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

static int atcmd_bt_vocs_server_act(int argc, char **argv)
{
	if (argc < 1) {
		AT_PRINTK("[ATBC] VOCS server op failed! wrong args num!");
		return -1;
	}
	if (strcmp(argv[0], "get") == 0) {
		return atcmd_bt_vocs_server_get(argc - 1, &argv[1]);
	} else {
		AT_PRINTK("[%s]Error: vocs server do not support %s \r\n", __func__, argv[0]);
		return -1;
	}
}

static int atcmd_bt_aics_server_get(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	if (rtk_bt_aics_server_get()) {
		AT_PRINTK("[ATBC] aics server get fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] aics server get successfully \r\n");

	return 0;
}

static int atcmd_bt_aics_server_act(int argc, char **argv)
{
	if (argc < 1) {
		AT_PRINTK("[ATBC] AICS server op failed! wrong args num!");
		return -1;
	}
	if (strcmp(argv[0], "get") == 0) {
		return atcmd_bt_aics_server_get(argc - 1, &argv[1]);
	} else {
		AT_PRINTK("[%s]Error: aics server do not support %s \r\n", __func__, argv[0]);
		return -1;
	}
}

static int atcmd_bt_cap_acceptor_cfg(int argc, char **argv)
{
	uint8_t channel = 0;
	uint8_t neighbor_addr[6] = {0};

	if ((argc != 1) && (argc != 2)) {
		AT_PRINTK("[ATBC] CAP acceptor cfg op failed! wrong args num!");
		return -1;
	}
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

static int atcmd_bt_vcp_client_gmute(int argc, char **argv)
{
	uint8_t group_idx;
	rtk_bt_le_audio_vcs_mute_state_t vcs_mute;

	if (argc != 2) {
		AT_PRINTK("[ATBC] vcp client gmute op failed! wrong args num!");
		return -1;
	}
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
	uint8_t group_idx;
	uint8_t volume_setting;

	if (argc != 2) {
		AT_PRINTK("[ATBC] vcp client gvolume op failed! wrong args num!");
		return -1;
	}
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

	if (argc < 2 || argc > 3) {
		AT_PRINTK("[ATBC] vcp client write op failed! wrong args num!");
		return -1;
	}
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
	uint16_t conn_handle;
	rtk_bt_le_audio_vcs_volume_state_t volume_state = {0};

	if (argc != 1) {
		AT_PRINTK("[ATBC] vcp client get op failed! wrong args num!");
		return -1;
	}
	conn_handle = (uint16_t)str_to_int(argv[0]);
	if (rtk_bt_vcp_client_get(conn_handle, &volume_state)) {
		AT_PRINTK("[ATBC] vcp client get fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] vcp client get successfully \r\n");

	return 0;
}

static int atcmd_bt_vcp_client_act(int argc, char **argv)
{
	if (argc < 1) {
		AT_PRINTK("[ATBC] VCP client op failed! wrong args num!");
		return -1;
	}
	if (strcmp(argv[0], "gmute") == 0) {
		return atcmd_bt_vcp_client_gmute(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "gvolume") == 0) {
		return atcmd_bt_vcp_client_gvolume(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "write") == 0) {
		return atcmd_bt_vcp_client_write(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "get") == 0) {
		return atcmd_bt_vcp_client_get(argc - 1, &argv[1]);
	} else {
		AT_PRINTK("[%s]Error: vcp client do not support %s \r\n", __func__, argv[0]);
		return -1;
	}
}

static int atcmd_bt_micp_client_gmute(int argc, char **argv)
{
	uint8_t group_idx;
	rtk_bt_le_audio_mics_mute_state_t mic_mute;

	if (argc != 2) {
		AT_PRINTK("[ATBC] micp client gmute op failed! wrong args num!");
		return -1;
	}
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
	uint16_t conn_handle;
	rtk_bt_le_audio_mics_mute_state_t mic_mute;

	if (argc != 2) {
		AT_PRINTK("[ATBC] micp client set mute op failed! wrong args num!");
		return -1;
	}
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
	uint16_t conn_handle;
	rtk_bt_le_audio_mics_mute_state_t mic_mute;

	if (argc != 1) {
		AT_PRINTK("[ATBC] micp client get mute op failed! wrong args num!");
		return -1;
	}
	conn_handle = (uint8_t)str_to_int(argv[0]);
	if (rtk_bt_micp_client_get_mute(conn_handle, &mic_mute)) {
		AT_PRINTK("[ATBC] micp client get mute fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] micp client get mute successfully \r\n");

	return 0;
}

static int atcmd_bt_micp_client_act(int argc, char **argv)
{
	if (argc < 1) {
		AT_PRINTK("[ATBC] MICP client op failed! wrong args num!");
		return -1;
	}
	if (strcmp(argv[0], "gmute") == 0) {
		return atcmd_bt_micp_client_gmute(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "set_mute") == 0) {
		return atcmd_bt_micp_client_set_mute(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "get_mute") == 0) {
		return atcmd_bt_micp_client_get_mute(argc - 1, &argv[1]);
	} else {
		AT_PRINTK("[%s]Error: micp client do not support %s \r\n", __func__, argv[0]);
		return -1;
	}
}

static int atcmd_bt_vocs_client_write(int argc, char **argv)
{
	uint16_t conn_handle;
	rtk_bt_le_audio_vocs_cp_op_t cp_op;
	int16_t volume_offset;

	if (argc != 3) {
		AT_PRINTK("[ATBC] vocs client write op failed! wrong args num!");
		return -1;
	}
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
	uint8_t group_idx;
	rtk_bt_le_audio_vocs_cp_op_t cp_op;
	int16_t volume_offset;

	if (argc != 3) {
		AT_PRINTK("[ATBC] vocs client gwrite op failed! wrong args num!");
		return -1;
	}
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
	uint16_t conn_handle;
	uint8_t srv_instance_id;
	uint8_t *des_str;

	if (argc != 3) {
		AT_PRINTK("[ATBC] vocs client wdes op failed! wrong args num!");
		return -1;
	}
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
	uint16_t conn_handle;
	rtk_bt_le_audio_vocs_srv_data_t srv_data = {0};

	if (argc != 1) {
		AT_PRINTK("[ATBC] vocs client get srv op failed! wrong args num!");
		return -1;
	}
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
	uint16_t conn_handle;

	if (argc != 1) {
		AT_PRINTK("[ATBC] vocs client get char op failed! wrong args num!");
		return -1;
	}
	conn_handle = (uint16_t)str_to_int(argv[0]);
	if (rtk_bt_vocs_client_get_char(conn_handle)) {
		AT_PRINTK("[ATBC] vocs client get char fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] vocs client get char successfully \r\n");

	return 0;
}

static int atcmd_bt_vocs_client_act(int argc, char **argv)
{
	if (argc < 1) {
		AT_PRINTK("[ATBC] VOCS client op failed! wrong args num!");
		return -1;
	}
	if (strcmp(argv[0], "write") == 0) {
		return atcmd_bt_vocs_client_write(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "gwrite") == 0) {
		return atcmd_bt_vocs_client_gwrite(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "wdes") == 0) {
		return atcmd_bt_vocs_client_wdes(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "get_srv") == 0) {
		return atcmd_bt_vocs_client_get_srv(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "get_char") == 0) {
		return atcmd_bt_vocs_client_get_char(argc - 1, &argv[1]);
	} else {
		AT_PRINTK("[%s]Error: vocs client do not support %s \r\n", __func__, argv[0]);
		return -1;
	}
}

static int atcmd_bt_aics_client_write(int argc, char **argv)
{
	uint16_t conn_handle;
	rtk_bt_le_audio_aics_cp_op_t cp_op;
	int8_t gaining_setting;

	if (argc != 2 && argc != 3) {
		AT_PRINTK("[ATBC] aics client write op failed! wrong args num!");
		return -1;
	}
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

	if (argc != 2 && argc != 3) {
		AT_PRINTK("[ATBC] aics client gwrite op failed! wrong args num!");
		return -1;
	}
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
	uint16_t conn_handle;
	uint8_t srv_instance_id;
	uint8_t *des_str;

	if (argc != 3) {
		AT_PRINTK("[ATBC] aics client wdes op failed! wrong args num!");
		return -1;
	}
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
	uint16_t conn_handle;
	rtk_bt_le_audio_aics_srv_data_t srv_data = {0};

	if (argc != 1) {
		AT_PRINTK("[ATBC] aics client get srv op failed! wrong args num!");
		return -1;
	}
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
	uint16_t conn_handle;

	if (argc != 1) {
		AT_PRINTK("[ATBC] aics client get char op failed! wrong args num!");
		return -1;
	}
	conn_handle = (uint16_t)str_to_int(argv[0]);
	if (rtk_bt_aics_client_get_char(conn_handle)) {
		AT_PRINTK("[ATBC] aics client get char fail \r\n");
		return -1;
	}
	AT_PRINTK("[ATBC] aics client get char successfully \r\n");

	return 0;
}

static int atcmd_bt_aics_client_act(int argc, char **argv)
{
	if (argc < 1) {
		AT_PRINTK("[ATBC] AICS client op failed! wrong args num!");
		return -1;
	}
	if (strcmp(argv[0], "write") == 0) {
		return atcmd_bt_aics_client_write(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "gwrite") == 0) {
		return atcmd_bt_aics_client_gwrite(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "wdes") == 0) {
		return atcmd_bt_aics_client_wdes(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "get_srv") == 0) {
		return atcmd_bt_aics_client_get_srv(argc - 1, &argv[1]);
	} else if (strcmp(argv[0], "get_char") == 0) {
		return atcmd_bt_aics_client_get_char(argc - 1, &argv[1]);
	} else {
		AT_PRINTK("[%s]Error: aics client do not support %s \r\n", __func__, argv[0]);
		return -1;
	}
}

static const cmd_table_t cap_initiator_cmd_table[] = {
	{"mcp",         atcmd_bt_mcp_server_act,                1, 4},
	{NULL,},
};

static const cmd_table_t cap_acceptor_cmd_table[] = {
	{"mcp",         atcmd_bt_mcp_client_act,                1, 5},
	{"vcp",         atcmd_bt_vcp_server_act,                1, 2},
	{"micp",        atcmd_bt_micp_server_act,               1, 2},
	{"vocs",        atcmd_bt_vocs_server_act,               1, 2},
	{"aics",        atcmd_bt_aics_server_act,               1, 2},
	{"cfg",         atcmd_bt_cap_acceptor_cfg,              1, 3},
	{NULL,},
};

static const cmd_table_t cap_commander_cmd_table[] = {
	{"mcp",         atcmd_bt_mcp_client_act,                1, 4},
	{"vcp",         atcmd_bt_vcp_client_act,                1, 5},
	{"micp",        atcmd_bt_micp_client_act,               1, 4},
	{"vocs",        atcmd_bt_vocs_client_act,               1, 5},
	{"aics",        atcmd_bt_aics_client_act,               1, 5},
	{NULL,},
};

int atcmd_bt_cap_cmd(int argc, char *argv[])
{
	if (argc < 2) {
		AT_PRINTK("[%s]Error: cap atcmd should have at least 2 parameters broadcast/unicast and role !!!\r\n", __func__);
		return -1;
	}
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