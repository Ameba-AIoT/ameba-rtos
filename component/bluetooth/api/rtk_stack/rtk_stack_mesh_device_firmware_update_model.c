/**
 * @file      rtk_stack_mesh_device_firmware_update_model.c
 * @brief     Bluetooth mesh stack device firmware update model relate api function definition
 * @copyright Copyright (c) 2024. Realtek Semiconductor Corporation. All rights reserved.
 */

#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_device_firmware_update_model.h>
#include <rtk_bt_device_firmware_update_interface.h>

#include <app_mesh_flags.h>
#include <blob_client_app.h>
#include <dfu_distributor_app.h>
#include <dfu_initiator_app.h>
#include <firmware_update.h>
#include <firmware_distribution.h>

#if defined(BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE) && BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE || \
    defined(BT_MESH_ENABLE_DFU_TARGET_ROLE) && BT_MESH_ENABLE_DFU_TARGET_ROLE
// The default max_blob_size capability in mesh stack is not big for ameba ic
// Should be called after blob transfer server model have been regist to set capability bigger
static void rtk_stack_blob_transfer_server_set_capability(void)
{
	blob_server_capabilities_t cap = {0};
	cap = blob_transfer_server_caps_get();
	cap.max_blob_size = 1500000;  // means the max image size
	blob_transfer_server_caps_set(&cap);
}
#endif

