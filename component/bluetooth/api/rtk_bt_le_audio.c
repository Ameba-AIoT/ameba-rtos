/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <bt_api_config.h>

#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#include <rtk_bt_le_audio.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>

uint16_t rtk_bt_le_audio_iso_data_send(rtk_bt_le_audio_iso_data_send_info_t *info)
{
	if (!info || !info->iso_conn_handle || !info->data_len || !info->p_data) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_ISO_DATA_SEND,
						   (void *)info, sizeof(rtk_bt_le_audio_iso_data_send_info_t));
}
uint16_t rtk_bt_le_audio_get_prefer_codec_cfg(uint8_t item, rtk_bt_le_audio_cfg_codec_t *p_cfg_codec)
{
	rtk_bt_le_audio_get_prefer_codec_cfg_param_t param = {0};

	param.item = item;
	param.p_cfg_codec = p_cfg_codec;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GET_PREFER_CODEC_CFG,
						   (void *)&param, sizeof(rtk_bt_le_audio_get_prefer_codec_cfg_param_t));
}

uint16_t rtk_bt_le_audio_qos_preferred_cfg_get(rtk_bt_le_audio_codec_cfg_item_t codec_cfg_item, rtk_bt_le_audio_qos_cfg_type_t qos_cfg_type,
											   rtk_bt_le_audio_qos_cfg_preferred_t *p_qos_cfg)
{
	rtk_bt_le_audio_get_prefer_qos_cfg_param_t param = {0};

	param.codec_cfg_item = codec_cfg_item;
	param.qos_cfg_type = qos_cfg_type;
	param.p_qos_cfg = p_qos_cfg;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GET_PREFER_QOS_CFG,
						   (void *)&param, sizeof(rtk_bt_le_audio_get_prefer_qos_cfg_param_t));
}

uint16_t rtk_bt_le_audio_pacs_get_info(uint16_t conn_handle, rtk_bt_le_audio_bap_pacs_info_t *p_pacs_info)
{
	rtk_bt_le_audio_pacs_get_info_param_t param = {0};

	param.conn_handle = conn_handle;
	param.p_pacs_info = p_pacs_info;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_PACS_GET_INFO,
						   (void *)&param, sizeof(rtk_bt_le_audio_pacs_get_info_param_t));
}

uint16_t rtk_bt_le_audio_pacs_get_pac_record(uint16_t conn_handle, rtk_bt_le_audio_role_t audio_role, uint8_t *p_pac_num,
											 rtk_bt_le_audio_bap_pacs_pac_record_t  *p_pac_tbl)
{
	rtk_bt_le_audio_pacs_get_pac_record_param_t param = {0};

	param.conn_handle = conn_handle;
	param.audio_role = audio_role;
	param.p_pac_num = p_pac_num;
	param.p_pac_tbl = p_pac_tbl;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_PACS_GET_PAC_RECORD,
						   (void *)&param, sizeof(rtk_bt_le_audio_pacs_get_pac_record_param_t));
}

uint16_t rtk_bt_le_audio_pacs_get_lc3_table_mask(uint16_t conn_handle, rtk_bt_le_audio_role_t audio_role, uint16_t pref_audio_contexts, uint8_t channel_count,
												 uint32_t *p_lc3_table_mask)
{
	rtk_bt_le_audio_pacs_get_lc3_table_mask_param_t param = {0};

	param.conn_handle = conn_handle;
	param.audio_role = audio_role;
	param.pref_audio_contexts = pref_audio_contexts;
	param.channel_count = channel_count;
	param.p_lc3_table_mask = p_lc3_table_mask;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_PACS_GET_LC3_TABLE_MASK,
						   (void *)&param, sizeof(rtk_bt_le_audio_pacs_get_lc3_table_mask_param_t));
}

uint16_t rtk_bt_le_audio_pacs_get_bis_array_by_sync_info(uint16_t conn_handle, rtk_bt_le_audio_sync_handle_t sync_handle, uint8_t filter, uint32_t *p_bis_array)
{
	rtk_bt_le_audio_pacs_get_bis_array_by_sync_info_param_t param = {0};

	param.conn_handle = conn_handle;
	param.sync_handle = sync_handle;
	param.filter = filter;
	param.p_bis_array = p_bis_array;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_PACS_GET_BIS_ARRAY_BY_SYNC_INFO,
						   (void *)&param, sizeof(rtk_bt_le_audio_pacs_get_bis_array_by_sync_info_param_t));
}

uint16_t rtk_bt_le_audio_sync_allocate(uint8_t adv_sid, rtk_bt_le_addr_t adv_addr, uint8_t *broadcast_id,
									   rtk_bt_le_audio_sync_handle_t *p_broadcast_sync_handle)
{
	rtk_bt_le_audio_sync_allocate_param_t param = {0};

	if (broadcast_id == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (p_broadcast_sync_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.adv_sid = adv_sid;
	memcpy(&param.adv_addr, &adv_addr, sizeof(rtk_bt_le_addr_t));
	memcpy(param.broadcast_id, broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);
	param.p_broadcast_sync_handle = p_broadcast_sync_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_SYNC_ALLOCATE,
						   (void *)&param, sizeof(rtk_bt_le_audio_sync_allocate_param_t));
}

uint16_t rtk_bt_le_audio_sync_release(rtk_bt_le_audio_sync_handle_t sync_handle)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_SYNC_RELEASE,
						   (void *)&sync_handle, sizeof(rtk_bt_le_audio_sync_handle_t));
}

uint16_t rtk_bt_le_audio_sync_find(uint8_t adv_sid, uint8_t *broadcast_id, rtk_bt_le_audio_sync_handle_t *p_broadcast_sync_handle)
{
	rtk_bt_le_audio_sync_find_param_t param = {0};

	if (broadcast_id == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (p_broadcast_sync_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.adv_sid = adv_sid;
	memcpy(param.broadcast_id, broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);
	param.p_broadcast_sync_handle = p_broadcast_sync_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_SYNC_FIND,
						   (void *)&param, sizeof(rtk_bt_le_audio_sync_find_param_t));
}

uint16_t rtk_bt_le_audio_pa_sync_establish(rtk_bt_le_audio_sync_handle_t sync_handle, rtk_bt_le_audio_pa_sync_establish_t *p_establish)
{
	rtk_bt_le_audio_pa_sync_establish_param_t param = {0};

	if (p_establish->skip > 0x01F3) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (p_establish->sync_timeout < 0x000A || p_establish->sync_timeout > 0x4000) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.sync_handle = sync_handle;
	memcpy(&param.establish_param, p_establish, sizeof(rtk_bt_le_audio_pa_sync_establish_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_PA_SYNC_ESTABLISH,
						   (void *)&param, sizeof(rtk_bt_le_audio_pa_sync_establish_param_t));
}

uint16_t rtk_bt_le_audio_pa_sync_terminate(rtk_bt_le_audio_sync_handle_t sync_handle)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_PA_SYNC_TERMINATE,
						   (void *)&sync_handle, sizeof(rtk_bt_le_audio_sync_handle_t));
}

