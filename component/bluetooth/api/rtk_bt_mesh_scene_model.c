#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_device.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_scene_model.h>
#include <rtk_bt_mesh_def.h>


#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
uint16_t rtk_bt_mesh_scene_store(rtk_bt_mesh_scene_store_t *scene_client_store)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL, RTK_BT_MESH_SCENE_CLIENT_MODEL_STORE, scene_client_store,
						  sizeof(rtk_bt_mesh_scene_store_t));

	return ret;
}

uint16_t rtk_bt_mesh_scene_recall(rtk_bt_mesh_scene_recall_t *scene_client_recall)
{
	uint16_t ret;
	static uint8_t tid = 0;

	tid++;
	scene_client_recall->tid = tid;
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL, RTK_BT_MESH_SCENE_CLIENT_MODEL_RECALL, scene_client_recall,
						  sizeof(rtk_bt_mesh_scene_recall_t));

	return ret;
}

uint16_t rtk_bt_mesh_scene_get(rtk_bt_mesh_scene_get_t *scene_client_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL, RTK_BT_MESH_SCENE_CLIENT_MODEL_GET, scene_client_get,
						  sizeof(rtk_bt_mesh_scene_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_scene_register_get(rtk_bt_mesh_scene_register_get_t *scene_register_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL, RTK_BT_MESH_SCENE_CLIENT_MODEL_REGISTER_GET, scene_register_get,
						  sizeof(rtk_bt_mesh_scene_register_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_scene_delete(rtk_bt_mesh_scene_delete_t *scene_delete)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL, RTK_BT_MESH_SCENE_CLIENT_MODEL_DELETE, scene_delete,
						  sizeof(rtk_bt_mesh_scene_delete_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_SCENE_CLIENT_MODEL

#if defined(BT_MESH_ENABLE_SCENE_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SERVER_MODEL
uint16_t rtk_bt_mesh_scene_server_db_set(rtk_bt_mesh_scene_server_db_set_t *scene_db_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL, RTK_BT_MESH_SCENE_SERVER_MODEL_ACT_DB_SET, scene_db_set,
						  sizeof(rtk_bt_mesh_scene_server_db_set_t));

	return ret;
}
#endif // BT_MESH_ENABLE_SCENE_SERVER_MODEL

#if defined(BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL
uint16_t rtk_bt_mesh_scene_setup_server_db_set(rtk_bt_mesh_scene_server_db_set_t *scene_db_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL, RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_ACT_DB_SET, scene_db_set,
						  sizeof(rtk_bt_mesh_scene_server_db_set_t));

	return ret;
}
#endif // BT_MESH_ENABLE_SCENE_SERVER_MODEL

#endif // end of RTK_BLE_MESH_SUPPORT