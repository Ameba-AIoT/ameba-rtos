/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <string.h>
#include <stdio.h>
#include <bt_api_config.h>
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#include <osif.h>
#include <ble_audio_flags.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>

#include <rtk_stack_internal.h>
#include <rtk_stack_config.h>
#include <rtk_stack_le_audio_internal.h>
#include <bt_gatt_svc.h>
#include <bt_gatt_client.h>
#include <gap_iso_data.h>
#include <gap.h>
#include <bap.h>
#include <bap_unicast_client.h>
#include <cap.h>
#include <ble_audio.h>
#include <pacs_mgr.h>
#include <ascs_mgr.h>
#include <bass_mgr.h>
#include <bass_client.h>
#include <csis_mgr.h>
#include <csis_client.h>
#include <mcp_client.h>
#include <mcp_mgr.h>
#include <ble_mgr.h>
#include <metadata_def.h>
#include <ble_audio_sync.h>
#include <broadcast_source_sm.h>
#include <base_data_generate.h>
#include <vocs_mgr.h>
#include <vocs_client.h>
#include <vcs_def.h>
#include <vcs_mgr.h>
#include <vcs_client.h>
#include <mics_mgr.h>
#include <mics_client.h>
#include <aics_def.h>
#include <aics_mgr.h>
#include <aics_client.h>
#include <bt_gatt_svc.h>
#include <bt_gatt_client.h>
#include <tmas_mgr.h>
#include <tmas_client.h>
#include <tmas_def.h>
#include <gmas_mgr.h>
#include <gmas_client.h>
#include <gmas_def.h>
#include <bt_direct_msg.h>

rtk_bt_le_audio_priv_t bt_le_audio_priv_data = {0};
uint8_t bt_stack_device_name[RTK_BT_GAP_DEVICE_NAME_LEN] = {0};
uint8_t device_name_len = 0;

rtk_bt_le_audio_link_t *bt_stack_le_audio_alloc_le_link(uint16_t conn_handle)
{
	rtk_bt_le_audio_link_t *p_link = NULL;
	uint8_t i;

	for (i = 0; i < RTK_BT_LE_AUDIO_MAX_BLE_LINK_NUM; i++) {
		if (bt_le_audio_priv_data.le_link[i].used == false) {
			p_link = &bt_le_audio_priv_data.le_link[i];
			p_link->used = true;
			p_link->conn_id = i;
			p_link->conn_handle = conn_handle;
			break;
		}
	}

	return p_link;
}

rtk_bt_le_audio_link_t *bt_stack_le_audio_find_link_by_conn_handle(uint16_t conn_handle)
{
	rtk_bt_le_audio_link_t *p_link = NULL;
	uint8_t i;

	for (i = 0; i < RTK_BT_LE_AUDIO_MAX_BLE_LINK_NUM; i++) {
		if (bt_le_audio_priv_data.le_link[i].used == true &&
			bt_le_audio_priv_data.le_link[i].conn_handle == conn_handle) {
			p_link = &bt_le_audio_priv_data.le_link[i];
			break;
		}
	}

	return p_link;
}

bool bt_stack_le_audio_link_free(rtk_bt_le_audio_link_t *p_link)
{
	if (p_link != NULL) {
		if (p_link->used == true) {
			if (p_link->brs_char_tbl) {
				osif_mem_free((void *)p_link->brs_char_tbl);
			}
			memset(p_link, 0, sizeof(rtk_bt_le_audio_link_t));
			p_link->conn_id = 0xFF;
			return true;
		}
	}

	return false;
}

rtk_bt_le_audio_ase_t *bt_stack_le_audio_find_ase(uint16_t conn_handle, uint8_t ase_id)
{
	rtk_bt_le_audio_link_t *p_link = bt_stack_le_audio_find_link_by_conn_handle(conn_handle);

	if (p_link) {
		for (uint8_t i = 0; i < RTK_BT_LE_AUDIO_DEMO_SINK_ASE_MAX_NUM; i++) {
			if (p_link->lea_ase_tbl[i].used == true &&
				p_link->lea_ase_tbl[i].ase_id == ase_id) {
				return &p_link->lea_ase_tbl[i];
			}
		}
	}

	return NULL;
}

uint16_t bt_stack_le_audio_find_source_ase_num_by_conn_hdl(uint16_t conn_handle)
{
	uint16_t source_ase_num = 0;
	rtk_bt_le_audio_link_t *p_link = bt_stack_le_audio_find_link_by_conn_handle(conn_handle);

	if (p_link) {
		for (uint8_t i = 0; i < RTK_BT_LE_AUDIO_DEMO_SOURCE_ASE_MAX_NUM; i++) {
			if (p_link->lea_ase_tbl[i].used == true &&
				p_link->lea_ase_tbl[i].direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
				source_ase_num++;
			}
		}
	}
	return source_ase_num;
}

static rtk_bt_le_audio_ase_t *bt_stack_le_audio_add_ase(uint16_t conn_handle, uint8_t ase_id)
{
	rtk_bt_le_audio_link_t *p_link = bt_stack_le_audio_find_link_by_conn_handle(conn_handle);

	if (p_link) {
		for (uint8_t i = 0; i < RTK_BT_LE_AUDIO_DEMO_SINK_ASE_MAX_NUM; i++) {
			if (p_link->lea_ase_tbl[i].used == false) {
				p_link->lea_ase_tbl[i].used = true;
				p_link->lea_ase_tbl[i].ase_id = ase_id;
				return &p_link->lea_ase_tbl[i];
			}
		}
	}
	return NULL;
}

static void bt_stack_le_audio_update_metadata(rtk_bt_le_audio_ase_t *p_ase, uint8_t metadata_length, uint8_t *p_metadata, bool *p_ccid_update,
											  bool *p_contexts_update)
{
	uint16_t audio_contexts = 0;
	uint8_t ccid_num = 0;
	uint8_t ccid[RTK_BT_LE_AUDIO_DEMO_CCID_MAX_NUM];
	bool ccid_update = false;

	*p_ccid_update = false;
	*p_contexts_update = false;

	if (metadata_length != 0 && p_metadata != NULL) {
		uint16_t idx = 0;
		uint16_t length;
		uint8_t type;

		for (; idx < metadata_length;) {
			length = p_metadata[idx];
			idx ++;
			type = p_metadata[idx];
			switch (type) {
			case RTK_BT_LE_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS: {
				if (length == 3) {
					LE_ARRAY_TO_UINT16(audio_contexts, &p_metadata[idx + 1]);
				}
				break;
			}

			case RTK_BT_LE_METADATA_TYPE_CCID_LIST: {
				if (length > 1) {
					ccid_num = (length - 1);
					if (ccid_num > RTK_BT_LE_AUDIO_DEMO_CCID_MAX_NUM) {
						ccid_num = RTK_BT_LE_AUDIO_DEMO_CCID_MAX_NUM;
					}
					memcpy(ccid, &p_metadata[idx + 1], ccid_num);
				}
				break;
			}

			default:
				break;
			}
			idx += length;
		}
	}
	if (p_ase->audio_contexts != audio_contexts) {
		BT_LOGA("[LEA STACK] %s: update audio_contexts 0x%x \r\n", __func__, audio_contexts);
		*p_contexts_update = true;
		p_ase->audio_contexts = audio_contexts;
	}
	if (p_ase->ccid_num != ccid_num) {
		ccid_update = true;
	} else if (p_ase->ccid_num == ccid_num) {
		if (ccid_num != 0 && memcmp(p_ase->ccid, ccid, ccid_num) != 0) {
			ccid_update = true;
		}
	}
	if (ccid_update) {
		BT_LOGA("[LEA STACK] %s: ccid_update, ccid_num %d \r\n", __func__, ccid_num);
		p_ase->ccid_num = ccid_num;
		if (p_ase->ccid_num) {
			memcpy(p_ase->ccid, ccid, p_ase->ccid_num);
		}
	}
	*p_ccid_update = ccid_update;
}