#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE
static uint16_t dfu_initiator_cb(uint8_t type, void *pdata)
{
	BT_LOGD("[%s][%d] type:%d.\r\n", __func__, __LINE__, type);
	switch (type) {
	case MESH_MSG_DFU_INIT_TRANSFER: {
		dfu_init_transfer_t *ptransfer = (dfu_init_transfer_t *)pdata;
		char *type_str[] = {"upload progress", "upload success", "upload fail"};
		BT_LOGD("[%s] type %s, client phase %d, ", __func__, type_str[ptransfer->type], ptransfer->init_phase);
#if 0
		if (ptransfer->type == DFU_INIT_CB_TYPE_UPLOAD_PROGRESS) {
			BT_LOGA("---------- %d%% ---------", ptransfer->progress);
		} else if (ptransfer->type == DFU_INIT_CB_TYPE_UPLOAD_SUCCESS) {
			BT_LOGA("Upload success for 0x%04x ", ptransfer->addr);
		} else if (ptransfer->type == DFU_INIT_CB_TYPE_UPLOAD_FAIL) {
			BT_LOGA("Upload fail for 0x%04x ", ptransfer->addr);
		}

		BT_LOGA("\r\n");
#else
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_dfu_evt_initiator_transfer_t *trans = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_INITIATOR_MODEL, RTK_BT_MESH_DFU_EVT_INITIATOR_TRANSFER, sizeof(rtk_bt_mesh_dfu_evt_initiator_transfer_t));
		trans = (rtk_bt_mesh_dfu_evt_initiator_transfer_t *)p_evt->data;
		trans->type = ptransfer->type;
		trans->init_phase = ptransfer->init_phase;
		if (RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_PROGRESS == trans->type) {
			trans->progress = ptransfer->progress;
		} else {
			trans->addr = ptransfer->addr;
		}
		rtk_bt_evt_indicate(p_evt, NULL);
#endif
		break;
	}
	case MESH_MSG_DFU_INIT_BLOB_PARAM: {
		dfu_init_blob_param_t *pparam = (dfu_init_blob_param_t *)pdata;
		BT_LOGD("[%s] Blob size %u, block size %d, total blocks %d, chunk size %d\r\n", __func__, (unsigned int)pparam->blob_size, pparam->block_size,
				pparam->total_blocks, pparam->chunk_size);
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t *blob;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_INITIATOR_MODEL, RTK_BT_MESH_DFU_EVT_INITIATOR_BLOB_PARAM,
									sizeof(rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t));
		blob = (rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t *)p_evt->data;
		blob->blob_size = pparam->blob_size;
		blob->block_size = pparam->block_size;
		blob->total_blocks = pparam->total_blocks;
		blob->chunk_size = pparam->chunk_size;
		rtk_bt_evt_indicate(p_evt, NULL);
		break;
	}
	case MESH_MSG_DFU_INIT_BLOCK_LOAD: {
		dfu_init_block_load_t *pblock_load = (dfu_init_block_load_t *)pdata;
		bool ram_or_flash = false;
		if (ram_or_flash) {
			/* if data from ram, set right data here*/
			for (uint32_t i = 0; i < pblock_load->block_size; i++) {
				pblock_load->pblock_data[i] = (pblock_load->offset + i) / 1024;
			}
		} else {
			/* if data from flash, set right start address here */
			BT_LOGD("[%s] Case load block data from flash, pblock_load->offset:%d, pblock_load->block_size:%d.\r\n", __func__, pblock_load->offset,
					pblock_load->block_size);
			if (!rtk_bt_dfu_load_data_from_flash(pblock_load->offset, pblock_load->block_size, pblock_load->pblock_data)) {
				BT_LOGE("[%s] Read new firmware data from flash fail.\r\n", __func__);
			}
		}
		break;
	}
	case MESH_MSG_DFU_INIT_CLIENT_DATA: {
		dfu_init_client_data_t *pclient_data = (dfu_init_client_data_t *)pdata;
		switch (pclient_data->type) {
		case FW_DIST_CLIENT_RECVS_STATUS: {
			fw_dist_client_recvs_status_t *pdata = (fw_dist_client_recvs_status_t *)pclient_data->pdata;
			BT_LOGD("[%s] Receive Firmware Distribution Receivers Status message, status:%d, list count:%d.\r\n", __func__, pdata->status, pdata->recvs_list_cnt);
			rtk_bt_evt_t *p_evt = NULL;
			rtk_bt_mesh_dfu_initiator_evt_dist_client_recvs_status *pstatus = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_INITIATOR_MODEL, RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_RECEIVES_STATUS,
										sizeof(rtk_bt_mesh_dfu_initiator_evt_dist_client_recvs_status));
			pstatus = (rtk_bt_mesh_dfu_initiator_evt_dist_client_recvs_status *)p_evt->data;
			pstatus->src = pdata->src;
			pstatus->status = pdata->status;
			pstatus->recvs_list_cnt = pdata->recvs_list_cnt;
			rtk_bt_evt_indicate(p_evt, NULL);
			break;
		}
		case FW_DIST_CLIENT_RECVS_LIST: {
			// fw_dist_client_recvs_list_t *pdata = (fw_dist_client_recvs_list_t *)pclient_data->pdata;
			break;
		}
		case FW_DIST_CLIENT_CAPS_STATUS: {
			// fw_dist_client_caps_status_t *pdata = (fw_dist_client_caps_status_t *)pclient_data->pdata;
			break;
		}
		case FW_DIST_CLIENT_STATUS: {
			fw_dist_client_dist_status_t *pdata = (fw_dist_client_dist_status_t *)pclient_data->pdata;
			BT_LOGD("[%s] Receive Firmware Distribution Status message, status:%d.\r\n", __func__, pdata->status);
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_INITIATOR_MODEL, RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_DISTRIBUTION_STATUS, sizeof(uint8_t));
			*(uint8_t *)p_evt->data = pdata->status;
			rtk_bt_evt_indicate(p_evt, NULL);
			break;
		}
		case FW_DIST_CLIENT_UPLOAD_STATUS: {
			fw_dist_client_upload_status_t *pdata = (fw_dist_client_upload_status_t *)pclient_data->pdata;
			BT_LOGD("[%s] Receive Firmware Distribution Upload Status message, status:%d.\r\n", __func__, pdata->status);
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_INITIATOR_MODEL, RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_UPLOAD_STATUS, sizeof(uint8_t));
			*(uint8_t *)p_evt->data = pdata->status;
			rtk_bt_evt_indicate(p_evt, NULL);
			break;
		}
		case FW_DIST_CLIENT_FW_STATUS: {
			// fw_dist_client_fw_status_t *pdata = (fw_dist_client_fw_status_t *)pclient_data->pdata;
			break;
		}
		default:
			BT_LOGE("[%s][%d] Unknown type:%d.\r\n", __func__, __LINE__, pclient_data->type);
			break;
		}
		break;
	}
	default:
		BT_LOGE("[%s][%d] Unknown type:%d.\r\n", __func__, __LINE__, type);
		break;
	}

	return 0x00;
}

bool rtk_stack_dfu_initiator_init(void)
{
#if F_BT_MESH_1_1_DFU_SUPPORT
	BT_LOGD("[%s] Enter.\r\n", __func__);
	rtk_bt_dfu_initiator_resource_init();
	dfu_init_models_init(0, dfu_initiator_cb);
	return true;
#else
	BT_LOGE("[%s] Mesh stack app_mesh_flags.h do not enable DFU function.\r\n", __func__);
	return ret;
#endif
}

