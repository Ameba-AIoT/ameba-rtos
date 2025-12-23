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
#include <rtk_stack_le_audio_internal.h>
#include <rtk_stack_config.h>
#include <rtk_bt_le_audio_def.h>
#include <rtk_bt_bap.h>
#include <trace_app.h>
#include <bt_direct_msg.h>
#include <gap_iso_data.h>
#include <gap.h>
#include <gap_ext_scan.h>
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
#include <gap_cig_mgr.h>

static uint8_t stack_bap_init_flag = 0;
static uint8_t default_sink_ase_num = 1;
static uint8_t default_source_ase_num = 1;
extern rtk_bt_le_audio_priv_t bt_le_audio_priv_data;
extern uint8_t bt_stack_device_name[];
extern uint8_t device_name_len;

static rtk_bt_le_ext_scan_param_t bt_stack_le_audio_def_ext_scan_param = {
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.phys = {true, true},
	.type = {RTK_BT_LE_SCAN_TYPE_ACTIVE, RTK_BT_LE_SCAN_TYPE_ACTIVE},
	.interval = {108, 108},
	.window = {54, 54},
	.duration = 0,
	.period = 0,
	.filter_policy = RTK_BT_LE_SCAN_FILTER_ALLOW_ALL,
	.duplicate_opt = RTK_BT_LE_SCAN_DUPLICATE_ENABLE,
};
static uint16_t bt_stack_le_audio_ext_scan_act(void *param);
static uint8_t bt_stack_le_audio_broadcast_code[RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN] = {0x82, 0x31, 0x5C, 0x28, 0xF6, 0x1B, 0x19, 0x58, 0x2D, 0x5E, 0x20, 0xD1, 0x23, 0x33, 0xD0, 0x8E};
static rtk_bt_le_audio_broadcast_source_config_t bt_le_audio_broadcast_source_config_param = {
	.primary_adv_interval_min = RTK_BT_LE_AUDIO_PRIMARY_ADV_INTERVAL_MIN,
	.primary_adv_interval_max = RTK_BT_LE_AUDIO_PRIMARY_ADV_INTERVAL_MAX,
	.primary_adv_channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.own_addr = {0},                /**<Set it in main. */
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
	.tx_power = 0x7F,
	.primary_adv_phy = RTK_BT_LE_PHYS_PRIM_ADV_1M,
	.secondary_adv_max_skip = 0,
	.secondary_adv_phy = RTK_BT_LE_PHYS_2M,
	.periodic_adv_interval_min = RTK_BT_LE_AUDIO_PA_INTERVAL_MIN,
	.periodic_adv_interval_max = RTK_BT_LE_AUDIO_PA_INTERVAL_MAX,
	.periodic_adv_prop = 0
};
/* this config will be overwrited by bt_stack_le_audio_gen_basic_data */
static uint8_t bt_stack_le_audio_media_codec_level2[] = {
	0x02,
	RTK_BT_LE_CODEC_CFG_TYPE_SAMPLING_FREQUENCY,
	RTK_BT_LE_AUDIO_BIRDS_SING_SAMPLING_RATE,
	0x02,
	RTK_BT_LE_CODEC_CFG_TYPE_FRAME_DURATION,
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_FRAME_DURATION,
	0x03,
	RTK_BT_LE_CODEC_CFG_TYPE_OCTET_PER_CODEC_FRAME,
	0x28, 0x00,
	0x02,
	RTK_BT_LE_CODEC_CFG_TYPE_BLOCKS_PER_SDU,
	1
};
#if (RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_NUM == 1) && (RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE == 1)
static const uint8_t bt_stack_le_audio_media_codec_level3_left[] = {
	0x05,
	RTK_BT_LE_CODEC_CFG_TYPE_AUDIO_CHANNEL_ALLOCATION,
	(uint8_t)(RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR),
	(uint8_t)(((RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR) >> 8) & 0xFF),
	(uint8_t)(((RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR) >> 16) & 0xFF),
	(uint8_t)(((RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR) >> 24) & 0xFF),
};
#else
static const uint8_t bt_stack_le_audio_media_codec_level3_left[] = {
	0x05,
	RTK_BT_LE_CODEC_CFG_TYPE_AUDIO_CHANNEL_ALLOCATION,
	(uint8_t)RTK_BT_LE_AUDIO_LOCATION_FL,
	(uint8_t)((RTK_BT_LE_AUDIO_LOCATION_FL >> 8) & 0xFF),
	(uint8_t)((RTK_BT_LE_AUDIO_LOCATION_FL >> 16) & 0xFF),
	(uint8_t)((RTK_BT_LE_AUDIO_LOCATION_FL >> 24) & 0xFF),
};
#endif
static const uint8_t bt_stack_le_audio_media_codec_level3_right[] = {
	0x05,
	RTK_BT_LE_CODEC_CFG_TYPE_AUDIO_CHANNEL_ALLOCATION,
	(uint8_t)RTK_BT_LE_AUDIO_LOCATION_FR,
	(uint8_t)((RTK_BT_LE_AUDIO_LOCATION_FR >> 8) & 0xFF),
	(uint8_t)((RTK_BT_LE_AUDIO_LOCATION_FR >> 16) & 0xFF),
	(uint8_t)((RTK_BT_LE_AUDIO_LOCATION_FR >> 24) & 0xFF),
};
static rtk_bt_le_audio_unicast_ase_qos_t bt_le_audio_bap_ase_qos = {
	.phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M,
	.retransmission_number = RTK_BT_LE_AUDIO_DEMO_ASE_QOS_RETRANS_NUM,
	.max_sdu = RTK_BT_LE_AUDIO_DEMO_ASE_QOS_MAX_SDU,
};

static uint8_t bt_le_audio_metadata[RTK_BT_LE_AUDIO_DEMO_CAP_METADATA_DATA_MAX_LEN];
static uint8_t bt_le_audio_metadata_len = 0;

static void bt_stack_le_audio_sync_cb(T_BLE_AUDIO_SYNC_HANDLE sync_handle, uint8_t cb_type, void *p_cb_data);

void bt_stack_le_audio_group_cb(T_AUDIO_GROUP_MSG msg, T_BLE_AUDIO_GROUP_HANDLE handle, void *buf);

