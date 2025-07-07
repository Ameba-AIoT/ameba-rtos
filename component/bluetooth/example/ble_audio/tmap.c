/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <string.h>
#include <stdio.h>
#include <bt_api_config.h>
#include <osif.h>
#include <rtk_bt_def.h>
#include <rtk_bt_device.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gap.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_audio_def.h>
#include <rtk_bt_bap.h>
#include <rtk_bt_cap.h>
#include <rtk_bt_tmap.h>
#include <bt_audio_intf.h>
#include <lc3_codec_entity.h>
#include <bt_audio_track_api.h>
#include <bt_audio_record_api.h>
#include <bt_audio_codec_wrapper.h>
#include <bt_utils.h>
#include <bt_le_audio_app_config.h>
#include <app_audio_data.h>

#define APP_LE_AUDIO_TMAP_UMS_DEVICE_NAME "Ameba LE Unicast Media Sender"
#define APP_LE_AUDIO_TMAP_UMR_DEVICE_NAME "Ameba LE Unicast Media Receiver"
#define APP_LE_AUDIO_TMAP_BMS_DEVICE_NAME "Ameba LE Broadcast Media Sender"
#define APP_LE_AUDIO_TMAP_BMR_DEVICE_NAME "Ameba LE Broadcast Media Receiver"
#define APP_LE_AUDIO_DEMO_GAP_APPERANCE RTK_BT_LE_GAP_APPEARANCE_HEADSET
#define APP_BT_LE_EXT_ADV_MAX_LOCAL_NAME_LENGTH 40
#define APP_LE_AUDIO_PRIMARY_ADV_INTERVAL_MIN (48)
#define APP_LE_AUDIO_PRIMARY_ADV_INTERVAL_MAX (48)
#define APP_LE_AUDIO_ADV_LEN_MAX 251
#define APP_LE_AUDIO_ADV_START_PARSING_IDX 7
#define APP_LE_AUDIO_DEMO_DATA_PATH_NUM 5
#define APP_BT_LE_AUDIO_COMMON_SCAN_TYPE 1
#define APP_BT_LE_AUDIO_BASS_SCAN_TYPE 2
#define APP_BT_LE_AUDIO_ADV_DATA_ASCS_BIT 0x01
#define APP_BT_LE_AUDIO_ADV_DATA_BASS_BIT 0x02
#define APP_BT_LE_AUDIO_ADV_DATA_CAP_BIT 0x04
#define APP_BT_LE_AUDIO_ADV_DATA_RSI_BIT 0x08
#define APP_BT_LE_AUDIO_SYNC_TIMEOUT (100)

typedef struct {
	uint16_t count;
	void *mtx;
	struct list_head head;
} app_le_audio_list_t;

typedef struct {
	bool used;
	uint16_t iso_conn_handle;
	void *p_iso_chann;
	void *p_track_hdl;
	void *p_record_hdl;
	void *p_codec_entity;
	rtk_bt_le_audio_iso_data_path_direction_t path_direction;
	uint32_t pkt_seq_num;
	uint32_t time_stamp;
	rtk_bt_le_audio_cfg_codec_t codec_t;
	short *p_encode_data;
	uint32_t last_decode_offset;
	struct enc_codec_buffer *p_enc_codec_buffer_t;
	uint32_t encode_byte;
} app_bt_le_audio_data_path_t;

typedef struct {
	struct list_head list;
	uint8_t scan_type;
	rtk_bt_le_ext_scan_res_ind_t scan_res_ind;          /**< @ref rtk_bt_le_ext_scan_res_ind_t */
	union {
		struct {
			uint8_t adv_data_flags;
			uint8_t ascs_announcement_type;
			uint16_t ascs_sink_available_contexts;
			uint16_t ascs_source_available_contexts;
			uint8_t cap_announcement_type;
			uint8_t local_name[APP_BT_LE_EXT_ADV_MAX_LOCAL_NAME_LENGTH];
		} common_scan_info_t;
		struct {
			uint8_t adv_sid;
			uint8_t broadcast_id[RTK_BT_LE_AUDIO_BROADCAST_ID_LEN];
		} bass_scan_info_t;
	};
} app_bt_le_audio_scan_dev_info_t;

/* static parameters */
static uint8_t bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_UNKNOWN;
static uint8_t cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_UNKNOWN;
static uint8_t tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_UNKNOWN;
static bool demo_init_flag = false;
static app_le_audio_list_t scan_dev_queue;
static app_bt_le_audio_data_path_t app_le_audio_data_path[APP_LE_AUDIO_DEMO_DATA_PATH_NUM] = {0};
static uint8_t bt_le_audio_demo_sink_pac_id = 0xFF;
static uint8_t bt_le_audio_demo_source_pac_id = 0xFF;
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
static uint8_t bt_le_audio_demo_vcs_id_array[RTK_BT_LE_AUDIO_DEFAULT_AICS_VCS_NUM] = {RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX};
#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
static uint8_t bt_le_audio_demo_mics_id_array[RTK_BT_LE_AUDIO_DEFAULT_AICS_MICS_NUM] = {RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX};
#endif
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
static uint8_t bt_le_audio_demo_vocs_features[RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM] = { RTK_BT_LE_AUDIO_VOCS_AUDIO_LOCATION_WRITE_WITHOUT_RSP_SUPPORT | \
																					RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_WRITE_WITHOUT_RSP_SUPPORT | \
																					RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_WRITE_WITHOUT_RSP_SUPPORT | \
																					RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_NOTIFY_SUPPORT
																				  };
#endif
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
static char *bt_le_audio_media_player_name = "TK player";
static char *bt_le_audio_media_track_title = "RTK media trace title";
#endif
/* source pac */
static uint8_t bt_le_audio_demo_pac_source_codec[] = {
	//Number_of_PAC_records
	1,
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
/* sink pac */
static uint8_t bt_le_audio_demo_pac_sink_codec[] = {
	//Number_of_PAC_records
	2,
	//PAC Record
	RTK_BT_LE_LC3_CODEC_ID, 0, 0, 0, 0,//Codec_ID
	//Codec_Specific_Capabilities_Length
	19,
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
	0x02,
	RTK_BT_LE_CODEC_CAP_TYPE_MAX_SUPPORTED_FRAMES_PER_SDU,
	0x02,
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
	19,
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
	0x02,
	RTK_BT_LE_CODEC_CAP_TYPE_MAX_SUPPORTED_FRAMES_PER_SDU,
	0x02,
	//Metadata_Length
	0x04,
	//Metadata
	0x03,
	RTK_BT_LE_METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS,
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_CONVERSATIONAL),
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_CONVERSATIONAL >> 8)
};

