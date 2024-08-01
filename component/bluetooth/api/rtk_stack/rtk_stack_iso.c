/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <string.h>
#include <stdio.h>
#include <osif.h>
#include "bt_api_config.h"
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#include "trace_app.h"
#include "gap_le.h"
#include "app_msg.h"
#include "rtk_stack_internal.h"
#include "gap_iso_data.h"
#include "bt_direct_msg.h"
#include "rtk_bt_common.h"
#include "rtk_bt_le_iso.h"
#include "rtk_bt_device.h"
#include "timer_api.h"
#include "ble_isoch_def.h"
#include <gap_conn_le.h>
#include "gap_cig_mgr.h"
#include "gap_big_mgr.h"
static uint16_t g_bt_stack_le_iso_bis_data_send_max_sdu = RTK_BLE_ISO_MAX_SDU_M_S;
rtk_bt_le_iso_role_t g_bt_stack_le_iso_role = RTK_BLE_ISO_ROLE_UNKNOWN;

typedef struct {
	uint32_t param;         /*!< param of the CIS or BIS*/
	void *info;             /*!< point to io message info to be sent */
} rtk_bt_le_iso_io_msg_buf_t;

T_APP_RESULT g_cis_request_ind_ret = APP_RESULT_PENDING;

extern uint16_t bt_stack_msg_send(uint16_t type, uint16_t subtype, void *msg);

#define IO_MSG_TYPE_LE_MGR_SUBTYPE_SEND_ISO_DATA 0x0001
#define IO_MSG_TYPE_LE_MGR_SUBTYPE_SEND_ISO_DATA_PARAM_BIS_BIT 0x0001
#define IO_MSG_TYPE_LE_MGR_SUBTYPE_SEND_ISO_DATA_PARAM_CIS_BIT 0x0002

extern bool rtk_bt_check_evt_cb_direct_calling(uint8_t group, uint8_t evt_code);
void bt_stack_le_iso_data_direct_callback(uint8_t cb_type, void *p_cb_data)
{
	rtk_bt_evt_t *p_evt = NULL;
	T_BT_DIRECT_CB_DATA *p_data = (T_BT_DIRECT_CB_DATA *)p_cb_data;

	switch (cb_type) {
	case BT_DIRECT_MSG_ISO_DATA_IND: {
		BT_LOGD("BT_DIRECT_MSG_ISO_DATA_IND, conn_handle 0x%x, pkt_status_flag 0x%x, pkt_seq_num 0x%x, ts_flag 0x%x, time_stamp 0x%x,iso_sdu_len 0x%x, p_buf %08x, offset %d\r\n",
				p_data->p_bt_direct_iso->conn_handle, p_data->p_bt_direct_iso->pkt_status_flag,
				p_data->p_bt_direct_iso->pkt_seq_num, p_data->p_bt_direct_iso->ts_flag,
				(unsigned int)p_data->p_bt_direct_iso->time_stamp,
				p_data->p_bt_direct_iso->iso_sdu_len, p_data->p_bt_direct_iso->p_buf, p_data->p_bt_direct_iso->offset);
		if (p_data->p_bt_direct_iso->iso_sdu_len) {
			BT_DUMPD("", p_data->p_bt_direct_iso->p_buf + p_data->p_bt_direct_iso->offset, p_data->p_bt_direct_iso->iso_sdu_len);
		}
		/* Send event */
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_ISO,
									RTK_BT_LE_ISO_EVT_DATA_RECEIVE_IND,
									sizeof(rtk_bt_le_iso_direct_iso_data_ind_t));
		if (p_evt) {
			rtk_bt_le_iso_direct_iso_data_ind_t *direct_iso_data_ind = (rtk_bt_le_iso_direct_iso_data_ind_t *)p_evt->data;
			//the definition of rtk_bt_le_iso_direct_iso_data_ind_t and T_BT_DIRECT_ISO_DATA_IND are different,cannot use memcpy
			//memcpy((void *)direct_iso_data_ind, (void *)p_data->p_bt_direct_iso, sizeof(rtk_bt_le_iso_direct_iso_data_ind_t));
			direct_iso_data_ind->iso_conn_handle = p_data->p_bt_direct_iso->conn_handle;
			direct_iso_data_ind->pkt_status_flag = (rtk_bt_le_iso_isoch_data_pkt_status_t)p_data->p_bt_direct_iso->pkt_status_flag;
			direct_iso_data_ind->offset = p_data->p_bt_direct_iso->offset;
			direct_iso_data_ind->iso_sdu_len = p_data->p_bt_direct_iso->iso_sdu_len;
			direct_iso_data_ind->pkt_seq_num = p_data->p_bt_direct_iso->pkt_seq_num;
			direct_iso_data_ind->ts_flag = p_data->p_bt_direct_iso->ts_flag;
			direct_iso_data_ind->time_stamp = p_data->p_bt_direct_iso->time_stamp;
			direct_iso_data_ind->buf_len = p_data->p_bt_direct_iso->offset + p_data->p_bt_direct_iso->iso_sdu_len;
			/* 2nd level ptr in write event msg need deep copy */
			direct_iso_data_ind->p_buf = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, direct_iso_data_ind->buf_len);
			memset(direct_iso_data_ind->p_buf, 0, direct_iso_data_ind->buf_len);
			memcpy((void *)direct_iso_data_ind->p_buf, (void *)p_data->p_bt_direct_iso->p_buf, direct_iso_data_ind->buf_len);
			/*  user_data point to the memory alloced for 2nd level ptr, so it's convenient
			    to free it when free p_evt */
			p_evt->user_data = direct_iso_data_ind->p_buf;
			rtk_bt_evt_indicate(p_evt, NULL);
		} else {
			BT_LOGE("%s: evt_t allocate fail \r\n", __func__);
		}

		gap_iso_data_cfm(p_data->p_bt_direct_iso->p_buf);
	}
	break;

	default:
		BT_LOGE("%s unhandled cb_type 0x%x", __func__, cb_type);
		break;
	}
}

bool bt_stack_le_iso_init(void)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	/* cis part */
	bool cis_flag = true;
	cause = cig_mgr_init(RTK_BLE_CIG_MAX_NUM, RTK_BLE_CIS_MAX_NUM);
	if (cause != GAP_CAUSE_SUCCESS) {
		BT_LOGE("%s cig_mgr_init fail (cause = 0x%x)\r\n", __func__, cause);
		return false;
	}

	cause = le_set_gap_param(GAP_PARAM_CIS_HOST_SUPPORT, sizeof(cis_flag), &cis_flag);
	if (cause != GAP_CAUSE_SUCCESS) {
		BT_LOGE("%s le_set_gap_param fail (cause = 0x%x)\r\n", __func__, cause);
		return false;
	}
	/* bis part */
	uint8_t big_handle_num = (RTK_BLE_BIG_BROADCASTER_MAX_NUM > RTK_BLE_BIG_RECEIVER_MAX_NUM) ? RTK_BLE_BIG_BROADCASTER_MAX_NUM : RTK_BLE_BIG_RECEIVER_MAX_NUM;
	uint8_t bis_num = (RTK_BLE_BIS_BROADCASTER_MAX_NUM > RTK_BLE_BIS_RECEIVER_MAX_NUM) ? RTK_BLE_BIS_BROADCASTER_MAX_NUM : RTK_BLE_BIS_RECEIVER_MAX_NUM;
	BT_LOGD("%s big_handle_num = 0x%x, bis_num = 0x%x\r\n", __func__, big_handle_num, bis_num);
	cause = gap_big_mgr_init(big_handle_num, bis_num);
	if (cause != GAP_CAUSE_SUCCESS) {
		BT_LOGE("%s cig_mgr_init fail (cause = 0x%x)\r\n", __func__, cause);
		return false;
	}

	g_bt_stack_le_iso_role = RTK_BLE_ISO_ROLE_UNKNOWN;
	gap_register_direct_cb(bt_stack_le_iso_data_direct_callback);

	return true;
}

void bt_stack_le_iso_deinit(void)
{
	g_bt_stack_le_iso_role = RTK_BLE_ISO_ROLE_UNKNOWN;
	gap_register_direct_cb(NULL);
}