void bt_stack_le_audio_demo_update_ase_state(T_ASCS_ASE_STATE *p_data)
{
	bool ccid_update = false;
	bool contexts_update = false;
	rtk_bt_le_audio_ase_t *p_ase = bt_stack_le_audio_find_ase(p_data->conn_handle, p_data->ase_data.ase_id);
	if (p_ase == NULL) {
		p_ase = bt_stack_le_audio_add_ase(p_data->conn_handle, p_data->ase_data.ase_id);
		if (p_ase == NULL) {
			BT_LOGE("[LEA STACK] %s bt_stack_le_audio_add_ase fail \r\n", __func__);
			return;
		}
		p_ase->direction = p_data->ase_data.direction;
	}
	p_ase->ase_state = p_data->ase_data.ase_state;
	if (p_data->ase_data.ase_state == RTK_BLE_AUDIO_ASCS_ASE_STATE_QOS_CONFIGURED) {
		p_ase->cig_id = p_data->ase_data.param.qos_configured.cig_id;
	}
	if (p_data->ase_data.ase_state == RTK_BLE_AUDIO_ASCS_ASE_STATE_IDLE ||
		p_data->ase_data.ase_state == RTK_BLE_AUDIO_ASCS_ASE_STATE_CODEC_CONFIGURED ||
		p_data->ase_data.ase_state == RTK_BLE_AUDIO_ASCS_ASE_STATE_QOS_CONFIGURED) {
		p_ase->ccid_num = 0;
		p_ase->audio_contexts = 0;
		bt_stack_le_audio_update_metadata(p_ase, 0, NULL, &ccid_update, &contexts_update);
	} else if (p_data->ase_data.ase_state == ASE_STATE_STREAMING) {
		bt_stack_le_audio_update_metadata(p_ase, p_data->ase_data.param.streaming.metadata_length,
										  p_data->ase_data.param.streaming.p_metadata,
										  &ccid_update, &contexts_update);
	}
}

void bt_stack_le_audio_gen_metadata(uint8_t *p_data, uint8_t *p_data_len, uint16_t contexts_type, uint8_t ccid_num, uint8_t *p_ccid)
{
	uint8_t idx = 0;

	if (!p_data || !p_data_len) {
		BT_LOGE("[LEA STACK] %s invalid input param \r\n", __func__);
		return;
	}
	p_data[idx++] = 0x03;
	p_data[idx++] = RTK_BT_LE_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS;
	LE_UINT16_TO_ARRAY(p_data + idx,
					   contexts_type);
	idx += 2;
	if (ccid_num && p_ccid != NULL) {
		p_data[idx++] = 1 + ccid_num;
		p_data[idx++] = RTK_BT_LE_METADATA_TYPE_CCID_LIST;
		memcpy(p_data + idx, p_ccid, ccid_num);
		idx += ccid_num;
	}
	*p_data_len = idx;
}

static bool bt_stack_le_audio_big_select_bis(rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info)
{
	p_sync_dev_info->bis_num = 0;
	if (p_sync_dev_info->support_bis_array != 0) {
		uint32_t support_bis_array = p_sync_dev_info->support_bis_array;
		uint8_t idx = 0;

		while (support_bis_array) {
			if (support_bis_array & 0x01) {
				p_sync_dev_info->bis_idx[p_sync_dev_info->bis_num] = (idx + 1);
				p_sync_dev_info->bis_num++;
				if (p_sync_dev_info->bis_num == RTK_BT_BIG_SYNC_MAX_BIS_NUM) {
					break;
				}
			}
			support_bis_array >>= 1;
			idx++;
		}
		if (p_sync_dev_info->bis_num) {
			return true;
		}
	} else {
		BT_LOGE("%s fail: support_bis_array is 0\r\n", __func__);
	}
	return false;
}

