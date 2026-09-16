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
#include <rtk_bt_mesh_private_beacon_model.h>
#include <msg.h>
#include <zephyr/bluetooth/mesh/priv_beacon_cli.h>
#include <zephyr_stack_mesh_internal.h>

#if defined(CONFIG_BT_MESH_PRIV_BEACON_CLI) && CONFIG_BT_MESH_PRIV_BEACON_CLI
static void prb_priv_beacon_status(struct bt_mesh_priv_beacon_cli *cli, uint16_t addr,
								   struct bt_mesh_priv_beacon *priv_beacon)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_private_beacon_status_t *status;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL, RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_STATUS,
								sizeof(rtk_bt_mesh_private_beacon_status_t));
	status = (rtk_bt_mesh_private_beacon_status_t *)p_evt->data;
	status->private_beacon = priv_beacon->enabled;
	status->random_update_interval_steps = priv_beacon->rand_interval;
	rtk_bt_evt_indicate(p_evt, NULL);
}

static void prb_priv_gatt_proxy_status(struct bt_mesh_priv_beacon_cli *cli, uint16_t addr, uint8_t gatt_proxy)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_private_gatt_proxy_status_t *status;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL, RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_GATT_PROXY_STATUS,
								sizeof(rtk_bt_mesh_private_gatt_proxy_status_t));
	status = (rtk_bt_mesh_private_gatt_proxy_status_t *)p_evt->data;
	status->private_gatt_proxy = gatt_proxy;
	rtk_bt_evt_indicate(p_evt, NULL);
}

static void prb_priv_node_id_status(struct bt_mesh_priv_beacon_cli *cli, uint16_t addr, struct bt_mesh_priv_node_id *priv_node_id)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_private_node_identity_status_t *status;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL, RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_NODE_IDENTITY_STATUS,
								sizeof(rtk_bt_mesh_private_node_identity_status_t));
	status = (rtk_bt_mesh_private_node_identity_status_t *)p_evt->data;
	status->status = priv_node_id->status;
	status->net_key_index = priv_node_id->net_idx;
	status->private_identity = priv_node_id->state;
	rtk_bt_evt_indicate(p_evt, NULL);
}

const struct bt_mesh_priv_beacon_cli_cb prb_cb = {
	.priv_beacon_status = prb_priv_beacon_status,
	.priv_gatt_proxy_status = prb_priv_gatt_proxy_status,
	.priv_node_id_status = prb_priv_node_id_status,
};

struct bt_mesh_priv_beacon_cli prb_cli = {
	.cb = &prb_cb,
};

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_private_beacon_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GET: {
		rtk_bt_mesh_private_beacon_get_t *param = (rtk_bt_mesh_private_beacon_get_t *)p_cmd->param;
		ret = (uint16_t)bt_mesh_priv_beacon_cli_get(param->net_key_index, param->dst, NULL);
		break;
	}
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_SET: {
		rtk_bt_mesh_private_beacon_set_t *param = (rtk_bt_mesh_private_beacon_set_t *)p_cmd->param;
		struct bt_mesh_priv_beacon val;
		val.enabled = param->private_beacon;
		if (param->has_random_update) {
			val.rand_interval = param->random_update_interval_steps;
		} else {
			val.rand_interval = 0;
		}
		ret = (uint16_t)bt_mesh_priv_beacon_cli_set(param->net_key_index, param->dst, &val, NULL);
		break;
	}
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GATT_PROXY_GET: {
		rtk_bt_mesh_private_gatt_proxy_get_t *param = (rtk_bt_mesh_private_gatt_proxy_get_t *)p_cmd->param;
		ret = (uint16_t)bt_mesh_priv_beacon_cli_gatt_proxy_get(param->net_key_index, param->dst, NULL);
		break;
	}
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GATT_PROXY_SET: {
		rtk_bt_mesh_private_gatt_proxy_set_t *param = (rtk_bt_mesh_private_gatt_proxy_set_t *)p_cmd->param;
		ret = (uint16_t)bt_mesh_priv_beacon_cli_gatt_proxy_set(param->net_key_index, param->dst, param->private_gatt_proxy, NULL);
		break;
	}
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_NODE_IDENTITY_GET: {
		rtk_bt_mesh_private_node_identity_get_t *param = (rtk_bt_mesh_private_node_identity_get_t *)p_cmd->param;
		ret = (uint16_t)bt_mesh_priv_beacon_cli_node_id_get(param->net_key_index, param->dst, param->sub_net_key_index, NULL);
		break;
	}
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_NODE_IDENTITY_SET: {
		rtk_bt_mesh_private_node_identity_set_t *param = (rtk_bt_mesh_private_node_identity_set_t *)p_cmd->param;
		struct bt_mesh_priv_node_id val;
		val.net_idx = param->sub_net_key_index;
		val.state = param->private_identity;
		ret = (uint16_t)bt_mesh_priv_beacon_cli_node_id_set(param->net_key_index, param->dst, &val, NULL);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown p_cmd->act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}


#endif
#endif