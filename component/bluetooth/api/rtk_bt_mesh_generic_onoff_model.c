#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_device.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_generic_onoff_model.h>
#include <rtk_bt_mesh_def.h>

#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL
uint16_t rtk_bt_mesh_generic_onoff_client_set(rtk_bt_mesh_generic_onoff_client_act_set_t *onoff_set)
{
	uint16_t ret;
	static uint8_t tid = 0;
	tid++;
	onoff_set->tid = tid;
	if (!rtk_bt_mesh_is_enable()) {
		return RTK_BT_ERR_NOT_READY;
	}
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_ONOFF_CLIENT_MODEL, RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_ACT_SET, onoff_set,
						  sizeof(rtk_bt_mesh_generic_onoff_client_act_set_t));
	if (ret & RTK_BT_STACK_MESH_ERROR_FLAG) {
		ret = ret & 0xff;
	} else {
		printf("other error:0x%x\r\n", ret);
	}
	return ret;
}

uint16_t rtk_bt_mesh_generic_onoff_client_get(rtk_bt_mesh_generic_onoff_client_act_get_t *onoff_get)
{
	uint16_t ret;
	if (!rtk_bt_mesh_is_enable()) {
		return RTK_BT_ERR_NOT_READY;
	}
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_GENERIC_ONOFF_CLIENT_MODEL, RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_ACT_GET, onoff_get,
						  sizeof(rtk_bt_mesh_generic_onoff_client_act_get_t));
	if (ret & RTK_BT_STACK_MESH_ERROR_FLAG) {
		ret = ret & 0xff;
	} else {
		printf("other error:0x%x\r\n", ret);
	}
	return ret;
}
#endif // end of BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL
#endif // end of RTK_BLE_MESH_SUPPORT
