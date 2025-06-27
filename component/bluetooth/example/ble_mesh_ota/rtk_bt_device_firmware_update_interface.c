/**
 * @file      rtk_bt_device_firmware_update_interface.c
 * @brief     Bluetooth device firmware update interface relate api function definition
 * @copyright Copyright (c) 2024. Realtek Semiconductor Corporation. All rights reserved.
 */

#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include <rtk_bt_device_firmware_update_interface.h>

#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE
static struct mesh_dfu_initiator_opts dfu_initiator_if = {0};

bool __attribute__((weak)) mesh_dfu_initiator_resource_init(struct mesh_dfu_initiator_opts *p_if)
{
	(void)p_if;
	BT_LOGE("[%s][%d] Weak function fail\r\n", __func__, __LINE__);
	return false;
}

bool rtk_bt_dfu_initiator_resource_init(void)
{
	BT_LOGD("[%s] Enter.\r\n", __func__);
	memset(&dfu_initiator_if, 0, sizeof(struct mesh_dfu_initiator_opts));
	if (!mesh_dfu_initiator_resource_init(&dfu_initiator_if)) {
		BT_LOGE("[%s] The current platform do not support mesh dfu.\r\n", __func__);
		return false;
	} else {
		return true;
	}
}

#endif  // BT_MESH_ENABLE_DFU_INITIATOR_ROLE

#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
static struct mesh_dfu_standalone_updater_opts dfu_standalone_updater_if = {0};

bool __attribute__((weak)) mesh_dfu_standalone_updater_resource_init(struct mesh_dfu_standalone_updater_opts *p_if)
{
	(void)p_if;
	BT_LOGE("[%s][%d] Weak function fail\r\n", __func__, __LINE__);
	return false;
}

bool rtk_bt_dfu_standalone_updater_resource_init(void)
{
	BT_LOGD("[%s] Enter.\r\n", __func__);
	memset(&dfu_standalone_updater_if, 0, sizeof(struct mesh_dfu_standalone_updater_opts));
	if (!mesh_dfu_standalone_updater_resource_init(&dfu_standalone_updater_if)) {
		BT_LOGE("[%s] The current platform do not support mesh dfu.\r\n", __func__);
		return false;
	} else {
		return true;
	}
}
#endif  // BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE

#if defined(BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE) && BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE
static struct mesh_dfu_distributor_opts dfu_distributor_if = {0};

bool __attribute__((weak)) mesh_dfu_distributor_resource_init(struct mesh_dfu_distributor_opts *p_if)
{
	(void)p_if;
	BT_LOGE("[%s][%d] Weak function fail\r\n", __func__, __LINE__);
	return false;
}

bool rtk_bt_dfu_distributor_resource_init(void)
{
	BT_LOGD("[%s] Enter.\r\n", __func__);
	memset(&dfu_distributor_if, 0, sizeof(struct mesh_dfu_distributor_opts));
	if (!mesh_dfu_distributor_resource_init(&dfu_distributor_if)) {
		BT_LOGE("[%s] The current platform do not support mesh dfu.\r\n", __func__);
		return false;
	} else {
		return true;
	}
}

bool rtk_bt_dfu_distributor_block_data(uint16_t block_num, uint8_t *pdata, uint32_t data_len)
{
	if (dfu_distributor_if.mesh_dfu_distributor_block_data) {
		return dfu_distributor_if.mesh_dfu_distributor_block_data(block_num, pdata, data_len);
	} else {
		BT_LOGE("[%s] The current platform do not support mesh dfu.\r\n", __func__);
		return false;
	}
}

#endif  // BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE

bool rtk_bt_dfu_load_data_from_flash(uint32_t addr, uint32_t len, uint8_t *pout)
{
	bool ret = false;

#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE
	if (dfu_initiator_if.mesh_dfu_initiator_load_data_flash) {
		ret = dfu_initiator_if.mesh_dfu_initiator_load_data_flash(addr, len, pout);
	} else {
		BT_LOGE("[%s] The current platform do not support mesh dfu.\r\n", __func__);
	}
#elif defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
	if (dfu_standalone_updater_if.mesh_dfu_standalone_updater_load_data_flash) {
		ret = dfu_standalone_updater_if.mesh_dfu_standalone_updater_load_data_flash(addr, len, pout);
	} else {
		BT_LOGE("[%s] The current platform do not support mesh dfu.\r\n", __func__);
	}
#elif defined(BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE) && BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE
	if (dfu_distributor_if.mesh_dfu_distributor_load_data_flash) {
		ret = dfu_distributor_if.mesh_dfu_distributor_load_data_flash(addr, len, pout);
	} else {
		BT_LOGE("[%s] The current platform do not support mesh dfu.\r\n", __func__);
	}
#else
	(void)addr;
	(void)len;
	(void)pout;
#endif

	return ret;
}

#if defined(BT_MESH_ENABLE_DFU_TARGET_ROLE) && BT_MESH_ENABLE_DFU_TARGET_ROLE
static struct mesh_dfu_fw_update_server_opts dfu_update_server_if = {0};

bool __attribute__((weak)) mesh_dfu_fw_updater_server_resource_init(struct mesh_dfu_fw_update_server_opts *p_if)
{
	(void)p_if;
	BT_LOGE("[%s][%d] Weak function fail\r\n", __func__, __LINE__);
	return false;
}

bool rtk_bt_dfu_target_resource_init(void)
{
	memset(&dfu_update_server_if, 0, sizeof(struct mesh_dfu_fw_update_server_opts));
	if (!mesh_dfu_fw_updater_server_resource_init(&dfu_update_server_if)) {
		BT_LOGE("[%s] The current platform do not support mesh dfu.\r\n", __func__);
		return false;
	} else {
		return true;
	}
}

bool rtk_bt_dfu_updater_server_start(uint8_t fw_image_idx)
{
	bool ret = false;

	if (dfu_update_server_if.mesh_dfu_fw_update_server_start) {
		ret = dfu_update_server_if.mesh_dfu_fw_update_server_start(fw_image_idx);
	} else {
		BT_LOGE("[%s] The current platform do not support mesh dfu.\r\n", __func__);
	}

	return ret;
}

bool rtk_bt_dfu_updater_server_apply(void)
{
	if (dfu_update_server_if.mesh_dfu_fw_update_server_apply) {
		return dfu_update_server_if.mesh_dfu_fw_update_server_apply();
	} else {
		BT_LOGE("[%s] The current platform do not support mesh dfu.\r\n", __func__);
		return false;
	}
}

bool rtk_bt_dfu_updater_server_block_data(uint16_t block_num, uint8_t *pdata, uint16_t data_len)
{
	if (dfu_update_server_if.mesh_dfu_fw_update_server_block_data) {
		return dfu_update_server_if.mesh_dfu_fw_update_server_block_data(block_num, pdata, data_len);
	} else {
		BT_LOGE("[%s] The current platform do not support mesh dfu.\r\n", __func__);
		return false;
	}
}

bool rtk_bt_dfu_updater_server_verify(void)
{
	if (dfu_update_server_if.mesh_dfu_fw_update_server_verify) {
		return dfu_update_server_if.mesh_dfu_fw_update_server_verify();
	} else {
		BT_LOGE("[%s] The current platform do not support mesh dfu.\r\n", __func__);
		return false;
	}
}

#endif  // BT_MESH_ENABLE_DFU_TARGET_ROLE

#endif  // RTK_BLE_MESH_SUPPORT
