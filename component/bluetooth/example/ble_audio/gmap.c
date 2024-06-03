/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <string.h>
#include <stdio.h>
#include <bt_api_config.h>
#if defined(CONFIG_BT_GMAP_SUPPORT) && CONFIG_BT_GMAP_SUPPORT
/* #if unsupported */
#include <osif.h>
#include <rtk_bt_gmap.h>
#include <rtk_bt_device.h>
#include <rtk_bt_gap.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_audio.h>
#include <bt_audio_intf.h>
#include <bt_audio_track_api.h>
#include <bt_audio_record_api.h>
#include <bt_audio_codec_wrapper.h>
#include <app_bt_le_audio_common.h>
#include <bt_utils.h>

/***************************************common resourses*************************************/
typedef enum {
	GMAP_QOS_LEVEL_A_LOWEST_LATENCY    = 0x00,
	GMAP_QOS_LEVEL_B_LOW_LATENCY       = 0x01,
	GMAP_QOS_LEVEL_C_BALANCED          = 0x02,
	GMAP_QOS_LEVEL_D_HIGH_RELIABILITY  = 0x03,
} app_bt_le_audio_gmap_qos_level_t;

typedef enum {
	GMAP_QOS_CONFIG_TYPE_16_1_gs       = 0x00,//unicast
	GMAP_QOS_CONFIG_TYPE_16_2_gs,
	GMAP_QOS_CONFIG_TYPE_32_1_gs,
	GMAP_QOS_CONFIG_TYPE_32_2_gs,
	GMAP_QOS_CONFIG_TYPE_48_1_gs,
	GMAP_QOS_CONFIG_TYPE_48_2_gs,
	GMAP_QOS_CONFIG_TYPE_32_1_gr,
	GMAP_QOS_CONFIG_TYPE_32_2_gr,
	GMAP_QOS_CONFIG_TYPE_48_1_gr,
	GMAP_QOS_CONFIG_TYPE_48_2_gr,
	GMAP_QOS_CONFIG_TYPE_48_3_gr,
	GMAP_QOS_CONFIG_TYPE_48_4_gr,
	GMAP_QOS_CONFIG_TYPE_48_1_g, //broadcast
	GMAP_QOS_CONFIG_TYPE_48_2_g,
	GMAP_QOS_CONFIG_TYPE_48_3_g,
	GMAP_QOS_CONFIG_TYPE_48_4_g,
	GMAP_QOS_CONFIG_TYPE_UNKOWN
} app_bt_le_audio_gmap_qos_set_name_t;

#if (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K)
static app_bt_le_audio_gmap_qos_level_t app_qos_level = GMAP_QOS_LEVEL_A_LOWEST_LATENCY;
static app_bt_le_audio_gmap_qos_set_name_t app_sink_ase_qos = GMAP_QOS_CONFIG_TYPE_UNKOWN;
static app_bt_le_audio_gmap_qos_set_name_t app_src_ase_qos = GMAP_QOS_CONFIG_TYPE_UNKOWN;
static app_bt_le_audio_gmap_qos_set_name_t app_broadcast_qos = GMAP_QOS_CONFIG_TYPE_UNKOWN;
#elif (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_32K)
static app_bt_le_audio_gmap_qos_level_t app_qos_level = GMAP_QOS_LEVEL_A_LOWEST_LATENCY;
static app_bt_le_audio_gmap_qos_set_name_t app_sink_ase_qos = GMAP_QOS_CONFIG_TYPE_32_2_gr;
static app_bt_le_audio_gmap_qos_set_name_t app_src_ase_qos = GMAP_QOS_CONFIG_TYPE_32_2_gs;
static app_bt_le_audio_gmap_qos_set_name_t app_broadcast_qos = GMAP_QOS_CONFIG_TYPE_UNKOWN;
#elif (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K)
static app_bt_le_audio_gmap_qos_level_t app_qos_level = GMAP_QOS_LEVEL_B_LOW_LATENCY;
static app_bt_le_audio_gmap_qos_set_name_t app_sink_ase_qos = GMAP_QOS_CONFIG_TYPE_48_2_gr;
static app_bt_le_audio_gmap_qos_set_name_t app_src_ase_qos = GMAP_QOS_CONFIG_TYPE_32_2_gs;
static app_bt_le_audio_gmap_qos_set_name_t app_broadcast_qos = GMAP_QOS_CONFIG_TYPE_48_2_g;
#else
static app_bt_le_audio_gmap_qos_level_t app_qos_level;
static app_bt_le_audio_gmap_qos_set_name_t app_sink_ase_qos = GMAP_QOS_CONFIG_TYPE_UNKOWN;
static app_bt_le_audio_gmap_qos_set_name_t app_src_ase_qos = GMAP_QOS_CONFIG_TYPE_UNKOWN;
static app_bt_le_audio_gmap_qos_set_name_t app_broadcast_qos = GMAP_QOS_CONFIG_TYPE_UNKOWN;
#endif
extern rtk_bt_le_ext_scan_param_t app_lea_def_ext_scan_param;
static bool gmap_demo_init_flag = false;
static uint8_t gmap_role = RTK_BT_LE_AUDIO_GMAP_ROLE_UNKNOWN;
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
static rtk_bt_le_create_conn_param_t app_lea_def_conn_param = {
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
#endif
/* bap pac config */
/* source pac */
static uint8_t app_lea_default_pac_source_codec[] = {
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
static uint8_t app_lea_default_pac_sink_codec[] = {
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

/* Define GMAP extended adv data default length */
#define LE_AUDIO_GMAP_EXT_ADV_DATA_DEFAULT_LEN         3
static uint8_t app_lea_adv_data[RTK_LE_AUDIO_ADV_LEN_MAX] = {
	0x02, //AD len
	RTK_BT_LE_GAP_ADTYPE_FLAGS, //AD types
	RTK_BT_LE_GAP_ADTYPE_FLAGS_GENERAL | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, //AD data
};

static uint8_t app_lea_def_ext_adv_handle = 0xFF;

static rtk_bt_le_ext_adv_param_t app_lea_def_ext_adv_param = {
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

static rtk_bt_le_scan_param_t app_lea_def_scan_param = {
	.type = RTK_BT_LE_SCAN_TYPE_ACTIVE,
	.interval = 0x60,
	.window = 0x30,
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.filter_policy = RTK_BT_LE_SCAN_FILTER_ALLOW_ALL,
	.duplicate_opt = 0,
};

static uint8_t app_lea_def_metadata[] = {
	0x03,
	RTK_BT_LE_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS,
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_GAME),
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_GAME >> 8),//ugg
	0x2,
	RTK_BT_LE_METADATA_TYPE_CCCD_LIST,
	0
};

static uint8_t app_lea_def_metadata_len = sizeof(app_lea_def_metadata);

/* bap broadcast source parameters */
/* for source encode task */
#define LEA_ENCODE_TASK_PRIO            5
#define LEA_ENCODE_TASK_STACK_SIZE      (1024*5)
#define RTK_BLE_AUDIO_USE_HW_GTIMER 1
#if defined(RTK_BLE_AUDIO_USE_HW_GTIMER) && RTK_BLE_AUDIO_USE_HW_GTIMER
#include "timer_api.h"
#define LEAUDIO_SEND_TIMER_ID           TIMER13
static gtimer_t g_le_audio_send_timer = {0};
#else
static void *g_le_audio_send_timer = NULL;
#endif

struct le_audio_demo_task_t {
	void *hdl;
	void *sem;
	uint8_t run;
};

static bool g_app_lea_encode_enable = false;

static struct le_audio_demo_task_t g_app_lea_encode_task = {
	.hdl = NULL,
	.sem = NULL,
	.run = 0,
};

static void  *g_app_lea_encode_sem = NULL;

typedef struct {
	struct list_head  list;
	uint16_t data_len;                  /*!< length of the SDU to be sent */
	uint8_t *p_data;                    /*!< point to data to be sent */
} app_lea_encode_data_t;

static uint32_t g_le_audio_send_timer_interval_us = RTK_BLE_AUDIO_DEFAULT_SDU_INTERVAL_M_S_US;

/* end bap broadcast source parameters */
/* bro sink param */
static bool g_app_lea_decode_enable = false;
/* end bro sink param */
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
static uint8_t app_lea_default_csis_sirk[] = {0x63, 0x68, 0x65, 0x6e, 0x67, 0x2d, 0x63, 0x61, 0x69, 0x73, 0x37, 0x38, 0x32, 0x53, 0xe8, 0x10};
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
static uint8_t app_lea_vcs_id_array[RTK_BT_LE_AUDIO_DEFAULT_AICS_VCS_NUM] = {RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX};
static uint8_t app_lea_mics_id_array[RTK_BT_LE_AUDIO_DEFAULT_AICS_MICS_NUM] = {RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX};
static uint8_t app_lea_vocs_features[RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM] = { RTK_BT_LE_AUDIO_VOCS_AUDIO_LOCATION_WRITE_WITHOUT_RSP_SUPPORT | \
																		   RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_WRITE_WITHOUT_RSP_SUPPORT | \
																		   RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_WRITE_WITHOUT_RSP_SUPPORT | \
																		   RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_NOTIFY_SUPPORT
																		 };
#endif
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
static uint8_t app_lea_mcs_service_id = 0;
#endif
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
/* for ext scan timer when csip member disconnect */
#define RTK_BT_LEA_GMAP_EXT_SCAN_TIMER_INTERVAL         1000
#define RTK_BT_LEA_GMAP_EXT_SCAN_TIME_COUNT             10
extern bool csip_discover_flag;
void *gmap_ext_scan_timer = NULL;
int16_t gmap_ext_scan_time_remaining;
static uint8_t gmap_ext_scan_flag = 1;
static void gmap_ext_scan_timer_handle(void *arg)
{
	(void)arg;
	if (gmap_ext_scan_time_remaining > 0) {
		gmap_ext_scan_time_remaining --;
		osif_timer_restart(&gmap_ext_scan_timer, RTK_BT_LEA_GMAP_EXT_SCAN_TIMER_INTERVAL);
		BT_LOGA("[APP] %s: gmap_ext_scan_time_remaining = %d\r\n", __func__, gmap_ext_scan_time_remaining);
	} else {
		/* ext scan time out and close ext scan */
		rtk_bt_le_gap_ext_scan_stop();
		BT_LOGA("[APP] %s: Time out and stop ext scan\r\n", __func__);
	}
}
#endif
/***************************************end common resourses********************************/
/******************************gmap broadcast game sender resources*************************/
app_bt_le_audio_initiator_info_t g_gmap_bgs_info = {
	.device_name = "Ameba LE Broadcast Game Sender",
	.lea_app_conf = {0},
	.p_bap_uni_cli_info = NULL,
	.p_bap_bro_sour_info = &app_bap_bro_sour_info,
	.p_bap_bro_assi_info = NULL,
	.group_handle = NULL,
	.status = RTK_BLE_AUDIO_INITIATOR_DISABLE
};

extern rtk_bt_le_audio_broadcast_source_group_param_t def_le_audio_broadcast_source_group_param;
static uint8_t app_lea_ori_broadcast_source_metadata[] = {
	0x03,
	RTK_BT_LE_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS,
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_MEDIA),
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_MEDIA >> 8),
	0x02,
	RTK_BT_LE_METADATA_TYPE_CCCD_LIST,
	0,
};

static uint8_t app_lea_gmap_broadcast_source_metadata[] = {
	0x03,
	RTK_BT_LE_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS,
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_GAME),
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_GAME >> 8),
	0x02,
	RTK_BT_LE_METADATA_TYPE_CCCD_LIST,
	0,
};
/*****************************end gmap broadcast game sender resources*************************/
/******************************gmap broadcast game receiver resources*************************/
app_bt_le_audio_acceptor_info_t g_gmap_bgr_info = {
	.sound_channel = RTK_BT_LE_AUDIO_STEREO,
	.device_name = "Ameba LE Broadcast Game Receiver",
	.csis_neighbor_addr = {0},
	.lea_app_conf = {0},
	.p_bap_uni_ser_info = NULL,
	.p_bap_bro_sink_info = &app_bap_bro_sink_info,
	.status = RTK_BLE_AUDIO_INITIATOR_DISABLE
};
/*****************************end gmap broadcast game receiver resources***********************/
/********************************gmap unicast game gateway resources***************************/
app_bt_le_audio_initiator_info_t g_gmap_ugg_info = {
	.device_name = "Ameba LE Unicast Game Gateway",
	.lea_app_conf = {0},
	.p_bap_uni_cli_info = &app_bap_uni_client_info,
	.p_bap_bro_sour_info = NULL,
	.p_bap_bro_assi_info = NULL,
	.group_handle = NULL,
	.status = RTK_BLE_AUDIO_INITIATOR_DISABLE
};
/******************************end gmap unicast game gateway resources*************************/
/********************************gmap unicast game terminal resources***************************/
app_bt_le_audio_acceptor_info_t g_gmap_ugt_info = {
	.sound_channel = RTK_BT_LE_AUDIO_STEREO,
	.device_name = "Ameba LE Unicast Game Terminal",
	.csis_neighbor_addr = {0},
	.lea_app_conf = {0},
	.p_bap_uni_ser_info = &app_bap_uni_server_info,
	.p_bap_bro_sink_info = NULL,
	.status = 0
};
/*******************************end gmap unicast game terminal resources*************************/

