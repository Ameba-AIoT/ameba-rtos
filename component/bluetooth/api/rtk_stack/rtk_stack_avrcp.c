/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <bt_api_config.h>
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <stdio.h>
#include <string.h>
#include <dlist.h>
#include <osif.h>
#include <rtk_bt_br_gap.h>
#include <rtk_bt_avrcp.h>
#include <gap_br.h>
#include <trace_app.h>
#include <remote.h>
#include <btm.h>
#include <bt_avrcp.h>
#include <bt_sdp.h>

static uint8_t volume_req = RTK_BT_DEFAULT_ABSOLUTE_VOLUME;
extern T_APP_BR_LINK *app_find_br_link(uint8_t *bd_addr);

static void app_avrcp_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
	(void)buf_len;
	T_BT_EVENT_PARAM *param = event_buf;
	T_APP_BR_LINK *p_link;
	bool handle = true;
	rtk_bt_evt_t *p_evt = NULL;

	switch (event_type) {

	case BT_EVENT_SDP_ATTR_INFO: {
		T_BT_SDP_ATTR_INFO *sdp_info = &param->sdp_attr_info.info;
		if ((sdp_info->srv_class_uuid_type == BT_SDP_UUID16) && (UUID_AV_REMOTE_CONTROL_TARGET == sdp_info->srv_class_uuid_data.uuid_16)) {
			uint8_t *p_attr;

			p_attr = bt_sdp_attr_find(sdp_info->p_attr,
									  sdp_info->attr_len,
									  SDP_ATTR_ADD_PROTO_DESC_LIST);
			if (p_attr) {
				uint8_t *p_attr_param;
				uint8_t *p_elem;
				uint8_t  loop;
				uint16_t uuid_1;
				uint16_t uuid_2;
				uint16_t l2c_psm;

				loop = 1;
				while ((p_elem = bt_sdp_elem_access(p_attr, sdp_info->attr_len, loop)) != NULL) {
					p_attr_param = bt_sdp_elem_access(p_elem, sdp_info->attr_len, 2);
					if (p_attr_param) {
						p_attr_param = bt_sdp_elem_access(p_elem, sdp_info->attr_len, 1);
						uuid_1 = p_attr_param[3] << 8 | p_attr_param[4];
						uuid_2 = p_attr_param[11] << 8 | p_attr_param[12];
						if (uuid_1 == UUID_L2CAP && uuid_2 == UUID_OBEX) {
							l2c_psm = p_attr_param[6] << 8 | p_attr_param[7];
							BT_LOGA("app_avrcp_bt_cback: parse l2c_psm 0x%x \r\n", l2c_psm);
							bt_avrcp_cover_art_connect_req(param->sdp_attr_info.bd_addr, l2c_psm);
							break;
						}
					}
					loop++;
				}
			} else {
				BT_LOGE("app_avrcp_bt_cback: no p_attr find \r\n");
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_CONN_IND: {
		rtk_bt_avrcp_conn_ind_t *p_avrcp_conn_ind = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_CONN_IND");
		BT_LOGA("app_avrcp_bt_cback: T_EVENT_AVRCP_CONN_IND \r\n");
		p_link = app_find_br_link(param->avrcp_conn_ind.bd_addr);
		if (p_link != NULL) {
			bt_avrcp_connect_cfm(p_link->bd_addr, true);
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_CONN_IND, sizeof(rtk_bt_avrcp_conn_ind_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_avrcp_conn_ind = (rtk_bt_avrcp_conn_ind_t *)p_evt->data;
			memcpy((void *)p_avrcp_conn_ind->bd_addr, (void *)p_link->bd_addr, 6);
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_CONN_CMPL: {
		rtk_bt_avrcp_conn_cmpl_t *p_avrcp_conn_cmpl_ind = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_CONN_CMPL");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_CONN_CMPL \r\n");
		p_link = app_find_br_link(param->avrcp_conn_cmpl.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_CONN_CMPL, sizeof(rtk_bt_avrcp_conn_cmpl_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_avrcp_conn_cmpl_ind = (rtk_bt_avrcp_conn_cmpl_t *)p_evt->data;
			memcpy((void *)p_avrcp_conn_cmpl_ind->bd_addr, (void *)p_link->bd_addr, 6);
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_DISCONN_CMPL: {
		rtk_bt_avrcp_disconn_cmpl_t *p_avrcp_disconn_cmpl_ind = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_DISCONN_CMPL");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_DISCONN_CMPL \r\n");
		p_link = app_find_br_link(param->avrcp_disconn_cmpl.bd_addr);
		if (p_link != NULL) {
			p_link->avrcp_play_status = BT_AVRCP_PLAY_STATUS_STOPPED;
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_DISCONN_CMPL, sizeof(rtk_bt_avrcp_disconn_cmpl_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_avrcp_disconn_cmpl_ind = (rtk_bt_avrcp_disconn_cmpl_t *)p_evt->data;
			memcpy((void *)p_avrcp_disconn_cmpl_ind->bd_addr, (void *)p_link->bd_addr, 6);
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_PLAY_STATUS_CHANGED_REG_REQ: {
		APP_PRINT_INFO0("BT_EVENT_AVRCP_PLAY_STATUS_CHANGED_REG_REQ");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_PLAY_STATUS_CHANGED_REG_REQ \r\n");
		p_link = app_find_br_link(param->avrcp_reg_play_status_changed.bd_addr);
		if (p_link != NULL) {
			if (p_link->is_streaming) {
				bt_avrcp_play_status_change_register_rsp(p_link->bd_addr, BT_AVRCP_PLAY_STATUS_PLAYING);
			} else {
				bt_avrcp_play_status_change_register_rsp(p_link->bd_addr, BT_AVRCP_PLAY_STATUS_PAUSED);
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_PLAY_STATUS_RSP:
	case BT_EVENT_AVRCP_PLAY_STATUS_CHANGED: {
		rtk_bt_avrcp_sub_event_t *p_avrcp_sub_event_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_PLAY_STATUS_CHANGED");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_PLAY_STATUS_CHANGED \r\n");
		p_link = app_find_br_link(param->avrcp_play_status_changed.bd_addr);
		if (p_link != NULL) {
			if (p_link->avrcp_play_status != param->avrcp_play_status_changed.play_status) {
				p_link->avrcp_play_status = param->avrcp_play_status_changed.play_status;
				p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_PLAY_STATUS_CHANGED_REG_REQ, sizeof(rtk_bt_avrcp_sub_event_t));
				if (!p_evt) {
					BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
					handle = false;
					break;
				}
				p_avrcp_sub_event_t = (rtk_bt_avrcp_sub_event_t *)p_evt->data;
				p_avrcp_sub_event_t->avrcp_play_status =  param->avrcp_play_status_changed.play_status;
				/* Send event */
				if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
					handle = false;
					break;
				}
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_PLAY: {
		rtk_bt_avrcp_digital_interface_command_t *p_passthrough_cmd_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_PLAY");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_PLAY \r\n");
		p_link = app_find_br_link(param->avrcp_volume_changed.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_PLAY_REQ_EVENT, sizeof(rtk_bt_avrcp_digital_interface_command_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_passthrough_cmd_t = (rtk_bt_avrcp_digital_interface_command_t *)p_evt->data;
			memcpy((void *)p_passthrough_cmd_t->bd_addr, (void *)param->avrcp_play.bd_addr, 6);
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_PAUSE: {
		rtk_bt_avrcp_digital_interface_command_t *p_passthrough_cmd_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_PAUSE");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_PAUSE \r\n");
		p_link = app_find_br_link(param->avrcp_volume_changed.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_PAUSE_REQ_EVENT, sizeof(rtk_bt_avrcp_digital_interface_command_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_passthrough_cmd_t = (rtk_bt_avrcp_digital_interface_command_t *)p_evt->data;
			memcpy((void *)p_passthrough_cmd_t->bd_addr, (void *)param->avrcp_pause.bd_addr, 6);
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_STOP: {
		rtk_bt_avrcp_digital_interface_command_t *p_passthrough_cmd_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_STOP");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_STOP \r\n");
		p_link = app_find_br_link(param->avrcp_volume_changed.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_STOP_REQ_EVENT, sizeof(rtk_bt_avrcp_digital_interface_command_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_passthrough_cmd_t = (rtk_bt_avrcp_digital_interface_command_t *)p_evt->data;
			memcpy((void *)p_passthrough_cmd_t->bd_addr, (void *)param->avrcp_stop.bd_addr, 6);
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_FORWARD: {
		rtk_bt_avrcp_digital_interface_command_t *p_passthrough_cmd_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_FORWARD");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_FORWARD \r\n");
		p_link = app_find_br_link(param->avrcp_volume_changed.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_FORWARD_REQ_EVENT, sizeof(rtk_bt_avrcp_digital_interface_command_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_passthrough_cmd_t = (rtk_bt_avrcp_digital_interface_command_t *)p_evt->data;
			memcpy((void *)p_passthrough_cmd_t->bd_addr, (void *)param->avrcp_forward.bd_addr, 6);
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_BACKWARD: {
		rtk_bt_avrcp_digital_interface_command_t *p_passthrough_cmd_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_BACKWARD");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_BACKWARD \r\n");
		p_link = app_find_br_link(param->avrcp_volume_changed.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_BACKWARD_REQ_EVENT, sizeof(rtk_bt_avrcp_digital_interface_command_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_passthrough_cmd_t = (rtk_bt_avrcp_digital_interface_command_t *)p_evt->data;
			memcpy((void *)p_passthrough_cmd_t->bd_addr, (void *)param->avrcp_backward.bd_addr, 6);
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_FAST_FORWARD_START: {
		rtk_bt_avrcp_digital_interface_command_t *p_passthrough_cmd_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_FAST_FORWARD_START");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_FAST_FORWARD_START \r\n");
		p_link = app_find_br_link(param->avrcp_volume_changed.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_FAST_FORWARD_START_REQ_EVENT, sizeof(rtk_bt_avrcp_digital_interface_command_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_passthrough_cmd_t = (rtk_bt_avrcp_digital_interface_command_t *)p_evt->data;
			memcpy((void *)p_passthrough_cmd_t->bd_addr, (void *)param->avrcp_fast_forward_start.bd_addr, 6);
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_FAST_FORWARD_STOP: {
		rtk_bt_avrcp_digital_interface_command_t *p_passthrough_cmd_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_FAST_FORWARD_STOP");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_FAST_FORWARD_STOP \r\n");
		p_link = app_find_br_link(param->avrcp_volume_changed.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_FAST_FORWARD_STOP_REQ_EVENT, sizeof(rtk_bt_avrcp_digital_interface_command_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_passthrough_cmd_t = (rtk_bt_avrcp_digital_interface_command_t *)p_evt->data;
			memcpy((void *)p_passthrough_cmd_t->bd_addr, (void *)param->avrcp_fast_forward_stop.bd_addr, 6);
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_REWIND_START: {
		rtk_bt_avrcp_digital_interface_command_t *p_passthrough_cmd_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_REWIND_START");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_REWIND_START \r\n");
		p_link = app_find_br_link(param->avrcp_volume_changed.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_REWIND_START_REQ_EVENT, sizeof(rtk_bt_avrcp_digital_interface_command_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_passthrough_cmd_t = (rtk_bt_avrcp_digital_interface_command_t *)p_evt->data;
			memcpy((void *)p_passthrough_cmd_t->bd_addr, (void *)param->avrcp_rewind_start.bd_addr, 6);
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_REWIND_STOP: {
		rtk_bt_avrcp_digital_interface_command_t *p_passthrough_cmd_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_REWIND_STOP");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_REWIND_STOP \r\n");
		p_link = app_find_br_link(param->avrcp_volume_changed.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_REWIND_STOP_REQ_EVENT, sizeof(rtk_bt_avrcp_digital_interface_command_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_passthrough_cmd_t = (rtk_bt_avrcp_digital_interface_command_t *)p_evt->data;
			memcpy((void *)p_passthrough_cmd_t->bd_addr, (void *)param->avrcp_rewind_stop.bd_addr, 6);
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_ABSOLUTE_VOLUME_SET: {
		rtk_bt_avrcp_absolute_volume_set_t *p_avrcp_absolute_volume_set_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_ABSOLUTE_VOLUME_SET");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_ABSOLUTE_VOLUME_SET \r\n");
		p_link = app_find_br_link(param->avrcp_volume_changed.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_ABSOLUTE_VOLUME_SET, sizeof(rtk_bt_avrcp_absolute_volume_set_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_avrcp_absolute_volume_set_t = (rtk_bt_avrcp_absolute_volume_set_t *)p_evt->data;
			memcpy((void *)p_avrcp_absolute_volume_set_t->bd_addr, (void *)param->avrcp_absolute_volume_set.bd_addr, 6);
			p_avrcp_absolute_volume_set_t->volume = param->avrcp_absolute_volume_set.volume;
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_VOLUME_CHANGED: {
		rtk_bt_avrcp_volume_t *p_avrcp_volume_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_VOLUME_CHANGED");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_VOLUME_CHANGED \r\n");
		p_link = app_find_br_link(param->avrcp_volume_changed.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_VOLUME_CHANGED, sizeof(rtk_bt_avrcp_volume_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_avrcp_volume_t = (rtk_bt_avrcp_volume_t *)p_evt->data;
			p_avrcp_volume_t->volume = param->avrcp_volume_changed.volume;
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_REG_VOLUME_CHANGED: {
		rtk_bt_avrcp_volume_change_req_t *p_volume_t = NULL;
		uint8_t cb_ret = 0;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_REG_VOLUME_CHANGED");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_REG_VOLUME_CHANGED \r\n");
		p_link = app_find_br_link(param->avrcp_reg_volume_changed.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_REG_VOLUME_CHANGED, sizeof(rtk_bt_avrcp_volume_change_req_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_volume_t = (rtk_bt_avrcp_volume_change_req_t *)p_evt->data;
			memcpy((void *)p_volume_t->bd_addr, (void *)param->avrcp_reg_volume_changed.bd_addr, 6);
			p_volume_t->volume = volume_req;
			/* Send event direct calling */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, &cb_ret)) {
				handle = false;
				break;
			}
			bt_avrcp_volume_change_register_rsp(p_link->bd_addr, volume_req);
		}
	}
	break;

	case BT_EVENT_AVRCP_ELEM_ATTR: {
		rtk_bt_avrcp_element_attr_info_t *p_attr_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_ELEM_ATTR");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_ELEM_ATTR \r\n");
		p_link = app_find_br_link(param->avrcp_elem_attr.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_ELEMENT_ATTR_INFO, sizeof(rtk_bt_avrcp_element_attr_info_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_attr_t = (rtk_bt_avrcp_element_attr_info_t *)p_evt->data;
			memcpy((void *)p_attr_t->bd_addr, (void *)param->avrcp_elem_attr.bd_addr, 6);
			p_attr_t->state = param->avrcp_elem_attr.state;
			p_attr_t->num_of_attr = param->avrcp_elem_attr.num_of_attr;
			if (p_attr_t->num_of_attr) {
				p_attr_t->attr = (rtk_bt_avrcp_element_attr *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_attr_t->num_of_attr * sizeof(rtk_bt_avrcp_element_attr));
				if (NULL == p_attr_t->attr) {
					BT_LOGE("app_avrcp_bt_cback: rtk_bt_avrcp_element_attr allocate fail \r\n");
					rtk_bt_event_free(p_evt);
					break;
				}
			}
			if (param->avrcp_elem_attr.state == 0) {
				for (uint8_t i = 0; i < param->avrcp_elem_attr.num_of_attr; i++) {
					p_attr_t->attr[i].attribute_id = param->avrcp_elem_attr.attr[i].attribute_id;
					p_attr_t->attr[i].character_set_id = param->avrcp_elem_attr.attr[i].character_set_id;
					p_attr_t->attr[i].length = param->avrcp_elem_attr.attr[i].length;
					if (p_attr_t->attr[i].length) {
						p_attr_t->attr[i].p_buf = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_attr_t->attr[i].length);
						if (NULL == p_attr_t->attr[i].p_buf) {
							BT_LOGE("app_avrcp_bt_cback: p_buf allocate fail \r\n");
							p_attr_t->attr[i].length = 0;
						} else {
							memcpy((void *)p_attr_t->attr[i].p_buf, param->avrcp_elem_attr.attr[i].p_buf, p_attr_t->attr[i].length);
						}
					}
					if (p_attr_t->attr[i].attribute_id == BT_AVRCP_ELEM_ATTR_DEFAULT_COVER_ART) {
						uint8_t image_handle[16] = {0};

						for (uint8_t j = 0; j < param->avrcp_elem_attr.attr[i].length; j++) {
							image_handle[2 * j + 1] = param->avrcp_elem_attr.attr[i].p_buf[j];
						}
						bt_avrcp_cover_art_get(p_link->bd_addr, image_handle);
					}
				}
			}
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_APP_SETTING_ATTRS_LIST_RSP: {
		rtk_bt_avrcp_app_setting_attrs_list_t *p_list_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_APP_SETTING_ATTRS_LIST_RSP");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_APP_SETTING_ATTRS_LIST_RSP \r\n");
		p_link = app_find_br_link(param->avrcp_app_setting_attrs_list_rsp.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_APP_SETTING_ATTRS_LIST_RSP, sizeof(rtk_bt_avrcp_app_setting_attrs_list_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_list_t = (rtk_bt_avrcp_app_setting_attrs_list_t *)p_evt->data;
			memcpy((void *)p_list_t->bd_addr, (void *)param->avrcp_app_setting_attrs_list_rsp.bd_addr, 6);
			p_list_t->state = param->avrcp_app_setting_attrs_list_rsp.state;
			p_list_t->num_of_attr = param->avrcp_app_setting_attrs_list_rsp.num_of_attr;
			if (p_list_t->num_of_attr) {
				p_list_t->p_attr_id = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_list_t->num_of_attr * sizeof(uint8_t));
				if (NULL == p_list_t->p_attr_id) {
					BT_LOGE("app_avrcp_bt_cback: app setting attrs allocate fail \r\n");
					rtk_bt_event_free(p_evt);
					break;
				}
			}
			if (param->avrcp_app_setting_attrs_list_rsp.state == 0) {
				for (uint8_t i = 0; i < param->avrcp_app_setting_attrs_list_rsp.num_of_attr; i++) {
					p_list_t->p_attr_id[i] = param->avrcp_app_setting_attrs_list_rsp.p_attr_id[i];
				}
			}
			p_evt->user_data = p_list_t->p_attr_id;
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_APP_SETTING_VALUES_LIST_RSP: {
		rtk_bt_avrcp_app_setting_values_list_t *p_values_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_APP_SETTING_VALUES_LIST_RSP");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_APP_SETTING_VALUES_LIST_RSP \r\n");
		p_link = app_find_br_link(param->avrcp_app_setting_values_list_rsp.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_APP_SETTING_VALUES_LIST_RSP, sizeof(rtk_bt_avrcp_app_setting_values_list_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_values_t = (rtk_bt_avrcp_app_setting_values_list_t *)p_evt->data;
			memcpy((void *)p_values_t->bd_addr, (void *)param->avrcp_app_setting_values_list_rsp.bd_addr, 6);
			p_values_t->state = param->avrcp_app_setting_values_list_rsp.state;
			p_values_t->num_of_value = param->avrcp_app_setting_values_list_rsp.num_of_value;
			if (p_values_t->num_of_value) {
				p_values_t->p_value = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_values_t->num_of_value * sizeof(uint8_t));
				if (NULL == p_values_t->p_value) {
					BT_LOGE("app_avrcp_bt_cback: app setting values allocate fail \r\n");
					rtk_bt_event_free(p_evt);
					break;
				}
			}
			if (param->avrcp_app_setting_values_list_rsp.state == 0) {
				for (uint8_t i = 0; i < param->avrcp_app_setting_values_list_rsp.num_of_value; i++) {
					p_values_t->p_value[i] = param->avrcp_app_setting_values_list_rsp.p_value[i];
				}
			}
			p_evt->user_data = p_values_t->p_value;
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_APP_SETTING_GET_RSP: {
		rtk_bt_avrcp_app_setting_get_rsp_t *p_rsp_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_APP_SETTING_GET_RSP");
		BT_LOGA("app_avrcp_bt_cback: BT_EVENT_AVRCP_APP_SETTING_GET_RSP \r\n");
		p_link = app_find_br_link(param->avrcp_app_setting_get_rsp.bd_addr);
		if (p_link != NULL) {
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_APP_SETTING_GET_RSP, sizeof(rtk_bt_avrcp_app_setting_get_rsp_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_rsp_t = (rtk_bt_avrcp_app_setting_get_rsp_t *)p_evt->data;
			memcpy((void *)p_rsp_t->bd_addr, (void *)param->avrcp_app_setting_get_rsp.bd_addr, 6);
			p_rsp_t->state = param->avrcp_app_setting_get_rsp.state;
			p_rsp_t->num_of_attr = param->avrcp_app_setting_get_rsp.num_of_attr;
			if (p_rsp_t->num_of_attr) {
				p_rsp_t->p_app_setting = (rtk_bt_avrcp_app_setting_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_rsp_t->num_of_attr * sizeof(rtk_bt_avrcp_app_setting_t));
				if (NULL == p_rsp_t->p_app_setting) {
					BT_LOGE("app_avrcp_bt_cback: app setting allocate fail \r\n");
					rtk_bt_event_free(p_evt);
					break;
				}
			}
			if (param->avrcp_app_setting_get_rsp.state == 0) {
				for (uint8_t i = 0; i < param->avrcp_app_setting_get_rsp.num_of_attr; i++) {
					memcpy((void *)&p_rsp_t->p_app_setting[i], (void *)&param->avrcp_app_setting_get_rsp.p_app_setting[i], sizeof(T_BT_AVRCP_APP_SETTING));
				}
			}
			p_evt->user_data = p_rsp_t->p_app_setting;
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	case BT_EVENT_AVRCP_COVER_ART_DATA_IND: {
		rtk_bt_avrcp_cover_art_data_ind_t *p_data_t = NULL;

		APP_PRINT_INFO0("BT_EVENT_AVRCP_COVER_ART_DATA_IND");
		BT_LOGD("app_avrcp_bt_cback: BT_EVENT_AVRCP_COVER_ART_DATA_IND \r\n");
		p_link = app_find_br_link(param->avrcp_cover_art_data_ind.bd_addr);
		if (p_link != NULL) {
			if (!param->avrcp_cover_art_data_ind.data_end) {
				bt_avrcp_cover_art_get(p_link->bd_addr, NULL);
			}
			p_evt = rtk_bt_event_create(RTK_BT_BR_GP_AVRCP, RTK_BT_AVRCP_EVT_COVER_ART_DATA_IND, sizeof(rtk_bt_avrcp_cover_art_data_ind_t));
			if (!p_evt) {
				BT_LOGE("app_avrcp_bt_cback: evt_t allocate fail \r\n");
				handle = false;
				break;
			}
			p_data_t = (rtk_bt_avrcp_cover_art_data_ind_t *)p_evt->data;
			memcpy((void *)p_data_t->bd_addr, (void *)param->avrcp_cover_art_data_ind.bd_addr, 6);
			p_data_t->data_end = param->avrcp_cover_art_data_ind.data_end;
			p_data_t->data_len = param->avrcp_cover_art_data_ind.data_len;
			p_data_t->p_data = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_data_t->data_len);
			if (NULL == p_data_t->p_data) {
				BT_LOGE("app_avrcp_bt_cback: p_data allocate fail \r\n");
				rtk_bt_event_free(p_evt);
				break;
			} else {
				memcpy((void *)p_data_t->p_data, (void *)param->avrcp_cover_art_data_ind.p_data, p_data_t->data_len);
			}
			p_evt->user_data = p_data_t->p_data;
			/* Send event */
			if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
				handle = false;
				break;
			}
		}
	}
	break;

	default: {
		handle = false;
	}
	break;

	}
	if (handle == true) {
		// APP_PRINT_INFO1("app_avrcp_bt_cback: event_type 0x%04x", event_type);
		// BT_LOGE("app_avrcp_bt_cback: event_type 0x%04x \r\n", event_type);
	}
}

static uint16_t bt_stack_avrcp_connect(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;

	if (bt_avrcp_connect_req(bd_addr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_disconnect(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;

	if (bt_avrcp_disconnect_req(bd_addr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_play(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;

	if (bt_avrcp_play(bd_addr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_pause(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;

	if (bt_avrcp_pause(bd_addr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_stop(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;

	if (bt_avrcp_stop(bd_addr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_forward(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;

	if (bt_avrcp_forward(bd_addr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_backward(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;

	if (bt_avrcp_backward(bd_addr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_rewind_start(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;

	if (bt_avrcp_rewind_start(bd_addr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_rewind_stop(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;

	if (bt_avrcp_rewind_stop(bd_addr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_fast_forward_start(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;

	if (bt_avrcp_fast_forward_start(bd_addr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_fast_forward_stop(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;

	if (bt_avrcp_fast_forward_stop(bd_addr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_absolute_volume_set(void *param)
{
	rtk_bt_avrcp_absolute_volume_set_t *p_volume_t = (rtk_bt_avrcp_absolute_volume_set_t *)param;

	if (bt_avrcp_absolute_volume_set(p_volume_t->bd_addr, p_volume_t->volume)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_volume_change_req(void *param)
{
	rtk_bt_avrcp_volume_change_req_t *p_volume_t = (rtk_bt_avrcp_volume_change_req_t *)param;

	volume_req = p_volume_t->volume;
	if (bt_avrcp_volume_change_req(p_volume_t->bd_addr, p_volume_t->volume)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_get_element_attr_req(void *param)
{
	rtk_bt_avrcp_get_element_attr_req_t *req_t = (rtk_bt_avrcp_get_element_attr_req_t *)param;

	if (bt_avrcp_get_element_attr_req(req_t->bd_addr, req_t->num, req_t->p_attr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_cover_art_connect(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;
	T_BT_SDP_UUID_DATA uuid;

	uuid.uuid_16 = UUID_AV_REMOTE_CONTROL_TARGET;

	if (bt_sdp_discov_start(bd_addr, BT_SDP_UUID16, uuid)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_app_setting_attrs_list(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;

	if (bt_avrcp_app_setting_attrs_list(bd_addr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_app_setting_values_list(void *param)
{
	rtk_bt_avrcp_app_setting_values_t *p_values = (rtk_bt_avrcp_app_setting_values_t *)param;

	if (bt_avrcp_app_setting_values_list(p_values->bd_addr, p_values->attr_id)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_app_setting_value_get(void *param)
{
	rtk_bt_avrcp_app_setting_value_get_t *p_get = (rtk_bt_avrcp_app_setting_value_get_t *)param;

	if (bt_avrcp_app_setting_value_get(p_get->bd_addr, p_get->attr_num, p_get->attr_list)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_avrcp_app_setting_value_set(void *param)
{
	rtk_bt_avrcp_app_setting_value_set_t *p_set = (rtk_bt_avrcp_app_setting_value_set_t *)param;
	uint8_t attr[2] = {0};

	attr[0] = p_set->attr_id;
	attr[1] = p_set->setting;

	if (bt_avrcp_app_setting_value_set(p_set->bd_addr, 1, attr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

uint16_t bt_stack_avrcp_act_handle(rtk_bt_cmd_t *p_cmd)
{
	int16_t ret = 0;
	APP_PRINT_INFO1("bt_stack_avrcp_act_handle: act = %d \r\n", p_cmd->act);
	switch (p_cmd->act) {

	case RTK_BT_AVRCP_ACT_CONNECT:
		ret = bt_stack_avrcp_connect(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_DISCONNECT:
		ret = bt_stack_avrcp_disconnect(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_PLAY:
		ret = bt_stack_avrcp_play(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_PAUSE:
		ret = bt_stack_avrcp_pause(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_STOP:
		ret = bt_stack_avrcp_stop(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_FORWARD:
		ret = bt_stack_avrcp_forward(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_BACKWARD:
		ret = bt_stack_avrcp_backward(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_REWIND_START:
		ret = bt_stack_avrcp_rewind_start(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_REWIND_STOP:
		ret = bt_stack_avrcp_rewind_stop(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_FAST_FORWARD_START:
		ret = bt_stack_avrcp_fast_forward_start(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_FAST_FORWARD_STOP:
		ret = bt_stack_avrcp_fast_forward_stop(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_ABSOLUTE_VOLUME_SET:
		ret = bt_stack_avrcp_absolute_volume_set(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_VOLUME_CHANGE_REQ:
		ret = bt_stack_avrcp_volume_change_req(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_GET_ELEMENT_ATTR:
		ret = bt_stack_avrcp_get_element_attr_req(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_CONN_COVER_ART:
		ret = bt_stack_avrcp_cover_art_connect(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_APP_SETTING_ATTRS_LIST:
		ret = bt_stack_avrcp_app_setting_attrs_list(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_APP_SETTING_VALUES_LIST:
		ret = bt_stack_avrcp_app_setting_values_list(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_APP_SETTING_VALUE_GET:
		ret = bt_stack_avrcp_app_setting_value_get(p_cmd->param);
		break;

	case RTK_BT_AVRCP_ACT_APP_SETTING_VALUE_SET:
		ret = bt_stack_avrcp_app_setting_value_set(p_cmd->param);
		break;

	default:
		BT_LOGE("bt_stack_avrcp_act_handle: unknown act: %d \r\n", p_cmd->act);
		ret = 0;
		break;
	}

	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);

	return ret;
}

uint16_t bt_stack_avrcp_init(uint8_t role)
{
	BT_LOGA("[AVRCP]app_avrcp_init\n");

	bt_avrcp_init(1);
	if (role == 0x01) {
		/* A2DP SNK */
		bt_avrcp_supported_features_set(BT_AVRCP_FEATURE_CATEGORY_1, BT_AVRCP_FEATURE_CATEGORY_2);
	} else {
		/* A2DP SOURCE */
		bt_avrcp_supported_features_set(BT_AVRCP_FEATURE_CATEGORY_1 | BT_AVRCP_FEATURE_CATEGORY_2, BT_AVRCP_FEATURE_CATEGORY_1);
	}
	bt_mgr_cback_register(app_avrcp_bt_cback);

	return RTK_BT_OK;
}

void bt_stack_avrcp_deinit(void)
{
	BT_LOGA("[AVRCP]app_avrcp_deinit\n");

	bt_avrcp_deinit();
}

#endif