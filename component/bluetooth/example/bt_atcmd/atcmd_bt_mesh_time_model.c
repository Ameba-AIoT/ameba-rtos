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
#include <rtk_bt_mesh_time_model.h>

static int atcmd_ble_mesh_time_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_time_get_t time_get;
	time_get.dst = str_to_int(argv[0]);
	time_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_time_get(&time_get);
	if (ret) {
		BT_LOGE("[%s] Get time failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support time client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_time_zone_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_time_zone_get_t time_zone_get;
	time_zone_get.dst = str_to_int(argv[0]);
	time_zone_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_time_zone_get(&time_zone_get);
	if (ret) {
		BT_LOGE("[%s] Get time zone failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support time client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_time_tai_utc_delta_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_time_tai_utc_delta_get_t time_tai_utc_delta_get;
	time_tai_utc_delta_get.dst = str_to_int(argv[0]);
	time_tai_utc_delta_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_time_tai_utc_delta_get(&time_tai_utc_delta_get);
	if (ret) {
		BT_LOGE("[%s] Get time tai utc delta failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support time client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_time_role_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_time_role_get_t time_role_get;
	time_role_get.dst = str_to_int(argv[0]);
	time_role_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_time_role_get(&time_role_get);
	if (ret) {
		BT_LOGE("[%s] Get time role failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support time client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_time_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
	uint16_t ret = 0;
	uint16_t len;
	rtk_bt_mesh_time_set_t time_set = {0};
	time_set.dst = str_to_int(argv[0]);
	time_set.app_key_index = str_to_int(argv[1]);
	len = strlen(argv[2]);
	if (len / 2 > 5) {
		BT_LOGE("[%s] The len %d of input data extend max value %d\r\n", __func__, len, 5);
		return -2;
	}
	if (hexdata_str_to_array(argv[2], time_set.time.tai_seconds, len)) {
		time_set.time.subsecond = str_to_int(argv[3]);
		time_set.time.uncertainty = str_to_int(argv[4]);
		time_set.time.time_authority = str_to_int(argv[5]);
		time_set.time.tai_utc_delta = str_to_int(argv[6]);
		time_set.time.time_zone_offset = str_to_int(argv[7]);
		ret = rtk_bt_mesh_time_set(&time_set);
		if (ret) {
			BT_LOGE("[%s]Set time failed! reason: 0x%x\r\n", __func__, ret);
			return -1;
		}
		return 0;
	} else {
		BT_LOGE("[%s] Input data format is not corrent.\r\n", __func__);
		return -3;
	}
#else
	(void)argv;
	BT_LOGE("Platform not support time client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_time_zone_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
	uint16_t ret = 0;
	uint16_t len;
	rtk_bt_mesh_time_zone_set_t time_zone_set = {0};
	time_zone_set.dst = str_to_int(argv[0]);
	time_zone_set.app_key_index = str_to_int(argv[1]);
	time_zone_set.time_zone_offset_new = str_to_int(argv[2]);
	len = strlen(argv[3]);
	if (len / 2 > 5) {
		BT_LOGE("[%s] The len %d of input data extend max value %d\r\n", __func__, len, 5);
		return -2;
	}
	if (hexdata_str_to_array(argv[3], time_zone_set.tai_of_zone_change, len)) {
		ret = rtk_bt_mesh_time_zone_set(&time_zone_set);
		if (ret) {
			BT_LOGE("[%s]Set time zone failed! reason: 0x%x\r\n", __func__, ret);
			return -1;
		}
		return 0;
	} else {
		BT_LOGE("[%s] Input data format is not corrent.\r\n", __func__);
		return -3;
	}
#else
	(void)argv;
	BT_LOGE("Platform not support time client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_time_tai_utc_delta_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
	uint16_t ret = 0;
	uint16_t len;
	rtk_bt_mesh_time_tai_utc_delta_set_t time_tai_utc_delta_set = {0};
	time_tai_utc_delta_set.dst = str_to_int(argv[0]);
	time_tai_utc_delta_set.app_key_index = str_to_int(argv[1]);
	time_tai_utc_delta_set.tai_utc_delta_new = str_to_int(argv[2]);
	len = strlen(argv[3]);
	if (len / 2 > 5) {
		BT_LOGE("[%s] The len %d of input data extend max value %d\r\n", __func__, len, 5);
		return -2;
	}
	if (hexdata_str_to_array(argv[3], time_tai_utc_delta_set.tai_of_delta_change, len)) {
		ret = rtk_bt_mesh_time_tai_utc_delta_set(&time_tai_utc_delta_set);
		if (ret) {
			BT_LOGE("[%s]Set time tai utc delta failed! reason: 0x%x\r\n", __func__, ret);
			return -1;
		}
		return 0;
	} else {
		BT_LOGE("[%s] Input data format is not corrent.\r\n", __func__);
		return -3;
	}
#else
	(void)argv;
	BT_LOGE("Platform not support time client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_time_role_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_time_role_set_t time_role_set;
	time_role_set.dst = str_to_int(argv[0]);
	time_role_set.role = str_to_int(argv[1]);
	time_role_set.app_key_index = str_to_int(argv[2]);
	ret = rtk_bt_mesh_time_role_set(&time_role_set);
	if (ret) {
		BT_LOGE("[%s] Set time role failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support time client model.\r\n");
	return -1;
#endif
}


static const cmd_table_t mesh_time_cmd_table[] = {
	{"tg",    atcmd_ble_mesh_time_get,    3, 3},
	{"tzg",    atcmd_ble_mesh_time_zone_get,    3, 3},
	{"ttudg",    atcmd_ble_mesh_time_tai_utc_delta_get,    3, 3},
	{"trg",    atcmd_ble_mesh_time_role_get,    3, 3},
	{"ts",    atcmd_ble_mesh_time_set,    9, 9},
	{"tzs",    atcmd_ble_mesh_time_zone_set,    5, 5},
	{"ttuds",    atcmd_ble_mesh_time_tai_utc_delta_set,    5, 5},
	{"trs",    atcmd_ble_mesh_time_role_set,    4, 4},
	{NULL,},
};

int atcmd_bt_mesh_time(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_time_cmd_table, "[AT+BLEMESHTIME]");
}

#endif // end of RTK_BLE_MESH_SUPPORT