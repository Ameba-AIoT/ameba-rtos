/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <bt_api_config.h>
#if defined(CONFIG_BT_PBP_SUPPORT) && CONFIG_BT_PBP_SUPPORT
#include <osif.h>
#include <rtk_bt_pbp.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_bt_gap.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_le_audio.h>
#include <app_bt_le_audio_common.h>
#include <bt_audio_intf.h>
#include <bt_audio_codec_wrapper.h>
#include <bt_utils.h>

/***************************************common resources******************************************/
/* Define PBP broadcast name length*/
#define RTK_LE_AUDIO_PBP_BROADCAST_NAME_LEN_MAX     32
#define RTK_LE_AUDIO_PBP_BROADCAST_NAME_LEN_MIN     4

static uint8_t pbp_role;
static bool pbp_demo_init_flag = false;

typedef enum {
	RTK_BT_LE_AUDIO_PBP_ROLE_UNKNOWN = 0,
	RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SOURCE = 0x01,       /**< PBP Broadcast Source role. */
	RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SINK = 0x02,         /**< PBP Broadcast Sink role. */
	RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT = 0x04,    /**< PBP Broadcast Assistant role. */
} app_bt_le_audio_pbp_role_t;

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
static uint8_t app_lea_mcs_service_id = 0;
#endif
/***************************************common resources end**************************************/

/*************************************pbp broadcast source parameter******************************/
#define PBP_BROADCAST_SOURCE_ENCODE_TASK_PRIO                5
#define PBP_BROADCAST_SOURCE_ENCODE_TASK_STACK_SIZE          (1024 * 5)
static app_bt_bap_broadcast_source_info_t g_pbp_bsrc_info = {
	.device_name = "PBP Broadcast Source",
	.lea_app_conf = {0},
	.p_bap_bro_sour_info = &app_bap_bro_sour_info,
	.status = RTK_BLE_AUDIO_BROADCAST_SOURCE_DISABLE
};
/* pbp broadcast source task*/
struct app_lea_pbp_task_t {
	void *hdl;
	void *sem;
	uint8_t run;
};
static struct app_lea_pbp_task_t g_pbp_bsrc_encode_task = {
	.hdl = NULL,
	.sem = NULL,
	.run = 0
};
static bool g_pbp_brc_task_enable = false;
static void *g_pbp_brc_encode_data_sem = NULL;

#define RTK_BLE_AUDIO_USE_HW_GTIMER 1
#if defined(RTK_BLE_AUDIO_USE_HW_GTIMER) && RTK_BLE_AUDIO_USE_HW_GTIMER
#include "timer_api.h"
#define PBP_BRAODCAST_SOURCE_SEND_TIMER_ID                   TIMER13
static gtimer_t g_lea_pbp_bsrc_send_timer = {0};
#else
static void *g_lea_pbp_bsrc_send_timer = NULL;
#endif
static uint32_t g_pbp_bsrc_send_timer_interval_us = RTK_BLE_AUDIO_DEFAULT_SDU_INTERVAL_M_S_US;
/*********************************************pbp broadcast source parameter end*******************/

/*********************************************pbp broadcast sink parameter*************************/
static bool g_pbp_bsink_decode_enable = false;

app_bt_le_audio_broadcast_sink_info_t g_pbp_bsink_info = {
	.sound_channel = RTK_BT_LE_AUDIO_STEREO,
	.device_name = "PBP Broadcast Sink",
	.lea_app_conf = {0},
	.p_bap_bro_sink_info = &app_bap_bro_sink_info,
	.status = 0
};
static rtk_bt_le_audio_bis_info_t sync_bis_info = {0};
static uint8_t app_pbp_bsink_pac_sink_codec[] = {
	//Number_of_PAC_records
	2,
	//PAC Record
	RTK_BT_LE_LC3_CODEC_ID, 0, 0, 0, 0,//Codec_ID
	//Codec_Specific_Capabilities_Length
	16,
	//Codec_Specific_Capabilities
	0x03,
	RTK_BT_LE_CODEC_CAP_TYPE_SUPPORTED_SAMPLING_FREQUENCIES,
	(uint8_t)(RTK_BT_LE_SAMPLING_FREQUENCY_8K | RTK_BT_LE_SAMPLING_FREQUENCY_16K | RTK_BT_LE_SAMPLING_FREQUENCY_24K | RTK_BT_LE_SAMPLING_FREQUENCY_32K | RTK_BT_LE_SAMPLING_FREQUENCY_44_1K | RTK_BT_LE_SAMPLING_FREQUENCY_48K),
	(uint8_t)((RTK_BT_LE_SAMPLING_FREQUENCY_8K | RTK_BT_LE_SAMPLING_FREQUENCY_16K | RTK_BT_LE_SAMPLING_FREQUENCY_24K | RTK_BT_LE_SAMPLING_FREQUENCY_32K | RTK_BT_LE_SAMPLING_FREQUENCY_44_1K | RTK_BT_LE_SAMPLING_FREQUENCY_48K) >> 8),
	0x02,
	RTK_BT_LE_CODEC_CAP_TYPE_SUPPORTED_FRAME_DURATIONS,
	RTK_BT_LE_FRAME_DURATION_PREFER_10_MS_BIT | RTK_BT_LE_FRAME_DURATION_10_MS_BIT | RTK_BT_LE_FRAME_DURATION_PREFER_7_5_MS_BIT | RTK_BT_LE_FRAME_DURATION_7_5_MS_BIT,
	0x02,
	RTK_BT_LE_CODEC_CAP_TYPE_AUDIO_CHANNEL_COUNTS,
	RTK_BT_LE_AUDIO_CHANNEL_COUNTS_1 | RTK_BT_LE_AUDIO_CHANNEL_COUNTS_2,
	0x05,
	RTK_BT_LE_CODEC_CAP_TYPE_SUPPORTED_OCTETS_PER_CODEC_FRAME,
	0x1A, 0x00, 0x9B, 0x00, //0x1A(26):8_1; 0x9B(155):48_6;
	//Metadata_Length
	0x04,
	//Metadata
	0x03,
	RTK_BT_LE_METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS,
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_MEDIA),
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_MEDIA >> 8), //must fit dongle
	//PAC Record
	RTK_BT_LE_LC3_CODEC_ID, 0, 0, 0, 0,//Codec_ID
	//Codec_Specific_Capabilities_Length
	16,
	//Codec_Specific_Capabilities
	0x03,
	RTK_BT_LE_CODEC_CAP_TYPE_SUPPORTED_SAMPLING_FREQUENCIES,
	(uint8_t)(RTK_BT_LE_SAMPLING_FREQUENCY_8K | RTK_BT_LE_SAMPLING_FREQUENCY_16K | RTK_BT_LE_SAMPLING_FREQUENCY_24K | RTK_BT_LE_SAMPLING_FREQUENCY_32K | RTK_BT_LE_SAMPLING_FREQUENCY_44_1K | RTK_BT_LE_SAMPLING_FREQUENCY_48K),
	(uint8_t)((RTK_BT_LE_SAMPLING_FREQUENCY_8K | RTK_BT_LE_SAMPLING_FREQUENCY_16K | RTK_BT_LE_SAMPLING_FREQUENCY_24K | RTK_BT_LE_SAMPLING_FREQUENCY_32K | RTK_BT_LE_SAMPLING_FREQUENCY_44_1K | RTK_BT_LE_SAMPLING_FREQUENCY_48K) >> 8),
	0x02,
	RTK_BT_LE_CODEC_CAP_TYPE_SUPPORTED_FRAME_DURATIONS,
	RTK_BT_LE_FRAME_DURATION_PREFER_10_MS_BIT | RTK_BT_LE_FRAME_DURATION_10_MS_BIT | RTK_BT_LE_FRAME_DURATION_PREFER_7_5_MS_BIT | RTK_BT_LE_FRAME_DURATION_7_5_MS_BIT,
	0x02,
	RTK_BT_LE_CODEC_CAP_TYPE_AUDIO_CHANNEL_COUNTS,
	RTK_BT_LE_AUDIO_CHANNEL_COUNTS_1 | RTK_BT_LE_AUDIO_CHANNEL_COUNTS_2,
	0x05,
	RTK_BT_LE_CODEC_CAP_TYPE_SUPPORTED_OCTETS_PER_CODEC_FRAME,
	0x1A, 0x00, 0x9B, 0x00, //0x1A(26):8_1; 0x9B(155):48_6;
	//Metadata_Length
	0x04,
	//Metadata
	0x03,
	RTK_BT_LE_METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS,
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_CONVERSATIONAL),
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_CONVERSATIONAL >> 8)
};
static uint8_t app_pbp_bsink_pac_sink_codec_len = sizeof(app_pbp_bsink_pac_sink_codec) / sizeof(uint8_t);

/* Define PBP extended adv data default length */
#define LE_AUDIO_PBP_EXT_ADV_DATA_DEFAULT_LEN         3
static uint8_t pbp_bsink_adv_data[RTK_LE_AUDIO_ADV_LEN_MAX] = {
	0x02, //AD len
	RTK_BT_LE_GAP_ADTYPE_FLAGS, //AD types
	RTK_BT_LE_GAP_ADTYPE_FLAGS_GENERAL | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, //AD data
};

static uint8_t pbp_bsink_ext_adv_handle = 0xFF;
static rtk_bt_le_ext_adv_param_t pbp_bsink_def_ext_adv_param = {
	.adv_event_prop = RTK_BT_LE_EXT_ADV_EXTENDED_ADV_CONN_UNDIRECTED,
	.primary_adv_interval_min = RTK_BT_LE_AUDIO_PRIMARY_ADV_INTERVAL_MIN,
	.primary_adv_interval_max = RTK_BT_LE_AUDIO_PRIMARY_ADV_INTERVAL_MAX,
	.primary_adv_channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.own_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},
	.peer_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
	.tx_power = 0x7F,
	.primary_adv_phy = RTK_BT_LE_PHYS_PRIM_ADV_1M,
	.secondary_adv_max_skip = 0,
	.secondary_adv_phy = RTK_BT_LE_PHYS_2M,
	.adv_sid = 0,
};
/*********************************************pbp broadcast sink parameter end********************/

/*********************************************pbp broadcast assistant parameter*******************/
static app_bt_le_audio_broadcast_assistant_info_t g_pbp_bro_assi_info = {
	.device_name = "PBP Broadcast Assistant",
	.lea_app_conf = {0},
	.p_bap_bro_assi_info = &app_bap_bro_assi_info,
	.group_handle = NULL,
	.status = 0
};

static rtk_bt_le_scan_param_t pbp_bro_assi_def_scan_param = {
	.type          = RTK_BT_LE_SCAN_TYPE_ACTIVE,
	.interval      = 0x60,
	.window        = 0x30,
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.filter_policy = RTK_BT_LE_SCAN_FILTER_ALLOW_ALL,
	.duplicate_opt = 0,
};

/*********************************************pbp broadcast assistant end**************************/

/*********************************************pbp broadcast source function************************/
static void app_bt_le_audio_pbp_update_broadcast_audio_announcements(uint8_t *pbp_broadcast_name, app_bt_bap_broadcast_source_param_cfg_t *bap_bsrc_param_cfg,
																	 uint8_t *p_announcements_data, uint8_t *p_announcements_data_len)
{
	uint8_t idx = *p_announcements_data_len;
	uint8_t features_offset = 0;
	uint8_t feature = 0;
	uint8_t broadcast_name_len = strlen((const char *)pbp_broadcast_name);

	if (broadcast_name_len > RTK_LE_AUDIO_PBP_BROADCAST_NAME_LEN_MAX) {
		BT_LOGE("[APP] %s pbp broadcast name too long, name len = %d\r\n", __func__, broadcast_name_len);
		return ;
	} else if (broadcast_name_len < RTK_LE_AUDIO_PBP_BROADCAST_NAME_LEN_MIN) {
		BT_LOGE("[APP] %s pbp broadcast name too short, name len = %d\r\n", __func__, broadcast_name_len);
		return ;
	}
	p_announcements_data[idx] = 0x09; // pbp len
	idx++;
	p_announcements_data[idx] = RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA;
	idx++;
	p_announcements_data[idx] = LO_WORD(RTK_BT_LE_PUBIC_BROADCAST_ANNOUNCEMENT_SRV_UUID);
	idx++;
	p_announcements_data[idx] = HI_WORD(RTK_BT_LE_PUBIC_BROADCAST_ANNOUNCEMENT_SRV_UUID);
	idx++;
	features_offset = idx;
	p_announcements_data[idx] = 0x00; // Public Broadcast Announcement features
	idx++;
	p_announcements_data[idx] = 0x04; // Metadata length
	idx++;
	p_announcements_data[idx] = 0x03; // Metadata
	idx++;
	p_announcements_data[idx] = RTK_BT_LE_METADATA_TYPE_PROGRAM_INFO;
	idx++;
	p_announcements_data[idx] = 'T';
	idx++;
	p_announcements_data[idx] = 'V';
	idx++;
	p_announcements_data[idx] = broadcast_name_len + 1;
	idx++;
	p_announcements_data[idx] = RTK_BT_LE_GAP_ADTYPE_BROADCAST_NAME;
	idx++;
	memcpy(&p_announcements_data[idx], pbp_broadcast_name, broadcast_name_len);
	idx += broadcast_name_len;

	//config Public Broadcast Announcement features
	if (bap_bsrc_param_cfg->cfg_encryption == true) {
		feature |= RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_ENCRYPTED;
	}
	if (bap_bsrc_param_cfg->cfg_codec_index >= RTK_BT_LE_CODEC_CFG_ITEM_48_1) {
		feature |= RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_HIGH_QUALITY_PRESENT;
	} else {
		feature |= RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_STANDARD_QUALITY_PRESENT;
	}
	p_announcements_data[features_offset] = feature;

	if (idx > RTK_LE_AUDIO_BROADCAST_AUDIO_ANNOUNCEMENT_LEN_MAX) {
		BT_LOGE("[APP] %s broadcast audio_announcements length %d is too long\r\n", __func__, idx);
		return ;
	}
	*p_announcements_data_len = idx;
}

