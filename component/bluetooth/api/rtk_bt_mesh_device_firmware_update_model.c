/**
 * @file      rtk_bt_mesh_device_firmware_update_model.c
 * @brief     Bluetooth LE MESH device firmware update model api function definition
 * @copyright Copyright (c) 2024. Realtek Semiconductor Corporation. All rights reserved.
 */

#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_mesh_device_firmware_update_model.h>
#include <rtk_bt_device.h>
#include <rtk_bt_common.h>
#include <rtk_bt_def.h>
#include <rtk_bt_mesh_def.h>

#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE
uint16_t rtk_bt_mesh_dfu_initiator_upload_start(rtk_bt_mesh_dfu_initiator_upload_start_t *param)
{
	uint16_t ret;

	BT_LOGD("[%s] Enter.\r\n", __func__);
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DFU_INITIATOR_MODEL, RTK_BT_MESH_DFU_ACT_INITIATOR_UPLOAD_START, param,
						  sizeof(rtk_bt_mesh_dfu_initiator_upload_start_t));

	return ret;
}

uint16_t rtk_bt_mesh_dfu_initiator_receivers_add(rtk_bt_mesh_dfu_initiator_dist_recv_add_t *param)
{
	uint16_t ret;

	BT_LOGD("[%s] Enter.\r\n", __func__);
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DFU_INITIATOR_MODEL, RTK_BT_MESH_DFU_ACT_INITIATOR_RECEIVERS_ADD, param,
						  sizeof(rtk_bt_mesh_dfu_initiator_dist_recv_add_t));

	return ret;
}

uint16_t rtk_bt_mesh_dfu_initiator_receivers_delete_all(rtk_bt_mesh_dfu_initiator_dist_recv_del_all_t *param)
{
	uint16_t ret;

	BT_LOGD("[%s] Enter.\r\n", __func__);
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DFU_INITIATOR_MODEL, RTK_BT_MESH_DFU_ACT_INITIATOR_RECEIVERS_DEL_ALL, param,
						  sizeof(rtk_bt_mesh_dfu_initiator_dist_recv_del_all_t));

	return ret;
}

uint16_t rtk_bt_mesh_dfu_initiator_distribute_start(rtk_bt_mesh_dfu_initiator_fw_dist_start_param_t *param)
{
	uint16_t ret;

	BT_LOGD("[%s] Enter.\r\n", __func__);
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DFU_INITIATOR_MODEL, RTK_BT_MESH_DFU_ACT_INITIATOR_DISTRIBUTE_START, param,
						  sizeof(rtk_bt_mesh_dfu_initiator_fw_dist_start_param_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_DFU_INITIATOR_ROLE

#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
uint16_t rtk_bt_mesh_dfu_standalone_updater_set_param(rtk_bt_mesh_dfu_standalone_updater_set_param_t *param)
{
	uint16_t ret;

	BT_LOGD("[%s] Enter.\r\n", __func__);
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL, RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_SET_PARAM, param,
						  sizeof(rtk_bt_mesh_dfu_standalone_updater_set_param_t));

	return ret;
}

uint16_t rtk_bt_mesh_dfu_standalone_updater_add_receiver(rtk_bt_mesh_dfu_standalone_updater_add_receiver_param_t *param)
{
	uint16_t ret;

	BT_LOGD("[%s] Enter.\r\n", __func__);
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL, RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_ADD_RECEIVER, param,
						  sizeof(rtk_bt_mesh_dfu_standalone_updater_add_receiver_param_t));

	return ret;
}

uint16_t rtk_bt_mesh_dfu_standalone_updater_start(rtk_bt_mesh_dfu_standalone_update_start_t *param)
{
	uint16_t ret;

	BT_LOGD("[%s] Enter.\r\n", __func__);
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL, RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_START, param,
						  sizeof(rtk_bt_mesh_dfu_standalone_update_start_t));

	return ret;
}
#endif  // BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE

#endif  // RTK_BLE_MESH_SUPPORT
