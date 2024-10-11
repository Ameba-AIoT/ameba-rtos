/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <atcmd_service.h>
#include <bt_api_config.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <bt_utils.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_common.h>
#include <rtk_bt_mesh_scene_model.h>

static int atcmd_ble_mesh_scene_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_scene_get_t scene_get;
	scene_get.dst = str_to_int(argv[0]);
	scene_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_scene_get(&scene_get);
	if (ret) {
		BT_LOGE("[%s] Get scene failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support scene client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_scene_store(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_scene_store_t scene_store;
	scene_store.dst = str_to_int(argv[0]);
	scene_store.scene_number = str_to_int(argv[1]);
	scene_store.ack = str_to_int(argv[2]);
	scene_store.app_key_index = str_to_int(argv[3]);
	ret = rtk_bt_mesh_scene_store(&scene_store);
	if (ret) {
		BT_LOGE("[%s] Store scene failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support scene client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_scene_recall(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_scene_recall_t scene_recall = {0};
	scene_recall.dst = str_to_int(argv[0]);
	scene_recall.scene_number = str_to_int(argv[1]);
	scene_recall.ack = str_to_int(argv[2]);
	scene_recall.app_key_index = str_to_int(argv[3]);
	if (argc > 4) {
		scene_recall.optional = 1;
		scene_recall.trans_time.num_steps = str_to_int(argv[4]);
		scene_recall.trans_time.step_resolution = str_to_int(argv[5]);
		scene_recall.delay = str_to_int(argv[6]);
	}
	ret = rtk_bt_mesh_scene_recall(&scene_recall);
	if (ret) {
		BT_LOGE("[%s] Recall scene failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support scene client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_scene_register_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_scene_register_get_t scene_register_get;
	scene_register_get.dst = str_to_int(argv[0]);
	scene_register_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_scene_register_get(&scene_register_get);
	if (ret) {
		BT_LOGE("[%s] Get scene register failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support scene client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_scene_delete(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_scene_delete_t scene_delete;
	scene_delete.dst = str_to_int(argv[0]);
	scene_delete.scene_number = str_to_int(argv[1]);
	scene_delete.ack = str_to_int(argv[2]);
	scene_delete.app_key_index = str_to_int(argv[3]);
	ret = rtk_bt_mesh_scene_delete(&scene_delete);
	if (ret) {
		BT_LOGE("[%s] Delete scene failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support scene client model.\r\n");
	return -1;
#endif
}


static const cmd_table_t mesh_scene_cmd_table[] = {
	{"sg",    atcmd_ble_mesh_scene_get,    3, 3},
	{"ss",    atcmd_ble_mesh_scene_store,    5, 5},
	{"sr",    atcmd_ble_mesh_scene_recall,    5, 8},
	{"srg",    atcmd_ble_mesh_scene_register_get,    3, 3},
	{"sd",    atcmd_ble_mesh_scene_delete,    5, 5},
	{NULL,},
};

int atcmd_bt_mesh_scene(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_scene_cmd_table, "[AT+BLEMESHSCENE]");
}

#endif // end of RTK_BLE_MESH_SUPPORT