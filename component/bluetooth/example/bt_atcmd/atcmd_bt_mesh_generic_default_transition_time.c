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
#include <rtk_bt_mesh_generic_default_transition_time.h>

static int atcmd_ble_mesh_generic_default_transition_time_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_default_transition_time_set_t time_set;
	time_set.dst = str_to_int(argv[0]);
	time_set.trans_time.num_steps = str_to_int(argv[1]);
	time_set.trans_time.step_resolution = str_to_int(argv[2]);
	time_set.ack = str_to_int(argv[3]);
	time_set.app_key_index = str_to_int(argv[4]);
	ret = rtk_bt_mesh_generic_default_transition_time_client_set(&time_set);
	if (ret) {
		BT_LOGE("[%s] Set default transition time failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic default transition time model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_default_transition_time_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_default_transition_time_get_t time_get;
	time_get.dst = str_to_int(argv[0]);
	time_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_default_transition_time_client_get(&time_get);
	if (ret) {
		BT_LOGE("[%s] Get default transition time failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic default transition time model.\r\n");
	return -1;
#endif
}


static const cmd_table_t mesh_generic_default_transition_time_cmd_table[] = {
	{"gdtts",    atcmd_ble_mesh_generic_default_transition_time_set,    6, 6},
	{"gdttg",    atcmd_ble_mesh_generic_default_transition_time_get,    3, 3},
	{NULL,},
};

int atcmd_bt_mesh_generic_default_transition_time(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_generic_default_transition_time_cmd_table, "[AT+BLEMESHGDTT]");
}

#endif // end of RTK_BLE_MESH_SUPPORT