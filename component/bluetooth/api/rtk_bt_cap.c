/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <rtk_bt_common.h>
#include <rtk_bt_def.h>
#include <rtk_bt_device.h>
#include <rtk_bt_le_audio_def.h>
#include <rtk_bt_bap.h>
#include <rtk_bt_cap.h>
#include <rtk_bt_gattc.h>

#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
uint16_t rtk_bt_le_audio_csis_gen_rsi(uint8_t *p_sirk, uint8_t *p_rsi)
{
	rtk_bt_le_audio_csis_gen_rsi_info_t info = {0};

	memcpy(info.sirk, p_sirk, RTK_BT_LE_CSIS_SIRK_LEN);
	info.p_rsi = p_rsi;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_CSIS_GEN_RSI,
						   (void *)&info, sizeof(rtk_bt_le_audio_csis_gen_rsi_info_t));
}

uint16_t rtk_bt_le_audio_csis_update_sirk(rtk_bt_le_audio_csis_sirk_type_t sirk_type, uint8_t *p_sirk)
{
	rtk_bt_le_audio_csis_update_sirk_param_t info = {0};

	info.sirk_type = sirk_type;
	memcpy(info.sirk, p_sirk, RTK_BT_LE_CSIS_SIRK_LEN);

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_CSIS_UPDATE_SIRK,
						   (void *)&info, sizeof(rtk_bt_le_audio_csis_update_sirk_param_t));
}

uint16_t rtk_bt_le_audio_csis_update_size(uint8_t csis_size)
{
	rtk_bt_le_audio_csis_update_size_param_t info = {0};

	info.csis_size = csis_size;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_CSIS_UPDATE_SIRK,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_CHECK_ADV_RSI,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_CFG_DISCOVER,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_ADD_GROUP,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_ADD_DEVICE,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_WRITE_LOCK,
						   (void *)&info, sizeof(rtk_bt_le_audio_csis_set_coordinator_write_lock_info_t));
}
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
uint16_t rtk_bt_le_audio_vcs_set_param(rtk_bt_le_audio_vcs_param_t *param)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VCS_SET_PARAM,
						   (void *)param, sizeof(rtk_bt_le_audio_vcs_param_t));
}

uint16_t rtk_bt_le_audio_vcs_get_param(rtk_bt_le_audio_vcs_param_t *param)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VCS_GET_PARAM,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VCS_WRITE_CP,
						   (void *)&param, sizeof(rtk_bt_le_audio_vcs_write_cp_param_t));
}
uint16_t rtk_bt_le_audio_vcs_get_volume_state(uint16_t conn_handle, rtk_bt_le_audio_vcs_volume_state_t *p_vcs_volume_state)
{
	rtk_bt_le_audio_vcs_get_volume_state_param_t param = {0};

	param.conn_handle = conn_handle;
	param.p_vcs_volume_state = p_vcs_volume_state;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VCS_GET_VOLUME_STATE,
						   (void *)&param, sizeof(rtk_bt_le_audio_vcs_get_volume_state_param_t));
}
uint16_t rtk_bt_le_audio_vcs_get_volume_flags(uint16_t conn_handle, uint8_t *p_volume_flags)
{
	uint16_t ret = RTK_BT_FAIL;
	rtk_bt_le_audio_vcs_get_volume_flags_param_t param = {0};

	if (!p_volume_flags) {
		return RTK_BT_ERR_PARAM_INVALID;
	}
	param.conn_handle = conn_handle;
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VCS_GET_VOLUME_FLAGS,
						  (void *)&param, sizeof(rtk_bt_le_audio_vcs_get_volume_flags_param_t));
	if (!ret) {
		*p_volume_flags = param.volume_flags;
	}

	return ret;
}
uint16_t rtk_bt_le_audio_vcs_change_mute(uint8_t group_idx, uint8_t mute)
{
	rtk_bt_le_audio_vcs_change_mute_param_t param = {0};

	param.group_idx = group_idx;
	param.mute = mute;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VCS_CHANGE_MUTE,
						   (void *)&param, sizeof(rtk_bt_le_audio_vcs_change_mute_param_t));
}
uint16_t rtk_bt_le_audio_vcs_change_volume(uint8_t group_idx, uint8_t volume_setting)
{
	rtk_bt_le_audio_vcs_change_volume_param_t param = {0};

	param.group_idx = group_idx;
	param.volume_setting = volume_setting;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VCS_CHANGE_VOLUME,
						   (void *)&param, sizeof(rtk_bt_le_audio_vcs_change_volume_param_t));
}
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
uint16_t rtk_bt_le_audio_mics_change_mute(uint8_t group_idx, rtk_bt_le_audio_mics_mute_state_t mic_mute)
{
	rtk_bt_le_audio_mics_change_mute_param_t param = {0};

	param.group_idx = group_idx;
	param.mic_mute = mic_mute;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_MICS_CHANGE_MUTE,
						   (void *)&param, sizeof(rtk_bt_le_audio_mics_change_mute_param_t));
}
uint16_t rtk_bt_le_audio_mics_set_mute_value(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t mic_mute)
{
	rtk_bt_le_audio_mics_set_mute_value_param_t param = {0};

	param.conn_handle = conn_handle;
	param.mic_mute = mic_mute;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_MICS_SET_MUTE_VALUE,
						   (void *)&param, sizeof(rtk_bt_le_audio_mics_set_mute_value_param_t));
}

