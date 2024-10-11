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

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <bt_utils.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_mesh_generic_onoff_model.h>

static int atcmd_ble_mesh_generic_onff_set(int argc, char **argv)
{
#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_onoff_client_act_set_t onoff_set = {0};
	switch (argc) {
	case 7: {
		onoff_set.optional = true;
		onoff_set.trans_time.num_steps = str_to_int(argv[4]);
		onoff_set.trans_time.step_resolution = str_to_int(argv[5]);
		onoff_set.delay = str_to_int(argv[6]);
		__attribute__((fallthrough));
	}
	case 4: {
		onoff_set.dst = str_to_int(argv[0]);
		onoff_set.on_off = str_to_int(argv[1]);
		onoff_set.ack = str_to_int(argv[2]);
		onoff_set.app_key_index = str_to_int(argv[3]);
		break;
	}
	default: {
		BT_LOGE("[%s] The input number %d is incorrect, only 4 or 7 is support\r\n", __func__, argc);
	}
	}

	ret = rtk_bt_mesh_generic_onoff_client_set(&onoff_set);
	if (ret) {
		BT_LOGE("[%s] Generic on off model set failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argc;
	(void)argv;
	BT_LOGE("Platform not support generic on off client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_onff_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_onoff_client_act_get_t onoff_get;
	onoff_get.dst = str_to_int(argv[0]);
	onoff_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_onoff_client_get(&onoff_get);
	if (ret) {
		BT_LOGE("[%s] Generic on off model get failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic on off client model.\r\n");
	return -1;
#endif
}

static const cmd_table_t mesh_generic_onoff_cmd_table[] = {
	{"set",    atcmd_ble_mesh_generic_onff_set,    5, 8},
	{"get",    atcmd_ble_mesh_generic_onff_get,    3, 3},
	{NULL,},
};

int atcmd_bt_mesh_generic_onoff(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_generic_onoff_cmd_table, "[AT+BLEMESHGOO]");
}

#endif // end of RTK_BLE_MESH_SUPPORT
