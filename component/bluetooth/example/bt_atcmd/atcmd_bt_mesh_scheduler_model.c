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
#include <rtk_bt_mesh_scheduler_model.h>

static int atcmd_ble_mesh_scheduler_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL) && BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_scheduler_get_t scheduler_get;
	scheduler_get.dst = str_to_int(argv[0]);
	scheduler_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_scheduler_get(&scheduler_get);
	if (ret) {
		BT_LOGE("[%s] Get scheduler failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support scheduler client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_scheduler_action_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL) && BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_scheduler_action_get_t scheduler_action_get;
	scheduler_action_get.dst = str_to_int(argv[0]);
	scheduler_action_get.index = str_to_int(argv[1]);
	scheduler_action_get.app_key_index = str_to_int(argv[2]);
	ret = rtk_bt_mesh_scheduler_action_get(&scheduler_action_get);
	if (ret) {
		BT_LOGE("[%s] Get scheduler action failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support scheduler client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_scheduler_action_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL) && BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_scheduler_action_set_t scheduler_action_set;
	scheduler_action_set.dst = str_to_int(argv[0]);
	scheduler_action_set.scheduler.index = str_to_int(argv[1]);
	scheduler_action_set.scheduler.year = str_to_int(argv[2]);
	scheduler_action_set.scheduler.month = str_to_int(argv[3]);
	scheduler_action_set.scheduler.day = str_to_int(argv[4]);
	scheduler_action_set.scheduler.hour = str_to_int(argv[5]);
	scheduler_action_set.scheduler.minute = str_to_int(argv[6]);
	scheduler_action_set.scheduler.second = str_to_int(argv[7]);
	scheduler_action_set.scheduler.day_of_week = str_to_int(argv[8]);
	scheduler_action_set.scheduler.action = str_to_int(argv[9]);
	scheduler_action_set.scheduler.num_steps = str_to_int(argv[10]);
	scheduler_action_set.scheduler.step_resolution = str_to_int(argv[11]);
	scheduler_action_set.scheduler.scene_number = str_to_int(argv[12]);
	scheduler_action_set.ack = str_to_int(argv[13]);
	scheduler_action_set.app_key_index = str_to_int(argv[14]);
	ret = rtk_bt_mesh_scheduler_action_set(&scheduler_action_set);
	if (ret) {
		BT_LOGE("[%s] Set scheduler action failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support scheduler client model.\r\n");
	return -1;
#endif
}

static const cmd_table_t mesh_scheduler_cmd_table[] = {
	{"scheg",    atcmd_ble_mesh_scheduler_get,    3, 3},
	{"scheag",    atcmd_ble_mesh_scheduler_action_get,    4, 4},
	{"scheas",    atcmd_ble_mesh_scheduler_action_set,    16, 16},
	{NULL,},
};

int atcmd_bt_mesh_scheduler(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_scheduler_cmd_table, "[AT+BLEMESHSCHEDULER]");
}

#endif // end of RTK_BLE_MESH_SUPPORT