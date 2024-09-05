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
#include <rtk_bt_mesh_generic_model.h>

static int atcmd_ble_mesh_generic_level_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_level_get_t level_get;
	level_get.dst = str_to_int(argv[0]);
	level_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_level_client_get(&level_get);
	if (ret) {
		BT_LOGE("[%s] Generic level model get failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic level client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_level_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_level_set_t level_set;
	level_set.dst = str_to_int(argv[0]);
	level_set.level = str_to_int(argv[1]);
	level_set.ack = str_to_int(argv[2]);
	level_set.app_key_index = str_to_int(argv[3]);
	if (argc > 4) {
		level_set.optional = 1;
		level_set.trans_time.num_steps = str_to_int(argv[4]);
		level_set.trans_time.step_resolution = str_to_int(argv[5]);
		level_set.delay = str_to_int(argv[6]);
	} else {
		level_set.optional = 0;
	}
	ret = rtk_bt_mesh_generic_level_client_set(&level_set);
	if (ret) {
		BT_LOGE("[%s] Generic level model set failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic level client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_level_delta_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_level_delta_set_t delta_set;
	delta_set.dst = str_to_int(argv[0]);
	delta_set.delta_level = str_to_int(argv[1]);
	delta_set.ack = str_to_int(argv[2]);
	delta_set.app_key_index = str_to_int(argv[3]);
	if (argc > 4) {
		delta_set.optional = 1;
		delta_set.trans_time.num_steps = str_to_int(argv[4]);
		delta_set.trans_time.step_resolution = str_to_int(argv[5]);
		delta_set.delay = str_to_int(argv[6]);
	} else {
		delta_set.optional = 0;
	}
	ret = rtk_bt_mesh_generic_level_client_delta_set(&delta_set);
	if (ret) {
		BT_LOGE("[%s] Generic level model delta set failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic level client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_level_move_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_level_move_set_t move_set;
	move_set.dst = str_to_int(argv[0]);
	move_set.delta_level = str_to_int(argv[1]);
	move_set.ack = str_to_int(argv[2]);
	move_set.app_key_index = str_to_int(argv[3]);
	if (argc > 4) {
		move_set.optional = 1;
		move_set.trans_time.num_steps = str_to_int(argv[4]);
		move_set.trans_time.step_resolution = str_to_int(argv[5]);
		move_set.delay = str_to_int(argv[6]);
	} else {
		move_set.optional = 0;
	}
	ret = rtk_bt_mesh_generic_level_client_move_set(&move_set);
	if (ret) {
		BT_LOGE("[%s] Generic level model delta set failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic level client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_power_on_off_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_on_power_up_get_t power_get;
	power_get.dst = str_to_int(argv[0]);
	power_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_on_power_up_client_get(&power_get);
	if (ret) {
		BT_LOGE("[%s] Generic power on off model get failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic power onoff client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_power_on_off_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_on_power_up_set_t power_set;
	power_set.dst = str_to_int(argv[0]);
	power_set.on_power_up = str_to_int(argv[1]);
	power_set.ack = str_to_int(argv[2]);
	power_set.app_key_index = str_to_int(argv[3]);
	ret = rtk_bt_mesh_generic_on_power_up_client_set(&power_set);
	if (ret) {
		BT_LOGE("[%s] Generic power on off model delta set failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic power onoff client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_power_level_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_power_level_get_t power_get;
	power_get.dst = str_to_int(argv[0]);
	power_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_power_level_client_get(&power_get);
	if (ret) {
		BT_LOGE("[%s] Generic power level model get failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic power level client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_power_level_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_power_level_set_t power_set;
	power_set.dst = str_to_int(argv[0]);
	power_set.power = str_to_int(argv[1]);
	power_set.ack = str_to_int(argv[2]);
	power_set.app_key_index = str_to_int(argv[3]);
	if (argc > 4) {
		power_set.optional = 1;
		power_set.trans_time.num_steps = str_to_int(argv[4]);
		power_set.trans_time.step_resolution = str_to_int(argv[5]);
		power_set.delay = str_to_int(argv[6]);
	} else {
		power_set.optional = 0;
	}
	ret = rtk_bt_mesh_generic_power_level_client_set(&power_set);
	if (ret) {
		BT_LOGE("[%s] Generic power level model set failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic power level client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_power_level_last_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_power_level_last_get_t power_get;
	power_get.dst = str_to_int(argv[0]);
	power_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_power_level_client_last_get(&power_get);
	if (ret) {
		BT_LOGE("[%s] Generic power level model get last failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic power level client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_power_level_default_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_power_level_default_get_t power_get;
	power_get.dst = str_to_int(argv[0]);
	power_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_power_level_client_default_get(&power_get);
	if (ret) {
		BT_LOGE("[%s] Generic power level model get default failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic power level client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_power_level_default_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_power_level_default_set_t power_set;
	power_set.dst = str_to_int(argv[0]);
	power_set.power = str_to_int(argv[1]);
	power_set.ack = str_to_int(argv[2]);
	power_set.app_key_index = str_to_int(argv[3]);
	ret = rtk_bt_mesh_generic_power_level_client_default_set(&power_set);
	if (ret) {
		BT_LOGE("[%s] Generic power level model set default failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic power level client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_power_level_range_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_power_level_range_get_t power_get;
	power_get.dst = str_to_int(argv[0]);
	power_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_power_level_client_range_get(&power_get);
	if (ret) {
		BT_LOGE("[%s] Generic power level model get range failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic power level client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_power_level_range_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_power_level_range_set_t power_set;
	power_set.dst = str_to_int(argv[0]);
	power_set.range_min = str_to_int(argv[1]);
	power_set.range_max = str_to_int(argv[2]);
	power_set.ack = str_to_int(argv[3]);
	power_set.app_key_index = str_to_int(argv[4]);
	ret = rtk_bt_mesh_generic_power_level_client_range_set(&power_set);
	if (ret) {
		BT_LOGE("[%s] Generic power level model set range failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic power level client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_battery_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_battery_get_t battery_get;
	battery_get.dst = str_to_int(argv[0]);
	battery_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_battery_client_get(&battery_get);
	if (ret) {
		BT_LOGE("[%s] Generic battery model get failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic battery client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_location_global_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_location_global_get_t global_get;
	global_get.dst = str_to_int(argv[0]);
	global_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_location_client_global_get(&global_get);
	if (ret) {
		BT_LOGE("[%s] Generic location model get global failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic location client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_location_global_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_location_global_set_t global_set;
	global_set.dst = str_to_int(argv[0]);
	global_set.global.global_latitude = str_to_int(argv[1]);
	global_set.global.global_longitude = str_to_int(argv[2]);
	global_set.global.global_altitude = str_to_int(argv[3]);
	global_set.ack = str_to_int(argv[4]);
	global_set.app_key_index = str_to_int(argv[5]);
	ret = rtk_bt_mesh_generic_location_client_global_set(&global_set);
	if (ret) {
		BT_LOGE("[%s] Generic location model set global failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic location client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_location_local_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_location_local_get_t local_get;
	local_get.dst = str_to_int(argv[0]);
	local_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_location_client_local_get(&local_get);
	if (ret) {
		BT_LOGE("[%s] Generic location model get local failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic location client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_location_local_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_location_local_set_t local_set = {0};
	local_set.dst = str_to_int(argv[0]);
	local_set.local.local_north = str_to_int(argv[1]);
	local_set.local.local_east = str_to_int(argv[2]);
	local_set.local.local_altitude = str_to_int(argv[3]);
	local_set.local.floor_num = str_to_int(argv[4]);
	local_set.local.uncertainty.stationary = str_to_int(argv[5]);
	local_set.local.uncertainty.update_time = str_to_int(argv[6]);
	local_set.local.uncertainty.precision = str_to_int(argv[7]);
	local_set.ack = str_to_int(argv[8]);
	local_set.app_key_index = str_to_int(argv[9]);
	ret = rtk_bt_mesh_generic_location_client_local_set(&local_set);
	if (ret) {
		BT_LOGE("[%s] Generic location model set local failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic location client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_user_properties_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_user_properties_get_t user_properties_get;
	user_properties_get.dst = str_to_int(argv[0]);
	user_properties_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_property_client_user_properties_get(&user_properties_get);
	if (ret) {
		BT_LOGE("[%s] Generic property model get user properties failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic property client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_user_property_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_user_property_get_t user_property_get;
	user_property_get.dst = str_to_int(argv[0]);
	user_property_get.property_id = str_to_int(argv[1]);
	user_property_get.app_key_index = str_to_int(argv[2]);
	ret = rtk_bt_mesh_generic_property_client_user_property_get(&user_property_get);
	if (ret) {
		BT_LOGE("[%s] Generic property model get user properties failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic property client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_user_property_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
	uint16_t ret = 0;
	uint16_t len;
	rtk_bt_mesh_generic_user_property_set_t user_property_set;
	user_property_set.dst = str_to_int(argv[0]);
	user_property_set.property_id = str_to_int(argv[1]);
	user_property_set.ack = str_to_int(argv[2]);
	user_property_set.app_key_index = str_to_int(argv[3]);
	if (argc > 4) {
		len = strlen(argv[4]);
		if (len / 2 > GENERIC_PROPERTY_DATA_MAX_LEN) {
			BT_LOGE("[%s] The len %d of input data extend max value %d\r\n", __func__, len, GENERIC_PROPERTY_DATA_MAX_LEN);
			return -2;
		}
		user_property_set.value_len = len / 2;
		if (hexdata_str_to_array(argv[4], user_property_set.pvalue, len)) {
			ret = rtk_bt_mesh_generic_property_client_user_property_set(&user_property_set);
			if (ret) {
				BT_LOGE("[%s] Set generic user property failed! reason: 0x%x\r\n", __func__, ret);
				return -1;
			}
			return 0;
		} else {
			BT_LOGE("[%s] Input data format is not corrent.\r\n", __func__);
			return -3;
		}
	} else {
		user_property_set.value_len = 0;
		ret = rtk_bt_mesh_generic_property_client_user_property_set(&user_property_set);
		if (ret) {
			BT_LOGE("[%s] Set generic user property failed! reason: 0x%x\r\n", __func__, ret);
			return -1;
		}
		return 0;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic property client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_admin_properties_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_admin_properties_get_t admin_properties_get;
	admin_properties_get.dst = str_to_int(argv[0]);
	admin_properties_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_property_client_admin_properties_get(&admin_properties_get);
	if (ret) {
		BT_LOGE("[%s] Generic property model get admin properties failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic property client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_admin_property_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_admin_property_get_t admin_property_get;
	admin_property_get.dst = str_to_int(argv[0]);
	admin_property_get.property_id = str_to_int(argv[1]);
	admin_property_get.app_key_index = str_to_int(argv[2]);
	ret = rtk_bt_mesh_generic_property_client_admin_property_get(&admin_property_get);
	if (ret) {
		BT_LOGE("[%s] Generic property model get admin property failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic property client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_admin_property_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
	uint16_t ret = 0;
	uint16_t len;
	rtk_bt_mesh_generic_admin_property_set_t admin_property_set;
	admin_property_set.dst = str_to_int(argv[0]);
	admin_property_set.property_id = str_to_int(argv[1]);
	admin_property_set.property_access = str_to_int(argv[2]);
	admin_property_set.ack = str_to_int(argv[3]);
	admin_property_set.app_key_index = str_to_int(argv[4]);
	if (argc > 5) {
		len = strlen(argv[5]);
		if (len / 2 > GENERIC_PROPERTY_DATA_MAX_LEN) {
			BT_LOGE("[%s] The len %d of input data extend max value %d\r\n", __func__, len, GENERIC_PROPERTY_DATA_MAX_LEN);
			return -2;
		}
		admin_property_set.value_len = len / 2;
		if (hexdata_str_to_array(argv[5], admin_property_set.pvalue, len)) {
			ret = rtk_bt_mesh_generic_property_client_admin_property_set(&admin_property_set);
			if (ret) {
				BT_LOGE("[%s] Set generic admin property failed! reason: 0x%x\r\n", __func__, ret);
				return -1;
			}
			return 0;
		} else {
			BT_LOGE("[%s] Input data format is not corrent.\r\n", __func__);
			return -3;
		}
	} else {
		admin_property_set.value_len = 0;
		ret = rtk_bt_mesh_generic_property_client_admin_property_set(&admin_property_set);
		if (ret) {
			BT_LOGE("[%s] Set generic admin property failed! reason: 0x%x\r\n", __func__, ret);
			return -1;
		}
		return 0;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic property client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_manu_properties_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_manu_properties_get_t manu_properties_get;
	manu_properties_get.dst = str_to_int(argv[0]);
	manu_properties_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_property_client_manu_properties_get(&manu_properties_get);
	if (ret) {
		BT_LOGE("[%s] Generic property model get manufacturer properties failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic property client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_manu_property_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_manu_property_get_t manu_property_get;
	manu_property_get.dst = str_to_int(argv[0]);
	manu_property_get.property_id = str_to_int(argv[1]);
	manu_property_get.app_key_index = str_to_int(argv[2]);
	ret = rtk_bt_mesh_generic_property_client_manu_property_get(&manu_property_get);
	if (ret) {
		BT_LOGE("[%s] Generic property model get manufacturer property failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic property client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_manu_property_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_manu_property_set_t manu_property_set;
	manu_property_set.dst = str_to_int(argv[0]);
	manu_property_set.property_id = str_to_int(argv[1]);
	manu_property_set.property_access = str_to_int(argv[2]);
	manu_property_set.ack = str_to_int(argv[3]);
	manu_property_set.app_key_index = str_to_int(argv[4]);
	ret = rtk_bt_mesh_generic_property_client_manu_property_set(&manu_property_set);
	if (ret) {
		BT_LOGE("[%s] Set generic property model manufacturer property failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic property client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_generic_client_properties_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_generic_client_properties_get_t client_properties_get;
	client_properties_get.dst = str_to_int(argv[0]);
	client_properties_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_generic_property_client_client_properties_get(&client_properties_get);
	if (ret) {
		BT_LOGE("[%s] Generic property model get client properties failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support generic property client model.\r\n");
	return -1;
#endif
}

static const cmd_table_t mesh_generic_level_cmd_table[] = {
	{"glg",    atcmd_ble_mesh_generic_level_get,    3, 3},
	{"gls",    atcmd_ble_mesh_generic_level_set,    5, 8},
	{"glds",   atcmd_ble_mesh_generic_level_delta_set,    5, 8},
	{"glms",   atcmd_ble_mesh_generic_level_move_set,    5, 8},
	{NULL,},
};

int atcmd_bt_mesh_generic_level(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_generic_level_cmd_table, "[AT+BLEMESHGLE]");
}

static const cmd_table_t mesh_generic_power_on_off_cmd_table[] = {
	{"gpg",    atcmd_ble_mesh_generic_power_on_off_get,    3, 3},
	{"gps",    atcmd_ble_mesh_generic_power_on_off_set,    5, 5},
	{NULL,},
};

int atcmd_bt_mesh_generic_power_on_off(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_generic_power_on_off_cmd_table, "[AT+BLEMESHGPOO]");
}

static const cmd_table_t mesh_generic_power_level_cmd_table[] = {
	{"gplg",    atcmd_ble_mesh_generic_power_level_get,    3, 3},
	{"gpls",    atcmd_ble_mesh_generic_power_level_set,    5, 8},
	{"gpllg",   atcmd_ble_mesh_generic_power_level_last_get,    3, 3},
	{"gpldg",   atcmd_ble_mesh_generic_power_level_default_get,    3, 3},
	{"gplds",   atcmd_ble_mesh_generic_power_level_default_set,    5, 5},
	{"gplrg",   atcmd_ble_mesh_generic_power_level_range_get,    3, 3},
	{"gplrs",   atcmd_ble_mesh_generic_power_level_range_set,    6, 6},
	{NULL,},
};

int atcmd_bt_mesh_generic_power_level(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_generic_power_level_cmd_table, "[AT+BLEMESHGPL]");
}

static const cmd_table_t mesh_generic_battery_cmd_table[] = {
	{"gbg",    atcmd_ble_mesh_generic_battery_get,    3, 3},
	{NULL,},
};

int atcmd_bt_mesh_generic_battery(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_generic_battery_cmd_table, "[AT+BLEMESHGB]");
}

static const cmd_table_t mesh_generic_location_cmd_table[] = {
	{"glgg",    atcmd_ble_mesh_generic_location_global_get,    3, 3},
	{"glgs",    atcmd_ble_mesh_generic_location_global_set,    7, 7},
	{"gllg",    atcmd_ble_mesh_generic_location_local_get,    3, 3},
	{"glls",    atcmd_ble_mesh_generic_location_local_set,    11, 11},
	{NULL,},
};

int atcmd_bt_mesh_generic_location(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_generic_location_cmd_table, "[AT+BLEMESHGLO]");
}

static const cmd_table_t mesh_generic_property_cmd_table[] = {
	{"gupsg",   atcmd_ble_mesh_generic_user_properties_get,    3, 3},
	{"gupg",    atcmd_ble_mesh_generic_user_property_get,    4, 4},
	{"gups",    atcmd_ble_mesh_generic_user_property_set,    5, 6},
	{"gapsg",   atcmd_ble_mesh_generic_admin_properties_get,    3, 3},
	{"gapg",    atcmd_ble_mesh_generic_admin_property_get,    4, 4},
	{"gaps",    atcmd_ble_mesh_generic_admin_property_set,    6, 7},
	{"gmpsg",   atcmd_ble_mesh_generic_manu_properties_get,    3, 3},
	{"gmpg",    atcmd_ble_mesh_generic_manu_property_get,    4, 4},
	{"gmps",    atcmd_ble_mesh_generic_manu_property_set,    6, 6},
	{"gcpsg",   atcmd_ble_mesh_generic_client_properties_get,    3, 3},
	{NULL,},
};

int atcmd_bt_mesh_generic_property(int argc, char *argv[])
{
	return  atcmd_bt_excute(argc, argv, mesh_generic_property_cmd_table, "[AT+BLEMESHGP]");
}

#endif // end of RTK_BLE_MESH_SUPPORT