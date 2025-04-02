/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <string.h>
#include <stdio.h>
#include <bt_api_config.h>
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
#include <osif.h>
#include <ble_audio_flags.h>
#include <ble_audio.h>
#include <gmas_mgr.h>
#include <gmas_def.h>
#include <gmas_client.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_stack_le_audio_internal.h>
#include <rtk_stack_config.h>
#include <rtk_bt_le_audio_def.h>
#include <rtk_bt_gmap.h>
#include <trace_app.h>

static uint16_t rtk_stack_le_audio_gmas_msg_cback(T_LE_AUDIO_MSG msg, void *buf)
{
	uint16_t app_result = BLE_AUDIO_CB_RESULT_SUCCESS;
	rtk_bt_evt_t *p_evt = NULL;

	switch (msg) {
	/* GMAS client event */
	case LE_AUDIO_MSG_GMAS_CLIENT_DIS_DONE: {
		T_GMAS_CLIENT_DIS_DONE *discovery_ind = (T_GMAS_CLIENT_DIS_DONE *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_GMAS_CLIENT_DIS_DONE: conn_handle %x,is_found %d,load_from_ftl %d\r\n", discovery_ind->conn_handle, discovery_ind->is_found,
						discovery_ind->load_from_ftl);
		BT_LOGD("LE_AUDIO_MSG_GMAS_CLIENT_DIS_DONE: conn_handle %x,is_found %d,load_from_ftl %d\r\n", discovery_ind->conn_handle,
				discovery_ind->is_found, discovery_ind->load_from_ftl);
		rtk_bt_le_audio_gmas_client_dis_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GMAP,
									RTK_BT_LE_AUDIO_GMAS_EVT_CLIENT_DISCOVERY_DONE_IND,
									sizeof(rtk_bt_le_audio_gmas_client_dis_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_gmas_client_dis_t *)p_evt->data;
		p_ind->conn_handle = discovery_ind->conn_handle;
		p_ind->is_found = discovery_ind->is_found;
		p_ind->load_from_ftl = discovery_ind->load_from_ftl;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
		break;
	}
	case LE_AUDIO_MSG_GMAS_CLIENT_READ_ROLE_RESULT: {
		T_GMAS_CLIENT_READ_ROLE_RESULT *read_res = (T_GMAS_CLIENT_READ_ROLE_RESULT *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_GMAS_CLIENT_READ_ROLE_RESULT: conn_handle 0x%x,role 0x%x,cause %d \r\n", read_res->conn_handle, read_res->gmap_role,
						read_res->cause);
		BT_LOGD("LE_AUDIO_MSG_GMAS_CLIENT_READ_ROLE_RESULT: conn_handle 0x%x,role 0x%x, cause %d\r\n", read_res->conn_handle, read_res->gmap_role,
				read_res->cause);
		rtk_bt_le_audio_gmas_client_read_role_result_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GMAP,
									RTK_BT_LE_AUDIO_GMAS_EVT_CLIENT_READ_ROLE_RESULT,
									sizeof(rtk_bt_le_audio_gmas_client_read_role_result_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_gmas_client_read_role_result_t *)p_evt->data;
		p_ind->conn_handle = read_res->conn_handle;
		p_ind->gmap_role = read_res->gmap_role;
		p_ind->cause = read_res->cause;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
		break;
	}
	case LE_AUDIO_MSG_GMAS_CLIENT_READ_FEATURE_RESULT: {
		T_GMAS_CLIENT_READ_FEATURES_RESULT *read_feat = (T_GMAS_CLIENT_READ_FEATURES_RESULT *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_GMAS_CLIENT_READ_FEATURE_RESULT: conn_handle 0x%x,char_uuid 0x%x,features 0x%x,cause %d \r\n",
						read_feat->conn_handle, read_feat->char_uuid, read_feat->features, read_feat->cause);
		BT_LOGD("LE_AUDIO_MSG_GMAS_CLIENT_READ_FEATURE_RESULT: conn_handle 0x%x,char_uuid 0x%x,features 0x%x,cause %d\r\n",
				read_feat->conn_handle, read_feat->char_uuid, read_feat->features, read_feat->cause);
		rtk_bt_le_audio_gmas_client_read_features_result_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GMAP,
									RTK_BT_LE_AUDIO_GMAS_EVT_CLIENT_READ_FEATURES_RESULT,
									sizeof(rtk_bt_le_audio_gmas_client_read_features_result_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_gmas_client_read_features_result_t *)p_evt->data;
		p_ind->conn_handle = read_feat->conn_handle;
		p_ind->char_uuid = read_feat->char_uuid;
		p_ind->features = read_feat->features;
		p_ind->cause = read_feat->cause;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
		break;
	}
	/* GMAS server event */
	case LE_AUDIO_MSG_GMAS_READ_IND: {
		T_GMAS_READ_IND *read_ind = (T_GMAS_READ_IND *)buf;
		uint8_t features;
		APP_PRINT_INFO3("LE_AUDIO_MSG_GMAS_READ_IND: conn_handle %x,cid %d,char_uuid %d\r\n", read_ind->conn_handle, read_ind->cid,
						read_ind->char_uuid);
		BT_LOGD("LE_AUDIO_MSG_GMAS_READ_IND: conn_handle %x,cid %d,char_uuid %d\r\n", read_ind->conn_handle,
				read_ind->cid, read_ind->char_uuid);

		switch (read_ind->char_uuid) {
		case GMAS_UUID_CHAR_UGG_FEATURES: {
			features = UGG_FEATURES_CHAR_BIT_VALUE_UGG_96K_SOURCE_FEATURE_SUP;
			break;
		}
		case GMAS_UUID_CHAR_UGT_FEATURES: {
			features = UGT_FEATURES_CHAR_BIT_VALUE_UGT_SOURCE_FEATURE_SUP |
					   UGT_FEATURES_CHAR_BIT_VALUE_UGT_80K_SOURCE_FEATURE_SUP |
					   UGT_FEATURES_CHAR_BIT_VALUE_UGT_SINK_FEATURE_SUP;
			break;
		}
		case GMAS_UUID_CHAR_BGS_FEATURES: {
			features = BGS_FEATURES_CHAR_BIT_VALUE_BGS_96K_FEATURE_SUP;
			break;
		}
		case GMAS_UUID_CHAR_BGR_FEATURES: {
			features = BGR_FEATURES_CHAR_BIT_VALUE_BGR_MULTISINK_FEATURE_SUP |
					   BGR_FEATURES_CHAR_BIT_VALUE_BGR_MULTIPLEX_FEATURE_SUP;
			break;
		}
		default:
			BT_LOGE("%s: unknown char_uuid 0x%x, not process\r\n", __func__, read_ind->char_uuid);
			return BLE_AUDIO_CB_RESULT_APP_ERR;
		}
		if (!gmas_features_read_confirm(read_ind->conn_handle, read_ind->cid,
										read_ind->char_uuid, features)) {
			BT_LOGE("%s: gmas_features_read_confirm error\r\n", __func__);
			return BLE_AUDIO_CB_RESULT_APP_ERR;
		}

		rtk_bt_le_audio_gmas_read_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GMAP,
									RTK_BT_LE_AUDIO_GMAS_EVT_READ_IND,
									sizeof(rtk_bt_le_audio_gmas_read_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_gmas_read_ind_t *)p_evt->data;
		p_ind->conn_handle = read_ind->conn_handle;
		p_ind->cid = read_ind->cid;
		p_ind->char_uuid = read_ind->char_uuid;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
		break;
	}

	default:
		break;
	}

	return app_result;
}