uint16_t rtk_bt_le_audio_sync_get_sync_info(rtk_bt_le_audio_sync_handle_t sync_handle, rtk_bt_le_audio_sync_info_t *p_sync_info)
{
	rtk_bt_le_audio_sync_get_sync_info_param_t param = {0};

	if (sync_handle == NULL || p_sync_info == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.sync_handle = sync_handle;
	param.p_sync_info = p_sync_info;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_SYNC_GET_SYNC_INFO,
						   (void *)&param, sizeof(rtk_bt_le_audio_sync_get_sync_info_param_t));
}

uint16_t rtk_bt_le_audio_sync_get_bis_sync_codec_cfg(rtk_bt_le_audio_sync_handle_t sync_handle, uint8_t bis_idx, rtk_bt_le_audio_cfg_codec_t *p_codec_cfg)
{
	rtk_bt_le_audio_sync_get_bis_codec_cfg_param_t param = {0};

	if (p_codec_cfg == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.sync_handle = sync_handle;
	param.bis_idx = bis_idx;
	param.p_codec_cfg = p_codec_cfg;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_SYNC_GET_BIS_CODEC_CFG,
						   (void *)&param, sizeof(rtk_bt_le_audio_sync_get_bis_codec_cfg_param_t));
}

uint16_t rtk_bt_le_audio_big_sync_establish(rtk_bt_le_audio_sync_handle_t sync_handle, rtk_bt_le_audio_big_sync_establish_t *p_establish)
{
	rtk_bt_le_audio_big_sync_establish_param_t param = {0};

	if (p_establish == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.sync_handle = sync_handle;
	memcpy(&param.establish_param, p_establish, sizeof(rtk_bt_le_audio_big_sync_establish_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BIG_SYNC_ESTABLISH,
						   (void *)&param, sizeof(rtk_bt_le_audio_big_sync_establish_param_t));
}

uint16_t rtk_bt_le_audio_big_sync_terminate(rtk_bt_le_audio_sync_handle_t sync_handle)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BIG_SYNC_TERMINATE,
						   (void *)&sync_handle, sizeof(rtk_bt_le_audio_sync_handle_t));
}

uint16_t rtk_bt_le_audio_sync_get_bis_info(rtk_bt_le_audio_sync_handle_t sync_handle, rtk_bt_le_audio_bis_info_t *p_bis_info)
{
	rtk_bt_le_audio_sync_get_bis_info_param_t param = {0};

	if (p_bis_info == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.sync_handle = sync_handle;
	param.p_bis_info = p_bis_info;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_SYNC_GET_BIS_INFO,
						   (void *)&param, sizeof(rtk_bt_le_audio_sync_get_bis_info_param_t));
}
uint16_t rtk_bt_le_audio_sync_get_support_bis_array(rtk_bt_le_audio_sync_handle_t sync_handle, uint32_t sink_audio_location, uint32_t *p_support_bis_array)
{
	rtk_bt_le_audio_sync_get_support_bis_array_param_t param = {0};

	if (sync_handle == NULL || p_support_bis_array == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.sync_handle = sync_handle;
	param.sink_audio_location = sink_audio_location;
	param.p_support_bis_array = p_support_bis_array;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_SYNC_GET_SUPPORT_BIS_ARRAY,
						   (void *)&param, sizeof(rtk_bt_le_audio_sync_get_support_bis_array_param_t));
}

uint16_t rtk_bt_le_audio_sync_setup_data_path(rtk_bt_le_audio_sync_handle_t sync_handle, uint8_t bis_idx, rtk_bt_le_audio_bis_data_path_param_t *p_setup_param)
{
	rtk_bt_le_audio_sync_setup_data_path_param_t param = {0};

	if (p_setup_param == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.sync_handle = sync_handle;
	param.bis_idx = bis_idx;
	memcpy(&param.setup_param, p_setup_param, sizeof(rtk_bt_le_audio_bis_data_path_param_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_SYNC_SETUP_DATA_PATH,
						   (void *)&param, sizeof(rtk_bt_le_audio_sync_setup_data_path_param_t));
}

uint16_t rtk_bt_le_audio_sync_remove_data_path(rtk_bt_le_audio_sync_handle_t sync_handle, uint8_t bis_idx)
{
	rtk_bt_le_audio_sync_remove_data_path_param_t param = {0};

	param.sync_handle = sync_handle;
	param.bis_idx = bis_idx;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_SYNC_REMOVE_DATA_PATH,
						   (void *)&param, sizeof(rtk_bt_le_audio_sync_remove_data_path_param_t));
}

uint16_t rtk_bt_le_audio_broadcast_source_create(uint8_t num_groups, rtk_bt_le_audio_broadcast_source_group_param_t *group,
												 rtk_bt_le_audio_broadcast_source_handle_t *p_broadcast_source_handle)
{
	rtk_bt_le_audio_broadcast_source_create_param_t param = {0};

	if (num_groups > RTK_BT_LE_AUDIO_BROADCASTER_GROUP_MAX_NUM) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.num_groups = num_groups;
	memcpy(&param.group, group, sizeof(rtk_bt_le_audio_broadcast_source_group_param_t));
	param.p_broadcast_source_handle = p_broadcast_source_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_CREATE,
						   (void *)&param, sizeof(rtk_bt_le_audio_broadcast_source_create_param_t));
}

uint16_t rtk_bt_le_audio_broadcast_source_get_info(rtk_bt_le_audio_broadcast_source_handle_t broadcast_source_handle,
												   rtk_bt_le_audio_broadcast_source_info_t *p_info)
{
	rtk_bt_le_audio_broadcast_source_info_get_param_t param = {0};

	if (p_info == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.broadcast_source_handle = broadcast_source_handle;
	param.p_info = p_info;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_GET_INFO,
						   (void *)&param, sizeof(rtk_bt_le_audio_broadcast_source_info_get_param_t));
}

uint16_t rtk_bt_le_audio_broadcast_source_config(rtk_bt_le_audio_broadcast_source_handle_t broadcast_source_handle, uint8_t adv_sid, uint8_t group_idx,
												 uint8_t *p_broadcast_audio_announcements, uint16_t p_announcements_len, rtk_bt_le_audio_broadcast_source_config_t *p_config)
{
	rtk_bt_le_audio_broadcast_source_config_param_t param = {0};

	if (p_config == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.broadcast_source_handle = broadcast_source_handle;
	param.adv_sid = adv_sid;
	param.group_idx = group_idx;
	param.p_announcements_len = p_announcements_len;
	param.p_broadcast_audio_announcements = p_broadcast_audio_announcements;
	memcpy(&param.config, p_config, sizeof(rtk_bt_le_audio_broadcast_source_config_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_CONFIG,
						   (void *)&param, sizeof(rtk_bt_le_audio_broadcast_source_config_param_t));
}

uint16_t rtk_bt_le_audio_broadcast_source_enable(rtk_bt_le_audio_broadcast_source_handle_t broadcast_source_handle,
												 rtk_bt_le_audio_broadcast_source_create_big_t *p_create_big)
{
	rtk_bt_le_audio_broadcast_source_enable_param_t param = {0};

	if (p_create_big == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.broadcast_source_handle = broadcast_source_handle;
	memcpy(&param.create_big, p_create_big, sizeof(rtk_bt_le_audio_broadcast_source_create_big_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_ENABLE,
						   (void *)&param, sizeof(rtk_bt_le_audio_broadcast_source_enable_param_t));
}

uint16_t rtk_bt_le_audio_broadcast_source_disable(rtk_bt_le_audio_broadcast_source_handle_t broadcast_source_handle)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_DISABLE,
						   (void *)&broadcast_source_handle, sizeof(rtk_bt_le_audio_broadcast_source_handle_t));
}

uint16_t rtk_bt_le_audio_broadcast_source_release(rtk_bt_le_audio_broadcast_source_handle_t broadcast_source_handle)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_RELEASE,
						   (void *)&broadcast_source_handle, sizeof(rtk_bt_le_audio_broadcast_source_handle_t));
}

