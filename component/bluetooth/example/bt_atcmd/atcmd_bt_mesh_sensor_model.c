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
#include <rtk_bt_mesh_sensor_model.h>

static int atcmd_ble_mesh_sensor_descriptor_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_sensor_descriptor_get_t sensor_descriptor_get;
	sensor_descriptor_get.dst = str_to_int(argv[0]);
	sensor_descriptor_get.app_key_index = str_to_int(argv[1]);
	if (argc > 2) {
		sensor_descriptor_get.property_id = str_to_int(argv[2]);
	} else {
		sensor_descriptor_get.property_id = 0;
	}
	ret = rtk_bt_mesh_sensor_descriptor_get(&sensor_descriptor_get);
	if (ret) {
		BT_LOGE("[%s] Get sensor descriptor failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support sensor client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_sensor_cadence_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_sensor_cadence_get_t sensor_cadence_get;
	sensor_cadence_get.dst = str_to_int(argv[0]);
	sensor_cadence_get.property_id = str_to_int(argv[1]);
	sensor_cadence_get.app_key_index = str_to_int(argv[2]);
	ret = rtk_bt_mesh_sensor_cadence_get(&sensor_cadence_get);
	if (ret) {
		BT_LOGE("[%s] Get sensor cadence failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support sensor client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_sensor_cadence_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
	uint16_t ret = 0;
	uint16_t len;
	rtk_bt_mesh_sensor_cadence_set_t sensor_cadence_set;
	sensor_cadence_set.dst = str_to_int(argv[0]);
	sensor_cadence_set.property_id = str_to_int(argv[1]);
	sensor_cadence_set.app_key_index = str_to_int(argv[2]);
	sensor_cadence_set.ack = str_to_int(argv[3]);
	sensor_cadence_set.fast_cadence_period_divisor = str_to_int(argv[4]);
	sensor_cadence_set.status_trigger_type = str_to_int(argv[5]);
	sensor_cadence_set.raw_value_len = str_to_int(argv[6]);
	if (sensor_cadence_set.status_trigger_type == 0) {
		len = strlen(argv[7]);
		hexdata_str_to_array(argv[7], sensor_cadence_set.trigger_down, len);
		hexdata_str_to_array(argv[8], sensor_cadence_set.trigger_up, len);
	} else {
		hexdata_str_to_array(argv[7], sensor_cadence_set.trigger_down, 2);
		hexdata_str_to_array(argv[8], sensor_cadence_set.trigger_up, 2);
	}
	sensor_cadence_set.status_min_interval = str_to_int(argv[9]);
	len = strlen(argv[10]);
	hexdata_str_to_array(argv[10], sensor_cadence_set.cadence_low, len);
	hexdata_str_to_array(argv[11], sensor_cadence_set.cadence_high, len);
	ret = rtk_bt_mesh_sensor_cadence_set(&sensor_cadence_set);
	if (ret) {
		BT_LOGE("[%s] Set sensor cadence failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support sensor client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_sensor_settings_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_sensor_settings_get_t sensor_settings_get;
	sensor_settings_get.dst = str_to_int(argv[0]);
	sensor_settings_get.property_id = str_to_int(argv[1]);
	sensor_settings_get.app_key_index = str_to_int(argv[2]);
	ret = rtk_bt_mesh_sensor_settings_get(&sensor_settings_get);
	if (ret) {
		BT_LOGE("[%s] Get sensor settings failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support sensor client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_sensor_setting_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_sensor_setting_get_t sensor_setting_get;
	sensor_setting_get.dst = str_to_int(argv[0]);
	sensor_setting_get.property_id = str_to_int(argv[1]);
	sensor_setting_get.setting_property_id = str_to_int(argv[2]);
	sensor_setting_get.app_key_index = str_to_int(argv[3]);
	ret = rtk_bt_mesh_sensor_setting_get(&sensor_setting_get);
	if (ret) {
		BT_LOGE("[%s] Get sensor setting failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support sensor client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_sensor_setting_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
	uint16_t ret = 0;
	uint16_t len;
	rtk_bt_mesh_sensor_setting_set_t sensor_setting_set;
	sensor_setting_set.dst = str_to_int(argv[0]);
	sensor_setting_set.property_id = str_to_int(argv[1]);
	sensor_setting_set.setting_property_id = str_to_int(argv[2]);
	sensor_setting_set.app_key_index = str_to_int(argv[3]);
	sensor_setting_set.ack = str_to_int(argv[4]);
	len = strlen(argv[5]);
	sensor_setting_set.setting_raw_len = len / 2;
	hexdata_str_to_array(argv[5], sensor_setting_set.setting_raw, len);
	ret = rtk_bt_mesh_sensor_setting_set(&sensor_setting_set);
	if (ret) {
		BT_LOGE("[%s] Set sensor setting failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support sensor client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_sensor_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_sensor_get_t sensor_get;
	sensor_get.dst = str_to_int(argv[0]);
	sensor_get.app_key_index = str_to_int(argv[1]);
	if (argc > 2) {
		sensor_get.property_id = str_to_int(argv[2]);
	} else {
		sensor_get.property_id = 0;
	}
	ret = rtk_bt_mesh_sensor_get(&sensor_get);
	if (ret) {
		BT_LOGE("[%s] Get sensor failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support sensor client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_sensor_column_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
	uint16_t ret = 0;
	uint16_t len;
	rtk_bt_mesh_sensor_column_get_t sensor_column_get;
	sensor_column_get.dst = str_to_int(argv[0]);
	sensor_column_get.property_id = str_to_int(argv[1]);
	sensor_column_get.app_key_index = str_to_int(argv[2]);
	len = strlen(argv[3]);
	sensor_column_get.raw_value_x_len = len / 2;
	hexdata_str_to_array(argv[3], sensor_column_get.raw_value_x, len);
	ret = rtk_bt_mesh_sensor_column_get(&sensor_column_get);
	if (ret) {
		BT_LOGE("[%s] Get sensor column failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support sensor client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_sensor_series_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
	uint16_t ret = 0;
	uint16_t len;
	uint16_t len2;
	rtk_bt_mesh_sensor_series_get_t sensor_series_get;
	sensor_series_get.dst = str_to_int(argv[0]);
	sensor_series_get.property_id = str_to_int(argv[1]);
	sensor_series_get.app_key_index = str_to_int(argv[2]);
	if (argc > 3) {
		if (argc != 5) {
			BT_LOGE("[%s] Input data number is uncorrent.\r\n", __func__);
		}
		len = strlen(argv[3]);
		len2 = strlen(argv[4]);
		if (len / 2 != len2 / 2) {
			BT_LOGE("[%s] Input data length is uncorrent.\r\n", __func__);
		}
		sensor_series_get.raw_value_x_len = len / 2;
		hexdata_str_to_array(argv[3], sensor_series_get.raw_value_x1, len);
		hexdata_str_to_array(argv[4], sensor_series_get.raw_value_x2, len);
	} else {
		sensor_series_get.raw_value_x_len = 0;
	}
	ret = rtk_bt_mesh_sensor_series_get(&sensor_series_get);
	if (ret) {
		BT_LOGE("[%s] Get sensor series failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support sensor client model.\r\n");
	return -1;
#endif
}

static const cmd_table_t mesh_sensor_cmd_table[] = {
	{"sdg",    atcmd_ble_mesh_sensor_descriptor_get,    3, 4},
	{"scg",    atcmd_ble_mesh_sensor_cadence_get,    4, 4},
	{"scs",    atcmd_ble_mesh_sensor_cadence_set,    13, 13},
	{"sssg",    atcmd_ble_mesh_sensor_settings_get,    4, 4},
	{"ssg",    atcmd_ble_mesh_sensor_setting_get,    5, 5},
	{"sss",    atcmd_ble_mesh_sensor_setting_set,    7, 7},
	{"sg",    atcmd_ble_mesh_sensor_get,    3, 4},
	{"scog",    atcmd_ble_mesh_sensor_column_get,    5, 5},
	{"sseg",    atcmd_ble_mesh_sensor_series_get,    4, 6},
	{NULL,},
};

int atcmd_bt_mesh_sensor(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_sensor_cmd_table, "[AT+BLEMESHSENSOR]");
}

#endif // end of RTK_BLE_MESH_SUPPORT