static uint16_t rtk_stack_le_audio_bap_msg_cback(T_LE_AUDIO_MSG msg, void *buf)
{
	rtk_bt_evt_t *p_evt = NULL;
	uint8_t cb_ret = 0;
	rtk_bt_le_audio_link_t *p_link = NULL;
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	uint16_t app_result = BLE_AUDIO_CB_RESULT_SUCCESS;

	switch (msg) {

	case LE_AUDIO_MSG_BAP_DIS_ALL_DONE: {
		rtk_bt_le_audio_bap_discovery_done_ind_t *p_ind = NULL;
		T_BAP_DIS_ALL_DONE *p_data = (T_BAP_DIS_ALL_DONE *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_BAP_DIS_ALL_DONE: conn_handle 0x%x, pacs_is_found %d, sink_pac_num %d, source_pac_num %d",
						p_data->conn_handle,
						p_data->pacs_is_found,
						p_data->sink_pac_num,
						p_data->source_pac_num);
		APP_PRINT_INFO5("LE_AUDIO_MSG_BAP_DIS_ALL_DONE: ascs_is_found %d, sink_ase_num %d, source_ase_num %d, bass_is_found %d, brs_char_num %d",
						p_data->ascs_is_found,
						p_data->sink_ase_num,
						p_data->source_ase_num,
						p_data->bass_is_found,
						p_data->brs_char_num);
		BT_LOGD("LE_AUDIO_MSG_BAP_DIS_ALL_DONE: ascs_is_found %d, sink_ase_num %d, source_ase_num %d, bass_is_found %d, brs_char_num %d",
				p_data->ascs_is_found,
				p_data->sink_ase_num,
				p_data->source_ase_num,
				p_data->bass_is_found,
				p_data->brs_char_num);
		p_link = bt_stack_le_audio_find_link_by_conn_handle(p_data->conn_handle);
		if (p_link) {
			if (p_data->ascs_is_found) {
				p_link->lea_disc_flags |= RTK_BT_LE_AUDIO_LINK_DISC_ASCS_DONE;
				p_link->lea_srv_found_flags |= RTK_BT_LE_AUDIO_LINK_DISC_ASCS_DONE;
			}
			if (p_data->pacs_is_found) {
				p_link->lea_disc_flags |= RTK_BT_LE_AUDIO_LINK_DISC_PACS_DONE;
				p_link->lea_srv_found_flags |= RTK_BT_LE_AUDIO_LINK_DISC_PACS_DONE;
			}
			if ((bt_le_audio_priv_data.bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI) && p_data->bass_is_found) {
				p_link->lea_disc_flags |= RTK_BT_LE_AUDIO_LINK_DISC_BASS_DONE;
				p_link->lea_srv_found_flags |= RTK_BT_LE_AUDIO_LINK_DISC_BASS_DONE;
				bt_stack_le_audio_link_init_brs_char(p_link, p_data->brs_char_num);
			}
		} else {
			BT_LOGE("[BAP] LE_AUDIO_MSG_BAP_DIS_ALL_DONE link not found \r\n");
			break;
		}
#if !defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) || !RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
		/* bap flow group will be create here cause no cisp event */
		rtk_bt_le_audio_group_info_t *p_group_info = bt_stack_le_audio_find_group_by_conn_handle(p_data->conn_handle);
		if (!p_group_info) {
			rtk_bt_le_audio_group_handle_t group_handle = NULL;
			rtk_bt_le_audio_device_handle_t device_handle = NULL;
			uint8_t conn_id = 0;
			rtk_bt_le_addr_type_t addr_type = 0;
			uint8_t addr_val[RTK_BD_ADDR_LEN] = {0};
			p_group_info = bt_stack_le_audio_find_group_by_idx(0);// default init one group
			if (!p_group_info) {
				group_handle = ble_audio_group_allocate();
				if (group_handle == NULL) {
					BT_LOGE("%s fail: ble_audio_group_allocate fail\r\n", __func__);
					break;
				}
				p_group_info = bt_stack_le_audio_add_group(group_handle, 0);
				if (false == ble_audio_group_reg_cb(group_handle, bt_stack_le_audio_group_cb)) {
					BT_LOGE("%s fail: ble_audio_group_reg_cb fail\r\n", __func__);
					break;
				}
			}
			group_handle = p_group_info->group_handle;
			if (false == le_get_conn_id_by_handle(p_data->conn_handle, &conn_id)) {
				BT_LOGE("%s fail: le_get_conn_id_by_handle fail\r\n", __func__);
				break;
			}
			if (false == le_get_conn_addr(conn_id, addr_val, (uint8_t *)&addr_type)) {
				BT_LOGE("%s fail: le_get_conn_addr fail\r\n", __func__);
				break;
			}

			device_handle = ble_audio_group_add_dev(group_handle, addr_val, addr_type);
			if (device_handle == NULL) {
				BT_LOGE("%s fail: ble_audio_group_add_dev fail\r\n", __func__);
				break;
			}
			BT_LOGA("[BAP] %s: add device in group OK (group_handle=%08x, device_handle=%08x) \r\n", __func__, group_handle, device_handle);
			/* get prefer codec cfg and prefer qos */
			if (p_group_info) {
				p_group_info->lea_unicast.audio_cfg_type = RTK_BT_LE_AUDIO_DEFAULT_UNICAST_AUDIO_CFG;
				p_group_info->lea_unicast.codec_cfg_item = RTK_BT_LE_AUDIO_DEFAULT_CODEC_CFG_ITEM;
				p_group_info->lea_unicast.qos_cfg_type = RTK_BT_LE_AUDIO_DEFAULT_QOS_CFG_TYPE;
#if defined(RTK_BT_LE_AUDIO_CIG_ISO_INTERVAL_CONFIG) && (RTK_BT_LE_AUDIO_CIG_ISO_INTERVAL_CONFIG == RTK_BT_ISO_INTERVAL_20_MS)
				p_group_info->lea_unicast.qos_cfg_type = RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY;
#endif
				if (false == qos_preferred_cfg_get((T_CODEC_CFG_ITEM)p_group_info->lea_unicast.codec_cfg_item,
												   (T_QOS_CFG_TYPE)p_group_info->lea_unicast.qos_cfg_type,
												   (T_QOS_CFG_PREFERRED *)&p_group_info->lea_unicast.preferred_qos)) {
					BT_LOGE("%s qos_preferred_cfg_get fail\r\n", __func__);
				} else {
					p_group_info->lea_unicast.session_qos.framing = p_group_info->lea_unicast.preferred_qos.framing;
					p_group_info->lea_unicast.session_qos.sdu_interval_m_s = p_group_info->lea_unicast.preferred_qos.sdu_interval;
					p_group_info->lea_unicast.session_qos.sdu_interval_s_m = p_group_info->lea_unicast.preferred_qos.sdu_interval;
					p_group_info->lea_unicast.session_qos.latency_m_s = p_group_info->lea_unicast.preferred_qos.max_transport_latency;
					p_group_info->lea_unicast.session_qos.latency_s_m = p_group_info->lea_unicast.preferred_qos.max_transport_latency;
					p_group_info->lea_unicast.session_qos.sink_presentation_delay = p_group_info->lea_unicast.preferred_qos.presentation_delay;
					p_group_info->lea_unicast.session_qos.source_presentation_delay = p_group_info->lea_unicast.preferred_qos.presentation_delay;
				}
			} else {
				BT_LOGE("%s bt_stack_le_audio_add_group fail \r\n", __func__);
				break;
			}
		} else {
			BT_LOGE("%s group handle %08x idx 0x%x is already in list \r\n", __func__, p_group_info->group_handle, p_group_info->group_idx);
			break;
		}
#endif
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP, RTK_BT_LE_AUDIO_EVT_BAP_DISCOVERY_DONE_IND, sizeof(rtk_bt_le_audio_bap_discovery_done_ind_t));
		if (!p_evt) {
			BT_LOGE("rtk_stack_le_audio_bap_msg_cback: evt_t allocate fail \r\n");
			break;
		}
		p_ind = (rtk_bt_le_audio_bap_discovery_done_ind_t *)p_evt->data;
		p_ind->conn_handle = p_data->conn_handle;
		p_ind->pacs_is_found = p_data->pacs_is_found;
		p_ind->sink_pac_num = p_data->sink_pac_num;
		p_ind->source_pac_num = p_data->source_pac_num;
		p_ind->ascs_is_found = p_data->ascs_is_found;
		p_ind->sink_ase_num = p_data->sink_ase_num;
		p_ind->source_ase_num = p_data->source_ase_num;
		p_ind->bass_is_found = p_data->bass_is_found;
		p_ind->brs_char_num = p_data->brs_char_num;
#if !defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) || !RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
		p_ind->group_idx = p_group_info->group_idx;
#endif
		/* Send event */
		if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
			BT_LOGE("rtk_stack_le_audio_bap_msg_cback: evt_t indicate fail \r\n");
			break;
		}
	}
	break;

	case LE_AUDIO_MSG_BAP_PACS_NOTIFY: {
		T_BAP_PACS_NOTIFY *p_data = (T_BAP_PACS_NOTIFY *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_BAP_PACS_NOTIFY: conn_handle 0x%x, pac_handle 0x%x, pacs_type 0x%x",
						p_data->conn_handle,
						p_data->pac_handle,
						p_data->pacs_type);
		BT_LOGD("LE_AUDIO_MSG_BAP_PACS_NOTIFY: conn_handle 0x%x, pac_handle 0x%x, pacs_type 0x%x\r\n",
				p_data->conn_handle,
				p_data->pac_handle,
				p_data->pacs_type);
	}
	break;

	case LE_AUDIO_MSG_BASS_CLIENT_CP_RESULT: {
		T_BASS_CLIENT_CP_RESULT *p_data = (T_BASS_CLIENT_CP_RESULT *)buf;
		APP_PRINT_INFO2("LE_AUDIO_MSG_BASS_CLIENT_CP_RESULT: conn_handle 0x%x, cause 0x%x", p_data->conn_handle, p_data->cause);
		BT_LOGD("LE_AUDIO_MSG_BASS_CLIENT_CP_RESULT: conn_handle 0x%x, cause 0x%x\r\n", p_data->conn_handle, p_data->cause);
	}
	break;

	case LE_AUDIO_MSG_BASS_CLIENT_SYNC_INFO_REQ: {
		uint8_t past_type = RTK_BT_LE_AUDIO_BASS_PAST_BY_LOCAL_SRC;
		T_BASS_CLIENT_SYNC_INFO_REQ *p_data = (T_BASS_CLIENT_SYNC_INFO_REQ *)buf;
		APP_PRINT_INFO2("LE_AUDIO_MSG_BASS_CLIENT_SYNC_INFO_REQ: conn_handle 0x%x, char_instance_id %d", p_data->conn_handle, p_data->char_instance_id);
		BT_LOGA("LE_AUDIO_MSG_BASS_CLIENT_SYNC_INFO_REQ: conn_handle 0x%x, char_instance_id %d\r\n", p_data->conn_handle, p_data->char_instance_id);
		if (bt_le_audio_priv_data.cap_role & RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR) {
			past_type = RTK_BT_LE_AUDIO_BASS_PAST_BY_LOCAL_SRC;
		} else if (bt_le_audio_priv_data.cap_role & RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER) {
			past_type = RTK_BT_LE_AUDIO_BASS_PAST_BY_REMOTE_SRC;
		} else {
			break;
		}
		p_link = bt_stack_le_audio_find_link_by_conn_handle(p_data->conn_handle);
		if (p_link) {
			rtk_bt_le_audio_bass_broadcast_source_data_t *p_brs_char = NULL;
			p_brs_char = (rtk_bt_le_audio_bass_broadcast_source_data_t *)bt_stack_le_audio_link_find_brs_char_by_instance_id(p_link, p_data->char_instance_id);
			if (p_brs_char != NULL && p_brs_char->handle != NULL) {
				if (false == ble_audio_check_remote_features(p_data->conn_handle, LE_SUPPORT_FEATURES_MASK_ARRAY_INDEX3, LE_SUPPORT_FEATURES_PAST_RECIPIENT_MASK_BIT)) {
					BT_LOGE("%s fail: ble_audio_check_remote_features fail\r\n", __func__);
					break;
				}
				//check local past feature
				if (false == le_check_supported_features(LE_SUPPORT_FEATURES_MASK_ARRAY_INDEX3, LE_SUPPORT_FEATURES_PAST_SENDER_MASK_BIT)) {
					BT_LOGE("%s fail: le_check_supported_features fail\r\n", __func__);
					break;
				}
				{
					T_BASS_PAST_SRV_DATA srv_data;
					srv_data.adv_a_match_ext_adv = 0;
					srv_data.adv_a_match_src = 0;
					srv_data.source_id = p_data->p_brs_data->source_id;
					if (past_type == RTK_BT_LE_AUDIO_BASS_PAST_BY_LOCAL_SRC) {
						if (false == bass_past_by_local_src(p_brs_char->handle, p_data->conn_handle, srv_data)) {
							BT_LOGE("%s fail: bass_past_by_remote_src fail\r\n", __func__);
							break;
						}
					} else {
						if (false == bass_past_by_remote_src(p_brs_char->handle, p_data->conn_handle, srv_data)) {
							BT_LOGE("%s fail: bass_past_by_remote_src fail\r\n", __func__);
							break;
						}
					}
				}
			}
		} else {
			BT_LOGE("[BAP] LE_AUDIO_MSG_BASS_CLIENT_SYNC_INFO_REQ link not found \r\n");
		}
	}
	break;

	case LE_AUDIO_MSG_BASS_CLIENT_BRS_DATA: {
		T_BASS_CLIENT_BRS_DATA *p_data = (T_BASS_CLIENT_BRS_DATA *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_BASS_CLIENT_BRS_DATA: conn_handle 0x%x, notify %d, read_cause 0x%x, char_instance_id %d",
						p_data->conn_handle, p_data->notify, p_data->read_cause, p_data->char_instance_id);
		BT_LOGD("LE_AUDIO_MSG_BASS_CLIENT_BRS_DATA: conn_handle 0x%x, notify %d, read_cause 0x%x, char_instance_id %d\r\n",
				p_data->conn_handle, p_data->notify, p_data->read_cause, p_data->char_instance_id);
		if ((bt_le_audio_priv_data.bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI) == 0) {
			break;
		}
		if (p_data->p_brs_data && p_data->p_brs_data->brs_is_used) {
			APP_PRINT_INFO7("LE_AUDIO_MSG_BASS_CLIENT_BRS_DATA: source_id %d, source_address_type 0x%x, source_address %s, source_adv_sid %d, pa_sync_state %d, bis_sync_state 0x%x, big_encryption %d",
							p_data->p_brs_data->source_id,
							p_data->p_brs_data->source_address_type,
							TRACE_BDADDR(p_data->p_brs_data->source_address),
							p_data->p_brs_data->source_adv_sid,
							p_data->p_brs_data->pa_sync_state,
							p_data->p_brs_data->bis_sync_state,
							p_data->p_brs_data->big_encryption);
			BT_LOGD("LE_AUDIO_MSG_BASS_CLIENT_BRS_DATA: source_id %d, source_adv_sid %d, pa_sync_state %d, bis_sync_state 0x%x, big_encryption %d\r\n",
					p_data->p_brs_data->source_id,
					p_data->p_brs_data->source_adv_sid,
					p_data->p_brs_data->pa_sync_state,
					(unsigned int)p_data->p_brs_data->bis_sync_state,
					p_data->p_brs_data->big_encryption);
			if (p_data->p_brs_data->num_subgroups) {
				APP_PRINT_INFO4("LE_AUDIO_MSG_BASS_CLIENT_BRS_DATA: num_subgroups %d, bis_info_size %d, bis_sync 0x%x, metadata_len %d",
								p_data->p_brs_data->num_subgroups,
								p_data->p_brs_data->bis_info_size,
								p_data->p_brs_data->p_cp_bis_info[0].bis_sync,
								p_data->p_brs_data->p_cp_bis_info[0].metadata_len);
				BT_LOGD("LE_AUDIO_MSG_BASS_CLIENT_BRS_DATA: num_subgroups %d, bis_info_size %d, bis_sync 0x%x, metadata_len %d\r\n",
						p_data->p_brs_data->num_subgroups,
						p_data->p_brs_data->bis_info_size,
						(unsigned int)p_data->p_brs_data->p_cp_bis_info[0].bis_sync,
						p_data->p_brs_data->p_cp_bis_info[0].metadata_len);
				BT_DUMPD("", p_data->p_brs_data->p_cp_bis_info[0].p_metadata, p_data->p_brs_data->p_cp_bis_info[0].metadata_len);
			}
			p_link = bt_stack_le_audio_find_link_by_conn_handle(p_data->conn_handle);
			if (p_link) {
				if (p_link->lea_disc_flags & RTK_BT_LE_AUDIO_LINK_DISC_BASS_DONE) {
					if (p_data->p_brs_data) {
						bt_stack_le_audio_link_update_brs_char(p_link, p_data);
						if ((bt_le_audio_priv_data.cap_role & RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR) && \
							p_data->p_brs_data->brs_is_used && \
							p_data->p_brs_data->big_encryption == (uint8_t)RTK_BT_LE_AUDIO_BASS_BRS_BIG_BROADCAST_CODE_REQUIRED) {
							T_BASS_CP_SET_BROADCAST_CODE cp_data = {0};
							cp_data.source_id = p_data->p_brs_data->source_id;
							memcpy(cp_data.broadcast_code, bt_stack_le_audio_broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
							if (false == bass_cp_set_broadcast_code(p_data->conn_handle, &cp_data, false)) {
								BT_LOGE("%s : bass_cp_set_broadcast_code fail\r\n", __func__);
							}
						}
					}
				}
			} else {
				BT_LOGE("[BAP] LE_AUDIO_MSG_BASS_CLIENT_BRS_DATA link not found \r\n");
			}
		}
	}
	break;

	case LE_AUDIO_MSG_SERVER_ATTR_CCCD_INFO: {
		T_SERVER_ATTR_CCCD_INFO *cccd_info = (T_SERVER_ATTR_CCCD_INFO *)buf;
		APP_PRINT_INFO6("LE_AUDIO_MSG_SERVER_ATTR_CCCD_INFO: conn_handle %d,service_id %d,char_uuid 0x%x,char_attrib_index 0x%x,ccc_bits 0x%x,param 0x%x\r\n",
						cccd_info->conn_handle, cccd_info->service_id, cccd_info->char_uuid, cccd_info->char_attrib_index, cccd_info->ccc_bits, cccd_info->param);
		BT_LOGD(
			"LE_AUDIO_MSG_SERVER_ATTR_CCCD_INFO: conn_handle %d,service_id %d,char_uuid 0x%x,char_attrib_index 0x%x,ccc_bits 0x%x,param 0x%x\r\n",
			cccd_info->conn_handle, cccd_info->service_id, cccd_info->char_uuid, cccd_info->char_attrib_index, cccd_info->ccc_bits, (unsigned int)cccd_info->param);
	}
	break;

	case LE_AUDIO_MSG_PACS_READ_AVAILABLE_CONTEXTS_IND: {
		T_PACS_READ_AVAILABLE_CONTEXTS_IND *p_data = (T_PACS_READ_AVAILABLE_CONTEXTS_IND *)buf;
		APP_PRINT_INFO2("LE_AUDIO_MSG_PACS_READ_AVAILABLE_CONTEXTS_IND: conn_handle 0x%x, cid=%d",
						p_data->conn_handle, p_data->cid);
		BT_LOGA("LE_AUDIO_MSG_PACS_READ_AVAILABLE_CONTEXTS_IND: conn_handle 0x%x, cid=%d,default_sink_available_contexts=0x%x,default_source_available_contexts=0x%x\r\n",
				p_data->conn_handle, p_data->cid, bt_le_audio_priv_data.lea_sink_available_contexts, bt_le_audio_priv_data.lea_source_available_contexts);
		pacs_available_contexts_read_cfm(p_data->conn_handle, p_data->cid, bt_le_audio_priv_data.lea_sink_available_contexts,
										 bt_le_audio_priv_data.lea_source_available_contexts);
	}
	break;

	case LE_AUDIO_MSG_ASCS_CIS_CONN_INFO: {
		T_ASCS_CIS_CONN_INFO *p_data = (T_ASCS_CIS_CONN_INFO *)buf;
		BT_LOGD("LE_AUDIO_MSG_ASCS_CIS_CONN_INFO: conn_handle 0x%x, cis_conn_handle %d cig_id %d cis_id %d\r\n",
				p_data->conn_handle,
				p_data->cis_conn_handle,
				p_data->cig_id,
				p_data->cis_id);
		rtk_bt_le_audio_ascs_cis_conn_info_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_ASCS_CIS_CONN_INFO,
									sizeof(rtk_bt_le_audio_ascs_cis_conn_info_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_ascs_cis_conn_info_t *)p_evt->data;
		p_ind->conn_handle = p_data->conn_handle;
		p_ind->cis_conn_handle = p_data->cis_conn_handle;
		p_ind->cig_id = p_data->cig_id;
		p_ind->cis_id = p_data->cis_id;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_ASCS_SETUP_DATA_PATH: {
		rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;
		T_ASCS_SETUP_DATA_PATH *p_data = (T_ASCS_SETUP_DATA_PATH *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_ASCS_SETUP_DATA_PATH: conn_handle 0x%x, ase_id %d, path_direction 0x%x, cis_conn_handle 0x%x",
						p_data->conn_handle,
						p_data->ase_id,
						p_data->path_direction,
						p_data->cis_conn_handle);
		BT_LOGD("LE_AUDIO_MSG_ASCS_SETUP_DATA_PATH: conn_handle 0x%x, ase_id %d, path_direction 0x%x, cis_conn_handle 0x%x\r\n",
				p_data->conn_handle,
				p_data->ase_id,
				p_data->path_direction,
				p_data->cis_conn_handle);
		APP_PRINT_INFO6("type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu %d, octets_per_codec_frame %d, audio_channel_allocation 0x%x",
						p_data->codec_parsed_data.type_exist,
						p_data->codec_parsed_data.frame_duration,
						p_data->codec_parsed_data.sample_frequency,
						p_data->codec_parsed_data.codec_frame_blocks_per_sdu,
						p_data->codec_parsed_data.octets_per_codec_frame,
						p_data->codec_parsed_data.audio_channel_allocation);
		BT_LOGD(
			"type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu %d, octets_per_codec_frame %d, audio_channel_allocation 0x%x\r\n",
			p_data->codec_parsed_data.type_exist,
			p_data->codec_parsed_data.frame_duration,
			p_data->codec_parsed_data.sample_frequency,
			p_data->codec_parsed_data.codec_frame_blocks_per_sdu,
			p_data->codec_parsed_data.octets_per_codec_frame,
			(unsigned int)p_data->codec_parsed_data.audio_channel_allocation);
		/* add data path */
		p_iso_chann = bt_stack_le_audio_data_path_add(LE_AUDIO_MSG_ASCS_SETUP_DATA_PATH, RTK_BT_LE_AUDIO_CIG_ISO_MODE, RTK_BLE_AUDIO_ISO_DATA_PATH_RX, (void *)p_data);
		if (!p_iso_chann) {
			BT_LOGE("[BAP] %s bt_stack_le_audio_data_path_add fail \r\n", __func__);
			break;
		}
		rtk_bt_le_audio_ase_t *p_lea_ase = NULL;
		T_ISOCH_INFO info = {0};
		p_lea_ase = bt_stack_le_audio_find_ase(p_data->conn_handle, p_data->ase_id);
		if (p_lea_ase) {
			p_lea_ase->iso_conn_handle = p_data->cis_conn_handle;
		}
		if (true != cig_mgr_get_isoch_info(p_iso_chann->iso_conn_handle, &info)) {
			BT_LOGE("[BAP] %s cig_mgr_get_isoch_info fail (cis_conn_handle = 0x%x)\r\n", __func__, p_iso_chann->iso_conn_handle);
			break;
		} else {
			p_iso_chann->transport_latency_m_to_s = info.transport_latency_m_to_s;
			p_iso_chann->transport_latency_s_to_m = info.transport_latency_s_to_m;
		}
		rtk_bt_le_audio_ascs_setup_data_path_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_ASCS_SETUP_DATA_PATH_IND,
									sizeof(rtk_bt_le_audio_ascs_setup_data_path_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_ascs_setup_data_path_ind_t *)p_evt->data;
		p_ind->conn_handle = p_data->conn_handle;
		p_ind->ase_id = p_data->ase_id;
		p_ind->path_direction = p_data->path_direction;
		p_ind->cis_conn_handle = p_data->cis_conn_handle;
		p_ind->codec_cfg.type_exist = p_data->codec_parsed_data.type_exist;
		p_ind->codec_cfg.frame_duration = p_data->codec_parsed_data.frame_duration;
		p_ind->codec_cfg.sample_frequency = p_data->codec_parsed_data.sample_frequency;
		p_ind->codec_cfg.codec_frame_blocks_per_sdu = p_data->codec_parsed_data.codec_frame_blocks_per_sdu;
		p_ind->codec_cfg.octets_per_codec_frame = p_data->codec_parsed_data.octets_per_codec_frame;
		p_ind->codec_cfg.audio_channel_allocation = p_data->codec_parsed_data.audio_channel_allocation;
		p_ind->iso_chann_t.p_iso_chann = (void *)p_iso_chann;
		p_ind->iso_chann_t.iso_conn_handle = p_iso_chann->iso_conn_handle;
		p_ind->iso_chann_t.path_direction = p_data->path_direction;
		p_ind->iso_chann_t.iso_interval = info.iso_interval;
		p_ind->presentation_delay = p_iso_chann->presentation_delay;
		p_ind->transport_latency_m_to_s = p_iso_chann->transport_latency_m_to_s;
		p_ind->transport_latency_s_to_m = p_iso_chann->transport_latency_s_to_m;
		if (p_data->path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
			p_ind->source_ase_num = bt_stack_le_audio_find_source_ase_num_by_conn_hdl(p_data->conn_handle);
		}
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_ASCS_REMOVE_DATA_PATH: {
		T_ASCS_REMOVE_DATA_PATH *p_data = (T_ASCS_REMOVE_DATA_PATH *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_ASCS_REMOVE_DATA_PATH: conn_handle 0x%x, ase_id %d, path_direction 0x%x, cis_conn_handle 0x%x",
						p_data->conn_handle,
						p_data->ase_id,
						p_data->path_direction,
						p_data->cis_conn_handle);
		BT_LOGD("LE_AUDIO_MSG_ASCS_REMOVE_DATA_PATH: conn_handle 0x%x, ase_id %d, path_direction 0x%x, cis_conn_handle 0x%x\r\n",
				p_data->conn_handle,
				p_data->ase_id,
				p_data->path_direction,
				p_data->cis_conn_handle);
		bt_stack_le_audio_handle_data_path_remove(p_data->cis_conn_handle, p_data->path_direction);
		{
			rtk_bt_le_audio_ase_t *p_lea_ase = NULL;
			p_lea_ase = bt_stack_le_audio_find_ase(p_data->conn_handle, p_data->ase_id);
			if (p_lea_ase) {
				p_lea_ase->iso_conn_handle = 0;
			}
		}
		rtk_bt_le_audio_ascs_remove_data_path_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_ASCS_REMOVE_DATA_PATH_IND,
									sizeof(rtk_bt_le_audio_ascs_remove_data_path_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_ascs_remove_data_path_ind_t *)p_evt->data;
		p_ind->conn_handle = p_data->conn_handle;
		p_ind->ase_id = p_data->ase_id;
		p_ind->path_direction = p_data->path_direction;
		p_ind->cis_conn_handle = p_data->cis_conn_handle;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case LE_AUDIO_MSG_ASCS_ASE_STATE: {
		T_ASCS_ASE_STATE *p_data = (T_ASCS_ASE_STATE *)buf;
		APP_PRINT_INFO3("LE_AUDIO_MSG_ASCS_ASE_STATE: conn_handle 0x%x, ase_id %d, ase_state %d",
						p_data->conn_handle,
						p_data->ase_data.ase_id,
						p_data->ase_data.ase_state);
		BT_LOGD("LE_AUDIO_MSG_ASCS_ASE_STATE: conn_handle 0x%x, ase_id %d, ase_state %d\r\n", p_data->conn_handle, p_data->ase_data.ase_id,
				p_data->ase_data.ase_state);
		bt_stack_le_audio_demo_update_ase_state(p_data);
		rtk_bt_le_audio_ascs_ase_state_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_ASCS_ASE_STATE_IND,
									sizeof(rtk_bt_le_audio_ascs_ase_state_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_ascs_ase_state_ind_t *)p_evt->data;
		p_ind->conn_handle = p_data->conn_handle;
		p_ind->ase_id = p_data->ase_data.ase_id;
		p_ind->direction = p_data->ase_data.direction;
		p_ind->ase_state = p_data->ase_data.ase_state;
		/* Send event */
		/* directly calling to avoid memcpy of metadata memory */
		rtk_bt_evt_indicate(p_evt, &cb_ret);
		break;
	}

	case LE_AUDIO_MSG_ASCS_CP_CONFIG_CODEC_IND: {
		T_ASCS_CP_CONFIG_CODEC_IND *p_data = (T_ASCS_CP_CONFIG_CODEC_IND *)buf;
		APP_PRINT_INFO2("LE_AUDIO_MSG_ASCS_CP_CONFIG_CODEC_IND: conn_handle 0x%x, number_of_ase %d",
						p_data->conn_handle,
						p_data->number_of_ase);
		BT_LOGD("LE_AUDIO_MSG_ASCS_CP_CONFIG_CODEC_IND: conn_handle 0x%x, number_of_ase %d\r\n",
				p_data->conn_handle,
				p_data->number_of_ase);
		for (uint8_t i = 0; i < p_data->number_of_ase; i++) {
			APP_PRINT_INFO5("ase param[%d]: ase_id %d, target_latency %d, target_phy %d, codec_id %b",
							i, p_data->param[i].data.ase_id,
							p_data->param[i].data.target_latency,
							p_data->param[i].data.target_phy,
							TRACE_BINARY(5, p_data->param[i].data.codec_id));
			APP_PRINT_INFO7("ase param[%d]: type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu %d, octets_per_codec_frame %d, audio_channel_allocation 0x%x",
							i, p_data->param[i].codec_parsed_data.type_exist,
							p_data->param[i].codec_parsed_data.frame_duration,
							p_data->param[i].codec_parsed_data.sample_frequency,
							p_data->param[i].codec_parsed_data.codec_frame_blocks_per_sdu,
							p_data->param[i].codec_parsed_data.octets_per_codec_frame,
							p_data->param[i].codec_parsed_data.audio_channel_allocation);
			BT_LOGD("ase param[%d]: ase_id %d, target_latency %d, target_phy %d, codec_id:\r\n",
					i, p_data->param[i].data.ase_id,
					p_data->param[i].data.target_latency,
					p_data->param[i].data.target_phy);
			BT_DUMPD("", p_data->param[i].data.codec_id, 5);
			BT_LOGD(
				"ase param[%d]: type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu %d, octets_per_codec_frame %d, audio_channel_allocation 0x%x\r\n",
				i, p_data->param[i].codec_parsed_data.type_exist,
				p_data->param[i].codec_parsed_data.frame_duration,
				p_data->param[i].codec_parsed_data.sample_frequency,
				p_data->param[i].codec_parsed_data.codec_frame_blocks_per_sdu,
				p_data->param[i].codec_parsed_data.octets_per_codec_frame,
				(unsigned int)p_data->param[i].codec_parsed_data.audio_channel_allocation);

			rtk_bt_le_audio_ascs_cp_codec_cfg_ind_t *p_codec_cfg = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
										RTK_BT_LE_AUDIO_EVT_ASCS_CP_CODEC_CFG_IND,
										sizeof(rtk_bt_le_audio_ascs_cp_codec_cfg_ind_t));
			if (!p_evt) {
				BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
				break;
			}
			p_codec_cfg = (rtk_bt_le_audio_ascs_cp_codec_cfg_ind_t *)p_evt->data;
			p_codec_cfg->conn_handle = p_data->conn_handle;
			p_codec_cfg->ase_id = p_data->param[i].data.ase_id;
			p_codec_cfg->codec_cfg.type_exist = p_data->param[i].codec_parsed_data.type_exist;
			p_codec_cfg->codec_cfg.frame_duration = p_data->param[i].codec_parsed_data.frame_duration;
			p_codec_cfg->codec_cfg.sample_frequency = p_data->param[i].codec_parsed_data.sample_frequency;
			p_codec_cfg->codec_cfg.codec_frame_blocks_per_sdu = p_data->param[i].codec_parsed_data.codec_frame_blocks_per_sdu;
			p_codec_cfg->codec_cfg.octets_per_codec_frame =  p_data->param[i].codec_parsed_data.octets_per_codec_frame;
			p_codec_cfg->codec_cfg.audio_channel_allocation = p_data->param[i].codec_parsed_data.audio_channel_allocation;
			/* Send event */
			rtk_bt_evt_indicate(p_evt, NULL);
		}
		break;
	}

	case LE_AUDIO_MSG_ASCS_CP_CONFIG_QOS_IND: {
		T_ASCS_CP_CONFIG_QOS_IND *p_data = (T_ASCS_CP_CONFIG_QOS_IND *)buf;
		APP_PRINT_INFO2("LE_AUDIO_MSG_ASCS_CP_CONFIG_QOS_IND: conn_handle 0x%x, number_of_ase %d",
						p_data->conn_handle,
						p_data->number_of_ase);
		BT_LOGD("LE_AUDIO_MSG_ASCS_CP_CONFIG_QOS_IND: conn_handle 0x%x, number_of_ase %d\r\n",
				p_data->conn_handle,
				p_data->number_of_ase);
		for (uint8_t i = 0; i < p_data->number_of_ase; i++) {
			uint16_t max_sdu = 0;
			uint16_t max_transport_latency = 0;
			uint32_t sdu_interval = 0;
			uint32_t presentation_delay = 0;
			LE_ARRAY_TO_UINT24(sdu_interval, p_data->param[i].sdu_interval);
			LE_ARRAY_TO_UINT24(presentation_delay, p_data->param[i].presentation_delay);
			LE_ARRAY_TO_UINT16(max_sdu, p_data->param[i].max_sdu);
			LE_ARRAY_TO_UINT16(max_transport_latency, p_data->param[i].max_transport_latency);
			APP_PRINT_INFO4("ase param[%d]: ase_id %d,  cig_id %d, cis_id %d",
							i, p_data->param[i].ase_id,
							p_data->param[i].cig_id, p_data->param[i].cis_id);
			APP_PRINT_INFO8("ase param[%d]: sdu_interval %x %x %x, framing %d, phy 0x%x, max_sdu  %x %x",
							i, p_data->param[i].sdu_interval[0], p_data->param[i].sdu_interval[1],
							p_data->param[i].sdu_interval[2],
							p_data->param[i].framing, p_data->param[i].phy,
							p_data->param[i].max_sdu[0], p_data->param[i].max_sdu[1]);
			APP_PRINT_INFO7("ase param[%d]: retransmission_number %d, max_transport_latency %x %x, presentation_delay  %x %x %x",
							i, p_data->param[i].retransmission_number,
							p_data->param[i].max_transport_latency[0],
							p_data->param[i].max_transport_latency[1],
							p_data->param[i].presentation_delay[0],
							p_data->param[i].presentation_delay[1],
							p_data->param[i].presentation_delay[2]);
			BT_LOGD("ase param[%d]: ase_id %d,  cig_id %d, cis_id %d\r\n",
					i, p_data->param[i].ase_id,
					p_data->param[i].cig_id, p_data->param[i].cis_id);
			BT_LOGD("ase param[%d]: sdu_interval %x %x %x, framing %d, phy 0x%x, max_sdu  %x %x\r\n",
					i, p_data->param[i].sdu_interval[0], p_data->param[i].sdu_interval[1],
					p_data->param[i].sdu_interval[2],
					p_data->param[i].framing, p_data->param[i].phy,
					p_data->param[i].max_sdu[0], p_data->param[i].max_sdu[1]);
			BT_LOGD("ase param[%d]: retransmission_number %d, max_transport_latency %x %x, presentation_delay  %x %x %x\r\n",
					i, p_data->param[i].retransmission_number,
					p_data->param[i].max_transport_latency[0],
					p_data->param[i].max_transport_latency[1],
					p_data->param[i].presentation_delay[0],
					p_data->param[i].presentation_delay[1],
					p_data->param[i].presentation_delay[2]);

			rtk_bt_le_audio_ascs_cp_qos_cfg_ind_t *p_qos_cfg = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
										RTK_BT_LE_AUDIO_EVT_ASCS_CP_QOS_CFG_IND,
										sizeof(rtk_bt_le_audio_ascs_cp_qos_cfg_ind_t));
			if (!p_evt) {
				BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
				break;
			}
			p_qos_cfg = (rtk_bt_le_audio_ascs_cp_qos_cfg_ind_t *)p_evt->data;
			p_qos_cfg->conn_handle = p_data->conn_handle;
			p_qos_cfg->ase_id = p_data->param[i].ase_id;
			p_qos_cfg->cig_id = p_data->param[i].cig_id;
			p_qos_cfg->cis_id = p_data->param[i].cis_id;
			p_qos_cfg->framing = p_data->param[i].framing;
			p_qos_cfg->phy = p_data->param[i].phy;
			p_qos_cfg->retransmission_number = p_data->param[i].retransmission_number;
			p_qos_cfg->max_sdu = max_sdu;
			p_qos_cfg->max_transport_latency = max_transport_latency;
			p_qos_cfg->sdu_interval = sdu_interval;
			p_qos_cfg->presentation_delay = presentation_delay;
			/* Send event */
			rtk_bt_evt_indicate(p_evt, NULL);
		}
		break;
	}

	case LE_AUDIO_MSG_ASCS_CP_ENABLE_IND: {
		T_ASCS_CP_ENABLE_IND *p_data = (T_ASCS_CP_ENABLE_IND *)buf;
		uint8_t i = 0;
		APP_PRINT_INFO2("LE_AUDIO_MSG_ASCS_CP_ENABLE_IND: conn_handle 0x%x, number_of_ase %d",
						p_data->conn_handle,
						p_data->number_of_ase);
		BT_LOGD("LE_AUDIO_MSG_ASCS_CP_ENABLE_IND: conn_handle 0x%x, number_of_ase %d\r\n",
				p_data->conn_handle,
				p_data->number_of_ase);
		for (i = 0; i < p_data->number_of_ase; i++) {
			APP_PRINT_INFO4("ase param[%d]: ase_id %d, metadata_length %d, metadata %b",
							i, p_data->param[i].ase_id,
							p_data->param[i].metadata_length,
							TRACE_BINARY(p_data->param[i].metadata_length, p_data->param[i].p_metadata));
			BT_LOGD("ase param[%d]: ase_id %d\r\n",
					i, p_data->param[i].ase_id);
			BT_DUMPD("", p_data->param[i].p_metadata, p_data->param[i].metadata_length);
		}
		rtk_bt_le_audio_ascs_cp_enable_ind_t *p_enable = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_ASCS_CP_ENABLE_IND,
									sizeof(rtk_bt_le_audio_ascs_cp_enable_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_enable = (rtk_bt_le_audio_ascs_cp_enable_ind_t *)p_evt->data;
		p_enable->conn_handle = p_data->conn_handle;
		p_enable->ase_num = p_data->number_of_ase;
		for (i = 0; i < p_data->number_of_ase; i++) {
			p_enable->ase_id[i] = p_data->param[i].ase_id;
		}
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);

		break;
	}

	case LE_AUDIO_MSG_ASCS_CP_DISABLE_IND: {
		T_ASCS_CP_DISABLE_IND *p_data = (T_ASCS_CP_DISABLE_IND *)buf;
		uint8_t i = 0;
		APP_PRINT_INFO3("LE_AUDIO_MSG_ASCS_CP_DISABLE_IND: conn_handle 0x%x, number_of_ase %d, ase_ids %b",
						p_data->conn_handle,
						p_data->number_of_ase, TRACE_BINARY(p_data->number_of_ase, p_data->ase_id));
		BT_LOGD("LE_AUDIO_MSG_ASCS_CP_DISABLE_IND: conn_handle 0x%x, number_of_ase %d\r\n",
				p_data->conn_handle,
				p_data->number_of_ase);
		rtk_bt_le_audio_ascs_cp_disable_ind_t *p_disable = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_ASCS_CP_DISABLE_IND,
									sizeof(rtk_bt_le_audio_ascs_cp_disable_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_disable = (rtk_bt_le_audio_ascs_cp_disable_ind_t *)p_evt->data;
		p_disable->conn_handle = p_data->conn_handle;
		p_disable->ase_num = p_data->number_of_ase;
		for (i = 0; i < p_data->number_of_ase; i++) {
			p_disable->ase_id[i] = p_data->ase_id[i];
		}
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
		break;
	}

	case LE_AUDIO_MSG_ASCS_CP_UPDATE_METADATA_IND: {
		T_ASCS_CP_UPDATE_METADATA_IND *p_data = (T_ASCS_CP_UPDATE_METADATA_IND *)buf;
		APP_PRINT_INFO2("LE_AUDIO_MSG_ASCS_CP_UPDATE_METADATA_IND: conn_handle 0x%x, number_of_ase %d",
						p_data->conn_handle,
						p_data->number_of_ase);
		BT_LOGD("LE_AUDIO_MSG_ASCS_CP_UPDATE_METADATA_IND: conn_handle 0x%x, number_of_ase %d\r\n",
				p_data->conn_handle,
				p_data->number_of_ase);
		for (uint8_t i = 0; i < p_data->number_of_ase; i++) {
			APP_PRINT_INFO4("ase param[%d]: ase_id %d, metadata_length %d, metadata %b",
							i, p_data->param[i].ase_id,
							p_data->param[i].metadata_length,
							TRACE_BINARY(p_data->param[i].metadata_length, p_data->param[i].p_metadata));
			BT_LOGD("ase param[%d]: ase_id %d\r\n",
					i, p_data->param[i].ase_id);
			BT_DUMPD("", p_data->param[i].p_metadata, p_data->param[i].metadata_length);
		}
		break;
	}

	case LE_AUDIO_MSG_ASCS_GET_PREFER_QOS: {
		T_ASCS_GET_PREFER_QOS *p_data = (T_ASCS_GET_PREFER_QOS *)buf;
		APP_PRINT_INFO5("LE_AUDIO_MSG_ASCS_GET_PREFER_QOS: conn_handle 0x%x, ase_id 0x%x, direction 0x%x, target_latency 0x%x, target_phy 0x%x",
						p_data->conn_handle, p_data->ase_id, p_data->direction, p_data->target_latency, p_data->target_phy);
		APP_PRINT_INFO6("LE_AUDIO_MSG_ASCS_GET_PREFER_QOS: codec_cfg.type_exist 0x%x, codec_cfg.frame_duration 0x%x,codec_cfg.sample_frequency 0x%x, codec_cfg.codec_frame_blocks_per_sdu 0x%x, codec_cfg.octets_per_codec_frame 0x%x,codec_cfg.audio_channel_allocation 0x%x",
						p_data->codec_cfg.type_exist, p_data->codec_cfg.frame_duration,
						p_data->codec_cfg.sample_frequency, p_data->codec_cfg.codec_frame_blocks_per_sdu, p_data->codec_cfg.octets_per_codec_frame,
						p_data->codec_cfg.audio_channel_allocation);
		BT_LOGD("LE_AUDIO_MSG_ASCS_GET_PREFER_QOS: conn_handle 0x%x, ase_id 0x%x, direction 0x%x, target_latency 0x%x, target_phy 0x%x\r\n",
				p_data->conn_handle, p_data->ase_id, p_data->direction, p_data->target_latency, p_data->target_phy);
		BT_LOGD(
			"LE_AUDIO_MSG_ASCS_GET_PREFER_QOS: codec_cfg.type_exist 0x%x, codec_cfg.frame_duration 0x%x,codec_cfg.sample_frequency 0x%x, codec_cfg.codec_frame_blocks_per_sdu 0x%x, codec_cfg.octets_per_codec_frame 0x%x,codec_cfg.audio_channel_allocation 0x%x\r\n",
			p_data->codec_cfg.type_exist, p_data->codec_cfg.frame_duration,
			p_data->codec_cfg.sample_frequency, p_data->codec_cfg.codec_frame_blocks_per_sdu, p_data->codec_cfg.octets_per_codec_frame,
			(unsigned int)p_data->codec_cfg.audio_channel_allocation);

		rtk_bt_le_audio_ascs_get_prefer_qos_ind_t *p_ind = NULL;
		T_QOS_CFG_PREFERRED qos = {0};
		T_ASCS_PREFER_QOS_DATA prefer_qos_data = {0};
		if (true == ascs_get_ase_prefer_qos(p_data->conn_handle, p_data->ase_id, &prefer_qos_data)) {
			BT_LOGD("%s ascs_get_ase_prefer_qos from stack ok\r\n", __func__);
			if (false == ascs_cfg_ase_prefer_qos(p_data->conn_handle, p_data->ase_id, &prefer_qos_data)) {
				BT_LOGE("%s: ascs_cfg_ase_prefer_qos fail!\r\n", __func__);
			}
		} else {
			BT_LOGE("%s ascs_get_ase_prefer_qos fail, get it from app\r\n", __func__);
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
										RTK_BT_LE_AUDIO_EVT_ASCS_GET_PREFER_QOS_IND,
										sizeof(rtk_bt_le_audio_ascs_get_prefer_qos_ind_t));
			if (!p_evt) {
				BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
				break;
			}
			p_ind = (rtk_bt_le_audio_ascs_get_prefer_qos_ind_t *)p_evt->data;
			p_ind->conn_handle = p_data->conn_handle;
			p_ind->ase_id = p_data->ase_id;
			p_ind->direction = p_data->direction;
			p_ind->codec_cfg.type_exist = p_data->codec_cfg.type_exist;
			p_ind->codec_cfg.frame_duration = p_data->codec_cfg.frame_duration;
			p_ind->codec_cfg.sample_frequency = p_data->codec_cfg.sample_frequency;
			p_ind->codec_cfg.codec_frame_blocks_per_sdu = p_data->codec_cfg.codec_frame_blocks_per_sdu;
			p_ind->codec_cfg.octets_per_codec_frame = p_data->codec_cfg.octets_per_codec_frame;
			p_ind->codec_cfg.audio_channel_allocation = p_data->codec_cfg.audio_channel_allocation;
			p_ind->target_latency = (rtk_bt_le_audio_ascs_ase_target_latency_t)p_data->target_latency;
			p_ind->target_phy = (rtk_bt_le_audio_ascs_ase_target_phy_t)p_data->target_phy;
			p_ind->p_prefer_qos_data = (rtk_bt_le_audio_ascs_prefer_qos_data_t *)
									   &prefer_qos_data; //make sure the definition of T_ASCS_PREFER_QOS_DATA and rtk_bt_le_audio_ascs_prefer_qos_data_t are same
			/* Send event */
			rtk_bt_evt_indicate(p_evt, &cb_ret);
			if (cb_ret == RTK_BT_EVT_CB_OK) {
				BT_LOGD(
					"RTK_BT_LE_AUDIO_EVT_ASCS_GET_PREFER_QOS_IND get data from app, conn_handle = %d,ase_id = %d,direction = %d,supported_framing = %d,preferred_phy=%d, preferred_retrans_number = %d,max_transport_latency = %d,presentation_delay_min = %d,presentation_delay_max = %d,preferred_presentation_delay_min = %d,preferred_presentation_delay_max = %d\r\n",
					p_data->conn_handle, p_data->ase_id, p_data->direction,
					prefer_qos_data.supported_framing, prefer_qos_data.preferred_phy, prefer_qos_data.preferred_retrans_number,
					prefer_qos_data.max_transport_latency, (int)prefer_qos_data.presentation_delay_min, (int)prefer_qos_data.presentation_delay_max,
					(int)prefer_qos_data.preferred_presentation_delay_min, (int)prefer_qos_data.preferred_presentation_delay_max);
				if (false == qos_preferred_cfg_get_by_codec(&p_data->codec_cfg, p_data->target_latency, &qos)) {
					BT_LOGE("%s: qos_preferred_cfg_get_by_codec fail!\r\n", __func__);
					break;
				}
				if (prefer_qos_data.supported_framing == 0) {
					prefer_qos_data.supported_framing = RTK_BLE_AUDIO_UNFRAMED;
				}
				if (prefer_qos_data.preferred_phy == 0) {
					prefer_qos_data.preferred_phy = p_data->target_phy;
				}
				if (prefer_qos_data.preferred_retrans_number == 0) {
					prefer_qos_data.preferred_retrans_number = qos.retransmission_number;
				}
				if (prefer_qos_data.max_transport_latency == 0) {
					prefer_qos_data.max_transport_latency = qos.max_transport_latency;
				}
				if (prefer_qos_data.presentation_delay_max == 0) {
					prefer_qos_data.presentation_delay_max = qos.presentation_delay;
				}
				if (prefer_qos_data.preferred_presentation_delay_max == 0) {
					prefer_qos_data.preferred_presentation_delay_max = qos.presentation_delay;
				}
				BT_LOGD(
					"RTK_BT_LE_AUDIO_EVT_ASCS_GET_PREFER_QOS_IND get data from app, conn_handle = %d,ase_id = %d,direction = %d,supported_framing = %d,preferred_phy=%d, preferred_retrans_number = %d,max_transport_latency = %d,presentation_delay_min = %d,presentation_delay_max = %d,preferred_presentation_delay_min = %d,preferred_presentation_delay_max = %d\r\n",
					p_data->conn_handle, p_data->ase_id, p_data->direction,
					prefer_qos_data.supported_framing, prefer_qos_data.preferred_phy, prefer_qos_data.preferred_retrans_number,
					prefer_qos_data.max_transport_latency, (int)prefer_qos_data.presentation_delay_min, (int)prefer_qos_data.presentation_delay_max,
					(int)prefer_qos_data.preferred_presentation_delay_min, (int)prefer_qos_data.preferred_presentation_delay_max);
				if (false == ascs_cfg_ase_prefer_qos(p_data->conn_handle, p_data->ase_id, &prefer_qos_data)) {
					BT_LOGE("%s: ascs_cfg_ase_prefer_qos fail!\r\n", __func__);
				}
			}
		}
		break;
	}

	case LE_AUDIO_MSG_ASCS_CIS_REQUEST_IND: {
		T_ASCS_CIS_REQUEST_IND *p_data = (T_ASCS_CIS_REQUEST_IND *)buf;
		APP_PRINT_INFO6("LE_AUDIO_MSG_ASCS_CIS_REQUEST_IND: conn_handle 0x%x, cis_conn_handle 0x%x, snk_ase_id %d, snk_ase_state %d, src_ase_id %d, src_ase_state %d",
						p_data->conn_handle, p_data->cis_conn_handle,
						p_data->snk_ase_id, p_data->snk_ase_state,
						p_data->src_ase_id, p_data->src_ase_state);
		BT_LOGD(
			"LE_AUDIO_MSG_ASCS_CIS_REQUEST_IND: conn_handle 0x%x, cis_conn_handle 0x%x, snk_ase_id %d, snk_ase_state %d, src_ase_id %d, src_ase_state %d\r\n",
			p_data->conn_handle, p_data->cis_conn_handle,
			p_data->snk_ase_id, p_data->snk_ase_state,
			p_data->src_ase_id, p_data->src_ase_state);
		app_result = BLE_AUDIO_CB_RESULT_SUCCESS;
		break;
	}

	case LE_AUDIO_MSG_ASCS_AUDIO_CONTEXTS_CHECK_IND: {
		uint16_t available_contexts;
		T_ASCS_AUDIO_CONTEXTS_CHECK_IND *p_data = (T_ASCS_AUDIO_CONTEXTS_CHECK_IND *)buf;
		APP_PRINT_INFO5("LE_AUDIO_MSG_ASCS_AUDIO_CONTEXTS_CHECK_IND: conn_handle 0x%x, is_update_metadata 0x%x, ase_id 0x%x, direction 0x%x, available_contexts 0x%x",
						p_data->conn_handle, p_data->is_update_metadata, p_data->ase_id, p_data->direction, p_data->available_contexts);
		BT_LOGD(
			"LE_AUDIO_MSG_ASCS_AUDIO_CONTEXTS_CHECK_IND: conn_handle 0x%x, is_update_metadata 0x%x, ase_id 0x%x, direction 0x%x, available_contexts 0x%x\r\n",
			p_data->conn_handle, p_data->is_update_metadata, p_data->ase_id, p_data->direction, p_data->available_contexts);
		if (p_data->direction == SERVER_AUDIO_SINK) {
			available_contexts = bt_le_audio_priv_data.lea_sink_available_contexts;
		} else {
			available_contexts = bt_le_audio_priv_data.lea_source_available_contexts;
		}
		if ((available_contexts & p_data->available_contexts) == 0) {
			app_result = BLE_AUDIO_CB_RESULT_REJECT;
		}
		if ((available_contexts | p_data->available_contexts) != available_contexts) {
			app_result = BLE_AUDIO_CB_RESULT_REJECT;
		}
		break;
	}

	case LE_AUDIO_MSG_ASCS_CIS_DISCONN_INFO: {
		T_ASCS_CIS_DISCONN_INFO *p_data = (T_ASCS_CIS_DISCONN_INFO *)buf;
		APP_PRINT_INFO5("LE_AUDIO_MSG_ASCS_CIS_DISCONN_INFO: conn_handle 0x%x, cis_conn_handle 0x%x, cause 0x%x, cig_id 0x%x, cis_id 0x%x",
						p_data->conn_handle, p_data->cis_conn_handle, p_data->cause, p_data->cig_id, p_data->cis_id);
		BT_LOGD("LE_AUDIO_MSG_ASCS_CIS_DISCONN_INFO: conn_handle 0x%x, cis_conn_handle 0x%x, cause 0x%x, cig_id 0x%x, cis_id 0x%x\r\n",
				p_data->conn_handle, p_data->cis_conn_handle, p_data->cause, p_data->cig_id, p_data->cis_id);
		break;
	}

	case LE_AUDIO_MSG_BASS_CP_IND: {
		T_BASS_CP_IND *p_data = (T_BASS_CP_IND *)buf;
		APP_PRINT_INFO2("LE_AUDIO_MSG_BASS_CP_IND: conn_handle 0x%x, cp opcode 0x%x",
						p_data->conn_handle, p_data->p_cp_data->cp_op);
		BT_LOGD("LE_AUDIO_MSG_BASS_CP_IND: conn_handle 0x%x, cp opcode 0x%x\r\n",
				p_data->conn_handle, p_data->p_cp_data->cp_op);
		if (p_data->p_cp_data->cp_op == BASS_CP_OP_SET_BROADCAST_CODE) {
			BT_LOGA("[BAP] %s: BASS_CP_OP_SET_BROADCAST_CODE\r\n", __func__);
			p_sync_dev_info = bt_stack_le_audio_sync_dev_find_by_source_id(p_data->p_cp_data->param.set_broadcast_code.source_id);
			if (!p_sync_dev_info) {
				BT_LOGE("[BAP] %s bt_stack_le_audio_sync_dev_find_by_source_id fail\r\n", __func__);
				break;
			}
			memcpy(p_sync_dev_info->broadcast_code, p_data->p_cp_data->param.set_broadcast_code.broadcast_code,
				   RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
			p_sync_dev_info->big_proc_flags |= RTK_BT_LE_AUDIO_BIG_PROC_BROADCAST_CODE_EXIST;
			bt_stack_le_audio_check_sync(p_sync_dev_info);
		} else if (p_data->p_cp_data->cp_op == BASS_CP_OP_REMOTE_SCAN_START) {
			BT_LOGA("[BAP] %s: BASS_CP_OP_REMOTE_SCAN_START\r\n", __func__);
		} else if (p_data->p_cp_data->cp_op == BASS_CP_OP_REMOTE_SCAN_STOP) {
			BT_LOGA("[BAP] %s: BASS_CP_OP_REMOTE_SCAN_STOP\r\n", __func__);
		} else if (p_data->p_cp_data->cp_op == BASS_CP_OP_ADD_SOURCE) {
			BT_LOGA("[BAP] %s: BASS_CP_OP_ADD_SOURCE\r\n", __func__);
		} else if (p_data->p_cp_data->cp_op == BASS_CP_OP_MODIFY_SOURCE) {
			BT_LOGA("[BAP] %s: BASS_CP_OP_MODIFY_SOURCE\r\n", __func__);
		} else if (p_data->p_cp_data->cp_op == BASS_CP_OP_REMOVE_SOURCE) {
			BT_LOGA("[BAP] %s: BASS_CP_OP_REMOVE_SOURCE\r\n", __func__);
		}
	}
	break;

	case LE_AUDIO_MSG_BASS_BRS_MODIFY: {
		T_BASS_BRS_MODIFY *p_data = (T_BASS_BRS_MODIFY *)buf;
		APP_PRINT_INFO2("LE_AUDIO_MSG_BASS_BRS_MODIFY: sync handle %p, source_id %d", p_data->handle, p_data->source_id);
		BT_LOGD("LE_AUDIO_MSG_BASS_BRS_MODIFY: sync handle %08x, source_id %d\r\n", p_data->handle, p_data->source_id);
		if (p_data->p_brs_data) {
			if (p_data->p_brs_data->brs_is_used) {
				APP_PRINT_INFO2("source_address_type 0x%02x, source_adv_sid 0x%02x",
								p_data->p_brs_data->source_address_type, p_data->p_brs_data->source_adv_sid);
				APP_PRINT_INFO4("pa_sync_state %d, bis_sync_state 0x%x, big_encryption %d, num_subgroups %d",
								p_data->p_brs_data->pa_sync_state, p_data->p_brs_data->bis_sync_state,
								p_data->p_brs_data->big_encryption, p_data->p_brs_data->num_subgroups);
				for (uint8_t i = 0; i < p_data->p_brs_data->num_subgroups; i++) {
					if (p_data->p_brs_data->p_cp_bis_info[i].metadata_len == 0) {
						APP_PRINT_INFO2("Subgroup[%d], BIS Sync: [0x%x], Metadata Length: [0]", i,
										p_data->p_brs_data->p_cp_bis_info[i].bis_sync);
					} else {
						APP_PRINT_INFO4("Subgroup[%d], BIS Sync: [0x%x] Metadata data[%d]: %b", i,
										p_data->p_brs_data->p_cp_bis_info[i].bis_sync,
										p_data->p_brs_data->p_cp_bis_info[i].metadata_len,
										TRACE_BINARY(p_data->p_brs_data->p_cp_bis_info[i].metadata_len,
													 p_data->p_brs_data->p_cp_bis_info[i].p_metadata));
					}
				}
				p_sync_dev_info = bt_stack_le_audio_sync_dev_find(p_data->handle);
				if (!p_sync_dev_info) {
					BT_LOGD("[BAP] %s bt_stack_le_audio_sync_dev_find fail\r\n", __func__);
					break;
				}
				p_sync_dev_info->adv_sid = p_data->p_brs_data->source_adv_sid;
				p_sync_dev_info->adv_addr.type = p_data->p_brs_data->source_address_type;
				memcpy(&p_sync_dev_info->adv_addr.addr_val, &p_data->p_brs_data->source_address, sizeof(rtk_bt_le_addr_t));
				memcpy(p_sync_dev_info->broadcast_id, p_data->p_brs_data->broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);
			}
		}
	}
	break;

	case LE_AUDIO_MSG_BASS_BA_ADD_SOURCE: {
		T_BASS_BA_ADD_SOURCE *p_data = (T_BASS_BA_ADD_SOURCE *)buf;
		APP_PRINT_INFO2("LE_AUDIO_MSG_BASS_BA_ADD_SOURCE: sync handle %p, source_id %d",
						p_data->handle, p_data->source_id);
		BT_LOGA("LE_AUDIO_MSG_BASS_BA_ADD_SOURCE: sync handle %08x, source_id %d\r\n",
				p_data->handle, p_data->source_id);
		if (false == ble_audio_sync_update_cb(p_data->handle, bt_stack_le_audio_sync_cb)) {
			BT_LOGE("%s: ble_audio_sync_update_cb fail!\r\n", __func__);
		}
		p_sync_dev_info = bt_stack_le_audio_sync_dev_find(p_data->handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s bt_stack_le_audio_sync_dev_find fail\r\n", __func__);
			break;
		}
		p_sync_dev_info->source_id = p_data->source_id;
	}
	break;

	case LE_AUDIO_MSG_BASS_LOCAL_ADD_SOURCE: {
		T_BASS_LOCAL_ADD_SOURCE *p_data = (T_BASS_LOCAL_ADD_SOURCE *)buf;
		APP_PRINT_INFO2("LE_AUDIO_MSG_BASS_LOCAL_ADD_SOURCE: sync handle %p, source_id %d",
						p_data->handle, p_data->source_id);
		BT_LOGD("LE_AUDIO_MSG_BASS_LOCAL_ADD_SOURCE: sync handle %08x, source_id %d\r\n",
				p_data->handle, p_data->source_id);
		p_sync_dev_info = bt_stack_le_audio_sync_dev_find(p_data->handle);
		if (!p_sync_dev_info) {
			BT_LOGD("[APP] %s bt_stack_le_audio_sync_dev_find fail\r\n", __func__);
			break;
		}
		p_sync_dev_info->source_id = p_data->source_id;
	}
	break;

	case LE_AUDIO_MSG_BASS_GET_PA_SYNC_PARAM: {
		T_BASS_GET_PA_SYNC_PARAM *p_data = (T_BASS_GET_PA_SYNC_PARAM *)buf;
		APP_PRINT_INFO4("LE_AUDIO_MSG_BASS_GET_PA_SYNC_PARAM: sync handle %p, source_id %d, is_past %d, pa_interval 0x%x",
						p_data->handle, p_data->source_id, p_data->is_past, p_data->pa_interval);
		BT_LOGD("LE_AUDIO_MSG_BASS_GET_PA_SYNC_PARAM: sync handle %08x, source_id %d, is_past %d, pa_interval 0x%x\r\n",
				p_data->handle, p_data->source_id, p_data->is_past, p_data->pa_interval);
		rtk_bt_le_audio_bass_get_pa_sync_param_ind_t *p_ind = NULL;
		rtk_bt_le_audio_bass_pa_sync_param_t pa_sync_param = {0};
		if (p_data->is_past) {
			p_sync_dev_info = bt_stack_le_audio_sync_dev_add(p_data->handle);
		} else {
			p_sync_dev_info = bt_stack_le_audio_sync_dev_find(p_data->handle);
		}
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s le audio sync handle add or find fail\r\n", __func__);
			break;
		}
		p_sync_dev_info->is_past = p_data->is_past;
		p_sync_dev_info->pa_interval = p_data->pa_interval;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_BASS_GET_PA_SYNC_PARAM_IND,
									sizeof(rtk_bt_le_audio_bass_get_pa_sync_param_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_bass_get_pa_sync_param_ind_t *)p_evt->data;
		p_ind->sync_handle = p_data->handle;
		p_ind->source_id = p_data->source_id;
		p_ind->is_past = p_data->is_past;
		p_ind->pa_interval = p_data->pa_interval;
		p_ind->p_pa_sync_param = &pa_sync_param;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, &cb_ret);
		if (cb_ret == RTK_BT_EVT_CB_OK) {
			BT_LOGD(
				"%s: RTK_BT_LE_AUDIO_EVT_BASS_GET_PA_SYNC_PARAM_IND, sync_handle = %08x,source_id = %d,pa_sync_options = %d,pa_sync_skip = %d,pa_sync_timeout = %d,past_timeout = %d\r\n",
				__func__,
				p_data->handle, p_data->source_id,
				pa_sync_param.pa_sync_options, pa_sync_param.pa_sync_skip, pa_sync_param.pa_sync_timeout, pa_sync_param.past_timeout);
			if (false == bass_cfg_pa_sync_param(p_data->source_id, pa_sync_param.pa_sync_options, pa_sync_param.pa_sync_skip, pa_sync_param.pa_sync_timeout,
												pa_sync_param.past_timeout)) {
				BT_LOGE("%s: bass_cfg_pa_sync_param fail!\r\n", __func__);
			}
		}
	}
	break;

	case LE_AUDIO_MSG_BASS_GET_BIG_SYNC_PARAM: {
		T_BASS_GET_BIG_SYNC_PARAM *p_data = (T_BASS_GET_BIG_SYNC_PARAM *)buf;
		APP_PRINT_INFO2("LE_AUDIO_MSG_BASS_GET_BIG_SYNC_PARAM: sync handle %p, source_id %d",
						p_data->handle, p_data->source_id);
		BT_LOGD("LE_AUDIO_MSG_BASS_GET_BIG_SYNC_PARAM: sync handle %08x, source_id %d\r\n",
				p_data->handle, p_data->source_id);
		rtk_bt_le_audio_bass_get_big_sync_param_ind_t *p_ind = NULL;
		rtk_bt_le_audio_bass_big_sync_param_t big_sync_param = {0};

		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_BASS_GET_BIG_SYNC_PARAM_IND,
									sizeof(rtk_bt_le_audio_bass_get_big_sync_param_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_bass_get_big_sync_param_ind_t *)p_evt->data;
		p_ind->sync_handle = p_data->handle;
		p_ind->source_id = p_data->source_id;
		p_ind->p_big_sync_param = &big_sync_param;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, &cb_ret);
		if (cb_ret == RTK_BT_EVT_CB_OK) {
			BT_LOGD("%s: RTK_BT_LE_AUDIO_EVT_BASS_GET_BIG_SYNC_PARAM_IND big_mse=%d,big_sync_timeout_10=%d\r\n", __func__, big_sync_param.big_mse,
					big_sync_param.big_sync_timeout);
			if (false == bass_cfg_big_sync_param(p_data->source_id, big_sync_param.big_mse, big_sync_param.big_sync_timeout)) {
				BT_LOGE("%s: bass_cfg_big_sync_param fail!\r\n", __func__);
			}
		}
	}
	break;

	case LE_AUDIO_MSG_BASS_GET_BROADCAST_CODE: {
		T_BASS_GET_BROADCAST_CODE *p_data = (T_BASS_GET_BROADCAST_CODE *)buf;
		APP_PRINT_INFO2("LE_AUDIO_MSG_BASS_GET_BROADCAST_CODE: sync handle %p, source_id %d",
						p_data->handle, p_data->source_id);
		BT_LOGD("LE_AUDIO_MSG_BASS_GET_BROADCAST_CODE: sync handle %08x, source_id %d\r\n",
				p_data->handle, p_data->source_id);
	}
	break;

	case LE_AUDIO_MSG_BASS_GET_PREFER_BIS_SYNC: {
		T_BASS_SET_PREFER_BIS_SYNC *p_data = (T_BASS_SET_PREFER_BIS_SYNC *)buf;
		T_BLE_AUDIO_SYNC_INFO sync_info = {0};
		uint32_t select_bis_array = 0;
		APP_PRINT_INFO3("LE_AUDIO_MSG_BASS_GET_PREFER_BIS_SYNC: sync handle %p, source_id %d, num_subgroups %d",
						p_data->handle, p_data->source_id,
						p_data->num_subgroups);
		BT_LOGD("LE_AUDIO_MSG_BASS_GET_PREFER_BIS_SYNC: sync handle %08x, source_id %d, num_subgroups %d\r\n",
				p_data->handle, p_data->source_id, p_data->num_subgroups);
		p_sync_dev_info = bt_stack_le_audio_sync_dev_find(p_data->handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s bt_stack_le_audio_sync_dev_find fail\r\n", __func__);
			break;
		}
		if (ble_audio_sync_get_info(p_data->handle, &sync_info)) {
			if (sync_info.p_base_mapping) {
				for (uint8_t i = 0; i < p_data->num_subgroups; i++) {
					if (p_data->p_cp_bis_info[i].bis_sync == BASS_CP_BIS_SYNC_NO_PREFER) {
						if (i < sync_info.p_base_mapping->num_subgroups) {
							select_bis_array = (p_sync_dev_info->support_bis_array & sync_info.p_base_mapping->p_subgroup[i].bis_array);
							if (select_bis_array != 0) {
								APP_PRINT_INFO2("BASE Mapping: bis_array 0x%x, select bis_array 0x%x",
												sync_info.p_base_mapping->p_subgroup[i].bis_array, select_bis_array);
								BT_LOGD("BASE Mapping: bis_array 0x%x, select bis_array 0x%x\r\n",
										(unsigned int)sync_info.p_base_mapping->p_subgroup[i].bis_array, (unsigned int)select_bis_array);
								bass_cfg_prefer_bis_sync(p_data->source_id, i, select_bis_array);
							}
						}
					}
				}
			}
		}
	}
	break;

	case LE_AUDIO_MSG_BASS_BRS_CHAR_NO_EMPTY: {
		T_BASS_BRS_CHAR_NO_EMPTY *p_data = (T_BASS_BRS_CHAR_NO_EMPTY *)buf;
		APP_PRINT_INFO1("LE_AUDIO_MSG_BASS_BRS_CHAR_NO_EMPTY:conn_handle 0x%x", p_data->conn_handle);
		BT_LOGD("LE_AUDIO_MSG_BASS_BRS_CHAR_NO_EMPTY:conn_handle 0x%x\r\n", p_data->conn_handle);
	}
	break;

	default:
		// BT_LOGE("%s: unknown msg 0x%04x, not process!\r\n", __func__, msg);
		// app_result = BLE_AUDIO_CB_RESULT_REJECT;
		break;
	}

	return app_result;
}

