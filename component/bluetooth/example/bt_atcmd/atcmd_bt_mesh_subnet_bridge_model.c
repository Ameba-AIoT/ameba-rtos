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
#include <rtk_bt_mesh_subnet_bridge_model.h>

static int atcmd_ble_mesh_sbr_client_subnet_bridge_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL) && BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_subnet_bridge_get_t sbr_get;
	sbr_get.dst = str_to_int(argv[0]);
	sbr_get.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_subnet_bridge_get(&sbr_get);
	if (ret) {
		BT_LOGE("[%s] Subnet bridge get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support subnet bridge.");
	return -1;
#endif
}

static int atcmd_ble_mesh_sbr_client_subnet_bridge_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL) && BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_subnet_bridge_set_t sbr_set;
	sbr_set.dst = str_to_int(argv[0]);
	sbr_set.net_key_index = str_to_int(argv[1]);
	sbr_set.state = str_to_int(argv[2]);
	ret = rtk_bt_mesh_subnet_bridge_set(&sbr_set);
	if (ret) {
		BT_LOGE("[%s] Subnet bridge set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support subnet bridge.");
	return -1;
#endif
}

static int atcmd_ble_mesh_sbr_client_bridging_table_add(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL) && BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_bridging_table_add_t sbr_add;
	sbr_add.dst = str_to_int(argv[0]);
	sbr_add.net_key_index = str_to_int(argv[1]);
	sbr_add.directions = str_to_int(argv[2]);
	sbr_add.net_key_index1 = str_to_int(argv[3]);
	sbr_add.net_key_index2 = str_to_int(argv[4]);
	sbr_add.addr1 = str_to_int(argv[5]);
	sbr_add.addr2 = str_to_int(argv[6]);
	ret = rtk_bt_mesh_bridging_table_add(&sbr_add);
	if (ret) {
		BT_LOGE("[%s] Bridging table add failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support subnet bridge.");
	return -1;
#endif
}

static int atcmd_ble_mesh_sbr_client_bridging_table_remove(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL) && BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_bridging_table_remove_t sbr_remove;
	sbr_remove.dst = str_to_int(argv[0]);
	sbr_remove.net_key_index = str_to_int(argv[1]);
	sbr_remove.net_key_index1 = str_to_int(argv[2]);
	sbr_remove.net_key_index2 = str_to_int(argv[3]);
	sbr_remove.addr1 = str_to_int(argv[4]);
	sbr_remove.addr2 = str_to_int(argv[5]);
	ret = rtk_bt_mesh_bridging_table_remove(&sbr_remove);
	if (ret) {
		BT_LOGE("[%s] Bridging table remove failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support subnet bridge.");
	return -1;
#endif
}

static int atcmd_ble_mesh_sbr_client_bridged_subnets_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL) && BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_bridged_subnets_get_t sbr_get;
	sbr_get.dst = str_to_int(argv[0]);
	sbr_get.net_key_index = str_to_int(argv[1]);
	sbr_get.filter = str_to_int(argv[2]);
	sbr_get.filter_net_key_index = str_to_int(argv[3]);
	sbr_get.start_index = str_to_int(argv[4]);
	ret = rtk_bt_mesh_bridged_subnets_get(&sbr_get);
	if (ret) {
		BT_LOGE("[%s] Bridged subnets get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support subnet bridge.");
	return -1;
#endif
}

static int atcmd_ble_mesh_sbr_client_bridging_table_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL) && BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_bridging_table_get_t sbr_get;
	sbr_get.dst = str_to_int(argv[0]);
	sbr_get.net_key_index = str_to_int(argv[1]);
	sbr_get.net_key_index1 = str_to_int(argv[2]);
	sbr_get.net_key_index2 = str_to_int(argv[3]);
	sbr_get.start_index = str_to_int(argv[4]);
	ret = rtk_bt_mesh_bridging_table_get(&sbr_get);
	if (ret) {
		BT_LOGE("[%s] Bridging table get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support subnet bridge.");
	return -1;
#endif
}

static int atcmd_ble_mesh_sbr_client_bridging_table_size_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL) && BT_MESH_ENABLE_SUBNET_BRIDGE_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_bridging_table_size_get_t sbr_get;
	sbr_get.dst = str_to_int(argv[0]);
	sbr_get.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_bridging_table_size_get(&sbr_get);
	if (ret) {
		BT_LOGE("[%s] Bridging table size get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support subnet bridge.");
	return -1;
#endif
}

static const cmd_table_t mesh_sbr_client_cmd_table[] = {
	{"sbg",     atcmd_ble_mesh_sbr_client_subnet_bridge_get,    3, 3},
	{"sbs",     atcmd_ble_mesh_sbr_client_subnet_bridge_set,    4, 4},
	{"bta",     atcmd_ble_mesh_sbr_client_bridging_table_add,    8, 8},
	{"btr",     atcmd_ble_mesh_sbr_client_bridging_table_remove,    7, 7},
	{"bsg",     atcmd_ble_mesh_sbr_client_bridged_subnets_get,    6, 6},
	{"btbg",    atcmd_ble_mesh_sbr_client_bridging_table_get,    6, 6},
	{"btsg",    atcmd_ble_mesh_sbr_client_bridging_table_size_get,    3, 3},
	{NULL,},
};

int atcmd_bt_mesh_sbr(int argc, char *argv[])
{
	atcmd_bt_excute(argc, argv, mesh_sbr_client_cmd_table, "AT+BLEMESHSBR");
	return 0;
}


#endif