static uint16_t app_bt_le_audio_pbp_bsrc_setup_data_path(app_bt_le_audio_bap_broadcast_source_info_t *p_bsrc_info)
{
	uint8_t i = 0, bis_idx = 0;
	uint16_t ret = 0;
	rtk_bt_le_audio_cfg_codec_t *p_bis_codec_cfg = &p_bsrc_info->bis_codec_cfg;

	app_bt_le_audio_broadcast_source_get_codec_from_level2(bap_broadcast_source_codec_level2,
														   bap_broadcast_source_codec_level2_len,
														   p_bis_codec_cfg);
	BT_LOGA("[APP] %s: type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu 0x%x, octets_per_codec_frame 0x%x, audio_channel_allocation 0x%x\r\n",
			__func__,
			p_bis_codec_cfg->type_exist,
			p_bis_codec_cfg->frame_duration,
			p_bis_codec_cfg->sample_frequency,
			p_bis_codec_cfg->codec_frame_blocks_per_sdu,
			p_bis_codec_cfg->octets_per_codec_frame,
			(unsigned int)p_bis_codec_cfg->audio_channel_allocation);

	//set up iso data path
	rtk_bt_le_audio_bis_data_path_param_t setup_path_param = {
		{RTK_BT_LE_LC3_CODEC_ID, 0, 0, 0, 0},           /*codec_id */
		0x1122,                                         /*controller_delay */
		0,                                              /*codec_config_len */
		NULL,                                           /*codec_data */
	};
	for (i = 0; i < p_bsrc_info->bis_info.num_bis; i++) {
		bis_idx = p_bsrc_info->bis_info.bis_conn_info[i].bis_idx;
		ret = rtk_bt_le_audio_broadcast_source_setup_data_path(p_bsrc_info->broadcast_source_handle, bis_idx, &setup_path_param,
															   &p_bsrc_info->bis_info.bis_conn_info[i].bis_conn_handle);
		BT_LOGA("[APP] broadcast source setup data path %s for broadcast_source_handle 0x%08x bis_index %d,bis_conn_handle 0x%x, ret: 0x%x\r\n",
				((RTK_BT_OK != ret) ? "fail" : "ok"),
				p_bsrc_info->broadcast_source_handle,
				bis_idx,
				p_bsrc_info->bis_info.bis_conn_info[i].bis_conn_handle,
				ret);

		if (bis_idx == 1) {
#if (RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM == 1) && (RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE == 1)
			p_bis_codec_cfg->audio_channel_allocation = RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR;
#else
			p_bis_codec_cfg->audio_channel_allocation = RTK_BT_LE_AUDIO_LOCATION_FL;
#endif
		}
#if RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM == 2
		else if (bis_idx == 2) {
			p_bis_codec_cfg->audio_channel_allocation = RTK_BT_LE_AUDIO_LOCATION_FR;
		}
#endif
		else {
			BT_LOGE("[APP] %s unsupport bis_idx %d\r\n", __func__, bis_idx);
		}
		if (ret == RTK_BT_OK) {
			app_bt_le_audio_iso_data_path_add(RTK_BT_LE_AUDIO_BIS_MODE, bis_idx, RTK_BLE_AUDIO_ISO_DATA_PATH_TX,
											  p_bsrc_info->bis_info.bis_conn_info[i].bis_conn_handle, p_bis_codec_cfg);
		}
	}
	return ret;
}

