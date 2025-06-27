/**
 * @file      rtk_bt_device_firmware_update_interface.h
 * @brief     Bluetooth device firmware update interface data struct definition
 * @copyright Copyright (c) 2024. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _RTK_BT_DEVICE_FIRMWARE_UPDATE_INTERFACE_H
#define _RTK_BT_DEVICE_FIRMWARE_UPDATE_INTERFACE_H

#include <bt_debug.h>
#include <rtk_bt_mesh_def.h>

#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE
struct mesh_dfu_initiator_opts {
	bool (*mesh_dfu_initiator_load_data_flash)(uint32_t addr, uint32_t len, uint8_t *pout);
};
#endif  // BT_MESH_ENABLE_DFU_INITIATOR_ROLE

#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
struct mesh_dfu_standalone_updater_opts {
	bool (*mesh_dfu_standalone_updater_load_data_flash)(uint32_t addr, uint32_t len, uint8_t *pout);
};
#endif  // BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE

#if defined(BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE) && BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE
struct mesh_dfu_distributor_opts {
	bool (*mesh_dfu_distributor_load_data_flash)(uint32_t addr, uint32_t len, uint8_t *pout);
	bool (*mesh_dfu_distributor_block_data)(uint16_t block_num, uint8_t *pdata, uint32_t data_len);
};
#endif  // BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE

#if defined(BT_MESH_ENABLE_DFU_TARGET_ROLE) && BT_MESH_ENABLE_DFU_TARGET_ROLE
struct mesh_dfu_fw_update_server_opts {
	bool (*mesh_dfu_fw_update_server_start)(uint8_t fw_image_idx);
	bool (*mesh_dfu_fw_update_server_apply)(void);
	bool (*mesh_dfu_fw_update_server_block_data)(uint16_t block_num, uint8_t *pdata, uint16_t data_len);
	bool (*mesh_dfu_fw_update_server_verify)(void);
};
#endif  // BT_MESH_ENABLE_DFU_TARGET_ROLE

bool rtk_bt_dfu_load_data_from_flash(uint32_t addr, uint32_t len, uint8_t *pout);

bool rtk_bt_dfu_initiator_resource_init(void);

bool rtk_bt_dfu_standalone_updater_resource_init(void);

bool rtk_bt_dfu_distributor_resource_init(void);

bool rtk_bt_dfu_distributor_block_data(uint16_t block_num, uint8_t *pdata, uint32_t data_len);

bool rtk_bt_dfu_target_resource_init(void);

bool rtk_bt_dfu_updater_server_start(uint8_t fw_image_idx);

bool rtk_bt_dfu_updater_server_apply(void);

bool rtk_bt_dfu_updater_server_block_data(uint16_t block_num, uint8_t *pdata, uint16_t data_len);

bool rtk_bt_dfu_updater_server_verify(void);

#endif  // _RTK_BT_DEVICE_FIRMWARE_UPDATE_INTERFACE_H
