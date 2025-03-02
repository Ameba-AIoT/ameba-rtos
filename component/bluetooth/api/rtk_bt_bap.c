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

uint16_t rtk_bt_le_audio_link_alloc(rtk_bt_le_conn_ind_t *conn_ind)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_LINK_ALLOCATE,
						   conn_ind, sizeof(rtk_bt_le_conn_ind_t));
}

uint16_t rtk_bt_le_audio_link_free(rtk_bt_le_disconn_ind_t *disconn_ind)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_LINK_FREE,
						   disconn_ind, sizeof(rtk_bt_le_disconn_ind_t));
}

uint16_t rtk_bt_le_audio_iso_data_send(rtk_bt_le_audio_iso_data_send_info_t *info)
{
	if (!info || !info->iso_conn_handle || !info->data_len || !info->p_data) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_ISO_DATA_SEND,
						   (void *)info, sizeof(rtk_bt_le_audio_iso_data_send_info_t));
}

uint16_t rtk_bt_le_audio_broadcast_source_create(rtk_bt_le_audio_codec_cfg_item_t cfg_codec_index, rtk_bt_le_audio_qos_cfg_type_t cfg_qos_type,
												 rtk_bt_le_addr_type_t local_addr_type, bool encryption, uint16_t stream_audio_contexts)
{
	rtk_bt_le_audio_broadcast_source_create_param_t param = {0};

	param.cfg_codec_index = cfg_codec_index;
	param.cfg_qos_type = cfg_qos_type;
	param.local_addr_type = local_addr_type;
	param.encryption = encryption;
	param.stream_audio_contexts = stream_audio_contexts;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_CREATE,
						   (void *)&param, sizeof(rtk_bt_le_audio_broadcast_source_create_param_t));
}

uint16_t rtk_bt_le_audio_broadcast_source_start(void)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_START,
						   NULL, 0);
}

uint16_t rtk_bt_le_audio_broadcast_source_stop(void)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_STOP,
						   NULL, 0);
}

uint16_t rtk_bt_le_audio_ext_scan_act(bool enable)
{

	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_EXT_SCAN_ACT,
						   (void *)&enable, sizeof(bool));
}

uint16_t rtk_bt_le_audio_broadcast_pa_sync_create(rtk_bt_le_addr_t addr, uint8_t adv_sid, uint8_t *broadcast_id)
{
	rtk_bt_le_audio_broadcast_sync_create_param_t param = {0};

	if (broadcast_id == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}
	param.adv_sid = adv_sid;
	memcpy(&param.adv_addr, &addr, sizeof(rtk_bt_le_addr_t));
	memcpy(param.broadcast_id, broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);

	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_BROADCAST_PA_SYNC_CREATE,
						   (void *)&param, sizeof(rtk_bt_le_audio_broadcast_sync_create_param_t));
}

uint16_t rtk_bt_le_audio_broadcast_pa_sync_terminate(rtk_bt_le_addr_t addr)
{
	rtk_bt_le_addr_t param = {0};
	memcpy(&param, &addr, sizeof(rtk_bt_le_addr_t));

	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_BROADCAST_PA_SYNC_TERMINATE,
						   (void *)&param, sizeof(rtk_bt_le_addr_t));
}

uint16_t rtk_bt_le_audio_broadcast_big_sync_create(rtk_bt_le_addr_t addr)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_CREATE,
						   (void *)&addr, sizeof(rtk_bt_le_addr_t));
}

uint16_t rtk_bt_le_audio_broadcast_big_sync_terminate(rtk_bt_le_addr_t addr)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_TERMINATE,
						   (void *)&addr, sizeof(rtk_bt_le_addr_t));
}

uint16_t rtk_bt_le_audio_broadcast_assistant_remote_ext_scan_act(bool enable, uint8_t group_idx)
{
	rtk_bt_le_audio_broadcast_assistant_remote_scan_param_t param = {0};
	param.enable = enable;
	param.group_idx = group_idx;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_REMOTE_SCAN_ACT,
						   (void *)&param, sizeof(rtk_bt_le_audio_broadcast_assistant_remote_scan_param_t));
}

uint16_t rtk_bt_le_audio_broadcast_assistant_reception_start(rtk_bt_le_addr_t addr, uint8_t group_idx)
{
	rtk_bt_le_audio_broadcast_assistant_reception_act_param_t param = {0};

	memcpy(&param.addr, &addr, sizeof(rtk_bt_le_addr_t));
	param.group_idx = group_idx;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_RECEPTION_START,
						   (void *)&param, sizeof(rtk_bt_le_audio_broadcast_assistant_reception_act_param_t));
}

uint16_t rtk_bt_le_audio_broadcast_assistant_reception_stop(rtk_bt_le_addr_t addr, uint8_t group_idx)
{
	rtk_bt_le_audio_broadcast_assistant_reception_act_param_t param = {0};

	memcpy(&param.addr, &addr, sizeof(rtk_bt_le_addr_t));
	param.group_idx = group_idx;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_RECEPTION_STOP,
						   (void *)&param, sizeof(rtk_bt_le_audio_broadcast_assistant_reception_act_param_t));
}

uint16_t rtk_bt_le_audio_broadcast_assistant_reception_remove(rtk_bt_le_addr_t addr, uint8_t group_idx)
{
	rtk_bt_le_audio_broadcast_assistant_reception_act_param_t param = {0};

	memcpy(&param.addr, &addr, sizeof(rtk_bt_le_addr_t));
	param.group_idx = group_idx;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_RECEPTION_REMOVE,
						   (void *)&param, sizeof(rtk_bt_le_audio_broadcast_assistant_reception_act_param_t));
}

uint16_t rtk_bt_le_audio_group_allocate(rtk_bt_le_audio_group_handle_t *p_group_handle)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_GROUP_ALLOCATE,
						   p_group_handle, sizeof(rtk_bt_le_audio_group_handle_t));
}

uint16_t rtk_bt_le_audio_group_release(rtk_bt_le_audio_group_handle_t group_handle)
{
	if (group_handle == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_GROUP_RELEASE,
						   (void *)&group_handle, sizeof(rtk_bt_le_audio_group_handle_t));
}

uint16_t rtk_bt_bap_unicast_client_start(uint8_t group_idx, uint16_t sink_context, uint16_t source_context)
{
	rtk_bt_le_audio_unicast_client_start_t start_t = {0};

	start_t.group_idx = group_idx;
	start_t.sink_context = sink_context;
	start_t.source_context = source_context;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_CLIENT_START,
						   &start_t, sizeof(rtk_bt_le_audio_unicast_client_start_t));
}

uint16_t rtk_bt_bap_unicast_client_stop(uint8_t group_idx, uint32_t cis_timeout_ms)
{
	rtk_bt_le_audio_unicast_client_stop_t stop_t = {0};

	stop_t.group_idx = group_idx;
	stop_t.cis_timeout_ms = cis_timeout_ms;
	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_CLIENT_STOP,
						   &stop_t, sizeof(rtk_bt_le_audio_unicast_client_stop_t));
}

uint16_t rtk_bt_bap_unicast_client_release(uint8_t group_idx)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_CLIENT_RELEASE,
						   &group_idx, sizeof(uint8_t));
}

uint16_t rtk_bt_bap_param_config(rtk_bt_le_audio_bap_param_config_t *p_param)
{
	if (!p_param) {
		BT_LOGE("%s input param fail \r\n", __func__);
		return RTK_BT_FAIL;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_ACT_BAP_PARAM_CONFIG,
						   &p_param, sizeof(rtk_bt_le_audio_bap_param_config_t));
}

#endif