uint16_t bt_stack_le_audio_check_sync(rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info)
{
	BT_LOGD("[LEA STACK] %s: big_proc_flags 0x%x, pa_sync_state %d, big_sync_state %d\r\n", __func__,
			p_sync_dev_info->big_proc_flags, p_sync_dev_info->pa_sync_state, p_sync_dev_info->big_sync_state);
	if (p_sync_dev_info->big_proc_flags & RTK_BT_LE_AUDIO_BIG_PROC_BIG_SYNC_REQ) {
		if (p_sync_dev_info->big_sync_state == RTK_BT_LE_AUDIO_BIG_SYNC_STATE_TERMINATED) {
			if (p_sync_dev_info->pa_sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZED) {
				if (p_sync_dev_info->big_proc_flags & RTK_BT_LE_AUDIO_BIG_PROC_BIG_INFO_RECEIVED) {

					T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM sync_param = {
						.encryption = 0,
						.broadcast_code = {0},
						.mse = 0,
						.big_sync_timeout = 100, //10*N ms
						.num_bis = 0,
						.bis = {0}
					};
					sync_param.encryption = p_sync_dev_info->encryption;
					if (p_sync_dev_info->encryption) {
						if (p_sync_dev_info->big_proc_flags & RTK_BT_LE_AUDIO_BIG_PROC_BROADCAST_CODE_EXIST) {
							memcpy(sync_param.broadcast_code, p_sync_dev_info->broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
						} else {
							if (false == bass_send_broadcast_code_required(p_sync_dev_info->source_id)) {
								BT_LOGE("%s fail: bass_send_broadcast_code_required fail\r\n", __func__);
								return 1;
							}
						}
					}
					if (bt_stack_le_audio_big_select_bis(p_sync_dev_info)) {
						sync_param.num_bis = p_sync_dev_info->bis_num;
						memcpy(sync_param.bis, p_sync_dev_info->bis_idx, sync_param.num_bis);
						BT_LOGA("%s: encryption = %d,big_sync_timeout = %d, num_bis = %d, bis[0]= %d\r\n", __func__,
								sync_param.encryption, sync_param.big_sync_timeout, sync_param.num_bis, sync_param.bis[0]);
						if (false == ble_audio_big_sync_establish(p_sync_dev_info->sync_handle, &sync_param)) {
							BT_LOGE("%s ble_audio_big_sync_establish fail\r\n", __func__);
							return 1;
						}
					}
					return 0;
				}
			} else if (p_sync_dev_info->pa_sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_TERMINATED) {
				if (false == ble_audio_pa_sync_establish(p_sync_dev_info->sync_handle,
														 0,
														 0,
														 0,
														 1000)) {
					BT_LOGE("%s ble_audio_pa_sync_establish fail\r\n", __func__);
					return 1;
				} else {
					return 0;
				}
			}
		} else if (p_sync_dev_info->big_sync_state == RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZED) {
			p_sync_dev_info->big_proc_flags &= ~RTK_BT_LE_AUDIO_BIG_PROC_BIG_SYNC_REQ;
		}
	}

	return 0;
}

static bool bt_stack_le_audio_sync_audio_channel_support(uint32_t source_audio_location, uint32_t sink_audio_location)
{
	uint32_t chnl = 0;

	if (source_audio_location == 0) {
		return true;
	}

	chnl = source_audio_location;
	while (chnl) {
		if (chnl & sink_audio_location) {
			return true;
		}
		chnl = chnl >> 2;
	}

	return false;
}

uint16_t bt_stack_le_audio_bass_get_supported_bis(rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info)
{
	uint32_t support_bis_array = 0;
	T_BLE_AUDIO_SYNC_INFO sync_info = {0};
	T_BASE_DATA_SUBGROUP_PARAM *p_subgroup = NULL;
	T_BASE_DATA_BIS_PARAM *p_bis = NULL;
	uint8_t i = 0, j = 0;

	if (false == ble_audio_sync_get_info(p_sync_dev_info->sync_handle, &sync_info)) {
		BT_LOGE("%s ble_audio_sync_get_info fail\r\n", __func__);
		return 1;
	}
	if (sync_info.p_base_mapping == NULL || sync_info.p_base_mapping->p_subgroup == NULL) {
		BT_LOGE("%s sync_info.p_base_mapping == NULL || sync_info.p_base_mapping->p_subgroup == NULL\r\n", __func__);
		return 1;
	}
	for (i = 0; i < sync_info.p_base_mapping->num_subgroups; i++) {
		p_subgroup = &sync_info.p_base_mapping->p_subgroup[i];
		if (p_subgroup == NULL) {
			continue;
		}
		for (j = 0; j < p_subgroup->num_bis; j++) {
			p_bis = &p_subgroup->p_bis_param[j];
			if (p_bis == NULL) {
				continue;
			}
			if (pacs_check_codec_cfg(SERVER_AUDIO_SINK, p_bis->codec_id, &p_bis->bis_codec_cfg)) {
				BT_LOGD("%s p_bis->bis_codec_cfg.audio_channel_allocation=0x%x\r\n", __func__, (unsigned int)p_bis->bis_codec_cfg.audio_channel_allocation);
				if (bt_stack_le_audio_sync_audio_channel_support(p_bis->bis_codec_cfg.audio_channel_allocation, bt_le_audio_priv_data.lea_sink_audio_location)) {
					support_bis_array |= (1 << (sync_info.p_base_mapping->p_subgroup[i].p_bis_param[j].bis_index - 1));
					BT_LOGD("%s support_bis_array=0x%x\r\n", __func__, (unsigned int)support_bis_array);
				}
			}
		}
	}
	p_sync_dev_info->support_bis_array = support_bis_array;
	BT_LOGD("[LEA STACK] support_bis_array 0x%x\r\n", support_bis_array);

	return 0;
}

rtk_bt_le_audio_sync_dev_info_t *bt_stack_le_audio_sync_dev_find_by_source_id(uint8_t source_id)
{
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	rtk_bt_le_audio_list_t *p_list = &bt_le_audio_priv_data.sync_handle_queue;
	void *pmtx = p_list->mtx;
	struct list_head *iterator = NULL, *phead = &p_list->head;
	bool found = false;

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_sync_dev_info = list_entry(iterator, rtk_bt_le_audio_sync_dev_info_t, list);
			if (p_sync_dev_info == NULL) {
				continue;
			}
			if (p_sync_dev_info->source_id == source_id) {
				found = true;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);
	if (found) {
		return p_sync_dev_info;
	} else {
		return NULL;
	}
}

rtk_bt_le_audio_sync_dev_info_t *bt_stack_le_audio_sync_dev_find_by_addr(uint8_t *bd_addr, uint8_t bd_type)
{
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	rtk_bt_le_audio_list_t *p_list = &bt_le_audio_priv_data.sync_handle_queue;
	void *pmtx = p_list->mtx;
	struct list_head *iterator = NULL, *phead = &p_list->head;
	bool found = false;

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_sync_dev_info = list_entry(iterator, rtk_bt_le_audio_sync_dev_info_t, list);
			if (p_sync_dev_info == NULL) {
				continue;
			}
			if ((p_sync_dev_info->adv_addr.type == bd_type) && \
				(memcmp(p_sync_dev_info->adv_addr.addr_val, bd_addr, 6) == 0)) {
				found = true;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);
	if (found) {
		return p_sync_dev_info;
	} else {
		return NULL;
	}
}

rtk_bt_le_audio_sync_dev_info_t *bt_stack_le_audio_sync_dev_find(void *sync_handle)
{
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	rtk_bt_le_audio_list_t *p_list = &bt_le_audio_priv_data.sync_handle_queue;
	void *pmtx = p_list->mtx;
	struct list_head *iterator = NULL, *phead = &p_list->head;
	bool found = false;

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_sync_dev_info = list_entry(iterator, rtk_bt_le_audio_sync_dev_info_t, list);
			if (p_sync_dev_info == NULL) {
				continue;
			}
			if (p_sync_dev_info->sync_handle == sync_handle) {
				found = true;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);
	if (found) {
		return p_sync_dev_info;
	} else {
		return NULL;
	}
}

uint16_t bt_stack_le_audio_sync_dev_remove(void *sync_handle)
{
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	rtk_bt_le_audio_list_t *p_list = &bt_le_audio_priv_data.sync_handle_queue;
	void *pmtx = p_list->mtx;

	p_sync_dev_info = bt_stack_le_audio_sync_dev_find(sync_handle);
	if (!p_sync_dev_info) {
		BT_LOGE("[LEA STACK] %s sync_handle 0x%x not found \r\n", __func__, sync_handle);
		return RTK_BT_OK;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_del_init(&p_sync_dev_info->list);
	p_list->count --;
	osif_mutex_give(pmtx);
	osif_mem_free(p_sync_dev_info);
	BT_LOGA("[LEA STACK] %s remove sync_handle(%08x) from sync_handle list ok\r\n", __func__, sync_handle);

	return RTK_BT_OK;
}

uint16_t bt_stack_le_audio_sync_dev_remove_all(void)
{
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	struct list_head *phead = &bt_le_audio_priv_data.sync_handle_queue.head;
	struct list_head *plist = NULL, *pnext = NULL;

	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_sync_dev_info = (rtk_bt_le_audio_sync_dev_info_t *)plist;
		if (p_sync_dev_info) {
			bt_stack_le_audio_sync_dev_remove(p_sync_dev_info->sync_handle);
		}
		plist = pnext;
	}

	return RTK_BT_OK;
}

rtk_bt_le_audio_sync_dev_info_t *bt_stack_le_audio_sync_dev_add(void *sync_handle)
{
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	rtk_bt_le_audio_list_t *p_list = &bt_le_audio_priv_data.sync_handle_queue;
	void *pmtx = p_list->mtx;
	struct list_head *phead = &p_list->head;

	p_sync_dev_info = bt_stack_le_audio_sync_dev_find(sync_handle);
	if (p_sync_dev_info) {
		BT_LOGE("[LEA STACK] %s sync_handle 0x%x is already in list \r\n", __func__, sync_handle);
		return p_sync_dev_info;
	}
	p_sync_dev_info = (rtk_bt_le_audio_sync_dev_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(rtk_bt_le_audio_sync_dev_info_t));
	if (!p_sync_dev_info) {
		BT_LOGE("[LEA STACK] %s allocate info memory fail \r\n", __func__);
		return NULL;
	}
	memset(p_sync_dev_info, 0, sizeof(rtk_bt_le_audio_sync_dev_info_t));
	p_sync_dev_info->sync_handle = sync_handle;
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_add_tail(&p_sync_dev_info->list, phead);/* insert list */
	p_list->count ++;
	osif_mutex_give(pmtx);
	BT_LOGD("[LEA STACK] %s sync_handle(%08x) in sync_handle list ok\r\n", __func__, sync_handle);

	return p_sync_dev_info;
}

void bt_stack_le_audio_sync_list_show(void)
{
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	rtk_bt_le_audio_list_t *p_list = &bt_le_audio_priv_data.sync_handle_queue;
	void *pmtx = p_list->mtx;
	struct list_head *phead = &p_list->head;
	struct list_head *plist = NULL, *pnext = NULL;
	uint8_t sync_dev_num = 0;
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_sync_dev_info = (rtk_bt_le_audio_sync_dev_info_t *)plist;
		if (p_sync_dev_info) {
			BT_LOGA("[LEA STACK] %s sync dev[%d] : [%02x:%02x:%02x:%02x:%02x:%02x], type %d, source_id %d, pa_sync_state %d, big_sync_state %d\r\n", __func__, sync_dev_num,
					p_sync_dev_info->adv_addr.addr_val[5], p_sync_dev_info->adv_addr.addr_val[4], p_sync_dev_info->adv_addr.addr_val[3],
					p_sync_dev_info->adv_addr.addr_val[2], p_sync_dev_info->adv_addr.addr_val[1], p_sync_dev_info->adv_addr.addr_val[0],
					p_sync_dev_info->adv_addr.type, p_sync_dev_info->source_id, p_sync_dev_info->pa_sync_state, p_sync_dev_info->big_sync_state);

			sync_dev_num++;
		}
		plist = pnext;
	}
	osif_mutex_give(pmtx);
}