static uint16_t bt_stack_le_audio_gmas_client_read_role(void *data)
{
	uint16_t conn_handle = 0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	conn_handle = *((uint16_t *)data);

	BT_LOGD("%s conn_handle %d\r\n", __func__, conn_handle);

	if (false == gmas_read_gmap_role(conn_handle)) {
		BT_LOGE("%s fail: gmas_read_role failed \r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_gmas_client_read_features(void *data)
{
	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	rtk_bt_le_audio_gmas_client_read_features_t *info = (rtk_bt_le_audio_gmas_client_read_features_t *)data;

	if (info->char_uuid != GMAS_UUID_CHAR_UGG_FEATURES && info->char_uuid != GMAS_UUID_CHAR_UGT_FEATURES && \
		info->char_uuid != GMAS_UUID_CHAR_BGS_FEATURES && info->char_uuid != GMAS_UUID_CHAR_BGR_FEATURES) {
		BT_LOGE("%s fail: illegal input char_uuid\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (false == gmas_read_features(info->conn_handle, info->char_uuid)) {
		BT_LOGE("%s fail: gmas_read_role failed \r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

uint16_t bt_stack_gmap_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = 0;
	BT_LOGD("bt_stack_gmap_act_handle: act = %d \r\n", p_cmd->act);
	switch (p_cmd->act) {
	case RTK_BT_LE_AUDIO_GMAS_ACT_READ_ROLE: {
		BT_LOGD("RTK_BT_LE_AUDIO_GMAS_ACT_READ_ROLE \r\n");
		ret = bt_stack_le_audio_gmas_client_read_role(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_GMAS_ACT_READ_FEATURES: {
		BT_LOGD("RTK_BT_LE_AUDIO_GMAS_ACT_READ_FEATURES \r\n");
		ret = bt_stack_le_audio_gmas_client_read_features(p_cmd->param);
		break;
	}
	default:
		BT_LOGE("bt_stack_gmap_act_handle: unknown act: %d \r\n", p_cmd->act);
		ret = 0;
		break;
	}

	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}

uint16_t bt_stack_gmap_init(void *p_conf)
{
	rtk_bt_le_audio_gmap_role_t gmap_role = 0;
	rtk_bt_le_audio_app_conf_t *p_le_audio_app_conf = NULL;

	if (p_conf == NULL) {
		BT_LOGE("%s p_conf fail\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_le_audio_app_conf = (rtk_bt_le_audio_app_conf_t *)p_conf;

	if ((p_le_audio_app_conf->gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_UGT) ||
		(p_le_audio_app_conf->gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGR)) {
		gmap_role |= p_le_audio_app_conf->gmap_role;
		if (gmas_init(gmap_role) == false) {
			BT_LOGE("%s:gmas_init fail, return value is false\r\n", __func__);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	}
	if ((p_le_audio_app_conf->gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_UGG) || (p_le_audio_app_conf->gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGS)) {
		if (gmas_client_init() == false) {
			BT_LOGE("%s:gmas_client_init fail, return value is false\r\n", __func__);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	}
	if (ble_audio_cback_register(rtk_stack_le_audio_gmas_msg_cback) == false) {
		BT_LOGE("%s: ble_audio_cback_register failed \r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	BT_LOGA("[GMAP] bt_stack_gmap_init\n");
	return RTK_BT_OK;
}

void bt_stack_gmap_deinit(void)
{
	BT_LOGA("[GMAP] bt_stack_gmap_deinit\n");
}

#endif /* RTK_BLE_AUDIO_GMAP_SUPPORT */