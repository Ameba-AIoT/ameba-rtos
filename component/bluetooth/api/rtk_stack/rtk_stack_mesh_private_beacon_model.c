#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_private_beacon_model.h>
#include <private_beacon_model.h>

#if defined(BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL) && BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL
uint8_t rtk_bt_mesh_private_beacon_client_model_indicate_event(uint32_t event_code, void *pdata, uint32_t len)
{
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL, event_code, len);
	if (len) {
		memcpy(p_evt->data, pdata, len);
	}
	return rtk_bt_evt_indicate(p_evt, NULL);
}

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
		rtk_bt_mesh_private_beacon_get_t *private_beacon_client_get = (rtk_bt_mesh_private_beacon_get_t *)p_cmd->param;
		ret = private_beacon_get(private_beacon_client_get->dst, private_beacon_client_get->net_key_index);
		break;
	}
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_SET: {
		rtk_bt_mesh_private_beacon_set_t *private_beacon_client_set = (rtk_bt_mesh_private_beacon_set_t *)p_cmd->param;
		ret = private_beacon_set(private_beacon_client_set->dst, private_beacon_client_set->net_key_index,
								 private_beacon_client_set->private_beacon, private_beacon_client_set->has_random_update,
								 private_beacon_client_set->random_update_interval_steps);
		break;
	}
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GATT_PROXY_GET: {
		rtk_bt_mesh_private_gatt_proxy_get_t *gatt_get = (rtk_bt_mesh_private_gatt_proxy_get_t *)p_cmd->param;
		ret = private_gatt_proxy_get(gatt_get->dst, gatt_get->net_key_index);
		break;
	}
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GATT_PROXY_SET: {
		rtk_bt_mesh_private_gatt_proxy_set_t *gatt_set = (rtk_bt_mesh_private_gatt_proxy_set_t *)p_cmd->param;
		ret = private_gatt_proxy_set(gatt_set->dst, gatt_set->net_key_index, gatt_set->private_gatt_proxy);
		break;
	}
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_NODE_IDENTITY_GET: {
		rtk_bt_mesh_private_node_identity_get_t *identity_get = (rtk_bt_mesh_private_node_identity_get_t *)p_cmd->param;
		ret = private_node_identity_get(identity_get->dst, identity_get->net_key_index, identity_get->sub_net_key_index);
		break;
	}
	case RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_NODE_IDENTITY_SET: {
		rtk_bt_mesh_private_node_identity_set_t *identity_set = (rtk_bt_mesh_private_node_identity_set_t *)p_cmd->param;
		ret = private_node_identity_set(identity_set->dst, identity_set->net_key_index, identity_set->sub_net_key_index, identity_set->private_identity);
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

bool private_beacon_client_init(void)
{
	/* register private beacon client model */
	return private_beacon_client_reg(0);
}
#endif
#endif