void *bt_stack_le_audio_link_find_brs_char_by_instance_id(rtk_bt_le_audio_link_t *p_link, uint8_t instance_id)
{
	if (instance_id < p_link->brs_char_num) {
		return &p_link->brs_char_tbl[instance_id];
	}
	return NULL;
}

void *bt_stack_le_audio_link_find_brs_char_by_sync_handle(rtk_bt_le_audio_link_t *p_link, void *sync_handle)
{
	for (uint8_t i = 0; i < p_link->brs_char_num; i++) {
		if (p_link->brs_char_tbl[i].brs_is_used &&
			p_link->brs_char_tbl[i].handle == sync_handle) {
			return &p_link->brs_char_tbl[i];
		}
	}

	return NULL;
}

void bt_stack_le_audio_link_update_brs_char(rtk_bt_le_audio_link_t *p_link, T_BASS_CLIENT_BRS_DATA *p_data)
{
	rtk_bt_le_audio_sync_dev_info_t *p_sync_info;
	rtk_bt_le_audio_bass_brs_char_info_t *p_brs_char = bt_stack_le_audio_link_find_brs_char_by_instance_id(p_link, p_data->char_instance_id);

	if (p_brs_char) {
		if (p_brs_char->brs_is_used != p_data->p_brs_data->brs_is_used) {
			if (p_data->p_brs_data->brs_is_used) {
				p_brs_char->brs_is_used = true;
				p_brs_char->source_id = p_data->p_brs_data->source_id;
				p_sync_info = bt_stack_le_audio_sync_dev_find_by_addr(p_data->p_brs_data->source_address,
																	  p_data->p_brs_data->source_address_type);
				if (p_sync_info) {
					BT_LOGA("[LEA STACK] %s: instance id: %d \r\n", __func__, p_brs_char->instance_id);
					p_brs_char->sync_handle = p_sync_info->sync_handle;
				}
			} else {
				p_brs_char->brs_is_used = false;
				p_brs_char->source_id = 0;
				p_brs_char->sync_handle = NULL;
			}
		}
	}
}

void bt_stack_le_audio_link_init_brs_char(rtk_bt_le_audio_link_t *p_link, uint8_t brs_char_num)
{
	if (p_link->brs_char_tbl) {
		osif_mem_free((void *)p_link->brs_char_tbl);
		p_link->brs_char_num = 0;
		p_link->brs_char_tbl = NULL;
	}
	if (brs_char_num) {
		p_link->brs_char_tbl = osif_mem_alloc(RAM_TYPE_DATA_ON, brs_char_num * sizeof(rtk_bt_le_audio_bass_brs_char_info_t));
		if (!p_link->brs_char_tbl) {
			BT_LOGE("[LEA STACK] %s: allocate char info fail \r\n", __func__);
		}
		{
			T_BASS_BRS_DATA *p_brs_data = NULL;
			rtk_bt_le_audio_sync_dev_info_t *p_sync_info;

			p_link->brs_char_num = brs_char_num;
			for (uint8_t i = 0; i < brs_char_num; i++) {
				p_link->brs_char_tbl[i].instance_id = i;
				p_brs_data = bass_get_brs_data(p_link->conn_handle, i);
				if (!p_brs_data) {
					BT_LOGE("[LEA STACK] %s get_brs_data fail for instance_id %d\n", __func__, i);
					continue;
				}
				if (p_brs_data) {
					if (p_brs_data->brs_is_used) {
						p_link->brs_char_tbl[i].brs_is_used = true;
						p_link->brs_char_tbl[i].source_id = p_brs_data->source_id;
						p_sync_info = bt_stack_le_audio_sync_dev_find_by_addr(p_brs_data->source_address,
																			  p_brs_data->source_address_type);
						if (p_sync_info) {
							BT_LOGA("[LEA STACK] %s: idx %d add sync", __func__, i);
							p_link->brs_char_tbl[i].handle = p_sync_info->sync_handle;
						}
					}
				}
				p_brs_data = NULL;
			}
		}
	}
}

rtk_bt_le_audio_group_info_t *bt_stack_le_audio_find_group_by_conn_handle(uint16_t conn_handle)
{
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;
	rtk_bt_le_audio_list_t *p_list = &bt_le_audio_priv_data.group_info_queue;
	void *pmtx = p_list->mtx;
	struct list_head *iterator = NULL, *phead = &p_list->head;
	rtk_bt_le_audio_device_handle_t device_handle = NULL;
	bool found = false;

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_group_info = list_entry(iterator, rtk_bt_le_audio_group_info_t, list);
			if (p_group_info == NULL) {
				continue;
			}
			device_handle = (rtk_bt_le_audio_device_handle_t *)ble_audio_group_find_dev_by_conn_handle(p_group_info->group_handle, conn_handle);
			if (device_handle) {
				found = true;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);
	if (found) {
		return p_group_info;
	} else {
		return NULL;
	}
}

rtk_bt_le_audio_group_info_t *bt_stack_le_audio_find_group_by_idx(uint8_t group_idx)
{
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;
	rtk_bt_le_audio_list_t *p_list = &bt_le_audio_priv_data.group_info_queue;
	void *pmtx = p_list->mtx;
	struct list_head *iterator = NULL, *phead = &p_list->head;
	bool found = false;

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_group_info = list_entry(iterator, rtk_bt_le_audio_group_info_t, list);
			if (p_group_info == NULL) {
				continue;
			}
			if (group_idx == p_group_info->group_idx) {
				found = true;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);
	if (found) {
		return p_group_info;
	} else {
		return NULL;
	}
}

