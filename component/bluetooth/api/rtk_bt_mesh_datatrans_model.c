#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_device.h>
#include <rtk_bt_common.h>
#include <rtk_bt_def.h>
#include <rtk_bt_mesh_datatrans_model.h>
#include <rtk_bt_mesh_def.h>

#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
uint16_t rtk_bt_mesh_datatrans_model_write(rtk_bt_mesh_datatrans_client_write_api_t *client_write)
{
	uint16_t ret;

	if (!client_write) {
		return RTK_BT_ERR_POINTER_INVALID;
	}
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DATATRANS_MODEL, RTK_BT_MESH_DATATRANS_MODEL_ACT_WRITE, client_write,
						  sizeof(rtk_bt_mesh_datatrans_client_write_api_t));

	return ret;
}

uint16_t rtk_bt_mesh_datatrans_model_read(rtk_bt_mesh_datatrans_client_read_api_t *client_read)
{
	int ret;

	if (!client_read) {
		return RTK_BT_ERR_POINTER_INVALID;
	}
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DATATRANS_MODEL, RTK_BT_MESH_DATATRANS_MODEL_ACT_READ, client_read, sizeof(rtk_bt_mesh_datatrans_client_read_api_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_DATATRANS_MODEL
#endif // end of RTK_BLE_MESH_SUPPORT