/* init le audio app config */
static rtk_bt_le_audio_app_conf_t bt_le_audio_demo_app_conf = {
	.device_name = {0},
	.device_name_len = 0,
	.bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_UNKNOWN,
	.cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_UNKNOWN,
	/* rtk_bt_le_audio_pacs_init_param_t */
	{
		.sink_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL,
		.source_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL,
		.sink_available_contexts = RTK_BT_LE_AUDIO_CONTEXT_UNSPECIFIED | RTK_BT_LE_AUDIO_CONTEXT_MEDIA | RTK_BT_LE_AUDIO_CONTEXT_CONVERSATIONAL,
		.sink_supported_contexts = RTK_BT_LE_AUDIO_CONTEXT_UNSPECIFIED | RTK_BT_LE_AUDIO_CONTEXT_MEDIA | RTK_BT_LE_AUDIO_CONTEXT_CONVERSATIONAL,
		.source_available_contexts = RTK_BT_LE_AUDIO_CONTEXT_MEDIA | RTK_BT_LE_AUDIO_CONTEXT_CONVERSATIONAL,
		.source_supported_contexts = RTK_BT_LE_AUDIO_CONTEXT_MEDIA | RTK_BT_LE_AUDIO_CONTEXT_CONVERSATIONAL,
		.p_sink_pac_id = &bt_le_audio_demo_sink_pac_id,
		.p_pac_sink_codec = bt_le_audio_demo_pac_sink_codec,
		.pac_sink_codec_len = sizeof(bt_le_audio_demo_pac_sink_codec),
		.p_source_pac_id = &bt_le_audio_demo_source_pac_id,
		.p_pac_source_codec = bt_le_audio_demo_pac_source_codec,
		.pac_source_codec_len = sizeof(bt_le_audio_demo_pac_source_codec),
	},
	/* rtk_bt_le_audio_ascs_init_param_t */
	{
		.sink_ase_num = 1,
		.source_ase_num = 1,
	},
	/* rtk_bt_le_audio_cap_init_param_t */
	{
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
		/* rtk_bt_le_audio_csis_init_param_t */
		{
			.csis_num = 1,
			.csis_size = 2,
			.csis_cfg = RTK_BT_LEA_CSIS_CFG_NOT_EXIST,
			.csis_feature = RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_SIZE_EXIST | RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_RANK_EXIST |
			RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_LOCK_EXIST | RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_SIRK_NOTIFY_SUPPORT,
			.csis_sirk_type = RTK_BT_LE_CSIS_SIRK_PLN,
			/* default csis sirk */
			{
				0x11, 0x22, 0x33, 0xc6, 0xaf, 0xbb, 0x65, 0xa2, 0x5a, 0x41, 0xf1, 0x53, 0x05, 0x68, 0x8e, 0x83
			},
		},
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
		/* rtk_bt_le_audio_aics_init_param_t */
		{
			.aics_vcs_num = RTK_BT_LE_AUDIO_DEFAULT_AICS_VCS_NUM,
			.p_aics_vcs_tbl = bt_le_audio_demo_vcs_id_array,
			.aics_mics_num = RTK_BT_LE_AUDIO_DEFAULT_AICS_MICS_NUM,
			.p_aics_mics_tbl = bt_le_audio_demo_mics_id_array,
		},
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
		/* rtk_bt_le_audio_vocs_init_param_t */
		{
			.vocs_num = RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM,
			.p_vocs_feature_tbl = bt_le_audio_demo_vocs_features,
		},
#endif
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
		/* rtk_bt_le_audio_mcs_init_param_t */
		{
			.general_mcs = true,
			.char_media_control_point_support = true,
			.char_media_control_point_opcodes_supported_optional_property_notify = true,
			.char_media_player_name_optional_property_notify = true,
			.char_track_title_optional_property_notify = true,
			.char_track_duration_optional_property_notify = true,
			.char_track_position_optional_property_notify = true,
			.char_playing_order_support = true,
			.char_playing_order_optional_property_notify = true,
			.char_playing_orders_supported_support = true,
			.char_playback_speed_support = true,
			.char_playback_speed_optional_property_notify = true,
			.char_seeking_speed_support = true,
			.char_seeking_speed_optional_property_notify = true,
			.char_media_player_icon_url_support = true,
			.media_state = RTK_BT_LE_AUDIO_MCS_MEDIA_STATE_PAUSED,
			.playing_orders_supported = 0x3FF,
			.media_control_point_opcodes_supported = 0x1FFFFF,
		},
#endif
	},
};
/* demo adv configuration */
static rtk_bt_le_ext_adv_param_t app_bt_le_audio_def_ext_adv_param = {
	.adv_event_prop = RTK_BT_LE_EXT_ADV_EXTENDED_ADV_CONN_UNDIRECTED,
	.primary_adv_interval_min = APP_LE_AUDIO_PRIMARY_ADV_INTERVAL_MIN,
	.primary_adv_interval_max = APP_LE_AUDIO_PRIMARY_ADV_INTERVAL_MAX,
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
static uint8_t app_bt_le_audio_adv_data[APP_LE_AUDIO_ADV_LEN_MAX] = {
	0x02, //AD len
	RTK_BT_LE_GAP_ADTYPE_FLAGS, //AD types
	RTK_BT_LE_GAP_ADTYPE_FLAGS_GENERAL | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, //AD data
	0x03,
	RTK_BT_LE_GAP_ADTYPE_APPEARANCE, //GAP_ADTYPE_APPEARANCE
	(uint8_t)(APP_LE_AUDIO_DEMO_GAP_APPERANCE),
	(uint8_t)(APP_LE_AUDIO_DEMO_GAP_APPERANCE >> 8)
	/* indicate APP_LE_AUDIO_ADV_START_PARSING_IDX */
};
static uint8_t app_bt_le_audio_def_ext_adv_handle = 0xFF;

static uint8_t app_bt_le_audio_adv_data_len = APP_LE_AUDIO_ADV_START_PARSING_IDX;

#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
static bool csip_discover_flag = false;
static rtk_bt_le_create_conn_param_t bt_le_audio_demo_conn_param = {
	.peer_addr = {
		.type = (rtk_bt_le_addr_type_t)0,
		.addr_val = {0},
	},
	.scan_interval = 0x60,
	.scan_window = 0x30,
	.filter_policy = RTK_BT_LE_CONN_FILTER_WITHOUT_WHITELIST,
	.conn_interval_max = 0x60,
	.conn_interval_min = 0x60,
	.conn_latency = 0,
	.supv_timeout = 0x100,
	.scan_timeout = 1000,
};

#define APP_LE_AUDIO_EXT_SCAN_TIMER_INTERVAL 1000
#define APP_LE_AUDIO_EXT_SCAN_TIMER_COUNT 10

static void *bt_le_audio_demo_ext_scan_timer = NULL;
static int16_t bt_le_audio_demo_ext_scan_time_remaining;
static void bt_le_audio_demo_ext_scan_timer_handle(void *arg)
{
	(void)arg;
	if (bt_le_audio_demo_ext_scan_time_remaining > 0) {
		bt_le_audio_demo_ext_scan_time_remaining --;
		osif_timer_restart(&bt_le_audio_demo_ext_scan_timer, APP_LE_AUDIO_EXT_SCAN_TIMER_INTERVAL);
		BT_LOGA("[APP] %s: bt_le_audio_demo_ext_scan_time_remaining = %d\r\n", __func__, bt_le_audio_demo_ext_scan_time_remaining);
	} else {
		/* ext scan time out and close ext scan */
		rtk_bt_le_audio_ext_scan_act(false);
		BT_LOGA("[APP] %s: Time out and stop ext scan\r\n", __func__);
	}
}
#endif

static void app_bt_le_audio_update_adv(uint8_t *device_name, uint8_t device_name_len, uint8_t *adv_data, uint8_t *adv_data_len)
{
	rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &bt_le_audio_demo_app_conf;
	uint16_t idx = *adv_data_len;
	uint16_t sink_available_contexts = p_lea_app_conf->pacs_param.sink_available_contexts;
	uint16_t source_available_contexts = p_lea_app_conf->pacs_param.source_available_contexts;
	uint8_t bap_role = p_lea_app_conf->bap_role;
#if defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT
	uint16_t tmas_role = p_lea_app_conf->tmap_role;
#endif
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
	uint8_t gmap_role = p_lea_app_conf->gmap_role;
#endif
	BT_LOGA("[APP] device_name_len %d *adv_data_len %d\r\n", device_name_len, *adv_data_len);
	adv_data[idx] = device_name_len + 1;
	idx++;
	adv_data[idx] = RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE;
	idx++;
	memcpy(&adv_data[idx], device_name, device_name_len);
	idx += device_name_len;
	adv_data[idx] = 0x04;
	idx++;
	adv_data[idx] = RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA;
	idx++;
	adv_data[idx] = LO_WORD(RTK_BT_LE_GATT_UUID_CAS);
	idx++;
	adv_data[idx] = HI_WORD(RTK_BT_LE_GATT_UUID_CAS);
	idx++;
	adv_data[idx] = RTK_BT_LE_ADV_TARGETED_ANNOUNCEMENT;
	idx++;
	if (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER) {
		adv_data[idx] = 0x09;
		idx++;
		adv_data[idx] = RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA;
		idx++;
		adv_data[idx] = LO_WORD(RTK_BT_LE_GATT_UUID_ASCS);
		idx++;
		adv_data[idx] = HI_WORD(RTK_BT_LE_GATT_UUID_ASCS);
		idx++;
		adv_data[idx] = RTK_BT_LE_ADV_TARGETED_ANNOUNCEMENT;
		idx++;
		adv_data[idx] = LO_WORD(sink_available_contexts);
		idx++;
		adv_data[idx] = HI_WORD(sink_available_contexts);
		idx++;
		adv_data[idx] = LO_WORD(source_available_contexts);
		idx++;
		adv_data[idx] = HI_WORD(source_available_contexts);
		idx++;
		adv_data[idx] = 0; // metadata length
		idx++;
	}
#if defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT
	if ((tmas_role & RTK_BT_LE_AUDIO_TMAP_ROLE_CT) || (tmas_role & RTK_BT_LE_AUDIO_TMAP_ROLE_UMR) || \
		(tmas_role & RTK_BT_LE_AUDIO_TMAP_ROLE_BMR)) {
		adv_data[idx] = 0x05;
		idx++;
		adv_data[idx] = RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA;
		idx++;
		adv_data[idx] = LO_WORD(RTK_BT_LE_GATT_UUID_TMAS);
		idx++;
		adv_data[idx] = HI_WORD(RTK_BT_LE_GATT_UUID_TMAS);
		idx++;
		adv_data[idx] = LO_WORD(tmas_role);
		idx++;
		adv_data[idx] = HI_WORD(tmas_role);
		idx++;
	}
#endif
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
	if ((gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGR) || (gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_UGT)) {
		adv_data[idx] = 0x04;
		idx++;
		adv_data[idx] = RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA;
		idx++;
		adv_data[idx] = LO_WORD(RTK_BT_LE_GATT_UUID_GMAS);
		idx++;
		adv_data[idx] = HI_WORD(RTK_BT_LE_GATT_UUID_GMAS);
		idx++;
		adv_data[idx] = gmap_role;
		idx++;
	}
#endif
	if (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_SCAN_DELE) {
		adv_data[idx] = 0x03;
		idx++;
		adv_data[idx] = RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA;
		idx++;
		adv_data[idx] = LO_WORD(RTK_BT_LE_GATT_UUID_BASS);
		idx++;
		adv_data[idx] = HI_WORD(RTK_BT_LE_GATT_UUID_BASS);
		idx++;
	}
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
	uint8_t prsi_data[RTK_BT_LE_CSI_RSI_LEN] = {0};
	if (rtk_bt_le_audio_csis_gen_rsi(p_lea_app_conf->cap_param.csis_param.csis_sirk, prsi_data) == RTK_BT_OK) {
		adv_data[idx] = RTK_BT_LE_CSI_RSI_LEN + 1;
		idx++;
		adv_data[idx] = RTK_BT_LE_GAP_ADTYPE_RSI;
		idx++;
		memcpy(adv_data + idx, prsi_data, RTK_BT_LE_CSI_RSI_LEN);
		idx += RTK_BT_LE_CSI_RSI_LEN;
	}
#endif
	if (idx > APP_LE_AUDIO_ADV_LEN_MAX) {
		BT_LOGE("[APP] %s adv len too long, adv len = %d\r\n", __func__, idx);
	}
	*adv_data_len = idx;
}

static app_bt_le_audio_scan_dev_info_t *app_bt_le_audio_scan_dev_list_find(rtk_bt_le_addr_t adv_addr, uint8_t scan_type)
{
	app_bt_le_audio_scan_dev_info_t *p_scan_dev_info = NULL;
	app_le_audio_list_t *p_list = &scan_dev_queue;
	void *pmtx = p_list->mtx;
	struct list_head *iterator = NULL, *phead = &p_list->head;
	bool found = false;

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_scan_dev_info = list_entry(iterator, app_bt_le_audio_scan_dev_info_t, list);
			if (p_scan_dev_info == NULL) {
				continue;
			}
			if (0 == memcmp(&p_scan_dev_info->scan_res_ind.addr, &adv_addr, sizeof(rtk_bt_le_addr_t)) && \
				p_scan_dev_info->scan_type == scan_type) {
				found = true;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);
	if (found) {
		return p_scan_dev_info;
	} else {
		return NULL;
	}
}

static uint16_t app_bt_le_audio_scan_dev_list_remove(rtk_bt_le_addr_t adv_addr, uint8_t scan_type)
{
	app_bt_le_audio_scan_dev_info_t *p_scan_dev_info = NULL;
	app_le_audio_list_t *p_list = &scan_dev_queue;
	void *pmtx = p_list->mtx;

	p_scan_dev_info = app_bt_le_audio_scan_dev_list_find(adv_addr, scan_type);
	if (!p_scan_dev_info) {
		BT_LOGE("[APP] %s device(%x %x %x %x %x %x) not found \r\n", __func__,
				adv_addr.addr_val[5], adv_addr.addr_val[4], adv_addr.addr_val[3], adv_addr.addr_val[2], adv_addr.addr_val[1], adv_addr.addr_val[0]);
		return RTK_BT_OK;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_del_init(&p_scan_dev_info->list);
	p_list->count --;
	osif_mutex_give(pmtx);
	osif_mem_free(p_scan_dev_info);
	BT_LOGD("[APP] %s remove device(%x %x %x %x %x %x) from scan list ok\r\n", __func__,
			adv_addr.addr_val[5], adv_addr.addr_val[4], adv_addr.addr_val[3], adv_addr.addr_val[2], adv_addr.addr_val[1], adv_addr.addr_val[0]);

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_scan_dev_list_remove_all(void)
{
	app_bt_le_audio_scan_dev_info_t *p_scan_dev_info = NULL;
	struct list_head *phead = &scan_dev_queue.head;
	struct list_head *plist = NULL, *pnext = NULL;

	/* common type */
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_scan_dev_info = (app_bt_le_audio_scan_dev_info_t *)plist;
		if (p_scan_dev_info && APP_BT_LE_AUDIO_COMMON_SCAN_TYPE == p_scan_dev_info->scan_type) {
			app_bt_le_audio_scan_dev_list_remove(p_scan_dev_info->scan_res_ind.addr, APP_BT_LE_AUDIO_COMMON_SCAN_TYPE);
		}
		plist = pnext;
	}
	/* bass */
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_scan_dev_info = (app_bt_le_audio_scan_dev_info_t *)plist;
		if (p_scan_dev_info && APP_BT_LE_AUDIO_BASS_SCAN_TYPE == p_scan_dev_info->scan_type) {
			app_bt_le_audio_scan_dev_list_remove(p_scan_dev_info->scan_res_ind.addr, APP_BT_LE_AUDIO_BASS_SCAN_TYPE);
		}
		plist = pnext;
	}

	return RTK_BT_OK;
}

static app_bt_le_audio_scan_dev_info_t *app_bt_le_audio_scan_dev_list_add(app_bt_le_audio_scan_dev_info_t *p_info, uint8_t scan_type)
{
	app_bt_le_audio_scan_dev_info_t *p_scan_dev_info = NULL;
	rtk_bt_le_addr_t adv_addr = {0};
	app_le_audio_list_t *p_list = &scan_dev_queue;
	void *pmtx = p_list->mtx;
	struct list_head *phead = &p_list->head;

	if (!p_info) {
		BT_LOGE("[APP] %s p_info is NULL \r\n", __func__);
		return NULL;
	}
	memcpy((void *)&adv_addr, (void *)&p_info->scan_res_ind.addr, sizeof(rtk_bt_le_addr_t));
	if (app_bt_le_audio_scan_dev_list_find(adv_addr, scan_type)) {
		BT_LOGE("[APP] %s device(%x %x %x %x %x %x) alreay in scan_dev_list , skip add action\r\n", __func__,
				adv_addr.addr_val[5], adv_addr.addr_val[4], adv_addr.addr_val[3], adv_addr.addr_val[2], adv_addr.addr_val[1], adv_addr.addr_val[0]);
		return NULL;
	}
	p_scan_dev_info = (app_bt_le_audio_scan_dev_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(app_bt_le_audio_scan_dev_info_t));
	if (!p_scan_dev_info) {
		BT_LOGE("[APP] %s allocate info memory fail \r\n", __func__);
		return NULL;
	}
	memset(p_scan_dev_info, 0, sizeof(app_bt_le_audio_scan_dev_info_t));
	memcpy((void *)p_scan_dev_info, (void *)p_info, sizeof(app_bt_le_audio_scan_dev_info_t));
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_add_tail(&p_scan_dev_info->list, phead);/* insert list */
	p_list->count ++;
	osif_mutex_give(pmtx);
	BT_LOGA("[APP] %s add device(%x %x %x %x %x %x) in scan_dev list ok\r\n", __func__,
			adv_addr.addr_val[5], adv_addr.addr_val[4], adv_addr.addr_val[3], adv_addr.addr_val[2], adv_addr.addr_val[1], adv_addr.addr_val[0]);

	return p_scan_dev_info;
}

static void app_bt_le_audio_scan_parse_report(uint8_t report_data_len, uint8_t *p_report_data, app_bt_le_audio_scan_dev_info_t *p_scan_dev_info)
{
	uint8_t *p_buffer;
	uint8_t pos = 0;

	while (pos < report_data_len) {
		/* Length of the AD structure. */
		uint16_t length = p_report_data[pos++];
		uint8_t type;

		if (length < 1) {
			return;
		}
		if ((length > 0x01) && ((pos + length) <= report_data_len)) {
			/* Copy the AD Data to buffer. */
			p_buffer = p_report_data + pos + 1;
			/* AD Type, one octet. */
			type = p_report_data[pos];
			switch (type) {

			case RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE: {
				if (length <= (APP_BT_LE_EXT_ADV_MAX_LOCAL_NAME_LENGTH + 1)) {
					memcpy((void *)p_scan_dev_info->common_scan_info_t.local_name, (void *)p_buffer, length - 1);
				}
			}
			break;

			case RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA: {

				uint16_t uuid = ((uint16_t)(*p_buffer) << 0) + ((uint16_t)(*(p_buffer + 1)) << 8);
				p_buffer += 2;

				if (uuid == RTK_BT_LE_GATT_UUID_ASCS) {
					p_scan_dev_info->common_scan_info_t.adv_data_flags |= APP_BT_LE_AUDIO_ADV_DATA_ASCS_BIT;

					p_scan_dev_info->common_scan_info_t.ascs_announcement_type = (uint8_t)(*p_buffer);
					p_buffer += 1;
					p_scan_dev_info->common_scan_info_t.ascs_sink_available_contexts = ((uint16_t)(*p_buffer) << 0) + ((uint16_t)(*(p_buffer + 1)) << 8);
					p_buffer += 2;
					p_scan_dev_info->common_scan_info_t.ascs_source_available_contexts = ((uint16_t)(*p_buffer) << 0) + ((uint16_t)(*(p_buffer + 1)) << 8);
					p_buffer += 2;
				} else if (uuid == RTK_BT_LE_GATT_UUID_CAS) {
					p_scan_dev_info->common_scan_info_t.adv_data_flags |= APP_BT_LE_AUDIO_ADV_DATA_CAP_BIT;
					p_scan_dev_info->common_scan_info_t.cap_announcement_type = (uint8_t)(*p_buffer);
					p_buffer += 1;
				} else if (uuid == RTK_BT_LE_GATT_UUID_BASS) {
					p_scan_dev_info->common_scan_info_t.adv_data_flags |= APP_BT_LE_AUDIO_ADV_DATA_BASS_BIT;
				}
			}
			break;

			case RTK_BT_LE_GAP_ADTYPE_RSI: {
				p_scan_dev_info->common_scan_info_t.adv_data_flags |= APP_BT_LE_AUDIO_ADV_DATA_RSI_BIT;
			}
			break;

			default:
				break;
			}
		}
		pos += length;
	}
}

static bool app_bt_le_audio_adv_filter_service_data(uint8_t report_data_len, uint8_t *p_report_data,
													uint16_t uuid, uint8_t **pp_service_data, uint16_t *p_data_len)
{
	uint8_t *p_buffer = NULL;
	uint8_t pos = 0;
	uint16_t length = 0;
	uint8_t type = 0;

	while (pos < report_data_len) {
		/* Length of the AD structure. */
		length = p_report_data[pos++];
		if (length < 1) {
			return false;
		}
		if ((length > 0x01) && ((pos + length) <= report_data_len)) {
			/* Copy the AD Data to buffer. */
			p_buffer = p_report_data + pos + 1;
			/* AD Type, one octet. */
			type = p_report_data[pos];
			switch (type) {
			case RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA:
			case RTK_BT_LE_GAP_ADTYPE_16BIT_MORE: {
				uint16_t srv_uuid = ((uint16_t)(*p_buffer) << 0) + ((uint16_t)(*(p_buffer + 1)) << 8);
				p_buffer += 2;
				if (srv_uuid == uuid) {
					if (pp_service_data != NULL) {
						*pp_service_data = p_buffer;
					}
					if (p_data_len != NULL) {
						*p_data_len = length - 3;
					}
					return true;
				}
			}
			break;
			default:
				break;
			}
		}
		pos += length;
	}

	return false;
}

static uint16_t app_bt_le_audio_common_scan_report_handle(rtk_bt_le_ext_scan_res_ind_t *scan_res_ind)
{
	app_bt_le_audio_scan_dev_info_t scan_dev_info = {0};

	if (!app_bt_le_audio_scan_dev_list_find(scan_res_ind->addr, APP_BT_LE_AUDIO_COMMON_SCAN_TYPE)) {
		app_bt_le_audio_scan_parse_report(scan_res_ind->len, scan_res_ind->data, &scan_dev_info);
		scan_dev_info.scan_type = APP_BT_LE_AUDIO_COMMON_SCAN_TYPE;
		if (scan_dev_info.common_scan_info_t.adv_data_flags) {
			memcpy(&scan_dev_info.scan_res_ind, scan_res_ind, sizeof(rtk_bt_le_ext_scan_res_ind_t));
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
			if (csip_discover_flag) {
				if (scan_dev_info.common_scan_info_t.adv_data_flags & APP_BT_LE_AUDIO_ADV_DATA_RSI_BIT) {
					if (RTK_BT_ERR_LOWER_STACK_API == rtk_bt_le_audio_csis_set_coordinator_check_adv_rsi(scan_res_ind->addr, scan_res_ind->len, scan_res_ind->data)) {
						BT_LOGE("[APP] %s: The scanned device is not in the same coordinator set\r\n", __func__);
						return RTK_BT_OK;
					}
				}
			}
#endif
			if (!app_bt_le_audio_scan_dev_list_add(&scan_dev_info, APP_BT_LE_AUDIO_COMMON_SCAN_TYPE)) {
				BT_LOGE("[APP] %s add device(%x %x %x %x %x %x >> Name %s) in scan_dev list fail \r\n", __func__,
						scan_res_ind->addr.addr_val[5], scan_res_ind->addr.addr_val[4], scan_res_ind->addr.addr_val[3],
						scan_res_ind->addr.addr_val[2], scan_res_ind->addr.addr_val[1], scan_res_ind->addr.addr_val[0],
						scan_dev_info.common_scan_info_t.local_name);
			} else {
				BT_LOGA("[APP] %s add device(%x %x %x %x %x %x >> Name %s) in scan_dev list \r\n", __func__,
						scan_res_ind->addr.addr_val[5], scan_res_ind->addr.addr_val[4], scan_res_ind->addr.addr_val[3],
						scan_res_ind->addr.addr_val[2], scan_res_ind->addr.addr_val[1], scan_res_ind->addr.addr_val[0],
						scan_dev_info.common_scan_info_t.local_name);
				BT_AT_PRINT("+BLEBAP:unicast,client,escan,%02x:%02x:%02x:%02x:%02x:%02x,%s\r\n",
							scan_res_ind->addr.addr_val[5], scan_res_ind->addr.addr_val[4], scan_res_ind->addr.addr_val[3],
							scan_res_ind->addr.addr_val[2], scan_res_ind->addr.addr_val[1], scan_res_ind->addr.addr_val[0],
							scan_dev_info.common_scan_info_t.local_name);
			}
		}
	}

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_bass_scan_report_handle(rtk_bt_le_ext_scan_res_ind_t *scan_res_ind)
{
	uint8_t *p_service_data = NULL;
	uint16_t service_data_len = 0;
	app_bt_le_audio_scan_dev_info_t scan_dev_info = {0}, *p_scan_dev_info = NULL;
	char addr_str[32] = {0};

	rtk_bt_br_addr_to_str(scan_res_ind->addr.addr_val, addr_str, sizeof(addr_str));
	if (app_bt_le_audio_adv_filter_service_data(scan_res_ind->len,
												scan_res_ind->data,
												RTK_BT_LE_BROADCAST_AUDIO_ANNOUNCEMENT_SRV_UUID,
												&p_service_data,
												&service_data_len)) {
		scan_dev_info.scan_type = APP_BT_LE_AUDIO_BASS_SCAN_TYPE;
		scan_dev_info.bass_scan_info_t.adv_sid = scan_res_ind->adv_sid;
		memcpy(&scan_dev_info.scan_res_ind, scan_res_ind, sizeof(rtk_bt_le_ext_scan_res_ind_t));
		memcpy(scan_dev_info.bass_scan_info_t.broadcast_id, p_service_data, service_data_len);
		p_scan_dev_info = app_bt_le_audio_scan_dev_list_find(scan_res_ind->addr, APP_BT_LE_AUDIO_BASS_SCAN_TYPE);
		if (!p_scan_dev_info) {
			p_scan_dev_info = app_bt_le_audio_scan_dev_list_add(&scan_dev_info, APP_BT_LE_AUDIO_BASS_SCAN_TYPE);
			if (!p_scan_dev_info) {
				BT_LOGE("[APP] %s broadcast scan find new device (addr: %s, broadcast_id: %02x %02x %02x), but add fail! \r\n", __func__, addr_str,
						scan_dev_info.bass_scan_info_t.broadcast_id[0],
						scan_dev_info.bass_scan_info_t.broadcast_id[1],
						scan_dev_info.bass_scan_info_t.broadcast_id[2]);
				return RTK_BT_OK;
			}
		} else {
			/* check parameters */
			if (p_scan_dev_info->bass_scan_info_t.adv_sid == scan_dev_info.bass_scan_info_t.adv_sid && \
				!memcmp((void *)p_scan_dev_info->bass_scan_info_t.broadcast_id, (void *)&scan_dev_info.bass_scan_info_t.broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN)) {
				return RTK_BT_OK;
			} else {
				p_scan_dev_info->bass_scan_info_t.adv_sid = scan_dev_info.bass_scan_info_t.adv_sid;
				memcpy(p_scan_dev_info->bass_scan_info_t.broadcast_id, scan_dev_info.bass_scan_info_t.broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);
			}
		}
		BT_LOGA("[APP] %s broadcast scan add new device in list (addr: %s, adv_sid %x, broadcast_id: %02x %02x %02x) \r\n", __func__,
				addr_str,
				p_scan_dev_info->bass_scan_info_t.adv_sid,
				scan_dev_info.bass_scan_info_t.broadcast_id[0],
				scan_dev_info.bass_scan_info_t.broadcast_id[1],
				scan_dev_info.bass_scan_info_t.broadcast_id[2]);
		BT_AT_PRINT("+BLEBAP:broadcast,sink,escan,%s,%x,%x %x %x\r\n",
					addr_str,
					p_scan_dev_info->bass_scan_info_t.adv_sid,
					scan_dev_info.bass_scan_info_t.broadcast_id[0],
					scan_dev_info.bass_scan_info_t.broadcast_id[1],
					scan_dev_info.bass_scan_info_t.broadcast_id[2]);

		return RTK_BT_OK;
	}

	return RTK_BT_ERR_MISMATCH;
}

static uint16_t app_bt_le_audio_scan_report_show(uint8_t scan_type)
{
	app_bt_le_audio_scan_dev_info_t *p_scan_dev_info = NULL;
	app_le_audio_list_t *p_list = &scan_dev_queue;
	void *pmtx = p_list->mtx;
	struct list_head *phead = &p_list->head;
	struct list_head *plist = NULL, *pnext = NULL;
	uint8_t scan_dev_num = 0;

	BT_LOGA("[APP] %s show scan type 0x%x device list: \r\n", __func__, scan_type);
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_scan_dev_info = (app_bt_le_audio_scan_dev_info_t *)plist;
		if (p_scan_dev_info) {
			if (APP_BT_LE_AUDIO_COMMON_SCAN_TYPE == scan_type) {
				if (p_scan_dev_info->scan_type == scan_type) {
					BT_LOGA("[APP] RemoteBd[%d] = [%02x:%02x:%02x:%02x:%02x:%02x], type %d \r\n", scan_dev_num,
							p_scan_dev_info->scan_res_ind.addr.addr_val[5], p_scan_dev_info->scan_res_ind.addr.addr_val[4], p_scan_dev_info->scan_res_ind.addr.addr_val[3],
							p_scan_dev_info->scan_res_ind.addr.addr_val[2], p_scan_dev_info->scan_res_ind.addr.addr_val[1], p_scan_dev_info->scan_res_ind.addr.addr_val[0],
							p_scan_dev_info->scan_res_ind.addr.type);
					scan_dev_num++;
				}
			} else {
				if (p_scan_dev_info->scan_type == scan_type) {
					BT_LOGA("[APP] RemoteBd[%d] = [%02x:%02x:%02x:%02x:%02x:%02x], type %d, broadcast_id [%02x:%02x:%02x]\r\n", scan_dev_num,
							p_scan_dev_info->scan_res_ind.addr.addr_val[5], p_scan_dev_info->scan_res_ind.addr.addr_val[4], p_scan_dev_info->scan_res_ind.addr.addr_val[3],
							p_scan_dev_info->scan_res_ind.addr.addr_val[2], p_scan_dev_info->scan_res_ind.addr.addr_val[1], p_scan_dev_info->scan_res_ind.addr.addr_val[0],
							p_scan_dev_info->scan_res_ind.addr.type,
							p_scan_dev_info->bass_scan_info_t.broadcast_id[0], p_scan_dev_info->bass_scan_info_t.broadcast_id[1], p_scan_dev_info->bass_scan_info_t.broadcast_id[2]);
					scan_dev_num++;
				}
			}
		}
		plist = pnext;
	}
	osif_mutex_give(pmtx);
	BT_LOGA("[APP] scan device list total num: %d\r\n", scan_dev_num);

	return RTK_BT_OK;
}

/* app scan handle flow */
static uint16_t app_bt_le_audio_scan_report_handle(rtk_bt_le_ext_scan_res_ind_t *scan_res_ind, uint8_t scan_type)
{
	uint16_t ret = 1;

	if (!scan_res_ind) {
		BT_LOGE("[APP] %s input scan_res_ind is NULL \r\n", __func__);
		return ret;
	}
	switch (scan_type) {
	case APP_BT_LE_AUDIO_COMMON_SCAN_TYPE: {
		ret = app_bt_le_audio_common_scan_report_handle(scan_res_ind);
		break;
	}

	case APP_BT_LE_AUDIO_BASS_SCAN_TYPE: {
		ret = app_bt_le_audio_bass_scan_report_handle(scan_res_ind);
		break;
	}

	default:
		BT_LOGE("[APP] %s input scan_type 0x%x is invalid \r\n", __func__, scan_type);
		break;
	}

	return ret;
}

/* path related app arch */
#define APP_LE_AUDIO_ENCODE_TASK_PRIO 5
#define APP_LE_AUDIO_ENCODE_TASK_STACK_SIZE (1024*5)
#define APP_LE_AUDIO_USE_HW_GTIMER 1
#define APP_LE_AUDIO_DEFAULT_SDU_INTERVAL_M_S_US (10000) //units: us
#define APP_LE_AUDIO_DEFAULT_SDU_INTERVAL_S_M_US (10000) //units: us
#define APP_LE_AUDIO_DEFAULT_PCM_BIT_WIDTH 16
#if defined(APP_LE_AUDIO_USE_HW_GTIMER) && APP_LE_AUDIO_USE_HW_GTIMER
#include "timer_api.h"
#define LEAUDIO_SEND_TIMER_ID TIMER13
static gtimer_t bt_le_audio_demo_send_timer = {0};
#else
static void *bt_le_audio_demo_send_timer = NULL;
#endif
struct le_audio_demo_task_t {
	void *hdl;
	void *sem;
	uint8_t run;
};
static bool bt_le_audio_demo_encode_enable = false;
static void *bt_le_audio_demo_encode_sem = NULL;
static struct le_audio_demo_task_t bt_le_audio_demo_encode_task = {
	.hdl = NULL,
	.sem = NULL,
	.run = 0,
};
static uint32_t bt_le_audio_demo_send_timer_interval_us = APP_LE_AUDIO_DEFAULT_SDU_INTERVAL_M_S_US;

static uint8_t app_bt_le_audio_find_total_path_num(rtk_bt_le_audio_iso_data_path_direction_t path_direction)
{
	uint8_t path_num = 0;

	for (uint16_t i = 0; i < APP_LE_AUDIO_DEMO_DATA_PATH_NUM; i ++) {
		if (app_le_audio_data_path[i].used && (app_le_audio_data_path[i].path_direction == path_direction)) {
			path_num ++;
		}
	}

	return path_num;
}

#if (defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT)
static bool app_bt_le_audio_check_audio_record_exist(void)
{
	for (uint16_t i = 0; i < APP_LE_AUDIO_DEMO_DATA_PATH_NUM; i ++) {
		if (app_le_audio_data_path[i].used && app_le_audio_data_path[i].p_record_hdl) {
			return true;
		}
	}

	return false;
}
#endif

static uint32_t app_bt_le_audio_translate_le_chnl_to_audio_chnl(uint32_t audio_channel_allocation)
{
	uint32_t le_chnl = audio_channel_allocation & RTK_BT_LE_AUDIO_LOCATION_MASK;
	uint32_t audio_chnl = RTK_BT_LE_AUDIO_LOCATION_FL;
	while (le_chnl) {
		if (le_chnl == 0x1) {
			audio_chnl = RTK_BT_LE_AUDIO_LOCATION_FL; //left
			break;
		} else if (le_chnl == 0x2) {
			audio_chnl = RTK_BT_LE_AUDIO_LOCATION_FR; //right
			break;
		} else if (le_chnl == 0x3) {
			audio_chnl = RTK_BT_LE_AUDIO_LOCATION_FR | RTK_BT_LE_AUDIO_LOCATION_FL; //right and left
			break;
		}
		le_chnl = le_chnl >> 2;
	}
	//BT_LOGA("%s audio_chnl = 0x%x, audio_channel_allocation=0x%x\r\n",__func__,(unsigned int)audio_chnl,(unsigned int)audio_channel_allocation);
	return audio_chnl;
}

static uint8_t app_bt_le_audio_get_lea_chnl_num(uint32_t audio_channel_allocation)
{
	uint8_t channels = 0;

	//count_bits_1
	while (audio_channel_allocation) {
		channels ++ ;
		audio_channel_allocation &= (audio_channel_allocation - 1);
	}
	//BT_LOGA("%s audio_channel_allocation = 0x%x, channels=%d\r\n",__func__,(unsigned int)audio_channel_allocation,channels);
	return channels;
}

static uint32_t app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(uint8_t sample_frequency)
{
	uint32_t sample_rate = 48000;
	switch (sample_frequency) {
	case RTK_BT_LE_SAMPLING_FREQUENCY_CFG_8K:
		sample_rate = 8000;
		break;
	case RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K:
		sample_rate = 16000;
		break;
	case RTK_BT_LE_SAMPLING_FREQUENCY_CFG_24K:
		sample_rate = 24000;
		break;
	case RTK_BT_LE_SAMPLING_FREQUENCY_CFG_32K:
		sample_rate = 32000;
		break;
	case RTK_BT_LE_SAMPLING_FREQUENCY_CFG_44_1K:
		sample_rate = 44100;
		break;
	case RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K:
		sample_rate = 48000;
		break;
	default:
		BT_LOGE("%s unsupport sample_frequency 0x%x\r\n", __func__, sample_frequency);
		break;
	}
	//BT_LOGA("%s sample_frequency = 0x%x, sample_rate=%d\r\n",__func__,sample_frequency,(int)sample_rate);
	return sample_rate;
}

static void *app_bt_le_audio_lc3_codec_entity_add(rtk_bt_le_audio_cfg_codec_t *p_codec)
{
	rtk_bt_audio_codec_conf_t audio_codec_conf = {0};
	rtk_bt_le_audio_lc3_cfg_t lc3_codec_cfg = {0};

	lc3_codec_cfg.frame_duration = p_codec->frame_duration;
	lc3_codec_cfg.sample_frequency = p_codec->sample_frequency;
	lc3_codec_cfg.codec_frame_blocks_per_sdu = p_codec->codec_frame_blocks_per_sdu;
	lc3_codec_cfg.octets_per_codec_frame = p_codec->octets_per_codec_frame;
#if 1
	lc3_codec_cfg.audio_channel_allocation = app_bt_le_audio_translate_le_chnl_to_audio_chnl(p_codec->audio_channel_allocation);
#else //default init two channel each codec
	lc3_codec_cfg.audio_channel_allocation = RTK_BT_LE_AUDIO_LOCATION_FR | RTK_BT_LE_AUDIO_LOCATION_FL;
#endif
	lc3_codec_cfg.compress_bytes = p_codec->octets_per_codec_frame * p_codec->codec_frame_blocks_per_sdu;
	BT_LOGA("[LEA STACK] %s audio_channel_allocation=0x%x, sample_frequency=0x%x \r\n", __func__, (unsigned int)lc3_codec_cfg.audio_channel_allocation,
			lc3_codec_cfg.sample_frequency);

	audio_codec_conf.codec_index = RTK_BT_AUDIO_CODEC_LC3;
	audio_codec_conf.param = (void *)&lc3_codec_cfg;
	audio_codec_conf.param_len = sizeof(lc3_codec_cfg);
	return rtk_bt_audio_codec_add(&audio_codec_conf);
}

static uint16_t app_bt_le_audio_lc3_codec_entity_remove(void *codec_entity)
{
	return rtk_bt_audio_codec_remove(RTK_BT_AUDIO_CODEC_LC3, codec_entity);
}

static rtk_bt_audio_track_t *app_bt_le_audio_track_add(rtk_bt_le_audio_cfg_codec_t *p_codec)
{
	float left_volume = 0.0, right_volume = 0.0;
	uint8_t channels = 0;
	uint32_t audio_chnl = 0;
	uint32_t rate = 0;
	uint32_t duration = 0;

	rate = app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(p_codec->sample_frequency);
	if (p_codec->frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) {
		duration = 10000;
	} else {
		duration = 7500;
	}
#if 1
	channels = app_bt_le_audio_get_lea_chnl_num(p_codec->audio_channel_allocation);
	audio_chnl = app_bt_le_audio_translate_le_chnl_to_audio_chnl(p_codec->audio_channel_allocation);
	if (audio_chnl & RTK_BT_LE_AUDIO_LOCATION_FL) {
		left_volume = RTK_BT_DEFAULT_AUDIO_TRACK_LEFT_VOLUME;
	}
	if (audio_chnl & RTK_BT_LE_AUDIO_LOCATION_FR) {
		right_volume = RTK_BT_DEFAULT_AUDIO_TRACK_RIGHT_VOLUME;
	}
#else
	channels = 2;//default init two channel each track
	left_volume = RTK_BT_DEFAULT_AUDIO_TRACK_LEFT_VOLUME;
	right_volume = RTK_BT_DEFAULT_AUDIO_TRACK_RIGHT_VOLUME;
#endif

	return rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_LC3, (float)left_volume, (float)right_volume,
								  channels, rate, BT_AUDIO_FORMAT_PCM_16_BIT, duration, NULL, true);
}