rtk_bt_le_audio_group_info_t *bt_stack_le_audio_find_group(void *group_handle)
{
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;
	rtk_bt_le_audio_list_t *p_list = &bt_le_audio_priv_data.group_info_queue;
	void *pmtx = p_list->mtx;
	struct list_head *iterator = NULL, *phead = &p_list->head;
	bool found = false;

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_group_info = list_entry(iterator, rtk_bt_le_audio_group_info_t, list);
			if (p_group_info == NULL) {
				continue;
			}
			if (p_group_info->group_handle == group_handle) {
				found = true;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);
	if (found) {
		return p_group_info;
	} else {
		return NULL;
	}
}

uint16_t bt_stack_le_audio_remove_group(void *group_handle)
{
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;
	rtk_bt_le_audio_list_t *p_list = &bt_le_audio_priv_data.group_info_queue;
	void *pmtx = p_list->mtx;

	p_group_info = bt_stack_le_audio_find_group(group_handle);
	if (!p_group_info) {
		BT_LOGE("[LEA STACK] %s group_handle 0x%x not found \r\n", __func__, group_handle);
		return RTK_BT_OK;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_del_init(&p_group_info->list);
	p_list->count --;
	osif_mutex_give(pmtx);
	osif_mem_free(p_group_info);
	BT_LOGD("[LEA STACK] %s remove group_handle(%08x) from group_handle list ok\r\n", __func__, group_handle);

	return RTK_BT_OK;
}

uint16_t bt_stack_le_audio_remove_group_all(void)
{
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;
	struct list_head *phead = &bt_le_audio_priv_data.group_info_queue.head;
	struct list_head *plist = NULL, *pnext = NULL;

	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_group_info = (rtk_bt_le_audio_group_info_t *)plist;
		if (p_group_info) {
			bt_stack_le_audio_remove_group(p_group_info->group_handle);
		}
		plist = pnext;
	}

	return RTK_BT_OK;
}

rtk_bt_le_audio_group_info_t *bt_stack_le_audio_add_group(void *group_handle, uint8_t set_mem_size)
{
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;
	rtk_bt_le_audio_list_t *p_list = &bt_le_audio_priv_data.group_info_queue;
	void *pmtx = p_list->mtx;
	struct list_head *phead = &p_list->head;

	p_group_info = bt_stack_le_audio_find_group(group_handle);
	if (p_group_info) {
		BT_LOGE("[LEA STACK] %s group_handle 0x%x is already in list \r\n", __func__, group_handle);
		return p_group_info;
	}
	p_group_info = (rtk_bt_le_audio_group_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(rtk_bt_le_audio_group_info_t));
	if (!p_group_info) {
		BT_LOGE("[LEA STACK] %s allocate info memory fail \r\n", __func__);
		return NULL;
	}
	memset(p_group_info, 0, sizeof(rtk_bt_le_audio_group_info_t));
	p_group_info->group_handle = group_handle;
	p_group_info->set_mem_size = set_mem_size;
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_add_tail(&p_group_info->list, phead);/* insert list */
	p_group_info->group_idx = p_list->count;
	p_list->count ++;
	osif_mutex_give(pmtx);
	BT_LOGA("[LEA STACK] %s group_handle(%08x idx 0x%02x) in group_handle list ok\r\n", __func__, group_handle, p_group_info->group_idx);

	return p_group_info;
}

rtk_bt_le_audio_iso_channel_info_t *bt_stack_le_audio_find_iso_chann(uint16_t iso_conn_handle, uint8_t direction)
{
	rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;
	rtk_bt_le_audio_list_t *p_list = NULL;
	void *pmtx = NULL;
	struct list_head *iterator = NULL, *phead = NULL;
	bool found = false;

	if (direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
		p_list = &bt_le_audio_priv_data.iso_input_queue;
	} else {
		p_list = &bt_le_audio_priv_data.iso_output_queue;
	}
	pmtx = p_list->mtx;
	phead = &p_list->head;
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_iso_chann = list_entry(iterator, rtk_bt_le_audio_iso_channel_info_t, list);
			if (p_iso_chann == NULL) {
				continue;
			}
			if (p_iso_chann->iso_conn_handle == iso_conn_handle) {
				found = true;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);
	if (found) {
		return p_iso_chann;
	} else {
		return NULL;
	}
}

/* for continuing handling */
uint16_t bt_stack_le_audio_hold_iso_chann(rtk_bt_le_audio_iso_channel_info_t *p_iso_chann)
{
	rtk_bt_le_audio_list_t *p_list = NULL;
	void *pmtx = NULL;
	uint16_t ret = 1;

	if (!p_iso_chann) {
		BT_LOGE("[LEA STACK] %s p_iso_chann is NULL \r\n", __func__);
		return ret;
	} else {
		if (p_iso_chann->path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
			p_list = &bt_le_audio_priv_data.iso_input_queue;
		} else {
			p_list = &bt_le_audio_priv_data.iso_output_queue;
		}
	}
	pmtx = p_list->mtx;
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	while (p_iso_chann->is_processing) {
		/* free mutex and give schedule for processing flow */
		osif_mutex_give(pmtx);
		osif_delay(1);
		osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	}
	p_iso_chann->is_processing = true;
	ret = 0;
	osif_mutex_give(pmtx);

	return ret;
}

/* release continuing handling */
void bt_stack_le_audio_release_iso_chann(rtk_bt_le_audio_iso_channel_info_t *p_iso_chann)
{
	rtk_bt_le_audio_list_t *p_list = NULL;
	void *pmtx = NULL;

	if (!p_iso_chann) {
		BT_LOGE("[LEA STACK] %s p_iso_chann is NULL \r\n", __func__);
		return;
	} else {
		if (p_iso_chann->path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
			p_list = &bt_le_audio_priv_data.iso_input_queue;
		} else {
			p_list = &bt_le_audio_priv_data.iso_output_queue;
		}
	}
	pmtx = p_list->mtx;
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	p_iso_chann->is_processing = false;
	osif_mutex_give(pmtx);
}

rtk_bt_le_audio_iso_channel_info_t *bt_stack_le_audio_add_iso_chann(uint16_t iso_conn_handle, uint8_t direction)
{
	rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;
	rtk_bt_le_audio_list_t *p_list = NULL;
	void *pmtx = NULL;
	struct list_head *phead = NULL;

	if (direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
		p_list = &bt_le_audio_priv_data.iso_input_queue;
	} else {
		p_list = &bt_le_audio_priv_data.iso_output_queue;
	}
	pmtx = p_list->mtx;
	phead = &p_list->head;
	p_iso_chann = bt_stack_le_audio_find_iso_chann(iso_conn_handle, direction);
	if (p_iso_chann) {
		return p_iso_chann;
	}
	p_iso_chann = osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(rtk_bt_le_audio_iso_channel_info_t));
	if (p_iso_chann) {
		memset((void *)p_iso_chann, 0, sizeof(rtk_bt_le_audio_iso_channel_info_t));
		p_iso_chann->iso_conn_handle = iso_conn_handle;
		p_iso_chann->path_direction = direction;
		INIT_LIST_HEAD(&p_iso_chann->list);
		osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
		list_add_tail(&p_iso_chann->list, phead);
		p_list->count ++;
		osif_mutex_give(pmtx);
		BT_LOGA("[LEA STACK] %s iso_conn_handle 0x%x, direction %d \r\n", __func__, iso_conn_handle, direction);
	}
	return p_iso_chann;
}

