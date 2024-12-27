#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_device.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_subnet_bridge_model.h>
#include <rtk_bt_mesh_def.h>

#if defined(BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL) && BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL
uint16_t rtk_bt_mesh_subnet_bridge_get(rtk_bt_mesh_subnet_bridge_get_t *subnet_bridge_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SUBNET_BRIDGE_CLIENT_MODEL, RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_GET, subnet_bridge_get,
						  sizeof(rtk_bt_mesh_subnet_bridge_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_subnet_bridge_set(rtk_bt_mesh_subnet_bridge_set_t *subnet_bridge_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SUBNET_BRIDGE_CLIENT_MODEL, RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_SET, subnet_bridge_set,
						  sizeof(rtk_bt_mesh_subnet_bridge_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_bridging_table_add(rtk_bt_mesh_bridging_table_add_t *bridging_table_add)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SUBNET_BRIDGE_CLIENT_MODEL, RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_ADD, bridging_table_add,
						  sizeof(rtk_bt_mesh_bridging_table_add_t));

	return ret;
}

uint16_t rtk_bt_mesh_bridging_table_remove(rtk_bt_mesh_bridging_table_remove_t *bridging_table_remove)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SUBNET_BRIDGE_CLIENT_MODEL, RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_REMOVE, bridging_table_remove,
						  sizeof(rtk_bt_mesh_bridging_table_remove_t));

	return ret;
}

uint16_t rtk_bt_mesh_bridged_subnets_get(rtk_bt_mesh_bridged_subnets_get_t *bridged_subnets_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SUBNET_BRIDGE_CLIENT_MODEL, RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGED_SUBNETS_GET, bridged_subnets_get,
						  sizeof(rtk_bt_mesh_bridged_subnets_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_bridging_table_get(rtk_bt_mesh_bridging_table_get_t *bridging_table_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SUBNET_BRIDGE_CLIENT_MODEL, RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_GET, bridging_table_get,
						  sizeof(rtk_bt_mesh_bridging_table_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_bridging_table_size_get(rtk_bt_mesh_bridging_table_size_get_t *bridging_table_size_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SUBNET_BRIDGE_CLIENT_MODEL, RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_SIZE_GET, bridging_table_size_get,
						  sizeof(rtk_bt_mesh_bridging_table_size_get_t));

	return ret;
}


#endif

#endif