uint16_t rtk_bt_le_audio_mics_get_mute_value(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t *p_mic_mute)
{
	rtk_bt_le_audio_mics_get_mute_value_param_t param = {0};

	param.conn_handle = conn_handle;
	param.p_mic_mute = p_mic_mute;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_MICS_GET_MUTE_VALUE,
						   (void *)&param, sizeof(rtk_bt_le_audio_mics_get_mute_value_param_t));
}
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
uint16_t rtk_bt_le_audio_mics_set_param(rtk_bt_le_audio_mics_param_t *param)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_MICS_SET_PARAM,
						   (void *)param, sizeof(rtk_bt_le_audio_mics_param_t));
}

uint16_t rtk_bt_le_audio_mics_get_param(rtk_bt_le_audio_mics_param_t *param)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_MICS_GET_PARAM,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VOCS_SET_PARAM,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VOCS_GET_PARAM,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_get_param_t));
}

uint16_t rtk_bt_le_audio_vocs_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t cfg_flags, bool enable)
{
	rtk_bt_le_audio_vocs_cfg_cccd_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.cfg_flags = cfg_flags;
	param.enable = enable;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VOCS_CFG_CCCD,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_cfg_cccd_param_t));
}

uint16_t rtk_bt_le_audio_vocs_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_char_type_t type)
{
	rtk_bt_le_audio_vocs_read_char_value_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.type = type;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VOCS_READ_CHAR_VALUE,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_CP,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_write_cp_param_t));
}

uint16_t rtk_bt_le_audio_vocs_write_cp_by_group(uint8_t group_idx, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_cp_op_t cp_op,
												rtk_bt_le_audio_vocs_cp_param_t cp_param)
{
	rtk_bt_le_audio_vocs_write_cp_by_group_param_t param = {0};

	param.group_idx = group_idx;
	param.srv_instance_id = srv_instance_id;
	param.cp_op = cp_op;
	memcpy(&param.cp_param, &cp_param, sizeof(rtk_bt_le_audio_vocs_cp_param_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_CP_BY_GROUP,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_write_cp_by_group_param_t));
}
uint16_t rtk_bt_le_audio_vocs_write_audio_location(uint16_t conn_handle, uint8_t srv_instance_id, uint32_t audio_location)
{
	rtk_bt_le_audio_vocs_write_audio_location_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.audio_location = audio_location;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_AUDIO_LOCATION,
						   (void *)&param, sizeof(rtk_bt_le_audio_vocs_write_audio_location_param_t));
}
uint16_t rtk_bt_le_audio_vocs_write_output_des(uint16_t conn_handle, uint8_t srv_instance_id, uint16_t output_des_len, uint8_t *p_output_des)
{
	rtk_bt_le_audio_vocs_write_output_des_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.output_des_len = output_des_len;
	param.p_output_des = p_output_des;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_OUTPUT_DES,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_VOCS_GET_SRV_DATA,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_AICS_SET_PARAM,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_AICS_GET_PARAM,
						   (void *)&param, sizeof(rtk_bt_le_audio_aics_get_param_t));
}

