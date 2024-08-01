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
#include <rtk_bt_hfp.h>
#include <rtk_bt_pbap.h>
#include <gap_br.h>
#include <stdio.h>
#include <string.h>
#include <dlist.h>
#include <osif.h>
#include <rtk_bt_br_gap.h>
#include <trace_app.h>
#include <bt_pbap.h>
#include <bt_sdp.h>
#include <bt_types.h>

static uint8_t hfp_role;
static uint8_t remote_addr[6] = {0};
extern T_APP_BR_LINK *app_find_br_link(uint8_t *bd_addr);

static void app_pbap_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
	(void)buf_len;
	T_BT_EVENT_PARAM *param = event_buf;
	T_APP_BR_LINK *p_link;
	bool handle = true;
	rtk_bt_evt_t *p_evt = NULL;

	switch (event_type) {

	case BT_EVENT_SDP_ATTR_INFO: {
		T_BT_SDP_ATTR_INFO *sdp_info = &param->sdp_attr_info.info;
		BT_LOGA("app_pbap_bt_cback: BT_EVENT_SDP_ATTR_INFO \r\n");
		if (bt_pbap_connect_req(param->sdp_attr_info.bd_addr, sdp_info->server_channel, sdp_info->supported_feat)) {
			BT_LOGA("bt_pbap_connect_req: SUCCESS \r\n");
		} else {
			BT_LOGA("bt_pbap_connect_req: FAIL \r\n");
		}
	}
	break;

	case BT_EVENT_PBAP_CONN_CMPL: {
		rtk_bt_pbap_conn_ind_t *p_pbap_conn_ind = NULL;
		BT_LOGA("app_pbap_bt_cback: PBAP Connected \r\n");
		p_link = app_find_br_link(param->pbap_conn_cmpl.bd_addr);
		if (p_link != NULL) {
			memcpy(remote_addr, param->pbap_conn_cmpl.bd_addr, 6);
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_EVT_CONN_CMPL, sizeof(rtk_bt_pbap_conn_ind_t));
			if (!p_evt) {
				BT_LOGE("app_pbap_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_pbap_conn_ind = (rtk_bt_pbap_conn_ind_t *)p_evt->data;
			memcpy((void *)p_pbap_conn_ind->bd_addr, (void *)param->pbap_conn_cmpl.bd_addr, 6);
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_PBAP_CONN_FAIL: {
		rtk_bt_pbap_conn_fail_ind_t *p_pbap_conn_fail_ind = NULL;
		BT_LOGA("app_pbap_bt_cback: PBAP connection fail \r\n");
		p_link = app_find_br_link(param->pbap_conn_fail.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_EVT_CONN_FAIL, sizeof(rtk_bt_pbap_conn_fail_ind_t));
			if (!p_evt) {
				BT_LOGE("app_pbap_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_pbap_conn_fail_ind = (rtk_bt_pbap_conn_fail_ind_t *)p_evt->data;
			memcpy((void *)p_pbap_conn_fail_ind->bd_addr, (void *)param->pbap_conn_fail.bd_addr, 6);
			p_pbap_conn_fail_ind->cause = param->pbap_conn_fail.cause;
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_PBAP_DISCONN_CMPL: {
		rtk_bt_pbap_disconn_cmpl_t *p_pbap_disc_cmpl_ind = NULL;
		BT_LOGA("app_pbap_bt_cback: PBAP Disconnected \r\n");
		p_evt = rtk_bt_event_create(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_EVT_DISCONN_CMPL, sizeof(rtk_bt_pbap_disconn_cmpl_t));
		if (!p_evt) {
			BT_LOGE("app_pbap_bt_cback: evt_t allocate fail \r\n");
			handle = false;
			break;
		}
		p_pbap_disc_cmpl_ind = (rtk_bt_pbap_disconn_cmpl_t *)p_evt->data;
		memcpy((void *)p_pbap_disc_cmpl_ind->bd_addr, (void *)param->pbap_disconn_cmpl.bd_addr, 6);
		p_pbap_disc_cmpl_ind->cause = param->pbap_disconn_cmpl.cause;
		/* Send event */
		if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
			handle = false;
			break;
		}
	}
	break;

	case BT_EVENT_PBAP_SET_PHONE_BOOK_CMPL: {
		rtk_bt_pbap_set_pb_cmpl_t *p_pbap_set_pb_cmpl = NULL;
		BT_LOGA("app_pbap_bt_cback: Set phone book completely \r\n");
		p_link = app_find_br_link(param->pbap_set_phone_book_cmpl.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_EVT_SET_PHONE_BOOK_CMPL, sizeof(rtk_bt_pbap_set_pb_cmpl_t));
			if (!p_evt) {
				BT_LOGE("app_pbap_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_pbap_set_pb_cmpl = (rtk_bt_pbap_set_pb_cmpl_t *)p_evt->data;
			memcpy((void *)p_pbap_set_pb_cmpl->bd_addr, (void *)param->pbap_set_phone_book_cmpl.bd_addr, 6);
			p_pbap_set_pb_cmpl->result = param->pbap_set_phone_book_cmpl.result;
			p_pbap_set_pb_cmpl->path = (rtk_bt_pbap_path)param->pbap_set_phone_book_cmpl.path;
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_PBAP_GET_PHONE_BOOK_CMPL: {
		rtk_bt_pbap_get_pb_cmpl_t *p_pbap_get_pb_cmpl = NULL;
		BT_LOGA("app_pbap_bt_cback: Get phone book completely \r\n");
		p_link = app_find_br_link(param->pbap_get_phone_book_cmpl.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_EVT_GET_PHONE_BOOK_CMPL, sizeof(rtk_bt_pbap_get_pb_cmpl_t));
			if (!p_evt) {
				BT_LOGE("app_pbap_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_pbap_get_pb_cmpl = (rtk_bt_pbap_get_pb_cmpl_t *)p_evt->data;
			memcpy((void *)p_pbap_get_pb_cmpl->bd_addr, (void *)param->pbap_get_phone_book_cmpl.bd_addr, 6);
			if (param->pbap_get_phone_book_cmpl.data_len) {
				p_pbap_get_pb_cmpl->p_data = osif_mem_alloc(RAM_TYPE_DATA_ON, param->pbap_get_phone_book_cmpl.data_len);
				if (!p_pbap_get_pb_cmpl->p_data) {
					BT_LOGE("app_pbap_bt_cback: p_data allocate fail \r\n");
					handle = false;
					break;
				}
				memcpy((void *)p_pbap_get_pb_cmpl->p_data, (void *)param->pbap_get_phone_book_cmpl.p_data, param->pbap_get_phone_book_cmpl.data_len);
				p_pbap_get_pb_cmpl->data_len = param->pbap_get_phone_book_cmpl.data_len;
			} else {
				p_pbap_get_pb_cmpl->data_len = 0;
			}
			p_pbap_get_pb_cmpl->pb_size = param->pbap_get_phone_book_cmpl.pb_size;
			p_pbap_get_pb_cmpl->new_missed_calls = param->pbap_get_phone_book_cmpl.new_missed_calls;
			p_pbap_get_pb_cmpl->data_end = param->pbap_get_phone_book_cmpl.data_end;
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_PBAP_CALLER_ID_NAME: {
		rtk_bt_pbap_caller_id_name_t *p_pbap_caller_id_name = NULL;
		BT_LOGA("app_pbap_bt_cback: Caller id name \r\n");
		p_link = app_find_br_link(param->pbap_caller_id_name.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_EVT_CALLER_ID_NAME, sizeof(rtk_bt_pbap_caller_id_name_t));
			if (!p_evt) {
				BT_LOGE("app_pbap_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_pbap_caller_id_name = (rtk_bt_pbap_caller_id_name_t *)p_evt->data;
			memcpy((void *)p_pbap_caller_id_name->bd_addr, (void *)param->pbap_caller_id_name.bd_addr, 6);
			if (param->pbap_caller_id_name.name_len > 0) {
				p_pbap_caller_id_name->name_ptr = osif_mem_alloc(RAM_TYPE_DATA_ON, param->pbap_caller_id_name.name_len);
				if (!p_pbap_caller_id_name->name_ptr) {
					BT_LOGE("app_pbap_bt_cback: name_ptr allocate fail \r\n");
					handle = false;
					break;
				}
				memcpy((void *)p_pbap_caller_id_name->name_ptr, (void *)param->pbap_caller_id_name.name_ptr, param->pbap_caller_id_name.name_len);
				p_pbap_caller_id_name->name_len = param->pbap_caller_id_name.name_len;
			} else {
				p_pbap_caller_id_name->name_len = 0;
			}
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_PBAP_GET_PHONE_BOOK_SIZE_CMPL: {
		rtk_bt_pbap_get_pb_size_cmpl *p_pbap_get_pb_size = NULL;
		BT_LOGA("app_pbap_bt_cback: Get phone book size \r\n");
		p_link = app_find_br_link(param->pbap_get_phone_book_size_cmpl.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_PBAP, RTK_BT_PBAP_EVT_GET_PHONE_BOOK_SIZE_CMPL, sizeof(rtk_bt_pbap_get_pb_size_cmpl));
			if (!p_evt) {
				BT_LOGE("app_pbap_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_pbap_get_pb_size = (rtk_bt_pbap_get_pb_size_cmpl *)p_evt->data;
			memcpy((void *)p_pbap_get_pb_size->bd_addr, (void *)param->pbap_get_phone_book_size_cmpl.bd_addr, 6);
			p_pbap_get_pb_size->pb_size = param->pbap_get_phone_book_size_cmpl.pb_size;
			p_pbap_get_pb_size->new_missed_calls = param->pbap_get_phone_book_size_cmpl.new_missed_calls;
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	default: {
		APP_PRINT_INFO1("app_pbap_bt_cback: default event_type 0x%04x", event_type);
		// BT_LOGE("app_pbap_bt_cback: default event_type 0x%04x \r\n", event_type);
		handle = false;
	}
	break;
	}

	if (handle == true) {
		APP_PRINT_INFO1("app_pbap_bt_cback: event_type 0x%04x", event_type);
	}
}

static uint16_t bt_stack_pbap_connect(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;
	T_BT_SDP_UUID_DATA uuid;

	if (RTK_BT_AUDIO_HFP_ROLE_HF == hfp_role) {
		uuid.uuid_16 = UUID_PBAP_PSE;
	} else {
		BT_LOGE("bt_stack_pbap_connect: only support phone book access profile client \r\n");
		return RTK_BT_FAIL;
	}

	if (bt_sdp_discov_start(bd_addr, BT_SDP_UUID16, uuid)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_pbap_disconnect(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;
	T_APP_BR_LINK *p_link;

	p_link = app_find_br_link(bd_addr);
	if (p_link != NULL) {
		if (bt_pbap_disconnect_req(bd_addr)) {
			return RTK_BT_OK;
		}
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_pbap_set_phone_book(void *param)
{
	rtk_bt_pbap_set_pb_t *p_set_pb_t = (rtk_bt_pbap_set_pb_t *)param;
	T_APP_BR_LINK *p_link;

	p_link = app_find_br_link(p_set_pb_t->bd_addr);
	if (p_link != NULL) {
		if (bt_pbap_phone_book_set(p_set_pb_t->bd_addr, (T_BT_PBAP_REPOSITORY)p_set_pb_t->repos, (T_BT_PBAP_PATH)p_set_pb_t->path)) {
			return RTK_BT_OK;
		}
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_pbap_phone_book_pull(void *param)
{
	rtk_bt_pbap_pb_pull_t *p_pb_pull_t = (rtk_bt_pbap_pb_pull_t *)param;
	T_APP_BR_LINK *p_link;

	p_link = app_find_br_link(p_pb_pull_t->bd_addr);
	if (p_link != NULL) {
		if (bt_pbap_phone_book_pull(p_pb_pull_t->bd_addr,
									(T_BT_PBAP_REPOSITORY)p_pb_pull_t->repos,
									(T_BT_PBAP_PHONE_BOOK)p_pb_pull_t->phone_book,
									p_pb_pull_t->start_offset,
									p_pb_pull_t->max_list_count,
									p_pb_pull_t->filter)) {
			return RTK_BT_OK;
		}
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_pbap_phone_book_pull_continue(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;
	T_APP_BR_LINK *p_link;

	p_link = app_find_br_link(bd_addr);
	if (p_link != NULL) {
		if (bt_pbap_pull_continue(bd_addr)) {
			return RTK_BT_OK;
		}
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_pbap_phone_book_size_get(void *param)
{
	rtk_bt_pbap_pb_size_get_t *p_pb_size_t = (rtk_bt_pbap_pb_size_get_t *)param;
	T_APP_BR_LINK *p_link;

	p_link = app_find_br_link(p_pb_size_t->bd_addr);
	if (p_link != NULL) {
		if (bt_pbap_phone_book_size_get(p_pb_size_t->bd_addr,
										(T_BT_PBAP_REPOSITORY)p_pb_size_t->repos,
										(T_BT_PBAP_PHONE_BOOK)p_pb_size_t->phone_book)) {
			return RTK_BT_OK;
		}
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_pbap_pull_caller_id_name(void *param)
{
	rtk_bt_pbap_pull_caller_id_name_t *p_pull_caller_id_name_t = (rtk_bt_pbap_pull_caller_id_name_t *)param;
	T_APP_BR_LINK *p_link;

	p_link = app_find_br_link(p_pull_caller_id_name_t->bd_addr);
	if (p_link != NULL) {
		if (bt_pbap_vcard_listing_by_number_pull(p_pull_caller_id_name_t->bd_addr, p_pull_caller_id_name_t->number)) {
			return RTK_BT_OK;
		}
	}

	return RTK_BT_FAIL;
}

uint16_t bt_stack_pbap_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = 0;
	BT_LOGD("bt_stack_pbap_act_handle: act = %d \r\n", p_cmd->act);
	switch (p_cmd->act) {

	case RTK_BT_PBAP_ACT_CONNECT:
		ret = bt_stack_pbap_connect(p_cmd->param);
		break;

	case RTK_BT_PBAP_ACT_DISCONNECT:
		ret = bt_stack_pbap_disconnect(p_cmd->param);
		break;

	case RTK_BT_PBAP_ACT_SET_PHONE_BOOK:
		ret = bt_stack_pbap_set_phone_book(p_cmd->param);
		break;

	case RTK_BT_PBAP_ACT_PHONE_BOOK_PULL:
		ret = bt_stack_pbap_phone_book_pull(p_cmd->param);
		break;

	case RTK_BT_PBAP_ACT_PHONE_BOOK_PULL_CONTINUE:
		ret = bt_stack_pbap_phone_book_pull_continue(p_cmd->param);
		break;

	case RTK_BT_PBAP_ACT_PHONE_BOOK_SIZE_GET:
		ret = bt_stack_pbap_phone_book_size_get(p_cmd->param);
		break;

	case RTK_BT_PBAP_ACT_PULL_CALLER_ID_NAME:
		ret = bt_stack_pbap_pull_caller_id_name(p_cmd->param);
		break;

	default:
		BT_LOGE("bt_stack_pbap_act_handle: unknown act: %d \r\n", p_cmd->act);
		ret = 0;
		break;
	}

	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);

	return ret;
}

uint16_t bt_stack_pbap_init(uint8_t role)
{
	BT_LOGA("[PBAP]app_pbap_init\n");
	bt_pbap_init(1);
	bt_mgr_cback_register(app_pbap_bt_cback);
	hfp_role = role;

	return RTK_BT_OK;
}

void bt_stack_pbap_deinit(void)
{
	BT_LOGA("[PBAP]app_pbap_deinit\n");
	bt_pbap_deinit();
}

#endif