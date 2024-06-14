#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_device.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_sensor_model.h>
#include <rtk_bt_mesh_def.h>

#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
uint16_t rtk_bt_mesh_sensor_descriptor_get(rtk_bt_mesh_sensor_descriptor_get_t *sensor_descriptor_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_DESCRIPTOR_CLIENT_MODEL_GET, sensor_descriptor_get,
						  sizeof(rtk_bt_mesh_sensor_descriptor_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_sensor_cadence_get(rtk_bt_mesh_sensor_cadence_get_t *sensor_cadence_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_CADENCE_CLIENT_MODEL_GET, sensor_cadence_get,
						  sizeof(rtk_bt_mesh_sensor_cadence_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_sensor_cadence_set(rtk_bt_mesh_sensor_cadence_set_t *sensor_cadence_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_CADENCE_CLIENT_MODEL_SET, sensor_cadence_set,
						  sizeof(rtk_bt_mesh_sensor_cadence_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_sensor_settings_get(rtk_bt_mesh_sensor_settings_get_t *sensor_settings_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_SETTINGS_CLIENT_MODEL_GET, sensor_settings_get,
						  sizeof(rtk_bt_mesh_sensor_settings_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_sensor_setting_get(rtk_bt_mesh_sensor_setting_get_t *sensor_setting_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_SETTING_CLIENT_MODEL_GET, sensor_setting_get,
						  sizeof(rtk_bt_mesh_sensor_setting_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_sensor_setting_set(rtk_bt_mesh_sensor_setting_set_t *sensor_setting_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_SETTING_CLIENT_MODEL_SET, sensor_setting_set,
						  sizeof(rtk_bt_mesh_sensor_setting_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_sensor_get(rtk_bt_mesh_sensor_get_t *sensor_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_CLIENT_MODEL_GET, sensor_get,
						  sizeof(rtk_bt_mesh_sensor_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_sensor_column_get(rtk_bt_mesh_sensor_column_get_t *sensor_column_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_COLUMN_CLIENT_MODEL_GET, sensor_column_get,
						  sizeof(rtk_bt_mesh_sensor_column_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_sensor_series_get(rtk_bt_mesh_sensor_series_get_t *sensor_series_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_SERIES_CLIENT_MODEL_GET, sensor_series_get,
						  sizeof(rtk_bt_mesh_sensor_series_get_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_SENSOR_CLIENT_MODEL

#if defined(BT_MESH_ENABLE_SENSOR_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SERVER_MODEL
uint16_t rtk_bt_mesh_sensor_server_db_set(rtk_bt_mesh_sensor_server_db_set_t *sensor_db_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL, RTK_BT_MESH_SENSOR_SERVER_MODEL_ACT_DB_SET, sensor_db_set,
						  sizeof(rtk_bt_mesh_sensor_server_db_set_t));

	return ret;
}
#endif // BT_MESH_ENABLE_SENSOR_SERVER_MODEL

#if defined(BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL
uint16_t rtk_bt_mesh_sensor_setup_server_db_set(rtk_bt_mesh_sensor_server_db_set_t *sensor_db_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_SENSOR_SETUP_SERVER_MODEL, RTK_BT_MESH_SENSOR_SETUP_SERVER_MODEL_ACT_DB_SET, sensor_db_set,
						  sizeof(rtk_bt_mesh_sensor_server_db_set_t));

	return ret;
}
#endif // BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL

#endif // end of RTK_BLE_MESH_SUPPORT