static uint16_t bt_stack_le_audio_pa_sync_state_change(T_BLE_AUDIO_SYNC_HANDLE sync_handle, T_BLE_AUDIO_PA_SYNC_STATE *p_pa_sync_state,
													   rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info)
{
	bool indicate = false;
	rtk_bt_evt_t *p_evt = NULL;

	if (p_pa_sync_state == NULL) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	switch (p_pa_sync_state->sync_state) {
	case GAP_PA_SYNC_STATE_TERMINATED: {
		BT_LOGD("%s GAP_PA_SYNC_STATE_TERMINATED\r\n", __func__);
		if (p_sync_dev_info) {
			p_sync_dev_info->big_proc_flags &= ~RTK_BT_LE_AUDIO_BIG_PROC_BIG_INFO_RECEIVED;
			p_sync_dev_info->support_bis_array = 0;
		}
		indicate = true;
		break;
	}
	case GAP_PA_SYNC_STATE_SYNCHRONIZING_SCAN_IDLE: {
		BT_LOGD("%s GAP_PA_SYNC_STATE_SYNCHRONIZING_SCAN_IDLE\r\n", __func__);
		break;
	}
	case GAP_PA_SYNC_STATE_SYNCHRONIZING_WAIT_SCANNING: {
		BT_LOGD("%s GAP_PA_SYNC_STATE_SYNCHRONIZING_WAIT_SCANNING\r\n", __func__);
		break;
	}
	case GAP_PA_SYNC_STATE_SYNCHRONIZING: {
		BT_LOGD("%s GAP_PA_SYNC_STATE_SYNCHRONIZING\r\n", __func__);
		indicate = true;
		break;
	}
	case GAP_PA_SYNC_STATE_SYNCHRONIZED: {
		BT_LOGD("%s GAP_PA_SYNC_STATE_SYNCHRONIZED\r\n", __func__);
		bt_stack_le_audio_sync_list_show();
		indicate = true;
		break;
	}
	case GAP_PA_SYNC_STATE_TERMINATING: {
		indicate = true;
		BT_LOGD("%s GAP_PA_SYNC_STATE_TERMINATING\r\n", __func__);

		break;
	}
	default:
		break;
	}

	if (indicate) {
		rtk_bt_le_audio_pa_sync_state_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_PA_SYNC_STATE_IND,
									sizeof(rtk_bt_le_audio_pa_sync_state_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			return RTK_BT_FAIL;
		}
		p_ind = (rtk_bt_le_audio_pa_sync_state_ind_t *)p_evt->data;
		p_ind->cause = p_pa_sync_state->cause;
		p_ind->sync_handle = sync_handle;
		p_ind->sync_state = p_pa_sync_state->sync_state;
		p_ind->action = p_pa_sync_state->action;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_big_sync_state_indicate(T_BLE_AUDIO_BIG_SYNC_STATE *p_big_sync_state, T_BLE_AUDIO_SYNC_HANDLE sync_handle)
{
	bool indicate = false;
	rtk_bt_evt_t *p_evt = NULL;

	if (p_big_sync_state == NULL) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	switch (p_big_sync_state->sync_state) {
	case BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATED: {
		BT_LOGD("%s BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATED\r\n", __func__);
		indicate = true;
		break;
	}
	case BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZING: {
		BT_LOGD("%s BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZING\r\n", __func__);
		break;
	}
	case BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED: {
		BT_LOGD("%s BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED\r\n", __func__);
		indicate = true;
		break;
	}
	case BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATING: {
		BT_LOGD("%s BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATING\r\n", __func__);
		break;
	}
	default:
		break;
	}
	if (indicate) {
		T_BLE_AUDIO_SYNC_INFO sync_info;
		if (!ble_audio_sync_get_info(sync_handle, &sync_info)) {
			BT_LOGE("[BAP] %s ble_audio_sync_get_info fail\r\n", __func__);
			return 1;
		}
		BT_LOGD("[BT STACK] adv_type %d, advertiser_address = [%02x:%02x:%02x:%02x:%02x:%02x], adv_sid: 0x%x, broadcast_id [%02x:%02x:%02x]\r\n",
				sync_info.advertiser_address_type,
				sync_info.advertiser_address[5], sync_info.advertiser_address[4],
				sync_info.advertiser_address[3], sync_info.advertiser_address[2],
				sync_info.advertiser_address[1], sync_info.advertiser_address[0],
				sync_info.adv_sid,
				sync_info.broadcast_id[0],
				sync_info.broadcast_id[1],
				sync_info.broadcast_id[2]);
		rtk_bt_le_audio_big_sync_state_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_BIG_SYNC_STATE_IND,
									sizeof(rtk_bt_le_audio_big_sync_state_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			return RTK_BT_FAIL;
		}
		p_ind = (rtk_bt_le_audio_big_sync_state_ind_t *)p_evt->data;
		p_ind->sync_state = p_big_sync_state->sync_state;
		p_ind->encryption = p_big_sync_state->encryption;
		p_ind->action = p_big_sync_state->action;
		p_ind->action_role = p_big_sync_state->action_role;
		p_ind->cause = p_big_sync_state->cause;
		p_ind->adv_type = sync_info.advertiser_address_type;
		memcpy((void *)p_ind->adv_addr_val, (void *)sync_info.advertiser_address, RTK_BD_ADDR_LEN);
		p_ind->adv_sid = sync_info.adv_sid;
		memcpy((void *)p_ind->broadcast_id, (void *)sync_info.broadcast_id, 3);
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}

	return RTK_BT_OK;
}

static void bt_stack_le_audio_sync_cb(T_BLE_AUDIO_SYNC_HANDLE sync_handle, uint8_t cb_type, void *p_cb_data)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;

	//BT_LOGD("[%s] sync_handle = %08x, cb_type = 0x%x\r\n", __func__,sync_handle,cb_type);

	T_BLE_AUDIO_SYNC_CB_DATA *p_sync_cb = (T_BLE_AUDIO_SYNC_CB_DATA *)p_cb_data;
	switch (cb_type) {
	case MSG_BLE_AUDIO_SYNC_HANDLE_RELEASED: {
		APP_PRINT_TRACE1("MSG_BLE_AUDIO_SYNC_HANDLE_RELEASED: action_role %d", p_sync_cb->p_sync_handle_released->action_role);
		BT_LOGD("MSG_BLE_AUDIO_SYNC_HANDLE_RELEASED: action_role %d", p_sync_cb->p_sync_handle_released->action_role);
		p_sync_dev_info = bt_stack_le_audio_sync_dev_find(sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[BAP] %s bt_stack_le_audio_sync_dev_find fail\r\n", __func__);
			break;
		}
		bt_stack_le_audio_sync_dev_remove(p_sync_dev_info->sync_handle);
	}
	break;

	case MSG_BLE_AUDIO_ADDR_UPDATE: {
		APP_PRINT_TRACE1("MSG_BLE_AUDIO_ADDR_UPDATE: advertiser_address %s", TRACE_BDADDR(p_sync_cb->p_addr_update->advertiser_address));
		BT_LOGD("MSG_BLE_AUDIO_ADDR_UPDATE: advertiser_address %s", TRACE_BDADDR(p_sync_cb->p_addr_update->advertiser_address));
	}
	break;

	case MSG_BLE_AUDIO_PA_SYNC_STATE: {
		PROTOCOL_PRINT_TRACE3("MSG_BLE_AUDIO_PA_SYNC_STATE: sync_state %d, action %d, cause 0x%x\r\n",
							  p_sync_cb->p_pa_sync_state->sync_state,
							  p_sync_cb->p_pa_sync_state->action,
							  p_sync_cb->p_pa_sync_state->cause);
		BT_LOGD("MSG_BLE_AUDIO_PA_SYNC_STATE: sync_state %d, action %d, cause 0x%x\r\n",
				p_sync_cb->p_pa_sync_state->sync_state,
				p_sync_cb->p_pa_sync_state->action,
				p_sync_cb->p_pa_sync_state->cause);
		p_sync_dev_info = bt_stack_le_audio_sync_dev_find(sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[BAP] %s bt_stack_le_audio_sync_dev_find fail\r\n", __func__);
			break;
		}
		p_sync_dev_info->pa_sync_state = p_sync_cb->p_pa_sync_state->sync_state;
		bt_stack_le_audio_pa_sync_state_change(sync_handle, p_sync_cb->p_pa_sync_state, p_sync_dev_info);
		if (p_sync_dev_info) {
			bt_stack_le_audio_check_sync(p_sync_dev_info);
		}
	}
	break;

	case MSG_BLE_AUDIO_PA_REPORT_INFO: {
		T_LE_PERIODIC_ADV_REPORT_INFO *p_info = p_sync_cb->p_le_periodic_adv_report_info;
		PROTOCOL_PRINT_TRACE5("MSG_BLE_AUDIO_PA_REPORT_INFO: sync_id %d,sync_handle 0x%x,tx_power %d,data_status %d,data_len %d\r\n",
							  p_info->sync_id,
							  p_info->sync_handle,
							  p_info->tx_power,
							  p_info->data_status,
							  p_info->data_len);
		BT_LOGD("MSG_BLE_AUDIO_PA_REPORT_INFO: sync_id %d,sync_handle 0x%x,tx_power %d,data_status %d,data_len %d\r\n",
				p_info->sync_id,
				p_info->sync_handle,
				p_info->tx_power,
				p_info->data_status,
				p_info->data_len);
		if (bt_le_audio_priv_data.bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) {
			p_sync_dev_info = bt_stack_le_audio_sync_dev_find(sync_handle);
			if (!p_sync_dev_info) {
				BT_LOGE("[BAP] %s bt_stack_le_audio_sync_dev_find fail\r\n", __func__);
				break;
			}
			if (p_sync_dev_info->big_sync_state == BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED &&
				p_sync_dev_info->pa_sync_state == GAP_PA_SYNC_STATE_SYNCHRONIZED) {
				//APP can terminate PA to save power when BIG state is BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED.
				bool ret = false;
				ret = ble_audio_pa_terminate(p_sync_dev_info->sync_handle);
				BT_LOGD("[BAP] %s: PA sync terminate %s\r\n", __func__, (ret == true) ? "success" : "fail");
			}
		}
	}
	break;

	case MSG_BLE_AUDIO_PA_BIGINFO: {
		PROTOCOL_PRINT_TRACE1("MSG_BLE_AUDIO_PA_BIGINFO: num_bis %d\r\n",
							  p_sync_cb->p_le_biginfo_adv_report_info->num_bis);
		BT_LOGD("MSG_BLE_AUDIO_PA_BIGINFO: num_bis %d\r\n",
				p_sync_cb->p_le_biginfo_adv_report_info->num_bis);
		T_LE_BIGINFO_ADV_REPORT_INFO *p_info = p_sync_cb->p_le_biginfo_adv_report_info;
		if (bt_le_audio_priv_data.bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) {
			p_sync_dev_info = bt_stack_le_audio_sync_dev_find(sync_handle);
			if (!p_sync_dev_info) {
				BT_LOGE("[BAP] %s bt_stack_le_audio_sync_dev_find fail\r\n", __func__);
				break;
			}
			if ((p_sync_dev_info->big_proc_flags & RTK_BT_LE_AUDIO_BIG_PROC_BIG_INFO_RECEIVED) == 0) {
				p_sync_dev_info->big_proc_flags |= RTK_BT_LE_AUDIO_BIG_PROC_BIG_INFO_RECEIVED;
				p_sync_dev_info->encryption = p_sync_cb->p_le_biginfo_adv_report_info->encryption;
				bt_stack_le_audio_check_sync(p_sync_dev_info);
			}
			{
				rtk_bt_le_audio_big_info_adv_report_t *p_report = NULL;
				p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
											RTK_BT_LE_AUDIO_EVT_BIG_INFO_ADV_REPORT,
											sizeof(rtk_bt_le_audio_big_info_adv_report_t));
				if (!p_evt) {
					BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
					break;
				}
				p_report = (rtk_bt_le_audio_big_info_adv_report_t *)p_evt->data;
				p_report->sync_id = p_info->sync_id;
				p_report->sync_handle = p_info->sync_handle;
				p_report->num_bis = p_info->num_bis;
				p_report->nse = p_info->nse;
				p_report->iso_interval = p_info->iso_interval;
				p_report->bn = p_info->bn;
				p_report->pto = p_info->pto;
				p_report->irc = p_info->irc;
				p_report->max_pdu = p_info->max_pdu;
				p_report->sdu_interval = p_info->sdu_interval;
				p_report->max_sdu = p_info->max_sdu;
				p_report->phy = p_info->phy;
				p_report->framing = p_info->framing;
				p_report->encryption = p_info->encryption;
				p_report->cb_sync_handle = sync_handle;
				/* Send event */
				rtk_bt_evt_indicate(p_evt, NULL);
			}
		}
	}
	break;

	case MSG_BLE_AUDIO_BIG_SYNC_STATE: {
		PROTOCOL_PRINT_TRACE3("MSG_BLE_AUDIO_BIG_SYNC_STATE: sync_state %d, action %d, cause 0x%x\r\n",
							  p_sync_cb->p_big_sync_state->sync_state,
							  p_sync_cb->p_big_sync_state->action,
							  p_sync_cb->p_big_sync_state->cause);
		BT_LOGD("MSG_BLE_AUDIO_BIG_SYNC_STATE: sync_state %d, action %d, cause 0x%x\r\n",
				p_sync_cb->p_big_sync_state->sync_state,
				p_sync_cb->p_big_sync_state->action,
				p_sync_cb->p_big_sync_state->cause);
		p_sync_dev_info = bt_stack_le_audio_sync_dev_find(sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[BAP] %s bt_stack_le_audio_sync_dev_find fail\r\n", __func__);
			break;
		} else {
			p_sync_dev_info->big_sync_state = p_sync_cb->p_big_sync_state->sync_state;
			bt_stack_le_audio_check_sync(p_sync_dev_info);
			if (p_sync_cb->p_big_sync_state->sync_state == BIG_SYNC_RECEIVER_SYNC_STATE_SYNCHRONIZED &&
				p_sync_cb->p_big_sync_state->action == BLE_AUDIO_BIG_SYNC &&
				(bt_le_audio_priv_data.bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK)) {
				T_BLE_AUDIO_BIS_INFO bis_sync_info;
				uint8_t codec_id[5] = {RTK_BT_LE_LC3_CODEC_ID, 0, 0, 0, 0};
				uint32_t controller_delay = 0x1122;

				if (!ble_audio_get_bis_sync_info(sync_handle, &bis_sync_info)) {
					BT_LOGE("[BAP] %s ble_audio_get_bis_sync_info fail\r\n", __func__);
					break;
				}
				for (uint8_t i = 0; i < bis_sync_info.bis_num; i++) {
					if (false == ble_audio_bis_setup_data_path(sync_handle, bis_sync_info.bis_info[i].bis_idx, codec_id, controller_delay, 0, NULL)) {
						BT_LOGE("[BAP] %s ble_audio_bis_setup_data_path fail, bis_idx: 0x%x\r\n", __func__, bis_sync_info.bis_info[i].bis_idx);
					}
				}
				//stop ext scan to save power when BIG sync
				if (p_sync_dev_info->is_past == false) {
					bool enable = false;
					if (bt_stack_le_audio_ext_scan_act(&enable)) {
						BT_LOGE("[BAP] %s bt_stack_le_audio_ext_scan_act fail \r\n", __func__);
					}
				}
			}
			bt_stack_le_audio_big_sync_state_indicate(p_sync_cb->p_big_sync_state, sync_handle);
		}
	}
	break;

	case MSG_BLE_AUDIO_BIG_SETUP_DATA_PATH: {
		rtk_bt_le_audio_bis_conn_info_t bis_conn_info = {0};
		rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;
		PROTOCOL_PRINT_TRACE2("MSG_BLE_AUDIO_BIG_SETUP_DATA_PATH: bis_idx %d, cause 0x%x\r\n",
							  p_sync_cb->p_setup_data_path->bis_idx,
							  p_sync_cb->p_setup_data_path->cause);
		BT_LOGD("MSG_BLE_AUDIO_BIG_SETUP_DATA_PATH: bis_idx %d, cause 0x%x\r\n",
				p_sync_cb->p_setup_data_path->bis_idx,
				p_sync_cb->p_setup_data_path->cause);
		bis_conn_info.sync_handle = sync_handle;
		bis_conn_info.bis_conn_handle = p_sync_cb->p_setup_data_path->bis_conn_handle;
		bis_conn_info.bis_idx = p_sync_cb->p_setup_data_path->bis_idx;
		bis_conn_info.path_direction = RTK_BLE_AUDIO_ISO_DATA_PATH_RX;
		p_iso_chann = bt_stack_le_audio_data_path_add(MSG_BLE_AUDIO_BIG_SETUP_DATA_PATH, RTK_BT_LE_AUDIO_BIS_MODE, RTK_BLE_AUDIO_ISO_DATA_PATH_RX,
													  (void *)&bis_conn_info);
		if (!p_iso_chann) {
			BT_LOGE("[BAP] %s bt_stack_le_audio_data_path_add fail\r\n", __func__);
			break;
		}
		{
			T_BLE_AUDIO_SYNC_INFO sync_info = {0};
			rtk_bt_le_audio_big_setup_data_path_ind_t *p_ind = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
										RTK_BT_LE_AUDIO_EVT_BIG_SETUP_DATA_PATH,
										sizeof(rtk_bt_le_audio_big_setup_data_path_ind_t));
			if (!p_evt) {
				BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
				break;
			}
			p_ind = (rtk_bt_le_audio_big_setup_data_path_ind_t *)p_evt->data;
			p_ind->bis_idx = p_sync_cb->p_setup_data_path->bis_idx;
			p_ind->bis_conn_handle = p_sync_cb->p_setup_data_path->bis_conn_handle;
			p_ind->cause = p_sync_cb->p_setup_data_path->cause;
			p_ind->iso_chann_t.p_iso_chann = (void *)p_iso_chann;
			p_ind->iso_chann_t.iso_conn_handle = p_iso_chann->iso_conn_handle;
			p_ind->iso_chann_t.path_direction = RTK_BLE_AUDIO_ISO_DATA_PATH_RX;
			p_ind->sync_handle = sync_handle;
			p_ind->presentation_delay = p_iso_chann->presentation_delay;
			if (ble_audio_sync_get_info(sync_handle, &sync_info)) {
				p_ind->iso_chann_t.iso_interval = sync_info.big_info.iso_interval;
				base_data_get_bis_codec_cfg(sync_info.p_base_mapping, p_sync_cb->p_setup_data_path->bis_idx, (T_CODEC_CFG *)&p_ind->codec_t);
			} else {
				BT_LOGE("%s ble_audio_sync_get_info fail\r\n", __func__);
			}
			/* Send event */
			rtk_bt_evt_indicate(p_evt, NULL);
		}
	}
	break;

	case MSG_BLE_AUDIO_BIG_REMOVE_DATA_PATH: {
		PROTOCOL_PRINT_TRACE2("MSG_BLE_AUDIO_BIG_REMOVE_DATA_PATH: bis_idx %d, cause 0x%x\r\n",
							  p_sync_cb->p_remove_data_path->bis_idx,
							  p_sync_cb->p_remove_data_path->cause);
		BT_LOGD("MSG_BLE_AUDIO_BIG_REMOVE_DATA_PATH: bis_idx %d, cause 0x%x\r\n",
				p_sync_cb->p_remove_data_path->bis_idx,
				p_sync_cb->p_remove_data_path->cause);
		bt_stack_le_audio_handle_data_path_remove(p_sync_cb->p_remove_data_path->bis_conn_handle, RTK_BLE_AUDIO_ISO_DATA_PATH_RX);
		{
			rtk_bt_le_audio_big_remove_data_path_ind_t *p_ind = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
										RTK_BT_LE_AUDIO_EVT_BIG_REMOVE_DATA_PATH,
										sizeof(rtk_bt_le_audio_big_remove_data_path_ind_t));
			if (!p_evt) {
				BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
				break;
			}
			p_ind = (rtk_bt_le_audio_big_remove_data_path_ind_t *)p_evt->data;
			p_ind->bis_idx = p_sync_cb->p_remove_data_path->bis_idx;
			p_ind->bis_conn_handle = p_sync_cb->p_remove_data_path->bis_conn_handle;
			p_ind->cause = p_sync_cb->p_remove_data_path->cause;
			p_ind->path_direction = RTK_BLE_AUDIO_ISO_DATA_PATH_RX;
			p_ind->sync_handle = sync_handle;
			/* Send event */
			rtk_bt_evt_indicate(p_evt, NULL);
		}
	}
	break;

	case MSG_BLE_AUDIO_BASE_DATA_MODIFY_INFO: {
		APP_PRINT_TRACE1("MSG_BLE_AUDIO_BASE_DATA_MODIFY_INFO: p_base_mapping %p\r\n",
						 p_sync_cb->p_base_data_modify_info->p_base_mapping);
		BT_LOGD("MSG_BLE_AUDIO_BASE_DATA_MODIFY_INFO: p_base_mapping %08x\r\n",
				p_sync_cb->p_base_data_modify_info->p_base_mapping);
		if (p_sync_cb->p_base_data_modify_info->p_base_mapping) {
			p_sync_dev_info = bt_stack_le_audio_sync_dev_find(sync_handle);
			if (!p_sync_dev_info) {
				BT_LOGE("[BAP] %s bt_stack_le_audio_sync_dev_find fail\r\n", __func__);
				break;
			}
			bt_stack_le_audio_bass_get_supported_bis(p_sync_dev_info);
		}
	}
	break;

	default:
		BT_LOGE("%s unhandled cb_type 0x%x\r\n", __func__, cb_type);
		break;
	}
	return;
}