uint16_t rtk_bt_le_audio_aics_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t cfg_flags, bool enable)
{
	rtk_bt_le_audio_aics_cfg_cccd_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.cfg_flags = cfg_flags;
	param.enable = enable;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_AICS_CFG_CCCD,
						   (void *)&param, sizeof(rtk_bt_le_audio_aics_cfg_cccd_param_t));
}

uint16_t rtk_bt_le_audio_aics_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_aics_char_type_t type)
{
	rtk_bt_le_audio_aics_read_char_value_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.type = type;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_AICS_READ_CHAR_VALUE,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_AICS_WRITE_CP,
						   (void *)&param, sizeof(rtk_bt_le_audio_aics_write_cp_param_t));
}

uint16_t rtk_bt_le_audio_aics_write_cp_by_group(uint8_t group_idx, uint8_t srv_instance_id, rtk_bt_le_audio_aics_cp_op_t cp_op,
												rtk_bt_le_audio_aics_cp_param_t cp_param)
{
	rtk_bt_le_audio_aics_write_cp_by_group_param_t param = {0};

	param.group_idx = group_idx;
	param.srv_instance_id = srv_instance_id;
	param.cp_op = cp_op;
	memcpy(&param.cp_param, &cp_param, sizeof(rtk_bt_le_audio_aics_cp_param_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_AICS_WRITE_CP_BY_GROUP,
						   (void *)&param, sizeof(rtk_bt_le_audio_aics_write_cp_by_group_param_t));
}

uint16_t rtk_bt_le_audio_aics_write_input_des(uint16_t conn_handle, uint8_t srv_instance_id, uint16_t input_des_len, uint8_t *p_input_des)
{
	rtk_bt_le_audio_aics_write_input_des_param_t param = {0};

	param.conn_handle = conn_handle;
	param.srv_instance_id = srv_instance_id;
	param.input_des_len = input_des_len;
	param.p_input_des = p_input_des;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_AICS_WRITE_INPUT_DES,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_AICS_GET_SRV_DATA,
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


	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_WRITE_MEDIA_CP,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_READ_CHAR_VALUE,
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_CFG_CCCD,
						   (void *)&info, sizeof(rtk_bt_le_audio_mcp_client_cfg_cccd_info_t));
}
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
uint16_t rtk_bt_le_audio_mcs_server_send_data(uint16_t char_uuid, void *p_param)
{
	rtk_bt_le_audio_mcp_server_send_data_param_t info = {0};

	if (char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME && char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE &&
		char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION && char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION &&
		char_uuid != RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_CHANGED) {
		return RTK_BT_ERR_UNSUPPORTED;
	}

	info.char_uuid = char_uuid;
	memcpy(&info.param, p_param, sizeof(info.param));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_MCP_SERVER_SEND_DATA,
						   (void *)&info, sizeof(rtk_bt_le_audio_mcp_server_send_data_param_t));
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

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_MCP_SERVER_READ_CFM,
						   (void *)&info, sizeof(rtk_bt_le_audio_mcp_server_read_confirm_param_t));
}

#endif

uint16_t rtk_bt_cap_param_config(rtk_bt_le_audio_cap_param_config_t *p_param)
{
	if (!p_param) {
		BT_LOGE("%s input param fail \r\n", __func__);
		return RTK_BT_FAIL;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_ACT_CAP_PARAM_CONFIG,
						   &p_param, sizeof(rtk_bt_le_audio_cap_param_config_t));
}

#endif