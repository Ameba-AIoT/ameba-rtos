/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <string.h>
#include <stdio.h>
#include <bt_api_config.h>
#if defined(CONFIG_BT_TMAP_SUPPORT) && CONFIG_BT_TMAP_SUPPORT
#include <osif.h>
#include <ble_audio_flags.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_stack_le_audio_internal.h>
#include <rtk_stack_config.h>
#include <rtk_bt_le_audio_def.h>
#include <rtk_bt_tmap.h>
#include <ble_audio.h>
#include <tmas_mgr.h>
#include <tmas_client.h>
#include <tmas_def.h>
#include <trace_app.h>

uint16_t rtk_stack_le_audio_tmas_msg_cback(T_LE_AUDIO_MSG msg, void *buf)
{
	uint16_t app_result = BLE_AUDIO_CB_RESULT_SUCCESS;
	rtk_bt_evt_t *p_evt = NULL;

	switch (msg) {
	case LE_AUDIO_MSG_TMAS_CLIENT_DIS_DONE: {
		T_TMAS_CLIENT_DIS_DONE *discovery_ind = (T_TMAS_CLIENT_DIS_DONE *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_TMAS_CLIENT_DIS_DONE: conn_handle %x,is_found %d,load_from_ftl %d\r\n", discovery_ind->conn_handle, discovery_ind->is_found,
						discovery_ind->load_from_ftl);
		BT_LOGD("LE_AUDIO_MSG_TMAS_CLIENT_DIS_DONE: conn_handle %x,is_found %d,load_from_ftl %d\r\n", discovery_ind->conn_handle,
				discovery_ind->is_found, discovery_ind->load_from_ftl);
		rtk_bt_le_audio_tmas_client_discovery_done_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_TMAP,
									RTK_BT_LE_AUDIO_EVT_TMAS_CLIENT_DISCOVERY_DONE_IND,
									sizeof(rtk_bt_le_audio_tmas_client_discovery_done_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_tmas_client_discovery_done_ind_t *)p_evt->data;
		p_ind->conn_handle = discovery_ind->conn_handle;
		p_ind->is_found = discovery_ind->is_found;
		p_ind->load_from_ftl = discovery_ind->load_from_ftl;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
	case LE_AUDIO_MSG_TMAS_CLIENT_READ_ROLE_RESULT: {
		T_TMAS_CLIENT_READ_ROLE_RESULT *read_res = (T_TMAS_CLIENT_READ_ROLE_RESULT *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_TMAS_CLIENT_READ_ROLE_RESULT: conn_handle 0x%x,role 0x%x,cause %d \r\n", read_res->conn_handle, read_res->role, read_res->cause);
		BT_LOGD("LE_AUDIO_MSG_TMAS_CLIENT_READ_ROLE_RESULT: conn_handle 0x%x,role 0x%x, cause %d\r\n", read_res->conn_handle, read_res->role,
				read_res->cause);
		rtk_bt_le_audio_tmas_read_role_result_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_TMAP,
									RTK_BT_LE_AUDIO_EVT_TMAS_CLIENT_READ_ROLE_RESULT,
									sizeof(rtk_bt_le_audio_tmas_read_role_result_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_tmas_read_role_result_t *)p_evt->data;
		p_ind->conn_handle = read_res->conn_handle;
		p_ind->role = read_res->role;
		p_ind->cause = read_res->cause;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
	default:
		// BT_LOGE("%s: unknown msg %x, not process\r\n", __func__, msg);
		// app_result = BLE_AUDIO_CB_RESULT_REJECT;
		break;
	}

	return app_result;
}

static uint16_t bt_stack_le_audio_tmas_client_read_role(void *data)
{
	uint16_t p_conn_handle = 0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_conn_handle = *((uint16_t *)data);

	BT_LOGD("%s conn_handle %d\r\n", __func__, p_conn_handle);

	if (false == tmas_read_role(p_conn_handle)) {
		BT_LOGE("%s fail: tmas_read_role failed \r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

uint16_t bt_stack_tmap_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = 0;
	BT_LOGD("bt_stack_tmap_act_handle: act = %d \r\n", p_cmd->act);
	switch (p_cmd->act) {
	case RTK_BT_LE_AUDIO_ACT_TMAS_READ_ROLE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_TMAS_READ_ROLE \r\n");
		ret = bt_stack_le_audio_tmas_client_read_role(p_cmd->param);
		break;
	}
	default:
		BT_LOGE("bt_stack_tmap_act_handle: unknown act: %d \r\n", p_cmd->act);
		ret = 0;
		break;
	}

	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}

uint16_t bt_stack_tmap_init(void *p_conf)
{
	rtk_bt_le_audio_app_conf_t *p_le_audio_app_conf = NULL;
	uint16_t tmas_role = 0;

	BT_LOGA("[TMAP] bt_stack_tmap_init\n");
	if (!p_conf) {
		BT_LOGE("[TMAP] p_conf is NULL \n");
		return RTK_BT_FAIL;
	} else {
		p_le_audio_app_conf = (rtk_bt_le_audio_app_conf_t *)p_conf;
	}
	if ((p_le_audio_app_conf->tmap_role & RTK_BT_LE_AUDIO_TMAP_ROLE_CT) || (p_le_audio_app_conf->tmap_role & RTK_BT_LE_AUDIO_TMAP_ROLE_UMR) ||
		(p_le_audio_app_conf->tmap_role & RTK_BT_LE_AUDIO_TMAP_ROLE_BMR)) {
		tmas_role |= p_le_audio_app_conf->tmap_role;
		if (tmas_init(tmas_role) == false) {
			BT_LOGE("%s:tmas_init fail, return value is false\r\n", __func__);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	} else {
		if (tmas_client_init() == false) {
			BT_LOGE("%s:tmas_client_init fail, return value is false\r\n", __func__);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	}
	ble_audio_cback_register(rtk_stack_le_audio_tmas_msg_cback);

	return RTK_BT_OK;
}

void bt_stack_tmap_deinit(void)
{
	BT_LOGA("[TMAP] bt_stack_tmap_deinit\n");
}
#endif