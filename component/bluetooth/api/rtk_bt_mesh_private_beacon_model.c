#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_device.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_private_beacon_model.h>
#include <rtk_bt_mesh_def.h>

#if defined(BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL) && BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL

uint16_t rtk_bt_mesh_private_beacon_get(rtk_bt_mesh_private_beacon_get_t *private_beacon_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL, RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GET, private_beacon_get,
						  sizeof(rtk_bt_mesh_private_beacon_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_private_beacon_set(rtk_bt_mesh_private_beacon_set_t *private_beacon_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL, RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_SET, private_beacon_set,
						  sizeof(rtk_bt_mesh_private_beacon_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_private_gatt_proxy_get(rtk_bt_mesh_private_gatt_proxy_get_t *private_gatt_proxy_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL, RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GATT_PROXY_GET, private_gatt_proxy_get,
						  sizeof(rtk_bt_mesh_private_gatt_proxy_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_private_gatt_proxy_set(rtk_bt_mesh_private_gatt_proxy_set_t *private_gatt_proxy_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL, RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GATT_PROXY_SET, private_gatt_proxy_set,
						  sizeof(rtk_bt_mesh_private_gatt_proxy_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_private_node_identity_get(rtk_bt_mesh_private_node_identity_get_t *private_node_identity_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL, RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_NODE_IDENTITY_GET, private_node_identity_get,
						  sizeof(rtk_bt_mesh_private_node_identity_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_private_node_identity_set(rtk_bt_mesh_private_node_identity_set_t *private_node_identity_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL, RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_NODE_IDENTITY_SET, private_node_identity_set,
						  sizeof(rtk_bt_mesh_private_node_identity_set_t));

	return ret;
}

#endif
#endif