uint16_t rtk_bt_le_audio_broadcast_source_setup_data_path(rtk_bt_le_audio_broadcast_source_handle_t broadcast_source_handle, uint8_t bis_idx,
														  rtk_bt_le_audio_bis_data_path_param_t *p_setup_param, uint16_t *p_bis_conn_handle)
{
	rtk_bt_le_audio_broadcast_source_setup_data_path_param_t param = {0};

	if (p_setup_param == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.broadcast_source_handle = broadcast_source_handle;
	param.bis_idx = bis_idx;
	memcpy(&param.setup_param, p_setup_param, sizeof(rtk_bt_le_audio_bis_data_path_param_t));
	param.p_bis_conn_handle = p_bis_conn_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_SETUP_DATA_PATH,
						   (void *)&param, sizeof(rtk_bt_le_audio_broadcast_source_setup_data_path_param_t));
}

uint16_t rtk_bt_le_audio_broadcast_source_remove_data_path(rtk_bt_le_audio_broadcast_source_handle_t broadcast_source_handle, uint8_t bis_idx)
{
	rtk_bt_le_audio_broadcast_source_remove_data_path_param_t param = {0};

	param.broadcast_source_handle = broadcast_source_handle;
	param.bis_idx = bis_idx;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_REMOVE_DATA_PATH,
						   (void *)&param, sizeof(rtk_bt_le_audio_broadcast_source_remove_data_path_param_t));
}

uint16_t rtk_bt_le_audio_broadcast_source_reconfig(rtk_bt_le_audio_broadcast_source_handle_t broadcast_source_handle, uint8_t group_idx, uint8_t subgroup_idx,
												   uint8_t metadata_len, uint8_t *p_metadata)
{
	rtk_bt_le_audio_broadcast_source_reconfig_param_t param = {0};

	param.broadcast_source_handle = broadcast_source_handle;
	param.group_idx = group_idx;
	param.subgroup_idx = subgroup_idx;
	param.metadata_len = metadata_len;
	param.p_metadata = p_metadata;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_RECONFIG,
						   (void *)&param, sizeof(rtk_bt_le_audio_broadcast_source_reconfig_param_t));
}

uint16_t rtk_bt_le_audio_bass_get_brs_data(uint16_t conn_handle, uint8_t char_instance_id, bool is_get_bis_info, rtk_bt_le_audio_bass_brs_data_t *p_brs_data)
{
	rtk_bt_le_audio_bass_client_get_brs_data_param_t param = {0};

	if (p_brs_data == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.conn_handle = conn_handle;
	param.char_instance_id = char_instance_id;
	param.is_get_bis_info = is_get_bis_info;
	param.p_brs_data = p_brs_data;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BASS_CLIENT_GET_BRS_CHAR_DATA,
						   (void *)&param, sizeof(rtk_bt_le_audio_bass_client_get_brs_data_param_t));
}

uint16_t rtk_bt_le_audio_bass_client_cp_add_source_by_sync_info(uint16_t conn_handle, rtk_bt_le_audio_sync_handle_t sync_handle, uint8_t pa_sync,
																uint32_t bis_array, bool is_req)
{
	rtk_bt_le_audio_bass_client_cp_add_source_by_sync_info_param_t param = {0};

	param.conn_handle = conn_handle;
	param.sync_handle = sync_handle;
	param.pa_sync = pa_sync;
	param.bis_array = bis_array;
	param.is_req = is_req;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BASS_CLIENT_CP_ADD_SOURCE_BY_SYNC_INFO,
						   (void *)&param, sizeof(rtk_bt_le_audio_bass_client_cp_add_source_by_sync_info_param_t));
}
uint16_t rtk_bt_le_audio_bass_client_cp_modify_source_by_sync_info(uint16_t conn_handle, rtk_bt_le_audio_sync_handle_t sync_handle, uint8_t source_id,
																   uint8_t pa_sync, uint32_t bis_array, bool is_req)
{
	rtk_bt_le_audio_bass_client_cp_modify_source_by_sync_info_param_t param = {0};

	param.conn_handle = conn_handle;
	param.sync_handle = sync_handle;
	param.source_id = source_id;
	param.pa_sync = pa_sync;
	param.bis_array = bis_array;
	param.is_req = is_req;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BASS_CLIENT_CP_MODIFY_SOURCE_BY_SYNC_INFO,
						   (void *)&param, sizeof(rtk_bt_le_audio_bass_client_cp_modify_source_by_sync_info_param_t));
}

uint16_t rtk_bt_le_audio_bass_client_write_cp_op(uint16_t conn_handle, uint8_t opcode, void *op_param, bool is_req)
{
	rtk_bt_le_audio_bass_client_write_cp_op_param_t param = {0};

	param.conn_handle = conn_handle;
	param.is_req = is_req;
	param.opcode = opcode;
	if (op_param) {
		memcpy(&param.op_param, op_param, sizeof(param.op_param));
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BASS_CLIENT_WRITE_CP_OP,
						   (void *)&param, sizeof(rtk_bt_le_audio_bass_client_write_cp_op_param_t));
}

uint16_t rtk_bt_le_audio_bass_client_past(uint8_t past_type, void *handle, uint16_t conn_handle, rtk_bt_le_audio_bass_past_srv_data_t srv_data)
{
	rtk_bt_le_audio_bass_client_past_param_t param = {0};

	param.past_type = past_type;
	param.handle = handle;
	param.conn_handle = conn_handle;
	memcpy(&param.srv_data, &srv_data, sizeof(rtk_bt_le_audio_bass_past_srv_data_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_BASS_CLIENT_PAST,
						   (void *)&param, sizeof(rtk_bt_le_audio_bass_client_past_param_t));
}

uint16_t rtk_bt_le_audio_group_allocate(rtk_bt_le_audio_group_handle_t *p_group_handle)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GROUP_ALLOCATE,
						   p_group_handle, sizeof(rtk_bt_le_audio_group_handle_t));
}

