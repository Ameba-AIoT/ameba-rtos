#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_device.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_time_model.h>
#include <rtk_bt_mesh_def.h>


#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
uint16_t rtk_bt_mesh_time_get(rtk_bt_mesh_time_get_t *time_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, RTK_BT_MESH_TIME_CLIENT_MODEL_GET, time_get,
						  sizeof(rtk_bt_mesh_time_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_time_zone_get(rtk_bt_mesh_time_zone_get_t *time_zone_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, RTK_BT_MESH_TIME_CLIENT_MODEL_ZONE_GET, time_zone_get,
						  sizeof(rtk_bt_mesh_time_zone_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_time_tai_utc_delta_get(rtk_bt_mesh_time_tai_utc_delta_get_t *time_delta_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, RTK_BT_MESH_TIME_CLIENT_MODEL_TAI_UTC_DELTA_GET, time_delta_get,
						  sizeof(rtk_bt_mesh_time_tai_utc_delta_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_time_role_get(rtk_bt_mesh_time_role_get_t *time_role_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, RTK_BT_MESH_TIME_CLIENT_MODEL_ROLE_GET, time_role_get,
						  sizeof(rtk_bt_mesh_time_role_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_time_set(rtk_bt_mesh_time_set_t *time_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, RTK_BT_MESH_TIME_CLIENT_MODEL_SET, time_set,
						  sizeof(rtk_bt_mesh_time_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_time_zone_set(rtk_bt_mesh_time_zone_set_t *time_zone_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, RTK_BT_MESH_TIME_CLIENT_MODEL_ZONE_SET, time_zone_set,
						  sizeof(rtk_bt_mesh_time_zone_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_time_tai_utc_delta_set(rtk_bt_mesh_time_tai_utc_delta_set_t *time_delta_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, RTK_BT_MESH_TIME_CLIENT_MODEL_TAI_UTC_DELTA_SET, time_delta_set,
						  sizeof(rtk_bt_mesh_time_tai_utc_delta_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_time_role_set(rtk_bt_mesh_time_role_set_t *time_role_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, RTK_BT_MESH_TIME_CLIENT_MODEL_ROLE_SET, time_role_set,
						  sizeof(rtk_bt_mesh_time_role_set_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_TIME_CLIENT_MODEL
#endif // end of RTK_BLE_MESH_SUPPORT