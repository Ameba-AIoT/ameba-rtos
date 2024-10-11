/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

/* -------------------------------- Includes -------------------------------- */
#include <bt_api_config.h>
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <stdio.h>
#include <string.h>
#include <dlist.h>
#include <osif.h>
#include <rtk_bt_br_gap.h>
#include <rtk_bt_rfc.h>
#include <gap_br.h>
#include <btm.h>
#include <trace_app.h>
#include <remote.h>
#include <bt_rfc.h>
#include <bt_sdp.h>

static uint8_t rfc_channel;
extern T_APP_BR_LINK *app_find_br_link(uint8_t *bd_addr);

static void bt_stack_rfc_evt_ind_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
	(void)buf_len;
	T_BT_EVENT_PARAM *param = (T_BT_EVENT_PARAM *)event_buf;
	rtk_bt_evt_t *p_evt = NULL;
	switch (event_type) {
	case BT_EVENT_SDP_ATTR_INFO: {
		rtk_bt_rfc_attr_info_t *p_info = NULL;
		T_BT_SDP_ATTR_INFO *sdp_info = &param->sdp_attr_info.info;
		APP_PRINT_INFO0("BT_EVENT_SDP_ATTR_INFO");
		BT_LOGA("bt_stack_rfc_evt_ind_cback: BT_EVENT_SDP_ATTR_INFO\r\n");
		if (!bt_rfc_conn_req(param->sdp_attr_info.bd_addr, rfc_channel, sdp_info->server_channel, RTK_BT_RFC_DEMO_DEFAULT_MTU_SIZE, RTK_BT_RFC_DEMO_DEFAULT_CREDITS)) {
			BT_LOGE("bt_stack_rfc_evt_ind_cback: bt_rfc_conn_req send failed\r\n");
		}
		{
			/* Create event*/
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_RFC, RTK_BT_RFC_EVT_RFC_ATTR_INFO, sizeof(rtk_bt_rfc_attr_info_t));
			if (!p_evt) {
				BT_LOGE("bt_stack_rfc_evt_ind_cback: evt_t allocate failed \r\n");
				break;
			}
			p_info = (rtk_bt_rfc_attr_info_t *)p_evt->data;
			memcpy((void *)p_info->bd_addr, (void *)param->sdp_attr_info.bd_addr, 6);
			p_info->srv_class_uuid_type = (rtk_bt_rfc_uuid_type_t)sdp_info->srv_class_uuid_type;
			memcpy((void *)&p_info->srv_class_uuid_data, (void *)&sdp_info->srv_class_uuid_data, sizeof(T_GAP_UUID_DATA));
			p_info->profile_version = sdp_info->profile_version;
			p_info->protocol_version = sdp_info->protocol_version;
			p_info->server_channel = sdp_info->server_channel;
			p_info->supported_feat = sdp_info->supported_feat;
			p_info->l2c_psm = sdp_info->l2c_psm;
			p_info->supported_repos = sdp_info->supported_repos;
			p_info->pbap_supported_feat = sdp_info->pbap_supported_feat;
			/* Send event */
			rtk_bt_evt_indicate(p_evt, NULL);
			break;
		}
		BT_LOGE("bt_stack_rfc_evt_ind_cback: Get sdp attr discov info, but uuid is not registered!!\r\n");
	}
	break;

	default: {
		//BT_LOGE("bt_stack_rfc_evt_ind_cback:default event_type 0x%04x\r\n", event_type);
	}
	break;
	}
}