uint16_t rtk_bt_le_audio_group_release(rtk_bt_le_audio_group_handle_t group_handle)
{
	if (group_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GROUP_RELEASE,
						   (void *)&group_handle, sizeof(rtk_bt_le_audio_group_handle_t));
}

uint16_t rtk_bt_le_audio_group_add_device(rtk_bt_le_audio_group_handle_t group_handle, uint16_t conn_handle, rtk_bt_le_audio_device_handle_t *p_device_handle)
{
	rtk_bt_le_audio_group_add_device_param_t param = {0};

	if (group_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.group_handle = group_handle;
	param.conn_handle = conn_handle;
	param.p_device_handle = p_device_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GROUP_ADD_DEVICE,
						   (void *)&param, sizeof(rtk_bt_le_audio_group_add_device_param_t));
}

uint16_t rtk_bt_le_audio_group_delete_device(rtk_bt_le_audio_group_handle_t group_handle, rtk_bt_le_audio_device_handle_t device_handle)
{
	rtk_bt_le_audio_group_delete_device_param_t param = {0};

	param.group_handle = group_handle;
	param.device_handle = device_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GROUP_DELETE_DEVICE,
						   (void *)&param, sizeof(rtk_bt_le_audio_group_delete_device_param_t));
}
uint16_t rtk_bt_le_audio_group_find_device(rtk_bt_le_audio_group_handle_t group_handle, rtk_bt_le_addr_t dev_addr,
										   rtk_bt_le_audio_device_handle_t *p_device_handle)
{
	rtk_bt_le_audio_group_find_device_param_t param = {0};

	if (group_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.group_handle = group_handle;
	memcpy((void *)&param.dev_addr, (void *)&dev_addr, sizeof(rtk_bt_le_addr_t));
	param.p_device_handle = p_device_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GROUP_FIND_DEVICE,
						   (void *)&param, sizeof(rtk_bt_le_audio_group_find_device_param_t));
}
uint16_t rtk_bt_le_audio_group_find_device_by_conn_handle(rtk_bt_le_audio_group_handle_t group_handle, uint16_t conn_handle,
														  rtk_bt_le_audio_device_handle_t *p_device_handle)
{
	rtk_bt_le_audio_group_find_device_by_conn_handle_param_t param = {0};

	if (group_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.group_handle = group_handle;
	param.conn_handle = conn_handle;
	param.p_device_handle = p_device_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GROUP_FIND_DEVICE_BY_CONN_HANDLE,
						   (void *)&param, sizeof(rtk_bt_le_audio_group_find_device_by_conn_handle_param_t));
}
uint16_t rtk_bt_le_audio_group_get_device_num(rtk_bt_le_audio_group_handle_t group_handle, uint8_t *device_num)
{
	rtk_bt_le_audio_group_get_device_num_param_t param = {0};

	if (group_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.group_handle = group_handle;
	param.p_device_num = device_num;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GROUP_GET_DEVICE_NUM,
						   (void *)&param, sizeof(rtk_bt_le_audio_group_get_device_num_param_t));
}
uint16_t rtk_bt_le_audio_group_get_used_device_num(rtk_bt_le_audio_group_handle_t group_handle, bool check_conn, uint8_t *p_used_device_num)
{
	rtk_bt_le_audio_group_get_used_device_num_param_t param = {0};

	if (group_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.group_handle = group_handle;
	param.check_conn = check_conn;
	param.p_used_device_num = p_used_device_num;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GROUP_GET_USED_DEVICE_NUM,
						   (void *)&param, sizeof(rtk_bt_le_audio_group_get_used_device_num_param_t));
}

uint16_t rtk_bt_le_audio_group_get_device_info(rtk_bt_le_audio_group_handle_t group_handle, rtk_bt_le_audio_device_handle_t device_handle,
											   rtk_bt_le_audio_group_device_info_t *p_device_info)
{
	rtk_bt_le_audio_group_get_device_info_param_t param = {0};

	if (group_handle == NULL || device_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.group_handle = group_handle;
	param.device_handle = device_handle;
	param.p_device_info = p_device_info;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GROUP_GET_DEVICE_INFO,
						   (void *)&param, sizeof(rtk_bt_le_audio_group_get_device_info_param_t));
}
uint16_t rtk_bt_le_audio_group_get_all_device_info(rtk_bt_le_audio_group_handle_t group_handle, uint8_t *device_num,
												   rtk_bt_le_audio_group_device_info_t *p_device_info_tbl)
{
	rtk_bt_le_audio_group_get_all_device_info_param_t param = {0};

	if (group_handle == NULL || device_num == NULL || p_device_info_tbl == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.group_handle = group_handle;
	param.p_device_num = device_num;
	param.p_device_info_tbl = p_device_info_tbl;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GROUP_GET_ALL_DEVICE_INFO,
						   (void *)&param, sizeof(rtk_bt_le_audio_group_get_all_device_info_param_t));
}

uint16_t rtk_bt_le_audio_stream_session_allocate(rtk_bt_le_audio_group_handle_t group_handle, rtk_bt_le_audio_stream_session_handle_t *p_stream_session_handle)
{
	rtk_bt_le_audio_stream_session_allocate_param_t param = {0};

	if (group_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.group_handle = group_handle;
	param.p_stream_session_handle = p_stream_session_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_STREAM_SESSION_ALLOCATE,
						   (void *)&param, sizeof(rtk_bt_le_audio_stream_session_allocate_param_t));
}

uint16_t rtk_bt_le_audio_stream_session_release(rtk_bt_le_audio_stream_session_handle_t stream_session_handle)
{
	if (stream_session_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_STREAM_SESSION_RELEASE,
						   (void *)&stream_session_handle, sizeof(rtk_bt_le_audio_stream_session_handle_t));
}

uint16_t rtk_bt_le_audio_stream_session_get_group_handle(rtk_bt_le_audio_stream_session_handle_t stream_session_handle,
														 rtk_bt_le_audio_group_handle_t *p_group_handle)
{
	rtk_bt_le_audio_stream_session_get_group_handle_param_t param = {0};

	if (stream_session_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.stream_session_handle = stream_session_handle;
	param.p_group_handle = p_group_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_STREAM_SESSION_GET_GROUP_HANDLE,
						   (void *)&param, sizeof(rtk_bt_le_audio_stream_session_get_group_handle_param_t));
}
uint16_t rtk_bt_le_audio_unicast_get_session_info(rtk_bt_le_audio_stream_session_handle_t stream_session_handle,
												  rtk_bt_le_audio_unicast_session_info_t *p_session_info)
{
	rtk_bt_le_audio_unicast_get_session_info_param_t param = {0};

	if (stream_session_handle == NULL || p_session_info == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.stream_session_handle = stream_session_handle;
	param.p_session_info = p_session_info;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_UNICAST_GET_SESSION_INFO,
						   (void *)&param, sizeof(rtk_bt_le_audio_unicast_get_session_info_param_t));
}

uint16_t rtk_bt_le_audio_unicast_get_cfg_mask(rtk_bt_le_audio_stream_session_handle_t stream_session_handle, uint32_t prefer_cfg, uint8_t device_num,
											  rtk_bt_le_audio_device_handle_t *p_device_handle_tbl, uint32_t *p_cfg_mask)
{
	rtk_bt_le_audio_unicast_get_cfg_mask_param_t param = {0};

	if (stream_session_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.stream_session_handle = stream_session_handle;
	param.prefer_cfg = prefer_cfg;
	param.device_num = device_num;
	param.p_device_handle_tbl = p_device_handle_tbl;
	param.p_cfg_mask = p_cfg_mask;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_UNICAST_GET_CFG_MASK,
						   (void *)&param, sizeof(rtk_bt_le_audio_unicast_get_cfg_mask_param_t));
}

uint16_t rtk_bt_le_audio_unicast_get_cis_info(rtk_bt_le_audio_stream_session_handle_t stream_session_handle, rtk_bt_le_audio_device_handle_t device_handle,
											  rtk_bt_le_audio_unicast_cis_infos_t *p_cis_infos)
{
	rtk_bt_le_audio_unicast_get_cis_info_param_t param = {0};

	if (stream_session_handle == NULL || device_handle == NULL || p_cis_infos == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.stream_session_handle = stream_session_handle;
	param.device_handle = device_handle;
	param.p_cis_infos = p_cis_infos;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_UNICAST_GET_CIS_INFO,
						   (void *)&param, sizeof(rtk_bt_le_audio_unicast_get_cis_info_param_t));
}

uint16_t rtk_bt_le_audio_unicast_config(rtk_bt_le_audio_stream_session_handle_t stream_session_handle, rtk_bt_le_audio_unicast_cfg_type_t cfg_type,
										uint8_t device_num, rtk_bt_le_audio_device_handle_t *p_device_handle_tbl)
{
	rtk_bt_le_audio_unicast_config_param_t param = {0};

	if (stream_session_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.stream_session_handle = stream_session_handle;
	param.cfg_type = cfg_type;
	param.device_num = device_num;
	param.p_device_handle_tbl = p_device_handle_tbl;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_UNICAST_CONFIG,
						   (void *)&param, sizeof(rtk_bt_le_audio_unicast_config_param_t));
}
uint16_t rtk_bt_le_audio_unicast_cfg_ase_codec(rtk_bt_le_audio_stream_session_handle_t stream_session_handle, rtk_bt_le_audio_device_handle_t device_handle,
											   uint8_t cfg_idx, rtk_bt_le_audio_unicast_ase_codec_t ase_codec)
{
	rtk_bt_le_audio_unicast_cfg_ase_codec_param_t param = {0};

	if (stream_session_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.stream_session_handle = stream_session_handle;
	param.device_handle = device_handle;
	param.cfg_idx = cfg_idx;
	memcpy((void *)&param.ase_codec, (void *)&ase_codec, sizeof(rtk_bt_le_audio_unicast_ase_codec_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_UNICAST_CFG_ASE_CODEC,
						   (void *)&param, sizeof(rtk_bt_le_audio_unicast_cfg_ase_codec_param_t));
}
uint16_t rtk_bt_le_audio_unicast_remove_config(rtk_bt_le_audio_stream_session_handle_t stream_session_handle)
{
	if (stream_session_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_UNICAST_REMOVE_CONFIG,
						   (void *)&stream_session_handle, sizeof(rtk_bt_le_audio_stream_session_handle_t));
}

uint16_t rtk_bt_le_audio_unicast_get_session_qos(rtk_bt_le_audio_stream_session_handle_t stream_session_handle,
												 rtk_bt_le_audio_unicast_session_qos_t *p_session_qos)
{
	rtk_bt_le_audio_unicast_get_session_qos_param_t param = {0};

	if (stream_session_handle == NULL || p_session_qos == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.stream_session_handle = stream_session_handle;
	param.p_session_qos = p_session_qos;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_UNICAST_GET_SESSION_QOS,
						   (void *)&param, sizeof(rtk_bt_le_audio_unicast_get_session_qos_param_t));
}

uint16_t rtk_bt_le_audio_unicast_cfg_ase_qos(rtk_bt_le_audio_stream_session_handle_t stream_session_handle, rtk_bt_le_audio_device_handle_t device_handle,
											 uint8_t ase_id, rtk_bt_le_audio_unicast_ase_qos_t ase_qos)
{
	rtk_bt_le_audio_unicast_cfg_ase_qos_param_t param = {0};

	if (stream_session_handle == NULL || device_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.stream_session_handle = stream_session_handle;
	param.device_handle = device_handle;
	param.ase_id = ase_id;
	memcpy(&param.ase_qos, &ase_qos, sizeof(rtk_bt_le_audio_unicast_ase_qos_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_UNICAST_CFG_ASE_QOS,
						   (void *)&param, sizeof(rtk_bt_le_audio_unicast_cfg_ase_qos_param_t));
}
uint16_t rtk_bt_le_audio_unicast_get_ase_qos(rtk_bt_le_audio_stream_session_handle_t stream_session_handle, rtk_bt_le_audio_device_handle_t device_handle,
											 uint8_t ase_id, rtk_bt_le_audio_unicast_ase_qos_t *p_ase_qos)
{
	rtk_bt_le_audio_unicast_get_ase_qos_param_t param = {0};

	if (stream_session_handle == NULL || device_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.stream_session_handle = stream_session_handle;
	param.device_handle = device_handle;
	param.ase_id = ase_id;
	param.p_ase_qos = p_ase_qos;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_UNICAST_GET_ASE_QOS,
						   (void *)&param, sizeof(rtk_bt_le_audio_unicast_get_ase_qos_param_t));
}
uint16_t rtk_bt_le_audio_unicast_start(rtk_bt_le_audio_stream_session_handle_t stream_session_handle)
{
	if (stream_session_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_UNICAST_START,
						   (void *)&stream_session_handle, sizeof(rtk_bt_le_audio_stream_session_handle_t));
}
uint16_t rtk_bt_le_audio_unicast_stop(rtk_bt_le_audio_stream_session_handle_t stream_session_handle, uint32_t cis_timeout_ms)
{
	rtk_bt_le_audio_unicast_stop_param_t param = {0};

	if (stream_session_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.stream_session_handle = stream_session_handle;
	param.cis_timeout_ms = cis_timeout_ms;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_UNICAST_STOP,
						   (void *)&param, sizeof(rtk_bt_le_audio_unicast_stop_param_t));
}
uint16_t rtk_bt_le_audio_unicast_release(rtk_bt_le_audio_stream_session_handle_t stream_session_handle)
{
	if (stream_session_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_UNICAST_RELEASE,
						   (void *)&stream_session_handle, sizeof(rtk_bt_le_audio_stream_session_handle_t));
}
uint16_t rtk_bt_le_audio_unicast_update(rtk_bt_le_audio_stream_session_handle_t stream_session_handle)
{
	if (stream_session_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_UNICAST_UPDATE,
						   (void *)&stream_session_handle, sizeof(rtk_bt_le_audio_stream_session_handle_t));
}

#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
uint16_t rtk_bt_le_audio_csis_gen_rsi(uint8_t *p_sirk, uint8_t *p_rsi)
{
	rtk_bt_le_audio_csis_gen_rsi_info_t info = {0};

	memcpy(info.sirk, p_sirk, RTK_BT_LE_CSIS_SIRK_LEN);
	info.p_rsi = p_rsi;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_CSIS_GEN_RSI,
						   (void *)&info, sizeof(rtk_bt_le_audio_csis_gen_rsi_info_t));
}

uint16_t rtk_bt_le_audio_csis_update_sirk(rtk_bt_le_audio_csis_sirk_type_t sirk_type, uint8_t *p_sirk)
{
	rtk_bt_le_audio_csis_update_sirk_param_t info = {0};

	info.sirk_type = sirk_type;
	memcpy(info.sirk, p_sirk, RTK_BT_LE_CSIS_SIRK_LEN);

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_CSIS_UPDATE_SIRK,
						   (void *)&info, sizeof(rtk_bt_le_audio_csis_update_sirk_param_t));
}

uint16_t rtk_bt_le_audio_csis_update_size(uint8_t csis_size)
{
	rtk_bt_le_audio_csis_update_size_param_t info = {0};

	info.csis_size = csis_size;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_CSIS_UPDATE_SIRK,
						   (void *)&info, sizeof(rtk_bt_le_audio_csis_update_size_param_t));
}
#endif
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
uint16_t rtk_bt_le_audio_csis_set_coordinator_check_adv_rsi(rtk_bt_le_addr_t adv_addr, uint8_t report_data_len, uint8_t *p_report_data)
{
	rtk_bt_le_audio_csis_set_coordinator_check_adv_rsi_info_t info = {0};

	if (report_data_len != 0 && p_report_data == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	info.report_data_len = report_data_len;
	info.p_report_data = p_report_data;
	memcpy(&info.adv_addr, &adv_addr, sizeof(rtk_bt_le_addr_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_CHECK_ADV_RSI,
						   (void *)&info, sizeof(rtk_bt_le_audio_csis_set_coordinator_check_adv_rsi_info_t));
}

uint16_t rtk_bt_le_audio_csis_set_coordinator_cfg_discover(rtk_bt_le_audio_group_handle_t group_handle, bool discover, uint32_t timeout_ms)
{
	rtk_bt_le_audio_csis_set_coordinator_cfg_discover_info_t info = {0};

	if (group_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	info.group_handle = group_handle;
	info.discover = discover;
	info.timeout_ms = timeout_ms;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_CFG_DISCOVER,
						   (void *)&info, sizeof(rtk_bt_le_audio_csis_set_coordinator_cfg_discover_info_t));
}

uint16_t rtk_bt_le_audio_csis_set_coordinator_add_group(rtk_bt_le_audio_csis_set_mem_info_t *p_mem_info, rtk_bt_le_audio_group_handle_t *p_group_handle,
														rtk_bt_le_audio_device_handle_t *p_device_handle)
{
	rtk_bt_le_audio_csis_set_coordinator_add_group_info_t info = {0};

	if (p_mem_info == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	memcpy(&info.mem_info, p_mem_info, sizeof(rtk_bt_le_audio_csis_set_mem_info_t));
	info.p_group_handle = p_group_handle;
	info.p_device_handle = p_device_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_ADD_GROUP,
						   (void *)&info, sizeof(rtk_bt_le_audio_csis_set_coordinator_add_group_info_t));
}

uint16_t rtk_bt_le_audio_csis_set_coordinator_add_device(rtk_bt_le_audio_group_handle_t group_handle, rtk_bt_le_audio_csis_set_mem_info_t *p_mem_info,
														 rtk_bt_le_audio_device_handle_t *p_device_handle)
{
	rtk_bt_le_audio_csis_set_coordinator_add_device_info_t info = {0};

	if (group_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	info.group_handle = group_handle;
	memcpy(&info.mem_info, p_mem_info, sizeof(rtk_bt_le_audio_csis_set_mem_info_t));
	info.p_device_handle = p_device_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_ADD_DEVICE,
						   (void *)&info, sizeof(rtk_bt_le_audio_csis_set_coordinator_add_device_info_t));
}
uint16_t rtk_bt_le_audio_csis_set_coordinator_write_lock(rtk_bt_le_audio_group_handle_t group_handle, bool lock)
{
	rtk_bt_le_audio_csis_set_coordinator_write_lock_info_t info = {0};

	if (group_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	info.group_handle = group_handle;
	info.lock = lock;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_WRITE_LOCK,
						   (void *)&info, sizeof(rtk_bt_le_audio_csis_set_coordinator_write_lock_info_t));
}
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
uint16_t rtk_bt_le_audio_vcs_set_param(rtk_bt_le_audio_vcs_param_t *param)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VCS_SET_PARAM,
						   (void *)param, sizeof(rtk_bt_le_audio_vcs_param_t));
}

uint16_t rtk_bt_le_audio_vcs_get_param(rtk_bt_le_audio_vcs_param_t *param)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VCS_GET_PARAM,
						   (void *)param, sizeof(rtk_bt_le_audio_vcs_param_t));
}
#endif
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
uint16_t rtk_bt_le_audio_vcs_write_cp(uint16_t conn_handle, rtk_bt_le_audio_vcs_cp_op_t cp_op, rtk_bt_le_audio_vcs_cp_param_t *cp_param)
{
	rtk_bt_le_audio_vcs_write_cp_param_t param = {0};

	param.conn_handle = conn_handle;
	param.cp_op = cp_op;
	memcpy(&param.cp_param, cp_param, sizeof(rtk_bt_le_audio_vcs_cp_param_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VCS_WRITE_CP,
						   (void *)&param, sizeof(rtk_bt_le_audio_vcs_write_cp_param_t));
}
uint16_t rtk_bt_le_audio_vcs_get_volume_state(uint16_t conn_handle, rtk_bt_le_audio_vcs_volume_state_t *p_vcs_volume_state)
{
	rtk_bt_le_audio_vcs_get_volume_state_param_t param = {0};

	param.conn_handle = conn_handle;
	param.p_vcs_volume_state = p_vcs_volume_state;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VCS_GET_VOLUME_STATE,
						   (void *)&param, sizeof(rtk_bt_le_audio_vcs_get_volume_state_param_t));
}
uint16_t rtk_bt_le_audio_vcs_change_mute(rtk_bt_le_audio_group_handle_t group_handle, uint8_t mute)
{
	rtk_bt_le_audio_vcs_change_mute_param_t param = {0};

	param.group_handle = group_handle;
	param.mute = mute;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VCS_CHANGE_MUTE,
						   (void *)&param, sizeof(rtk_bt_le_audio_vcs_change_mute_param_t));
}
uint16_t rtk_bt_le_audio_vcs_change_volume(rtk_bt_le_audio_group_handle_t group_handle, uint8_t volume_setting)
{
	rtk_bt_le_audio_vcs_change_volume_param_t param = {0};

	param.group_handle = group_handle;
	param.volume_setting = volume_setting;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VCS_CHANGE_VOLUME,
						   (void *)&param, sizeof(rtk_bt_le_audio_vcs_change_volume_param_t));
}
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
uint16_t rtk_bt_le_audio_mics_change_mute(rtk_bt_le_audio_group_handle_t group_handle, rtk_bt_le_audio_mics_mute_state_t mic_mute)
{
	rtk_bt_le_audio_mics_change_mute_param_t param = {0};

	param.group_handle = group_handle;
	param.mic_mute = mic_mute;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_MICS_CHANGE_MUTE,
						   (void *)&param, sizeof(rtk_bt_le_audio_mics_change_mute_param_t));
}
uint16_t rtk_bt_le_audio_mics_set_mute_value(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t mic_mute)
{
	rtk_bt_le_audio_mics_set_mute_value_param_t param = {0};

	param.conn_handle = conn_handle;
	param.mic_mute = mic_mute;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_MICS_SET_MUTE_VALUE,
						   (void *)&param, sizeof(rtk_bt_le_audio_mics_set_mute_value_param_t));
}