static uint16_t rtk_mesh_dfu_initiator_upload_start(rtk_bt_mesh_dfu_initiator_upload_start_t *start)
{
	uint16_t ret = RTK_BT_MESH_DFU_MODEL_API_FAIL;
	fw_id_t firmware_id;

	BT_LOGD("[%s] Enter.\r\n", __func__);

	firmware_id.company_id = start->fw_id.company_id;
	memcpy(firmware_id.version, start->fw_id.version, start->fw_id_len - sizeof(firmware_id.company_id));

	if (dfu_init_upload_start(start->dst, start->app_key_index, start->update_timeout_base, start->p_metadata, start->metadata_len, &firmware_id, start->fw_id_len,
							  start->fw_image_size)) {
		ret = RTK_BT_MESH_DFU_MODEL_API_SUCCESS;
	}

	return ret;
}

static uint16_t rtk_mesh_dfu_initiator_receiver_add(rtk_bt_mesh_dfu_initiator_dist_recv_add_t *p)
{
	uint16_t ret = RTK_BT_MESH_DFU_MODEL_API_FAIL;
	fw_dist_receiver_t *rece;

	BT_LOGD("[%s] Enter.\r\n", __func__);

	rece = (fw_dist_receiver_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, p->recv_num * sizeof(fw_dist_receiver_t));

	if (!rece) {
		BT_LOGE("[%s] Memory alloc fail.\r\n", __func__);
		return ret;
	}

	for (uint32_t i = 0; i < p->recv_num; i++) {
		(rece + i)->addr = (p->recv + i)->addr;
		(rece + i)->update_fw_image_idx = (p->recv + i)->update_fw_image_idx;
	}

	ret = fw_dist_recvs_add(p->dst, p->app_key_index, rece, p->recv_num);

	if (rece) {
		osif_mem_free(rece);
	}

	return ret;
}

