#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_device.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_generic_default_transition_time.h>
#include <rtk_bt_mesh_def.h>

#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL
uint16_t rtk_bt_mesh_generic_default_transition_time_client_get(rtk_bt_mesh_generic_default_transition_time_get_t *time_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL, RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_GET, time_get,
						  sizeof(rtk_bt_mesh_generic_default_transition_time_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_generic_default_transition_time_client_set(rtk_bt_mesh_generic_default_transition_time_set_t *time_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL, RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_SET, time_set,
						  sizeof(rtk_bt_mesh_generic_default_transition_time_set_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL
#endif // end of RTK_BLE_MESH_SUPPORT