/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <string.h>
#include <stdio.h>
#include <bt_api_config.h>
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#include <osif.h>
#include <ble_audio_flags.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_stack_le_audio_internal.h>
#include <rtk_stack_config.h>
#include <rtk_bt_le_audio_def.h>
#include <rtk_bt_cap.h>
#include <trace_app.h>
#include <bt_direct_msg.h>
#include <gap_iso_data.h>
#include <gap.h>
#include <bap.h>
#include <bap_unicast_client.h>
#include <cap.h>
#include <ble_audio.h>
#include <pacs_mgr.h>
#include <ascs_mgr.h>
#include <bass_mgr.h>
#include <bass_client.h>
#include <csis_mgr.h>
#include <csis_client.h>
#include <mcp_client.h>
#include <mcp_mgr.h>
#include <ble_mgr.h>
#include <metadata_def.h>
#include <ble_audio_sync.h>
#include <broadcast_source_sm.h>
#include <base_data_generate.h>
#include <vocs_mgr.h>
#include <vocs_client.h>
#include <vcs_def.h>
#include <vcs_mgr.h>
#include <vcs_client.h>
#include <mics_mgr.h>
#include <mics_client.h>
#include <aics_def.h>
#include <aics_mgr.h>
#include <aics_client.h>
#include <bt_gatt_svc.h>
#include <bt_gatt_client.h>
#include <tmas_mgr.h>
#include <tmas_client.h>
#include <tmas_def.h>
#include <gmas_mgr.h>
#include <gmas_client.h>
#include <gmas_def.h>

static uint8_t stack_cap_init_flag = 0;
extern rtk_bt_le_audio_priv_t bt_le_audio_priv_data;
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
const uint8_t default_csis_sirk[RTK_BT_LE_CSIS_SIRK_LEN] = {0x11, 0x22, 0x33, 0xc6, 0xaf, 0xbb, 0x65, 0xa2, 0x5a, 0x41, 0xf1, 0x53, 0x05, 0x68, 0x8e, 0x83};
static rtk_bt_le_audio_csis_sirk_type_t default_csis_sirk_type = RTK_BT_LE_CSIS_SIRK_PLN;
uint16_t default_csis_size = RTK_BT_LE_CSIS_SIRK_LEN;
#endif

extern bool rtk_bt_check_evt_cb_direct_calling(uint8_t group, uint8_t evt_code);

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
static uint16_t rtk_stack_mcp_client_read_result_handle(T_MCP_CLIENT_READ_RESULT *p_read_result)
{
	rtk_bt_evt_t *p_evt = NULL;
	uint16_t data_len = 0;
	uint8_t  *data_ptr =  NULL;
	rtk_bt_le_audio_mcp_client_read_result_ind_t *p_read_result_ind = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
								RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_READ_RESULT_IND,
								sizeof(rtk_bt_le_audio_mcp_client_read_result_ind_t));
	if (!p_evt) {
		BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
		return RTK_BT_FAIL;
	}

	p_read_result_ind = (rtk_bt_le_audio_mcp_client_read_result_ind_t *)p_evt->data;
	p_read_result_ind->cause = p_read_result->cause;
	p_read_result_ind->conn_handle = p_read_result->conn_handle;
	p_read_result_ind->general_mcs = p_read_result->gmcs;
	p_read_result_ind->srv_instance_id = p_read_result->srv_instance_id;
	p_read_result_ind->char_uuid = p_read_result->char_uuid;
	BT_LOGD("LE_AUDIO_MSG_MCP_CLIENT_READ_RESULT char_uuid=0x%x\r\n", p_read_result->char_uuid);
	switch (p_read_result->char_uuid) {
	case MCS_UUID_CHAR_MEDIA_PLAYER_NAME:
		data_len = p_read_result->data.media_player_name.media_player_name_len + 1;
		data_ptr = p_read_result->data.media_player_name.p_media_player_name;
		break;
	case MCS_UUID_CHAR_TRACK_TITLE:
		data_len = p_read_result->data.track_title.track_title_len + 1;
		data_ptr = p_read_result->data.track_title.p_track_title;
		break;
	default:
		break;
	}
	if (data_len) {
		p_evt->user_data = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, data_len);
		if (!p_evt->user_data) {
			BT_LOGE("p_evt->user_data alloc fail, len = %d\r\n", data_len);
			return RTK_BT_ERR_NO_MEMORY;
		}
		memset(p_evt->user_data, 0, data_len);
		memcpy((void *)p_evt->user_data, (void *)data_ptr, data_len - 1);
	}
	switch (p_read_result->char_uuid) {
	case MCS_UUID_CHAR_MEDIA_PLAYER_NAME:
		if (data_len != 0) {
			APP_PRINT_INFO1("rtk_stack_mcp_client_read_result_handle: player_name %s", TRACE_STRING(data_ptr));
			BT_LOGD("rtk_stack_mcp_client_read_result_handle: player_name %s\r\n", data_ptr);
			p_read_result_ind->data.media_player_name.p_media_player_name = p_evt->user_data;
			p_read_result_ind->data.media_player_name.media_player_name_len = data_len;
		}
		break;
	case MCS_UUID_CHAR_TRACK_TITLE:
		if (data_len != 0) {
			APP_PRINT_INFO1("rtk_stack_mcp_client_read_result_handle: title_url %s", TRACE_STRING(data_ptr));
			BT_LOGD("rtk_stack_mcp_client_read_result_handle: title_url %s\r\n", data_ptr);
			p_read_result_ind->data.track_title.p_track_title = p_evt->user_data;
			p_read_result_ind->data.track_title.track_title_len = data_len;
		}
		break;
	default:
		memcpy(&p_read_result_ind->data, &p_read_result->data, sizeof(p_read_result->data));
		break;
	}
	/* Send event */
	rtk_bt_evt_indicate(p_evt, NULL);

	return RTK_BT_OK;
}

static uint16_t rtk_stack_mcp_client_notify_handle(T_MCP_CLIENT_NOTIFY *p_notify_result)
{
	rtk_bt_evt_t *p_evt = NULL;
	uint16_t data_len = 0;
	uint8_t  *data_ptr =  NULL;
	rtk_bt_le_audio_mcp_client_notify_ind_t *p_notify_result_ind = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
								RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_NOTIFY_IND,
								sizeof(rtk_bt_le_audio_mcp_client_notify_ind_t));
	if (!p_evt) {
		BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
		return RTK_BT_FAIL;
	}
	p_notify_result_ind = (rtk_bt_le_audio_mcp_client_notify_ind_t *)p_evt->data;
	p_notify_result_ind->conn_handle = p_notify_result->conn_handle;
	p_notify_result_ind->general_mcs = p_notify_result->gmcs;
	p_notify_result_ind->srv_instance_id = p_notify_result->srv_instance_id;
	p_notify_result_ind->char_uuid = p_notify_result->char_uuid;
	BT_LOGD("LE_AUDIO_MSG_MCP_CLIENT_NOTIFY char_uuid=0x%x\r\n", p_notify_result->char_uuid);
	switch (p_notify_result->char_uuid) {
	case MCS_UUID_CHAR_MEDIA_PLAYER_NAME:
		data_len = p_notify_result->data.media_player_name.media_player_name_len + 1;
		data_ptr = p_notify_result->data.media_player_name.p_media_player_name;
		break;
	case MCS_UUID_CHAR_TRACK_TITLE:
		data_len = p_notify_result->data.track_title.track_title_len + 1;
		data_ptr = p_notify_result->data.track_title.p_track_title;
		break;
	default:
		break;
	}
	if (data_len) {
		p_evt->user_data = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, data_len);
		if (!p_evt->user_data) {
			BT_LOGE("p_evt->user_data alloc fail, len = %d\r\n", data_len);
			return RTK_BT_ERR_NO_MEMORY;
		}
		memset(p_evt->user_data, 0, data_len);
		memcpy((void *)p_evt->user_data, (void *)data_ptr, data_len - 1);
	}
	switch (p_notify_result->char_uuid) {
	case MCS_UUID_CHAR_MEDIA_PLAYER_NAME:
		if (data_len != 0) {
			APP_PRINT_INFO1("rtk_stack_mcs_client_read_result_handle: player_name %s", TRACE_STRING(data_ptr));
			BT_LOGD("rtk_stack_mcs_client_read_result_handle: player_name %s\r\n", data_ptr);
			p_notify_result_ind->data.media_player_name.p_media_player_name = p_evt->user_data;
			p_notify_result_ind->data.media_player_name.media_player_name_len = data_len;
		}
		break;
	case MCS_UUID_CHAR_TRACK_TITLE:
		if (data_len != 0) {
			APP_PRINT_INFO1("rtk_stack_mcs_client_read_result_handle: title_url %s", TRACE_STRING(data_ptr));
			BT_LOGD("rtk_stack_mcs_client_read_result_handle: title_url %s\r\n", data_ptr);
			p_notify_result_ind->data.track_title.p_track_title = p_evt->user_data;
			p_notify_result_ind->data.track_title.track_title_len = data_len;
		}
		break;
	default:
		memcpy(&p_notify_result_ind->data, &p_notify_result->data, sizeof(p_notify_result->data));
		break;
	}
	/* Send event */
	rtk_bt_evt_indicate(p_evt, NULL);

	return RTK_BT_OK;
}
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
static bool bt_stack_mcp_handle_operation(T_MCP_SERVER_WRITE_MEDIA_CP_IND *p_mcp_req)
{
	uint8_t opcode = p_mcp_req->opcode;
	T_MCP_SERVER_SET_PARAM set_param = {0};

	BT_LOGD("MCP CP: opcode 0x%x, curr media state %d\r\n", opcode, bt_le_audio_priv_data.mcp.media_state);
	if (opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PLAY) {
		if (bt_le_audio_priv_data.mcp.media_state == MCS_MEDIA_STATE_SEEKING ||
			bt_le_audio_priv_data.mcp.media_state == MCS_MEDIA_STATE_PAUSED) {
			bt_le_audio_priv_data.mcp.media_state = MCS_MEDIA_STATE_PLAYING;
			set_param.char_uuid = MCS_UUID_CHAR_MEDIA_STATE;
			set_param.param.media_state = MCS_MEDIA_STATE_PLAYING;
			mcp_server_set_param(bt_le_audio_priv_data.mcp.gmcs_id, &set_param);
		}
	} else if (opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE) {
		if (bt_le_audio_priv_data.mcp.media_state == MCS_MEDIA_STATE_SEEKING ||
			bt_le_audio_priv_data.mcp.media_state == MCS_MEDIA_STATE_PLAYING) {
			bt_le_audio_priv_data.mcp.media_state = MCS_MEDIA_STATE_PAUSED;
			set_param.char_uuid = MCS_UUID_CHAR_MEDIA_STATE;
			set_param.param.media_state = MCS_MEDIA_STATE_PAUSED;
			mcp_server_set_param(bt_le_audio_priv_data.mcp.gmcs_id, &set_param);
		}
	} else if (opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_STOP) {
		if (bt_le_audio_priv_data.mcp.media_state != MCS_MEDIA_STATE_INACTIVE) {
			bt_le_audio_priv_data.mcp.media_state = MCS_MEDIA_STATE_PAUSED;
			set_param.char_uuid = MCS_UUID_CHAR_MEDIA_STATE;
			set_param.param.media_state = MCS_MEDIA_STATE_PAUSED;
			mcp_server_set_param(bt_le_audio_priv_data.mcp.gmcs_id, &set_param);
		}
	} else if (opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FAST_FORWARD) {
		bt_le_audio_priv_data.mcp.media_state = MCS_MEDIA_STATE_SEEKING;
		set_param.char_uuid = MCS_UUID_CHAR_MEDIA_STATE;
		set_param.param.media_state = MCS_MEDIA_STATE_SEEKING;
		mcp_server_set_param(bt_le_audio_priv_data.mcp.gmcs_id, &set_param);
	} else if (opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FAST_REWIND) {
		bt_le_audio_priv_data.mcp.media_state = MCS_MEDIA_STATE_SEEKING;
		set_param.char_uuid = MCS_UUID_CHAR_MEDIA_STATE;
		set_param.param.media_state = MCS_MEDIA_STATE_SEEKING;
		mcp_server_set_param(bt_le_audio_priv_data.mcp.gmcs_id, &set_param);
	} else if (opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PREVIOUS_TRACK) {
		T_MCP_SERVER_SEND_DATA_PARAM send_data_param = {0};
		send_data_param.char_uuid = MCS_UUID_CHAR_TRACK_CHANGED;
		mcp_server_send_data(bt_le_audio_priv_data.mcp.gmcs_id, &send_data_param);
	} else if (opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_NEXT_TRACK) {
		T_MCP_SERVER_SEND_DATA_PARAM send_data_param = {0};
		send_data_param.char_uuid = MCS_UUID_CHAR_TRACK_CHANGED;
		mcp_server_send_data(bt_le_audio_priv_data.mcp.gmcs_id, &send_data_param);
	}
	return true;
}
#endif

extern void bt_stack_le_audio_group_cb(T_AUDIO_GROUP_MSG msg, T_BLE_AUDIO_GROUP_HANDLE handle, void *buf);

