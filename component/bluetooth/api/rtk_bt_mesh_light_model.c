#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_device.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_light_model.h>
#include <rtk_bt_mesh_def.h>


#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
/*******************light lightness client model***************************/
uint16_t rtk_bt_mesh_light_lightness_get(rtk_bt_mesh_light_lightness_get_t *lightness_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_GET, lightness_get,
						  sizeof(rtk_bt_mesh_light_lightness_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lightness_set(rtk_bt_mesh_light_lightness_set_t *lightness_set)
{
	uint16_t ret;
	static uint8_t tid = 0;
	tid++;
	lightness_set->tid = tid;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_SET, lightness_set,
						  sizeof(rtk_bt_mesh_light_lightness_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lightness_linear_get(rtk_bt_mesh_light_lightness_linear_get_t *lightness_linear_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_CLIENT_MODEL_GET, lightness_linear_get,
						  sizeof(rtk_bt_mesh_light_lightness_linear_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lightness_linear_set(rtk_bt_mesh_light_lightness_linear_set_t *lightness_linear_set)
{
	uint16_t ret;
	static uint8_t tid = 0;
	tid++;
	lightness_linear_set->tid = tid;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_CLIENT_MODEL_SET, lightness_linear_set,
						  sizeof(rtk_bt_mesh_light_lightness_linear_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lightness_last_get(rtk_bt_mesh_light_lightness_last_get_t *lightness_last_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_CLIENT_MODEL_GET, lightness_last_get,
						  sizeof(rtk_bt_mesh_light_lightness_last_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lightness_default_get(rtk_bt_mesh_light_lightness_default_get_t *lightness_default_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_CLIENT_MODEL_GET, lightness_default_get,
						  sizeof(rtk_bt_mesh_light_lightness_default_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lightness_default_set(rtk_bt_mesh_light_lightness_default_set_t *lightness_default_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_CLIENT_MODEL_SET, lightness_default_set,
						  sizeof(rtk_bt_mesh_light_lightness_default_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lightness_range_get(rtk_bt_mesh_light_lightness_range_get_t *lightness_range_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_CLIENT_MODEL_GET, lightness_range_get,
						  sizeof(rtk_bt_mesh_light_lightness_range_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lightness_range_set(rtk_bt_mesh_light_lightness_range_set_t *lightness_range_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_CLIENT_MODEL_SET, lightness_range_set,
						  sizeof(rtk_bt_mesh_light_lightness_range_set_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL



#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
/***********************light ctl client model***********************************/

uint16_t rtk_bt_mesh_light_ctl_get(rtk_bt_mesh_light_ctl_get_t *ctl_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_GET, ctl_get,
						  sizeof(rtk_bt_mesh_light_ctl_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_ctl_set(rtk_bt_mesh_light_ctl_set_t *ctl_set)
{
	uint16_t ret;
	static uint8_t tid = 0;
	tid++;
	ctl_set->tid = tid;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_SET, ctl_set,
						  sizeof(rtk_bt_mesh_light_ctl_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_ctl_temperature_get(rtk_bt_mesh_light_ctl_temperature_get_t *ctl_temperature_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_GET, ctl_temperature_get,
						  sizeof(rtk_bt_mesh_light_ctl_temperature_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_ctl_temperature_set(rtk_bt_mesh_light_ctl_temperature_set_t *ctl_temperature_set)
{
	uint16_t ret;
	static uint8_t tid = 0;
	tid++;
	ctl_temperature_set->tid = tid;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_SET, ctl_temperature_set,
						  sizeof(rtk_bt_mesh_light_ctl_temperature_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_ctl_temperature_range_get(rtk_bt_mesh_light_ctl_temperature_range_get_t *ctl_temperature_range_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_RANGE_GET, ctl_temperature_range_get,
						  sizeof(rtk_bt_mesh_light_ctl_temperature_range_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_ctl_temperature_range_set(rtk_bt_mesh_light_ctl_temperature_range_set_t *ctl_temperature_range_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_RANGE_SET, ctl_temperature_range_set,
						  sizeof(rtk_bt_mesh_light_ctl_temperature_range_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_ctl_default_get(rtk_bt_mesh_light_ctl_default_get_t *ctl_default_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_DEFAULT_GET, ctl_default_get,
						  sizeof(rtk_bt_mesh_light_ctl_default_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_ctl_default_set(rtk_bt_mesh_light_ctl_default_set_t *ctl_default_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_DEFAULT_SET, ctl_default_set,
						  sizeof(rtk_bt_mesh_light_ctl_default_set_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL




#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
/*************light hsl client model**********************/


uint16_t rtk_bt_mesh_light_hsl_get(rtk_bt_mesh_light_hsl_get_t *hsl_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_GET, hsl_get,
						  sizeof(rtk_bt_mesh_light_hsl_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_hsl_set(rtk_bt_mesh_light_hsl_set_t *hsl_set)
{
	uint16_t ret;
	static uint8_t tid = 0;
	tid++;
	hsl_set->tid = tid;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_SET, hsl_set,
						  sizeof(rtk_bt_mesh_light_hsl_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_hsl_target_get(rtk_bt_mesh_light_hsl_target_get_t *hsl_target_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_TARGET_CLIENT_MODEL_GET, hsl_target_get,
						  sizeof(rtk_bt_mesh_light_hsl_target_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_hsl_hue_get(rtk_bt_mesh_light_hsl_hue_get_t *hsl_hue_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_HUE_CLIENT_MODEL_GET, hsl_hue_get,
						  sizeof(rtk_bt_mesh_light_hsl_hue_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_hsl_hue_set(rtk_bt_mesh_light_hsl_hue_set_t *hsl_hue_set)
{
	uint16_t ret;
	static uint8_t tid = 0;
	tid++;
	hsl_hue_set->tid = tid;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_HUE_CLIENT_MODEL_SET, hsl_hue_set,
						  sizeof(rtk_bt_mesh_light_hsl_hue_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_hsl_saturation_get(rtk_bt_mesh_light_hsl_saturation_get_t *hsl_saturation_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_SATURATION_CLIENT_MODEL_GET, hsl_saturation_get,
						  sizeof(rtk_bt_mesh_light_hsl_saturation_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_hsl_saturation_set(rtk_bt_mesh_light_hsl_saturation_set_t *hsl_saturation_set)
{
	uint16_t ret;
	static uint8_t tid = 0;
	tid++;
	hsl_saturation_set->tid = tid;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_SATURATION_CLIENT_MODEL_SET, hsl_saturation_set,
						  sizeof(rtk_bt_mesh_light_hsl_saturation_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_hsl_default_get(rtk_bt_mesh_light_hsl_default_get_t *hsl_default_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_DEFAULT_CLIENT_MODEL_GET, hsl_default_get,
						  sizeof(rtk_bt_mesh_light_hsl_default_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_hsl_default_set(rtk_bt_mesh_light_hsl_default_set_t *hsl_default_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_DEFAULT_CLIENT_MODEL_SET, hsl_default_set,
						  sizeof(rtk_bt_mesh_light_hsl_default_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_hsl_range_get(rtk_bt_mesh_light_hsl_range_get_t *hsl_range_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_RANGE_CLIENT_MODEL_GET, hsl_range_get,
						  sizeof(rtk_bt_mesh_light_hsl_range_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_hsl_range_set(rtk_bt_mesh_light_hsl_range_set_t *hsl_range_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_RANGE_CLIENT_MODEL_SET, hsl_range_set,
						  sizeof(rtk_bt_mesh_light_hsl_range_set_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL




#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
/*************light xyl client model**********************/


uint16_t rtk_bt_mesh_light_xyl_get(rtk_bt_mesh_light_xyl_get_t *xyl_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_GET, xyl_get,
						  sizeof(rtk_bt_mesh_light_xyl_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_xyl_set(rtk_bt_mesh_light_xyl_set_t *xyl_set)
{
	uint16_t ret;
	static uint8_t tid = 0;
	tid++;
	xyl_set->tid = tid;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_SET, xyl_set,
						  sizeof(rtk_bt_mesh_light_xyl_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_xyl_target_get(rtk_bt_mesh_light_xyl_target_get_t *xyl_target_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_XYL_TARGET_CLIENT_MODEL_GET, xyl_target_get,
						  sizeof(rtk_bt_mesh_light_xyl_target_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_xyl_default_get(rtk_bt_mesh_light_xyl_default_get_t *xyl_default_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_XYL_DEFAULT_CLIENT_MODEL_GET, xyl_default_get,
						  sizeof(rtk_bt_mesh_light_xyl_default_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_xyl_default_set(rtk_bt_mesh_light_xyl_default_set_t *xyl_default_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_XYL_DEFAULT_CLIENT_MODEL_SET, xyl_default_set,
						  sizeof(rtk_bt_mesh_light_xyl_default_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_xyl_range_get(rtk_bt_mesh_light_xyl_range_get_t *xyl_range_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_XYL_RANGE_CLIENT_MODEL_GET, xyl_range_get,
						  sizeof(rtk_bt_mesh_light_xyl_range_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_xyl_range_set(rtk_bt_mesh_light_xyl_range_set_t *xyl_range_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_XYL_RANGE_CLIENT_MODEL_SET, xyl_range_set,
						  sizeof(rtk_bt_mesh_light_xyl_range_set_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL



#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
/*******************light lc client model**********************************/


uint16_t rtk_bt_mesh_light_lc_mode_get(rtk_bt_mesh_light_lc_mode_get_t *lc_mode_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LC_MODE_CLIENT_MODEL_GET, lc_mode_get,
						  sizeof(rtk_bt_mesh_light_lc_mode_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lc_mode_set(rtk_bt_mesh_light_lc_mode_set_t *lc_mode_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LC_MODE_CLIENT_MODEL_SET, lc_mode_set,
						  sizeof(rtk_bt_mesh_light_lc_mode_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lc_om_get(rtk_bt_mesh_light_lc_om_get_t *lc_om_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LC_OM_CLIENT_MODEL_GET, lc_om_get,
						  sizeof(rtk_bt_mesh_light_lc_om_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lc_om_set(rtk_bt_mesh_light_lc_om_set_t *lc_om_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LC_OM_CLIENT_MODEL_SET, lc_om_set,
						  sizeof(rtk_bt_mesh_light_lc_om_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lc_lightonoff_get(rtk_bt_mesh_light_lc_lightonoff_get_t *lc_lightonoff_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LC_LIGHTONOFF_CLIENT_MODEL_GET, lc_lightonoff_get,
						  sizeof(rtk_bt_mesh_light_lc_lightonoff_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lc_lightonoff_set(rtk_bt_mesh_light_lc_lightonoff_set_t *lc_lightonoff_set)
{
	uint16_t ret;
	static uint8_t tid = 0;
	tid++;
	lc_lightonoff_set->tid = tid;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LC_LIGHTONOFF_CLIENT_MODEL_SET, lc_lightonoff_set,
						  sizeof(rtk_bt_mesh_light_lc_lightonoff_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lc_property_get(rtk_bt_mesh_light_lc_property_get_t *lc_property_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LC_PROPERTY_CLIENT_MODEL_GET, lc_property_get,
						  sizeof(rtk_bt_mesh_light_lc_property_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_light_lc_property_set(rtk_bt_mesh_light_lc_property_set_t *lc_property_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LC_PROPERTY_CLIENT_MODEL_SET, lc_property_set,
						  sizeof(rtk_bt_mesh_light_lc_property_set_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL

#endif // end of RTK_BLE_MESH_SUPPORT