void bt_stack_le_audio_remove_iso_chann(uint16_t iso_conn_handle, uint8_t direction)
{
	rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;
	rtk_bt_le_audio_list_t *p_list = NULL;
	void *pmtx = NULL;
	struct list_head *iterator = NULL, *phead = NULL;
	bool found = false;

	if (direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
		p_list = &bt_le_audio_priv_data.iso_input_queue;
	} else {
		p_list = &bt_le_audio_priv_data.iso_output_queue;
	}
	pmtx = p_list->mtx;
	phead = &p_list->head;
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_iso_chann = list_entry(iterator, rtk_bt_le_audio_iso_channel_info_t, list);
			if (p_iso_chann == NULL) {
				continue;
			}
			if (p_iso_chann->iso_conn_handle == iso_conn_handle) {
				found = true;
				break;
			}
		}
	}
	if (!found) {
		BT_LOGE("[LEA STACK] %s iso_conn_handle 0x%x, direction %d is not in list \r\n", __func__, iso_conn_handle, direction);
		osif_mutex_give(pmtx);
		return;
	}
	while (p_iso_chann->is_processing) {
		/* free mutex and give schedule for processing flow */
		osif_mutex_give(pmtx);
		osif_delay(1);
		osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	}
	list_del_init(&p_iso_chann->list);
	p_list->count --;
	osif_mem_free(p_iso_chann);
	osif_mutex_give(pmtx);
}

static rtk_bt_le_audio_iso_channel_info_t *bt_stack_le_audio_handle_data_path_setup(rtk_bt_le_audio_setup_data_path_info_t *p_data)
{
	rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = bt_stack_le_audio_find_iso_chann(p_data->iso_conn_handle,
																					   p_data->path_direction);
	uint8_t chnl_cnt;
	uint8_t blocks_num = 1;

	if (p_iso_chann != NULL) {
		BT_LOGE("[LEA STACK] %s iso channel already exist \r\n", __func__);
		return NULL;
	} else {
		p_iso_chann = bt_stack_le_audio_add_iso_chann(p_data->iso_conn_handle,
													  p_data->path_direction);
		if (p_iso_chann == NULL) {
			BT_LOGE("[LEA STACK] %s bt_stack_le_audio_add_iso_chann fail \r\n", __func__);
			return NULL;
		}
	}
	/* hold iso channel and do configuration */
	bt_stack_le_audio_hold_iso_chann(p_iso_chann);
	p_iso_chann->iso_mode = p_data->iso_mode;
	p_iso_chann->codec_data = p_data->codec_parsed_data;
	p_iso_chann->presentation_delay = p_data->presentation_delay;
	p_iso_chann->pkt_seq_num = 0;
	if (p_iso_chann->codec_data.audio_channel_allocation == AUDIO_LOCATION_MONO) {
		chnl_cnt = 1;
	} else {
		chnl_cnt = __builtin_popcount(p_iso_chann->codec_data.audio_channel_allocation);
	}
	if (p_iso_chann->codec_data.type_exist & RTK_BT_LE_CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST) {
		blocks_num = p_iso_chann->codec_data.codec_frame_blocks_per_sdu;
	}
	p_iso_chann->frame_num = blocks_num * chnl_cnt;
	BT_LOGA("[LEA STACK] %s: frame_num %d, sample_frequency 0x%x, audio_channel_allocation 0x%02x, presentation_delay: %u \r\n",
			__func__,
			p_iso_chann->frame_num,
			p_iso_chann->codec_data.sample_frequency,
			p_iso_chann->codec_data.audio_channel_allocation,
			p_iso_chann->presentation_delay);
	/* release iso channel */
	bt_stack_le_audio_release_iso_chann(p_iso_chann);

	return p_iso_chann;
}

static rtk_bt_le_audio_iso_channel_info_t *bt_stack_le_audio_bap_data_path_setup(rtk_bt_le_audio_bap_setup_data_path_ind_t *p_data, uint8_t iso_mode,
																				 uint8_t direction)
{
	(void)direction;
	rtk_bt_le_audio_setup_data_path_info_t data = {0};
	rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;
	T_AUDIO_SESSION_QOS_CFG session_qos = {0};

	data.iso_mode = iso_mode;
	data.iso_conn_handle = p_data->cis_conn_handle;
	memcpy((void *)&data.codec_parsed_data, (void *)&p_data->codec_parsed_data, sizeof(rtk_bt_le_audio_cfg_codec_t));
	data.path_direction = p_data->path_direction;
	data.presentation_delay = 40000;
	{
		if (false == bap_unicast_audio_get_session_qos(p_data->stream_session_handle, &session_qos)) {
			BT_LOGE("%s fail: bap_unicast_audio_get_session_qos fail\r\n", __func__);
		} else {
			if (RTK_BLE_AUDIO_ISO_DATA_PATH_TX == p_data->path_direction) {
				data.presentation_delay = session_qos.sink_presentation_delay;
			} else {
				data.presentation_delay = session_qos.source_presentation_delay;
			}
		}
	}
	p_iso_chann = bt_stack_le_audio_handle_data_path_setup(&data);
	if (!p_iso_chann) {
		BT_LOGE("[LEA STACK] %s fail \r\n", __func__);
		return NULL;
	}

	return p_iso_chann;
}

static rtk_bt_le_audio_iso_channel_info_t *bt_stack_le_audio_ascs_data_path_setup(T_ASCS_SETUP_DATA_PATH *p_data, uint8_t iso_mode, uint8_t direction)
{
	(void)direction;
	rtk_bt_le_audio_setup_data_path_info_t data = {0};
	T_ASE_QOS_CFG_STATE_DATA cfg_data;
	rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;

	data.iso_mode = iso_mode;
	data.iso_conn_handle = p_data->cis_conn_handle;
	memcpy((void *)&data.codec_parsed_data, (void *)&p_data->codec_parsed_data, sizeof(rtk_bt_le_audio_cfg_codec_t));
	data.path_direction = p_data->path_direction;
	data.presentation_delay = 40000;
	if (ascs_get_qos_cfg(p_data->conn_handle, p_data->ase_id, (T_ASE_QOS_CFG_STATE_DATA *)&cfg_data)) {
		data.presentation_delay = cfg_data.presentation_delay[0] | cfg_data.presentation_delay[1] << 8 |
								  cfg_data.presentation_delay[2] << 16;
	}
	p_iso_chann = bt_stack_le_audio_handle_data_path_setup(&data);
	if (!p_iso_chann) {
		BT_LOGE("[LEA STACK] %s fail \r\n", __func__);
		return NULL;
	}

	return p_iso_chann;
}