static uint16_t rtk_mesh_dfu_initiator_distribute_start(rtk_bt_mesh_dfu_initiator_fw_dist_start_param_t *p)
{
	fw_dist_start_data_t start;

	BT_LOGD("[%s] Enter.\r\n", __func__);
	start.dist_app_key_index = p->data.dist_app_key_index;
	start.dist_ttl = p->data.dist_ttl;
	start.dist_timeout_base = p->data.dist_timeout_base;
	start.dist_transfer_mode = p->data.dist_transfer_mode;
	start.update_policy = p->data.update_policy;
	start.dist_fw_image_idx = p->data.dist_fw_image_idx;
	memcpy(start.dist_multicast_addr, p->data.dist_multicast_addr, 16);

	return fw_dist_start(p->dst, p->app_key_index, start, p->dist_dst_len);
}

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_dfu_initiator_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;

	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("[%s] Error: BLE MESH profile is not initiated.\r\n", __func__);
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}

	BT_LOGD("[%s] p_cmd->act:%d.\r\n", __func__, p_cmd->act);
	switch (p_cmd->act) {
	case RTK_BT_MESH_DFU_ACT_INITIATOR_UPLOAD_START: {
		ret = rtk_mesh_dfu_initiator_upload_start(p_cmd->param);
		break;
	}
	case RTK_BT_MESH_DFU_ACT_INITIATOR_RECEIVERS_ADD: {
		ret = rtk_mesh_dfu_initiator_receiver_add(p_cmd->param);
		break;
	}
	case RTK_BT_MESH_DFU_ACT_INITIATOR_RECEIVERS_DEL_ALL: {
		rtk_bt_mesh_dfu_initiator_dist_recv_del_all_t *del = (rtk_bt_mesh_dfu_initiator_dist_recv_del_all_t *)p_cmd->param;
		ret = fw_dist_recvs_delete_all(del->dst, del->app_key_index);
		break;
	}
	case RTK_BT_MESH_DFU_ACT_INITIATOR_DISTRIBUTE_START: {
		ret = rtk_mesh_dfu_initiator_distribute_start(p_cmd->param);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown p_cmd->act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}
#endif  // BT_MESH_ENABLE_DFU_INITIATOR_ROLE

#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE || \
    defined(BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE) && BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE
static uint16_t dfu_standalone_updater_or_distributor_cb(uint8_t type, void *pdata)
{
	BT_LOGD("[%s] type:%d.\r\n", __func__, type);
	switch (type) {
	case MESH_MSG_DFU_DIST_TRANSFER: {
		dfu_dist_transfer_t *ptransfer = (dfu_dist_transfer_t *)pdata;
		char *type_str[] = {"node fail", "transfer progress", "transfer success", "transfer fail", "verify", "complete"};
		BT_LOGD("[%s] type %s, client phase %d, ", __func__, type_str[ptransfer->type], ptransfer->dist_phase);
#if 0
		if (ptransfer->type == DFU_DIST_CB_TYPE_NODE_FAIL) {
			BT_LOGA("fail for addr 0x%04x\r\n", ptransfer->paddr[0]);
		} else if (ptransfer->type == DFU_DIST_CB_TYPE_TRANSFER_PROGRESS) {
			BT_LOGA(" ---------- %d%% ---------", ptransfer->progress);
		} else if (ptransfer->type == DFU_DIST_CB_TYPE_TRANSFER_SUCCESS ||
				   ptransfer->type == DFU_DIST_CB_TYPE_VERIFY ||
				   ptransfer->type == DFU_DIST_CB_TYPE_COMPLETE) {
			uint8_t i = 0;
			while (i < ptransfer->addr_num) {
				BT_LOGA("0x%04x ", ptransfer->paddr[i++]);
			}
		}

		BT_LOGA("\r\n");
#else
		uint8_t group = 0, event = 0;
		rtk_bt_evt_t *p_evt = NULL;
#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
		group = RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL;
#elif defined(BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE) && BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE
		group = RTK_BT_LE_GP_MESH_DFU_DISTRIBUTOR_MODEL;
#endif
		switch (ptransfer->type) {
		case DFU_DIST_CB_TYPE_NODE_FAIL: {
			rtk_bt_mesh_dfu_evt_distributor_transfer_node_fail_t *node_fail = NULL;
			p_evt = rtk_bt_event_create(group, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_NODE_FAIL, sizeof(rtk_bt_mesh_dfu_evt_distributor_transfer_node_fail_t));
			node_fail = (rtk_bt_mesh_dfu_evt_distributor_transfer_node_fail_t *)p_evt->data;
			node_fail->dist_phase = ptransfer->dist_phase;
			node_fail->addr = ptransfer->paddr[0];
			break;
		}
		case DFU_DIST_CB_TYPE_TRANSFER_PROGRESS: {
			rtk_bt_mesh_dfu_evt_distributor_transfer_progress_t *trans_progress = NULL;
			p_evt = rtk_bt_event_create(group, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_PROGRESS, sizeof(rtk_bt_mesh_dfu_evt_distributor_transfer_progress_t));
			trans_progress = (rtk_bt_mesh_dfu_evt_distributor_transfer_progress_t *)p_evt->data;
			trans_progress->dist_phase = ptransfer->dist_phase;
			trans_progress->progress = ptransfer->progress;
			break;
		}
		case DFU_DIST_CB_TYPE_TRANSFER_SUCCESS: {
			if (0 == event) {
				event = RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_SUCCESS;
			}
			__attribute__((fallthrough));
		}
		case DFU_DIST_CB_TYPE_VERIFY: {
			if (0 == event) {
				event = RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_VERIFY;
			}
			__attribute__((fallthrough));
		}
		case DFU_DIST_CB_TYPE_COMPLETE: {
			if (0 == event) {
				event = RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_COMPLETE;
			}
			rtk_bt_mesh_dfu_evt_distributor_transfer_other_t *trans_other = NULL;
			uint16_t num_of_addr = ptransfer->addr_num;
			p_evt = rtk_bt_event_create(group, event, sizeof(rtk_bt_mesh_dfu_evt_distributor_transfer_other_t) + num_of_addr * sizeof(uint16_t));
			trans_other = (rtk_bt_mesh_dfu_evt_distributor_transfer_other_t *)p_evt->data;
			trans_other->dist_phase = ptransfer->dist_phase;
			trans_other->addr_num = num_of_addr;
			trans_other->paddr = (uint16_t *)((uint8_t *)p_evt->data + sizeof(rtk_bt_mesh_dfu_evt_distributor_transfer_other_t));
			for (uint16_t i = 0; i < num_of_addr; i++) {
				trans_other->paddr[i] = ptransfer->paddr[i];
			}
			break;
		}
		default: {
			BT_LOGE("[%s] Unknown type:%d.\r\n", __func__, ptransfer->type);
			break;
		}
		}
		if (p_evt) {
			rtk_bt_evt_indicate(p_evt, NULL);
		}
#endif
		break;
	}
	case MESH_MSG_DFU_DIST_BLOB_PARAM: {
		dfu_dist_blob_param_t *pparam = (dfu_dist_blob_param_t *)pdata;
		BT_LOGD("[%s] blob size %u, block size %d, total blocks %d, chunk size %d\r\n", __func__, (unsigned int)pparam->blob_size, pparam->block_size,
				pparam->total_blocks, pparam->chunk_size);
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t *blob;
#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_PARAM,
									sizeof(rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t));
#elif defined(BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE) && BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_DISTRIBUTOR_MODEL, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_PARAM,
									sizeof(rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t));
#endif
		blob = (rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t *)p_evt->data;
		blob->blob_size = pparam->blob_size;
		blob->block_size = pparam->block_size;
		blob->total_blocks = pparam->total_blocks;
		blob->chunk_size = pparam->chunk_size;
		rtk_bt_evt_indicate(p_evt, NULL);
		break;
	}
	case MESH_MSG_DFU_DIST_BLOCK_LOAD: {
		dfu_dist_block_load_t *pblock_load = (dfu_dist_block_load_t *)pdata;
		bool ram_or_flash = false;
		if (ram_or_flash) {
			/* if data from ram, set right data here*/
			BT_LOGD("[%s] Case load block data from ram, pblock_load->offset:%d, pblock_load->block_size:%d.\r\n", __func__, pblock_load->offset, pblock_load->block_size);
			for (uint32_t i = 0; i < pblock_load->block_size; i++) {
				pblock_load->pblock_data[i] = (pblock_load->offset + i) % 1024;
			}
		} else {
			/* if data from flash, set right start address here */
			BT_LOGD("[%s] Case load block data from flash, pblock_load->offset:%d, pblock_load->block_size:%d.\r\n", __func__, pblock_load->offset,
					pblock_load->block_size);
			if (!rtk_bt_dfu_load_data_from_flash(pblock_load->offset, pblock_load->block_size, pblock_load->pblock_data)) {
				BT_LOGE("[%s] Read new firmware data from flash fail.\r\n", __func__);
			}
		}
		break;
	}
#if defined(BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE) && BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE
	case MESH_MSG_DFU_DIST_UPLOAD: {
		dfu_dist_upload_t *pupload = (dfu_dist_upload_t *)pdata;
		BT_LOGD("[%s] pupload->upload_type:%d.\r\n", __func__, pupload->upload_type);
		switch (pupload->upload_type) {
		case FW_DIST_SERVER_UPLOAD_START: {
			BT_LOGD("[%s] Case distributor server upload start.\r\n", __func__);
			rtk_bt_evt_t *p_evt = NULL;
#if 1
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_DISTRIBUTOR_MODEL, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_UPLOAD_START, 0);
			rtk_bt_evt_indicate(p_evt, NULL);
#else
			// Should report this event through directed calling, because there is point in data struct
			fw_dist_server_upload_start_t *pdata = (fw_dist_server_upload_start_t *)pupload->pupload_data;
			rtk_bt_mesh_dfu_evt_distributor_server_upload_start_t *start;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_DISTRIBUTOR_MODEL, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_UPLOAD_START,
										sizeof(rtk_bt_mesh_dfu_evt_distributor_server_upload_start_t));
			start = (rtk_bt_mesh_dfu_evt_distributor_server_upload_start_t *)p_evt->data;
			start->upload_ttl = pdata->upload_ttl;
			start->upload_timeout_base = pdata->upload_timeout_base;
			memcpy(start->upload_blob_id, pdata->upload_blob_id);
			start->upload_fw_size = pdata->upload_fw_size;
			start->upload_fw_metadata_len = pdata->upload_fw_metadata_len;
			start->pupload_metadata = pdata->pupload_metadata;
			start->upload_fw_id_len = pdata->upload_fw_id_len;
			start->pupload_fw_id = pdata->pupload_fw_id;
			rtk_bt_evt_indicate(p_evt, NULL);
#endif
			break;
		}
		case FW_DIST_SERVER_UPLOAD_OOB_START: {
			// fw_dist_server_upload_oob_start_t *pdata = (fw_dist_server_upload_oob_start_t *)
			//                                            pupload->pupload_data;
			// DFU/SR/FD/BV-58-C
			// download & verify success
			fw_dist_server_upload_oob_done(true);
			break;
		}
		case FW_DIST_SERVER_UPLOAD_BLOCK_DATA: {
			BT_LOGD("[%s] Case distributor server block data.\r\n", __func__);
			fw_dist_server_upload_block_data_t *pblock = (fw_dist_server_upload_block_data_t *)pupload->pupload_data;
			if (!rtk_bt_dfu_distributor_block_data(pblock->block_num, pblock->pdata, pblock->data_len)) {
				BT_LOGE("[%s] Save a block data to flash fail for distribute server upload, block num:%d.\r\n", __func__, pblock->block_num);
			} else {
				BT_LOGD("[%s] Receive a block data, block num:%d.\r\n", __func__, pblock->block_num);
				rtk_bt_evt_t *p_evt = NULL;
				p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_DISTRIBUTOR_MODEL, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_UPLOAD_BLOCK_DATA, sizeof(uint16_t));
				*(uint16_t *)(p_evt->data) = pblock->block_num;
				rtk_bt_evt_indicate(p_evt, NULL);
			}
			break;
		}
		case FW_DIST_SERVER_UPLOAD_COMPLETE: {
			BT_LOGD("[%s] Case distributor server upload complete.\r\n", __func__);
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_DISTRIBUTOR_MODEL, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_UPLOAD_COMPLETE, 0);
			rtk_bt_evt_indicate(p_evt, NULL);
			break;
		}
		case FW_DIST_SERVER_UPLOAD_FAIL: {
			fw_dist_server_upload_fail_t *pdata = (fw_dist_server_upload_fail_t *)pupload->pupload_data;
#if 1
			BT_LOGA("[%s] Case distributor server upload fail, reason:%d.\r\n", __func__, pdata->reason);
#else
			rtk_bt_mesh_dfu_evt_distributor_server_upload_fail_t *fail = NULL;
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_DISTRIBUTOR_MODEL, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_UPLOAD_COMPLETE,
										sizeof(rtk_bt_mesh_dfu_evt_distributor_server_upload_fail_t));
			fail = (rtk_bt_mesh_dfu_evt_distributor_server_upload_fail_t *)p_evt->data;
			fail->reason = pdata->reason;
			rtk_bt_evt_indicate(p_evt, NULL);
