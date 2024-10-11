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
#include <rtk_bt_mesh_health_model.h>

static int atcmd_ble_mesh_health_fault_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_health_fault_get_t fault_get;
	fault_get.dst = str_to_int(argv[0]);
	fault_get.company_id = str_to_int(argv[1]);
	fault_get.app_key_index = str_to_int(argv[2]);
	ret = rtk_bt_mesh_health_fault_get(&fault_get);
	if (ret) {
		BT_LOGE("[%s] Health model get fault failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support health client model.\r\n");
	return -1;
#endif
	return 0;
}

static int atcmd_ble_mesh_health_fault_clear(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_health_fault_clear_t fault_clear;
	fault_clear.dst = str_to_int(argv[0]);
	fault_clear.company_id = str_to_int(argv[1]);
	fault_clear.ack = str_to_int(argv[2]);
	fault_clear.app_key_index = str_to_int(argv[3]);
	ret = rtk_bt_mesh_health_fault_clear(&fault_clear);
	if (ret) {
		BT_LOGE("[%s] Health model clear fault failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support health client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_health_fault_test(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_health_fault_test_t fault_test;
	fault_test.dst = str_to_int(argv[0]);
	fault_test.test_id = str_to_int(argv[1]);
	fault_test.company_id = str_to_int(argv[2]);
	fault_test.ack = str_to_int(argv[3]);
	fault_test.app_key_index = str_to_int(argv[4]);
	ret = rtk_bt_mesh_health_fault_test(&fault_test);
	if (ret) {
		BT_LOGE("[%s] Health model test fault failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support health client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_health_period_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_health_period_get_t period_get;
	period_get.dst = str_to_int(argv[0]);
	period_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_health_period_get(&period_get);
	if (ret) {
		BT_LOGE("[%s] Health model get period failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support health client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_health_period_set(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_health_period_set_t period_set;
	period_set.dst = str_to_int(argv[0]);
	period_set.fast_period_divisor = str_to_int(argv[1]);
	period_set.ack = str_to_int(argv[2]);
	period_set.app_key_index = str_to_int(argv[3]);
	ret = rtk_bt_mesh_health_period_set(&period_set);
	if (ret) {
		BT_LOGE("[%s] Health model set period failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support health client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_health_attn_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_health_attn_get_t attn_get;
	attn_get.dst = str_to_int(argv[0]);
	attn_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_health_attn_get(&attn_get);
	if (ret) {
		BT_LOGE("[%s] Health model get attention timer failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support health client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_health_attn_set(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_health_attn_set_t attn_set;
	attn_set.dst = str_to_int(argv[0]);
	attn_set.attn = str_to_int(argv[1]);
	attn_set.ack = str_to_int(argv[2]);
	attn_set.app_key_index = str_to_int(argv[3]);
	ret = rtk_bt_mesh_health_attn_set(&attn_set);
	if (ret) {
		BT_LOGE("[%s] Health model set attention timer failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support health client model.\r\n");
	return -1;
#endif
}

static const cmd_table_t mesh_health_cmd_table[] = {
	{"hfg",    atcmd_ble_mesh_health_fault_get,    4, 4},
	{"hfc",    atcmd_ble_mesh_health_fault_clear,    5, 5},
	{"hft",    atcmd_ble_mesh_health_fault_test,    6, 6},
	{"hpg",    atcmd_ble_mesh_health_period_get,    3, 3},
	{"hps",    atcmd_ble_mesh_health_period_set,    5, 5},
	{"hag",    atcmd_ble_mesh_health_attn_get,    3, 3},
	{"has",    atcmd_ble_mesh_health_attn_set,    5, 5},
	{NULL,},
};

int atcmd_bt_mesh_health(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_health_cmd_table, "[AT+BLEMESHHEALTH]");
}

#endif // end of RTK_BLE_MESH_SUPPORT