static void app_rfc_cback(uint8_t *bd_addr, T_BT_RFC_MSG_TYPE msg_type, void *msg_buf)
{
	rtk_bt_evt_t *p_evt = NULL;
	T_APP_BR_LINK *p_link;
	switch (msg_type) {
	case BT_RFC_MSG_CONN_IND: {
		rtk_bt_rfc_conn_ind_t *p_rfc_conn_ind = NULL;

		p_link = app_find_br_link(bd_addr);
		if (p_link != NULL) {
			T_BT_RFC_CONN_IND *p_ind = (T_BT_RFC_CONN_IND *)msg_buf;

			APP_PRINT_INFO0("BT_RFC_MSG_CONN_IND");
			BT_LOGA("app_rfc_cback: BT_RFC_MSG_CONN_IND \r\n");
			if (p_ind->local_server_chann != rfc_channel) {
				APP_PRINT_INFO1("app_rfc_cback(): Recv unexpected rfc conn ind %u",
								p_ind->local_server_chann);
				break;
			}
			bt_rfc_conn_cfm(p_link->bd_addr, p_ind->local_server_chann, true, p_ind->frame_size, 100);
			{
				p_evt = rtk_bt_event_create(RTK_BT_BR_GP_RFC, RTK_BT_RFC_EVT_CONN_IND, sizeof(rtk_bt_rfc_conn_ind_t));
				if (!p_evt) {
					BT_LOGE("app_rfc_cback: evt_t allocate fail \r\n");
					break;
				}
				p_rfc_conn_ind = (rtk_bt_rfc_conn_ind_t *)p_evt->data;
				memcpy((void *)p_rfc_conn_ind->bd_addr, (void *)p_link->bd_addr, 6);
				p_rfc_conn_ind->local_server_chann = p_ind->local_server_chann;
				p_rfc_conn_ind->frame_size = p_ind->frame_size;
				/* Send event */
				if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
					break;
				}
			}
		} else {
			APP_PRINT_INFO0("RFC p_link is NULL");
			BT_LOGE("app_rfc_cback: RFC p_link is NULL \r\n");
		}
	}
	break;

	case BT_RFC_MSG_CONN_CMPL: {
		rtk_bt_rfc_conn_cmpl_t *p_rfc_conn_cmpl_ind = NULL;

		p_link = app_find_br_link(bd_addr);
		if (p_link != NULL) {
			T_BT_RFC_CONN_CMPL *p_cmpl = (T_BT_RFC_CONN_CMPL *)msg_buf;

			APP_PRINT_INFO0("BT_RFC_MSG_CONN_CMPL");
			BT_LOGA("app_rfc_cback: BT_RFC_MSG_CONN_CMPL \r\n");
			if (p_cmpl->local_server_chann != rfc_channel) {
				APP_PRINT_INFO1("app_rfc_cback(): Recv unexpected rfc conn cmpl %u",
								p_cmpl->local_server_chann);
				break;
			}
			{
				p_evt = rtk_bt_event_create(RTK_BT_BR_GP_RFC, RTK_BT_RFC_EVT_CONN_CMPL, sizeof(rtk_bt_rfc_conn_cmpl_t));
				if (!p_evt) {
					BT_LOGE("app_rfc_cback: evt_t allocate fail \r\n");
					break;
				}
				p_rfc_conn_cmpl_ind = (rtk_bt_rfc_conn_cmpl_t *)p_evt->data;
				memcpy((void *)p_rfc_conn_cmpl_ind->bd_addr, (void *)p_link->bd_addr, 6);
				p_rfc_conn_cmpl_ind->local_server_chann = p_cmpl->local_server_chann;
				p_rfc_conn_cmpl_ind->frame_size = p_cmpl->frame_size;
				p_rfc_conn_cmpl_ind->remain_credits = p_cmpl->remain_credits;
				/* Send event */
				if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
					break;
				}
			}
		} else {
			APP_PRINT_INFO0("RFC p_link is NULL");
			BT_LOGE("app_rfc_cback: RFC p_link is NULL \r\n");
		}
	}
	break;

	case BT_RFC_MSG_DISCONN_CMPL: {
		rtk_bt_rfc_disconn_cmpl_t *p_rfc_disconn_cmpl_ind = NULL;

		p_link = app_find_br_link(bd_addr);
		if (p_link != NULL) {
			T_BT_RFC_DISCONN_CMPL *p_info = (T_BT_RFC_DISCONN_CMPL *)msg_buf;

			APP_PRINT_INFO0("BT_RFC_MSG_DISCONN_CMPL");
			BT_LOGA("app_rfc_cback: BT_RFC_MSG_DISCONN_CMPL \r\n");
			if (p_info->local_server_chann != rfc_channel) {
				APP_PRINT_INFO1("app_rfc_cback(): Recv unexpected rfc disconn cmpl %u",
								p_info->local_server_chann);
				break;
			}
			{
				p_evt = rtk_bt_event_create(RTK_BT_BR_GP_RFC, RTK_BT_RFC_EVT_DISCONN_CMPL, sizeof(rtk_bt_rfc_disconn_cmpl_t));
				if (!p_evt) {
					BT_LOGE("app_rfc_cback: evt_t allocate fail \r\n");
					break;
				}
				p_rfc_disconn_cmpl_ind = (rtk_bt_rfc_disconn_cmpl_t *)p_evt->data;
				memcpy((void *)p_rfc_disconn_cmpl_ind->bd_addr, (void *)p_link->bd_addr, 6);
				p_rfc_disconn_cmpl_ind->local_server_chann = p_info->local_server_chann;
				p_rfc_disconn_cmpl_ind->cause = p_info->cause;
				/* Send event */
				if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
					break;
				}
			}
		} else {
			APP_PRINT_INFO0("RFC p_link is NULL");
			BT_LOGE("app_rfc_cback: RFC p_link is NULL \r\n");
		}
	}
	break;

	case BT_RFC_MSG_DATA_IND: {
		rtk_bt_rfc_data_ind_t *p_rfc_data_ind = NULL;

		p_link = app_find_br_link(bd_addr);
		if (p_link != NULL) {
			T_BT_RFC_DATA_IND *p_ind = (T_BT_RFC_DATA_IND *)msg_buf;

			APP_PRINT_INFO0("BT_RFC_MSG_DATA_IND");
			BT_LOGA("app_rfc_cback: BT_RFC_MSG_DATA_IND \r\n");
			if (p_ind->local_server_chann != rfc_channel) {
				APP_PRINT_INFO1("app_rfc_cback(): Recv unexpected data from %u",
								p_ind->local_server_chann);
				break;
			}
			bt_rfc_credits_give(bd_addr, p_ind->local_server_chann, 1);
			{
				p_evt = rtk_bt_event_create(RTK_BT_BR_GP_RFC, RTK_BT_RFC_EVT_DATA_IND, sizeof(rtk_bt_rfc_data_ind_t));
				if (!p_evt) {
					BT_LOGE("app_rfc_cback: evt_t allocate fail \r\n");
					break;
				}
				p_rfc_data_ind = (rtk_bt_rfc_data_ind_t *)p_evt->data;
				memcpy((void *)p_rfc_data_ind->bd_addr, (void *)p_link->bd_addr, 6);
				p_rfc_data_ind->local_server_chann = p_ind->local_server_chann;
				p_rfc_data_ind->remain_credits = p_ind->remain_credits;
				p_rfc_data_ind->length = p_ind->length;
				p_rfc_data_ind->buf = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_rfc_data_ind->length);
				if (NULL == p_rfc_data_ind->buf) {
					BT_LOGE("app_rfc_cback: data buf allocate fail \r\n");
					rtk_bt_event_free(p_evt);
					break;
				}
				memcpy((void *)p_rfc_data_ind->buf, (void *)p_ind->buf, p_rfc_data_ind->length);
				/* Send event */
				if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
					break;
				}
			}
		} else {
			APP_PRINT_INFO0("RFC p_link is NULL");
			BT_LOGE("app_rfc_cback: RFC p_link is NULL \r\n");
		}
	}
	break;

	case BT_RFC_MSG_CREDIT_INFO: {
		rtk_bt_rfc_credit_info_t *p_rfc_credit_info = NULL;
		p_link = app_find_br_link(bd_addr);
		if (p_link != NULL) {
			T_BT_RFC_CREDIT_INFO *p_info = (T_BT_RFC_CREDIT_INFO *)msg_buf;
			APP_PRINT_INFO0("BT_RFC_MSG_CREDIT_INFO");
			BT_LOGA("app_rfc_cback: BT_RFC_MSG_CREDIT_INFO \r\n");
			if (p_info->local_server_chann != rfc_channel) {
				APP_PRINT_INFO1("app_rfc_cback(): Recv unexpected data from %u",
								p_info->local_server_chann);
				break;
			}
			{
				p_evt = rtk_bt_event_create(RTK_BT_BR_GP_RFC, RTK_BT_RFC_EVT_CREDITS_INFO, sizeof(rtk_bt_rfc_credit_info_t));
				if (!p_evt) {
					BT_LOGE("app_rfc_cback: evt_t allocate fail \r\n");
					break;
				}
				p_rfc_credit_info = (rtk_bt_rfc_credit_info_t *)p_evt->data;
				p_rfc_credit_info->local_server_chann = p_info->local_server_chann;
				p_rfc_credit_info->remain_credits = p_info->remain_credits;
				/* Send event */
				if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
					break;
				}
			}
		} else {
			APP_PRINT_INFO0("RFC p_link is NULL");
			BT_LOGE("app_rfc_cback: RFC p_link is NULL \r\n");
		}

	}
	break;

	default: {
		//BT_LOGE("app_rfc_cback:default event_type 0x%04x\r\n", event_type);
	}
	break;
	}
}

static uint16_t bt_stack_rfc_connect(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;
	T_BT_SDP_UUID_DATA uuid;

	uuid.uuid_16 = UUID_RFCOMM;

	if (bt_sdp_discov_start(bd_addr, BT_SDP_UUID16, uuid)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_rfc_disconnect(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;
	T_APP_BR_LINK *p_link;

	p_link = app_find_br_link(bd_addr);
	if (p_link != NULL) {
		if (bt_rfc_disconn_req(bd_addr, rfc_channel)) {
			return RTK_BT_OK;
		}
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_rfc_send_data(void *param)
{
	rtk_bt_rfc_data_send_t *p_send_data_t = (rtk_bt_rfc_data_send_t *)param;
	T_APP_BR_LINK *p_link;

	p_link = app_find_br_link(p_send_data_t->bd_addr);
	if (!p_link) {
		BT_LOGE("bt_stack_rfc_send_data failed: no link found\r\n");
		return RTK_BT_FAIL;
	}
	if (bt_rfc_data_send(p_send_data_t->bd_addr,
						 p_send_data_t->server_chann,
						 p_send_data_t->p_data,
						 p_send_data_t->data_len)) {
		return RTK_BT_OK;
	}
	BT_LOGE("bt_stack_rfc_send_data failed: bt_rfc_data_send() failed\r\n");

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_rfc_credits_give(void *param)
{
	rtk_bt_rfc_credits_give_t *p_credits_give_t = (rtk_bt_rfc_credits_give_t *)param;
	T_APP_BR_LINK *p_link;

	p_link = app_find_br_link(p_credits_give_t->bd_addr);
	if (!p_link) {
		BT_LOGE("bt_stack_rfc_credits_give failed: no link found\r\n");
		return RTK_BT_FAIL;
	}
	if (bt_rfc_credits_give(p_credits_give_t->bd_addr,
							p_credits_give_t->local_server_chann,
							p_credits_give_t->credits)) {
		return RTK_BT_OK;
	}
	BT_LOGE("bt_stack_rfc_credits_give failed: bt_rfc_credits_give() failed\r\n");

	return RTK_BT_FAIL;
}

uint16_t bt_stack_rfc_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = 0;
	BT_LOGD("bt_stack_rfc_act_handle: act = %d \r\n", p_cmd->act);
	switch (p_cmd->act) {
	case RTK_BT_RFC_ACT_CONNECT:
		ret = bt_stack_rfc_connect(p_cmd->param);
		break;

	case RTK_BT_RFC_ACT_DISCONNECT:
		ret = bt_stack_rfc_disconnect(p_cmd->param);
		break;

	case RTK_BT_RFC_ACT_SEND_DATA:
		ret = bt_stack_rfc_send_data(p_cmd->param);
		break;

	case RTK_BT_RFC_ACT_CREDITS_GIVE:
		ret = bt_stack_rfc_credits_give(p_cmd->param);
		break;

	default:
		BT_LOGE("bt_stack_rfc_act_handle: unknown act: %d \r\n", p_cmd->act);
		ret = 0;
		break;
	}

	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);

	return ret;
}

uint16_t bt_stack_rfc_init(uint8_t server_chann)
{
	bt_rfc_profile_register(server_chann, app_rfc_cback);
	/* Register event callback function*/
	bt_mgr_cback_register(bt_stack_rfc_evt_ind_cback);
	rfc_channel = server_chann;

	return RTK_BT_OK;
}

void bt_stack_rfc_deinit(void)
{
	/* RFC deinit */
	BT_LOGA("rfc_demo_deinit\r\n");
}

#endif