static uint16_t app_bt_le_audio_track_remove(void *audio_track_hdl)
{
	return rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_LC3, audio_track_hdl);
}

#if defined(RTK_BLE_AUDIO_RECORD_SUPPORT) && RTK_BLE_AUDIO_RECORD_SUPPORT
static rtk_bt_audio_record_t *app_bt_le_audio_record_add(rtk_bt_le_audio_cfg_codec_t *p_codec)
{
	uint8_t channels = 0;
	uint32_t channel_allocation = 0;
	uint32_t rate = 0;
	uint16_t desired_record_period = 0;
	uint32_t record_frame_size = 0;
	uint32_t buffer_bytes = 0;
	rtk_bt_audio_record_t *leaudio_record_hdl = NULL;

	//parsing channels
	channel_allocation = p_codec->audio_channel_allocation;
	while (channel_allocation) {
		channels ++ ;
		channel_allocation &= (channel_allocation - 1);
	}
	rate = app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(p_codec->sample_frequency);

	//desired record period 1 ms;
	desired_record_period = 1;
	//record_frame_size = sample rate 16k * 1ms / 1000 = 16
	record_frame_size = desired_record_period * rate / 1000;
	//buffer_bytes = record_frame_size * channels *(2 bytes) 16_bit pcm = 16*2*2 = 64 bytes
	buffer_bytes = record_frame_size * channels * 2;

	leaudio_record_hdl = rtk_bt_audio_record_add(RTK_BT_AUDIO_CODEC_LC3, channels, rate, buffer_bytes, 0x7f);
	if (!leaudio_record_hdl) {
		BT_LOGE("[LEA STACK] %s failed ! delete record\r\n", __func__);
		rtk_bt_audio_record_del(RTK_BT_AUDIO_CODEC_LC3, leaudio_record_hdl);
		leaudio_record_hdl = NULL;
	}
	// // set default mic bst gain 20DB
	// if (rtk_bt_audio_record_set_mic_bst_gain(RTK_BT_AUDIO_AMIC1, RTK_BT_AUDIO_MICBST_GAIN_20DB)) {
	//     BT_LOGA("[LEA STACK] %s set le audio default mic bst gain 20DB success !\r\n", __func__);
	// }

	return leaudio_record_hdl;
}
#endif

static uint16_t app_bt_le_audio_record_remove(void *audio_record_hdl)
{
	return rtk_bt_audio_record_del(RTK_BT_AUDIO_CODEC_LC3, audio_record_hdl);
}

static uint16_t app_bt_le_audio_add_data_path(uint16_t iso_conn_handle, void *p_iso_chann, rtk_bt_le_audio_iso_data_path_direction_t path_direction,
											  rtk_bt_le_audio_cfg_codec_t codec_t)
{
	for (uint16_t i = 0; i < APP_LE_AUDIO_DEMO_DATA_PATH_NUM; i ++) {
		if (!app_le_audio_data_path[i].used) {
			app_le_audio_data_path[i].used = true;
			app_le_audio_data_path[i].iso_conn_handle = iso_conn_handle;
			app_le_audio_data_path[i].p_iso_chann = p_iso_chann;
			app_le_audio_data_path[i].path_direction = path_direction;
			app_le_audio_data_path[i].codec_t = codec_t;
			/* codec track or record add */
			app_le_audio_data_path[i].p_codec_entity = app_bt_le_audio_lc3_codec_entity_add(&app_le_audio_data_path[i].codec_t);
			if (!app_le_audio_data_path[i].p_codec_entity) {
				BT_LOGE("[APP] %s: codec add fail \r\n", __func__);
				goto error;
			}
			if (RTK_BLE_AUDIO_ISO_DATA_PATH_RX == app_le_audio_data_path[i].path_direction) {
				app_le_audio_data_path[i].p_track_hdl = app_bt_le_audio_track_add(&app_le_audio_data_path[i].codec_t);
				if (!app_le_audio_data_path[i].p_track_hdl) {
					BT_LOGE("[APP] %s track add fail \r\n", __func__);
					app_bt_le_audio_lc3_codec_entity_remove(app_le_audio_data_path[i].p_codec_entity);
					goto error;
				}
			} else {
#if defined(RTK_BLE_AUDIO_RECORD_SUPPORT) && RTK_BLE_AUDIO_RECORD_SUPPORT
				app_le_audio_data_path[i].p_record_hdl = app_bt_le_audio_record_add(&app_le_audio_data_path[i].codec_t);
				if (!app_le_audio_data_path[i].p_record_hdl) {
					BT_LOGE("[APP] %s record add fail \r\n", __func__);
					app_bt_le_audio_lc3_codec_entity_remove(app_le_audio_data_path[i].p_codec_entity);
					goto error;
				}
#endif
			}
			return 0;

error:
			memset((void *)&app_le_audio_data_path[i], 0, sizeof(app_bt_le_audio_data_path_t));
		}
	}
	BT_LOGE("[APP] %s: app_bt_le_audio_add_data_path fail, APP_LE_AUDIO_DEMO_DATA_PATH_NUM not enough! \r\n", __func__);
	return 1;
}

static uint16_t app_bt_le_audio_remove_data_path(uint16_t iso_conn_handle, rtk_bt_le_audio_iso_data_path_direction_t path_direction)
{
	for (uint16_t i = 0; i < APP_LE_AUDIO_DEMO_DATA_PATH_NUM; i ++) {
		if (app_le_audio_data_path[i].used &&
			(app_le_audio_data_path[i].iso_conn_handle == iso_conn_handle) &&
			(app_le_audio_data_path[i].path_direction == path_direction)) {
			if (app_le_audio_data_path[i].p_encode_data) {
				osif_mem_free((void *)app_le_audio_data_path[i].p_encode_data);
			}
			if (app_le_audio_data_path[i].p_codec_entity) {
				app_bt_le_audio_lc3_codec_entity_remove(app_le_audio_data_path[i].p_codec_entity);
			}
			if (app_le_audio_data_path[i].p_track_hdl) {
				app_bt_le_audio_track_remove(app_le_audio_data_path[i].p_track_hdl);
			}
			if (app_le_audio_data_path[i].p_record_hdl) {
				app_bt_le_audio_record_remove(app_le_audio_data_path[i].p_record_hdl);
			}
			memset((void *)&app_le_audio_data_path[i], 0, sizeof(app_bt_le_audio_data_path_t));
			return 0;
		}
	}
	BT_LOGE("[APP] %s: app_bt_le_audio_remove_data_path fail, not found! \r\n", __func__);
	return 1;
}

static void app_bt_le_audio_remove_data_path_all(void)
{
	for (uint16_t i = 0; i < APP_LE_AUDIO_DEMO_DATA_PATH_NUM; i ++) {
		if (app_le_audio_data_path[i].used) {
			if (app_le_audio_data_path[i].p_encode_data) {
				osif_mem_free((void *)app_le_audio_data_path[i].p_encode_data);
			}
			if (app_le_audio_data_path[i].p_codec_entity) {
				app_bt_le_audio_lc3_codec_entity_remove(app_le_audio_data_path[i].p_codec_entity);
			}
			if (app_le_audio_data_path[i].p_track_hdl) {
				app_bt_le_audio_track_remove(app_le_audio_data_path[i].p_track_hdl);
			}
			if (app_le_audio_data_path[i].p_record_hdl) {
				app_bt_le_audio_record_remove(app_le_audio_data_path[i].p_record_hdl);
			}
			memset((void *)&app_le_audio_data_path[i], 0, sizeof(app_bt_le_audio_data_path_t));
		}
	}
}

static uint16_t app_bt_le_audio_data_received(uint16_t iso_handle, uint8_t path_direction, uint8_t *data, uint16_t data_len)
{
	app_bt_le_audio_data_path_t *p_app_bt_le_audio_data_path = NULL;

	for (uint16_t i = 0; i < APP_LE_AUDIO_DEMO_DATA_PATH_NUM; i ++) {
		if (app_le_audio_data_path[i].used &&
			(app_le_audio_data_path[i].iso_conn_handle == iso_handle) &&
			(app_le_audio_data_path[i].path_direction == path_direction)) {
			p_app_bt_le_audio_data_path = &app_le_audio_data_path[i];
			break;
		}
	}
	if (!p_app_bt_le_audio_data_path) {
		BT_LOGE("[APP] %s cannot find matched p_app_bt_le_audio_data_path \r\n", __func__);
		return 1;
	}
	/* do audio data received flow */
	if (rtk_bt_audio_recvd_data_in(RTK_BT_AUDIO_CODEC_LC3,
								   p_app_bt_le_audio_data_path->p_track_hdl,
								   p_app_bt_le_audio_data_path->p_codec_entity,
								   data,
								   data_len,
								   0)) {
		BT_LOGE("[APP] %s Stream Data receive Fail! \r\n", __func__);
		return 1;
	} else {
		BT_LOGD("[APP] %s ok iso_conn_handle=0x%x \r\n", __func__, iso_handle);
	}

	return 0;
}

#if defined(RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT) && RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT

static uint16_t app_bt_le_audio_parse_two_chnl_pcm_data_to_one_chnl(short *in_buf, uint32_t pcm_in_len, short *out_buf, uint32_t pcm_out_len,
																	uint32_t audio_channel_allocation)
{
	uint8_t bytes_per_num = APP_LE_AUDIO_DEFAULT_PCM_BIT_WIDTH / 8;
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

static uint16_t app_bt_le_audio_encode_birds_sing(app_bt_le_audio_data_path_t *p_iso_path, short *p_pcm_data, uint32_t pcm_total_num)
{
	uint8_t bytes_per_num = (APP_LE_AUDIO_DEFAULT_PCM_BIT_WIDTH / 8), pcm_channels = 0;
	uint32_t encode_byte = 0, encode_num = 0, pcm_num_offset = 0, pcm_frame_size = 0;
	uint8_t encode_channels = 0;
	uint32_t sample_rate = 0, frame_duration_us = 0;
	rtk_bt_le_audio_cfg_codec_t *p_codec = NULL;
	uint32_t parse_byte, parse_num = 0;
	uint32_t audio_channel_allocation = 0;

	if (!p_iso_path) {
		BT_LOGE("[APP] %s p_iso_path is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	} else {
		p_iso_path->p_enc_codec_buffer_t = NULL;
		p_codec = &p_iso_path->codec_t;
	}
	if (!p_codec) {
		BT_LOGE("[APP] %s p_codec is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	}
	//birds_sing is 16K 2 channel 16bit array
	pcm_channels = 2;
	sample_rate = app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(p_codec->sample_frequency);
	frame_duration_us = (p_codec->frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500;
	pcm_frame_size = sample_rate * frame_duration_us / 1000 / 1000;
	encode_channels = app_bt_le_audio_get_lea_chnl_num(p_codec->audio_channel_allocation);
	encode_num = pcm_frame_size * encode_channels;
	encode_byte = encode_num * bytes_per_num;
	if (p_iso_path->p_encode_data == NULL) {
		p_iso_path->p_encode_data = (short *)osif_mem_alloc(RAM_TYPE_DATA_ON, encode_byte);
		if (p_iso_path->p_encode_data == NULL) {
			BT_LOGE("[APP] %s p_iso_path->p_encode_data alloc fail\r\n", __func__);
			return RTK_BT_FAIL;
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
		return RTK_BT_FAIL;
	}
	if (pcm_num_offset > pcm_total_num) {
		pcm_num_offset = 0;
	}
	p_iso_path->last_decode_offset = pcm_num_offset;
	p_iso_path->encode_byte = encode_byte;
	BT_LOGD("[APP] %s encode_num = %d,pcm_total_num=%d,pcm_num_offset= %d, last_decode_offset = %d,encode_byte = %d\r\n", __func__, (int)encode_num,
			(int)pcm_total_num, (int)pcm_num_offset, (int)p_iso_path->last_decode_offset, (int)p_iso_path->encode_byte);
	/* encode */
	p_iso_path->p_enc_codec_buffer_t = rtk_bt_audio_data_encode(RTK_BT_AUDIO_CODEC_LC3, p_iso_path->p_codec_entity, p_iso_path->p_encode_data,
																p_iso_path->encode_byte);
	if (!p_iso_path->p_enc_codec_buffer_t) {
		BT_LOGE("[APP] %s rtk_bt_audio_data_encode fail\r\n", __func__);
		return RTK_BT_FAIL;
	}

	return RTK_BT_OK;
}
#endif

static void bt_le_audio_demo_encode_task_entry(void *ctx)
{
	(void)ctx;
	uint32_t sample_rate = 0, frame_duration_us = 0;
	uint16_t frame_num = 0;
	uint16_t ret = RTK_BT_FAIL;
#if defined(RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT) && RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT
	short *p_pcm_data = NULL;
	uint32_t pcm_total_num = 0;
	p_pcm_data = (short *)birds_sing;
	pcm_total_num = birds_sing_size / (APP_LE_AUDIO_DEFAULT_PCM_BIT_WIDTH / 8);
#endif
	bt_le_audio_demo_encode_task.run = 1;
	//give sem each 10ms in
	while (bt_le_audio_demo_encode_task.run) {
		if (bt_le_audio_demo_encode_sem) {
			osif_sem_take(bt_le_audio_demo_encode_sem, BT_TIMEOUT_FOREVER);
		}
		BT_LOGD("[APP] %s sys_time %d\r\n", __func__, (int)osif_sys_time_get());
		/* foreach iso tx app data path */
		for (uint16_t i = 0; i < APP_LE_AUDIO_DEMO_DATA_PATH_NUM; i ++) {
			if (app_le_audio_data_path[i].used && (app_le_audio_data_path[i].path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX)) {
				sample_rate = app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(app_le_audio_data_path[i].codec_t.sample_frequency);
				frame_duration_us = (app_le_audio_data_path[i].codec_t.frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500;
				frame_num = app_le_audio_data_path[i].codec_t.codec_frame_blocks_per_sdu;
				app_le_audio_data_path[i].pkt_seq_num ++;
				app_le_audio_data_path[i].time_stamp += sample_rate * frame_duration_us * frame_num / 1000 / 1000;
#if defined(RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT) && RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT
				if (RTK_BT_OK != app_bt_le_audio_encode_birds_sing(&app_le_audio_data_path[i], p_pcm_data, pcm_total_num)) {
					BT_LOGE("[APP] %s app_bt_le_audio_encode_birds_sing fail\r\n", __func__);
					continue;
				}
#endif
			}
		}
		/* send flow (seperate encode and send flow is for decreasing time offset between different iso path ,caused by encoding time cost) */
		for (uint16_t i = 0; i < APP_LE_AUDIO_DEMO_DATA_PATH_NUM; i ++) {
			if (app_le_audio_data_path[i].used && (app_le_audio_data_path[i].path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX)) {
				/* send */
				if (app_le_audio_data_path[i].p_enc_codec_buffer_t) {
					rtk_bt_le_audio_iso_data_send_info_t send_info = {0};
					send_info.p_data = app_le_audio_data_path[i].p_enc_codec_buffer_t->pbuffer;
					send_info.data_len = app_le_audio_data_path[i].p_enc_codec_buffer_t->frame_size;
					send_info.iso_conn_handle = app_le_audio_data_path[i].iso_conn_handle;
					send_info.pkt_seq_num = app_le_audio_data_path[i].pkt_seq_num;
					send_info.ts_flag = true;
					if (send_info.ts_flag) {
						send_info.time_stamp = app_le_audio_data_path[i].time_stamp;
					}
					ret = rtk_bt_le_audio_iso_data_send(&send_info);
					if (ret == RTK_BT_OK) {
						BT_LOGD("[APP] %s ok, iso_conn_handle 0x%x, seq_num %d, available heap %d sys_time %d time_stamp 0x%x\r\n", __func__, app_le_audio_data_path[i].iso_conn_handle,
								app_le_audio_data_path[i].pkt_seq_num, osif_mem_peek(RAM_TYPE_DATA_ON), (int)osif_sys_time_get(), send_info.time_stamp);
						BT_DUMPD("", app_le_audio_data_path[i].p_enc_codec_buffer_t->pbuffer, app_le_audio_data_path[i].p_enc_codec_buffer_t->frame_size);
					} else {
						BT_LOGE("[APP] %s failed, iso_conn_handle 0x%x, seq_num %d, ret 0x%x\r\n", __func__, app_le_audio_data_path[i].iso_conn_handle,
								app_le_audio_data_path[i].pkt_seq_num, ret);
						BT_DUMPD("", app_le_audio_data_path[i].p_enc_codec_buffer_t->pbuffer, app_le_audio_data_path[i].p_enc_codec_buffer_t->frame_size);
					}
#if defined(RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT) && RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT
					if (app_le_audio_data_path[i].p_enc_codec_buffer_t) {
						rtk_bt_audio_free_encode_buffer(RTK_BT_AUDIO_CODEC_LC3, app_le_audio_data_path[i].p_codec_entity, app_le_audio_data_path[i].p_enc_codec_buffer_t);
					}
#endif
					app_le_audio_data_path[i].p_enc_codec_buffer_t = NULL;
				}
			}
		}
	}
	BT_LOGD("[APP] %s task_delete\r\n", __func__);
	osif_sem_give(bt_le_audio_demo_encode_task.sem);
	bt_le_audio_demo_encode_task.run = 0;
	bt_le_audio_demo_encode_task.hdl = NULL;
	osif_task_delete(NULL);
}

static void app_bt_le_audio_send_timer_handler(void *arg)
{
	(void)arg;

	if (bt_le_audio_demo_encode_task.run) {
		if (bt_le_audio_demo_encode_sem) {
			osif_sem_give(bt_le_audio_demo_encode_sem);
		}
	}
}

static void app_bt_le_audio_send_timer_init(void)
{
#if defined(APP_LE_AUDIO_USE_HW_GTIMER) && APP_LE_AUDIO_USE_HW_GTIMER
	BT_LOGD("[APP] %s hw timer id %d,time_interval_us = %d\r\n", __func__, LEAUDIO_SEND_TIMER_ID, (int)bt_le_audio_demo_send_timer_interval_us);
	if (bt_le_audio_demo_send_timer.handler == NULL) {
		gtimer_init(&bt_le_audio_demo_send_timer, LEAUDIO_SEND_TIMER_ID);
		gtimer_start_periodical(&bt_le_audio_demo_send_timer, bt_le_audio_demo_send_timer_interval_us, (void *)app_bt_le_audio_send_timer_handler, NULL);
	}
#else
	BT_LOGD("[APP] %s sw time_interval_us = %d\r\n", __func__, (int)bt_le_audio_demo_send_timer_interval_us);
	if (bt_le_audio_demo_send_timer_interval_us % 1000 != 0) {
		BT_LOGE("[APP] %s error: interval %d(us) cannot use sw timer, please use hw timer instead\r\n", __func__, (int)bt_le_audio_demo_send_timer_interval_us);
		return;
	}
	if (!bt_le_audio_demo_send_timer) {
		if (false == osif_timer_create(&bt_le_audio_demo_send_timer, "le_audio_send_timer", 1, bt_le_audio_demo_send_timer_interval_us / 1000, true,
									   app_bt_le_audio_send_timer_handler)) {
			BT_LOGE("[APP] %s osif_timer_create fail\r\n", __func__);
			return;
		}
	}
	if (false == osif_timer_start(&bt_le_audio_demo_send_timer)) {
		BT_LOGE("[APP] %s osif_timer_start fail\r\n", __func__);
		return;
	}
#endif
}

static void app_bt_le_audio_send_timer_deinit(void)
{
#if defined(APP_LE_AUDIO_USE_HW_GTIMER) && APP_LE_AUDIO_USE_HW_GTIMER
	if (bt_le_audio_demo_send_timer.handler) {
		gtimer_stop(&bt_le_audio_demo_send_timer);
		gtimer_deinit(&bt_le_audio_demo_send_timer);
		bt_le_audio_demo_send_timer.handler = NULL;
	}
#else
	if (bt_le_audio_demo_send_timer) {
		if (false == osif_timer_stop(&bt_le_audio_demo_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_stop fail \r\n", __func__);
		}
		if (false == osif_timer_delete(&bt_le_audio_demo_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_delete fail \r\n", __func__);
		}
		bt_le_audio_demo_send_timer = NULL;
	}
#endif
}

static void app_bt_le_audio_send_timer_update(uint32_t time_interval_us)
{
	BT_LOGA("[APP] %s: time_interval_us=%d\r\n", __func__, (int)time_interval_us);
	bt_le_audio_demo_send_timer_interval_us = time_interval_us;
#if defined(APP_LE_AUDIO_USE_HW_GTIMER) && APP_LE_AUDIO_USE_HW_GTIMER
	if (bt_le_audio_demo_send_timer.handler) {
		gtimer_reload(&bt_le_audio_demo_send_timer, time_interval_us);
	} else {
		BT_LOGE("[APP] %s: timer not init\r\n", __func__);
	}
#else
	if (bt_le_audio_demo_send_timer) {
		if (false == osif_timer_stop(&bt_le_audio_demo_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_stop fail \r\n", __func__);
		}
		if (false == osif_timer_restart(&bt_le_audio_demo_send_timer, time_interval_us / 1000)) {
			BT_LOGE("[APP] %s osif_timer_start fail\r\n", __func__);
		}
	}
#endif
}

static void app_bt_le_audio_cap_encode_data_control(bool enable)
{
	BT_LOGA("[APP] %s %d\r\n", __func__, enable);
	if (enable) {
		if (bt_le_audio_demo_encode_enable == true) {
			BT_LOGE("[APP] %s: encode is alreay enabled\r\n", __func__);
			return;
		}
		bt_le_audio_demo_encode_enable = true;
		if (bt_le_audio_demo_encode_sem == NULL) {
			osif_sem_create(&bt_le_audio_demo_encode_sem, 0, 0xFFFFFFFF);
		}
		if (bt_le_audio_demo_encode_task.sem == NULL) {
			osif_sem_create(&bt_le_audio_demo_encode_task.sem, 0, 1);
		}
		if (bt_le_audio_demo_encode_task.hdl == NULL) {
			if (osif_task_create(&bt_le_audio_demo_encode_task.hdl, ((const char *)"lea_encode_task"), bt_le_audio_demo_encode_task_entry,
								 NULL, APP_LE_AUDIO_ENCODE_TASK_STACK_SIZE, APP_LE_AUDIO_ENCODE_TASK_PRIO) != true) {
				BT_LOGE("[APP] %s xTaskCreate(lea_encode_task) failed\r\n", __func__);
			}
		}
		app_bt_le_audio_send_timer_init();
	} else {
		if (bt_le_audio_demo_encode_enable == false) {
			BT_LOGE("[APP] %s: encode is alreay disabled\r\n", __func__);
			return;
		}
		bt_le_audio_demo_encode_enable = false;
		app_bt_le_audio_send_timer_deinit();
		if (bt_le_audio_demo_encode_task.hdl) {
			bt_le_audio_demo_encode_task.run = 0;
			osif_sem_give(bt_le_audio_demo_encode_sem);
			osif_sem_take(bt_le_audio_demo_encode_task.sem, BT_TIMEOUT_FOREVER);
			osif_sem_delete(bt_le_audio_demo_encode_task.sem);
			bt_le_audio_demo_encode_task.sem = NULL;
			bt_le_audio_demo_encode_task.hdl = NULL;
		}
		if (bt_le_audio_demo_encode_sem) {
			osif_sem_delete(bt_le_audio_demo_encode_sem);
			bt_le_audio_demo_encode_sem = NULL;
		}
	}
}

/* Basic Audio Profile APP Callback */
static rtk_bt_evt_cb_ret_t app_bt_bap_callback(uint8_t evt_code, void *data, uint32_t len)
{
	(void)len;
	uint16_t ret = RTK_BT_OK;
	switch (evt_code) {

	case RTK_BT_LE_AUDIO_EVT_BAP_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_bap_discovery_done_ind_t *param = (rtk_bt_le_audio_bap_discovery_done_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BAP_DISCOVERY_DONE_IND(conn_handle %d, pacs_is_found %d,sink_pac_num %d, source_pac_num %d\r\n",
				param->conn_handle, param->pacs_is_found, param->sink_pac_num, param->source_pac_num);
		BT_LOGA("[APP] ascs_is_found 0x%x, sink_ase_num %d, source_ase_num %d, bass_is_found %d, brs_char_num %d)\r\n",
				param->ascs_is_found, param->sink_ase_num, param->source_ase_num, param->bass_is_found, param->brs_char_num);
#if !defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) || !RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
		BT_LOGA("[APP] Add group idx 0x%02x \r\n", param->group_idx);
#endif
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_SETUP_DATA_PATH_IND: {
		rtk_bt_le_audio_ascs_setup_data_path_ind_t *param = (rtk_bt_le_audio_ascs_setup_data_path_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_SETUP_DATA_PATH_IND(conn_handle %d, ase_id 0x%x, path_direction 0x%x, cis_conn_handle 0x%x, dev_num %d \r\n",
				param->conn_handle, param->ase_id, param->path_direction, param->cis_conn_handle, (int)param->dev_num);
		BT_AT_PRINT("+BLEBAP:unicast,server,ascs,setup_data_path,%d,0x%x,0x%x,0x%x\r\n",
					param->conn_handle, param->ase_id, param->path_direction, param->cis_conn_handle);
		BT_LOGD("[APP] type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu 0x%x, octets_per_codec_frame 0x%x, audio_channel_allocation 0x%x)\r\n",
				param->codec_cfg.type_exist, param->codec_cfg.frame_duration, param->codec_cfg.sample_frequency, param->codec_cfg.codec_frame_blocks_per_sdu,
				param->codec_cfg.octets_per_codec_frame, (unsigned int)param->codec_cfg.audio_channel_allocation);
		app_bt_le_audio_add_data_path(param->iso_chann_t.iso_conn_handle,
									  param->iso_chann_t.p_iso_chann,
									  param->iso_chann_t.path_direction,
									  param->codec_cfg);
		if (param->iso_chann_t.path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
			if (app_bt_le_audio_find_total_path_num(RTK_BLE_AUDIO_ISO_DATA_PATH_TX) == param->dev_num) {
				app_bt_le_audio_cap_encode_data_control(true);
				app_bt_le_audio_send_timer_update((param->codec_cfg.frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500);
			}
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_REMOVE_DATA_PATH_IND: {
		rtk_bt_le_audio_ascs_remove_data_path_ind_t *param = (rtk_bt_le_audio_ascs_remove_data_path_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_REMOVE_DATA_PATH_IND(conn_handle %d, ase_id 0x%x, path_direction 0x%x, cis_conn_handle 0x%x\r\n",
				param->conn_handle, param->ase_id, param->path_direction, param->cis_conn_handle);
		BT_AT_PRINT("+BLEBAP:unicast,server,ascs,remove_data_path,%d,0x%x,0x%x,0x%x\r\n",
					param->conn_handle, param->ase_id, param->path_direction, param->cis_conn_handle);
		if (param->path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
			app_bt_le_audio_cap_encode_data_control(false);
		}
		app_bt_le_audio_remove_data_path(param->cis_conn_handle, param->path_direction);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_CIS_CONN_INFO: {
		rtk_bt_le_audio_ascs_cis_conn_info_t *param = (rtk_bt_le_audio_ascs_cis_conn_info_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CIS_CONN_INFO: conn_handle %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x\r\n",
				param->conn_handle, param->cis_conn_handle, param->cig_id, param->cis_id);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_ASE_STATE_IND: {
		rtk_bt_le_audio_ascs_ase_state_ind_t *param = (rtk_bt_le_audio_ascs_ase_state_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_ASE_STATE_IND(conn_handle %d, ase_id 0x%x, direction 0x%x, ase_state 0x%x)\r\n",
				param->conn_handle, param->ase_id, param->direction, param->ase_state);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_CP_CODEC_CFG_IND: {
		rtk_bt_le_audio_ascs_cp_codec_cfg_ind_t *param = (rtk_bt_le_audio_ascs_cp_codec_cfg_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CP_CODEC_CFG_IND(conn_handle %d, ase_id 0x%x) \r\n",
				param->conn_handle, param->ase_id);
		BT_LOGD("[APP] type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu 0x%x, octets_per_codec_frame 0x%x, audio_channel_allocation 0x%x)\r\n",
				param->codec_cfg.type_exist, param->codec_cfg.frame_duration, param->codec_cfg.sample_frequency, param->codec_cfg.codec_frame_blocks_per_sdu,
				param->codec_cfg.octets_per_codec_frame, (unsigned int)param->codec_cfg.audio_channel_allocation);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_CP_QOS_CFG_IND: {
		rtk_bt_le_audio_ascs_cp_qos_cfg_ind_t *param = (rtk_bt_le_audio_ascs_cp_qos_cfg_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CP_QOS_CFG_IND(conn_handle %d, ase_id 0x%x, cig_id 0x%x, cis_id 0x%x \r\n",
				param->conn_handle, param->ase_id, param->cig_id, param->cis_id);
		BT_LOGD("[APP] framing 0x%x, phy 0x%x, retransmission_number 0x%x, max_sdu 0x%x, max_transport_latency 0x%x, sdu_interval  0x%x, presentation_delay 0x%x)\r\n",
				param->framing, param->phy, param->retransmission_number, param->max_sdu,
				param->max_transport_latency, (unsigned int)param->sdu_interval, (unsigned int)param->presentation_delay);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_CP_ENABLE_IND: {
		rtk_bt_le_audio_ascs_cp_enable_ind_t *param = (rtk_bt_le_audio_ascs_cp_enable_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CP_ENABLE_IND(conn_handle %d, ase_num 0x%x\r\n",
				param->conn_handle, param->ase_num);
#if 0
		BT_DUMPA("ase id: ", param->ase_id, param->ase_num);
#endif
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_CP_DISABLE_IND: {
		rtk_bt_le_audio_ascs_cp_disable_ind_t *param = (rtk_bt_le_audio_ascs_cp_disable_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CP_DISABLE_IND(conn_handle %d, ase_num 0x%x\r\n",
				param->conn_handle, param->ase_num);
#if 0
		BT_DUMPA("ase id: ", param->ase_id, param->ase_num);
#endif
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_GET_PREFER_QOS_IND: {
		rtk_bt_le_audio_ascs_get_prefer_qos_ind_t *param = (rtk_bt_le_audio_ascs_get_prefer_qos_ind_t *)data;
		BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_GET_PREFER_QOS_IND: conn_handle %d, ase_id 0x%x, direction 0x%x, target_latency 0x%x, target_phy 0x%x,",
				param->conn_handle, param->ase_id, param->direction, param->target_latency, param->target_phy);
		BT_LOGD("[APP] codec_cfg.type_exist 0x%x, codec_cfg.frame_duration 0x%x,codec_cfg.sample_frequency 0x%x, codec_cfg.codec_frame_blocks_per_sdu 0x%x, codec_cfg.octets_per_codec_frame 0x%x,codec_cfg.audio_channel_allocation 0x%x\r\n",
				param->codec_cfg.type_exist, param->codec_cfg.frame_duration,
				param->codec_cfg.sample_frequency, param->codec_cfg.codec_frame_blocks_per_sdu, param->codec_cfg.octets_per_codec_frame,
				(unsigned int)param->codec_cfg.audio_channel_allocation);
		//The application can set these parameters,otherwise default values in upstack lib will be used.
		rtk_bt_le_audio_ascs_prefer_qos_data_t prefer_qos_data = {
			.supported_framing = RTK_BLE_AUDIO_UNFRAMED,
			.preferred_phy = 0,
			.preferred_retrans_number = 0,
			.max_transport_latency = 0,
			.presentation_delay_min = 0,
			.presentation_delay_max = 40000,
			.preferred_presentation_delay_min = 0,
			.preferred_presentation_delay_max = 0,
		};
		memcpy(param->p_prefer_qos_data, &prefer_qos_data, sizeof(rtk_bt_le_audio_ascs_prefer_qos_data_t));
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BASS_GET_PA_SYNC_PARAM_IND: { //BT api shall not be called here
		rtk_bt_le_audio_bass_pa_sync_param_t pa_sync_param = {
			.pa_sync_options = 0,
			.pa_sync_skip = 0,
			.pa_sync_timeout = APP_BT_LE_AUDIO_SYNC_TIMEOUT,
			.past_timeout = APP_BT_LE_AUDIO_SYNC_TIMEOUT
		};
		rtk_bt_le_audio_bass_get_pa_sync_param_ind_t *param = (rtk_bt_le_audio_bass_get_pa_sync_param_ind_t *)data;
		BT_LOGA("[APP] broadcast sink get pa sync param ind: sync_handle: %08x,source_id %d,is_past: %d,pa_interval %d\r\n",
				param->sync_handle, param->source_id, param->is_past, param->pa_interval);
		memcpy(param->p_pa_sync_param, &pa_sync_param, sizeof(rtk_bt_le_audio_bass_pa_sync_param_t));
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BASS_GET_BIG_SYNC_PARAM_IND: { //BT api shall not be called here
		rtk_bt_le_audio_bass_big_sync_param_t big_sync_param = {
			.big_mse = 0,
			.big_sync_timeout = APP_BT_LE_AUDIO_SYNC_TIMEOUT
		};
		rtk_bt_le_audio_bass_get_big_sync_param_ind_t *param = (rtk_bt_le_audio_bass_get_big_sync_param_ind_t *)data;
		BT_LOGA("[APP] broadcast sink get big sync param ind: sync_handle: %08x,source_id %d\r\n", param->sync_handle, param->source_id);
		memcpy(param->p_big_sync_param, &big_sync_param, sizeof(rtk_bt_le_audio_bass_big_sync_param_t));
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ISO_DATA_RECEIVE_IND: {
		rtk_bt_le_audio_direct_iso_data_ind_t *p_bt_direct_iso = (rtk_bt_le_audio_direct_iso_data_ind_t *)data;
		if (p_bt_direct_iso->iso_sdu_len) {
			BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_ISO_DATA_RECEIVE_IND, sys_time=%d,iso_conn_handle 0x%x, pkt_status_flag 0x%x, pkt_seq_num 0x%x, ts_flag 0x%x, time_stamp 0x%x,iso_sdu_len 0x%x, p_buf %08x, buf_len %d, offset %d\r\n",
					(int)osif_sys_time_get(), p_bt_direct_iso->iso_conn_handle, p_bt_direct_iso->pkt_status_flag,
					p_bt_direct_iso->pkt_seq_num, p_bt_direct_iso->ts_flag,
					(unsigned int)p_bt_direct_iso->time_stamp, p_bt_direct_iso->iso_sdu_len, p_bt_direct_iso->p_buf, p_bt_direct_iso->buf_len, p_bt_direct_iso->offset);
#if 0
			{
				uint16_t cur_idx = UINT16_MAX;
				static uint16_t last_pkt_num[APP_LE_AUDIO_DEMO_DATA_PATH_NUM] = {0};
				static uint16_t conn_hdl[APP_LE_AUDIO_DEMO_DATA_PATH_NUM] = {0};
				for (uint16_t i = 0; i < APP_LE_AUDIO_DEMO_DATA_PATH_NUM; i++) {
					if (conn_hdl[i] == 0) {
						conn_hdl[i] = p_bt_direct_iso->iso_conn_handle;
						cur_idx = i;
						break;
					} else if (conn_hdl[i] == p_bt_direct_iso->iso_conn_handle) {
						cur_idx = i;
						break;
					}
				}
				if (cur_idx >= APP_LE_AUDIO_DEMO_DATA_PATH_NUM) {
					BT_LOGE("[APP] Error: No available space for conn_hdl: 0x%x\r\n",
							p_bt_direct_iso->iso_conn_handle);
					break;
				}
				uint16_t expected_seq = (last_pkt_num[cur_idx] == UINT16_MAX)
										? 0
										: last_pkt_num[cur_idx] + 1;
				if (p_bt_direct_iso->pkt_seq_num != expected_seq) {
					BT_LOGE("[APP] Data loss: conn_handle: 0x%x, cur_seq_num: %u, last_seq_num: %u\r\n",
							p_bt_direct_iso->iso_conn_handle,
							p_bt_direct_iso->pkt_seq_num,
							last_pkt_num[cur_idx]);
				}
				last_pkt_num[cur_idx] = p_bt_direct_iso->pkt_seq_num;
			}
#endif
			if (app_bt_le_audio_data_received(p_bt_direct_iso->iso_conn_handle, RTK_BLE_AUDIO_ISO_DATA_PATH_RX,
											  p_bt_direct_iso->p_buf + p_bt_direct_iso->offset,
											  p_bt_direct_iso->iso_sdu_len)) {
				BT_LOGE("[APP] %s app le audio data parsing fail \r\n", __func__);
				break;
			}
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_PA_SYNC_STATE_IND: {
		rtk_bt_le_audio_pa_sync_state_ind_t *param = (rtk_bt_le_audio_pa_sync_state_ind_t *)data;
		BT_LOGA("[APP] broadcast %s pa sync state change: sync_handle: %08x, sync_state 0x%x, action 0x%x, cause: 0x%x\r\n",
				(bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) ? "sink" : "assistant", param->sync_handle, param->sync_state, param->action, param->cause);
		BT_AT_PRINT("+BLEBAP:broadcast,%s,sync_state,%p,0x%x,0x%x,0x%x\r\n",
					(bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) ? "sink" : "assistant",
					param->sync_handle, param->sync_state, param->action, param->cause);
		if (param->sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZED) {
			BT_LOGA("[APP] broadcast %s pa sync synchronized\r\n", (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) ? "sink" : "assistant");
			if (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI) {
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
				if (bt_le_audio_demo_ext_scan_timer) {
					osif_timer_stop(&bt_le_audio_demo_ext_scan_timer);
				}
#endif
				/* stop ext scan */
				rtk_bt_le_audio_ext_scan_act(false);
			}
		} else if (param->sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZING_WAIT_SCANNING) {
			BT_LOGA("[APP] broadcast %s pa sync synchronizing wait scanning \r\n", (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) ? "sink" : "assistant");
			if (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI) {
				/* start ext scan */
				rtk_bt_le_audio_ext_scan_act(true);
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
				if (bt_le_audio_demo_ext_scan_timer) {
					bt_le_audio_demo_ext_scan_time_remaining = APP_LE_AUDIO_EXT_SCAN_TIMER_COUNT;
					if (false == osif_timer_start(&bt_le_audio_demo_ext_scan_timer)) {
						BT_LOGE("[APP] %s osif_timer_start fail \r\n", __func__);
					}
					BT_LOGA("[APP] %s: ext scan timer start\r\n", __func__);
				}
#endif
			}
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BIG_SETUP_DATA_PATH: {
		rtk_bt_le_audio_big_setup_data_path_ind_t *param = (rtk_bt_le_audio_big_setup_data_path_ind_t *)data;
		BT_LOGD("[APP] broadcast sink big setup data path ind: bis_conn_handle: %08x, idx 0x%x, cause: 0x%x\r\n",
				param->bis_conn_handle, param->bis_idx, param->cause);
		app_bt_le_audio_add_data_path(param->iso_chann_t.iso_conn_handle,
									  param->iso_chann_t.p_iso_chann,
									  param->iso_chann_t.path_direction,
									  param->codec_t);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BIG_REMOVE_DATA_PATH: {
		rtk_bt_le_audio_big_remove_data_path_ind_t *param = (rtk_bt_le_audio_big_remove_data_path_ind_t *)data;
		BT_LOGD("[APP] broadcast sink big setup data path ind: bis_conn_handle: %08x, idx 0x%x, cause: 0x%x\r\n",
				param->bis_conn_handle, param->bis_idx, param->cause);
		app_bt_le_audio_remove_data_path(param->bis_conn_handle, param->path_direction);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BAP_SETUP_DATA_PATH_IND: {
		rtk_bt_le_audio_bap_setup_data_path_ind_t *param = (rtk_bt_le_audio_bap_setup_data_path_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BAP_SETUP_DATA_PATH_IND(group handle %08x, session handle %08x, dev_handle %08x, ase_id %d,path_direction=%d,cis_conn_handle=0x%x,dev_num %d \r\n",
				param->group_handle, param->stream_session_handle, param->device_handle, param->ase_id, param->path_direction, param->cis_conn_handle, (int)param->dev_num);
		BT_AT_PRINT("+BLEBAP:unicast,client,start,%p,%p,%p,%d,%d,0x%x\r\n",
					param->group_handle, param->stream_session_handle, param->device_handle,
					param->ase_id, param->path_direction, param->cis_conn_handle);
		BT_LOGD("[APP] type_exist 0x%x, frame_duration %d, sample_frequency %d, codec_frame_blocks_per_sdu %d,octets_per_codec_frame=%d,audio_channel_allocation=0x%x)\r\n",
				param->codec_parsed_data.type_exist, param->codec_parsed_data.frame_duration, param->codec_parsed_data.sample_frequency,
				param->codec_parsed_data.codec_frame_blocks_per_sdu, param->codec_parsed_data.octets_per_codec_frame,
				(unsigned int)param->codec_parsed_data.audio_channel_allocation);
		app_bt_le_audio_add_data_path(param->iso_chann_t.iso_conn_handle,
									  param->iso_chann_t.p_iso_chann,
									  param->iso_chann_t.path_direction,
									  param->codec_parsed_data);
		if (param->iso_chann_t.path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
			if (app_bt_le_audio_find_total_path_num(RTK_BLE_AUDIO_ISO_DATA_PATH_TX) == param->dev_num) {
				app_bt_le_audio_cap_encode_data_control(true);
				app_bt_le_audio_send_timer_update((param->codec_parsed_data.frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500);
			}
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BAP_REMOVE_DATA_PATH_IND: {
		rtk_bt_le_audio_bap_remove_data_path_ind_t *param = (rtk_bt_le_audio_bap_remove_data_path_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BAP_REMOVE_DATA_PATH_IND(group handle %08x, session handle %08x, dev_handle %08x, ase_id %d,path_direction=%d,cis_conn_handle=0x%x,cause=0x%x)\r\n",
				param->group_handle, param->stream_session_handle, param->device_handle, param->ase_id, param->path_direction, param->cis_conn_handle, param->cause);
		BT_AT_PRINT("+BLEBAP:unicast,client,stop,%p,%p,%p,%d,%d,0x%x,0x%x\r\n",
					param->group_handle, param->stream_session_handle, param->device_handle,
					param->ase_id, param->path_direction, param->cis_conn_handle, param->cause);
		if (param->path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
			app_bt_le_audio_cap_encode_data_control(false);
		}
		app_bt_le_audio_remove_data_path(param->cis_conn_handle, param->path_direction);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_GROUP_DEV_MSG_IND: {
		rtk_bt_le_audio_group_dev_msg_ind_t *param = (rtk_bt_le_audio_group_dev_msg_ind_t *)data;
		BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_GROUP_DEV_MSG_IND:msg_type 0x%x, cause 0x%x, group_handle %08x, dev_handle %08x\r\n",
				param->msg_type, param->cause, param->group_handle, param->device_handle);
		switch (param->msg_type) {
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_CONN:
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_CONN\r\n");
			break;
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_DISCONN:
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
			ret = rtk_bt_le_audio_csis_set_coordinator_cfg_discover(param->group_handle, true, RTK_BLE_AUDIO_DEFAULT_CSIS_DISV_TIMEOUT);
			BT_LOGA("[APP] %s: start csis discover in csis group %s (group_handle=%08x) \r\n", __func__, (RTK_BT_OK != ret) ? "fail" : "ok", param->group_handle);
			csip_discover_flag = true;
			app_bt_le_audio_scan_dev_list_remove_all();
			// transfer to ext scanning state
			ret = rtk_bt_le_audio_ext_scan_act(true);
			BT_LOGA("[APP] %s: start ext scan in csis group(%08x) %s \r\n", __func__, param->group_handle, (RTK_BT_OK != ret) ? "fail" : "ok");
			// set ext scan time out
			if (bt_le_audio_demo_ext_scan_timer) {
				bt_le_audio_demo_ext_scan_time_remaining = APP_LE_AUDIO_EXT_SCAN_TIMER_COUNT;
				if (false == osif_timer_start(&bt_le_audio_demo_ext_scan_timer)) {
					BT_LOGE("[APP] %s osif_timer_start fail \r\n", __func__);
				}
				BT_LOGA("[APP] %s: ext scan timer start\r\n", __func__);
			}
#else
			(void)ret;
#endif
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_DISCONN\r\n");
			break;
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_REMOVE:
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_REMOVE\r\n");
			break;
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_CLEAR:
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_CLEAR\r\n");
			break;
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_EMPTY:
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_EMPTY\r\n");
			break;
		default:
			BT_LOGE("[APP] %s unsupport group dev msg 0x%x\r\n", __func__, param->msg_type);
			break;
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BROADCAST_SOURCE_STATE_IND: {
		rtk_bt_le_audio_broadcast_source_state_ind_t *param = (rtk_bt_le_audio_broadcast_source_state_ind_t *)data;
		BT_LOGD("[APP] broadcast source state change: broadcast_source_handle: %08x, broadcast_source_state 0x%x, cause: 0x%x\r\n",
				param->broadcast_source_handle, param->broadcast_source_state, param->cause);
		BT_AT_PRINT("+BLEBAP:broadcast,source,state,%p,0x%x,0x%x\r\n",
					param->broadcast_source_handle, param->broadcast_source_state, param->cause);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BROADCAST_SOURCE_SETUP_DATA_PATH_IND: {
		rtk_bt_le_audio_bap_big_setup_data_path_ind_t *param = (rtk_bt_le_audio_bap_big_setup_data_path_ind_t *)data;
		BT_LOGA("[APP] broadcast setup path indication: bis_idx: 0x%x, bis_conn_handle 0x%x, cause: 0x%x\r\n",
				param->bis_idx, param->bis_conn_handle, param->cause);
		BT_LOGA("[APP] broadcast setup path indication: iso_conn_handle: 0x%x, path_direction 0x%x, cause: 0x%x\r\n",
				param->iso_chann_t.iso_conn_handle, param->iso_chann_t.path_direction, param->cause);

		BT_AT_PRINT("+BLEBAP:setup path bis_idx,bis_conn_handle,cause,0x%x,0x%x,0x%x\r\n",
					param->bis_idx, param->bis_conn_handle, param->cause);
		app_bt_le_audio_add_data_path(param->iso_chann_t.iso_conn_handle,
									  param->iso_chann_t.p_iso_chann,
									  param->iso_chann_t.path_direction,
									  param->codec_t);
		if (param->iso_chann_t.path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
			app_bt_le_audio_cap_encode_data_control(true);
			app_bt_le_audio_send_timer_update((param->codec_t.frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500);
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BROADCAST_SOURCE_REMOVE_DATA_PATH_IND: {
		rtk_bt_le_audio_bap_big_remove_data_path_ind_t *param = (rtk_bt_le_audio_bap_big_remove_data_path_ind_t *)data;
		BT_LOGA("[APP] broadcast remove path indication: bis_conn_handle 0x%x, cause: 0x%x\r\n",
				param->bis_conn_handle, param->cause);
		BT_AT_PRINT("+BLEBAP:remove path bis_idx,bis_conn_handle,cause,0x%x,0x%x,0x%x\r\n",
					param->bis_conn_handle, param->cause);
		if (param->path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
			app_bt_le_audio_cap_encode_data_control(false);
		}
		app_bt_le_audio_remove_data_path(param->bis_conn_handle, param->path_direction);
		break;
	}

	default:
		BT_LOGE("[APP] default evt_code 0x%04x\r\n", evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

/* Common Audio Profile APP Callback */
static rtk_bt_evt_cb_ret_t app_bt_cap_callback(uint8_t evt_code, void *data, uint32_t len)
{
	(void)len;

	switch (evt_code) {

	case RTK_BT_LE_AUDIO_EVT_CAP_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_cap_discovery_done_ind_t *param = (rtk_bt_le_audio_cap_discovery_done_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_CAP_DISCOVERY_DONE_IND: conn_handle 0x%x, load_from_ftl %d, cas_is_found %d, cas_inc_csis %d, vcs_is_found %d, mics_is_found %d\r\n",
				param->conn_handle, param->load_from_ftl, param->cas_is_found, param->cas_inc_csis, param->vcs_is_found, param->mics_is_found);
		break;
	}
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
	case RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_csis_client_discovery_done_ind_t *param = (rtk_bt_le_audio_csis_client_discovery_done_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_DISCOVERY_DONE_IND: conn_handle %d,is_found %d, load_from_ftl %d, srv_num %d, group_idx 0x%x \r\n",
				param->conn_handle, param->is_found, param->load_from_ftl, param->srv_num, param->group_idx);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_READ_RESULT_IND: {
		uint16_t ret = RTK_BT_FAIL;
		rtk_bt_le_audio_csis_client_read_result_ind_t *param = (rtk_bt_le_audio_csis_client_read_result_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_READ_RESULT_IND: cause 0x%x, conn_handle %d, group_handle 0x%08x, dev_handle 0x%08x, group_idx 0x%x\r\n",
				param->cause, param->conn_handle, param->group_handle, param->dev_handle, param->group_idx);
		BT_LOGA("[APP] srv_instance_id %d, char_exit 0x%x, srv_uuid 0x%x, rank %d, set_mem_size %d\r\n",
				param->mem_info.srv_instance_id, param->mem_info.char_exit, param->mem_info.srv_uuid, param->mem_info.rank, param->mem_info.set_mem_size);
		if (param->cause == 0) {
			if (param->group_handle == NULL) {//the server isnt in any group
				if (param->mem_info.set_mem_size > 1) {
					app_bt_le_audio_scan_dev_list_remove_all();
					csip_discover_flag = true;
					ret = rtk_bt_le_audio_ext_scan_act(true);
					BT_LOGA("[APP] %s: start ext scan %s \r\n", __func__, (RTK_BT_OK != ret) ? "fail" : "ok");
					if (bt_le_audio_demo_ext_scan_timer) {
						bt_le_audio_demo_ext_scan_time_remaining = APP_LE_AUDIO_EXT_SCAN_TIMER_COUNT;
						if (false == osif_timer_start(&bt_le_audio_demo_ext_scan_timer)) {
							BT_LOGE("[APP] %s osif_timer_start fail \r\n", __func__);
						}
						BT_LOGA("[APP] %s: ext scan timer start\r\n", __func__);
					}
				}
			}
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SEARCH_DONE_IND: {
		uint16_t ret = RTK_BT_FAIL;
		rtk_bt_le_audio_csis_client_search_done_ind_t *param = (rtk_bt_le_audio_csis_client_search_done_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SEARCH_DONE_IND: group_handle 0x%08x, set_mem_size %d, search_done %d,search_timeout %d\r\n",
				param->group_handle, param->set_mem_size, param->search_done, param->search_timeout);
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
		csip_discover_flag = false;
		if (bt_le_audio_demo_ext_scan_timer) {
			osif_timer_stop(&bt_le_audio_demo_ext_scan_timer);
		}
#endif
		ret = rtk_bt_le_audio_ext_scan_act(false);
		BT_LOGA("[APP] %s: stop ext scan in csis group(%08x) %s \r\n", __func__, param->group_handle, (RTK_BT_OK != ret) ? "fail" : "ok");
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SET_MEM_FOUND_IND: {
		char le_addr[30] = {0};
		uint16_t ret = RTK_BT_FAIL;
		rtk_bt_le_audio_csis_client_set_mem_found_ind_t *param = (rtk_bt_le_audio_csis_client_set_mem_found_ind_t *) data;
		rtk_bt_le_addr_to_str(&(param->bd_addr), le_addr, sizeof(le_addr));
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SET_MEM_FOUND_IND: group_handle %08x, dev_handle %08x, bd_addr %s, addr_type %d, srv_uuid 0x%x, rank %d, set_mem_size %d\r\n",
				param->group_handle, param->dev_handle, le_addr, param->bd_addr.type,
				param->srv_uuid, param->rank, param->set_mem_size);
		// auto connect to new member
		rtk_bt_le_create_conn_param_t conn_param;
		memcpy(&conn_param, &bt_le_audio_demo_conn_param, sizeof(rtk_bt_le_create_conn_param_t));
		conn_param.peer_addr.type = (rtk_bt_le_addr_type_t)param->bd_addr.type;
		memcpy((void *)conn_param.peer_addr.addr_val, (void *)param->bd_addr.addr_val, RTK_BD_ADDR_LEN);
		ret = rtk_bt_le_gap_connect(&conn_param);
		BT_LOGA("[APP] %s: GAP connecting to device...\r\n", __func__);
		if (ret) {
			BT_LOGE("[APP] %s connect failed\r\n", __func__);
			break;
		}
		break;
	}
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
	case RTK_BT_LE_AUDIO_EVT_MCP_SERVER_WRITE_MEDIA_CP_IND: {
		rtk_bt_le_audio_mcp_server_write_media_cp_ind_t *p_ind = (rtk_bt_le_audio_mcp_server_write_media_cp_ind_t *)data;
		BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_MCP_SERVER_WRITE_MEDIA_CP_IND: conn_handle %d, cid %d, service_id %d, opcode 0x%x\r\n",
				p_ind->conn_handle, p_ind->cid, p_ind->service_id, p_ind->opcode);
		BT_AT_PRINT("+BLECAP:initiator,mcp,write,%d,%d,0x%x\r\n",
					p_ind->conn_handle, p_ind->service_id, p_ind->opcode);
		switch (p_ind->opcode) {
		case RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PLAY:
			BT_LOGA("[APP] play\r\n");
			break;
		case RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE:
			BT_LOGA("[APP] pause\r\n");
			break;
		case RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_STOP:
			BT_LOGA("[APP] stop\r\n");
			break;
		case RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FAST_REWIND:
			BT_LOGA("[APP] fast rewind\r\n");
			break;
		case RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FAST_FORWARD:
			BT_LOGA("[APP] fast forward\r\n");
			break;
		case RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_MOVE_RELATIVE:
			BT_LOGA("[APP] move relative offset %d\r\n", (int)p_ind->param.move_relative_opcode_offset);
			break;
		case RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_SEGMENT:
			BT_LOGA("[APP] goto segment %d\r\n", (int)p_ind->param.goto_segment_opcode_n);
			break;
		case RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PREVIOUS_TRACK:
			BT_LOGA("[APP] previous track\r\n");
			break;
		case RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_NEXT_TRACK:
			BT_LOGA("[APP] next track\r\n");
			break;
		case RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FIRST_TRACK:
			BT_LOGA("[APP] first track\r\n");
			break;
		case RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_LAST_TRACK:
			BT_LOGA("[APP] last track\r\n");
			break;
		case RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_TRACK:
			BT_LOGA("[APP] go to track %d\r\n", (int)p_ind->param.goto_track_opcode_n);
			break;
		case RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_GROUP:
			BT_LOGA("[APP] go to group %d\r\n", (int)p_ind->param.goto_group_opcode_n);
			break;
		default:
			BT_LOGE("[APP] %s unprocess op code 0x%x for MCS server\r\n", __func__, p_ind->opcode);
			break;
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_MCP_SERVER_READ_IND: {
		rtk_bt_le_audio_mcp_server_read_confirm_param_t cfm_param = {0};
		rtk_bt_le_audio_mcp_server_read_ind_t *p_ind = (rtk_bt_le_audio_mcp_server_read_ind_t *)data;
		BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_MCP_SERVER_READ_IND: conn_handle %d, cid %d, service_id %d, char_uuid 0x%x, offset 0x%x\r\n",
				p_ind->conn_handle, p_ind->cid, p_ind->service_id, p_ind->char_uuid, p_ind->offset);
		BT_AT_PRINT("+BLECAP:initiator,mcp,read,%d,%d,0x%x\r\n",
					p_ind->conn_handle, p_ind->service_id, p_ind->char_uuid);
		switch (p_ind->char_uuid) {
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME:
			BT_LOGA("[APP] read char media player name\r\n");
			cfm_param.param.media_player_name.p_media_player_name = (uint8_t *)bt_le_audio_media_player_name;
			cfm_param.param.media_player_name.media_player_name_len = strlen(bt_le_audio_media_player_name);
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE:
			BT_LOGA("[APP] read track title\r\n");
			cfm_param.param.track_title.p_track_title = (uint8_t *)bt_le_audio_media_track_title;
			cfm_param.param.track_title.track_title_len = strlen(bt_le_audio_media_track_title);
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION:
			BT_LOGA("[APP] read track duration\r\n");
			/* APP Specificed */
			cfm_param.param.track_duration = 0;
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION:
			BT_LOGA("[APP] read track position\r\n");
			/* APP Specificed */
			cfm_param.param.track_position = 0;
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID:
			BT_LOGA("[APP] read content control id\r\n");
			cfm_param.param.content_control_id = RTK_BT_LE_AUDIO_GMCS_CCID;
			break;
		default:
			BT_LOGE("[APP] %s unprocess char_uuid 0x%x for MCS server\r\n", __func__, p_ind->char_uuid);
			return RTK_BT_EVT_CB_REJECT;
		}
		memcpy((void *)p_ind->p_cfm, (void *)&cfm_param, sizeof(rtk_bt_le_audio_mcp_server_read_confirm_param_t));
		break;
	}
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
	case RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_vcs_client_discovery_done_ind_t *param = (rtk_bt_le_audio_vcs_client_discovery_done_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_DISCOVERY_DONE_IND: conn_handle %d,is_found %d,load_from_ftl %d,type_exist 0x%x\r\n",
				param->conn_handle, param->is_found, param->load_from_ftl, param->type_exist);
		if (param->type_exist & RTK_BT_LE_AUDIO_VCS_VOLUME_STATE_FLAG) {
			rtk_bt_le_audio_vcs_volume_state_t volume_state = {0};
			if (!rtk_bt_le_audio_vcs_get_volume_state(param->conn_handle, &volume_state)) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_CAP_DISCOVERY_DONE_IND: volume_setting %d, mute %d, change_counter 0x%x \r\n",
						volume_state.volume_setting, volume_state.mute, volume_state.change_counter);
			}
		}
		if (param->type_exist & RTK_BT_LE_AUDIO_VCS_VOLUME_FLAGS_FLAG) {
			uint8_t volume_flags;
			if (!rtk_bt_le_audio_vcs_get_volume_flags(param->conn_handle, &volume_flags)) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_CAP_DISCOVERY_DONE_IND: volume_flags 0x%x \r\n",
						volume_flags);
			}
		}
	}
	break;

	case RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_STATE_IND: {
		rtk_bt_le_audio_vcs_client_volume_state_ind_t *param = (rtk_bt_le_audio_vcs_client_volume_state_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_STATE_IND: conn_handle %d, is_notify 0x%x, volume_setting 0x%x, mute 0x%x, change_counter 0x%x\r\n",
				param->conn_handle, param->is_notify, param->volume_state.volume_setting, param->volume_state.mute, param->volume_state.change_counter);
		BT_AT_PRINT("+BLECAP:commander,vcp,volume_state,%d,0x%x,0x%x,0x%x,0x%x\r\n",
					param->conn_handle, param->is_notify, param->volume_state.volume_setting,
					param->volume_state.mute, param->volume_state.change_counter);
	}
	break;

	case RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_FLAG_IND: {
		rtk_bt_le_audio_vcs_client_volume_flag_ind_t *param = (rtk_bt_le_audio_vcs_client_volume_flag_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_FLAG_IND: conn_handle %d, is_notify 0x%x,volume_flags 0x%x\r\n",
				param->conn_handle, param->is_notify, param->volume_flags);
		BT_AT_PRINT("+BLECAP:commander,vcp,volume_flag,%d,0x%x,0x%x\r\n",
					param->conn_handle, param->is_notify, param->volume_flags);
	}
	break;
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
	case RTK_BT_LE_AUDIO_EVT_MICS_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_mics_client_discovery_done_ind_t *param = (rtk_bt_le_audio_mics_client_discovery_done_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MICS_CLIENT_DISCOVERY_DONE_IND: conn_handle %d,is_found %d,load_from_ftl %d,mic_mute 0x%x\r\n",
				param->conn_handle, param->is_found, param->load_from_ftl, param->mic_mute);
	}
	break;

	case RTK_BT_LE_AUDIO_EVT_MICS_CLIENT_NOTIFY_IND: {
		rtk_bt_le_audio_mics_client_notify_ind_t *param = (rtk_bt_le_audio_mics_client_notify_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MICS_CLIENT_NOTIFY_IND: conn_handle %d,mic_mute %d\r\n", param->conn_handle, param->mic_mute);
		BT_AT_PRINT("+BLECAP:commander,micp,notify,%d,%d\r\n",
					param->conn_handle, param->mic_mute);
	}
	break;
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	case RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_vocs_client_discovery_done_ind_t *param = (rtk_bt_le_audio_vocs_client_discovery_done_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_DISCOVERY_DONE_IND: conn_handle %d,is_found %d, load_from_ftl %d, srv_num %d\r\n",
				param->conn_handle, param->is_found, param->load_from_ftl, param->srv_num);
		for (uint8_t i = 0; i < param->srv_num; i++) {
			if (!param->load_from_ftl) {
				rtk_bt_le_audio_vocs_cfg_cccd(param->conn_handle, i,
											  RTK_BT_LE_AUDIO_VOCS_VOLUME_OFFSET_STATE_FLAG | RTK_BT_LE_AUDIO_VOCS_AUDIO_LOCATION_FLAG | RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_FLAG, true);
			}
		}
	}
	break;
	case RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_READ_RESULT_IND: {
		rtk_bt_le_audio_vocs_client_read_result_ind_t *param = (rtk_bt_le_audio_vocs_client_read_result_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_READ_RESULT_IND: conn_handle %d,srv_instance_id %d, type %d, cause 0x%x\r\n",
				param->conn_handle, param->srv_instance_id, param->type, param->cause);
		switch (param->type) {
		case RTK_BT_LE_AUDIO_VOCS_CHAR_OFFSET_STATE: {
			BT_LOGA("[APP] vocs char input state: srv_instance_id %d, volume_offset %d, change_counter %d\r\n",
					param->srv_instance_id, param->data.volume_offset.volume_offset, param->data.volume_offset.change_counter);
			BT_AT_PRINT("+BLECAP:commander,vocs,get_char,%d,%d,0x%x,%d,%d,%d\r\n",
						param->conn_handle, param->srv_instance_id, param->type, param->cause,
						param->data.volume_offset.volume_offset, param->data.volume_offset.change_counter);
			break;
		}
		case RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_LOCATION: {
			BT_LOGA("[APP] vocs char gain setting prop: srv_instance_id %d, audio_location %d\r\n",
					param->srv_instance_id, (unsigned int)param->data.audio_location);
			BT_AT_PRINT("+BLECAP:commander,vocs,get_char,%d,%d,0x%x,%d,%d\r\n",
						param->conn_handle, param->srv_instance_id, param->type, param->cause,
						(unsigned int)param->data.audio_location);
			break;
		}
		case RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_OUTPUT_DESC:
			BT_LOGA("[APP] vocs char output des: srv_instance_id %d, output_des_len %d, p_output_des %s\r\n",
					param->srv_instance_id, param->data.output_des.output_des_len, param->data.output_des.p_output_des);
			BT_AT_PRINT("+BLECAP:commander,vocs,get_char,%d,%d,0x%x,%d,%d,%s\r\n",
						param->conn_handle, param->srv_instance_id, param->type, param->cause,
						param->data.output_des.output_des_len, param->data.output_des.p_output_des);
			break;
		default:
			break;
		}
	}
	break;
	case RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_NOTIFY_IND: {
		rtk_bt_le_audio_vocs_client_notify_ind_t *param = (rtk_bt_le_audio_vocs_client_notify_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_NOTIFY_IND: conn_handle 0x%x, srv_instance_id %d, type=%d\r\n",
				param->conn_handle, param->srv_instance_id, param->type);
		BT_AT_PRINT("+BLECAP:commander,vocs,notify,0x%x,%d,%d\r\n",
					param->conn_handle, param->srv_instance_id, param->type);
	}
	break;
	case RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_CP_RESULT_IND: {
		rtk_bt_le_audio_vocs_client_cp_result_ind_t *param = (rtk_bt_le_audio_vocs_client_cp_result_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_CP_RESULT_IND: conn_handle 0x%x, cause 0x%x, srv_instance_id %d, cp_op=%d\r\n",
				param->conn_handle, param->cause, param->srv_instance_id, param->cp_op);
		BT_AT_PRINT("+BLECAP:commander,vocs,cp_result,0x%x,0x%x,%d,%d\r\n",
					param->conn_handle, param->cause, param->srv_instance_id, param->cp_op);
	}
	break;
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	case RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_aics_client_discovery_done_ind_t *param = (rtk_bt_le_audio_aics_client_discovery_done_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_DISCOVERY_DONE_IND: conn_handle %d,is_found %d, load_from_ftl %d, srv_num %d\r\n",
				param->conn_handle, param->is_found, param->load_from_ftl, param->srv_num);
		for (uint8_t i = 0; i < param->vcs_instance.instance_num; i++) {
			BT_LOGA("[APP] AICS for VCS: srv_instance_id %d enable CCCD\r\n", param->vcs_instance.instance_id[i]);
			rtk_bt_le_audio_aics_cfg_cccd(param->conn_handle, param->vcs_instance.instance_id[i],
										  RTK_BT_LE_AUDIO_AICS_INPUT_STATE_FLAG | RTK_BT_LE_AUDIO_AICS_INPUT_STATUS_FLAG | RTK_BT_LE_AUDIO_AICS_INPUT_DES_FLAG, true);
		}
		for (uint8_t i = 0; i < param->mics_instance.instance_num; i++) {
			BT_LOGA("[APP] AICS for MICS: srv_instance_id %d enable CCCD\r\n", param->vcs_instance.instance_id[i]);
			rtk_bt_le_audio_aics_cfg_cccd(param->conn_handle, param->vcs_instance.instance_id[i],
										  RTK_BT_LE_AUDIO_AICS_INPUT_STATE_FLAG | RTK_BT_LE_AUDIO_AICS_INPUT_STATUS_FLAG | RTK_BT_LE_AUDIO_AICS_INPUT_DES_FLAG, true);
		}
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_NOTIFY_IND: {
		rtk_bt_le_audio_aics_client_notify_ind_t *param = (rtk_bt_le_audio_aics_client_notify_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_NOTIFY_IND: conn_handle %d,srv_instance_id %d, type %d\r\n",
				param->conn_handle, param->srv_instance_id, param->type);
		BT_AT_PRINT("+BLECAP:commander,aics,notify,%d,%d,%d\r\n",
					param->conn_handle, param->srv_instance_id, param->type);
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_READ_RESULT_IND: {
		rtk_bt_le_audio_aics_client_read_result_ind_t *param = (rtk_bt_le_audio_aics_client_read_result_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_READ_RESULT_IND: conn_handle %d,srv_instance_id %d, type %d, cause 0x%x\r\n",
				param->conn_handle, param->srv_instance_id, param->type, param->cause);
		switch (param->type) {
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_STATE: {
			BT_LOGA("[APP] aics char input state: srv_instance_id %d, gain_setting %d, mute %d, gain_mode %d,change_counter %d\r\n",
					param->srv_instance_id, param->data.input_state.gain_setting, param->data.input_state.mute, param->data.input_state.gain_mode,
					param->data.input_state.change_counter);
			BT_AT_PRINT("+BLECAP:commander,aics,get_char,%d,%d,0x%x,%d,%d,%d,%d,%d\r\n",
						param->conn_handle, param->srv_instance_id, param->cause, param->type,
						param->data.input_state.gain_setting, param->data.input_state.mute,
						param->data.input_state.gain_mode, param->data.input_state.change_counter);
			break;
		}
		case RTK_BT_LE_AUDIO_AICS_CHAR_GAIN_SETTING_PROP: {
			BT_LOGA("[APP] aics char gain setting prop: srv_instance_id %d, gain_setting_units %d, gain_setting_min %d, gain_setting_max %d\r\n",
					param->srv_instance_id, param->data.setting_prop.gain_setting_units, param->data.setting_prop.gain_setting_min, param->data.setting_prop.gain_setting_max);
			BT_AT_PRINT("+BLECAP:commander,aics,get_char,%d,%d,0x%x,%d,%d,%d,%d\r\n",
						param->conn_handle, param->srv_instance_id, param->cause, param->type,
						param->data.setting_prop.gain_setting_units, param->data.setting_prop.gain_setting_min,
						param->data.setting_prop.gain_setting_max);
			break;
		}
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_TYPE:
			BT_LOGA("[APP] aics char input type: srv_instance_id %d, input type %d\r\n", param->srv_instance_id, param->data.input_type);
			BT_AT_PRINT("+BLECAP:commander,aics,get_char,%d,%d,0x%x,%d,%d\r\n",
						param->conn_handle, param->srv_instance_id, param->cause,
						param->type, param->data.input_type);
			break;
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_STATUS:
			BT_LOGA("[APP] aics char input status: srv_instance_id %d, input status %d\r\n", param->srv_instance_id, param->data.input_status);
			BT_AT_PRINT("+BLECAP:commander,aics,get_char,%d,%d,0x%x,%d,%d\r\n",
						param->conn_handle, param->srv_instance_id, param->cause,
						param->type, param->data.input_status);
			break;
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_DES:
			BT_LOGA("[APP] aics char input des: srv_instance_id %d, input_des_len %d, p_input_des %s\r\n",
					param->srv_instance_id, param->data.input_des.input_des_len, param->data.input_des.p_input_des);
			BT_AT_PRINT("+BLECAP:commander,aics,get_char,%d,%d,0x%x,%d,%d,%s\r\n",
						param->conn_handle, param->srv_instance_id, param->cause, param->type,
						param->data.input_des.input_des_len, param->data.input_des.p_input_des);
			break;
		default:
			break;
		}
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_CP_RESULT_IND: {
		rtk_bt_le_audio_aics_client_cp_result_ind_t *param = (rtk_bt_le_audio_aics_client_cp_result_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_CP_RESULT_IND: conn_handle %d,srv_instance_id %d, cp_op %d, cause 0x%x\r\n",
				param->conn_handle, param->srv_instance_id, param->cp_op, param->cause);
		BT_AT_PRINT("+BLECAP:commander,aics,cp_result,%d,%d,%d,0x%x\r\n",
					param->conn_handle, param->srv_instance_id, param->cp_op, param->cause);
		break;
	}
#endif
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
	case RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_mcp_client_discovery_done_ind_t *param = (rtk_bt_le_audio_mcp_client_discovery_done_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_DISCOVERY_DONE_IND(conn_handle %d,is_found %d, load_from_ftl %d, general_mcs %d, srv_num %d\r\n",
				param->conn_handle, param->is_found, param->load_from_ftl, param->general_mcs, param->srv_num);
		BT_AT_PRINT("+BLECAP:acceptor,mcp,dis,%d,%d,%d,%d,%d\r\n",
					param->conn_handle, param->is_found, param->load_from_ftl, param->general_mcs, param->srv_num);
		if (param->is_found && param->general_mcs) {
			rtk_bt_le_audio_mcs_client_read_char_value(param->conn_handle, 0, RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID, true);
			rtk_bt_le_audio_mcs_client_cfg_cccd(param->conn_handle, 0, true, 0xFFFFFFFF, true);
			rtk_bt_le_audio_mcs_client_read_char_value(param->conn_handle, 0, RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_STATE, true);
			rtk_bt_le_audio_mcs_client_read_char_value(param->conn_handle, 0, RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME, true);
		}
		if (param->general_mcs) {
			BT_LOGA("[APP] Discovery done for conn_handle %d\r\n", param->conn_handle);
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_READ_RESULT_IND: {
		rtk_bt_le_audio_mcp_client_read_result_ind_t *param = (rtk_bt_le_audio_mcp_client_read_result_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_READ_RESULT_IND(cause 0x%x, conn_handle %d, general_mcs %d, srv_instance_id %d, char_uuid 0x%x\r\n",
				param->cause, param->conn_handle, param->general_mcs, param->srv_instance_id, param->char_uuid);
		if (param->general_mcs) {
			switch (param->char_uuid) {
			case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME:
				BT_LOGA("[APP] media_player_name_len %d, p_media_player_name %s\r\n", param->data.media_player_name.media_player_name_len,
						param->data.media_player_name.p_media_player_name);
				BT_AT_PRINT("+BLECAP:acceptor,mcp,read,%d,%d,%d,0x%x,0x%x,%d,%s\r\n",
							param->conn_handle, param->srv_instance_id, param->general_mcs, param->cause,
							param->char_uuid, param->data.media_player_name.media_player_name_len,
							param->data.media_player_name.p_media_player_name);
				break;
			case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE:
				BT_LOGA("[APP] track_title_len %d, p_track_title %s\r\n", param->data.track_title.track_title_len, param->data.track_title.p_track_title);
				break;
			case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_CHANGED:
				BT_LOGA("[APP] track changed\n");
				break;
			case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION:
				BT_LOGA("[APP] track_duration %d\r\n", (int)param->data.track_duration);
				break;
			case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION:
				BT_LOGA("[APP] track_position %d\r\n", (int)param->data.track_position);
				break;
			case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_SEEKING_SPEED:
				BT_LOGA("[APP] seeking_speed %d\r\n", (int)param->data.seeking_speed);
				break;
			case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYBACK_SPEED:
				BT_LOGA("[APP] playback_speed %d\r\n", (int)param->data.playback_speed);
				break;
			case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYING_ORDER:
				BT_LOGA("[APP] playing_order %d\r\n", (int)param->data.playing_order);
				break;
			case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYING_ORDERS_SUPPORTED:
				BT_LOGA("[APP] playing_order support %d\r\n", (int)param->data.playing_orders_supported);
				break;
			case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_STATE:
				BT_LOGA("[APP] media_state 0x%x \r\n", param->data.media_state);
				break;
			case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODE_SUPPORTED:
				BT_LOGA("[APP] media control point opcode supported %d\r\n", (int)param->data.media_control_point_opcodes_supported);
				break;
			case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID:
				BT_LOGA("[APP] read content control id %d\r\n", param->data.content_control_id);
				break;
			default:
				break;
			}
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_NOTIFY_IND : {
		rtk_bt_le_audio_mcp_client_notify_ind_t *param = (rtk_bt_le_audio_mcp_client_notify_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_NOTIFY_IND: conn_handle %d, general_mcs %d, srv_instance_id %d, char_uuid 0x%x,\r\n",
				param->conn_handle, param->general_mcs, param->srv_instance_id, param->char_uuid);
		switch (param->char_uuid) {
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME:
			BT_LOGA("media_player_name %s\r\n", param->data.media_player_name.p_media_player_name);
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE:
			BT_LOGA("track_title_len %d, p_track_title %s\r\n", param->data.track_title.track_title_len, param->data.track_title.p_track_title);
			BT_AT_PRINT("+BLECAP:acceptor,mcp,notify,%d,%d,%d,0x%x,%d,%s\r\n",
						param->conn_handle, param->general_mcs, param->srv_instance_id, param->char_uuid,
						param->data.track_title.track_title_len, param->data.track_title.p_track_title);
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION:
			BT_LOGA("track_duration %d\r\n", (int)param->data.track_duration);
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION:
			BT_LOGA("track_position %d\r\n", (int)param->data.track_position);
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_CHANGED:
			BT_LOGA("track changed\r\n");
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_SEEKING_SPEED:
			BT_LOGA("seeking_speed %d\r\n", (int)param->data.seeking_speed);
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYBACK_SPEED:
			BT_LOGA("playback_speed %d\r\n", (int)param->data.playback_speed);
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYING_ORDER:
			BT_LOGA("playing_order %d\r\n", (int)param->data.playing_order);
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_STATE:
			BT_LOGA("media_state %x\r\n", param->data.media_state);
			break;
		default:
			BT_LOGE("unprocess char_uuid 0x%x\r\n", param->char_uuid);
			break;
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_MEDIA_CP_NOTIFY_IND: {
		rtk_bt_le_audio_mcs_media_cp_nodify_ind_t *param = (rtk_bt_le_audio_mcs_media_cp_nodify_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_MEDIA_CP_NOTIFY_IND conn_handle=0x%x, general_mcs=%d, srv_instance_id=%d, requested_opcode=0x%x, result_code=0x%x\r\n",
				param->conn_handle, param->general_mcs, param->srv_instance_id, param->requested_opcode, param->result_code);
		BT_AT_PRINT("+BLECAP:acceptor,mcp,write,0x%x,%d,%d,0x%x,0x%x\r\n",
					param->conn_handle, param->general_mcs, param->srv_instance_id, param->requested_opcode, param->result_code);

		break;
	}
#endif
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
	case RTK_BT_LE_AUDIO_EVT_VCS_VOLUME_CP_IND: {
		float audio_track_volume = 0;
		uint8_t target_volume = 0;
		rtk_bt_le_audio_vcs_volume_cp_ind_t *param = (rtk_bt_le_audio_vcs_volume_cp_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VCS_VOLUME_CP_IND: conn_handle %d, volume_setting 0x%x, mute %d,cp_op %d\r\n",
				param->conn_handle, param->volume_setting, param->mute, param->cp_op);
		BT_AT_PRINT("+BLECAP:acceptor,vcp,volume_cp,%d,0x%x,%d,%d\r\n",
					param->conn_handle, param->volume_setting, param->mute, param->cp_op);

		switch (param->cp_op) {
		case RTK_BT_LE_AUDIO_VCS_CP_RELATIVE_VOLUME_DOWN:
			BT_LOGA("[APP] Relative Volume Down\r\n");
			target_volume = MAX(param->volume_setting - RTK_BT_LE_AUDIO_DEFAULT_VCS_STEP_SIZE, 0);
			audio_track_volume = 1.0 * param->volume_setting / RTK_BT_LE_AUDIO_MAX_VCS_VOL_SETTING;
			rtk_bt_audio_track_set_hardware_volume(audio_track_volume, audio_track_volume);
			break;
		case RTK_BT_LE_AUDIO_VCS_CP_RELATIVE_VOLUME_UP:
			BT_LOGA("[APP] Relative Volume Up\r\n");
			target_volume = MIN(param->volume_setting + RTK_BT_LE_AUDIO_DEFAULT_VCS_STEP_SIZE, RTK_BT_LE_AUDIO_MAX_VCS_VOL_SETTING);
			audio_track_volume = 1.0 * target_volume / RTK_BT_LE_AUDIO_MAX_VCS_VOL_SETTING;
			rtk_bt_audio_track_set_hardware_volume(audio_track_volume, audio_track_volume);
			break;
		case RTK_BT_LE_AUDIO_VCS_CP_UNMUTE_RELATIVE_VOLUME_DOWN:
			BT_LOGA("[APP] Unmute/Relative Volume Down\r\n");
			target_volume = MAX(param->volume_setting - RTK_BT_LE_AUDIO_DEFAULT_VCS_STEP_SIZE, 0);
			audio_track_volume = 1.0 * target_volume / RTK_BT_LE_AUDIO_MAX_VCS_VOL_SETTING;
			rtk_bt_audio_track_set_hardware_volume(audio_track_volume, audio_track_volume);
			rtk_bt_audio_track_set_mute(false);
			break;
		case RTK_BT_LE_AUDIO_VCS_CP_UNMUTE_RELATIVE_VOLUME_UP:
			BT_LOGA("[APP] Unmute/Relative Volume Up\r\n");
			target_volume = MIN(param->volume_setting + RTK_BT_LE_AUDIO_DEFAULT_VCS_STEP_SIZE, RTK_BT_LE_AUDIO_MAX_VCS_VOL_SETTING);
			audio_track_volume = 1.0 * target_volume / RTK_BT_LE_AUDIO_MAX_VCS_VOL_SETTING;
			rtk_bt_audio_track_set_hardware_volume(audio_track_volume, audio_track_volume);
			rtk_bt_audio_track_set_mute(false);
			break;
		case RTK_BT_LE_AUDIO_VCS_CP_SET_ABSOLUTE_VOLUME:
			BT_LOGA("[APP] Set Absolute Volume,volume_setting=0x%x\r\n", param->volume_setting);
			audio_track_volume = 1.0 * param->volume_setting / RTK_BT_LE_AUDIO_MAX_VCS_VOL_SETTING;
			rtk_bt_audio_track_set_hardware_volume(audio_track_volume, audio_track_volume);
			break;
		case RTK_BT_LE_AUDIO_VCS_CP_UNMUTE:
			BT_LOGA("[APP] Unmute\r\n");
			rtk_bt_audio_track_set_mute(false);
			break;
		case RTK_BT_LE_AUDIO_VCS_CP_MUTE:
			BT_LOGA("[APP] Mute\r\n");
			rtk_bt_audio_track_set_mute(true);
			break;
		default:
			BT_LOGE("[APP] unknown cp_op %d\r\n", param->cp_op);
			break;
		}
		break;
	}
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
	case RTK_BT_LE_AUDIO_EVT_MICS_WRITE_MUTE_IND: {
		rtk_bt_le_audio_mics_write_mute_ind_t *param = (rtk_bt_le_audio_mics_write_mute_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MICS_WRITE_MUTE_IND: mic_mute %d\r\n", param->mic_mute);
		BT_AT_PRINT("+BLECAP:acceptor,micp,write_mute,%d\r\n", param->mic_mute);
		//call record api

		if (!app_bt_le_audio_check_audio_record_exist()) {
			BT_LOGE("[APP] %s: Audio record is not exist\r\n", __func__);
			break;
		}
		if (param->mic_mute == 0x01) {
			// muted
			rtk_bt_audio_record_set_mute(0, true);
			rtk_bt_audio_record_set_mute(1, true);
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MICS_WRITE_MUTE_IND: muted\r\n");
		} else if (param->mic_mute == 0x00) {
			// unmuted
			rtk_bt_audio_record_set_mute(0, false);
			rtk_bt_audio_record_set_mute(1, false);
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MICS_WRITE_MUTE_IND: unmuted\r\n");
		} else if (param->mic_mute == 0x02) {
			//muted disabled
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MICS_WRITE_MUTE_IND: set mute disabled\r\n");
		} else {
			BT_LOGE("[APP] RTK_BT_LE_AUDIO_EVT_MICS_WRITE_MUTE_IND: mic device set mute wrong cmd !!!\r\n");
		}
		break;
	}
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	case RTK_BT_LE_AUDIO_EVT_AICS_CP_IND: {
		uint16_t ret = RTK_BT_FAIL;
		rtk_bt_le_audio_aics_cp_ind_t *param = (rtk_bt_le_audio_aics_cp_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_AICS_CP_IND: conn_handle %d, srv_instance_id %d, cp_op 0x%x, gain_setting %d\r\n",
				param->conn_handle, param->srv_instance_id, param->cp_op, param->gain_setting);
		BT_AT_PRINT("+BLECAP:acceptor,acis,cp_ind,%d,%d,0x%x,%d\r\n",
					param->conn_handle, param->srv_instance_id, param->cp_op, param->gain_setting);
		//float audio_track_volume = 0;
		uint8_t cur_gain_mode = 0;
		uint8_t value_len = 0, *p_value = NULL;
		int8_t gain_setting = 0;
		uint8_t gain_setting_units = 0;
		// int8_t  gain_setting_min = 0, gain_setting_max = 0;
		switch (param->cp_op) {
		case RTK_BT_LE_AUDIO_AICS_CP_SET_GAIN_SETTING: {
			//check whether in app_lea_vcs_id_array or app_lea_mics_id_array
			if (param->srv_instance_id == RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX) {
				//check if current gain mode is manual mode
				value_len = sizeof(rtk_bt_le_audio_aics_input_state_t);
				p_value = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, value_len);
				if (p_value == NULL) {
					BT_LOGE("[APP] %s: osif_mem_alloc len %d fail\r\n", __func__, value_len);
					return RTK_BT_EVT_CB_REJECT;
				}
				ret = rtk_bt_le_audio_aics_get_param(param->srv_instance_id, RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATE, value_len, p_value);
				if (ret == RTK_BT_OK) {
					rtk_bt_le_audio_aics_input_state_t *p_input_state = (rtk_bt_le_audio_aics_input_state_t *)p_value;
					cur_gain_mode = p_input_state->gain_mode;
					gain_setting = p_input_state->gain_setting;
					if (cur_gain_mode == RTK_BT_LE_AUDIO_AICS_GAIN_MODE_AUTOMATIC_ONLY || cur_gain_mode == RTK_BT_LE_AUDIO_AICS_GAIN_MODE_AUTOMATIC) {
						BT_LOGE("[APP] %s: cannot set gain setting ! current gain mode: %d is automatic!\r\n", __func__, cur_gain_mode);
						break;
					}
				}
				osif_mem_free(p_value);
				//get current aics gain setting properties
				value_len = sizeof(rtk_bt_le_audio_aics_gain_setting_prop_t);
				p_value = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, value_len);
				if (p_value == NULL) {
					BT_LOGE("[APP] %s: osif_mem_alloc len %d fail\r\n", __func__, value_len);
					return RTK_BT_EVT_CB_REJECT;
				}
				ret = rtk_bt_le_audio_aics_get_param(param->srv_instance_id, RTK_BT_LE_AUDIO_AICS_PARAM_GAIN_SETTING_PROP, value_len, p_value);
				if (ret == RTK_BT_OK) {
					rtk_bt_le_audio_aics_gain_setting_prop_t *p_gain_setting = (rtk_bt_le_audio_aics_gain_setting_prop_t *)p_value;
					gain_setting_units = p_gain_setting->gain_setting_units;
					// gain_setting_min = p_gain_setting->gain_setting_min;
					// gain_setting_max = p_gain_setting->gain_setting_min;
				}
				// adjust audio track
				// target_volume = param->gain_setting
				// target_volume = MAX(param->volume_setting - RTK_BLE_AUDIO_DEFAULT_VCS_STEP_SIZE,0);
				// audio_track_volume = 1.0 * param->gain_setting  / RTK_BLE_AUDIO_MAX_VCS_VOL_SETTING;
				// rtk_bt_audio_track_set_hardware_volume(audio_track_volume,audio_track_volume);
			} else if (param->srv_instance_id == RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX) {
				if (!app_bt_le_audio_check_audio_record_exist()) {
					BT_LOGE("[APP] %s: Audio record is not exist\r\n", __func__);
					break;
				}
				//check if current gain mode is manual mode
				value_len = sizeof(rtk_bt_le_audio_aics_input_state_t);
				p_value = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, value_len);
				if (p_value == NULL) {
					BT_LOGE("[APP] %s: osif_mem_alloc len %d fail\r\n", __func__, value_len);
					return RTK_BT_EVT_CB_REJECT;
				}
				ret = rtk_bt_le_audio_aics_get_param(param->srv_instance_id, RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATE, value_len, p_value);
				if (ret == RTK_BT_OK) {
					rtk_bt_le_audio_aics_input_state_t *p_input_state = (rtk_bt_le_audio_aics_input_state_t *)p_value;
					cur_gain_mode = p_input_state->gain_mode;
					gain_setting = p_input_state->gain_setting;
					if (cur_gain_mode == RTK_BT_LE_AUDIO_AICS_GAIN_MODE_AUTOMATIC_ONLY || cur_gain_mode == RTK_BT_LE_AUDIO_AICS_GAIN_MODE_AUTOMATIC) {
						BT_LOGE("[APP] %s: cannot set gain setting ! current gain mode: %d is automatic!\r\n", __func__, cur_gain_mode);
						break;
					}
				}
				osif_mem_free(p_value);
				//get curent aics gain setting properties
				value_len = sizeof(rtk_bt_le_audio_aics_gain_setting_prop_t);
				p_value = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, value_len);
				if (p_value == NULL) {
					BT_LOGE("[APP] %s: osif_mem_alloc len %d fail\r\n", __func__, value_len);
					return RTK_BT_EVT_CB_REJECT;
				}
				ret = rtk_bt_le_audio_aics_get_param(param->srv_instance_id, RTK_BT_LE_AUDIO_AICS_PARAM_GAIN_SETTING_PROP, value_len, p_value);
				if (ret == RTK_BT_OK) {
					rtk_bt_le_audio_aics_gain_setting_prop_t *p_gain_setting = (rtk_bt_le_audio_aics_gain_setting_prop_t *)p_value;
					gain_setting_units = p_gain_setting->gain_setting_units;
					//gain_setting_min = p_gain_setting->gain_setting_min;
					//gain_setting_max = p_gain_setting->gain_setting_min;
				}
				int8_t gain_setting_db = 0;
				uint8_t lea_cur_mic_bst_gain = 0;
				//calculate the setting mic bst gain
				//ex. setting gain range [-100,100],gain unit 50, gain_setting = -100, default mic bst gain = 20db
				//gain_setting_db = (-100/50) * (50 * 0.1 db) = -10 db
				//current mic gain = default mic bst gain + gain_setting_db =  20db - 10db = 10db
				gain_setting_db = (gain_setting / (int8_t)gain_setting_units) * ((int8_t)gain_setting_units / 10);
				lea_cur_mic_bst_gain = rtk_bt_audio_record_get_mic_bst_gain(RTK_BT_AUDIO_AMIC1);
				lea_cur_mic_bst_gain = (lea_cur_mic_bst_gain * 5 + gain_setting_db) / 5;
				if (lea_cur_mic_bst_gain > RTK_BT_AUDIO_MICBST_GAIN_40DB) {
					BT_LOGE("[APP] %s: mic bst gain exceed the max 40 db !\r\n", __func__);
					break;
				}
				// set mic bst gain
				rtk_bt_audio_record_set_mic_bst_gain(RTK_BT_AUDIO_AMIC1, lea_cur_mic_bst_gain);
				BT_LOGA("[APP] Ameba MIC set bst gain success! current bst gain is %d db\r\n", 5 * lea_cur_mic_bst_gain);
			}
			break;
		}
		case RTK_BT_LE_AUDIO_AICS_CP_UNMUTE: {
			if (param->srv_instance_id == RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX) {
				rtk_bt_audio_track_set_mute(false);
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_AICS_CP_MUTE: aics Ameba bluetooth set unmuted\r\n");
			} else if (param->srv_instance_id == RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX) {
				if (!app_bt_le_audio_check_audio_record_exist()) {
					BT_LOGE("[APP] %s: Audio record is not exist\r\n", __func__);
					break;
				}
				rtk_bt_audio_record_set_mute(0, false);
				rtk_bt_audio_record_set_mute(1, false);
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_AICS_CP_MUTE: aics Ameba MIC set unmuted\r\n");
			}
			break;
		}
		case RTK_BT_LE_AUDIO_AICS_CP_MUTE: {
			if (param->srv_instance_id == RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX) {
				rtk_bt_audio_track_set_mute(true);
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_AICS_CP_MUTE: aics Ameba bluetooth set muted\r\n");
			} else if (param->srv_instance_id == RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX) {
				if (!app_bt_le_audio_check_audio_record_exist()) {
					BT_LOGE("[APP] %s: Audio record is not exist\r\n", __func__);
					break;
				}
				//set mute
				rtk_bt_audio_record_set_mute(0, true);
				rtk_bt_audio_record_set_mute(1, true);
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_AICS_CP_MUTE: aics Ameba MIC set muted\r\n");
			}
			break;
		}
		case RTK_BT_LE_AUDIO_AICS_CP_SET_MANUAL_GAIN_MODE: {
			//check current gain mode if is mannul only or automatic only
			value_len = sizeof(rtk_bt_le_audio_aics_input_state_t);
			p_value = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, value_len);
			if (p_value == NULL) {
				BT_LOGE("[APP] %s: osif_mem_alloc len %d fail\r\n", __func__, value_len);
				return RTK_BT_EVT_CB_REJECT;
			}
			ret = rtk_bt_le_audio_aics_get_param(param->srv_instance_id, RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATE, value_len, p_value);
			if (ret == RTK_BT_OK) {
				rtk_bt_le_audio_aics_input_state_t *p_input_state = (rtk_bt_le_audio_aics_input_state_t *)p_value;
				cur_gain_mode = p_input_state->gain_mode;
				if (cur_gain_mode == RTK_BT_LE_AUDIO_AICS_GAIN_MODE_AUTOMATIC_ONLY || cur_gain_mode == RTK_BT_LE_AUDIO_AICS_GAIN_MODE_MANUAL_ONLY) {
					BT_LOGE("[APP] %s: set gain mode failed ! Setting gain mode is not supported for current gain mode: %d\r\n", __func__, cur_gain_mode);
					break;
				}
			}
			if (param->srv_instance_id == RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_AICS_CP_SET_MANUAL_GAIN_MODE: srv_instance_id %d set Manual Gain Mode success\r\n", param->srv_instance_id);
			} else if (param->srv_instance_id == RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_AICS_CP_SET_MANUAL_GAIN_MODE: srv_instance_id %d set Manual Gain Mode success\r\n", param->srv_instance_id);
			}
			break;
		}
		case RTK_BT_LE_AUDIO_AICS_CP_SET_AUTOMATIC_GAIN_MODE: {
			//check current gain mode if is mannul only or automatic only
			value_len = sizeof(rtk_bt_le_audio_aics_input_state_t);
			p_value = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, value_len);
			if (p_value == NULL) {
				BT_LOGE("[APP] %s: osif_mem_alloc len %d fail\r\n", __func__, value_len);
				return RTK_BT_EVT_CB_REJECT;
			}
			ret = rtk_bt_le_audio_aics_get_param(param->srv_instance_id, RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATE, value_len, p_value);
			if (ret == RTK_BT_OK) {
				rtk_bt_le_audio_aics_input_state_t *p_input_state = (rtk_bt_le_audio_aics_input_state_t *)p_value;
				cur_gain_mode = p_input_state->gain_mode;
				if (cur_gain_mode == RTK_BT_LE_AUDIO_AICS_GAIN_MODE_AUTOMATIC_ONLY || cur_gain_mode == RTK_BT_LE_AUDIO_AICS_GAIN_MODE_MANUAL_ONLY) {
					BT_LOGE("[APP] %s: set gain mode failed ! Setting gain mode is not supported for current gain mode: %d\r\n", __func__, cur_gain_mode);
					break;
				}
			}
			if (param->srv_instance_id == RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_AICS_CP_SET_AUTOMATIC_GAIN_MODE: srv_instance_id %d set Automatic Gain Mode success\r\n", param->srv_instance_id);
			} else if (param->srv_instance_id == RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_AICS_CP_SET_AUTOMATIC_GAIN_MODE: srv_instance_id %d set Automatic Gain Mode success\r\n", param->srv_instance_id);
			}
			break;
		}
		default: {
			BT_LOGE("[APP] %s default evt_code 0x%04x\r\n", __func__, param->cp_op);
			ret = RTK_BT_ERR_UNHANDLED;
			break;
		}
		}
		if (p_value) {
			osif_mem_free(p_value);
		}
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_AICS_WRITE_INPUT_DES_IND: {
		rtk_bt_le_audio_aics_write_input_des_ind_t *param = (rtk_bt_le_audio_aics_write_input_des_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_AICS_WRITE_INPUT_DES_IND: conn_handle %d, srv_instance_id %d,input_des_len %d,p_input_des %s\r\n",
				param->conn_handle, param->srv_instance_id, param->input_des.input_des_len, param->input_des.p_input_des);
		break;
	}
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	case RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_OFFSET_STATE_IND: {
		rtk_bt_le_audio_vocs_write_offset_state_ind_t *param = (rtk_bt_le_audio_vocs_write_offset_state_ind_t *)data;
		float left_volume = 0, right_volume = 0;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_OFFSET_STATE_IND: conn_handle 0x%x, srv_instance_id %d, volume_offset %d\r\n",
				param->conn_handle, param->srv_instance_id, param->volume_offset);
		BT_AT_PRINT("+BLECAP:acceptor,vocs,write_offset_state,0x%x,%d,%d\r\n",
					param->conn_handle, param->srv_instance_id, param->volume_offset);
		if (param->srv_instance_id == RTK_BT_LE_AUDIO_VOCS_IDX) {
			// set volume offset
			// volume_offset should in range [-255, 255]
			rtk_bt_audio_track_get_volume(&left_volume, &right_volume);
			left_volume += ((param->volume_offset) / RTK_BT_LE_AUDIO_VOCS_MAX_VAL) * 0.5;
			right_volume += ((param->volume_offset) / RTK_BT_LE_AUDIO_VOCS_MAX_VAL) * 0.5;
			if (left_volume >= 0 && right_volume >= 0) {
				left_volume = MIN(left_volume, 1);
				right_volume = MIN(right_volume, 1);
			} else {
				left_volume = MAX(0, left_volume);
				right_volume = MAX(0, right_volume);
			}
			rtk_bt_audio_track_set_hardware_volume(left_volume, right_volume);
			BT_LOGA("[APP] vocs set offset, left volume is %f, right volume is %f\r\n", left_volume, right_volume);
		}
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_AUDIO_LOCATION_IND: {
		rtk_bt_le_audio_vocs_write_audio_location_ind_t *param = (rtk_bt_le_audio_vocs_write_audio_location_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_AUDIO_LOCATION_IND: conn_handle 0x%x, srv_instance_id %d, audio_location 0x%x\r\n",
				param->conn_handle, param->srv_instance_id, (unsigned int)param->audio_location);
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_OUTPUT_DES_IND: {
		rtk_bt_le_audio_vocs_write_output_des_ind_t *param = (rtk_bt_le_audio_vocs_write_output_des_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_OUTPUT_DES_IND: conn_handle 0x%x, srv_instance_id %d,output_des_len %d, output_des %s\r\n",
				param->conn_handle, param->srv_instance_id, param->output_des.output_des_len, param->output_des.p_output_des);
		BT_AT_PRINT("+BLECAP:acceptor,vocs,write_output_des,0x%x,%d,%d,%s\r\n",
					param->conn_handle, param->srv_instance_id,
					param->output_des.output_des_len, param->output_des.p_output_des);
		break;
	}
#endif
	default:
		BT_LOGE("[APP] default evt_code 0x%04x\r\n", evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

/* tmap APP Callback */
static rtk_bt_evt_cb_ret_t app_bt_tmap_callback(uint8_t evt_code, void *data, uint32_t len)
{
	(void)len;
	uint16_t ret = RTK_BT_FAIL;

	switch (evt_code) {
	case RTK_BT_LE_AUDIO_EVT_TMAS_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_tmas_client_discovery_done_ind_t *param = (rtk_bt_le_audio_tmas_client_discovery_done_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_TMAS_CLIENT_DISCOVERY_DONE_IND: conn_handle %d,is_found %d, load_from_ftl %d\r\n",
				param->conn_handle, param->is_found, param->load_from_ftl);
		ret = rtk_bt_le_audio_tmas_client_read_role(param->conn_handle);
		if (ret) {
			BT_LOGE("[APP] %s tmas client read role failed\r\n", __func__);
			break;
		}
	}
	break;

	case RTK_BT_LE_AUDIO_EVT_TMAS_CLIENT_READ_ROLE_RESULT: {
		rtk_bt_le_audio_tmas_read_role_result_t *param = (rtk_bt_le_audio_tmas_read_role_result_t *) data;
		uint8_t tmap_role = (uint8_t)param->role;
		if (tmap_role & RTK_BT_LE_AUDIO_TMAP_ROLE_CT) {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_TMAS_CLIENT_READ_ROLE_RESULT: TMAP Call Terminal\r\n");
		}
		if (tmap_role & RTK_BT_LE_AUDIO_TMAP_ROLE_UMR) {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_TMAS_CLIENT_READ_ROLE_RESULT: TMAP Unicast Media Receiver\r\n");
		}
		if (tmap_role & RTK_BT_LE_AUDIO_TMAP_ROLE_BMR) {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_TMAS_CLIENT_READ_ROLE_RESULT: TMAP Broadcast Media Receiver\r\n");
		}
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_TMAS_CLIENT_READ_ROLE_RESULT: conn_handle %d,cause %d \r\n", param->conn_handle, param->cause);
	}
	break;

	default:
		BT_LOGE("[APP] default evt_code 0x%04x\r\n", evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t app_le_audio_common_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
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
		BT_LOGE("[APP] Unknown common gap cb evt type: %d\r\n", evt_code);
		break;
	}

	return ret;
}

/* Le Gap APP Callback */
static rtk_bt_evt_cb_ret_t app_bt_le_audio_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
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

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
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
		app_bt_le_audio_scan_dev_list_remove_all();
		BT_AT_PRINT("+BLEGAP:scan,start,%d,%d\r\n", (scan_start_ind->err == 0) ? 0 : -1, scan_start_ind->scan_type);
		break;
	}

	case RTK_BT_LE_GAP_EVT_SCAN_RES_IND: {
		rtk_bt_le_scan_res_ind_t *scan_res_ind = (rtk_bt_le_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->adv_report.addr), le_addr, sizeof(le_addr));
		BT_LOGA("[APP] Scan info, [Device]: %s, AD evt type: %d, RSSI: %d, len: %d \r\n",
				le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
				scan_res_ind->adv_report.len);
		BT_AT_PRINT("+BLEGAP:scan,info,%s,%d,%d,%d\r\n",
					le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
					scan_res_ind->adv_report.len);
		break;
	}

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	case RTK_BT_LE_GAP_EVT_EXT_SCAN_RES_IND: {
		rtk_bt_le_ext_scan_res_ind_t *scan_res_ind = (rtk_bt_le_ext_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->addr), le_addr, sizeof(le_addr));
#if 0
		BT_LOGA("[APP] Ext Scan info, [Device]: %s, AD evt type: 0x%x, RSSI: %d, PHY: 0x%x, TxPower: %d, Len: %d\r\n",
				le_addr, scan_res_ind->evt_type, scan_res_ind->rssi,
				(scan_res_ind->primary_phy << 4) | scan_res_ind->secondary_phy,
				scan_res_ind->tx_power, scan_res_ind->len);
#endif
		BT_AT_PRINT("+BLEGAP:escan,%s,0x%x,%d,0x%x,%d,%d\r\n",
					le_addr, scan_res_ind->evt_type, scan_res_ind->rssi,
					(scan_res_ind->primary_phy << 4) | scan_res_ind->secondary_phy,
					scan_res_ind->tx_power, scan_res_ind->len);
		if (bap_role & (RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI | RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI)) {
			app_bt_le_audio_scan_report_handle(scan_res_ind, APP_BT_LE_AUDIO_COMMON_SCAN_TYPE);
		}
		if (bap_role & (RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK | RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI)) {
			app_bt_le_audio_scan_report_handle(scan_res_ind, APP_BT_LE_AUDIO_BASS_SCAN_TYPE);
		}
		break;
	}
#endif

	case RTK_BT_LE_GAP_EVT_SCAN_STOP_IND: {
		rtk_bt_le_scan_stop_ind_t *scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)param;
		if (!scan_stop_ind->err) {
			BT_LOGA("[APP] Scan stopped, reason: 0x%x\r\n", scan_stop_ind->stop_reason);
			if (cap_role & (RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR | RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER)) {
				app_bt_le_audio_scan_report_show(APP_BT_LE_AUDIO_COMMON_SCAN_TYPE);
				app_bt_le_audio_scan_report_show(APP_BT_LE_AUDIO_BASS_SCAN_TYPE);
			}
			if (cap_role & RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR) {
				app_bt_le_audio_scan_report_show(APP_BT_LE_AUDIO_BASS_SCAN_TYPE);
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
			if (rtk_bt_le_audio_link_alloc(conn_ind)) {
				BT_LOGE("[APP] rtk_bt_le_audio_link_alloc fail \r\n");
				break;
			}
			if (cap_role & (RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR | RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER)) {
				uint16_t ret = RTK_BT_OK;
				if ((ret = rtk_bt_le_sm_start_security(conn_ind->conn_handle)) != RTK_BT_OK) {
					BT_LOGE("[APP] GAP start security flow failed! err: 0x%x", ret);
					break;
				}
			} else if (cap_role == RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR) {
				/* acceptor action */
			} else {
				BT_LOGE("[APP] no cap demo role 0x%x", cap_role);
			}
		} else {
			BT_LOGE("[APP] Connection establish failed(err: 0x%x), remote device: %s\r\n",
					conn_ind->err, le_addr);
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
		if (rtk_bt_le_audio_link_free(disconn_ind)) {
			BT_LOGE("[APP] rtk_bt_le_audio_link_free fail \r\n");
			break;
		}
		//start ext adv
		if (cap_role & RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR) {
			rtk_bt_le_gap_start_ext_adv(app_bt_le_audio_def_ext_adv_handle, 0, 0);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND: {
		rtk_bt_le_conn_update_ind_t *conn_update_ind =
			(rtk_bt_le_conn_update_ind_t *)param;
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
		rtk_bt_le_remote_conn_update_req_ind_t *rmt_update_req =
			(rtk_bt_le_remote_conn_update_req_ind_t *)param;
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
		rtk_bt_le_data_len_change_ind_t *data_len_change =
			(rtk_bt_le_data_len_change_ind_t *)param;
		BT_LOGA("[APP] Data len is updated, conn_handle: %d, "\
				"max_tx_octets: 0x%x, max_tx_time: 0x%x, "\
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
		rtk_bt_le_auth_pair_cfm_ind_t *pair_cfm_ind =
			(rtk_bt_le_auth_pair_cfm_ind_t *)param;
		BT_LOGA("[APP] Just work pairing need user to confirm, conn_handle: %d!\r\n",
				pair_cfm_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:pair_cfm,%d\r\n", pair_cfm_ind->conn_handle);
		rtk_bt_le_pair_cfm_t pair_cfm_param = {0};
		pair_cfm_param.conn_handle = pair_cfm_ind->conn_handle;
		pair_cfm_param.confirm = 1;
		rtk_bt_le_sm_pairing_confirm(&pair_cfm_param);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_DISPLAY_IND: {
		rtk_bt_le_auth_key_display_ind_t *key_dis_ind =
			(rtk_bt_le_auth_key_display_ind_t *)param;
		BT_LOGA("[APP] Auth passkey display: %d, conn_handle:%d\r\n",
				(int)key_dis_ind->passkey,
				(int)key_dis_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_display,%d,%d\r\n",
					(int)key_dis_ind->conn_handle,
					(int)key_dis_ind->passkey);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_INPUT_IND: {
		rtk_bt_le_auth_key_input_ind_t *key_input_ind =
			(rtk_bt_le_auth_key_input_ind_t *)param;
		BT_LOGA("[APP] Please input the auth passkey get from remote, conn_handle: %d\r\n",
				key_input_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_input,%d\r\n", key_input_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_CONFIRM_IND: {
		rtk_bt_le_auth_key_cfm_ind_t *key_cfm_ind =
			(rtk_bt_le_auth_key_cfm_ind_t *)param;
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
		rtk_bt_le_auth_oob_input_ind_t *oob_input_ind =
			(rtk_bt_le_auth_oob_input_ind_t *)param;
		BT_LOGA("[APP] Bond use oob key, conn_handle: %d. Please input the oob tk \r\n",
				oob_input_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:oobkey_input,%d\r\n", oob_input_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_COMPLETE_IND: {
		rtk_bt_le_auth_complete_ind_t *auth_cplt_ind =
			(rtk_bt_le_auth_complete_ind_t *)param;
		BT_AT_PRINT("+BLEGAP:sec,%d,%d\r\n",
					auth_cplt_ind->conn_handle,
					(auth_cplt_ind->err == 0) ? 0 : -1);
		if (auth_cplt_ind->err) {
			BT_LOGE("[APP] Pairing failed(err: 0x%x), conn_handle: %d\r\n",
					auth_cplt_ind->err, auth_cplt_ind->conn_handle);
		} else {
			BT_LOGA("[APP] Pairing success, conn_handle: %d\r\n", auth_cplt_ind->conn_handle);
			BT_DUMPHEXA("[APP] long term key is 0x", auth_cplt_ind->dev_ltk, auth_cplt_ind->dev_ltk_length, true);
			if (cap_role & (RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR | RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER)) {
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
		rtk_bt_le_bond_modify_ind_t *bond_mdf_ind =
			(rtk_bt_le_bond_modify_ind_t *)param;
		char ident_addr[30] = {0};
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->remote_addr), le_addr, sizeof(le_addr));
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->ident_addr), ident_addr, sizeof(ident_addr));
		BT_LOGA("[APP] Bond info modified, op: %d\r\n", bond_mdf_ind->op);
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
		BT_LOGE("[APP] Unknown gap cb evt type: %d", evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t app_bt_le_audio_tmap_umr_gatts_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;
	if (RTK_BT_GATTS_EVT_MTU_EXCHANGE == event) {
		rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;
		if (p_gatt_mtu_ind->result == RTK_BT_OK) {
			BT_LOGA("[APP] GATTS mtu exchange successfully, mtu_size: %d, conn_handle: %d \r\n",
					p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
		} else {
			BT_LOGE("[APP] GATTS mtu exchange fail \r\n");
		}
		return RTK_BT_EVT_CB_OK;
	}

	if (RTK_BT_GATTS_EVT_CLIENT_SUPPORTED_FEATURES == event) {
		rtk_bt_gatts_client_supported_features_ind_t *p_ind = (rtk_bt_gatts_client_supported_features_ind_t *)data;
		if (p_ind->features & RTK_BT_GATTS_CLIENT_SUPPORTED_FEATURES_EATT_BEARER_BIT) {
			BT_LOGA("[APP] Client Supported features is writed: conn_handle %d, features 0x%02x. Remote client supports EATT.\r\n",
					p_ind->conn_handle, p_ind->features);
		}
		return RTK_BT_EVT_CB_OK;
	}
	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t app_le_audio_tmap_ums_gattc_app_callback(uint8_t event, void *data, uint32_t len)
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

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
static uint16_t app_bt_le_audio_vcs_set_param(void)
{
	rtk_bt_le_audio_vcs_param_t vcs_param = {
		.volume_setting = RTK_BT_LE_AUDIO_DEFAULT_VCS_VOL_SETTING,
		.mute = RTK_BT_LE_AUDIO_VCS_NOT_MUTED,
		.change_counter = RTK_BT_LE_AUDIO_DEFAULT_VCS_CHG_CNT,
		.volume_flags = RTK_BT_LE_AUDIO_DEFAULT_VCS_VOL_FLAG,
		.step_size = RTK_BT_LE_AUDIO_DEFAULT_VCS_STEP_SIZE
	};
	rtk_bt_le_audio_vcs_set_param(&vcs_param);

	return RTK_BT_OK;
}
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
static uint16_t app_bt_le_audio_vocs_set_param(uint8_t sound_channel)
{
	rtk_bt_le_audio_vocs_param_t vocs_param = {0};
	uint8_t set_mask = 0;
	uint8_t srv_instance_id = 0;

	srv_instance_id = RTK_BT_LE_AUDIO_VOCS_IDX;
	set_mask = RTK_BT_LE_AUDIO_VOCS_AUDIO_LOCATION_FLAG | RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_FLAG;
	if (sound_channel == RTK_BT_LE_AUDIO_STEREO) {
		vocs_param.audio_location = RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR;
	} else if (sound_channel == RTK_BT_LE_AUDIO_LEFT) {
		vocs_param.audio_location = RTK_BT_LE_AUDIO_LOCATION_FL;
	} else if (sound_channel == RTK_BT_LE_AUDIO_RIGHT) {
		vocs_param.audio_location = RTK_BT_LE_AUDIO_LOCATION_FR;
	} else {
		BT_LOGE("[APP] sound_channel %d error\r\n", sound_channel);
	}
	vocs_param.output_des.p_output_des = (uint8_t *)RTK_BT_LE_AUDIO_VOCS_DES;
	vocs_param.output_des.output_des_len = strlen(RTK_BT_LE_AUDIO_VOCS_DES);
	BT_APP_PROCESS(rtk_bt_le_audio_vocs_set_param(srv_instance_id, set_mask, &vocs_param));

	return RTK_BT_OK;
}
#endif

static void app_bt_le_audio_ext_adv_init(uint8_t *name, uint8_t name_len)
{
	app_bt_le_audio_update_adv(name, name_len, app_bt_le_audio_adv_data, &app_bt_le_audio_adv_data_len);
	rtk_bt_le_gap_create_ext_adv(&app_bt_le_audio_def_ext_adv_param, &app_bt_le_audio_def_ext_adv_handle);
	rtk_bt_le_gap_set_ext_adv_data(app_bt_le_audio_def_ext_adv_handle, (uint8_t *)app_bt_le_audio_adv_data, app_bt_le_audio_adv_data_len);
	rtk_bt_le_gap_start_ext_adv(app_bt_le_audio_def_ext_adv_handle, 0, 0);
}

static void app_bt_le_audio_ext_adv_deinit(void)
{
	/* stop and remove ea */
	rtk_bt_le_gap_stop_ext_adv(app_bt_le_audio_def_ext_adv_handle);
	app_bt_le_audio_adv_data_len = APP_LE_AUDIO_ADV_START_PARSING_IDX;
}

int bt_tmap_main(uint8_t role, uint8_t enable, uint32_t sound_channel)
{
	if (1 == enable) {
		if (demo_init_flag) {
			BT_LOGE("%s Already init! \r\n", __func__);
			return -1;
		}
		switch (role) {
		/* TMAP Broadcast Media Sender init*/
		case RTK_BT_LE_AUDIO_TMAP_ROLE_BMS: {
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &bt_le_audio_demo_app_conf;
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};

			/* config le audio app configuration */
			{
				p_lea_app_conf->bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SOUR | RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI;
				p_lea_app_conf->cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR | RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER;
				p_lea_app_conf->tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_BMS;
				memset((void *)p_lea_app_conf->device_name, 0, RTK_BT_GAP_DEVICE_NAME_LEN);
				memcpy((void *)p_lea_app_conf->device_name, (uint8_t *)APP_LE_AUDIO_TMAP_BMS_DEVICE_NAME, strlen((const char *)APP_LE_AUDIO_TMAP_BMS_DEVICE_NAME));
				p_lea_app_conf->device_name_len = strlen((const char *)APP_LE_AUDIO_TMAP_BMS_DEVICE_NAME);
			}
			/* set GAP config */
			{
				bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTC | RTK_BT_PROFILE_LEAUDIO | RTK_BT_PROFILE_BAP | RTK_BT_PROFILE_CAP | RTK_BT_PROFILE_TMAP;
				bt_app_conf.mtu_size = 180;
				bt_app_conf.master_init_mtu_req = true;
				bt_app_conf.prefer_all_phy = 0;
				bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.max_tx_octets = 0x40;
				bt_app_conf.max_tx_time = 0x200;
				memcpy((void *)&bt_app_conf.le_audio_app_conf, (void *)p_lea_app_conf, sizeof(rtk_bt_le_audio_app_conf_t));
			}
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_audio_gap_app_callback));
			/* register le audio event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_BAP, app_bt_bap_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_CAP, app_bt_cap_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_TMAP, app_bt_tmap_callback));
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(p_lea_app_conf->device_name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(APP_LE_AUDIO_DEMO_GAP_APPERANCE));
			/* broadcast source init */
			{
				rtk_bt_le_audio_broadcast_source_create_param_t brs_create_param = {0};
				brs_create_param.cfg_codec_index = RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_CODEC_CFG;
				brs_create_param.cfg_qos_type = RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_QOS_CFG;
				brs_create_param.manual_qos_flag = false;
				brs_create_param.p_manual_qos_cfg = NULL;
				brs_create_param.local_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC;
				brs_create_param.encryption = false;
				brs_create_param.stream_audio_contexts = RTK_BT_LE_AUDIO_CONTEXT_MEDIA;
				BT_APP_PROCESS(rtk_bt_le_audio_broadcast_source_create(&brs_create_param));
			}
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
			/* when csip member disconnect, start ext scan timer in app_bt_le_audio_bap_unicast_client_common_cb*/
			{
				BT_LOGA("[LE Audio] Create initiator ext scan timer \r\n");
				if (false == osif_timer_create(&bt_le_audio_demo_ext_scan_timer, "lea_ini_ext_scan_timer", NULL,
											   APP_LE_AUDIO_EXT_SCAN_TIMER_INTERVAL,
											   false,
											   bt_le_audio_demo_ext_scan_timer_handle)) {
					BT_LOGE("[LE Audio] Create initiator ext scan timer fail \r\n");
					return 1;
				}
			}
#endif
			{
				scan_dev_queue.count = 0;
				if (scan_dev_queue.mtx == NULL) {
					osif_mutex_create(&scan_dev_queue.mtx);
				}
				INIT_LIST_HEAD(&scan_dev_queue.head);
			}
			if (rtk_bt_audio_init()) {
				BT_LOGE("[APP] %s: rtk_bt_audio_init fail \r\n", __func__);
				goto exit;
			}
			bap_role = p_lea_app_conf->bap_role;
			cap_role = p_lea_app_conf->cap_role;
			tmap_role = p_lea_app_conf->tmap_role;
			demo_init_flag = true;
			break;
		}
		/* TMAP Broadcast Media Receiver init*/
		case RTK_BT_LE_AUDIO_TMAP_ROLE_BMR: {
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &bt_le_audio_demo_app_conf;
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			char dev_name[40] = {0};
			char channel[2] = {0};

			/* config le audio app configuration */
			{
				p_lea_app_conf->bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK;
				p_lea_app_conf->cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR;
				p_lea_app_conf->tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_BMR;
				if (RTK_BT_LE_AUDIO_LOCATION_FL == sound_channel) {
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
					p_lea_app_conf->cap_param.csis_param.csis_cfg = RTK_BT_LEA_CSIS_CFG_RANK_1;
#endif
					p_lea_app_conf->pacs_param.sink_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL;
					p_lea_app_conf->pacs_param.source_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL;
					channel[0] = 'L';
				} else if (RTK_BT_LE_AUDIO_LOCATION_FR == sound_channel) {
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
					p_lea_app_conf->cap_param.csis_param.csis_cfg = RTK_BT_LEA_CSIS_CFG_RANK_2;
#endif
					p_lea_app_conf->pacs_param.sink_audio_location = RTK_BT_LE_AUDIO_LOCATION_FR;
					p_lea_app_conf->pacs_param.source_audio_location = RTK_BT_LE_AUDIO_LOCATION_FR;
					channel[0] = 'R';
				} else if ((RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR) == sound_channel) {
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
					p_lea_app_conf->cap_param.csis_param.csis_cfg = RTK_BT_LEA_CSIS_CFG_RANK_1;
					p_lea_app_conf->cap_param.csis_param.csis_size = 1,
#endif
									p_lea_app_conf->pacs_param.sink_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR;
					p_lea_app_conf->pacs_param.source_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR;
					channel[0] = 'S';
				} else {
					BT_LOGE("[APP] invalid channel config: 0x%x \r\n", sound_channel);
					return -1;
				}
				memset((void *)p_lea_app_conf->device_name, 0, RTK_BT_GAP_DEVICE_NAME_LEN);
				memcpy((void *)p_lea_app_conf->device_name, (uint8_t *)APP_LE_AUDIO_TMAP_BMR_DEVICE_NAME, strlen((const char *)APP_LE_AUDIO_TMAP_BMR_DEVICE_NAME));
				p_lea_app_conf->device_name_len = strlen((const char *)APP_LE_AUDIO_TMAP_BMR_DEVICE_NAME);
			}
			/* set GAP config */
			{
				bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_LEAUDIO | RTK_BT_PROFILE_BAP | RTK_BT_PROFILE_CAP | RTK_BT_PROFILE_TMAP;
				bt_app_conf.mtu_size = 180;
				bt_app_conf.master_init_mtu_req = true;
				bt_app_conf.prefer_all_phy = 0;
				bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.max_tx_octets = 0x40;
				bt_app_conf.max_tx_time = 0x200;
				memcpy((void *)&bt_app_conf.le_audio_app_conf, (void *)p_lea_app_conf, sizeof(rtk_bt_le_audio_app_conf_t));
			}
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_LOGA("[APP] sink_pac_id = %d,source_pac_id = %d\r\n", (int)bt_le_audio_demo_sink_pac_id,
					(int)bt_le_audio_demo_source_pac_id);
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
			app_bt_le_audio_vcs_set_param();
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
			app_bt_le_audio_vocs_set_param(sound_channel);
#endif
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_audio_gap_app_callback));
			/* Initilize GATTS part */
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(p_lea_app_conf->device_name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(APP_LE_AUDIO_DEMO_GAP_APPERANCE));
			/* mix RTK_BT_DEV_NAME channel bt mac address */
			strcpy(dev_name, APP_LE_AUDIO_TMAP_BMR_DEVICE_NAME);
			snprintf(&dev_name[strlen(APP_LE_AUDIO_TMAP_BMR_DEVICE_NAME)], 10, " %s (%02X%02X)", channel, bd_addr.addr_val[1], bd_addr.addr_val[0]);
			app_bt_le_audio_ext_adv_init((uint8_t *)dev_name, strlen((const char *)APP_LE_AUDIO_TMAP_BMR_DEVICE_NAME) + 9);
			/* register le audio event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_BAP, app_bt_bap_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_CAP, app_bt_cap_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_TMAP, app_bt_tmap_callback));
			{
				scan_dev_queue.count = 0;
				if (scan_dev_queue.mtx == NULL) {
					osif_mutex_create(&scan_dev_queue.mtx);
				}
				INIT_LIST_HEAD(&scan_dev_queue.head);
			}
			if (rtk_bt_audio_init()) {
				BT_LOGE("[APP] %s: rtk_bt_audio_init fail \r\n", __func__);
				goto exit;
			}
			bap_role = p_lea_app_conf->bap_role;
			cap_role = p_lea_app_conf->cap_role;
			tmap_role = p_lea_app_conf->tmap_role;
			demo_init_flag = true;
			break;
		}
		/* TMAP Unicast Media Sender init */
		case RTK_BT_LE_AUDIO_TMAP_ROLE_UMS: {
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &bt_le_audio_demo_app_conf;
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};

			/* config le audio app configuration */
			{
				p_lea_app_conf->bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI;
				p_lea_app_conf->cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR | RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER;
				p_lea_app_conf->tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_UMS;
				memset((void *)p_lea_app_conf->device_name, 0, RTK_BT_GAP_DEVICE_NAME_LEN);
				memcpy((void *)p_lea_app_conf->device_name, (uint8_t *)APP_LE_AUDIO_TMAP_UMS_DEVICE_NAME, strlen((const char *)APP_LE_AUDIO_TMAP_UMS_DEVICE_NAME));
				p_lea_app_conf->device_name_len = strlen((const char *)APP_LE_AUDIO_TMAP_UMS_DEVICE_NAME);
			}
			/* set GAP config */
			{
				bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTC | RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_LEAUDIO | RTK_BT_PROFILE_BAP | RTK_BT_PROFILE_CAP |
												  RTK_BT_PROFILE_TMAP;
				bt_app_conf.mtu_size = 180;
				bt_app_conf.master_init_mtu_req = true;
				bt_app_conf.prefer_all_phy = 0;
				bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.max_tx_octets = 0x40;
				bt_app_conf.max_tx_time = 0x200;
				memcpy((void *)&bt_app_conf.le_audio_app_conf, (void *)p_lea_app_conf, sizeof(rtk_bt_le_audio_app_conf_t));
			}
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_COMMON_GP_GAP, app_le_audio_common_gap_app_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_audio_gap_app_callback));
			/* Initilize GATT (client) part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTC, app_le_audio_tmap_ums_gattc_app_callback));
			/* register le audio event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_BAP, app_bt_bap_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_CAP, app_bt_cap_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_TMAP, app_bt_tmap_callback));
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
			/* when csip member disconnect, start ext scan timer in app_bt_le_audio_bap_unicast_client_common_cb*/
			{
				BT_LOGA("[LE Audio] Create initiator ext scan timer \r\n");
				if (false == osif_timer_create(&bt_le_audio_demo_ext_scan_timer, "lea_ini_ext_scan_timer", NULL,
											   APP_LE_AUDIO_EXT_SCAN_TIMER_INTERVAL,
											   false,
											   bt_le_audio_demo_ext_scan_timer_handle)) {
					BT_LOGE("[LE Audio] Create initiator ext scan timer fail \r\n");
					return 1;
				}
			}
#endif
			{
				scan_dev_queue.count = 0;
				if (scan_dev_queue.mtx == NULL) {
					osif_mutex_create(&scan_dev_queue.mtx);
				}
				INIT_LIST_HEAD(&scan_dev_queue.head);
			}
			if (rtk_bt_audio_init()) {
				BT_LOGE("[APP] %s: rtk_bt_audio_init fail \r\n", __func__);
				goto exit;
			}
			bap_role = p_lea_app_conf->bap_role;
			cap_role = p_lea_app_conf->cap_role;
			tmap_role = p_lea_app_conf->tmap_role;
			demo_init_flag = true;
			break;
		}
		/* TMAP Unicast Media Receiver init */
		case RTK_BT_LE_AUDIO_TMAP_ROLE_UMR: {
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &bt_le_audio_demo_app_conf;
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			char dev_name[40] = {0};
			char channel[2] = {0};

			/* config le audio app configuration */
			{
				p_lea_app_conf->bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER | RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK | RTK_BT_LE_AUDIO_BAP_ROLE_SCAN_DELE;
				p_lea_app_conf->cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR;
				p_lea_app_conf->tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_UMR;
				if (RTK_BT_LE_AUDIO_LOCATION_FL == sound_channel) {
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
					p_lea_app_conf->cap_param.csis_param.csis_cfg = RTK_BT_LEA_CSIS_CFG_RANK_1;
#endif
					p_lea_app_conf->pacs_param.sink_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL;
					p_lea_app_conf->pacs_param.source_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL;
					channel[0] = 'L';
				} else if (RTK_BT_LE_AUDIO_LOCATION_FR == sound_channel) {
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
					p_lea_app_conf->cap_param.csis_param.csis_cfg = RTK_BT_LEA_CSIS_CFG_RANK_2;
#endif
					p_lea_app_conf->pacs_param.sink_audio_location = RTK_BT_LE_AUDIO_LOCATION_FR;
					p_lea_app_conf->pacs_param.source_audio_location = RTK_BT_LE_AUDIO_LOCATION_FR;
					channel[0] = 'R';
				} else if ((RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR) == sound_channel) {
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
					p_lea_app_conf->cap_param.csis_param.csis_cfg = RTK_BT_LEA_CSIS_CFG_RANK_1;
					p_lea_app_conf->cap_param.csis_param.csis_size = 1,
#endif
									p_lea_app_conf->pacs_param.sink_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR;
					p_lea_app_conf->pacs_param.source_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR;
					channel[0] = 'S';
				} else {
					BT_LOGE("[APP] invalid channel config: 0x%x \r\n", sound_channel);
					return -1;
				}
				memset((void *)p_lea_app_conf->device_name, 0, RTK_BT_GAP_DEVICE_NAME_LEN);
				memcpy((void *)p_lea_app_conf->device_name, (uint8_t *)APP_LE_AUDIO_TMAP_UMR_DEVICE_NAME, strlen((const char *)APP_LE_AUDIO_TMAP_UMR_DEVICE_NAME));
				p_lea_app_conf->device_name_len = strlen((const char *)APP_LE_AUDIO_TMAP_UMR_DEVICE_NAME);
			}
			/* set GAP config */
			{
				bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTC | RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_LEAUDIO | RTK_BT_PROFILE_BAP | RTK_BT_PROFILE_CAP |
												  RTK_BT_PROFILE_TMAP;
				bt_app_conf.mtu_size = 180;
				bt_app_conf.master_init_mtu_req = true;
				bt_app_conf.prefer_all_phy = 0;
				bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.max_tx_octets = 0x40;
				bt_app_conf.max_tx_time = 0x200;
				memcpy((void *)&bt_app_conf.le_audio_app_conf, (void *)p_lea_app_conf, sizeof(rtk_bt_le_audio_app_conf_t));
			}
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_LOGA("[APP] sink_pac_id = %d,source_pac_id = %d\r\n", (int)bt_le_audio_demo_sink_pac_id,
					(int)bt_le_audio_demo_source_pac_id);
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
			app_bt_le_audio_vcs_set_param();
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
			app_bt_le_audio_vocs_set_param(sound_channel);
#endif
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_audio_gap_app_callback));
			/* Initilize GATTS part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTS, app_bt_le_audio_tmap_umr_gatts_app_callback));
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(p_lea_app_conf->device_name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(APP_LE_AUDIO_DEMO_GAP_APPERANCE));
			/* mix RTK_BT_DEV_NAME channel bt mac address */
			strcpy(dev_name, APP_LE_AUDIO_TMAP_UMR_DEVICE_NAME);
			snprintf(&dev_name[strlen(APP_LE_AUDIO_TMAP_UMR_DEVICE_NAME)], 10, " %s (%02X%02X)", channel, bd_addr.addr_val[1], bd_addr.addr_val[0]);
			app_bt_le_audio_ext_adv_init((uint8_t *)dev_name, strlen((const char *)APP_LE_AUDIO_TMAP_UMR_DEVICE_NAME) + 9);
			/* register le audio event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_BAP, app_bt_bap_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_CAP, app_bt_cap_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_TMAP, app_bt_tmap_callback));
			{
				scan_dev_queue.count = 0;
				if (scan_dev_queue.mtx == NULL) {
					osif_mutex_create(&scan_dev_queue.mtx);
				}
				INIT_LIST_HEAD(&scan_dev_queue.head);
			}
			if (rtk_bt_audio_init()) {
				BT_LOGE("[APP] %s: rtk_bt_audio_init fail \r\n", __func__);
				goto exit;
			}
			bap_role = p_lea_app_conf->bap_role;
			cap_role = p_lea_app_conf->cap_role;
			tmap_role = p_lea_app_conf->tmap_role;
			demo_init_flag = true;
			break;
		}

		default:
			BT_LOGE("[APP] unknown role 0x%x\r\n", role);
			return -1;
		}
	} else if (0 == enable) {
		if (!demo_init_flag) {
			BT_LOGE("%s No need deinit! \r\n", __func__);
			return -1;
		}
		switch (role) {
		/* TMAP Broadcast Media Sender deinit */
		case RTK_BT_LE_AUDIO_TMAP_ROLE_BMS: {
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
			/* Deinit ext scan timer*/
			if (bt_le_audio_demo_ext_scan_timer) {
				osif_timer_stop(&bt_le_audio_demo_ext_scan_timer);
				osif_timer_delete(&bt_le_audio_demo_ext_scan_timer);
				bt_le_audio_demo_ext_scan_time_remaining = APP_LE_AUDIO_EXT_SCAN_TIMER_COUNT;
			}
#endif
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			{
				app_bt_le_audio_scan_dev_list_remove_all();
				scan_dev_queue.count = 0;
				if (scan_dev_queue.mtx) {
					osif_mutex_delete(scan_dev_queue.mtx);
					scan_dev_queue.mtx = NULL;
				}
				INIT_LIST_HEAD(&scan_dev_queue.head);
			}
			app_bt_le_audio_remove_data_path_all();
			rtk_bt_audio_deinit();
			bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_UNKNOWN;
			cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_UNKNOWN;
			tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_UNKNOWN;
			demo_init_flag = false;
			break;
		}
		/* TMAP Broadcast Media Receiver deinit */
		case RTK_BT_LE_AUDIO_TMAP_ROLE_BMR: {
			app_bt_le_audio_ext_adv_deinit();
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			{
				app_bt_le_audio_scan_dev_list_remove_all();
				scan_dev_queue.count = 0;
				if (scan_dev_queue.mtx) {
					osif_mutex_delete(scan_dev_queue.mtx);
					scan_dev_queue.mtx = NULL;
				}
				INIT_LIST_HEAD(&scan_dev_queue.head);
			}
			app_bt_le_audio_remove_data_path_all();
			rtk_bt_audio_deinit();
			bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_UNKNOWN;
			cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_UNKNOWN;
			tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_UNKNOWN;
			demo_init_flag = false;
			break;
		}
		/* TMAP Unicast Media Sender deinit */
		case RTK_BT_LE_AUDIO_TMAP_ROLE_UMS: {
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
			/* Deinit ext scan timer*/
			if (bt_le_audio_demo_ext_scan_timer) {
				osif_timer_stop(&bt_le_audio_demo_ext_scan_timer);
				osif_timer_delete(&bt_le_audio_demo_ext_scan_timer);
				bt_le_audio_demo_ext_scan_time_remaining = APP_LE_AUDIO_EXT_SCAN_TIMER_COUNT;
			}
#endif
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			{
				app_bt_le_audio_scan_dev_list_remove_all();
				scan_dev_queue.count = 0;
				if (scan_dev_queue.mtx) {
					osif_mutex_delete(scan_dev_queue.mtx);
					scan_dev_queue.mtx = NULL;
				}
				INIT_LIST_HEAD(&scan_dev_queue.head);
			}
			app_bt_le_audio_remove_data_path_all();
			rtk_bt_audio_deinit();
			bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_UNKNOWN;
			cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_UNKNOWN;
			tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_UNKNOWN;
			demo_init_flag = false;
			break;
		}
		/* TMAP Unicast Media Receiver deinit */
		case RTK_BT_LE_AUDIO_TMAP_ROLE_UMR: {
			app_bt_le_audio_ext_adv_deinit();
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			{
				app_bt_le_audio_scan_dev_list_remove_all();
				scan_dev_queue.count = 0;
				if (scan_dev_queue.mtx) {
					osif_mutex_delete(scan_dev_queue.mtx);
					scan_dev_queue.mtx = NULL;
				}
				INIT_LIST_HEAD(&scan_dev_queue.head);
			}
			app_bt_le_audio_remove_data_path_all();
			rtk_bt_audio_deinit();
			bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_UNKNOWN;
			cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_UNKNOWN;
			tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_UNKNOWN;
			demo_init_flag = false;
			break;
		}

		default:
			BT_LOGE("[APP] unknown role 0x%x\r\n", role);
			return -1;
		}
	}

	return 0;

exit:
	/* Disable BT */
	rtk_bt_disable();
	{
		app_bt_le_audio_scan_dev_list_remove_all();
		scan_dev_queue.count = 0;
		if (scan_dev_queue.mtx) {
			osif_mutex_delete(scan_dev_queue.mtx);
			scan_dev_queue.mtx = NULL;
		}
		INIT_LIST_HEAD(&scan_dev_queue.head);
	}
	app_bt_le_audio_remove_data_path_all();
	rtk_bt_audio_deinit();
	bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_UNKNOWN;
	cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_UNKNOWN;
	tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_UNKNOWN;
	demo_init_flag = false;
	return 1;

}