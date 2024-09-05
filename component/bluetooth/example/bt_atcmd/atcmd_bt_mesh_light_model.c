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
#include <rtk_bt_mesh_light_model.h>

/*****************light lightness*********************/
static int atcmd_ble_mesh_light_lightness_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lightness_get_t light_lightness_get;
	light_lightness_get.dst = str_to_int(argv[0]);
	light_lightness_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_lightness_get(&light_lightness_get);
	if (ret) {
		BT_LOGE("[%s] Get light lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light lightness client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lightness_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lightness_set_t light_lightness_set;
	light_lightness_set.dst = str_to_int(argv[0]);
	light_lightness_set.lightness = str_to_int(argv[1]);
	light_lightness_set.ack = str_to_int(argv[2]);
	light_lightness_set.app_key_index = str_to_int(argv[3]);
	if (argc > 4) {
		light_lightness_set.optional = 1;
		light_lightness_set.trans_time.num_steps = str_to_int(argv[4]);
		light_lightness_set.trans_time.step_resolution = str_to_int(argv[5]);
		light_lightness_set.delay = str_to_int(argv[6]);
	} else {
		light_lightness_set.optional = 0;
	}
	ret = rtk_bt_mesh_light_lightness_set(&light_lightness_set);
	if (ret) {
		BT_LOGE("[%s] Set light lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light lightness client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lightness_linear_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lightness_linear_get_t light_lightness_linear_get;
	light_lightness_linear_get.dst = str_to_int(argv[0]);
	light_lightness_linear_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_lightness_linear_get(&light_lightness_linear_get);
	if (ret) {
		BT_LOGE("[%s] Get light linear lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light lightness client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lightness_linear_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lightness_linear_set_t light_lightness_linear_set;
	light_lightness_linear_set.dst = str_to_int(argv[0]);
	light_lightness_linear_set.lightness = str_to_int(argv[1]);
	light_lightness_linear_set.ack = str_to_int(argv[2]);
	light_lightness_linear_set.app_key_index = str_to_int(argv[3]);
	if (argc > 4) {
		light_lightness_linear_set.optional = 1;
		light_lightness_linear_set.trans_time.num_steps = str_to_int(argv[4]);
		light_lightness_linear_set.trans_time.step_resolution = str_to_int(argv[5]);
		light_lightness_linear_set.delay = str_to_int(argv[6]);
	} else {
		light_lightness_linear_set.optional = 0;
	}
	ret = rtk_bt_mesh_light_lightness_linear_set(&light_lightness_linear_set);
	if (ret) {
		BT_LOGE("[%s] Set light linear lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light lightness client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lightness_last_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lightness_last_get_t light_lightness_last_get;
	light_lightness_last_get.dst = str_to_int(argv[0]);
	light_lightness_last_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_lightness_last_get(&light_lightness_last_get);
	if (ret) {
		BT_LOGE("[%s] Get light last lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light lightness client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lightness_default_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lightness_default_get_t light_lightness_default_get;
	light_lightness_default_get.dst = str_to_int(argv[0]);
	light_lightness_default_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_lightness_default_get(&light_lightness_default_get);
	if (ret) {
		BT_LOGE("[%s] Get light default lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light lightness client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lightness_default_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lightness_default_set_t light_lightness_default_set;
	light_lightness_default_set.dst = str_to_int(argv[0]);
	light_lightness_default_set.lightness = str_to_int(argv[1]);
	light_lightness_default_set.ack = str_to_int(argv[2]);
	light_lightness_default_set.app_key_index = str_to_int(argv[3]);
	ret = rtk_bt_mesh_light_lightness_default_set(&light_lightness_default_set);
	if (ret) {
		BT_LOGE("[%s] Set light default lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light lightness client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lightness_range_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lightness_range_get_t light_lightness_range_get;
	light_lightness_range_get.dst = str_to_int(argv[0]);
	light_lightness_range_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_lightness_range_get(&light_lightness_range_get);
	if (ret) {
		BT_LOGE("[%s] Get light lightness range failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light lightness client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lightness_range_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lightness_range_set_t light_lightness_range_set;
	light_lightness_range_set.dst = str_to_int(argv[0]);
	light_lightness_range_set.range_min = str_to_int(argv[1]);
	light_lightness_range_set.range_max = str_to_int(argv[2]);
	light_lightness_range_set.ack = str_to_int(argv[3]);
	light_lightness_range_set.app_key_index = str_to_int(argv[4]);
	ret = rtk_bt_mesh_light_lightness_range_set(&light_lightness_range_set);
	if (ret) {
		BT_LOGE("[%s] Set light lightness range failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light lightness client model.\r\n");
	return -1;
#endif
}

/*****************light ctl*********************/
static int atcmd_ble_mesh_light_ctl_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_ctl_get_t light_ctl_get;
	light_ctl_get.dst = str_to_int(argv[0]);
	light_ctl_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_ctl_get(&light_ctl_get);
	if (ret) {
		BT_LOGE("[%s] Get light ctl lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light CTL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_ctl_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_ctl_set_t light_ctl_set;
	light_ctl_set.dst = str_to_int(argv[0]);
	light_ctl_set.lightness = str_to_int(argv[1]);
	light_ctl_set.temperature = str_to_int(argv[2]);
	light_ctl_set.delta_uv = str_to_int(argv[3]);
	light_ctl_set.ack = str_to_int(argv[4]);
	light_ctl_set.app_key_index = str_to_int(argv[5]);
	if (argc > 6) {
		light_ctl_set.optional = 1;
		light_ctl_set.trans_time.num_steps = str_to_int(argv[6]);
		light_ctl_set.trans_time.step_resolution = str_to_int(argv[7]);
		light_ctl_set.delay = str_to_int(argv[8]);
	} else {
		light_ctl_set.optional = 0;
	}
	ret = rtk_bt_mesh_light_ctl_set(&light_ctl_set);
	if (ret) {
		BT_LOGE("[%s] Set light ctl lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light CTL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_ctl_temperature_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_ctl_temperature_get_t light_ctl_temperature_get;
	light_ctl_temperature_get.dst = str_to_int(argv[0]);
	light_ctl_temperature_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_ctl_temperature_get(&light_ctl_temperature_get);
	if (ret) {
		BT_LOGE("[%s] Get light ctl temperature failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light CTL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_ctl_temperature_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_ctl_temperature_set_t light_ctl_temperature_set = {0};
	light_ctl_temperature_set.dst = str_to_int(argv[0]);
	light_ctl_temperature_set.temperature = str_to_int(argv[1]);
	light_ctl_temperature_set.delta_uv = str_to_int(argv[2]);
	light_ctl_temperature_set.ack = str_to_int(argv[3]);
	light_ctl_temperature_set.app_key_index = str_to_int(argv[4]);
	if (argc > 5) {
		light_ctl_temperature_set.optional = 1;
		light_ctl_temperature_set.trans_time.num_steps = str_to_int(argv[5]);
		light_ctl_temperature_set.trans_time.step_resolution = str_to_int(argv[6]);
		light_ctl_temperature_set.delay = str_to_int(argv[7]);
	} else {
		light_ctl_temperature_set.optional = 0;
	}
	ret = rtk_bt_mesh_light_ctl_temperature_set(&light_ctl_temperature_set);
	if (ret) {
		BT_LOGE("[%s] Set light ctl temperature failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light CTL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_ctl_temperature_range_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_ctl_temperature_range_get_t light_ctl_temperature_range_get;
	light_ctl_temperature_range_get.dst = str_to_int(argv[0]);
	light_ctl_temperature_range_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_ctl_temperature_range_get(&light_ctl_temperature_range_get);
	if (ret) {
		BT_LOGE("[%s] Get light ctl temperature range failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light CTL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_ctl_temperature_range_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_ctl_temperature_range_set_t light_ctl_temperature_range_set;
	light_ctl_temperature_range_set.dst = str_to_int(argv[0]);
	light_ctl_temperature_range_set.range_min = str_to_int(argv[1]);
	light_ctl_temperature_range_set.range_max = str_to_int(argv[2]);
	light_ctl_temperature_range_set.ack = str_to_int(argv[3]);
	light_ctl_temperature_range_set.app_key_index = str_to_int(argv[4]);
	ret = rtk_bt_mesh_light_ctl_temperature_range_set(&light_ctl_temperature_range_set);
	if (ret) {
		BT_LOGE("[%s] Set light ctl temperature range failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light CTL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_ctl_default_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_ctl_default_get_t light_ctl_default_get;
	light_ctl_default_get.dst = str_to_int(argv[0]);
	light_ctl_default_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_ctl_default_get(&light_ctl_default_get);
	if (ret) {
		BT_LOGE("[%s] Get light ctl default lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light CTL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_ctl_default_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_ctl_default_set_t light_ctl_default_set;
	light_ctl_default_set.dst = str_to_int(argv[0]);
	light_ctl_default_set.lightness = str_to_int(argv[1]);
	light_ctl_default_set.temperature = str_to_int(argv[2]);
	light_ctl_default_set.delta_uv = str_to_int(argv[3]);
	light_ctl_default_set.ack = str_to_int(argv[4]);
	light_ctl_default_set.app_key_index = str_to_int(argv[5]);
	ret = rtk_bt_mesh_light_ctl_default_set(&light_ctl_default_set);
	if (ret) {
		BT_LOGE("[%s] Set light ctl default lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light CTL client model.\r\n");
	return -1;
#endif
}

/*************light hsl*****************/
static int atcmd_ble_mesh_light_hsl_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_hsl_get_t light_hsl_get;
	light_hsl_get.dst = str_to_int(argv[0]);
	light_hsl_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_hsl_get(&light_hsl_get);
	if (ret) {
		BT_LOGE("[%s] Get light hsl lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light HSL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_hsl_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_hsl_set_t light_hsl_set = {0};
	light_hsl_set.dst = str_to_int(argv[0]);
	light_hsl_set.lightness = str_to_int(argv[1]);
	light_hsl_set.hue = str_to_int(argv[2]);
	light_hsl_set.saturation = str_to_int(argv[3]);
	light_hsl_set.ack = str_to_int(argv[4]);
	light_hsl_set.app_key_index = str_to_int(argv[5]);
	if (argc > 6) {
		light_hsl_set.optional = 1;
		light_hsl_set.trans_time.num_steps = str_to_int(argv[6]);
		light_hsl_set.trans_time.step_resolution = str_to_int(argv[7]);
		light_hsl_set.delay = str_to_int(argv[8]);
	} else {
		light_hsl_set.optional = 0;
	}
	ret = rtk_bt_mesh_light_hsl_set(&light_hsl_set);
	if (ret) {
		BT_LOGE("[%s] Set light hsl lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light HSL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_hsl_target_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_hsl_target_get_t light_hsl_target_get;
	light_hsl_target_get.dst = str_to_int(argv[0]);
	light_hsl_target_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_hsl_target_get(&light_hsl_target_get);
	if (ret) {
		BT_LOGE("[%s] Get light hsl target lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light HSL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_hsl_hue_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_hsl_hue_get_t light_hsl_hue_get;
	light_hsl_hue_get.dst = str_to_int(argv[0]);
	light_hsl_hue_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_hsl_hue_get(&light_hsl_hue_get);
	if (ret) {
		BT_LOGE("[%s] Get light hsl hue lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light HSL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_hsl_hue_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_hsl_hue_set_t light_hsl_hue_set;
	light_hsl_hue_set.dst = str_to_int(argv[0]);
	light_hsl_hue_set.hue = str_to_int(argv[1]);
	light_hsl_hue_set.ack = str_to_int(argv[2]);
	light_hsl_hue_set.app_key_index = str_to_int(argv[3]);
	if (argc > 4) {
		light_hsl_hue_set.optional = 1;
		light_hsl_hue_set.trans_time.num_steps = str_to_int(argv[4]);
		light_hsl_hue_set.trans_time.step_resolution = str_to_int(argv[5]);
		light_hsl_hue_set.delay = str_to_int(argv[6]);
	} else {
		light_hsl_hue_set.optional = 0;
	}
	ret = rtk_bt_mesh_light_hsl_hue_set(&light_hsl_hue_set);
	if (ret) {
		BT_LOGE("[%s] Get light hsl hue lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light HSL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_hsl_saturation_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_hsl_saturation_get_t light_hsl_saturation_get;
	light_hsl_saturation_get.dst = str_to_int(argv[0]);
	light_hsl_saturation_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_hsl_saturation_get(&light_hsl_saturation_get);
	if (ret) {
		BT_LOGE("[%s] Get light hsl saturation lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light HSL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_hsl_saturation_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_hsl_saturation_set_t light_hsl_saturation_set;
	light_hsl_saturation_set.dst = str_to_int(argv[0]);
	light_hsl_saturation_set.saturation = str_to_int(argv[1]);
	light_hsl_saturation_set.ack = str_to_int(argv[2]);
	light_hsl_saturation_set.app_key_index = str_to_int(argv[3]);
	if (argc > 4) {
		light_hsl_saturation_set.optional = 1;
		light_hsl_saturation_set.trans_time.num_steps = str_to_int(argv[4]);
		light_hsl_saturation_set.trans_time.step_resolution = str_to_int(argv[5]);
		light_hsl_saturation_set.delay = str_to_int(argv[6]);
	} else {
		light_hsl_saturation_set.optional = 0;
	}
	ret = rtk_bt_mesh_light_hsl_saturation_set(&light_hsl_saturation_set);
	if (ret) {
		BT_LOGE("[%s] Get light hsl saturation lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light HSL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_hsl_default_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_hsl_default_get_t light_hsl_default_get;
	light_hsl_default_get.dst = str_to_int(argv[0]);
	light_hsl_default_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_hsl_default_get(&light_hsl_default_get);
	if (ret) {
		BT_LOGE("[%s] Get light hsl default lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light HSL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_hsl_default_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_hsl_default_set_t light_hsl_default_set = {0};
	light_hsl_default_set.dst = str_to_int(argv[0]);
	light_hsl_default_set.lightness = str_to_int(argv[1]);
	light_hsl_default_set.hue = str_to_int(argv[2]);
	light_hsl_default_set.saturation = str_to_int(argv[3]);
	light_hsl_default_set.ack = str_to_int(argv[4]);
	light_hsl_default_set.app_key_index = str_to_int(argv[5]);
	ret = rtk_bt_mesh_light_hsl_default_set(&light_hsl_default_set);
	if (ret) {
		BT_LOGE("[%s] Set light hsl default lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light HSL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_hsl_range_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_hsl_range_get_t light_hsl_range_get;
	light_hsl_range_get.dst = str_to_int(argv[0]);
	light_hsl_range_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_hsl_range_get(&light_hsl_range_get);
	if (ret) {
		BT_LOGE("[%s] Get light hsl lightness range failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light HSL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_hsl_range_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_hsl_range_set_t light_hsl_range_set = {0};
	light_hsl_range_set.dst = str_to_int(argv[0]);
	light_hsl_range_set.hue_range_min = str_to_int(argv[1]);
	light_hsl_range_set.hue_range_max = str_to_int(argv[2]);
	light_hsl_range_set.saturation_range_min = str_to_int(argv[3]);
	light_hsl_range_set.saturation_range_max = str_to_int(argv[4]);
	light_hsl_range_set.ack = str_to_int(argv[5]);
	light_hsl_range_set.app_key_index = str_to_int(argv[6]);
	ret = rtk_bt_mesh_light_hsl_range_set(&light_hsl_range_set);
	if (ret) {
		BT_LOGE("[%s] Get light hsl lightness range failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light HSL client model.\r\n");
	return -1;
#endif
}

/**************light xyl*********************/
static int atcmd_ble_mesh_light_xyl_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_xyl_get_t light_xyl_get;
	light_xyl_get.dst = str_to_int(argv[0]);
	light_xyl_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_xyl_get(&light_xyl_get);
	if (ret) {
		BT_LOGE("[%s] Get light xyl lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light XYL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_xyl_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_xyl_set_t light_xyl_set = {0};
	light_xyl_set.dst = str_to_int(argv[0]);
	light_xyl_set.xyl.xyl_lightness = str_to_int(argv[1]);
	light_xyl_set.xyl.xyl_x = str_to_int(argv[2]);
	light_xyl_set.xyl.xyl_y = str_to_int(argv[3]);
	light_xyl_set.ack = str_to_int(argv[4]);
	light_xyl_set.app_key_index = str_to_int(argv[5]);
	if (argc > 6) {
		light_xyl_set.optional = 1;
		light_xyl_set.trans_time.num_steps = str_to_int(argv[6]);
		light_xyl_set.trans_time.step_resolution = str_to_int(argv[7]);
		light_xyl_set.delay = str_to_int(argv[8]);
	} else {
		light_xyl_set.optional = 0;
	}
	ret = rtk_bt_mesh_light_xyl_set(&light_xyl_set);
	if (ret) {
		BT_LOGE("[%s] Set light xyl lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light XYL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_xyl_target_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_xyl_target_get_t light_xyl_target_get;
	light_xyl_target_get.dst = str_to_int(argv[0]);
	light_xyl_target_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_xyl_target_get(&light_xyl_target_get);
	if (ret) {
		BT_LOGE("[%s] Get light xyl target lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light XYL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_xyl_default_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_xyl_default_get_t light_xyl_default_get;
	light_xyl_default_get.dst = str_to_int(argv[0]);
	light_xyl_default_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_xyl_default_get(&light_xyl_default_get);
	if (ret) {
		BT_LOGE("[%s] Get light xyl default lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light XYL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_xyl_default_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_xyl_default_set_t light_xyl_set;
	light_xyl_set.dst = str_to_int(argv[0]);
	light_xyl_set.xyl.xyl_lightness = str_to_int(argv[1]);
	light_xyl_set.xyl.xyl_x = str_to_int(argv[2]);
	light_xyl_set.xyl.xyl_y = str_to_int(argv[3]);
	light_xyl_set.ack = str_to_int(argv[4]);
	light_xyl_set.app_key_index = str_to_int(argv[5]);
	ret = rtk_bt_mesh_light_xyl_default_set(&light_xyl_set);
	if (ret) {
		BT_LOGE("[%s] Set light xyl default lightness failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light XYL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_xyl_range_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_xyl_range_get_t light_xyl_range_get;
	light_xyl_range_get.dst = str_to_int(argv[0]);
	light_xyl_range_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_xyl_range_get(&light_xyl_range_get);
	if (ret) {
		BT_LOGE("[%s] Get light xyl lightness range failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light XYL client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_xyl_range_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_xyl_range_set_t light_xyl_range_set;
	light_xyl_range_set.dst = str_to_int(argv[0]);
	light_xyl_range_set.xyl_x_range_min = str_to_int(argv[1]);
	light_xyl_range_set.xyl_x_range_max = str_to_int(argv[2]);
	light_xyl_range_set.xyl_y_range_min = str_to_int(argv[3]);
	light_xyl_range_set.xyl_y_range_max = str_to_int(argv[4]);
	light_xyl_range_set.ack = str_to_int(argv[5]);
	light_xyl_range_set.app_key_index = str_to_int(argv[6]);
	ret = rtk_bt_mesh_light_xyl_range_set(&light_xyl_range_set);
	if (ret) {
		BT_LOGE("[%s] Set light xyl lightness range failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light XYL client model.\r\n");
	return -1;
#endif
}

/************light lc******************/
static int atcmd_ble_mesh_light_lc_mode_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lc_mode_get_t light_lc_mode_get;
	light_lc_mode_get.dst = str_to_int(argv[0]);
	light_lc_mode_get.app_key_index = str_to_int(argv[1]);
	rtk_bt_mesh_light_lc_mode_get(&light_lc_mode_get);
	if (ret) {
		BT_LOGE("[%s] Get light lc mode failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light LC client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lc_mode_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lc_mode_set_t light_lc_mode_set;
	light_lc_mode_set.dst = str_to_int(argv[0]);
	light_lc_mode_set.mode = str_to_int(argv[1]);
	light_lc_mode_set.ack = str_to_int(argv[2]);
	light_lc_mode_set.app_key_index = str_to_int(argv[3]);
	ret = rtk_bt_mesh_light_lc_mode_set(&light_lc_mode_set);
	if (ret) {
		BT_LOGE("[%s] Set light lc mode failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light LC client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lc_om_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lc_om_get_t light_lc_om_get;
	light_lc_om_get.dst = str_to_int(argv[0]);
	light_lc_om_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_lc_om_get(&light_lc_om_get);
	if (ret) {
		BT_LOGE("[%s] Get light lc om failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light LC client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lc_om_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lc_om_set_t light_lc_om_set;
	light_lc_om_set.dst = str_to_int(argv[0]);
	light_lc_om_set.mode = str_to_int(argv[1]);
	light_lc_om_set.ack = str_to_int(argv[2]);
	light_lc_om_set.app_key_index = str_to_int(argv[3]);
	ret = rtk_bt_mesh_light_lc_om_set(&light_lc_om_set);
	if (ret) {
		BT_LOGE("[%s] Set light lc om failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light LC client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lc_lightonoff_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lc_lightonoff_get_t light_lc_lightonoff_get;
	light_lc_lightonoff_get.dst = str_to_int(argv[0]);
	light_lc_lightonoff_get.app_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_light_lc_lightonoff_get(&light_lc_lightonoff_get);
	if (ret) {
		BT_LOGE("[%s] Get light lc light on off failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light LC client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lc_lightonoff_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lc_lightonoff_set_t light_lc_lightonoff_set;
	light_lc_lightonoff_set.dst = str_to_int(argv[0]);
	light_lc_lightonoff_set.light_on_off = str_to_int(argv[1]);
	light_lc_lightonoff_set.ack = str_to_int(argv[2]);
	light_lc_lightonoff_set.app_key_index = str_to_int(argv[3]);
	if (argc > 4) {
		light_lc_lightonoff_set.optional = 1;
		light_lc_lightonoff_set.trans_time.num_steps = str_to_int(argv[4]);
		light_lc_lightonoff_set.trans_time.step_resolution = str_to_int(argv[5]);
		light_lc_lightonoff_set.delay = str_to_int(argv[6]);
	} else {
		light_lc_lightonoff_set.optional = 0;
	}
	ret = rtk_bt_mesh_light_lc_lightonoff_set(&light_lc_lightonoff_set);
	if (ret) {
		BT_LOGE("[%s] Set light lc light on off failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light LC client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lc_property_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_light_lc_property_get_t light_lc_property_get;
	light_lc_property_get.dst = str_to_int(argv[0]);
	light_lc_property_get.app_key_index = str_to_int(argv[1]);
	light_lc_property_get.property_id = str_to_int(argv[2]);//cannot be 0
	rtk_bt_mesh_light_lc_property_get(&light_lc_property_get);
	if (ret) {
		BT_LOGE("[%s] Get light lc property failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support light LC client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_light_lc_property_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
	uint16_t ret = 0;
	uint16_t len;
	rtk_bt_mesh_light_lc_property_set_t light_lc_property_set;
	light_lc_property_set.dst = str_to_int(argv[0]);
	light_lc_property_set.ack = str_to_int(argv[1]);
	light_lc_property_set.app_key_index = str_to_int(argv[2]);
	light_lc_property_set.property_id = str_to_int(argv[3]);//cannot be 0
	if (argc > 4) {
		len = strlen(argv[4]);
		if (len / 2 > LIGHT_LC_PROPERTY_DATA_MAX_LEN) {
			BT_LOGE("[%s] The len %d of input data extend max value %d\r\n", __func__, len, LIGHT_LC_PROPERTY_DATA_MAX_LEN);
			return -2;
		}
		light_lc_property_set.value_len = len / 2;
		if (hexdata_str_to_array(argv[4], light_lc_property_set.pvalue, len)) {
			ret = rtk_bt_mesh_light_lc_property_set(&light_lc_property_set);
			if (ret) {
				BT_LOGE("[%s] Set light lc property failed! reason: 0x%x\r\n", __func__, ret);
				return -1;
			}
			return 0;
		} else {
			BT_LOGE("[%s] Input data format is not corrent.\r\n", __func__);
			return -3;
		}
	} else {
		light_lc_property_set.value_len = 0;
		ret = rtk_bt_mesh_light_lc_property_set(&light_lc_property_set);
		if (ret) {
			BT_LOGE("[%s] Set light lc property failed! reason: 0x%x\r\n", __func__, ret);
			return -1;
		}
		return 0;
	}
#else
	(void)argv;
	BT_LOGE("Platform not support light LC client model.\r\n");
	return -1;
#endif

}

static const cmd_table_t mesh_light_lightness_cmd_table[] = {
	{"llg",    atcmd_ble_mesh_light_lightness_get,    3, 3},
	{"lls",    atcmd_ble_mesh_light_lightness_set,    5, 8},
	{"lllg",    atcmd_ble_mesh_light_lightness_linear_get,    3, 3},
	{"llls",    atcmd_ble_mesh_light_lightness_linear_set,    5, 8},
	{"lllag",    atcmd_ble_mesh_light_lightness_last_get,    3, 3},
	{"lldg",    atcmd_ble_mesh_light_lightness_default_get,    3, 3},
	{"llds",    atcmd_ble_mesh_light_lightness_default_set,    5, 5},
	{"llrg",    atcmd_ble_mesh_light_lightness_range_get,    3, 3},
	{"llrs",    atcmd_ble_mesh_light_lightness_range_set,    6, 6},
	{NULL,},
};

int atcmd_bt_mesh_light_lightness(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_light_lightness_cmd_table, "[AT+BLEMESHLL]");
}

static const cmd_table_t mesh_light_ctl_cmd_table[] = {
	{"lcg",    atcmd_ble_mesh_light_ctl_get,    3, 3},
	{"lcs",    atcmd_ble_mesh_light_ctl_set,    7, 10},
	{"lctg",    atcmd_ble_mesh_light_ctl_temperature_get,    3, 3},
	{"lcts",    atcmd_ble_mesh_light_ctl_temperature_set,    6, 9},
	{"lctrg",    atcmd_ble_mesh_light_ctl_temperature_range_get,    3, 3},
	{"lctrs",    atcmd_ble_mesh_light_ctl_temperature_range_set,    6, 6},
	{"lcdg",    atcmd_ble_mesh_light_ctl_default_get,    3, 3},
	{"lcds",    atcmd_ble_mesh_light_ctl_default_set,    7, 7},
	{NULL,},
};

int atcmd_bt_mesh_light_ctl(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_light_ctl_cmd_table, "[AT+BLEMESHLCTL]");
}

static const cmd_table_t mesh_light_hsl_cmd_table[] = {
	{"lhg",    atcmd_ble_mesh_light_hsl_get,    3, 3},
	{"lhs",    atcmd_ble_mesh_light_hsl_set,    7, 10},
	{"lhtg",    atcmd_ble_mesh_light_hsl_target_get,    3, 3},
	{"lhhg",    atcmd_ble_mesh_light_hsl_hue_get,    3, 3},
	{"lhhs",    atcmd_ble_mesh_light_hsl_hue_set,    5, 8},
	{"lhsg",    atcmd_ble_mesh_light_hsl_saturation_get,    3, 3},
	{"lhss",    atcmd_ble_mesh_light_hsl_saturation_set,    5, 8},
	{"lhdg",    atcmd_ble_mesh_light_hsl_default_get,    3, 3},
	{"lhds",    atcmd_ble_mesh_light_hsl_default_set,    7, 7},
	{"lhrg",    atcmd_ble_mesh_light_hsl_range_get,    3, 3},
	{"lhrs",    atcmd_ble_mesh_light_hsl_range_set,    8, 8},
	{NULL,},
};

int atcmd_bt_mesh_light_hsl(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_light_hsl_cmd_table, "[AT+BLEMESHLHSL]");
}

static const cmd_table_t mesh_light_xyl_cmd_table[] = {
	{"lxg",    atcmd_ble_mesh_light_xyl_get,    3, 3},
	{"lxs",    atcmd_ble_mesh_light_xyl_set,    7, 10},
	{"lxtg",    atcmd_ble_mesh_light_xyl_target_get,    3, 3},
	{"lxdg",    atcmd_ble_mesh_light_xyl_default_get,    3, 3},
	{"lxds",    atcmd_ble_mesh_light_xyl_default_set,    7, 7},
	{"lxrg",    atcmd_ble_mesh_light_xyl_range_get,    3, 3},
	{"lxrs",    atcmd_ble_mesh_light_xyl_range_set,    8, 8},
	{NULL,},
};

int atcmd_bt_mesh_light_xyl(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_light_xyl_cmd_table, "[AT+BLEMESHLXYL]");
}

static const cmd_table_t mesh_light_lc_cmd_table[] = {
	{"llcmg",    atcmd_ble_mesh_light_lc_mode_get,    3, 3},
	{"llcms",    atcmd_ble_mesh_light_lc_mode_set,    5, 5},
	{"llcomg",    atcmd_ble_mesh_light_lc_om_get,    3, 3},
	{"llcoms",    atcmd_ble_mesh_light_lc_om_set,    5, 5},
	{"llcloog",    atcmd_ble_mesh_light_lc_lightonoff_get,    3, 3},
	{"llcloos",    atcmd_ble_mesh_light_lc_lightonoff_set,    5, 8},
	{"llcpg",    atcmd_ble_mesh_light_lc_property_get,    4, 4},
	{"llcps",    atcmd_ble_mesh_light_lc_property_set,    5, 6},
	{NULL,},
};

int atcmd_bt_mesh_light_lc(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_light_lc_cmd_table, "[AT+BLEMESHLLC]");
}
#endif // end of RTK_BLE_MESH_SUPPORT