#endif
			break;
		}
		case FW_DIST_SERVER_URI_CHECK: {
			// download from URI
			fw_dist_server_uri_check_t *pdata = (fw_dist_server_uri_check_t *)pupload->pupload_data;
			BT_LOGA("URI: %s\r\n", pdata->puri);
			/* set the firmware image size */
			// fw_dist_server_ctx.upload_fw_size = 300;
			*pdata->pstatus = FW_DIST_STATUS_SUCCESS;
			break;
		}
		default:
			BT_LOGE("[%s] Unknown pupload->upload_type:%d.", __func__, pupload->upload_type);
			break;
		}
		break;
	}
#endif  // BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE
	default:
		BT_LOGE("[%s] Unknown type:%d.", __func__, type);
		break;
	}

	return 0x00;
}
#endif  // BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE or BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE

#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
bool rtk_stack_dfu_standalone_updater_init(void)
{
#if F_BT_MESH_1_1_DFU_SUPPORT
	BT_LOGD("[%s] Enter.\r\n", __func__);
	if (!rtk_bt_dfu_standalone_updater_resource_init()) {
		BT_LOGE("[%s] DFU standalone updater init fail.\r\n", __func__);
		return false;
	}

	dfu_standalone_updater_models_init(0, dfu_standalone_updater_or_distributor_cb);
	return true;
#else
	BT_LOGE("[%s] Mesh stack app_mesh_flags.h do not enable DFU function.\r\n", __func__);
	return false;
#endif
}

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_dfu_standalone_updater_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_DFU_MODEL_API_FAIL;
	static rtk_bt_mesh_dfu_standalone_updater_set_param_t dfu_param = {0};

	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("[%s] Error: BLE MESH profile is not initiated.\r\n", __func__);
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}

	BT_LOGD("[%s] p_cmd->act:%d.\r\n", __func__, p_cmd->act);
	switch (p_cmd->act) {
	case RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_SET_PARAM: {
		memcpy(&dfu_param, p_cmd->param, sizeof(rtk_bt_mesh_dfu_standalone_updater_set_param_t));
		ret = RTK_BT_MESH_DFU_MODEL_API_SUCCESS;
		break;
	}
	case RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_ADD_RECEIVER: {
		rtk_bt_mesh_dfu_standalone_updater_add_receiver_param_t *rece = (rtk_bt_mesh_dfu_standalone_updater_add_receiver_param_t *)p_cmd->param;
		if (dfu_dist_add_receiver(rece->addr, rece->update_image_index)) {
			ret = RTK_BT_MESH_DFU_MODEL_API_SUCCESS;
		} else {
			BT_LOGE("[%s] Mesh stack api return fail for p_cmd->act:%d.\r\n", __func__, p_cmd->act);
		}
		break;
	}
	case RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_START: {
		rtk_bt_mesh_dfu_standalone_update_start_t *start = (rtk_bt_mesh_dfu_standalone_update_start_t *)p_cmd->param;
		if (dfu_dist_start(start->dst, start->app_key_index, start->update_timeout_base, dfu_param.meta_data, dfu_param.meta_data_len, dfu_param.image_size)) {
			ret = RTK_BT_MESH_DFU_MODEL_API_SUCCESS;
		} else {
			BT_LOGE("[%s] Mesh stack api return fail for p_cmd->act:%d.\r\n", __func__, p_cmd->act);
		}
		break;
	}
	default:
		BT_LOGE("[%s] Unknown p_cmd->act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}
#endif  // BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE

#if defined(BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE) && BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE
bool rtk_stack_dfu_distributor_init(void)
{
#if F_BT_MESH_1_1_DFU_SUPPORT
	BT_LOGD("[%s] Enter.\r\n", __func__);
	if (!rtk_bt_dfu_distributor_resource_init()) {
		BT_LOGE("[%s] DFU distributor init fail.\r\n", __func__);
		return false;
	}

	dfu_dist_models_init(0, dfu_standalone_updater_or_distributor_cb);

	// Set blob transfer server capablity, the default value of stack is not enough
	rtk_stack_blob_transfer_server_set_capability();

	// Set distributor server capability, the defualt value of stack is not enough
	uint32_t image_size = 1500000;
	fw_dist_caps_t dist_cap;
	dist_cap = fw_dist_server_caps_get();
	dist_cap.max_fw_image_size = image_size;
	dist_cap.max_upload_space = image_size;
	dist_cap.remaining_upload_space = image_size;
	fw_dist_server_caps_set(&dist_cap);

	BT_LOGD("[%s] Regist distributor relate models and set relate capability finish.\r\n", __func__);
	return true;
#else
	BT_LOGE("[%s] Mesh stack app_mesh_flags.h do not enable DFU function.\r\n", __func__);
	return false;
#endif
}
#endif  // BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE

#if defined(BT_MESH_ENABLE_DFU_TARGET_ROLE) && BT_MESH_ENABLE_DFU_TARGET_ROLE
// if mesh stack directory/common/dfu_updater.c changed, should review this function
static int32_t dfu_fw_update_server_data(const mesh_model_info_p pmodel_info, uint32_t type, void *pargs)
{
	BT_LOGD("[%s] type:%d\r\n", __func__, type);
	(void)pmodel_info;
	switch (type) {
	case FW_UPDATE_SERVER_METADATA_CHECK: {
#if 0
		fw_update_server_metadata_check_t *pdata = (fw_update_server_metadata_check_t *)pargs;
		bool check_result = false;

		// Should direct forwarding event
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_dfu_evt_target_meatdata_t *meta;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_TARGET_MODEL, RTK_BT_MESH_DFU_EVT_TARGET_METADATA_CHECK, sizeof(rtk_bt_mesh_dfu_evt_target_meatdata_t));
		meta = (rtk_bt_mesh_dfu_evt_target_meatdata_t *)p_evt->data;
		meta->fw_image_idx = pdata->fw_image_idx;
		meta->pmetadata = pdata->pmetadata;
		meta->metadata_len = pdata->metadata_len;
		rtk_bt_evt_indicate(p_evt, &check_result);

		if (!check_result) {
			*(pdata->pstatus) = FW_UPDATE_STATUS_METADATA_CHECK_FAILED
		}
		BT_LOGD("[%s] Meta data check result:%d.\r\n", __func__, check_result);
#endif
		break;
	}
	case FW_UPDATE_SERVER_START: {
		fw_update_server_start_t *pstart = (fw_update_server_start_t *)pargs;
		*(pstart->pcan_update) = rtk_bt_dfu_updater_server_start(pstart->fw_image_idx);

		/* change callback since Role change to Updater */
		blob_transfer_server_set_data_cb(fw_update_handle_blob_server_data);
		BT_LOGD("[%s] Case start.\r\n", __func__);
#if 0
		fw_update_server_start_t *pdata = (fw_update_server_start_t *)pargs;
		bool ready_start = false;

		// Should direct forwarding event
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_TARGET_MODEL, RTK_BT_MESH_DFU_EVT_TARGET_START, sizeof(uint8_t));
		*(uint8_t *)(p_evt->data) = pdata->fw_image_idx;
		rtk_bt_evt_indicate(p_evt, &ready_start);

		*(p_evt->pcan_update) = ready_start;
		BT_LOGD("[%s] whether can update state:%d.\r\n", __func__, ready_start);
#endif
		break;
	}
	case FW_UPDATE_SERVER_VERIFY: {
		bool verify_result = false;
		verify_result = rtk_bt_dfu_updater_server_verify();
		fw_update_server_set_verify_result(verify_result);
		if (verify_result) {
			BT_LOGD("[%s] Verify result:%d.\r\n", __func__, verify_result);
		} else {
			BT_LOGE("[%s] Verify fail.\r\n", __func__);
		}
#if 0
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_TARGET_MODEL, RTK_BT_MESH_DFU_EVT_TARGET_VERIFY, 0);
		rtk_bt_evt_indicate(p_evt, NULL);
#endif
		break;
	}
	case FW_UPDATE_SERVER_VERIFY_CANCEL: {
		BT_LOGA("[%s] Verify cancel.\r\n", __func__);
#if 0
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_TARGET_MODEL, RTK_BT_MESH_DFU_EVT_TARGET_VERIFY_CANCEL, 0);
		rtk_bt_evt_indicate(p_evt, NULL);
#endif
		break;
	}
	case FW_UPDATE_SERVER_APPLY: {
		BT_LOGD("[%s] APPLY.\r\n", __func__);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_TARGET_MODEL, RTK_BT_MESH_DFU_EVT_TARGET_APPLY, 0);
		rtk_bt_evt_indicate(p_evt, NULL);
		rtk_bt_dfu_updater_server_apply();
		break;
	}
	case FW_UPDATE_SERVER_BLOCK_DATA: {
		fw_update_server_block_data_t *pblock = (fw_update_server_block_data_t *)pargs;
		if (!rtk_bt_dfu_updater_server_block_data(pblock->block_num, pblock->pdata, pblock->data_len)) {
			BT_LOGE("[%s] Save a block data to flash fail for target role.\r\n", __func__);
		}
		BT_LOGD("[%s] Receive a block data.\r\n", __func__);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_TARGET_MODEL, RTK_BT_MESH_DFU_EVT_TARGET_BLOCK_DATA, sizeof(uint16_t));
		*(uint16_t *)(p_evt->data) = pblock->block_num;
		rtk_bt_evt_indicate(p_evt, NULL);
		break;
	}
	case FW_UPDATE_SERVER_FAIL: {
		fw_update_server_fail_t *pdata = (fw_update_server_fail_t *)pargs;
		BT_LOGA("[%s] Image index %d, reason %d", __func__, pdata->fw_image_index, pdata->reason);
#if 0
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_dfu_evt_target_fw_update_server_fail_t *fail = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_TARGET_MODEL, RTK_BT_MESH_DFU_EVT_TARGET_FAIL, sizeof(rtk_bt_mesh_dfu_evt_target_fw_update_server_fail_t));
		fail = (rtk_bt_mesh_dfu_evt_target_fw_update_server_fail_t *)p_evt->data;
		fail->fw_image_index = pdata->fw_image_index;
		fail->reason = pdata->reason;
		rtk_bt_evt_indicate(p_evt, NULL);
