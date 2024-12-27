/**
 * @file      atcmd_bt_mesh_device_firmware_update_model.c
 * @brief     Bluetooth mesh at command device firmware update model relate api function definition
 * @copyright Copyright (c) 2024. Realtek Semiconductor Corporation. All rights reserved.
 */

#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <string.h>
#include <osif.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <bt_utils.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_device_firmware_update_model.h>

// updat timeout base:usual set to 1, blob server timeout:(update_timeout_base + 1) * 10s
// If do this second time, should set fw_id.version more than last time
static int atcmd_ble_mesh_dfu_initiator_upload_start(int argc, char **argv)
{
#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE
	rtk_bt_mesh_dfu_initiator_upload_start_t start = {0};
	uint16_t ret = 0;
	start.dst = str_to_int(argv[0]);
	start.app_key_index = str_to_int(argv[1]);
	start.update_timeout_base = str_to_int(argv[2]);
	start.fw_image_size = str_to_int(argv[3]);
	// Set the default fw id for OTA
	start.fw_id_len = 4;
	start.fw_id.company_id = 0x005d;
	start.fw_id.version[0] = 0x00;
	start.fw_id.version[1] = 0x01;
	BT_LOGD("[%s] Dst:0x%x.\r\n", __func__, start.dst);
	switch (argc) {
	case 4: {
		break;
	}
	case 5: {
		// If execute mesh OTA more than first time, the firmware id of second time should be larger than last time before TX to distributor
		start.fw_id.version[1] = str_to_int(argv[4]);
		BT_LOGD("[%s] Set firmware id to %d.\r\n", __func__, start.fw_id.version[1]);
		break;
	}
	case 6: {
		start.metadata_len = str_to_int(argv[4]);
		start.p_metadata = osif_mem_alloc(RAM_TYPE_DATA_ON, start.metadata_len);
		if (start.p_metadata) {
			if (!hexdata_str_to_array(argv[5], start.p_metadata, start.metadata_len)) {
				BT_LOGE("[%s] String to hex for UUID fail!\r\n", __func__);
				osif_mem_free(start.p_metadata);
				return -2;
			}
			BT_LOGD("[%s] metadata0:0x%x, metadata-1:0x%x.\r\n", __func__, start.p_metadata[0], start.p_metadata[start.metadata_len - 1]);
		} else {
			BT_LOGE("[%s] Malloc fail.\r\n", __func__);
			return -3;
		}
		break;
	}
	default: {
		BT_LOGE("[%s] Unexpected argc:%d.\r\n", __func__, argc);
		break;
	}
	}
	ret = rtk_bt_mesh_dfu_initiator_upload_start(&start);
	if (start.p_metadata) {
		osif_mem_free(start.p_metadata);
	}
	if (ret) {
		BT_LOGE("[%s] DFU initiator upload start failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argc;
	(void)argv;
	BT_LOGE("[ATBC] Do not suppport DFU initiator role.\r\n");
	return -1;
#endif
}

// If add receivers second time, should delete all receivers before do it
static int atcmd_ble_mesh_dfu_initiator_receivers_add(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE
	rtk_bt_mesh_dfu_initiator_dist_recv_add_t add = {0};
	uint16_t ret = 0;
	add.dst = str_to_int(argv[0]);
	add.app_key_index = str_to_int(argv[1]);
	add.recv_num = str_to_int(argv[2]);
	add.recv = osif_mem_alloc(RAM_TYPE_DATA_ON, add.recv_num * sizeof(rtk_bt_mesh_dfu_initiator_dist_recv_t));
	BT_LOGD("[%s] Dst:0x%x.\r\n", __func__, add.dst);
	if (add.recv) {
		for (uint8_t i = 0; i < add.recv_num; i++) {
			(add.recv + i)->addr = str_to_int(argv[i + 3]);
			(add.recv + i)->update_fw_image_idx = 0;  // only regist fw_update_server_add_info once time, so can only be 0
		}
	} else {
		BT_LOGE("[%s] Malloc fail.\r\n", __func__);
		return -2;
	}
	ret = rtk_bt_mesh_dfu_initiator_receivers_add(&add);
	if (add.recv) {
		osif_mem_free(add.recv);
	}
	if (ret) {
		BT_LOGE("[%s] DFU initiator receivers add failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("[ATBC] Do not suppport DFU initiator role.\r\n");
	return -1;
#endif
}

// For second time mesh OTA for distributor, should delete all receivers before 2nd mesh OTA
static int atcmd_ble_mesh_dfu_initiator_receivers_delete_all(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE
	rtk_bt_mesh_dfu_initiator_dist_recv_del_all_t del = {0};
	uint16_t ret = 0;
	del.dst = str_to_int(argv[0]);
	del.app_key_index = str_to_int(argv[1]);
	BT_LOGD("[%s] Dst:0x%x.\r\n", __func__, del.dst);
	ret = rtk_bt_mesh_dfu_initiator_receivers_delete_all(&del);
	if (ret) {
		BT_LOGE("[%s] DFU initiator receivers delete all failed! reason: 0x%x.\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("[ATBC] Do not suppport DFU initiator role.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_dfu_initiator_distribute_start(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE
	rtk_bt_mesh_dfu_initiator_fw_dist_start_param_t start = {0};
	uint16_t ret = 0;

	start.dst = str_to_int(argv[0]);
	start.app_key_index = str_to_int(argv[1]);
	start.dist_dst_len = 2;  // len of start.data.dist_multicast_addr(Distribution Multicast Address on spec), only use Multicast Address and group address temp
	start.data.dist_app_key_index = str_to_int(argv[2]);
	start.data.dist_ttl = str_to_int(argv[3]);
	start.data.dist_timeout_base = str_to_int(argv[4]);
	start.data.dist_transfer_mode = RTK_BT_MESH_DFU_BLOB_TRANSFER_MODE_PUSH;
	start.data.update_policy = RTK_BT_MESH_DFU_INITIATOR_FW_UPDATE_POLICY_VERIFY_AND_UPDATE;
	start.data.dist_fw_image_idx = 0;
	*(uint16_t *)start.data.dist_multicast_addr = str_to_int(argv[5]);

	BT_LOGD("[%s] Dst:0x%x.\r\n", __func__, start.dst);
	ret = rtk_bt_mesh_dfu_initiator_distribute_start(&start);
	if (ret) {
		BT_LOGE("[%s] DFU initiator distribute start failed! reason: 0x%x.\r\n", __func__, ret);
		return -2;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("[ATBC] Do not suppport DFU initiator role.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_dfu_standalone_updater_set_param(int argc, char **argv)
{
#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
	uint16_t ret = 0;
	rtk_bt_mesh_dfu_standalone_updater_set_param_t param = {0};
	uint8_t meta_data[] = {0x01, 0x03, 0x06, 0x09};
	uint8_t firmware_id[] = {0x01, 0x02, 0x03, 0x04};
	uint8_t len_meta, len_fw_id;

	len_meta = sizeof(meta_data);
	len_fw_id = sizeof(firmware_id);

	memcpy(param.meta_data, meta_data, len_meta);
	param.meta_data_len = len_meta;
	memcpy(param.fw_id, firmware_id, len_fw_id);
	param.fw_id_len = len_fw_id;

	switch (argc) {
	case 5: {
		// todo:Set firmware id
		BT_LOGE("[%s] todo for firmware id.\r\n", __func__);
		__attribute__((fallthrough));
	}
	case 3: {
		// todo:Set metadata
		BT_LOGE("[%s] todo for meta data.\r\n", __func__);
		__attribute__((fallthrough));
	}
	case 1: {
		param.image_size = str_to_int(argv[0]);
		break;
	}
	default: {
		BT_LOGE("[%s] Error input param num:%d.\r\n", __func__, argc);
		break;
	}
	}
	ret = rtk_bt_mesh_dfu_standalone_updater_set_param(&param);
	if (ret) {
		BT_LOGE("[%s] DFU standalone updater set param failed! reason: 0x%x.\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argc;
	(void)argv;
	BT_LOGE("[ATBC] Do not suppport DFU standalone updater role.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_dfu_standalone_updater_add_receiver(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
	rtk_bt_mesh_dfu_standalone_updater_add_receiver_param_t rece = {0};
	uint16_t ret = 0;
	rece.addr = str_to_int(argv[0]);
	rece.update_image_index = 0;  // Set ot 0 temp, only fw_update_server_add_info once time, so can only be 0
	ret = rtk_bt_mesh_dfu_standalone_updater_add_receiver(&rece);
	if (ret) {
		BT_LOGE("[%s] DFU standalone updater add receiver failed! reason: 0x%x.\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("[ATBC] Do not suppport DFU standalone updater role.\r\n");
	return -1;
#endif
}

// timeout:(update_timeout_base+1)*10s  nomal set to 1 for RTK stack and set to 5 for zephyr stack
static int atcmd_ble_mesh_dfu_standalone_updater_start(int argc, char **argv)
{
#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
	(void)argc;
	rtk_bt_mesh_dfu_standalone_update_start_t start = {0};
	uint16_t ret = 0;

	start.dst = str_to_int(argv[0]);
	start.app_key_index = str_to_int(argv[1]);
	start.update_timeout_base = str_to_int(argv[2]);

	ret = rtk_bt_mesh_dfu_standalone_updater_start(&start);

	if (ret) {
		BT_LOGE("[%s] DFU standalone updater start failed! reason: 0x%x.\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argc;
	(void)argv;
	BT_LOGE("[ATBC] Do not suppport DFU standalone updater role.\r\n");
	return -1;
#endif
}

static const cmd_table_t mesh_dfu_model_cmd_table[] = {
	{"init_upstart",        atcmd_ble_mesh_dfu_initiator_upload_start,          5, 7},
	{"init_recadd",         atcmd_ble_mesh_dfu_initiator_receivers_add,         5, 10},
	{"init_recdela",        atcmd_ble_mesh_dfu_initiator_receivers_delete_all,  3, 3},
	{"init_disstart",       atcmd_ble_mesh_dfu_initiator_distribute_start,      7, 7},
	{"stand_param",         atcmd_ble_mesh_dfu_standalone_updater_set_param,    2, 6},
	{"stand_add",           atcmd_ble_mesh_dfu_standalone_updater_add_receiver, 2, 2},
	{"stand_start",         atcmd_ble_mesh_dfu_standalone_updater_start,        4, 4},
	{NULL,},
};

int atcmd_bt_mesh_device_firmware_update(int argc, char *argv[])
{
	atcmd_bt_excute(argc, argv, mesh_dfu_model_cmd_table, "[AT+BLEMESHDFU]");
	return 0;
}

#endif  // RTK_BLE_MESH_SUPPORT