uint16_t rtk_stack_le_audio_cap_msg_cback(T_LE_AUDIO_MSG msg, void *buf)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_audio_link_t *p_link = NULL;
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;
#endif
	uint16_t app_result = BLE_AUDIO_CB_RESULT_SUCCESS;

	switch (msg) {

	case LE_AUDIO_MSG_CAP_DIS_DONE: {
		T_CAP_DIS_DONE *p_data = (T_CAP_DIS_DONE *)buf;
		APP_PRINT_INFO6("LE_AUDIO_MSG_CAP_DIS_DONE: conn_handle 0x%x, load_from_ftl %d, cas_is_found %d, cas_inc_csis %d, vcs_is_found %d, mics_is_found %d",
						p_data->conn_handle, p_data->load_from_ftl, p_data->cas_is_found, p_data->cas_inc_csis, p_data->vcs_is_found, p_data->mics_is_found);
		BT_LOGD(
			"LE_AUDIO_MSG_CAP_DIS_DONE: conn_handle 0x%x, load_from_ftl %d, cas_is_found %d, cas_inc_csis %d, vcs_is_found %d, mics_is_found %d\r\n",
			p_data->conn_handle, p_data->load_from_ftl, p_data->cas_is_found, p_data->cas_inc_csis, p_data->vcs_is_found, p_data->mics_is_found);
		p_link = bt_stack_le_audio_find_link_by_conn_handle(p_data->conn_handle);
		if (p_link) {
			if (p_data->cas_is_found) {
				p_link->lea_disc_flags |= RTK_BT_LE_AUDIO_LINK_DISC_CAS_DONE;
				p_link->lea_srv_found_flags |= RTK_BT_LE_AUDIO_LINK_DISC_CAS_DONE;
				if (p_data->cas_inc_csis) {
					p_link->lea_srv_found_flags |= RTK_BT_LE_AUDIO_LINK_DISC_CSIS_DONE;
				}
			}
		} else {
			BT_LOGE("[CAP] LE_AUDIO_MSG_CAP_DIS_DONE link not found \r\n");
			break;
		}
		rtk_bt_le_audio_cap_discovery_done_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_CAP_DISCOVERY_DONE_IND,
									sizeof(rtk_bt_le_audio_cap_discovery_done_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_cap_discovery_done_ind_t *)p_evt->data;
		p_ind->conn_handle = p_data->conn_handle;
		p_ind->load_from_ftl = p_data->load_from_ftl;
		p_ind->cas_is_found = p_data->cas_is_found;
		p_ind->cas_inc_csis = p_data->cas_inc_csis;
		p_ind->vcs_is_found = p_data->vcs_is_found;
		p_ind->mics_is_found = p_data->mics_is_found;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_CLIENT_ATTR_CCCD_INFO: {
		T_CLIENT_ATTR_CCCD_INFO *cccd_info = (T_CLIENT_ATTR_CCCD_INFO *)buf;
		APP_PRINT_INFO8("LE_AUDIO_MSG_CLIENT_ATTR_CCCD_INFO: conn_handle %d,srv_cfg %d,srv_instance_id 0x%x,srv_uuid 0x%x,char_instance_id 0x%x,char_uuid 0x%x,cccd_cfg 0x%x,cause 0x%x",
						cccd_info->conn_handle, cccd_info->srv_cfg, cccd_info->srv_instance_id, cccd_info->srv_uuid, cccd_info->char_instance_id,
						cccd_info->char_uuid, cccd_info->cccd_cfg, cccd_info->cause);
		BT_LOGD(
			"LE_AUDIO_MSG_CLIENT_ATTR_CCCD_INFO: conn_handle %d,srv_cfg %d,srv_instance_id 0x%x,srv_uuid 0x%x,char_instance_id 0x%x,char_uuid 0x%x,cccd_cfg 0x%x,cause 0x%x\r\n",
			cccd_info->conn_handle, cccd_info->srv_cfg, cccd_info->srv_instance_id, cccd_info->srv_uuid, cccd_info->char_instance_id,
			cccd_info->char_uuid, cccd_info->cccd_cfg, cccd_info->cause);
		if (cccd_info->cause) {
			BT_LOGE(
				"LE_AUDIO_MSG_CLIENT_ATTR_CCCD_INFO: conn_handle %d,srv_cfg %d,srv_instance_id 0x%x,srv_uuid 0x%x,char_instance_id 0x%x,char_uuid 0x%x,cccd_cfg 0x%x,cause 0x%x\r\n",
				cccd_info->conn_handle, cccd_info->srv_cfg, cccd_info->srv_instance_id, cccd_info->srv_uuid, cccd_info->char_instance_id,
				cccd_info->char_uuid, cccd_info->cccd_cfg, cccd_info->cause);
		}
	}
	break;

#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
	case LE_AUDIO_MSG_CSIS_CLIENT_DIS_DONE: {
		T_CSIS_CLIENT_DIS_DONE *p_dis_done = (T_CSIS_CLIENT_DIS_DONE *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_CSIS_CLIENT_DIS_DONE: conn_handle 0x%x, is_found %d, load_from_ftl %d, srv_num %d",
						p_dis_done->conn_handle, p_dis_done->is_found, p_dis_done->load_from_ftl, p_dis_done->srv_num);
		BT_LOGA("LE_AUDIO_MSG_CSIS_CLIENT_DIS_DONE: conn_handle 0x%x, is_found %d, load_from_ftl %d, srv_num %d \r\n",
				p_dis_done->conn_handle, p_dis_done->is_found, p_dis_done->load_from_ftl, p_dis_done->srv_num);
		if (p_dis_done->is_found) {
			break;
		}
		p_group_info = bt_stack_le_audio_find_group_by_conn_handle(p_dis_done->conn_handle);
		if (p_group_info) {
			BT_LOGE("[CAP] %s connection 0x%x(group handle 0x%x) has already in group list \r\n", __func__, p_dis_done->conn_handle, p_group_info->group_handle);
			break;
		}
		/* allocate new group */
		{
			rtk_bt_le_audio_group_handle_t group_handle = NULL;
			rtk_bt_le_audio_device_handle_t device_handle = NULL;
			uint8_t conn_id = 0;
			rtk_bt_le_addr_type_t addr_type = 0;
			uint8_t addr_val[RTK_BD_ADDR_LEN] = {0};
			group_handle = ble_audio_group_allocate();
			if (group_handle == NULL) {
				BT_LOGE("%s fail: ble_audio_group_allocate fail\r\n", __func__);
				break;
			}
			if (false == le_get_conn_id_by_handle(p_dis_done->conn_handle, &conn_id)) {
				BT_LOGE("%s fail: le_get_conn_id_by_handle fail\r\n", __func__);
				break;
			}
			if (false == le_get_conn_addr(conn_id, addr_val, (uint8_t *)&addr_type)) {
				BT_LOGE("%s fail: le_get_conn_addr fail\r\n", __func__);
				break;
			}
			if (false == ble_audio_group_reg_cb(group_handle, bt_stack_le_audio_group_cb)) {
				BT_LOGE("%s fail: ble_audio_group_reg_cb fail\r\n", __func__);
				break;
			}
			device_handle = ble_audio_group_add_dev(group_handle, addr_val, addr_type);
			if (device_handle == NULL) {
				BT_LOGE("%s fail: ble_audio_group_add_dev fail\r\n", __func__);
				break;
			}
			BT_LOGA("[CAP] %s: add device in group OK (group_handle=%08x, device_handle=%08x) \r\n", __func__, group_handle, device_handle);
			p_group_info = bt_stack_le_audio_add_group(group_handle, 0);
			/* get prefer codec cfg and prefer qos */
			if (p_group_info) {
				p_group_info->lea_unicast.audio_cfg_type = RTK_BT_LE_AUDIO_DEFAULT_UNICAST_AUDIO_CFG;
				p_group_info->lea_unicast.codec_cfg_item = RTK_BT_LE_AUDIO_DEFAULT_CODEC_CFG_ITEM;
				p_group_info->lea_unicast.qos_cfg_type = RTK_BT_LE_AUDIO_DEFAULT_QOS_CFG_TYPE;
#if defined(RTK_BT_LE_AUDIO_CIG_ISO_INTERVAL_CONFIG) && (RTK_BT_LE_AUDIO_CIG_ISO_INTERVAL_CONFIG == RTK_BT_ISO_INTERVAL_20_MS)
				p_group_info->lea_unicast.qos_cfg_type = RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY;
#endif
				if (false == qos_preferred_cfg_get((T_CODEC_CFG_ITEM)p_group_info->lea_unicast.codec_cfg_item,
												   (T_QOS_CFG_TYPE)p_group_info->lea_unicast.qos_cfg_type,
												   (T_QOS_CFG_PREFERRED *)&p_group_info->lea_unicast.preferred_qos)) {
					BT_LOGE("%s qos_preferred_cfg_get fail\r\n", __func__);
				} else {
					p_group_info->lea_unicast.session_qos.framing = p_group_info->lea_unicast.preferred_qos.framing;
					p_group_info->lea_unicast.session_qos.sdu_interval_m_s = p_group_info->lea_unicast.preferred_qos.sdu_interval;
					p_group_info->lea_unicast.session_qos.sdu_interval_s_m = p_group_info->lea_unicast.preferred_qos.sdu_interval;
					p_group_info->lea_unicast.session_qos.latency_m_s = p_group_info->lea_unicast.preferred_qos.max_transport_latency;
					p_group_info->lea_unicast.session_qos.latency_s_m = p_group_info->lea_unicast.preferred_qos.max_transport_latency;
					p_group_info->lea_unicast.session_qos.sink_presentation_delay = p_group_info->lea_unicast.preferred_qos.presentation_delay;
					p_group_info->lea_unicast.session_qos.source_presentation_delay = p_group_info->lea_unicast.preferred_qos.presentation_delay;
				}
			} else {
				BT_LOGE("%s fail: bt_stack_le_audio_add_group fail\r\n", __func__);
				break;
			}
		}
		rtk_bt_le_audio_csis_client_discovery_done_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_DISCOVERY_DONE_IND,
									sizeof(rtk_bt_le_audio_csis_client_discovery_done_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_csis_client_discovery_done_ind_t *)p_evt->data;
		p_ind->conn_handle = p_dis_done->conn_handle;
		p_ind->is_found = p_dis_done->is_found;
		p_ind->load_from_ftl = p_dis_done->load_from_ftl;
		p_ind->srv_num = p_dis_done->srv_num;
		p_ind->group_idx = p_group_info->group_idx;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_CSIS_CLIENT_READ_RESULT: {
		bool group_create = false;
		T_CSIS_CLIENT_READ_RESULT *p_read_result = (T_CSIS_CLIENT_READ_RESULT *)buf;
		APP_PRINT_INFO6("LE_AUDIO_MSG_CSIS_CLIENT_READ_RESULT: cause 0x%x, conn_handle 0x%x, group_handle %p, dev_handle %p, bd_addr %s, addr_type %d",
						p_read_result->cause, p_read_result->conn_handle, p_read_result->group_handle, p_read_result->dev_handle,
						TRACE_BDADDR(p_read_result->mem_info.bd_addr), p_read_result->mem_info.addr_type);
		APP_PRINT_INFO7("LE_AUDIO_MSG_CSIS_CLIENT_READ_RESULT: srv_instance_id %d, char_exit 0x%x, srv_uuid 0x%x, rank %d, set_mem_size %d, sirk_type %d, SIRK %b",
						p_read_result->mem_info.srv_instance_id, p_read_result->mem_info.char_exit, p_read_result->mem_info.srv_uuid,
						p_read_result->mem_info.rank, p_read_result->mem_info.set_mem_size, p_read_result->mem_info.sirk_type,
						TRACE_BINARY(RTK_BT_LE_CSIS_SIRK_LEN, p_read_result->mem_info.sirk));
		BT_LOGA("LE_AUDIO_MSG_CSIS_CLIENT_READ_RESULT: cause 0x%x, conn_handle 0x%x, group_handle 0x%08x, dev_handle 0x%08x\r\n",
				p_read_result->cause, p_read_result->conn_handle, p_read_result->group_handle, p_read_result->dev_handle);
		BT_LOGA("LE_AUDIO_MSG_CSIS_CLIENT_READ_RESULT: srv_instance_id %d, char_exit 0x%x, srv_uuid 0x%x, rank %d, set_mem_size %d\r\n",
				p_read_result->mem_info.srv_instance_id, p_read_result->mem_info.char_exit, p_read_result->mem_info.srv_uuid,
				p_read_result->mem_info.rank, p_read_result->mem_info.set_mem_size);
		p_link = bt_stack_le_audio_find_link_by_conn_handle(p_read_result->conn_handle);
		if (!p_link) {
			BT_LOGE("%s cannot find relate le link \r\n", __func__);
			break;
		}
		if (p_read_result->cause == 0) {
			rtk_bt_le_audio_group_handle_t group_handle = NULL;
			rtk_bt_le_audio_device_handle_t device_handle = NULL;

			if (p_read_result->group_handle == NULL) {//the server isnt in any group
				if (set_coordinator_add_group(&group_handle, bt_stack_le_audio_group_cb, &device_handle, &p_read_result->mem_info) == false) {
					BT_LOGE("%s set_coordinator_check_add_group fail\r\n", __func__);
					break;
				}
				p_group_info = bt_stack_le_audio_add_group(group_handle, p_read_result->mem_info.set_mem_size);
				/* get prefer codec cfg and prefer qos */
				if (p_group_info) {
					p_group_info->lea_unicast.audio_cfg_type = RTK_BT_LE_AUDIO_DEFAULT_UNICAST_AUDIO_CFG;
					p_group_info->lea_unicast.codec_cfg_item = RTK_BT_LE_AUDIO_DEFAULT_CODEC_CFG_ITEM;
					p_group_info->lea_unicast.qos_cfg_type = RTK_BT_LE_AUDIO_DEFAULT_QOS_CFG_TYPE;
#if defined(RTK_BT_LE_AUDIO_CIG_ISO_INTERVAL_CONFIG) && (RTK_BT_LE_AUDIO_CIG_ISO_INTERVAL_CONFIG == RTK_BT_ISO_INTERVAL_20_MS)
					p_group_info->lea_unicast.qos_cfg_type = RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY;
#endif
					if (false == qos_preferred_cfg_get((T_CODEC_CFG_ITEM)p_group_info->lea_unicast.codec_cfg_item,
													   (T_QOS_CFG_TYPE)p_group_info->lea_unicast.qos_cfg_type,
													   (T_QOS_CFG_PREFERRED *)&p_group_info->lea_unicast.preferred_qos)) {
						BT_LOGE("%s qos_preferred_cfg_get fail\r\n", __func__);
					} else {
						p_group_info->lea_unicast.session_qos.framing = p_group_info->lea_unicast.preferred_qos.framing;
						p_group_info->lea_unicast.session_qos.sdu_interval_m_s = p_group_info->lea_unicast.preferred_qos.sdu_interval;
						p_group_info->lea_unicast.session_qos.sdu_interval_s_m = p_group_info->lea_unicast.preferred_qos.sdu_interval;
						p_group_info->lea_unicast.session_qos.latency_m_s = p_group_info->lea_unicast.preferred_qos.max_transport_latency;
						p_group_info->lea_unicast.session_qos.latency_s_m = p_group_info->lea_unicast.preferred_qos.max_transport_latency;
						p_group_info->lea_unicast.session_qos.sink_presentation_delay = p_group_info->lea_unicast.preferred_qos.presentation_delay;
						p_group_info->lea_unicast.session_qos.source_presentation_delay = p_group_info->lea_unicast.preferred_qos.presentation_delay;
					}
					group_create = true;
				}
				if (p_read_result->mem_info.set_mem_size > 1) {
					if (false == set_coordinator_cfg_discover(group_handle, true, RTK_BT_LE_AUDIO_DEMO_CSIS_DISV_TIMEOUT)) {
						BT_LOGE("%s set_coordinator_cfg_discover fail\r\n", __func__);
						break;
					}
					BT_LOGA("[CAP] %s: start csis discover in csis group (group_handle=0x%08x) \r\n", __func__, group_handle);
				}
			} else {
				if (p_read_result->dev_handle == NULL) {
					if (false == set_coordinator_add_dev(p_read_result->group_handle, &device_handle, &p_read_result->mem_info)) {
						BT_LOGE("%s set_coordinator_add_dev fail\r\n", __func__);
						break;
					}
					p_group_info = bt_stack_le_audio_find_group(p_read_result->group_handle);
					if (p_group_info) {
						p_group_info->set_mem_size = p_read_result->mem_info.set_mem_size;
					}
				}
			}
			p_link->lea_disc_flags |= RTK_BT_LE_AUDIO_LINK_DISC_CSIS_DONE;
		}
		rtk_bt_le_audio_csis_client_read_result_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_READ_RESULT_IND,
									sizeof(rtk_bt_le_audio_csis_client_read_result_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_csis_client_read_result_ind_t *)p_evt->data;
		p_ind->cause = p_read_result->cause;
		p_ind->conn_handle = p_read_result->conn_handle;
		p_ind->group_handle = p_read_result->group_handle;
		p_ind->dev_handle = p_read_result->dev_handle;
		p_ind->mem_info.bd_addr.type = p_read_result->mem_info.addr_type;
		memcpy(p_ind->mem_info.bd_addr.addr_val, p_read_result->mem_info.bd_addr, RTK_BD_ADDR_LEN);
		p_ind->mem_info.srv_uuid = p_read_result->mem_info.srv_uuid;
		p_ind->mem_info.srv_instance_id = p_read_result->mem_info.srv_instance_id;
		p_ind->mem_info.char_exit = p_read_result->mem_info.char_exit;
		p_ind->mem_info.rank = p_read_result->mem_info.rank;
		p_ind->mem_info.lock = (rtk_bt_le_audio_csis_lock_t)p_read_result->mem_info.lock;
		p_ind->mem_info.set_mem_size = p_read_result->mem_info.set_mem_size;
		p_ind->mem_info.sirk_type = (rtk_bt_le_audio_csis_sirk_type_t)p_read_result->mem_info.sirk_type;
		memcpy(p_ind->mem_info.sirk, p_read_result->mem_info.sirk, RTK_BT_LE_CSIS_SIRK_LEN);
		if (group_create) {
			p_ind->group_idx = p_group_info->group_idx;
		} else {
			p_ind->group_idx = 0xFF;
		}
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_CSIS_CLIENT_LOCK_STATE: {
		T_CSIS_CLIENT_LOCK_STATE *p_result = (T_CSIS_CLIENT_LOCK_STATE *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_CSIS_CLIENT_LOCK_STATE: group_handle %p, event 0x%x, state %d",
						p_result->group_handle, p_result->event, p_result->lock_state);
		BT_LOGD("LE_AUDIO_MSG_CSIS_CLIENT_LOCK_STATE: group_handle 0x%08x, event 0x%x, state %d\r\n",
				p_result->group_handle, p_result->event, p_result->lock_state);
	}
	break;

	case LE_AUDIO_MSG_CSIS_CLIENT_SIZE_NOTIFY: {
		T_CSIS_CLIENT_SIZE_NOTIFY *p_result = (T_CSIS_CLIENT_SIZE_NOTIFY *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_CSIS_CLIENT_SIZE_NOTIFY: group_handle %p, old_size %d, new_size %d",
						p_result->group_handle, p_result->old_size, p_result->new_size);
	}
	break;

	case LE_AUDIO_MSG_CSIS_CLIENT_SEARCH_TIMEOUT: {
		T_CSIS_CLIENT_SEARCH_TIMEOUT *p_result = (T_CSIS_CLIENT_SEARCH_TIMEOUT *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_CSIS_CLIENT_SEARCH_TIMEOUT: group_handle %p, set_mem_size %d, search_done %d",
						p_result->group_handle, p_result->set_mem_size, p_result->search_done);
		BT_LOGD("LE_AUDIO_MSG_CSIS_CLIENT_SEARCH_TIMEOUT: group_handle 0x%08x, set_mem_size %d, search_done %d\r\n",
				p_result->group_handle, p_result->set_mem_size, p_result->search_done);
		set_coordinator_cfg_discover(NULL, false, 0);
		rtk_bt_le_audio_csis_client_search_done_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SEARCH_DONE_IND,
									sizeof(rtk_bt_le_audio_csis_client_search_done_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_csis_client_search_done_ind_t *)p_evt->data;
		p_ind->group_handle = p_result->group_handle;
		p_ind->set_mem_size = p_result->set_mem_size;
		p_ind->search_done = false;
		p_ind->search_timeout = p_result->search_done;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_CSIS_CLIENT_SEARCH_DONE: {
		T_CSIS_CLIENT_SEARCH_DONE *p_result = (T_CSIS_CLIENT_SEARCH_DONE *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_CSIS_CLIENT_SEARCH_DONE: group_handle %p, set_mem_size %d, search_done %d",
						p_result->group_handle, p_result->set_mem_size, p_result->search_done);
		BT_LOGD("LE_AUDIO_MSG_CSIS_CLIENT_SEARCH_DONE: group_handle 0x%08x, set_mem_size %d, search_done %d\r\n",
				p_result->group_handle, p_result->set_mem_size, p_result->search_done);
		set_coordinator_cfg_discover(NULL, false, 0);
		rtk_bt_le_audio_csis_client_search_done_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SEARCH_DONE_IND,
									sizeof(rtk_bt_le_audio_csis_client_search_done_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_csis_client_search_done_ind_t *)p_evt->data;
		p_ind->group_handle = p_result->group_handle;
		p_ind->set_mem_size = p_result->set_mem_size;
		p_ind->search_done = p_result->search_done;
		p_ind->search_timeout = false;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_CSIS_CLIENT_SET_MEM_FOUND: {
		T_CSIS_CLIENT_SET_MEM_FOUND *p_add = (T_CSIS_CLIENT_SET_MEM_FOUND *)buf;
		APP_PRINT_INFO8("LE_AUDIO_MSG_CSIS_CLIENT_SET_MEM_FOUND: group_handle %p, dev_handle %p, bd_addr %s, addr_type %d, srv_uuid 0x%x, rank %d, set_mem_size %d, SIRK %b",
						p_add->group_handle, p_add->dev_handle, TRACE_BDADDR(p_add->bd_addr), p_add->addr_type,
						p_add->srv_uuid, p_add->rank, p_add->set_mem_size, TRACE_BINARY(RTK_BT_LE_CSIS_SIRK_LEN, p_add->sirk));
		BT_LOGD(
			"LE_AUDIO_MSG_CSIS_CLIENT_SET_MEM_FOUND: group_handle %08x, dev_handle %08x, bd_addr %s, addr_type %d, srv_uuid 0x%x, rank %d, set_mem_size %d",
			p_add->group_handle, p_add->dev_handle, TRACE_BDADDR(p_add->bd_addr), p_add->addr_type,
			p_add->srv_uuid, p_add->rank, p_add->set_mem_size);

		rtk_bt_le_audio_csis_client_set_mem_found_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SET_MEM_FOUND_IND,
									sizeof(rtk_bt_le_audio_csis_client_set_mem_found_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_csis_client_set_mem_found_ind_t *)p_evt->data;
		p_ind->group_handle = p_add->group_handle;
		p_ind->dev_handle = p_add->dev_handle;
		p_ind->bd_addr.type = p_add->addr_type;
		memcpy(p_ind->bd_addr.addr_val, p_add->bd_addr, RTK_BD_ADDR_LEN);
		p_ind->srv_uuid = p_add->srv_uuid;
		p_ind->rank = p_add->rank;
		p_ind->set_mem_size = p_add->set_mem_size;
		memcpy(p_ind->sirk, p_add->sirk, RTK_BT_LE_CSIS_SIRK_LEN);

		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_CSIS_CLIENT_SIRK_NOTIFY: {
		T_CSIS_CLIENT_SIRK_NOTIFY *p_state = (T_CSIS_CLIENT_SIRK_NOTIFY *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_CSIS_CLIENT_SIRK_NOTIFY: group_handle %p, sirk_type %d, SIRK %b",
						p_state->group_handle, p_state->sirk_type, TRACE_BINARY(RTK_BT_LE_CSIS_SIRK_LEN, p_state->sirk));
		BT_LOGD("LE_AUDIO_MSG_CSIS_CLIENT_SIRK_NOTIFY: group_handle 0x%08x, sirk_type %d\r\n",
				p_state->group_handle, p_state->sirk_type);
	}
	break;
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
	case LE_AUDIO_MSG_MCP_SERVER_WRITE_MEDIA_CP_IND: {
		T_MCP_SERVER_WRITE_MEDIA_CP_IND *p_cp_ind = (T_MCP_SERVER_WRITE_MEDIA_CP_IND *)buf;
		BT_LOGD("LE_AUDIO_MSG_MCP_SERVER_WRITE_MEDIA_CP_IND: conn_handle %d, cid %d, service_id %d, opcode %d\r\n",
				p_cp_ind->conn_handle, p_cp_ind->cid, p_cp_ind->service_id, p_cp_ind->opcode);
		if (!bt_stack_mcp_handle_operation(p_cp_ind)) {
			BT_LOGE("%s bt_stack_mcp_handle_operation fail\r\n", __func__);
			app_result = BLE_AUDIO_CB_RESULT_REJECT;
		}
		rtk_bt_le_audio_mcp_server_write_media_cp_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_MCP_SERVER_WRITE_MEDIA_CP_IND,
									sizeof(rtk_bt_le_audio_mcp_server_write_media_cp_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_mcp_server_write_media_cp_ind_t *)p_evt->data;
		p_ind->conn_handle = p_cp_ind->conn_handle;
		p_ind->cid = p_cp_ind->cid;
		p_ind->service_id = p_cp_ind->service_id;
		p_ind->opcode = p_cp_ind->opcode;
		if (p_ind->opcode == RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_MOVE_RELATIVE ||
			p_ind->opcode == RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_SEGMENT ||
			p_ind->opcode == RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_TRACK ||
			p_ind->opcode == RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_GROUP) {
			memcpy(&p_ind->param, &p_cp_ind->param, sizeof(p_cp_ind->param));
		}
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_MCP_SERVER_READ_IND: {
		uint8_t cb_ret = 0;
		T_MCP_SERVER_READ_IND *p_read_ind = (T_MCP_SERVER_READ_IND *)buf;
		rtk_bt_le_audio_mcp_server_read_confirm_param_t app_cfm = {0};
		T_MCP_SERVER_READ_CFM read_cfm = {0};
		BT_LOGD("LE_AUDIO_MSG_MCP_SERVER_READ_IND: conn_handle %d, cid %d, service_id %d, char_uuid 0x%x, offset 0x%x\r\n",
				p_read_ind->conn_handle, p_read_ind->cid, p_read_ind->service_id, p_read_ind->char_uuid, p_read_ind->offset);
		if (p_read_ind->char_uuid == MCS_UUID_CHAR_MEDIA_PLAYER_NAME || p_read_ind->char_uuid == MCS_UUID_CHAR_TRACK_TITLE ||
			p_read_ind->char_uuid == MCS_UUID_CHAR_TRACK_DURATION || p_read_ind->char_uuid == MCS_UUID_CHAR_TRACK_POSITION ||
			p_read_ind->char_uuid == MCS_UUID_CHAR_CONTENT_CONTROL_ID) {
			rtk_bt_le_audio_mcp_server_read_ind_t *p_ind = NULL;
			if (false == rtk_bt_check_evt_cb_direct_calling(RTK_BT_LE_GP_CAP, RTK_BT_LE_AUDIO_EVT_MCP_SERVER_READ_IND)) {
				BT_LOGE("%s: RTK_BT_LE_AUDIO_EVT_MCP_SERVER_READ_IND is not direct calling!\r\n", __func__);
				break;
			}
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
										RTK_BT_LE_AUDIO_EVT_MCP_SERVER_READ_IND,
										sizeof(rtk_bt_le_audio_mcp_server_read_ind_t));
			if (!p_evt) {
				BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
				break;
			}
			p_ind = (rtk_bt_le_audio_mcp_server_read_ind_t *)p_evt->data;
			p_ind->conn_handle = p_read_ind->conn_handle;
			p_ind->cid = p_read_ind->cid;
			p_ind->service_id = p_read_ind->service_id;
			p_ind->char_uuid = p_read_ind->char_uuid;
			p_ind->offset = p_read_ind->offset;
			p_ind->p_cfm = &app_cfm;
			/* Send event */
			rtk_bt_evt_indicate(p_evt, &cb_ret);
			if (cb_ret == RTK_BT_EVT_CB_OK) {
				read_cfm.cause = BLE_AUDIO_CB_RESULT_SUCCESS;
				read_cfm.conn_handle = p_read_ind->conn_handle;
				read_cfm.cid = p_read_ind->cid;
				read_cfm.service_id = p_read_ind->service_id;
				read_cfm.char_uuid = p_read_ind->char_uuid;
				read_cfm.offset = p_read_ind->offset;
				if (read_cfm.char_uuid == RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME) {
					read_cfm.param.media_player_name.p_media_player_name = app_cfm.param.media_player_name.p_media_player_name;
					read_cfm.param.media_player_name.media_player_name_len = app_cfm.param.media_player_name.media_player_name_len;
				} else if (read_cfm.char_uuid == RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE) {
					read_cfm.param.track_title.p_track_title = app_cfm.param.track_title.p_track_title;
					read_cfm.param.track_title.track_title_len = app_cfm.param.track_title.track_title_len;
				} else if (read_cfm.char_uuid == RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_ICON_URL) {
					read_cfm.param.media_player_icon_url.p_media_player_icon_url = app_cfm.param.media_player_icon_url.p_media_player_icon_url;
					read_cfm.param.media_player_icon_url.media_player_icon_url_len = app_cfm.param.media_player_icon_url.media_player_icon_url_len;
				} else {
					memcpy(&read_cfm.param, &app_cfm.param, sizeof(app_cfm.param));
				}
				if (false == mcp_server_read_cfm(&read_cfm)) {
					BT_LOGE("%s mcp_server_read_cfm fail\r\n", __func__);
					return RTK_BT_ERR_LOWER_STACK_API;
				}
			}
		}
		break;
	}
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
	case LE_AUDIO_MSG_VCS_CLIENT_DIS_DONE: {
		T_VCS_CLIENT_DIS_DONE *dis_done = (T_VCS_CLIENT_DIS_DONE *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_VCS_CLIENT_DIS_DONE: conn_handle %x,is_found %d,load_from_ftl %d,type_exist 0x%x\r\n",
						dis_done->conn_handle, dis_done->is_found, dis_done->load_from_ftl, dis_done->type_exist);
		BT_LOGD("LE_AUDIO_MSG_VCS_CLIENT_DIS_DONE: conn_handle %x,is_found %d,load_from_ftl %d,type_exist 0x%x\r\n",
				dis_done->conn_handle, dis_done->is_found, dis_done->load_from_ftl, dis_done->type_exist);
		rtk_bt_le_audio_vcs_client_discovery_done_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_DISCOVERY_DONE_IND,
									sizeof(rtk_bt_le_audio_vcs_client_discovery_done_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_vcs_client_discovery_done_ind_t *)p_evt->data;
		p_ind->conn_handle = dis_done->conn_handle;
		p_ind->is_found = dis_done->is_found;
		p_ind->load_from_ftl = dis_done->load_from_ftl;
		p_ind->type_exist = dis_done->type_exist;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_VCS_CLIENT_CP_RESULT: {
		T_VCS_CLIENT_CP_RESULT *p_cp_result = (T_VCS_CLIENT_CP_RESULT *)buf;
		if (p_cp_result->cause != GAP_CAUSE_SUCCESS) {
			APP_PRINT_ERROR3("LE_AUDIO_MSG_VCS_CLIENT_CP_RESULT: conn_handle 0x%x, cause 0x%x, op 0x%x",
							 p_cp_result->conn_handle, p_cp_result->cause, p_cp_result->cp_op);
			BT_LOGE("LE_AUDIO_MSG_VCS_CLIENT_CP_RESULT: conn_handle 0x%x, cause 0x%x, op 0x%x\r\n",
					p_cp_result->conn_handle, p_cp_result->cause, p_cp_result->cp_op);
		}
	}
	break;

	case LE_AUDIO_MSG_VCS_CLIENT_VOLUME_STATE_DATA: {
		T_VCS_CLIENT_VOLUME_STATE_DATA *p_data = (T_VCS_CLIENT_VOLUME_STATE_DATA *)buf;
		APP_PRINT_INFO7("LE_AUDIO_MSG_VCS_CLIENT_VOLUME_STATE_DATA: conn_handle 0x%x, is_notify 0x%x, type 0x%x, volume_flags 0x%x, volume_setting 0x%x, mute 0x%x, change_counter 0x%x",
						p_data->conn_handle, p_data->is_notify, p_data->type, p_data->data.volume_flags,
						p_data->data.volume_state.volume_setting, p_data->data.volume_state.mute, p_data->data.volume_state.change_counter);
		BT_LOGD(
			"LE_AUDIO_MSG_VCS_CLIENT_VOLUME_STATE_DATA: conn_handle 0x%x, is_notify 0x%x, type 0x%x, volume_flags 0x%x, volume_setting 0x%x, mute 0x%x, change_counter 0x%x\r\n",
			p_data->conn_handle, p_data->is_notify, p_data->type, p_data->data.volume_flags,
			p_data->data.volume_state.volume_setting, p_data->data.volume_state.mute, p_data->data.volume_state.change_counter);
		if (p_data->type == VCS_CHAR_VOLUME_STATE) {
			rtk_bt_le_audio_vcs_client_volume_state_ind_t *p_ind = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
										RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_STATE_IND,
										sizeof(rtk_bt_le_audio_vcs_client_volume_state_ind_t));
			if (!p_evt) {
				BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
				break;
			}
			p_ind = (rtk_bt_le_audio_vcs_client_volume_state_ind_t *)p_evt->data;
			p_ind->conn_handle = p_data->conn_handle;
			p_ind->is_notify = p_data->is_notify;
			p_ind->volume_state.volume_setting = p_data->data.volume_state.volume_setting;
			p_ind->volume_state.mute = p_data->data.volume_state.mute;
			p_ind->volume_state.change_counter = p_data->data.volume_state.change_counter;
		} else if (p_data->type == VCS_CHAR_VOLUME_FLAGS) {
			rtk_bt_le_audio_vcs_client_volume_flag_ind_t *p_ind = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
										RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_FLAG_IND,
										sizeof(rtk_bt_le_audio_vcs_client_volume_flag_ind_t));
			if (!p_evt) {
				BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
				break;
			}
			p_ind = (rtk_bt_le_audio_vcs_client_volume_flag_ind_t *)p_evt->data;
			p_ind->conn_handle = p_data->conn_handle;
			p_ind->is_notify = p_data->is_notify;
			p_ind->volume_flags = p_data->data.volume_flags;
		} else {
			BT_LOGE("%s unknown p_data->type = %d\r\n", __func__, p_data->type);
			break;
		}
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
	case LE_AUDIO_MSG_MICS_CLIENT_DIS_DONE: {
		T_MICS_CLIENT_DIS_DONE *dis_done = (T_MICS_CLIENT_DIS_DONE *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_MICS_CLIENT_DIS_DONE: conn_handle %x,is_found %d,load_from_ftl %d,mic_mute 0x%x\r\n",
						dis_done->conn_handle, dis_done->is_found, dis_done->load_from_ftl, dis_done->mic_mute);
		BT_LOGD("LE_AUDIO_MSG_MICS_CLIENT_DIS_DONE: conn_handle %x,is_found %d,load_from_ftl %d,mic_mute 0x%x\r\n",
				dis_done->conn_handle, dis_done->is_found, dis_done->load_from_ftl, dis_done->mic_mute);
		rtk_bt_le_audio_mics_client_discovery_done_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_MICS_CLIENT_DISCOVERY_DONE_IND,
									sizeof(rtk_bt_le_audio_mics_client_discovery_done_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_mics_client_discovery_done_ind_t *)p_evt->data;
		p_ind->conn_handle = dis_done->conn_handle;
		p_ind->is_found = dis_done->is_found;
		p_ind->load_from_ftl = dis_done->load_from_ftl;
		p_ind->mic_mute = dis_done->mic_mute;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_MICS_CLIENT_WRITE_RESULT: {
		T_MICS_CLIENT_WRITE_RESULT *write_result = (T_MICS_CLIENT_WRITE_RESULT *)buf;
		if (write_result->cause != GAP_SUCCESS) {
			APP_PRINT_ERROR2("LE_AUDIO_MSG_MICS_CLIENT_WRITE_RESULT: conn_handle %x write fail error cause: 0x%x", write_result->conn_handle, write_result->cause);
			BT_LOGE("LE_AUDIO_MSG_MICS_CLIENT_WRITE_RESULT:conn_handle %x write fail error cause: 0x%x\r\n", write_result->conn_handle,
					write_result->cause);
		}
	}
	break;

	case LE_AUDIO_MSG_MICS_CLIENT_NOTIFY: {
		T_MICS_CLIENT_NOTIFY *notify = (T_MICS_CLIENT_NOTIFY *)buf;
		APP_PRINT_INFO2("LE_AUDIO_MSG_MICS_CLIENT_NOTIFY: conn_handle %x,mic_mute %d\r\n", notify->conn_handle, notify->mic_mute);
		BT_LOGD("LE_AUDIO_MSG_MICS_CLIENT_NOTIFY: conn_handle %x,mic_mute %d\r\n", notify->conn_handle, notify->mic_mute);
		rtk_bt_le_audio_mics_client_notify_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_MICS_CLIENT_NOTIFY_IND,
									sizeof(rtk_bt_le_audio_mics_client_notify_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_mics_client_notify_ind_t *)p_evt->data;
		p_ind->conn_handle = notify->conn_handle;
		p_ind->mic_mute = notify->mic_mute;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	case LE_AUDIO_MSG_VOCS_CLIENT_DIS_DONE: {
		T_VOCS_CLIENT_DIS_DONE *dis_done = (T_VOCS_CLIENT_DIS_DONE *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_VOCS_CLIENT_DIS_DONE: conn_handle %d,is_found %d, load_from_ftl %d, srv_num %d",
						dis_done->conn_handle, dis_done->is_found, dis_done->load_from_ftl, dis_done->srv_num);
		BT_LOGD("LE_AUDIO_MSG_VOCS_CLIENT_DIS_DONE: conn_handle %d,is_found %d, load_from_ftl %d, srv_num %d\r\n",
				dis_done->conn_handle, dis_done->is_found, dis_done->load_from_ftl, dis_done->srv_num);
		rtk_bt_le_audio_vocs_client_discovery_done_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_DISCOVERY_DONE_IND,
									sizeof(rtk_bt_le_audio_vocs_client_discovery_done_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_vocs_client_discovery_done_ind_t *)p_evt->data;
		p_ind->conn_handle = dis_done->conn_handle;
		p_ind->is_found = dis_done->is_found;
		p_ind->load_from_ftl = dis_done->load_from_ftl;
		p_ind->srv_num = dis_done->srv_num;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_VOCS_CLIENT_NOTIFY: {
		T_VOCS_CLIENT_NOTIFY *notify_data = (T_VOCS_CLIENT_NOTIFY *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_VOCS_CLIENT_NOTIFY: conn_handle 0x%x, srv_instance_id %d, type=%d",
						notify_data->conn_handle, notify_data->srv_instance_id, notify_data->type);
		BT_LOGD("LE_AUDIO_MSG_VOCS_CLIENT_NOTIFY: conn_handle 0x%x, srv_instance_id %d, type=%d\r\n",
				notify_data->conn_handle, notify_data->srv_instance_id, notify_data->type);
		rtk_bt_le_audio_vocs_client_notify_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_NOTIFY_IND,
									sizeof(rtk_bt_le_audio_vocs_client_notify_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_vocs_client_notify_ind_t *)p_evt->data;
		p_ind->conn_handle = notify_data->conn_handle;
		p_ind->srv_instance_id = notify_data->srv_instance_id;
		p_ind->type = (rtk_bt_le_audio_vocs_char_type_t)notify_data->type;
		switch (p_ind->type) {
		case RTK_BT_LE_AUDIO_VOCS_CHAR_OFFSET_STATE:
			p_ind->data.volume_offset.volume_offset = notify_data->data.volume_offset.volume_offset;
			p_ind->data.volume_offset.change_counter = notify_data->data.volume_offset.change_counter;
			break;
		case RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_LOCATION:
			p_ind->data.audio_location = notify_data->data.audio_location;
			break;
		case RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_OUTPUT_DESC:
			p_ind->data.output_des.output_des_len = notify_data->data.output_des.output_des_len + 1;
			p_ind->data.output_des.p_output_des = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_ind->data.output_des.output_des_len);
			if (!p_ind->data.output_des.p_output_des) {
				BT_LOGE("p_ind->p_data alloc fail, len = %d\r\n", p_ind->data.output_des.output_des_len);
				rtk_bt_event_free(p_evt);
				break;
			}
			memset(p_ind->data.output_des.p_output_des, 0, p_ind->data.output_des.output_des_len);
			memcpy((void *)p_ind->data.output_des.p_output_des, (void *)notify_data->data.output_des.p_output_des, notify_data->data.output_des.output_des_len);
			p_evt->user_data = p_ind->data.output_des.p_output_des;
			BT_LOGD("vocs char output des: srv_instance_id %d, output_des_len %d, p_output_des %s\r\n",
					p_ind->srv_instance_id, p_ind->data.output_des.output_des_len, p_ind->data.output_des.p_output_des);
			break;
		default:
			BT_LOGE("%s RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_READ_RESULT_IND type (%d) error\r\n", __func__, p_ind->type);
			break;
		}
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_VOCS_CLIENT_READ_RESULT: {
		T_VOCS_CLIENT_READ_RESULT *read_result = (T_VOCS_CLIENT_READ_RESULT *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_VOCS_CLIENT_READ_RESULT: conn_handle %d,srv_instance_id %d, type %d, cause 0x%x",
						read_result->conn_handle, read_result->srv_instance_id, read_result->type, read_result->cause);
		BT_LOGD("LE_AUDIO_MSG_VOCS_CLIENT_READ_RESULT: conn_handle %d,srv_instance_id %d, type %d, cause 0x%x\r\n",
				read_result->conn_handle, read_result->srv_instance_id, read_result->type, read_result->cause);
		rtk_bt_le_audio_vocs_client_read_result_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_READ_RESULT_IND,
									sizeof(rtk_bt_le_audio_vocs_client_read_result_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_vocs_client_read_result_ind_t *)p_evt->data;
		p_ind->conn_handle = read_result->conn_handle;
		p_ind->srv_instance_id = read_result->srv_instance_id;
		p_ind->type = (rtk_bt_le_audio_vocs_char_type_t)read_result->type;
		switch (p_ind->type) {
		case RTK_BT_LE_AUDIO_VOCS_CHAR_OFFSET_STATE:
			p_ind->data.volume_offset.volume_offset = read_result->data.volume_offset.volume_offset;
			p_ind->data.volume_offset.change_counter = read_result->data.volume_offset.change_counter;
			break;
		case RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_LOCATION:
			p_ind->data.audio_location = read_result->data.audio_location;
			break;
		case RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_OUTPUT_DESC:
			p_ind->data.output_des.output_des_len = read_result->data.output_des.output_des_len + 1;
			p_ind->data.output_des.p_output_des = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_ind->data.output_des.output_des_len);
			if (!p_ind->data.output_des.p_output_des) {
				BT_LOGE("p_ind->p_data alloc fail, len = %d\r\n", p_ind->data.output_des.output_des_len);
				rtk_bt_event_free(p_evt);
				break;
			}
			memset(p_ind->data.output_des.p_output_des, 0, p_ind->data.output_des.output_des_len);
			memcpy((void *)p_ind->data.output_des.p_output_des, (void *)read_result->data.output_des.p_output_des, read_result->data.output_des.output_des_len);
			p_evt->user_data = p_ind->data.output_des.p_output_des;
			break;
		default:
			BT_LOGE("%s RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_READ_RESULT_IND type (%d) error\r\n", __func__, p_ind->type);
			break;
		}
		p_ind->cause = read_result->cause;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_VOCS_CLIENT_CP_RESULT: {
		T_VOCS_CLIENT_CP_RESULT *p_cp_result = (T_VOCS_CLIENT_CP_RESULT *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_VOCS_CLIENT_CP_RESULT: conn_handle 0x%x, cause 0x%x, srv_instance_id %d, cp_op=%d",
						p_cp_result->conn_handle, p_cp_result->cause, p_cp_result->srv_instance_id, p_cp_result->cp_op);
		BT_LOGD("LE_AUDIO_MSG_VOCS_CLIENT_CP_RESULT: conn_handle 0x%x, cause 0x%x, srv_instance_id %d, cp_op=%d\r\n",
				p_cp_result->conn_handle, p_cp_result->cause, p_cp_result->srv_instance_id, p_cp_result->cp_op);
		rtk_bt_le_audio_vocs_client_cp_result_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_CP_RESULT_IND,
									sizeof(rtk_bt_le_audio_vocs_client_cp_result_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_vocs_client_cp_result_ind_t *)p_evt->data;
		p_ind->conn_handle = p_cp_result->conn_handle;
		p_ind->srv_instance_id = p_cp_result->srv_instance_id;
		p_ind->cause = p_cp_result->cause;
		p_ind->cp_op = (rtk_bt_le_audio_vocs_cp_op_t)p_cp_result->cp_op;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	case LE_AUDIO_MSG_AICS_CLIENT_DIS_DONE: {
		T_AICS_CLIENT_DIS_DONE *dis_done = (T_AICS_CLIENT_DIS_DONE *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_VOCS_CLIENT_DIS_DONE: conn_handle %d,is_found %d, load_from_ftl %d, srv_num %d",
						dis_done->conn_handle, dis_done->is_found, dis_done->load_from_ftl, dis_done->srv_num);
		BT_LOGD("LE_AUDIO_MSG_VOCS_CLIENT_DIS_DONE: conn_handle %d,is_found %d, load_from_ftl %d, srv_num %d\r\n",
				dis_done->conn_handle, dis_done->is_found, dis_done->load_from_ftl, dis_done->srv_num);
		rtk_bt_le_audio_aics_client_discovery_done_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_DISCOVERY_DONE_IND,
									sizeof(rtk_bt_le_audio_aics_client_discovery_done_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_aics_client_discovery_done_ind_t *)p_evt->data;
		p_ind->conn_handle = dis_done->conn_handle;
		p_ind->is_found = dis_done->is_found;
		p_ind->load_from_ftl = dis_done->load_from_ftl;
		p_ind->srv_num = dis_done->srv_num;
		vcs_get_all_inc_aics(p_ind->conn_handle, (T_ATTR_INSTANCE *)&p_ind->vcs_instance);
		mics_get_all_inc_aics(p_ind->conn_handle, (T_ATTR_INSTANCE *)&p_ind->mics_instance);
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_AICS_CLIENT_NOTIFY: {
		T_AICS_CLIENT_NOTIFY *notify = (T_AICS_CLIENT_NOTIFY *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_AICS_CLIENT_NOTIFY: conn_handle %d,srv_instance_id %d, type %d",
						notify->conn_handle, notify->srv_instance_id, notify->type);
		BT_LOGD("LE_AUDIO_MSG_AICS_CLIENT_NOTIFY: conn_handle %d,srv_instance_id %d, type %d\r\n",
				notify->conn_handle, notify->srv_instance_id, notify->type);
		rtk_bt_le_audio_aics_client_notify_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_NOTIFY_IND,
									sizeof(rtk_bt_le_audio_aics_client_notify_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_aics_client_notify_ind_t *)p_evt->data;
		p_ind->conn_handle = notify->conn_handle;
		p_ind->srv_instance_id = notify->srv_instance_id;
		p_ind->type = (rtk_bt_le_audio_aics_char_type_t)notify->type;
		switch (p_ind->type) {
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_STATE:
			memcpy(&p_ind->data.input_state, &notify->data.input_state, sizeof(rtk_bt_le_audio_aics_input_state_t));
			break;
		case RTK_BT_LE_AUDIO_AICS_CHAR_GAIN_SETTING_PROP:
			memcpy(&p_ind->data.setting_prop, &notify->data.setting_prop, sizeof(rtk_bt_le_audio_aics_gain_setting_prop_t));
			break;
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_TYPE:
			p_ind->data.input_type = notify->data.input_type;
			break;
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_STATUS:
			p_ind->data.input_status = notify->data.input_status;
			break;
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_DES:
			p_ind->data.input_des.input_des_len = notify->data.input_des.input_des_len + 1;
			p_ind->data.input_des.p_input_des = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, notify->data.input_des.input_des_len);
			if (!p_ind->data.input_des.p_input_des) {
				BT_LOGE("p_ind->p_data alloc fail, len = %d\r\n", p_ind->data.input_des.input_des_len);
				rtk_bt_event_free(p_evt);
				break;
			}
			memset(p_ind->data.input_des.p_input_des, 0, p_ind->data.input_des.input_des_len);
			memcpy((void *)p_ind->data.input_des.p_input_des, (void *)notify->data.input_des.p_input_des, notify->data.input_des.input_des_len);
			p_evt->user_data = p_ind->data.input_des.p_input_des;
			break;
		default:
			BT_LOGE("%s RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_READ_RESULT_IND type (%d) error\r\n", __func__, p_ind->type);
			break;
		}
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_AICS_CLIENT_READ_RESULT: {
		T_AICS_CLIENT_READ_RESULT *read_result = (T_AICS_CLIENT_READ_RESULT *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_AICS_CLIENT_READ_RESULT: conn_handle %d,srv_instance_id %d, type %d, cause 0x%x",
						read_result->conn_handle, read_result->srv_instance_id, read_result->type, read_result->cause);
		BT_LOGD("LE_AUDIO_MSG_AICS_CLIENT_READ_RESULT: conn_handle %d,srv_instance_id %d, type %d, cause 0x%x\r\n",
				read_result->conn_handle, read_result->srv_instance_id, read_result->type, read_result->cause);
		rtk_bt_le_audio_aics_client_read_result_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_READ_RESULT_IND,
									sizeof(rtk_bt_le_audio_aics_client_read_result_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_aics_client_read_result_ind_t *)p_evt->data;
		p_ind->conn_handle = read_result->conn_handle;
		p_ind->srv_instance_id = read_result->srv_instance_id;
		p_ind->type = (rtk_bt_le_audio_aics_char_type_t)read_result->type;
		switch (p_ind->type) {
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_STATE:
			memcpy(&p_ind->data.input_state, &read_result->data.input_state, sizeof(rtk_bt_le_audio_aics_input_state_t));
			break;
		case RTK_BT_LE_AUDIO_AICS_CHAR_GAIN_SETTING_PROP:
			memcpy(&p_ind->data.setting_prop, &read_result->data.setting_prop, sizeof(rtk_bt_le_audio_aics_gain_setting_prop_t));
			break;
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_TYPE:
			p_ind->data.input_type = read_result->data.input_type;
			break;
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_STATUS:
			p_ind->data.input_status = read_result->data.input_status;
			break;
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_DES:
			p_ind->data.input_des.input_des_len = read_result->data.input_des.input_des_len + 1;
			p_ind->data.input_des.p_input_des = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_ind->data.input_des.input_des_len);
			if (!p_ind->data.input_des.p_input_des) {
				BT_LOGE("p_ind->p_data alloc fail, len = %d\r\n", p_ind->data.input_des.input_des_len);
				rtk_bt_event_free(p_evt);
				break;
			}
			memset(p_ind->data.input_des.p_input_des, 0, p_ind->data.input_des.input_des_len);
			memcpy((void *)p_ind->data.input_des.p_input_des, (void *)read_result->data.input_des.p_input_des, read_result->data.input_des.input_des_len);
			p_evt->user_data = p_ind->data.input_des.p_input_des;
			BT_LOGD("aics char input des: srv_instance_id %d, input_des_len %d, p_input_des %s\r\n",
					p_ind->srv_instance_id, p_ind->data.input_des.input_des_len, p_ind->data.input_des.p_input_des);
			break;
		default:
			BT_LOGE("%s RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_READ_RESULT_IND type (%d) error\r\n", __func__, p_ind->type);
			break;
		}
		p_ind->cause = read_result->cause;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_AICS_CLIENT_CP_RESULT: {
		T_AICS_CLIENT_CP_RESULT *read_result = (T_AICS_CLIENT_CP_RESULT *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_AICS_CLIENT_CP_RESULT: conn_handle %d,srv_instance_id %d, cp_op %d, cause 0x%x",
						read_result->conn_handle, read_result->srv_instance_id, read_result->cp_op, read_result->cause);
		BT_LOGD("LE_AUDIO_MSG_AICS_CLIENT_CP_RESULT: conn_handle %d,srv_instance_id %d, cp_op %d, cause 0x%x\r\n",
				read_result->conn_handle, read_result->srv_instance_id, read_result->cp_op, read_result->cause);
		rtk_bt_le_audio_aics_client_cp_result_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_CP_RESULT_IND,
									sizeof(rtk_bt_le_audio_aics_client_cp_result_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_aics_client_cp_result_ind_t *)p_evt->data;
		p_ind->conn_handle = read_result->conn_handle;
		p_ind->srv_instance_id = read_result->srv_instance_id;
		p_ind->cp_op = (rtk_bt_le_audio_aics_cp_op_t)read_result->cp_op;
		p_ind->cause = read_result->cause;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
#endif
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
	case LE_AUDIO_MSG_MCP_CLIENT_DIS_DONE: {
		T_MCP_CLIENT_DIS_DONE *p_dis_done = (T_MCP_CLIENT_DIS_DONE *)buf;
		BT_LOGD("LE_AUDIO_MSG_MCP_CLIENT_DIS_DONE conn_handle=0x%x, general_mcs=%d, is_found=%d, load_from_ftl=%d, srv_num=%d\r\n",
				p_dis_done->conn_handle, p_dis_done->gmcs, p_dis_done->is_found, p_dis_done->load_from_ftl, p_dis_done->srv_num);
		rtk_bt_le_audio_mcp_client_discovery_done_ind_t *p_discovery_done_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_DISCOVERY_DONE_IND,
									sizeof(rtk_bt_le_audio_mcp_client_discovery_done_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_discovery_done_ind = (rtk_bt_le_audio_mcp_client_discovery_done_ind_t *)p_evt->data;
		p_discovery_done_ind->conn_handle = p_dis_done->conn_handle;
		p_discovery_done_ind->general_mcs = p_dis_done->gmcs;
		p_discovery_done_ind->is_found = p_dis_done->is_found;
		p_discovery_done_ind->srv_num = p_dis_done->srv_num;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_MCP_CLIENT_READ_RESULT: {
		T_MCP_CLIENT_READ_RESULT *p_read_result = (T_MCP_CLIENT_READ_RESULT *)buf;
		BT_LOGD("LE_AUDIO_MSG_MCP_CLIENT_READ_RESULT conn_handle=0x%x, general_mcs=%d, srv_instance_id=%d, char_uuid=0x%x, cause = 0x%x\r\n",
				p_read_result->conn_handle, p_read_result->gmcs, p_read_result->srv_instance_id, p_read_result->char_uuid, p_read_result->cause);
		rtk_stack_mcp_client_read_result_handle(p_read_result);
	}
	break;

	case LE_AUDIO_MSG_MCP_CLIENT_NOTIFY: {
		T_MCP_CLIENT_NOTIFY *p_notify_result = (T_MCP_CLIENT_NOTIFY *)buf;
		BT_LOGD("LE_AUDIO_MSG_MCP_CLIENT_NOTIFY conn_handle=0x%x, general_mcs=%d, srv_instance_id=%d, char_uuid=0x%x\r\n",
				p_notify_result->conn_handle, p_notify_result->gmcs, p_notify_result->srv_instance_id, p_notify_result->char_uuid);
		rtk_stack_mcp_client_notify_handle(p_notify_result);
	}
	break;

	case LE_AUDIO_MSG_MCP_CLIENT_MEDIA_CP_NOTIFY: {
		T_MCP_CLIENT_MEDIA_CP_NOTIFY *p_cp_notify = (T_MCP_CLIENT_MEDIA_CP_NOTIFY *)buf;
		BT_LOGD(
			"LE_AUDIO_MSG_MCP_CLIENT_MEDIA_CP_NOTIFY conn_handle=0x%x, general_mcs=%d, srv_instance_id=%d, requested_opcode=0x%x, result_code=0x%x\r\n",
			p_cp_notify->conn_handle, p_cp_notify->gmcs, p_cp_notify->srv_instance_id, p_cp_notify->requested_opcode, p_cp_notify->result_code);
		rtk_bt_le_audio_mcs_media_cp_nodify_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_MEDIA_CP_NOTIFY_IND,
									sizeof(rtk_bt_le_audio_mcs_media_cp_nodify_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_mcs_media_cp_nodify_ind_t *)p_evt->data;
		p_ind->conn_handle = p_cp_notify->conn_handle;
		p_ind->general_mcs = p_cp_notify->gmcs;
		p_ind->srv_instance_id = p_cp_notify->srv_instance_id;
		p_ind->requested_opcode = p_cp_notify->requested_opcode;
		p_ind->result_code = p_cp_notify->result_code;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
#endif
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
	case LE_AUDIO_MSG_VCS_VOLUME_CP_IND: {
		T_VCS_VOLUME_CP_IND *p_volume_state = (T_VCS_VOLUME_CP_IND *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_VCS_VOLUME_CP_IND: conn_handle 0x%x, volume_setting %d, mute %d,cp_op %d",
						p_volume_state->conn_handle, p_volume_state->volume_setting, p_volume_state->mute, p_volume_state->cp_op);
		BT_LOGD("LE_AUDIO_MSG_VCS_VOLUME_CP_IND: conn_handle 0x%x, volume_setting %d, mute %d,cp_op %d\r\n",
				p_volume_state->conn_handle, p_volume_state->volume_setting, p_volume_state->mute, p_volume_state->cp_op);
		rtk_bt_le_audio_vcs_volume_cp_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_VCS_VOLUME_CP_IND,
									sizeof(rtk_bt_le_audio_vcs_volume_cp_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_vcs_volume_cp_ind_t *)p_evt->data;
		p_ind->conn_handle = p_volume_state->conn_handle;
		p_ind->volume_setting = p_volume_state->volume_setting;
		p_ind->mute = p_volume_state->mute;
		p_ind->cp_op = (rtk_bt_le_audio_vcs_cp_op_t)p_volume_state->cp_op;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
	case LE_AUDIO_MSG_MICS_WRITE_MUTE_IND: {
		T_MICS_WRITE_MUTE_IND *p_write_ind = (T_MICS_WRITE_MUTE_IND *)buf;
		APP_PRINT_INFO1("LE_AUDIO_MSG_MICS_WRITE_MUTE_IND: mute_value %d", p_write_ind->mic_mute);
		BT_LOGD("LE_AUDIO_MSG_MICS_WRITE_MUTE_IND: mute_value %d\r\n", p_write_ind->mic_mute);

		rtk_bt_le_audio_mics_write_mute_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_MICS_WRITE_MUTE_IND,
									sizeof(rtk_bt_le_audio_mics_write_mute_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_mics_write_mute_ind_t *)p_evt->data;
		p_ind->mic_mute = (rtk_bt_le_audio_mics_mute_state_t)p_write_ind->mic_mute;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	case LE_AUDIO_MSG_AICS_CP_IND: {
		T_AICS_CP_IND *p_aics_op = (T_AICS_CP_IND *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_AICS_CP_IND: conn_handle 0x%x, srv_instance_id %d, cp_op 0x%x, gain_setting %d",
						p_aics_op->conn_handle, p_aics_op->srv_instance_id, p_aics_op->cp_op, p_aics_op->gain_setting);
		BT_LOGD("LE_AUDIO_MSG_AICS_CP_IND: conn_handle 0x%x, srv_instance_id %d, cp_op 0x%x, gain_setting %d\r\n",
				p_aics_op->conn_handle, p_aics_op->srv_instance_id, p_aics_op->cp_op, p_aics_op->gain_setting);
		rtk_bt_le_audio_aics_cp_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_AICS_CP_IND,
									sizeof(rtk_bt_le_audio_aics_cp_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_aics_cp_ind_t *)p_evt->data;
		p_ind->conn_handle = p_aics_op->conn_handle;
		p_ind->srv_instance_id = p_aics_op->srv_instance_id;
		p_ind->cp_op = (rtk_bt_le_audio_aics_cp_op_t)p_aics_op->cp_op;
		p_ind->gain_setting = p_aics_op->gain_setting;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
	case LE_AUDIO_MSG_AICS_WRITE_INPUT_DES_IND: {
		T_AICS_WRITE_INPUT_DES_IND *p_input_des = (T_AICS_WRITE_INPUT_DES_IND *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_AICS_WRITE_INPUT_DES_IND: conn_handle %d, srv_instance_id %d, input_des_len %d",
						p_input_des->conn_handle, p_input_des->srv_instance_id, p_input_des->input_des.input_des_len);
		BT_LOGD("LE_AUDIO_MSG_AICS_WRITE_INPUT_DES_IND: conn_handle %d,srv_instance_id %d,input_des_len %d,p_input_des=%s\r\n",
				p_input_des->conn_handle, p_input_des->srv_instance_id, p_input_des->input_des.input_des_len, p_input_des->input_des.p_input_des);
		rtk_bt_le_audio_aics_write_input_des_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_AICS_WRITE_INPUT_DES_IND,
									sizeof(rtk_bt_le_audio_aics_write_input_des_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_aics_write_input_des_ind_t *)p_evt->data;
		p_ind->srv_instance_id = p_input_des->srv_instance_id;
		p_ind->input_des.input_des_len = p_input_des->input_des.input_des_len + 1;
		p_ind->input_des.p_input_des = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_ind->input_des.input_des_len);
		if (!p_ind->input_des.p_input_des) {
			BT_LOGE("p_ind->p_data alloc fail, len = %d\r\n", p_ind->input_des.input_des_len);
			rtk_bt_event_free(p_evt);
			break;
		}
		memset(p_ind->input_des.p_input_des, 0, p_ind->input_des.input_des_len);
		memcpy((void *)p_ind->input_des.p_input_des, (void *)p_input_des->input_des.p_input_des, p_input_des->input_des.input_des_len);
		p_evt->user_data = p_ind->input_des.p_input_des;

		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	case LE_AUDIO_MSG_VOCS_WRITE_OFFSET_STATE_IND: {
		T_VOCS_WRITE_OFFSET_STATE_IND *p_volume_offset_state = (T_VOCS_WRITE_OFFSET_STATE_IND *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_VOCS_WRITE_OFFSET_STATE_IND: conn_handle 0x%x, srv_instance_id %d, volume_offset %d",
						p_volume_offset_state->conn_handle, p_volume_offset_state->srv_instance_id, p_volume_offset_state->volume_offset);
		BT_LOGD("LE_AUDIO_MSG_VOCS_WRITE_OFFSET_STATE_IND: conn_handle 0x%x, srv_instance_id %d, volume_offset %d\r\n",
				p_volume_offset_state->conn_handle, p_volume_offset_state->srv_instance_id, p_volume_offset_state->volume_offset);
		rtk_bt_le_audio_vocs_write_offset_state_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_OFFSET_STATE_IND,
									sizeof(rtk_bt_le_audio_vocs_write_offset_state_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_vocs_write_offset_state_ind_t *)p_evt->data;
		p_ind->conn_handle = p_volume_offset_state->conn_handle;
		p_ind->srv_instance_id = p_volume_offset_state->srv_instance_id;
		p_ind->volume_offset = p_volume_offset_state->volume_offset;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
	case LE_AUDIO_MSG_VOCS_WRITE_AUDIO_LOCATION_IND: {
		T_VOCS_WRITE_AUDIO_LOCATION_IND *p_location = (T_VOCS_WRITE_AUDIO_LOCATION_IND *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_VOCS_WRITE_AUDIO_LOCATION_IND: conn_handle 0x%x, srv_instance_id %d, audio_location 0x%x",
						p_location->conn_handle, p_location->srv_instance_id, (unsigned int)p_location->audio_location);
		BT_LOGD("LE_AUDIO_MSG_VOCS_WRITE_AUDIO_LOCATION_IND: conn_handle 0x%x, srv_instance_id %d, audio_location 0x%x\r\n",
				p_location->conn_handle, p_location->srv_instance_id, (unsigned int)p_location->audio_location);
		rtk_bt_le_audio_vocs_write_audio_location_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_AUDIO_LOCATION_IND,
									sizeof(rtk_bt_le_audio_vocs_write_audio_location_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_vocs_write_audio_location_ind_t *)p_evt->data;
		p_ind->conn_handle = p_location->conn_handle;
		p_ind->srv_instance_id = p_location->srv_instance_id;
		p_ind->audio_location = p_location->audio_location;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
	case LE_AUDIO_MSG_VOCS_WRITE_OUTPUT_DES_IND: {
		T_VOCS_WRITE_OUTPUT_DES_IND *p_des = (T_VOCS_WRITE_OUTPUT_DES_IND *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_VOCS_WRITE_OUTPUT_DES_IND: conn_handle 0x%x, srv_instance_id %d, output_des_len %d",
						p_des->conn_handle, p_des->srv_instance_id, p_des->output_des.output_des_len);
		BT_LOGD("LE_AUDIO_MSG_VOCS_WRITE_OUTPUT_DES_IND: conn_handle 0x%x, srv_instance_id %d, output_des_len %d, output_des %s\r\n",
				p_des->conn_handle, p_des->srv_instance_id, p_des->output_des.output_des_len, p_des->output_des.p_output_des);
		rtk_bt_le_audio_vocs_write_output_des_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_CAP,
									RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_OUTPUT_DES_IND,
									sizeof(rtk_bt_le_audio_vocs_write_output_des_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_vocs_write_output_des_ind_t *)p_evt->data;
		p_ind->conn_handle = p_des->conn_handle;
		p_ind->srv_instance_id = p_des->srv_instance_id;
		p_ind->output_des.output_des_len = p_des->output_des.output_des_len + 1;
		p_ind->output_des.p_output_des = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_ind->output_des.output_des_len);
		if (!p_ind->output_des.p_output_des) {
			BT_LOGE("p_ind->p_data alloc fail, len = %d\r\n", p_ind->output_des.output_des_len);
			rtk_bt_event_free(p_evt);
			break;
		}
		memset(p_ind->output_des.p_output_des, 0, p_ind->output_des.output_des_len);
		memcpy((void *)p_ind->output_des.p_output_des, (void *)p_des->output_des.p_output_des, p_des->output_des.output_des_len);
		p_evt->user_data = p_ind->output_des.p_output_des;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
#endif
	default:
		// BT_LOGE("%s: unknown msg %x, not process\r\n", __func__, msg);
		// app_result = BLE_AUDIO_CB_RESULT_REJECT;
		break;
	}
	return app_result;
}

#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
static uint16_t bt_stack_le_audio_csis_gen_rsi(void *data)
{
	rtk_bt_le_audio_csis_gen_rsi_info_t *p_csis_gen_rsi_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_csis_gen_rsi_info = (rtk_bt_le_audio_csis_gen_rsi_info_t *)data;

	if (csis_gen_rsi(p_csis_gen_rsi_info->sirk, p_csis_gen_rsi_info->p_rsi) == false) {
		BT_LOGE("%s csis_gen_rsi fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_csis_update_sirk(void *data)
{
	uint8_t service_id = 0;
	T_ATTR_UUID srv_uuid = {0};
	rtk_bt_le_audio_csis_update_sirk_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_csis_update_sirk_param_t *)data;

	if (0 == memcmp(default_csis_sirk, p_param->sirk, RTK_BT_LE_CSIS_SIRK_LEN) &&
		default_csis_sirk_type == p_param->sirk_type) {
		BT_LOGE("%s csis_sirk not changed, skip\r\n", __func__);
		return RTK_BT_OK;
	}

	BT_LOGD("%s:p_param->sirk_type:%d\r\n", __func__, p_param->sirk_type);
	BT_DUMPD("", p_param->sirk, RTK_BT_LE_CSIS_SIRK_LEN);

	srv_uuid.is_uuid16 = true;
	srv_uuid.p.uuid16 = GATT_UUID_CAS;
	srv_uuid.instance_id = 0;
	if (false == csis_get_srv_id(&service_id, &srv_uuid)) {
		BT_LOGE("%s csis_get_srv_id fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	BT_LOGD("%s:service_id = %d\r\n", __func__, service_id);

	if (false == csis_update_sirk(service_id, p_param->sirk, (T_CSIS_SIRK_TYPE)p_param->sirk_type)) {
		BT_LOGE("%s csis_update_sirk fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	default_csis_sirk_type = p_param->sirk_type;
	memcpy((uint8_t *)default_csis_sirk, (uint8_t *)p_param->sirk, RTK_BT_LE_CSIS_SIRK_LEN);
	memcpy((uint8_t *)bt_le_audio_priv_data.lea_csis_sirk, (uint8_t *)default_csis_sirk, RTK_BT_LE_CSIS_SIRK_LEN);

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_csis_update_size(void *data)
{
	uint8_t service_id = 0;
	T_ATTR_UUID srv_uuid = {0};
	rtk_bt_le_audio_csis_update_size_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_csis_update_size_param_t *)data;

	if (default_csis_size == p_param->csis_size) {
		BT_LOGE("%s csis_size not changed, skip\r\n", __func__);
		return RTK_BT_OK;
	}

	BT_LOGD("%s:csis_size:%d\r\n", __func__, p_param->csis_size);

	srv_uuid.is_uuid16 = true;
	srv_uuid.p.uuid16 = GATT_UUID_CAS;
	srv_uuid.instance_id = 0;
	if (false == csis_get_srv_id(&service_id, &srv_uuid)) {
		BT_LOGE("%s csis_get_srv_id fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	BT_LOGD("%s:service_id = %d\r\n", __func__, service_id);

	if (false == csis_update_size(service_id, p_param->csis_size)) {
		BT_LOGE("%s csis_update_size fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	default_csis_size = p_param->csis_size;

	return RTK_BT_OK;
}
#endif


#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
static uint16_t bt_stack_le_audio_csis_set_coordinator_check_adv_rsi(void *data)
{
	rtk_bt_le_audio_csis_set_coordinator_check_adv_rsi_info_t *p_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_info = (rtk_bt_le_audio_csis_set_coordinator_check_adv_rsi_info_t *)data;

	if (set_coordinator_check_adv_rsi(p_info->report_data_len, p_info->p_report_data, p_info->adv_addr.addr_val, p_info->adv_addr.type) == false) {
		BT_LOGE("%s set_coordinator_check_adv_rsi fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_csis_set_coordinator_cfg_discover(void *data)
{
	rtk_bt_le_audio_csis_set_coordinator_cfg_discover_info_t *p_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_info = (rtk_bt_le_audio_csis_set_coordinator_cfg_discover_info_t *)data;

	if (set_coordinator_cfg_discover(p_info->group_handle, p_info->discover, p_info->timeout_ms) == false) {
		BT_LOGE("%s set_coordinator_cfg_discover fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_csis_set_coordinator_add_group(void *data)
{
	rtk_bt_le_audio_csis_set_coordinator_add_group_info_t *p_info = NULL;
	T_CSIS_SET_MEM_INFO mem_info = {0};

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_info = (rtk_bt_le_audio_csis_set_coordinator_add_group_info_t *)data;

	memcpy(mem_info.bd_addr, p_info->mem_info.bd_addr.addr_val, RTK_BD_ADDR_LEN);
	mem_info.addr_type = p_info->mem_info.bd_addr.type;
	mem_info.srv_uuid = p_info->mem_info.srv_uuid;
	mem_info.srv_instance_id = p_info->mem_info.srv_instance_id;
	mem_info.char_exit = p_info->mem_info.char_exit;
	mem_info.rank = p_info->mem_info.rank;
	mem_info.lock = (T_CSIS_LOCK)p_info->mem_info.lock;
	mem_info.set_mem_size = p_info->mem_info.set_mem_size;
	mem_info.sirk_type = (T_CSIS_SIRK_TYPE)p_info->mem_info.sirk_type;
	memcpy(mem_info.sirk, p_info->mem_info.sirk, RTK_BT_LE_CSIS_SIRK_LEN);

	if (set_coordinator_add_group(p_info->p_group_handle, bt_stack_le_audio_group_cb, p_info->p_device_handle, &mem_info) == false) {
		BT_LOGE("%s set_coordinator_check_add_group fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_csis_set_coordinator_add_device(void *data)
{
	rtk_bt_le_audio_csis_set_coordinator_add_device_info_t *p_info = NULL;
	T_CSIS_SET_MEM_INFO mem_info = {0};

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_info = (rtk_bt_le_audio_csis_set_coordinator_add_device_info_t *)data;

	memcpy(mem_info.bd_addr, p_info->mem_info.bd_addr.addr_val, RTK_BD_ADDR_LEN);
	mem_info.addr_type = p_info->mem_info.bd_addr.type;
	mem_info.srv_uuid = p_info->mem_info.srv_uuid;
	mem_info.srv_instance_id = p_info->mem_info.srv_instance_id;
	mem_info.char_exit = p_info->mem_info.char_exit;
	mem_info.rank = p_info->mem_info.rank;
	mem_info.lock = (T_CSIS_LOCK)p_info->mem_info.lock;
	mem_info.set_mem_size = p_info->mem_info.set_mem_size;
	mem_info.sirk_type = (T_CSIS_SIRK_TYPE)p_info->mem_info.sirk_type;
	memcpy(mem_info.sirk, p_info->mem_info.sirk, RTK_BT_LE_CSIS_SIRK_LEN);

	if (set_coordinator_add_dev(p_info->group_handle, p_info->p_device_handle, &mem_info) == false) {
		BT_LOGE("%s set_coordinator_add_dev fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_csis_set_coordinator_write_lock(void *data)
{
	rtk_bt_le_audio_csis_set_coordinator_write_lock_info_t *p_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_info = (rtk_bt_le_audio_csis_set_coordinator_write_lock_info_t *)data;

	if (p_info->lock == true) {
		if (set_coordinator_write_lock(p_info->group_handle) == false) {
			BT_LOGE("%s set_coordinator_write_lock fail\r\n", __func__);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	} else {
		if (set_coordinator_write_unlock(p_info->group_handle) == false) {
			BT_LOGE("%s set_coordinator_write_unlock fail\r\n", __func__);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	}

	return RTK_BT_OK;
}
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
static uint16_t bt_stack_le_audio_mcp_client_write_media_cp(void *data)
{
	T_MCP_CLIENT_WRITE_MEDIA_CP_PARAM mcp_param = {0};
	rtk_bt_le_audio_mcp_client_write_media_cp_info_t *p_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_info = (rtk_bt_le_audio_mcp_client_write_media_cp_info_t *)data;
	mcp_param.opcode = p_info->opcode;
	memcpy(&mcp_param.param, &p_info->op_param, sizeof(p_info->op_param));

	if (mcp_client_write_media_cp(p_info->conn_handle, p_info->srv_instance_id, p_info->general_mcs, &mcp_param, p_info->is_req) == false) {
		BT_LOGE("%s mcp_client_write_media_cp fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_mcp_client_read_char_value(void *data)
{
	rtk_bt_le_audio_mcp_client_read_char_value_info_t *p_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_info = (rtk_bt_le_audio_mcp_client_read_char_value_info_t *)data;

	BT_LOGD("%s conn_handle=0x%x,srv_instance_id=%d,char_uuid=0x%x,general_mcs=%d\r\n", __func__, p_info->conn_handle, p_info->srv_instance_id,
			p_info->char_uuid, p_info->general_mcs);

	if (mcp_client_read_char_value(p_info->conn_handle, p_info->srv_instance_id, p_info->char_uuid, p_info->general_mcs) == false) {
		BT_LOGE("%s mcp_client_read_char_value fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_mcp_client_cfg_cccd(void *data)
{
	rtk_bt_le_audio_mcp_client_cfg_cccd_info_t *p_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_info = (rtk_bt_le_audio_mcp_client_cfg_cccd_info_t *)data;

	BT_LOGD("%s conn_handle=0x%x,cfg_flags=0x%x,general_mcs=%d,srv_instance_id=%d,enable=%d\r\n", __func__, p_info->conn_handle,
			(unsigned int)p_info->cfg_flags, p_info->general_mcs, p_info->srv_instance_id, p_info->enable);

	mcp_client_cfg_cccd(p_info->conn_handle, p_info->cfg_flags, p_info->general_mcs, p_info->srv_instance_id, p_info->enable);

	return RTK_BT_OK;
}
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
static uint16_t bt_stack_le_audio_mcp_server_send_data(void *data)
{
	T_MCP_SERVER_SEND_DATA_PARAM send_param = {0};
	rtk_bt_le_audio_mcp_server_send_data_param_t *p_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_info = (rtk_bt_le_audio_mcp_server_send_data_param_t *)data;

	send_param.char_uuid = p_info->char_uuid;

	if (p_info->char_uuid == RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME) {
		send_param.param.media_player_name.p_media_player_name = p_info->param.media_player_name.p_media_player_name;
		send_param.param.media_player_name.media_player_name_len = p_info->param.media_player_name.media_player_name_len;
	} else if (p_info->char_uuid == RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE) {
		send_param.param.track_title.p_track_title = p_info->param.track_title.p_track_title;
		send_param.param.track_title.track_title_len = p_info->param.track_title.track_title_len;
	} else {
		memcpy(&send_param.param, &p_info->param, sizeof(p_info->param));
	}

	BT_LOGD("%s service_id %d, char_uuid=0x%x\r\n", __func__, bt_le_audio_priv_data.mcp.gmcs_id, p_info->char_uuid);

	if (false == mcp_server_send_data(bt_le_audio_priv_data.mcp.gmcs_id, &send_param)) {
		BT_LOGE("%s mcp_server_send_data fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_mcp_server_read_cfm(void *data)
{
	T_MCP_SERVER_READ_CFM read_cfm = {0};
	rtk_bt_le_audio_mcp_server_read_confirm_param_t *p_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_info = (rtk_bt_le_audio_mcp_server_read_confirm_param_t *)data;

	read_cfm.cause = BLE_AUDIO_CB_RESULT_SUCCESS;
	read_cfm.conn_handle = p_info->conn_handle;
	read_cfm.cid = p_info->cid;
	read_cfm.service_id = p_info->service_id;
	read_cfm.char_uuid = p_info->char_uuid;
	read_cfm.offset = p_info->offset;
	if (p_info->char_uuid == RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME) {
		read_cfm.param.media_player_name.p_media_player_name = p_info->param.media_player_name.p_media_player_name;
		read_cfm.param.media_player_name.media_player_name_len = p_info->param.media_player_name.media_player_name_len;
	} else if (p_info->char_uuid == RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE) {
		read_cfm.param.track_title.p_track_title = p_info->param.track_title.p_track_title;
		read_cfm.param.track_title.track_title_len = p_info->param.track_title.track_title_len;
	} else if (p_info->char_uuid == RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_ICON_URL) {
		read_cfm.param.media_player_icon_url.p_media_player_icon_url = p_info->param.media_player_icon_url.p_media_player_icon_url;
		read_cfm.param.media_player_icon_url.media_player_icon_url_len = p_info->param.media_player_icon_url.media_player_icon_url_len;
	} else {
		memcpy(&read_cfm.param, &p_info->param, sizeof(p_info->param));
	}

	BT_LOGD("%s service_id %d, char_uuid=0x%x\r\n", __func__, p_info->service_id, p_info->char_uuid);
	switch (p_info->char_uuid) {
	case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME:
		BT_LOGD("media_player_name_len %d, p_media_player_name %s\r\n", read_cfm.param.media_player_name.media_player_name_len,
				read_cfm.param.media_player_name.p_media_player_name);
		break;
	case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE:
		BT_LOGD("track_title_len %d, p_track_title %s\r\n", read_cfm.param.track_title.track_title_len, read_cfm.param.track_title.p_track_title);
		break;
	case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION:
		BT_LOGD("read track duration %d\r\n", (int)read_cfm.param.track_duration);
		break;
	case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION:
		BT_LOGD("read track position %d \r\n", (int)read_cfm.param.track_position);
		break;
	case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID:
		BT_LOGD("read content control id %d\r\n", read_cfm.param.content_control_id);
		break;
	default:
		BT_LOGE("%s unprocess char_uuid 0x%x for MCS server\r\n", __func__, p_info->char_uuid);
		return RTK_BT_OK;
	}

	if (false == mcp_server_read_cfm(&read_cfm)) {
		BT_LOGE("%s mcp_server_read_cfm fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
static uint16_t bt_stack_le_audio_vcs_param_set(void *data)
{
	T_VCS_PARAM vcs_param = {0};
	rtk_bt_le_audio_vcs_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_vcs_param_t *)data;

	vcs_param.volume_setting = p_param->volume_setting;
	vcs_param.mute = p_param->mute;
	vcs_param.change_counter = p_param->change_counter;
	vcs_param.volume_flags = p_param->volume_flags;
	vcs_param.step_size = p_param->step_size;

	if (false == vcs_set_param(&vcs_param)) {
		BT_LOGE("%s fail: vcs_set_param fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_vcs_param_get(void *data)
{
	T_VCS_PARAM vcs_param = {0};
	rtk_bt_le_audio_vcs_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_vcs_param_t *)data;

	if (false == vcs_get_param(&vcs_param)) {
		BT_LOGE("%s fail: vcs_get_param fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	p_param->volume_setting = vcs_param.volume_setting;
	p_param->mute = vcs_param.mute;
	p_param->change_counter = vcs_param.change_counter;
	p_param->volume_flags = vcs_param.volume_flags;
	p_param->step_size = vcs_param.step_size;

	return RTK_BT_OK;
}
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
static uint16_t bt_stack_le_audio_vcs_write_cp(void *data)
{
	T_VCS_VOLUME_CP_PARAM cp_param = {0};
	rtk_bt_le_audio_vcs_write_cp_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_vcs_write_cp_param_t *)data;

	if (VCS_CP_SET_ABSOLUTE_VOLUME == (T_VCS_CP_OP)p_param->cp_op) {
		cp_param.volume_setting = p_param->cp_param.volume_setting;
	}

	BT_LOGD("%s conn_handle=%d,cp_op=%d \r\n", __func__, p_param->conn_handle, p_param->cp_op);

	if (false == vcs_write_cp(p_param->conn_handle, (T_VCS_CP_OP)p_param->cp_op, &cp_param)) {
		BT_LOGE("%s fail: vcs_write_cp fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_vcs_get_volume_state(void *data)
{
	T_VOLUME_STATE volume_state = {0};
	rtk_bt_le_audio_vcs_volume_state_t *p_vcs_volume_state = NULL;
	rtk_bt_le_audio_vcs_get_volume_state_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_vcs_get_volume_state_param_t *)data;
	p_vcs_volume_state = p_param->p_vcs_volume_state;

	if (false == vcs_get_volume_state(p_param->conn_handle, &volume_state)) {
		BT_LOGE("%s fail: vcs_get_volume_state fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	p_vcs_volume_state->volume_setting = volume_state.volume_setting;
	p_vcs_volume_state->mute = volume_state.mute;
	p_vcs_volume_state->change_counter = volume_state.change_counter;

	BT_LOGD("%s volume_setting=%d,mute=%d,muchange_countere=%d \r\n", __func__, p_vcs_volume_state->volume_setting, p_vcs_volume_state->mute,
			p_vcs_volume_state->change_counter);

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_vcs_get_volume_flags(void *data)
{
	rtk_bt_le_audio_vcs_get_volume_flags_param_t *p_param = (rtk_bt_le_audio_vcs_get_volume_flags_param_t *)data;

	if (!p_param) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (false == vcs_get_volume_flags(p_param->conn_handle, &p_param->volume_flags)) {
		BT_LOGE("%s fail: vcs_get_volume_state fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_vcs_change_mute(void *data)
{
	T_LE_AUDIO_CAUSE cause = LE_AUDIO_CAUSE_SUCCESS;
	rtk_bt_le_audio_vcs_change_mute_param_t *p_param = NULL;
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_param = (rtk_bt_le_audio_vcs_change_mute_param_t *)data;
	p_group_info = bt_stack_le_audio_find_group_by_idx(p_param->group_idx);
	if (!p_group_info) {
		BT_LOGE("%s fail: get group info fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	BT_LOGD("%s mute %d\r\n", __func__, p_param->mute);
	cause = cap_vcs_change_mute(p_group_info->group_handle, p_param->mute);
	if (LE_AUDIO_CAUSE_SUCCESS != cause) {
		BT_LOGE("%s fail: cap_vcs_change_mute cause = 0x%x\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_vcs_change_volume(void *data)
{
	T_LE_AUDIO_CAUSE cause = LE_AUDIO_CAUSE_SUCCESS;
	rtk_bt_le_audio_vcs_change_volume_param_t *p_param = NULL;
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_param = (rtk_bt_le_audio_vcs_change_volume_param_t *)data;
	p_group_info = bt_stack_le_audio_find_group_by_idx(p_param->group_idx);
	if (!p_group_info) {
		BT_LOGE("%s fail: get group info fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	cause = cap_vcs_change_volume(p_group_info->group_handle, p_param->volume_setting);
	if (LE_AUDIO_CAUSE_SUCCESS != cause) {
		BT_LOGE("%s fail: cap_vcs_change_volume cause = 0x%x\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
static uint16_t bt_stack_le_audio_mics_change_mute(void *data)
{
	T_LE_AUDIO_CAUSE cause = LE_AUDIO_CAUSE_SUCCESS;
	rtk_bt_le_audio_mics_change_mute_param_t *p_param = NULL;
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_param = (rtk_bt_le_audio_mics_change_mute_param_t *)data;
	p_group_info = bt_stack_le_audio_find_group_by_idx(p_param->group_idx);
	if (!p_group_info) {
		BT_LOGE("%s fail: get group info fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	cause = cap_mics_change_mute(p_group_info->group_handle, (T_MICS_MUTE)p_param->mic_mute);
	if (LE_AUDIO_CAUSE_SUCCESS != cause) {
		BT_LOGE("%s fail: cap_mics_change_mute cause = 0x%x\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_mics_set_mute_value(void *data)
{
	rtk_bt_le_audio_mics_set_mute_value_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_mics_set_mute_value_param_t *)data;

	BT_LOGE("%s conn_handle=%d mic_mute=%d\r\n", __func__, p_param->conn_handle, p_param->mic_mute);

	if (false == mics_write_mute_value(p_param->conn_handle, (T_MICS_MUTE)p_param->mic_mute)) {
		BT_LOGE("%s fail: mics_write_mute_value fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_mics_get_mute_value(void *data)
{
	rtk_bt_le_audio_mics_get_mute_value_param_t *p_param = NULL;
	T_MICS_MUTE mic_mute = MICS_RFU;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_mics_get_mute_value_param_t *)data;

	if (false == mics_get_mute_value(p_param->conn_handle, &mic_mute)) {
		BT_LOGE("%s fail: mics_get_mute_value fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	BT_LOGE("%s conn_handle=%d mic_mute=%d\r\n", __func__, p_param->conn_handle, mic_mute);

	*p_param->p_mic_mute = (rtk_bt_le_audio_mics_mute_state_t)mic_mute;

	return RTK_BT_OK;
}
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
static uint16_t bt_stack_le_audio_mics_set_param(void *data)
{
	T_MICS_PARAM mics_param = {0};
	rtk_bt_le_audio_mics_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_mics_param_t *)data;

	mics_param.mic_mute = (T_MICS_MUTE)p_param->mic_mute;

	if (false == mics_set_param(&mics_param)) {
		BT_LOGE("%s fail: mics_set_param fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_mics_get_param(void *data)
{
	T_MICS_PARAM mics_param = {0};
	rtk_bt_le_audio_mics_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_mics_param_t *)data;

	if (false == mics_get_param(&mics_param)) {
		BT_LOGE("%s fail: mics_get_param fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	p_param->mic_mute = (rtk_bt_le_audio_mics_mute_state_t)mics_param.mic_mute;

	return RTK_BT_OK;
}
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
static uint16_t bt_stack_le_audio_vocs_set_param(void *data)
{
	T_VOCS_PARAM_SET vocs_param = {0};
	rtk_bt_le_audio_vocs_set_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_vocs_set_param_t *)data;

	vocs_param.set_mask = p_param->set_mask;
	vocs_param.volume_offset = p_param->vocs_param.volume_offset;
	vocs_param.change_counter = p_param->vocs_param.change_counter;
	vocs_param.audio_location = p_param->vocs_param.audio_location;
	vocs_param.output_des.output_des_len = p_param->vocs_param.output_des.output_des_len;
	vocs_param.output_des.p_output_des = p_param->vocs_param.output_des.p_output_des;

	if (false == vocs_set_param(p_param->srv_instance_id, &vocs_param)) {
		BT_LOGE("%s fail: vocs_set_param fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_vocs_get_param(void *data)
{
	T_VOCS_PARAM_GET vocs_param = {0};
	rtk_bt_le_audio_vocs_get_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_vocs_get_param_t *)data;

	if (false == vocs_get_param(p_param->srv_instance_id, &vocs_param)) {
		BT_LOGE("%s fail: vocs_get_param fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	p_param->p_vocs_param->volume_offset = vocs_param.volume_offset;
	p_param->p_vocs_param->change_counter = vocs_param.change_counter;
	p_param->p_vocs_param->audio_location = vocs_param.audio_location;
	p_param->p_vocs_param->output_des.output_des_len = vocs_param.output_des.output_des_len;

	//if p_output_des is NULL, APP want to query output descripiton length only
	if (p_param->p_vocs_param->output_des.p_output_des != NULL) {
		memcpy(p_param->p_vocs_param->output_des.p_output_des, vocs_param.output_des.p_output_des, vocs_param.output_des.output_des_len);
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_vocs_cfg_cccd(void *data)
{
	rtk_bt_le_audio_vocs_cfg_cccd_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_vocs_cfg_cccd_param_t *)data;

	BT_LOGD("%s conn_handle %d,srv_instance_id %d,cfg_flags 0x%x, enable %d\r\n", __func__, p_param->conn_handle, p_param->srv_instance_id,
			p_param->cfg_flags, p_param->enable);
	vocs_cfg_cccd(p_param->conn_handle, p_param->srv_instance_id, p_param->cfg_flags, p_param->enable);

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_vocs_read_char_value(void *data)
{
	rtk_bt_le_audio_vocs_read_char_value_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_vocs_read_char_value_param_t *)data;

	if (false == vocs_read_char_value(p_param->conn_handle, p_param->srv_instance_id, (T_VOCS_CHAR_TYPE)p_param->type)) {
		BT_LOGE("%s fail: vocs_read_char_value fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_vocs_write_cp(void *data)
{
	T_VOCS_CP_PARAM cp_param = {0};
	rtk_bt_le_audio_vocs_write_cp_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_vocs_write_cp_param_t *)data;

	cp_param.volume_offset = p_param->cp_param.volume_offset;

	BT_LOGD("%s conn_handle %d, srv_instance_id %d,cp_op 0x%x, volume_offset %d\r\n", __func__,
			p_param->conn_handle, p_param->srv_instance_id, p_param->cp_op, cp_param.volume_offset);

	if (false == vocs_write_cp(p_param->conn_handle, p_param->srv_instance_id, (T_VOCS_CP_OP)p_param->cp_op, &cp_param)) {
		BT_LOGE("%s fail: vocs_write_cp fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_vocs_write_cp_by_group(void *data)
{
	T_VOCS_CP_PARAM cp_param = {0};
	T_LE_AUDIO_CAUSE ret = LE_AUDIO_CAUSE_SUCCESS;
	rtk_bt_le_audio_vocs_write_cp_by_group_param_t *p_param = NULL;
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_param = (rtk_bt_le_audio_vocs_write_cp_by_group_param_t *)data;
	p_group_info = bt_stack_le_audio_find_group_by_idx(p_param->group_idx);
	if (!p_group_info) {
		BT_LOGE("%s fail: get group info fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	cp_param.volume_offset = p_param->cp_param.volume_offset;
	BT_LOGD("%s group_handle %08x, srv_instance_id %d,cp_op 0x%x,volume_offset %d\r\n", __func__,
			p_group_info->group_handle, p_param->srv_instance_id, p_param->cp_op, cp_param.volume_offset);

	ret = vocs_write_cp_by_group(p_group_info->group_handle, p_param->srv_instance_id, (T_VOCS_CP_OP)p_param->cp_op, &cp_param);
	if (LE_AUDIO_CAUSE_SUCCESS != ret) {
		BT_LOGE("%s fail: vocs_write_cp_by_group ret = 0x%x\r\n", __func__, ret);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_vocs_write_audio_location(void *data)
{
	T_LE_AUDIO_CAUSE ret = LE_AUDIO_CAUSE_SUCCESS;
	rtk_bt_le_audio_vocs_write_audio_location_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_vocs_write_audio_location_param_t *)data;

	BT_LOGD("%s conn_handle %d, srv_instance_id %d,audio_location 0x%x\r\n", __func__, p_param->conn_handle, p_param->srv_instance_id,
			(unsigned int)p_param->audio_location);

	ret = vocs_write_audio_location(p_param->conn_handle, p_param->srv_instance_id, p_param->audio_location);
	if (LE_AUDIO_CAUSE_SUCCESS != ret) {
		BT_LOGE("%s fail: vocs_write_audio_location ret = 0x%x\r\n", __func__, ret);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_vocs_write_output_des(void *data)
{
	T_LE_AUDIO_CAUSE ret = LE_AUDIO_CAUSE_SUCCESS;
	rtk_bt_le_audio_vocs_write_output_des_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_vocs_write_output_des_param_t *)data;

	BT_LOGD("%s conn_handle %d, srv_instance_id %d,output_des_len %d,p_output_des %s\r\n", __func__,
			p_param->conn_handle, p_param->srv_instance_id, p_param->output_des_len, p_param->p_output_des);

	if (false == vocs_write_output_des(p_param->conn_handle, p_param->srv_instance_id, p_param->p_output_des, p_param->output_des_len)) {
		BT_LOGE("%s fail: vocs_write_output_des ret = 0x%x\r\n", __func__, ret);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_vocs_get_srv_data(void *data)
{
	T_VOCS_SRV_DATA value = {0};
	rtk_bt_le_audio_vocs_get_srv_data_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_vocs_get_srv_data_param_t *)data;

	if (false == vocs_get_srv_data(p_param->conn_handle, p_param->srv_instance_id, &value)) {
		BT_LOGE("%s fail: vocs_get_srv_data fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	p_param->p_srv_data->srv_instance_id = value.srv_instance_id;
	p_param->p_srv_data->type_exist = value.type_exist;
	p_param->p_srv_data->volume_offset.volume_offset = value.volume_offset.volume_offset;
	p_param->p_srv_data->volume_offset.change_counter = value.volume_offset.change_counter;
	p_param->p_srv_data->audio_location = value.audio_location;
	p_param->p_srv_data->output_des.output_des_len = value.output_des.output_des_len;
	BT_LOGD("%s vocs_get_srv_data output_des_len %d,p_output_des %s\r\n", __func__, value.output_des.output_des_len,
			value.output_des.p_output_des);
	//if p_output_des is NULL, APP want to query output descripiton length only
	if (p_param->p_srv_data->output_des.output_des_len) {
		memcpy(p_param->p_srv_data->output_des.p_output_des, value.output_des.p_output_des, sizeof(value.output_des.output_des_len));
	}

	return RTK_BT_OK;
}
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
static uint16_t bt_stack_le_audio_aics_set_param(void *data)
{
	rtk_bt_le_audio_aics_set_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_aics_set_param_t *)data;

	BT_LOGD("%s srv_instance_id %d,aics_param_type 0x%x\r\n", __func__, p_param->srv_instance_id, p_param->aics_param_type);

	if (false == aics_set_param(p_param->srv_instance_id, (T_AICS_PARAM_TYPE)p_param->aics_param_type, p_param->value_len, p_param->p_value,
								p_param->set_change_counter)) {
		BT_LOGE("%s fail: aics_set_param fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_aics_get_param(void *data)
{
	rtk_bt_le_audio_aics_get_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_aics_get_param_t *)data;

	if (!p_param->p_value) {
		BT_LOGE("%s fail: returned param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (false == aics_get_param(p_param->srv_instance_id, (T_AICS_PARAM_TYPE)p_param->aics_param_type, p_param->p_value)) {
		BT_LOGE("%s fail: aics_get_param fail,srv_instance_id=%d,aics_param_type=%d\r\n", __func__, p_param->srv_instance_id,
				p_param->aics_param_type);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_aics_cfg_cccd(void *data)
{
	rtk_bt_le_audio_aics_cfg_cccd_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_aics_cfg_cccd_param_t *)data;

	BT_LOGD("%s conn_handle %d,srv_instance_id %d,cfg_flags 0x%x, enable %d\r\n", __func__, p_param->conn_handle, p_param->srv_instance_id,
			p_param->cfg_flags, p_param->enable);
	aics_cfg_cccd(p_param->conn_handle, p_param->srv_instance_id, p_param->cfg_flags, p_param->enable);

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_aics_read_char_value(void *data)
{
	rtk_bt_le_audio_aics_read_char_value_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_aics_read_char_value_param_t *)data;

	if (false == aics_read_char_value(p_param->conn_handle, p_param->srv_instance_id, (T_AICS_CHAR_TYPE)p_param->type)) {
		BT_LOGE("%s fail: aics_read_char_value fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_aics_write_cp(void *data)
{
	T_AICS_CP_PARAM cp_param = {0};
	rtk_bt_le_audio_aics_write_cp_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_aics_write_cp_param_t *)data;

	cp_param.gaining_setting = p_param->cp_param.gaining_setting;

	BT_LOGD("%s conn_handle %d, srv_instance_id %d,cp_op 0x%x, gaining_setting %d\r\n", __func__,
			p_param->conn_handle, p_param->srv_instance_id, p_param->cp_op, cp_param.gaining_setting);

	if (false == aics_write_cp(p_param->conn_handle, p_param->srv_instance_id, (T_AICS_CP_OP)p_param->cp_op, &cp_param)) {
		BT_LOGE("%s fail: aics_write_cp fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_aics_write_cp_by_group(void *data)
{
	T_AICS_CP_PARAM cp_param = {0};
	T_LE_AUDIO_CAUSE ret = LE_AUDIO_CAUSE_SUCCESS;
	rtk_bt_le_audio_aics_write_cp_by_group_param_t *p_param = NULL;
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_param = (rtk_bt_le_audio_aics_write_cp_by_group_param_t *)data;
	p_group_info = bt_stack_le_audio_find_group_by_idx(p_param->group_idx);
	if (!p_group_info) {
		BT_LOGE("%s fail: get group info fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	cp_param.gaining_setting = p_param->cp_param.gaining_setting;
	BT_LOGD("%s group_handle %08x, srv_instance_id %d,cp_op 0x%x,gaining_setting %d\r\n", __func__,
			p_group_info->group_handle, p_param->srv_instance_id, p_param->cp_op, cp_param.gaining_setting);
	ret = aics_write_cp_by_group(p_group_info->group_handle, p_param->srv_instance_id, (T_AICS_CP_OP)p_param->cp_op, &cp_param);
	if (LE_AUDIO_CAUSE_SUCCESS != ret) {
		BT_LOGE("%s fail: aics_write_cp_by_group ret = 0x%x\r\n", __func__, ret);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_aics_write_input_des(void *data)
{
	T_LE_AUDIO_CAUSE ret = LE_AUDIO_CAUSE_SUCCESS;
	rtk_bt_le_audio_aics_write_input_des_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_aics_write_input_des_param_t *)data;

	BT_LOGD("%s conn_handle %d, srv_instance_id %d,input_des_len %d,p_input_des %s\r\n", __func__,
			p_param->conn_handle, p_param->srv_instance_id, p_param->input_des_len, p_param->p_input_des);

	if (false == aics_write_input_des(p_param->conn_handle, p_param->srv_instance_id, p_param->p_input_des, p_param->input_des_len)) {
		BT_LOGE("%s fail: aics_write_input_des ret = 0x%x\r\n", __func__, ret);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_aics_get_srv_data(void *data)
{
	T_AICS_SRV_DATA value = {0};
	rtk_bt_le_audio_aics_get_srv_data_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_aics_get_srv_data_param_t *)data;

	if (false == aics_get_srv_data(p_param->conn_handle, p_param->srv_instance_id, &value)) {
		BT_LOGE("%s fail: aics_get_srv_data fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	p_param->p_srv_data->srv_instance_id = value.srv_instance_id;
	p_param->p_srv_data->type_exist = value.type_exist;
	p_param->p_srv_data->input_state.gain_setting = value.input_state.gain_setting;
	p_param->p_srv_data->input_state.mute = value.input_state.mute;
	p_param->p_srv_data->input_state.gain_mode = value.input_state.gain_mode;
	p_param->p_srv_data->input_state.change_counter = value.input_state.change_counter;
	p_param->p_srv_data->setting_prop.gain_setting_units = value.setting_prop.gain_setting_units;
	p_param->p_srv_data->setting_prop.gain_setting_min = value.setting_prop.gain_setting_min;
	p_param->p_srv_data->setting_prop.gain_setting_max = value.setting_prop.gain_setting_max;
	p_param->p_srv_data->input_type = value.input_type;
	p_param->p_srv_data->input_status = value.input_status;
	p_param->p_srv_data->input_des.input_des_len = value.input_des.input_des_len;

	//if p_input_des is NULL, APP want to query input descripiton length only
	if (p_param->p_srv_data->input_des.input_des_len) {
		memcpy(p_param->p_srv_data->input_des.p_input_des, value.input_des.p_input_des, sizeof(value.input_des.input_des_len));
	}

	return RTK_BT_OK;
}

#endif

static uint16_t bt_stack_le_audio_cap_param_config(void *data)
{
	rtk_bt_le_audio_cap_param_config_t *p_config = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error \r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (stack_cap_init_flag) {
		BT_LOGE("%s fail: cap param config should be invoked before initialization \r\n", __func__);
		return RTK_BT_FAIL;
	}
	p_config = (rtk_bt_le_audio_cap_param_config_t *)data;
	switch (p_config->cfg_type) {
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
	case RTK_BT_LE_AUDIO_CAP_CSIS_SIRK_CONFIG: {
		memcpy((void *)default_csis_sirk, (void *)p_config->cfg.csis_sirk, RTK_BT_LE_CSIS_SIRK_LEN);
		BT_LOGA("%s default_csis_sirk is configured \r\n", __func__);
		break;
	}
#endif
	default:
		BT_LOGE("%s: unknown cfg_type: 0x%02x \r\n", __func__, (uint8_t)p_config->cfg_type);
		return RTK_BT_FAIL;
		break;
	}

	return RTK_BT_OK;
}

uint16_t bt_stack_cap_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = 0;
	BT_LOGD("bt_stack_cap_act_handle: act = %d \r\n", p_cmd->act);
	switch (p_cmd->act) {

#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
	case RTK_BT_LE_AUDIO_ACT_CSIS_GEN_RSI: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_CSIS_GEN_RSI \r\n");
		ret = bt_stack_le_audio_csis_gen_rsi(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_CSIS_UPDATE_SIRK: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_CSIS_UPDATE_SIRK \r\n");
		ret = bt_stack_le_audio_csis_update_sirk(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_CSIS_UPDATE_SIZE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_CSIS_UPDATE_SIZE \r\n");
		ret = bt_stack_le_audio_csis_update_size(p_cmd->param);
		break;
	}
#endif
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
	case RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_CHECK_ADV_RSI: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_CHECK_ADV_RSI \r\n");
		ret = bt_stack_le_audio_csis_set_coordinator_check_adv_rsi(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_CFG_DISCOVER: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_CFG_DISCOVER \r\n");
		ret = bt_stack_le_audio_csis_set_coordinator_cfg_discover(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_ADD_GROUP: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_ADD_GROUP \r\n");
		ret = bt_stack_le_audio_csis_set_coordinator_add_group(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_ADD_DEVICE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_ADD_DEVICE \r\n");
		ret = bt_stack_le_audio_csis_set_coordinator_add_device(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_WRITE_LOCK: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_CSIS_SET_COORDINATOR_WRITE_LOCK \r\n");
		ret = bt_stack_le_audio_csis_set_coordinator_write_lock(p_cmd->param);
		break;
	}
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
	case RTK_BT_LE_AUDIO_ACT_MCP_SERVER_SEND_DATA: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_MCP_SERVER_SEND_DATA \r\n");
		ret = bt_stack_le_audio_mcp_server_send_data(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_MCP_SERVER_READ_CFM: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_MCP_SERVER_READ_CFM \r\n");
		ret = bt_stack_le_audio_mcp_server_read_cfm(p_cmd->param);
		break;
	}
#endif
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
	case RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_WRITE_MEDIA_CP: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_WRITE_MEDIA_CP \r\n");
		ret = bt_stack_le_audio_mcp_client_write_media_cp(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_READ_CHAR_VALUE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_READ_CHAR_VALUE \r\n");
		ret = bt_stack_le_audio_mcp_client_read_char_value(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_CFG_CCCD: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_MCP_CLIENT_CFG_CCCD \r\n");
		ret = bt_stack_le_audio_mcp_client_cfg_cccd(p_cmd->param);
		break;
	}
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
	case RTK_BT_LE_AUDIO_ACT_VCS_SET_PARAM: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VCS_SET_PARAM \r\n");
		ret = bt_stack_le_audio_vcs_param_set(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_VCS_GET_PARAM: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VCS_GET_PARAM \r\n");
		ret = bt_stack_le_audio_vcs_param_get(p_cmd->param);
		break;
	}
#endif
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
	case RTK_BT_LE_AUDIO_ACT_VCS_WRITE_CP: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VCS_WRITE_CP \r\n");
		ret = bt_stack_le_audio_vcs_write_cp(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_VCS_GET_VOLUME_STATE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VCS_GET_VOLUME_STATE \r\n");
		ret = bt_stack_le_audio_vcs_get_volume_state(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_VCS_GET_VOLUME_FLAGS: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VCS_GET_VOLUME_FLAGS \r\n");
		ret = bt_stack_le_audio_vcs_get_volume_flags(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_VCS_CHANGE_MUTE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VCS_CHANGE_MUTE \r\n");
		ret = bt_stack_le_audio_vcs_change_mute(p_cmd->param);
		break;
	}

	case RTK_BT_LE_AUDIO_ACT_VCS_CHANGE_VOLUME: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VCS_CHANGE_VOLUME \r\n");
		ret = bt_stack_le_audio_vcs_change_volume(p_cmd->param);
		break;
	}
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
	case RTK_BT_LE_AUDIO_ACT_MICS_CHANGE_MUTE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_MICS_CHANGE_MUTE \r\n");
		ret = bt_stack_le_audio_mics_change_mute(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_MICS_SET_MUTE_VALUE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_MICS_SET_MUTE_VALUE \r\n");
		ret = bt_stack_le_audio_mics_set_mute_value(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_MICS_GET_MUTE_VALUE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_MICS_GET_MUTE_VALUE \r\n");
		ret = bt_stack_le_audio_mics_get_mute_value(p_cmd->param);
		break;
	}
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
	case RTK_BT_LE_AUDIO_ACT_MICS_SET_PARAM: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_MICS_SET_PARAM \r\n");
		ret = bt_stack_le_audio_mics_set_param(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_MICS_GET_PARAM: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_MICS_GET_PARAM \r\n");
		ret = bt_stack_le_audio_mics_get_param(p_cmd->param);
		break;
	}
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	case RTK_BT_LE_AUDIO_ACT_VOCS_SET_PARAM: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VOCS_SET_PARAM \r\n");
		ret = bt_stack_le_audio_vocs_set_param(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_VOCS_GET_PARAM: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VOCS_GET_PARAM \r\n");
		ret = bt_stack_le_audio_vocs_get_param(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_VOCS_CFG_CCCD: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VOCS_CFG_CCCD \r\n");
		ret = bt_stack_le_audio_vocs_cfg_cccd(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_VOCS_READ_CHAR_VALUE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VOCS_READ_CHAR_VALUE \r\n");
		ret = bt_stack_le_audio_vocs_read_char_value(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_CP: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_CP \r\n");
		ret = bt_stack_le_audio_vocs_write_cp(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_CP_BY_GROUP: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_CP_BY_GROUP \r\n");
		ret = bt_stack_le_audio_vocs_write_cp_by_group(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_AUDIO_LOCATION: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_AUDIO_LOCATION \r\n");
		ret = bt_stack_le_audio_vocs_write_audio_location(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_OUTPUT_DES: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VOCS_WRITE_OUTPUT_DES \r\n");
		ret = bt_stack_le_audio_vocs_write_output_des(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_VOCS_GET_SRV_DATA: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_VOCS_GET_SRV_DATA \r\n");
		ret = bt_stack_le_audio_vocs_get_srv_data(p_cmd->param);
		break;
	}
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	case RTK_BT_LE_AUDIO_ACT_AICS_SET_PARAM: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_AICS_SET_PARAM \r\n");
		ret = bt_stack_le_audio_aics_set_param(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_AICS_GET_PARAM: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_AICS_GET_PARAM \r\n");
		ret = bt_stack_le_audio_aics_get_param(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_AICS_CFG_CCCD: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_AICS_CFG_CCCD \r\n");
		ret = bt_stack_le_audio_aics_cfg_cccd(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_AICS_READ_CHAR_VALUE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_AICS_READ_CHAR_VALUE \r\n");
		ret = bt_stack_le_audio_aics_read_char_value(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_AICS_WRITE_CP: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_AICS_WRITE_CP \r\n");
		ret = bt_stack_le_audio_aics_write_cp(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_AICS_WRITE_CP_BY_GROUP: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_AICS_WRITE_CP_BY_GROUP \r\n");
		ret = bt_stack_le_audio_aics_write_cp_by_group(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_AICS_WRITE_INPUT_DES: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_AICS_WRITE_INPUT_DES \r\n");
		ret = bt_stack_le_audio_aics_write_input_des(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_AICS_GET_SRV_DATA: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_AICS_GET_SRV_DATA \r\n");
		ret = bt_stack_le_audio_aics_get_srv_data(p_cmd->param);
		break;
	}
#endif
	case RTK_BT_LE_AUDIO_ACT_CAP_PARAM_CONFIG: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_CAP_PARAM_CONFIG \r\n");
		ret = bt_stack_le_audio_cap_param_config(p_cmd->param);
		break;
	}
	default:
		BT_LOGE("bt_stack_cap_act_handle: unknown act: %d \r\n", p_cmd->act);
		ret = 0;
		break;
	}

	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}

#if (defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT)
static uint16_t bt_stack_vc_mic_init(void *le_audio_app_conf)
{
	(void)le_audio_app_conf;
	T_BLE_AUDIO_VC_MIC_PARAMS vc_mic_param = {0};
	rtk_bt_le_audio_app_conf_t *p_le_audio_app_conf = NULL;
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	rtk_bt_le_audio_aics_init_param_t *p_aics_param = NULL;
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	rtk_bt_le_audio_vocs_init_param_t *p_vocs_param = NULL;
#endif
	BT_LOGA("[CAP] bt_stack_vc_mic_init\n");
	if (le_audio_app_conf == NULL) {
		BT_LOGE("%s le_audio_app_conf fail\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_le_audio_app_conf = (rtk_bt_le_audio_app_conf_t *)le_audio_app_conf;

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	p_aics_param = &p_le_audio_app_conf->cap_param.aics_param;
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	p_vocs_param = &p_le_audio_app_conf->cap_param.vocs_param;
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
	vc_mic_param.vcs_enable = true;
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	vc_mic_param.aics_vcs_num = p_aics_param->aics_vcs_num;
	vc_mic_param.p_aics_vcs_tbl = p_aics_param->p_aics_vcs_tbl;
	vc_mic_param.aics_total_num += vc_mic_param.aics_vcs_num;
	BT_LOGD("%s aics_vcs_num %d, aics_vcs_tbl[0] %d, aics_total_num %d\r\n", __func__, vc_mic_param.aics_vcs_num, *vc_mic_param.p_aics_vcs_tbl,
			vc_mic_param.aics_total_num);
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	vc_mic_param.vocs_num = p_vocs_param->vocs_num;
	vc_mic_param.p_vocs_feature_tbl = p_vocs_param->p_vocs_feature_tbl;
	BT_LOGD("%s vocs_num %d, vocs_feature_tbl[0] %d\r\n", __func__, vc_mic_param.vocs_num, *vc_mic_param.p_vocs_feature_tbl);
#endif
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
	vc_mic_param.mics_enable = true;
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	vc_mic_param.aics_mics_num = p_aics_param->aics_mics_num;
	vc_mic_param.p_aics_mics_tbl = p_aics_param->p_aics_mics_tbl;
	vc_mic_param.aics_total_num += vc_mic_param.aics_mics_num;
	BT_LOGD("%s aics_mics_num %d, aics_mics_tbl[0] %d, aics_total_num %d\r\n", __func__, vc_mic_param.aics_mics_num,
			*vc_mic_param.p_aics_mics_tbl, vc_mic_param.aics_total_num);
#endif
#endif

	if (false == ble_audio_vc_mic_init(&vc_mic_param)) {
		BT_LOGE("%s:ble_audio_vc_mic_init fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	(void)p_le_audio_app_conf;

	return RTK_BT_OK;
}
#endif

#if (defined (RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT)
uint16_t bt_stack_csis_cap_init(uint8_t csip_role, T_CAP_INIT_PARAMS *p_param, rtk_bt_le_audio_csis_init_param_t *p_csis_param)
{
	char *role[] = {"Set Coordinator", "Set Member"};

	BT_LOGA("[CAP] bt_stack_csis_cap_init, role %s \n", role[csip_role]);
#if defined (RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
	if (RTK_BT_CSIP_ROLE_SET_COORDINATOR == csip_role) {
		p_param->csip_set_coordinator = true;
	}
#endif
#if defined (RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
	if (RTK_BT_CSIP_ROLE_SET_MEMBER == csip_role) {
		memcpy(bt_le_audio_priv_data.lea_csis_sirk, default_csis_sirk, RTK_BT_LE_CSIS_SIRK_LEN);
		p_param->csis_num = p_csis_param->csis_num;
		p_param->cas.enable = true;
		p_param->cas.csis_sirk_type = p_csis_param->csis_sirk_type;
		p_param->cas.csis_size = p_csis_param->csis_size;
		if (p_csis_param->csis_cfg == RTK_BT_LEA_CSIS_CFG_RANK_1) {
			p_param->cas.csis_rank = 1;
		} else if (p_csis_param->csis_cfg == RTK_BT_LEA_CSIS_CFG_RANK_2) {
			p_param->cas.csis_rank = 2;
		} else {
			BT_LOGE("[CAP] csis_cfg(0x%x) not exist \n", p_csis_param->csis_cfg);
			BT_LOGE("[CAP] set default csis_cfg(0x01) \n");
			p_param->cas.csis_rank = 1;
		}
		p_param->cas.csis_feature = p_csis_param->csis_feature;
		p_param->cas.csis_sirk = p_csis_param->csis_sirk;
	}
#endif
	return RTK_BT_OK;
}

void bt_stack_csis_deinit(void)
{
	BT_LOGA("[CAP] bt_stack_csis_deinit\n");
}
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
static uint16_t bt_stack_mcs_server_init(void *le_audio_app_conf)
{
	T_MCP_SERVER_REG_SRV_PARAM reg_param = {0};
	T_MCP_SERVER_SET_PARAM set_param = {0};
	rtk_bt_le_audio_mcs_init_param_t *p_mcs_param = NULL;
	rtk_bt_le_audio_app_conf_t *p_le_audio_app_conf = NULL;
	rtk_bt_le_audio_mcp_t *p_mcp = &bt_le_audio_priv_data.mcp;

	BT_LOGA("[CAP] bt_stack_mcs_server_init\n");
	p_le_audio_app_conf = (rtk_bt_le_audio_app_conf_t *)le_audio_app_conf;
	p_mcs_param = &p_le_audio_app_conf->cap_param.mcs_param;
	reg_param.gmcs = p_mcs_param->general_mcs;
	reg_param.char_media_control_point.support = p_mcs_param->char_media_control_point_support;
	reg_param.char_track_duration.optional_property_notify = p_mcs_param->char_track_duration_optional_property_notify;
	reg_param.char_track_position.optional_property_notify = p_mcs_param->char_track_position_optional_property_notify;
	p_mcp->gmcs_id = mcp_server_reg_srv(&reg_param);
	if (p_mcp->gmcs_id != 0xFF) {
		set_param.char_uuid = MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED;
		set_param.param.media_control_point_opcodes_supported =
			(MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PLAY |
			 MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PAUSE |
			 MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_FAST_REWIND |
			 MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_FAST_FORWARD |
			 MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_STOP |
			 MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PREVIOUS_TRACK |
			 MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_NEXT_TRACK);
		if (false == mcp_server_set_param(p_mcp->gmcs_id, &set_param)) {
			BT_LOGE("%s:mcp_server_set_param char_uuid 0x%x fail\r\n", __func__, set_param.char_uuid);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
		set_param.char_uuid = MCS_UUID_CHAR_MEDIA_STATE;
		set_param.param.media_state = MCS_MEDIA_STATE_PAUSED;
		if (false == mcp_server_set_param(p_mcp->gmcs_id, &set_param)) {
			BT_LOGE("%s:mcp_server_set_param char_uuid 0x%x fail\r\n", __func__, set_param.char_uuid);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
		set_param.char_uuid = MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED;
		set_param.param.media_control_point_opcodes_supported = p_mcs_param->media_control_point_opcodes_supported;
		if (false == mcp_server_set_param(p_mcp->gmcs_id, &set_param)) {
			BT_LOGE("%s:mcp_server_set_param char_uuid 0x%x fail\r\n", __func__, set_param.char_uuid);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
		p_mcp->media_state = MCS_MEDIA_STATE_PAUSED;
	} else {
		BT_LOGE("%s:mcp_server_reg_srv fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	BT_LOGD("%s:mcs_service_id=0x%x\r\n", __func__, p_mcp->gmcs_id);

	return RTK_BT_OK;
}

uint16_t bt_stack_mcs_server_cap_init(T_CAP_INIT_PARAMS *p_param)
{
	BT_LOGA("[CAP] bt_stack_mcs_server_cap_init\n");
	p_param->mcs.mcp_media_control_server = true;
	p_param->mcs.mcs_num = 1;
	p_param->mcs.ots_num = 0;

	return RTK_BT_OK;
}

void bt_stack_mcp_server_cap_deinit(void)
{
	BT_LOGA("[CAP] bt_stack_mcp_server_cap_deinit\n");
}
#endif

#if defined (RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
uint16_t bt_stack_mcp_client_cap_init(T_CAP_INIT_PARAMS *p_param)
{
	BT_LOGA("[CAP] bt_stack_mcp_client_cap_init\n");
	p_param->mcp_media_control_client = true;

	return RTK_BT_OK;
}

void bt_stack_mcp_client_cap_deinit(void)
{
	BT_LOGA("[CAP] bt_stack_mcp_client_cap_deinit\n");
}
#endif

uint16_t bt_stack_cap_init(void *p_conf)
{
	rtk_bt_le_audio_app_conf_t *p_le_audio_app_conf = NULL;
	T_CAP_INIT_PARAMS cap_init_param = {0};
	uint16_t ret = RTK_BT_FAIL;

	BT_LOGA("[CAP] bt_stack_cap_init\n");
	if (!p_conf) {
		BT_LOGE("[CAP] p_conf is NULL \n");
		return RTK_BT_FAIL;
	} else {
		p_le_audio_app_conf = (rtk_bt_le_audio_app_conf_t *)p_conf;
	}
	if (p_le_audio_app_conf->cap_role == RTK_BT_LE_AUDIO_CAP_ROLE_UNKNOWN) {
		BT_LOGE("%s set cap role according cap role\r\n", __func__);
		return ret;
	}
	ble_audio_cback_register(rtk_stack_le_audio_cap_msg_cback);
#if (defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT)
	if (RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR & p_le_audio_app_conf->cap_role) {
		ret = bt_stack_vc_mic_init(p_le_audio_app_conf);
		if (ret != RTK_BT_OK) {
			BT_LOGE("%s:bt_stack_le_audio_vcs_init fail,ret= 0x%x\r\n", __func__, ret);
			return ret;
		}
	}
#endif
	/* cap initiator */
	if (RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR & p_le_audio_app_conf->cap_role) {
		cap_init_param.cap_role |= CAP_INITIATOR_ROLE;
		cap_init_param.cas_client = true;
#if defined (RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
		bt_stack_csis_cap_init(RTK_BT_CSIP_ROLE_SET_COORDINATOR, &cap_init_param, &p_le_audio_app_conf->cap_param.csis_param);
#endif
#if defined (RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
		cap_init_param.vcp_micp.vcp_vcs_client = true;
#endif
#if defined (RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
		cap_init_param.vcp_micp.micp_mic_controller = true;
#endif
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
		bt_stack_mcs_server_cap_init(&cap_init_param);
#endif
	}
	/* cap commander */
	if (RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER & p_le_audio_app_conf->cap_role) {
		cap_init_param.cap_role |= CAP_COMMANDER_ROLE;
		cap_init_param.cas_client = true;
#if defined (RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
		bt_stack_csis_cap_init(RTK_BT_CSIP_ROLE_SET_COORDINATOR, &cap_init_param, &p_le_audio_app_conf->cap_param.csis_param);
#endif
#if defined (RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
		cap_init_param.vcp_micp.vcp_vcs_client = true;
#endif
#if defined (RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
		cap_init_param.vcp_micp.micp_mic_controller = true;
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
		cap_init_param.vcp_micp.vcp_aics_client = true;
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
		cap_init_param.vcp_micp.vcp_vocs_client = true;
#endif
	}
	/* cap acceptor */
	if (RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR & p_le_audio_app_conf->cap_role) {
		cap_init_param.cap_role |= CAP_ACCEPTOR_ROLE;
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
		if (p_le_audio_app_conf->cap_param.csis_param.csis_cfg != RTK_BT_LEA_CSIS_CFG_NOT_EXIST) {
			bt_stack_csis_cap_init(RTK_BT_CSIP_ROLE_SET_MEMBER, &cap_init_param, &p_le_audio_app_conf->cap_param.csis_param);
		}
#endif
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
		bt_stack_mcp_client_cap_init(&cap_init_param);
#endif
	}
	if (cap_init(&cap_init_param) == false) {
		BT_LOGE("%s:cap_init fail, return value is false\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
	if (p_le_audio_app_conf->cap_role & RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR) {
		bt_stack_mcs_server_init(p_le_audio_app_conf);
	}
#endif
	stack_cap_init_flag = 1;

	return RTK_BT_OK;
}

void bt_stack_cap_deinit(void)
{
	stack_cap_init_flag = 0;
	BT_LOGA("[CAP]bt_stack_cap_deinit\n");
}

#endif