uint16_t bt_stack_le_audio_data_send(void *data)
{
	T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;
	rtk_bt_le_audio_iso_data_send_info_t *param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	param = (rtk_bt_le_audio_iso_data_send_info_t *)data;

	BT_DUMPD(__func__, param->p_data, param->data_len);

#if 0
	static bool is_odd = false;
	static uint32_t last_even_time = 0, now_even_time = 0, last_odd_time = 0, now_odd_time = 0;
	if (is_odd) {
		now_odd_time = osif_sys_time_get();
		if (last_odd_time != 0 && (now_odd_time - last_odd_time > 11 || now_odd_time - last_odd_time < 9)) {
			BT_LOGE("%s gap_iso_send_data iso_conn_handle = 0x%x,pkt_seq_num %d,last_odd_time %d, now_odd_time %d,delta_time %d\r\n", __func__,
					param->iso_conn_handle, param->pkt_seq_num, (int)last_odd_time, (int)now_odd_time, (int)(now_odd_time - last_odd_time));
		}
		last_odd_time = now_odd_time;
		is_odd = false;
	} else {
		now_even_time = osif_sys_time_get();
		if (last_even_time != 0 && (now_even_time - last_even_time > 11 || now_even_time - last_even_time < 9)) {
			BT_LOGE("%s gap_iso_send_data iso_conn_handle = 0x%x,pkt_seq_num %d,last_even_time %d, now_even_time %d,delta_time %d\r\n", __func__,
					param->iso_conn_handle, param->pkt_seq_num, (int)last_even_time, (int)now_even_time, (int)(now_even_time - last_even_time));
		}
		last_even_time = now_even_time;
		is_odd = true;
	}

#endif
	cause = gap_iso_send_data(param->p_data,
							  param->iso_conn_handle,
							  param->data_len,
							  param->ts_flag,
							  param->time_stamp,
							  param->pkt_seq_num);
	if (GAP_CAUSE_SUCCESS != cause) {
		if (cause == GAP_CAUSE_ERROR_CREDITS) {
			BT_LOGE("%s gap_iso_send_data warning (cause = 0x%x,iso_conn_handle = 0x%x)\r\n", __func__, cause, param->iso_conn_handle);
			return RTK_BT_ERR_NO_CREDITS;
		} else {
			BT_LOGE("%s gap_iso_send_data fail (cause = 0x%x,iso_conn_handle = 0x%x)\r\n", __func__, cause, param->iso_conn_handle);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	} else {
		BT_LOGD("%s gap_iso_send_data ok,iso_conn_handle=0x%x,pkt_seq_num=%d,time_stamp=%d, time = %d\r\n", __func__, param->iso_conn_handle,
				param->pkt_seq_num, (int)param->time_stamp, (int)osif_sys_time_get());
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_acl_link_allocate(void *data)
{
	rtk_bt_le_audio_link_t *p_link = NULL;
	rtk_bt_le_conn_ind_t *conn_ind = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	conn_ind = (rtk_bt_le_conn_ind_t *)data;
	if (bt_stack_le_audio_find_link_by_conn_handle(conn_ind->conn_handle)) {
		BT_LOGE("%s GAP conncet(handle 0x%x) is already allocated \r\n", __func__, conn_ind->conn_handle);
		return RTK_BT_OK;
	}
	p_link = bt_stack_le_audio_alloc_le_link(conn_ind->conn_handle);
	if (p_link != NULL) {
		p_link->state = RTK_BT_LE_GAP_EVT_CONNECT_IND;
	} else {
		BT_LOGE("%s GAP conncet allocate le link fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_acl_link_free(void *data)
{
	rtk_bt_le_audio_link_t *p_link = NULL;
	rtk_bt_le_disconn_ind_t *disconn_ind = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	disconn_ind = (rtk_bt_le_disconn_ind_t *)data;
	p_link = bt_stack_le_audio_find_link_by_conn_handle(disconn_ind->conn_handle);
	if (!p_link) {
		BT_LOGE("%s GAP disconncet(handle 0x%x) is not in list \r\n", __func__, disconn_ind->conn_handle);
		return RTK_BT_OK;
	}
	bt_stack_le_audio_link_free(p_link);

	return RTK_BT_OK;
}

static uint8_t bt_stack_le_audio_broadcast_gen_pbp_adv_data(uint8_t **p_audio_adv_data)
{
	uint8_t idx = 0;
	uint8_t broadcast_id_idx = 0;
	uint8_t announcements_idx = 0;
	uint8_t feature = 0;
	uint8_t *audio_adv_data;
	uint8_t audio_data_len;
	T_BROADCAST_SOURCE_INFO src_info = {0};

	if (false == broadcast_source_get_info(bt_le_audio_priv_data.bsrc.source_handle, &src_info)) {
		BT_LOGE("[LEA STACK] %s broadcast_source_get_info fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	audio_data_len = 23 + device_name_len;
	audio_adv_data = osif_mem_alloc(RAM_TYPE_DATA_ON, audio_data_len);
	if (!audio_adv_data) {
		BT_LOGE("[LEA STACK] %s allocate audio data fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	audio_adv_data[idx++] = 0x03;
	audio_adv_data[idx++] = RTK_BT_LE_GAP_ADTYPE_APPEARANCE;
	audio_adv_data[idx++] = LO_WORD(RTK_BT_LE_GAP_APPEARANCE_GENERIC_MEDIA_PLAYER);
	audio_adv_data[idx++] = HI_WORD(RTK_BT_LE_GAP_APPEARANCE_GENERIC_MEDIA_PLAYER);
	audio_adv_data[idx++] = 0x06;
	audio_adv_data[idx++] = RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA;
	audio_adv_data[idx++] = (uint8_t)(RTK_BT_LE_BROADCAST_AUDIO_ANNOUNCEMENT_SRV_UUID);
	audio_adv_data[idx++] = (uint8_t)(RTK_BT_LE_BROADCAST_AUDIO_ANNOUNCEMENT_SRV_UUID >> 8);
	broadcast_id_idx = idx;
	audio_adv_data[idx++] = 0x00; // broadcast id
	audio_adv_data[idx++] = 0x00;
	audio_adv_data[idx++] = 0x00;
	audio_adv_data[idx++] = 0x09;
	audio_adv_data[idx++] = RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA;
	audio_adv_data[idx++] = (uint8_t)(RTK_BT_LE_PUBIC_BROADCAST_ANNOUNCEMENT_SRV_UUID);
	audio_adv_data[idx++] = (uint8_t)(RTK_BT_LE_PUBIC_BROADCAST_ANNOUNCEMENT_SRV_UUID >> 8);
	announcements_idx = idx;
	audio_adv_data[idx++] = 0x00; // Public Broadcast Announcement features
	audio_adv_data[idx++] = 0x04; // Metadata length
	audio_adv_data[idx++] = 0x03; // Metadata
	audio_adv_data[idx++] = RTK_BT_LE_METADATA_TYPE_PROGRAM_INFO;
	audio_adv_data[idx++] = 'T';
	audio_adv_data[idx++] = 'V';
	audio_adv_data[idx++] = device_name_len + 1;
	audio_adv_data[idx++] = RTK_BT_LE_GAP_ADTYPE_BROADCAST_NAME;
	memcpy(&audio_adv_data[idx], (void *)bt_stack_device_name, device_name_len);
	idx += device_name_len;
	memcpy(&audio_adv_data[broadcast_id_idx], src_info.broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);
	if (bt_le_audio_priv_data.bsrc.cfg_encryption) {
		feature |= RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_ENCRYPTED;
	}
	if (bt_le_audio_priv_data.bsrc.cfg_codec_index >= RTK_BT_LE_CODEC_CFG_ITEM_48_1) {
		feature |= RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_HIGH_QUALITY_PRESENT;
	} else {
		feature |= RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_STANDARD_QUALITY_PRESENT;
	}
	audio_adv_data[announcements_idx] = feature;
	*p_audio_adv_data = audio_adv_data;

	return idx;
}

static uint16_t bt_stack_le_audio_broadcast_config(void)
{
	uint8_t *p_broadcast_audio_announcements = NULL;
	uint8_t broadcast_audio_announcements_len = 0;
	uint16_t pa_data_len = 0;
	uint8_t *pa_data = NULL;

	if (!bt_le_audio_priv_data.bsrc.source_handle) {
		BT_LOGE("[LEA STACK] %s source handle is NULL \r\n", __func__);
		goto error;
	}
	broadcast_audio_announcements_len = bt_stack_le_audio_broadcast_gen_pbp_adv_data(&p_broadcast_audio_announcements);
	if (broadcast_audio_announcements_len == 0) {
		BT_LOGE("[LEA STACK] %s broadcast_audio_announcements_len is 0 \r\n", __func__);
		goto error;
	}

	if (false == broadcast_source_set_eadv_param(bt_le_audio_priv_data.bsrc.source_handle,
												 bt_le_audio_priv_data.bsrc.source_adv_sid,
												 bt_le_audio_broadcast_source_config_param.primary_adv_interval_min,
												 bt_le_audio_broadcast_source_config_param.primary_adv_interval_max,
												 bt_le_audio_broadcast_source_config_param.primary_adv_channel_map,
												 (T_GAP_LOCAL_ADDR_TYPE)bt_le_audio_priv_data.bsrc.cfg_local_addr_type,
												 bt_le_audio_priv_data.bsrc.source_address,
												 (T_GAP_ADV_FILTER_POLICY)bt_le_audio_broadcast_source_config_param.filter_policy,
												 bt_le_audio_broadcast_source_config_param.tx_power,
												 (T_GAP_PHYS_PRIM_ADV_TYPE)bt_le_audio_broadcast_source_config_param.primary_adv_phy,
												 bt_le_audio_broadcast_source_config_param.secondary_adv_max_skip,
												 (T_GAP_PHYS_TYPE)bt_le_audio_broadcast_source_config_param.secondary_adv_phy,
												 broadcast_audio_announcements_len,
												 p_broadcast_audio_announcements)) {
		BT_LOGE("[LEA STACK] %s broadcast_source_set_eadv_param fail \r\n", __func__);
		goto error;
	}

	for (uint8_t i = 0; i < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_GROUP_NUM; i++) {
		if (false == base_data_gen_pa_data(bt_le_audio_priv_data.bsrc.group[i].group_idx, &pa_data_len, &pa_data)) {
			BT_LOGE("%s fail: base_data_gen_pa_data fail\r\n", __func__);
			goto error;
		}
		if (false == broadcast_source_set_pa_param(bt_le_audio_priv_data.bsrc.source_handle,
												   bt_le_audio_broadcast_source_config_param.periodic_adv_interval_min,
												   bt_le_audio_broadcast_source_config_param.periodic_adv_interval_max,
												   bt_le_audio_broadcast_source_config_param.periodic_adv_prop,
												   pa_data_len,
												   pa_data)) {
			BT_LOGE("%s fail: broadcast_source_set_pa_param fail\r\n", __func__);
			goto error;
		}
	}

	if (false == broadcast_source_config(bt_le_audio_priv_data.bsrc.source_handle)) {
		BT_LOGE("%s fail: broadcast_source_config fail\r\n", __func__);
		goto error;
	}

	return RTK_BT_OK;

error:
	if (p_broadcast_audio_announcements) {
		osif_mem_free(p_broadcast_audio_announcements);
	}
	if (pa_data) {
		osif_mem_free(pa_data);
	}
	return RTK_BT_ERR_LOWER_STACK_API;
}

static bool bt_stack_le_audio_broadcast_establish(void)
{
	T_BIG_MGR_ISOC_BROADCASTER_CREATE_BIG_PARAM big_param = {0};

	big_param.num_bis = bt_le_audio_priv_data.bsrc.group[0].group_bis_num;
	big_param.sdu_interval = bt_le_audio_priv_data.bsrc.prefer_qos.sdu_interval;
#if (RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_NUM == 1) && (RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE == 1)
	big_param.max_sdu = 2 * bt_le_audio_priv_data.bsrc.prefer_qos.max_sdu;
#else
	big_param.max_sdu = bt_le_audio_priv_data.bsrc.prefer_qos.max_sdu;
#endif
	big_param.max_transport_latency = bt_le_audio_priv_data.bsrc.prefer_qos.max_transport_latency;
	big_param.rtn = bt_le_audio_priv_data.bsrc.prefer_qos.retransmission_number;
#if (RTK_BT_LE_AUDIO_BIG_ISO_INTERVAL_CONFIG == RTK_BT_ISO_INTERVAL_20_MS)
	big_param.max_transport_latency = 60;
	BT_LOGA("[LEA STACK] %s: ISO interval=20 ms \r\n", __func__);
#elif (RTK_BT_LE_AUDIO_BIG_ISO_INTERVAL_CONFIG == RTK_BT_ISO_INTERVAL_30_MS)
	big_param.rtn = 3;
	big_param.max_transport_latency = 80;
	BT_LOGA("[LEA STACK] %s: ISO interval=30 ms \r\n", __func__);
#else
	BT_LOGA("[LEA STACK] %s: ISO interval=10 ms \r\n", __func__);
	//use default prefer qos value
#endif
	big_param.phy = RTK_BT_LE_AUDIO_BROADCAST_SOURCE_PHY;
	big_param.packing = 0x00; /* 0x00: Sequential 0x01: Interleaved */
	big_param.framing = bt_le_audio_priv_data.bsrc.prefer_qos.framing;
	big_param.encryption = 0x00;
	if (bt_le_audio_priv_data.bsrc.cfg_encryption) {
		big_param.encryption = 0x01;
		memcpy(big_param.broadcast_code, bt_stack_le_audio_broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
	}
	BT_LOGA("[LEA STACK] %s: num_bis:%d, sdu_interval:%d, rtn: %d, max_sdu: %d, max_transport_latency: %d, phy:0x%x, packing:0x%x, framing:0x%x, encryption: 0x%x \r\n",
			__func__, big_param.num_bis, big_param.sdu_interval, big_param.rtn,
			big_param.max_sdu, big_param.max_transport_latency, big_param.phy,
			big_param.packing, big_param.framing, big_param.encryption);
	if (false == broadcast_source_establish(bt_le_audio_priv_data.bsrc.source_handle, big_param)) {
		BT_LOGE("%s fail: broadcast_source_establish fail\r\n", __func__);
		return false;
	}
	BT_LOGA("[LEA STACK] %s (source handle 0x%x) Success \r\n", __func__, bt_le_audio_priv_data.bsrc.source_handle);

	return true;
}

static uint16_t bt_stack_le_audio_update_broadcast_state(void)
{
	uint16_t ret = RTK_BT_OK;
	const char *bsrc_state[] = {"IDLE", "CONFIGURED_STARTING", "CONFIGURED", "CONFIGURED_STOPPING", "STREAMING_STARTING",
								"STREAMING", "STREAMING_STOPPING"
							   };
	BT_LOGA("[LEA STACK] Update broadcast state: %s prefer state:%s\r\n",
			bsrc_state[bt_le_audio_priv_data.bsrc.state], bsrc_state[bt_le_audio_priv_data.bsrc.prefer_state]);

	if (bt_le_audio_priv_data.bsrc.state == bt_le_audio_priv_data.bsrc.prefer_state) {
		/* broadcast state has moved to wanted */
		return ret;
	}

	switch (bt_le_audio_priv_data.bsrc.state) {
	case RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_IDLE: {
		if (bt_le_audio_priv_data.bsrc.prefer_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_CONFIGURED ||
			bt_le_audio_priv_data.bsrc.prefer_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING) {
			ret = bt_stack_le_audio_broadcast_config();
		}
		break;
	}
	case RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_CONFIGURED: {
		if (bt_le_audio_priv_data.bsrc.prefer_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING) {
			if (false == bt_stack_le_audio_broadcast_establish()) {
				BT_LOGE("%s fail: bt_stack_le_audio_broadcast_establish fail\r\n", __func__);
				ret = RTK_BT_ERR_LOWER_STACK_API;
			}
		} else if (bt_le_audio_priv_data.bsrc.prefer_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_IDLE) {
			if (false == broadcast_source_release(bt_le_audio_priv_data.bsrc.source_handle)) {
				BT_LOGE("%s fail: broadcast_source_release fail\r\n", __func__);
				ret = RTK_BT_ERR_LOWER_STACK_API;
			}
		}
		break;
	}
	case RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING: {
		if (bt_le_audio_priv_data.bsrc.prefer_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_CONFIGURED ||
			bt_le_audio_priv_data.bsrc.prefer_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_IDLE) {
			if (false == broadcast_source_disable(bt_le_audio_priv_data.bsrc.source_handle, HCI_ERR_LOCAL_HOST_TERMINATE)) {
				BT_LOGE("%s fail: broadcast_source_disable fail\r\n", __func__);
				ret = RTK_BT_ERR_LOWER_STACK_API;
			}
		}
		break;
	}
	default:
		break;
	}

	return ret;
}

static void bt_stack_le_audio_broadcast_source_cb(T_BROADCAST_SOURCE_HANDLE handle, uint8_t cb_type, void *p_cb_data)
{
	T_BROADCAST_SOURCE_SM_CB_DATA *p_sm_data = (T_BROADCAST_SOURCE_SM_CB_DATA *)p_cb_data;
	rtk_bt_evt_t *p_evt = NULL;

	BT_LOGD("%s: cb_type = 0x%x\r\n", __func__, cb_type);

	switch (cb_type) {
	case MSG_BROADCAST_SOURCE_STATE_CHANGE: {
		APP_PRINT_INFO2("MSG_BROADCAST_SOURCE_STATE_CHANGE: state %d, cause 0x%x", p_sm_data->p_state_change->state, p_sm_data->p_state_change->cause);
		BT_LOGD("MSG_BROADCAST_SOURCE_STATE_CHANGE: state %d, cause 0x%x\r\n", p_sm_data->p_state_change->state, p_sm_data->p_state_change->cause);
		if (p_sm_data->p_state_change->state == BROADCAST_SOURCE_STATE_CONFIGURED_STARTING) {
			BT_LOGD("%s: BROADCAST_SOURCE_STATE_CONFIGURED_STARTING\r\n", __func__);
		} else if (p_sm_data->p_state_change->state == BROADCAST_SOURCE_STATE_CONFIGURED) {
			BT_LOGD("%s: BROADCAST_SOURCE_STATE_CONFIGURED\r\n", __func__);
		}
		bt_le_audio_priv_data.bsrc.state = (rtk_bt_le_audio_broadcast_source_state_t)p_sm_data->p_state_change->state;
		if (p_sm_data->p_state_change->cause == GAP_SUCCESS ||
			p_sm_data->p_state_change->cause == (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)) {
			bt_stack_le_audio_update_broadcast_state();
			if (p_sm_data->p_state_change->state == BROADCAST_SOURCE_STATE_STREAMING) {
				uint8_t codec_id[5] = {LC3_CODEC_ID, 0, 0, 0, 0};
				BT_LOGA("[BAP] broadcast source streaming\r\n");
				for (uint8_t i = 0; i < bt_le_audio_priv_data.bsrc.group[0].group_bis_num; i ++) {
					if (false == broadcast_source_setup_data_path(handle, bt_le_audio_priv_data.bsrc.group[0].sub_group[0].bis_idx[i], codec_id, 0, 0, NULL)) {
						BT_LOGE("%s fail: broadcast_source_setup_data_path fail\r\n", __func__);
						break;
					}
				}
			}
		}
		rtk_bt_le_audio_broadcast_source_state_ind_t *ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_BROADCAST_SOURCE_STATE_IND,
									sizeof(rtk_bt_le_audio_broadcast_source_state_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		ind = (rtk_bt_le_audio_broadcast_source_state_ind_t *)p_evt->data;
		ind->broadcast_source_handle = handle;
		ind->broadcast_source_state = p_sm_data->p_state_change->state;
		ind->cause = p_sm_data->p_state_change->cause;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case MSG_BROADCAST_SOURCE_METADATA_UPDATE: {
		APP_PRINT_INFO1("MSG_BROADCAST_SOURCE_METADATA_UPDATE: cause 0x%x", p_sm_data->cause);
		BT_LOGD("MSG_BROADCAST_SOURCE_METADATA_UPDATE: cause 0x%x", p_sm_data->cause);
	}
	break;

	case MSG_BROADCAST_SOURCE_RECONFIG: {
		APP_PRINT_INFO1("MSG_BROADCAST_SOURCE_RECONFIG: cause 0x%x", p_sm_data->cause);
		BT_LOGD("MSG_BROADCAST_SOURCE_RECONFIG: cause 0x%x\r\n", p_sm_data->cause);
	}
	break;

	case MSG_BROADCAST_SOURCE_BIG_SYNC_MODE: {
		APP_PRINT_INFO2("MSG_BROADCAST_SOURCE_BIG_SYNC_MODE: big_sync_mode %d, cause 0x%x",
						p_sm_data->p_big_sync_mode->big_sync_mode,
						p_sm_data->p_big_sync_mode->cause);
		BT_LOGD("MSG_BROADCAST_SOURCE_BIG_SYNC_MODE: big_sync_mode %d, cause 0x%x",
				p_sm_data->p_big_sync_mode->big_sync_mode,
				p_sm_data->p_big_sync_mode->cause);
	}
	break;

	case MSG_BROADCAST_SOURCE_SETUP_DATA_PATH: {
		rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;
		rtk_bt_le_audio_bis_conn_handle_info_t bis_info = {0};
		T_BROADCAST_SOURCE_INFO src_info = {0};
		APP_PRINT_INFO2("MSG_BROADCAST_SOURCE_SETUP_DATA_PATH: bis_idx %d, cause 0x%x",
						p_sm_data->p_setup_data_path->bis_idx,
						p_sm_data->p_setup_data_path->cause);
		BT_LOGD("MSG_BROADCAST_SOURCE_SETUP_DATA_PATH: bis_idx %d, cause 0x%x",
				p_sm_data->p_setup_data_path->bis_idx,
				p_sm_data->p_setup_data_path->cause);
		bis_info.bis_idx = p_sm_data->p_setup_data_path->bis_idx;
		bis_info.bis_conn_handle = p_sm_data->p_setup_data_path->bis_conn_handle;
		p_iso_chann = bt_stack_le_audio_data_path_add(MSG_BROADCAST_SOURCE_SETUP_DATA_PATH, RTK_BT_LE_AUDIO_BIS_MODE, RTK_BLE_AUDIO_ISO_DATA_PATH_TX,
													  (void *)&bis_info);
		if (!p_iso_chann) {
			BT_LOGE("[BAP] %s bt_stack_le_audio_data_path_add fail\r\n", __func__);
			break;
		} else {
			T_BROADCAST_SOURCE_INFO src_info = {0};
			if (false == broadcast_source_get_info(bt_le_audio_priv_data.bsrc.source_handle, &src_info)) {
				BT_LOGE("[BAP] %s broadcast_source_get_info fail\r\n", __func__);
			} else {
				p_iso_chann->transport_latency_big = src_info.big_transport_latency;
			}
		}
		rtk_bt_le_audio_bap_big_setup_data_path_ind_t *ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_BROADCAST_SOURCE_SETUP_DATA_PATH_IND,
									sizeof(rtk_bt_le_audio_bap_big_setup_data_path_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		ind = (rtk_bt_le_audio_bap_big_setup_data_path_ind_t *)p_evt->data;
		ind->bis_idx = p_sm_data->p_setup_data_path->bis_idx;
		ind->bis_conn_handle = p_sm_data->p_setup_data_path->bis_conn_handle;
		ind->cause = p_sm_data->p_setup_data_path->cause;
		ind->iso_chann_t.p_iso_chann = (void *)p_iso_chann;
		ind->iso_chann_t.iso_conn_handle = p_iso_chann->iso_conn_handle;
		ind->iso_chann_t.path_direction = RTK_BLE_AUDIO_ISO_DATA_PATH_TX;
		if (broadcast_source_get_info(bt_le_audio_priv_data.bsrc.source_handle, &src_info)) {
			ind->iso_chann_t.iso_interval = src_info.big_iso_interval;
		} else {
			BT_LOGE("%s broadcast_source_get_info fail\r\n", __func__);
		}
		ind->presentation_delay = p_iso_chann->presentation_delay;
		ind->transport_latency_big = p_iso_chann->transport_latency_big;
		memcpy((void *)&ind->codec_t, &bt_le_audio_priv_data.bsrc.codec_cfg, sizeof(rtk_bt_le_audio_cfg_codec_t));
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case MSG_BROADCAST_SOURCE_REMOVE_DATA_PATH: {
		APP_PRINT_INFO2("MSG_BROADCAST_SOURCE_REMOVE_DATA_PATH: bis_conn_handle 0x%x, cause 0x%x",
						p_sm_data->p_remove_data_path->bis_conn_handle, p_sm_data->p_remove_data_path->cause);
		BT_LOGD("MSG_BROADCAST_SOURCE_REMOVE_DATA_PATH: bis_conn_handle %d, cause 0x%x\r\n",
				p_sm_data->p_remove_data_path->bis_conn_handle, p_sm_data->p_remove_data_path->cause);
		bt_stack_le_audio_handle_data_path_remove(p_sm_data->p_remove_data_path->bis_conn_handle, RTK_BLE_AUDIO_ISO_DATA_PATH_TX);
		rtk_bt_le_audio_bap_big_remove_data_path_ind_t *ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_BROADCAST_SOURCE_REMOVE_DATA_PATH_IND,
									sizeof(rtk_bt_le_audio_bap_big_remove_data_path_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		ind = (rtk_bt_le_audio_bap_big_remove_data_path_ind_t *)p_evt->data;
		ind->bis_conn_handle = p_sm_data->p_remove_data_path->bis_conn_handle;
		ind->cause = p_sm_data->p_remove_data_path->cause;
		ind->path_direction = RTK_BLE_AUDIO_ISO_DATA_PATH_TX;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	default:
		BT_LOGE("%s: default cb_type 0x%04x, not process!\r\n", __func__, cb_type);
		break;
	}
}

static bool bt_stack_le_audio_broadcast_source_set_codec_to_level2(uint8_t *p_codec, uint8_t codec_len, uint8_t type, uint16_t value)
{
	uint8_t pos = 0, *p_buffer = NULL;
	uint8_t length = 0;
	bool ret = false;

	p_buffer = p_codec;
	pos = 0;
	while (pos < codec_len) {
		/* Codec_Specific_Capabilities TLV length, one octet. */
		length = (uint8_t) * (p_buffer + pos);
		if (type == (uint8_t) * (p_buffer + pos + 1)) {
			switch (type) {
			case RTK_BT_LE_CODEC_CFG_TYPE_SAMPLING_FREQUENCY: {
				*(p_buffer + pos + 2) = (uint8_t)value & 0xFF;
			}
			break;
			case RTK_BT_LE_CODEC_CFG_TYPE_FRAME_DURATION: {
				*(p_buffer + pos + 2) = (uint8_t)value & 0xFF;
			}
			break;
			case RTK_BT_LE_CODEC_CFG_TYPE_OCTET_PER_CODEC_FRAME: {
				*(p_buffer + pos + 2) = value & 0xFF;
				*(p_buffer + pos + 3) = value >> 8;
			}
			break;
			case RTK_BT_LE_CODEC_CFG_TYPE_BLOCKS_PER_SDU: {
				*(p_buffer + pos + 2) = (uint8_t)value & 0xFF;
			}
			break;
			default:
				break;
			}
		}
		pos += (length + 1);
	}
	BT_DUMPD(__func__, p_codec, codec_len);

	return ret;
}

static void bt_stack_le_audio_free_basic_data(void)
{
	for (uint8_t i = 0; i < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_GROUP_NUM; i ++) {
		if (bt_le_audio_priv_data.bsrc.group[i].group_idx != 0xFF) {
			base_data_del_group(bt_le_audio_priv_data.bsrc.group[i].group_idx);
			bt_le_audio_priv_data.bsrc.group[i].group_idx = 0xFF;
			for (uint8_t j = 0; j < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_SUB_GROUP_NUM; j ++) {
				bt_le_audio_priv_data.bsrc.group[i].sub_group[j].subgroup_idx = 0xFF;
				for (uint8_t k = 0; k < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_NUM; k ++) {
					bt_le_audio_priv_data.bsrc.group[i].sub_group[j].bis_idx[k] = 0xFF;
				}
			}
		}
	}
}

static uint16_t bt_stack_le_audio_gen_basic_data(void)
{
	uint8_t error_idx = 0;
	uint8_t codec_id2[RTK_BT_LE_AUDIO_CODEC_ID_LEN] = {LC3_CODEC_ID, 0, 0, 0, 0};

	/* do group free firstly */
	bt_stack_le_audio_free_basic_data();
	/* do level2 config */
	bt_stack_le_audio_broadcast_source_set_codec_to_level2(bt_stack_le_audio_media_codec_level2, sizeof(bt_stack_le_audio_media_codec_level2),
														   RTK_BT_LE_CODEC_CFG_TYPE_SAMPLING_FREQUENCY, bt_le_audio_priv_data.bsrc.codec_cfg.sample_frequency);
	bt_stack_le_audio_broadcast_source_set_codec_to_level2(bt_stack_le_audio_media_codec_level2, sizeof(bt_stack_le_audio_media_codec_level2),
														   RTK_BT_LE_CODEC_CFG_TYPE_FRAME_DURATION, bt_le_audio_priv_data.bsrc.codec_cfg.frame_duration);
	bt_stack_le_audio_broadcast_source_set_codec_to_level2(bt_stack_le_audio_media_codec_level2, sizeof(bt_stack_le_audio_media_codec_level2),
														   RTK_BT_LE_CODEC_CFG_TYPE_OCTET_PER_CODEC_FRAME, bt_le_audio_priv_data.bsrc.codec_cfg.octets_per_codec_frame);
	bt_stack_le_audio_broadcast_source_set_codec_to_level2(bt_stack_le_audio_media_codec_level2, sizeof(bt_stack_le_audio_media_codec_level2),
														   RTK_BT_LE_CODEC_CFG_TYPE_BLOCKS_PER_SDU, bt_le_audio_priv_data.bsrc.codec_cfg.codec_frame_blocks_per_sdu);
	/* add group */
	for (uint8_t i = 0; i < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_GROUP_NUM; i ++) {
		bt_le_audio_priv_data.bsrc.group[i].presentation_delay = bt_le_audio_priv_data.bsrc.prefer_qos.presentation_delay;
		if (base_data_add_group(&bt_le_audio_priv_data.bsrc.group[i].group_idx,
								bt_le_audio_priv_data.bsrc.group[i].presentation_delay) == false) {
			error_idx = 1;
			bt_le_audio_priv_data.bsrc.group[i].group_idx = 0xFF;
			goto error;
		} else {
			/* add subgroup */
			for (uint8_t j = 0; j < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_SUB_GROUP_NUM; j ++) {
				/* update metadata */
				bt_stack_le_audio_gen_metadata(bt_le_audio_metadata, &bt_le_audio_metadata_len, bt_le_audio_priv_data.bsrc.group[i].sub_group[j].stream_audio_contexts, 0,
											   NULL);
				if (base_data_add_subgroup(&bt_le_audio_priv_data.bsrc.group[i].sub_group[j].subgroup_idx, bt_le_audio_priv_data.bsrc.group[i].group_idx,
										   codec_id2,
										   sizeof(bt_stack_le_audio_media_codec_level2), bt_stack_le_audio_media_codec_level2, bt_le_audio_metadata_len,
										   bt_le_audio_metadata) == false) {
					error_idx = 2;
					bt_le_audio_priv_data.bsrc.group[i].sub_group[j].subgroup_idx = 0xFF;
					goto error;
				} else {
					uint8_t cfg_len = 0;
					uint8_t *level3_cfg = NULL;
					/* add bis */
					for (uint8_t k = 0; k < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_NUM; k ++) {
						if (k == 0) {
							cfg_len = sizeof(bt_stack_le_audio_media_codec_level3_left);
							level3_cfg = (uint8_t *)bt_stack_le_audio_media_codec_level3_left;
						} else if (k == 1) {
							cfg_len = sizeof(bt_stack_le_audio_media_codec_level3_right);
							level3_cfg = (uint8_t *)bt_stack_le_audio_media_codec_level3_right;
						} else {
							BT_LOGE("[LEA STACK] %s: Warning !!!!! bis num is overflow \r\n", __func__);
							error_idx = 3;
							goto error;
						}
						if (base_data_add_bis(&bt_le_audio_priv_data.bsrc.group[i].sub_group[j].bis_idx[k], bt_le_audio_priv_data.bsrc.group[i].group_idx,
											  bt_le_audio_priv_data.bsrc.group[i].sub_group[j].subgroup_idx,
											  cfg_len, level3_cfg) == false) {
							error_idx = 3;
							bt_le_audio_priv_data.bsrc.group[i].sub_group[j].bis_idx[k] = 0xFF;
							goto error;
						}
					}
				}
			}
		}
	}
	/* get bis num */
	for (uint8_t i = 0; i < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_GROUP_NUM; i ++) {
		base_data_get_bis_num(bt_le_audio_priv_data.bsrc.group[i].group_idx, &bt_le_audio_priv_data.bsrc.group[i].group_bis_num);
		BT_LOGA("[LEA STACK] %s: group index: %x, group bis number :%d \r\n", __func__,
				bt_le_audio_priv_data.bsrc.group[i].group_idx, bt_le_audio_priv_data.bsrc.group[i].group_bis_num);
	}

	return 0;
error:
	BT_LOGE("[LEA STACK] %s:Error idx %d \r\n", __func__, error_idx);

	return 1;
}

static void *bt_stack_le_audio_bsrc_get_handle(uint8_t *bd_addr, uint8_t bd_type, uint8_t adv_sid, uint8_t broadcast_id[3])
{
	if (bt_le_audio_priv_data.bsrc.source_handle != NULL &&
		bt_le_audio_priv_data.bsrc.source_adv_sid == adv_sid &&
		memcmp(bt_le_audio_priv_data.bsrc.broadcast_id, broadcast_id, 3) == 0 &&
		bt_le_audio_priv_data.bsrc.cfg_local_addr_type == bd_type &&
		memcmp(bt_le_audio_priv_data.bsrc.source_address, bd_addr, 6) == 0
	   ) {
		return bt_le_audio_priv_data.bsrc.source_handle;
	}
	return NULL;
}

static void bt_stack_le_audio_link_brs_char_add_source_handle(void)
{
	rtk_bt_le_audio_link_t *p_link = NULL;
	T_BASS_BRS_DATA *p_brs_data = NULL;

	for (uint8_t i = 0; i < RTK_BT_LE_AUDIO_MAX_BLE_LINK_NUM; i++) {
		if (bt_le_audio_priv_data.le_link[i].used == true &&
			bt_le_audio_priv_data.le_link[i].brs_char_tbl != NULL) {
			p_link = &bt_le_audio_priv_data.le_link[i];
			for (uint8_t j = 0; j < p_link->brs_char_num; j++) {
				if (p_link->brs_char_tbl[j].brs_is_used &&
					p_link->brs_char_tbl[j].handle == NULL) {
					p_brs_data = bass_get_brs_data(p_link->conn_handle, j);
					if (p_brs_data) {
						p_link->brs_char_tbl[j].handle = bt_stack_le_audio_bsrc_get_handle(p_brs_data->source_address,
																						   p_brs_data->source_address_type,
																						   p_brs_data->source_adv_sid, p_brs_data->broadcast_id);
					}
					p_brs_data = NULL;
				}
			}
		}
	}
}

static uint16_t bt_stack_le_audio_broadcast_init(rtk_bt_le_audio_codec_cfg_item_t codec_index, rtk_bt_le_audio_qos_cfg_type_t qos_type,
												 bool manual_qos_flag, rtk_bt_le_audio_qos_cfg_preferred_t *p_manual_qos_cfg,
												 rtk_bt_le_addr_type_t local_addr_type, bool encryption,
												 uint16_t stream_audio_contexts)
{
	T_BROADCAST_SOURCE_INFO src_info = {0};

	if (bt_le_audio_priv_data.bsrc.source_handle == NULL) {
		BT_LOGE("[LEA STACK] %s: broadcast need to add before initialized \r\n", __func__);
		return 1;
	}
	bt_le_audio_priv_data.bsrc.cfg_codec_index = codec_index;
	for (uint8_t i = 0; i < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_GROUP_NUM; i ++) {
		bt_le_audio_priv_data.bsrc.group[i].group_idx = 0xFF;
		bt_le_audio_priv_data.bsrc.group[i].group_bis_num = RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_NUM;
		for (uint8_t j = 0; j < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_SUB_GROUP_NUM; j ++) {
			bt_le_audio_priv_data.bsrc.group[i].sub_group[j].subgroup_idx = 0xFF;
			bt_le_audio_priv_data.bsrc.group[i].sub_group[j].stream_audio_contexts = stream_audio_contexts;
			for (uint8_t k = 0; k < RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_NUM; k ++) {
				bt_le_audio_priv_data.bsrc.group[i].sub_group[j].bis_idx[k] = 0xFF;
			}
		}
	}
	bt_le_audio_priv_data.bsrc.cfg_qos_type = qos_type;
	bt_le_audio_priv_data.bsrc.cfg_local_addr_type = local_addr_type;
	bt_le_audio_priv_data.bsrc.cfg_encryption = encryption;
	/* get prefer codec */
	if (false == codec_preferred_cfg_get((T_CODEC_CFG_ITEM)bt_le_audio_priv_data.bsrc.cfg_codec_index, (T_CODEC_CFG *)&bt_le_audio_priv_data.bsrc.codec_cfg)) {
		BT_LOGE("%s codec_preferred_cfg_get fail\r\n", __func__);
		goto error;
	}
	if (manual_qos_flag) {
		if (NULL == p_manual_qos_cfg) {
			BT_LOGE("%s: invalid manual qos cfg param\r\n", __func__);
			goto error;
		}
		/* Qos from manual config value*/
		memcpy((void *)&bt_le_audio_priv_data.bsrc.prefer_qos, (void *)p_manual_qos_cfg, sizeof(rtk_bt_le_audio_qos_cfg_preferred_t));
	} else {
		if (false == qos_preferred_cfg_get((T_CODEC_CFG_ITEM)codec_index, (T_QOS_CFG_TYPE)qos_type, (T_QOS_CFG_PREFERRED *)&bt_le_audio_priv_data.bsrc.prefer_qos)) {
			BT_LOGE("%s qos_preferred_cfg_get fail\r\n", __func__);
			goto error;
		}
	}
	if (bt_stack_le_audio_gen_basic_data()) {
		BT_LOGE("%s bt_stack_le_audio_gen_basic_data fail \r\n", __func__);
		goto error;
	}
	if (broadcast_source_get_info(bt_le_audio_priv_data.bsrc.source_handle, &src_info)) {
		bt_le_audio_priv_data.bsrc.source_adv_sid = src_info.adv_sid;
		memcpy(bt_le_audio_priv_data.bsrc.broadcast_id, src_info.broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);
		if (gap_get_param(GAP_PARAM_BD_ADDR, bt_le_audio_priv_data.bsrc.source_address)) {
			BT_LOGE("%s gap_get_param fail \r\n", __func__);
			goto error;
		}
		/* for broadcast assistant */
		bt_stack_le_audio_link_brs_char_add_source_handle();
	}
	return 0;

error:
	bt_stack_le_audio_free_basic_data();
	memset((void *)&bt_le_audio_priv_data.bsrc, 0, sizeof(rtk_bt_le_audio_broadcast_source_cb_t));
	return RTK_BT_ERR_LOWER_STACK_API;
}

static uint16_t bt_stack_le_audio_broadcast_source_create(void *data)
{
	rtk_bt_le_audio_broadcast_source_create_param_t *p_param = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_param = (rtk_bt_le_audio_broadcast_source_create_param_t *)data;
	if (bt_le_audio_priv_data.bsrc.source_handle) {
		BT_LOGE("%s source_handle is not NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	bt_le_audio_priv_data.bsrc.source_handle = broadcast_source_add(bt_stack_le_audio_broadcast_source_cb);
	bt_le_audio_priv_data.bsrc.state = RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_IDLE;
	if (bt_le_audio_priv_data.bsrc.source_handle == NULL) {
		BT_LOGE("%s broadcast_source_add fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (bt_stack_le_audio_broadcast_init(p_param->cfg_codec_index, p_param->cfg_qos_type,
										 p_param->manual_qos_flag, p_param->p_manual_qos_cfg,
										 p_param->local_addr_type, p_param->encryption,
										 p_param->stream_audio_contexts)) {
		BT_LOGE("%s bt_stack_le_audio_broadcast_init fail\r\n", __func__);
		broadcast_source_release(bt_le_audio_priv_data.bsrc.source_handle);
		bt_le_audio_priv_data.bsrc.source_handle = NULL;
		return RTK_BT_FAIL;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_broadcast_source_target_state(rtk_bt_le_audio_bap_act_t event)
{
	if (bt_le_audio_priv_data.bsrc.source_handle == NULL) {
		return RTK_BT_FAIL;
	}

	switch (event) {
	case RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_START: {
		bt_le_audio_priv_data.bsrc.prefer_state = RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING;
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_STOP: {
		bt_le_audio_priv_data.bsrc.prefer_state = RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_IDLE;
		break;
	}
	default:
		BT_LOGE("%s: unknown event type %d \r\n", __func__, event);
		break;
	}

	return bt_stack_le_audio_update_broadcast_state();
}

static uint16_t bt_stack_le_audio_ext_scan_act(void *param)
{
	bool enable = *((bool *)param);
	T_GAP_LE_EXT_SCAN_PARAM extended_scan_param[GAP_EXT_SCAN_MAX_PHYS_NUM];
	uint8_t scan_phys = 0;
	T_GAP_CAUSE cause;
	T_GAP_DEV_STATE dev_state;
	rtk_bt_le_ext_scan_param_t *p_param = &bt_stack_le_audio_def_ext_scan_param;

	cause = le_get_gap_param(GAP_PARAM_DEV_STATE, &dev_state);
	if (cause) {
		BT_LOGE("%s: le_get_gap_param failed \r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	if (enable) {
		if (dev_state.gap_scan_state == GAP_SCAN_STATE_SCANNING) {
			BT_LOGA("%s: has already started \r\n", __func__);
			return RTK_BT_OK;
		}
		// set ext scan parameter
		if (p_param->phys[0]) {
			scan_phys |= GAP_EXT_SCAN_PHYS_1M_BIT;
			extended_scan_param[0].scan_type = (T_GAP_SCAN_MODE)p_param->type[0];
			extended_scan_param[0].scan_interval = p_param->interval[0];
			extended_scan_param[0].scan_window = p_param->window[0];
		}
		if (p_param->phys[1]) {
			scan_phys |= GAP_EXT_SCAN_PHYS_CODED_BIT;
			extended_scan_param[1].scan_type = (T_GAP_SCAN_MODE)p_param->type[1];
			extended_scan_param[1].scan_interval = p_param->interval[1];
			extended_scan_param[1].scan_window = p_param->window[1];
		}

		cause = le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_LOCAL_ADDR_TYPE, sizeof(p_param->own_addr_type), &p_param->own_addr_type);
		if (cause) {
			return RTK_BT_ERR_LOWER_STACK_API;
		}

		cause = le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_FILTER_POLICY, sizeof(p_param->filter_policy), &p_param->filter_policy);
		if (cause) {
			return RTK_BT_ERR_LOWER_STACK_API;
		}

		cause = le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_FILTER_DUPLICATES, sizeof(p_param->duplicate_opt), &p_param->duplicate_opt);
		if (cause) {
			return RTK_BT_ERR_LOWER_STACK_API;
		}

		cause = le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_DURATION, sizeof(p_param->duration), &p_param->duration);
		if (cause) {
			return RTK_BT_ERR_LOWER_STACK_API;
		}

		cause = le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_PERIOD, sizeof(p_param->period), &p_param->period);
		if (cause) {
			return RTK_BT_ERR_LOWER_STACK_API;
		}

		cause = le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_PHYS, sizeof(scan_phys), &scan_phys);
		if (cause) {
			return RTK_BT_ERR_LOWER_STACK_API;
		}

		if (p_param->phys[0]) {
			le_ext_scan_set_phy_param(LE_SCAN_PHY_LE_1M, &extended_scan_param[0]);
		}

		if (p_param->phys[1]) {
			le_ext_scan_set_phy_param(LE_SCAN_PHY_LE_CODED, &extended_scan_param[1]);
		}
		// ext scan start
		cause = le_ext_scan_start();
		if (cause) {
			BT_LOGE("%s: le_ext_scan_start failed cause:0x%x \r\n", __func__, cause);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	} else {
		if (dev_state.gap_scan_state == GAP_SCAN_STATE_IDLE) {
			BT_LOGA("%s: scan has already stopped \r\n", __func__);
			return RTK_BT_OK;
		}
		// ext scan stop
		cause = le_ext_scan_stop();
		if (cause) {
			BT_LOGE("%s: le_ext_scan_stop failed cause:0x%x \r\n", __func__, cause);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_broadcast_pa_sync_create(void *param)
{
	rtk_bt_le_audio_broadcast_sync_create_param_t *p_create_param = (rtk_bt_le_audio_broadcast_sync_create_param_t *)param;
	T_BLE_AUDIO_SYNC_HANDLE sync_handle = NULL;
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	bool enable = true;
	rtk_bt_le_audio_pa_sync_establish_t establish_param = {
		.options = 0,
		.sync_cte_type = 0,
		.skip = 0,
		.sync_timeout = RTK_BT_LE_AUDIO_BROADCAST_SYNC_TIMEOUT //10*N ms
	};

	// ext scan start
	bt_stack_le_audio_ext_scan_act(&enable);

	sync_handle = ble_audio_sync_find(p_create_param->adv_sid, p_create_param->broadcast_id);

	/* create sync when sync_handle is NULL */
	if (sync_handle == NULL) {
		sync_handle = ble_audio_sync_allocate(bt_stack_le_audio_sync_cb,
											  p_create_param->adv_addr.type,
											  p_create_param->adv_addr.addr_val,
											  p_create_param->adv_sid,
											  p_create_param->broadcast_id);
		if (!sync_handle) {
			BT_LOGE("%s: ble_audio_sync_allocate fail\r\n", __func__);
		}
		p_sync_dev_info = bt_stack_le_audio_sync_dev_add((void *)sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("%s: bt_stack_le_audio_sync_dev_add fail\r\n", __func__);
			return RTK_BT_FAIL;
		}
		p_sync_dev_info->adv_sid = p_create_param->adv_sid;
		memcpy(&p_sync_dev_info->adv_addr, &p_create_param->adv_addr, sizeof(rtk_bt_le_addr_t));
		memcpy(p_sync_dev_info->broadcast_id, p_create_param->broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);
	} else {
		p_sync_dev_info = bt_stack_le_audio_sync_dev_find(sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("%s: bt_stack_le_audio_sync_dev_find fail\r\n", __func__);
			return RTK_BT_FAIL;
		}
	}

	if (p_sync_dev_info->pa_sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_TERMINATED) {
		if (false == ble_audio_pa_sync_establish(sync_handle,
												 establish_param.options,
												 establish_param.sync_cte_type,
												 establish_param.skip,
												 establish_param.sync_timeout)) {
			BT_LOGE("%s ble_audio_pa_sync_establish fail\r\n", __func__);
			return RTK_BT_ERR_LOWER_STACK_API;
		}

	} else {
		BT_LOGE("%s: wrong pa sync state: %d \r\n", __func__, p_sync_dev_info->pa_sync_state);
		return RTK_BT_FAIL;
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_broadcast_pa_sync_terminate(void *param)
{
	rtk_bt_le_addr_t *paddr = (rtk_bt_le_addr_t *)param;
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;

	p_sync_dev_info = bt_stack_le_audio_sync_dev_find_by_addr(paddr->addr_val, paddr->type);
	if (!p_sync_dev_info) {
		BT_LOGE("%s: not find device in sync list\r\n", __func__);
		return RTK_BT_FAIL;
	}

	if (!p_sync_dev_info->sync_handle) {
		BT_LOGE("%s: sync_handle is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	}

	if (p_sync_dev_info->pa_sync_state != RTK_BT_LE_AUDIO_PA_SYNC_STATE_TERMINATED) {
		if (false == ble_audio_pa_terminate(p_sync_dev_info->sync_handle)) {
			BT_LOGE("%s: ble_audio_pa_terminate fail\r\n", __func__);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	} else {
		BT_LOGE("%s: pa_sync_state:0x%x, has already terminated \r\n", __func__, p_sync_dev_info->pa_sync_state);
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_broadcast_big_sync_create(void *param)
{
	rtk_bt_le_addr_t *paddr = (rtk_bt_le_addr_t *)param;
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;

	p_sync_dev_info = bt_stack_le_audio_sync_dev_find_by_addr(paddr->addr_val, paddr->type);
	if (!p_sync_dev_info) {
		BT_LOGE("%s: not find device in sync list, please do PA sync before BIG sync\r\n", __func__);
		return RTK_BT_FAIL;
	}
	p_sync_dev_info->big_proc_flags |= RTK_BT_LE_AUDIO_BIG_PROC_BIG_SYNC_REQ;
	if (bt_stack_le_audio_bass_get_supported_bis(p_sync_dev_info)) {
		p_sync_dev_info->big_proc_flags &= ~RTK_BT_LE_AUDIO_BIG_PROC_BIG_SYNC_REQ;
		BT_LOGE("%s: bt_stack_le_audio_bass_get_supported_bis failed\r\n", __func__);
		return RTK_BT_FAIL;
	}

	bt_stack_le_audio_check_sync(p_sync_dev_info);

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_broadcast_big_sync_create_by_handle(void *param)
{
	rtk_bt_le_audio_sync_handle_t *sync_handle = (rtk_bt_le_audio_sync_handle_t *)param;
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;


	p_sync_dev_info = bt_stack_le_audio_sync_dev_find(*sync_handle);
	if (!p_sync_dev_info) {
		BT_LOGE("%s: not find device in sync list, please do PA sync before BIG sync\r\n", __func__);
		return RTK_BT_FAIL;
	}
	p_sync_dev_info->big_proc_flags |= RTK_BT_LE_AUDIO_BIG_PROC_BIG_SYNC_REQ;
	if (bt_stack_le_audio_bass_get_supported_bis(p_sync_dev_info)) {
		p_sync_dev_info->big_proc_flags &= ~RTK_BT_LE_AUDIO_BIG_PROC_BIG_SYNC_REQ;
		BT_LOGE("%s: bt_stack_le_audio_bass_get_supported_bis failed\r\n", __func__);
		return RTK_BT_FAIL;
	}

	bt_stack_le_audio_check_sync(p_sync_dev_info);

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_broadcast_big_sync_terminate(void *param)
{
	rtk_bt_le_addr_t *paddr = (rtk_bt_le_addr_t *)param;
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;

	p_sync_dev_info = bt_stack_le_audio_sync_dev_find_by_addr(paddr->addr_val, paddr->type);
	if (!p_sync_dev_info) {
		BT_LOGE("%s: not find device in sync list\r\n", __func__);
		return RTK_BT_FAIL;
	}

	if (!p_sync_dev_info->sync_handle) {
		BT_LOGE("%s: sync_handle is NULL\r\n",  __func__);
		return RTK_BT_FAIL;
	}

	if (p_sync_dev_info->big_sync_state != BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATED) {
		if (false == ble_audio_big_terminate(p_sync_dev_info->sync_handle)) {
			BT_LOGE("%s: ble_audio_big_terminate fail\r\n", __func__);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	} else {
		BT_LOGE("%s: big_sync_state:0x%x, has already terminated \r\n", __func__, p_sync_dev_info->big_sync_state);
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_broadcast_big_sync_terminate_by_handle(void *param)
{
	rtk_bt_le_audio_sync_handle_t *sync_handle = (rtk_bt_le_audio_sync_handle_t *)param;
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;

	p_sync_dev_info = bt_stack_le_audio_sync_dev_find(*sync_handle);
	if (!p_sync_dev_info) {
		BT_LOGE("%s: not find device in sync list\r\n", __func__);
		return RTK_BT_FAIL;
	}

	if (!p_sync_dev_info->sync_handle) {
		BT_LOGE("%s: sync_handle is NULL\r\n",  __func__);
		return RTK_BT_FAIL;
	}

	if (p_sync_dev_info->big_sync_state != BIG_SYNC_RECEIVER_SYNC_STATE_TERMINATED) {
		if (false == ble_audio_big_terminate(p_sync_dev_info->sync_handle)) {
			BT_LOGE("%s: ble_audio_big_terminate fail\r\n", __func__);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	} else {
		BT_LOGE("%s: big_sync_state:0x%x, has already terminated \r\n", __func__, p_sync_dev_info->big_sync_state);
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_broadcast_assistant_remote_scan_act(void *param)
{
	rtk_bt_le_audio_broadcast_assistant_remote_scan_param_t *p_act_param = (rtk_bt_le_audio_broadcast_assistant_remote_scan_param_t *)param;
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;
	uint8_t device_num  = 0;
	rtk_bt_le_audio_link_t *p_link = NULL;

	T_AUDIO_DEV_INFO *p_dev_tbl = NULL;

	if (!p_act_param) {
		BT_LOGE("%s: fail: param error \r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_group_info = bt_stack_le_audio_find_group_by_idx(p_act_param->group_idx);
	if (!p_group_info) {
		BT_LOGE("%s: no group info (idx 0x%02x) found \r\n", __func__, p_act_param->group_idx);
		return RTK_BT_FAIL;
	}

	if (!p_group_info->group_handle) {
		BT_LOGE("%s: group (idx 0x%02x) handle is NULL \r\n", __func__, p_act_param->group_idx);
		return RTK_BT_FAIL;
	}
	// start or stop ext scan
	bt_stack_le_audio_ext_scan_act(&p_act_param->enable);

	device_num = ble_audio_group_get_dev_num(p_group_info->group_handle);
	if (!device_num) {
		BT_LOGE("%s: group (idx 0x%02x) connected device is 0 \r\n", __func__, p_act_param->group_idx);
		return RTK_BT_FAIL;
	}
	p_dev_tbl = osif_mem_alloc(RAM_TYPE_DATA_ON, device_num * sizeof(T_AUDIO_DEV_INFO));
	if (!p_dev_tbl) {
		BT_LOGE("%s p_dev_tbl osif_mem_alloc fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	memset(p_dev_tbl, 0, device_num * sizeof(T_AUDIO_DEV_INFO));

	if (false == ble_audio_group_get_info(p_group_info->group_handle, &device_num, p_dev_tbl)) {
		BT_LOGE("%s fail: ble_audio_group_get_info fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	for (uint8_t i = 0 ; i < device_num; i++) {
		p_link = bt_stack_le_audio_find_link_by_conn_handle(p_dev_tbl[i].conn_handle);
		if (!p_link) {
			BT_LOGE("%s fail: find no connect link for conn_handle:%d\r\n", __func__, p_dev_tbl[i].conn_handle);
			continue;
		}
		// start or stop remote scan
		if (p_act_param->enable) {
			if (false == bass_cp_remote_scan_start(p_dev_tbl[i].conn_handle, false)) {
				BT_LOGE("%s fail: bass_cp_remote_scan_start conn handle:%d\r\n", __func__, p_dev_tbl[i].conn_handle);
				return RTK_BT_ERR_LOWER_STACK_API;
			}
		} else {
			if (false == bass_cp_remote_scan_stop(p_dev_tbl[i].conn_handle, false)) {
				BT_LOGE("%s fail: bass_cp_remote_scan_stop fail conn handle:%d\r\n", __func__, p_dev_tbl[i].conn_handle);
				return RTK_BT_ERR_LOWER_STACK_API;
			}
		}
	}

	if (p_dev_tbl) {
		osif_mem_free(p_dev_tbl);
	}

	return RTK_BT_OK;
}


static uint16_t bt_stack_le_audio_broadcast_assistant_reception_start(void *param)
{
	bool ret = false;
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;
	rtk_bt_le_audio_link_t *p_link = NULL;
	uint8_t device_num  = 0;
	T_AUDIO_DEV_INFO *p_dev_tbl = NULL;
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	uint8_t pa_sync = BASS_PA_SYNC_NO_PAST;
	char addr_str[32] = {0};
	rtk_bt_le_audio_bass_broadcast_source_data_t *p_brs_char = NULL;
	uint32_t bis_array = 0;

	rtk_bt_le_audio_broadcast_assistant_reception_act_param_t *p_act_param = (rtk_bt_le_audio_broadcast_assistant_reception_act_param_t *)param;

	rtk_bt_le_addr_to_str(&p_act_param->addr, addr_str, sizeof(addr_str));

	if (!p_act_param) {
		BT_LOGE("%s: fail: param error \r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_group_info = bt_stack_le_audio_find_group_by_idx(p_act_param->group_idx);
	if (!p_group_info) {
		BT_LOGE("%s: no group info (idx 0x%02x) found \r\n", __func__, p_act_param->group_idx);
		return RTK_BT_FAIL;
	}

	if (!p_group_info->group_handle) {
		BT_LOGE("%s: group_handle is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}

	p_sync_dev_info = bt_stack_le_audio_sync_dev_find_by_addr(p_act_param->addr.addr_val, p_act_param->addr.type);

	if (!p_sync_dev_info) {
		BT_LOGE("%s: src_adv_addr (%s) not find\r\n", __func__, addr_str);
		return RTK_BT_ERR_PARAM_INVALID;
	} else {
		if (!p_sync_dev_info->sync_handle) {
			BT_LOGE("%s: sync_handle is NULL\r\n", __func__);
			return RTK_BT_ERR_PARAM_INVALID;
		}
		if (p_sync_dev_info->pa_sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZED) {
			pa_sync = BASS_PA_SYNC_PAST;
		}
	}

	device_num = ble_audio_group_get_dev_num(p_group_info->group_handle);
	if (!device_num) {
		BT_LOGE("%s: group (idx 0x%02x) connected device is 0 \r\n", __func__, p_act_param->group_idx);
		return RTK_BT_FAIL;
	}
	p_dev_tbl = osif_mem_alloc(RAM_TYPE_DATA_ON, device_num * sizeof(T_AUDIO_DEV_INFO));
	if (!p_dev_tbl) {
		BT_LOGE("%s p_dev_tbl osif_mem_alloc fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	memset(p_dev_tbl, 0, device_num * sizeof(T_AUDIO_DEV_INFO));

	if (false == ble_audio_group_get_info(p_group_info->group_handle, &device_num, p_dev_tbl)) {
		BT_LOGE("%s fail: ble_audio_group_get_info fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	for (uint8_t i = 0 ; i < device_num; i++) {
		if (p_dev_tbl[i].is_used && p_dev_tbl[i].conn_state == GAP_CONN_STATE_CONNECTED) {
			p_link = bt_stack_le_audio_find_link_by_conn_handle(p_dev_tbl[i].conn_handle);
			if (!p_link) {
				BT_LOGE("%s fail: no connect link for conn_handle:%d\r\n", __func__, p_dev_tbl[i].conn_handle);
				continue;
			}
			if (p_link->lea_disc_flags & RTK_BT_LE_AUDIO_LINK_DISC_BASS_DONE) {
				bis_array = bap_pacs_check_cfg_by_sync_info((T_BLE_AUDIO_SYNC_HANDLE)p_sync_dev_info->sync_handle,
															p_link->conn_handle,
															(BA_PACS_CHECK_FILTER_CODEC | BA_PACS_CHECK_FILTER_ALLOCATION));
				if (bis_array == 0) {
					BT_LOGE("%s fail: conn_handle: %d, bis_array == 0\r\n", __func__, p_link->conn_handle);
					break;
				}
				p_brs_char = bt_stack_le_audio_link_find_brs_char_by_sync_handle(p_link, p_sync_dev_info->sync_handle);
				if (p_brs_char) {
					/* find the sync_handle in brs_char_tbl, means the broadcast source is added, so modify it */
					ret = bass_cp_modify_source_by_sync_info((T_BLE_AUDIO_SYNC_HANDLE)p_sync_dev_info->sync_handle, p_link->conn_handle,
															 p_brs_char->source_id, pa_sync,
															 bis_array, false);
					BT_LOGA("%s: bass modify source for sync_handle %08x %s \r\n", __func__, p_sync_dev_info->sync_handle,
							((true != ret) ? "fail" : "ok"));
				} else {
					/* not find the sync_handle in brs_char_tbl, means the broadcast source has not added, so add it */
					ret = bass_cp_add_source_by_sync_info((T_BLE_AUDIO_SYNC_HANDLE)p_sync_dev_info->sync_handle, p_link->conn_handle,
														  pa_sync, bis_array, false);
					BT_LOGA("%s: bass add source for sync_handle %08x %s \r\n", __func__, p_sync_dev_info->sync_handle,
							((true != ret) ? "fail" : "ok"));
				}
			}
		}
	}

	if (p_dev_tbl) {
		osif_mem_free(p_dev_tbl);
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_broadcast_assistant_reception_stop(void *param)
{
	bool ret = false;
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;
	rtk_bt_le_audio_link_t *p_link = NULL;
	uint8_t device_num  = 0;
	T_AUDIO_DEV_INFO *p_dev_tbl = NULL;
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	char addr_str[32] = {0};
	rtk_bt_le_audio_bass_broadcast_source_data_t *p_brs_char = NULL;
	rtk_bt_le_audio_broadcast_assistant_reception_act_param_t *p_act_param = (rtk_bt_le_audio_broadcast_assistant_reception_act_param_t *)param;

	rtk_bt_le_addr_to_str(&p_act_param->addr, addr_str, sizeof(addr_str));

	if (!p_act_param) {
		BT_LOGE("%s: fail: param error \r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_group_info = bt_stack_le_audio_find_group_by_idx(p_act_param->group_idx);
	if (!p_group_info) {
		BT_LOGE("%s: no group info (idx 0x%02x) found \r\n", __func__, p_act_param->group_idx);
		return RTK_BT_FAIL;
	}

	if (!p_group_info->group_handle) {
		BT_LOGE("%s: group_handle is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}

	p_sync_dev_info = bt_stack_le_audio_sync_dev_find_by_addr(p_act_param->addr.addr_val, p_act_param->addr.type);

	if (!p_sync_dev_info) {
		BT_LOGE("%s: src_adv_addr (%s) not find\r\n", __func__, addr_str);
		return RTK_BT_ERR_PARAM_INVALID;
	} else {
		if (!p_sync_dev_info->sync_handle) {
			BT_LOGE("%s: sync_handle is NULL\r\n", __func__);
			return RTK_BT_ERR_PARAM_INVALID;
		}
	}

	device_num = ble_audio_group_get_dev_num(p_group_info->group_handle);
	if (!device_num) {
		BT_LOGE("%s: group (idx 0x%02x) connected device is 0 \r\n", __func__, p_act_param->group_idx);
		return RTK_BT_FAIL;
	}
	p_dev_tbl = osif_mem_alloc(RAM_TYPE_DATA_ON, device_num * sizeof(T_AUDIO_DEV_INFO));
	if (!p_dev_tbl) {
		BT_LOGE("%s p_dev_tbl osif_mem_alloc fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	memset(p_dev_tbl, 0, device_num * sizeof(T_AUDIO_DEV_INFO));

	if (false == ble_audio_group_get_info(p_group_info->group_handle, &device_num, p_dev_tbl)) {
		BT_LOGE("%s fail: ble_audio_group_get_info fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	for (uint8_t i = 0 ; i < device_num; i++) {
		if (p_dev_tbl[i].is_used && p_dev_tbl[i].conn_state == GAP_CONN_STATE_CONNECTED) {
			p_link = bt_stack_le_audio_find_link_by_conn_handle(p_dev_tbl[i].conn_handle);
			if (!p_link) {
				BT_LOGE("%s fail: no connect link for conn_handle:%d\r\n", __func__, p_dev_tbl[i].conn_handle);
				continue;
			}
			if (p_link->lea_disc_flags & RTK_BT_LE_AUDIO_LINK_DISC_BASS_DONE) {
				p_brs_char = bt_stack_le_audio_link_find_brs_char_by_sync_handle(p_link, p_sync_dev_info->sync_handle);
				if (p_brs_char) {
					/* find the sync_handle in brs_char_tbl,modify it */
					ret = bass_cp_modify_source_by_sync_info((T_BLE_AUDIO_SYNC_HANDLE)p_sync_dev_info->sync_handle, p_link->conn_handle,
															 p_brs_char->source_id, BASS_PA_NOT_SYNC, 0, false);
					BT_LOGA("%s: bass modify source for sync_handle %08x %s \r\n", __func__, p_sync_dev_info->sync_handle,
							((true != ret) ? "fail" : "ok"));
				} else {
					BT_LOGE("%s fail: not find sync_handle %08x in brs_char_tbl for conn_handle=%d \r\n",
							__func__, p_sync_dev_info->sync_handle, p_link->conn_handle);
				}
			}
		}
	}

	if (p_dev_tbl) {
		osif_mem_free(p_dev_tbl);
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_broadcast_assistant_reception_remove(void *param)
{
	bool ret = false;
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;
	rtk_bt_le_audio_link_t *p_link = NULL;
	uint8_t device_num  = 0;
	T_AUDIO_DEV_INFO *p_dev_tbl = NULL;
	rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	char addr_str[32] = {0};
	rtk_bt_le_audio_bass_broadcast_source_data_t *p_brs_char = NULL;
	rtk_bt_le_audio_broadcast_assistant_reception_act_param_t *p_act_param = (rtk_bt_le_audio_broadcast_assistant_reception_act_param_t *)param;

	rtk_bt_le_addr_to_str(&p_act_param->addr, addr_str, sizeof(addr_str));

	if (!p_act_param) {
		BT_LOGE("%s: fail: param error \r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_group_info = bt_stack_le_audio_find_group_by_idx(p_act_param->group_idx);
	if (!p_group_info) {
		BT_LOGE("%s: no group info (idx 0x%02x) found \r\n", __func__, p_act_param->group_idx);
		return RTK_BT_FAIL;
	}

	if (!p_group_info->group_handle) {
		BT_LOGE("%s: group_handle is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}

	p_sync_dev_info = bt_stack_le_audio_sync_dev_find_by_addr(p_act_param->addr.addr_val, p_act_param->addr.type);

	if (!p_sync_dev_info) {
		BT_LOGE("%s: src_adv_addr (%s) not find\r\n", __func__, addr_str);
		return RTK_BT_ERR_PARAM_INVALID;
	} else {
		if (!p_sync_dev_info->sync_handle) {
			BT_LOGE("%s: sync_handle is NULL\r\n", __func__);
			return RTK_BT_ERR_PARAM_INVALID;
		}
	}

	device_num = ble_audio_group_get_dev_num(p_group_info->group_handle);
	if (!device_num) {
		BT_LOGE("%s: group (idx 0x%02x) connected device is 0 \r\n", __func__, p_act_param->group_idx);
		return RTK_BT_FAIL;
	}
	p_dev_tbl = osif_mem_alloc(RAM_TYPE_DATA_ON, device_num * sizeof(T_AUDIO_DEV_INFO));
	if (!p_dev_tbl) {
		BT_LOGE("%s p_dev_tbl osif_mem_alloc fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	memset(p_dev_tbl, 0, device_num * sizeof(T_AUDIO_DEV_INFO));

	if (false == ble_audio_group_get_info(p_group_info->group_handle, &device_num, p_dev_tbl)) {
		BT_LOGE("%s fail: ble_audio_group_get_info fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	for (uint8_t i = 0 ; i < device_num; i++) {
		if (p_dev_tbl[i].is_used && p_dev_tbl[i].conn_state == GAP_CONN_STATE_CONNECTED) {
			p_link = bt_stack_le_audio_find_link_by_conn_handle(p_dev_tbl[i].conn_handle);
			if (!p_link) {
				BT_LOGE("%s fail: no connect link for conn_handle:%d\r\n", __func__, p_dev_tbl[i].conn_handle);
				continue;
			}
			if (p_link->lea_disc_flags & RTK_BT_LE_AUDIO_LINK_DISC_BASS_DONE) {
				p_brs_char = bt_stack_le_audio_link_find_brs_char_by_sync_handle(p_link, p_sync_dev_info->sync_handle);
				if (p_brs_char) {
					/* find the sync_handle in brs_char_tbl,remove it */
					T_BASS_CP_REMOVE_SOURCE remove_source;
					remove_source.source_id = p_brs_char->source_id;
					ret = bass_cp_remove_source(p_link->conn_handle, &remove_source, false);
					BT_LOGA("%s: bass remove source for sync_handle %08x %s \r\n", __func__, p_sync_dev_info->sync_handle,
							((true != ret) ? "fail" : "ok"));
				} else {
					BT_LOGE("%s fail: not find sync_handle %08x in brs_char_tbl for conn_handle=%d \r\n",
							__func__, p_sync_dev_info->sync_handle, p_link->conn_handle);
				}
			}
		}
	}

	if (p_dev_tbl) {
		osif_mem_free(p_dev_tbl);
	}

	return RTK_BT_OK;
}

static void bt_stack_le_audio_group_dev_msg_ind(rtk_bt_le_audio_group_dev_msg_type_t msg_type, T_BLE_AUDIO_GROUP_HANDLE group_handle,
												T_BLE_AUDIO_DEV_HANDLE dev_handle, uint16_t cause)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_audio_group_dev_msg_ind_t *p_ind = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
								RTK_BT_LE_AUDIO_EVT_GROUP_DEV_MSG_IND,
								sizeof(rtk_bt_le_audio_group_dev_msg_ind_t));
	if (!p_evt) {
		BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
		return;
	}
	p_ind = (rtk_bt_le_audio_group_dev_msg_ind_t *)p_evt->data;
	p_ind->cause = cause;
	p_ind->msg_type = msg_type;
	p_ind->group_handle = group_handle;
	p_ind->device_handle = dev_handle;

	/* Send event */
	rtk_bt_evt_indicate(p_evt, NULL);
}

void bt_stack_le_audio_group_cb(T_AUDIO_GROUP_MSG msg, T_BLE_AUDIO_GROUP_HANDLE handle, void *buf)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;

	switch (msg) {
	case AUDIO_GROUP_MSG_DEV_CONN: {
		T_AUDIO_GROUP_MSG_DEV_CONN *p_data = (T_AUDIO_GROUP_MSG_DEV_CONN *)buf;
		APP_PRINT_INFO2("AUDIO_GROUP_MSG_DEV_CONN: group handle 0x%x, dev handle 0x%x",
						handle, p_data->dev_handle);
		BT_LOGD("AUDIO_GROUP_MSG_DEV_CONN: group handle 0x%08x, dev handle 0x%08x",
				handle, p_data->dev_handle);
		bt_stack_le_audio_group_dev_msg_ind(RTK_BT_LE_AUDIO_GROUP_MSG_DEV_CONN, handle, p_data->dev_handle, 0);
	}
	break;

	case AUDIO_GROUP_MSG_DEV_DISCONN: {
		T_AUDIO_GROUP_MSG_DEV_DISCONN *p_data = (T_AUDIO_GROUP_MSG_DEV_DISCONN *)buf;
		APP_PRINT_INFO3("AUDIO_GROUP_MSG_DEV_DISCONN: group handle 0x%x, dev handle 0x%x, cause 0x%x",
						handle, p_data->dev_handle, p_data->cause);
		BT_LOGD("AUDIO_GROUP_MSG_DEV_DISCONN: group handle 0x%08x, dev handle 0x%08x, cause 0x%x\r\n",
				handle, p_data->dev_handle, p_data->cause);
		if (bt_le_audio_priv_data.bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI) {
			p_group_info = bt_stack_le_audio_find_group(handle);
			if (p_group_info == NULL) {
				BT_LOGE("[BAP] %s: p_group_info is NULL\r\n", __func__);
				break;
			}
			if (0 == ble_audio_group_get_used_dev_num(handle, true)) {
				if (p_group_info->lea_unicast.session_handle) {
					bap_unicast_audio_remove_session(p_group_info->lea_unicast.session_handle);
					BT_LOGA("AUDIO_GROUP_MSG_DEV_DISCONN: group handle 0x%08x, remove session_handle 0x%08x \r\n",
							handle, p_group_info->lea_unicast.session_handle);
					break;
				}
			}
		}
		bt_stack_le_audio_group_dev_msg_ind(RTK_BT_LE_AUDIO_GROUP_MSG_DEV_DISCONN, handle, p_data->dev_handle, p_data->cause);
	}
	break;

	case AUDIO_GROUP_MSG_DEV_BOND_REMOVE: {
		T_AUDIO_GROUP_MSG_DEV_BOND_REMOVE *p_data = (T_AUDIO_GROUP_MSG_DEV_BOND_REMOVE *)buf;
		APP_PRINT_INFO2("AUDIO_GROUP_MSG_DEV_BOND_REMOVE: group handle 0x%x, dev handle 0x%x",
						handle, p_data->dev_handle);
		BT_LOGD("AUDIO_GROUP_MSG_DEV_BOND_REMOVE: group handle 0x%08x, dev handle 0x%08x\r\n",
				handle, p_data->dev_handle);
		ble_audio_group_del_dev(handle, &p_data->dev_handle);
		bt_stack_le_audio_group_dev_msg_ind(RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_REMOVE, handle, p_data->dev_handle, 0);
	}
	break;

	case AUDIO_GROUP_MSG_DEV_BOND_CLEAR: {
		APP_PRINT_INFO1("AUDIO_GROUP_MSG_DEV_BOND_CLEAR: group handle 0x%x", handle);
		BT_LOGD("AUDIO_GROUP_MSG_DEV_BOND_CLEAR: group handle 0x%08x\r\n", handle);
		bt_stack_le_audio_remove_group(handle);
		bt_stack_le_audio_group_dev_msg_ind(RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_CLEAR, handle, NULL, 0);
	}
	break;

	case AUDIO_GROUP_MSG_DEV_EMPTY: {
		APP_PRINT_INFO1("AUDIO_GROUP_MSG_DEV_EMPTY: group handle 0x%x", handle);
		BT_LOGD("AUDIO_GROUP_MSG_DEV_EMPTY: group handle 0x%08x\r\n", handle);
		/* remove group list here*/
		bt_stack_le_audio_remove_group(handle);
		bt_stack_le_audio_group_dev_msg_ind(RTK_BT_LE_AUDIO_GROUP_MSG_DEV_EMPTY, handle, NULL, 0);
	}
	break;

	case AUDIO_GROUP_MSG_BAP_STATE: {
		T_AUDIO_GROUP_BAP_STATE *p_data = (T_AUDIO_GROUP_BAP_STATE *)buf;
		APP_PRINT_INFO6("AUDIO_GROUP_MSG_BAP_STATE: group handle 0x%x, session handle 0x%x, curr_action %d, state %d, result %d, cause 0x%x",
						handle, p_data->handle, p_data->curr_action, p_data->state, p_data->result, p_data->cause);
		BT_LOGD("AUDIO_GROUP_MSG_BAP_STATE: group handle %08x, session handle 0x%08x, curr_action %d, state %d, result %d, cause 0x%x\r\n",
				handle, p_data->handle, p_data->curr_action, p_data->state, p_data->result, p_data->cause);
		if (p_data->result == BAP_UNICAST_RESULT_ASE_CP_ERR) {
			APP_PRINT_INFO5("AUDIO_GROUP_MSG_BAP_STATE: cp error, conn_handle 0x%x, opcode 0x%x, ase_id %d, response_code 0x%x, reason 0x%x",
							p_data->addl_info.cp_error.conn_handle, p_data->addl_info.cp_error.opcode,
							p_data->addl_info.cp_error.ase_id, p_data->addl_info.cp_error.response_code, p_data->addl_info.cp_error.reason);
			BT_LOGE("AUDIO_GROUP_MSG_BAP_STATE: cp error, conn_handle 0x%x, opcode 0x%x, ase_id %d, response_code 0x%x, reason 0x%x\r\n",
					p_data->addl_info.cp_error.conn_handle, p_data->addl_info.cp_error.opcode,
					p_data->addl_info.cp_error.ase_id, p_data->addl_info.cp_error.response_code, p_data->addl_info.cp_error.reason);
		}
		p_group_info = bt_stack_le_audio_find_group(handle);
		if (p_group_info == NULL) {
			BT_LOGE("[BAP] %s: p_group_info is NULL\r\n", __func__);
			break;
		}
		if (p_group_info != NULL && p_group_info->lea_unicast.session_handle == p_data->handle) {
			p_group_info->lea_unicast.bap_state = (rtk_bt_le_audio_stream_state_t)p_data->state;
			if ((p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_IDLE ||
				 p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_IDLE_CONFIGURED) &&
				p_group_info->lea_unicast.release_req) {
				bap_unicast_audio_remove_session(p_group_info->lea_unicast.session_handle);
			}
		}
	}
	break;

	case AUDIO_GROUP_MSG_BAP_START_QOS_CFG: {
		T_AUDIO_GROUP_BAP_START_QOS_CFG *p_data = (T_AUDIO_GROUP_BAP_START_QOS_CFG *)buf;
		T_BAP_UNICAST_SESSION_INFO session_info = {0};
		T_AUDIO_ASE_QOS_CFG ase_qos_cfg = {0};
		APP_PRINT_INFO8("AUDIO_GROUP_MSG_BAP_START_QOS_CFG: group handle 0x%x, session handle 0x%x, sink presentation delay(0x%x-0x%x),source presentation delay(0x%x-0x%x), sink latency 0x%x, source latency 0x%x",
						handle, p_data->handle,
						p_data->sink_presentation_delay_min, p_data->sink_presentation_delay_max,
						p_data->source_presentation_delay_min, p_data->source_presentation_delay_max,
						p_data->sink_transport_latency_max, p_data->source_transport_latency_max);
		APP_PRINT_INFO4("sink_preferred_presentation_delay(0x%x-0x%x), source_preferred_presentation_delay(0x%x-0x%x)",
						p_data->sink_preferred_presentation_delay_min, p_data->sink_preferred_presentation_delay_max,
						p_data->source_preferred_presentation_delay_min, p_data->source_preferred_presentation_delay_max);
		BT_LOGD(
			"AUDIO_GROUP_MSG_BAP_START_QOS_CFG: group handle %08x, session handle %08x, sink presentation delay(0x%x-0x%x),source presentation delay(0x%x-0x%x), sink latency 0x%x, source latency 0x%x\r\n",
			handle, p_data->handle,
			(unsigned int)p_data->sink_presentation_delay_min, (unsigned int)p_data->sink_presentation_delay_max,
			(unsigned int)p_data->source_presentation_delay_min, (unsigned int)p_data->source_presentation_delay_max,
			(unsigned int)p_data->sink_transport_latency_max, (unsigned int)p_data->source_transport_latency_max);
		BT_LOGD("sink_preferred_presentation_delay(0x%x-0x%x), source_preferred_presentation_delay(0x%x-0x%x)\r\n",
				(unsigned int)p_data->sink_preferred_presentation_delay_min, (unsigned int)p_data->sink_preferred_presentation_delay_max,
				(unsigned int)p_data->source_preferred_presentation_delay_min, (unsigned int)p_data->source_preferred_presentation_delay_max);
		p_group_info = bt_stack_le_audio_find_group(handle);
		if (p_group_info == NULL) {
			BT_LOGE("[BAP] %s: p_group_info is NULL\r\n", __func__);
			break;
		}
		if (false == bap_unicast_audio_cfg_session_qos(p_data->handle, (T_AUDIO_SESSION_QOS_CFG *)&p_group_info->lea_unicast.session_qos)) {
			BT_LOGE("%s: bap_unicast_audio_cfg_session_qos fail!\r\n", __func__);
			break;
		}
		if (false == bap_unicast_audio_get_session_info(p_data->handle, &session_info)) {
			BT_LOGE("%s: bap_unicast_audio_get_session_info fail!\r\n", __func__);
			break;
		}
		for (int i = 0; i < session_info.dev_num; i++) {
			if (GAP_CONN_STATE_CONNECTED == session_info.dev_info[i].conn_state) {
				for (int j = 0; j < session_info.dev_info[i].ase_num; j++) {
					if (bap_unicast_audio_get_ase_qos(p_data->handle, session_info.dev_info[i].dev_handle, session_info.dev_info[i].ase_info[j].ase_id,
													  &ase_qos_cfg)) {
#if defined(RTK_BT_LE_AUDIO_CIG_ISO_INTERVAL_CONFIG) && (RTK_BT_LE_AUDIO_CIG_ISO_INTERVAL_CONFIG == RTK_BT_ISO_INTERVAL_20_MS)
						ase_qos_cfg.retransmission_number = 5;
#endif
						BT_LOGA("%s: ase qos: phy 0x%x, max_sdu: %d, retransmission_number: %d \r\n", __func__,
								ase_qos_cfg.phy,
								ase_qos_cfg.max_sdu,
								ase_qos_cfg.retransmission_number);
					} else {
						BT_LOGA("%s: bap_unicast_audio_get_ase_qos fail \r\n", __func__);
						BT_LOGA("%s: use default qos config fail ase qos: phy 0x%x, max_sdu %d, retransmission_number %d \r\n", __func__,
								bt_le_audio_bap_ase_qos.phy,
								bt_le_audio_bap_ase_qos.max_sdu,
								bt_le_audio_bap_ase_qos.retransmission_number);
						memcpy((void *)&ase_qos_cfg, (void *)&bt_le_audio_bap_ase_qos, sizeof(T_AUDIO_ASE_QOS_CFG));
						if (session_info.dev_info[i].ase_info[j].direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
							if (session_info.cfg_type == UNICAST_AUDIO_CFG_4 || \
								session_info.cfg_type == UNICAST_AUDIO_CFG_5) {
								ase_qos_cfg.max_sdu *= 2;
							}
						}
					}
					if (false == bap_unicast_audio_cfg_ase_qos(p_data->handle, session_info.dev_info[i].dev_handle,
															   session_info.dev_info[i].ase_info[j].ase_id, (T_AUDIO_ASE_QOS_CFG *)&ase_qos_cfg)) {
						BT_LOGE("%s: bap_unicast_audio_cfg_ase_qos fail!\r\n", __func__);
						break;
					}
				}
			}
		}
	}
	break;

	case AUDIO_GROUP_MSG_BAP_START_METADATA_CFG: {
		T_AUDIO_GROUP_BAP_START_METADATA_CFG *p_data = (T_AUDIO_GROUP_BAP_START_METADATA_CFG *)buf;
		APP_PRINT_INFO5("AUDIO_GROUP_MSG_BAP_START_METADATA_CFG: group handle 0x%x, session handle 0x%x, dev_handle 0x%x, ase_id %d, direction %d",
						handle, p_data->handle, p_data->dev_handle, p_data->ase_id, p_data->direction);
		BT_LOGD("AUDIO_GROUP_MSG_BAP_START_METADATA_CFG: group handle %08x, session handle %08x, dev_handle %08x, ase_id %d, direction %d\r\n",
				handle, p_data->handle, p_data->dev_handle, p_data->ase_id, p_data->direction);
		p_group_info = bt_stack_le_audio_find_group(handle);
		if (p_group_info == NULL) {
			BT_LOGE("[BAP] %s: p_group_info is NULL\r\n", __func__);
			break;
		}
		if (p_group_info->lea_unicast.session_handle == p_data->handle) {
			bt_stack_le_audio_gen_metadata(bt_le_audio_metadata, &bt_le_audio_metadata_len, p_group_info->lea_unicast.contexts_type, p_group_info->lea_unicast.ccid_num,
										   p_group_info->lea_unicast.ccid);
			if (false == bap_unicast_audio_cfg_ase_metadata(p_data->handle, p_data->dev_handle, p_data->ase_id, bt_le_audio_metadata_len, bt_le_audio_metadata)) {
				BT_LOGE("%s: bap_unicast_audio_cfg_ase_metadata fail!\r\n", __func__);
			}
		}
	}
	break;

	case AUDIO_GROUP_MSG_BAP_SESSION_REMOVE: {
		T_AUDIO_GROUP_BAP_SESSION_REMOVE *p_data = (T_AUDIO_GROUP_BAP_SESSION_REMOVE *)buf;
		APP_PRINT_INFO2("AUDIO_GROUP_MSG_BAP_SESSION_REMOVE: group handle 0x%x, session handle 0x%x",
						handle, p_data->handle);
		BT_LOGD("AUDIO_GROUP_MSG_BAP_SESSION_REMOVE: group handle %08x, session handle %08x\r\n",
				handle, p_data->handle);
		p_group_info = bt_stack_le_audio_find_group(handle);
		if (p_group_info == NULL) {
			BT_LOGE("[BAP] %s: p_group_info is NULL\r\n", __func__);
			break;
		}
		if (p_group_info->lea_unicast.session_handle && p_group_info->lea_unicast.session_handle == p_data->handle) {
			p_group_info->lea_unicast.session_handle = NULL;
		} else {
			BT_LOGE("[BAP] %s: unmatch session_handle\r\n", __func__);
			break;
		}
		/* if session stream remove is initiated proactively, retain group. Otherwise release group */
		if (true == p_group_info->lea_unicast.release_req) {
			p_group_info->lea_unicast.release_req = false;
			break;
		}
		for (uint8_t i = 0; i < p_group_info->lea_unicast.dev_num; i++) {
			if (false == ble_audio_group_del_dev(handle, &p_group_info->lea_unicast.dev_tbl[i])) {
				BT_LOGE("AUDIO_GROUP_MSG_BAP_SESSION_REMOVE: ble_audio_group_del_dev failed! \r\n");
				break;
			}
		}
		if (false == ble_audio_group_release(&handle)) {
			BT_LOGE("AUDIO_GROUP_MSG_BAP_SESSION_REMOVE: ble_audio_group_release failed \r\n");
			break;
		}
	}
	break;

	case AUDIO_GROUP_MSG_BAP_CREATE_CIS: {
		T_AUDIO_GROUP_BAP_CREATE_CIS *p_data = (T_AUDIO_GROUP_BAP_CREATE_CIS *)buf;
		APP_PRINT_INFO3("AUDIO_GROUP_MSG_BAP_CREATE_CIS: group handle 0x%x, session handle 0x%x, dev_num 0x%x",
						handle, p_data->handle, p_data->dev_num);
		BT_LOGD("AUDIO_GROUP_MSG_BAP_CREATE_CIS: group handle 0x%08x, session handle 0x%08x, dev_num 0x%x\r\n",
				handle, p_data->handle, p_data->dev_num);
	}
	break;
	case AUDIO_GROUP_MSG_BAP_CIS_DISCONN: {
		T_AUDIO_GROUP_BAP_CIS_DISCONN *p_data = (T_AUDIO_GROUP_BAP_CIS_DISCONN *)buf;
		APP_PRINT_INFO4("AUDIO_GROUP_MSG_BAP_CIS_DISCONN: group handle 0x%x, session handle 0x%x, conn_handle 0x%x, reason 0x%x",
						handle, p_data->handle, p_data->conn_handle, p_data->cause);
		BT_LOGD("AUDIO_GROUP_MSG_BAP_CIS_DISCONN: group handle 0x%08x, session handle 0x%08x, conn_handle 0x%x, reason 0x%x\r\n",
				handle, p_data->handle, p_data->conn_handle, p_data->cause);
	}
	break;

	case AUDIO_GROUP_MSG_BAP_SETUP_DATA_PATH: {
		rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;
		rtk_bt_le_audio_bap_setup_data_path_ind_t param = {0};
		T_AUDIO_GROUP_MSG_BAP_SETUP_DATA_PATH *p_data = (T_AUDIO_GROUP_MSG_BAP_SETUP_DATA_PATH *)buf;
		APP_PRINT_INFO6("AUDIO_GROUP_MSG_BAP_SETUP_DATA_PATH: group handle 0x%x, session handle 0x%x, dev_handle 0x%x, ase_id 0x%x, direction %d, cis_conn_handle 0x%x",
						handle, p_data->handle,
						p_data->dev_handle,
						p_data->ase_id,
						p_data->path_direction,
						p_data->cis_conn_handle);
		BT_LOGD(
			"AUDIO_GROUP_MSG_BAP_SETUP_DATA_PATH: group handle %08x, session handle %08x, dev_handle %08x, ase_id 0x%x, direction %d, cis_conn_handle 0x%x\r\n",
			handle, p_data->handle,
			p_data->dev_handle,
			p_data->ase_id,
			p_data->path_direction,
			p_data->cis_conn_handle);
		param.group_handle = handle;
		param.stream_session_handle = p_data->handle;
		param.device_handle = p_data->dev_handle;
		param.ase_id = p_data->ase_id;
		param.path_direction = p_data->path_direction;
		param.cis_conn_handle = p_data->cis_conn_handle;
		memcpy((void *)&param.codec_parsed_data, (void *)&p_data->codec_parsed_data, sizeof(rtk_bt_le_audio_cfg_codec_t));
		p_iso_chann = bt_stack_le_audio_data_path_add(AUDIO_GROUP_MSG_BAP_SETUP_DATA_PATH, RTK_BT_LE_AUDIO_CIG_ISO_MODE, p_data->path_direction, (void *)&param);
		if (!p_iso_chann) {
			BT_LOGE("[BAP] %s bt_stack_le_audio_data_path_add fail \r\n", __func__);
			break;
		}
		T_ISOCH_INFO info = {0};
		if (true != cig_mgr_get_isoch_info(p_iso_chann->iso_conn_handle, &info)) {
			BT_LOGE("[BAP] %s cig_mgr_get_isoch_info fail (cis_conn_handle = 0x%x)\r\n", __func__, p_iso_chann->iso_conn_handle);
			break;
		} else {
			p_iso_chann->transport_latency_m_to_s = info.transport_latency_m_to_s;
			p_iso_chann->transport_latency_s_to_m = info.transport_latency_s_to_m;
		}
		rtk_bt_le_audio_bap_setup_data_path_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_BAP_SETUP_DATA_PATH_IND,
									sizeof(rtk_bt_le_audio_bap_setup_data_path_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_bap_setup_data_path_ind_t *)p_evt->data;
		p_ind->group_handle = handle;
		p_ind->stream_session_handle = p_data->handle;
		p_ind->device_handle = p_data->dev_handle;
		p_ind->ase_id = p_data->ase_id;
		p_ind->path_direction = p_data->path_direction;
		p_ind->cis_conn_handle = p_data->cis_conn_handle;
		memcpy(&p_ind->codec_parsed_data, &p_data->codec_parsed_data, sizeof(rtk_bt_le_audio_cfg_codec_t));
		p_ind->iso_chann_t.p_iso_chann = (void *)p_iso_chann;
		p_ind->iso_chann_t.iso_conn_handle = p_iso_chann->iso_conn_handle;
		p_ind->iso_chann_t.path_direction = p_data->path_direction;
		p_ind->iso_chann_t.iso_interval = info.iso_interval;
		p_ind->transport_latency_m_to_s = p_iso_chann->transport_latency_m_to_s;
		p_ind->transport_latency_s_to_m = p_iso_chann->transport_latency_s_to_m;
		if (p_data->path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
			rtk_bt_le_audio_group_info_t *p_group_info = bt_stack_le_audio_find_group(handle);
			if (p_group_info) {
				p_ind->dev_num = p_group_info->lea_unicast.dev_num;
			}
		} else {
			p_ind->dev_num = 0;
		}
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case AUDIO_GROUP_MSG_BAP_REMOVE_DATA_PATH: {
		T_AUDIO_GROUP_MSG_BAP_REMOVE_DATA_PATH *p_data = (T_AUDIO_GROUP_MSG_BAP_REMOVE_DATA_PATH *)buf;
		APP_PRINT_INFO7("AUDIO_GROUP_MSG_BAP_REMOVE_DATA_PATH: group handle 0x%x, session handle 0x%x, dev_handle 0x%x, ase_id 0x%x, direction %d, cis_conn_handle 0x%x, cause 0x%x",
						handle, p_data->handle,
						p_data->dev_handle,
						p_data->ase_id,
						p_data->path_direction,
						p_data->cis_conn_handle,
						p_data->cause);
		BT_LOGD(
			"AUDIO_GROUP_MSG_BAP_REMOVE_DATA_PATH: group handle %08x, session handle %08x, dev_handle %08x, ase_id 0x%x, direction %d, cis_conn_handle 0x%x, cause 0x%x\r\n",
			handle, p_data->handle,
			p_data->dev_handle,
			p_data->ase_id,
			p_data->path_direction,
			p_data->cis_conn_handle,
			p_data->cause);
		bt_stack_le_audio_handle_data_path_remove(p_data->cis_conn_handle, p_data->path_direction);
		rtk_bt_le_audio_bap_remove_data_path_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_BAP_REMOVE_DATA_PATH_IND,
									sizeof(rtk_bt_le_audio_bap_remove_data_path_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		}
		p_ind = (rtk_bt_le_audio_bap_remove_data_path_ind_t *)p_evt->data;
		p_ind->group_handle = handle;
		p_ind->stream_session_handle = p_data->handle;
		p_ind->device_handle = p_data->dev_handle;
		p_ind->ase_id = p_data->ase_id;
		p_ind->path_direction = p_data->path_direction;
		p_ind->cis_conn_handle = p_data->cis_conn_handle;
		p_ind->cause = p_data->cause;
		/* Send event */
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	break;

	case AUDIO_GROUP_MSG_BAP_METADATA_UPDATE: {
		T_AUDIO_GROUP_MSG_BAP_METADATA_UPDATE *p_data = (T_AUDIO_GROUP_MSG_BAP_METADATA_UPDATE *)buf;
		APP_PRINT_INFO7("AUDIO_GROUP_MSG_BAP_METADATA_UPDATE: group handle 0x%x, session handle 0x%x, dev_handle 0x%x, ase_id 0x%x, direction %d, metadata_length %d, p_metadata %b",
						handle, p_data->handle,
						p_data->dev_handle,
						p_data->ase_id,
						p_data->direction,
						p_data->metadata_length,
						TRACE_BINARY(p_data->metadata_length, p_data->p_metadata));
		BT_LOGD("AUDIO_GROUP_MSG_BAP_METADATA_UPDATE: group handle %08x, session handle %08x, dev_handle %08x, ase_id 0x%x,direction %d",
				handle, p_data->handle,
				p_data->dev_handle,
				p_data->ase_id,
				p_data->direction);
	}
	break;

	default:
		BT_LOGE("%s: default msg 0x%04x, not process!\r\n", __func__, msg);
		break;
	}
}

static uint16_t bt_stack_le_audio_group_allocate(void *data)
{
	T_BLE_AUDIO_GROUP_HANDLE group_handle = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	group_handle = ble_audio_group_allocate();
	if (group_handle == NULL) {
		BT_LOGE("%s fail: ble_audio_group_allocate fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	BT_LOGD("%s ble_audio_group_allocate ok,group_handle=%08x\r\n", __func__, group_handle);

	if (false == ble_audio_group_reg_cb(group_handle, bt_stack_le_audio_group_cb)) {
		BT_LOGE("%s fail: ble_audio_group_reg_cb fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	*(rtk_bt_le_audio_group_handle_t *)data = group_handle;

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_group_release(void *data)
{
	T_BLE_AUDIO_GROUP_HANDLE group_handle = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	group_handle = *(T_BLE_AUDIO_GROUP_HANDLE *)data;

	BT_LOGD("%s: group_handle=%08x\r\n", __func__, group_handle);

	if (false == ble_audio_group_release(&group_handle)) {
		BT_LOGE("%s fail: ble_audio_group_release fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return RTK_BT_OK;
}

static bool bt_stack_le_audio_ready_to_start(rtk_bt_le_audio_group_info_t *p_group_info, uint16_t sink_context, uint16_t source_context)
{
	uint8_t dev_num = 0, ready_dev_num = 0;
	T_AUDIO_DEV_INFO *p_dev_tbl;

	if (p_group_info->lea_unicast.session_handle) {
		if (p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_RELEASING ||
			p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_STARTING ||
			p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_STREAMING ||
			p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_PARTIAL_STREAMING ||
			p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_STOPPING) {
			BT_LOGE("%s: failed, group_handle 0x%x, invalid bap_state %d \r\n", __func__,
					p_group_info->group_handle, p_group_info->lea_unicast.bap_state);
			return false;
		}
	}
	dev_num = ble_audio_group_get_dev_num(p_group_info->group_handle);
	if (dev_num) {
		BT_LOGA("%s: group device number %d \r\n", __func__, dev_num);
		p_dev_tbl = osif_mem_alloc(RAM_TYPE_DATA_ON, dev_num * sizeof(T_AUDIO_DEV_INFO));
		if (!p_dev_tbl) {
			BT_LOGE("%s: alloc dev tble fail \r\n", __func__);
			return false;
		}
		p_group_info->lea_unicast.dev_num = dev_num;
		if (ble_audio_group_get_info(p_group_info->group_handle, &dev_num, p_dev_tbl)) {
			for (uint8_t i = 0; i < dev_num; i++) {
				p_group_info->lea_unicast.dev_tbl[i] = p_dev_tbl[i].dev_handle;
				if (p_dev_tbl[i].conn_state == GAP_CONN_STATE_CONNECTED) {
					rtk_bt_le_audio_link_t *p_link = bt_stack_le_audio_find_link_by_conn_handle(p_dev_tbl[i].conn_handle);
					if (p_link) {
						if (p_link->lea_srv_found_flags & RTK_BT_LE_AUDIO_LINK_DISC_CSIS_DONE) {
							if ((p_link->lea_disc_flags & RTK_BT_LE_AUDIO_LINK_DISC_CSIS_DONE) == 0) {
								/* wait csis read procedure done */
								continue;
							}
						}
						if (p_link->lea_srv_found_flags & RTK_BT_LE_AUDIO_LINK_DISC_PACS_DONE) {
							T_BAP_PACS_INFO pacs_info;
							if (bap_pacs_get_info(p_link->conn_handle, &pacs_info)) {
								/* Check available context */
								if (sink_context != 0 && (pacs_info.snk_avail_context & sink_context) == 0) {
									BT_LOGE("%s: sink_context 0x%x 0x%x \r\n", __func__, pacs_info.snk_avail_context, sink_context);
									continue;
								}
								if (source_context != 0 && (pacs_info.src_avail_context & source_context) == 0) {
									continue;
								}
								p_group_info->lea_unicast.ready_conn_handle = p_dev_tbl[i].conn_handle;
								ready_dev_num++;
							}
						}
					}
				}
			}
		} else {
			BT_LOGE("%s: ble_audio_group_get_info fail \r\n", __func__);
			return false;
		}
		osif_mem_free((void *)p_dev_tbl);
		if (ready_dev_num) {
			BT_LOGA("%s: ready_dev_num %d \r\n", __func__, ready_dev_num);
			return true;
		}
	}
	BT_LOGE("%s: failed, group_handle 0x%x \r\n", __func__, p_group_info->group_handle);
	return false;
}

static bool bt_stack_le_audio_select_media_prefer_codec(rtk_bt_le_audio_group_info_t *p_group_info, uint16_t sink_context)
{
	uint32_t cfg_mask = 0;
	uint8_t channel_count = 1;
	uint32_t sup_snk_mask = 0;
	T_CODEC_CFG cfg;

	cfg_mask = bap_unicast_get_audio_cfg_mask(p_group_info->lea_unicast.session_handle,
											  RTK_BT_LE_MEDIA_AUDIO_CFG_SUPPORT,
											  p_group_info->lea_unicast.dev_num, p_group_info->lea_unicast.dev_tbl);
	if (cfg_mask) {
		uint8_t idx = 0;
		if ((cfg_mask & RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER) != 0) {
			cfg_mask &= RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER;
		}
		while (cfg_mask) {
			if (cfg_mask & 0x01) {
				p_group_info->lea_unicast.audio_cfg_type = (rtk_bt_le_audio_unicast_cfg_type_t)idx;
				break;
			}
			cfg_mask >>= 1;
			idx++;
		}
		if (p_group_info->lea_unicast.audio_cfg_type == (rtk_bt_le_audio_unicast_cfg_type_t)UNICAST_AUDIO_CFG_4 || \
			p_group_info->lea_unicast.audio_cfg_type == (rtk_bt_le_audio_unicast_cfg_type_t)UNICAST_AUDIO_CFG_5) {
			channel_count = 2;
		}
		sup_snk_mask = bap_pacs_get_lc3_snk_table_msk(p_group_info->lea_unicast.ready_conn_handle,
													  sink_context,
													  channel_count, 1);
		if (!sup_snk_mask) {
			sup_snk_mask = bap_pacs_get_lc3_snk_table_msk(p_group_info->lea_unicast.ready_conn_handle,
														  AUDIO_CONTEXT_UNSPECIFIED,
														  channel_count, 1);
			BT_LOGA("%s: bap pacs get table msk from AUDIO_CONTEXT_UNSPECIFIED sup_snk_mask 0x%x \r\n", __func__, sup_snk_mask);
		} else {
			BT_LOGA("%s: bap pacs get table msk from contex 0x%x sup_snk_mask 0x%x \r\n", __func__, sink_context, sup_snk_mask);
		}
		if (sup_snk_mask != 0) {
			if ((sup_snk_mask & RTK_BT_LE_MEDIA_CODEC_CFG_PREFER) != 0) {
				sup_snk_mask &= RTK_BT_LE_MEDIA_CODEC_CFG_PREFER;
			}
			idx = 0;
			while (sup_snk_mask) {
				if (sup_snk_mask & 0x01) {
					if (codec_preferred_cfg_get((T_CODEC_CFG_ITEM)idx, &cfg)) {
						p_group_info->lea_unicast.codec_cfg_item = (rtk_bt_le_audio_codec_cfg_item_t)idx;
						BT_LOGA("%s: codec_preferred_cfg_get: audio_cfg_type: %d, codec_cfg_item: %d, frame_duration: 0x%02x, sample_frequency: 0x%02x, codec_frame_blocks_per_sdu: %d, octets_per_codec_frame: %d, audio_channel_allocation: 0x%08x\r\n",
								__func__,
								p_group_info->lea_unicast.audio_cfg_type, p_group_info->lea_unicast.codec_cfg_item, cfg.frame_duration, cfg.sample_frequency, cfg.codec_frame_blocks_per_sdu,
								cfg.octets_per_codec_frame, cfg.audio_channel_allocation);
						return true;
					}
				}
				sup_snk_mask >>= 1;
				idx++;
			}
		}
	}
	BT_LOGE("%s: fail \r\n", __func__);

	return false;
}

static uint32_t bt_stack_le_audio_get_channel_location(T_BAP_PACS_INFO *p_info, uint8_t direction, uint8_t channel_num, uint32_t used_loc)
{
	uint32_t temp_audio_loc;
	uint32_t audio_loc = 0;
	uint16_t index = 0;
	uint8_t num = 0;

	if (direction == SERVER_AUDIO_SINK) {
		temp_audio_loc = p_info->snk_audio_loc;
	} else {
		temp_audio_loc = p_info->src_audio_loc;
	}
	temp_audio_loc &= ~used_loc;
	while (temp_audio_loc) {
		if (temp_audio_loc & 0x01) {
			audio_loc |= (1 << index);
			num++;
		}
		temp_audio_loc >>= 1;
		index++;
		if (num >= channel_num) {
			break;
		}
	}
	if (num != channel_num) {
		BT_LOGE("%s: get failed, direction 0x%x, channel_num %d, used_loc 0x%x", __func__,
				direction, channel_num, used_loc);
		return 0;
	}
	return audio_loc;
}

static bool bt_stack_le_audio_config_codec(rtk_bt_le_audio_group_info_t *p_group_info)
{
	T_BAP_UNICAST_SESSION_INFO session_info;
	T_AUDIO_ASE_CODEC_CFG ase_codec_cfg = {0};

	if (!codec_preferred_cfg_get((T_CODEC_CFG_ITEM)p_group_info->lea_unicast.codec_cfg_item, &ase_codec_cfg.codec_cfg)) {
		BT_LOGE("%s: codec_preferred_cfg_get fail \r\n", __func__);
		return false;
	}
	ase_codec_cfg.codec_id[0] = LC3_CODEC_ID;
	ase_codec_cfg.target_latency = (T_ASE_TARGET_LATENCY)p_group_info->lea_unicast.target_latency;
	ase_codec_cfg.target_phy = ASE_TARGET_PHY_2M;
	if (bap_unicast_audio_get_session_info(p_group_info->lea_unicast.session_handle, &session_info)) {
#if 0
		BT_LOGA("%s: session info, state %d, cfg_type %d, conn_dev_num %d, dev_num %d", __func__,
				session_info.state, session_info.cfg_type,
				session_info.conn_dev_num, session_info.dev_num);
		for (uint8_t i = 0; i < session_info.dev_num; i++) {
			BT_LOGA("dev_info[%d]: dev_handle %p, conn_state %d, ase_num %d",
					i, session_info.dev_info[i].dev_handle,
					session_info.dev_info[i].conn_state,
					session_info.dev_info[i].ase_num);
			for (uint8_t j = 0; j < session_info.dev_info[i].ase_num; j++) {
				BT_LOGA("ase_info[%d]: ase_id %d, cfg_idx %d, ase_state 0x%x, direction %d, channel_num %d",
						j, session_info.dev_info[i].ase_info[j].ase_id,
						session_info.dev_info[i].ase_info[j].cfg_idx,
						session_info.dev_info[i].ase_info[j].ase_state,
						session_info.dev_info[i].ase_info[j].direction,
						session_info.dev_info[i].ase_info[j].channel_num);
			}
		}
#endif
		uint32_t sink_channel_allocation;
		uint32_t source_channel_allocation;

		p_group_info->lea_unicast.sink_used_location = 0;
		p_group_info->lea_unicast.source_used_location = 0;
		ase_codec_cfg.codec_cfg.type_exist |= CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST;

		for (uint8_t i = 0; i < session_info.dev_num; i++) {
			T_AUDIO_DEV_INFO dev_info;
			T_BAP_PACS_INFO pacs_info;

			if (!ble_audio_group_get_dev_info(p_group_info->group_handle,
											  session_info.dev_info[i].dev_handle, &dev_info)) {
				continue;
			}
			if (dev_info.conn_state != GAP_CONN_STATE_CONNECTED) {
				continue;
			}
			if (!bap_pacs_get_info(dev_info.conn_handle, &pacs_info)) {
				continue;
			}
			for (uint8_t j = 0; j < session_info.dev_info[i].ase_num; j++) {
				if (session_info.dev_info[i].ase_info[j].direction == SERVER_AUDIO_SINK) {
					sink_channel_allocation = bt_stack_le_audio_get_channel_location(&pacs_info,
																					 session_info.dev_info[i].ase_info[j].direction,
																					 session_info.dev_info[i].ase_info[j].channel_num,
																					 p_group_info->lea_unicast.sink_used_location);
					ase_codec_cfg.codec_cfg.audio_channel_allocation = sink_channel_allocation;
					p_group_info->lea_unicast.sink_used_location |= sink_channel_allocation;
				} else {
					source_channel_allocation = bt_stack_le_audio_get_channel_location(&pacs_info,
																					   session_info.dev_info[i].ase_info[j].direction,
																					   session_info.dev_info[i].ase_info[j].channel_num,
																					   p_group_info->lea_unicast.source_used_location);
					ase_codec_cfg.codec_cfg.audio_channel_allocation = source_channel_allocation;
					p_group_info->lea_unicast.source_used_location |= source_channel_allocation;
				}
				if (!bap_unicast_audio_cfg_ase_codec(p_group_info->lea_unicast.session_handle,
													 session_info.dev_info[i].dev_handle,
													 session_info.dev_info[i].ase_info[j].cfg_idx, &ase_codec_cfg)) {
					BT_LOGE("%s: bap_unicast_audio_cfg_ase_codec fail \r\n", __func__);
					return false;
				}
			}
		}
	} else {
		BT_LOGE("%s: bap_unicast_audio_get_session_info fail \r\n", __func__);
		return false;
	}
	return true;
}

static uint16_t bt_stack_le_audio_client_start(void *data)
{
	rtk_bt_le_audio_unicast_client_start_t *p_config = NULL;
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error \r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_config = (rtk_bt_le_audio_unicast_client_start_t *)data;
	p_group_info = bt_stack_le_audio_find_group_by_idx(p_config->group_idx);
	if (!p_group_info) {
		BT_LOGE("%s no group info (idx 0x%02x) found \r\n", __func__, p_config->group_idx);
		return RTK_BT_FAIL;
	}
	if (!bt_stack_le_audio_ready_to_start(p_group_info, p_config->sink_context, p_config->source_context)) {
		BT_LOGE("%s group idx 0x%02x is not ready \r\n", __func__, p_config->group_idx);
		return RTK_BT_FAIL;
	}
	/* allocate session handle */
	if (!p_group_info->lea_unicast.session_handle) {
		p_group_info->lea_unicast.session_handle = audio_stream_session_allocate(p_group_info->group_handle);
		if (!p_group_info->lea_unicast.session_handle) {
			BT_LOGE("%s: allocate session handle fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
		if (bt_le_audio_priv_data.mcp.mcp_enabled_cccd) {
			p_group_info->lea_unicast.ccid_num = 1;
			p_group_info->lea_unicast.ccid[0] = RTK_BT_LE_AUDIO_GMCS_CCID;
		}
#endif
		p_group_info->lea_unicast.bap_state = (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_IDLE;
		p_group_info->lea_unicast.contexts_type = p_config->sink_context;
		p_group_info->lea_unicast.target_latency = RTK_BT_LE_AUDIO_DEFAULT_UNICAST_ASE_TARGET_LATENCY;
#if defined(RTK_BT_LE_AUDIO_CIG_ISO_INTERVAL_CONFIG) && (RTK_BT_LE_AUDIO_CIG_ISO_INTERVAL_CONFIG == RTK_BT_ISO_INTERVAL_20_MS)
		p_group_info->lea_unicast.target_latency = RTK_BLE_AUDIO_ASCS_ASE_TARGET_HIGHER_RELIABILITY;
#endif
	}
	if (p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_IDLE) {
		if (!bt_stack_le_audio_select_media_prefer_codec(p_group_info, p_config->sink_context)) {
			BT_LOGE("%s: select media prefer codec fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
		if (!bap_unicast_audio_cfg(p_group_info->lea_unicast.session_handle, (T_UNICAST_AUDIO_CFG_TYPE)p_group_info->lea_unicast.audio_cfg_type,
								   p_group_info->lea_unicast.dev_num, p_group_info->lea_unicast.dev_tbl)) {
			BT_LOGE("%s: bap unicast audio cfg fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
		if (!bt_stack_le_audio_config_codec(p_group_info)) {
			BT_LOGE("%s: bt_stack_le_audio_config_codec fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
		if (!bap_unicast_audio_start(p_group_info->lea_unicast.session_handle)) {
			BT_LOGE("%s: bap_unicast_audio_start fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
	} else if (p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_IDLE_CONFIGURED ||
			   p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_CONFIGURED) {
		if (p_group_info->lea_unicast.contexts_type != p_config->sink_context) {
			BT_LOGE("%s: group contexts type 0x%x is not matched with cofig sink cotext type 0x%x \r\n", __func__,
					p_group_info->lea_unicast.contexts_type, p_config->sink_context);
			return RTK_BT_FAIL;
		}
		if (!bap_unicast_audio_start(p_group_info->lea_unicast.session_handle)) {
			BT_LOGE("%s: bap_unicast_audio_start fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
	}

	return RTK_BT_OK;
}

static uint16_t bt_stack_le_audio_client_stop(void *data)
{
	rtk_bt_le_audio_unicast_client_stop_t *p_stop = NULL;
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error \r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_stop = (rtk_bt_le_audio_unicast_client_stop_t *)data;
	p_group_info = bt_stack_le_audio_find_group_by_idx(p_stop->group_idx);
	if (!p_group_info) {
		BT_LOGE("%s no group info (idx 0x%02x) found \r\n", __func__, p_stop->group_idx);
		return RTK_BT_FAIL;
	}
	if (p_group_info->lea_unicast.session_handle) {
		if (p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_STREAMING ||
			p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_PARTIAL_STREAMING ||
			p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_STARTING) {
			if (bap_unicast_audio_stop(p_group_info->lea_unicast.session_handle, p_stop->cis_timeout_ms)) {
				return RTK_BT_OK;
			}
		} else if (p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_STOPPING) {
			return RTK_BT_OK;
		}
	} else {
		BT_LOGE("%s session_handle is NULL (idx 0x%02x handle 0x%x) \r\n", __func__, p_stop->group_idx, p_group_info->group_handle);
	}
	return RTK_BT_FAIL;
}

static uint16_t bt_stack_le_audio_client_release(void *data)
{
	uint8_t group_idx;
	rtk_bt_le_audio_group_info_t *p_group_info = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error \r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	group_idx = *(uint8_t *)data;
	p_group_info = bt_stack_le_audio_find_group_by_idx(group_idx);
	if (!p_group_info) {
		BT_LOGE("%s no group info (idx 0x%02x) found \r\n", __func__, group_idx);
		return RTK_BT_FAIL;
	}
	if (p_group_info->lea_unicast.session_handle) {
		p_group_info->lea_unicast.release_req = true;
		if (p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_CONFIGURED ||
			p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_STARTING ||
			p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_STREAMING ||
			p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_PARTIAL_STREAMING ||
			p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_STOPPING) {
			if (bap_unicast_audio_release(p_group_info->lea_unicast.session_handle)) {
				return RTK_BT_OK;
			}
		} else if (p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_IDLE ||
				   p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_IDLE_CONFIGURED) {
			if (bap_unicast_audio_remove_session(p_group_info->lea_unicast.session_handle)) {
				return RTK_BT_OK;
			}
		} else if (p_group_info->lea_unicast.bap_state == (rtk_bt_le_audio_stream_state_t)AUDIO_STREAM_STATE_RELEASING) {
			return RTK_BT_OK;
		}
	} else {
		BT_LOGE("%s session_handle is NULL (idx 0x%02x handle 0x%x) \r\n", __func__, group_idx, p_group_info->group_handle);
	}
	return RTK_BT_FAIL;
}

static uint16_t bt_stack_le_audio_bap_param_config(void *data)
{
	rtk_bt_le_audio_bap_param_config_t *p_config = NULL;

	if (!data) {
		BT_LOGE("%s fail: param error \r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (stack_bap_init_flag) {
		BT_LOGE("%s fail: bap param config should be invoked before initialization \r\n", __func__);
		return RTK_BT_FAIL;
	}
	p_config = (rtk_bt_le_audio_bap_param_config_t *)data;
	switch (p_config->cfg_type) {
	case RTK_BT_LE_AUDIO_BAP_BROADCAST_SOURCE_CONFIG: {
		memcpy((void *)&bt_le_audio_broadcast_source_config_param, (void *)&p_config->cfg.brs_config, sizeof(rtk_bt_le_audio_broadcast_source_config_t));
		BT_LOGA("%s bt_le_audio_broadcast_source_config_param is configured \r\n", __func__);
		break;
	}
	case RTK_BT_LE_AUDIO_BAP_BROADCAST_CODE_CONFIG: {
		memcpy((void *)bt_stack_le_audio_broadcast_code, (void *)p_config->cfg.broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
		BT_LOGA("%s bt_stack_le_audio_broadcast_code is configured \r\n", __func__);
		break;
	}
	case RTK_BT_LE_AUDIO_BAP_ASE_QOS_CONFIG: {
		memcpy((void *)&bt_le_audio_bap_ase_qos, (void *)&p_config->cfg.bap_ase_qos, sizeof(rtk_bt_le_audio_unicast_ase_qos_t));
		BT_LOGA("%s bt_le_audio_bap_ase_qos is configured \r\n", __func__);
		break;
	}
	default:
		BT_LOGE("%s: unknown cfg_type: 0x%02x \r\n", __func__, (uint8_t)p_config->cfg_type);
		return RTK_BT_FAIL;
		break;
	}

	return RTK_BT_OK;
}

uint16_t bt_stack_bap_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = 0;
	BT_LOGD("bt_stack_bap_act_handle: act = %d \r\n", p_cmd->act);
	switch (p_cmd->act) {
	case RTK_BT_LE_AUDIO_ACT_ISO_DATA_SEND: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_DATA_SEND \r\n");
		ret = bt_stack_le_audio_data_send(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_LINK_ALLOCATE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_LINK_ALLOCATE \r\n");
		ret = bt_stack_le_audio_acl_link_allocate(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_LINK_FREE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_LINK_FREE \r\n");
		ret = bt_stack_le_audio_acl_link_free(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_CREATE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_CREATE \r\n");
		ret = bt_stack_le_audio_broadcast_source_create(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_GROUP_ALLOCATE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_GROUP_ALLOCATE \r\n");
		ret = bt_stack_le_audio_group_allocate(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_GROUP_RELEASE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_GROUP_RELEASE \r\n");
		ret = bt_stack_le_audio_group_release(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_CLIENT_START: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_CLIENT_START \r\n");
		ret = bt_stack_le_audio_client_start(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_CLIENT_STOP: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_CLIENT_STOP \r\n");
		ret = bt_stack_le_audio_client_stop(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_CLIENT_RELEASE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_CLIENT_RELEASE \r\n");
		ret = bt_stack_le_audio_client_release(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_BAP_PARAM_CONFIG: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BAP_PARAM_CONFIG \r\n");
		ret = bt_stack_le_audio_bap_param_config(p_cmd->param);
		break;
	}
	case RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_START: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_START \r\n");
		ret = bt_stack_le_audio_broadcast_source_target_state(RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_START);
		break;
	}

	case RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_STOP: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_STOP \r\n");
		ret = bt_stack_le_audio_broadcast_source_target_state(RTK_BT_LE_AUDIO_ACT_BROADCAST_SOURCE_STOP);
		break;
	}

	case RTK_BT_LE_AUDIO_ACT_EXT_SCAN_ACT: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_EXT_SCAN_ACT \r\n");
		ret = bt_stack_le_audio_ext_scan_act(p_cmd->param);
		break;
	}

	case RTK_BT_LE_AUDIO_ACT_BROADCAST_PA_SYNC_CREATE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BROADCAST_PA_SYNC_CREATE \r\n");
		ret = bt_stack_le_audio_broadcast_pa_sync_create(p_cmd->param);
		break;
	}

	case RTK_BT_LE_AUDIO_ACT_BROADCAST_PA_SYNC_TERMINATE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BROADCAST_PA_SYNC_TERMINATE \r\n");
		ret = bt_stack_le_audio_broadcast_pa_sync_terminate(p_cmd->param);
		break;
	}

	case RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_CREATE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_CREATE \r\n");
		ret = bt_stack_le_audio_broadcast_big_sync_create(p_cmd->param);
		break;
	}

	case RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_CREATE_BY_HANDLE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_CREATE_BY_HANDLE \r\n");
		ret = bt_stack_le_audio_broadcast_big_sync_create_by_handle(p_cmd->param);
		break;
	}

	case RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_TERMINATE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_TERMINATE \r\n");
		ret = bt_stack_le_audio_broadcast_big_sync_terminate(p_cmd->param);
		break;
	}

	case RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_TERMINATE_BY_HANDLE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BROADCAST_BIG_SYNC_TERMINATE_BY_HANDLE \r\n");
		ret = bt_stack_le_audio_broadcast_big_sync_terminate_by_handle(p_cmd->param);
		break;
	}

	case RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_REMOTE_SCAN_ACT: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_REMOTE_SCAN_ACT \r\n");
		ret = bt_stack_le_audio_broadcast_assistant_remote_scan_act(p_cmd->param);
		break;
	}

	case RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_RECEPTION_START: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_RECEPTION_START \r\n");
		ret = bt_stack_le_audio_broadcast_assistant_reception_start(p_cmd->param);
		break;
	}

	case RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_RECEPTION_STOP: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_RECEPTION_STOP \r\n");
		ret = bt_stack_le_audio_broadcast_assistant_reception_stop(p_cmd->param);
		break;
	}

	case RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_RECEPTION_REMOVE: {
		BT_LOGD("RTK_BT_LE_AUDIO_ACT_BROADCAST_ASSISTANT_RECEPTION_REMOVE \r\n");
		ret = bt_stack_le_audio_broadcast_assistant_reception_remove(p_cmd->param);
		break;
	}

	default:
		BT_LOGE("bt_stack_bap_act_handle: unknown act: %d \r\n", p_cmd->act);
		ret = 0;
		break;
	}

	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}

void bt_stack_le_audio_data_direct_callback(uint8_t cb_type, void *p_cb_data)
{
	rtk_bt_evt_t *p_evt = NULL;
	T_BT_DIRECT_CB_DATA *p_data = (T_BT_DIRECT_CB_DATA *)p_cb_data;

	switch (cb_type) {
	case BT_DIRECT_MSG_ISO_DATA_IND: {
		BT_LOGD(
			"BT_DIRECT_MSG_ISO_DATA_IND, conn_handle 0x%x, pkt_status_flag 0x%x, pkt_seq_num 0x%x, ts_flag 0x%x, time_stamp 0x%x,iso_sdu_len 0x%x, p_buf %08x, offset %d\r\n",
			p_data->p_bt_direct_iso->conn_handle, p_data->p_bt_direct_iso->pkt_status_flag,
			p_data->p_bt_direct_iso->pkt_seq_num, p_data->p_bt_direct_iso->ts_flag,
			(unsigned int)p_data->p_bt_direct_iso->time_stamp,
			p_data->p_bt_direct_iso->iso_sdu_len, p_data->p_bt_direct_iso->p_buf, p_data->p_bt_direct_iso->offset);
		if (p_data->p_bt_direct_iso->iso_sdu_len) {
			BT_DUMPD("", p_data->p_bt_direct_iso->p_buf + p_data->p_bt_direct_iso->offset, p_data->p_bt_direct_iso->iso_sdu_len);
			BT_LOGD("%s pkt_seq_num=%d\r\n", __func__, p_data->p_bt_direct_iso->pkt_seq_num);
		}
		/* Send event */
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_BAP,
									RTK_BT_LE_AUDIO_EVT_ISO_DATA_RECEIVE_IND,
									sizeof(rtk_bt_le_audio_direct_iso_data_ind_t));
		if (!p_evt) {
			BT_LOGE("%s rtk_bt_event_create fail\r\n", __func__);
			break;
		} else {
			rtk_bt_le_audio_iso_channel_info_t *p_iso_chann = NULL;
			p_iso_chann = bt_stack_le_audio_find_iso_chann(p_data->p_bt_direct_iso->conn_handle, RTK_BLE_AUDIO_ISO_DATA_PATH_RX);
			if (!p_iso_chann) {
				BT_LOGE("[BAP] %s BT_DIRECT_MSG_ISO_DATA_IND cannot find matched iso channel \r\n", __func__);
				break;
			} else {
				/* hold iso channel */
				bt_stack_le_audio_hold_iso_chann(p_iso_chann);
			}
			rtk_bt_le_audio_direct_iso_data_ind_t *direct_iso_data_ind = (rtk_bt_le_audio_direct_iso_data_ind_t *)p_evt->data;
			//the definition of rtk_bt_le_iso_direct_iso_data_ind_t and T_BT_DIRECT_ISO_DATA_IND are different,cannot use memcpy
			//memcpy((void *)direct_iso_data_ind, (void *)p_data->p_bt_direct_iso, sizeof(rtk_bt_le_iso_direct_iso_data_ind_t));
			direct_iso_data_ind->iso_conn_handle = p_data->p_bt_direct_iso->conn_handle;
			direct_iso_data_ind->pkt_status_flag = (rtk_bt_le_audio_isoch_data_pkt_status_t)p_data->p_bt_direct_iso->pkt_status_flag;
			direct_iso_data_ind->offset = p_data->p_bt_direct_iso->offset;
			direct_iso_data_ind->iso_sdu_len = p_data->p_bt_direct_iso->iso_sdu_len;
			direct_iso_data_ind->pkt_seq_num = p_data->p_bt_direct_iso->pkt_seq_num;
			direct_iso_data_ind->ts_flag = p_data->p_bt_direct_iso->ts_flag;
			direct_iso_data_ind->time_stamp = p_data->p_bt_direct_iso->time_stamp;
			direct_iso_data_ind->buf_len = p_data->p_bt_direct_iso->offset + p_data->p_bt_direct_iso->iso_sdu_len;
			if (direct_iso_data_ind->iso_sdu_len) {
				direct_iso_data_ind->p_buf = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, direct_iso_data_ind->buf_len);
				if (!direct_iso_data_ind->p_buf) {
					BT_LOGE("direct_iso_data_ind->p_buf alloc fail, len = %d\r\n", direct_iso_data_ind->buf_len);
					bt_stack_le_audio_release_iso_chann(p_iso_chann);
					rtk_bt_event_free(p_evt);
					break;
				}
				memset(direct_iso_data_ind->p_buf, 0, direct_iso_data_ind->buf_len);
				memcpy((void *)direct_iso_data_ind->p_buf, (void *)p_data->p_bt_direct_iso->p_buf, direct_iso_data_ind->buf_len);
			} else {
				direct_iso_data_ind->p_buf = NULL;
			}
			/*  user_data point to the memory alloced for 2nd level ptr, so it's convenient
			    to free it when free p_evt */
			p_evt->user_data = direct_iso_data_ind->p_buf;
			gap_iso_data_cfm(p_data->p_bt_direct_iso->p_buf);
			rtk_bt_evt_indicate(p_evt, NULL);
			bt_stack_le_audio_release_iso_chann(p_iso_chann);
		}
	}
	break;

	default:
		BT_LOGE("%s unhandled cb_type 0x%x", __func__, cb_type);
		break;
	}
}

uint16_t bt_stack_bap_init(void *p_conf)
{
	rtk_bt_le_audio_app_conf_t *p_le_audio_app_conf = NULL;
	T_BAP_ROLE_INFO role_info = {0};

	BT_LOGA("[BAP] bt_stack_bap_init\n");
	if (!p_conf) {
		BT_LOGE("[BAP] p_conf is NULL \n");
		return RTK_BT_FAIL;
	} else {
		p_le_audio_app_conf = (rtk_bt_le_audio_app_conf_t *)p_conf;
	}
	ble_audio_cback_register(rtk_stack_le_audio_bap_msg_cback);
	if (p_le_audio_app_conf->bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI) {
		role_info.role_mask |= BAP_UNICAST_CLT_SRC_ROLE;
		role_info.role_mask |= BAP_UNICAST_CLT_SNK_ROLE;
		role_info.isoc_cig_max_num = RTK_BT_LE_AUDIO_CIG_MAX_NUM;
		role_info.isoc_cis_max_num = RTK_BT_LE_AUDIO_CIS_MAX_NUM;
	}
	if (p_le_audio_app_conf->bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER) {
		role_info.role_mask |= BAP_UNICAST_SRV_SRC_ROLE;
		role_info.role_mask |= BAP_UNICAST_SRV_SNK_ROLE;
		role_info.isoc_cig_max_num = RTK_BT_LE_AUDIO_CIG_MAX_NUM;
		role_info.isoc_cis_max_num = RTK_BT_LE_AUDIO_CIS_MAX_NUM;
		if (p_le_audio_app_conf->ascs_param.sink_ase_num) {
			role_info.snk_ase_num = p_le_audio_app_conf->ascs_param.sink_ase_num;
			default_sink_ase_num = p_le_audio_app_conf->ascs_param.sink_ase_num;
		} else {
			role_info.snk_ase_num = default_sink_ase_num;
		}
		if (p_le_audio_app_conf->ascs_param.source_ase_num) {
			role_info.src_ase_num = p_le_audio_app_conf->ascs_param.source_ase_num;
			default_source_ase_num = p_le_audio_app_conf->ascs_param.source_ase_num;
		} else {
			role_info.src_ase_num = default_source_ase_num;
		}
		BT_LOGD("[BAP]app_bap_init role_info snk_ase_num=%d, src_ase_num=%d\r\n", role_info.snk_ase_num, role_info.src_ase_num);
	}
	if (p_le_audio_app_conf->bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SOUR) {
		role_info.role_mask |= BAP_BROADCAST_SOURCE_ROLE;
		role_info.pa_adv_num = RTK_BT_LE_AUDIO_PA_ADV_SET_MAX_NUM;
		role_info.isoc_big_broadcaster_num = RTK_BT_LE_AUDIO_BROADCASTER_GROUP_MAX_NUM;
		role_info.isoc_bis_broadcaster_num = RTK_BT_LE_AUDIO_BROADCASTER_BIS_MAX_NUM;
	}
	if (p_le_audio_app_conf->bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) {
		role_info.role_mask |= BAP_BROADCAST_SINK_ROLE;
		role_info.pa_sync_num = RTK_BT_LE_AUDIO_SYNC_HANDLE_MAX_NUM;
		role_info.isoc_big_receiver_num = RTK_BT_LE_AUDIO_RECEIVER_BIG_MAX_NUM;
		role_info.isoc_bis_receiver_num = RTK_BT_LE_AUDIO_RECEIVER_BIS_MAX_NUM;
	}
	if (p_le_audio_app_conf->bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI) {
		role_info.role_mask |= BAP_BROADCAST_ASSISTANT_ROLE;
		role_info.pa_sync_num = RTK_BT_LE_AUDIO_SYNC_HANDLE_MAX_NUM;
	}
	if (p_le_audio_app_conf->bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_SCAN_DELE) {
		role_info.role_mask |= BAP_SCAN_DELEGATOR_ROLE;
		role_info.pa_sync_num = RTK_BT_LE_AUDIO_SYNC_HANDLE_MAX_NUM;
		role_info.brs_num = RTK_BT_LE_AUDIO_RECEIVER_BIG_MAX_NUM;
	}
	role_info.init_gap = true;
	if (bap_role_init(&role_info) == false) {
		BT_LOGE("%s:bap_role_init fail\r\n", __func__);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	bt_le_audio_priv_data.lea_sink_available_contexts = p_le_audio_app_conf->pacs_param.sink_available_contexts;
	bt_le_audio_priv_data.lea_source_available_contexts = p_le_audio_app_conf->pacs_param.source_available_contexts;
	bt_le_audio_priv_data.lea_sink_supported_contexts = p_le_audio_app_conf->pacs_param.sink_supported_contexts;
	bt_le_audio_priv_data.lea_source_supported_contexts = p_le_audio_app_conf->pacs_param.source_supported_contexts;
	if (RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR & p_le_audio_app_conf->cap_role) {
		T_PACS_PARAMS pacs_params = {0};
		rtk_bt_le_audio_pacs_init_param_t *p_pacs = &p_le_audio_app_conf->pacs_param;

		pacs_params.sink_locations.is_exist = true;
		pacs_params.sink_locations.is_notify = true;
		pacs_params.sink_locations.is_write = false;
		pacs_params.sink_locations.sink_audio_location = p_pacs->sink_audio_location;
		pacs_params.source_locations.is_exist = true;
		pacs_params.source_locations.is_notify = true;
		pacs_params.source_locations.is_write = false;
		pacs_params.source_locations.source_audio_location = p_pacs->source_audio_location;
		pacs_params.supported_contexts.is_notify = true;
		pacs_params.supported_contexts.sink_supported_contexts = p_pacs->sink_supported_contexts;
		pacs_params.supported_contexts.source_supported_contexts = p_pacs->source_supported_contexts;
		*p_pacs->p_sink_pac_id = pacs_pac_add(SERVER_AUDIO_SINK, p_pacs->p_pac_sink_codec,
											  p_pacs->pac_sink_codec_len,
											  true);
		*p_pacs->p_source_pac_id = pacs_pac_add(SERVER_AUDIO_SOURCE, p_pacs->p_pac_source_codec,
												p_pacs->pac_source_codec_len,
												true);
		BT_LOGA("[BAP]SINK PAC ID 0x%02x, SOURCE PAC ID 0x%02x \n", *p_pacs->p_sink_pac_id, *p_pacs->p_source_pac_id);

		if (pacs_init(&pacs_params) == false) {
			BT_LOGE("%s pacs_init fail\r\n", __func__);
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	}
	gap_register_direct_cb(bt_stack_le_audio_data_direct_callback);
	stack_bap_init_flag = 1;

	return RTK_BT_OK;
}

void bt_stack_bap_deinit(void)
{
	BT_LOGA("[BAP]bt_stack_bap_deinit\n");
	stack_bap_init_flag = 0;
	bt_le_audio_priv_data.bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_UNKNOWN;
}

#endif