/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if defined(CONFIG_BT_CAP_SUPPORT) && CONFIG_BT_CAP_SUPPORT
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <rtk_bt_common.h>
#include <rtk_bt_def.h>
#include <rtk_bt_device.h>
#include <app_bt_le_audio_common.h>
#include <rtk_bt_le_audio.h>
#include <rtk_bt_bap.h>
#include <rtk_bt_cap.h>
#include <rtk_bt_gattc.h>

/* cap acceptor param */
extern app_bt_le_audio_acceptor_info_t g_acceptor_info;

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
/* mcp media control server */
uint16_t rtk_bt_mcp_media_send(uint16_t char_uuid, int32_t track_value)
{
	app_bt_le_audio_mcp_server_info_t *p_mcp_srv_info = &app_mcp_svr_info;
	rtk_bt_le_audio_mcp_server_send_data_param_t info = {0};
	uint16_t ret = RTK_BT_FAIL;

	/* choose uuid */
	switch (char_uuid) {
	case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME:
		info.param.media_player_name.p_media_player_name = p_mcp_srv_info->media_player_name.p_media_player_name;
		info.param.media_player_name.media_player_name_len = p_mcp_srv_info->media_player_name.media_player_name_len;
		BT_LOGA("send char media player name %s\r\n", info.param.media_player_name.p_media_player_name);
		break;
	case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE:
		info.param.track_title.p_track_title = p_mcp_srv_info->track_title.p_track_title;
		info.param.track_title.track_title_len = p_mcp_srv_info->track_title.track_title_len;
		BT_LOGA("send track title %s\r\n", info.param.track_title.p_track_title);
		break;
	case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION:
		// info.param.track_duration = p_mcp_srv_info->track_duration;
		info.param.track_duration = track_value;
		BT_LOGA("send track duration %d\r\n", (int)info.param.track_duration);
		break;
	case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION:
		// info.param.track_position = p_mcp_srv_info->track_position;
		info.param.track_position = track_value;
		BT_LOGA("send track position %d\r\n", (int)info.param.track_position);
		break;
	case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_CHANGED:
		BT_LOGA("send track changed\r\n");
		break;
	default:
		BT_LOGE("%s unsupport char_uuid 0x%x for MCS server\r\n", __func__, char_uuid);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	ret = rtk_bt_le_audio_mcs_server_send_data(p_mcp_srv_info->mcs_service_id, char_uuid, &info.param);
	if (RTK_BT_OK != ret) {
		BT_LOGE("[APP] rtk_bt_le_audio_mcs_server_send_data failed! err: 0x%x\r\n", ret);
	}
	return ret;
}
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
/* mcp media control client */
uint16_t rtk_bt_mcp_media_discover(uint16_t conn_handle)
{
	uint16_t ret = RTK_BT_FAIL;

	BT_LOGA("%s dicover all for conn_handle=%d\r\n", __func__, conn_handle);
	ret = rtk_bt_gattc_discover_all(conn_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("[APP] GATTC Discover failed! err: 0x%x\r\n", ret);
	} else {
		BT_LOGA("[APP] GATTC Discover Start, waiting ...\r\n");
	}
	return ret;
}

uint16_t rtk_bt_mcp_media_write(uint16_t conn_handle, uint8_t opcode, int32_t op_param)
{
	uint16_t ret = RTK_BT_FAIL;

	BT_LOGA("%s mcs_client_write_media_cp conn_handle=%d opcode=0x%x,op_param=%d\r\n", __func__, conn_handle, opcode, (int)op_param);
	ret = rtk_bt_le_audio_mcs_client_write_media_cp(conn_handle, 0, true, true, opcode, &op_param);
	return ret;
}

uint16_t rtk_bt_mcp_media_read(uint16_t conn_handle, uint16_t char_uuid)
{
	uint16_t ret = RTK_BT_FAIL;

	BT_LOGA("%s rtk_bt_le_audio_mcs_client_read_char_value conn_handle=%d opcode=0x%x\r\n", __func__, conn_handle, char_uuid);
	ret = rtk_bt_le_audio_mcs_client_read_char_value(conn_handle, 0, true, char_uuid);
	return ret;
}

uint16_t rtk_bt_mcp_media_cccd(uint16_t conn_handle, uint32_t cfg_cccd, bool enable)
{
	uint16_t ret = RTK_BT_FAIL;

	BT_LOGA("%s mcs_client_cfg_cccd for conn_handle=%d cfg_cccd=0x%x enable=%d\r\n", __func__, conn_handle, (unsigned int)cfg_cccd, enable);
	ret = rtk_bt_le_audio_mcs_client_cfg_cccd(conn_handle, 0, true, cfg_cccd, enable);
	return ret;
}
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
/* vcp volume renderer */
uint16_t rtk_bt_vcp_server_get(rtk_bt_le_audio_vcs_param_t *p_vcs_param)
{
	uint16_t ret = RTK_BT_FAIL;

	if (!p_vcs_param) {
		BT_LOGE("%s input p_vcs_param is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	ret = rtk_bt_le_audio_vcs_get_param(p_vcs_param);
	if (ret == RTK_BT_OK) {
		BT_LOGA("vcs param: volume_setting %d, mute %d, change_counter %d, volume_flags 0x%x,step_size %d\r\n",
				p_vcs_param->volume_setting, p_vcs_param->mute, p_vcs_param->change_counter, p_vcs_param->volume_flags, p_vcs_param->step_size);
	} else {
		BT_LOGE("%s rtk_bt_le_audio_vcs_get_param fail,ret=0x%x\r\n", __func__, ret);
	}
	return ret;
}
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
/* vcp client */
uint16_t rtk_bt_vcp_client_gmute(uint8_t group_idx, rtk_bt_le_audio_vcs_mute_state_t vcs_mute)
{
	uint16_t ret = RTK_BT_FAIL;
	rtk_bt_le_audio_group_handle_t group_handle = NULL;

	if (vcs_mute > 1) {
		BT_LOGE("%s: vcs_mute value (%d) error\r\n", __func__, vcs_mute);
		return RTK_BT_FAIL;
	}
	group_handle = app_bt_le_audio_group_list_find_by_group_idx(group_idx);
	if (!group_handle) {
		BT_LOGE("%s: group_idx (%d) not find\r\n", __func__, group_idx);
		return RTK_BT_FAIL;
	}
	ret = rtk_bt_le_audio_vcs_change_mute(group_handle, vcs_mute);

	return ret;
}

uint16_t rtk_bt_vcp_client_gvolume(uint8_t group_idx, uint8_t volume_setting)
{
	uint16_t ret = RTK_BT_FAIL;
	rtk_bt_le_audio_group_handle_t group_handle = NULL;

	group_handle = app_bt_le_audio_group_list_find_by_group_idx(group_idx);
	if (!group_handle) {
		BT_LOGE("%s: group_idx (%d) not find\r\n", __func__, group_idx);
		return RTK_BT_FAIL;
	}
	ret = rtk_bt_le_audio_vcs_change_volume(group_handle, volume_setting);

	return ret;
}

uint16_t rtk_bt_vcp_client_write(uint16_t conn_handle, rtk_bt_le_audio_vcs_cp_op_t cp_op, uint8_t volume_setting)
{
	uint16_t ret = RTK_BT_FAIL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	rtk_bt_le_audio_vcs_cp_param_t cp_param = {
		.volume_setting = RTK_BLE_AUDIO_DEFAULT_VCS_VOL_SETTING
	};

	if (cp_op >= RTK_BT_LE_AUDIO_VCS_CP_MAX) {
		BT_LOGE("%s: cp_op value (%d) error for write\r\n", __func__, cp_op);
		return RTK_BT_FAIL;
	}
	p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(conn_handle);
	if (!p_device_info) {
		BT_LOGE("%s: conn_handle (%d) not find\r\n", __func__, conn_handle);
		return RTK_BT_FAIL;
	}
	if (!p_device_info->vcs_is_found) {
		BT_LOGE("%s: conn_handle (%d) not support vcs\r\n", __func__, conn_handle);
		return RTK_BT_FAIL;
	}
	if (cp_op == RTK_BT_LE_AUDIO_VCS_CP_SET_ABSOLUTE_VOLUME) {
		if (volume_setting == p_device_info->vcs_volume_state.volume_setting) {
			BT_LOGA("%s: volume_setting is same with current for write cp_op %d\r\n", __func__, cp_op);
			return RTK_BT_OK;
		} else {
			cp_param.volume_setting = volume_setting;
		}
	}
	ret = rtk_bt_le_audio_vcs_write_cp(conn_handle, cp_op, &cp_param);

	return ret;
}

uint16_t rtk_bt_vcp_client_get(uint16_t conn_handle, rtk_bt_le_audio_vcs_volume_state_t *p_volume_state)
{
	uint16_t ret = RTK_BT_FAIL;

	if (!p_volume_state) {
		BT_LOGE("%s p_volume_state is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	ret = rtk_bt_le_audio_vcs_get_volume_state(conn_handle, p_volume_state);
	BT_LOGA("[APP] vcs get volume_setting=%d, mute=%d, change_counter=%d for conn_handle %d\r\n",
			p_volume_state->volume_setting,
			p_volume_state->mute,
			p_volume_state->change_counter,
			conn_handle);

	return ret;
}
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
/* micp server */
uint16_t rtk_bt_micp_server_get(rtk_bt_le_audio_mics_param_t *p_mics_param)
{
	uint16_t ret = RTK_BT_FAIL;

	if (!p_mics_param) {
		BT_LOGE("%s p_mics_param is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	ret = rtk_bt_le_audio_mics_get_param(p_mics_param);
	if (ret == RTK_BT_OK) {
		BT_LOGA("mics param: mic_mute %d\r\n", p_mics_param->mic_mute);
	} else {
		BT_LOGE("%srtk_bt_le_audio_mics_get_param fail,ret=0x%x\r\n", __func__, ret);
	}
	return ret;
}
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
/* micp client */
uint16_t rtk_bt_micp_client_gmute(uint8_t group_idx, rtk_bt_le_audio_mics_mute_state_t mic_mute)
{
	uint16_t ret = RTK_BT_FAIL;
	rtk_bt_le_audio_group_handle_t group_handle = NULL;

	if (mic_mute > 2) {
		BT_LOGE("%s: mic_mute value (%d) error\r\n", __func__, mic_mute);
		return RTK_BT_FAIL;
	}
	group_handle = app_bt_le_audio_group_list_find_by_group_idx(group_idx);
	if (!group_handle) {
		BT_LOGE("%s: group_idx (%d) not find\r\n", __func__, group_idx);
		return RTK_BT_FAIL;
	}
	ret = rtk_bt_le_audio_mics_change_mute(group_handle, mic_mute);

	return ret;
}

uint16_t rtk_bt_micp_client_set_mute(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t mic_mute)
{
	uint16_t ret = RTK_BT_FAIL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;

	if (mic_mute > 2) {
		BT_LOGE("%s: mic_mute value (%d) error\r\n", __func__, mic_mute);
		return RTK_BT_FAIL;
	}
	p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(conn_handle);
	if (!p_device_info) {
		BT_LOGE("%s: conn_handle (%d) not find\r\n", __func__, conn_handle);
		return RTK_BT_FAIL;
	}
	if (!p_device_info->mics_is_found) {
		BT_LOGE("%s: conn_handle (%d) not support mics\r\n", __func__, conn_handle);
		return RTK_BT_FAIL;
	}
	if (mic_mute == p_device_info->mic_mute) {
		BT_LOGA("%s: mic_mute is same with current for set mic_mute %d\r\n", __func__, mic_mute);
		return RTK_BT_OK;
	} else {
		BT_LOGA("%s: mics_set_mute for conn_handle %d mic_mute %d\r\n", __func__, conn_handle, mic_mute);
		ret = rtk_bt_le_audio_mics_set_mute_value(conn_handle, mic_mute);
	}

	return ret;
}

uint16_t rtk_bt_micp_client_get_mute(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t *p_mic_mute)
{
	uint16_t ret = RTK_BT_FAIL;

	if (!p_mic_mute) {
		BT_LOGE("%s p_mic_mute is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	ret = rtk_bt_le_audio_mics_get_mute_value(conn_handle, p_mic_mute);
	BT_LOGA("%s: get mic_mute %d\r\n", __func__, *p_mic_mute);

	return ret;
}
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
/* vocs server */
uint16_t rtk_bt_vocs_server_get(rtk_bt_le_audio_vocs_param_t *p_vocs_param)
{
	uint16_t ret = RTK_BT_FAIL;

	if (!p_vocs_param) {
		BT_LOGE("%s p_vocs_param is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (!p_vocs_param->output_des.p_output_des) {
		BT_LOGE("%s p_vocs_param->output_des.p_output_des is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	memset(p_vocs_param->output_des.p_output_des, 0, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
	for (uint8_t srv_instance_id = 0; srv_instance_id < RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM; srv_instance_id++) {
		ret = rtk_bt_le_audio_vocs_get_param(srv_instance_id, p_vocs_param);
		if (ret == RTK_BT_OK) {
			BT_LOGA("vocs param: srv_instance_id %d, volume_offset %d, change_counter %d, audio_location 0x%x,output_des_len %d, output_des %s\r\n",
					srv_instance_id, p_vocs_param->volume_offset, p_vocs_param->change_counter,
					(unsigned int)p_vocs_param->audio_location, p_vocs_param->output_des.output_des_len, p_vocs_param->output_des.p_output_des);
		}
	}

	return ret;
}

/* vocs client */
uint16_t rtk_bt_vocs_client_write(uint16_t conn_handle, rtk_bt_le_audio_vocs_cp_op_t cp_op, int16_t volume_offset)
{
	uint16_t ret = RTK_BT_FAIL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	uint8_t srv_instance_id = 0;
	rtk_bt_le_audio_vocs_cp_param_t cp_param = {
		.counter_used = false,
		.change_counter = 0,
		.volume_offset = 0,
	};

	p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(conn_handle);
	if (p_device_info == NULL) {
		BT_LOGE("%s: conn_handle (%d) not find\r\n", __func__, conn_handle);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (p_device_info->vocs_is_found == false) {
		BT_LOGE("%s: conn_handle (%d) not support vocs\r\n", __func__, conn_handle);
		return RTK_BT_ERR_UNSUPPORTED;
	}
	if (cp_op >= RTK_BT_LE_AUDIO_VOCS_CP_MAX) {
		BT_LOGE("%s: cp_op value (%d) error for write\r\n", __func__, cp_op);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (cp_op == RTK_BT_LE_AUDIO_VOCS_CP_SET_VOLUME_OFFSET) {
		cp_param.volume_offset = volume_offset;
	}
	for (srv_instance_id = 0; srv_instance_id < p_device_info->vocs_srv_num; srv_instance_id++) {
		ret = rtk_bt_le_audio_vocs_write_cp(conn_handle, srv_instance_id, cp_op, cp_param);
	}

	return ret;
}

uint16_t rtk_bt_vocs_client_gwrite(uint8_t group_idx, rtk_bt_le_audio_vocs_cp_op_t cp_op, int16_t volume_offset)
{
	uint16_t ret = RTK_BT_FAIL;
	rtk_bt_le_audio_group_handle_t group_handle = NULL;
	uint8_t srv_instance_id = 0;
	rtk_bt_le_audio_vocs_cp_param_t cp_param = {
		.counter_used = false,
		.change_counter = 0,
		.volume_offset = 0,
	};

	group_handle = app_bt_le_audio_group_list_find_by_group_idx(group_idx);
	if (group_handle == NULL) {
		BT_LOGE("%s: group_idx (%d) not find\r\n", __func__, group_idx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (cp_op >= RTK_BT_LE_AUDIO_VOCS_CP_MAX) {
		BT_LOGE("%s: cp_op value (%d) error for write\r\n", __func__, cp_op);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (cp_op == RTK_BT_LE_AUDIO_VOCS_CP_SET_VOLUME_OFFSET) {
		cp_param.volume_offset = volume_offset;
	}
	ret = rtk_bt_le_audio_vocs_write_cp_by_group(group_handle, srv_instance_id, cp_op, cp_param);

	return ret;
}

uint16_t rtk_bt_vocs_client_wdes(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t *des_str)
{
	uint16_t ret = RTK_BT_FAIL;
	uint8_t des_str_len = 0;

	if (!des_str) {
		BT_LOGE("%s des_str is NULL for wdes\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	des_str_len = strlen((char *)des_str) + 1;
	ret = rtk_bt_le_audio_vocs_write_output_des(conn_handle, srv_instance_id, des_str_len, des_str);
	BT_LOGA("%s: srv_instance_id %d vocs_write_output_des %s %s for conn_handle=%d \r\n", __func__, srv_instance_id, des_str, (RTK_BT_OK != ret) ? "fail" : "ok",
			conn_handle);

	return ret;
}

uint16_t rtk_bt_vocs_client_get_srv(uint16_t conn_handle, rtk_bt_le_audio_vocs_srv_data_t *p_srv_data)
{
	uint16_t ret = RTK_BT_FAIL;
	uint8_t srv_instance_id = 0;
	app_bt_le_audio_device_info_t *p_device_info = NULL;

	if (!p_srv_data) {
		BT_LOGE("%s p_srv_data is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (!p_srv_data->output_des.p_output_des) {
		BT_LOGE("%s p_srv_data->output_des.p_output_des is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(conn_handle);
	if (p_device_info == NULL) {
		BT_LOGE("%s: conn_handle (%d) not find\r\n", __func__, conn_handle);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (p_device_info->vocs_is_found == false) {
		BT_LOGE("%s: conn_handle (%d) not support vocs\r\n", __func__, conn_handle);
		return RTK_BT_ERR_UNSUPPORTED;
	}
	memset(p_srv_data->output_des.p_output_des, 0, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
	for (srv_instance_id = 0; srv_instance_id < p_device_info->vocs_srv_num; srv_instance_id++) {
		ret = rtk_bt_le_audio_vocs_get_srv_data(conn_handle, srv_instance_id, p_srv_data);
		if (ret == RTK_BT_OK) {
			BT_LOGA("vocs srv data: srv_instance_id %d, type_exist 0x%x,volume_offset %d, change_counter %d, audio_location 0x%x,output_des_len %d, output_des %s\r\n",
					p_srv_data->srv_instance_id, p_srv_data->type_exist, p_srv_data->volume_offset.volume_offset, p_srv_data->volume_offset.change_counter,
					(unsigned int)p_srv_data->audio_location, p_srv_data->output_des.output_des_len, p_srv_data->output_des.p_output_des);
		}
	}

	return ret;
}

uint16_t rtk_bt_vocs_client_get_char(uint16_t conn_handle)
{
	uint16_t ret = RTK_BT_FAIL;
	uint8_t srv_instance_id = 0;
	uint8_t vocs_char_type = 0;
	app_bt_le_audio_device_info_t *p_device_info = NULL;

	p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(conn_handle);
	if (p_device_info == NULL) {
		BT_LOGE("%s: conn_handle (%d) not find\r\n", __func__, conn_handle);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (p_device_info->vocs_is_found == false) {
		BT_LOGE("%s: conn_handle (%d) not support vocs\r\n", __func__, conn_handle);
		return RTK_BT_ERR_UNSUPPORTED;
	}
	for (srv_instance_id = 0; srv_instance_id < p_device_info->vocs_srv_num; srv_instance_id++) {
		for (vocs_char_type = RTK_BT_LE_AUDIO_VOCS_CHAR_OFFSET_STATE; vocs_char_type <= RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_OUTPUT_DESC; vocs_char_type++) {
			ret = rtk_bt_le_audio_vocs_read_char_value(conn_handle, srv_instance_id, vocs_char_type);
		}
	}

	return ret;
}
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
/* aics server */
uint16_t rtk_bt_aics_server_get(uint8_t srv_instance_id, uint8_t aics_param_type, uint8_t value_len, uint8_t *p_value)
{
	uint16_t ret = RTK_BT_FAIL;
	ret = rtk_bt_le_audio_aics_get_param(srv_instance_id, aics_param_type, value_len, p_value);
	if (ret == RTK_BT_OK) {
		switch (aics_param_type) {
		case RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATE: {
			rtk_bt_le_audio_aics_input_state_t *p_input_state = (rtk_bt_le_audio_aics_input_state_t *)p_value;
			BT_LOGA("aics param: srv_instance_id %d, gain_setting %d, mute %d, gain_mode %d,change_counter %d\r\n",
					srv_instance_id, p_input_state->gain_setting, p_input_state->mute, p_input_state->gain_mode, p_input_state->change_counter);
			break;
		}
		case RTK_BT_LE_AUDIO_AICS_PARAM_GAIN_SETTING_PROP: {
			rtk_bt_le_audio_aics_gain_setting_prop_t *p_setting_prop = (rtk_bt_le_audio_aics_gain_setting_prop_t *)p_value;
			BT_LOGA("aics param: srv_instance_id %d, gain_setting_units %d, gain_setting_min %d, gain_setting_max %d\r\n",
					srv_instance_id, p_setting_prop->gain_setting_units, p_setting_prop->gain_setting_min, p_setting_prop->gain_setting_max);
			break;
		}
		case RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_TYPE:
			BT_LOGA("aics param: srv_instance_id %d, input type %d\r\n", srv_instance_id, *(uint8_t *)p_value);
			break;
		case RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATUS:
			BT_LOGA("aics param: srv_instance_id %d, input status %d\r\n", srv_instance_id, *(uint8_t *)p_value);
			break;
		default:
			break;
		}
	} else {
		BT_LOGE("%s: aics get param fail,aics_param_type %d \r\n", __func__, aics_param_type);
	}

	return ret;
}

/* aics client */
uint16_t rtk_bt_aics_client_write(uint16_t conn_handle, rtk_bt_le_audio_aics_cp_op_t cp_op, int8_t gaining_setting)
{
	uint16_t ret = RTK_BT_FAIL;
	uint8_t srv_instance_id = 0;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	rtk_bt_le_audio_aics_cp_param_t cp_param = {
		.gaining_setting = 0,
	};

	p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(conn_handle);
	if (p_device_info == NULL) {
		BT_LOGE("%s: conn_handle (%d) not find\r\n", __func__, conn_handle);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (p_device_info->aics_is_found == false) {
		BT_LOGE("%s: conn_handle (%d) not support vocs\r\n", __func__, conn_handle);
		return RTK_BT_ERR_UNSUPPORTED;
	}
	if (cp_op >= RTK_BT_LE_AUDIO_AICS_CP_MAX) {
		BT_LOGE("%s: cp_op value (%d) error for write\r\n", __func__, cp_op);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (cp_op == RTK_BT_LE_AUDIO_AICS_CP_SET_GAIN_SETTING) {
		cp_param.gaining_setting = gaining_setting;
	}
	for (srv_instance_id = 0; srv_instance_id < p_device_info->aics_srv_num; srv_instance_id++) {
		ret = rtk_bt_le_audio_aics_write_cp(conn_handle, srv_instance_id, cp_op, cp_param);
	}

	return ret;
}

uint16_t rtk_bt_aics_client_gwrite(uint8_t group_idx, rtk_bt_le_audio_aics_cp_op_t cp_op, int8_t gaining_setting)
{
	uint16_t ret = RTK_BT_FAIL;
	uint8_t srv_instance_id = 0;
	rtk_bt_le_audio_group_handle_t group_handle = NULL;
	rtk_bt_le_audio_aics_cp_param_t cp_param = {
		.gaining_setting = 0,
	};

	group_handle = app_bt_le_audio_group_list_find_by_group_idx(group_idx);
	if (group_handle == NULL) {
		BT_LOGE("%s: group_idx (%d) not find\r\n", __func__, group_idx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (cp_op >= RTK_BT_LE_AUDIO_AICS_CP_MAX) {
		BT_LOGE("%s: cp_op value (%d) error for gwrite\r\n", __func__, cp_op);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (cp_op == RTK_BT_LE_AUDIO_AICS_CP_SET_GAIN_SETTING) {
		cp_param.gaining_setting = gaining_setting;
	}
	ret = rtk_bt_le_audio_aics_write_cp_by_group(group_handle, srv_instance_id, cp_op, cp_param);

	return ret;
}

uint16_t rtk_bt_aics_client_wdes(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t *des_str)
{
	uint16_t ret = RTK_BT_FAIL;
	uint8_t des_str_len = 0;

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
	des_str_len = strlen((char *)des_str) + 1;
	ret = rtk_bt_le_audio_aics_write_input_des(conn_handle, srv_instance_id, des_str_len, des_str);
	BT_LOGA("%s: srv_instance_id %d aics_write_input_des %s %s for conn_handle=%d \r\n", __func__, srv_instance_id, des_str, (RTK_BT_OK != ret) ? "fail" : "ok",
			conn_handle);

	return ret;
}

uint16_t rtk_bt_aics_client_get_srv(uint16_t conn_handle, rtk_bt_le_audio_aics_srv_data_t *p_srv_data)
{
	uint16_t ret = RTK_BT_FAIL;
	uint8_t srv_instance_id = 0;
	app_bt_le_audio_device_info_t *p_device_info = NULL;

	if (!p_srv_data) {
		BT_LOGE("%s p_srv_data is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (!p_srv_data->input_des.p_input_des) {
		BT_LOGE("%s p_srv_data->input_des.p_input_des is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(conn_handle);
	if (p_device_info == NULL) {
		BT_LOGE("%s: conn_handle (%d) not find\r\n", __func__, conn_handle);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (p_device_info->aics_is_found == false) {
		BT_LOGE("%s: conn_handle (%d) not support aics\r\n", __func__, conn_handle);
		return RTK_BT_ERR_UNSUPPORTED;
	}
	memset(p_srv_data->input_des.p_input_des, 0, RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH);
	for (srv_instance_id = 0; srv_instance_id < p_device_info->aics_srv_num; srv_instance_id++) {
		ret = rtk_bt_le_audio_aics_get_srv_data(conn_handle, srv_instance_id, p_srv_data);
		if (ret == RTK_BT_OK) {
			BT_LOGA("aics srv data: srv_instance_id %d, type_exist 0x%x,gain_setting %d, mute %d, gain_mode %d,change_counter %d, gain_setting_units 0x%x,gain_setting_min %d,gain_setting_max %d,input_type %d,input_status %d,input_des_len %d, input_des %s\r\n",
					p_srv_data->srv_instance_id, p_srv_data->type_exist,
					p_srv_data->input_state.gain_setting, p_srv_data->input_state.mute, p_srv_data->input_state.gain_mode, p_srv_data->input_state.change_counter,
					p_srv_data->setting_prop.gain_setting_units, p_srv_data->setting_prop.gain_setting_min, p_srv_data->setting_prop.gain_setting_max,
					p_srv_data->input_type, p_srv_data->input_status,
					p_srv_data->input_des.input_des_len, p_srv_data->input_des.p_input_des);
		}
	}

	return ret;
}

uint16_t rtk_bt_aics_client_get_char(uint16_t conn_handle)
{
	uint16_t ret = RTK_BT_FAIL;
	uint8_t srv_instance_id = 0;
	uint8_t aics_char_type = 0;
	app_bt_le_audio_device_info_t *p_device_info = NULL;

	p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(conn_handle);
	if (p_device_info == NULL) {
		BT_LOGE("%s: conn_handle (%d) not find\r\n", __func__, conn_handle);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (p_device_info->aics_is_found == false) {
		BT_LOGE("%s: conn_handle (%d) not support aics\r\n", __func__, conn_handle);
		return RTK_BT_ERR_UNSUPPORTED;
	}
	for (srv_instance_id = 0; srv_instance_id < p_device_info->aics_srv_num; srv_instance_id++) {
		for (aics_char_type = RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_STATE; aics_char_type <= RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_DES; aics_char_type++) {
			ret = rtk_bt_le_audio_aics_read_char_value(conn_handle, srv_instance_id, aics_char_type);
		}
	}

	return ret;
}
#endif

uint16_t rtk_bt_cap_acceptor_cfg(uint8_t channel, uint8_t *p_neighbor_addr)
{
	char ch = 0;
	uint8_t device_name_len = 0;
	/* Check bt enable status (rtk_bt_bap_unicast_server_cfg should only be invoked before RTK_BT_IS_ENABLED()) */
	if (rtk_bt_is_enable()) {
		BT_LOGE("%s: should not be invoked after bt enable \r\n", __func__);
		return RTK_BT_FAIL;
	}

	if (channel < 1 || channel > 3) {
		BT_LOGE("%s: channel should be within 1 - 3 \r\n", __func__);
		return RTK_BT_FAIL;
	}
	device_name_len = strlen((char *)g_acceptor_info.device_name);
	if (device_name_len > RTK_BT_GAP_DEVICE_NAME_LEN) {
		BT_LOGE("%s: device_name_len (%d) > RTK_BT_GAP_DEVICE_NAME_LEN(%d)\r\n", __func__, device_name_len, RTK_BT_GAP_DEVICE_NAME_LEN);
		return RTK_BT_FAIL;
	}
	switch (channel) {
	case RTK_BT_LE_AUDIO_LEFT:
		ch = 'L';
		break;
	case RTK_BT_LE_AUDIO_RIGHT:
		ch = 'R';
		break;
	case RTK_BT_LE_AUDIO_STEREO:
		ch = 'S';
		break;
	default:
		BT_LOGE("%s: unknown channel allocation \r\n", __func__);
		break;
	}
	g_acceptor_info.device_name[device_name_len - 1] = ch;
	g_acceptor_info.sound_channel = channel;
	if (p_neighbor_addr) {
		memcpy((void *)&g_acceptor_info.csis_neighbor_addr, p_neighbor_addr, 6);
	}

	return RTK_BT_OK;
}
#endif