uint16_t rtk_bt_le_audio_mics_get_mute_value(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t *p_mic_mute)
{
	rtk_bt_le_audio_mics_get_mute_value_param_t param = {0};

	param.conn_handle = conn_handle;
	param.p_mic_mute = p_mic_mute;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_MICS_GET_MUTE_VALUE,
						   (void *)&param, sizeof(rtk_bt_le_audio_mics_get_mute_value_param_t));
}
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
uint16_t rtk_bt_le_audio_mics_set_param(rtk_bt_le_audio_mics_param_t *param)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_MICS_SET_PARAM,
						   (void *)param, sizeof(rtk_bt_le_audio_mics_param_t));
}

uint16_t rtk_bt_le_audio_mics_get_param(rtk_bt_le_audio_mics_param_t *param)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_MICS_GET_PARAM,
						   (void *)param, sizeof(rtk_bt_le_audio_mics_param_t));
}
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
uint16_t rtk_bt_le_audio_vocs_set_param(uint8_t srv_instance_id, uint8_t set_mask, rtk_bt_le_audio_vocs_param_t *p_vocs_param)
{
	rtk_bt_le_audio_vocs_set_param_t param = {0};

	if (p_vocs_param == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.srv_instance_id = srv_instance_id;
	param.set_mask = set_mask;
	param.vocs_param.volume_offset = p_vocs_param->volume_offset;
	param.vocs_param.change_counter = p_vocs_param->change_counter;
	param.vocs_param.audio_location = p_vocs_param->audio_location;
	param.vocs_param.output_des.output_des_len = p_vocs_param->output_des.output_des_len;
	param.vocs_param.output_des.p_output_des = p_vocs_param->output_des.p_output_des;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VOCS_SET_PARAM,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_set_param_t));
}

uint16_t rtk_bt_le_audio_vocs_get_param(uint8_t srv_instance_id, rtk_bt_le_audio_vocs_param_t *p_vocs_param)
{
	rtk_bt_le_audio_vocs_get_param_t param = {0};

	if (p_vocs_param == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.srv_instance_id = srv_instance_id;
	param.p_vocs_param = p_vocs_param;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VOCS_GET_PARAM,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_get_param_t));
}

uint16_t rtk_bt_le_audio_vocs_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t cfg_flags, bool enable)
{
	rtk_bt_le_audio_vocs_cfg_cccd_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.cfg_flags = cfg_flags;
	param.enable = enable;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VOCS_CFG_CCCD,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_cfg_cccd_param_t));
}

uint16_t rtk_bt_le_audio_vocs_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_char_type_t type)
{
	rtk_bt_le_audio_vocs_read_char_value_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.type = type;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VOCS_READ_CHAR_VALUE,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_read_char_value_param_t));
}

uint16_t rtk_bt_le_audio_vocs_write_cp(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_cp_op_t cp_op,
									   rtk_bt_le_audio_vocs_cp_param_t cp_param)
{
	rtk_bt_le_audio_vocs_write_cp_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.cp_op = cp_op;
	memcpy(&param.cp_param, &cp_param, sizeof(rtk_bt_le_audio_vocs_cp_param_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_CP,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_write_cp_param_t));
}

uint16_t rtk_bt_le_audio_vocs_write_cp_by_group(rtk_bt_le_audio_group_handle_t group_handle, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_cp_op_t cp_op,
												rtk_bt_le_audio_vocs_cp_param_t cp_param)
{
	rtk_bt_le_audio_vocs_write_cp_by_group_param_t param = {0};

	param.group_handle = group_handle;
	param.srv_instance_id = srv_instance_id;
	param.cp_op = cp_op;
	memcpy(&param.cp_param, &cp_param, sizeof(rtk_bt_le_audio_vocs_cp_param_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_CP_BY_GROUP,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_write_cp_by_group_param_t));
}
uint16_t rtk_bt_le_audio_vocs_write_audio_location(uint16_t conn_handle, uint8_t srv_instance_id, uint32_t audio_location)
{
	rtk_bt_le_audio_vocs_write_audio_location_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.audio_location = audio_location;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_AUDIO_LOCATION,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_write_audio_location_param_t));
}
uint16_t rtk_bt_le_audio_vocs_write_output_des(uint16_t conn_handle, uint8_t srv_instance_id, uint16_t output_des_len, uint8_t *p_output_des)
{
	rtk_bt_le_audio_vocs_write_output_des_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.output_des_len = output_des_len;
	param.p_output_des = p_output_des;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_OUTPUT_DES,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_write_output_des_param_t));
}
uint16_t rtk_bt_le_audio_vocs_get_srv_data(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_srv_data_t *p_srv_data)
{
	rtk_bt_le_audio_vocs_get_srv_data_param_t param = {0};

	if (p_srv_data == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.p_srv_data = p_srv_data;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_VOCS_GET_SRV_DATA,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_get_srv_data_param_t));
}
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
uint16_t rtk_bt_le_audio_aics_set_param(uint8_t srv_instance_id, rtk_bt_le_audio_aics_param_type_t aics_param_type, uint8_t value_len, uint8_t *p_value,
										bool set_change_counter)
{
	rtk_bt_le_audio_aics_set_param_t param = {0};

	if (p_value == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.srv_instance_id = srv_instance_id;
	param.aics_param_type = aics_param_type;
	param.value_len = value_len;
	param.p_value = p_value;
	param.set_change_counter = set_change_counter;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_AICS_SET_PARAM,
						   (void *)&param, sizeof(rtk_bt_le_audio_aics_set_param_t));
}

uint16_t rtk_bt_le_audio_aics_get_param(uint8_t srv_instance_id, rtk_bt_le_audio_aics_param_type_t aics_param_type, uint8_t value_len, uint8_t *p_value)
{
	rtk_bt_le_audio_aics_get_param_t param = {0};

	if (p_value == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.srv_instance_id = srv_instance_id;
	param.aics_param_type = aics_param_type;
	param.value_len = value_len;
	param.p_value = p_value;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_AICS_GET_PARAM,
						   (void *)&param, sizeof(rtk_bt_le_audio_aics_get_param_t));
}