static uint16_t bt_stack_le_iso_data_send(void *data)
{
	T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;
	rtk_bt_le_iso_data_send_info_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	param = (rtk_bt_le_iso_data_send_info_t *)data;

	BT_DUMPD(__func__, param->p_data, param->data_len);

#if 0
	static bool is_odd = false;
	static uint32_t last_even_time = 0, now_even_time = 0, last_odd_time = 0, now_odd_time = 0;
	if (is_odd) {
		now_odd_time = osif_sys_time_get();
		if (last_odd_time != 0 && (now_odd_time - last_odd_time > 11 || now_odd_time - last_odd_time < 9)) {
			BT_LOGE("%s gap_iso_send_data iso_conn_handle = 0x%x,pkt_seq_num %d,last_odd_time %d, now_odd_time %d,delta_time %d\r\n", __func__,
					param->iso_conn_handle, param->pkt_seq_num, (int)last_odd_time, (int)now_odd_time, (int)(now_odd_time - last_odd_time));
		}
		last_odd_time = now_odd_time;
		is_odd = false;
	} else {
		now_even_time = osif_sys_time_get();
		if (last_even_time != 0 && (now_even_time - last_even_time > 11 || now_even_time - last_even_time < 9)) {
			BT_LOGE("%s gap_iso_send_data iso_conn_handle = 0x%x,pkt_seq_num %d,last_even_time %d, now_even_time %d,delta_time %d\r\n", __func__,
					param->iso_conn_handle, param->pkt_seq_num, (int)last_even_time, (int)now_even_time, (int)(now_even_time - last_even_time));
		}
		last_even_time = now_even_time;
		is_odd = true;
	}

#endif

	cause = gap_iso_send_data(param->p_data,
							  param->iso_conn_handle,
							  param->data_len,
							  param->ts_flag,
							  param->time_stamp,
							  param->pkt_seq_num);
	if (GAP_CAUSE_SUCCESS != cause) {
		if (cause == GAP_CAUSE_ERROR_CREDITS) {
			BT_LOGE("%s gap_iso_send_data warning (cause = 0x%x,iso_conn_handle = 0x%x)\r\n", __func__, cause, param->iso_conn_handle);
			return RTK_BT_ERR_NO_CREDITS;
		} else {
			BT_LOGE("%s gap_iso_send_data fail (cause = 0x%x,iso_conn_handle = 0x%x)\r\n", __func__, cause, param->iso_conn_handle);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	} else {
		BT_LOGD("%s gap_iso_send_data ok,iso_conn_handle=0x%x,pkt_seq_num=%d,time_stamp=%d, time = %d\r\n", __func__, param->iso_conn_handle,
				param->pkt_seq_num, (int)param->time_stamp, (int)osif_sys_time_get());
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_get_conn_handle(void *data)
{
	uint8_t conn_id = 0;
	rtk_bt_le_iso_cig_get_conn_handle_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_cig_get_conn_handle_param_t *)data;
	if (!param->cis_conn_handle) {
		BT_LOGE("%s fail: cis_conn_handle error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (true != cig_mgr_get_conn_id(param->cis_conn_handle, &conn_id)) {
		BT_LOGE("%s cig_mgr_get_conn_id fail (cis_conn_handle = 0x%x)\r\n", __func__, param->cis_conn_handle);
		return RTK_BT_ERR_LOWER_STACK_API;
	};

	*param->p_conn_handle = le_get_conn_handle(conn_id);

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_get_cis_info(void *data)
{
	T_CIS_INFO *p_info = NULL;
	rtk_bt_le_iso_cig_get_cis_info_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_cig_get_cis_info_param_t *)data;

	if (!param->cis_conn_handle) {
		BT_LOGE("%s fail: cis_conn_handle error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_info = (T_CIS_INFO *)param->p_cis_info;

	if (true != cig_mgr_get_cis_info(param->cis_conn_handle, p_info)) {
		BT_LOGE("%s cig_mgr_get_cis_info fail (cis_conn_handle = 0x%x)\r\n", __func__, param->cis_conn_handle);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_get_isoch_info(void *data)
{
	T_ISOCH_INFO *p_info = NULL;
	rtk_bt_le_iso_cig_get_isoch_info_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_cig_get_isoch_info_param_t *)data;

	if (!param->cis_conn_handle) {
		BT_LOGE("%s fail: cis_conn_handle error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_info = (T_ISOCH_INFO *)param->p_isoch_info;

	if (true != cig_mgr_get_isoch_info(param->cis_conn_handle, p_info)) {
		BT_LOGE("%s cig_mgr_get_isoch_info fail (cis_conn_handle = 0x%x)\r\n", __func__, param->cis_conn_handle);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_setup_path(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	rtk_bt_le_iso_setup_path_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_setup_path_param_t *)data;

	if (!param->iso_conn_handle) {
		BT_LOGE("%s fail: iso_conn_handle error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cause = cig_mgr_setup_data_path(param->iso_conn_handle, (uint8_t)param->data_path_direction, param->data_path_id,
									param->codec_id, param->controller_delay, param->codec_config_len, param->p_codec_config);
	if (cause) {
		BT_LOGE("%s fail: cig_mgr_setup_data_path fail cause = 0x%x\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_remove_path(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	rtk_bt_le_iso_remove_path_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_remove_path_param_t *)data;

	if (!param->iso_conn_handle) {
		BT_LOGE("%s fail: iso_conn_handle error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cause = cig_mgr_remove_data_path(param->iso_conn_handle, (uint8_t)param->data_path_direction);

	if (cause) {
		BT_LOGE("%s fail: cause = 0x%x data_path_direction=%d\r\n", __func__, cause, param->data_path_direction);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}


static uint16_t bt_stack_le_iso_cig_disconnect(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint8_t reason = HCI_ERR_REMOTE_USER_TERMINATE;
	uint16_t cis_conn_handle = 0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cis_conn_handle = *(uint16_t *)data;

	cause = cig_mgr_disconnect(cis_conn_handle, reason);
	if (cause) {
		BT_LOGE("%s fail: cig_mgr_disconnect fail cause = 0x%x\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_read_iso_tx_sync(void *data)
{
	rtk_bt_le_iso_cig_read_iso_tx_sync_t *param = NULL;
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_cig_read_iso_tx_sync_t *)data;

	if (!param->cis_conn_handle) {
		BT_LOGE("%s fail: cis_conn_handle error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	//If sending request operation is success, the reading result will be returned by initiator and acceptor cb
	//MSG_CIG_MGR_READ_ISO_TX_SYNC
	cause = cig_mgr_read_iso_tx_sync(param->cis_conn_handle);
	if (cause) {
		BT_LOGE("%s fail: cig_mgr_read_iso_tx_sync fail cause = 0x%x\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_read_link_quality(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	rtk_bt_le_iso_cig_read_link_quality_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	param = (rtk_bt_le_iso_cig_read_link_quality_t *)data;

	if (!param->cis_conn_handle) {
		BT_LOGE("%s fail: cis_conn_handle error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	//If sending request operation is success, the reading result will be returned by initiator and acceptor cb
	//MSG_CIG_MGR_READ_ISO_LINK_QUALITY
	cause = cig_mgr_read_iso_link_quality(param->cis_conn_handle);
	if (cause) {
		BT_LOGE("%s fail: cig_mgr_read_iso_link_quality fail cause = 0x%x\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_initiator_add_cis(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint8_t cig_id = 0;
	uint8_t cis_id = 0;
	rtk_bt_le_iso_cig_cis_info_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_cig_cis_info_t *)data;

	cig_id = param->cig_id;
	cis_id = param->cis_id;

	if (cig_id > RTK_BLE_CIG_MAX_NUM || cis_id > RTK_BLE_CIS_MAX_NUM) {
		return RTK_BT_ERR_NO_RESOURCE;
	}
#if 0
	BT_LOGD("%s cig_id=%d\r\n", __func__, cig_id);
	BT_LOGD("%s cis_id=%d\r\n", __func__, cis_id);
#endif
	cause = cig_mgr_add_cis(cig_id, cis_id);
	if (cause) {
		BT_LOGE("%s cig_mgr_add_cis fail(cause = 0x%x,cig_id = 0x%x,cis_id = 0x%x)\r\n", __func__, cause, cig_id, cis_id);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_initiator_set_cig_param(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint8_t cig_id = 0;
	T_CIG_CFG_PARAM cig_param = {0};
	rtk_bt_le_iso_cig_initiator_set_cig_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_cig_initiator_set_cig_param_t *)data;

	cig_id = param->cig_id;

	if (cig_id > RTK_BLE_CIG_MAX_NUM) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cig_param.sdu_interval_m_s = param->sdu_interval_m_s;
	cig_param.sdu_interval_s_m = param->sdu_interval_s_m;
	cig_param.sca = param->sca;
	cig_param.packing = param->packing;
	cig_param.framing = param->framing;
	cig_param.latency_m_s = param->latency_m_s;
	cig_param.latency_s_m = param->latency_s_m;
#if 0
	BT_LOGD("%s cig_id=%d\r\n", __func__, cig_id);
	BT_LOGD("%s cig_param.sdu_interval_m_s=%d\r\n", __func__, cig_param.sdu_interval_m_s);
	BT_LOGD("%s cig_param.sdu_interval_s_m=%d\r\n", __func__, cig_param.sdu_interval_s_m);
	BT_LOGD("%s cig_param.sca=%d\r\n", __func__, cig_param.sca);
	BT_LOGD("%s cig_param.packing=%d\r\n", __func__, cig_param.packing);
	BT_LOGD("%s cig_param.framing=%d\r\n", __func__, cig_param.framing);
	BT_LOGD("%s cig_param.latency_m_s=%d\r\n", __func__, cig_param.latency_m_s);
	BT_LOGD("%s cig_param.latency_s_m=%d\r\n", __func__, cig_param.latency_s_m);
#endif
	cause = cig_mgr_set_cig_param(cig_id, &cig_param);
	if (cause) {
		BT_LOGE("%s cig_mgr_set_cig_param fail(cause = 0x%x,cig_id = 0x%x)\r\n", __func__, cause, cig_id);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_initiator_set_cis_param(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint8_t cis_id = 0;
	T_CIS_CFG_PARAM cis_param = {0};
	rtk_bt_le_iso_cig_initiator_set_cis_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_cig_initiator_set_cis_param_t *)data;

	cis_id = param->cis_id;

	if (cis_id > RTK_BLE_CIS_MAX_NUM) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cis_param.max_sdu_m_s = param->max_sdu_m_s;
	cis_param.max_sdu_s_m = param->max_sdu_s_m;
	cis_param.phy_m_s = param->phy_m_s;
	cis_param.phy_s_m = param->phy_s_m;
	cis_param.rtn_m_s = param->rtn_m_s;
	cis_param.rtn_s_m = param->rtn_s_m;

	BT_LOGD("%s cis_id=%d max_sdu_m_s=%d max_sdu_s_m=%d phy_m_s=%d phy_s_m=%d rtn_m_s=%d rtn_s_m=%d\r\n",
			__func__, cis_id, cis_param.max_sdu_m_s, cis_param.max_sdu_s_m, cis_param.phy_m_s, cis_param.phy_s_m, cis_param.rtn_m_s, cis_param.rtn_s_m);

	cause = cig_mgr_set_cis_param(cis_id, &cis_param);
	if (cause) {
		BT_LOGE("%s cig_mgr_set_cis_param fail(cause = 0x%x,cis_id = 0x%x)\r\n", __func__, cause, cis_id);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_initiator_start_setting(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint8_t cig_id = 0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cig_id = *(uint8_t *)data;

	if (cig_id > RTK_BLE_CIG_MAX_NUM) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
#if 0
	BT_LOGD("%s cig_id=%d\r\n", __func__, cig_id);
#endif
	cause = cig_mgr_start_setting(cig_id);
	if (cause) {
		BT_LOGE("%s cig_mgr_start_setting fail(cause = 0x%x,cig_id = 0x%x)\r\n", __func__, cause, cig_id);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_initiator_set_cis_acl_link(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint8_t cis_id = 0;
	uint8_t conn_id = 0;
	rtk_bt_le_iso_cig_initiator_set_cis_acl_link_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_cig_initiator_set_cis_acl_link_param_t *)data;

	cis_id = param->cis_id;

	if (cis_id > RTK_BLE_CIS_MAX_NUM) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (false == le_get_conn_id_by_handle(param->conn_handle, &conn_id)) {
		BT_LOGE("%s le_get_conn_id_by_handle fail(conn_handle = 0x%x)\r\n", __func__, param->conn_handle);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	BT_LOGD("%s cis_id=%d,conn_id=%d\r\n", __func__, cis_id, conn_id);

	cause = cig_mgr_set_cis_acl_link(cis_id, conn_id);
	if (cause) {
		BT_LOGE("%s cig_mgr_set_cis_acl_link fail(cause = 0x%x, cis_id = 0x%x, conn_id = 0x%x)\r\n", __func__, cause, cis_id, conn_id);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_initiator_create_cis_by_cig_id(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint8_t cig_id = 0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cig_id = *(uint8_t *)data;

	if (cig_id > RTK_BLE_CIG_MAX_NUM) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	BT_LOGD("%s cig_id=%d\r\n", __func__, cig_id);

	cause = cig_mgr_create_cis_by_cig_id(cig_id);
	if (cause) {
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_initiator_create_cis_by_cis_conn_handle(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint8_t cig_id = 0;
	uint8_t cis_count = 0;
	uint16_t *p_cis_conn_handle = NULL;
	rtk_bt_le_iso_cig_initiator_create_cis_by_cis_conn_handle_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_cig_initiator_create_cis_by_cis_conn_handle_param_t *)data;

	cig_id = param->cig_id;

	if (cig_id > RTK_BLE_CIG_MAX_NUM) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cis_count = param->cis_count;
	p_cis_conn_handle = param->p_cis_conn_handle;

	cause = cig_mgr_create_cis_by_cis_conn_handle(cig_id, cis_count, p_cis_conn_handle);
	if (cause) {
		BT_LOGE("%s cig_mgr_create_cis_by_cis_conn_handle fail(cause = 0x%x,cig_id = 0x%x)\r\n", __func__, cause, cig_id);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_initiator_get_cis_conn_handle(void *data)
{
	uint8_t cis_id = 0;
	uint16_t *p_cis_conn_handle = 0;
	rtk_bt_le_iso_cig_initiator_get_cis_handle_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_cig_initiator_get_cis_handle_param_t *)data;

	cis_id = param->cis_id;

	if (cis_id > RTK_BLE_CIS_MAX_NUM) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_cis_conn_handle = param->p_cis_handle;

	if (true != cig_mgr_get_cis_handle(cis_id, p_cis_conn_handle)) {
		BT_LOGE("%s cig_mgr_get_cis_handle fail (cis_id = 0x%x)\r\n", __func__, cis_id);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_initiator_remove_cig(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint8_t cig_id = 0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cig_id = *(uint8_t *)data;

	if (cig_id > RTK_BLE_CIG_MAX_NUM) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cause = cig_mgr_remove_cig(cig_id);
	if (cause) {
		BT_LOGE("%s fail (cause = 0x%x,cig_id = 0x%x)\r\n", __func__, cause, cig_id);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_acceptor_accept_cis(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint16_t cis_conn_handle = 0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cis_conn_handle = *(uint16_t *)data;

	cause = cig_mgr_acceptor_accept_cis(cis_conn_handle);
	if (cause) {
		BT_LOGE("%s cig_mgr_acceptor_accept_cis fail (cause = 0x%x,cis_conn_handle = 0x%x)\r\n", __func__, cause, cis_conn_handle);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_acceptor_reject_cis(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint16_t cis_conn_handle = 0;
	uint8_t reason = 0;
	rtk_bt_le_iso_cig_acceptor_reject_cis_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_cig_acceptor_reject_cis_param_t *)data;
	cis_conn_handle = param->cis_conn_handle;
	reason = param->reason;

	cause = cig_mgr_acceptor_reject_cis(cis_conn_handle, reason);
	if (cause) {
		BT_LOGE("%s cig_mgr_acceptor_reject_cis fail (cause = 0x%x,cis_conn_handle = 0x%x)\r\n", __func__, cause, cis_conn_handle);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_cig_acceptor_config_sdu_param(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint16_t cis_conn_handle = 0;
	bool acceptor_config_sdu_flag = false;
	uint32_t sdu_interval_m_s = 0;
	uint32_t sdu_interval_s_m = 0;
	uint16_t max_sdu_m_s = 0;
	uint16_t max_sdu_s_m = 0;
	rtk_bt_le_iso_cig_acceptor_config_sdu_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_cig_acceptor_config_sdu_param_t *)data;
	cis_conn_handle = param->cis_conn_handle;
	acceptor_config_sdu_flag = param->acceptor_config_sdu_flag;
	sdu_interval_m_s = param->sdu_interval_m_s;
	sdu_interval_s_m = param->sdu_interval_s_m;
	max_sdu_m_s = param->max_sdu_m_s;
	max_sdu_s_m = param->max_sdu_s_m;

	cause = cig_mgr_acceptor_config_sdu_param(cis_conn_handle, acceptor_config_sdu_flag, sdu_interval_m_s, sdu_interval_s_m, max_sdu_m_s, max_sdu_s_m);
	if (GAP_CAUSE_SUCCESS != cause) {
		BT_LOGE("%s cig_mgr_acceptor_config_sdu_param fail(cause = 0x%x,cis_conn_handle = 0x%x)\r\n", __func__, cause, cis_conn_handle);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t  bt_stack_le_iso_cig_acceptor_config_cis_req_action(void *data)
{
	rtk_bt_le_iso_cig_acceptor_config_cis_req_action_t action = (rtk_bt_le_iso_cig_acceptor_config_cis_req_action_t)0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	action = *(rtk_bt_le_iso_cig_acceptor_config_cis_req_action_t *)data;
	switch (action) {
	case RTK_BLE_ISO_ACCEPTOR_CIS_REQ_ACTION_ACCEPT:
		g_cis_request_ind_ret = APP_RESULT_ACCEPT;
		break;
	case RTK_BLE_ISO_ACCEPTOR_CIS_REQ_ACTION_REJECT:
		g_cis_request_ind_ret = APP_RESULT_REJECT;
		break;
	case RTK_BLE_ISO_ACCEPTOR_CIS_REQ_ACTION_PENDING:
		g_cis_request_ind_ret = APP_RESULT_PENDING;
		break;
	default:
		BT_LOGE("%s error action value (0x%x)\r\n", __func__, action);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	return RTK_BT_OK;
}
rtk_bt_le_iso_cig_acceptor_config_cis_req_action_t convert_stack_app_result_to_cis_req_action(T_APP_RESULT app_result)
{
	rtk_bt_le_iso_cig_acceptor_config_cis_req_action_t cis_req_action = RTK_BLE_ISO_ACCEPTOR_CIS_REQ_ACTION_ACCEPT;
	switch (app_result) {
	case APP_RESULT_ACCEPT:
		cis_req_action =  RTK_BLE_ISO_ACCEPTOR_CIS_REQ_ACTION_ACCEPT;
		break;
	case APP_RESULT_REJECT:
		cis_req_action = RTK_BLE_ISO_ACCEPTOR_CIS_REQ_ACTION_REJECT;
		break;
	case APP_RESULT_PENDING:
		cis_req_action = RTK_BLE_ISO_ACCEPTOR_CIS_REQ_ACTION_PENDING;
		break;
	default:
		BT_LOGE("%s: unknown app_result(0x%x)\r\n", __func__, app_result);
		break;
	}
	return cis_req_action;
}
T_APP_RESULT bt_stack_le_iso_cig_initiator_cb(uint8_t cig_id, uint8_t cb_type, void *p_cb_data)
{
	T_APP_RESULT result = APP_RESULT_SUCCESS;
	T_CIG_MGR_CB_DATA *p_data = (T_CIG_MGR_CB_DATA *)p_cb_data;
	rtk_bt_cmd_t *p_cmd = NULL;
	rtk_bt_evt_t *p_evt = NULL;
	UNUSED(cig_id);

	BT_LOGD("%s: cig_id %d, cb_type 0x%x\r\n", __func__, cig_id, cb_type);

	switch (cb_type) {
	case MSG_CIG_MGR_START_SETTING: {
		BT_LOGD("MSG_CIG_MGR_START_SETTING: cause 0x%x, cig_id %d, cis_count %d\r\n",
				p_data->p_cig_mgr_start_setting_rsp->cause,
				p_data->p_cig_mgr_start_setting_rsp->cig_id,
				p_data->p_cig_mgr_start_setting_rsp->cis_count);
		APP_PRINT_INFO3("MSG_CIG_MGR_START_SETTING: cause 0x%x, cig_id %d, cis_count %d",
						p_data->p_cig_mgr_start_setting_rsp->cause,
						p_data->p_cig_mgr_start_setting_rsp->cig_id,
						p_data->p_cig_mgr_start_setting_rsp->cis_count);
		if ((p_data->p_cig_mgr_start_setting_rsp->cause == GAP_SUCCESS) && p_data->p_cig_mgr_start_setting_rsp->cis_count) {
			for (uint8_t i = 0; i < p_data->p_cig_mgr_start_setting_rsp->cis_count; i++) {
				BT_LOGA("MSG_CIG_MGR_START_SETTING: cis_info[%d], cig_id 0x%x, cis_id 0x%x, cis_conn_handle 0x%x\r\n",
						i,
						p_data->p_cig_mgr_start_setting_rsp->cig_id,
						p_data->p_cig_mgr_start_setting_rsp->cis_info[i].cis_id,
						p_data->p_cig_mgr_start_setting_rsp->cis_info[i].cis_conn_handle);
				APP_PRINT_INFO4("MSG_CIG_MGR_START_SETTING: cis_info[%d], cig_id 0x%x, cis_id 0x%x, cis_conn_handle 0x%x",
								i,
								p_data->p_cig_mgr_start_setting_rsp->cig_id,
								p_data->p_cig_mgr_start_setting_rsp->cis_info[i].cis_id,
								p_data->p_cig_mgr_start_setting_rsp->cis_info[i].cis_conn_handle);
			}
		}
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_cig_mgr_start_setting_rsp->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_CIG_MGR_START_SETTING: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_CIG_MGR_CREATE_CIS: {
		BT_LOGA("MSG_CIG_MGR_CREATE_CIS: cause 0x%x, cis_count 0x%x\r\n",
				p_data->p_cig_mgr_create_cis_rsp->cause,
				p_data->p_cig_mgr_create_cis_rsp->cis_count);
		APP_PRINT_INFO2("MSG_CIG_MGR_CREATE_CIS: cause 0x%x, cis_count 0x%x",
						p_data->p_cig_mgr_create_cis_rsp->cause,
						p_data->p_cig_mgr_create_cis_rsp->cis_count);
		for (uint8_t i = 0; i < p_data->p_cig_mgr_create_cis_rsp->cis_count; i++) {
			BT_LOGA("cis_info[%d]: cig_id 0x%x, cis_id 0x%x, state 0x%x, cis_conn_handle 0x%x\r\n", i,
					p_data->p_cig_mgr_create_cis_rsp->cis_info[i].cig_id,
					p_data->p_cig_mgr_create_cis_rsp->cis_info[i].cis_id,
					p_data->p_cig_mgr_create_cis_rsp->cis_info[i].state,
					p_data->p_cig_mgr_create_cis_rsp->cis_info[i].cis_conn_handle);
			APP_PRINT_INFO5("cis_info[%d]: cig_id 0x%x, cis_id 0x%x, state 0x%x, cis_conn_handle 0x%x", i,
							p_data->p_cig_mgr_create_cis_rsp->cis_info[i].cig_id,
							p_data->p_cig_mgr_create_cis_rsp->cis_info[i].cis_id,
							p_data->p_cig_mgr_create_cis_rsp->cis_info[i].state,
							p_data->p_cig_mgr_create_cis_rsp->cis_info[i].cis_conn_handle);
		}
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_cig_mgr_create_cis_rsp->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_CIG_MGR_CREATE_CIS: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_CIG_MGR_REMOVE_CIG: {
		BT_LOGD("MSG_CIG_MGR_REMOVE_CIG: cause 0x%x, cig_id 0x%x\r\n",
				p_data->p_cig_mgr_remove_cig_rsp->cause, p_data->p_cig_mgr_remove_cig_rsp->cig_id);
		APP_PRINT_INFO2("MSG_CIG_MGR_REMOVE_CIG: cause 0x%x, cig_id 0x%x",
						p_data->p_cig_mgr_remove_cig_rsp->cause, p_data->p_cig_mgr_remove_cig_rsp->cig_id);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_cig_mgr_remove_cig_rsp->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_CIG_MGR_REMOVE_CIG: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_CIG_MGR_CIS_ESTABLISHED_INFO: {
		BT_LOGD(
			"MSG_CIG_MGR_CIS_ESTABLISHED_INFO: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cig_sync_delay 0x%x, cis_sync_delay 0x%x\r\n",
			p_data->p_cig_mgr_cis_established_info->cause,
			p_data->p_cig_mgr_cis_established_info->conn_id,
			p_data->p_cig_mgr_cis_established_info->cis_conn_handle,
			p_data->p_cig_mgr_cis_established_info->cig_id,
			p_data->p_cig_mgr_cis_established_info->cis_id,
			(unsigned int)p_data->p_cig_mgr_cis_established_info->cig_sync_delay,
			(unsigned int)p_data->p_cig_mgr_cis_established_info->cis_sync_delay);
		BT_LOGD("MSG_CIG_MGR_CIS_ESTABLISHED_INFO: transport_latency_m_s 0x%x, transport_latency_s_m 0x%x, phy_m_s 0x%x, phy_s_m 0x%x, nse 0x%x\r\n",
				(unsigned int)p_data->p_cig_mgr_cis_established_info->transport_latency_m_s,
				(unsigned int)p_data->p_cig_mgr_cis_established_info->transport_latency_s_m,
				p_data->p_cig_mgr_cis_established_info->phy_m_s,
				p_data->p_cig_mgr_cis_established_info->phy_s_m,
				p_data->p_cig_mgr_cis_established_info->nse);
		BT_LOGD(
			"MSG_CIG_MGR_CIS_ESTABLISHED_INFO: bn_m_s 0x%x, bn_s_m 0x%x, ft_m_s 0x%x, ft_s_m 0x%x, max_pdu_m_s 0x%x, max_pdu_s_m 0x%x, iso_interval 0x%x",
			p_data->p_cig_mgr_cis_established_info->bn_m_s,
			p_data->p_cig_mgr_cis_established_info->bn_s_m,
			p_data->p_cig_mgr_cis_established_info->ft_m_s,
			p_data->p_cig_mgr_cis_established_info->ft_s_m,
			p_data->p_cig_mgr_cis_established_info->max_pdu_m_s,
			p_data->p_cig_mgr_cis_established_info->max_pdu_s_m,
			p_data->p_cig_mgr_cis_established_info->iso_interval);
		APP_PRINT_INFO7("MSG_CIG_MGR_CIS_ESTABLISHED_INFO: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cig_sync_delay 0x%x, cis_sync_delay 0x%x",
						p_data->p_cig_mgr_cis_established_info->cause,
						p_data->p_cig_mgr_cis_established_info->conn_id,
						p_data->p_cig_mgr_cis_established_info->cis_conn_handle,
						p_data->p_cig_mgr_cis_established_info->cig_id,
						p_data->p_cig_mgr_cis_established_info->cis_id,
						p_data->p_cig_mgr_cis_established_info->cig_sync_delay,
						p_data->p_cig_mgr_cis_established_info->cis_sync_delay);
		APP_PRINT_INFO5("MSG_CIG_MGR_CIS_ESTABLISHED_INFO: transport_latency_m_s 0x%x, transport_latency_s_m 0x%x, phy_m_s 0x%x, phy_s_m 0x%x, nse 0x%x",
						p_data->p_cig_mgr_cis_established_info->transport_latency_m_s,
						p_data->p_cig_mgr_cis_established_info->transport_latency_s_m,
						p_data->p_cig_mgr_cis_established_info->phy_m_s,
						p_data->p_cig_mgr_cis_established_info->phy_s_m,
						p_data->p_cig_mgr_cis_established_info->nse);
		APP_PRINT_INFO7("MSG_CIG_MGR_CIS_ESTABLISHED_INFO: bn_m_s 0x%x, bn_s_m 0x%x, ft_m_s 0x%x, ft_s_m 0x%x, max_pdu_m_s 0x%x, max_pdu_s_m 0x%x, iso_interval 0x%x",
						p_data->p_cig_mgr_cis_established_info->bn_m_s,
						p_data->p_cig_mgr_cis_established_info->bn_s_m,
						p_data->p_cig_mgr_cis_established_info->ft_m_s,
						p_data->p_cig_mgr_cis_established_info->ft_s_m,
						p_data->p_cig_mgr_cis_established_info->max_pdu_m_s,
						p_data->p_cig_mgr_cis_established_info->max_pdu_s_m,
						p_data->p_cig_mgr_cis_established_info->iso_interval);
		{
			rtk_bt_le_iso_cig_cis_established_info_t *p_info = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_ISO,
										RTK_BT_LE_ISO_EVT_CIG_CIS_ESTABLISHED_INFO,
										sizeof(rtk_bt_le_iso_cig_cis_established_info_t));
			p_info = (rtk_bt_le_iso_cig_cis_established_info_t *)p_evt->data;
			p_info->cause = p_data->p_cig_mgr_cis_established_info->cause;
			p_info->conn_handle = le_get_conn_handle(p_data->p_cig_mgr_cis_established_info->conn_id);
			p_info->cig_id = p_data->p_cig_mgr_cis_established_info->cig_id;
			p_info->cis_id = p_data->p_cig_mgr_cis_established_info->cis_id;
			p_info->cis_conn_handle = p_data->p_cig_mgr_cis_established_info->cis_conn_handle;
			p_info->cig_sync_delay = p_data->p_cig_mgr_cis_established_info->cig_sync_delay;
			p_info->cis_sync_delay = p_data->p_cig_mgr_cis_established_info->cis_sync_delay;
			p_info->transport_latency_m_s = p_data->p_cig_mgr_cis_established_info->transport_latency_m_s;
			p_info->transport_latency_s_m = p_data->p_cig_mgr_cis_established_info->transport_latency_s_m;
			p_info->phy_m_s = p_data->p_cig_mgr_cis_established_info->phy_m_s;
			p_info->phy_s_m = p_data->p_cig_mgr_cis_established_info->phy_s_m;
			p_info->nse = p_data->p_cig_mgr_cis_established_info->nse;
			p_info->bn_m_s = p_data->p_cig_mgr_cis_established_info->bn_m_s;
			p_info->bn_s_m = p_data->p_cig_mgr_cis_established_info->bn_s_m;
			p_info->ft_m_s = p_data->p_cig_mgr_cis_established_info->ft_m_s;
			p_info->ft_s_m = p_data->p_cig_mgr_cis_established_info->ft_s_m;
			p_info->max_pdu_m_s = p_data->p_cig_mgr_cis_established_info->max_pdu_m_s;
			p_info->max_pdu_s_m = p_data->p_cig_mgr_cis_established_info->max_pdu_s_m;
			p_info->iso_interval = p_data->p_cig_mgr_cis_established_info->iso_interval;
			rtk_bt_evt_indicate(p_evt, NULL);
		}
	}
	break;

	case MSG_CIG_MGR_SETUP_DATA_PATH: {
		BT_LOGD("MSG_CIG_MGR_SETUP_DATA_PATH: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, data_path_adding_path 0x%x\r\n",
				p_data->p_cig_mgr_setup_data_path_rsp->cause,
				p_data->p_cig_mgr_setup_data_path_rsp->conn_id,
				p_data->p_cig_mgr_setup_data_path_rsp->cis_conn_handle,
				p_data->p_cig_mgr_setup_data_path_rsp->cig_id,
				p_data->p_cig_mgr_setup_data_path_rsp->cis_id,
				p_data->p_cig_mgr_setup_data_path_rsp->data_path_adding_path);
		APP_PRINT_INFO5("MSG_CIG_MGR_SETUP_DATA_PATH: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x",
						p_data->p_cig_mgr_setup_data_path_rsp->cause,
						p_data->p_cig_mgr_setup_data_path_rsp->conn_id,
						p_data->p_cig_mgr_setup_data_path_rsp->cis_conn_handle,
						p_data->p_cig_mgr_setup_data_path_rsp->cig_id,
						p_data->p_cig_mgr_setup_data_path_rsp->cis_id);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_cig_mgr_setup_data_path_rsp->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_CIG_MGR_SETUP_DATA_PATH: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_CIG_MGR_REMOVE_DATA_PATH: {
		BT_LOGD("MSG_CIG_MGR_REMOVE_DATA_PATH: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x\r\n",
				p_data->p_cig_mgr_remove_data_path_rsp->cause,
				p_data->p_cig_mgr_remove_data_path_rsp->conn_id,
				p_data->p_cig_mgr_remove_data_path_rsp->cis_conn_handle,
				p_data->p_cig_mgr_remove_data_path_rsp->cig_id,
				p_data->p_cig_mgr_remove_data_path_rsp->cis_id);
		APP_PRINT_INFO5("MSG_CIG_MGR_REMOVE_DATA_PATH: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x",
						p_data->p_cig_mgr_remove_data_path_rsp->cause,
						p_data->p_cig_mgr_remove_data_path_rsp->conn_id,
						p_data->p_cig_mgr_remove_data_path_rsp->cis_conn_handle,
						p_data->p_cig_mgr_remove_data_path_rsp->cig_id,
						p_data->p_cig_mgr_remove_data_path_rsp->cis_id);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_cig_mgr_remove_data_path_rsp->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_CIG_MGR_REMOVE_DATA_PATH: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_CIG_MGR_READ_ISO_TX_SYNC: {
		BT_LOGA(
			"MSG_CIG_MGR_READ_ISO_TX_SYNC: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, packet_sequence_number 0x%x, time_stamp 0x%x, time_offset 0x%x\r\n",
			p_data->p_cig_mgr_read_iso_tx_sync_rsp->cause, p_data->p_cig_mgr_read_iso_tx_sync_rsp->conn_id,
			p_data->p_cig_mgr_read_iso_tx_sync_rsp->cis_conn_handle,
			p_data->p_cig_mgr_read_iso_tx_sync_rsp->cig_id, p_data->p_cig_mgr_read_iso_tx_sync_rsp->cis_id,
			p_data->p_cig_mgr_read_iso_tx_sync_rsp->packet_sequence_number,
			(unsigned int)p_data->p_cig_mgr_read_iso_tx_sync_rsp->time_stamp,
			(unsigned int)p_data->p_cig_mgr_read_iso_tx_sync_rsp->time_offset);
		APP_PRINT_INFO8("MSG_CIG_MGR_READ_ISO_TX_SYNC: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, packet_sequence_number 0x%x, time_stamp 0x%x, time_offset 0x%x",
						p_data->p_cig_mgr_read_iso_tx_sync_rsp->cause, p_data->p_cig_mgr_read_iso_tx_sync_rsp->conn_id,
						p_data->p_cig_mgr_read_iso_tx_sync_rsp->cis_conn_handle,
						p_data->p_cig_mgr_read_iso_tx_sync_rsp->cig_id, p_data->p_cig_mgr_read_iso_tx_sync_rsp->cis_id,
						p_data->p_cig_mgr_read_iso_tx_sync_rsp->packet_sequence_number,
						p_data->p_cig_mgr_read_iso_tx_sync_rsp->time_stamp,
						p_data->p_cig_mgr_read_iso_tx_sync_rsp->time_offset);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_cig_mgr_read_iso_link_quality_rsp->cause;
			if (p_cmd->ret == 0) { //OK
				rtk_bt_le_iso_cig_read_iso_tx_sync_t *param = (rtk_bt_le_iso_cig_read_iso_tx_sync_t *)p_cmd->param;
				rtk_bt_le_iso_cig_read_iso_tx_sync_info_t *p_tx_sync_info = param->p_tx_sync_info;
				p_tx_sync_info->conn_handle = le_get_conn_handle(p_data->p_cig_mgr_read_iso_tx_sync_rsp->conn_id);
				p_tx_sync_info->cig_id = p_data->p_cig_mgr_read_iso_tx_sync_rsp->cig_id;
				p_tx_sync_info->cis_id = p_data->p_cig_mgr_read_iso_tx_sync_rsp->cis_id;
				p_tx_sync_info->packet_sequence_number = p_data->p_cig_mgr_read_iso_tx_sync_rsp->packet_sequence_number;
				p_tx_sync_info->time_stamp = p_data->p_cig_mgr_read_iso_tx_sync_rsp->time_stamp;
				p_tx_sync_info->time_offset = p_data->p_cig_mgr_read_iso_tx_sync_rsp->time_offset;
			}
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_BIG_MGR_READ_ISO_TX_SYNC: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_CIG_MGR_READ_ISO_LINK_QUALITY: {
		BT_LOGD(
			"MSG_CIG_MGR_READ_ISO_LINK_QUALITY: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, tx_unacked_packets 0x%x, tx_flushed_packets 0x%x, tx_last_subevent_packets 0x%x\r\n",
			p_data->p_cig_mgr_read_iso_link_quality_rsp->cause,
			p_data->p_cig_mgr_read_iso_link_quality_rsp->conn_id,
			p_data->p_cig_mgr_read_iso_link_quality_rsp->cis_conn_handle,
			p_data->p_cig_mgr_read_iso_link_quality_rsp->cig_id,
			p_data->p_cig_mgr_read_iso_link_quality_rsp->cis_id,
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_unacked_packets,
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_flushed_packets,
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_last_subevent_packets);
		BT_LOGD(
			"MSG_CIG_MGR_READ_ISO_LINK_QUALITY: retransmitted_packets 0x%x, crc_error_packets %d, rx_unreceived_packets 0x%x, duplicate_packets 0x%x\r\n",
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->retransmitted_packets,
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->crc_error_packets,
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->rx_unreceived_packets,
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->duplicate_packets);
		APP_PRINT_INFO8("MSG_CIG_MGR_READ_ISO_LINK_QUALITY: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, tx_unacked_packets 0x%x, tx_flushed_packets 0x%x, tx_last_subevent_packets 0x%x",
						p_data->p_cig_mgr_read_iso_link_quality_rsp->cause,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->conn_id,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->cis_conn_handle,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->cig_id,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->cis_id,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_unacked_packets,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_flushed_packets,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_last_subevent_packets);
		APP_PRINT_INFO4("MSG_CIG_MGR_READ_ISO_LINK_QUALITY: retransmitted_packets 0x%x, crc_error_packets %d, rx_unreceived_packets 0x%x, duplicate_packets 0x%x",
						p_data->p_cig_mgr_read_iso_link_quality_rsp->retransmitted_packets,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->crc_error_packets,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->rx_unreceived_packets,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->duplicate_packets);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_cig_mgr_read_iso_link_quality_rsp->cause;
			if (p_cmd->ret == 0) { //OK
				rtk_bt_le_iso_cig_read_link_quality_t *param = (rtk_bt_le_iso_cig_read_link_quality_t *)p_cmd->param;
				rtk_bt_le_iso_cig_read_link_quality_info_t *p_link_quality_info = param->p_link_quality_info;
				p_link_quality_info->conn_handle = le_get_conn_handle(p_data->p_cig_mgr_read_iso_link_quality_rsp->conn_id);
				p_link_quality_info->cig_id = p_data->p_cig_mgr_read_iso_link_quality_rsp->cig_id;
				p_link_quality_info->cis_id = p_data->p_cig_mgr_read_iso_link_quality_rsp->cis_id;
				p_link_quality_info->tx_unacked_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_unacked_packets;
				p_link_quality_info->tx_flushed_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_flushed_packets;
				p_link_quality_info->tx_last_subevent_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_last_subevent_packets;
				p_link_quality_info->retransmitted_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->retransmitted_packets;
				p_link_quality_info->crc_error_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->crc_error_packets;
				p_link_quality_info->rx_unreceived_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->rx_unreceived_packets;
				p_link_quality_info->duplicate_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->duplicate_packets;
			}
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_CIG_MGR_READ_ISO_LINK_QUALITY: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_CIG_MGR_DISCONNECT: {
		BT_LOGD("MSG_CIG_MGR_DISCONNECT: conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cause 0x%x\r\n",
				p_data->p_cig_mgr_disconnect_rsp->conn_id, p_data->p_cig_mgr_disconnect_rsp->cis_conn_handle,
				p_data->p_cig_mgr_disconnect_rsp->cig_id, p_data->p_cig_mgr_disconnect_rsp->cis_id,
				p_data->p_cig_mgr_disconnect_rsp->cause);
		APP_PRINT_INFO5("MSG_CIG_MGR_DISCONNECT: conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cause 0x%x",
						p_data->p_cig_mgr_disconnect_rsp->conn_id, p_data->p_cig_mgr_disconnect_rsp->cis_conn_handle,
						p_data->p_cig_mgr_disconnect_rsp->cig_id, p_data->p_cig_mgr_disconnect_rsp->cis_id,
						p_data->p_cig_mgr_disconnect_rsp->cause);
	}
	break;

	case MSG_CIG_MGR_DISCONNECT_INFO: {
		BT_LOGD("MSG_CIG_MGR_DISCONNECT_INFO: conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cause 0x%x\r\n",
				p_data->p_cig_mgr_disconnect_info->conn_id, p_data->p_cig_mgr_disconnect_info->cis_conn_handle,
				p_data->p_cig_mgr_disconnect_info->cig_id, p_data->p_cig_mgr_disconnect_info->cis_id,
				p_data->p_cig_mgr_disconnect_info->cause);
		APP_PRINT_INFO5("MSG_CIG_MGR_DISCONNECT_INFO: conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cause 0x%x",
						p_data->p_cig_mgr_disconnect_info->conn_id, p_data->p_cig_mgr_disconnect_info->cis_conn_handle,
						p_data->p_cig_mgr_disconnect_info->cig_id, p_data->p_cig_mgr_disconnect_info->cis_id,
						p_data->p_cig_mgr_disconnect_info->cause);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = 0;//p_data->p_cig_mgr_disconnect_rsp->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGD("[%s] MSG_CIG_MGR_DISCONNECT_INFO: find no pending command, indicate it \r\n", __func__);
			rtk_bt_le_iso_cig_disconnect_info_t *p_info = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_ISO,
										RTK_BT_LE_ISO_EVT_CIG_DISCONNECT_INFO,
										sizeof(rtk_bt_le_iso_cig_disconnect_info_t));
			p_info = (rtk_bt_le_iso_cig_disconnect_info_t *)p_evt->data;
			p_info->cause = p_data->p_cig_mgr_disconnect_info->cause;
			p_info->cis_conn_handle = p_data->p_cig_mgr_disconnect_info->cis_conn_handle;
			p_info->conn_handle = le_get_conn_handle(p_data->p_cig_mgr_disconnect_info->conn_id);
			p_info->cig_id = p_data->p_cig_mgr_disconnect_info->cig_id;
			p_info->cis_id = p_data->p_cig_mgr_disconnect_info->cis_id;
			rtk_bt_evt_indicate(p_evt, NULL);
		}
	}
	break;

	default:
		break;
	}
	return result;
}
static uint16_t bt_stack_le_iso_cig_initiator_register_callback(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint8_t cig_id = 0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cig_id = *(uint8_t *)data;

	if (cig_id > RTK_BLE_CIG_MAX_NUM) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cause = cig_mgr_reg_cig(cig_id, bt_stack_le_iso_cig_initiator_cb);
	if (cause) {
		BT_LOGE("%s cig_mgr_reg_cig (cause = 0x%x,cig_id = 0x%x)\r\n", __func__, cause, cig_id);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	g_bt_stack_le_iso_role = RTK_BLE_ISO_ROLE_CIS_INITIATOR;

	return RTK_BT_OK;
}

T_APP_RESULT bt_stack_le_iso_cig_acceptor_cb(uint8_t cb_type, void *p_cb_data)
{
	T_APP_RESULT result = APP_RESULT_SUCCESS;
	T_CIG_MGR_CB_DATA *p_data = (T_CIG_MGR_CB_DATA *)p_cb_data;
	rtk_bt_cmd_t *p_cmd = NULL;
	rtk_bt_evt_t *p_evt = NULL;

	BT_LOGD("app_isoc_cis_acceptor_cb: cb_type 0x%x\r\n", cb_type);

	switch (cb_type) {
	case MSG_CIG_MGR_CIS_ESTABLISHED_INFO: {
		BT_LOGD(
			"MSG_CIG_MGR_CIS_ESTABLISHED_INFO: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cig_sync_delay 0x%x, cis_sync_delay 0x%x\r\n",
			p_data->p_cig_mgr_cis_established_info->cause,
			p_data->p_cig_mgr_cis_established_info->conn_id,
			p_data->p_cig_mgr_cis_established_info->cis_conn_handle,
			p_data->p_cig_mgr_cis_established_info->cig_id,
			p_data->p_cig_mgr_cis_established_info->cis_id,
			(unsigned int)p_data->p_cig_mgr_cis_established_info->cig_sync_delay,
			(unsigned int)p_data->p_cig_mgr_cis_established_info->cis_sync_delay);
		BT_LOGD("MSG_CIG_MGR_CIS_ESTABLISHED_INFO: transport_latency_m_s 0x%x, transport_latency_s_m 0x%x, phy_m_s 0x%x, phy_s_m 0x%x, nse 0x%x\r\n",
				(unsigned int)p_data->p_cig_mgr_cis_established_info->transport_latency_m_s,
				(unsigned int)p_data->p_cig_mgr_cis_established_info->transport_latency_s_m,
				p_data->p_cig_mgr_cis_established_info->phy_m_s,
				p_data->p_cig_mgr_cis_established_info->phy_s_m,
				p_data->p_cig_mgr_cis_established_info->nse);
		BT_LOGD(
			"MSG_CIG_MGR_CIS_ESTABLISHED_INFO: bn_m_s 0x%x, bn_s_m 0x%x, ft_m_s 0x%x, ft_s_m 0x%x, max_pdu_m_s 0x%x, max_pdu_s_m 0x%x, iso_interval 0x%x\r\n",
			p_data->p_cig_mgr_cis_established_info->bn_m_s,
			p_data->p_cig_mgr_cis_established_info->bn_s_m,
			p_data->p_cig_mgr_cis_established_info->ft_m_s,
			p_data->p_cig_mgr_cis_established_info->ft_s_m,
			p_data->p_cig_mgr_cis_established_info->max_pdu_m_s,
			p_data->p_cig_mgr_cis_established_info->max_pdu_s_m,
			p_data->p_cig_mgr_cis_established_info->iso_interval);
		APP_PRINT_INFO7("MSG_CIG_MGR_CIS_ESTABLISHED_INFO: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cig_sync_delay 0x%x, cis_sync_delay 0x%x",
						p_data->p_cig_mgr_cis_established_info->cause,
						p_data->p_cig_mgr_cis_established_info->conn_id,
						p_data->p_cig_mgr_cis_established_info->cis_conn_handle,
						p_data->p_cig_mgr_cis_established_info->cig_id,
						p_data->p_cig_mgr_cis_established_info->cis_id,
						p_data->p_cig_mgr_cis_established_info->cig_sync_delay,
						p_data->p_cig_mgr_cis_established_info->cis_sync_delay);
		APP_PRINT_INFO5("MSG_CIG_MGR_CIS_ESTABLISHED_INFO: transport_latency_m_s 0x%x, transport_latency_s_m 0x%x, phy_m_s 0x%x, phy_s_m 0x%x, nse 0x%x",
						p_data->p_cig_mgr_cis_established_info->transport_latency_m_s,
						p_data->p_cig_mgr_cis_established_info->transport_latency_s_m,
						p_data->p_cig_mgr_cis_established_info->phy_m_s,
						p_data->p_cig_mgr_cis_established_info->phy_s_m,
						p_data->p_cig_mgr_cis_established_info->nse);
		APP_PRINT_INFO7("MSG_CIG_MGR_CIS_ESTABLISHED_INFO: bn_m_s 0x%x, bn_s_m 0x%x, ft_m_s 0x%x, ft_s_m 0x%x, max_pdu_m_s 0x%x, max_pdu_s_m 0x%x, iso_interval 0x%x",
						p_data->p_cig_mgr_cis_established_info->bn_m_s,
						p_data->p_cig_mgr_cis_established_info->bn_s_m,
						p_data->p_cig_mgr_cis_established_info->ft_m_s,
						p_data->p_cig_mgr_cis_established_info->ft_s_m,
						p_data->p_cig_mgr_cis_established_info->max_pdu_m_s,
						p_data->p_cig_mgr_cis_established_info->max_pdu_s_m,
						p_data->p_cig_mgr_cis_established_info->iso_interval);
		{
			rtk_bt_le_iso_cig_cis_established_info_t *p_info = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_ISO,
										RTK_BT_LE_ISO_EVT_CIG_CIS_ESTABLISHED_INFO,
										sizeof(rtk_bt_le_iso_cig_cis_established_info_t));
			p_info = (rtk_bt_le_iso_cig_cis_established_info_t *)p_evt->data;
			p_info->cause = p_data->p_cig_mgr_cis_established_info->cause;
			p_info->conn_handle = le_get_conn_handle(p_data->p_cig_mgr_cis_established_info->conn_id);
			p_info->cig_id = p_data->p_cig_mgr_cis_established_info->cig_id;
			p_info->cis_id = p_data->p_cig_mgr_cis_established_info->cis_id;
			p_info->cis_conn_handle = p_data->p_cig_mgr_cis_established_info->cis_conn_handle;
			p_info->cig_sync_delay = p_data->p_cig_mgr_cis_established_info->cig_sync_delay;
			p_info->cis_sync_delay = p_data->p_cig_mgr_cis_established_info->cis_sync_delay;
			p_info->transport_latency_m_s = p_data->p_cig_mgr_cis_established_info->transport_latency_m_s;
			p_info->transport_latency_s_m = p_data->p_cig_mgr_cis_established_info->transport_latency_s_m;
			p_info->phy_m_s = p_data->p_cig_mgr_cis_established_info->phy_m_s;
			p_info->phy_s_m = p_data->p_cig_mgr_cis_established_info->phy_s_m;
			p_info->nse = p_data->p_cig_mgr_cis_established_info->nse;
			p_info->bn_m_s = p_data->p_cig_mgr_cis_established_info->bn_m_s;
			p_info->bn_s_m = p_data->p_cig_mgr_cis_established_info->bn_s_m;
			p_info->ft_m_s = p_data->p_cig_mgr_cis_established_info->ft_m_s;
			p_info->ft_s_m = p_data->p_cig_mgr_cis_established_info->ft_s_m;
			p_info->max_pdu_m_s = p_data->p_cig_mgr_cis_established_info->max_pdu_m_s;
			p_info->max_pdu_s_m = p_data->p_cig_mgr_cis_established_info->max_pdu_s_m;
			p_info->iso_interval = p_data->p_cig_mgr_cis_established_info->iso_interval;
			rtk_bt_evt_indicate(p_evt, NULL);
		}
	}
	break;

	case MSG_CIG_MGR_SETUP_DATA_PATH: {
		BT_LOGD("MSG_CIG_MGR_SETUP_DATA_PATH: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x\r\n",
				p_data->p_cig_mgr_setup_data_path_rsp->cause,
				p_data->p_cig_mgr_setup_data_path_rsp->conn_id,
				p_data->p_cig_mgr_setup_data_path_rsp->cis_conn_handle,
				p_data->p_cig_mgr_setup_data_path_rsp->cig_id,
				p_data->p_cig_mgr_setup_data_path_rsp->cis_id);
		APP_PRINT_INFO5("MSG_CIG_MGR_SETUP_DATA_PATH: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x",
						p_data->p_cig_mgr_setup_data_path_rsp->cause,
						p_data->p_cig_mgr_setup_data_path_rsp->conn_id,
						p_data->p_cig_mgr_setup_data_path_rsp->cis_conn_handle,
						p_data->p_cig_mgr_setup_data_path_rsp->cig_id,
						p_data->p_cig_mgr_setup_data_path_rsp->cis_id);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_cig_mgr_setup_data_path_rsp->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_CIG_MGR_SETUP_DATA_PATH: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_CIG_MGR_REMOVE_DATA_PATH: {
		BT_LOGD(
			"MSG_CIG_MGR_REMOVE_DATA_PATH: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, data_path_removing_path 0x%x\r\n",
			p_data->p_cig_mgr_remove_data_path_rsp->cause,
			p_data->p_cig_mgr_remove_data_path_rsp->conn_id,
			p_data->p_cig_mgr_remove_data_path_rsp->cis_conn_handle,
			p_data->p_cig_mgr_remove_data_path_rsp->cig_id,
			p_data->p_cig_mgr_remove_data_path_rsp->cis_id,
			p_data->p_cig_mgr_remove_data_path_rsp->data_path_removing_path);
		APP_PRINT_INFO6("MSG_CIG_MGR_REMOVE_DATA_PATH: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, data_path_removing_path 0x%x",
						p_data->p_cig_mgr_remove_data_path_rsp->cause,
						p_data->p_cig_mgr_remove_data_path_rsp->conn_id,
						p_data->p_cig_mgr_remove_data_path_rsp->cis_conn_handle,
						p_data->p_cig_mgr_remove_data_path_rsp->cig_id,
						p_data->p_cig_mgr_remove_data_path_rsp->cis_id,
						p_data->p_cig_mgr_remove_data_path_rsp->data_path_removing_path);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_cig_mgr_remove_data_path_rsp->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_CIG_MGR_REMOVE_DATA_PATH: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_CIG_MGR_READ_ISO_TX_SYNC: {
		BT_LOGD(
			"MSG_CIG_MGR_READ_ISO_TX_SYNC: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, packet_sequence_number 0x%x, time_stamp 0x%x, time_offset 0x%x\r\n",
			p_data->p_cig_mgr_read_iso_tx_sync_rsp->cause, p_data->p_cig_mgr_read_iso_tx_sync_rsp->conn_id,
			p_data->p_cig_mgr_read_iso_tx_sync_rsp->cis_conn_handle,
			p_data->p_cig_mgr_read_iso_tx_sync_rsp->cig_id, p_data->p_cig_mgr_read_iso_tx_sync_rsp->cis_id,
			p_data->p_cig_mgr_read_iso_tx_sync_rsp->packet_sequence_number,
			(unsigned int)p_data->p_cig_mgr_read_iso_tx_sync_rsp->time_stamp,
			(unsigned int)p_data->p_cig_mgr_read_iso_tx_sync_rsp->time_offset);
		APP_PRINT_INFO8("MSG_CIG_MGR_READ_ISO_TX_SYNC: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, packet_sequence_number 0x%x, time_stamp 0x%x, time_offset 0x%x",
						p_data->p_cig_mgr_read_iso_tx_sync_rsp->cause, p_data->p_cig_mgr_read_iso_tx_sync_rsp->conn_id,
						p_data->p_cig_mgr_read_iso_tx_sync_rsp->cis_conn_handle,
						p_data->p_cig_mgr_read_iso_tx_sync_rsp->cig_id, p_data->p_cig_mgr_read_iso_tx_sync_rsp->cis_id,
						p_data->p_cig_mgr_read_iso_tx_sync_rsp->packet_sequence_number,
						p_data->p_cig_mgr_read_iso_tx_sync_rsp->time_stamp,
						p_data->p_cig_mgr_read_iso_tx_sync_rsp->time_offset);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_cig_mgr_read_iso_link_quality_rsp->cause;
			if (p_cmd->ret == 0) { //OK
				rtk_bt_le_iso_cig_read_iso_tx_sync_t *param = (rtk_bt_le_iso_cig_read_iso_tx_sync_t *)p_cmd->param;
				rtk_bt_le_iso_cig_read_iso_tx_sync_info_t *p_tx_sync_info = param->p_tx_sync_info;
				p_tx_sync_info->conn_handle = le_get_conn_handle(p_data->p_cig_mgr_read_iso_tx_sync_rsp->conn_id);
				p_tx_sync_info->cig_id = p_data->p_cig_mgr_read_iso_tx_sync_rsp->cig_id;
				p_tx_sync_info->cis_id = p_data->p_cig_mgr_read_iso_tx_sync_rsp->cis_id;
				p_tx_sync_info->packet_sequence_number = p_data->p_cig_mgr_read_iso_tx_sync_rsp->packet_sequence_number;
				p_tx_sync_info->time_stamp = p_data->p_cig_mgr_read_iso_tx_sync_rsp->time_stamp;
				p_tx_sync_info->time_offset = p_data->p_cig_mgr_read_iso_tx_sync_rsp->time_offset;
			}
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_BIG_MGR_READ_ISO_TX_SYNC: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_CIG_MGR_READ_ISO_LINK_QUALITY: {
		BT_LOGD(
			"MSG_CIG_MGR_READ_ISO_LINK_QUALITY: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, tx_unacked_packets 0x%x, tx_flushed_packets 0x%x, tx_last_subevent_packets 0x%x\r\n",
			p_data->p_cig_mgr_read_iso_link_quality_rsp->cause,
			p_data->p_cig_mgr_read_iso_link_quality_rsp->conn_id,
			p_data->p_cig_mgr_read_iso_link_quality_rsp->cis_conn_handle,
			p_data->p_cig_mgr_read_iso_link_quality_rsp->cig_id,
			p_data->p_cig_mgr_read_iso_link_quality_rsp->cis_id,
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_unacked_packets,
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_flushed_packets,
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_last_subevent_packets);
		BT_LOGD(
			"MSG_CIG_MGR_READ_ISO_LINK_QUALITY: retransmitted_packets 0x%x, crc_error_packets %d, rx_unreceived_packets 0x%x, duplicate_packets 0x%x\r\n",
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->retransmitted_packets,
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->crc_error_packets,
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->rx_unreceived_packets,
			(unsigned int)p_data->p_cig_mgr_read_iso_link_quality_rsp->duplicate_packets);
		APP_PRINT_INFO8("MSG_CIG_MGR_READ_ISO_LINK_QUALITY: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, tx_unacked_packets 0x%x, tx_flushed_packets 0x%x, tx_last_subevent_packets 0x%x",
						p_data->p_cig_mgr_read_iso_link_quality_rsp->cause,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->conn_id,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->cis_conn_handle,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->cig_id,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->cis_id,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_unacked_packets,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_flushed_packets,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_last_subevent_packets);
		APP_PRINT_INFO4("MSG_CIG_MGR_READ_ISO_LINK_QUALITY: retransmitted_packets 0x%x, crc_error_packets %d, rx_unreceived_packets 0x%x, duplicate_packets 0x%x",
						p_data->p_cig_mgr_read_iso_link_quality_rsp->retransmitted_packets,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->crc_error_packets,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->rx_unreceived_packets,
						p_data->p_cig_mgr_read_iso_link_quality_rsp->duplicate_packets);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_cig_mgr_read_iso_link_quality_rsp->cause;
			if (p_cmd->ret == 0) { //OK
				rtk_bt_le_iso_cig_read_link_quality_t *param = (rtk_bt_le_iso_cig_read_link_quality_t *)p_cmd->param;
				rtk_bt_le_iso_cig_read_link_quality_info_t *p_link_quality_info = param->p_link_quality_info;
				p_link_quality_info->conn_handle = le_get_conn_handle(p_data->p_cig_mgr_read_iso_link_quality_rsp->conn_id);
				p_link_quality_info->cig_id = p_data->p_cig_mgr_read_iso_link_quality_rsp->cig_id;
				p_link_quality_info->cis_id = p_data->p_cig_mgr_read_iso_link_quality_rsp->cis_id;
				p_link_quality_info->tx_unacked_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_unacked_packets;
				p_link_quality_info->tx_flushed_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_flushed_packets;
				p_link_quality_info->tx_last_subevent_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->tx_last_subevent_packets;
				p_link_quality_info->retransmitted_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->retransmitted_packets;
				p_link_quality_info->crc_error_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->crc_error_packets;
				p_link_quality_info->rx_unreceived_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->rx_unreceived_packets;
				p_link_quality_info->duplicate_packets = p_data->p_cig_mgr_read_iso_link_quality_rsp->duplicate_packets;
			}
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_CIG_MGR_READ_ISO_LINK_QUALITY: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_CIG_MGR_DISCONNECT: {
		BT_LOGD("MSG_CIG_MGR_DISCONNECT: conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cause 0x%x\r\n",
				p_data->p_cig_mgr_disconnect_rsp->conn_id, p_data->p_cig_mgr_disconnect_rsp->cis_conn_handle,
				p_data->p_cig_mgr_disconnect_rsp->cig_id,  p_data->p_cig_mgr_disconnect_rsp->cis_id,
				p_data->p_cig_mgr_disconnect_rsp->cause);
		APP_PRINT_INFO5("MSG_CIG_MGR_DISCONNECT: conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cause 0x%x",
						p_data->p_cig_mgr_disconnect_rsp->conn_id, p_data->p_cig_mgr_disconnect_rsp->cis_conn_handle,
						p_data->p_cig_mgr_disconnect_rsp->cig_id,  p_data->p_cig_mgr_disconnect_rsp->cis_id,
						p_data->p_cig_mgr_disconnect_rsp->cause);
	}
	break;

	case MSG_CIG_MGR_DISCONNECT_INFO: {
		BT_LOGD("MSG_CIG_MGR_DISCONNECT_INFO: conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cause 0x%x\r\n",
				p_data->p_cig_mgr_disconnect_info->conn_id, p_data->p_cig_mgr_disconnect_info->cis_conn_handle,
				p_data->p_cig_mgr_disconnect_info->cig_id, p_data->p_cig_mgr_disconnect_info->cis_id,
				p_data->p_cig_mgr_disconnect_info->cause);
		APP_PRINT_INFO5("MSG_CIG_MGR_DISCONNECT_INFO: conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cause 0x%x",
						p_data->p_cig_mgr_disconnect_info->conn_id, p_data->p_cig_mgr_disconnect_info->cis_conn_handle,
						p_data->p_cig_mgr_disconnect_info->cig_id, p_data->p_cig_mgr_disconnect_info->cis_id,
						p_data->p_cig_mgr_disconnect_info->cause);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = 0;//p_data->p_cig_mgr_disconnect_rsp->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGD("[%s] MSG_CIG_MGR_DISCONNECT_INFO: find no pending command, indicate it \r\n", __func__);
			rtk_bt_le_iso_cig_disconnect_info_t *p_info = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_ISO,
										RTK_BT_LE_ISO_EVT_CIG_DISCONNECT_INFO,
										sizeof(rtk_bt_le_iso_cig_disconnect_info_t));
			p_info = (rtk_bt_le_iso_cig_disconnect_info_t *)p_evt->data;
			p_info->cause = p_data->p_cig_mgr_disconnect_info->cause;
			p_info->cis_conn_handle = p_data->p_cig_mgr_disconnect_info->cis_conn_handle;
			p_info->conn_handle = le_get_conn_handle(p_data->p_cig_mgr_disconnect_info->conn_id);
			p_info->cig_id = p_data->p_cig_mgr_disconnect_info->cig_id;
			p_info->cis_id = p_data->p_cig_mgr_disconnect_info->cis_id;
			rtk_bt_evt_indicate(p_evt, NULL);
		}
	}
	break;

	case MSG_CIG_MGR_CIS_REQUEST_IND: {
		BT_LOGD("MSG_CIG_MGR_CIS_REQUEST_IND: conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x\r\n",
				p_data->p_cig_mgr_cis_request_ind->conn_id,
				p_data->p_cig_mgr_cis_request_ind->cis_conn_handle,
				p_data->p_cig_mgr_cis_request_ind->cig_id,
				p_data->p_cig_mgr_cis_request_ind->cis_id);
		APP_PRINT_INFO4("MSG_CIG_MGR_CIS_REQUEST_IND: conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x",
						p_data->p_cig_mgr_cis_request_ind->conn_id,
						p_data->p_cig_mgr_cis_request_ind->cis_conn_handle,
						p_data->p_cig_mgr_cis_request_ind->cig_id,
						p_data->p_cig_mgr_cis_request_ind->cis_id);
		if (APP_RESULT_ACCEPT == g_cis_request_ind_ret) {
			BT_LOGD("MSG_CIG_MGR_CIS_REQUEST_IND: Host stack accepted the request!\r\n");
		} else if (APP_RESULT_REJECT == g_cis_request_ind_ret) {
			BT_LOGD("MSG_CIG_MGR_CIS_REQUEST_IND: Host stack rejected the request!\r\n");
		} else if (APP_RESULT_PENDING == g_cis_request_ind_ret) {
			BT_LOGD("MSG_CIG_MGR_CIS_REQUEST_IND: Host stack pending the request!\r\n");
			rtk_bt_le_iso_cig_acceptor_request_cis_ind_t *request_cis_ind = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_ISO,
										RTK_BT_LE_ISO_EVT_CIG_ACCEPTOR_REQUEST_CIS_IND,
										sizeof(rtk_bt_le_iso_cig_acceptor_request_cis_ind_t));
			request_cis_ind = (rtk_bt_le_iso_cig_acceptor_request_cis_ind_t *)p_evt->data;
			request_cis_ind->cis_conn_handle = p_data->p_cig_mgr_cis_request_ind->cis_conn_handle;
			request_cis_ind->conn_handle = le_get_conn_handle(p_data->p_cig_mgr_cis_request_ind->conn_id);
			request_cis_ind->cig_id = p_data->p_cig_mgr_cis_request_ind->cig_id;
			request_cis_ind->cis_id = p_data->p_cig_mgr_cis_request_ind->cis_id;
			request_cis_ind->cis_req_action = convert_stack_app_result_to_cis_req_action(g_cis_request_ind_ret);
			rtk_bt_evt_indicate(p_evt, NULL);
		}
		result = g_cis_request_ind_ret;
	}
	break;

	case MSG_CIG_MGR_ACCEPT_CIS_INFO: {
		BT_LOGD("MSG_CIG_MGR_ACCEPT_CIS_INFO: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x\r\n",
				p_data->p_cig_mgr_accept_cis_info->cause,
				p_data->p_cig_mgr_accept_cis_info->conn_id,
				p_data->p_cig_mgr_accept_cis_info->cis_conn_handle,
				p_data->p_cig_mgr_accept_cis_info->cig_id,
				p_data->p_cig_mgr_accept_cis_info->cis_id);
		APP_PRINT_INFO5("MSG_CIG_MGR_ACCEPT_CIS_INFO: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x",
						p_data->p_cig_mgr_accept_cis_info->cause,
						p_data->p_cig_mgr_accept_cis_info->conn_id,
						p_data->p_cig_mgr_accept_cis_info->cis_conn_handle,
						p_data->p_cig_mgr_accept_cis_info->cig_id,
						p_data->p_cig_mgr_accept_cis_info->cis_id);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_cig_mgr_accept_cis_info->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGD("[%s] MSG_CIG_MGR_ACCEPT_CIS_INFO: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_CIG_MGR_REJECT_CIS_INFO: {
		BT_LOGD("MSG_CIG_MGR_REJECT_CIS_INFO: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x\r\n",
				p_data->p_cig_mgr_reject_cis_info->cause,
				p_data->p_cig_mgr_reject_cis_info->conn_id,
				p_data->p_cig_mgr_reject_cis_info->cis_conn_handle,
				p_data->p_cig_mgr_reject_cis_info->cig_id,
				p_data->p_cig_mgr_reject_cis_info->cis_id);
		APP_PRINT_INFO5("MSG_CIG_MGR_REJECT_CIS_INFO: cause 0x%x, conn_id %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x",
						p_data->p_cig_mgr_reject_cis_info->cause,
						p_data->p_cig_mgr_reject_cis_info->conn_id,
						p_data->p_cig_mgr_reject_cis_info->cis_conn_handle,
						p_data->p_cig_mgr_reject_cis_info->cig_id,
						p_data->p_cig_mgr_reject_cis_info->cis_id);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_cig_mgr_reject_cis_info->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_CIG_MGR_DISCONNECT: find no pending command \r\n", __func__);
		}
	}
	break;

	default:
		break;
	}
	return result;
}

static uint16_t bt_stack_le_iso_cig_acceptor_register_callback(void *data)
{
	UNUSED(data);

	g_bt_stack_le_iso_role = RTK_BLE_ISO_ROLE_CIS_ACCEPTOR;

	cig_mgr_reg_acceptor_cb(bt_stack_le_iso_cig_acceptor_cb); //the API return void

	return RTK_BT_OK;
}

static void rtk_stack_iso_handle_bis_broadcaster_state_evt(uint8_t big_handle, uint8_t adv_handle,
														   T_GAP_BIG_ISOC_BROADCAST_STATE new_state, uint16_t cause)
{
	uint8_t cb_type = MSG_BIG_MGR_ISOC_BROADCAST_STATE_CHANGE_INFO;
	bool indicate = false;
	rtk_bt_cmd_t *p_cmd = NULL;
	rtk_bt_evt_t *p_evt = NULL;
	BT_LOGD("%s: big_handle 0x%x, adv_handle 0x%x, new_state 0x%x, cause 0x%x\r\n", __func__,
			big_handle, adv_handle, new_state, cause);
	switch (new_state) {
	case BIG_ISOC_BROADCAST_STATE_BROADCASTING: {
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] BIG_ISOC_BROADCAST_STATE_BROADCASTING: find no pending command \r\n", __func__);
		}
		indicate = true;
		break;
	}
	case BIG_ISOC_BROADCAST_STATE_IDLE: {
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = 0;//cause;//cause is 0x116 (HCI_ERR_LOCAL_HOST_TERMINATE) when broadcaster terminate big
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] BIG_ISOC_BROADCAST_STATE_IDLE: find no pending command \r\n", __func__);
		}
		indicate = true;
		break;
	}
	default:
		break;
	}

	if (indicate) {
		rtk_bt_le_iso_big_broadcaster_sync_state_ind_t *sync_state_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_ISO,
									RTK_BT_LE_ISO_EVT_BIG_BROADCASTER_SYNC_STATE_IND,
									sizeof(rtk_bt_le_iso_big_broadcaster_sync_state_ind_t));
		sync_state_ind = (rtk_bt_le_iso_big_broadcaster_sync_state_ind_t *)p_evt->data;
		sync_state_ind->big_handle = big_handle;
		sync_state_ind->sync_state = (rtk_bt_le_iso_big_broadcaster_sync_state_t)new_state;
		sync_state_ind->adv_handle = adv_handle;
		sync_state_ind->cause = cause;
		rtk_bt_evt_indicate(p_evt, NULL);
	}
}

T_APP_RESULT bt_stack_le_iso_big_broadcaster_cb(uint8_t big_handle, uint8_t cb_type, void *p_cb_data)
{
	rtk_bt_cmd_t *p_cmd = NULL;
	rtk_bt_evt_t *p_evt = NULL;
	T_APP_RESULT result = APP_RESULT_SUCCESS;
	T_BIG_MGR_CB_DATA *p_data = (T_BIG_MGR_CB_DATA *)p_cb_data;

	switch (cb_type) {
	case MSG_BIG_MGR_SETUP_DATA_PATH: {
		APP_PRINT_INFO3("MSG_BIG_MGR_SETUP_DATA_PATH: cause 0x%x, big_handle 0x%x, bis_conn_handle 0x%x",
						p_data->p_big_mgr_setup_data_path_rsp->cause,
						p_data->p_big_mgr_setup_data_path_rsp->big_handle,
						p_data->p_big_mgr_setup_data_path_rsp->bis_conn_handle);
		BT_LOGA("MSG_BIG_MGR_SETUP_DATA_PATH: cause 0x%x, big_handle 0x%x, bis_conn_handle 0x%x\r\n",
				p_data->p_big_mgr_setup_data_path_rsp->cause,
				p_data->p_big_mgr_setup_data_path_rsp->big_handle,
				p_data->p_big_mgr_setup_data_path_rsp->bis_conn_handle);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_big_mgr_setup_data_path_rsp->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_BIG_MGR_SETUP_DATA_PATH: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_BIG_MGR_REMOVE_DATA_PATH: {
		APP_PRINT_INFO3("MSG_BIG_MGR_REMOVE_DATA_PATH: cause 0x%x, big_handle 0x%x, bis_conn_handle 0x%x",
						p_data->p_big_mgr_remove_data_path_rsp->cause,
						p_data->p_big_mgr_remove_data_path_rsp->big_handle,
						p_data->p_big_mgr_remove_data_path_rsp->bis_conn_handle);
		BT_LOGA("MSG_BIG_MGR_REMOVE_DATA_PATH: cause 0x%x, big_handle 0x%x, bis_conn_handle 0x%x\r\n",
				p_data->p_big_mgr_remove_data_path_rsp->cause,
				p_data->p_big_mgr_remove_data_path_rsp->big_handle,
				p_data->p_big_mgr_remove_data_path_rsp->bis_conn_handle);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_big_mgr_remove_data_path_rsp->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_BIG_MGR_REMOVE_DATA_PATH: find no pending command \r\n", __func__);
		}
	}
	break;
	case MSG_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_CMPL_INFO: {
		APP_PRINT_INFO7("MSG_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_CMPL_INFO: big_handle 0x%x, adv_handle 0x%x, cause 0x%x, big_sync_delay 0x%x, transport_latency_big 0x%x, phy %d, nse %d",
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->big_handle,
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->adv_handle,
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->cause,
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->big_sync_delay,
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->transport_latency_big,
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->phy,
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->nse);
		BT_LOGD(
			"MSG_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_CMPL_INFO: big_handle 0x%x, adv_handle 0x%x, cause 0x%x, big_sync_delay 0x%x, transport_latency_big 0x%x, phy %d, nse %d\r\n",
			p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->big_handle,
			p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->adv_handle,
			p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->cause,
			(unsigned int)p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->big_sync_delay,
			(unsigned int)p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->transport_latency_big,
			p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->phy,
			p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->nse);
		APP_PRINT_INFO6("MSG_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_CMPL_INFO: bn %d, pto %d, irc %d, max_pdu %d, iso_interval 0x%x, num_bis %d",
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->bn,
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->pto,
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->irc,
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->max_pdu,
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->iso_interval,
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->num_bis);
		BT_LOGD("MSG_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_CMPL_INFO: bn %d, pto %d, irc %d, max_pdu %d, iso_interval 0x%x, num_bis %d\r\n",
				p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->bn,
				p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->pto,
				p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->irc,
				p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->max_pdu,
				p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->iso_interval,
				p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->num_bis);
		if (p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->cause == GAP_SUCCESS) {
			for (uint8_t i = 0; i < p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->num_bis; i++) {
				APP_PRINT_INFO2("MSG_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_CMPL_INFO: bis index %d, bis_conn_handle 0x%x",
								p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->bis_conn_handle_info[i].bis_idx,
								p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->bis_conn_handle_info[i].bis_conn_handle);
				BT_LOGD("MSG_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_CMPL_INFO: bis index %d, bis_conn_handle 0x%x\r\n",
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->bis_conn_handle_info[i].bis_idx,
						p_data->p_big_mgr_isoc_bc_create_big_cmpl_info->bis_conn_handle_info[i].bis_conn_handle);
			}
		}
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_ISO,
									RTK_BT_LE_ISO_EVT_BIG_BROADCASTER_CREATE_CMPL_INFO,
									sizeof(rtk_bt_le_iso_big_broadcaster_create_cmpl_info_t));
		memcpy((void *)p_evt->data, (void *)p_data->p_big_mgr_isoc_bc_create_big_cmpl_info, sizeof(T_BIG_MGR_ISOC_BC_CREATE_BIG_CMPL_INFO));
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case MSG_BIG_MGR_ISOC_BROADCAST_STATE_CHANGE_INFO: {
		rtk_stack_iso_handle_bis_broadcaster_state_evt(p_data->p_big_mgr_isoc_broadcast_state_change_info->big_handle,
													   p_data->p_big_mgr_isoc_broadcast_state_change_info->adv_handle,
													   (T_GAP_BIG_ISOC_BROADCAST_STATE)p_data->p_big_mgr_isoc_broadcast_state_change_info->new_state,
													   p_data->p_big_mgr_isoc_broadcast_state_change_info->cause);
	}
	break;
	case MSG_BIG_MGR_READ_ISO_TX_SYNC: {
		APP_PRINT_INFO7("MSG_BIG_MGR_READ_ISO_TX_SYNC: cause 0x%x, bis_conn_handle 0x%x, big_handle 0x%x, adv_handle 0x%x, packet_sequence_number 0x%x, time_stamp 0x%x, time_offset 0x%x",
						p_data->p_big_mgr_read_iso_tx_sync_rsp->cause,
						p_data->p_big_mgr_read_iso_tx_sync_rsp->bis_conn_handle,
						p_data->p_big_mgr_read_iso_tx_sync_rsp->big_handle,
						p_data->p_big_mgr_read_iso_tx_sync_rsp->adv_handle,
						p_data->p_big_mgr_read_iso_tx_sync_rsp->packet_sequence_number,
						p_data->p_big_mgr_read_iso_tx_sync_rsp->time_stamp,
						p_data->p_big_mgr_read_iso_tx_sync_rsp->time_offset);
		BT_LOGD(
			"MSG_BIG_MGR_READ_ISO_TX_SYNC: cause 0x%x, bis_conn_handle 0x%x, big_handle 0x%x, adv_handle 0x%x, packet_sequence_number 0x%x, time_stamp 0x%x, time_offset 0x%x\r\n",
			p_data->p_big_mgr_read_iso_tx_sync_rsp->cause,
			p_data->p_big_mgr_read_iso_tx_sync_rsp->bis_conn_handle,
			p_data->p_big_mgr_read_iso_tx_sync_rsp->big_handle,
			p_data->p_big_mgr_read_iso_tx_sync_rsp->adv_handle,
			p_data->p_big_mgr_read_iso_tx_sync_rsp->packet_sequence_number,
			(unsigned int)p_data->p_big_mgr_read_iso_tx_sync_rsp->time_stamp,
			(unsigned int)p_data->p_big_mgr_read_iso_tx_sync_rsp->time_offset);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_big_mgr_read_iso_tx_sync_rsp->cause;
			if (p_cmd->ret == 0) { //OK
				rtk_bt_le_iso_big_broadcaster_read_tx_sync_t *param = (rtk_bt_le_iso_big_broadcaster_read_tx_sync_t *)p_cmd->param;
				rtk_bt_le_iso_big_broadcaster_read_tx_sync_info_t *p_tx_sync_info = param->p_tx_sync_info;
				p_tx_sync_info->big_handle = p_data->p_big_mgr_read_iso_tx_sync_rsp->big_handle;
				p_tx_sync_info->adv_handle = p_data->p_big_mgr_read_iso_tx_sync_rsp->adv_handle;
				p_tx_sync_info->packet_sequence_number = p_data->p_big_mgr_read_iso_tx_sync_rsp->packet_sequence_number;
				p_tx_sync_info->time_stamp = p_data->p_big_mgr_read_iso_tx_sync_rsp->time_stamp;
				p_tx_sync_info->time_offset = p_data->p_big_mgr_read_iso_tx_sync_rsp->time_offset;
			}
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_BIG_MGR_READ_ISO_TX_SYNC: find no pending command \r\n", __func__);
		}
	}
	break;
	default:
		break;
	}
	UNUSED(big_handle);

	return result;
}
static uint16_t bt_stack_le_iso_big_broadcaster_init(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	rtk_bt_le_iso_big_init_param_t *param = NULL;
	uint8_t big_num = 0, bis_num = 0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_big_init_param_t *)data;
	big_num = param->big_num;
	bis_num = param->bis_num;

	if (big_num > RTK_BLE_BIG_BROADCASTER_MAX_NUM || bis_num > RTK_BLE_BIS_BROADCASTER_MAX_NUM) {
		BT_LOGE("%s big_num(%d) > max(%d) or bis_num(%d) > max(%d)\r\n", __func__,
				big_num, RTK_BLE_BIG_BROADCASTER_MAX_NUM, bis_num, RTK_BLE_BIS_BROADCASTER_MAX_NUM);
		return RTK_BT_ERR_NO_RESOURCE;
	}

	cause = gap_big_mgr_isoc_broadcaster_init(big_num, bis_num, bt_stack_le_iso_big_broadcaster_cb);
	if (cause) {
		BT_LOGE("%s gap_big_mgr_isoc_broadcaster_init fail (cause = 0x%x)\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	g_bt_stack_le_iso_role = RTK_BLE_ISO_ROLE_BIS_BROADCASTER;

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_big_broadcaster_create(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	T_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_PARAM create_big_param = {0};
	rtk_bt_le_iso_big_broadcaster_create_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_big_broadcaster_create_param_t *)data;

	if (!param->p_big_param) {
		BT_LOGE("%s fail: p_big_param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	memcpy(&create_big_param, param->p_big_param, sizeof(T_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_PARAM));
	g_bt_stack_le_iso_bis_data_send_max_sdu = create_big_param.max_sdu;

	if (param->big_handle == NULL) {
		BT_LOGE("%s create_big_param.big_handle is NULL\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	*param->big_handle = GAP_INVALID_BIG_HANDLE;
	cause = gap_big_mgr_isoc_broadcaster_create_big(param->adv_handle, &create_big_param, param->big_handle);
	if (cause) {
		BT_LOGE("%s gap_big_mgr_isoc_broadcaster_create_big fail (cause = 0x%x)\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_big_broadcaster_terminate(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint8_t big_handle = 0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	big_handle = *(uint8_t *)data;

	cause = gap_big_mgr_isoc_broadcaster_terminate_big(big_handle, HCI_ERR_LOCAL_HOST_TERMINATE);
	if (cause) {
		BT_LOGE("%s gap_big_mgr_isoc_broadcaster_terminate_big fail (cause = 0x%x)\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_big_broadcaster_read_tx_sync(void *data)
{
	rtk_bt_le_iso_big_broadcaster_read_tx_sync_t *param = NULL;
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	param = (rtk_bt_le_iso_big_broadcaster_read_tx_sync_t *)data;

	if (!param->bis_conn_handle) {
		BT_LOGE("%s fail: bis_conn_handle error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cause = gap_big_mgr_read_iso_tx_sync(param->bis_conn_handle);
	if (cause) {
		BT_LOGE("%s gap_big_mgr_read_iso_tx_sync fail (cause = 0x%x)\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

T_BIG_MGR_SYNC_RECEIVER_DEV_STATE rtk_stack_iso_big_sync_receiver_dev_state = {0, 0};
void rtk_stack_iso_handle_big_receiver_sync_state_evt(uint8_t big_handle, uint8_t sync_id,
													  uint16_t sync_handle,
													  T_GAP_BIG_SYNC_RECEIVER_SYNC_STATE new_state, uint16_t cause)
{
	uint8_t cb_type = MSG_BIG_MGR_SYNC_RECEIVER_SYNC_STATE_CHANGE_INFO;
	rtk_bt_cmd_t *p_cmd = NULL;
	rtk_bt_evt_t *p_evt = NULL;
	bool indicate = false;

	BT_LOGD("%s: big_handle 0x%x, sync_id 0x%x, sync_handle 0x%x, new_state 0x%x, cause 0x%x\r\n", __func__,
			big_handle, sync_id, sync_handle, new_state, cause);

	switch (new_state) {
	case BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATED: {
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATED: find no pending command \r\n", __func__);
		}
		indicate = true;
		break;
	}
	case BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED: {
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED: find no pending command \r\n", __func__);
		}
		indicate = true;
		break;
	}
	default:
		break;
	}

	if (indicate) {
		rtk_bt_le_iso_big_receiver_sync_state_ind_t *sync_state_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_ISO,
									RTK_BT_LE_ISO_EVT_BIG_RECEIVER_SYNC_STATE_IND,
									sizeof(rtk_bt_le_iso_big_receiver_sync_state_ind_t));
		sync_state_ind = (rtk_bt_le_iso_big_receiver_sync_state_ind_t *)p_evt->data;
		sync_state_ind->big_handle = big_handle;
		sync_state_ind->sync_state = (rtk_bt_le_iso_big_receiver_sync_state_t)new_state;
		sync_state_ind->sync_id = sync_id;
		sync_state_ind->sync_handle = sync_handle;
		sync_state_ind->cause = cause;
		rtk_bt_evt_indicate(p_evt, NULL);
	}
}
void rtk_stack_iso_handle_bis_receiver_dev_state_evt(uint8_t big_handle, T_BIG_MGR_SYNC_RECEIVER_DEV_STATE new_state, uint16_t cause)
{
	BT_LOGD("%s: big_handle 0x%x, new_state.gap_big_create_sync_state 0x%x, cause 0x%x\r\n", __func__, big_handle,
			new_state.gap_big_create_sync_state, cause);

	if (rtk_stack_iso_big_sync_receiver_dev_state.gap_big_create_sync_state !=
		new_state.gap_big_create_sync_state) {
		if (new_state.gap_big_create_sync_state == GAP_BIG_MGR_SYNC_RECEIVER_CREATE_SYNC_DEV_STATE_IDLE) {
		} else if (new_state.gap_big_create_sync_state == GAP_BIG_MGR_SYNC_RECEIVER_CREATE_SYNC_DEV_STATE_SYNCHRONIZING) {
		}
	}

	rtk_stack_iso_big_sync_receiver_dev_state = new_state;
}
T_APP_RESULT bt_stack_le_iso_big_receiver_cb(uint8_t big_handle, uint8_t cb_type, void *p_cb_data)
{
	rtk_bt_cmd_t *p_cmd = NULL;
	rtk_bt_evt_t *p_evt = NULL;
	T_APP_RESULT result = APP_RESULT_SUCCESS;
	T_BIG_MGR_CB_DATA *p_data = (T_BIG_MGR_CB_DATA *)p_cb_data;

	switch (cb_type) {
	case MSG_BIG_MGR_SETUP_DATA_PATH: {
		APP_PRINT_INFO3("MSG_BIG_MGR_SETUP_DATA_PATH: cause 0x%x, big_handle 0x%x, bis_conn_handle 0x%x",
						p_data->p_big_mgr_setup_data_path_rsp->cause,
						p_data->p_big_mgr_setup_data_path_rsp->big_handle,
						p_data->p_big_mgr_setup_data_path_rsp->bis_conn_handle);
		BT_LOGA("MSG_BIG_MGR_SETUP_DATA_PATH: cause 0x%x, big_handle 0x%x, bis_conn_handle 0x%x\r\n",
				p_data->p_big_mgr_setup_data_path_rsp->cause,
				p_data->p_big_mgr_setup_data_path_rsp->big_handle,
				p_data->p_big_mgr_setup_data_path_rsp->bis_conn_handle);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_big_mgr_setup_data_path_rsp->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_BIG_MGR_SETUP_DATA_PATH: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_BIG_MGR_REMOVE_DATA_PATH: {
		APP_PRINT_INFO3("MSG_BIG_MGR_REMOVE_DATA_PATH: cause 0x%x, big_handle 0x%x, bis_conn_handle 0x%x",
						p_data->p_big_mgr_remove_data_path_rsp->cause,
						p_data->p_big_mgr_remove_data_path_rsp->big_handle,
						p_data->p_big_mgr_remove_data_path_rsp->bis_conn_handle);
		BT_LOGA("MSG_BIG_MGR_REMOVE_DATA_PATH: cause 0x%x, big_handle 0x%x, bis_conn_handle 0x%x",
				p_data->p_big_mgr_remove_data_path_rsp->cause,
				p_data->p_big_mgr_remove_data_path_rsp->big_handle,
				p_data->p_big_mgr_remove_data_path_rsp->bis_conn_handle);
		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_big_mgr_remove_data_path_rsp->cause;
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_BIG_MGR_REMOVE_DATA_PATH: find no pending command \r\n", __func__);
		}
	}
	break;

	case MSG_BIG_MGR_SYNC_RECEIVER_DEV_STATE_CHANGE_INFO: {
		rtk_stack_iso_handle_bis_receiver_dev_state_evt(big_handle,
														p_data->p_big_mgr_sync_receiver_dev_state_change_info->state,
														p_data->p_big_mgr_sync_receiver_dev_state_change_info->cause);
	}
	break;

	case MSG_BIG_MGR_SYNC_RECEIVER_SYNC_STATE_CHANGE_INFO: {
		rtk_stack_iso_handle_big_receiver_sync_state_evt(
			p_data->p_big_mgr_sync_receiver_sync_state_change_info->big_handle,
			p_data->p_big_mgr_sync_receiver_sync_state_change_info->sync_id,
			p_data->p_big_mgr_sync_receiver_sync_state_change_info->sync_handle,
			p_data->p_big_mgr_sync_receiver_sync_state_change_info->state,
			p_data->p_big_mgr_sync_receiver_sync_state_change_info->cause);
	}
	break;

	case MSG_BIG_MGR_SYNC_RECEIVER_BIG_SYNC_ESTABLISHED_INFO: {
		APP_PRINT_INFO6("MSG_BIG_MGR_SYNC_RECEIVER_BIG_SYNC_ESTABLISHED_INFO: big_handle 0x%x, sync_handle 0x%x, sync_id %d, cause 0x%x, transport_latency_big 0x%x, nse %d",
						p_data->p_big_mgr_sync_rx_big_sync_established_info->big_handle,
						p_data->p_big_mgr_sync_rx_big_sync_established_info->sync_handle,
						p_data->p_big_mgr_sync_rx_big_sync_established_info->sync_id,
						p_data->p_big_mgr_sync_rx_big_sync_established_info->cause,
						p_data->p_big_mgr_sync_rx_big_sync_established_info->transport_latency_big,
						p_data->p_big_mgr_sync_rx_big_sync_established_info->nse);
		APP_PRINT_INFO8("MSG_BIG_MGR_SYNC_RECEIVER_BIG_SYNC_ESTABLISHED_INFO: big_handle 0x%x, sync_handle 0x%x, bn %d, pto %d, irc %d, max_pdu %d, iso_interval 0x%x, num_bis %d",
						p_data->p_big_mgr_sync_rx_big_sync_established_info->big_handle,
						p_data->p_big_mgr_sync_rx_big_sync_established_info->sync_handle,
						p_data->p_big_mgr_sync_rx_big_sync_established_info->bn,
						p_data->p_big_mgr_sync_rx_big_sync_established_info->pto,
						p_data->p_big_mgr_sync_rx_big_sync_established_info->irc,
						p_data->p_big_mgr_sync_rx_big_sync_established_info->max_pdu,
						p_data->p_big_mgr_sync_rx_big_sync_established_info->iso_interval,
						p_data->p_big_mgr_sync_rx_big_sync_established_info->num_bis);
		for (uint8_t i = 0; i < p_data->p_big_mgr_sync_rx_big_sync_established_info->num_bis; i++) {
			APP_PRINT_INFO2("MSG_BIG_MGR_SYNC_RECEIVER_BIG_SYNC_ESTABLISHED_INFO: bis index %d, bis_conn_handle 0x%x",
							p_data->p_big_mgr_sync_rx_big_sync_established_info->bis_conn_handle_info[i].bis_idx,
							p_data->p_big_mgr_sync_rx_big_sync_established_info->bis_conn_handle_info[i].bis_conn_handle);
			BT_LOGD("MSG_BIG_MGR_SYNC_RECEIVER_BIG_SYNC_ESTABLISHED_INFO: bis index %d, bis_conn_handle 0x%x\r\n",
					p_data->p_big_mgr_sync_rx_big_sync_established_info->bis_conn_handle_info[i].bis_idx,
					p_data->p_big_mgr_sync_rx_big_sync_established_info->bis_conn_handle_info[i].bis_conn_handle);
		}
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_ISO,
									RTK_BT_LE_ISO_EVT_BIG_RECEIVER_SYNC_ESTABLISHED_INFO,
									sizeof(rtk_bt_le_iso_big_receiver_sync_established_info_t));
		memcpy((void *)p_evt->data, (void *)p_data->p_big_mgr_sync_rx_big_sync_established_info, sizeof(T_BIG_MGR_SYNC_RECEIVER_BIG_SYNC_ESTABLISHED_INFO));
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;
	case MSG_BIG_MGR_READ_ISO_LINK_QUALITY: {
		APP_PRINT_INFO7("MSG_BIG_MGR_READ_ISO_LINK_QUALITY: cause 0x%x, bis_conn_handle 0x%x, big_handle 0x%x, sync_id %d, sync_handle 0%x, crc_error_packets 0x%x, rx_unreceived_packets 0x%x",
						p_data->p_big_mgr_read_iso_link_quality_rsp->cause,
						p_data->p_big_mgr_read_iso_link_quality_rsp->bis_conn_handle,
						p_data->p_big_mgr_read_iso_link_quality_rsp->big_handle,
						p_data->p_big_mgr_read_iso_link_quality_rsp->sync_id,
						p_data->p_big_mgr_read_iso_link_quality_rsp->sync_handle,
						p_data->p_big_mgr_read_iso_link_quality_rsp->crc_error_packets,
						p_data->p_big_mgr_read_iso_link_quality_rsp->rx_unreceived_packets);
		BT_LOGD(
			"MSG_BIG_MGR_READ_ISO_LINK_QUALITY: cause 0x%x, bis_conn_handle 0x%x, big_handle 0x%x, sync_id %d, sync_handle 0%x, crc_error_packets 0x%x, rx_unreceived_packets 0x%x\r\n",
			p_data->p_big_mgr_read_iso_link_quality_rsp->cause,
			p_data->p_big_mgr_read_iso_link_quality_rsp->bis_conn_handle,
			p_data->p_big_mgr_read_iso_link_quality_rsp->big_handle,
			p_data->p_big_mgr_read_iso_link_quality_rsp->sync_id,
			p_data->p_big_mgr_read_iso_link_quality_rsp->sync_handle,
			(unsigned int)p_data->p_big_mgr_read_iso_link_quality_rsp->crc_error_packets,
			(unsigned int)p_data->p_big_mgr_read_iso_link_quality_rsp->rx_unreceived_packets);

		p_cmd = bt_stack_pending_cmd_search(cb_type);
		if (p_cmd) {
			bt_stack_pending_cmd_delete(p_cmd);
			p_cmd->ret = p_data->p_big_mgr_read_iso_link_quality_rsp->cause;
			if (p_cmd->ret == 0) { //OK
				rtk_bt_le_iso_big_receiver_read_link_quality_t *param = (rtk_bt_le_iso_big_receiver_read_link_quality_t *)p_cmd->param;
				rtk_bt_le_iso_big_receiver_read_link_quality_info_t *p_link_quality_info = param->p_link_quality_info;
				p_link_quality_info->big_handle = p_data->p_big_mgr_read_iso_link_quality_rsp->big_handle;
				p_link_quality_info->sync_id = p_data->p_big_mgr_read_iso_link_quality_rsp->sync_id;
				p_link_quality_info->sync_handle = p_data->p_big_mgr_read_iso_link_quality_rsp->sync_handle;
				p_link_quality_info->crc_error_packets = p_data->p_big_mgr_read_iso_link_quality_rsp->crc_error_packets;
				p_link_quality_info->rx_unreceived_packets = p_data->p_big_mgr_read_iso_link_quality_rsp->rx_unreceived_packets;
			}
			osif_sem_give(p_cmd->psem);
		} else {
			BT_LOGE("[%s] MSG_BIG_MGR_READ_ISO_LINK_QUALITY: find no pending command \r\n", __func__);
		}
	}
	break;
	default:
		break;
	}
	return result;
}
static uint16_t bt_stack_le_iso_big_receiver_init(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	rtk_bt_le_iso_big_init_param_t *param = NULL;
	uint8_t big_num = 0, bis_num = 0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_big_init_param_t *)data;
	big_num = param->big_num;
	bis_num = param->bis_num;

	if (big_num > RTK_BLE_BIG_RECEIVER_MAX_NUM || bis_num > RTK_BLE_BIS_RECEIVER_MAX_NUM) {
		BT_LOGE("%s big_num(%d) > max(%d) or bis_num(%d) > max(%d)\r\n", __func__,
				big_num, RTK_BLE_BIG_RECEIVER_MAX_NUM, bis_num, RTK_BLE_BIS_RECEIVER_MAX_NUM);
		return RTK_BT_ERR_NO_RESOURCE;
	}

	cause = gap_big_mgr_sync_receiver_init(big_num, bis_num, bt_stack_le_iso_big_receiver_cb);
	if (cause) {
		BT_LOGE("%s gap_big_mgr_sync_receiver_init fail (cause = 0x%x)\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	g_bt_stack_le_iso_role = RTK_BLE_ISO_ROLE_BIS_RECEIVER;

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_big_receiver_create_sync(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	rtk_bt_le_iso_big_receiver_create_sync_t *param = NULL;
	T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM big_create_sync_param = {0};
	rtk_bt_le_iso_big_receiver_create_sync_param_t *p_sync_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_big_receiver_create_sync_t *)data;
	p_sync_param = (rtk_bt_le_iso_big_receiver_create_sync_param_t *)param->p_sync_param;
	if (!p_sync_param) {
		BT_LOGE("%s fail: p_sync_param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	big_create_sync_param.encryption = p_sync_param->encryption;
	memcpy(big_create_sync_param.broadcast_code, p_sync_param->broadcast_code, 16);
	big_create_sync_param.mse = p_sync_param->mse;
	big_create_sync_param.big_sync_timeout = p_sync_param->big_sync_timeout;
	big_create_sync_param.num_bis = p_sync_param->num_bis;
	for (uint8_t i = 0; i < p_sync_param->num_bis; i ++) {
		big_create_sync_param.bis[i] = i + 1;
	}
	BT_LOGD("%s big_create_sync_param.encryption 0x%x\r\n", __func__, big_create_sync_param.encryption);
	BT_LOGD("%s big_create_sync_param.broadcast_code[0] 0x%x\r\n", __func__, big_create_sync_param.broadcast_code[0]);
	BT_LOGD("%s big_create_sync_param.mse 0x%x\r\n", __func__, big_create_sync_param.mse);
	BT_LOGD("%s big_create_sync_param.big_sync_timeout 0x%x\r\n", __func__, big_create_sync_param.big_sync_timeout);
	BT_LOGD("%s big_create_sync_param.num_bis 0x%x\r\n", __func__, big_create_sync_param.num_bis);

	*param->p_big_handle = GAP_INVALID_BIG_HANDLE;

	cause = gap_big_mgr_sync_receiver_big_create_sync(param->sync_handle, &big_create_sync_param, param->p_big_handle);
	if (cause) {
		BT_LOGE("%s gap_big_mgr_sync_receiver_big_create_sync fail (cause = 0x%x)\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_big_receiver_terminate_sync(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	uint8_t big_handle = 0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	big_handle = *(uint8_t *)data;

	if (!big_handle) {
		BT_LOGE("%s fail: big_handle error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cause = gap_big_mgr_sync_receiver_big_terminate_sync(big_handle);
	if (cause) {
		BT_LOGE("%s gap_big_mgr_sync_receiver_big_terminate_sync fail (cause = 0x%x)\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}
static uint16_t bt_stack_le_iso_big_receiver_read_link_quality(void *data)
{
	rtk_bt_le_iso_big_receiver_read_link_quality_t *param = NULL;
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param = (rtk_bt_le_iso_big_receiver_read_link_quality_t *)data;

	if (!param->bis_conn_handle) {
		BT_LOGE("%s fail: bis_conn_handle error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cause = gap_big_mgr_read_iso_link_quality(param->bis_conn_handle);
	if (cause) {
		BT_LOGE("%s gap_big_mgr_read_iso_link_quality fail (cause = 0x%x)\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_iso_big_setup_path(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	rtk_bt_le_iso_setup_path_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	param = (rtk_bt_le_iso_setup_path_param_t *)data;

	if (!param->iso_conn_handle) {
		BT_LOGE("%s fail: bis_conn_handle error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cause = gap_big_mgr_setup_data_path(param->iso_conn_handle, param->data_path_direction, param->data_path_id,
										param->codec_id, param->controller_delay, param->codec_config_len, param->p_codec_config);
	if (cause) {
		BT_LOGE("%s gap_big_mgr_setup_data_path fail (cause = 0x%x)\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}
static uint16_t bt_stack_le_iso_big_remove_path(void *data)
{
	T_GAP_CAUSE cause = (T_GAP_CAUSE)0;
	rtk_bt_le_iso_remove_path_param_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	param = (rtk_bt_le_iso_remove_path_param_t *)data;

	if (!param->iso_conn_handle) {
		BT_LOGE("%s fail: iso_conn_handle error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	cause = gap_big_mgr_remove_data_path(param->iso_conn_handle, param->data_path_direction);
	if (cause) {
		BT_LOGE("%s gap_big_mgr_remove_data_path fail (cause = 0x%x)\r\n", __func__, cause);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

uint16_t bt_stack_le_iso_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = 0;
	BT_LOGD("bt_stack_le_iso_act_handle: act = %d \r\n", p_cmd->act);
	switch (p_cmd->act) {
	case RTK_BT_LE_ISO_ACT_CIG_SETUP_PATH:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_SETUP_PATH \r\n");
		p_cmd->user_data = MSG_CIG_MGR_SETUP_DATA_PATH;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_cig_setup_path(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_CIG_REMOVE_PATH:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_REMOVE_PATH \r\n");
		p_cmd->user_data = MSG_CIG_MGR_REMOVE_DATA_PATH;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_cig_remove_path(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_CIG_DISCONNECT:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_DISCONNECT \r\n");
		p_cmd->user_data = MSG_CIG_MGR_DISCONNECT_INFO;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_cig_disconnect(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_CIG_READ_ISO_TX_SYNC:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_READ_ISO_TX_SYNC \r\n");
		p_cmd->user_data = MSG_CIG_MGR_READ_ISO_TX_SYNC;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_cig_read_iso_tx_sync(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_CIG_READ_LINK_QUALITY:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_READ_LINK_QUALITY \r\n");
		p_cmd->user_data = MSG_CIG_MGR_READ_ISO_LINK_QUALITY;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_cig_read_link_quality(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_CIG_INITIATOR_ADD_CIS:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_INITIATOR_ADD_CIS \r\n");
		ret = bt_stack_le_iso_cig_initiator_add_cis(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_CIG_INITIATOR_SET_CIG_PARAM:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_INITIATOR_SET_CIG_PARAM \r\n");
		ret = bt_stack_le_iso_cig_initiator_set_cig_param(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_CIG_INITIATOR_SET_CIS_PARAM:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_INITIATOR_SET_CIS_PARAM \r\n");
		ret = bt_stack_le_iso_cig_initiator_set_cis_param(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_CIG_INITIATOR_START_SETTING:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_INITIATOR_START_SETTING \r\n");
		p_cmd->user_data = MSG_CIG_MGR_START_SETTING;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_cig_initiator_start_setting(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_CIG_INITIATOR_SET_CIS_ACL_LINK:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_INITIATOR_SET_CIS_ACL_LINK \r\n");
		ret = bt_stack_le_iso_cig_initiator_set_cis_acl_link(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_CIG_INITIATOR_CREATE_CIS_BY_CIG_ID:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_INITIATOR_CREATE_CIS_BY_CIG_ID \r\n");
		p_cmd->user_data = MSG_CIG_MGR_CREATE_CIS;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_cig_initiator_create_cis_by_cig_id(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_CIG_INITIATOR_CREATE_CIS_BY_CIG_CONN_HANDLE:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_INITIATOR_CREATE_CIS_BY_CIG_CONN_HANDLE \r\n");
		p_cmd->user_data = MSG_CIG_MGR_CREATE_CIS;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_cig_initiator_create_cis_by_cis_conn_handle(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_CIG_INITIATOR_REMOVE_CIG:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_INITIATOR_REMOVE_CIG \r\n");
		p_cmd->user_data = MSG_CIG_MGR_REMOVE_CIG;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_cig_initiator_remove_cig(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_ACCEPT_CIS:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_ACCEPT_CIS \r\n");
		p_cmd->user_data = MSG_CIG_MGR_ACCEPT_CIS_INFO;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_cig_acceptor_accept_cis(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_REJECT_CIS:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_REJECT_CIS \r\n");
		p_cmd->user_data = MSG_CIG_MGR_REJECT_CIS_INFO;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_cig_acceptor_reject_cis(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_CONFIG_SDU_PARAM:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_CONFIG_SDU_PARAM \r\n");
		ret = bt_stack_le_iso_cig_acceptor_config_sdu_param(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_CONFIG_CIS_REQ_ACTION:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_CONFIG_CIS_REQ_ACTION \r\n");
		ret = bt_stack_le_iso_cig_acceptor_config_cis_req_action(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_REGISTER_CALLBACK:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_REGISTER_CALLBACK \r\n");
		ret = bt_stack_le_iso_cig_acceptor_register_callback(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_CIG_INITIATOR_REGISTER_CALLBACK:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_INITIATOR_REGISTER_CALLBACK \r\n");
		ret = bt_stack_le_iso_cig_initiator_register_callback(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_CIG_GET_CONN_HANDLE:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_GET_CONN_HANDLE \r\n");
		ret = bt_stack_le_iso_cig_get_conn_handle(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_CIG_GET_CIS_INFO:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_GET_CIS_INFO \r\n");
		ret = bt_stack_le_iso_cig_get_cis_info(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_CIG_GET_ISOCH_INFO:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_GET_ISOCH_INFO \r\n");
		ret = bt_stack_le_iso_cig_get_isoch_info(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_CIG_INITIATOR_GET_CIS_CONN_HANDLE:
		BT_LOGD("RTK_BT_LE_ISO_ACT_CIG_INITIATOR_GET_CIS_CONN_HANDLE \r\n");
		ret = bt_stack_le_iso_cig_initiator_get_cis_conn_handle(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_INIT:
		BT_LOGD("RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_INIT \r\n");
		ret = bt_stack_le_iso_big_broadcaster_init(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_CREATE:
		BT_LOGD("RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_CREATE \r\n");
		p_cmd->user_data = MSG_BIG_MGR_ISOC_BROADCAST_STATE_CHANGE_INFO;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_big_broadcaster_create(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_TERMINATE:
		BT_LOGD("RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_TERMINATE \r\n");
		p_cmd->user_data = MSG_BIG_MGR_ISOC_BROADCAST_STATE_CHANGE_INFO;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_big_broadcaster_terminate(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_READ_TX_SYNC:
		BT_LOGD("RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_READ_TX_SYNC \r\n");
		p_cmd->user_data = MSG_BIG_MGR_READ_ISO_TX_SYNC;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_big_broadcaster_read_tx_sync(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_BIG_RECEIVER_INIT:
		BT_LOGD("RTK_BT_LE_ISO_ACT_BIG_RECEIVER_INIT \r\n");
		ret = bt_stack_le_iso_big_receiver_init(p_cmd->param);
		break;
	case RTK_BT_LE_ISO_ACT_BIG_RECEIVER_CREATE_SYNC:
		BT_LOGD("RTK_BT_LE_ISO_ACT_BIG_RECEIVER_CREATE_SYNC \r\n");
		p_cmd->user_data = MSG_BIG_MGR_SYNC_RECEIVER_SYNC_STATE_CHANGE_INFO;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_big_receiver_create_sync(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_BIG_RECEIVER_TERMINATE_SYNC:
		BT_LOGD("RTK_BT_LE_ISO_ACT_BIG_RECEIVER_TERMINATE_SYNC \r\n");
		p_cmd->user_data = MSG_BIG_MGR_SYNC_RECEIVER_SYNC_STATE_CHANGE_INFO;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_big_receiver_terminate_sync(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_BIG_RECEIVER_READ_LINK_QUALITY:
		BT_LOGD("RTK_BT_LE_ISO_ACT_BIG_RECEIVER_READ_LINK_QUALITY \r\n");
		p_cmd->user_data = MSG_BIG_MGR_READ_ISO_LINK_QUALITY;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_big_receiver_read_link_quality(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_BIG_SETUP_PATH:
		BT_LOGD("RTK_BT_LE_ISO_ACT_BIG_SETUP_PATH \r\n");
		p_cmd->user_data = MSG_BIG_MGR_SETUP_DATA_PATH;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_big_setup_path(p_cmd->param);
		goto async_handle;
	case RTK_BT_LE_ISO_ACT_BIG_REMOVE_PATH:
		BT_LOGD("RTK_BT_LE_ISO_ACT_BIG_REMOVE_PATH \r\n");
		p_cmd->user_data = MSG_BIG_MGR_REMOVE_DATA_PATH;
		bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_iso_big_remove_path(p_cmd->param);
		goto async_handle;

	case RTK_BT_LE_ISO_ACT_ISO_DATA_SEND:
		BT_LOGD("RTK_BT_LE_ISO_ACT_ISO_DATA_SEND \r\n");
		ret = bt_stack_le_iso_data_send(p_cmd->param);
		break;

	default:
		BT_LOGE("bt_stack_le_iso_act_handle: unknown act: %d \r\n", p_cmd->act);
		ret = 0;
		break;
	}

	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;

async_handle:
	if (ret) {
		/*  if bt_stack_xxx failed, shall not wait for callback, and the pending p_cmd
		    should be deleted here */
		bt_stack_pending_cmd_delete(p_cmd);
		p_cmd->ret = ret;
		osif_sem_give(p_cmd->psem);
	}
	return ret;
}
#endif //end of #if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT