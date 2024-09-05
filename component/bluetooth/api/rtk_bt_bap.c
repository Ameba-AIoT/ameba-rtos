/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <rtk_bt_common.h>
#include <rtk_bt_def.h>
#include <rtk_bt_device.h>
#include <app_bt_le_audio_common.h>
#include <rtk_bt_le_audio.h>
#include <rtk_bt_bap.h>

#if defined(CONFIG_BT_BAP_SUPPORT) && CONFIG_BT_BAP_SUPPORT
/* broadcast sink param */
extern app_bt_le_audio_broadcast_sink_info_t g_bro_sink_info;
/* unicast server param */
extern app_bt_le_audio_unicast_server_info_t g_uni_ser_info;
#endif

/* common function */
uint16_t rtk_bt_bap_ext_scan(bool enable)
{
	if (enable) {
		if (rtk_bt_le_gap_set_ext_scan_param(&app_lea_def_ext_scan_param)) {
			BT_LOGE("%s rtk_bt_le_gap_set_ext_scan_param fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
		if (rtk_bt_le_gap_start_ext_scan()) {
			BT_LOGE("%s rtk_bt_le_gap_set_ext_scan_param fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
	} else {
		rtk_bt_le_gap_stop_ext_scan();
		app_bt_le_audio_bass_scan_dev_list_remove_all();
	}

	return RTK_BT_OK;
}

uint16_t rtk_bt_bap_bass_scan(bool enable, bool remote_scan_enable, uint8_t group_idx)
{
	uint16_t ret = RTK_BT_OK;
	uint8_t i = 0, device_num = 0, opcode = 0;
	uint32_t dev_tbl_size = 0;
	rtk_bt_le_audio_group_device_info_t *p_dev_tbl = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	rtk_bt_le_audio_group_handle_t group_handle = NULL;

	if (remote_scan_enable) {
		group_handle = app_bt_le_audio_group_list_find_by_group_idx(group_idx);
		if (group_handle == NULL) {
			BT_LOGE("%s group_idx (%d) not find \r\n", __func__, group_idx);
			return RTK_BT_FAIL;
		}
	} else {
		BT_LOGA("%s no remote scan \r\n", __func__);
	}
	if (enable) {
		if (rtk_bt_le_gap_set_ext_scan_param(&app_lea_def_ext_scan_param)) {
			BT_LOGE("%s rtk_bt_le_gap_set_ext_scan_param fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
		rtk_bt_le_gap_start_ext_scan();
		opcode = RTK_BT_LE_AUDIO_BASS_CP_REMOTE_SCAN_START;
		if (!group_handle) {
			return RTK_BT_OK;
		}
	} else {
		rtk_bt_le_gap_stop_ext_scan();
		opcode = RTK_BT_LE_AUDIO_BASS_CP_REMOTE_SCAN_STOP;
		if (!group_handle) {
			return RTK_BT_OK;
		}
	}
	/* send remote_scan start or stop when group_handle is not NULL */
	if (rtk_bt_le_audio_group_get_used_device_num(group_handle, true, &device_num)) {
		BT_LOGE("%s rtk_bt_le_audio_group_get_used_device_num fail \r\n", __func__);
	} else {
		BT_LOGA("%s device_num=(%d) \r\n", __func__, device_num);
	}
	dev_tbl_size = device_num * sizeof(rtk_bt_le_audio_group_device_info_t);
	p_dev_tbl = (rtk_bt_le_audio_group_device_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, dev_tbl_size);
	if (!p_dev_tbl) {
		BT_LOGE("%s p_dev_tbl osif_mem_alloc fail \r\n", __func__);
		return RTK_BT_FAIL;
	} else {
		memset(p_dev_tbl, 0, dev_tbl_size);
	}
	for (i = 0; i < device_num; i++) {
		ret = rtk_bt_le_audio_group_get_all_device_info(group_handle, &device_num, p_dev_tbl);
		if (ret != RTK_BT_OK) {
			BT_LOGE("%s get_all_device_info fail for group_handle = %08x \r\n", __func__, group_handle);
			continue;
		}
		if (p_dev_tbl[i].is_used && p_dev_tbl[i].conn_state == RTK_BT_LE_AUDIO_GAP_CONN_STATE_CONNECTED) {
			p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(p_dev_tbl[i].conn_handle);
			if (!p_device_info) {
				BT_LOGE("%s group_list_find_dev fail for conn_handle = %d \r\n", __func__, p_dev_tbl[i].conn_handle);
				continue;
			}
			if (p_device_info->bass_is_found) {
				ret = rtk_bt_le_audio_bass_client_write_cp_op(p_dev_tbl[i].conn_handle, opcode, NULL, false);
				BT_LOGE("%s bass remote scan %s %08x %s, ret: 0x%x\r\n", __func__, ((enable) ? "start" : "stop"), group_handle,
						((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
			}
		}
	}
	if (p_dev_tbl) {
		osif_mem_free(p_dev_tbl);
	}

	return RTK_BT_OK;
}

/* bap broadcast source */
uint16_t rtk_bt_bap_broadcast_source_start(void)
{
	rtk_bt_le_audio_cfg_codec_t codec_cfg = {0};
	rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
	rtk_bt_le_audio_qos_cfg_preferred_t qos_cfg_preferred = {0};
	app_bt_le_audio_bap_broadcast_source_info_t *p_bap_bro_sour_info = &app_bap_bro_sour_info;
	rtk_bt_le_audio_qos_cfg_preferred_t *p_prefer_qos = &bap_broadcast_source_cfg.prefer_qos;

	/* get prefer codec */
	if (rtk_bt_le_audio_get_prefer_codec_cfg(bap_broadcast_source_cfg.cfg_codec_index, &codec_cfg)) {
		BT_LOGE("%s rtk_bt_le_audio_get_prefer_codec_cfg fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	BT_LOGA("%s prefer_codec_cfg: type_exist: 0x%x frame_duration: 0x%x sample_frequency: 0x%x codec_frame_blocks_per_sdu: %d  octets_per_codec_frame: %d audio_channel_allocation: 0x%d\r\n",
			__func__,
			codec_cfg.type_exist,
			codec_cfg.frame_duration,
			codec_cfg.sample_frequency,
			codec_cfg.codec_frame_blocks_per_sdu,
			codec_cfg.octets_per_codec_frame, codec_cfg.audio_channel_allocation);
	/* update broadcast source codec_level2 */
	app_bt_le_audio_broadcast_source_set_codec_to_level2(bap_broadcast_source_codec_level2, bap_broadcast_source_codec_level2_len,
														 RTK_BT_LE_CODEC_CFG_TYPE_SAMPLING_FREQUENCY, codec_cfg.sample_frequency);
	app_bt_le_audio_broadcast_source_set_codec_to_level2(bap_broadcast_source_codec_level2, bap_broadcast_source_codec_level2_len,
														 RTK_BT_LE_CODEC_CFG_TYPE_FRAME_DURATION, codec_cfg.frame_duration);
	app_bt_le_audio_broadcast_source_set_codec_to_level2(bap_broadcast_source_codec_level2, bap_broadcast_source_codec_level2_len,
														 RTK_BT_LE_CODEC_CFG_TYPE_OCTET_PER_CODEC_FRAME, codec_cfg.octets_per_codec_frame);

	if (bap_broadcast_source_cfg.qos_manual_cfg == true) {
		/* Manual config QoS parameter*/
		qos_cfg_preferred.presentation_delay = p_prefer_qos->presentation_delay;
		qos_cfg_preferred.sdu_interval = p_prefer_qos->sdu_interval;
		qos_cfg_preferred.retransmission_number = p_prefer_qos->retransmission_number;
		qos_cfg_preferred.max_sdu = p_prefer_qos->max_sdu;
		qos_cfg_preferred.max_transport_latency = p_prefer_qos->max_transport_latency;
		qos_cfg_preferred.framing = p_prefer_qos->framing;
		BT_LOGA("%s: Manul config QoS\r\n", __func__);
	} else {
		/* get prefer QoS according to codec index */
		if (rtk_bt_le_audio_qos_preferred_cfg_get(bap_broadcast_source_cfg.cfg_codec_index, bap_broadcast_source_cfg.cfg_qos_type, &qos_cfg_preferred)) {
			BT_LOGE("%s rtk_bt_le_audio_qos_preferred_cfg_get fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
	}
#if LEA_BIG_ISO_INTERVAL_CONFIG == ISO_INTERVAL_20_MS
#if (RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM == 1) && (RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE == 0) && \
    (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K) && (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_10_MS)
	qos_cfg_preferred.max_transport_latency = 60;
#endif
#elif LEA_BIG_ISO_INTERVAL_CONFIG == ISO_INTERVAL_30_MS
#if (RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM == 1) && (RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE == 1) && \
    (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K) && (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_10_MS)
	qos_cfg_preferred.retransmission_number = 3;
	qos_cfg_preferred.max_transport_latency = 80;
#else
#error "Please change LEA_SOURCE_FIX_SAMPLE_FREQUENCY to RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K when enable LEA_BIG_ISO_INTERVAL_CONFIG 30 ms"
#endif
#endif
	/* update def_le_audio_broadcast_source_group_param */
	def_le_audio_broadcast_source_group_param.presentation_delay = qos_cfg_preferred.presentation_delay;
	/* update def_le_audio_broadcast_source_create_big_param */
	def_le_audio_broadcast_source_create_big_param.sdu_interval = qos_cfg_preferred.sdu_interval;
	def_le_audio_broadcast_source_create_big_param.rtn = qos_cfg_preferred.retransmission_number;
#if (RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM == 1) && (RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE == 1)
	/* config stereo mode in one bis, need to adjust max sdu size */
	def_le_audio_broadcast_source_create_big_param.max_sdu = 2 * qos_cfg_preferred.max_sdu;
#else
	def_le_audio_broadcast_source_create_big_param.max_sdu = qos_cfg_preferred.max_sdu;
#endif
	def_le_audio_broadcast_source_create_big_param.max_transport_latency = qos_cfg_preferred.max_transport_latency;
	def_le_audio_broadcast_source_create_big_param.framing = qos_cfg_preferred.framing;
	BT_LOGA("%s: presentation_delay:%d sdu_interval: %d rtn: %d max_sdu: %d max_transport_latency: %d framing: %d\r\n", __func__,
			def_le_audio_broadcast_source_group_param.presentation_delay,
			def_le_audio_broadcast_source_create_big_param.sdu_interval,
			def_le_audio_broadcast_source_create_big_param.rtn,
			def_le_audio_broadcast_source_create_big_param.max_sdu,
			def_le_audio_broadcast_source_create_big_param.max_transport_latency,
			def_le_audio_broadcast_source_create_big_param.framing);
	if (bap_broadcast_source_cfg.cfg_encryption) {
		def_le_audio_broadcast_source_create_big_param.encryption = bap_broadcast_source_cfg.cfg_encryption;
		memcpy(def_le_audio_broadcast_source_create_big_param.broadcast_code, app_lea_def_broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
	} else {
		def_le_audio_broadcast_source_create_big_param.encryption = bap_broadcast_source_cfg.cfg_encryption;
	}
	/* generate base data */
	memset(&p_bap_bro_sour_info->bis_info, 0, sizeof(rtk_bt_le_audio_bis_info_t));
	p_bap_bro_sour_info->bis_info.num_bis = bap_broadcast_source_cfg.cfg_bis_num;
	def_le_audio_broadcast_source_group_param.group_idx = &p_bap_bro_sour_info->group_idx;
	def_le_audio_broadcast_source_group_param.subgroup[0].num_bis = bap_broadcast_source_cfg.cfg_bis_num;
	def_le_audio_broadcast_source_group_param.subgroup[0].subgroup_idx = &p_bap_bro_sour_info->subgroup_idx;
	def_le_audio_broadcast_source_group_param.subgroup[0].bis[0].bis_idx = &p_bap_bro_sour_info->bis_info.bis_conn_info[0].bis_idx;
#if (RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM == 2)
	def_le_audio_broadcast_source_group_param.subgroup[0].bis[1].bis_idx = &p_bap_bro_sour_info->bis_info.bis_conn_info[1].bis_idx;
#endif
	if (rtk_bt_le_audio_broadcast_source_create(1, &def_le_audio_broadcast_source_group_param, &p_bap_bro_sour_info->broadcast_source_handle)) {
		BT_LOGE("%s rtk_bt_le_audio_broadcast_source_create fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (rtk_bt_le_gap_get_bd_addr(&bd_addr)) {
		BT_LOGE("%s rtk_bt_le_gap_get_bd_addr fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	memcpy(&def_le_audio_broadcast_source_config_param.own_addr, &bd_addr, sizeof(rtk_bt_le_addr_t));
	if (rtk_bt_le_audio_broadcast_source_config(p_bap_bro_sour_info->broadcast_source_handle,
												RTK_BLE_AUDIO_DEFAULT_BROADCAST_SOURCE_ADV_SID,
												p_bap_bro_sour_info->group_idx,
												app_lea_def_broadcast_audio_announcements,
												app_lea_def_broadcast_audio_announcements_len,
												&def_le_audio_broadcast_source_config_param)) {
		BT_LOGE("%s rtk_bt_le_audio_broadcast_source_config fail \r\n", __func__);
		return RTK_BT_FAIL;
	}

	return RTK_BT_OK;
}

uint16_t rtk_bt_bap_broadcast_source_stop(void)
{
	uint16_t ret = 0;

	if (app_bap_bro_sour_info.broadcast_source_handle == NULL) {
		BT_LOGE("%s: broadcast_source_handle is NULL!\r\n", __func__);
		return RTK_BT_FAIL;
	}

	/* broadcast stream remove iso data path */
	for (uint8_t i = 0; i < app_bap_bro_sour_info.bis_info.num_bis; i++) {
		ret = rtk_bt_le_audio_broadcast_source_remove_data_path(app_bap_bro_sour_info.broadcast_source_handle, app_bap_bro_sour_info.bis_info.bis_conn_info[i].bis_idx);
		BT_LOGA("broadcast source remove data path %s for broadcast_source_handle 0x%08x bis_index %d,bis_conn_handle 0x%x, ret: 0x%x\r\n",
				((RTK_BT_OK != ret) ? "fail" : "ok"),
				app_bap_bro_sour_info.broadcast_source_handle,
				app_bap_bro_sour_info.bis_info.bis_conn_info[i].bis_idx,
				app_bap_bro_sour_info.bis_info.bis_conn_info[i].bis_conn_handle, ret);
		if (ret == RTK_BT_OK) {
			app_bt_le_audio_iso_data_path_remove(app_bap_bro_sour_info.bis_info.bis_conn_info[i].bis_conn_handle, RTK_BLE_AUDIO_ISO_DATA_PATH_TX);
		} else {
			BT_LOGE("%s: rtk_bt_le_audio_broadcast_source_remove_data_path fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
	}

	/* broadcast stream disable */
	if (rtk_bt_le_audio_broadcast_source_disable(app_bap_bro_sour_info.broadcast_source_handle)) {
		BT_LOGE("%s rtk_bt_le_audio_broadcast_source_disable fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	/* broadcast stream release */
	if (rtk_bt_le_audio_broadcast_source_release(app_bap_bro_sour_info.broadcast_source_handle)) {
		BT_LOGE("%s rtk_bt_le_audio_broadcast_source_release fail \r\n", __func__);
		return RTK_BT_FAIL;
	}

	return RTK_BT_OK;
}

/* bap broadcast sink */
static uint16_t rtk_bt_le_audio_broadcast_sink_select_bis(app_bt_le_audio_bap_broadcast_sink_info_t *p_bap_bro_sink_info,
														  rtk_bt_le_audio_sync_handle_t sync_handle)
{
	uint16_t ret = RTK_BT_OK;
	uint8_t num_bis = 0, idx = 0;
	uint8_t bis_idx[RTK_BT_LE_AUDIO_RECEIVER_BIS_MAX_NUM] = {0};
	uint32_t support_bis_array = 0;
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;

	if (!p_bap_bro_sink_info || !sync_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(sync_handle);
	if (!p_sync_dev_info) {
		BT_LOGE("%s not find sync dev info for sync_handle %08x\r\n", __func__, sync_handle);
		return RTK_BT_FAIL;
	}
	ret = rtk_bt_le_audio_sync_get_support_bis_array(sync_handle, p_bap_bro_sink_info->config_sink_audio_location, &support_bis_array);
	if (ret != RTK_BT_OK) {
		BT_LOGE("%s rtk_bt_le_audio_sync_get_support_bis_array fail,ret =0x%x\r\n", __func__, ret);
		return ret;
	}
	if (support_bis_array == 0) {
		BT_LOGE("%s supported_bis_array is 0, no support bis in broadcast source\r\n", __func__);
		return RTK_BT_ERR_UNSUPPORTED;
	}
	p_sync_dev_info->support_bis_array = support_bis_array;
	BT_LOGA("%s support_bis_array=0x%x\r\n", __func__, (unsigned int)support_bis_array);
	while (support_bis_array) {
		if (support_bis_array & 0x01) {
			bis_idx[num_bis] = (idx + 1);
			BT_LOGA("%s bis_idx[%d]=%d\r\n", __func__, num_bis, bis_idx[num_bis]);
			num_bis++;
			if (num_bis == RTK_BT_LE_AUDIO_RECEIVER_BIS_MAX_NUM) {
				break;
			}
		}
		support_bis_array >>= 1;
		idx++;
	}
	if (num_bis) {
		p_sync_dev_info->num_bis = num_bis;
		memcpy(p_sync_dev_info->bis_idx, bis_idx, sizeof(bis_idx));
		BT_LOGA("%s num_bis=%d, p_sync_dev_info->bis_idx[0]=%d\r\n", __func__, num_bis, p_sync_dev_info->bis_idx[0]);
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

static uint16_t rtk_bt_le_audio_broadcast_sink_big_sync(app_bt_le_audio_bap_broadcast_sink_info_t *p_bap_bro_sink_info, rtk_bt_le_addr_t adv_addr)
{
	uint16_t ret = RTK_BT_OK;
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	char addr_str[32] = {0};
	rtk_bt_le_addr_to_str(&adv_addr, addr_str, sizeof(addr_str));

	if (!p_bap_bro_sink_info) {
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_sync_dev_info = app_bt_le_audio_sync_dev_list_find_by_addr(adv_addr);
	if (!p_sync_dev_info) {
		BT_LOGE("%s not find sync dev info for addr_str %s\r\n", __func__, addr_str);
		return RTK_BT_FAIL;
	}
	ret = rtk_bt_le_audio_broadcast_sink_select_bis(p_bap_bro_sink_info, p_sync_dev_info->sync_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("rtk_bt_le_audio_broadcast_sink_select_bis fail ,ret=0x%x\r\n", ret);
		return ret;
	}
	BT_LOGA("addr_str %s pa_sync_state=%d,big_sync_state =%d\r\n", addr_str, p_sync_dev_info->pa_sync_state, p_sync_dev_info->big_sync_state);
	if (p_sync_dev_info->pa_sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZED &&
		p_sync_dev_info->big_sync_state != RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZING &&
		p_sync_dev_info->big_sync_state != RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZED) {
		rtk_bt_le_audio_big_sync_establish_t big_sync_establish_param = {
			.encryption = p_sync_dev_info->is_encryption,
			.broadcast_code = {0},
			.mse = 0,
			.big_sync_timeout = RTK_LE_AUDIO_SYNC_TIMEOUT, //10*N ms
			.num_bis = 0,
			.bis = {0}
		};
		if (p_sync_dev_info->is_encryption) {
			memcpy(big_sync_establish_param.broadcast_code, p_sync_dev_info->broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
		}
		big_sync_establish_param.num_bis = p_sync_dev_info->num_bis;
		memcpy(big_sync_establish_param.bis, p_sync_dev_info->bis_idx, p_sync_dev_info->num_bis);
		ret = rtk_bt_le_audio_big_sync_establish(p_sync_dev_info->sync_handle, &big_sync_establish_param);
		BT_LOGA("broadcast sink big establish %s for sync_handle 0x%08x ! ret: 0x%x\r\n",
				((RTK_BT_OK != ret) ? "fail" : "ok"), p_sync_dev_info->sync_handle, ret);
	}

	return RTK_BT_OK;
}

uint16_t rtk_bt_bap_broadcast_sync_start(rtk_bt_bap_bass_scan_dev_info_t *p_info)
{
	uint16_t ret = RTK_BT_OK;
	rtk_bt_le_audio_sync_handle_t sync_handle = NULL;
	rtk_bt_le_audio_pa_sync_establish_t establish_param = {
		.options = 0,
		.sync_cte_type = 0,
		.skip = 0,
		.sync_timeout = RTK_LE_AUDIO_SYNC_TIMEOUT //10*N ms
	};
	app_bt_le_audio_bass_scan_dev_info_t *p_bscan_dev_info = NULL;
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	char addr_str[32] = {0};

	if (!p_info) {
		BT_LOGE("%s p_info is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	rtk_bt_le_addr_to_str(&p_info->adv_addr, addr_str, sizeof(addr_str));
	p_bscan_dev_info = app_bt_le_audio_bass_scan_dev_list_find(p_info->adv_addr);
	if (!p_bscan_dev_info) {
		BT_LOGE("%s: (%s) is not contained within bass scan device list, please do rtk_bt_bap_bass_scan firstly \r\n", __func__, addr_str);
		return RTK_BT_FAIL;
	}
	/* do pa sync */
	{
		ret = rtk_bt_le_audio_sync_find(p_bscan_dev_info->adv_sid, p_bscan_dev_info->broadcast_id, &sync_handle);
		if (ret != RTK_BT_OK) {
			BT_LOGE("%s rtk_bt_le_audio_sync_find fail (ret=0x%x)\r\n", __func__, ret);
			return RTK_BT_FAIL;
		}
		/* create sync when sync_handle is NULL */
		if (sync_handle == NULL) {
			ret = rtk_bt_le_audio_sync_allocate(p_bscan_dev_info->adv_sid, p_info->adv_addr, p_bscan_dev_info->broadcast_id, &sync_handle);
			if (RTK_BT_OK != ret) {
				BT_LOGE("%s rtk_bt_le_audio_sync_allocate fail, ret: 0x%x\r\n", __func__, ret);
				return ret;
			}
		}
		ret = rtk_bt_le_audio_pa_sync_establish(sync_handle, &establish_param);
		if (RTK_BT_OK != ret || sync_handle == NULL) {
			BT_LOGE("%s rtk_bt_le_audio_pa_sync_establish fail, ret: 0x%x\r\n", __func__, ret);
			return ret;
		}
		p_sync_dev_info = app_bt_le_audio_sync_dev_list_add(sync_handle);
		if (p_sync_dev_info == NULL) {
			BT_LOGE("%s app_bt_le_audio_sync_dev_list_add fail\r\n", __func__);
			return RTK_BT_FAIL;
		}
		p_sync_dev_info->adv_sid = p_bscan_dev_info->adv_sid;
		memcpy(&p_sync_dev_info->adv_addr, &p_info->adv_addr, sizeof(rtk_bt_le_addr_t));
		memcpy(p_sync_dev_info->broadcast_id, p_bscan_dev_info->broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);
		ret = rtk_bt_le_gap_set_ext_scan_param(&app_lea_def_ext_scan_param);
		if (ret != RTK_BT_OK) {
			BT_LOGE("%s rtk_bt_le_gap_set_ext_scan_param fail for addr_str (%s)\r\n", __func__, addr_str);
			return ret;
		}
		/* if ext scan is stop previous, rtk_bt_le_gap_start_ext_scan() will be scuess */
		/* else if ext scan is started previous, rtk_bt_le_gap_start_ext_scan() will be fail cause currently is started */
		/* so there is no need to judge return value of rtk_bt_le_gap_start_ext_scan() */
		/* if rtk_bt_le_gap_start_ext_scan() finally fail due to other reason, the flow will also be terminated by rtk_bt_le_audio_broadcast_sink_big_sync() following */
		rtk_bt_le_gap_start_ext_scan();
	}
	if (app_bap_bro_sink_info.big_info_ind_sem) {
		osif_sem_take(app_bap_bro_sink_info.big_info_ind_sem, 1000);
		ret = rtk_bt_le_audio_broadcast_sink_big_sync(&app_bap_bro_sink_info, p_info->adv_addr);
		if (ret) {
			rtk_bt_le_gap_stop_ext_scan();
		}
	}

	return ret;
}

static uint16_t rtk_bt_bap_broadcast_big_sync_term(app_bt_le_audio_sync_dev_info_t *p_sync_dev_info)
{
	uint16_t ret = RTK_BT_OK;
	char addr_str[32] = {0};

	if (p_sync_dev_info == NULL) {
		BT_LOGE("%s: not find sync dev info for addr_str (%s)\r\n", __func__, addr_str);
		return RTK_BT_ERR_NO_ENTRY;
	}
	rtk_bt_le_addr_to_str(&p_sync_dev_info->adv_addr, addr_str, sizeof(addr_str));

	if (p_sync_dev_info->big_sync_state == RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZED) {
		ret = rtk_bt_le_audio_big_sync_terminate(p_sync_dev_info->sync_handle);
		BT_LOGA("broadcast sink big terminate %s for addr_str %s ! ret: 0x%x\r\n", ((RTK_BT_OK != ret) ? "fail" : "ok"),
				addr_str, ret);
	}

	return ret;
}

uint16_t rtk_bt_bap_broadcast_sync_term(rtk_bt_le_addr_t *padv_addr)
{
	uint16_t ret = RTK_BT_OK;
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	char addr_str[32] = {0};

	if (!padv_addr) {
		BT_LOGE("%s padv_addr is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	rtk_bt_le_addr_to_str(padv_addr, addr_str, sizeof(addr_str));
	p_sync_dev_info = app_bt_le_audio_sync_dev_list_find_by_addr(*padv_addr);
	if (!p_sync_dev_info) {
		BT_LOGE("%s not find sync dev info for addr_str (%s)\r\n", __func__, addr_str);
		return RTK_BT_ERR_NO_ENTRY;
	}
	if (rtk_bt_bap_broadcast_big_sync_term(p_sync_dev_info)) {
		BT_LOGE("%s: big sync terminate fail for addr_str (%s)\r\n", __func__, addr_str);
		return RTK_BT_FAIL;
	}
	if (p_sync_dev_info->pa_sync_state != RTK_BT_LE_AUDIO_PA_SYNC_STATE_TERMINATED) {
		ret = rtk_bt_le_audio_pa_sync_terminate(p_sync_dev_info->sync_handle);
		BT_LOGA("%s rtk_bt_le_audio_pa_sync_terminate addr_str (%s) %s, ret: 0x%x\r\n", __func__, addr_str,
				((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
	}

	return ret;
}

#if defined(CONFIG_BT_BAP_SUPPORT) && CONFIG_BT_BAP_SUPPORT
uint16_t rtk_bt_bap_broadcast_sink_cfg(uint8_t channel)
{
	char ch = 0;
	uint8_t device_name_len = 0;
	/* Check bt enable status (rtk_bt_bap_unicast_server_cfg should only be invoked before RTK_BT_IS_ENABLED()) */
	if (rtk_bt_is_enable()) {
		BT_LOGE("%s: should not be invoked after bt enable \r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (channel < 1 || channel > 3) {
		BT_LOGE("%s: channel should be within 1 - 3 \r\n", __func__);
		return RTK_BT_FAIL;
	}
	device_name_len = strlen((char *)g_bro_sink_info.device_name);
	if (device_name_len > RTK_BT_GAP_DEVICE_NAME_LEN) {
		BT_LOGE("%s: device_name_len (%d) > RTK_BT_GAP_DEVICE_NAME_LEN(%d)\r\n", __func__, device_name_len, RTK_BT_GAP_DEVICE_NAME_LEN);
		return RTK_BT_FAIL;
	}
	switch (channel) {
	case RTK_BT_LE_AUDIO_LEFT:
		ch = 'L';
		break;
	case RTK_BT_LE_AUDIO_RIGHT:
		ch = 'R';
		break;
	case RTK_BT_LE_AUDIO_STEREO:
		ch = 'S';
		break;
	default:
		BT_LOGE("%s: unknown channel allocation \r\n", __func__);
		break;
	}
	g_bro_sink_info.device_name[device_name_len - 1] = ch;
	g_bro_sink_info.sound_channel = channel;

	return RTK_BT_OK;
}
#endif

/* bap broadcast assistant */
uint16_t rtk_bt_bap_broadcast_assistant_sync_start(rtk_bt_bap_bass_scan_dev_info_t *p_info)
{
	uint16_t ret = RTK_BT_OK;
	rtk_bt_le_audio_sync_handle_t sync_handle = NULL;
	rtk_bt_le_audio_pa_sync_establish_t establish_param = {
		.options = 0,
		.sync_cte_type = 0,
		.skip = 0,
		.sync_timeout = RTK_LE_AUDIO_SYNC_TIMEOUT //10*N ms
	};
	app_bt_le_audio_bass_scan_dev_info_t *p_bscan_dev_info = NULL;
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	char addr_str[32] = {0};

	if (!p_info) {
		BT_LOGE("%s p_info is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	rtk_bt_le_addr_to_str(&p_info->adv_addr, addr_str, sizeof(addr_str));
	p_bscan_dev_info = app_bt_le_audio_bass_scan_dev_list_find(p_info->adv_addr);
	if (!p_bscan_dev_info) {
		BT_LOGE("%s: (%s) is not contained within bass scan device list, please do rtk_bt_bap_bass_scan firstly \r\n", __func__, addr_str);
		return RTK_BT_FAIL;
	}
	/* do pa sync */
	{
		ret = rtk_bt_le_audio_sync_find(p_bscan_dev_info->adv_sid, p_bscan_dev_info->broadcast_id, &sync_handle);
		if (ret != RTK_BT_OK) {
			BT_LOGE("%s rtk_bt_le_audio_sync_find fail (ret=0x%x)\r\n", __func__, ret);
			return RTK_BT_FAIL;
		}
		/* create sync when sync_handle is NULL */
		if (sync_handle == NULL) {
			ret = rtk_bt_le_audio_sync_allocate(p_bscan_dev_info->adv_sid, p_info->adv_addr, p_bscan_dev_info->broadcast_id, &sync_handle);
			if (RTK_BT_OK != ret) {
				BT_LOGE("%s rtk_bt_le_audio_sync_allocate fail, ret: 0x%x\r\n", __func__, ret);
				return ret;
			}
		}
		ret = rtk_bt_le_audio_pa_sync_establish(sync_handle, &establish_param);
		if (RTK_BT_OK != ret || sync_handle == NULL) {
			BT_LOGE("%s rtk_bt_le_audio_pa_sync_establish fail, ret: 0x%x\r\n", __func__, ret);
			return ret;
		}
		p_sync_dev_info = app_bt_le_audio_sync_dev_list_add(sync_handle);
		if (p_sync_dev_info == NULL) {
			BT_LOGE("%s app_bt_le_audio_sync_dev_list_add fail\r\n", __func__);
			return RTK_BT_FAIL;
		}
		p_sync_dev_info->adv_sid = p_bscan_dev_info->adv_sid;
		memcpy(&p_sync_dev_info->adv_addr, &p_info->adv_addr, sizeof(rtk_bt_le_addr_t));
		memcpy(p_sync_dev_info->broadcast_id, p_bscan_dev_info->broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);
		ret = rtk_bt_le_gap_set_ext_scan_param(&app_lea_def_ext_scan_param);
		if (ret != RTK_BT_OK) {
			BT_LOGE("%s rtk_bt_le_gap_set_ext_scan_param fail for addr_str (%s)\r\n", __func__, addr_str);
			return ret;
		}
		/* if ext scan is stop previous, rtk_bt_le_gap_start_ext_scan() will be scuess */
		/* else if ext scan is started previous, rtk_bt_le_gap_start_ext_scan() will be fail cause currently is started */
		/* so there is no need to judge return value of rtk_bt_le_gap_start_ext_scan() */
		/* if rtk_bt_le_gap_start_ext_scan() finally fail due to other reason, the flow will also be terminated by rtk_bt_le_audio_broadcast_sink_big_sync() following */
		rtk_bt_le_gap_start_ext_scan();
	}
	if (app_bap_bro_assi_info.pa_sync_ind_sem) {
		osif_sem_take(app_bap_bro_assi_info.pa_sync_ind_sem, 1000);
		app_bt_le_audio_bass_pa_show();
	}

	return ret;
}

uint16_t rtk_bt_bap_broadcast_assistant_sync_term(rtk_bt_le_addr_t *padv_addr)
{
	uint16_t ret = RTK_BT_OK;
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	char addr_str[32] = {0};

	if (!padv_addr) {
		BT_LOGE("%s padv_addr is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	rtk_bt_le_addr_to_str(padv_addr, addr_str, sizeof(addr_str));
	p_sync_dev_info = app_bt_le_audio_sync_dev_list_find_by_addr(*padv_addr);
	if (!p_sync_dev_info) {
		BT_LOGE("%s not find sync dev info for addr_str (%s)\r\n", __func__, addr_str);
		return RTK_BT_ERR_NO_ENTRY;
	}
	if (p_sync_dev_info->pa_sync_state != RTK_BT_LE_AUDIO_PA_SYNC_STATE_TERMINATED) {
		ret = rtk_bt_le_audio_pa_sync_terminate(p_sync_dev_info->sync_handle);
		BT_LOGA("%s rtk_bt_le_audio_pa_sync_terminate addr_str (%s) %s, ret: 0x%x\r\n", __func__, addr_str,
				((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
	}
	ret = rtk_bt_le_audio_sync_release(p_sync_dev_info->sync_handle);
	BT_LOGA("rtk_bt_le_audio_sync_release %s, ret: 0x%x\r\n", ((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
	ret = app_bt_le_audio_sync_dev_list_remove(p_sync_dev_info->sync_handle);
	BT_LOGA("%s app_bt_le_audio_sync_dev_list_remove addr_str (%s) %s, ret: 0x%x\r\n", __func__, addr_str,
			((RTK_BT_OK != ret) ? "fail" : "ok"), ret);

	return ret;
}

uint16_t rtk_bt_bap_broadcast_assistant_reception_start(rtk_bt_le_addr_t *padv_addr, uint8_t group_idx)
{
	uint16_t ret = RTK_BT_OK;
	uint8_t i = 0, j = 0, group_num = 0, device_num = 0;
	uint8_t pa_sync = RTK_BT_LE_AUDIO_BASS_PA_SYNC_NO_PAST;
	uint32_t bis_array = 0, dev_tbl_size = 0;
	rtk_bt_le_audio_group_device_info_t *p_dev_tbl = NULL;
	rtk_bt_le_audio_group_handle_t group_handle = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;

	if (!padv_addr) {
		BT_LOGE("%s padv_addr is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	group_num = app_bt_le_audio_group_list_get_num();
	if (group_idx >= group_num) {
		BT_LOGE("%s error: group_idx(%d) >= group_num (%d)\r\n", __func__, group_idx, group_num);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	group_handle = app_bt_le_audio_group_list_find_by_group_idx(group_idx);
	if (group_handle == NULL) {
		BT_LOGE("%s: group_idx (%d) not find\r\n", __func__, group_idx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_sync_dev_info = app_bt_le_audio_sync_dev_list_find_by_addr(*padv_addr);
	if (p_sync_dev_info == NULL) {
		BT_LOGE("%s: src_adv_addr (%s) not find\r\n", __func__, padv_addr->addr_val);
		return RTK_BT_ERR_PARAM_INVALID;
	} else {
		if (p_sync_dev_info->pa_sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZED) {
			pa_sync = RTK_BT_LE_AUDIO_BASS_PA_SYNC_PAST;
		}
	}
	ret = rtk_bt_le_audio_group_get_used_device_num(group_handle, true, &device_num);
	if (ret != RTK_BT_OK) {
		BT_LOGE("%s get_device_num fail for group_handle = %08x\r\n", __func__, group_handle);
		return RTK_BT_FAIL;
	} else {
		BT_LOGA("%s device_num = %d\r\n", __func__, device_num);
	}
	dev_tbl_size = device_num * sizeof(rtk_bt_le_audio_group_device_info_t);
	p_dev_tbl = (rtk_bt_le_audio_group_device_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, dev_tbl_size);
	if (p_dev_tbl == NULL) {
		BT_LOGE("%s p_dev_tbl osif_mem_alloc fail, size=%d\r\n", __func__, (int)dev_tbl_size);
		return RTK_BT_ERR_NO_MEMORY;
	}
	memset(p_dev_tbl, 0, dev_tbl_size);
	for (i = 0; i < device_num; i++) {
		ret = rtk_bt_le_audio_group_get_all_device_info(group_handle, &device_num, p_dev_tbl);
		if (ret != RTK_BT_OK) {
			BT_LOGE("%s get_all_device_info fail for group_handle=%08x\r\n", __func__, group_handle);
			continue;
		}
		if (p_dev_tbl[i].is_used && p_dev_tbl[i].conn_state == RTK_BT_LE_AUDIO_GAP_CONN_STATE_CONNECTED) {
			BT_LOGA("%s conn_handle=%d\r\n", __func__, p_dev_tbl[i].conn_handle);
			p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(p_dev_tbl[i].conn_handle);
			if (p_device_info == NULL) {
				BT_LOGE("%s group_list_find_dev fail for conn_handle=%d\r\n", __func__, p_dev_tbl[i].conn_handle);
				continue;
			}
			if (p_device_info->bass_is_found == 0) {
				BT_LOGE("%s bass_is_found not support for conn_handle=%d\r\n", __func__, p_dev_tbl[i].conn_handle);
				continue;
			}
			ret = rtk_bt_le_audio_pacs_get_bis_array_by_sync_info(p_dev_tbl[i].conn_handle, p_sync_dev_info->sync_handle, RTK_BT_LE_AUDIO_PACS_BA_CHECK_FILTER_CODEC,
																  &bis_array);
			if (ret != RTK_BT_OK) {
				BT_LOGE("%s get_bis_array_by_sync_info fail for conn_handle = %d\r\n", __func__, p_dev_tbl[i].conn_handle);
				continue;
			}
			for (j = 0; j < p_device_info->brs_char_num; j++) {
				if (p_device_info->brs_char_tbl[j].brs_is_used && p_device_info->brs_char_tbl[j].sync_handle == p_sync_dev_info->sync_handle) {
					break;
				}
			}
			/* find the sync_handle in brs_char_tbl, means the broadcast source is added, so modify it */
			if (j != p_device_info->brs_char_num) {
				ret = rtk_bt_le_audio_bass_client_cp_modify_source_by_sync_info(p_dev_tbl[i].conn_handle, p_sync_dev_info->sync_handle,
																				p_device_info->brs_char_tbl[j].source_id, pa_sync, bis_array, false);
			} else {
				ret = rtk_bt_le_audio_bass_client_cp_add_source_by_sync_info(p_dev_tbl[i].conn_handle, p_sync_dev_info->sync_handle, pa_sync, bis_array, false);
			}
			BT_LOGA("%s bass add source for sync_handle %08x %s, ret: 0x%x\r\n", __func__, p_sync_dev_info->sync_handle,
					((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
		}
	}
	if (p_dev_tbl) {
		osif_mem_free(p_dev_tbl);
	}

	return RTK_BT_OK;
}

uint16_t rtk_bt_bap_broadcast_assistant_reception_stop(rtk_bt_le_addr_t *padv_addr, uint8_t group_idx)
{
	uint16_t ret = RTK_BT_OK;
	uint8_t i = 0, j = 0, group_num = 0, device_num = 0;
	uint32_t dev_tbl_size = 0;
	rtk_bt_le_audio_group_device_info_t *p_dev_tbl = NULL;
	rtk_bt_le_audio_group_handle_t group_handle = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;

	if (!padv_addr) {
		BT_LOGE("%s padv_addr is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	group_num = app_bt_le_audio_group_list_get_num();
	if (group_idx >= group_num) {
		BT_LOGE("%s error: group_idx(%d) >= group_num (%d)\r\n", __func__, group_idx, group_num);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	group_handle = app_bt_le_audio_group_list_find_by_group_idx(group_idx);
	if (group_handle == NULL) {
		BT_LOGE("%s: group_idx (%d) not find\r\n", __func__, group_idx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_sync_dev_info = app_bt_le_audio_sync_dev_list_find_by_addr(*padv_addr);
	if (p_sync_dev_info == NULL) {
		BT_LOGE("%s: src_adv_addr (%s) not find\r\n", __func__, padv_addr->addr_val);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	ret = rtk_bt_le_audio_group_get_used_device_num(group_handle, true, &device_num);
	if (ret != RTK_BT_OK) {
		BT_LOGE("%s get_device_num fail for group_handle = %08x\r\n", __func__, group_handle);
		return RTK_BT_FAIL;
	} else {
		BT_LOGA("%s device_num = %d\r\n", __func__, device_num);
	}
	dev_tbl_size = device_num * sizeof(rtk_bt_le_audio_group_device_info_t);
	p_dev_tbl = (rtk_bt_le_audio_group_device_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, dev_tbl_size);
	if (p_dev_tbl == NULL) {
		BT_LOGE("%s p_dev_tbl osif_mem_alloc fail, size=%d\r\n", __func__, (int)dev_tbl_size);
		return RTK_BT_ERR_NO_MEMORY;
	}
	memset(p_dev_tbl, 0, dev_tbl_size);
	for (i = 0; i < device_num; i++) {
		ret = rtk_bt_le_audio_group_get_all_device_info(group_handle, &device_num, p_dev_tbl);
		if (ret != RTK_BT_OK) {
			BT_LOGE("%s get_all_device_info fail for group_handle = %08x\r\n", __func__, group_handle);
			continue;
		}
		if (p_dev_tbl[i].is_used && p_dev_tbl[i].conn_state == RTK_BT_LE_AUDIO_GAP_CONN_STATE_CONNECTED) {
			p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(p_dev_tbl[i].conn_handle);
			if (p_device_info == NULL) {
				BT_LOGE("%s group_list_find_dev fail for conn_handle = %d\r\n", __func__, p_dev_tbl[i].conn_handle);
				continue;
			}
			if (p_device_info->bass_is_found) {
				for (j = 0; j < p_device_info->brs_char_num; j++) {
					if (p_device_info->brs_char_tbl[j].brs_is_used && p_device_info->brs_char_tbl[j].sync_handle == p_sync_dev_info->sync_handle) {
						break;
					}
				}
				/* find the sync_handle in brs_char_tbl, means the broadcast source is added, so modify it */
				if (j != p_device_info->brs_char_num) {
					ret = rtk_bt_le_audio_bass_client_cp_modify_source_by_sync_info(p_dev_tbl[i].conn_handle, p_sync_dev_info->sync_handle,
																					p_device_info->brs_char_tbl[j].source_id,
																					RTK_BT_LE_AUDIO_BASS_PA_NOT_SYNC, 0, false);
					BT_LOGA("%s bass modify source by sync_info for sync_handle %08x %s, ret: 0x%x\r\n", __func__, p_sync_dev_info->sync_handle,
							((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
				} else {
					BT_LOGE("%s not find sync_handle %08x in brs_char_tbl for conn_handle=%d\r\n", __func__, p_sync_dev_info->sync_handle,
							p_dev_tbl[i].conn_handle);
				}
			}
		}
	}
	if (p_dev_tbl) {
		osif_mem_free(p_dev_tbl);
	}

	return RTK_BT_OK;
}

uint16_t rtk_bt_bap_broadcast_assistant_reception_remove(rtk_bt_le_addr_t *padv_addr, uint8_t group_idx)
{
	uint16_t ret = RTK_BT_OK;
	uint8_t i = 0, j = 0, group_num = 0, device_num = 0;
	uint32_t dev_tbl_size = 0;
	rtk_bt_le_audio_group_device_info_t *p_dev_tbl = NULL;
	rtk_bt_le_audio_group_handle_t group_handle = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;

	if (!padv_addr) {
		BT_LOGE("%s padv_addr is NULL \r\n", __func__);
		return RTK_BT_FAIL;
	}
	group_num = app_bt_le_audio_group_list_get_num();
	if (group_idx >= group_num) {
		BT_LOGE("%s error: group_idx(%d) >= group_num (%d)\r\n", __func__, group_idx, group_num);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	group_handle = app_bt_le_audio_group_list_find_by_group_idx(group_idx);
	if (group_handle == NULL) {
		BT_LOGE("%s: group_idx (%d) not find\r\n", __func__, group_idx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_sync_dev_info = app_bt_le_audio_sync_dev_list_find_by_addr(*padv_addr);
	if (p_sync_dev_info == NULL) {
		BT_LOGE("%s: src_adv_addr (%s) not find\r\n", __func__, padv_addr->addr_val);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	ret = rtk_bt_le_audio_group_get_used_device_num(group_handle, true, &device_num);
	if (ret != RTK_BT_OK) {
		BT_LOGE("%s get_device_num fail for group_handle = %08x\r\n", __func__, group_handle);
		return RTK_BT_FAIL;
	} else {
		BT_LOGA("%s device_num = %d\r\n", __func__, device_num);
	}
	dev_tbl_size = device_num * sizeof(rtk_bt_le_audio_group_device_info_t);
	p_dev_tbl = (rtk_bt_le_audio_group_device_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, dev_tbl_size);
	if (p_dev_tbl == NULL) {
		BT_LOGE("%s p_dev_tbl osif_mem_alloc fail, size=%d\r\n", __func__, (int)dev_tbl_size);
		return RTK_BT_ERR_NO_MEMORY;
	}
	memset(p_dev_tbl, 0, dev_tbl_size);
	for (i = 0; i < device_num; i++) {
		ret = rtk_bt_le_audio_group_get_all_device_info(group_handle, &device_num, p_dev_tbl);
		if (ret != RTK_BT_OK) {
			BT_LOGE("%s get_all_device_info fail for group_handle = %08x\r\n", __func__, group_handle);
			continue;
		}
		if (p_dev_tbl[i].is_used && p_dev_tbl[i].conn_state == RTK_BT_LE_AUDIO_GAP_CONN_STATE_CONNECTED) {
			p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(p_dev_tbl[i].conn_handle);
			if (p_device_info == NULL) {
				BT_LOGE("%s group_list_find_dev fail for conn_handle = %d\r\n", __func__, p_dev_tbl[i].conn_handle);
				continue;
			}
			if (p_device_info->bass_is_found) {
				for (j = 0; j < p_device_info->brs_char_num; j++) {
					if (p_device_info->brs_char_tbl[j].brs_is_used && p_device_info->brs_char_tbl[j].sync_handle == p_sync_dev_info->sync_handle) {
						break;
					}
				}
				/* find the sync_handle in brs_char_tbl, means the broadcast source is added, so modify it */
				if (j != p_device_info->brs_char_num) {
					rtk_bt_le_audio_bass_cp_remove_source_param_t remove_source_param = {0};
					remove_source_param.source_id = p_device_info->brs_char_tbl[j].source_id;
					ret = rtk_bt_le_audio_bass_client_write_cp_op(p_dev_tbl[i].conn_handle, RTK_BT_LE_AUDIO_BASS_CP_REMOVE_SOURCE, (void *)&remove_source_param, false);
					BT_LOGA("%s bass remove source for sync_handle %08x %s, ret: 0x%x\r\n", __func__, p_sync_dev_info->sync_handle,
							((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
				} else {
					BT_LOGE("%s not find sync_handle %08x in brs_char_tbl for conn_handle=%d\r\n", __func__, p_sync_dev_info->sync_handle, p_dev_tbl[i].conn_handle);
				}
			}
		}
	}
	if (p_dev_tbl) {
		osif_mem_free(p_dev_tbl);
	}

	return RTK_BT_OK;
}

/* bap scan delegator none */

/* bap unicast client */
static uint16_t rtk_bt_bap_unicast_client_audio_config(rtk_bt_le_audio_group_handle_t group_handle, rtk_bt_le_audio_device_handle_t *device_handle_tbl)
{
	uint8_t idx = 0;
	uint32_t audio_cfg_mask = 0;
	rtk_bt_le_audio_unicast_cfg_type_t audio_cfg_type = RTK_BT_LE_UNICAST_AUDIO_CFG_UNKNOWN;
	uint32_t local_support_audio_cfg_mask = 0, prefer_audio_cfg_mask = 0;
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;

	p_group_info = app_bt_le_audio_group_list_find(group_handle);
	if (p_group_info == NULL) {
		BT_LOGE("%s: p_group_info is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_MEDIA) {
		local_support_audio_cfg_mask = RTK_BT_LE_MEDIA_AUDIO_CFG_SUPPORT;
	} else if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION) {
		local_support_audio_cfg_mask = RTK_BT_LE_CONVERSATIONAL_AUDIO_CFG_SUPPORT;
	} else {
		BT_LOGE("%s: play_mode(%d) error\r\n", __func__, p_group_info->play_mode);
		return RTK_BT_FAIL;
	}
	if (rtk_bt_le_audio_unicast_get_cfg_mask(p_group_info->stream_session_handle,
											 local_support_audio_cfg_mask,
											 p_group_info->dev_num,
											 device_handle_tbl,
											 &audio_cfg_mask)) {
		BT_LOGE("%s: rtk_bt_le_audio_unicast_get_cfg_mask fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (audio_cfg_mask) {
		if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_MEDIA) {
			prefer_audio_cfg_mask = RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER;
		} else if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION) {
			prefer_audio_cfg_mask = RTK_BT_LE_CONVERSATIONAL_AUDIO_CFG_PREFER;
		}
		if (p_group_info->dev_num == 2) {
			if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_MEDIA) {
				prefer_audio_cfg_mask = RTK_BT_LE_UNICAST_AUDIO_CFG_6_II_BIT;
			} else if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION) {
				prefer_audio_cfg_mask = RTK_BT_LE_UNICAST_AUDIO_CFG_11_II_BIT;
			}
		} else if (p_group_info->dev_num == 1) {
			//get first device info
			p_device_info = app_bt_le_audio_group_list_find_dev_by_idx(group_handle, 0);
			if (p_device_info) {
				if (p_device_info->sink_ase_num == 2) {
					if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_MEDIA) {
						prefer_audio_cfg_mask = RTK_BT_LE_UNICAST_AUDIO_CFG_6_I_BIT;
					} else if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION) {
						prefer_audio_cfg_mask = RTK_BT_LE_UNICAST_AUDIO_CFG_11_I_BIT;
					}
				} else if (p_device_info->sink_ase_num == 1) {
					if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_MEDIA) {
#if RTK_BLE_AUDIO_UNICAST_ONE_CIS_SETEO_MODE
						prefer_audio_cfg_mask = RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT;
#else
						prefer_audio_cfg_mask = RTK_BT_LE_UNICAST_AUDIO_CFG_1_BIT;
#endif
					} else if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION) {
						prefer_audio_cfg_mask = RTK_BT_LE_UNICAST_AUDIO_CFG_3_BIT;
					}
				} else {
					BT_LOGE("%s: sink_ase_num(%d) not support\r\n", __func__, p_device_info->sink_ase_num);
				}
			}
		} else {
			BT_LOGE("%s: dev_num(%d) not support\r\n", __func__, p_group_info->dev_num);
		}
		if ((audio_cfg_mask & prefer_audio_cfg_mask) != 0) {
			audio_cfg_mask &= prefer_audio_cfg_mask;
		}
		BT_LOGA("%s: audio_cfg_mask is 0x%x\r\n", __func__, (unsigned int)audio_cfg_mask);
		while (audio_cfg_mask) {
			if (audio_cfg_mask & 0x01) {
				audio_cfg_type = (rtk_bt_le_audio_unicast_cfg_type_t)idx;
				break;
			}
			audio_cfg_mask >>= 1;
			idx++;
		}
	}
	if (audio_cfg_type >= RTK_BT_LE_UNICAST_AUDIO_CFG_MAX) {
		BT_LOGE("%s: audio_cfg_type (0x%x) error\r\n", __func__, audio_cfg_type);
		return RTK_BT_ERR_UNSUPPORTED;
	} else {
		BT_LOGE("%s: get audio_cfg_type (0x%x) ok\r\n", __func__, audio_cfg_type);
	}
	p_group_info->audio_cfg_type = audio_cfg_type;

	return RTK_BT_OK;
}

static void le_audio_unicast_client_fix_codec_mask(uint32_t *sup_snk_mask)
{
	uint32_t mask = *sup_snk_mask;

	BT_LOGA("%s before fix_codec_mask mask = 0x%x\r\n", __func__, (unsigned int)mask);
#if defined(LEA_SOURCE_FIX_FRAME_DUARTION)
	if (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) {
		mask &= 0xAAAAAAAA; //1010, clear odd bit(7.5ms is in odd bit)
	} else {
		mask &= 0x55555555; //0101, clear even bit(10ms is in even bit)
	}
	BT_LOGA("%s after fix duration(%d), mask = 0x%x\r\n", __func__, LEA_SOURCE_FIX_FRAME_DUARTION, (unsigned int)mask);
#endif
#if defined(LEA_SOURCE_FIX_SAMPLE_FREQUENCY)
	switch (LEA_SOURCE_FIX_SAMPLE_FREQUENCY) {
	case RTK_BT_LE_SAMPLING_FREQUENCY_CFG_8K:
		mask &= 0x3; //0011, keep bit 0 and bit 1
		break;
	case RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K:
		mask &= 0xc; //1100, keep bit 2 and bit 3
		break;
	case RTK_BT_LE_SAMPLING_FREQUENCY_CFG_24K:
		mask &= 0x30; //110000, keep bit 4 and bit 5
		break;
	case RTK_BT_LE_SAMPLING_FREQUENCY_CFG_32K:
		mask &= 0xc0; //11000000, keep bit 6 and bit 7
		break;
	case RTK_BT_LE_SAMPLING_FREQUENCY_CFG_44_1K:
		mask &= 0x300; //1100000000, keep bit 8 and bit 9
		break;
	case RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K:
		mask &= 0xfc00; //1111110000000000, keep bit 10 ~ bit 15
		break;
	default:
		break;
	}
	BT_LOGA("%s after fix sample frequency(%d), mask = 0x%x\r\n", __func__, LEA_SOURCE_FIX_SAMPLE_FREQUENCY, (unsigned int)mask);
#endif
	*sup_snk_mask = mask;
}

static uint16_t rtk_bt_bap_unicast_client_codec_cfg(rtk_bt_le_audio_group_handle_t group_handle)
{
	rtk_bt_le_audio_cfg_codec_t *p_codec_cfg = NULL;
	rtk_bt_le_audio_cfg_codec_t codec_cfg = {0};
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	uint8_t idx = 0, i = 0;
	uint8_t channel_count = 1; //default is 1
	uint32_t lc3_table_mask = 0, sup_snk_mask = 0xFFFFFFFF;
	uint32_t local_support_codec_cfg_mask = 0;
#if defined(LEA_SOURCE_ASE_PREFER_CODEC_CONFIG) && LEA_SOURCE_ASE_PREFER_CODEC_CONFIG
	uint8_t src_codec_idx = 0;
	rtk_bt_le_audio_cfg_codec_t src_ase_codec = {0};
	uint32_t sup_src_ase_mask = 0;
#endif
	p_group_info = app_bt_le_audio_group_list_find(group_handle);
	if (p_group_info == NULL) {
		BT_LOGE("%s p_group_info is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	}
	//set default prefer_codec
	memcpy(&p_group_info->prefer_codec, &app_lea_default_prefer_codec, sizeof(rtk_bt_le_audio_unicast_ase_codec_t));
	p_codec_cfg = &p_group_info->prefer_codec.codec_cfg;
	if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_MEDIA) {
		local_support_codec_cfg_mask = RTK_BT_LE_MEDIA_CODEC_CFG_SUPPORT;
	} else if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION) {
		local_support_codec_cfg_mask = RTK_BT_LE_CONVERSATIONAL_CODEC_CFG_SUPPORT;
	} else {
		BT_LOGE("%s play_mode(%d) error\r\n", __func__, p_group_info->play_mode);
		return RTK_BT_FAIL;
	}
	if (p_group_info->audio_cfg_type == RTK_BT_LE_UNICAST_AUDIO_CFG_4 || p_group_info->audio_cfg_type == RTK_BT_LE_UNICAST_AUDIO_CFG_5) {
		channel_count = 2;
	}
	for (i = 0; i < p_group_info->dev_num; i++) {
		p_device_info = app_bt_le_audio_group_list_find_dev_by_idx(group_handle, i);
		if (p_device_info == NULL) {
			BT_LOGE("%s p_device_info is NULL\r\n", __func__);
			continue;
		}
		/* lc3_table_mask define in rtk_bt_le_audio_codec_cfg_item_bit_mask_t */
		rtk_bt_le_audio_pacs_get_lc3_table_mask(p_device_info->conn_handle,
												RTK_BLE_AUDIO_SINK,
												RTK_BT_LE_AUDIO_CONTEXT_MEDIA,
												channel_count,
												&lc3_table_mask);
		/* get UNSPECIFIED lc3_table_mask when the MEDIA lc3_table_mask is 0 */
		if (!lc3_table_mask) {
			rtk_bt_le_audio_pacs_get_lc3_table_mask(p_device_info->conn_handle,
													RTK_BLE_AUDIO_SINK,
													RTK_BT_LE_AUDIO_CONTEXT_UNSPECIFIED,
													channel_count,
													&lc3_table_mask);
		}
		BT_LOGA("%s conn_handle = %d lc3_table_mask = 0x%x\r\n", __func__, p_device_info->conn_handle, (unsigned int)lc3_table_mask);
		sup_snk_mask &= lc3_table_mask;
	}
	if (sup_snk_mask != 0) {
		if ((sup_snk_mask & local_support_codec_cfg_mask) != 0) {
			BT_LOGA("%s sup_snk_mask(0x%x), local_support_codec_cfg_mask(0x%x)\r\n", __func__, (unsigned int)sup_snk_mask, (unsigned int)local_support_codec_cfg_mask);
			sup_snk_mask &= local_support_codec_cfg_mask;
		}
#if defined(LEA_SOURCE_ASE_PREFER_CODEC_CONFIG) && LEA_SOURCE_ASE_PREFER_CODEC_CONFIG
		if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION) {
			sup_src_ase_mask = sup_snk_mask;
			while (sup_src_ase_mask) {
				if (sup_src_ase_mask & 0x01) {
					if (src_codec_idx == RTK_BT_LE_CODEC_CFG_ITEM_32_2) {
						BT_APP_PROCESS(rtk_bt_le_audio_get_prefer_codec_cfg(src_codec_idx, &src_ase_codec));
						BT_LOGA("%s: source ase prefer_codec_cfg idx is %d\r\n", __func__, src_codec_idx);
						memcpy(&app_lea_source_ase_prefer_codec.codec_cfg, &src_ase_codec, sizeof(rtk_bt_le_audio_cfg_codec_t));
						break;
					}
				}
				sup_src_ase_mask >>= 1;
				src_codec_idx++;
			}
		}
#endif
#if defined(LEA_SOURCE_FIX_SAMPLE_FREQUENCY) || defined(LEA_SOURCE_FIX_FRAME_DUARTION)
		le_audio_unicast_client_fix_codec_mask(&sup_snk_mask);
#endif
		BT_LOGA("%s final sup_snk_mask = 0x%x\r\n", __func__, (unsigned int)sup_snk_mask);
		while (sup_snk_mask) {
#if defined(CODEC_DFG_TEST) && CODEC_DFG_TEST
			if (sup_snk_mask & 0x01) {
				if (idx == RTK_BT_LE_CODEC_CFG_ITEM_48_4) {
					BT_APP_PROCESS(rtk_bt_le_audio_get_prefer_codec_cfg(idx, &codec_cfg));
					BT_LOGA("%s prefer_codec_cfg idx is %d\r\n", __func__, idx);
					break;
				}
			}
			sup_snk_mask >>= 1;
			idx++;
#else
			if (sup_snk_mask & 0x01) {
				if (rtk_bt_le_audio_get_prefer_codec_cfg(idx, &codec_cfg)) {
					BT_LOGE("%s rtk_bt_le_audio_get_prefer_codec_cfg fail idx 0x%x \r\n", __func__, idx);
					return RTK_BT_FAIL;
				}
				BT_LOGA("%s prefer_codec_cfg idx is %d\r\n", __func__, idx);
				break;
			}
			sup_snk_mask >>= 1;
			idx++;
#endif
		}
	} else {
		return RTK_BT_ERR_UNSUPPORTED;
	}
	if (idx >= RTK_BT_LE_CODEC_CFG_ITEM_LC3_MAX) {
		BT_LOGE("%s prefer_codec_cfg idx (%d) is unsupport\r\n", __func__, idx);
		return RTK_BT_ERR_UNSUPPORTED;
	} else {
		BT_LOGA("%s codec cfg idx %d, type_exist 0x%x, frame_duration %d, sample_frequency %d, codec_frame_blocks_per_sdu %d, octets_per_codec_frame=%d, audio_channel_allocation=0x%x)\r\n",
				__func__,
				idx, codec_cfg.type_exist, codec_cfg.frame_duration, codec_cfg.sample_frequency,
				codec_cfg.codec_frame_blocks_per_sdu, codec_cfg.octets_per_codec_frame,
				(unsigned int)codec_cfg.audio_channel_allocation);
	}
	memcpy(p_codec_cfg, &codec_cfg, sizeof(rtk_bt_le_audio_cfg_codec_t));
	bap_unicast_client_cfg.cfg_codec_index = idx;// used for unicast session cfg

	if ((p_codec_cfg->type_exist & RTK_BT_LE_CODEC_CFG_SAMPLING_FREQUENCY_EXIST) == 0) {
		BT_LOGA("%s sample_frequency is exist, use default 16k\r\n", __func__);
		p_codec_cfg->sample_frequency = RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K;
		p_codec_cfg->type_exist |= RTK_BT_LE_CODEC_CFG_SAMPLING_FREQUENCY_EXIST;
	}
	if ((p_codec_cfg->type_exist & RTK_BT_LE_CODEC_CFG_FRAME_DURATION_EXIST) == 0) {
		BT_LOGA("%s frame_duration is exist, use default 10ms\r\n", __func__);
		p_codec_cfg->frame_duration = RTK_BT_LE_FRAME_DURATION_CFG_10_MS;
		p_codec_cfg->type_exist |= RTK_BT_LE_CODEC_CFG_FRAME_DURATION_EXIST;
	}
	if ((p_codec_cfg->type_exist & RTK_BT_LE_CODEC_CFG_OCTET_PER_CODEC_FRAME_EXIST) == 0) {
		BT_LOGA("%s codec_frame_blocks_per_sdu is exist, use default 40\r\n", __func__);
		p_codec_cfg->octets_per_codec_frame = 40;
		p_codec_cfg->type_exist |= RTK_BT_LE_CODEC_CFG_OCTET_PER_CODEC_FRAME_EXIST;
	}
	if ((p_codec_cfg->type_exist & RTK_BT_LE_CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST) == 0) {
		BT_LOGA("%s codec_frame_blocks_per_sdu is exist, use default 1\r\n", __func__);
		p_codec_cfg->codec_frame_blocks_per_sdu = 1;
		p_codec_cfg->type_exist |= RTK_BT_LE_CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST;
	}

	return RTK_BT_OK;
}

static uint16_t rtk_bt_bap_unicast_client_qos_cfg(rtk_bt_le_audio_group_handle_t group_handle)
{
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	rtk_bt_le_audio_qos_cfg_preferred_t *p_preferred_qos = &bap_unicast_client_cfg.prefer_qos;

	p_group_info = app_bt_le_audio_group_list_find(group_handle);
	if (p_group_info == NULL) {
		BT_LOGE("%s: p_group_info is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	}

	if (rtk_bt_le_audio_qos_preferred_cfg_get(bap_unicast_client_cfg.cfg_codec_index, bap_unicast_client_cfg.cfg_qos_type, p_preferred_qos)) {
		BT_LOGE("%s rtk_bt_le_audio_qos_preferred_cfg_get fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	app_lea_default_unicast_session_qos.framing = p_preferred_qos->framing;
	app_lea_default_unicast_session_qos.sdu_interval_m_s = p_preferred_qos->sdu_interval;
	app_lea_default_unicast_session_qos.sdu_interval_s_m = p_preferred_qos->sdu_interval;
	app_lea_default_unicast_session_qos.latency_m_s = p_preferred_qos->max_transport_latency;
	app_lea_default_unicast_session_qos.latency_s_m = p_preferred_qos->max_transport_latency;
	app_lea_default_unicast_session_qos.sink_presentation_delay = p_preferred_qos->presentation_delay;
	app_lea_default_unicast_session_qos.source_presentation_delay = p_preferred_qos->presentation_delay;
	BT_LOGA("%s: sdu_interval:%d, framing:0x%x, max_sdu:%d, rtn:%d, max_transport_latency:%d, presentation_delay:%d\r\n",
			__func__, p_preferred_qos->sdu_interval, p_preferred_qos->framing,
			p_preferred_qos->max_sdu, p_preferred_qos->retransmission_number,
			p_preferred_qos->max_transport_latency, p_preferred_qos->presentation_delay);
	// will be direct called in RTK_BT_LE_AUDIO_EVT_BAP_START_QOS_CFG_IND event
	memcpy(&p_group_info->session_qos, &app_lea_default_unicast_session_qos, sizeof(rtk_bt_le_audio_unicast_session_qos_t));

	return RTK_BT_OK;
}

static uint16_t rtk_bt_le_audio_unicast_client_cfg_ase_codec(rtk_bt_le_audio_group_handle_t group_handle)
{
	uint8_t i = 0, j = 0;
	rtk_bt_le_audio_unicast_session_info_t session_info = {0};
	rtk_bt_le_audio_group_device_info_t dev_info = {0};
	rtk_bt_le_audio_bap_pacs_info_t pacs_info = {0};
	uint32_t sink_channel_allocation = 0, source_channel_allocation = 0;
	uint32_t sink_used_location = 0, source_used_location = 0;
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	rtk_bt_le_audio_unicast_ase_codec_t *p_prefer_codec = NULL;

	p_group_info = app_bt_le_audio_group_list_find(group_handle);
	if (p_group_info == NULL) {
		BT_LOGE("%s p_group_info is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	}
	p_prefer_codec = &p_group_info->prefer_codec;
	if (rtk_bt_le_audio_unicast_get_session_info(p_group_info->stream_session_handle, &session_info)) {
		BT_LOGE("%s rtk_bt_le_audio_unicast_get_session_info fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	BT_LOGA("%s: stream_session_handle %08x state=%d cfg_type=%d conn_dev_num=%d dev_num=%d\r\n", __func__,
			p_group_info->stream_session_handle, session_info.state, session_info.cfg_type, session_info.conn_dev_num, session_info.dev_num);
	for (i = 0; i < session_info.dev_num; i++) {
		BT_LOGA("%s i=%d,dev_handle=%08x,conn_state=%d,ase_num=%d\r\n", __func__, i,
				session_info.dev_info[i].dev_handle, session_info.dev_info[i].conn_state, session_info.dev_info[i].ase_num);
		if (rtk_bt_le_audio_group_get_device_info(group_handle,
												  session_info.dev_info[i].dev_handle,
												  &dev_info)) {
			BT_LOGE("%s rtk_bt_le_audio_group_get_device_info fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
		if (dev_info.conn_state != RTK_BT_LE_AUDIO_GAP_CONN_STATE_CONNECTED) {
			BT_LOGE("%s skip cfg_ase_codec when not connect \r\n", __func__);
			continue;
		}
		if (rtk_bt_le_audio_pacs_get_info(dev_info.conn_handle, &pacs_info)) {
			BT_LOGE("%s rtk_bt_le_audio_pacs_get_info fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
		for (j = 0; j < session_info.dev_info[i].ase_num; j++) {
			BT_LOGA("%s j=%d, ase_id=%d,cfg_idx=%d,ase_state=%d,direction=%d,channel_num=%d\r\n", __func__, j,
					session_info.dev_info[i].ase_info[j].ase_id, session_info.dev_info[i].ase_info[j].cfg_idx,
					session_info.dev_info[i].ase_info[j].ase_state, session_info.dev_info[i].ase_info[j].direction, session_info.dev_info[i].ase_info[j].channel_num);
			if (session_info.dev_info[i].ase_info[j].direction == RTK_BLE_AUDIO_SINK) {
				BT_LOGA("%s snk_audio_loc=0x%x\r\n", __func__, (unsigned int)pacs_info.snk_audio_loc);
				sink_channel_allocation = app_bt_le_audio_get_channel_location(pacs_info.snk_audio_loc, session_info.dev_info[i].ase_info[j].channel_num, sink_used_location);
				sink_used_location |= sink_channel_allocation;
				p_prefer_codec->codec_cfg.audio_channel_allocation = sink_channel_allocation;
				p_prefer_codec->codec_cfg.type_exist |= RTK_BT_LE_CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST;
			} else if (session_info.dev_info[i].ase_info[j].direction == RTK_BLE_AUDIO_SOURCE) {
				BT_LOGA("%s src_audio_loc=0x%x\r\n", __func__, (unsigned int)pacs_info.src_audio_loc);
				source_channel_allocation = app_bt_le_audio_get_channel_location(pacs_info.src_audio_loc, session_info.dev_info[i].ase_info[j].channel_num,
																				 source_used_location);
				source_used_location |= source_channel_allocation;
				p_prefer_codec->codec_cfg.audio_channel_allocation = source_channel_allocation;
				p_prefer_codec->codec_cfg.type_exist |= RTK_BT_LE_CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST;
#if defined(LEA_SOURCE_ASE_PREFER_CODEC_CONFIG) && LEA_SOURCE_ASE_PREFER_CODEC_CONFIG
				app_lea_source_ase_prefer_codec.codec_cfg.audio_channel_allocation = source_channel_allocation;
				app_lea_source_ase_prefer_codec.codec_cfg.type_exist |= RTK_BT_LE_CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST;
				BT_LOGA("%s: source ase prefer codec: audio_channel_allocation=0x%x\r\n", __func__,
						(unsigned int)app_lea_source_ase_prefer_codec.codec_cfg.audio_channel_allocation);
				BT_LOGA("%s: source ase prefer codec: type_exist 0x%x, frame_duration %d, sample_frequency %d, codec_frame_blocks_per_sdu %d,octets_per_codec_frame=%d)\r\n",
						__func__,
						app_lea_source_ase_prefer_codec.codec_cfg.type_exist, app_lea_source_ase_prefer_codec.codec_cfg.frame_duration,
						app_lea_source_ase_prefer_codec.codec_cfg.sample_frequency,
						app_lea_source_ase_prefer_codec.codec_cfg.codec_frame_blocks_per_sdu, app_lea_source_ase_prefer_codec.codec_cfg.octets_per_codec_frame);
#endif
			} else {
				BT_LOGE("%s skip cfg_ase_codec direction is wrong \r\n", __func__);
				continue;
			}
			BT_LOGA("%s unicast_cfg_ase_codec audio_channel_allocation=0x%x\r\n", __func__, (unsigned int)p_prefer_codec->codec_cfg.audio_channel_allocation);
			BT_LOGA("%s type_exist 0x%x, frame_duration %d, sample_frequency %d, codec_frame_blocks_per_sdu %d,octets_per_codec_frame=%d)\r\n", __func__,
					p_prefer_codec->codec_cfg.type_exist, p_prefer_codec->codec_cfg.frame_duration, p_prefer_codec->codec_cfg.sample_frequency,
					p_prefer_codec->codec_cfg.codec_frame_blocks_per_sdu, p_prefer_codec->codec_cfg.octets_per_codec_frame);
			if (session_info.dev_info[i].ase_info[j].direction == RTK_BLE_AUDIO_SINK) {
				if (rtk_bt_le_audio_unicast_cfg_ase_codec(p_group_info->stream_session_handle, session_info.dev_info[i].dev_handle,
														  session_info.dev_info[i].ase_info[j].cfg_idx, *p_prefer_codec)) {
					BT_LOGE("%s rtk_bt_le_audio_unicast_cfg_ase_codec fail \r\n", __func__);
					return RTK_BT_FAIL;
				}
			} else if (session_info.dev_info[i].ase_info[j].direction == RTK_BLE_AUDIO_SOURCE) {
#if defined(LEA_SOURCE_ASE_PREFER_CODEC_CONFIG) && LEA_SOURCE_ASE_PREFER_CODEC_CONFIG
				if (rtk_bt_le_audio_unicast_cfg_ase_codec(p_group_info->stream_session_handle, session_info.dev_info[i].dev_handle,
														  session_info.dev_info[i].ase_info[j].cfg_idx, app_lea_source_ase_prefer_codec)) {
					BT_LOGE("%s rtk_bt_le_audio_unicast_cfg_ase_codec fail \r\n", __func__);
					return RTK_BT_FAIL;
				}
#else
				if (rtk_bt_le_audio_unicast_cfg_ase_codec(p_group_info->stream_session_handle, session_info.dev_info[i].dev_handle,
														  session_info.dev_info[i].ase_info[j].cfg_idx, *p_prefer_codec)) {
					BT_LOGE("%s rtk_bt_le_audio_unicast_cfg_ase_codec fail \r\n", __func__);
					return RTK_BT_FAIL;
				}
#endif
			}
		}
	}

	return RTK_BT_OK;
}

uint16_t rtk_bt_bap_unicast_client_start(uint8_t group_idx, rtk_bt_le_audio_play_mode_t play_mode)
{
	uint8_t i = 0, j = 0, group_num = 0;
	rtk_bt_le_audio_group_handle_t group_handle = NULL;
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	uint8_t device_handle_tbl_len = 0;
	rtk_bt_le_audio_device_handle_t *p_device_handle_tbl = NULL;

	group_num = app_bt_le_audio_group_list_get_num();
	if (!group_num) {
		BT_LOGE("%s: group num is %d, maybe discovery not start or not done\r\n", __func__, group_num);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (group_idx >= group_num) {
		BT_LOGE("%s: group_idx %d must less than group num %d\r\n", __func__, group_idx, group_num);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	group_handle = app_bt_le_audio_group_list_find_by_group_idx(group_idx);
	if (!group_handle) {
		BT_LOGE("%s: group_handle is NULL\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	BT_LOGA("%s: start, group_idx=%d,group_num=%d,play_mode=%d\r\n", __func__, group_idx, group_num, play_mode);
	p_group_info = app_bt_le_audio_group_list_find(group_handle);
	if (p_group_info == NULL) {
		BT_LOGE("%s: p_group_info is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	}
	p_group_info->play_mode = play_mode;
	/* allocate stream session */
	if (!p_group_info->stream_session_handle) {
		if (rtk_bt_le_audio_stream_session_allocate(p_group_info->group_handle, &p_group_info->stream_session_handle)) {
			BT_LOGE("%s: rtk_bt_le_audio_stream_session_allocate fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
		if (!p_group_info->stream_session_handle) {
			BT_LOGE("%s: allocate stream_session_handle fail\r\n", __func__);
			return RTK_BT_FAIL;
		}
	} else {
		BT_LOGA("%s: stream_session_handle:%x already exist \r\n", __func__, p_group_info->stream_session_handle);
	}
	device_handle_tbl_len = p_group_info->dev_num * sizeof(rtk_bt_le_audio_device_handle_t);
	p_device_handle_tbl = (rtk_bt_le_audio_device_handle_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, device_handle_tbl_len);
	if (!p_device_handle_tbl) {
		BT_LOGE("%s osif_mem_alloc (len=%d) fail\r\n", __func__, device_handle_tbl_len);
		return RTK_BT_FAIL;
	}
	memset(p_device_handle_tbl, 0, device_handle_tbl_len);
	for (i = 0; i < p_group_info->dev_num; i++) {
		p_device_info = app_bt_le_audio_group_list_find_dev_by_idx(p_group_info->group_handle, i);
		if (p_device_info == NULL) {
			BT_LOGE("%s p_device_info is NULL\r\n", __func__);
			continue;
		}
		p_device_handle_tbl[j++] = p_device_info->device_handle;
	}
	/* select cfg type ,must call before rtk_bt_bap_unicast_client_codec_cfg */
	if (rtk_bt_bap_unicast_client_audio_config(p_group_info->group_handle, p_device_handle_tbl)) {
		BT_LOGE("%s rtk_bt_bap_unicast_client_audio_config fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	/* select prefer codec cfg,the return p_prefer_codec->codec_cfg.audio_channel_allocation is 0 ,set it in rtk_bt_le_audio_unicast_client_cfg_ase_codec */
	if (rtk_bt_bap_unicast_client_codec_cfg(p_group_info->group_handle)) {
		BT_LOGE("%s rtk_bt_bap_unicast_client_codec_cfg fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	/* config unicast session prefer qos*/
	if (rtk_bt_bap_unicast_client_qos_cfg(p_group_info->group_handle)) {
		BT_LOGE("%s: rtk_bt_bap_unicast_client_qos_cfg fail \r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_IDLE || \
		p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_IDLE_CONFIGURED) {
		/* unicast config */
		if (rtk_bt_le_audio_unicast_config(p_group_info->stream_session_handle, p_group_info->audio_cfg_type,
										   p_group_info->dev_num, p_device_handle_tbl)) {
			BT_LOGE("%s rtk_bt_le_audio_unicast_config fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
		/* get session info and config ase codec */
		if (rtk_bt_le_audio_unicast_client_cfg_ase_codec(p_group_info->group_handle)) {
			BT_LOGE("%s rtk_bt_le_audio_unicast_client_cfg_ase_codec fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
		if (rtk_bt_le_audio_unicast_start(p_group_info->stream_session_handle)) {
			BT_LOGE("%s rtk_bt_le_audio_unicast_start fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
	} else if (p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_CONFIGURED) {
		if (rtk_bt_le_audio_unicast_start(p_group_info->stream_session_handle)) {
			BT_LOGE("%s rtk_bt_le_audio_unicast_start fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
	} else {
		BT_LOGE("%s: no need to start unicast stream\r\n", __func__);
		if (p_device_handle_tbl) {
			osif_mem_free(p_device_handle_tbl);
			p_device_handle_tbl = NULL;
		}
		return RTK_BT_FAIL;
	}

	if (p_device_handle_tbl) {
		osif_mem_free(p_device_handle_tbl);
		p_device_handle_tbl = NULL;
	}

	return RTK_BT_OK;
}

uint16_t rtk_bt_bap_unicast_client_stop(uint8_t group_idx)
{
	rtk_bt_le_audio_group_handle_t group_handle = NULL;
	app_bt_le_audio_group_info_t *p_group_info = NULL;

	group_handle = app_bt_le_audio_group_list_find_by_group_idx(group_idx);
	if (!group_handle) {
		BT_LOGE("%s: group_handle is NULL\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_group_info = app_bt_le_audio_group_list_find(group_handle);
	if (!p_group_info) {
		BT_LOGE("%s: p_group_info is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_STREAMING ||
		p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_PARTIAL_STREAMING ||
		p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_STARTING) {
		BT_LOGE("%s: stop unicast stream\r\n ", __func__);
		if (rtk_bt_le_audio_unicast_stop(p_group_info->stream_session_handle, 1000)) {
			BT_LOGE("%s: rtk_bt_le_audio_unicast_stop fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
	} else if (p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_STOPPING) {
		BT_LOGE("%s: has already stopped \r\n", __func__);
	}

	return RTK_BT_OK;
}

uint16_t rtk_bt_bap_unicast_client_release(uint8_t group_idx)
{
	rtk_bt_le_audio_group_handle_t group_handle = NULL;
	app_bt_le_audio_group_info_t *p_group_info = NULL;

	group_handle = app_bt_le_audio_group_list_find_by_group_idx(group_idx);
	if (!group_handle) {
		BT_LOGE("%s: group_handle is NULL\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_group_info = app_bt_le_audio_group_list_find(group_handle);
	if (!p_group_info) {
		BT_LOGE("%s: p_group_info is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_CONFIGURED ||
		p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_STARTING ||
		p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_STREAMING ||
		p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_PARTIAL_STREAMING ||
		p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_STOPPING) {
		if (rtk_bt_le_audio_unicast_release(p_group_info->stream_session_handle)) {
			BT_LOGE("%s: rtk_bt_le_audio_unicast_release fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
		BT_LOGA("%s: release successfully\r\n", __func__);
	} else {
		BT_LOGA("%s: inappropriate bap state %d \r\n", __func__, p_group_info->bap_state);
		return RTK_BT_FAIL;
	}

	return RTK_BT_OK;
}

uint16_t rtk_bt_bap_unicast_client_stream_session_release(uint8_t group_idx)
{
	rtk_bt_le_audio_group_handle_t group_handle = NULL;
	app_bt_le_audio_group_info_t *p_group_info = NULL;

	group_handle = app_bt_le_audio_group_list_find_by_group_idx(group_idx);
	if (!group_handle) {
		BT_LOGE("%s: group_handle is NULL\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_group_info = app_bt_le_audio_group_list_find(group_handle);
	if (!p_group_info) {
		BT_LOGE("%s: p_group_info is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_IDLE || \
		p_group_info->bap_state == RTK_BT_LE_AUDIO_STREAM_STATE_IDLE_CONFIGURED) {
		/* release stream_session*/
		if (rtk_bt_le_audio_stream_session_release(p_group_info->stream_session_handle)) {
			BT_LOGE("%s: rtk_bt_le_audio_stream_session_release fail \r\n", __func__);
			return RTK_BT_FAIL;
		}
		p_group_info->stream_session_handle = NULL;
		BT_LOGA("%s: remove stream session ok\r\n", __func__);
	} else {
		BT_LOGA("%s: inappropriate bap state %d \r\n", __func__, p_group_info->bap_state);
		return RTK_BT_FAIL;
	}

	return RTK_BT_OK;
}

#if defined(CONFIG_BT_BAP_SUPPORT) && CONFIG_BT_BAP_SUPPORT
/* bap unicast server */
uint16_t rtk_bt_bap_unicast_server_cfg(uint8_t channel)
{
	char ch = 0;
	uint8_t device_name_len = 0;
	/* Check bt enable status (rtk_bt_bap_unicast_server_cfg should only be invoked before RTK_BT_IS_ENABLED()) */
	if (rtk_bt_is_enable()) {
		BT_LOGE("%s: should not be invoked after bt enable \r\n", __func__);
		return RTK_BT_FAIL;
	}

	if (channel < 1 || channel > 3) {
		BT_LOGE("%s: channel should be within 1 - 3 \r\n", __func__);
		return RTK_BT_FAIL;
	}
	device_name_len = strlen((char *)g_uni_ser_info.device_name);
	if (device_name_len > RTK_BT_GAP_DEVICE_NAME_LEN) {
		BT_LOGE("%s: device_name_len (%d) > RTK_BT_GAP_DEVICE_NAME_LEN(%d)\r\n", __func__, device_name_len, RTK_BT_GAP_DEVICE_NAME_LEN);
		return RTK_BT_FAIL;
	}
	switch (channel) {
	case RTK_BT_LE_AUDIO_LEFT:
		ch = 'L';
		break;
	case RTK_BT_LE_AUDIO_RIGHT:
		ch = 'R';
		break;
	case RTK_BT_LE_AUDIO_STEREO:
		ch = 'S';
		break;
	default:
		BT_LOGE("%s: unknown channel allocation \r\n", __func__);
		break;
	}
	g_uni_ser_info.device_name[device_name_len - 1] = ch;
	g_uni_ser_info.sound_channel = channel;

	return RTK_BT_OK;
}
#endif
#endif