#endif
		break;
	}
	default:
		BT_LOGE("[%s] Unknown type:%d\r\n", __func__, type);
		break;
	}

	return MODEL_SUCCESS;
}

static bool blob_transfer_server_model_init(void)
{
	bool ret = false;
#if F_BT_MESH_1_1_MBT_SUPPORT
	ret = blob_transfer_server_reg(0, fw_update_handle_blob_server_data);
	if (ret) {
		rtk_stack_blob_transfer_server_set_capability();
		BT_LOGD("[%s] Blob transfer server model reg and set capability success.\r\n", __func__);
	} else {
		BT_LOGE("[%s] Blob transfer server model reg fail.\r\n", __func__);
	}
#else
	BT_LOGE("[%s] Mesh stack app_mesh_flags.h do not enable blob transfer function.\r\n", __func__);
#endif
	return ret;
}

static bool firmware_update_server_model_init(void)
{
	bool ret = false;
#if F_BT_MESH_1_1_DFU_SUPPORT
	ret = fw_update_server_reg(0, dfu_fw_update_server_data);

	if (ret) {
		fw_id_t fw_id;
		fw_id.company_id = 0x005d;
		fw_id.version[0] = 0x00;
		fw_id.version[1] = 0x01;
		ret = fw_update_server_add_info(&fw_id, 4, NULL, 0);
		if (!ret) {
			BT_LOGE("[%s] Firmware update server model add info fail.\r\n", __func__);
		} else {
			BT_LOGD("[%s] Firmware update server model init and set firmware info success.\r\n", __func__);
		}
	} else {
		BT_LOGE("[%s] Firmware update server model reg fail.\r\n", __func__);
	}
	return ret;
#else
	BT_LOGE("[%s] Mesh stack app_mesh_flags.h do not enable firmware models.\r\n", __func__);
#endif
	return ret;
}

bool rtk_stack_dfu_target_init(void)
{
	bool ret = false;

	BT_LOGD("[%s] Enter.\r\n", __func__);
	ret = rtk_bt_dfu_target_resource_init();
	if (!ret) {
		BT_LOGE("[%s] Target resource init fail.\r\n", __func__);
		return ret;
	}

	ret = blob_transfer_server_model_init();
	if (!ret) {
		BT_LOGE("[%s] Blob trnasfer server init fail.\r\n", __func__);
		return ret;
	}

	ret = firmware_update_server_model_init();
	if (!ret) {
		BT_LOGE("[%s] Firmware updater server init fail.\r\n", __func__);
		return ret;
	}

	return true;
}

#endif  // BT_MESH_ENABLE_DFU_TARGET_ROLE

#endif  // RTK_BLE_MESH_SUPPORT
