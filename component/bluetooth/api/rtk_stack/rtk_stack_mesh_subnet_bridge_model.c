#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_subnet_bridge_model.h>
#include <subnet_bridge_model.h>

#if defined(BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL) && BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL
uint8_t rtk_bt_mesh_subnet_bridge_client_model_indicate_event(uint32_t event_code, void *pdata, uint32_t len)
{
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SUBNET_BRIDGE_CLIENT_MODEL, event_code, len);
	if (len) {
		if (event_code == RTK_BT_MESH_SUBNET_BRIDGE_BRIDGED_SUBNETS_CLIENT_MODEL_LIST) {
			memcpy(p_evt->data, pdata, sizeof(rtk_bt_mesh_bridged_subnets_list_t));
			rtk_bt_mesh_bridged_subnets_list_t *data = (rtk_bt_mesh_bridged_subnets_list_t *) pdata;
			memcpy((uint8_t *)(p_evt->data) + sizeof(rtk_bt_mesh_bridged_subnets_list_t), data->bridged_subnets_list,
				   len - sizeof(rtk_bt_mesh_bridged_subnets_list_t));
		} else if (event_code == RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_LIST) {
			memcpy(p_evt->data, pdata, sizeof(rtk_bt_mesh_bridging_table_list_t));
			rtk_bt_mesh_bridging_table_list_t *data = (rtk_bt_mesh_bridging_table_list_t *) pdata;
			memcpy((uint8_t *)(p_evt->data) + sizeof(rtk_bt_mesh_bridging_table_list_t), data->bridged_addrs_list,
				   len - sizeof(rtk_bt_mesh_bridging_table_list_t));
		} else {
			memcpy(p_evt->data, pdata, len);
		}
	}
	return rtk_bt_evt_indicate(p_evt, NULL);
}

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_subnet_bridge_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_GET: {
		rtk_bt_mesh_subnet_bridge_get_t *subnet_bridge_client_get = (rtk_bt_mesh_subnet_bridge_get_t *)p_cmd->param;
		ret = subnet_bridge_get(subnet_bridge_client_get->dst, subnet_bridge_client_get->net_key_index);
		break;
	}
	case RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_SET: {
		rtk_bt_mesh_subnet_bridge_set_t *subnet_bridge_client_set = (rtk_bt_mesh_subnet_bridge_set_t *)p_cmd->param;
		ret = subnet_bridge_set(subnet_bridge_client_set->dst, subnet_bridge_client_set->net_key_index, subnet_bridge_client_set->state);
		break;
	}
	case RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_ADD: {
		rtk_bt_mesh_bridging_table_add_t *subnet_bridge_client_add = (rtk_bt_mesh_bridging_table_add_t *)p_cmd->param;
		ret = bridging_table_add(subnet_bridge_client_add->dst, subnet_bridge_client_add->net_key_index, subnet_bridge_client_add->directions,
								 subnet_bridge_client_add->net_key_index1, subnet_bridge_client_add->net_key_index2,
								 subnet_bridge_client_add->addr1, subnet_bridge_client_add->addr2);
		break;
	}
	case RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_REMOVE: {
		rtk_bt_mesh_bridging_table_remove_t *subnet_bridge_client_remove = (rtk_bt_mesh_bridging_table_remove_t *)p_cmd->param;
		ret = bridging_table_remove(subnet_bridge_client_remove->dst, subnet_bridge_client_remove->net_key_index,
									subnet_bridge_client_remove->net_key_index1, subnet_bridge_client_remove->net_key_index2,
									subnet_bridge_client_remove->addr1, subnet_bridge_client_remove->addr2);
		break;
	}
	case RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGED_SUBNETS_GET: {
		rtk_bt_mesh_bridged_subnets_get_t *subnet_bridge_client_bridged_get = (rtk_bt_mesh_bridged_subnets_get_t *)p_cmd->param;
		ret = bridged_subnets_get(subnet_bridge_client_bridged_get->dst, subnet_bridge_client_bridged_get->net_key_index,
								  subnet_bridge_client_bridged_get->filter, subnet_bridge_client_bridged_get->filter_net_key_index,
								  subnet_bridge_client_bridged_get->start_index);
		break;
	}
	case RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_GET: {
		rtk_bt_mesh_bridging_table_get_t *subnet_bridge_client_table_get = (rtk_bt_mesh_bridging_table_get_t *)p_cmd->param;
		ret = bridging_table_get(subnet_bridge_client_table_get->dst, subnet_bridge_client_table_get->net_key_index,
								 subnet_bridge_client_table_get->net_key_index1, subnet_bridge_client_table_get->net_key_index2,
								 subnet_bridge_client_table_get->start_index);
		break;
	}
	case RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_SIZE_GET: {
		rtk_bt_mesh_bridging_table_size_get_t *subnet_bridge_client_size_get = (rtk_bt_mesh_bridging_table_size_get_t *)p_cmd->param;
		ret = bridging_table_size_get(subnet_bridge_client_size_get->dst, subnet_bridge_client_size_get->net_key_index);
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

bool subnet_bridge_client_init(void)
{
	/* register subnet bridge client model */
	return subnet_bridge_client_reg(0);
}
#endif
#endif