// GMAP broadcast QoS config can refer to GMAP spec page 36~38
static uint16_t app_bt_le_audio_gmap_broadcast_qos_config(app_bt_le_audio_gmap_qos_level_t qos_level,
														  app_bt_le_audio_gmap_qos_set_name_t set_name,
														  rtk_bt_le_audio_qos_cfg_preferred_t *p_broadcast_qos)
{
	if (p_broadcast_qos == NULL) {
		printf("%s: p_broadcast_qos is NULL\r\n", __func__);
		return -1;
	}
	switch (qos_level) {
	case GMAP_QOS_LEVEL_A_LOWEST_LATENCY: {
		switch (set_name) {
		case GMAP_QOS_CONFIG_TYPE_48_1_g: {
			p_broadcast_qos->max_sdu = 75;
			p_broadcast_qos->retransmission_number = 1;
			p_broadcast_qos->max_transport_latency = 8;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 7500;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_2_g: {
			p_broadcast_qos->max_sdu = 100;
			p_broadcast_qos->retransmission_number = 1;
			p_broadcast_qos->max_transport_latency = 10;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 10000;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_3_g: {
			p_broadcast_qos->max_sdu = 90;
			p_broadcast_qos->retransmission_number = 1;
			p_broadcast_qos->max_transport_latency = 8;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 7500;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_4_g: {
			p_broadcast_qos->max_sdu = 120;
			p_broadcast_qos->retransmission_number = 1;
			p_broadcast_qos->max_transport_latency = 10;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 10000;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		default:
			printf("unkown set name: %d\r\n", set_name);
			break;
		}
		break;
	}

	case GMAP_QOS_LEVEL_B_LOW_LATENCY: {
		switch (set_name) {
		case GMAP_QOS_CONFIG_TYPE_48_1_g: {
			p_broadcast_qos->max_sdu = 75;
			p_broadcast_qos->retransmission_number = 2;
			p_broadcast_qos->max_transport_latency = 8;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 7500;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_2_g: {
			p_broadcast_qos->max_sdu = 100;
			p_broadcast_qos->retransmission_number = 2;
			p_broadcast_qos->max_transport_latency = 10;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 10000;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_3_g: {
			p_broadcast_qos->max_sdu = 90;
			p_broadcast_qos->retransmission_number = 2;
			p_broadcast_qos->max_transport_latency = 8;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 7500;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_4_g: {
			p_broadcast_qos->max_sdu = 120;
			p_broadcast_qos->retransmission_number = 2;
			p_broadcast_qos->max_transport_latency = 10;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 10000;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		default:
			printf("unkown set name: %d\r\n", set_name);
			break;
		}
		break;
	}
	case GMAP_QOS_LEVEL_C_BALANCED: {
		switch (set_name) {
		case GMAP_QOS_CONFIG_TYPE_48_1_g: {
			p_broadcast_qos->max_sdu = 75;
			p_broadcast_qos->retransmission_number = 3;
			p_broadcast_qos->max_transport_latency = 8;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 7500;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_2_g: {
			p_broadcast_qos->max_sdu = 100;
			p_broadcast_qos->retransmission_number = 3;
			p_broadcast_qos->max_transport_latency = 10;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 10000;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_3_g: {
			p_broadcast_qos->max_sdu = 90;
			p_broadcast_qos->retransmission_number = 3;
			p_broadcast_qos->max_transport_latency = 8;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 7500;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_4_g: {
			p_broadcast_qos->max_sdu = 120;
			p_broadcast_qos->retransmission_number = 3;
			p_broadcast_qos->max_transport_latency = 10;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 10000;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		default:
			printf("unkown set name: %d\r\n", set_name);
			break;
		}
		break;
	}
	case GMAP_QOS_LEVEL_D_HIGH_RELIABILITY: {
		switch (set_name) {
		case GMAP_QOS_CONFIG_TYPE_48_1_g: {
			p_broadcast_qos->max_sdu = 75;
			p_broadcast_qos->retransmission_number = 4;
			p_broadcast_qos->max_transport_latency = 8;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 7500;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_2_g: {
			p_broadcast_qos->max_sdu = 100;
			p_broadcast_qos->retransmission_number = 4;
			p_broadcast_qos->max_transport_latency = 10;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 10000;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_3_g: {
			p_broadcast_qos->max_sdu = 90;
			p_broadcast_qos->retransmission_number = 4;
			p_broadcast_qos->max_transport_latency = 8;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 7500;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_4_g: {
			p_broadcast_qos->max_sdu = 120;
			p_broadcast_qos->retransmission_number = 4;
			p_broadcast_qos->max_transport_latency = 10;
			p_broadcast_qos->presentation_delay = 10000;
			p_broadcast_qos->sdu_interval = 10000;
			p_broadcast_qos->framing = RTK_BLE_AUDIO_UNFRAMED;
			break;
		}
		default:
			printf("unkown set name: %d\r\n", set_name);
			break;
		}
		break;
	}
	default:
		printf("unkown qos level: %d\r\n", qos_level);
		break;
	}

	return 0;
}

// GMAP unicast QoS config can refer to GMAP spec page 31~32
static uint16_t app_bt_le_audio_gmap_ase_qos_config(app_bt_le_audio_gmap_qos_level_t qos_level,
													app_bt_le_audio_gmap_qos_set_name_t set_name,
													rtk_bt_le_audio_unicast_session_qos_t *p_session_qos,
													rtk_bt_le_audio_unicast_ase_qos_t *p_ase_qos)
{
	if (p_ase_qos == NULL || p_session_qos == NULL) {
		printf("%s: p_ase_qos or p_session_qos is NULL\r\n", __func__);
		return -1;
	}

	switch (qos_level) {
	case GMAP_QOS_LEVEL_A_LOWEST_LATENCY: {
		switch (set_name) {
		case GMAP_QOS_CONFIG_TYPE_16_1_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 1;
			p_ase_qos->max_sdu = 30;
			p_session_qos->latency_s_m = 15;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_16_2_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 1;
			p_ase_qos->max_sdu = 40;
			p_session_qos->latency_s_m = 20;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_1_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 1;
			p_ase_qos->max_sdu = 60;
			p_session_qos->latency_s_m = 15;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_2_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 1;
			p_ase_qos->max_sdu = 80;
			p_session_qos->latency_s_m = 20;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_1_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 1;
			p_ase_qos->max_sdu = 75;
			p_session_qos->latency_s_m = 15;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_2_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 1;
			p_ase_qos->max_sdu = 100;
			p_session_qos->latency_s_m = 20;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_1_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 1;
			p_ase_qos->max_sdu = 60;
			p_session_qos->latency_m_s = 15;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_2_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 1;
			p_ase_qos->max_sdu = 80;
			p_session_qos->latency_m_s = 20;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_1_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 1;
			p_ase_qos->max_sdu = 75;
			p_session_qos->latency_m_s = 15;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_2_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 1;
			p_ase_qos->max_sdu = 100;
			p_session_qos->latency_m_s = 20;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_3_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 1;
			p_ase_qos->max_sdu = 90;
			p_session_qos->latency_m_s = 15;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_4_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 1;
			p_ase_qos->max_sdu = 120;
			p_session_qos->latency_m_s = 20;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		default:
			printf("unkown set name: %d\r\n", set_name);
			break;
		}
		break;
	}
	case GMAP_QOS_LEVEL_B_LOW_LATENCY: {
		switch (set_name) {
		case GMAP_QOS_CONFIG_TYPE_16_1_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 2;
			p_ase_qos->max_sdu = 30;
			p_session_qos->latency_s_m = 15;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_16_2_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 2;
			p_ase_qos->max_sdu = 40;
			p_session_qos->latency_s_m = 20;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_1_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 2;
			p_ase_qos->max_sdu = 60;
			p_session_qos->latency_s_m = 15;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_2_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 2;
			p_ase_qos->max_sdu = 80;
			p_session_qos->latency_s_m = 20;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_1_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 2;
			p_ase_qos->max_sdu = 75;
			p_session_qos->latency_s_m = 15;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_2_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 2;
			p_ase_qos->max_sdu = 100;
			p_session_qos->latency_s_m = 20;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_1_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 2;
			p_ase_qos->max_sdu = 60;
			p_session_qos->latency_m_s = 15;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_2_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 2;
			p_ase_qos->max_sdu = 80;
			p_session_qos->latency_m_s = 20;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_1_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 2;
			p_ase_qos->max_sdu = 75;
			p_session_qos->latency_m_s = 15;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_2_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 2;
			p_ase_qos->max_sdu = 100;
			p_session_qos->latency_m_s = 20;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_3_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 2;
			p_ase_qos->max_sdu = 90;
			p_session_qos->latency_m_s = 15;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_4_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 2;
			p_ase_qos->max_sdu = 120;
			p_session_qos->latency_m_s = 20;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		default:
			printf("unkown set name: %d\r\n", set_name);
			break;
		}
		break;
	}
	case GMAP_QOS_LEVEL_C_BALANCED: {
		switch (set_name) {
		case GMAP_QOS_CONFIG_TYPE_16_1_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 3;
			p_ase_qos->max_sdu = 30;
			p_session_qos->latency_s_m = 15;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_16_2_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 3;
			p_ase_qos->max_sdu = 40;
			p_session_qos->latency_s_m = 20;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_1_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 3;
			p_ase_qos->max_sdu = 60;
			p_session_qos->latency_s_m = 15;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_2_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 3;
			p_ase_qos->max_sdu = 80;
			p_session_qos->latency_s_m = 20;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_1_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 3;
			p_ase_qos->max_sdu = 75;
			p_session_qos->latency_s_m = 15;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_2_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 3;
			p_ase_qos->max_sdu = 100;
			p_session_qos->latency_s_m = 20;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_1_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 3;
			p_ase_qos->max_sdu = 60;
			p_session_qos->latency_m_s = 15;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_2_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 3;
			p_ase_qos->max_sdu = 80;
			p_session_qos->latency_m_s = 20;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_1_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 3;
			p_ase_qos->max_sdu = 75;
			p_session_qos->latency_m_s = 15;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_2_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 3;
			p_ase_qos->max_sdu = 100;
			p_session_qos->latency_m_s = 20;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_3_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 3;
			p_ase_qos->max_sdu = 90;
			p_session_qos->latency_m_s = 15;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_4_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 3;
			p_ase_qos->max_sdu = 120;
			p_session_qos->latency_m_s = 20;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		default:
			printf("unkown set name: %d\r\n", set_name);
			break;
		}
		break;
	}
	case GMAP_QOS_LEVEL_D_HIGH_RELIABILITY: {
		switch (set_name) {
		case GMAP_QOS_CONFIG_TYPE_16_1_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 5;
			p_ase_qos->max_sdu = 30;
			p_session_qos->latency_s_m = 15;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_16_2_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 5;
			p_ase_qos->max_sdu = 40;
			p_session_qos->latency_s_m = 20;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_1_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 5;
			p_ase_qos->max_sdu = 60;
			p_session_qos->latency_s_m = 15;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_2_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 5;
			p_ase_qos->max_sdu = 80;
			p_session_qos->latency_s_m = 20;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_1_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 5;
			p_ase_qos->max_sdu = 75;
			p_session_qos->latency_s_m = 15;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_2_gs: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 5;
			p_ase_qos->max_sdu = 100;
			p_session_qos->latency_s_m = 20;
			p_session_qos->source_presentation_delay = 60000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_1_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 5;
			p_ase_qos->max_sdu = 60;
			p_session_qos->latency_m_s = 15;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_32_2_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 5;
			p_ase_qos->max_sdu = 80;
			p_session_qos->latency_m_s = 20;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_1_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 5;
			p_ase_qos->max_sdu = 75;
			p_session_qos->latency_m_s = 15;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_2_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 5;
			p_ase_qos->max_sdu = 100;
			p_session_qos->latency_m_s = 20;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_3_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 5;
			p_ase_qos->max_sdu = 90;
			p_session_qos->latency_m_s = 15;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		case GMAP_QOS_CONFIG_TYPE_48_4_gr: {
			p_ase_qos->phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M;
			p_ase_qos->retransmission_number = 5;
			p_ase_qos->max_sdu = 120;
			p_session_qos->latency_m_s = 20;
			p_session_qos->sink_presentation_delay = 10000;
			break;
		}
		default:
			printf("unkown set name: %d\r\n", set_name);
			break;
		}
		break;
	}
	default:
		printf("unkown GMAP qos level: %d\r\n", qos_level);
		break;
	}

	return 0;
}

static rtk_bt_evt_cb_ret_t app_bt_le_audio_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	char le_addr[30] = {0};
	char *role = NULL;

	switch (evt_code) {
	case RTK_BT_LE_GAP_EVT_ADV_START_IND: {
		rtk_bt_le_adv_start_ind_t *adv_start_ind = (rtk_bt_le_adv_start_ind_t *)param;
		if (!adv_start_ind->err) {
			printf("[APP]ADV started: adv_type %d \r\n", adv_start_ind->adv_type);
		} else {
			printf("[APP]ADV start failed, err 0x%x \r\n", adv_start_ind->err);
		}
		BT_AT_PRINT("+BLEGAP=adv,start,%d,%d\r\n", (adv_start_ind->err == 0) ? 0 : -1, adv_start_ind->adv_type);
		break;
	}

	case RTK_BT_LE_GAP_EVT_ADV_STOP_IND: {
		rtk_bt_le_adv_stop_ind_t *adv_stop_ind = (rtk_bt_le_adv_stop_ind_t *)param;
		if (!adv_stop_ind->err) {
			printf("[APP]ADV stopped: reason 0x%x \r\n", adv_stop_ind->stop_reason);
		} else {
			printf("[APP]ADV stop failed, err 0x%x \r\n", adv_stop_ind->err);
		}
		BT_AT_PRINT("+BLEGAP=adv,start,%d,%d\r\n", (adv_start_ind->err == 0) ? 0 : -1, adv_start_ind->adv_type);
		break;
	}

#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
	case RTK_BT_LE_GAP_EVT_EXT_ADV_IND: {
		rtk_bt_le_ext_adv_ind_t *ext_adv_ind = (rtk_bt_le_ext_adv_ind_t *)param;
		if (!ext_adv_ind->err) {
			if (ext_adv_ind->is_start) {
				printf("[APP] Ext ADV(%d) started\r\n", ext_adv_ind->adv_handle);
			} else {
				printf("[APP] Ext ADV(%d) stopped: reason 0x%x \r\n", ext_adv_ind->adv_handle, ext_adv_ind->stop_reason);
			}
		} else {
			if (ext_adv_ind->is_start) {
				printf("[APP] Ext ADV(%d) started failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
			} else {
				printf("[APP] Ext ADV(%d) stopped failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
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
			printf("[APP] Scan started, scan_type: %d\r\n", scan_start_ind->scan_type);
		} else {
			printf("[APP] Scan start failed(err: 0x%x)\r\n", scan_start_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:scan,start,%d,%d\r\n", (scan_start_ind->err == 0) ? 0 : -1, scan_start_ind->scan_type);
		break;
	}

	case RTK_BT_LE_GAP_EVT_SCAN_RES_IND: {
		rtk_bt_le_scan_res_ind_t *scan_res_ind = (rtk_bt_le_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->adv_report.addr), le_addr, sizeof(le_addr));
		printf("[APP] Scan info, [Device]: %s, AD evt type: %d, RSSI: %i, len: %d \r\n",
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
		printf("[APP] Ext Scan info, [Device]: %s, AD evt type: 0x%x, RSSI: %i, PHY: 0x%x, TxPower: %d, Len: %d\r\n",
			   le_addr, scan_res_ind->evt_type, scan_res_ind->rssi,
			   (scan_res_ind->primary_phy << 4) | scan_res_ind->secondary_phy,
			   scan_res_ind->tx_power, scan_res_ind->len);
#endif
		BT_AT_PRINT("+BLEGAP:escan,%s,0x%x,%i,0x%x,%d,%d\r\n",
					le_addr, scan_res_ind->evt_type, scan_res_ind->rssi,
					(scan_res_ind->primary_phy << 4) | scan_res_ind->secondary_phy,
					scan_res_ind->tx_power, scan_res_ind->len);
		if (gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_UGG || gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGS) {
			app_bt_le_audio_scan_report_handle(scan_res_ind);
		}
		if (gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGR) {
			app_bt_le_audio_bass_scan_report_handle(scan_res_ind);
		}
		break;
	}
#endif

	case RTK_BT_LE_GAP_EVT_SCAN_STOP_IND: {
		rtk_bt_le_scan_stop_ind_t *scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)param;
		if (!scan_stop_ind->err) {
			printf("[APP] Scan stopped, reason: 0x%x\r\n", scan_stop_ind->stop_reason);
			if (gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGR) {
				app_bt_le_audio_bass_scan_report_show();
			}
			if (gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_UGG) {
				app_bt_le_audio_scan_report_show();
			}
		} else {
			printf("[APP] Scan stop failed(err: 0x%x)\r\n", scan_stop_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:scan,stop,%d,0x%x\r\n", (scan_stop_ind->err == 0) ? 0 : -1, scan_stop_ind->stop_reason);
		break;
	}
	case RTK_BT_LE_GAP_EVT_CONNECT_IND: {
		rtk_bt_le_conn_ind_t *conn_ind = (rtk_bt_le_conn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(conn_ind->peer_addr), le_addr, sizeof(le_addr));
		if (!conn_ind->err) {
			role = conn_ind->role ? "slave" : "master";
			printf("[APP] Connected, handle: %d, role: %s, remote device: %s\r\n",
				   (int)conn_ind->conn_handle, role, le_addr);
			if (gmap_role == RTK_BT_LE_AUDIO_GMAP_ROLE_UGG) {
				g_gmap_ugg_info.status = RTK_BLE_AUDIO_INITIATOR_CONNECT;
				uint16_t ret = RTK_BT_OK;
				if ((ret = rtk_bt_le_sm_start_security(conn_ind->conn_handle)) != RTK_BT_OK) {
					printf("[APP] GAP start security flow failed! err: 0x%x", ret);
					break;
				}
			} else if (gmap_role == RTK_BT_LE_AUDIO_GMAP_ROLE_UGT) {
				g_gmap_ugt_info.status = RTK_BLE_AUDIO_ACCEPTOR_CONN;
			} else {
				printf("[APP] no gmap demo role 0x%x", gmap_role);
			}
		} else {
			printf("[APP] Connection establish failed(err: 0x%x), remote device: %s\r\n",
				   conn_ind->err, le_addr);
		}
		BT_AT_PRINT("+BLEGAP:conn,%d,%d,%s\r\n", (conn_ind->err == 0) ? 0 : -1, (int)conn_ind->conn_handle, le_addr);
		break;
	}

	case RTK_BT_LE_GAP_EVT_DISCONN_IND: {
		rtk_bt_le_disconn_ind_t *disconn_ind = (rtk_bt_le_disconn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(disconn_ind->peer_addr), le_addr, sizeof(le_addr));
		role = disconn_ind->role ? "slave" : "master";
		printf("[APP] Disconnected, reason: 0x%x, handle: %d, role: %s, remote device: %s\r\n",
			   disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		BT_AT_PRINT("+BLEGAP:disconn,0x%x,%d,%s,%s\r\n",
					disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		//start ext adv
		if (gmap_role == RTK_BT_LE_AUDIO_GMAP_ROLE_UGT) {
			rtk_bt_le_gap_start_ext_adv(app_lea_def_ext_adv_handle, 0, 0);
			g_gmap_ugt_info.status = RTK_BLE_AUDIO_ACCEPTOR_DISC;
		} else if (gmap_role == RTK_BT_LE_AUDIO_GMAP_ROLE_UGG) {
			app_bt_le_audio_device_list_remove(disconn_ind->conn_handle);
			g_gmap_ugg_info.status = RTK_BLE_AUDIO_INITIATOR_DISCONNECT;
		} else {
			printf("[APP] no gmap demo role 0x%x", gmap_role);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND: {
		rtk_bt_le_conn_update_ind_t *conn_update_ind = (rtk_bt_le_conn_update_ind_t *)param;
		if (conn_update_ind->err) {
			printf("[APP] Update conn param failed, conn_handle: %d, err: 0x%x\r\n",
				   conn_update_ind->conn_handle, conn_update_ind->err);
			BT_AT_PRINT("+BLEGAP:conn_update,%d,-1\r\n", conn_update_ind->conn_handle);
		} else {
			printf("[APP] Conn param is updated, conn_handle: %d, conn_interval: 0x%x, "\
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
		printf("[APP] Remote device request a change in conn param, conn_handle: %d, "\
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
		printf("[APP] Data len is updated, conn_handle: %d, "\
			   "max_tx_octets: 0x%x, max_tx_time: 0x%x, "\
			   "max_rx_octets: 0x%x, max_rx_time: 0x%x\r\n",
			   data_len_change->conn_handle, data_len_change->max_tx_octets,
			   data_len_change->max_tx_time, data_len_change->max_rx_octets,
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
		rtk_bt_le_phy_update_ind_t *phy_update_ind = (rtk_bt_le_phy_update_ind_t *)param;
		if (phy_update_ind->err) {
			printf("[APP] Update PHY failed, conn_handle: %d, err: 0x%x\r\n",
				   phy_update_ind->conn_handle, phy_update_ind->err);
			BT_AT_PRINT("+BLEGAP:conn_phy,%d,-1\r\n", phy_update_ind->conn_handle);
		} else {
			printf("[APP] PHY is updated, conn_handle: %d, tx_phy: %d, rx_phy: %d\r\n",
				   phy_update_ind->conn_handle, phy_update_ind->tx_phy, phy_update_ind->rx_phy);
			BT_AT_PRINT("+BLEGAP:conn_phy,%d,0,%d,%d\r\n",
						phy_update_ind->conn_handle,
						phy_update_ind->tx_phy,
						phy_update_ind->rx_phy);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PAIRING_CONFIRM_IND: {
		rtk_bt_le_auth_pair_cfm_ind_t *pair_cfm_ind = (rtk_bt_le_auth_pair_cfm_ind_t *)param;
		APP_PROMOTE("[APP] Just work pairing need user to confirm, conn_handle: %d!\r\n",
					pair_cfm_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:pair_cfm,%d\r\n", pair_cfm_ind->conn_handle);
		rtk_bt_le_pair_cfm_t pair_cfm_param = {0};
		pair_cfm_param.conn_handle = pair_cfm_ind->conn_handle;
		pair_cfm_param.confirm = 1;
		rtk_bt_le_sm_pairing_confirm(&pair_cfm_param);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_DISPLAY_IND: {
		rtk_bt_le_auth_key_display_ind_t *key_dis_ind = (rtk_bt_le_auth_key_display_ind_t *)param;
		APP_PROMOTE("[APP] Auth passkey display: %d, conn_handle:%d\r\n",
					(int)key_dis_ind->passkey, (int)key_dis_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_display,%d,%d\r\n",
					(int)key_dis_ind->conn_handle,
					(int)key_dis_ind->passkey);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_INPUT_IND: {
		rtk_bt_le_auth_key_input_ind_t *key_input_ind = (rtk_bt_le_auth_key_input_ind_t *)param;
		APP_PROMOTE("[APP] Please input the auth passkey get from remote, conn_handle: %d\r\n",
					key_input_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_input,%d\r\n", key_input_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_CONFIRM_IND: {
		rtk_bt_le_auth_key_cfm_ind_t *key_cfm_ind = (rtk_bt_le_auth_key_cfm_ind_t *)param;
		APP_PROMOTE("[APP] Auth passkey confirm: %d, conn_handle: %d. "\
					"Please comfirm if the passkeys are equal!\r\n",
					(int)key_cfm_ind->passkey, (int)key_cfm_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_cfm,%d,%d\r\n",
					(int)key_cfm_ind->conn_handle,
					(int)key_cfm_ind->passkey);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_OOB_KEY_INPUT_IND: {
		rtk_bt_le_auth_oob_input_ind_t *oob_input_ind = (rtk_bt_le_auth_oob_input_ind_t *)param;
		APP_PROMOTE("[APP] Bond use oob key, conn_handle: %d. Please input the oob tk \r\n",
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
			printf("[APP] Pairing failed(err: 0x%x), conn_handle: %d\r\n",
				   auth_cplt_ind->err, auth_cplt_ind->conn_handle);
		} else {
			printf("[APP] Pairing success, conn_handle: %d\r\n", auth_cplt_ind->conn_handle);
			printf("[APP] long term key is 0x");
			for (uint8_t i = 1; i <= auth_cplt_ind->dev_ltk_length; i++) {
				printf("%02x", auth_cplt_ind->dev_ltk[auth_cplt_ind->dev_ltk_length - i]); //End size conversion
			}
			printf("\r\n");
			if ((gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_UGG) || (gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGS)) {
				uint16_t ret = rtk_bt_gattc_discover_all(auth_cplt_ind->conn_handle);
				if (RTK_BT_OK != ret) {
					printf("[APP] GATTC Discover failed! err: 0x%x\r\n", ret);
					break;
				} else {
					printf("[APP] GATTC Discover Start, waiting ...\r\n");
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
		printf("[APP] Bond info modified, op: %d\r\n", bond_mdf_ind->op);
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
				printf("[APP] Resolving list %s %s fail, cause:%x.\r\n",
					   (p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_ADD) ? "add" : "remove",
					   le_addr, p_ind->err);
				BT_AT_PRINT("+BLEGAP:resolv_list_modify,%d,-1\r\n", p_ind->op);
			} else {
				printf("[APP] Resolving list %s %s success, %s privacy mode.\r\n",
					   (p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_ADD) ? "add" : "remove",
					   le_addr, p_ind->entry.device_mode ? "device" : "network");
				BT_AT_PRINT("+BLEGAP:resolv_list_modify,%d,0,%s,%s\r\n",
							p_ind->op, le_addr, p_ind->entry.device_mode ? "device" : "network");
			}
		} else if (p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_CLEAR) {
			if (p_ind->err) {
				printf("[APP] Resolving list clear fail, cause:%x.\r\n", p_ind->err);
			} else {
				printf("[APP] Resolving list clear success.\r\n");
			}
			BT_AT_PRINT("+BLEGAP:resolv_list_modify,%d,%d\r\n", p_ind->op, (p_ind->err == 0) ? 0 : -1);
		}
		break;
	}
#endif

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
	case RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND: {
		rtk_bt_le_pa_sync_ind_t *pa_sync_ind = (rtk_bt_le_pa_sync_ind_t *)param;
		printf("[APP] PA sync state change: sync_id: %d, state = %d, cause: 0x%x\r\n",
			   pa_sync_ind->sync_id, pa_sync_ind->state, pa_sync_ind->cause);

		if (pa_sync_ind->state == RTK_BT_LE_PA_SYNC_STATE_SYNCHRONIZED) {
			rtk_bt_le_pa_sync_sync_param_t sync_param;
			rtk_bt_le_pa_synced_info_t *p_info = &pa_sync_ind->info;
			if (RTK_BT_OK == rtk_bt_le_gap_pa_sync_get_param(RTK_BT_LE_PA_SYNC_PARAM_SYNCHRONIZED_PARAM,
															 &sync_param, pa_sync_ind->sync_id)) {
				rtk_bt_le_addr_t addr;
				addr.type = sync_param.adv_addr_type;
				memcpy(addr.addr_val, sync_param.adv_addr, RTK_BD_ADDR_LEN);
				rtk_bt_le_addr_to_str(&addr, le_addr, sizeof(le_addr));
				printf("[APP] PA SYNCHRONIZED PARAM: [Device]: %s, sync_handle:0x%x, adv_sid:%d, skip:%d, sync_timeout:%d, ",
					   le_addr, sync_param.sync_handle, sync_param.adv_sid, sync_param.skip, sync_param.sync_timeout);
				printf("sync_cte_type:%d, adv_phy:%d, adv_clock_accuracy:0x%x, ",
					   sync_param.sync_cte_type, sync_param.adv_phy, sync_param.adv_clock_accuracy);
				printf("periodic_adv_interval:%d, sync_transfer_received_flag:%d\r\n",
					   sync_param.periodic_adv_interval, sync_param.sync_transfer_received_flag);
				BT_AT_PRINT("+BLEGAP:pa_sync,%s,0x%x,%d,%d\r\n",
							le_addr, p_info->sync_handle, p_info->adv_sid, p_info->past_received);
			}
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_PA_ADV_REPORT_IND: {
		rtk_bt_le_pa_adv_report_ind_t *pa_report = (rtk_bt_le_pa_adv_report_ind_t *)param;
		printf("[APP] PA sync ADV report: sync_id %d, sync_handle 0x%x, tx_power %d, rssi %d, cte_type %d, data_status 0x%x, data_len %d\r\n",
			   pa_report->sync_id, pa_report->sync_handle, pa_report->tx_power, pa_report->rssi,
			   pa_report->cte_type, pa_report->data_status, pa_report->data_len);
		BT_AT_PRINT("+BLEGAP:pa_report,%d,0x%x,%d,%d,%d,0x%x,%d\r\n",
					pa_report->sync_id, pa_report->sync_handle, pa_report->tx_power, pa_report->rssi,
					pa_report->cte_type, pa_report->data_status, pa_report->data_len);
		break;
	}
#endif

#if defined(RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT) && RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT
	case RTK_BT_LE_GAP_PAST_RECEIVED_INFO_IND: {
		rtk_bt_le_past_recv_ind_t *past_recv_ind = (rtk_bt_le_past_recv_ind_t *)param;
		printf("[APP] PAST received info: conn_handle %d, service_data %d\r\n",
			   past_recv_ind->conn_handle, past_recv_ind->service_data);
		break;
	}
#endif

#if defined(RTK_BLE_5_2_POWER_CONTROL_SUPPORT) && RTK_BLE_5_2_POWER_CONTROL_SUPPORT
	case RTK_BT_LE_GAP_EVT_TXPOWER_REPORT_IND: {
		rtk_bt_le_txpower_ind_t *txpower_ind = (rtk_bt_le_txpower_ind_t *)param;
		printf("[APP] TX power report: conn_handle %d, type %d, txpower %d\r\n",
			   txpower_ind->conn_handle, txpower_ind->type, txpower_ind->txpower);
		BT_AT_PRINT("+BLEGAP:txpower_report,%d,%d,%d\r\n",
					txpower_ind->conn_handle, txpower_ind->type, txpower_ind->txpower);
		break;
	}
#endif

	default:
		printf("[APP] Unkown gap cb evt type: %d", evt_code);
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
			printf("[APP] ECFC connection established fail(0x%x). %s\r\n", p_ind->err, addr_str);
		} else {
			printf("[APP] ECFC connection established OK. %s\r\n", addr_str);
			printf("[APP] proto_id(%d) conn_handle(%d) remote_mtu(%d) local_mtu(%d) local_mps(%d)\r\n",
				   p_ind->proto_id, p_ind->conn_handle, p_ind->remote_mtu, p_ind->local_mtu, p_ind->local_mps);
			BT_DUMP16A("[APP] cid: ", p_ind->cid, p_ind->cid_num);
		}
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_DATA_IND: {
		rtk_bt_ecfc_data_ind_t *p_ind = (rtk_bt_ecfc_data_ind_t *)param;
		printf("[APP] ECFC data received. proto_id(%d) conn_handle(%d) cid(%d) length(%d)\r\n",
			   p_ind->proto_id, p_ind->conn_handle, p_ind->cid, p_ind->length);
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_CONN_REQ_IND: {
		char addr_str[40] = {0};
		rtk_bt_ecfc_conn_req_ind_t *p_ind = (rtk_bt_ecfc_conn_req_ind_t *)param;
		rtk_bt_addr_to_str(p_ind->bd_type, p_ind->bd_addr, addr_str, 40);
		printf("[APP] ECFC connection requset. %s\r\n", addr_str);
		printf("[APP] proto_id(%d) conn_handle(%d) remote_mtu(%d) identity_id(%d)\r\n",
			   p_ind->proto_id, p_ind->conn_handle, p_ind->remote_mtu, p_ind->identity_id);
		BT_DUMP16A("[APP] cid: ", p_ind->cid, p_ind->cid_num);
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_DISCONN_IND: {
		rtk_bt_ecfc_disconn_ind_t *p_ind = (rtk_bt_ecfc_disconn_ind_t *)param;
		printf("[APP] ECFC disconnect. proto_id(%d) conn_handle(%d) cid(%d) cause(0x%x)\r\n",
			   p_ind->proto_id, p_ind->conn_handle, p_ind->cid, p_ind->cause);
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_RECONF_REQ_IND: { //BT api shall not be called here
		rtk_bt_ecfc_reconf_req_ind_t *p_ind = (rtk_bt_ecfc_reconf_req_ind_t *)param;
		printf("[APP] ECFC reconfigure requset. proto_id(%d) conn_handle(%d) remote_mtu(%d) remote_mps(%d)\r\n",
			   p_ind->proto_id, p_ind->conn_handle, p_ind->remote_mtu, p_ind->remote_mps);
		BT_DUMP16A("[APP] cid: ", p_ind->cid, p_ind->cid_num);
		if (p_ind->remote_mtu < RTK_BT_GAP_ECFC_MIN_MTU || p_ind->remote_mps < RTK_BT_GAP_ECFC_MIN_MPS) {
			ret = RTK_BT_EVT_CB_REJECT;
			printf("[APP] Reject!\r\n");
		} else {
			ret = RTK_BT_EVT_CB_OK;
			printf("[APP] Accept!\r\n");
		}
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_RECONF_RSP_IND: {
		rtk_bt_ecfc_reconf_rsp_ind_t *p_ind = (rtk_bt_ecfc_reconf_rsp_ind_t *)param;
		if (p_ind->cause) {
			printf("[APP] ECFC reconfigure fail\r\n");
		} else {
			printf("[APP] ECFC reconfigure OK, local_mtu(%d) local mps(%d)\r\n",
				   p_ind->local_mtu, p_ind->local_mps);
			BT_DUMP16A("[APP] cid: ", p_ind->cid, p_ind->cid_num);
		}
		break;
	}
#endif
	default:
		printf("[APP] Unkown common gap cb evt type: %d\r\n", evt_code);
		break;
	}

	return ret;
}

#if (defined RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT && (RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT)) || \
    (defined RTK_BLE_AUDIO_RECORD_SUPPORT && (RTK_BLE_AUDIO_RECORD_SUPPORT))

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
		BT_DUMPD(__func__, p_data, data_len);
		p_iso_path->status_ok_cnt ++;
	} else {
		BT_LOGE("[APP] %s failed, iso_conn_handle 0x%x, seq_num %d, ret 0x%x\r\n", __func__, p_iso_path->iso_conn_handle, p_iso_path->pkt_seq_num, ret);
		BT_DUMPD(__func__, p_data, data_len);
		p_iso_path->status_fail_cnt++;
	}
	app_bt_le_audio_iso_data_tx_statistics(p_iso_path);

	return ret;
}
#endif

#if (defined RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT) && (RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT)
extern short audio_demo[];
extern uint32_t audio_demo_size;

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
			memcpy(p_iso_path->p_encode_data, p_pcm_data + pcm_num_offset, (pcm_total_num - pcm_num_offset) * bytes_per_num);
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
			app_bt_le_audio_parse_two_chnl_pcm_data_to_one_chnl(p_pcm_data + pcm_num_offset, (pcm_total_num - pcm_num_offset) * bytes_per_num / 2,
																p_iso_path->p_encode_data,
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

#if (defined RTK_BLE_AUDIO_RECORD_SUPPORT) && (RTK_BLE_AUDIO_RECORD_SUPPORT)
static uint16_t app_bt_le_audio_encode_record_data(app_lea_iso_data_path_t *p_iso_path)
{
	uint8_t bytes_per_num = (DEFAULT_PCM_BIT_WIDTH / 8), channels = 0;
	uint32_t encode_byte = 0, encode_num = 0, record_frame_size = 0;
	uint32_t sample_rate = 0, frame_duration_us = 0;
	uint32_t channel_allocation = 0;
	rtk_bt_le_audio_cfg_codec_t *p_codec = NULL;
	uint16_t read_size = 0;
	struct enc_codec_buffer *penc_codec_buffer_t = NULL;
	uint16_t ret = 0;

	if (!p_iso_path) {
		BT_LOGE("[APP] %s p_iso_path is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	} else {
		p_iso_path->is_processing = true;
		p_iso_path->iso_data_tx_queue_num ++;
		p_iso_path->p_enc_codec_buffer_t = NULL;
		p_codec = &p_iso_path->codec;
	}
	if (!p_codec) {
		BT_LOGE("[APP] %s p_codec is NULL\r\n", __func__);
		goto exit;
	}
	//audio record is 16K 2 channel 16bit array
	//parsing channels
	channel_allocation = p_codec->audio_channel_allocation;
	while (channel_allocation) {
		channels ++ ;
		channel_allocation &= (channel_allocation - 1);
	}
	sample_rate = app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(LEA_SOURCE_FIX_SAMPLE_FREQUENCY);
	//16k sample rate 2 channel  10 ms pcm data  16k/1000/1000 *10ms*1000 * 2*2 = 640 bytes
	frame_duration_us = (p_codec->frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500;
	record_frame_size = sample_rate * frame_duration_us / 1000 / 1000; // 160
	encode_num = record_frame_size * channels; // 160 * channels
	encode_byte = encode_num * bytes_per_num; // 160 * channels * 2 Bytes

	if (p_iso_path->p_encode_data == NULL) {
		p_iso_path->p_encode_data = (short *)osif_mem_alloc(RAM_TYPE_DATA_ON, encode_byte);
		if (p_iso_path->p_encode_data == NULL) {
			BT_LOGE("[APP] %s p_iso_path->p_encode_data alloc fail\r\n", __func__);
			goto exit;
		}
		memset(p_iso_path->p_encode_data, 0, encode_byte);
	}
	if (!p_iso_path->record_hdl) {
		BT_LOGE("[APP] %s audio record hdl is null\r\n", __func__);
		goto exit;
	}
	read_size = rtk_bt_audio_record_data_get(RTK_BT_AUDIO_CODEC_LC3,
											 p_iso_path->record_hdl,
											 p_iso_path->codec_entity,
											 (void *)p_iso_path->p_encode_data,
											 encode_byte, true);
	if (!read_size) {
		BT_LOGE("[APP] %s audio record data read size is 0, get fail \r\n", __func__);
		goto exit;
	}
	p_iso_path->encode_byte = encode_byte;
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

static void app_bt_le_audio_encode_task_entry(void *ctx)
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
#if defined(RTK_BLE_AUDIO_RECORD_SUPPORT) && RTK_BLE_AUDIO_RECORD_SUPPORT
	struct enc_codec_buffer *penc_codec_buffer_t = NULL;
	void *pcodec_entity = NULL;
#endif
	g_app_lea_encode_task.run = 1;
	//give sem each 10ms in
	while (g_app_lea_encode_task.run) {
		if (g_app_lea_encode_sem) {
			osif_sem_take(g_app_lea_encode_sem, BT_TIMEOUT_FOREVER);
		}
		BT_LOGD("[APP] %s sys_time %d\r\n", __func__, (int)osif_sys_time_get());
#if defined(RTK_BLE_AUDIO_RECORD_SUPPORT) && RTK_BLE_AUDIO_RECORD_SUPPORT
		penc_codec_buffer_t = NULL;
		pcodec_entity = NULL;
#endif
		tx_iso_data_path_num = app_bt_le_audio_iso_data_path_get_num(RTK_BLE_AUDIO_ISO_DATA_PATH_TX);
		for (i = 0 ; i < tx_iso_data_path_num; i++) {
			p_iso_path = app_bt_le_audio_iso_data_path_find_by_idx(i, RTK_BLE_AUDIO_ISO_DATA_PATH_TX);
			if (p_iso_path == NULL) {
				BT_LOGE("[APP] %s p_iso_path is NULL\r\n", __func__);
				continue;
			}
			//skip removing iso path
			if (p_iso_path->is_removing) {
				BT_LOGD("[APP] %s p_iso_path(%p) is removing\r\n", __func__, p_iso_path);
				continue;
			}
			//wait if encode task is fast and send task is slow
			if (p_iso_path->iso_data_tx_queue_num > 128) {
				BT_LOGD("[APP] %s encode is fast iso_data_tx_queue_num = %d, wait\r\n", __func__, p_iso_path->iso_data_tx_queue_num);
				continue;
			}
#if defined(RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT) && RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT
			if (RTK_BT_OK != app_bt_le_audio_encode_birds_sing(p_iso_path, p_pcm_data, pcm_total_num)) {
				printf("[APP] %s app_bt_le_audio_encode_birds_sing fail\r\n", __func__);
				continue;
			}
#endif
#if defined(RTK_BLE_AUDIO_RECORD_SUPPORT) && RTK_BLE_AUDIO_RECORD_SUPPORT
			/* currently, not support multiple record handle, so only fetch one record pcm data and copy which to other iso_path */
			if (i == 0) {
				if (RTK_BT_OK != app_bt_le_audio_encode_record_data(p_iso_path)) {
					printf("[APP] %s app_bt_le_audio_encode_record_data fail\r\n", __func__);
					continue;
				} else {
					penc_codec_buffer_t = p_iso_path->p_enc_codec_buffer_t;
					pcodec_entity = p_iso_path->codec_entity;
				}
			} else {
				if (penc_codec_buffer_t) {
					p_iso_path->is_processing = true;
					p_iso_path->iso_data_tx_queue_num++;
					p_iso_path->p_enc_codec_buffer_t = penc_codec_buffer_t;
				}
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
#if defined(RTK_BLE_AUDIO_RECORD_SUPPORT) && RTK_BLE_AUDIO_RECORD_SUPPORT
			/* currently, not support multiple record handle, so penc_codec_buffer_t should be free after the final iso data has been transmitted */
			if (i == (tx_iso_data_path_num - 1)) {
				if (penc_codec_buffer_t) {
					rtk_bt_audio_free_encode_buffer(RTK_BT_AUDIO_CODEC_LC3, pcodec_entity, penc_codec_buffer_t);
				}
			}
#endif
		}
		//osif_delay(1);
	}
	BT_LOGD("[APP] %s task_delete\r\n", __func__);
	osif_sem_give(g_app_lea_encode_task.sem);
	g_app_lea_encode_task.run = 0;
	g_app_lea_encode_task.hdl = NULL;
	osif_task_delete(NULL);
}

static void app_bt_le_audio_send_timer_handler(void *arg)
{
	(void)arg;
	uint8_t i = 0, tx_iso_data_path_num = 0;
	uint32_t sample_rate = 0;
	uint32_t frame_duration_us = 0;
	uint16_t frame_num = 0;
	tx_iso_data_path_num = app_bt_le_audio_iso_data_path_get_num(RTK_BLE_AUDIO_ISO_DATA_PATH_TX);
	app_lea_iso_data_path_t *p_iso_path = NULL;
	if (g_app_lea_encode_task.run) {
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
		if (g_app_lea_encode_sem) {
			osif_sem_give(g_app_lea_encode_sem);
		}
	}
}

static void app_bt_le_audio_send_timer_init(void)
{
#if defined(RTK_BLE_AUDIO_USE_HW_GTIMER) && RTK_BLE_AUDIO_USE_HW_GTIMER
	BT_LOGD("[APP] %s hw timer id %d,time_interval_us = %d\r\n", __func__, LEAUDIO_SEND_TIMER_ID, (int)g_le_audio_send_timer_interval_us);
	if (g_le_audio_send_timer.handler == NULL) {
		gtimer_init(&g_le_audio_send_timer, LEAUDIO_SEND_TIMER_ID);
		gtimer_start_periodical(&g_le_audio_send_timer, g_le_audio_send_timer_interval_us, (void *)app_bt_le_audio_send_timer_handler, NULL);
	}
#else
	BT_LOGD("[APP] %s sw time_interval_us = %d\r\n", __func__, (int)g_le_audio_send_timer_interval_us);
	if (g_le_audio_send_timer_interval_us % 1000 != 0) {
		BT_LOGE("[APP] %s error: interval %d(us) cannot use sw timer, please use hw timer instead\r\n", __func__, (int) g_le_audio_send_timer_interval_us);
		return;
	}
	if (!g_le_audio_send_timer) {
		if (false == osif_timer_create(&g_le_audio_send_timer, "le_audio_send_timer", 1, g_le_audio_send_timer_interval_us / 1000, true,
									   app_bt_le_audio_send_timer_handler)) {
			BT_LOGE("[APP] %s osif_timer_create fail\r\n", __func__);
			return;
		}
	}
	if (false == osif_timer_start(&g_le_audio_send_timer)) {
		BT_LOGE("[APP] %s osif_timer_start fail\r\n", __func__);
		return;
	}
#endif
}

static void app_bt_le_audio_send_timer_deinit(void)
{
#if defined(RTK_BLE_AUDIO_USE_HW_GTIMER) && RTK_BLE_AUDIO_USE_HW_GTIMER
	if (g_le_audio_send_timer.handler) {
		gtimer_stop(&g_le_audio_send_timer);
		gtimer_deinit(&g_le_audio_send_timer);
		g_le_audio_send_timer.handler = NULL;
	}
#else
	if (g_le_audio_send_timer) {
		if (false == osif_timer_stop(&g_le_audio_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_stop fail \r\n", __func__);
		}
		if (false == osif_timer_delete(&g_le_audio_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_delete fail \r\n", __func__);
		}
		g_le_audio_send_timer = NULL;
	}
#endif
}

static void app_bt_le_audio_send_timer_update(uint32_t time_interval_us)
{
	BT_LOGA("[APP] %s: time_interval_us=%d\r\n", __func__, (int)time_interval_us);
	g_le_audio_send_timer_interval_us = time_interval_us;
#if defined(RTK_BLE_AUDIO_USE_HW_GTIMER) && RTK_BLE_AUDIO_USE_HW_GTIMER
	if (g_le_audio_send_timer.handler) {
		gtimer_reload(&g_le_audio_send_timer, time_interval_us);
	} else {
		BT_LOGD("[APP] %s: timer not init\r\n", __func__);
	}
#else
	if (g_le_audio_send_timer) {
		if (false == osif_timer_stop(&g_le_audio_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_stop fail \r\n", __func__);
		}
		if (false == osif_timer_restart(&g_le_audio_send_timer, time_interval_us / 1000)) {
			BT_LOGE("[APP] %s osif_timer_start fail\r\n", __func__);
		}
	}
#endif
}

void app_bt_le_audio_gmap_encode_data_control(bool enable)
{
	BT_LOGA("[APP] %s %d\r\n", __func__, enable);
	if (enable) {
		if (g_app_lea_encode_enable == true) {
			BT_LOGE("[APP] %s: encode is alreay enabled\r\n", __func__);
			return;
		}
		g_app_lea_encode_enable = true;
		if (g_app_lea_encode_sem == NULL) {
			osif_sem_create(&g_app_lea_encode_sem, 0, 0xFFFFFFFF);
		}
		// encode task
		if (g_app_lea_encode_task.sem == NULL) {
			osif_sem_create(&g_app_lea_encode_task.sem, 0, 1);
		}
		if (g_app_lea_encode_task.hdl == NULL) {
			if (osif_task_create(&g_app_lea_encode_task.hdl, ((const char *)"lea_encode_task"), app_bt_le_audio_encode_task_entry,
								 NULL, LEA_ENCODE_TASK_STACK_SIZE, LEA_ENCODE_TASK_PRIO) != true) {
				BT_LOGE("[APP] %s xTaskCreate(lea_encode_task) failed\r\n", __func__);
			}
		}
		app_bt_le_audio_send_timer_init();
	} else {
		if (g_app_lea_encode_enable == false) {
			BT_LOGE("[APP] %s: encode is alreay disabled\r\n", __func__);
			return;
		}
		g_app_lea_encode_enable = false;
		app_bt_le_audio_send_timer_deinit();
		if (g_app_lea_encode_task.hdl) {
			g_app_lea_encode_task.run = 0;
			osif_sem_give(g_app_lea_encode_sem);
			osif_sem_take(g_app_lea_encode_task.sem, BT_TIMEOUT_FOREVER);
			osif_sem_delete(g_app_lea_encode_task.sem);
			g_app_lea_encode_task.sem = NULL;
			g_app_lea_encode_task.hdl = NULL;
		}
		if (g_app_lea_encode_sem) {
			osif_sem_delete(g_app_lea_encode_sem);
			g_app_lea_encode_sem = NULL;
		}
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

void app_bt_le_audio_gmap_decode_data_control(bool enable)
{
	BT_LOGD("[APP] %s %d\r\n", __func__, enable);
	if (enable) {
		if (g_app_lea_decode_enable == true) {
			BT_LOGE("[APP] %s: decode is alreay enabled\r\n", __func__);
			return;
		}
		g_app_lea_decode_enable = true;
	} else {
		if (g_app_lea_decode_enable == false) {
			BT_LOGE("[APP] %s: decode is alreay disabled\r\n", __func__);
			return;
		}
		g_app_lea_decode_enable = false;
	}
}

static uint16_t app_bt_le_audio_broadcast_source_setup_data_path(app_bt_le_audio_bap_broadcast_source_info_t *p_bap_bro_sour_info)
{
	uint8_t i = 0, bis_idx = 0;
	uint16_t ret = 0;
	rtk_bt_le_audio_cfg_codec_t *p_bis_codec_cfg = &p_bap_bro_sour_info->bis_codec_cfg;

	BT_LOGA("[APP] %s type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu 0x%x, octets_per_codec_frame 0x%x, audio_channel_allocation 0x%x)\r\n",
			__func__, p_bis_codec_cfg->type_exist, p_bis_codec_cfg->frame_duration,
			p_bis_codec_cfg->sample_frequency, p_bis_codec_cfg->codec_frame_blocks_per_sdu,
			p_bis_codec_cfg->octets_per_codec_frame, (unsigned int)p_bis_codec_cfg->audio_channel_allocation);

	//set up iso data path
	rtk_bt_le_audio_bis_data_path_param_t setup_path_param = {
		{RTK_BT_LE_LC3_CODEC_ID, 0, 0, 0, 0},           /*codec_id */
		0x1122,                                         /*controller_delay */
		0,                                              /*codec_config_len */
		NULL,                                           /*codec_data */
	};
	for (i = 0; i < p_bap_bro_sour_info->bis_info.num_bis; i++) {
		bis_idx = p_bap_bro_sour_info->bis_info.bis_conn_info[i].bis_idx;
		ret = rtk_bt_le_audio_broadcast_source_setup_data_path(p_bap_bro_sour_info->broadcast_source_handle, bis_idx, &setup_path_param,
															   &p_bap_bro_sour_info->bis_info.bis_conn_info[i].bis_conn_handle);
		BT_LOGA("[APP] broadcast source setup data path %s for broadcast_source_handle 0x%p bis_index %d,bis_conn_handle 0x%x, ret: 0x%x\r\n",
				((RTK_BT_OK != ret) ? "fail" : "ok"), p_bap_bro_sour_info->broadcast_source_handle, bis_idx, p_bap_bro_sour_info->bis_info.bis_conn_info[i].bis_conn_handle,
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
			ret = app_bt_le_audio_iso_data_path_add(RTK_BT_LE_AUDIO_BIS_MODE, bis_idx, RTK_BLE_AUDIO_ISO_DATA_PATH_TX,
													p_bap_bro_sour_info->bis_info.bis_conn_info[i].bis_conn_handle, p_bis_codec_cfg);
			if (ret == RTK_BT_OK) {
				app_bt_le_audio_send_timer_update((p_bis_codec_cfg->frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500);
			}
		}
	}

	return ret;
}

static uint16_t app_bt_le_audio_broadcast_sink_setup_data_path(rtk_bt_le_audio_sync_handle_t sync_handle)
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
		BT_LOGE("[APP] %s not find sync dev info for sync_handle %p\r\n", __func__, sync_handle);
		return RTK_BT_FAIL;
	}

	ret = rtk_bt_le_audio_sync_get_bis_info(sync_handle, &bis_info);
	if (ret != RTK_BT_OK) {
		BT_LOGE("[APP] rtk_bt_le_audio_sync_get_bis_info fail,ret = 0x%x\r\n", ret);
	}
	//set up iso data path
	for (i = 0; i < bis_info.num_bis; i++) {
		bis_idx = bis_info.bis_conn_info[i].bis_idx;
		bis_conn_handle = bis_info.bis_conn_info[i].bis_conn_handle;
		ret = rtk_bt_le_audio_sync_get_bis_sync_codec_cfg(sync_handle, bis_idx, &bis_codec_cfg);
		if (ret != RTK_BT_OK) {
			BT_LOGE("[APP] rtk_bt_le_audio_sync_get_bis_sync_codec_cfg fail,ret = 0x%x\r\n", ret);
			continue;
		} else {
			BT_LOGD("[APP] rtk_bt_le_audio_sync_get_bis_sync_codec_cfg ok (sync_handle %p, bis_idx %d,", sync_handle, bis_idx);
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
		BT_LOGA("[APP] broadcast sink setup data path %s for sync_handle 0x%p bis_index %d, ret: 0x%x\r\n",
				((RTK_BT_OK != ret) ? "fail" : "ok"), sync_handle, bis_idx, ret);

		if (ret != RTK_BT_OK) {
			BT_LOGE("[APP] rtk_bt_le_audio_sync_setup_data_path fail,bis_idx: 0x%x\r\n", bis_idx);
			continue;
		}
	}

	return ret;
}

#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
static bool record_handle_exist_check_func(void)
{
	/* check whether there is iso tx path and record handle is allocated */
	uint8_t tx_path_num = 0;
	bool found_record = false;
	app_lea_iso_data_path_t *p_iso_path = NULL;

	tx_path_num = app_bt_le_audio_iso_data_path_get_num(RTK_BLE_AUDIO_ISO_DATA_PATH_TX);
	if (tx_path_num) {
		for (uint8_t i = 0; i < tx_path_num; i ++) {
			p_iso_path = app_bt_le_audio_iso_data_path_find_by_idx(i, RTK_BLE_AUDIO_ISO_DATA_PATH_TX);
			if (p_iso_path) {
				if (p_iso_path->record_hdl) {
					found_record = true;
					BT_LOGA("[APP] Found record handle: index 0x%x, record handle 0x%p \r\n",
							i, p_iso_path->record_hdl);
					break;
				}
			}
		}
	}

	return found_record;
}
#endif

static rtk_bt_evt_cb_ret_t app_bt_le_audio_callback(uint8_t evt_code, void *data, uint32_t len)
{
	(void)len;
	uint16_t ret =  RTK_BT_OK;
	app_bt_le_audio_bap_unicast_server_info_t *p_bap_uni_ser_info = g_gmap_ugt_info.p_bap_uni_ser_info;
	app_bt_le_audio_bap_broadcast_sink_info_t *p_bap_bro_sink_info = g_gmap_bgr_info.p_bap_bro_sink_info;
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	app_bt_le_audio_group_info_t *p_group_info = NULL;

	switch (evt_code) {
	/*********************************************gmap event*********************************************/
	case RTK_BT_LE_AUDIO_EVT_GMAS_READ_IND: {
		rtk_bt_le_audio_gmas_read_ind_t *param = (rtk_bt_le_audio_gmas_read_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_READ_IND: conn_handle %d, cid %d, char_uuid %d\r\n",
				param->conn_handle, param->cid, param->char_uuid);
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_gmas_client_dis_t *param = (rtk_bt_le_audio_gmas_client_dis_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_DISCOVERY_DONE_IND: conn_handle %d,is_found %d, load_from_ftl %d\r\n",
				param->conn_handle, param->is_found, param->load_from_ftl);
		ret = rtk_bt_le_audio_gmas_client_read_role(param->conn_handle);
		if (ret) {
			BT_LOGE("[APP] %s gmas client read role failed\r\n", __func__);
			break;
		}
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_ROLE_RESULT: {
		rtk_bt_le_audio_gmas_client_read_role_result_t *param = (rtk_bt_le_audio_gmas_client_read_role_result_t *) data;
		if (param->gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_UGG) {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_ROLE_RESULT: GMAP Unicast Game Gateway\r\n");
		}
		if (param->gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_UGT) {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_ROLE_RESULT: GMAP Unicast Game Terminal\r\n");
		}
		if (param->gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGS) {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_ROLE_RESULT: GMAP Broadcast Game Sender\r\n");
		}
		if (param->gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGR) {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_ROLE_RESULT: GMAP Broadcast Game Receiver\r\n");
		}
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_ROLE_RESULT: conn_handle %d,cause %d \r\n", param->conn_handle, param->cause);

		ret = rtk_bt_le_audio_gmas_client_read_features(param->conn_handle, param->gmap_role);
		if (ret) {
			BT_LOGE("[APP] %s gmas client read features failed\r\n", __func__);
			break;
		}
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: {
		rtk_bt_le_audio_gmas_client_read_features_result_t *param = (rtk_bt_le_audio_gmas_client_read_features_result_t *) data;
		switch (param->char_uuid) {
		case GMAS_CHAR_UUID_UGG: {
			if (param->features & 0x01) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: UGG MULTIPLEX FEATURE SUPPORT\r\n");
			}
			if (param->features & 0x02) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: UGG 96 KBPS FEATURE SUPPORT\r\n");
			}
			if (param->features & 0x04) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: UGG MULTISINK FEATURE SUPPORT\r\n");
			}
			break;
		}
		case GMAS_CHAR_UUID_UGT: {
			if (param->features & 0x01) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: UGT SOURCE FEATURE SUPPORT\r\n");
			}
			if (param->features & 0x02) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: UGT 80 KBPS FEATURE SUPPORT\r\n");
			}
			if (param->features & 0x04) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: UGT SINK FEATURE SUPPORT\r\n");
			}
			if (param->features & 0x08) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: UGT 64 KBPS FEATURE SUPPORT\r\n");
			}
			if (param->features & 0x10) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: UGT MULTIPLEX FEATURE SUPPORT\r\n");
			}
			if (param->features & 0x20) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: UGT MULTISINK FEATURE SUPPORT\r\n");
			}
			if (param->features & 0x40) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: UGT MULTISOURCE FEATURE SUPPORT\r\n");
			}
			break;
		}
		case GMAS_CHAR_UUID_BGS: {
			if (param->features & 0x01) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: BGS 96 KBPS FEATURE SUPPORT\r\n");
			}
			break;
		}
		case GMAS_CHAR_UUID_BGR: {
			if (param->features & 0x01) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: BGR MULTISINK FEATURE SUPPORT\r\n");
			}
			if (param->features & 0x02) {
				BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_GMAS_CLIENT_READ_FEATURES_RESULT: BGR MULTIPLEX FEATURE SUPPORT\r\n");
			}
			break;
		}
		default:
			BT_LOGE("[APP] Unknown char_uuid: 0x%x\r\n", param->char_uuid);
			break;
		}
		break;
	}
	/*******************************************gmap event end*******************************************/
	/*******************************************bap_broadcast_source_event*******************************************/
	case RTK_BT_LE_AUDIO_EVT_BROADCAST_SOURCE_STATE_IND: {
		app_bt_le_audio_bap_broadcast_source_info_t *p_bap_bro_sour_info = g_gmap_bgs_info.p_bap_bro_sour_info;
		rtk_bt_le_audio_broadcast_source_state_ind_t *param = (rtk_bt_le_audio_broadcast_source_state_ind_t *)data;
		BT_LOGD("[APP] broadcast source state change: broadcast_source_handle: %p, broadcast_source_state 0x%x, cause: 0x%x\r\n",
				param->broadcast_source_handle, param->broadcast_source_state, param->cause);
		p_bap_bro_sour_info->broadcast_source_state = param->broadcast_source_state;
		if (param->broadcast_source_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_IDLE) {
			BT_LOGA("[APP] broadcast source idle\r\n");
			app_bt_le_audio_gmap_encode_data_control(false);
		} else if (param->broadcast_source_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_CONFIGURED) {
			BT_LOGA("[APP] broadcast source configured\r\n");
			if (param->cause == 0) {
				memcpy(def_le_audio_broadcast_source_create_big_param.broadcast_code, app_lea_def_broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
				ret = rtk_bt_le_audio_broadcast_source_enable(p_bap_bro_sour_info->broadcast_source_handle, &def_le_audio_broadcast_source_create_big_param);
				if (ret != RTK_BT_OK) {
					BT_LOGE("[APP] rtk_bt_le_audio_broadcast_source_enable fail,ret = 0x%x\r\n", ret);
					break;
				}
			}
		} else if (param->broadcast_source_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING) {
			BT_LOGA("[APP] broadcast source streaming\r\n");
			if (param->cause == 0) {
				app_bt_le_audio_broadcast_source_get_codec_from_level2(bap_broadcast_source_codec_level2, bap_broadcast_source_codec_level2_len,
																	   &p_bap_bro_sour_info->bis_codec_cfg);
				ret = app_bt_le_audio_broadcast_source_setup_data_path(p_bap_bro_sour_info);
				if (ret != RTK_BT_OK) {
					BT_LOGE("[APP] app_bt_le_audio_broadcast_source_setup_data_path fail,ret = 0x%x\r\n", ret);
					break;
				}
				app_bt_le_audio_gmap_encode_data_control(true);
			}
		} else if (param->broadcast_source_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING_STARTING) {
			BT_LOGA("[APP] broadcast source streaming starting\r\n");
			g_gmap_bgs_info.status = RTK_BLE_AUDIO_INITIATOR_BROADCAST_START;
		} else if (param->broadcast_source_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING_STOPPING) {
			BT_LOGA("[APP] broadcast source streaming stopping\r\n");
			g_gmap_bgs_info.status = RTK_BLE_AUDIO_INITIATOR_BROADCAST_STOP;
		}
		break;
	}
	/*******************************************bap_broadcast_source_event end******************************************/
	/***********************************************bap_uni_client_event************************************************/
	case RTK_BT_LE_AUDIO_EVT_BAP_DISCOVERY_DONE_IND: {
		app_bt_le_audio_device_info_t *p_device_info = NULL;
		rtk_bt_le_audio_bap_discovery_done_ind_t *param = (rtk_bt_le_audio_bap_discovery_done_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BAP_DISCOVERY_DONE_IND(conn_handle %d, pacs_is_found %d,sink_pac_num %d, source_pac_num %d\r\n",
				param->conn_handle, param->pacs_is_found, param->sink_pac_num, param->source_pac_num);
		BT_LOGA("[APP] ascs_is_found 0x%x, sink_ase_num %d, source_ase_num %d, bass_is_found %d, brs_char_num %d)\r\n",
				param->ascs_is_found, param->sink_ase_num, param->source_ase_num, param->bass_is_found, param->brs_char_num);
		//check wheather it support le audio bass
		if (gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGS) {
			if (param->bass_is_found == 0) {
				BT_LOGE("[APP] conn_handle(%d) not support bass, skip it\r\n", param->conn_handle);
				break;
			}
		} else {
			if (param->pacs_is_found == 0 || param->ascs_is_found == 0) {
				BT_LOGE("[APP] conn_handle(%d) not support pacs and ascs, skip it\r\n", param->conn_handle);
				break;
			}
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
			if (gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGS) {
				if (param->brs_char_num) {
					app_bt_le_audio_bass_brs_char_init(p_device_info, param->brs_char_num);
				}
			}
			/* when enable RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT, group info will be add by rtk_bt_le_audio_csis_set_coordinator_add_group
			within event RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_READ_RESULT_IND by bt stack */
#if !defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) || !RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
			if (gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_UGG) {
				app_bt_le_audio_new_device_add_in_group(param->conn_handle, &g_gmap_ugg_info.group_handle);
			}
			if (gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGS) {
				app_bt_le_audio_new_device_add_in_group(param->conn_handle, &g_gmap_bgs_info.group_handle);
			}
#endif
		}
		printf("[APP] Discovery done for conn_handle %d\r\n", param->conn_handle);
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_BAP_STATE_IND: {
		rtk_bt_le_audio_bap_state_ind_t *param = (rtk_bt_le_audio_bap_state_ind_t *)data;
		BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_BAP_STATE_IND(cause 0x%x, group_handle %p,stream_session_handle %p, curr_action %d, stream_state = %d, result= 0x%x), heap %d\r\n",
				param->cause, param->group_handle, param->stream_session_handle, param->curr_action, param->stream_state, param->result, osif_mem_peek(RAM_TYPE_DATA_ON));
		p_group_info = app_bt_le_audio_group_list_find(param->group_handle);
		if (p_group_info == NULL) {
			BT_LOGE("[APP] %s: p_group_info is NULL\r\n", __func__);
			break;
		}
		p_group_info->bap_state = param->stream_state;
		if (param->stream_state == RTK_BT_LE_AUDIO_STREAM_STATE_STREAMING || param->stream_state == RTK_BT_LE_AUDIO_STREAM_STATE_PARTIAL_STREAMING) {
			app_bt_le_audio_gmap_encode_data_control(true);
			g_gmap_ugg_info.status = RTK_BLE_AUDIO_INITIATOR_UNICAST_START;
			if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION) {
				//init rx thread
				app_bt_le_audio_gmap_decode_data_control(true);
			}
		} else if (param->stream_state == RTK_BT_LE_AUDIO_STREAM_STATE_STOPPING ||
				   param->stream_state == RTK_BT_LE_AUDIO_STREAM_STATE_RELEASING) {
			app_bt_le_audio_gmap_encode_data_control(false);
			g_gmap_ugg_info.status = RTK_BLE_AUDIO_INITIATOR_UNICAST_STOP;
			if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION) {
				//deinit rx thread
				app_bt_le_audio_gmap_decode_data_control(false);
			}
		} else if (param->stream_state == RTK_BT_LE_AUDIO_STREAM_STATE_IDLE) {
			BT_LOGD("[APP] %s: RTK_BT_LE_AUDIO_STREAM_STATE_IDLE \r\n", __func__);
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BAP_START_QOS_CFG_IND: {
		rtk_bt_le_audio_bap_start_qos_cfg_ind_t *param = (rtk_bt_le_audio_bap_start_qos_cfg_ind_t *)data;
		BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_BAP_START_QOS_CFG_IND(group handle %p, session handle %p, sink presentation delay(0x%x-0x%x),source presentation delay(0x%x-0x%x), sink latency 0x%x, source latency 0x%x),",
				param->group_handle, param->stream_session_handle,
				(unsigned int)param->sink_presentation_delay_min, (unsigned int)param->sink_presentation_delay_max,
				(unsigned int)param->source_presentation_delay_min, (unsigned int)param->source_presentation_delay_max,
				(unsigned int)param->sink_transport_latency_max, (unsigned int)param->source_transport_latency_max);
		BT_LOGD("[APP] sink_preferred_presentation_delay(0x%x-0x%x), source_preferred_presentation_delay(0x%x-0x%x)\r\n",
				(unsigned int)param->sink_preferred_presentation_delay_min, (unsigned int)param->sink_preferred_presentation_delay_max,
				(unsigned int)param->source_preferred_presentation_delay_min, (unsigned int)param->source_preferred_presentation_delay_max);
		rtk_bt_le_audio_unicast_ase_qos_t snk_ase_qos = {0};
		rtk_bt_le_audio_unicast_ase_qos_t src_ase_qos = {0};
		//config session_qos when received RTK_BT_LE_AUDIO_EVT_BAP_START_QOS_CFG_IND, change the config if need
		p_group_info = app_bt_le_audio_group_list_find(param->group_handle);
		if (p_group_info) {
			app_bt_le_audio_gmap_ase_qos_config(app_qos_level, app_sink_ase_qos,
												&p_group_info->session_qos, &snk_ase_qos);
			memcpy(param->p_session_qos, &p_group_info->session_qos, sizeof(rtk_bt_le_audio_unicast_session_qos_t));
			memcpy(param->p_snk_ase_qos, &snk_ase_qos, sizeof(rtk_bt_le_audio_unicast_ase_qos_t));
			app_bt_le_audio_gmap_ase_qos_config(app_qos_level, app_src_ase_qos,
												&p_group_info->session_qos, &src_ase_qos);
			memcpy(param->p_src_ase_qos, &src_ase_qos, sizeof(rtk_bt_le_audio_unicast_ase_qos_t));
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BAP_START_METADATA_CFG_IND: {
		rtk_bt_le_audio_bap_start_metadata_cfg_ind_t *param = (rtk_bt_le_audio_bap_start_metadata_cfg_ind_t *)data;
		BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_BAP_START_METADATA_CFG_IND(group handle %p, session handle %p, dev_handle %p, ase_id %d)\r\n",
				param->group_handle, param->stream_session_handle, param->device_handle, param->ase_id);
		*param->p_metadata_len = app_lea_def_metadata_len;
		memcpy(param->p_metadata, app_lea_def_metadata, app_lea_def_metadata_len);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BAP_SETUP_DATA_PATH_IND: {
		rtk_bt_le_audio_bap_setup_data_path_ind_t *param = (rtk_bt_le_audio_bap_setup_data_path_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BAP_SETUP_DATA_PATH_IND(group handle %p, session handle %p, dev_handle %p, ase_id %d,path_direction=%d,cis_conn_handle=0x%x\r\n",
				param->group_handle, param->stream_session_handle, param->device_handle, param->ase_id, param->path_direction, param->cis_conn_handle);
		BT_LOGD("[APP] type_exist 0x%x, frame_duration %d, sample_frequency %d, codec_frame_blocks_per_sdu %d,octets_per_codec_frame=%d,audio_channel_allocation=0x%x)\r\n",
				param->codec_parsed_data.type_exist, param->codec_parsed_data.frame_duration, param->codec_parsed_data.sample_frequency,
				param->codec_parsed_data.codec_frame_blocks_per_sdu, param->codec_parsed_data.octets_per_codec_frame,
				(unsigned int)param->codec_parsed_data.audio_channel_allocation);
		// get ase qos
		rtk_bt_le_audio_unicast_ase_qos_t ase_qos = {0};
		if (rtk_bt_le_audio_unicast_get_ase_qos(param->stream_session_handle, param->device_handle, param->ase_id, &ase_qos)) {
			BT_LOGE("[APP] %s: get ase qos failed \r\n", __func__);
			break;
		}
		BT_LOGA("[APP] group handle %p, session handle %p, dev_handle %p, ase_id %d, phy=0x%x, max_sdu=%d, retransmission_number=%u \r\n",
				param->group_handle, param->stream_session_handle, param->device_handle, param->ase_id, ase_qos.phy, ase_qos.max_sdu, ase_qos.retransmission_number);
		//add the iso data path when data path setup
		if (!app_bt_le_audio_iso_data_path_add(RTK_BT_LE_AUDIO_CIS_MODE, 0, param->path_direction, param->cis_conn_handle, &param->codec_parsed_data)) {
			if (param->path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
				app_bt_le_audio_send_timer_update((param->codec_parsed_data.frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500);
			}
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BAP_REMOVE_DATA_PATH_IND: {
		rtk_bt_le_audio_bap_remove_data_path_ind_t *param = (rtk_bt_le_audio_bap_remove_data_path_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BAP_REMOVE_DATA_PATH_IND(group handle %p, session handle %p, dev_handle %p, ase_id %d,path_direction=%d,cis_conn_handle=0x%x,cause=0x%x)\r\n",
				param->group_handle, param->stream_session_handle, param->device_handle, param->ase_id, param->path_direction, param->cis_conn_handle, param->cause);
		//remove the iso conn handle when tx data path remove
		app_bt_le_audio_iso_data_path_remove(param->cis_conn_handle, param->path_direction);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_GROUP_DEV_MSG_IND: {
		rtk_bt_le_audio_group_dev_msg_ind_t *param = (rtk_bt_le_audio_group_dev_msg_ind_t *)data;
		BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_GROUP_DEV_MSG_IND:msg_type 0x%x, cause 0x%x, group_handle %p, dev_handle %p\r\n",
				param->msg_type, param->cause, param->group_handle, param->device_handle);
		p_group_info = app_bt_le_audio_group_list_find(param->group_handle);
		if (p_group_info == NULL) {
			BT_LOGE("[APP] %s: p_group_info is NULL\r\n", __func__);
			break;
		}
		switch (param->msg_type) {
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_CONN:
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_CONN\r\n");
			break;
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_DISCONN:
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_DISCONN\r\n");
			app_bt_le_audio_group_list_remove_dev(param->group_handle, param->device_handle);
			if (gmap_role == RTK_BT_LE_AUDIO_GMAP_ROLE_BGS || gmap_role == RTK_BT_LE_AUDIO_GMAP_ROLE_UGG) {
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
				ret = rtk_bt_le_audio_csis_set_coordinator_cfg_discover(param->group_handle, true, RTK_BLE_AUDIO_DEFAULT_CSIS_DISV_TIMEOUT);
				BT_LOGA("[APP] %s: start csis discover in csis group %s (group_handle=%p) \r\n", __func__, (RTK_BT_OK != ret) ? "fail" : "ok", param->group_handle);
				csip_discover_flag = true;
				// transfer to ext scanning state
				app_bt_le_audio_scan_dev_list_remove_all();
				ret = rtk_bt_le_gap_ext_scan_set_param(&app_lea_def_ext_scan_param);
				if (RTK_BT_OK == ret) {
					ret = rtk_bt_le_gap_ext_scan_start();
				}
				BT_LOGA("[APP] %s: start ext scan in csis group(%p) %s \r\n", __func__, param->group_handle, (RTK_BT_OK != ret) ? "fail" : "ok");
				// set ext scan time out
				if (gmap_ext_scan_timer) {
					gmap_ext_scan_time_remaining = 10;
					if (false == osif_timer_start(&gmap_ext_scan_timer)) {
						BT_LOGE("[APP] %s osif_timer_start fail \r\n", __func__);
					}
					BT_LOGA("[APP] %s: ext scan timer start\r\n", __func__);
				}
#endif
			} else {
				app_bt_le_audio_gmap_encode_data_control(false);
				if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION) {
					//deinit rx thread
					app_bt_le_audio_gmap_decode_data_control(false);
				}
			}
			break;
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_REMOVE:
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_REMOVE\r\n");
			ret = rtk_bt_le_audio_group_delete_device(param->group_handle, param->device_handle);
			app_bt_le_audio_group_list_remove_dev(param->group_handle, param->device_handle);
			BT_LOGD("[APP] %s: delete device in group %s (group_handle=%p,device_handle=%p) \r\n", __func__, (RTK_BT_OK != ret) ? "fail" : "ok",
					param->group_handle, param->device_handle);
			break;
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_CLEAR:
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_CLEAR\r\n");
			break;
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_EMPTY:
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_EMPTY\r\n");
			app_bt_le_audio_group_list_remove(param->group_handle);
			break;
		default:
			BT_LOGE("[APP] %s unsupport group dev msg 0x%x\r\n", __func__, param->msg_type);
			break;
		}
		break;
	}
	/*********************************************bap_uni_client_event end**********************************************/
	/*********************************************bap_unicast_server_event**********************************************/
	case RTK_BT_LE_AUDIO_EVT_ASCS_CP_CODEC_CFG_IND: {
		rtk_bt_le_audio_ascs_cp_codec_cfg_ind_t *param = (rtk_bt_le_audio_ascs_cp_codec_cfg_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CP_CODEC_CFG_IND(conn_handle %d, ase_id 0x%x\r\n",
				param->conn_handle, param->ase_id);
		BT_LOGD("[APP] type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu 0x%x, octets_per_codec_frame 0x%x, audio_channel_allocation 0x%x)\r\n",
				param->codec_cfg.type_exist, param->codec_cfg.frame_duration, param->codec_cfg.sample_frequency, param->codec_cfg.codec_frame_blocks_per_sdu,
				param->codec_cfg.octets_per_codec_frame, (unsigned int) param->codec_cfg.audio_channel_allocation);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_CP_QOS_CFG_IND: {
		rtk_bt_le_audio_ascs_cp_qos_cfg_ind_t *param = (rtk_bt_le_audio_ascs_cp_qos_cfg_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CP_QOS_CFG_IND(conn_handle %d, ase_id 0x%x, cig_id 0x%x, cis_id 0x%x,",
				param->conn_handle, param->ase_id, param->cig_id, param->cis_id);
		BT_LOGD("[APP] framing 0x%x, phy 0x%x, retransmission_number 0x%x, max_sdu 0x%x, max_transport_latency 0x%x, sdu_interval  0x%x, presentation_delay 0x%x)\r\n",
				param->framing, param->phy, param->retransmission_number, param->max_sdu,
				param->max_transport_latency, (unsigned int)param->sdu_interval, (unsigned int)param->presentation_delay);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_CP_ENABLE_IND: {
		rtk_bt_le_audio_ascs_cp_enable_ind_t *param = (rtk_bt_le_audio_ascs_cp_enable_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CP_ENABLE_IND(conn_handle %d, ase_num 0x%x",
				param->conn_handle, param->ase_num);
#if 0
		for (uint8_t i = 0; i < param->ase_num; i++) {
			printf(", ase id[%d] 0x%x", i, param->ase_id[i]);
		}
		printf(")\r\n");
#endif
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_CP_DISABLE_IND: {
		rtk_bt_le_audio_ascs_cp_disable_ind_t *param = (rtk_bt_le_audio_ascs_cp_disable_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CP_DISABLE_IND(conn_handle %d, ase_num 0x%x",
				param->conn_handle, param->ase_num);
#if 0
		for (uint8_t i = 0; i < param->ase_num; i++) {
			printf(", ase id[%d] 0x%x", i, param->ase_id[i]);
		}
		printf(")\r\n");
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
			.supported_framing = RTK_BLE_AUDIO_UNFRAMED_SUPPORTED,
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

	case RTK_BT_LE_AUDIO_EVT_ASCS_ASE_STATE_IND: {
		rtk_bt_le_audio_ascs_ase_state_ind_t *param = (rtk_bt_le_audio_ascs_ase_state_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_ASE_STATE_IND(conn_handle %d, ase_id 0x%x, audio_role 0x%x, ase_state 0x%x)\r\n",
				param->conn_handle, param->ase_id, param->audio_role, param->ase_state);
		switch (param->ase_state) {
		case RTK_BLE_AUDIO_ASCS_ASE_STATE_IDLE:
			BT_LOGA("[APP] (ASCS_ASE_STATE_IDLE)\r\n");
			if (param->audio_role == RTK_BLE_AUDIO_SINK) {
				if (p_bap_uni_ser_info->enabling_sink_ase_num > 0) {
					p_bap_uni_ser_info->enabling_sink_ase_num--;
				} else {
					BT_LOGE("[APP] enabling_sink_ase_num is already %d, something is wrong!!\r\n", p_bap_uni_ser_info->enabling_sink_ase_num);
					break;
				}
			} else if (param->audio_role == RTK_BLE_AUDIO_SOURCE) {
				if (p_bap_uni_ser_info->enabling_source_ase_num > 0) {
					p_bap_uni_ser_info->enabling_source_ase_num--;
				} else {
					BT_LOGE("[APP] enabling_source_ase_num is already %d, something is wrong!!\r\n", p_bap_uni_ser_info->enabling_source_ase_num);
					break;
				}
			}
			BT_LOGA("[APP] enabling_sink_ase_num=%d,enabling_source_ase_num=%d\r\n", p_bap_uni_ser_info->enabling_sink_ase_num,
					p_bap_uni_ser_info->enabling_source_ase_num);
			break;
		case RTK_BLE_AUDIO_ASCS_ASE_STATE_CODEC_CONFIGURED:
			BT_LOGA("[APP] (ASCS_ASE_STATE_CODEC_CONFIGURED)\r\n");
			break;
		case RTK_BLE_AUDIO_ASCS_ASE_STATE_QOS_CONFIGURED:
			BT_LOGA("[APP] (ASCS_ASE_STATE_QOS_CONFIGURED)\r\n");
			break;
		case RTK_BLE_AUDIO_ASCS_ASE_STATE_ENABLING:
			BT_LOGA("[APP] (ASCS_ASE_STATE_ENABLING)\r\n");
			if (param->audio_role == RTK_BLE_AUDIO_SINK) {
				if (p_bap_uni_ser_info->enabling_sink_ase_num < p_bap_uni_ser_info->config_sink_ase_num) {
					p_bap_uni_ser_info->enabling_sink_ase_num++;
				} else {
					BT_LOGE("[APP] enabling_sink_ase_num(%d) >= config_sink_ase_num(%d), something is wrong!!\r\n", p_bap_uni_ser_info->enabling_sink_ase_num,
							p_bap_uni_ser_info->config_sink_ase_num);
					break;
				}
			} else if (param->audio_role == RTK_BLE_AUDIO_SOURCE) {
				if (p_bap_uni_ser_info->enabling_source_ase_num < p_bap_uni_ser_info->config_source_ase_num) {
					p_bap_uni_ser_info->enabling_source_ase_num++;
				} else {
					BT_LOGE("[APP] enabling_source_ase_num(%d) >= config_source_ase_num(%d), something is wrong!!\r\n", p_bap_uni_ser_info->enabling_source_ase_num,
							p_bap_uni_ser_info->config_source_ase_num);
					break;
				}
			}
			BT_LOGA("[APP] enabling_sink_ase_num=%d,enabling_source_ase_num=%d\r\n", p_bap_uni_ser_info->enabling_sink_ase_num,
					p_bap_uni_ser_info->enabling_source_ase_num);
			break;
		case RTK_BLE_AUDIO_ASCS_ASE_STATE_STREAMING:
			BT_LOGA("[APP] (ASCS_ASE_STATE_STREAMING)\r\n");
			g_gmap_ugt_info.status = RTK_BLE_AUDIO_ACCEPTOR_START;
			if (param->audio_role == RTK_BLE_AUDIO_SINK) {
				if (p_bap_uni_ser_info->streaming_sink_ase_num < p_bap_uni_ser_info->enabling_sink_ase_num) {
					p_bap_uni_ser_info->streaming_sink_ase_num++;
				} else {
					BT_LOGE("[APP] streaming_sink_ase_num(%d) >= enabling_sink_ase_num(%d), something is wrong!!\r\n", p_bap_uni_ser_info->streaming_sink_ase_num,
							p_bap_uni_ser_info->enabling_source_ase_num);
					break;
				}
				BT_LOGA("[APP] streaming_sink_ase_num=%d,enabling_sink_ase_num=%d\r\n", p_bap_uni_ser_info->streaming_sink_ase_num,
						p_bap_uni_ser_info->enabling_sink_ase_num);
				if (p_bap_uni_ser_info->streaming_sink_ase_num == p_bap_uni_ser_info->enabling_sink_ase_num) {
					//init rx thread
					app_bt_le_audio_gmap_decode_data_control(true);
				}
			} else if (param->audio_role == RTK_BLE_AUDIO_SOURCE) {
				if (p_bap_uni_ser_info->streaming_source_ase_num < p_bap_uni_ser_info->enabling_source_ase_num) {
					p_bap_uni_ser_info->streaming_source_ase_num++;
				} else {
					BT_LOGE("[APP] streaming_source_ase_num(%d) >= enabling_source_ase_num(%d), something is wrong!!\r\n",
							p_bap_uni_ser_info->streaming_source_ase_num, p_bap_uni_ser_info->enabling_source_ase_num);
					break;
				}
				if (p_bap_uni_ser_info->streaming_source_ase_num == p_bap_uni_ser_info->enabling_source_ase_num) {
					//init tx thread in at cmd
				}
			}
			break;
		case RTK_BLE_AUDIO_ASCS_ASE_STATE_DISABLING: //there is no disabling state inidicate, use idle instead?
			BT_LOGA("[APP] (ASCS_ASE_STATE_DISABLING)\r\n");
			break;
		case RTK_BLE_AUDIO_ASCS_ASE_STATE_RELEASING:
			BT_LOGA("[APP] (ASCS_ASE_STATE_RELEASING)\r\n");
			if (param->audio_role == RTK_BLE_AUDIO_SINK) {
				if (p_bap_uni_ser_info->streaming_sink_ase_num > 0) {
					p_bap_uni_ser_info->streaming_sink_ase_num--;
				} else {
					BT_LOGE("[APP] streaming_sink_ase_num is already %d, something is wrong!!\r\n", p_bap_uni_ser_info->streaming_sink_ase_num);
					break;
				}
				BT_LOGD("[APP] streaming_sink_ase_num=%d\r\n", p_bap_uni_ser_info->streaming_sink_ase_num);
				if (p_bap_uni_ser_info->streaming_sink_ase_num == 0) {
					//deinit rx thread
					app_bt_le_audio_gmap_decode_data_control(false);
				}
			} else if (param->audio_role == RTK_BLE_AUDIO_SOURCE) {
				if (p_bap_uni_ser_info->streaming_source_ase_num > 0) {
					p_bap_uni_ser_info->streaming_source_ase_num--;
				} else {
					BT_LOGE("[APP] streaming_source_ase_num is already %d, something is wrong!!\r\n", p_bap_uni_ser_info->streaming_source_ase_num);
					break;
				}
				if (p_bap_uni_ser_info->streaming_source_ase_num == 0) {
					//deinit tx thread
					app_bt_le_audio_gmap_encode_data_control(false);
				}
			}
			break;
		default:
			break;
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_SETUP_DATA_PATH_IND: {
		rtk_bt_le_audio_ascs_setup_data_path_ind_t *param = (rtk_bt_le_audio_ascs_setup_data_path_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_SETUP_DATA_PATH_IND(conn_handle %d, ase_id 0x%x, path_direction 0x%x, cis_conn_handle 0x%x\r\n",
				param->conn_handle, param->ase_id, param->path_direction, param->cis_conn_handle);
		BT_LOGA("[APP] type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu 0x%x, octets_per_codec_frame 0x%x, audio_channel_allocation 0x%x)\r\n",
				param->codec_cfg.type_exist, param->codec_cfg.frame_duration, param->codec_cfg.sample_frequency, param->codec_cfg.codec_frame_blocks_per_sdu,
				param->codec_cfg.octets_per_codec_frame, (unsigned int)param->codec_cfg.audio_channel_allocation);

		//add the iso data path when data path setup
		if (!app_bt_le_audio_iso_data_path_add(RTK_BT_LE_AUDIO_CIS_MODE, 0, param->path_direction, param->cis_conn_handle, &param->codec_cfg)) {
			if (param->path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
				app_bt_le_audio_gmap_encode_data_control(true);
				app_bt_le_audio_send_timer_update((param->codec_cfg.frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500);
			}
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_ASCS_REMOVE_DATA_PATH_IND: {
		rtk_bt_le_audio_ascs_remove_data_path_ind_t *param = (rtk_bt_le_audio_ascs_remove_data_path_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_REMOVE_DATA_PATH_IND(conn_handle %d, ase_id 0x%x, path_direction 0x%x, cis_conn_handle 0x%x\r\n",
				param->conn_handle, param->ase_id, param->path_direction, param->cis_conn_handle);
		//remove the iso conn handle when tx data path remove
		app_bt_le_audio_iso_data_path_remove(param->cis_conn_handle, param->path_direction);
		g_gmap_ugt_info.status = RTK_BLE_AUDIO_ACCEPTOR_STOP;
		break;
	}
	/******************************************end bap_unicast_server_event*********************************************/
	/*********************************************bap_broadcast_sink_event**********************************************/
	case RTK_BT_LE_AUDIO_EVT_ISO_DATA_RECEIVE_IND: {
		rtk_bt_le_audio_direct_iso_data_ind_t *p_bt_direct_iso = (rtk_bt_le_audio_direct_iso_data_ind_t *)data;
		app_lea_iso_data_path_t *p_iso_path = NULL;
		if (p_bt_direct_iso->iso_sdu_len) {
			BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_ISO_DATA_RECEIVE_IND, sys_time=%d,iso_conn_handle 0x%x, pkt_status_flag 0x%x, pkt_seq_num 0x%x, ts_flag 0x%x, time_stamp 0x%x,iso_sdu_len 0x%x, p_buf %p, buf_len %d, offset %d\r\n",
					(int)osif_sys_time_get(), p_bt_direct_iso->iso_conn_handle, p_bt_direct_iso->pkt_status_flag,
					p_bt_direct_iso->pkt_seq_num, p_bt_direct_iso->ts_flag,
					(unsigned int)p_bt_direct_iso->time_stamp, p_bt_direct_iso->iso_sdu_len, p_bt_direct_iso->p_buf, p_bt_direct_iso->buf_len, p_bt_direct_iso->offset);
			p_iso_path = app_bt_le_audio_iso_data_path_find(p_bt_direct_iso->iso_conn_handle, RTK_BLE_AUDIO_ISO_DATA_PATH_RX);
			if (p_iso_path == NULL) {
				BT_LOGE("[APP] %s p_iso_path is NULL\r\n", __func__);
				break;
			}
			if (g_app_lea_decode_enable) {
				p_iso_path->iso_data_rx_queue_num++;
				if (rtk_bt_audio_recvd_data_in(RTK_BT_AUDIO_CODEC_LC3,
											   p_iso_path->track_hdl,
											   p_iso_path->codec_entity,
											   p_bt_direct_iso->p_buf + p_bt_direct_iso->offset,
											   p_bt_direct_iso->iso_sdu_len)) {
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
		BT_LOGD("[APP] broadcast sink pa sync state change: sync_handle: %p, sync_state 0x%x, action 0x%x, cause: 0x%x\r\n",
				param->sync_handle, param->sync_state, param->action, param->cause);
		p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s not find sync dev info for sync_handle %p\r\n", __func__, param->sync_handle);
			break;
		}
		p_sync_dev_info->pa_sync_state = param->sync_state;
		if (param->sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZED) {
			BT_LOGA("[APP] broadcast sink pa sync synchronized\r\n");
		} else if (param->sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_TERMINATED) {
			BT_LOGA("[APP] broadcast sink pa sync termiated\r\n");
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_PA_BIG_INFO_IND: {
		rtk_bt_le_audio_pa_biginfo_ind_t *param = (rtk_bt_le_audio_pa_biginfo_ind_t *)data;
		BT_LOGD("[APP] broadcast sink pa biginfo indication: sync_handle: %p, num_bis 0x%x, sync_id 0x%x, pa_sync_handle 0x%x, encryption 0x%x\r\n",
				param->sync_handle, param->biginfo.num_bis, param->biginfo.sync_id, param->biginfo.pa_sync_handle, param->biginfo.encryption);
		if (gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGR) {
			p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
			if (!p_sync_dev_info) {
				BT_LOGE("[APP] %s not find sync dev info for sync_handle %p\r\n", __func__, param->sync_handle);
				break;
			}
			p_sync_dev_info->is_encryption = param->biginfo.encryption;
			if (p_sync_dev_info->big_sync_state == RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZING ||
				p_sync_dev_info->big_sync_state == RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZED) {
				BT_LOGD("[APP] skip big info ind when RTK_BLE_AUDIO_BROADCAST_SINK_BIG_SYNC_SYNCED\r\n");
				break;
			}
			if (p_sync_dev_info->is_past) {
				BT_LOGD("[APP] skip broadcast sink big sync establish when PAST\r\n");
				break;
			}
			//give sem to establish big sync
			if (p_bap_bro_sink_info->big_info_ind_sem) {
				osif_sem_give(p_bap_bro_sink_info->big_info_ind_sem);
			}
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_PA_ADV_REPORT_IND: {
		rtk_bt_le_audio_pa_adv_report_ind_t *param = (rtk_bt_le_audio_pa_adv_report_ind_t *)data;
		BT_LOGD("[APP] broadcast sink pa adv report: sync_handle: %p,sync_id %d, pa_sync_handle 0x%x,tx_power %d, rssi %d, cte_type %d, data_status 0x%x, data_len %d\r\n",
				param->sync_handle, param->sync_id, param->pa_sync_handle, param->tx_power, param->rssi, param->cte_type, param->data_status, param->data_len);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BASE_DATA_MAPPING_MODIFY_IND: {
		rtk_bt_le_audio_base_data_mapping_modify_ind_t *param = (rtk_bt_le_audio_base_data_mapping_modify_ind_t *)data;
		BT_LOGD("[APP] broadcast sink base data modify: sync_handle: %p, base_data_mapping %p,\r\n", param->sync_handle, param->base_data_mapping);
		p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s not find sync dev info for sync_handle %p\r\n", __func__, param->sync_handle);
			break;
		}
		if (param->base_data_mapping != p_sync_dev_info->base_data_mapping) {
			p_sync_dev_info->base_data_mapping = param->base_data_mapping;
			BT_LOGD("[APP] save new base_data_mapping %p for sync_handle %p\r\n", param->base_data_mapping, param->sync_handle);
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BIG_SYNC_STATE_IND: {
		rtk_bt_le_audio_big_sync_state_ind_t *param = (rtk_bt_le_audio_big_sync_state_ind_t *)data;
		BT_LOGD("[APP] broadcast sink big sync state change: sync_handle: %p, sync_state 0x%x, action 0x%x, cause: 0x%x\r\n",
				param->sync_handle, param->sync_state, param->action, param->cause);
		p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s not find sync dev info for sync_handle %p\r\n", __func__, param->sync_handle);
			break;
		}
		p_sync_dev_info->big_sync_state = param->sync_state;
		if (param->sync_state == RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZED && param->action == RTK_BT_LE_AUDIO_BIG_SYNC) {
			BT_LOGA("[APP] broadcast sink big sync synchronized\r\n");
			//setup data path
			ret = app_bt_le_audio_broadcast_sink_setup_data_path(param->sync_handle);
			BT_LOGA("[APP] app_bt_le_audio_broadcast_sink_setup_data_path %s after big sync synchronized! ret: 0x%x\r\n",
					((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
			if (p_sync_dev_info->is_past == false) {
				//terminate PA sync to save bandwitch
				ret = rtk_bt_le_audio_pa_sync_terminate(param->sync_handle);
				BT_LOGA("[APP] rtk_bt_le_audio_pa_sync_terminate %s after big sync synchronized! ret: 0x%x\r\n",
						((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
				//stop ext scan
				ret = rtk_bt_le_gap_ext_scan_stop();
				BT_LOGA("[APP] rtk_bt_le_gap_ext_scan_stop %s after big sync synchronized! ret: 0x%x\r\n",
						((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
			}
			app_bt_le_audio_gmap_decode_data_control(true);
		} else if (param->sync_state == RTK_BT_LE_AUDIO_BIG_SYNC_STATE_TERMINATED) {
			BT_LOGA("[APP] broadcast sink big sync termiated\r\n");
			ret = rtk_bt_le_audio_sync_release(param->sync_handle);
			BT_LOGA("[APP] rtk_bt_le_audio_sync_release %s, ret: 0x%x\r\n", ((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
			//deinit rx thread
			app_bt_le_audio_gmap_decode_data_control(false);
			ret = app_bt_le_audio_sync_dev_list_remove(p_sync_dev_info->sync_handle);
			BT_LOGA("[APP] %s: app_bt_le_audio_sync_dev_list_remove %s, ret: 0x%x\r\n",
					__func__, ((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
		}
		break;
	}
	/*********************************************bap_broadcast_sink_event end**********************************************/
	/********************************************bap_broadcast_delegator_event**********************************************/
	case RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: {
		rtk_bt_le_audio_bass_cp_ind_t *param = (rtk_bt_le_audio_bass_cp_ind_t *) data;
		switch (param->opcode) {
		case RTK_BT_LE_AUDIO_BASS_CP_REMOTE_SCAN_STOP:
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: conn_handle %d RTK_BT_LE_AUDIO_BASS_CP_REMOTE_SCAN_STOP\r\n", param->conn_handle);
			break;
		case RTK_BT_LE_AUDIO_BASS_CP_REMOTE_SCAN_START:
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: conn_handle %d RTK_BT_LE_AUDIO_BASS_CP_REMOTE_SCAN_START\r\n", param->conn_handle);
			break;
		case RTK_BT_LE_AUDIO_BASS_CP_ADD_SOURCE: {
			char addr_str[32] = {0};
			rtk_bt_le_addr_to_str(&param->op_param.add_source_param.adv_addr.addr_val, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: conn_handle %d RTK_BT_LE_AUDIO_BASS_CP_ADD_SOURCE,type %d, addr_val %s,broadcast_id[%x%x%x],pa_sync %d,pa_interval %d,num_subgroups %d\r\n",
					param->conn_handle, param->op_param.add_source_param.adv_addr.type, addr_str,
					param->op_param.add_source_param.broadcast_id[2], param->op_param.add_source_param.broadcast_id[1], param->op_param.add_source_param.broadcast_id[0],
					param->op_param.add_source_param.pa_sync, param->op_param.add_source_param.pa_interval, param->op_param.add_source_param.num_subgroups);
			if (param->op_param.add_source_param.num_subgroups) {
				BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: bis_info_size %d, bis_sync_state %d, metadata_len %d\r\n",
						param->op_param.add_source_param.bis_info_size, (unsigned int)param->op_param.add_source_param.p_cp_bis_info[0].bis_sync_state,
						param->op_param.add_source_param.p_cp_bis_info[0].metadata_len);
				BT_DUMPD(__func__, param->op_param.add_source_param.p_cp_bis_info[0].p_metadata,
						 param->op_param.add_source_param.p_cp_bis_info[0].metadata_len);
			}
			break;
		}
		case RTK_BT_LE_AUDIO_BASS_CP_MODIFY_SOURCE: {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: conn_handle %d RTK_BT_LE_AUDIO_BASS_CP_MODIFY_SOURCE,source_id %d,pa_sync %d,pa_interval %d,num_subgroups %d\r\n",
					param->conn_handle, param->op_param.modify_source_param.source_id,
					param->op_param.modify_source_param.pa_sync, param->op_param.modify_source_param.pa_interval, param->op_param.modify_source_param.num_subgroups);
			if (param->op_param.modify_source_param.num_subgroups) {
				BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: bis_info_size %d, bis_sync_state %d, metadata_len %d\r\n",
						param->op_param.modify_source_param.bis_info_size, (unsigned int)param->op_param.modify_source_param.p_cp_bis_info[0].bis_sync_state,
						param->op_param.modify_source_param.p_cp_bis_info[0].metadata_len);
				BT_DUMPD(__func__, param->op_param.modify_source_param.p_cp_bis_info[0].p_metadata,
						 param->op_param.modify_source_param.p_cp_bis_info[0].metadata_len);
			}
			break;
		}
		case RTK_BT_LE_AUDIO_BASS_CP_REMOVE_SOURCE: {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: conn_handle %d RTK_BT_LE_AUDIO_BASS_CP_REMOVE_SOURCE source_id %d\r\n", param->conn_handle,
					param->op_param.remove_source_param.source_id);
			p_sync_dev_info = app_bt_le_audio_sync_dev_list_find_by_source_id(param->op_param.remove_source_param.source_id);
			if (!p_sync_dev_info) {
				BT_LOGE("[APP] %s app_bt_le_audio_sync_dev_list_find fail\r\n", __func__);
				break;
			}
			app_bt_le_audio_sync_dev_list_remove(p_sync_dev_info->sync_handle);
			break;
		}
		case RTK_BT_LE_AUDIO_BASS_CP_SET_BROADCAST_CODE: {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_CP_IND: conn_handle %d RTK_BT_LE_AUDIO_BASS_CP_SET_BROADCAST_CODE source_id %d\r\n", param->conn_handle,
					param->op_param.remove_source_param.source_id);
			BT_DUMPA(__func__, param->op_param.set_broadcast_code_param.broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
			break;
		}
		default:
			break;
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BASS_BRS_MODIFY_IND: {
		rtk_bt_le_audio_bass_brs_modify_ind_t *param = (rtk_bt_le_audio_bass_brs_modify_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BASS_BRS_MODIFY_IND: sync_handle %p, source_id %d\r\n", param->sync_handle, param->source_id);
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
				BT_DUMPD(__func__, param->p_brs_data->p_cp_bis_info[0].p_metadata, param->p_brs_data->p_cp_bis_info[0].metadata_len);
			}
			p_sync_dev_info->adv_sid = param->p_brs_data->source_adv_sid;
			memcpy(&p_sync_dev_info->adv_addr, &param->p_brs_data->source_address, sizeof(rtk_bt_le_addr_t));
			memcpy(p_sync_dev_info->broadcast_id, param->p_brs_data->broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BASS_BA_ADD_SOURCE_IND: {
		rtk_bt_le_audio_bass_ba_add_source_ind_t *param = (rtk_bt_le_audio_bass_ba_add_source_ind_t *)data;
		BT_LOGA("[APP] scan delegator ba add source ind: sync_handle: %p,source_id %d\r\n", param->sync_handle, param->source_id);
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
		BT_LOGA("[APP] broadcast sink get pa sync param ind: sync_handle: %p,source_id %d,is_past: %d,pa_interval %d\r\n",
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
		BT_LOGA("[APP] broadcast sink get big sync param ind: sync_handle: %p,source_id %d\r\n", param->sync_handle, param->source_id);
		memcpy(param->p_big_sync_param, &big_sync_param, sizeof(rtk_bt_le_audio_bass_big_sync_param_t));
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BASS_GET_BROADCAST_CODE_IND: { //BT api shall not be called here
		rtk_bt_le_audio_bass_get_broadcast_code_ind_t *param = (rtk_bt_le_audio_bass_get_broadcast_code_ind_t *)data;
		BT_LOGA("[APP] broadcast sink get broadcast code ind: sync_handle: %p,source_id %d\r\n", param->sync_handle, param->source_id);
		p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s app_bt_le_audio_sync_dev_list_find fail\r\n", __func__);
			break;
		}
		BT_DUMPD(__func__,  p_sync_dev_info->broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
		memcpy(param->p_broadcast_code, p_sync_dev_info->broadcast_code, RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BASS_GET_PREFER_BIS_SYNC_IND: { //BT api shall not be called here
		rtk_bt_le_audio_bass_get_prefer_bis_sync_ind_t *param = (rtk_bt_le_audio_bass_get_prefer_bis_sync_ind_t *)data;
		BT_LOGA("[APP] broadcast sink set prefer bis sync ind: sync_handle: %p,source_id %d\r\n",
				param->sync_handle, param->source_id);
		p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
		if (p_sync_dev_info) {
			BT_LOGE("[APP] %s app_bt_le_audio_sync_dev_list_add fail\r\n", __func__);
			break;
		}
		*param->p_support_bis_array = p_sync_dev_info->support_bis_array;
		break;
	}
		/***********************************************end bap_broadcast_delegator_event*******************************************/
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
	/****************************************************vcp_volume_render_event************************************************/
	case RTK_BT_LE_AUDIO_EVT_VCS_VOLUME_CP_IND: {
		float audio_track_volume = 0;
		uint8_t target_volume = 0;
		rtk_bt_le_audio_vcs_volume_cp_ind_t *param = (rtk_bt_le_audio_vcs_volume_cp_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VCS_VOLUME_CP_IND: conn_handle %d, volume_setting 0x%x, mute %d,cp_op %d\r\n",
				param->conn_handle, param->volume_setting, param->mute, param->cp_op);
		switch (param->cp_op) {
		case RTK_BT_LE_AUDIO_VCS_CP_RELATIVE_VOLUME_DOWN:
			BT_LOGA("[APP] Relative Volume Down\r\n");
			target_volume = MAX(param->volume_setting - RTK_BLE_AUDIO_DEFAULT_VCS_STEP_SIZE, 0);
			audio_track_volume = 1.0 * param->volume_setting / RTK_BLE_AUDIO_MAX_VCS_VOL_SETTING;
			rtk_bt_audio_track_set_hardware_volume(audio_track_volume, audio_track_volume);
			break;
		case RTK_BT_LE_AUDIO_VCS_CP_RELATIVE_VOLUME_UP:
			BT_LOGA("[APP] Relative Volume Up\r\n");
			target_volume = MIN(param->volume_setting + RTK_BLE_AUDIO_DEFAULT_VCS_STEP_SIZE, RTK_BLE_AUDIO_MAX_VCS_VOL_SETTING);
			audio_track_volume = 1.0 * target_volume / RTK_BLE_AUDIO_MAX_VCS_VOL_SETTING;
			rtk_bt_audio_track_set_hardware_volume(audio_track_volume, audio_track_volume);
			break;
		case RTK_BT_LE_AUDIO_VCS_CP_UNMUTE_RELATIVE_VOLUME_DOWN:
			BT_LOGA("[APP] Unmute/Relative Volume Down\r\n");
			target_volume = MAX(param->volume_setting - RTK_BLE_AUDIO_DEFAULT_VCS_STEP_SIZE, 0);
			audio_track_volume = 1.0 * target_volume / RTK_BLE_AUDIO_MAX_VCS_VOL_SETTING;
			rtk_bt_audio_track_set_hardware_volume(audio_track_volume, audio_track_volume);
			rtk_bt_audio_track_set_mute(false);
			break;
		case RTK_BT_LE_AUDIO_VCS_CP_UNMUTE_RELATIVE_VOLUME_UP:
			BT_LOGA("[APP] Unmute/Relative Volume Up\r\n");
			target_volume = MIN(param->volume_setting + RTK_BLE_AUDIO_DEFAULT_VCS_STEP_SIZE, RTK_BLE_AUDIO_MAX_VCS_VOL_SETTING);
			audio_track_volume = 1.0 * target_volume / RTK_BLE_AUDIO_MAX_VCS_VOL_SETTING;
			rtk_bt_audio_track_set_hardware_volume(audio_track_volume, audio_track_volume);
			rtk_bt_audio_track_set_mute(false);
			break;
		case RTK_BT_LE_AUDIO_VCS_CP_SET_ABSOLUTE_VOLUME:
			BT_LOGA("[APP] Set Absolute Volume,volume_setting=0x%x\r\n", param->volume_setting);
			audio_track_volume = 1.0 * param->volume_setting / RTK_BLE_AUDIO_MAX_VCS_VOL_SETTING;
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
		/**************************************************end vcp_volume_render_event**********************************************/
#endif
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
	/***************************************************vcp_volume_controller_event*********************************************/
	case RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_vcs_client_discovery_done_ind_t *param = (rtk_bt_le_audio_vcs_client_discovery_done_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_DISCOVERY_DONE_IND: conn_handle %d,is_found %d,load_from_ftl %d,type_exist 0x%x\r\n",
				param->conn_handle, param->is_found, param->load_from_ftl, param->type_exist);
		app_bt_le_audio_device_info_t *p_device_info = NULL;
		p_device_info = app_bt_le_audio_device_list_add(param->conn_handle);
		if (p_device_info) {
			p_device_info->vcs_is_found = param->is_found;
			p_device_info->vcs_type_exist = param->type_exist;
		}
	}
	break;
	case RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_STATE_IND: {
		app_bt_le_audio_device_info_t *p_device_info = NULL;
		rtk_bt_le_audio_vcs_client_volume_state_ind_t *param = (rtk_bt_le_audio_vcs_client_volume_state_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_STATE_IND: conn_handle %d, is_notify 0x%x, volume_setting 0x%x, mute 0x%x, change_counter 0x%x\r\n",
				param->conn_handle, param->is_notify, param->volume_state.volume_setting, param->volume_state.mute, param->volume_state.change_counter);
		p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(param->conn_handle);
		if (p_device_info) {
			memcpy(&p_device_info->vcs_volume_state, &param->volume_state, sizeof(rtk_bt_le_audio_vcs_volume_state_t));
		}
	}
	break;
	case RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_FLAG_IND: {
		app_bt_le_audio_device_info_t *p_device_info = NULL;
		rtk_bt_le_audio_vcs_client_volume_flag_ind_t *param = (rtk_bt_le_audio_vcs_client_volume_flag_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_FLAG_IND: conn_handle %d, is_notify 0x%x,volume_flags 0x%x\r\n",
				param->conn_handle, param->is_notify, param->volume_flags);
		p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(param->conn_handle);
		if (p_device_info) {
			p_device_info->vcs_volume_flags = param->volume_flags;
		}
	}
	break;
		/**************************************************end vcp_volume_controller_event******************************************/
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
	/****************************************************micp_mic_controller_event**********************************************/
	case RTK_BT_LE_AUDIO_EVT_MICS_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_mics_client_discovery_done_ind_t *param = (rtk_bt_le_audio_mics_client_discovery_done_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MICS_CLIENT_DISCOVERY_DONE_IND: conn_handle %d,is_found %d,load_from_ftl %d,mic_mute 0x%x\r\n",
				param->conn_handle, param->is_found, param->load_from_ftl, param->mic_mute);
		app_bt_le_audio_device_info_t *p_device_info = NULL;
		p_device_info = app_bt_le_audio_device_list_add(param->conn_handle);
		if (p_device_info) {
			p_device_info->mics_is_found = param->is_found;
			p_device_info->mic_mute = param->mic_mute;
		}
	}
	break;
	case RTK_BT_LE_AUDIO_EVT_MICS_CLIENT_NOTIFY_IND: {
		rtk_bt_le_audio_mics_client_notify_ind_t *param = (rtk_bt_le_audio_mics_client_notify_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MICS_CLIENT_NOTIFY_IND: conn_handle %d,mic_mute %d\r\n", param->conn_handle, param->mic_mute);
		app_bt_le_audio_device_info_t *p_device_info = NULL;
		p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(param->conn_handle);
		if (p_device_info) {
			p_device_info->mic_mute = param->mic_mute;
		}
	}
	break;
		/**************************************************end micp_mic_controller_event********************************************/
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	/******************************************************vocs_client_event****************************************************/
	case RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_vocs_client_discovery_done_ind_t *param = (rtk_bt_le_audio_vocs_client_discovery_done_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_DISCOVERY_DONE_IND: conn_handle %d,is_found %d, load_from_ftl %d, srv_num %d\r\n",
				param->conn_handle, param->is_found, param->load_from_ftl, param->srv_num);
		app_bt_le_audio_device_info_t *p_device_info = NULL;
		p_device_info = app_bt_le_audio_device_list_add(param->conn_handle);
		if (p_device_info) {
			p_device_info->vocs_is_found = param->is_found;
			p_device_info->vocs_srv_num = param->srv_num;
		}
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
			break;
		}
		case RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_LOCATION: {
			BT_LOGA("[APP] vocs char gain setting prop: srv_instance_id %d, audio_location %d\r\n",
					param->srv_instance_id, (unsigned int)param->data.audio_location);
			break;
		}
		case RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_OUTPUT_DESC:
			BT_LOGA("[APP] vocs char output des: srv_instance_id %d, output_des_len %d, p_output_des %s\r\n",
					param->srv_instance_id, param->data.output_des.output_des_len, param->data.output_des.p_output_des);
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
	}
	break;
	case RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_CP_RESULT_IND: {
		rtk_bt_le_audio_vocs_client_cp_result_ind_t *param = (rtk_bt_le_audio_vocs_client_cp_result_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_CP_RESULT_IND: conn_handle 0x%x, cause 0x%x, srv_instance_id %d, cp_op=%d\r\n",
				param->conn_handle, param->cause, param->srv_instance_id, param->cp_op);
	}
	break;
		/****************************************************end vocs_client_event**************************************************/
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	/******************************************************aics_client_event****************************************************/
	case RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_aics_client_discovery_done_ind_t *param = (rtk_bt_le_audio_aics_client_discovery_done_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_DISCOVERY_DONE_IND: conn_handle %d,is_found %d, load_from_ftl %d, srv_num %d\r\n",
				param->conn_handle, param->is_found, param->load_from_ftl, param->srv_num);
		app_bt_le_audio_device_info_t *p_device_info = NULL;
		p_device_info = app_bt_le_audio_device_list_add(param->conn_handle);
		if (p_device_info) {
			p_device_info->aics_is_found = param->is_found;
			p_device_info->aics_srv_num = param->srv_num;
		}
		for (uint8_t i = 0; i < param->srv_num; i++) {
			if (!param->load_from_ftl) {
				rtk_bt_le_audio_aics_cfg_cccd(param->conn_handle, i, RTK_BT_LE_AUDIO_AICS_INPUT_STATE_FLAG | RTK_BT_LE_AUDIO_AICS_INPUT_STATUS_FLAG
											  | RTK_BT_LE_AUDIO_AICS_INPUT_DES_FLAG | RTK_BT_LE_AUDIO_AICS_INPUT_TYPE_FLAG
											  | RTK_BT_LE_AUDIO_AICS_GAIN_SETTING_PROP_FLAG, true);
			}
		}
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
			break;
		}
		case RTK_BT_LE_AUDIO_AICS_CHAR_GAIN_SETTING_PROP: {
			BT_LOGA("[APP] aics char gain setting prop: srv_instance_id %d, gain_setting_units %d, gain_setting_min %d, gain_setting_max %d\r\n",
					param->srv_instance_id, param->data.setting_prop.gain_setting_units, param->data.setting_prop.gain_setting_min, param->data.setting_prop.gain_setting_max);
			break;
		}
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_TYPE:
			BT_LOGA("[APP] aics char input type: srv_instance_id %d, input type %d\r\n", param->srv_instance_id, param->data.input_type);
			break;
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_STATUS:
			BT_LOGA("[APP] aics char input status: srv_instance_id %d, input status %d\r\n", param->srv_instance_id, param->data.input_status);
			break;
		case RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_DES:
			BT_LOGA("[APP] aics char input des: srv_instance_id %d, input_des_len %d, p_input_des %s\r\n",
					param->srv_instance_id, param->data.input_des.input_des_len, param->data.input_des.p_input_des);
			break;
		default:
			break;
		}
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_NOTIFY_IND: {
		rtk_bt_le_audio_aics_client_notify_ind_t *param = (rtk_bt_le_audio_aics_client_notify_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_NOTIFY_IND: conn_handle %d,srv_instance_id %d, type %d\r\n",
				param->conn_handle, param->srv_instance_id, param->type);
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_CP_RESULT_IND: {
		rtk_bt_le_audio_aics_client_cp_result_ind_t *param = (rtk_bt_le_audio_aics_client_cp_result_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_CP_RESULT_IND: conn_handle %d,srv_instance_id %d, cp_op %d, cause 0x%x\r\n",
				param->conn_handle, param->srv_instance_id, param->cp_op, param->cause);
		break;
	}
		/****************************************************end aics_client_event**************************************************/
#endif
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
	/***********************************************csip_set_coordinator_event**************************************************/
	case RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_group_handle_t *p_group_handle = NULL;
		rtk_bt_le_audio_csis_client_discovery_done_ind_t *param = (rtk_bt_le_audio_csis_client_discovery_done_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_DISCOVERY_DONE_IND: conn_handle %d,is_found %d, load_from_ftl %d, srv_num %d \r\n",
				param->conn_handle, param->is_found, param->load_from_ftl, param->srv_num);
		app_bt_le_audio_device_info_t *p_device_info = NULL;
		p_device_info = app_bt_le_audio_device_list_add(param->conn_handle);
		if (p_device_info) {
			p_device_info->csis_is_found = param->is_found;
			p_device_info->csis_srv_num = param->srv_num;
		}
		//remote device not support csis
		if (param->is_found == false) {
			if (gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_BGS) {
				p_group_handle = &g_gmap_bgs_info.group_handle;
			} else if (gmap_role & RTK_BT_LE_AUDIO_GMAP_ROLE_UGG) {
				p_group_handle = &g_gmap_ugg_info.group_handle;
			} else {
				BT_LOGE("[APP] %s unknown role 0x%x \r\n", __func__, gmap_role);
				break;
			}
			app_bt_le_audio_new_device_add_in_group(param->conn_handle, p_group_handle);
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_READ_RESULT_IND: {
		rtk_bt_le_audio_csis_client_read_result_ind_t *param = (rtk_bt_le_audio_csis_client_read_result_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_READ_RESULT_IND: cause 0x%x, conn_handle %d, group_handle 0x%p, dev_handle 0x%p,\r\n",
				param->cause, param->conn_handle, param->group_handle, param->dev_handle);
		BT_LOGA("[APP] srv_instance_id %d, char_exit 0x%x, srv_uuid 0x%x, rank %d, set_mem_size %d\r\n",
				param->mem_info.srv_instance_id, param->mem_info.char_exit, param->mem_info.srv_uuid, param->mem_info.rank, param->mem_info.set_mem_size);
		if (param->cause == 0) {
			app_bt_le_audio_group_info_t *p_group_info = NULL;
			rtk_bt_le_audio_group_handle_t group_handle = NULL;
			rtk_bt_le_audio_device_handle_t device_handle = NULL;
			if (param->group_handle == NULL) {//the server isnt in any group
				ret = rtk_bt_le_audio_csis_set_coordinator_add_group(&param->mem_info, &group_handle, &device_handle);
				BT_LOGA("[APP] %s: add device in csis group %s (group_handle=%p,device_handle=%p) \r\n", __func__, (RTK_BT_OK != ret) ? "fail" : "ok", group_handle,
						device_handle);
				if (RTK_BT_OK == ret) {
					p_group_info = app_bt_le_audio_group_list_add_dev(group_handle, device_handle, param->conn_handle);
					if (p_group_info) {
						p_group_info->set_mem_size = param->mem_info.set_mem_size;
					}
					if (param->mem_info.set_mem_size > 1) {
						ret = rtk_bt_le_audio_csis_set_coordinator_cfg_discover(group_handle, true, RTK_BLE_AUDIO_DEFAULT_CSIS_DISV_TIMEOUT);
						BT_LOGA("[APP] %s: start csis discover in csis group %s (group_handle=%p) \r\n", __func__, (RTK_BT_OK != ret) ? "fail" : "ok", group_handle);
						app_bt_le_audio_scan_dev_list_remove_all();
						csip_discover_flag = true;
						ret = rtk_bt_le_gap_ext_scan_set_param(&app_lea_def_ext_scan_param);
						if (RTK_BT_OK == ret) {
							ret = rtk_bt_le_gap_ext_scan_start();
						}
						BT_LOGA("[APP] %s: start ext scan in csis group(%p) %s \r\n", __func__, group_handle, (RTK_BT_OK != ret) ? "fail" : "ok");
					}
					// set ext scan time out
					if (gmap_ext_scan_timer) {
						gmap_ext_scan_time_remaining = 10;
						if (false == osif_timer_start(&gmap_ext_scan_timer)) {
							BT_LOGE("[APP] %s osif_timer_start fail \r\n", __func__);
						}
						BT_LOGA("[APP] %s: ext scan timer start\r\n", __func__);
					}
				}
			} else {
				if (param->dev_handle == NULL) {
					ret = rtk_bt_le_audio_csis_set_coordinator_add_device(param->group_handle, &param->mem_info, &device_handle);
					BT_LOGA("[APP] %s: add device in csis group %s (group_handle=%p,device_handle=%p) \r\n", __func__, (RTK_BT_OK != ret) ? "fail" : "ok",
							param->group_handle, device_handle);
					p_group_info = app_bt_le_audio_group_list_add_dev(param->group_handle, device_handle, param->conn_handle);
					if (p_group_info) {
						p_group_info->set_mem_size = param->mem_info.set_mem_size;
					}
				} else {
					BT_LOGA("[APP] %s: add device in csis group %s (group_handle=%p,device_handle=%p) \r\n", __func__, (RTK_BT_OK != ret) ? "fail" : "ok",
							param->group_handle, param->dev_handle);
					app_bt_le_audio_group_list_add_dev(param->group_handle, param->dev_handle, param->conn_handle);
				}
			}
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SEARCH_DONE_IND: {
		rtk_bt_le_audio_csis_client_search_done_ind_t *param = (rtk_bt_le_audio_csis_client_search_done_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SEARCH_DONE_IND: group_handle 0x%p, set_mem_size %d, search_done %d,search_timeout %d\r\n",
				param->group_handle, param->set_mem_size, param->search_done, param->search_timeout);
		ret = rtk_bt_le_audio_csis_set_coordinator_cfg_discover(param->group_handle, false, 0);
		BT_LOGA("[APP] %s: cancel csis discover in csis group %s (group_handle=%p) \r\n", __func__, (RTK_BT_OK != ret) ? "fail" : "ok",
				param->group_handle);
		csip_discover_flag = false;
		// avoid close ext scan again
		if (gmap_ext_scan_timer) {
			osif_timer_stop(&gmap_ext_scan_timer);
		}
		ret = rtk_bt_le_gap_ext_scan_stop();
		BT_LOGA("[APP] %s: stop ext scan in csis group(%p) %s \r\n", __func__, param->group_handle, (RTK_BT_OK != ret) ? "fail" : "ok");
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SET_MEM_FOUND_IND: {
		char le_addr[30] = {0};
		rtk_bt_le_audio_csis_client_set_mem_found_ind_t *param = (rtk_bt_le_audio_csis_client_set_mem_found_ind_t *) data;
		rtk_bt_le_addr_to_str(&(param->bd_addr), le_addr, sizeof(le_addr));
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SET_MEM_FOUND_IND: group_handle %p, dev_handle %p, bd_addr %s, addr_type %d, srv_uuid 0x%x, rank %d, set_mem_size %d\r\n",
				param->group_handle, param->dev_handle, le_addr, param->bd_addr.type,
				param->srv_uuid, param->rank, param->set_mem_size);
		// auto connect to new member
		rtk_bt_le_create_conn_param_t conn_param;
		memcpy(&conn_param, &app_lea_def_conn_param, sizeof(rtk_bt_le_create_conn_param_t));
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
		/*********************************************end csip_set_coordinator_event************************************************/
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
	/***************************************************micp_mic_device_event***************************************************/
	case RTK_BT_LE_AUDIO_EVT_MICS_WRITE_MUTE_IND: {
		rtk_bt_le_audio_mics_write_mute_ind_t *param = (rtk_bt_le_audio_mics_write_mute_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MICS_WRITE_MUTE_IND: mic_mute %d\r\n", param->mic_mute);
		//call record api
		if (!record_handle_exist_check_func()) {
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
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MICS_WRITE_MUTE_IND: mic device set mute wrong cmd !!!\r\n");
		}
		break;
	}
		/*************************************************end micp_mic_device_event*************************************************/
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	/*****************************************************vocs_server_event*****************************************************/
	case RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_OFFSET_STATE_IND: {
		rtk_bt_le_audio_vocs_write_offset_state_ind_t *param = (rtk_bt_le_audio_vocs_write_offset_state_ind_t *)data;
		float left_volume = 0, right_volume = 0;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_OFFSET_STATE_IND: conn_handle 0x%x, srv_instance_id %d, volume_offset %d\r\n",
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
		break;
	}
		/***************************************************end vocs_server_event***************************************************/
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	/*****************************************************aics_server_event*****************************************************/
	case RTK_BT_LE_AUDIO_EVT_AICS_CP_IND: {
		rtk_bt_le_audio_aics_cp_ind_t *param = (rtk_bt_le_audio_aics_cp_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_AICS_CP_IND: conn_handle %d, srv_instance_id %d, cp_op 0x%x, gain_setting %d\r\n",
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
				if (!record_handle_exist_check_func()) {
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
				if (!record_handle_exist_check_func()) {
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
				if (!record_handle_exist_check_func()) {
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
			BT_LOGD("[APP] %s default evt_code 0x%04x\r\n", __func__, param->cp_op);
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
		/***************************************************end aics_server_event***************************************************/
#endif

	default:
		BT_LOGD("[APP] default evt_code 0x%04x\r\n", evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t app_le_audio_ugg_gattc_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;
	if (RTK_BT_GATTC_EVT_MTU_EXCHANGE == event) {
		rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;
		if (p_gatt_mtu_ind->result == RTK_BT_OK) {
			printf("[APP] GATTC mtu exchange success, mtu_size: %d, conn_handle: %d \r\n",
				   p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
		} else {
			printf("[APP] GATTC mtu exchange fail \r\n");
		}
		return RTK_BT_EVT_CB_OK;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t app_bt_le_audio_ugt_gatts_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;
	if (RTK_BT_GATTS_EVT_MTU_EXCHANGE == event) {
		rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;
		if (p_gatt_mtu_ind->result == RTK_BT_OK) {
			printf("[APP] GATTS mtu exchange successfully, mtu_size: %d, conn_handle: %d \r\n",
				   p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
		} else {
			printf("[APP] GATTS mtu exchange fail \r\n");
		}
		return RTK_BT_EVT_CB_OK;
	}

	if (RTK_BT_GATTS_EVT_CLIENT_SUPPORTED_FEATURES == event) {
		rtk_bt_gatts_client_supported_features_ind_t *p_ind = (rtk_bt_gatts_client_supported_features_ind_t *)data;
		if (p_ind->features & RTK_BT_GATTS_CLIENT_SUPPORTED_FEATURES_EATT_BEARER_BIT) {
			printf("[APP] Client Supported features is writed: conn_handle %d, features 0x%02X. Remote client supports EATT.\r\n",
				   p_ind->conn_handle, p_ind->features);
		}
		return RTK_BT_EVT_CB_OK;
	}
	return RTK_BT_EVT_CB_OK;
}

static uint16_t app_bt_le_audio_bgs_init(void)
{
	/* intialize iso data path */
	app_bt_le_audio_iso_data_path_init();
	/* intialize group list */
	app_bt_le_audio_group_list_init();
	app_bt_le_audio_scan_dev_list_init();

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_bgs_deinit(void)
{
	/* deintialize group list */
	app_bt_le_audio_scan_dev_list_deinit();
	app_bt_le_audio_group_list_deinit();
	/* deintialize iso data path */
	app_bt_le_audio_iso_data_path_deinit();

	g_gmap_bgs_info.group_handle = NULL;

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_bgr_init(app_bt_le_audio_acceptor_info_t *p_info)
{
	app_bt_le_audio_bap_broadcast_sink_info_t *p_bap_bro_sink_info = p_info->p_bap_bro_sink_info;

	if (p_bap_bro_sink_info->big_info_ind_sem == NULL) {
		osif_sem_create(&p_bap_bro_sink_info->big_info_ind_sem, 0, 0xFFFFFFFF);
	}
	/* intialize iso data path */
	app_bt_le_audio_iso_data_path_init();
	/* intialize bscan and sync dev list */
	app_bt_le_audio_broadcast_dev_list_init();

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_bgr_deinit(app_bt_le_audio_acceptor_info_t *p_info)
{
	app_bt_le_audio_bap_broadcast_sink_info_t *p_bap_bro_sink_info = p_info->p_bap_bro_sink_info;

	/* deintialize iso data path */
	app_bt_le_audio_iso_data_path_deinit();
	/* deintialize bscan and sync dev list */
	app_bt_le_audio_broadcast_dev_list_deinit();
	if (p_bap_bro_sink_info->big_info_ind_sem) {
		osif_sem_delete(p_bap_bro_sink_info->big_info_ind_sem);
		p_bap_bro_sink_info->big_info_ind_sem = NULL;
	}

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_ugg_init(void)
{
	/* intialize iso data path */
	app_bt_le_audio_iso_data_path_init();
	/* intialize group list */
	app_bt_le_audio_group_list_init();
	app_bt_le_audio_scan_dev_list_init();

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_ugg_deinit(void)
{
	/* deintialize group list */
	app_bt_le_audio_scan_dev_list_deinit();
	app_bt_le_audio_group_list_deinit();
	/* deintialize iso data path */
	app_bt_le_audio_iso_data_path_deinit();

	g_gmap_ugg_info.group_handle = NULL;

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_ugt_init(void)
{
	/* intialize iso data path */
	app_bt_le_audio_iso_data_path_init();

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_ugt_deinit(void)
{
	/* deintialize iso data path */
	app_bt_le_audio_iso_data_path_deinit();

	return RTK_BT_OK;
}

#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
static uint16_t app_bt_le_audio_update_csis_sirk(void *conf, uint8_t sound_channel, uint8_t *local_addr, uint8_t *peer_addr)
{
	uint16_t ret = 0;
	rtk_bt_le_audio_app_conf_t *p_app_conf = (rtk_bt_le_audio_app_conf_t *)conf;

	//update g_csis_sirk, first: right bd address; then: left bd address
	if (sound_channel == RTK_BT_LE_AUDIO_LEFT) {
		memcpy(&p_app_conf->cap_param.csis_param.csis_sirk[4], peer_addr, RTK_BD_ADDR_LEN);
		memcpy(&p_app_conf->cap_param.csis_param.csis_sirk[10], local_addr, RTK_BD_ADDR_LEN);
	} else if (sound_channel == RTK_BT_LE_AUDIO_RIGHT) {
		memcpy(&p_app_conf->cap_param.csis_param.csis_sirk[4], local_addr, RTK_BD_ADDR_LEN);
		memcpy(&p_app_conf->cap_param.csis_param.csis_sirk[10], peer_addr, RTK_BD_ADDR_LEN);
	}
	ret = rtk_bt_le_audio_csis_update_sirk(RTK_BT_LE_CSIS_SIRK_PLN, p_app_conf->cap_param.csis_param.csis_sirk);

	return ret;
}
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
static uint16_t app_bt_le_audio_aics_set_param(void)
{
	rtk_bt_le_audio_aics_input_state_t input_state = {0};
	rtk_bt_le_audio_aics_gain_setting_prop_t gain_prop = {0};
	rtk_bt_le_audio_input_type_t input_type = 0;
	uint8_t input_status = 0;

	input_state.gain_mode = RTK_BT_LE_AUDIO_AICS_GAIN_MODE_MANUAL;
	input_state.gain_setting = 20;
	input_state.mute = RTK_BT_LE_AUDIO_AICS_NOT_MUTED;
	BT_APP_PROCESS(rtk_bt_le_audio_aics_set_param(RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX, RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATE,
												  sizeof(rtk_bt_le_audio_aics_input_state_t), (uint8_t *)&input_state, false));
	BT_APP_PROCESS(rtk_bt_le_audio_aics_set_param(RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX, RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATE,
												  sizeof(rtk_bt_le_audio_aics_input_state_t), (uint8_t *)&input_state, false));
	gain_prop.gain_setting_max = 100;
	gain_prop.gain_setting_min = -100;
	gain_prop.gain_setting_units = 50;
	BT_APP_PROCESS(rtk_bt_le_audio_aics_set_param(RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX, RTK_BT_LE_AUDIO_AICS_PARAM_GAIN_SETTING_PROP,
												  sizeof(rtk_bt_le_audio_aics_gain_setting_prop_t), (uint8_t *)&gain_prop, false));
	BT_APP_PROCESS(rtk_bt_le_audio_aics_set_param(RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX, RTK_BT_LE_AUDIO_AICS_PARAM_GAIN_SETTING_PROP,
												  sizeof(rtk_bt_le_audio_aics_gain_setting_prop_t), (uint8_t *)&gain_prop, false));
	input_type = RTK_BT_LE_AUDIO_INPUT_BLUETOOTH;
	BT_APP_PROCESS(rtk_bt_le_audio_aics_set_param(RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX, RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_TYPE,
												  sizeof(uint8_t), (uint8_t *)&input_type, false));
	input_type = RTK_BT_LE_AUDIO_INPUT_MICROPHONE;
	BT_APP_PROCESS(rtk_bt_le_audio_aics_set_param(RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX, RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_TYPE,
												  sizeof(uint8_t), (uint8_t *)&input_type, false));
	input_status = RTK_BT_LE_AUDIO_AICS_INPUT_STATUS_ACTIVE;
	BT_APP_PROCESS(rtk_bt_le_audio_aics_set_param(RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX, RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATUS,
												  sizeof(uint8_t), (uint8_t *)&input_status, false));
	BT_APP_PROCESS(rtk_bt_le_audio_aics_set_param(RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX, RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATUS,
												  sizeof(uint8_t), (uint8_t *)&input_status, false));
	BT_APP_PROCESS(rtk_bt_le_audio_aics_set_param(RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX, RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_DES,
												  strlen(RTK_BT_LE_AUDIO_BLUETOOTH_DES), (uint8_t *)RTK_BT_LE_AUDIO_BLUETOOTH_DES, false));
	BT_APP_PROCESS(rtk_bt_le_audio_aics_set_param(RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX, RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_DES,
												  strlen(RTK_BT_LE_AUDIO_MIC_DES), (uint8_t *)RTK_BT_LE_AUDIO_MIC_DES, false));

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

int bt_gmap_main(uint8_t role, uint8_t enable)
{
	if (1 == enable) {
		if (gmap_demo_init_flag) {
			printf("%s Already init! \r\n", __func__);
			return -1;
		}

		switch (role) {
		/* GMAP broadcast game sender */
		case RTK_BT_LE_AUDIO_GMAP_ROLE_BGS: {
			app_bt_le_audio_initiator_info_t *p_bgs_info = &g_gmap_bgs_info;
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &p_bgs_info->lea_app_conf;
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			app_lea_audio_cap_param_t cap_param_t = {0};
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
			cap_param_t.p_mcs_service_id = &app_lea_mcs_service_id;
#endif
			//set lea_app_conf, must called before rtk_bt_enable
			app_bt_le_audio_lea_app_conf((void *)p_lea_app_conf,
										 0,
										 RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR | RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER,
										 RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SOUR,
										 app_lea_default_pac_source_codec,
										 sizeof(app_lea_default_pac_source_codec),
										 app_lea_default_pac_sink_codec,
										 sizeof(app_lea_default_pac_sink_codec),
										 &cap_param_t);
			gmap_role |= RTK_BT_LE_AUDIO_GMAP_ROLE_BGS;
			p_lea_app_conf->gmap_role = gmap_role;
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTC | RTK_BT_PROFILE_LEAUDIO;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			bt_app_conf.le_audio_app_conf = p_bgs_info->lea_app_conf;
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
			BT_APP_PROCESS(rtk_bt_le_gap_set_scan_param(&app_lea_def_scan_param));
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_COMMON_GP_GAP, app_le_audio_common_gap_app_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_audio_gap_app_callback));
			/* register le audio event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_AUDIO, app_bt_le_audio_callback));
			app_bt_le_audio_bgs_init();
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
			/* when csip member disconnect, start ext scan timer in app_bt_le_audio_bap_unicast_client_common_cb*/
			if (gmap_ext_scan_flag) {
				printf("[LE Audio] Create GMAP BGS ext scan timer \r\n");
				if (false == osif_timer_create(&gmap_ext_scan_timer, "lea_bgs_ext_scan_timer", NULL,
											   RTK_BT_LEA_GMAP_EXT_SCAN_TIMER_INTERVAL, false,
											   gmap_ext_scan_timer_handle)) {
					printf("[LE Audio] Create GMAP BGS ext scan timer fail \r\n");
					gmap_ext_scan_flag = 0;
					return 1;
				} else {
					gmap_ext_scan_time_remaining = RTK_BT_LEA_GMAP_EXT_SCAN_TIME_COUNT;
				}
			}
#endif
			// GMAP broadcast QoS configuration
			bap_broadcast_source_cfg.qos_manual_cfg = true;
			app_bt_le_audio_gmap_broadcast_qos_config(app_qos_level, app_broadcast_qos, &bap_broadcast_source_cfg.prefer_qos);
			// update gmap bgs metadata
			def_le_audio_broadcast_source_group_param.subgroup[0].p_metadata = app_lea_gmap_broadcast_source_metadata;
			def_le_audio_broadcast_source_group_param.subgroup[0].metadata_len = sizeof(app_lea_gmap_broadcast_source_metadata);
			g_gmap_bgs_info.status = RTK_BLE_AUDIO_INITIATOR_ENABLE;
			gmap_demo_init_flag = true;
			break;
		}
		/* gmap broadcast game receiver */
		case RTK_BT_LE_AUDIO_GMAP_ROLE_BGR: {
			app_bt_le_audio_acceptor_info_t *p_bgr_info = &g_gmap_bgr_info;
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &p_bgr_info->lea_app_conf;
			app_bt_le_audio_bap_broadcast_sink_info_t *p_bap_bro_sink_info = p_bgr_info->p_bap_bro_sink_info;
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			app_lea_audio_cap_param_t cap_param_t = {0};
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
			cap_param_t.p_csis_sirk = app_lea_default_csis_sirk;
			cap_param_t.csis_sirk_len = RTK_BT_LE_CSIS_SIRK_LEN;
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
			cap_param_t.aics_vcs_num = RTK_BT_LE_AUDIO_DEFAULT_AICS_VCS_NUM;
			cap_param_t.p_aics_vcs_tbl = app_lea_vcs_id_array;
			cap_param_t.aics_mics_num = RTK_BT_LE_AUDIO_DEFAULT_AICS_MICS_NUM;
			cap_param_t.p_aics_mics_tbl = app_lea_mics_id_array;
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
			cap_param_t.vocs_num = RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM;
			cap_param_t.p_vocs_feature_tbl = app_lea_vocs_features;
#endif
			//set lea_app_conf, must called before rtk_bt_enable
			app_bt_le_audio_lea_app_conf((void *)p_lea_app_conf,
										 p_bgr_info->sound_channel,
										 RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR,
										 RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK,
										 app_lea_default_pac_source_codec,
										 sizeof(app_lea_default_pac_source_codec),
										 app_lea_default_pac_sink_codec,
										 sizeof(app_lea_default_pac_sink_codec),
										 &cap_param_t);
			gmap_role |= RTK_BT_LE_AUDIO_GMAP_ROLE_BGR;
			p_lea_app_conf->gmap_role = gmap_role;
			p_lea_app_conf->pacs_param.sink_available_contexts |= RTK_BT_LE_AUDIO_CONTEXT_GAME;
			p_lea_app_conf->pacs_param.source_available_contexts |= RTK_BT_LE_AUDIO_CONTEXT_GAME;
			/* bap parameters */
			p_bap_bro_sink_info->config_sink_audio_location = p_lea_app_conf->pacs_param.sink_audio_location;
			//set GAP configuration
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTC | RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_LEAUDIO;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			bt_app_conf.le_audio_app_conf = p_bgr_info->lea_app_conf;
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_LOGA("[APP] sink_pac_id = %d,source_pac_id = %d\r\n", (int)(*bt_app_conf.le_audio_app_conf.pacs_param.p_sink_pac_id),
					(int)(*bt_app_conf.le_audio_app_conf.pacs_param.p_source_pac_id));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
			app_bt_le_audio_update_csis_sirk((void *)p_lea_app_conf,
											 p_bgr_info->sound_channel,
											 bd_addr.addr_val,
											 p_bgr_info->csis_neighbor_addr);
#endif
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
			rtk_bt_le_audio_vcs_param_t vcs_param = {
				.volume_setting = RTK_BLE_AUDIO_DEFAULT_VCS_VOL_SETTING,
				.mute = RTK_BT_LE_AUDIO_VCS_NOT_MUTED,
				.change_counter = RTK_BLE_AUDIO_DEFAULT_VCS_CHG_CNT,
				.volume_flags = RTK_BLE_AUDIO_DEFAULT_VCS_VOL_FLAG,
				.step_size = RTK_BLE_AUDIO_DEFAULT_VCS_STEP_SIZE
			};
			rtk_bt_le_audio_vcs_set_param(&vcs_param);
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
			app_bt_le_audio_aics_set_param();
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
			app_bt_le_audio_vocs_set_param(p_bgr_info->sound_channel);
#endif
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_audio_gap_app_callback));
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(p_bgr_info->device_name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_GENERIC_MEDIA_PLAYER));
			uint8_t app_lea_adv_data_len = LE_AUDIO_GMAP_EXT_ADV_DATA_DEFAULT_LEN;
			app_bt_le_audio_update_adv((void *)p_lea_app_conf, p_bgr_info->device_name, app_lea_adv_data, &app_lea_adv_data_len);
			/* 1. create ext adv */
			BT_APP_PROCESS(rtk_bt_le_gap_create_ext_adv(&app_lea_def_ext_adv_param, &app_lea_def_ext_adv_handle));
			BT_LOGA("[APP] def_ext_adv_param.adv_handle = 0x%x\r\n", app_lea_def_ext_adv_handle);
			/* 2. set ext adv data */
			BT_APP_PROCESS(rtk_bt_le_gap_set_ext_adv_data(app_lea_def_ext_adv_handle, (uint8_t *)app_lea_adv_data, app_lea_adv_data_len));
			/* 3. start ext adv */
			BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(app_lea_def_ext_adv_handle, 0, 0));
			/* register le audio event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_AUDIO, app_bt_le_audio_callback));
			/* intialize iso data path */
			app_bt_le_audio_bgr_init(&g_gmap_bgr_info);
			p_bgr_info->status = RTK_BLE_AUDIO_BROADCAST_SINK_ENABLE;
			gmap_demo_init_flag = true;
			break;
		}
		/* gmap unicast game gateway */
		case RTK_BT_LE_AUDIO_GMAP_ROLE_UGG: {
			app_bt_le_audio_initiator_info_t *p_ugg_info = &g_gmap_ugg_info;
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &p_ugg_info->lea_app_conf;
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			app_lea_audio_cap_param_t cap_param_t = {0};
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
			cap_param_t.p_mcs_service_id = &app_lea_mcs_service_id;
#endif
			//set lea_app_conf, must called before rtk_bt_enable
			app_bt_le_audio_lea_app_conf((void *)p_lea_app_conf,
										 0,
										 RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR | RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER,
										 RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI,
										 app_lea_default_pac_source_codec,
										 sizeof(app_lea_default_pac_source_codec),
										 app_lea_default_pac_sink_codec,
										 sizeof(app_lea_default_pac_sink_codec),
										 &cap_param_t);
			gmap_role |= RTK_BT_LE_AUDIO_GMAP_ROLE_UGG;
			p_lea_app_conf->gmap_role = gmap_role;
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTC | RTK_BT_PROFILE_LEAUDIO;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			bt_app_conf.le_audio_app_conf = p_ugg_info->lea_app_conf;
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
			BT_APP_PROCESS(rtk_bt_le_gap_set_scan_param(&app_lea_def_scan_param));
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_COMMON_GP_GAP, app_le_audio_common_gap_app_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_audio_gap_app_callback));
			/* Initilize GATT (client) part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTC, app_le_audio_ugg_gattc_app_callback));
			/* register le audio event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_AUDIO, app_bt_le_audio_callback));
			app_bt_le_audio_ugg_init();
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
			/* when csip member disconnect, start ext scan timer in app_bt_le_audio_bap_unicast_client_common_cb*/
			if (gmap_ext_scan_flag) {
				printf("[LE Audio] Create GMAP UGG ext scan timer \r\n");
				if (false == osif_timer_create(&gmap_ext_scan_timer, "lea_ugg_ext_scan_timer", NULL,
											   RTK_BT_LEA_GMAP_EXT_SCAN_TIMER_INTERVAL, false,
											   gmap_ext_scan_timer_handle)) {
					printf("[LE Audio] Create GMAP UGG ext scan timer fail \r\n");
					gmap_ext_scan_flag = 0;
					return 1;
				} else {
					gmap_ext_scan_time_remaining = RTK_BT_LEA_GMAP_EXT_SCAN_TIME_COUNT;
				}
			}
#endif
			g_gmap_ugg_info.status = RTK_BLE_AUDIO_INITIATOR_ENABLE;
			gmap_demo_init_flag = true;
			break;
		}
		/* gmap unicast game terminal */
		case RTK_BT_LE_AUDIO_GMAP_ROLE_UGT: {
			app_bt_le_audio_acceptor_info_t *p_ugt_info = &g_gmap_ugt_info;
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &p_ugt_info->lea_app_conf;
			app_bt_le_audio_bap_unicast_server_info_t *p_bap_uni_ser_info = p_ugt_info->p_bap_uni_ser_info;
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			app_lea_audio_cap_param_t cap_param_t = {0};
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
			cap_param_t.p_csis_sirk = app_lea_default_csis_sirk;
			cap_param_t.csis_sirk_len = RTK_BT_LE_CSIS_SIRK_LEN;
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
			cap_param_t.aics_vcs_num = RTK_BT_LE_AUDIO_DEFAULT_AICS_VCS_NUM;
			cap_param_t.p_aics_vcs_tbl = app_lea_vcs_id_array;
			cap_param_t.aics_mics_num = RTK_BT_LE_AUDIO_DEFAULT_AICS_MICS_NUM;
			cap_param_t.p_aics_mics_tbl = app_lea_mics_id_array;
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
			cap_param_t.vocs_num = RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM;
			cap_param_t.p_vocs_feature_tbl = app_lea_vocs_features;
#endif
			//set lea_app_conf, must called before rtk_bt_enable
			app_bt_le_audio_lea_app_conf((void *)p_lea_app_conf,
										 p_ugt_info->sound_channel,
										 RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR,
										 RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER,
										 app_lea_default_pac_source_codec,
										 sizeof(app_lea_default_pac_source_codec),
										 app_lea_default_pac_sink_codec,
										 sizeof(app_lea_default_pac_sink_codec),
										 &cap_param_t);
			gmap_role |= RTK_BT_LE_AUDIO_GMAP_ROLE_UGT;
			p_lea_app_conf->gmap_role = gmap_role;
			p_lea_app_conf->pacs_param.sink_available_contexts |= RTK_BT_LE_AUDIO_CONTEXT_GAME;
			p_lea_app_conf->pacs_param.source_available_contexts |= RTK_BT_LE_AUDIO_CONTEXT_GAME;
			/* bap parameters */
			p_bap_uni_ser_info->config_sink_ase_num = p_lea_app_conf->ascs_param.sink_ase_num;
			p_bap_uni_ser_info->config_source_ase_num = p_lea_app_conf->ascs_param.source_ase_num;
			BT_LOGA("[APP] config_sink_ase_num = %d,config_source_ase_num = %d\r\n", p_bap_uni_ser_info->config_sink_ase_num,
					p_bap_uni_ser_info->config_source_ase_num);
			//set GAP configuration
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTC | RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_LEAUDIO;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			bt_app_conf.le_audio_app_conf = p_ugt_info->lea_app_conf;
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_LOGA("[APP] sink_pac_id = %d,source_pac_id = %d\r\n", (int)(*bt_app_conf.le_audio_app_conf.pacs_param.p_sink_pac_id),
					(int)(*bt_app_conf.le_audio_app_conf.pacs_param.p_source_pac_id));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
			app_bt_le_audio_update_csis_sirk((void *)p_lea_app_conf,
											 p_ugt_info->sound_channel,
											 bd_addr.addr_val,
											 p_ugt_info->csis_neighbor_addr);
#endif
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
			rtk_bt_le_audio_vcs_param_t vcs_param = {
				.volume_setting = RTK_BLE_AUDIO_DEFAULT_VCS_VOL_SETTING,
				.mute = RTK_BT_LE_AUDIO_VCS_NOT_MUTED,
				.change_counter = RTK_BLE_AUDIO_DEFAULT_VCS_CHG_CNT,
				.volume_flags = RTK_BLE_AUDIO_DEFAULT_VCS_VOL_FLAG,
				.step_size = RTK_BLE_AUDIO_DEFAULT_VCS_STEP_SIZE
			};
			rtk_bt_le_audio_vcs_set_param(&vcs_param);
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
			rtk_bt_le_audio_mics_param_t mics_param = {
				.mic_mute = RTK_BT_LE_AUDIO_MICS_NOT_MUTE
			};
			rtk_bt_le_audio_mics_set_param(&mics_param);
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
			app_bt_le_audio_aics_set_param();
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
			app_bt_le_audio_vocs_set_param(p_ugt_info->sound_channel);
#endif
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_audio_gap_app_callback));
			/* Initilize GATTS part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTS, app_bt_le_audio_ugt_gatts_callback));
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(p_ugt_info->device_name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_GENERIC_MEDIA_PLAYER));
			uint8_t app_lea_adv_data_len = LE_AUDIO_GMAP_EXT_ADV_DATA_DEFAULT_LEN;
			app_bt_le_audio_update_adv((void *) p_lea_app_conf, p_ugt_info->device_name, app_lea_adv_data, &app_lea_adv_data_len);
			/* 1. create ext adv */
			BT_APP_PROCESS(rtk_bt_le_gap_create_ext_adv(&app_lea_def_ext_adv_param, &app_lea_def_ext_adv_handle));
			BT_LOGA("[APP] def_ext_adv_param.adv_handle = 0x%x\r\n", app_lea_def_ext_adv_handle);
			/* 2. set ext adv data */
			BT_APP_PROCESS(rtk_bt_le_gap_set_ext_adv_data(app_lea_def_ext_adv_handle, (uint8_t *)app_lea_adv_data, app_lea_adv_data_len));
			/* 3. start ext adv */
			BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(app_lea_def_ext_adv_handle, 0, 0));
			/* register le audio event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_AUDIO, app_bt_le_audio_callback));
			/* intialize iso data path */
			app_bt_le_audio_ugt_init();
			p_ugt_info->status = RTK_BLE_AUDIO_ACCEPTOR_ENABLE;
			gmap_demo_init_flag = true;
			break;
		}
		default:
			BT_LOGD("[APP] unknown role 0x%x\r\n", role);
			break;
		}
	} else if (0 == enable) {
		if (!gmap_demo_init_flag) {
			printf("%s No need deinit! \r\n", __func__);
			return -1;
		}
		switch (role) {
		/* gmap broadcast game sender */
		case RTK_BT_LE_AUDIO_GMAP_ROLE_BGS: {
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
			/* Deinit ext scan timer*/
			if (gmap_ext_scan_flag) {
				osif_timer_stop(&gmap_ext_scan_timer);
				osif_timer_delete(&gmap_ext_scan_timer);
				gmap_ext_scan_time_remaining = RTK_BT_LEA_GMAP_EXT_SCAN_TIME_COUNT;
			}
#endif
			if (g_gmap_bgs_info.status == RTK_BLE_AUDIO_INITIATOR_DISABLE) {
				BT_LOGE("[APP] %s: already disabled\r\n", __func__);
				return -1;
			}
			if (g_gmap_bgs_info.status == RTK_BLE_AUDIO_INITIATOR_BROADCAST_START) {
				BT_LOGE("[APP] %s: please stop first before disable \r\n", __func__);
				return -1;
			}
			app_bt_le_audio_bgs_deinit();
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			def_le_audio_broadcast_source_group_param.subgroup[0].p_metadata = app_lea_ori_broadcast_source_metadata;
			g_gmap_bgs_info.status = RTK_BLE_AUDIO_INITIATOR_DISABLE;
			gmap_role = RTK_BT_LE_AUDIO_GMAP_ROLE_UNKNOWN;
			gmap_demo_init_flag = false;
			break;
		}
		/* gmap broadcast game receiver */
		case RTK_BT_LE_AUDIO_GMAP_ROLE_BGR: {
			if (g_gmap_bgr_info.status == RTK_BLE_AUDIO_ACCEPTOR_DISABLE) {
				BT_LOGE("[APP] %s: already disabled \r\n", __func__);
				return -1;
			}
			/* 1. stop and remove ea */
			rtk_bt_le_gap_stop_ext_adv(app_lea_def_ext_adv_handle);
			app_bt_le_audio_bgr_deinit(&g_gmap_bgr_info);
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			g_gmap_bgr_info.status = RTK_BLE_AUDIO_ACCEPTOR_DISABLE;
			gmap_role = RTK_BT_LE_AUDIO_GMAP_ROLE_UNKNOWN;
			gmap_demo_init_flag = false;
			break;
		}
		/* gmap unicast game gateway */
		case RTK_BT_LE_AUDIO_GMAP_ROLE_UGG: {
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
			/* Deinit ext scan timer*/
			if (gmap_ext_scan_flag) {
				osif_timer_stop(&gmap_ext_scan_timer);
				osif_timer_delete(&gmap_ext_scan_timer);
				gmap_ext_scan_time_remaining = RTK_BT_LEA_GMAP_EXT_SCAN_TIME_COUNT;
			}
#endif
			if (g_gmap_ugg_info.status == RTK_BLE_AUDIO_INITIATOR_DISABLE) {
				BT_LOGE("[APP] %s: already disabled\r\n", __func__);
				return -1;
			}
			if (g_gmap_ugg_info.status == RTK_BLE_AUDIO_INITIATOR_UNICAST_START) {
				BT_LOGE("[APP] %s: please stop first before disable \r\n", __func__);
				return -1;
			}
			app_bt_le_audio_ugg_deinit();
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			g_gmap_ugg_info.status = RTK_BLE_AUDIO_INITIATOR_DISABLE;
			gmap_role = RTK_BT_LE_AUDIO_GMAP_ROLE_UNKNOWN;
			gmap_demo_init_flag = false;
			break;
		}
		/* gmap unicast game terminal */
		case RTK_BT_LE_AUDIO_GMAP_ROLE_UGT: {
			if (g_gmap_ugt_info.status == RTK_BLE_AUDIO_ACCEPTOR_DISABLE) {
				BT_LOGE("[APP] %s: already disabled \r\n", __func__);
				return -1;
			}
			if (g_gmap_ugt_info.status == RTK_BLE_AUDIO_ACCEPTOR_START) {
				BT_LOGE("[APP] %s: please stop first before disable \r\n", __func__);
				return -1;
			}
			/* 1. stop and remove ea */
			rtk_bt_le_gap_stop_ext_adv(app_lea_def_ext_adv_handle);
			app_bt_le_audio_ugt_deinit();
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			g_gmap_ugt_info.status = RTK_BLE_AUDIO_ACCEPTOR_DISABLE;
			gmap_role = RTK_BT_LE_AUDIO_GMAP_ROLE_UNKNOWN;
			gmap_demo_init_flag = false;
			break;
		}
		default:
			BT_LOGD("[APP] unknown role 0x%x\r\n", role);
			break;
		}
	}
	return 0;
}
#endif