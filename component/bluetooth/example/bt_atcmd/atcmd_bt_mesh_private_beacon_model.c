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
#include <rtk_bt_mesh_private_beacon_model.h>

static int atcmd_ble_mesh_prb_client_private_beacon_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL) && BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_private_beacon_get_t prb_get;
	prb_get.dst = str_to_int(argv[0]);
	prb_get.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_private_beacon_get(&prb_get);
	if (ret) {
		BT_LOGE("[%s] Private beacon get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support private beacon.");
	return -1;
#endif
}

static int atcmd_ble_mesh_prb_client_private_beacon_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL) && BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_private_beacon_set_t prb_set;
	prb_set.dst = str_to_int(argv[0]);
	prb_set.net_key_index = str_to_int(argv[1]);
	prb_set.private_beacon = str_to_int(argv[2]);
	prb_set.has_random_update = 0;
	if (argc > 3) {
		prb_set.has_random_update = 1;
		prb_set.random_update_interval_steps = str_to_int(argv[3]);
	}
	ret = rtk_bt_mesh_private_beacon_set(&prb_set);
	if (ret) {
		BT_LOGE("[%s] Private beacon set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support private beacon.");
	return -1;
#endif
}

static int atcmd_ble_mesh_prb_client_private_gatt_proxy_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL) && BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_private_gatt_proxy_get_t gatt_get;
	gatt_get.dst = str_to_int(argv[0]);
	gatt_get.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_private_gatt_proxy_get(&gatt_get);
	if (ret) {
		BT_LOGE("[%s] Private gatt proxy get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support private beacon.");
	return -1;
#endif
}

static int atcmd_ble_mesh_prb_client_private_gatt_proxy_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL) && BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_private_gatt_proxy_set_t gatt_set;
	gatt_set.dst = str_to_int(argv[0]);
	gatt_set.net_key_index = str_to_int(argv[1]);
	gatt_set.private_gatt_proxy = str_to_int(argv[2]);
	ret = rtk_bt_mesh_private_gatt_proxy_set(&gatt_set);
	if (ret) {
		BT_LOGE("[%s] Private gatt proxy set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support private beacon.");
	return -1;
#endif
}

static int atcmd_ble_mesh_prb_client_private_node_identity_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL) && BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_private_node_identity_get_t identity_get;
	identity_get.dst = str_to_int(argv[0]);
	identity_get.net_key_index = str_to_int(argv[1]);
	identity_get.sub_net_key_index = str_to_int(argv[2]);
	ret = rtk_bt_mesh_private_node_identity_get(&identity_get);
	if (ret) {
		BT_LOGE("[%s] Private node identity get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support private beacon.");
	return -1;
#endif
}

static int atcmd_ble_mesh_prb_client_private_node_identity_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL) && BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_private_node_identity_set_t identity_set;
	identity_set.dst = str_to_int(argv[0]);
	identity_set.net_key_index = str_to_int(argv[1]);
	identity_set.sub_net_key_index = str_to_int(argv[2]);
	identity_set.private_identity = str_to_int(argv[3]);
	ret = rtk_bt_mesh_private_node_identity_set(&identity_set);
	if (ret) {
		BT_LOGE("[%s] Private node identity set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support private beacon.");
	return -1;
#endif
}

static const cmd_table_t mesh_prb_client_cmd_table[] = {
	{"prbg",     atcmd_ble_mesh_prb_client_private_beacon_get,    3, 3},
	{"prbs",     atcmd_ble_mesh_prb_client_private_beacon_set,    4, 5},
	{"pgpg",     atcmd_ble_mesh_prb_client_private_gatt_proxy_get,    3, 3},
	{"pgps",     atcmd_ble_mesh_prb_client_private_gatt_proxy_set,    4, 4},
	{"pnig",     atcmd_ble_mesh_prb_client_private_node_identity_get,    4, 4},
	{"pnis",     atcmd_ble_mesh_prb_client_private_node_identity_set,    5, 5},
	{NULL,},
};

int atcmd_bt_mesh_prb(int argc, char *argv[])
{
	atcmd_bt_excute(argc, argv, mesh_prb_client_cmd_table, "AT+BLEMESHPRB");
	return 0;
}

#endif