uint16_t rtk_bt_le_audio_aics_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t cfg_flags, bool enable)
{
	rtk_bt_le_audio_aics_cfg_cccd_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.cfg_flags = cfg_flags;
	param.enable = enable;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_AICS_CFG_CCCD,
						   (void *)&param, sizeof(rtk_bt_le_audio_aics_cfg_cccd_param_t));
}

uint16_t rtk_bt_le_audio_aics_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_aics_char_type_t type)
{
	rtk_bt_le_audio_aics_read_char_value_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.type = type;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_AICS_READ_CHAR_VALUE,
						   (void *)&param, sizeof(rtk_bt_le_audio_aics_read_char_value_param_t));
}

uint16_t rtk_bt_le_audio_aics_write_cp(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_aics_cp_op_t cp_op,
									   rtk_bt_le_audio_aics_cp_param_t cp_param)
{
	rtk_bt_le_audio_aics_write_cp_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.cp_op = cp_op;
	memcpy(&param.cp_param, &cp_param, sizeof(rtk_bt_le_audio_aics_cp_param_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_AICS_WRITE_CP,
						   (void *)&param, sizeof(rtk_bt_le_audio_aics_write_cp_param_t));
}

uint16_t rtk_bt_le_audio_aics_write_cp_by_group(rtk_bt_le_audio_group_handle_t group_handle, uint8_t srv_instance_id, rtk_bt_le_audio_aics_cp_op_t cp_op,
												rtk_bt_le_audio_aics_cp_param_t cp_param)
{
	rtk_bt_le_audio_aics_write_cp_by_group_param_t param = {0};

	param.group_handle = group_handle;
	param.srv_instance_id = srv_instance_id;
	param.cp_op = cp_op;
	memcpy(&param.cp_param, &cp_param, sizeof(rtk_bt_le_audio_aics_cp_param_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_AICS_WRITE_CP_BY_GROUP,
						   (void *)&param, sizeof(rtk_bt_le_audio_aics_write_cp_by_group_param_t));
}

uint16_t rtk_bt_le_audio_aics_write_input_des(uint16_t conn_handle, uint8_t srv_instance_id, uint16_t input_des_len, uint8_t *p_input_des)
{
	rtk_bt_le_audio_aics_write_input_des_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.input_des_len = input_des_len;
	param.p_input_des = p_input_des;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_AICS_WRITE_INPUT_DES,
						   (void *)&param, sizeof(rtk_bt_le_audio_aics_write_input_des_param_t));
}

uint16_t rtk_bt_le_audio_aics_get_srv_data(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_aics_srv_data_t *p_srv_data)
{
	rtk_bt_le_audio_aics_get_srv_data_param_t param = {0};

	if (p_srv_data == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.p_srv_data = p_srv_data;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_AICS_GET_SRV_DATA,
						   (void *)&param, sizeof(rtk_bt_le_audio_aics_get_srv_data_param_t));
}
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
uint16_t rtk_bt_le_audio_mcs_client_write_media_cp(uint16_t conn_handle, uint8_t srv_instance_id, bool general_mcs, bool is_req, uint8_t opcode,
												   void *p_op_param)
{
	rtk_bt_le_audio_mcp_client_write_media_cp_info_t info = {0};

	info.conn_handle = conn_handle;
	info.srv_instance_id = srv_instance_id;
	info.general_mcs = general_mcs;
	info.is_req = is_req;
	info.opcode = opcode;
	memcpy(&info.op_param, p_op_param, sizeof(info.op_param));


	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_WRITE_MEDIA_CP,
						   (void *)&info, sizeof(rtk_bt_le_audio_mcp_client_write_media_cp_info_t));
}

uint16_t rtk_bt_le_audio_mcs_client_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, bool general_mcs, uint16_t char_uuid)
{
	rtk_bt_le_audio_mcp_client_read_char_value_info_t info = {0};

	if (char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME && char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE &&
		char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION && char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION &&
		char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_STATE && char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID) {
		return RTK_BT_ERR_UNSUPPORTED;
	}

	info.conn_handle = conn_handle;
	info.srv_instance_id = srv_instance_id;
	info.general_mcs = general_mcs;
	info.char_uuid = char_uuid;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_READ_CHAR_VALUE,
						   (void *)&info, sizeof(rtk_bt_le_audio_mcp_client_read_char_value_info_t));
}

uint16_t rtk_bt_le_audio_mcs_client_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, bool general_mcs, uint32_t cfg_flags, bool enable)
{
	rtk_bt_le_audio_mcp_client_cfg_cccd_info_t info = {0};

	info.conn_handle = conn_handle;
	info.srv_instance_id = srv_instance_id;
	info.general_mcs = general_mcs;
	info.cfg_flags = cfg_flags;
	info.enable = enable;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_CFG_CCCD,
						   (void *)&info, sizeof(rtk_bt_le_audio_mcp_client_cfg_cccd_info_t));
}
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
uint16_t rtk_bt_le_audio_mcs_server_set_param(uint8_t service_id, uint16_t char_uuid, void *p_param)
{
	rtk_bt_le_audio_mcp_server_set_param_t info = {0};

	info.service_id = service_id;
	info.char_uuid = char_uuid;
	memcpy(&info.param, p_param, sizeof(info.param));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_MCP_SERVER_SET_PARAM,
						   (void *)&info, sizeof(rtk_bt_le_audio_mcp_server_set_param_t));
}

uint16_t rtk_bt_le_audio_mcs_server_send_data(uint8_t service_id, uint16_t char_uuid, void *p_param)
{
	rtk_bt_le_audio_mcp_server_send_data_param_t info = {0};

	if (char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME && char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE &&
		char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION && char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION &&
		char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_CHANGED) {
		return RTK_BT_ERR_UNSUPPORTED;
	}

	info.service_id = service_id;
	info.char_uuid = char_uuid;
	memcpy(&info.param, p_param, sizeof(info.param));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_MCP_SERVER_SEND_DATA,
						   (void *)&info, sizeof(rtk_bt_le_audio_mcp_server_set_param_t));
}

uint16_t rtk_bt_le_audio_mcs_server_read_confirm(uint16_t conn_handle, uint16_t cid, uint8_t service_id, uint16_t char_uuid, uint16_t offset, void *p_param)
{
	rtk_bt_le_audio_mcp_server_read_confirm_param_t info = {0};

	if (char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME && char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE &&
		char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION && char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION &&
		char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID) {
		return RTK_BT_ERR_UNSUPPORTED;
	}

	info.conn_handle = conn_handle;
	info.cid = cid;
	info.service_id = service_id;
	info.char_uuid = char_uuid;
	info.offset = offset;
	memcpy(&info.param, p_param, sizeof(info.param));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_MCP_SERVER_READ_CFM,
						   (void *)&info, sizeof(rtk_bt_le_audio_mcp_server_read_confirm_param_t));
}

#endif

#endif //end of #if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT