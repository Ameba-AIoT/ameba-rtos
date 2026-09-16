/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <string.h>

#include <rtk_bt_common.h>
#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/mesh.h>
#include <zephyr/net/buf.h>
#include <access.h>
#include <net.h>
#if defined(CONFIG_BT_MESH_LOW_POWER) && CONFIG_BT_MESH_LOW_POWER
#include <lpn.h>
#endif
#include <prov.h>
#include <subnet.h>
#include <bt_app_config.h>
#include <math.h>
#include <rtk_bt_def.h>
#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_common.h>
#include <rtk_bt_mesh_remote_prov_model.h>
#include <msg.h>
#include <zephyr/bluetooth/mesh/rpr_cli.h>
#include <zephyr/bluetooth/mesh/rpr.h>
#include <zephyr_stack_mesh_internal.h>

#if defined(CONFIG_BT_MESH_RPR_CLI) && CONFIG_BT_MESH_RPR_CLI
static void rpr_scan_report(struct bt_mesh_rpr_cli *cli,
							const struct bt_mesh_rpr_node *srv,
							struct bt_mesh_rpr_unprov *unprov,
							struct net_buf_simple *adv_data)
{
	(void)cli;
	(void)adv_data;
	uint16_t oob = unprov->oob;
	uint32_t uri_hash = unprov->hash;
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_mesh_rmt_prov_client_scan_report_t *scan_report = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_REPORT,
								sizeof(rtk_bt_mesh_rmt_prov_client_scan_report_t));
	scan_report = (rtk_bt_mesh_rmt_prov_client_scan_report_t *)p_evt->data;
	scan_report->src = srv->addr;
	scan_report->rssi = unprov->rssi;
	scan_report->oob = oob;
	scan_report->uri_hash = uri_hash;
	memcpy(scan_report->uuid, unprov->uuid, 16);
	rtk_bt_evt_indicate(p_evt, NULL);
}

static void rpr_link_status(struct bt_mesh_rpr_cli *cli,
							const struct bt_mesh_rpr_node *srv,
							struct bt_mesh_rpr_link *link)
{
	(void)cli;
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_mesh_rmt_prov_client_link_status_t *link_status = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_LINK_STATUS,
								sizeof(rtk_bt_mesh_rmt_prov_client_link_status_t));
	link_status = (rtk_bt_mesh_rmt_prov_client_link_status_t *)p_evt->data;
	link_status->src = srv->addr;
	memcpy(&link_status->prov_status, &link->status, 1);
	memcpy(&link_status->link_status, &link->state, 1);
	rtk_bt_evt_indicate(p_evt, NULL);
}

static void rpr_link_report(struct bt_mesh_rpr_cli *cli,
							const struct bt_mesh_rpr_node *srv,
							struct bt_mesh_rpr_link *link,
							uint8_t reason)
{
	(void)cli;
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_mesh_rmt_prov_client_link_report_t *link_report;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_LINK_REPORT,
								sizeof(rtk_bt_mesh_rmt_prov_client_link_report_t));
	link_report = (rtk_bt_mesh_rmt_prov_client_link_report_t *)p_evt->data;
	link_report->src = srv->addr;
	memcpy(&link_report->prov_status, &link->status, 1);
	memcpy(&link_report->link_status, &link->state, 1);
	link_report->close_reason = reason;

	rtk_bt_evt_indicate(p_evt, NULL);
}

struct bt_mesh_rpr_cli rpr_cli = {
	.scan_report = rpr_scan_report,
	.link_status = rpr_link_status,
	.link_report = rpr_link_report,
};

static uint16_t rtk_bt_mesh_remote_prov_client_scan(rtk_bt_mesh_remote_prov_client_scan_start_t *rmt_scan)
{
	const struct bt_mesh_rpr_node srv = {
		.addr = rmt_scan->dst,
		.net_idx = rmt_scan->net_key_index,
		.ttl = BT_MESH_TTL_DEFAULT,
	};

	uint8_t uuid_flag = 0;
	for (int i = 0; i < 16; i++) {
		if (rmt_scan->uuid[i] != 0) {
			uuid_flag = 1;
			break;
		}
	}
	struct bt_mesh_rpr_scan_status rsp;

	uint16_t ret = 0;
	if (uuid_flag) {
		ret = bt_mesh_rpr_scan_start(&rpr_cli, &srv, rmt_scan->uuid, rmt_scan->scan_timeout, rmt_scan->scanned_items_limit, &rsp);
	} else {
		ret = bt_mesh_rpr_scan_start(&rpr_cli, &srv, NULL, rmt_scan->scan_timeout, rmt_scan->scanned_items_limit, &rsp);
	}

	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_mesh_rmt_prov_client_scan_status_t *scan_status = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_STATUS,
								sizeof(rtk_bt_mesh_rmt_prov_client_scan_status_t));
	scan_status = (rtk_bt_mesh_rmt_prov_client_scan_status_t *)p_evt->data;
	scan_status->src = rmt_scan->dst;
	memcpy(&scan_status->prov_status, &rsp.status, 1);
	memcpy(&scan_status->scan_status, &rsp.scan, 1);
	scan_status->scanned_items_limit = rsp.max_devs;
	scan_status->timeout = rsp.timeout;
	rtk_bt_evt_indicate(p_evt, NULL);

	return ret;
}