static rtk_bt_le_audio_iso_channel_info_t *bt_stack_le_audio_broadcast_source_path_setup(rtk_bt_le_audio_bis_conn_handle_info_t *p_data, uint8_t iso_mode,
																						 uint8_t direction)
{
	rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;
	rtk_bt_le_audio_setup_data_path_info_t data = {0};

	data.iso_mode = iso_mode;
	data.iso_conn_handle = p_data->bis_conn_handle;
	data.idx = p_data->bis_idx;
	data.path_direction = direction;
	data.presentation_delay = bt_le_audio_priv_data.bsrc.prefer_qos.presentation_delay;
	memcpy((void *)&data.codec_parsed_data, (void *)&bt_le_audio_priv_data.bsrc.codec_cfg, sizeof(rtk_bt_le_audio_cfg_codec_t));

	for (uint8_t i = 0; i < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_GROUP_NUM; i ++) {
		for (uint8_t j = 0; j < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_SUB_GROUP_NUM; j ++) {
			for (uint8_t k = 0; k < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_NUM; k ++) {
				if (bt_le_audio_priv_data.bsrc.group[i].sub_group[j].bis_idx[k] == p_data->bis_idx) {
					if (k == 0) {
#if (RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_NUM == 1) && (RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE == 1)
						data.codec_parsed_data.audio_channel_allocation = RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR;
						bt_le_audio_priv_data.bsrc.codec_cfg.audio_channel_allocation = RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR;
#else
						data.codec_parsed_data.audio_channel_allocation = RTK_BT_LE_AUDIO_LOCATION_FL;
						bt_le_audio_priv_data.bsrc.codec_cfg.audio_channel_allocation = RTK_BT_LE_AUDIO_LOCATION_FL;
#endif
					} else if (k == 1) {
						data.codec_parsed_data.audio_channel_allocation = RTK_BT_LE_AUDIO_LOCATION_FR;
						bt_le_audio_priv_data.bsrc.codec_cfg.audio_channel_allocation = RTK_BT_LE_AUDIO_LOCATION_FR;
					}
					break;
				}
			}
		}
	}
	p_iso_chann = bt_stack_le_audio_handle_data_path_setup(&data);
	if (!p_iso_chann) {
		BT_LOGE("[LEA STACK] %s bt_stack_le_audio_handle_data_path_setup fail \r\n", __func__);
		return NULL;
	}

	return p_iso_chann;
}

static rtk_bt_le_audio_iso_channel_info_t *bt_stack_le_audio_big_setup_data_path_setup(rtk_bt_le_audio_bis_conn_info_t *p_data, uint8_t iso_mode,
																					   uint8_t direction)
{
	(void)direction;
	rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;
	rtk_bt_le_audio_setup_data_path_info_t data = {0};
	T_BLE_AUDIO_SYNC_INFO sync_info = {0};

	data.iso_mode = iso_mode;
	data.iso_conn_handle = p_data->bis_conn_handle;
	data.idx = p_data->bis_idx;
	data.path_direction = p_data->path_direction;
	data.presentation_delay = 40000;
	{
		if (false == ble_audio_sync_get_info(p_data->sync_handle, &sync_info)) {
			BT_LOGE("%s ble_audio_sync_get_info fail\r\n", __func__);
			return NULL;
		}
		if (false == base_data_get_bis_codec_cfg(sync_info.p_base_mapping, p_data->bis_idx, (T_CODEC_CFG *)&data.codec_parsed_data)) {
			BT_LOGE("%s base_data_get_bis_codec_cfg fail\r\n", __func__);
			return NULL;
		}
		if (sync_info.p_base_mapping) {
			data.presentation_delay = sync_info.p_base_mapping->presentation_delay;
			BT_LOGA("[LEA STACK] %s get base mapping presentation delay %d \r\n", __func__, data.presentation_delay);
		}
	}
	p_iso_chann = bt_stack_le_audio_handle_data_path_setup(&data);
	if (!p_iso_chann) {
		BT_LOGE("[LEA STACK] %s bt_stack_le_audio_handle_data_path_setup fail \r\n", __func__);
		return NULL;
	}

	return p_iso_chann;
}

rtk_bt_le_audio_iso_channel_info_t *bt_stack_le_audio_data_path_add(uint16_t event, uint8_t iso_mode, uint8_t direction, void *p_data)
{
	rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;

	switch (event) {
	case AUDIO_GROUP_MSG_BAP_SETUP_DATA_PATH: {
		if (!p_data) {
			BT_LOGE("[LEA STACK] %s AUDIO_GROUP_MSG_BAP_SETUP_DATA_PATH p_data is NULL \r\n", __func__);
			break;
		}
		p_iso_chann = bt_stack_le_audio_bap_data_path_setup((rtk_bt_le_audio_bap_setup_data_path_ind_t *)p_data, iso_mode, direction);
		break;
	}
	case LE_AUDIO_MSG_ASCS_SETUP_DATA_PATH: {
		if (!p_data) {
			BT_LOGE("[LEA STACK] %s LE_AUDIO_MSG_ASCS_SETUP_DATA_PATH p_data is NULL \r\n", __func__);
			break;
		}
		p_iso_chann = bt_stack_le_audio_ascs_data_path_setup((T_ASCS_SETUP_DATA_PATH *)p_data, iso_mode, direction);
		break;
	}
	case MSG_BROADCAST_SOURCE_SETUP_DATA_PATH: {
		if (!p_data) {
			BT_LOGE("[LEA STACK] %s MSG_BROADCAST_SOURCE_SETUP_DATA_PATH p_data is NULL \r\n", __func__);
			break;
		}
		p_iso_chann = bt_stack_le_audio_broadcast_source_path_setup((rtk_bt_le_audio_bis_conn_handle_info_t *)p_data, iso_mode, direction);
		break;
	}
	case MSG_BLE_AUDIO_BIG_SETUP_DATA_PATH: {
		if (!p_data) {
			BT_LOGE("[LEA STACK] %s: MSG_BLE_AUDIO_BIG_SETUP_DATA_PATH p_data is NULL \r\n", __func__);
			break;
		}
		p_iso_chann = bt_stack_le_audio_big_setup_data_path_setup((rtk_bt_le_audio_bis_conn_info_t *)p_data, iso_mode, direction);
		break;
	}
	default:
		BT_LOGE("[LEA STACK] %s: invalid event 0x%x \r\n", __func__, event);
		break;
	}

	return p_iso_chann;
}

void bt_stack_le_audio_handle_data_path_remove(uint16_t cis_conn_handle, uint8_t path_direction)
{
	rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = bt_stack_le_audio_find_iso_chann(cis_conn_handle, path_direction);
	if (p_iso_chann != NULL) {
		bt_stack_le_audio_remove_iso_chann(p_iso_chann->iso_conn_handle, p_iso_chann->path_direction);
	}
}

uint16_t bt_stack_le_audio_handle_data_path_remove_all(void)
{
	rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;
	struct list_head *phead = NULL;
	struct list_head *plist = NULL, *pnext = NULL;

	/* remove input path */
	phead = &bt_le_audio_priv_data.iso_input_queue.head;
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_iso_chann = (rtk_bt_le_audio_iso_channel_info_t *)plist;
		if (p_iso_chann) {
			bt_stack_le_audio_handle_data_path_remove(p_iso_chann->iso_conn_handle, p_iso_chann->path_direction);
		}
		plist = pnext;
	}
	/* remove output path */
	phead = &bt_le_audio_priv_data.iso_output_queue.head;
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_iso_chann = (rtk_bt_le_audio_iso_channel_info_t *)plist;
		if (p_iso_chann) {
			bt_stack_le_audio_handle_data_path_remove(p_iso_chann->iso_conn_handle, p_iso_chann->path_direction);
		}
		plist = pnext;
	}

	return RTK_BT_OK;
}