static void app_bt_le_audio_pbp_bsrc_send_timer_handler(void *arg)
{
	(void)arg;
	uint8_t i = 0, tx_iso_data_path_num = 0;
	uint32_t sample_rate = 0;
	uint32_t frame_duration_us = 0;
	uint16_t frame_num = 0;
	tx_iso_data_path_num = app_bt_le_audio_iso_data_path_get_num(RTK_BLE_AUDIO_ISO_DATA_PATH_TX);
	app_lea_iso_data_path_t *p_iso_path = NULL;
	if (g_pbp_bsrc_encode_task.run) {
		for (i = 0 ; i < tx_iso_data_path_num; i++) {
			p_iso_path = app_bt_le_audio_iso_data_path_find_by_idx(i, RTK_BLE_AUDIO_ISO_DATA_PATH_TX);
			if (p_iso_path == NULL) {
				BT_LOGE("[APP] %s p_iso_path is NULL\r\n", __func__);
				continue;
			}
			p_iso_path->pkt_seq_num ++;
			sample_rate = app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(p_iso_path->codec.sample_frequency);
			frame_duration_us = (p_iso_path->codec.frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500;
			frame_num = p_iso_path->codec.codec_frame_blocks_per_sdu;
			p_iso_path->time_stamp += sample_rate * frame_duration_us * frame_num / 1000 / 1000;
		}
		if (g_pbp_brc_encode_data_sem) {
			osif_sem_give(g_pbp_brc_encode_data_sem);
		}
	}
}

static void app_bt_le_audio_pbp_bsrc_send_timer_init(void)
{
#if defined(RTK_BLE_AUDIO_USE_HW_GTIMER) && RTK_BLE_AUDIO_USE_HW_GTIMER
	BT_LOGD("[APP] %s hw timer id %d,time_interval_us = %d\r\n", __func__, PBP_BRAODCAST_SOURCE_SEND_TIMER_ID, (int)g_pbp_bsrc_send_timer_interval_us);
	if (g_lea_pbp_bsrc_send_timer.handler == NULL) {
		gtimer_init(&g_lea_pbp_bsrc_send_timer, PBP_BRAODCAST_SOURCE_SEND_TIMER_ID);
		gtimer_start_periodical(&g_lea_pbp_bsrc_send_timer, g_pbp_bsrc_send_timer_interval_us, (void *)app_bt_le_audio_pbp_bsrc_send_timer_handler, NULL);
	}
#else
	BT_LOGD("[APP] %s sw time_interval_us = %d\r\n", __func__, (int)g_pbp_bsrc_send_timer_interval_us);
	if (g_pbp_bsrc_send_timer_interval_us % 1000 != 0) {
		BT_LOGE("[APP] %s error: interval %d(us) cannot use sw timer, please use hw timer instead\r\n", __func__, (int)g_pbp_bsrc_send_timer_interval_us);
		return;
	}
	if (!g_lea_pbp_bsrc_send_timer) {
		if (false == osif_timer_create(&g_lea_pbp_bsrc_send_timer,
									   "le_audio_pbp_bsrc_send_timer",
									   1,
									   g_pbp_bsrc_send_timer_interval_us / 1000,
									   true,
									   app_bt_le_audio_pbp_bsrc_send_timer_handler)) {
			BT_LOGE("[APP] %s osif_timer_create fail\r\n", __func__);
			return;
		}
	}
	if (false == osif_timer_start(&g_lea_pbp_bsrc_send_timer)) {
		BT_LOGE("[APP] %s osif_timer_start fail\r\n", __func__);
		return;
	}
#endif
}

static void app_bt_le_audio_pbp_bsrc_send_timer_deinit(void)
{
#if defined(RTK_BLE_AUDIO_USE_HW_GTIMER) && RTK_BLE_AUDIO_USE_HW_GTIMER
	if (g_lea_pbp_bsrc_send_timer.handler) {
		gtimer_stop(&g_lea_pbp_bsrc_send_timer);
		gtimer_deinit(&g_lea_pbp_bsrc_send_timer);
		g_lea_pbp_bsrc_send_timer.handler = NULL;
	}
#else
	if (g_lea_pbp_bsrc_send_timer) {
		if (false == osif_timer_stop(&g_lea_pbp_bsrc_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_stop fail \r\n", __func__);
		}
		if (false == osif_timer_delete(&g_lea_pbp_bsrc_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_delete fail \r\n", __func__);
		}
		g_lea_pbp_bsrc_send_timer = NULL;
	}
#endif
}

static void app_bt_le_audio_iso_data_tx_statistics(app_lea_iso_data_path_t *p_iso_path)
{
	if (p_iso_path == NULL) {
		return;
	}

	if (p_iso_path->status_ok_cnt % 100 == 0) {
		BT_LOGE("[APP] iso_conn_handle 0x%x: tx ok cnt %d,tx retry cnt %d,tx fail cnt %d, FreeHeap %d\r\n", p_iso_path->iso_conn_handle,
				(int)p_iso_path->status_ok_cnt, (int)p_iso_path->status_retry_cnt, (int)p_iso_path->status_fail_cnt, osif_mem_peek(RAM_TYPE_DATA_ON));
	}
}

static void app_bt_le_audio_iso_data_rx_statistics(app_lea_iso_data_path_t *p_iso_path, uint16_t pkt_seq_num)
{
	if (p_iso_path == NULL) {
		return;
	}
	if (p_iso_path->pkt_seq_num != 0) {
		if (p_iso_path->pkt_seq_num + 1 != pkt_seq_num) {
			p_iso_path->status_lost_cnt++;
		}
	}
	p_iso_path->pkt_seq_num = pkt_seq_num;
	if (p_iso_path->status_ok_cnt % 100 == 0) {
		BT_LOGE("[APP] iso_conn_handle 0x%x,iso_data_rx_queue_num %d, rx_ok_cnt %d, rx_fail_cnt %d, rx_lost_cnt %d, FreeHeap %d\r\n",
				p_iso_path->iso_conn_handle, p_iso_path->iso_data_rx_queue_num, (int)p_iso_path->status_ok_cnt, (int)p_iso_path->status_fail_cnt,
				(int)p_iso_path->status_lost_cnt, osif_mem_peek(RAM_TYPE_DATA_ON));
	}
}

static uint16_t app_bt_le_audio_encode_data_send(app_lea_iso_data_path_t *p_iso_path, uint8_t *p_data, uint16_t data_len)
{
	uint16_t ret = RTK_BT_OK;
	rtk_bt_le_audio_iso_data_send_info_t send_info = {0};

	if (!p_iso_path || !p_data) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	send_info.p_data = p_data;
	send_info.data_len = data_len;
	send_info.iso_conn_handle = p_iso_path->iso_conn_handle;
	send_info.pkt_seq_num = p_iso_path->pkt_seq_num;
	send_info.ts_flag = true;
	if (send_info.ts_flag) {
		send_info.time_stamp = p_iso_path->time_stamp;
	}

	ret = rtk_bt_le_audio_iso_data_send(&send_info);
	if (ret == RTK_BT_OK) {
		BT_LOGD("[APP] %s ok, iso_conn_handle 0x%x, seq_num %d, available heap %d sys_time %d\r\n", __func__, p_iso_path->iso_conn_handle,
				p_iso_path->pkt_seq_num, osif_mem_peek(RAM_TYPE_DATA_ON), (int)osif_sys_time_get());
		BT_DUMPD("", p_data, data_len);
		p_iso_path->status_ok_cnt++;
	} else {
		BT_LOGE("[APP] %s failed, iso_conn_handle 0x%x, seq_num %d, ret 0x%x\r\n", __func__, p_iso_path->iso_conn_handle, p_iso_path->pkt_seq_num, ret);
		BT_DUMPD("", p_data, data_len);
		p_iso_path->status_fail_cnt++;
	}
	app_bt_le_audio_iso_data_tx_statistics(p_iso_path);

	return ret;
}
#if defined(RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT) && RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT
static uint16_t app_bt_le_audio_parse_two_chnl_pcm_data_to_one_chnl(short *in_buf, uint32_t pcm_in_len, short *out_buf, uint32_t pcm_out_len,
																	uint32_t audio_channel_allocation)
{
	uint8_t bytes_per_num = DEFAULT_PCM_BIT_WIDTH / 8;
	uint32_t p_read, p_write = 0;
	if (!in_buf || !out_buf) {
		BT_LOGE("[APP] %s: in_buf or out buf is NULL!! \r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (pcm_out_len < pcm_in_len / 2) {
		BT_LOGE("[APP] %s: pcm out length is too short!! \r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (audio_channel_allocation != RTK_BT_LE_AUDIO_LOCATION_FL && audio_channel_allocation != RTK_BT_LE_AUDIO_LOCATION_FR) {
		BT_LOGE("[APP] %s: wrong audio channel allocation !! \r\n", __func__);
		return RTK_BT_FAIL;
	}
	p_read = 0;
	p_write = 0;
	while (p_read < pcm_in_len && p_write < pcm_out_len) {
		//parse 2-channel pcm data ==> left channel
		if (audio_channel_allocation == RTK_BT_LE_AUDIO_LOCATION_FL) {
			if (p_read % 2 == 0) {
				memcpy(out_buf + p_write, in_buf + p_read, bytes_per_num);
				p_write++;
			}
			//parse 2-channel pcm data ==> right channel
		} else if (audio_channel_allocation == RTK_BT_LE_AUDIO_LOCATION_FR) {
			if (p_read % 2 == 1) {
				memcpy(out_buf + p_write, in_buf + p_read, bytes_per_num);
				p_write++;
			}
		} else {
			BT_LOGE("[APP] %s: wrong audio channel allocation !! \r\n", __func__);
		}
		p_read++;
	}
	return RTK_BT_OK;
}

extern short audio_demo[];
extern uint32_t audio_demo_size;
static uint16_t app_bt_le_audio_encode_birds_sing(app_lea_iso_data_path_t *p_iso_path, short *p_pcm_data, uint32_t pcm_total_num)
{
	uint8_t bytes_per_num = (DEFAULT_PCM_BIT_WIDTH / 8), pcm_channels = 0;
	uint32_t encode_byte = 0, encode_num = 0, pcm_num_offset = 0, pcm_frame_size = 0;
	uint8_t encode_channels = 0;
	uint32_t sample_rate = 0, frame_duration_us = 0;
	rtk_bt_le_audio_cfg_codec_t *p_codec = NULL;
	uint32_t parse_byte, parse_num = 0;
	uint32_t audio_channel_allocation = 0;
	struct enc_codec_buffer *penc_codec_buffer_t = NULL;
	uint16_t ret = RTK_BT_FAIL;

	if (!p_iso_path) {
		BT_LOGE("[APP] %s p_iso_path is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	} else {
		p_iso_path->is_processing = true;
		p_iso_path->iso_data_tx_queue_num++;
		p_iso_path->p_enc_codec_buffer_t = NULL;
		p_codec = &p_iso_path->codec;
	}
	if (!p_codec) {
		BT_LOGE("[APP] %s p_codec is NULL\r\n", __func__);
		goto exit;
	}
	//birds_sing is 16K 2 channel 16bit array
	pcm_channels = 2;
	sample_rate = app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(LEA_SOURCE_FIX_SAMPLE_FREQUENCY);
	frame_duration_us = (p_codec->frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500;
	pcm_frame_size = sample_rate * frame_duration_us / 1000 / 1000;
	encode_channels = app_bt_le_audio_get_lea_chnl_num(p_codec->audio_channel_allocation);
	encode_num = pcm_frame_size * encode_channels;
	encode_byte = encode_num * bytes_per_num;

	if (p_iso_path->p_encode_data == NULL) {
		p_iso_path->p_encode_data = (short *)osif_mem_alloc(RAM_TYPE_DATA_ON, encode_byte);
		if (p_iso_path->p_encode_data == NULL) {
			BT_LOGE("[APP] %s p_iso_path->p_encode_data alloc fail\r\n", __func__);
			goto exit;
		}
		memset(p_iso_path->p_encode_data, 0, encode_byte);
	}
	pcm_num_offset = p_iso_path->last_decode_offset;
	if (encode_channels == pcm_channels) {
		//if codec entity is 2 channel, no need to parse 2-channel bird sing to one channel
		if (pcm_num_offset + encode_num > pcm_total_num) {
			//pcm_total_num is not integral multiple of parse_num, padding 0 at the end
			memset(p_iso_path->p_encode_data, 0, encode_byte);
			memcpy(p_iso_path->p_encode_data, p_pcm_data + pcm_num_offset, (pcm_total_num - pcm_num_offset)*bytes_per_num);
		} else {
			memcpy(p_iso_path->p_encode_data, p_pcm_data + pcm_num_offset, encode_byte);
		}
		pcm_num_offset += encode_num;
	} else if (encode_channels == 1) {
		//if codec entity is one channel, need to parse 2-channel bird sing to one channel
		parse_num = pcm_frame_size * pcm_channels;
		parse_byte = parse_num * bytes_per_num;
		audio_channel_allocation = app_bt_le_audio_translate_le_chnl_to_audio_chnl(p_codec->audio_channel_allocation);
		if (pcm_num_offset + parse_num > pcm_total_num) {
			//pcm_total_num is not integral multiple of parse_num, padding 0 at the end
			app_bt_le_audio_parse_two_chnl_pcm_data_to_one_chnl(p_pcm_data + pcm_num_offset, (pcm_total_num - pcm_num_offset)*bytes_per_num / 2, p_iso_path->p_encode_data,
																encode_byte / 2, audio_channel_allocation);
		} else {
			app_bt_le_audio_parse_two_chnl_pcm_data_to_one_chnl(p_pcm_data + pcm_num_offset, parse_byte / 2, p_iso_path->p_encode_data, encode_byte / 2,
																audio_channel_allocation);
		}
		pcm_num_offset += parse_num;
	} else {
		BT_LOGE("[APP] %s: wrong encode channels number!\r\n", __func__);
		goto exit;
	}
	if (pcm_num_offset > pcm_total_num) {
		pcm_num_offset = 0;
	}

	p_iso_path->last_decode_offset = pcm_num_offset;
	p_iso_path->encode_byte = encode_byte;
	BT_LOGD("[APP] %s encode_num = %d,pcm_total_num=%d,pcm_num_offset= %d, last_decode_offset = %d,encode_byte = %d\r\n", __func__, (int)encode_num,
			(int)pcm_total_num, (int)pcm_num_offset, (int)p_iso_path->last_decode_offset, (int)p_iso_path->encode_byte);
	/* encode */
	penc_codec_buffer_t = rtk_bt_audio_data_encode(RTK_BT_AUDIO_CODEC_LC3, p_iso_path->codec_entity, p_iso_path->p_encode_data, p_iso_path->encode_byte);
	if (penc_codec_buffer_t == NULL) {
		BT_LOGE("[APP] %s rtk_bt_audio_data_encode fail\r\n", __func__);
		goto exit;
	}
	p_iso_path->p_enc_codec_buffer_t = penc_codec_buffer_t;
	ret = RTK_BT_OK;

exit:
	if (ret) {
		p_iso_path->iso_data_tx_queue_num --;
		p_iso_path->is_processing = false;
	}

	return ret;
}
#endif

static void app_bt_le_audio_pbp_encode_task_entry(void *ctx)
{
	(void)ctx;
	app_lea_iso_data_path_t *p_iso_path = NULL;
	uint8_t i = 0, tx_iso_data_path_num = 0;
#if defined(RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT) && RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT
	short *p_pcm_data = NULL;
	uint32_t pcm_total_num = 0;
	p_pcm_data = audio_demo;
	pcm_total_num = audio_demo_size * 2;
#endif
	g_pbp_bsrc_encode_task.run = 1;
	//give sem each 10ms in
	while (g_pbp_bsrc_encode_task.run) {
		if (g_pbp_brc_encode_data_sem) {
			osif_sem_take(g_pbp_brc_encode_data_sem, BT_TIMEOUT_FOREVER);
		}
		BT_LOGD("[APP] %s sys_time %d\r\n", __func__, (int)osif_sys_time_get());
		tx_iso_data_path_num = app_bt_le_audio_iso_data_path_get_num(RTK_BLE_AUDIO_ISO_DATA_PATH_TX);
		for (i = 0 ; i < tx_iso_data_path_num; i++) {
			p_iso_path = app_bt_le_audio_iso_data_path_find_by_idx(i, RTK_BLE_AUDIO_ISO_DATA_PATH_TX);
			if (p_iso_path == NULL) {
				BT_LOGE("[APP] %s p_iso_path is NULL\r\n", __func__);
				continue;
			}
			//skip removing iso path
			if (p_iso_path->is_removing) {
				BT_LOGD("[APP] %s p_iso_path(%08x) is removing\r\n", __func__, p_iso_path);
				continue;
			}
			//wait if encode task is fast and send task is slow
			if (p_iso_path->iso_data_tx_queue_num > 128) {
				BT_LOGD("[APP] %s encode is fast iso_data_tx_queue_num = %d, wait\r\n", __func__, p_iso_path->iso_data_tx_queue_num);
				continue;
			}
#if defined(RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT) && RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT
			if (RTK_BT_OK != app_bt_le_audio_encode_birds_sing(p_iso_path, p_pcm_data, pcm_total_num)) {
				BT_LOGE("[APP] %s app_bt_le_audio_encode_birds_sing fail\r\n", __func__);
				continue;
			}
#endif
		}
		/* send flow (seperate encode and send flow is for decreasing time offset between different iso path ,caused by encoding time cost) */
		for (i = 0 ; i < tx_iso_data_path_num; i++) {
			p_iso_path = app_bt_le_audio_iso_data_path_find_by_idx(i, RTK_BLE_AUDIO_ISO_DATA_PATH_TX);
			if (p_iso_path == NULL) {
				BT_LOGE("[APP] %s p_iso_path is NULL\r\n", __func__);
				continue;
			}
			/* send */
			if (p_iso_path->p_enc_codec_buffer_t) {
				app_bt_le_audio_encode_data_send(p_iso_path, p_iso_path->p_enc_codec_buffer_t->pbuffer, p_iso_path->p_enc_codec_buffer_t->frame_size);
#if defined(RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT) && RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT
				rtk_bt_audio_free_encode_buffer(RTK_BT_AUDIO_CODEC_LC3, p_iso_path->codec_entity, p_iso_path->p_enc_codec_buffer_t);
#endif
				p_iso_path->p_enc_codec_buffer_t = NULL;
				p_iso_path->iso_data_tx_queue_num --;
				p_iso_path->is_processing = false;
			}
		}
	}
	BT_LOGD("[APP] %s task_delete\r\n", __func__);
	osif_sem_give(g_pbp_bsrc_encode_task.sem);
	g_pbp_bsrc_encode_task.run = 0;
	g_pbp_bsrc_encode_task.hdl = NULL;
	osif_task_delete(NULL);
}

static void app_bt_le_audio_pbp_encode_data_control(bool enable)
{
	if (enable) {
		if (g_pbp_brc_task_enable == true) {
			BT_LOGE("[APP] %s: encode task is alreay enabled\r\n", __func__);
			return ;
		}

		g_pbp_brc_task_enable = true;

		if (g_pbp_brc_encode_data_sem == NULL) {
			osif_sem_create(&g_pbp_brc_encode_data_sem, 0, 0XFFFFFFFF);
		}

		if (g_pbp_bsrc_encode_task.sem == NULL) {
			osif_sem_create(&g_pbp_bsrc_encode_task.sem, 0, 1);
		}

		if (g_pbp_bsrc_encode_task.hdl == NULL) {
			if (false == osif_task_create(&g_pbp_bsrc_encode_task.hdl, ((const char *)"pbp_bsrc_encode_task"), app_bt_le_audio_pbp_encode_task_entry,
										  NULL, PBP_BROADCAST_SOURCE_ENCODE_TASK_STACK_SIZE, PBP_BROADCAST_SOURCE_ENCODE_TASK_PRIO)) {
				BT_LOGE("[APP] %s xTaskCreate(lea_encode_task) failed\r\n", __func__);
				return ;
			}
		}
		app_bt_le_audio_pbp_bsrc_send_timer_init();
	} else {
		if (g_pbp_brc_task_enable == false) {
			BT_LOGE("[APP] %s: encode task is alreay disabled\r\n", __func__);
			return ;
		}
		g_pbp_brc_task_enable = false;
		app_bt_le_audio_pbp_bsrc_send_timer_deinit();
		if (g_pbp_bsrc_encode_task.hdl) {
			g_pbp_bsrc_encode_task.run = 0;
			osif_sem_give(g_pbp_brc_encode_data_sem);
			osif_sem_take(g_pbp_bsrc_encode_task.sem, BT_TIMEOUT_FOREVER);
			osif_sem_delete(g_pbp_bsrc_encode_task.sem);
			g_pbp_bsrc_encode_task.hdl = NULL;
			g_pbp_bsrc_encode_task.sem = NULL;
		}
		if (g_pbp_brc_encode_data_sem) {
			osif_sem_delete(g_pbp_brc_encode_data_sem);
			g_pbp_brc_encode_data_sem = NULL;
		}
	}
}

static uint16_t app_bt_le_audio_pbp_broadcast_source_init(void)
{
	/* intialize iso data path */
	app_bt_le_audio_iso_data_path_init();

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_pbp_broadcast_source_deinit(void)
{
	/* intialize iso data path */
	app_bt_le_audio_iso_data_path_deinit();

	return RTK_BT_OK;
}
/*********************************************pbp broadcast source function end**************************/
/*********************************************pbp broadcast sink function********************************/
void app_bt_le_audio_pbp_bsink_decode_data_control(bool enable)
{
	BT_LOGD("[APP] %s %d\r\n", __func__, enable);
	if (enable) {
		if (g_pbp_bsink_decode_enable == true) {
			BT_LOGE("[APP] %s: decode is alreay enabled\r\n", __func__);
			return;
		}
		g_pbp_bsink_decode_enable = true;
	} else {
		if (g_pbp_bsink_decode_enable == false) {
			BT_LOGE("[APP] %s: decode is alreay disabled\r\n", __func__);
			return;
		}
		g_pbp_bsink_decode_enable = false;
	}
}

static uint16_t app_bt_le_audio_pbp_bsink_setup_data_path(rtk_bt_le_audio_sync_handle_t sync_handle)
{
	uint8_t i = 0, bis_idx = 0;
	uint16_t ret = 0;
	uint16_t bis_conn_handle = 0;
	rtk_bt_le_audio_bis_info_t bis_info = {0};
	rtk_bt_le_audio_cfg_codec_t bis_codec_cfg = {0};
	rtk_bt_le_audio_bis_data_path_param_t param = {
		{RTK_BT_LE_LC3_CODEC_ID, 0, 0, 0, 0},           /*codec_id */
		0x1122,                                         /*controller_delay */
		0,                                              /*codec_config_len */
		NULL,                                           /*codec_data */
	};
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;

	if (!sync_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(sync_handle);
	if (!p_sync_dev_info) {
		BT_LOGE("[APP] %s not find sync dev info for sync_handle %08x\r\n", __func__, sync_handle);
		return RTK_BT_FAIL;
	}

	ret = rtk_bt_le_audio_sync_get_bis_info(sync_handle, &bis_info);
	if (ret != RTK_BT_OK) {
		BT_LOGE("[APP] rtk_bt_le_audio_sync_get_bis_info fail,ret = 0x%x\r\n", ret);
	}
	sync_bis_info = bis_info;
	//set up iso data path
	for (i = 0; i < bis_info.num_bis; i++) {
		bis_idx = bis_info.bis_conn_info[i].bis_idx;
		bis_conn_handle = bis_info.bis_conn_info[i].bis_conn_handle;
		ret = rtk_bt_le_audio_sync_get_bis_sync_codec_cfg(sync_handle, bis_idx, &bis_codec_cfg);
		if (ret != RTK_BT_OK) {
			BT_LOGE("[APP] rtk_bt_le_audio_sync_get_bis_sync_codec_cfg fail,ret = 0x%x\r\n", ret);
			continue;
		} else {
			BT_LOGD("[APP] rtk_bt_le_audio_sync_get_bis_sync_codec_cfg ok (sync_handle %08x, bis_idx %d,", sync_handle, bis_idx);
			BT_LOGA("[APP] type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu 0x%x, octets_per_codec_frame 0x%x, audio_channel_allocation 0x%x)\r\n",
					bis_codec_cfg.type_exist, bis_codec_cfg.frame_duration,
					bis_codec_cfg.sample_frequency, bis_codec_cfg.codec_frame_blocks_per_sdu,
					bis_codec_cfg.octets_per_codec_frame, (unsigned int)bis_codec_cfg.audio_channel_allocation);
			app_bt_le_audio_iso_data_path_add(RTK_BT_LE_AUDIO_BIS_MODE, bis_idx, RTK_BLE_AUDIO_ISO_DATA_PATH_RX, bis_conn_handle, &bis_codec_cfg);
		}
	}
	for (i = 0; i < bis_info.num_bis; i++) {
		bis_idx = bis_info.bis_conn_info[i].bis_idx;
		ret = rtk_bt_le_audio_sync_setup_data_path(sync_handle, bis_idx, &param);
		BT_LOGA("[APP] broadcast sink setup data path %s for sync_handle 0x%08x bis_index %d, ret: 0x%x\r\n",
				((RTK_BT_OK != ret) ? "fail" : "ok"), sync_handle, bis_idx, ret);
		if (ret != RTK_BT_OK) {
			BT_LOGE("[APP] rtk_bt_le_audio_sync_setup_data_path fail,bis_idx: 0x%x\r\n", bis_idx);
			continue;
		}
	}

	return ret;
}

static uint16_t app_bt_le_audio_pbp_bsink_remove_data_path(rtk_bt_le_audio_sync_handle_t sync_handle)
{
	uint8_t i = 0;
	uint16_t ret = 0;
	uint16_t bis_conn_handle = 0;
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	if (!sync_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(sync_handle);
	if (!p_sync_dev_info) {
		BT_LOGE("[APP] %s not find sync dev info for sync_handle %08x\r\n", __func__, sync_handle);
		return RTK_BT_FAIL;
	}
	//remove iso data path
	for (i = 0; i < sync_bis_info.num_bis; i++) {
		bis_conn_handle = sync_bis_info.bis_conn_info[i].bis_conn_handle;
		app_bt_le_audio_iso_data_path_remove(bis_conn_handle, RTK_BLE_AUDIO_ISO_DATA_PATH_RX);
	}
	return ret;
}

static uint16_t app_bt_le_audio_pbp_broadcast_sink_init(app_bt_le_audio_broadcast_sink_info_t *p_info)
{
	app_bt_le_audio_bap_broadcast_sink_info_t *p_bap_bro_sink_info = p_info->p_bap_bro_sink_info;

	if (p_bap_bro_sink_info->big_info_ind_sem == NULL) {
		osif_sem_create(&p_bap_bro_sink_info->big_info_ind_sem, 0, 0xFFFFFFFF);
	}
	/* intialize iso data path */
	app_bt_le_audio_iso_data_path_init();
	/* intialize sync device list */
	app_bt_le_audio_broadcast_dev_list_init();

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_pbp_broadcast_sink_deinit(app_bt_le_audio_broadcast_sink_info_t *p_info)
{
	app_bt_le_audio_bap_broadcast_sink_info_t *p_bap_bro_sink_info = p_info->p_bap_bro_sink_info;

	/* deintialize iso data path */
	app_bt_le_audio_iso_data_path_deinit();
	app_bt_le_audio_broadcast_dev_list_deinit();
	if (p_bap_bro_sink_info->big_info_ind_sem) {
		osif_sem_delete(p_bap_bro_sink_info->big_info_ind_sem);
		p_bap_bro_sink_info->big_info_ind_sem = NULL;
	}

	return RTK_BT_OK;
}
/*********************************************pbp broadcast sink function end**********************************************/

/*********************************************pbp broadcast assistant function*********************************************/
static uint16_t app_bt_le_audio_pbp_broadcast_assistant_init(app_bt_le_audio_broadcast_assistant_info_t *p_info)
{
	app_bt_le_audio_bap_broadcast_assistant_info_t *p_bap_bro_assi_info = p_info->p_bap_bro_assi_info;

	if (p_bap_bro_assi_info->pa_sync_ind_sem == NULL) {
		osif_sem_create(&p_bap_bro_assi_info->pa_sync_ind_sem, 0, 0xFFFFFFFF);
	}

	/* intialize scan, bscan and sync dev list */
	app_bt_le_audio_scan_dev_list_init();

	app_bt_le_audio_broadcast_dev_list_init();

	/* intialize group list */
	app_bt_le_audio_group_list_init();

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_pbp_broadcast_assistant_deinit(app_bt_le_audio_broadcast_assistant_info_t *p_info)
{
	app_bt_le_audio_bap_broadcast_assistant_info_t *p_bap_bro_assi_info = p_info->p_bap_bro_assi_info;

	if (p_bap_bro_assi_info->pa_sync_ind_sem) {
		osif_sem_delete(p_bap_bro_assi_info->pa_sync_ind_sem);
	}

	/* deintialize scan, bscan and sync dev list */
	app_bt_le_audio_scan_dev_list_deinit();

	app_bt_le_audio_broadcast_dev_list_deinit();

	/* deintialize group list */
	app_bt_le_audio_group_list_deinit();

	p_info->group_handle = NULL;

	return RTK_BT_OK;
}
/*********************************************pbp broadcast assistant function end*****************************************/
static rtk_bt_evt_cb_ret_t rtk_bt_le_audio_gap_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	char le_addr[30] = {0};
	char *role = NULL;

	switch (evt_code) {
	case RTK_BT_LE_GAP_EVT_ADV_START_IND: {
		rtk_bt_le_adv_start_ind_t *adv_start_ind = (rtk_bt_le_adv_start_ind_t *)param;
		if (!adv_start_ind->err) {
			BT_LOGA("[APP]ADV started: adv_type %d \r\n", adv_start_ind->adv_type);
		} else {
			BT_LOGE("[APP]ADV start failed, err 0x%x \r\n", adv_start_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:adv,start,%d,%d\r\n", (adv_start_ind->err == 0) ? 0 : -1, adv_start_ind->adv_type);
		break;
	}

	case RTK_BT_LE_GAP_EVT_ADV_STOP_IND: {
		rtk_bt_le_adv_stop_ind_t *adv_stop_ind = (rtk_bt_le_adv_stop_ind_t *)param;
		if (!adv_stop_ind->err) {
			BT_LOGA("[APP]ADV stopped: reason 0x%x \r\n", adv_stop_ind->stop_reason);
		} else {
			BT_LOGE("[APP]ADV stop failed, err 0x%x \r\n", adv_stop_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:adv,stop,%d,0x%x\r\n", (adv_stop_ind->err == 0) ? 0 : -1, adv_stop_ind->stop_reason);
		break;
	}

#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
	case RTK_BT_LE_GAP_EVT_EXT_ADV_IND: {
		rtk_bt_le_ext_adv_ind_t *ext_adv_ind = (rtk_bt_le_ext_adv_ind_t *)param;
		if (!ext_adv_ind->err) {
			if (ext_adv_ind->is_start) {
				BT_LOGA("[APP] Ext ADV(%d) started\r\n", ext_adv_ind->adv_handle);
			} else {
				BT_LOGA("[APP] Ext ADV(%d) stopped: reason 0x%x \r\n", ext_adv_ind->adv_handle, ext_adv_ind->stop_reason);
			}
		} else {
			if (ext_adv_ind->is_start) {
				BT_LOGE("[APP] Ext ADV(%d) started failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
			} else {
				BT_LOGE("[APP] Ext ADV(%d) stopped failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
			}
		}
		BT_AT_PRINT("+BLEGAP:eadv,%s,%d,%d\r\n",
					ext_adv_ind->is_start ? "start" : "stop",
					(ext_adv_ind->err == 0) ? 0 : -1,
					ext_adv_ind->adv_handle);
		break;
	}
#endif

	case RTK_BT_LE_GAP_EVT_SCAN_START_IND: {
		rtk_bt_le_scan_start_ind_t *scan_start_ind = (rtk_bt_le_scan_start_ind_t *)param;
		if (!scan_start_ind->err) {
			BT_LOGA("[APP] Scan started, scan_type: %d\r\n", scan_start_ind->scan_type);
		} else {
			BT_LOGE("[APP] Scan start failed(err: 0x%x)\r\n", scan_start_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:scan,start,%d,%d\r\n", (scan_start_ind->err == 0) ? 0 : -1, scan_start_ind->scan_type);
		break;
	}

	case RTK_BT_LE_GAP_EVT_SCAN_RES_IND: {
		rtk_bt_le_scan_res_ind_t *scan_res_ind = (rtk_bt_le_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->adv_report.addr), le_addr, sizeof(le_addr));
		BT_LOGA("[APP] Scan info, [Device]: %s, AD evt type: %d, RSSI: %i, len: %d \r\n",
				le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
				scan_res_ind->adv_report.len);
		BT_AT_PRINT("+BLEGAP:scan,info,%s,%d,%i,%d\r\n",
					le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
					scan_res_ind->adv_report.len);
		break;
	}

#if defined(RTK_BLE_5_0_AE_SCAN_SUPPORT) && RTK_BLE_5_0_AE_SCAN_SUPPORT
	case RTK_BT_LE_GAP_EVT_EXT_SCAN_RES_IND: {
		rtk_bt_le_ext_scan_res_ind_t *scan_res_ind = (rtk_bt_le_ext_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->addr), le_addr, sizeof(le_addr));
#if 0
		BT_LOGA("[APP] Ext Scan info, [Device]: %s, AD evt type: 0x%x, RSSI: %i, PHY: 0x%x, TxPower: %d, Len: %d\r\n",
				le_addr, scan_res_ind->evt_type, scan_res_ind->rssi,
				(scan_res_ind->primary_phy << 4) | scan_res_ind->secondary_phy,
				scan_res_ind->tx_power, scan_res_ind->len);
#endif
		BT_AT_PRINT("+BLEGAP:escan,%s,0x%x,%i,0x%x,%d,%d\r\n",
					le_addr, scan_res_ind->evt_type, scan_res_ind->rssi,
					(scan_res_ind->primary_phy << 4) | scan_res_ind->secondary_phy,
					scan_res_ind->tx_power, scan_res_ind->len);
		if (RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT == pbp_role) {
			app_bt_le_audio_scan_report_handle(scan_res_ind);
		}
		if (RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SINK == pbp_role ||
			RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT == pbp_role) {
			app_bt_le_audio_bass_scan_report_handle(scan_res_ind);
		}
		break;
	}
#endif

	case RTK_BT_LE_GAP_EVT_SCAN_STOP_IND: {
		rtk_bt_le_scan_stop_ind_t *scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)param;
		if (!scan_stop_ind->err) {
			BT_LOGA("[APP] Scan stopped, reason: 0x%x\r\n", scan_stop_ind->stop_reason);
			if (pbp_role & (RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT | RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SINK)) {
				app_bt_le_audio_bass_scan_report_show();
			}
		} else {
			BT_LOGE("[APP] Scan stop failed(err: 0x%x)\r\n", scan_stop_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:scan,stop,%d,0x%x\r\n", (scan_stop_ind->err == 0) ? 0 : -1, scan_stop_ind->stop_reason);
		break;
	}

	case RTK_BT_LE_GAP_EVT_CONNECT_IND: {
		rtk_bt_le_conn_ind_t *conn_ind = (rtk_bt_le_conn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(conn_ind->peer_addr), le_addr, sizeof(le_addr));
		if (!conn_ind->err) {
			role = conn_ind->role ? "slave" : "master";
			BT_LOGA("[APP] Connected, handle: %d, role: %s, remote device: %s\r\n",
					(int)conn_ind->conn_handle, role, le_addr);
			if (RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT == pbp_role) {
				app_bt_le_audio_broadcast_assistant_info_t *p_bro_assi_info = &g_pbp_bro_assi_info;
				p_bro_assi_info->status = RTK_BLE_AUDIO_BROADCAST_ASSISTANT_CONN;
				uint16_t ret = RTK_BT_OK;
				if ((ret = rtk_bt_le_sm_start_security(conn_ind->conn_handle)) != RTK_BT_OK) {
					BT_LOGE("[APP] GAP start security flow failed! err: 0x%x", ret);
					break;
				}
			}
		} else {
			BT_LOGE("[APP] Connection establish failed(err: 0x%x), remote device: %s\r\n", conn_ind->err, le_addr);
		}
		BT_AT_PRINT("+BLEGAP:conn,%d,%d,%s\r\n", (conn_ind->err == 0) ? 0 : -1, (int)conn_ind->conn_handle, le_addr);
		break;
	}

	case RTK_BT_LE_GAP_EVT_DISCONN_IND: {
		rtk_bt_le_disconn_ind_t *disconn_ind = (rtk_bt_le_disconn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(disconn_ind->peer_addr), le_addr, sizeof(le_addr));
		role = disconn_ind->role ? "slave" : "master";
		BT_LOGA("[APP] Disconnected, reason: 0x%x, handle: %d, role: %s, remote device: %s\r\n",
				disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		BT_AT_PRINT("+BLEGAP:disconn,0x%x,%d,%s,%s\r\n",
					disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		if (RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SINK == pbp_role) {
			rtk_bt_le_gap_start_ext_adv(pbp_bsink_ext_adv_handle, 0, 0);
		} else if (RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT == pbp_role) {
			app_bt_le_audio_device_list_remove(disconn_ind->conn_handle);
			g_pbp_bro_assi_info.status = RTK_BLE_AUDIO_BROADCAST_ASSISTANT_DISC;
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND: {
		rtk_bt_le_conn_update_ind_t *conn_update_ind = (rtk_bt_le_conn_update_ind_t *)param;
		if (conn_update_ind->err) {
			BT_LOGE("[APP] Update conn param failed, conn_handle: %d, err: 0x%x\r\n",
					conn_update_ind->conn_handle, conn_update_ind->err);
			BT_AT_PRINT("+BLEGAP:conn_update,%d,-1\r\n", conn_update_ind->conn_handle);
		} else {
			BT_LOGA("[APP] Conn param is updated, conn_handle: %d, conn_interval: 0x%x, "\
					"conn_latency: 0x%x, supervision_timeout: 0x%x\r\n",
					conn_update_ind->conn_handle,
					conn_update_ind->conn_interval,
					conn_update_ind->conn_latency,
					conn_update_ind->supv_timeout);
			BT_AT_PRINT("+BLEGAP:conn_update,%d,0,0x%x,0x%x,0x%x\r\n",
						conn_update_ind->conn_handle,
						conn_update_ind->conn_interval,
						conn_update_ind->conn_latency,
						conn_update_ind->supv_timeout);
		}

		break;
	}

	case RTK_BT_LE_GAP_EVT_REMOTE_CONN_UPDATE_REQ_IND: { //BT api shall not be called here
		rtk_bt_le_remote_conn_update_req_ind_t *rmt_update_req = (rtk_bt_le_remote_conn_update_req_ind_t *)param;
		BT_LOGA("[APP] Remote device request a change in conn param, conn_handle: %d, "\
				"conn_interval_max: 0x%x, conn_interval_min: 0x%x, conn_latency: 0x%x, "\
				"timeout: 0x%x. The host stack accept it.\r\n",
				rmt_update_req->conn_handle,
				rmt_update_req->conn_interval_max,
				rmt_update_req->conn_interval_min,
				rmt_update_req->conn_latency,
				rmt_update_req->supv_timeout);
		return RTK_BT_EVT_CB_ACCEPT;
		break;
	}

	case RTK_BT_LE_GAP_EVT_DATA_LEN_CHANGE_IND: {
		rtk_bt_le_data_len_change_ind_t *data_len_change = (rtk_bt_le_data_len_change_ind_t *)param;
		BT_LOGA("[APP] Data len is updated, conn_handle: %d, "       \
				"max_tx_octets: 0x%x, max_tx_time: 0x%x, "        \
				"max_rx_octets: 0x%x, max_rx_time: 0x%x\r\n",
				data_len_change->conn_handle,
				data_len_change->max_tx_octets,
				data_len_change->max_tx_time,
				data_len_change->max_rx_octets,
				data_len_change->max_rx_time);
		BT_AT_PRINT("+BLEGAP:conn_datalen,%d,0x%x,0x%x,0x%x,0x%x\r\n",
					data_len_change->conn_handle,
					data_len_change->max_tx_octets,
					data_len_change->max_tx_time,
					data_len_change->max_rx_octets,
					data_len_change->max_rx_time);
		break;
	}

	case RTK_BT_LE_GAP_EVT_PHY_UPDATE_IND: {
		rtk_bt_le_phy_update_ind_t *phy_update_ind =
			(rtk_bt_le_phy_update_ind_t *)param;
		if (phy_update_ind->err) {
			BT_LOGE("[APP] Update PHY failed, conn_handle: %d, err: 0x%x\r\n",
					phy_update_ind->conn_handle,
					phy_update_ind->err);
			BT_AT_PRINT("+BLEGAP:conn_phy,%d,-1\r\n", phy_update_ind->conn_handle);
		} else {
			BT_LOGA("[APP] PHY is updated, conn_handle: %d, tx_phy: %d, rx_phy: %d\r\n",
					phy_update_ind->conn_handle,
					phy_update_ind->tx_phy,
					phy_update_ind->rx_phy);
			BT_AT_PRINT("+BLEGAP:conn_phy,%d,0,%d,%d\r\n",
						phy_update_ind->conn_handle,
						phy_update_ind->tx_phy,
						phy_update_ind->rx_phy);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PAIRING_CONFIRM_IND: {
		uint16_t ret = 0;
		rtk_bt_le_auth_pair_cfm_ind_t *pair_cfm_ind = (rtk_bt_le_auth_pair_cfm_ind_t *)param;
		BT_LOGA("[APP] Just work pairing need user to confirm, conn_handle: %d!\r\n",
				pair_cfm_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:pair_cfm,%d\r\n", pair_cfm_ind->conn_handle);
		rtk_bt_le_pair_cfm_t pair_cfm_param = {0};
		pair_cfm_param.conn_handle = pair_cfm_ind->conn_handle;
		pair_cfm_param.confirm = 1;
		ret = rtk_bt_le_sm_pairing_confirm(&pair_cfm_param);
		if (RTK_BT_OK == ret) {
			BT_LOGA("[APP] Just work pairing auto confirm succcess\r\n");
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_DISPLAY_IND: {
		rtk_bt_le_auth_key_display_ind_t *key_dis_ind = (rtk_bt_le_auth_key_display_ind_t *)param;
		BT_LOGA("[APP] Auth passkey display: %d, conn_handle:%d\r\n",
				(int)key_dis_ind->passkey,
				(int)key_dis_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_display,%d,%d\r\n",
					(int)key_dis_ind->conn_handle,
					(int)key_dis_ind->passkey);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_INPUT_IND: {
		rtk_bt_le_auth_key_input_ind_t *key_input_ind = (rtk_bt_le_auth_key_input_ind_t *)param;
		BT_LOGA("[APP] Please input the auth passkey get from remote, conn_handle: %d\r\n",
				key_input_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_input,%d\r\n", key_input_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_CONFIRM_IND: {
		rtk_bt_le_auth_key_cfm_ind_t *key_cfm_ind = (rtk_bt_le_auth_key_cfm_ind_t *)param;
		BT_LOGA("[APP] Auth passkey confirm: %d, conn_handle: %d. "  \
				"Please comfirm if the passkeys are equal!\r\n",
				(int)key_cfm_ind->passkey,
				(int)key_cfm_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_cfm,%d,%d\r\n",
					(int)key_cfm_ind->conn_handle,
					(int)key_cfm_ind->passkey);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_OOB_KEY_INPUT_IND: {
		rtk_bt_le_auth_oob_input_ind_t *oob_input_ind = (rtk_bt_le_auth_oob_input_ind_t *)param;
		BT_LOGA("[APP] Bond use oob key, conn_handle: %d. Please input the oob tk \r\n",
				oob_input_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:oobkey_input,%d\r\n", oob_input_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_COMPLETE_IND: {
		rtk_bt_le_auth_complete_ind_t *auth_cplt_ind = (rtk_bt_le_auth_complete_ind_t *)param;
		BT_AT_PRINT("+BLEGAP:sec,%d,%d\r\n",
					auth_cplt_ind->conn_handle,
					(auth_cplt_ind->err == 0) ? 0 : -1);
		if (auth_cplt_ind->err) {
			BT_LOGE("[APP] Pairing failed(err: 0x%x), conn_handle: %d\r\n",
					auth_cplt_ind->err, auth_cplt_ind->conn_handle);
		} else {
			BT_LOGA("[APP] Pairing success, conn_handle: %d\r\n", auth_cplt_ind->conn_handle);
			BT_DUMPHEXA("[APP] long term key is 0x", auth_cplt_ind->dev_ltk, auth_cplt_ind->dev_ltk_length, true);
			if (RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT == pbp_role) {
				uint16_t ret = rtk_bt_gattc_discover_all(auth_cplt_ind->conn_handle);
				if (RTK_BT_OK != ret) {
					BT_LOGE("[APP] GATTC Discover failed! err: 0x%x\r\n", ret);
					break;
				} else {
					BT_LOGA("[APP] GATTC Discover Start, waiting ...\r\n");
				}
			}
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_BOND_MODIFY_IND: {
		rtk_bt_le_bond_modify_ind_t *bond_mdf_ind = (rtk_bt_le_bond_modify_ind_t *)param;
		char ident_addr[30] = {0};
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->remote_addr), le_addr, sizeof(le_addr));
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->ident_addr), ident_addr, sizeof(ident_addr));
		BT_LOGA("[APP] Bond info modified, op: %d, addr: %s, ident_addr: %s\r\n", bond_mdf_ind->op, le_addr, ident_addr);
		BT_AT_PRINT("+BLEGAP:bond_modify,%d,%s,%s\r\n", bond_mdf_ind->op, le_addr, ident_addr);
		break;
	}

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
	case RTK_BT_LE_GAP_EVT_RESOLV_LIST_MODIFY_IND: {
		rtk_bt_le_modify_resolv_list_ind_t *p_ind = (rtk_bt_le_modify_resolv_list_ind_t *)param;
		rtk_bt_le_addr_t addr;

		if (p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_ADD || p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_REMOVE) {
			addr.type = (rtk_bt_le_addr_type_t)p_ind->entry.addr_type;
			memcpy(addr.addr_val, p_ind->entry.addr, RTK_BD_ADDR_LEN);
			rtk_bt_le_addr_to_str(&addr, le_addr, sizeof(le_addr));
			if (p_ind->err) {
				BT_LOGE("[APP] Resolving list %s %s fail, cause:%x.\r\n",
						(p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_ADD) ? "add" : "remove",
						le_addr, p_ind->err);
				BT_AT_PRINT("+BLEGAP:resolv_list_modify,%d,-1\r\n", p_ind->op);
			} else {
				BT_LOGA("[APP] Resolving list %s %s success, %s privacy mode.\r\n",
						(p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_ADD) ? "add" : "remove",
						le_addr, p_ind->entry.device_mode ? "device" : "network");
				BT_AT_PRINT("+BLEGAP:resolv_list_modify,%d,0,%s,%s\r\n",
							p_ind->op, le_addr, p_ind->entry.device_mode ? "device" : "network");
			}
		} else if (p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_CLEAR) {
			if (p_ind->err) {
				BT_LOGE("[APP] Resolving list clear fail, cause:%x.\r\n", p_ind->err);
			} else {
				BT_LOGA("[APP] Resolving list clear success.\r\n");
			}
			BT_AT_PRINT("+BLEGAP:resolv_list_modify,%d,%d\r\n", p_ind->op, (p_ind->err == 0) ? 0 : -1);
		}
		break;
	}
#endif

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
	case RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND: {
		rtk_bt_le_pa_sync_ind_t *pa_sync_ind = (rtk_bt_le_pa_sync_ind_t *)param;
		BT_LOGA("[APP] PA sync state change: sync_id: %d, state = %d, cause: 0x%x\r\n",
				pa_sync_ind->sync_id, pa_sync_ind->state, pa_sync_ind->cause);

		if (pa_sync_ind->state == RTK_BT_LE_PA_SYNC_STATE_SYNCHRONIZED) {
			rtk_bt_le_pa_synced_info_t *p_info = &pa_sync_ind->info;
			rtk_bt_le_addr_to_str(&p_info->addr, le_addr, sizeof(le_addr));
			BT_LOGA("[APP] PA SYNCHRONIZED PARAM: [Device]: %s, sync_handle:0x%x, adv_sid: %d, past_received: %d\r\n",
					le_addr, p_info->sync_handle, p_info->adv_sid, p_info->past_received);
			BT_AT_PRINT("+BLEGAP:pa_sync,%s,0x%x,%d,%d\r\n",
						le_addr, p_info->sync_handle, p_info->adv_sid, p_info->past_received);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_PA_ADV_REPORT_IND: {
		rtk_bt_le_pa_adv_report_ind_t *pa_report = (rtk_bt_le_pa_adv_report_ind_t *)param;
		BT_LOGA("[APP] PA sync ADV report: sync_id %d, sync_handle 0x%x, tx_power %d, rssi %d, cte_type %d, data_status 0x%x, data_len %d\r\n",
				pa_report->sync_id, pa_report->sync_handle, pa_report->tx_power, pa_report->rssi,
				pa_report->cte_type, pa_report->data_status, pa_report->data_len);
		BT_AT_PRINT("+BLEGAP:pa_report,%d,0x%x,%d,%d,%d,0x%x,%d\r\n",
					pa_report->sync_id, pa_report->sync_handle, pa_report->tx_power, pa_report->rssi,
					pa_report->cte_type, pa_report->data_status, pa_report->data_len);
		break;
	}
#endif

#if defined(RTK_BLE_5_2_POWER_CONTROL_SUPPORT) && RTK_BLE_5_2_POWER_CONTROL_SUPPORT
	case RTK_BT_LE_GAP_EVT_TXPOWER_REPORT_IND: {
		rtk_bt_le_txpower_ind_t *txpower_ind = (rtk_bt_le_txpower_ind_t *)param;
		BT_LOGA("[APP] TX power report: conn_handle %d, type %d, txpower %d\r\n",
				txpower_ind->conn_handle, txpower_ind->type, txpower_ind->txpower);
		BT_AT_PRINT("+BLEGAP:txpower_report,%d,%d,%d\r\n",
					txpower_ind->conn_handle, txpower_ind->type, txpower_ind->txpower);
		break;
	}
#endif

	default:
		BT_LOGE("[APP] Unkown gap cb evt type: %d\r\n", evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t rtk_bt_pbp_broadcast_assistant_gattc_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;
	if (RTK_BT_GATTC_EVT_MTU_EXCHANGE == event) {
		rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;
		if (p_gatt_mtu_ind->result == RTK_BT_OK) {
			BT_LOGA("[APP] GATTC mtu exchange success, mtu_size: %d, conn_handle: %d \r\n",
					p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
		} else {
			BT_LOGE("[APP] GATTC mtu exchange fail \r\n");
		}
		return RTK_BT_EVT_CB_OK;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t rtk_bt_le_audio_pbp_app_callback(uint8_t evt_code, void *data, uint32_t len)
{
	uint16_t ret = RTK_BT_OK;
	(void)len;
	app_bt_le_audio_bap_broadcast_source_info_t *p_bsrc_info = g_pbp_bsrc_info.p_bap_bro_sour_info;
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;

	switch (evt_code) {
	/*********************************************pbp_broadcast_source_event**********************************************/
	case RTK_BT_LE_AUDIO_EVT_BROADCAST_SOURCE_STATE_IND: {
		rtk_bt_le_audio_broadcast_source_state_ind_t *param = (rtk_bt_le_audio_broadcast_source_state_ind_t *)data;
		BT_LOGD("[APP] broadcast source state change: broadcast_source_handle: %08x, broadcast_source_state 0x%x, cause: 0x%x\r\n",
				param->broadcast_source_handle, param->broadcast_source_state, param->cause);
		BT_AT_PRINT("+BLEBAP:broadcast,source,state,%p,0x%x,0x%x\r\n",
					param->broadcast_source_handle, param->broadcast_source_state, param->cause);
		// update broadcast state
		p_bsrc_info->broadcast_source_state = param->broadcast_source_state;
		if (param->broadcast_source_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_IDLE) {
			BT_LOGA("[APP] broadcast source idle \r\n");
		} else if (param->broadcast_source_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_CONFIGURED) {
			BT_LOGA("[APP] broadcast source configured \r\n");
			if (param->cause == 0) {
				memcpy(def_le_audio_broadcast_source_create_big_param.broadcast_code, app_lea_def_broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
				ret = rtk_bt_le_audio_broadcast_source_enable(p_bsrc_info->broadcast_source_handle, &def_le_audio_broadcast_source_create_big_param);
				if (ret != RTK_BT_OK) {
					BT_LOGE("[APP] rtk_bt_le_audio_broadcast_source_enable fail,ret = 0x%x\r\n", ret);
					break;
				}
			}
		} else if (param->broadcast_source_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING) {
			BT_LOGA("[APP] broadcast source streaming\r\n");
			g_pbp_bsrc_info.status = RTK_BLE_AUDIO_BROADCAST_SOURCE_START;
			if (param->cause == 0) {
				ret = app_bt_le_audio_pbp_bsrc_setup_data_path(p_bsrc_info);
				if (ret != RTK_BT_OK) {
					BT_LOGE("[APP] app_bt_le_audio_pbp_bsrc_setup_data_path fail,ret = 0x%x\r\n", ret);
					break;
				}
				app_bt_le_audio_pbp_encode_data_control(true);
			}
		} else if (param->broadcast_source_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING_STARTING) {
			BT_LOGA("[APP] broadcast source streaming starting \r\n");
		} else if (param->broadcast_source_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING_STOPPING) {
			BT_LOGA("[APP] broadcast source streaming stopping \r\n");
			app_bt_le_audio_pbp_encode_data_control(false);
			g_pbp_bsrc_info.status = RTK_BLE_AUDIO_BROADCAST_SOURCE_STOP;
		}
		break;
	}
	/*********************************************pbp_broadcast_source_event end*********************************************/

	/*********************************************pbp_broadcast_sink_event***************************************************/
	case RTK_BT_LE_AUDIO_EVT_ISO_DATA_RECEIVE_IND: {
		rtk_bt_le_audio_direct_iso_data_ind_t *p_bt_direct_iso = (rtk_bt_le_audio_direct_iso_data_ind_t *)data;
		app_lea_iso_data_path_t *p_iso_path = NULL;
		if (p_bt_direct_iso->iso_sdu_len) {
			BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_ISO_DATA_RECEIVE_IND, sys_time=%d,iso_conn_handle 0x%x, pkt_status_flag 0x%x, pkt_seq_num 0x%x, ts_flag 0x%x, time_stamp 0x%x,iso_sdu_len 0x%x, p_buf %08x, buf_len %d, offset %d\r\n",
					(int)osif_sys_time_get(), p_bt_direct_iso->iso_conn_handle, p_bt_direct_iso->pkt_status_flag,
					p_bt_direct_iso->pkt_seq_num, p_bt_direct_iso->ts_flag,
					(unsigned int)p_bt_direct_iso->time_stamp, p_bt_direct_iso->iso_sdu_len, p_bt_direct_iso->p_buf, p_bt_direct_iso->buf_len, p_bt_direct_iso->offset);
			p_iso_path = app_bt_le_audio_iso_data_path_find(p_bt_direct_iso->iso_conn_handle, RTK_BLE_AUDIO_ISO_DATA_PATH_RX);
			if (p_iso_path == NULL) {
				BT_LOGE("[APP] %s p_iso_path is NULL\r\n", __func__);
				break;
			}
			if (g_pbp_bsink_decode_enable) {
				p_iso_path->iso_data_rx_queue_num++;
				if (rtk_bt_audio_recvd_data_in(RTK_BT_AUDIO_CODEC_LC3,
											   p_iso_path->track_hdl,
											   p_iso_path->codec_entity,
											   p_bt_direct_iso->p_buf + p_bt_direct_iso->offset,
											   p_bt_direct_iso->iso_sdu_len,
											   0)) {
					BT_LOGE("[APP] %s Stream Data receive Fail! \r\n", __func__);
					p_iso_path->status_fail_cnt++;
				} else {
					BT_LOGD("[APP] app_bt_le_audio_track_recv_data ok iso_conn_handle=0x%x,pkt_seq_num=%d\r\n", p_iso_path->iso_conn_handle,
							p_bt_direct_iso->pkt_seq_num);
					p_iso_path->status_ok_cnt++;
				}
				app_bt_le_audio_iso_data_rx_statistics(p_iso_path, p_bt_direct_iso->pkt_seq_num);
				p_iso_path->iso_data_rx_queue_num--;
			}
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_PA_SYNC_STATE_IND: {
		rtk_bt_le_audio_pa_sync_state_ind_t *param = (rtk_bt_le_audio_pa_sync_state_ind_t *)data;
		BT_LOGD("[APP] broadcast sink pa sync state change: sync_handle: %08x, sync_state 0x%x, action 0x%x, cause: 0x%x\r\n",
				param->sync_handle, param->sync_state, param->action, param->cause);
		BT_AT_PRINT("+BLEBAP:broadcast,%s,sync_state,%p,0x%x,0x%x,0x%x\r\n",
					(pbp_role & RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SINK) ? "sink" : "assistant",
					param->sync_handle, param->sync_state, param->action, param->cause);

		p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s not find sync dev info for sync_handle %08x\r\n", __func__, param->sync_handle);
			break;
		}
		p_sync_dev_info->pa_sync_state = param->sync_state;
		if (param->sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZED) {
			BT_LOGA("[APP] broadcast sink pa sync synchronized\r\n");
			if (RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT == pbp_role) {
				//stop ext scan
				ret = rtk_bt_le_gap_stop_ext_scan();
				BT_LOGD("[APP] rtk_bt_le_gap_stop_ext_scan %s after pa sync synchronized! ret: 0x%x\r\n", ((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
				//give sem to show pa sync list
				app_bt_le_audio_bap_broadcast_assistant_info_t *p_bro_assi_info = g_pbp_bro_assi_info.p_bap_bro_assi_info;
				if (p_bro_assi_info->pa_sync_ind_sem) {
					osif_sem_give(p_bro_assi_info->pa_sync_ind_sem);
				}
			}
		} else if (param->sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_TERMINATED) {
			BT_LOGA("[APP] broadcast sink pa sync termiated\r\n");
		}
		break;
	}

	// broadcast sink && scan delegator event
	case RTK_BT_LE_AUDIO_EVT_PA_BIG_INFO_IND: {
		rtk_bt_le_audio_pa_biginfo_ind_t *param = (rtk_bt_le_audio_pa_biginfo_ind_t *)data;
		if (RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SINK == pbp_role) {
			BT_LOGD("[APP] broadcast sink pa biginfo indication: sync_handle: %08x, num_bis 0x%x, sync_id 0x%x, pa_sync_handle 0x%x, encryption 0x%x\r\n",
					param->sync_handle, param->biginfo.num_bis, param->biginfo.sync_id, param->biginfo.pa_sync_handle, param->biginfo.encryption);
			p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
			if (!p_sync_dev_info) {
				BT_LOGE("[APP] %s not find sync dev info for sync_handle %08x \r\n", __func__, param->sync_handle);
				break;
			}
			p_sync_dev_info->is_encryption = param->biginfo.encryption;
			if (p_sync_dev_info->big_sync_state == RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZING ||
				p_sync_dev_info->big_sync_state == RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZED) {
				BT_LOGD("[APP] skip big info ind when RTK_BLE_AUDIO_BROADCAST_SINK_BIG_SYNC_SYNCED \r\n");
				break;
			}
			if (p_sync_dev_info->is_past) {
				BT_LOGD("[APP] skip broadcast sink big sync establish when PAST \r\n");
				break;
			}
			//give sem to establish big sync
			app_bt_le_audio_bap_broadcast_sink_info_t *p_bsink_info = g_pbp_bsink_info.p_bap_bro_sink_info;
			if (p_bsink_info->big_info_ind_sem) {
				osif_sem_give(p_bsink_info->big_info_ind_sem);
			}
		} else if (RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT == pbp_role) {
			BT_LOGD("[APP] broadcast assistant pa biginfo indication: sync_handle: %08x, num_bis 0x%x, sync_id 0x%x, pa_sync_handle 0x%x, encryption 0x%x\r\n",
					param->sync_handle, param->biginfo.num_bis, param->biginfo.sync_id, param->biginfo.pa_sync_handle, param->biginfo.encryption);
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_PA_ADV_REPORT_IND: {
		rtk_bt_le_audio_pa_adv_report_ind_t *param = (rtk_bt_le_audio_pa_adv_report_ind_t *)data;
		BT_LOGD("[APP] pa adv report: sync_handle: %08x,sync_id %d, pa_sync_handle 0x%x,tx_power %d, rssi %d, cte_type %d, data_status 0x%x, data_len %d\r\n",
				param->sync_handle, param->sync_id, param->pa_sync_handle, param->tx_power, param->rssi, param->cte_type, param->data_status, param->data_len);
		break;
	}

	// broadcast sink && scan delegator event
	case RTK_BT_LE_AUDIO_EVT_BASE_DATA_MAPPING_MODIFY_IND: {
		rtk_bt_le_audio_base_data_mapping_modify_ind_t *param = (rtk_bt_le_audio_base_data_mapping_modify_ind_t *)data;
		BT_LOGD("[APP] broadcast sink base data modify: sync_handle: %08x, base_data_mapping %08x,\r\n", param->sync_handle, param->base_data_mapping);
		p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s not find sync dev info for sync_handle %08x\r\n", __func__, param->sync_handle);
			break;
		}
		if (param->base_data_mapping != p_sync_dev_info->base_data_mapping) {
			p_sync_dev_info->base_data_mapping = param->base_data_mapping;
			BT_LOGD("[APP] save new base_data_mapping %08x for sync_handle %08x\r\n", param->base_data_mapping, param->sync_handle);
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BIG_SYNC_STATE_IND: {
		rtk_bt_le_audio_big_sync_state_ind_t *param = (rtk_bt_le_audio_big_sync_state_ind_t *)data;
		BT_LOGD("[APP] broadcast sink big sync state change: sync_handle: %08x, sync_state 0x%x, action 0x%x, cause: 0x%x\r\n",
				param->sync_handle, param->sync_state, param->action, param->cause);
		BT_AT_PRINT("+BLEBAP:broadcast,sink,sync_state,%p,0x%x,0x%x,0x%x\r\n",
					param->sync_handle, param->sync_state, param->action, param->cause);

		p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s not find sync dev info for sync_handle %08x\r\n", __func__, param->sync_handle);
			break;
		}
		p_sync_dev_info->big_sync_state = param->sync_state;
		if (param->sync_state == RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZED && param->action == RTK_BT_LE_AUDIO_BIG_SYNC) {
			BT_LOGA("[APP] broadcast sink big sync synchronized\r\n");
			g_pbp_bsink_info.status = RTK_BLE_AUDIO_BROADCAST_SINK_START;
			//setup data path
			ret = app_bt_le_audio_pbp_bsink_setup_data_path(param->sync_handle);
			BT_LOGA("[APP] app_bt_le_audio_broadcast_sink_setup_data_path %s after big sync synchronized! ret: 0x%x\r\n",
					((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
			if (p_sync_dev_info->is_past == false) {
				//terminate PA sync to save bandwitch
				ret = rtk_bt_le_audio_pa_sync_terminate(param->sync_handle);
				BT_LOGA("[APP] rtk_bt_le_audio_pa_sync_terminate %s after big sync synchronized! ret: 0x%x\r\n",
						((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
				//stop ext scan
				ret = rtk_bt_le_gap_stop_ext_scan();
				BT_LOGA("[APP] rtk_bt_le_gap_stop_ext_scan %s after big sync synchronized! ret: 0x%x\r\n",
						((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
			}
			//init rx thread
			app_bt_le_audio_pbp_bsink_decode_data_control(true);
		} else if (param->sync_state == RTK_BT_LE_AUDIO_BIG_SYNC_STATE_TERMINATED) {
			BT_LOGA("[APP] broadcast sink big sync termiated\r\n");
			ret = app_bt_le_audio_pbp_bsink_remove_data_path(param->sync_handle);
			BT_LOGA("[APP] app_bt_le_audio_pbp_bsink_remove_data_path %s after big sync terminated! ret: 0x%x\r\n",
					((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
			ret = rtk_bt_le_audio_sync_release(param->sync_handle);
			BT_LOGA("[APP] rtk_bt_le_audio_sync_release %s, ret: 0x%x\r\n", ((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
			//deinit rx thread
			app_bt_le_audio_pbp_bsink_decode_data_control(false);
			ret = app_bt_le_audio_sync_dev_list_remove(p_sync_dev_info->sync_handle);
			BT_LOGA("[APP] %s: app_bt_le_audio_sync_dev_list_remove %s, ret: 0x%x\r\n",
					__func__, ((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
			g_pbp_bsink_info.status = RTK_BLE_AUDIO_BROADCAST_SINK_STOP;
		}
		break;
	}
	// scan delegator event callback
	case RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: {
		rtk_bt_le_audio_bass_cp_ind_t *param = (rtk_bt_le_audio_bass_cp_ind_t *) data;
		switch (param->opcode) {
		case RTK_BT_LE_AUDIO_BASS_CP_REMOTE_SCAN_STOP: {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: conn_handle %d RTK_BT_LE_AUDIO_BASS_CP_REMOTE_SCAN_STOP\r\n", param->conn_handle);
		}
		break;

		case RTK_BT_LE_AUDIO_BASS_CP_REMOTE_SCAN_START: {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: conn_handle %d RTK_BT_LE_AUDIO_BASS_CP_REMOTE_SCAN_START\r\n", param->conn_handle);
		}
		break;

		case RTK_BT_LE_AUDIO_BASS_CP_ADD_SOURCE: {
			char addr_str[32] = {0};
			rtk_bt_le_addr_to_str(&param->op_param.add_source_param.adv_addr.addr_val, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: conn_handle %d RTK_BT_LE_AUDIO_BASS_CP_ADD_SOURCE,type %d, addr_val %s,broadcast_id[%x%x%x],pa_sync %d,pa_interval %d,num_subgroups %d\r\n",
					param->conn_handle, param->op_param.add_source_param.adv_addr.type, addr_str,
					param->op_param.add_source_param.broadcast_id[2], param->op_param.add_source_param.broadcast_id[1], param->op_param.add_source_param.broadcast_id[0],
					param->op_param.add_source_param.pa_sync, param->op_param.add_source_param.pa_interval, param->op_param.add_source_param.num_subgroups);
			BT_AT_PRINT("+BLEBAP:broadcast,sink,bass,add_source,%d,%d,%s,%x%x%x,%d,%d,%d\r\n",
						param->conn_handle, param->op_param.add_source_param.adv_addr.type, addr_str,
						param->op_param.add_source_param.broadcast_id[2], param->op_param.add_source_param.broadcast_id[1], param->op_param.add_source_param.broadcast_id[0],
						param->op_param.add_source_param.pa_sync, param->op_param.add_source_param.pa_interval, param->op_param.add_source_param.num_subgroups);

			if (param->op_param.add_source_param.num_subgroups) {
				BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: bis_info_size %d, bis_sync_state %d, metadata_len %d\r\n",
						param->op_param.add_source_param.bis_info_size, (unsigned int)param->op_param.add_source_param.p_cp_bis_info[0].bis_sync_state,
						param->op_param.add_source_param.p_cp_bis_info[0].metadata_len);
				BT_DUMPD("", param->op_param.add_source_param.p_cp_bis_info[0].p_metadata,
						 param->op_param.add_source_param.p_cp_bis_info[0].metadata_len);
			}
		}
		break;

		case RTK_BT_LE_AUDIO_BASS_CP_MODIFY_SOURCE: {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: conn_handle %d RTK_BT_LE_AUDIO_BASS_CP_MODIFY_SOURCE,source_id %d,pa_sync %d,pa_interval %d,num_subgroups %d\r\n",
					param->conn_handle, param->op_param.modify_source_param.source_id,
					param->op_param.modify_source_param.pa_sync, param->op_param.modify_source_param.pa_interval, param->op_param.modify_source_param.num_subgroups);
			BT_AT_PRINT("+BLEBAP:broadcast,sink,bass,modify_source,%d,%d,%d,%d,%d\r\n",
						param->conn_handle, param->op_param.modify_source_param.source_id,
						param->op_param.modify_source_param.pa_sync, param->op_param.modify_source_param.pa_interval,
						param->op_param.modify_source_param.num_subgroups);

			if (param->op_param.modify_source_param.num_subgroups) {
				BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: bis_info_size %d, bis_sync_state %d, metadata_len %d\r\n",
						param->op_param.modify_source_param.bis_info_size, (unsigned int)param->op_param.modify_source_param.p_cp_bis_info[0].bis_sync_state,
						param->op_param.modify_source_param.p_cp_bis_info[0].metadata_len);
				BT_DUMPD("", param->op_param.modify_source_param.p_cp_bis_info[0].p_metadata,
						 param->op_param.modify_source_param.p_cp_bis_info[0].metadata_len);
			}
		}
		break;

		case RTK_BT_LE_AUDIO_BASS_CP_REMOVE_SOURCE: {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: conn_handle %d RTK_BT_LE_AUDIO_BASS_CP_REMOVE_SOURCE source_id %d\r\n", param->conn_handle,
					param->op_param.remove_source_param.source_id);
			BT_AT_PRINT("+BLEBAP:broadcast,sink,bass,remove_source,%d,%d\r\n",
						param->conn_handle, param->op_param.remove_source_param.source_id);

			p_sync_dev_info = app_bt_le_audio_sync_dev_list_find_by_source_id(param->op_param.remove_source_param.source_id);
			if (!p_sync_dev_info) {
				BT_LOGE("[APP] %s app_bt_le_audio_sync_dev_list_find fail\r\n", __func__);
				break;
			}
			app_bt_le_audio_sync_dev_list_remove(p_sync_dev_info->sync_handle);

		}
		break;

		case RTK_BT_LE_AUDIO_BASS_CP_SET_BROADCAST_CODE: {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: conn_handle %d RTK_BT_LE_AUDIO_BASS_CP_SET_BROADCAST_CODE source_id %d\r\n", param->conn_handle,
					param->op_param.remove_source_param.source_id);
			BT_DUMPA("", param->op_param.set_broadcast_code_param.broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
		}
		break;

		default:
			break;
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BASS_BRS_MODIFY_IND: {
		rtk_bt_le_audio_bass_brs_modify_ind_t *param = (rtk_bt_le_audio_bass_brs_modify_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_BRS_MODIFY_IND: sync_handle %08x, source_id %d\r\n", param->sync_handle, param->source_id);
		BT_AT_PRINT("+BLEBAP:broadcast,sink,bass,brs_modify,%08x,%d\r\n",
					param->sync_handle, param->source_id);

		p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s app_bt_le_audio_sync_dev_list_find fail\r\n", __func__);
			break;
		}
		p_sync_dev_info->source_id = param->source_id;
		if (param->p_brs_data && param->p_brs_data->brs_is_used) {
			BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_BASS_BRS_MODIFY_IND: source_id %d, source_adv_sid %d, pa_sync_state %d, big_encryption %d\r\n",
					param->p_brs_data->source_id, param->p_brs_data->source_adv_sid, param->p_brs_data->pa_sync_state, param->p_brs_data->big_encryption);
			if (param->p_brs_data->num_subgroups) {
				BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_BASS_BRS_MODIFY_IND: num_subgroups %d, bis_info_size %d, bis_sync_state 0x%x, metadata_len %d\r\n",
						param->p_brs_data->num_subgroups, param->p_brs_data->bis_info_size, (unsigned int)param->p_brs_data->p_cp_bis_info[0].bis_sync_state,
						param->p_brs_data->p_cp_bis_info[0].metadata_len);
				BT_DUMPD("", param->p_brs_data->p_cp_bis_info[0].p_metadata, param->p_brs_data->p_cp_bis_info[0].metadata_len);
			}
			p_sync_dev_info->adv_sid = param->p_brs_data->source_adv_sid;
			memcpy(&p_sync_dev_info->adv_addr, &param->p_brs_data->source_address, sizeof(rtk_bt_le_addr_t));
			memcpy(p_sync_dev_info->broadcast_id, param->p_brs_data->broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);
		}
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_BASS_BA_ADD_SOURCE_IND: {
		rtk_bt_le_audio_bass_ba_add_source_ind_t *param = (rtk_bt_le_audio_bass_ba_add_source_ind_t *)data;
		BT_LOGA("[APP] scan delegator ba add source ind: sync_handle: %08x,source_id %d\r\n", param->sync_handle, param->source_id);
		p_sync_dev_info = app_bt_le_audio_sync_dev_list_add(param->sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s app_bt_le_audio_sync_dev_list_add fail\r\n", __func__);
			break;
		}
		p_sync_dev_info->source_id = param->source_id;
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BASS_GET_PA_SYNC_PARAM_IND: { //BT api shall not be called here
		rtk_bt_le_audio_bass_pa_sync_param_t pa_sync_param = {
			.pa_sync_options = 0,
			.pa_sync_skip = 0,
			.pa_sync_timeout = RTK_LE_AUDIO_SYNC_TIMEOUT,
			.past_timeout = RTK_LE_AUDIO_SYNC_TIMEOUT
		};
		rtk_bt_le_audio_bass_get_pa_sync_param_ind_t *param = (rtk_bt_le_audio_bass_get_pa_sync_param_ind_t *)data;
		BT_LOGA("[APP] broadcast sink get pa sync param ind: sync_handle: %08x,source_id %d,is_past: %d,pa_interval %d\r\n",
				param->sync_handle, param->source_id, param->is_past, param->pa_interval);
		if (param->is_past) {
			p_sync_dev_info = app_bt_le_audio_sync_dev_list_add(param->sync_handle);
		} else {
			p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
		}
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s app_bt_le_audio_sync_dev_list_find fail\r\n", __func__);
			break;
		}
		p_sync_dev_info->is_past = param->is_past;
		p_sync_dev_info->pa_interval = param->pa_interval;
		memcpy(param->p_pa_sync_param, &pa_sync_param, sizeof(rtk_bt_le_audio_bass_pa_sync_param_t));
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BASS_GET_BIG_SYNC_PARAM_IND: { //BT api shall not be called here
		rtk_bt_le_audio_bass_big_sync_param_t big_sync_param = {
			.big_mse = 0,
			.big_sync_timeout = RTK_LE_AUDIO_SYNC_TIMEOUT
		};
		rtk_bt_le_audio_bass_get_big_sync_param_ind_t *param = (rtk_bt_le_audio_bass_get_big_sync_param_ind_t *)data;
		BT_LOGA("[APP] broadcast sink get big sync param ind: sync_handle: %08x,source_id %d\r\n", param->sync_handle, param->source_id);
		memcpy(param->p_big_sync_param, &big_sync_param, sizeof(rtk_bt_le_audio_bass_big_sync_param_t));
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BASS_GET_BROADCAST_CODE_IND: { //BT api shall not be called here
		rtk_bt_le_audio_bass_get_broadcast_code_ind_t *param = (rtk_bt_le_audio_bass_get_broadcast_code_ind_t *)data;
		BT_LOGA("[APP] broadcast sink get broadcast code ind: sync_handle: %08x,source_id %d\r\n", param->sync_handle, param->source_id);
		p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s app_bt_le_audio_sync_dev_list_find fail\r\n", __func__);
			break;
		}
		BT_DUMPD("", p_sync_dev_info->broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
		memcpy(param->p_broadcast_code, p_sync_dev_info->broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BASS_GET_PREFER_BIS_SYNC_IND: { //BT api shall not be called here
		rtk_bt_le_audio_bass_get_prefer_bis_sync_ind_t *param = (rtk_bt_le_audio_bass_get_prefer_bis_sync_ind_t *)data;
		BT_LOGA("[APP] broadcast sink set prefer bis sync ind: sync_handle: %08x,source_id %d\r\n",
				param->sync_handle, param->source_id);
		p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
		if (p_sync_dev_info) {
			BT_LOGE("[APP] %s app_bt_le_audio_sync_dev_list_add fail\r\n", __func__);
			break;
		}
		*param->p_support_bis_array = p_sync_dev_info->support_bis_array;
	}
	break;

	/*********************************************pbp_broadcast_sink_event end*************************************************/

	/*********************************************pbp_broadcast_assistant event**********************************************/
	case RTK_BT_LE_AUDIO_EVT_BAP_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_bap_discovery_done_ind_t *param = (rtk_bt_le_audio_bap_discovery_done_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BAP_DISCOVERY_DONE_IND(conn_handle %d, pacs_is_found %d,sink_pac_num %d, source_pac_num %d\r\n",
				param->conn_handle, param->pacs_is_found, param->sink_pac_num, param->source_pac_num);
		BT_LOGA("[APP] ascs_is_found 0x%x, sink_ase_num %d, source_ase_num %d, bass_is_found %d, brs_char_num %d)\r\n",
				param->ascs_is_found, param->sink_ase_num, param->source_ase_num, param->bass_is_found, param->brs_char_num);
		//check wheather it support le audio bass
		if (param->bass_is_found == 0) {
			BT_LOGE("[APP] conn_handle(%d) not support bass, skip it\r\n", param->conn_handle);
			break;
		}
		p_device_info = app_bt_le_audio_device_list_add(param->conn_handle);
		if (p_device_info) {
			p_device_info->pacs_is_found = param->pacs_is_found;
			p_device_info->sink_pac_num = param->sink_pac_num;
			p_device_info->source_pac_num = param->source_pac_num;
			p_device_info->ascs_is_found = param->ascs_is_found;
			p_device_info->sink_ase_num = param->sink_ase_num;
			p_device_info->source_ase_num = param->source_ase_num;
			p_device_info->bass_is_found = param->bass_is_found;
			if (param->brs_char_num) {
				app_bt_le_audio_bass_brs_char_init(p_device_info, param->brs_char_num);
			}
			app_bt_le_audio_new_device_add_in_group(param->conn_handle, &g_pbp_bro_assi_info.group_handle);
		}
		BT_LOGA("[APP] Discovery done for conn_handle %d\r\n", param->conn_handle);
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_BASS_CLIENT_BRS_DATA_IND: {
		rtk_bt_le_audio_bass_client_brs_data_ind_t *param = (rtk_bt_le_audio_bass_client_brs_data_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CLIENT_BRS_DATA_IND: conn_handle 0x%x, notify %d, read_cause 0x%x, char_instance_id %d\r\n",
				param->conn_handle, param->notify, param->read_cause, param->char_instance_id);
		BT_AT_PRINT("+BLEBAP:broadcast,assistant,reception_stop,0x%x,%d,0x%x,%d\r\n",
					param->conn_handle, param->notify, param->read_cause, param->char_instance_id);

		if (param->p_brs_data && param->p_brs_data->brs_is_used) {
			BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CLIENT_BRS_DATA_IND: source_id %d, source_adv_sid %d, pa_sync_state %d, big_encryption %d\r\n",
					param->p_brs_data->source_id, param->p_brs_data->source_adv_sid, param->p_brs_data->pa_sync_state, param->p_brs_data->big_encryption);
			if (param->p_brs_data->num_subgroups) {
				BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CLIENT_BRS_DATA_IND: num_subgroups %d, bis_info_size %d, bis_sync_state 0x%x, metadata_len %d\r\n",
						param->p_brs_data->num_subgroups, param->p_brs_data->bis_info_size, (unsigned int)param->p_brs_data->p_cp_bis_info[0].bis_sync_state,
						param->p_brs_data->p_cp_bis_info[0].metadata_len);
				BT_DUMPD("", param->p_brs_data->p_cp_bis_info[0].p_metadata, param->p_brs_data->p_cp_bis_info[0].metadata_len);
			}
		}
		p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(param->conn_handle);
		if (p_device_info && p_device_info->bass_is_found) {
			if (param->p_brs_data) {
				app_bt_le_audio_bass_brs_char_update(p_device_info, param->char_instance_id, param->p_brs_data);
				if (param->p_brs_data->brs_is_used && param->p_brs_data->big_encryption == RTK_BT_LE_AUDIO_BASS_BRS_BIG_BROADCAST_CODE_REQUIRED) {
					rtk_bt_le_audio_bass_cp_set_broadcast_code_param_t bc_data = {0};
					bc_data.source_id = param->p_brs_data->source_id;
					memcpy(bc_data.broadcast_code, app_lea_def_broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
					ret = rtk_bt_le_audio_bass_client_write_cp_op(param->conn_handle, RTK_BT_LE_AUDIO_BASS_CP_SET_BROADCAST_CODE, &bc_data, false);
					BT_LOGA("[APP] bass write cp set broadcast code %s, ret: 0x%x\r\n", ((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
				}
			}
		}
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_BASS_CLIENT_SYNC_INFO_REQ_IND: {
		rtk_bt_le_audio_bass_client_sync_info_req_ind_t *param = (rtk_bt_le_audio_bass_client_sync_info_req_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CLIENT_SYNC_INFO_REQ_IND: conn_handle 0x%x, char_instance_id %d\r\n", param->conn_handle,
				param->char_instance_id);
		BT_AT_PRINT("+BLEBAP:broadcast,assistant,reception_start,0x%x,%d\r\n",
					param->conn_handle, param->char_instance_id);

		p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(param->conn_handle);
		if (p_device_info && p_device_info->bass_is_found) {
			if (param->p_brs_data) {
				for (uint8_t i = 0; i < p_device_info->brs_char_num; i++) {
					if (p_device_info->brs_char_tbl[i].instance_id == param->char_instance_id) {
						if (p_device_info->brs_char_tbl[i].sync_handle) {
							uint8_t past_type = 0;
							rtk_bt_le_audio_bass_past_srv_data_t srv_data = {
								.adv_a_match_ext_adv = 0,
								.adv_a_match_src = 0,
								.source_id = param->p_brs_data->source_id
							};
							//fix me
							/*if() {
							    past_type = RTK_BT_LE_AUDIO_BASS_PAST_BY_REMOTE_SRC;
							} else {
							    past_type = RTK_BT_LE_AUDIO_BASS_PAST_BY_LOCAL_SRC;
							}*/
							past_type = RTK_BT_LE_AUDIO_BASS_PAST_BY_REMOTE_SRC;
							ret = rtk_bt_le_audio_bass_client_past(past_type, p_device_info->brs_char_tbl[i].sync_handle, param->conn_handle, srv_data);
							BT_LOGA("[APP] rtk_bt_le_audio_bass_client_past %s, ret: 0x%x\r\n", ((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
						}
					}
				}
			}
		}
		break;
	}
	/*********************************************pbp_broadcast_assistant event**********************************************/
	default: {
		BT_LOGD("[APP] %s default evt_code 0x%04x\r\n", __func__, evt_code);
		ret = RTK_BT_ERR_UNHANDLED;
		break;
	}
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t rtk_bt_pbp_broadcast_assistant_common_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)param;
	(void)len;
	rtk_bt_evt_cb_ret_t ret = RTK_BT_EVT_CB_OK;

	switch (evt_code) {
#if defined(RTK_BT_5_2_L2C_ECFC_SUPPORT) && RTK_BT_5_2_L2C_ECFC_SUPPORT
	case RTK_BT_GAP_EVT_ECFC_CONN_CMPL_IND: {
		char addr_str[40] = {0};
		rtk_bt_ecfc_conn_cmpl_ind_t *p_ind = (rtk_bt_ecfc_conn_cmpl_ind_t *)param;
		rtk_bt_addr_to_str(p_ind->bd_type, p_ind->bd_addr, addr_str, 40);
		if (p_ind->err) {
			BT_LOGE("[APP] ECFC connection established fail(0x%x). %s\r\n", p_ind->err, addr_str);
		} else {
			BT_LOGA("[APP] ECFC connection established OK. %s\r\n", addr_str);
			BT_LOGA("[APP] proto_id(%d) conn_handle(%d) remote_mtu(%d) local_mtu(%d) local_mps(%d)\r\n",
					p_ind->proto_id, p_ind->conn_handle, p_ind->remote_mtu, p_ind->local_mtu, p_ind->local_mps);
			BT_DUMP16A("[APP] cid: ", p_ind->cid, p_ind->cid_num);
		}
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_DATA_IND: {
		rtk_bt_ecfc_data_ind_t *p_ind = (rtk_bt_ecfc_data_ind_t *)param;
		BT_LOGA("[APP] ECFC data received. proto_id(%d) conn_handle(%d) cid(%d) length(%d)\r\n",
				p_ind->proto_id, p_ind->conn_handle, p_ind->cid, p_ind->length);
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_CONN_REQ_IND: {
		char addr_str[40] = {0};
		rtk_bt_ecfc_conn_req_ind_t *p_ind = (rtk_bt_ecfc_conn_req_ind_t *)param;
		rtk_bt_addr_to_str(p_ind->bd_type, p_ind->bd_addr, addr_str, 40);
		BT_LOGA("[APP] ECFC connection requset. %s\r\n", addr_str);
		BT_LOGA("[APP] proto_id(%d) conn_handle(%d) remote_mtu(%d) identity_id(%d)\r\n",
				p_ind->proto_id, p_ind->conn_handle, p_ind->remote_mtu, p_ind->identity_id);
		BT_DUMP16A("[APP] cid: ", p_ind->cid, p_ind->cid_num);
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_DISCONN_IND: {
		rtk_bt_ecfc_disconn_ind_t *p_ind = (rtk_bt_ecfc_disconn_ind_t *)param;
		BT_LOGA("[APP] ECFC disconnect. proto_id(%d) conn_handle(%d) cid(%d) cause(0x%x)\r\n",
				p_ind->proto_id, p_ind->conn_handle, p_ind->cid, p_ind->cause);
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_RECONF_REQ_IND: { //BT api shall not be called here
		rtk_bt_ecfc_reconf_req_ind_t *p_ind = (rtk_bt_ecfc_reconf_req_ind_t *)param;
		BT_LOGA("[APP] ECFC reconfigure requset. proto_id(%d) conn_handle(%d) remote_mtu(%d) remote_mps(%d)\r\n",
				p_ind->proto_id, p_ind->conn_handle, p_ind->remote_mtu, p_ind->remote_mps);
		BT_DUMP16A("[APP] cid: ", p_ind->cid, p_ind->cid_num);
		if (p_ind->remote_mtu < RTK_BT_GAP_ECFC_MIN_MTU || p_ind->remote_mps < RTK_BT_GAP_ECFC_MIN_MPS) {
			ret = RTK_BT_EVT_CB_REJECT;
			BT_LOGE("[APP] Reject!\r\n");
		} else {
			ret = RTK_BT_EVT_CB_OK;
			BT_LOGA("[APP] Accept!\r\n");
		}
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_RECONF_RSP_IND: {
		rtk_bt_ecfc_reconf_rsp_ind_t *p_ind = (rtk_bt_ecfc_reconf_rsp_ind_t *)param;
		if (p_ind->cause) {
			BT_LOGE("[APP] ECFC reconfigure fail\r\n");
		} else {
			BT_LOGA("[APP] ECFC reconfigure OK, local_mtu(%d) local mps(%d)\r\n",
					p_ind->local_mtu, p_ind->local_mps);
			BT_DUMP16A("[APP] cid: ", p_ind->cid, p_ind->cid_num);
		}
		break;
	}
#endif
	default:
		BT_LOGE("[APP] Unkown common gap cb evt type: %d\r\n", evt_code);
		break;
	}

	return ret;
}

int bt_pbp_main(uint8_t role, uint8_t enable)
{
	if (1 == enable) {
		if (pbp_demo_init_flag) {
			BT_LOGE("%s Already init! \r\n", __func__);
			return -1;
		}

		pbp_role = role;

		switch (role) {
		/* pbp broadcast source init*/
		case RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SOURCE: {
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			rtk_bt_app_conf_t bt_app_conf = {0};
			app_bt_bap_broadcast_source_info_t *p_bap_bsrc_info = &g_pbp_bsrc_info;
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &p_bap_bsrc_info->lea_app_conf;
			app_lea_audio_cap_param_t cap_param_t = {0};
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
			cap_param_t.p_mcs_service_id = &app_lea_mcs_service_id;
#endif
			/* set lea_app_conf, must called before rtk_bt_enable */
			app_bt_le_audio_lea_app_conf((void *)p_lea_app_conf, 0,
										 RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR,
										 RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SOUR,
										 NULL, 0,
										 NULL, 0,
										 &cap_param_t);

			/* set GAP configuration */
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_LEAUDIO;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			bt_app_conf.le_audio_app_conf = g_pbp_bsrc_info.lea_app_conf;

			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, rtk_bt_le_audio_gap_callback));
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(p_bap_bsrc_info->device_name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_GENERIC_MEDIA_PLAYER));

			/* Update broadcast audio announcement*/
			app_bt_le_audio_pbp_update_broadcast_audio_announcements(p_bap_bsrc_info->device_name,
																	 &bap_broadcast_source_cfg,
																	 app_lea_def_broadcast_audio_announcements,
																	 &app_lea_def_broadcast_audio_announcements_len);
			/* register le audio event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_AUDIO, rtk_bt_le_audio_pbp_app_callback));

			app_bt_le_audio_pbp_broadcast_source_init();
			p_bap_bsrc_info->status = RTK_BLE_AUDIO_BROADCAST_SOURCE_ENABLE;
			pbp_demo_init_flag = true;
			break;
		}

		case RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SINK: {
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			rtk_bt_app_conf_t bt_app_conf = {0};
			app_bt_le_audio_bap_broadcast_sink_info_t *p_pbp_bsink_info = g_pbp_bsink_info.p_bap_bro_sink_info;
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &g_pbp_bsink_info.lea_app_conf;
			char addr_str[30] = {0};
			app_lea_audio_cap_param_t cap_param_t = {0};
			//set lea_app_conf, must called before rtk_bt_enable
			app_bt_le_audio_lea_app_conf((void *)p_lea_app_conf, g_pbp_bsink_info.sound_channel,
										 RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR,
										 RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK | RTK_BT_LE_AUDIO_BAP_ROLE_SCAN_DELE,
										 app_pbp_bsink_pac_sink_codec,
										 app_pbp_bsink_pac_sink_codec_len,
										 NULL, 0,
										 &cap_param_t);

			p_pbp_bsink_info->config_sink_audio_location = p_lea_app_conf->pacs_param.sink_audio_location;

			//set GAP configuration
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_LEAUDIO;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			bt_app_conf.le_audio_app_conf = g_pbp_bsink_info.lea_app_conf;

			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_LOGA("[APP] p_sink_pac_id = %d\r\n", *bt_app_conf.le_audio_app_conf.pacs_param.p_sink_pac_id);

			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, rtk_bt_le_audio_gap_callback));
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(g_pbp_bsink_info.device_name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_GENERIC_MEDIA_PLAYER));

			uint8_t pbp_bsink_adv_data_len = LE_AUDIO_PBP_EXT_ADV_DATA_DEFAULT_LEN;
			app_bt_le_audio_update_adv((void *)p_lea_app_conf, g_pbp_bsink_info.device_name, pbp_bsink_adv_data, &pbp_bsink_adv_data_len);

			/* create ext adv */
			BT_APP_PROCESS(rtk_bt_le_gap_create_ext_adv(&pbp_bsink_def_ext_adv_param, &pbp_bsink_ext_adv_handle));
			BT_LOGA("[APP] def_ext_adv_param.adv_handle = 0x%x\r\n", pbp_bsink_ext_adv_handle);
			/* set ext adv data */
			BT_APP_PROCESS(rtk_bt_le_gap_set_ext_adv_data(pbp_bsink_ext_adv_handle, (uint8_t *)pbp_bsink_adv_data, pbp_bsink_adv_data_len));
			/* start ext adv */
			BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(pbp_bsink_ext_adv_handle, 0, 0));

			/* intialize le audio part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_AUDIO, rtk_bt_le_audio_pbp_app_callback));

			app_bt_le_audio_pbp_broadcast_sink_init(&g_pbp_bsink_info);
			g_pbp_bsink_info.status = RTK_BLE_AUDIO_BROADCAST_SINK_ENABLE;
			pbp_demo_init_flag = true;
			break;
		}

		case RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT: {
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			rtk_bt_app_conf_t bt_app_conf = {0};
			app_bt_le_audio_broadcast_assistant_info_t *p_bro_assi_info = &g_pbp_bro_assi_info;
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &g_pbp_bro_assi_info.lea_app_conf;
			if (p_bro_assi_info->status != RTK_BLE_AUDIO_BROADCAST_ASSISTANT_DISABLE) {
				BT_LOGE("[APP] %s: invalid status(%d), le audio example already enable \r\n", __func__, p_bro_assi_info->status);
				return -1;
			}
			//set lea_app_conf, must called before rtk_bt_enable
			app_bt_le_audio_lea_app_conf((void *)p_lea_app_conf, 0,
										 RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER,
										 RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI,
										 NULL, 0,
										 NULL, 0,
										 NULL);
			//set GAP configuration
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTC | RTK_BT_PROFILE_LEAUDIO;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			bt_app_conf.le_audio_app_conf = g_pbp_bro_assi_info.lea_app_conf;

			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));

			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

			BT_APP_PROCESS(rtk_bt_le_gap_set_scan_param(&pbp_bro_assi_def_scan_param));

			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_COMMON_GP_GAP, rtk_bt_pbp_broadcast_assistant_common_gap_app_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, rtk_bt_le_audio_gap_callback));

			/* Initilize GATT (client) part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTC, rtk_bt_pbp_broadcast_assistant_gattc_app_callback));

			/* register le audio event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_AUDIO, rtk_bt_le_audio_pbp_app_callback));

			app_bt_le_audio_pbp_broadcast_assistant_init(p_bro_assi_info);
			p_bro_assi_info->status = RTK_BLE_AUDIO_BROADCAST_ASSISTANT_ENABLE;
			pbp_demo_init_flag = true;

			break;
		}
		default:
			BT_LOGE("[APP] unknown role 0x%x\r\n", role);
			break;

		}
	} else if (0 == enable) {
		if (!pbp_demo_init_flag) {
			BT_LOGE("%s No need deinit! \r\n", __func__);
			return -1;
		}

		switch (role) {
		/* pbp broadcast source*/
		case RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SOURCE: {
			if (g_pbp_bsrc_info.status == RTK_BLE_AUDIO_BROADCAST_SOURCE_DISABLE) {
				BT_LOGE("[APP] %s: already disabled \r\n", __func__);
				return -1;
			}
			if (g_pbp_bsrc_info.status == RTK_BLE_AUDIO_BROADCAST_SOURCE_START) {
				BT_LOGE("[APP] %s: please stop broadcast first before disable \r\n", __func__);
				return -1;
			}
			app_bt_le_audio_pbp_broadcast_source_deinit();
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			g_pbp_bsrc_info.status = RTK_BLE_AUDIO_BROADCAST_SOURCE_DISABLE;
			pbp_demo_init_flag = false;
		}
		break;

		case RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SINK: {
			if (g_pbp_bsink_info.status == RTK_BLE_AUDIO_BROADCAST_SINK_DISABLE) {
				BT_LOGE("[APP] %s: already disabled \r\n", __func__);
				return -1;
			}
			if (g_pbp_bsink_info.status == RTK_BLE_AUDIO_BROADCAST_SINK_START) {
				BT_LOGE("[APP] %s: please terminate sync with broadcast stream before disable\r\n", __func__);
				return -1;
			}
			/* Stop and remove ea */
			rtk_bt_le_gap_stop_ext_adv(pbp_bsink_ext_adv_handle);
			app_bt_le_audio_pbp_broadcast_sink_deinit(&g_pbp_bsink_info);
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			g_pbp_bsink_info.status = RTK_BLE_AUDIO_BROADCAST_SINK_DISABLE;
			pbp_demo_init_flag = false;
		}
		break;

		case RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT: {
			if (g_pbp_bro_assi_info.status == RTK_BLE_AUDIO_BROADCAST_ASSISTANT_DISABLE) {
				BT_LOGE("[APP] %s: already disabled\r\n", __func__);
				return -1;
			}
			app_bt_le_audio_pbp_broadcast_assistant_deinit(&g_pbp_bro_assi_info);
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			g_pbp_bro_assi_info.status = RTK_BLE_AUDIO_BROADCAST_ASSISTANT_DISABLE;
			pbp_demo_init_flag = false;
		}
		break;

		default:
			BT_LOGE("[APP] unknown role 0x%x\r\n", role);
			break;

		}
	}

	return 0;

}
#endif