static uint16_t rtk_bt_mesh_remote_prov_client_scan_caps_get(rtk_bt_mesh_remote_prov_client_scan_capa_get_t *rmt_capa)
{
	const struct bt_mesh_rpr_node srv = {
		.addr = rmt_capa->dst,
		.net_idx = rmt_capa->net_key_index,
		.ttl = BT_MESH_TTL_DEFAULT,
	};

	struct bt_mesh_rpr_caps rsp;
	uint16_t ret = 0;
	ret = bt_mesh_rpr_scan_caps_get(&rpr_cli, &srv, &rsp);
	rtk_bt_mesh_rmt_prov_client_scan_capa_status_t *scan_capa_status;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_CAPA_STATUS,
								sizeof(rtk_bt_mesh_rmt_prov_client_scan_capa_status_t));
	scan_capa_status = (rtk_bt_mesh_rmt_prov_client_scan_capa_status_t *)p_evt->data;
	scan_capa_status->src = rmt_capa->dst;
	scan_capa_status->max_scan_items = rsp.max_devs;
	scan_capa_status->support_active_scan = rsp.active_scan;
	rtk_bt_evt_indicate(p_evt, NULL);
	return ret;
}

rtk_bt_mesh_remote_prov_client_link_open_t rpr_link_open_info;

extern rtk_bt_mesh_provisioning_method_t pro_provisioning_method;
static uint16_t rtk_bt_mesh_remote_prov_client_link_open_prov(rtk_bt_mesh_remote_prov_client_link_open_t *rmt_link)
{
	pro_provisioning_method = RTK_BT_MESH_PROVISIONING_REMOTE_PROV_METHOD;
	rpr_link_open_info = *rmt_link;
	return 0;
}

rtk_bt_mesh_remote_prov_client_link_open_dkri_t rpr_dkri_info = {0, 0, 0xff};
static uint16_t rtk_bt_mesh_remote_prov_client_link_open_dkri(rtk_bt_mesh_remote_prov_client_link_open_dkri_t *rmt_dkri)
{
	rpr_dkri_info = *rmt_dkri;
	return 0;
}

static uint16_t rtk_bt_mesh_remote_prov_client_refresh_dev_key(uint8_t *attn_dur)
{
	(void)attn_dur;
	if (rpr_dkri_info.dkri_procedure == RTK_BT_MESH_RMT_PROV_DKRI_DEV_KEY_REFRESH) {
		struct bt_mesh_rpr_node srv = {
			.addr = rpr_dkri_info.dst,
			.net_idx = rpr_dkri_info.net_key_index,
			.ttl = BT_MESH_TTL_DEFAULT,
		};
		return bt_mesh_pb_remote_open_node(&rpr_cli, &srv, srv.addr, 0);
	}
	return 0;
}

static uint16_t rtk_bt_mesh_remote_prov_client_refresh_node_addr(rtk_bt_mesh_remote_prov_client_node_addr_refresh_t *addr_refresh)
{
	if (rpr_dkri_info.dkri_procedure == RTK_BT_MESH_RMT_PROV_DKRI_NODE_ADDR_REFRESH) {
		struct bt_mesh_rpr_node srv = {
			.addr = rpr_dkri_info.dst,
			.net_idx = rpr_dkri_info.net_key_index,
			.ttl = BT_MESH_TTL_DEFAULT,
		};
		return bt_mesh_pb_remote_open_node(&rpr_cli, &srv, addr_refresh->node_addr, 0);
	}
	return 0;
}

static uint16_t rtk_bt_mesh_remote_prov_client_refresh_compo_data(uint8_t *attn_dur)
{
	(void)attn_dur;
	if (rpr_dkri_info.dkri_procedure == RTK_BT_MESH_RMT_PROV_DKRI_NODE_COMPO_REFRESH) {
		struct bt_mesh_rpr_node srv = {
			.addr = rpr_dkri_info.dst,
			.net_idx = rpr_dkri_info.net_key_index,
			.ttl = BT_MESH_TTL_DEFAULT,
		};
		return bt_mesh_pb_remote_open_node(&rpr_cli, &srv, srv.addr, 1);
	}
	return 0;
}

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_remote_prov_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	if (!p_cmd) {
		BT_LOGE("[%s] param is NULL!\r\n", __func__);
		return RTK_BT_FAIL;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_SCAN_START:
		ret = rtk_bt_mesh_remote_prov_client_scan(p_cmd->param);
		break;
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_SCAN_CAPA_GET: {
		ret = rtk_bt_mesh_remote_prov_client_scan_caps_get(p_cmd->param);
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_LINK_OPEN: {
		ret = rtk_bt_mesh_remote_prov_client_link_open_prov(p_cmd->param);
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_LINK_OPEN_DKRI: {
		ret = rtk_bt_mesh_remote_prov_client_link_open_dkri(p_cmd->param);
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_DEVICE_KEY_REFRESH: {
		ret = rtk_bt_mesh_remote_prov_client_refresh_dev_key(p_cmd->param);
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_NODE_ADDR_REFRESH: {
		ret = rtk_bt_mesh_remote_prov_client_refresh_node_addr(p_cmd->param);
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_ACT_COMPO_DATA_REFRESH: {
		ret = rtk_bt_mesh_remote_prov_client_refresh_compo_data(p_cmd->param);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}

#endif
#endif