static void bt_stack_le_audio_init_queue(rtk_bt_le_audio_list_t *p_queue)
{
	if (!p_queue) {
		return;
	}
	p_queue->count = 0;
	if (p_queue->mtx == NULL) {
		osif_mutex_create(&p_queue->mtx);
	}
	INIT_LIST_HEAD(&p_queue->head);
}

static void bt_stack_le_audio_deinit_queue(rtk_bt_le_audio_list_t *p_queue)
{
	if (!p_queue) {
		return;
	}
	if (p_queue->mtx != NULL) {
		osif_mutex_delete(p_queue->mtx);
		p_queue->mtx = NULL;
	}
	p_queue->count = 0;
	INIT_LIST_HEAD(&p_queue->head);
}

uint16_t bt_stack_le_audio_common_init(void)
{
	if (bt_le_audio_priv_data.init_flag) {
		BT_LOGA("%s: bt_le_audio_priv_data is already initialized \r\n", __func__);
		return 0;
	}
	memset((void *)&bt_le_audio_priv_data, 0, sizeof(rtk_bt_le_audio_priv_t));
	bt_stack_le_audio_init_queue(&bt_le_audio_priv_data.iso_input_queue);
	bt_stack_le_audio_init_queue(&bt_le_audio_priv_data.iso_output_queue);
	bt_stack_le_audio_init_queue(&bt_le_audio_priv_data.sync_handle_queue);
	bt_stack_le_audio_init_queue(&bt_le_audio_priv_data.group_info_queue);
	bt_le_audio_priv_data.init_flag = true;

	return 1;
}

void bt_stack_le_audio_common_deinit(void)
{
	if (!bt_le_audio_priv_data.init_flag) {
		BT_LOGA("%s: bt_le_audio_priv_data is already deinit \r\n", __func__);
		return;
	}
	bt_le_audio_priv_data.init_flag = false;
	bt_stack_le_audio_remove_group_all();
	bt_stack_le_audio_sync_dev_remove_all();
	bt_stack_le_audio_handle_data_path_remove_all();
	bt_stack_le_audio_deinit_queue(&bt_le_audio_priv_data.iso_input_queue);
	bt_stack_le_audio_deinit_queue(&bt_le_audio_priv_data.iso_output_queue);
	bt_stack_le_audio_deinit_queue(&bt_le_audio_priv_data.sync_handle_queue);
	bt_stack_le_audio_deinit_queue(&bt_le_audio_priv_data.group_info_queue);
}

uint16_t bt_stack_le_audio_init(rtk_bt_app_conf_t *papp_conf, void *io_msg_q, void *evt_msg_q)
{
	rtk_bt_le_audio_app_conf_t *p_le_audio_app_conf = NULL;
	T_BLE_AUDIO_PARAMS ble_audio_param = {0};
	T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;
	bool cis_flag = true;

	if (papp_conf == NULL) {
		BT_LOGE("%s papp_conf is NULL\r\n", __func__);
		return 1;
	}
	p_le_audio_app_conf = &papp_conf->le_audio_app_conf;
	//set cis support when unicast enable
	if ((p_le_audio_app_conf->bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER) || (p_le_audio_app_conf->bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI)) {
		cause = le_set_gap_param(GAP_PARAM_CIS_HOST_SUPPORT, sizeof(cis_flag), &cis_flag);
		if (cause != GAP_CAUSE_SUCCESS) {
			BT_LOGE("%s le_set_gap_param fail (cause = 0x%x)\r\n", __func__, cause);
			return false;
		}
	}
	ble_audio_param.evt_queue_handle = evt_msg_q;
	ble_audio_param.io_queue_handle = io_msg_q;
	if ((papp_conf->app_profile_support & RTK_BT_PROFILE_GATTC) == 0) {
#if (defined(RTK_BT_5_2_EATT_SUPPORT) && RTK_BT_5_2_EATT_SUPPORT) && (defined(F_BT_5_2_EATT_SUPPORT) && F_BT_5_2_EATT_SUPPORT)
		ble_audio_param.bt_gatt_client_init = (GATT_CLIENT_DISCOV_MODE_REG_SVC_BIT |
											   GATT_CLIENT_DISCOV_MODE_CCCD_STORAGE_BIT |
											   GATT_CLIENT_DISCOV_MODE_USE_EXT_CLIENT |
											   GATT_CLIENT_DISCOV_MODE_GATT_SVC);
		gatt_client_cfg_client_supported_feature(GATT_SVC_CLIENT_SUPPORTED_FEATURES_EATT_BEARER_BIT);
#else
		ble_audio_param.bt_gatt_client_init = (GATT_CLIENT_DISCOV_MODE_REG_SVC_BIT | GATT_CLIENT_DISCOV_MODE_CCCD_STORAGE_BIT);
#endif
	}
	ble_audio_param.acl_link_num = le_get_max_link_num();
	ble_audio_param.io_event_type = IO_MSG_TYPE_LE_AUDIO;
	if (ble_audio_init(&ble_audio_param) == false) {
		BT_LOGE("%s:ble_audio_init fail\r\n", __func__);
		return 1;
	}
	bt_stack_le_audio_common_init();
	bt_le_audio_priv_data.lea_sink_available_contexts = p_le_audio_app_conf->pacs_param.sink_available_contexts;
	bt_le_audio_priv_data.lea_source_available_contexts = p_le_audio_app_conf->pacs_param.source_available_contexts;
	bt_le_audio_priv_data.lea_sink_supported_contexts = p_le_audio_app_conf->pacs_param.sink_supported_contexts;
	bt_le_audio_priv_data.lea_source_supported_contexts = p_le_audio_app_conf->pacs_param.source_supported_contexts;
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
	bt_le_audio_priv_data.csis_cfg = p_le_audio_app_conf->cap_param.csis_param.csis_cfg;
	/* memcpy csis_sirk */
	memcpy((void *)bt_le_audio_priv_data.lea_csis_sirk, (void *)p_le_audio_app_conf->cap_param.csis_param.csis_sirk, RTK_BT_LE_CSIS_SIRK_LEN);
#endif
	bt_le_audio_priv_data.lea_sink_audio_location = p_le_audio_app_conf->pacs_param.sink_audio_location;
	bt_le_audio_priv_data.lea_source_audio_location = p_le_audio_app_conf->pacs_param.source_audio_location;
	bt_le_audio_priv_data.bap_role = (rtk_bt_le_audio_bap_role_t)p_le_audio_app_conf->bap_role;
	bt_le_audio_priv_data.cap_role = (rtk_bt_le_audio_cap_role_t)p_le_audio_app_conf->cap_role;
	memcpy((void *)&bt_le_audio_priv_data.lea_app_conf, (void *)p_le_audio_app_conf, sizeof(rtk_bt_le_audio_app_conf_t));
	/* memcpy device name */
	memcpy((void *)bt_stack_device_name, (void *)p_le_audio_app_conf->device_name, p_le_audio_app_conf->device_name_len);
	device_name_len = p_le_audio_app_conf->device_name_len;

	return 0;
}

void bt_stack_le_audio_deinit(void)
{
	BT_LOGD("%s\n", __func__);

	gap_register_direct_cb(NULL);
	ble_audio_deinit();
	bt_stack_le_audio_common_deinit();
}

#endif //end of #if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT