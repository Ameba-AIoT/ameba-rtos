#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_device.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_generic_model.h>
#include <rtk_bt_mesh_def.h>

#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
uint16_t rtk_bt_mesh_generic_level_client_get(rtk_bt_mesh_generic_level_get_t *level_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_GET, level_get,
						  sizeof(rtk_bt_mesh_generic_level_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_level_client_set(rtk_bt_mesh_generic_level_set_t *level_set)
{
	uint16_t ret;
	static uint8_t tid = 0;

	tid++;
	level_set->tid = tid;
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_SET, level_set,
						  sizeof(rtk_bt_mesh_generic_level_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_level_client_delta_set(rtk_bt_mesh_generic_level_delta_set_t *delta_set)
{
	uint16_t ret;
	static uint8_t tid = 0;

	tid++;
	delta_set->tid = tid;
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_DELTA_SET, delta_set,
						  sizeof(rtk_bt_mesh_generic_level_delta_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_level_client_move_set(rtk_bt_mesh_generic_level_move_set_t *move_set)
{
	uint16_t ret;
	static uint8_t tid = 0;

	tid++;
	move_set->tid = tid;
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_MOVE_SET, move_set,
						  sizeof(rtk_bt_mesh_generic_level_move_set_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL
uint16_t rtk_bt_mesh_generic_on_power_up_client_get(rtk_bt_mesh_generic_on_power_up_get_t *power_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_CLIENT_MODEL, RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_GET, power_get,
						  sizeof(rtk_bt_mesh_generic_on_power_up_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_on_power_up_client_set(rtk_bt_mesh_generic_on_power_up_set_t *power_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_CLIENT_MODEL, RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_SET, power_set,
						  sizeof(rtk_bt_mesh_generic_on_power_up_set_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
/******power level*******/
uint16_t rtk_bt_mesh_generic_power_level_client_get(rtk_bt_mesh_generic_power_level_get_t *power_level_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_GET, power_level_get,
						  sizeof(rtk_bt_mesh_generic_power_level_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_power_level_client_set(rtk_bt_mesh_generic_power_level_set_t *power_level_set)
{
	uint16_t ret;
	static uint8_t tid = 0;

	tid++;
	power_level_set->tid = tid;
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_SET, power_level_set,
						  sizeof(rtk_bt_mesh_generic_power_level_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_power_level_client_last_get(rtk_bt_mesh_generic_power_level_last_get_t *last_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_LAST_GET, last_get,
						  sizeof(rtk_bt_mesh_generic_power_level_last_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_power_level_client_default_get(rtk_bt_mesh_generic_power_level_default_get_t *default_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_DEFAULT_GET, default_get,
						  sizeof(rtk_bt_mesh_generic_power_level_default_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_power_level_client_default_set(rtk_bt_mesh_generic_power_level_default_set_t *default_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_DEFAULT_SET, default_set,
						  sizeof(rtk_bt_mesh_generic_power_level_default_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_power_level_client_range_get(rtk_bt_mesh_generic_power_level_range_get_t *range_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_RANGE_GET, range_get,
						  sizeof(rtk_bt_mesh_generic_power_level_range_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_power_level_client_range_set(rtk_bt_mesh_generic_power_level_range_set_t *range_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_RANGE_SET, range_set,
						  sizeof(rtk_bt_mesh_generic_power_level_range_set_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL
uint16_t rtk_bt_mesh_generic_battery_client_get(rtk_bt_mesh_generic_battery_get_t *battery_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_BATTERY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_BATTERY_CLIENT_MODEL_GET, battery_get,
						  sizeof(rtk_bt_mesh_generic_battery_get_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
uint16_t rtk_bt_mesh_generic_location_client_global_get(rtk_bt_mesh_generic_location_global_get_t *global_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_LOCATION_CLIENT_MODEL, RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_GLOBAL_GET, global_get,
						  sizeof(rtk_bt_mesh_generic_location_global_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_location_client_global_set(rtk_bt_mesh_generic_location_global_set_t *global_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_LOCATION_CLIENT_MODEL, RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_GLOBAL_SET, global_set,
						  sizeof(rtk_bt_mesh_generic_location_global_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_location_client_local_get(rtk_bt_mesh_generic_location_local_get_t *local_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_LOCATION_CLIENT_MODEL, RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_LOCAL_GET, local_get,
						  sizeof(rtk_bt_mesh_generic_location_local_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_location_client_local_set(rtk_bt_mesh_generic_location_local_set_t *local_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_LOCATION_CLIENT_MODEL, RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_LOCAL_SET, local_set,
						  sizeof(rtk_bt_mesh_generic_location_local_set_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
uint16_t rtk_bt_mesh_generic_property_client_user_properties_get(rtk_bt_mesh_generic_user_properties_get_t *user_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTIES_GET, user_get,
						  sizeof(rtk_bt_mesh_generic_user_properties_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_property_client_user_property_get(rtk_bt_mesh_generic_user_property_get_t *user_property_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTY_GET, user_property_get,
						  sizeof(rtk_bt_mesh_generic_user_property_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_property_client_user_property_set(rtk_bt_mesh_generic_user_property_set_t *user_property_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTY_SET, user_property_set,
						  sizeof(rtk_bt_mesh_generic_user_property_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_property_client_admin_properties_get(rtk_bt_mesh_generic_admin_properties_get_t *admin_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTIES_GET, admin_get,
						  sizeof(rtk_bt_mesh_generic_admin_properties_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_property_client_admin_property_get(rtk_bt_mesh_generic_admin_property_get_t *admin_property_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTY_GET, admin_property_get,
						  sizeof(rtk_bt_mesh_generic_admin_property_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_property_client_admin_property_set(rtk_bt_mesh_generic_admin_property_set_t *admin_property_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTY_SET, admin_property_set,
						  sizeof(rtk_bt_mesh_generic_admin_property_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_property_client_manu_properties_get(rtk_bt_mesh_generic_manu_properties_get_t *manu_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTIES_GET, manu_get,
						  sizeof(rtk_bt_mesh_generic_manu_properties_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_property_client_manu_property_get(rtk_bt_mesh_generic_manu_property_get_t *manu_property_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTY_GET, manu_property_get,
						  sizeof(rtk_bt_mesh_generic_manu_property_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_property_client_manu_property_set(rtk_bt_mesh_generic_manu_property_set_t *manu_property_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTY_SET, manu_property_set,
						  sizeof(rtk_bt_mesh_generic_manu_property_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_property_client_client_properties_get(rtk_bt_mesh_generic_client_properties_get_t *client_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_CLIENT_PROPERTY_GET, client_get,
						  sizeof(rtk_bt_mesh_generic_client_properties_get_t));

	return ret;
}
#endif // BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL
uint16_t rtk_bt_mesh_generic_property_client_user_db_set(rtk_bt_mesh_generic_property_server_db_set_t *user_db_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_ACT_DB_SET, user_db_set,
						  sizeof(rtk_bt_mesh_generic_property_server_db_set_t));

	return ret;
}
#endif // end of BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL
uint16_t rtk_bt_mesh_generic_property_client_admin_db_set(rtk_bt_mesh_generic_property_server_db_set_t *admin_db_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_ACT_DB_SET, admin_db_set,
						  sizeof(rtk_bt_mesh_generic_property_server_db_set_t));

	return ret;
}
#endif // end of BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL
uint16_t rtk_bt_mesh_generic_property_client_manu_db_set(rtk_bt_mesh_generic_property_server_db_set_t *manu_db_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_ACT_DB_SET, manu_db_set,
						  sizeof(rtk_bt_mesh_generic_property_server_db_set_t));

	return ret;
}
#endif // end of BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL
#endif // end of RTK_BLE_MESH_SUPPORT