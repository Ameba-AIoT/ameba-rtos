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
#include <rtk_bt_def.h>
#include <bt_audio_intf.h>
#include <bt_audio_codec_wrapper.h>
#include <bt_audio_track_api.h>
#include <bt_audio_record_api.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_le_audio.h>
#include <rtk_bt_common.h>
#include <lc3_codec_entity.h>
#include "app_bt_le_audio_common.h"
#include <bt_utils.h>

#if defined(CONFIG_BT_APP_DEBUG) && CONFIG_BT_APP_DEBUG
void *g_app_printf_mtx;
#endif

/* common broadcast source information parameter */
app_bt_le_audio_bap_broadcast_source_info_t app_bap_bro_sour_info = {
	.group_idx = 0xFF,
	.subgroup_idx = 0xFF,
	.bis_info = {0},
	.bis_codec_cfg = {
		.type_exist = RTK_BT_LE_CODEC_CFG_SAMPLING_FREQUENCY_EXIST | RTK_BT_LE_CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST |
		RTK_BT_LE_CODEC_CFG_OCTET_PER_CODEC_FRAME_EXIST | RTK_BT_LE_CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST |
		RTK_BT_LE_CODEC_CFG_FRAME_DURATION_EXIST,
		.frame_duration = 0,
		.sample_frequency = 0,
		.codec_frame_blocks_per_sdu = 0,
		.octets_per_codec_frame = 0,
		.audio_channel_allocation = 0
	},
	.broadcast_source_handle = NULL,
	.broadcast_source_state = 0
};
/* common broadcast sink information parameter */
app_bt_le_audio_bap_broadcast_sink_info_t app_bap_bro_sink_info = {0};
/* common unicast client information parameter */
app_bt_le_audio_bap_unicast_client_info_t app_bap_uni_client_info = {0};
/* common unicast server information parameter */
app_bt_le_audio_bap_unicast_server_info_t app_bap_uni_server_info = {0};
/* common broadcast source information parameter */
app_bt_le_audio_bap_broadcast_assistant_info_t app_bap_bro_assi_info = {0};
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
/* common mcp server info parameter */
app_bt_le_audio_mcp_server_info_t app_mcp_svr_info = {0};
#endif

rtk_bt_le_ext_scan_param_t app_lea_def_ext_scan_param = {
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

/* common broadcast source parameters */
uint8_t bap_broadcast_source_codec_level2[] = {
	0x02,
	RTK_BT_LE_CODEC_CFG_TYPE_SAMPLING_FREQUENCY,
#if defined(LEA_SOURCE_FIX_SAMPLE_FREQUENCY)
	LEA_SOURCE_FIX_SAMPLE_FREQUENCY,
#else
	RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K,
#endif
	0x02,
	RTK_BT_LE_CODEC_CFG_TYPE_FRAME_DURATION,
#if defined(LEA_SOURCE_FIX_FRAME_DUARTION)
	LEA_SOURCE_FIX_FRAME_DUARTION,
#else
	RTK_BT_LE_FRAME_DURATION_CFG_10_MS,
#endif
	0x03,
	RTK_BT_LE_CODEC_CFG_TYPE_OCTET_PER_CODEC_FRAME,
	//select OCTET_PER_CODEC_FRAME from app_lea_audio_support_capablity_table according LEA_SOURCE_FIX_FRAME_DUARTION and LEA_SOURCE_FIX_SAMPLE_FREQUENCY
#if defined(LEA_SOURCE_FIX_FRAME_DUARTION) || defined(LEA_SOURCE_FIX_SAMPLE_FREQUENCY)
	0x00, 0x00,
#else
	0x28, 0x00,
#endif
	0x02,
	RTK_BT_LE_CODEC_CFG_TYPE_BLOCKS_PER_SDU,
	1
};

uint8_t bap_broadcast_source_codec_level2_len = sizeof(bap_broadcast_source_codec_level2);

#if (RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM == 1) && (RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE == 1)
static uint8_t app_lea_broadcast_source_codec_level3_bis_left[] = {
	0x05,
	RTK_BT_LE_CODEC_CFG_TYPE_AUDIO_CHANNEL_ALLOCATION,
	(uint8_t)(RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR),
	(uint8_t)(((RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR) >> 8) & 0xFF),
	(uint8_t)(((RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR) >> 16) & 0xFF),
	(uint8_t)(((RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR) >> 24) & 0xFF),
};
#else
static uint8_t app_lea_broadcast_source_codec_level3_bis_left[] = {
	0x05,
	RTK_BT_LE_CODEC_CFG_TYPE_AUDIO_CHANNEL_ALLOCATION,
	(uint8_t)RTK_BT_LE_AUDIO_LOCATION_FL,
	(uint8_t)((RTK_BT_LE_AUDIO_LOCATION_FL >> 8) & 0xFF),
	(uint8_t)((RTK_BT_LE_AUDIO_LOCATION_FL >> 16) & 0xFF),
	(uint8_t)((RTK_BT_LE_AUDIO_LOCATION_FL >> 24) & 0xFF),
};
#endif

#if (RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM == 2)
static uint8_t app_lea_broadcast_source_codec_level3_bis_right[] = {
	0x05,
	RTK_BT_LE_CODEC_CFG_TYPE_AUDIO_CHANNEL_ALLOCATION,
	(uint8_t)RTK_BT_LE_AUDIO_LOCATION_FR,
	(uint8_t)((RTK_BT_LE_AUDIO_LOCATION_FR >> 8) & 0xFF),
	(uint8_t)((RTK_BT_LE_AUDIO_LOCATION_FR >> 16) & 0xFF),
	(uint8_t)((RTK_BT_LE_AUDIO_LOCATION_FR >> 24) & 0xFF),
};
#endif
static uint8_t app_lea_broadcast_source_metadata[] = {
	0x03,
	RTK_BT_LE_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS,
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_MEDIA),
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_MEDIA >> 8),
	0x02,
	RTK_BT_LE_METADATA_TYPE_CCCD_LIST,
	0,
};

rtk_bt_le_audio_broadcast_source_group_param_t def_le_audio_broadcast_source_group_param = {
	.presentation_delay = RTK_BLE_AUDIO_DEFAULT_BROADCASTER_PRESENTATION_DELAY,
	.num_subgroups = RTK_BLE_AUDIO_DEFAULT_BROADCASTER_SUBGROUP_NUM,
	.subgroup = {
		{
			.num_bis = RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM,
			.bis = {
				{
					//left
					.codec_cfg_l3_len = sizeof(app_lea_broadcast_source_codec_level3_bis_left),
					.p_codec_cfg_l3 = app_lea_broadcast_source_codec_level3_bis_left,
					.bis_idx = NULL
				},
#if RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM == 2
				{
					//right
					.codec_cfg_l3_len = sizeof(app_lea_broadcast_source_codec_level3_bis_right),
					.p_codec_cfg_l3 = app_lea_broadcast_source_codec_level3_bis_right,
					.bis_idx = NULL
				},
#else
				{0},
#endif
				{0}, {0}
			},
			.codec_id = {RTK_BT_LE_LC3_CODEC_ID, 0, 0, 0, 0},
			.codec_cfg_l2_len = sizeof(bap_broadcast_source_codec_level2),
			.p_codec_cfg_l2 = bap_broadcast_source_codec_level2,
			.metadata_len = sizeof(app_lea_broadcast_source_metadata),
			.p_metadata = app_lea_broadcast_source_metadata,
			.subgroup_idx = NULL
		}, {0}
	},
	.group_idx = NULL
};

rtk_bt_le_audio_broadcast_source_config_t def_le_audio_broadcast_source_config_param = {
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

uint8_t app_lea_def_broadcast_audio_announcements[RTK_LE_AUDIO_BROADCAST_AUDIO_ANNOUNCEMENT_LEN_MAX] = {
	0x06,
	RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA,
	(uint8_t)(RTK_BT_LE_BROADCAST_AUDIO_ANNOUNCEMENT_SRV_UUID),
	(uint8_t)(RTK_BT_LE_BROADCAST_AUDIO_ANNOUNCEMENT_SRV_UUID >> 8),
	0x00,// broadcast id, len=3
	0x00,
	0x00
};

uint8_t app_lea_def_broadcast_audio_announcements_len = RTK_LE_AUDIO_BROADCAST_AUDIO_ANNOUNCEMENT_DEFAULT_LEN;

uint8_t app_lea_def_broadcast_code[] = {0x82, 0x31, 0x5C, 0x28, 0xF6, 0x1B, 0x19, 0x58, 0x2D, 0x5E, 0x20, 0xD1, 0x23, 0x33, 0xD0, 0x8E};

app_bt_bap_broadcast_source_param_cfg_t bap_broadcast_source_cfg = {
	.codec_cfg = {0},
	.prefer_qos = {0},
	.qos_manual_cfg = false,
	.cfg_bis_num = RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM,
#if (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K)
#if (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS)
	.cfg_codec_index = RTK_BT_LE_CODEC_CFG_ITEM_16_1,
#else
	.cfg_codec_index = RTK_BT_LE_CODEC_CFG_ITEM_16_2,
#endif
#elif (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_24K)
#if (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS)
	.cfg_codec_index = RTK_BT_LE_CODEC_CFG_ITEM_24_1,
#else
	.cfg_codec_index = RTK_BT_LE_CODEC_CFG_ITEM_24_2,
#endif
#elif (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_32K)
#if (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS)
	.cfg_codec_index = RTK_BT_LE_CODEC_CFG_ITEM_32_1,
#else
	.cfg_codec_index = RTK_BT_LE_CODEC_CFG_ITEM_32_2,
#endif
#elif (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K)
#if (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS)
	.cfg_codec_index = RTK_BT_LE_CODEC_CFG_ITEM_48_1,
#else
	.cfg_codec_index = RTK_BT_LE_CODEC_CFG_ITEM_48_2,
#endif
#else
	.cfg_codec_index = RTK_BT_LE_CODEC_CFG_ITEM_16_2,
#endif
#if (LEA_BIG_ISO_INTERVAL_CONFIG == ISO_INTERVAL_20_MS) || (LEA_BIG_ISO_INTERVAL_CONFIG == ISO_INTERVAL_30_MS)
	.cfg_qos_type = RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY,
#else
	.cfg_qos_type = RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY,
#endif
	.cfg_encryption = false
};

rtk_bt_le_audio_unicast_ase_qos_t bap_def_ase_qos = {
	.phy = RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M,
#if RTK_BLE_AUDIO_UNICAST_ONE_CIS_SETEO_MODE
#if (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K)
#if (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS)
	.retransmission_number = 2,
	.max_sdu = 60,
#else
	.retransmission_number = 2,
	.max_sdu = 80,
#endif
#elif (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_24K)
#if (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS)
	.retransmission_number = 2,
	.max_sdu = 90,
#else
	.retransmission_number = 2,
	.max_sdu = 120,
#endif
#elif (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_32K)
#if (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS)
	.retransmission_number = 2,
	.max_sdu = 120,
#else
	.retransmission_number = 2,
	.max_sdu = 160,
#endif
#elif (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K)
#if (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS)
	.retransmission_number = 5,
	.max_sdu = 150,
#else
	.retransmission_number = 5,
	.max_sdu = 200,
#endif
#else
	.retransmission_number = 5,
	.max_sdu = 200,
#endif
#else
#if (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K)
#if (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS)
	.retransmission_number = 2,
	.max_sdu = 30,
#else
	.retransmission_number = 2,
	.max_sdu = 40,
#endif
#elif (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_24K)
#if (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS)
	.retransmission_number = 2,
	.max_sdu = 45,
#else
	.retransmission_number = 2,
	.max_sdu = 60,
#endif
#elif (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_32K)
#if (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS)
	.retransmission_number = 2,
	.max_sdu = 60,
#else
	.retransmission_number = 2,
	.max_sdu = 80,
#endif
#elif (LEA_SOURCE_FIX_SAMPLE_FREQUENCY == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K)
#if (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS)
	.retransmission_number = 5,
	.max_sdu = 75,
#else
	.retransmission_number = 5,
	.max_sdu = 100,
#endif
#else
	.retransmission_number = 5,
	.max_sdu = 100,
#endif
#endif
};

app_bt_bap_unicast_client_param_cfg_t bap_unicast_client_cfg = {
	.codec_cfg = {0},
	.prefer_qos = {0},
	.cfg_codec_index = RTK_BT_LE_CODEC_CFG_ITEM_LC3_MAX,
#if LEA_CIG_ISO_INTERVAL_CONFIG == ISO_INTERVAL_20_MS
	.cfg_qos_type = RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY,
#elif LEA_CIG_ISO_INTERVAL_CONFIG == ISO_INTERVAL_10_MS
	.cfg_qos_type = RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY
#else
#error "now only support LEA_CIG_ISO_INTERVAL_CONFIG 10ms and 20ms"
#endif
};

/* common unicast client parameters */
rtk_bt_le_audio_unicast_session_qos_t app_lea_default_unicast_session_qos = {
	.sca = 0,
	.packing = 0,
	.framing = 0,
	.sdu_interval_m_s = 0,
	.sdu_interval_s_m = 0,
	.latency_m_s = 0,//max transport latency
	.latency_s_m = 0,
	.sink_presentation_delay = 0,
	.source_presentation_delay = 0
};

rtk_bt_le_audio_unicast_ase_codec_t app_lea_default_prefer_codec = {
	.codec_id = {RTK_BT_LE_LC3_CODEC_ID, 0, 0, 0, 0},
	.target_latency = RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY,//will affect rtn
	.target_phy = RTK_BT_LE_PHYS_2M,
	.codec_cfg = {0}
};

#if defined(LEA_SOURCE_ASE_PREFER_CODEC_CONFIG) && LEA_SOURCE_ASE_PREFER_CODEC_CONFIG
rtk_bt_le_audio_unicast_ase_codec_t app_lea_source_ase_prefer_codec = {
	.codec_id = {RTK_BT_LE_LC3_CODEC_ID, 0, 0, 0, 0},
	.target_latency = RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY,//will affect rtn
	.target_phy = RTK_BT_LE_PHYS_2M,
	.codec_cfg = {0}
};
#endif

rtk_bt_le_audio_broadcast_source_create_big_t def_le_audio_broadcast_source_create_big_param = {
	.num_bis = RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM,                                                   /*< Total number of BISes in the BIG. *//**<Set it in main. */
#if defined(LEA_SOURCE_FIX_FRAME_DUARTION)
	.sdu_interval = (LEA_SOURCE_FIX_FRAME_DUARTION == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500,           /*< The interval, in microseconds, of periodic SDUs.
                                                                                                                Range: 0x0000FF to 0x0FFFFF */
#else
	.sdu_interval = RTK_BLE_AUDIO_DEFAULT_SDU_INTERVAL_M_S_US,                                              /*< The interval, in microseconds, of periodic SDUs.
                                                                                                                Range: 0x0000FF to 0x0FFFFF */
#endif
	.max_sdu = RTK_BLE_AUDIO_MAX_SDU_M_S,                                                                   /*< Maximum size of an SDU, in octets. */
	.max_transport_latency = RTK_BLE_AUDIO_DEFAULT_LATENCY_M_S,                                             /*< Maximum transport latency, in milliseconds.Range: 0x0005 to 0x0FA0 */
	.rtn = RTK_BLE_AUDIO_DEFAULT_RTN_M_S,                                                                   /*< The number of times that every BIS Data PDU should be retransmitted.Range: 0x00 to 0x1E */
	.phy = RTK_BLE_AUDIO_DEFAULT_PHY_2M,                                                                    /*< A bit field that indicates the PHY used for transmission of PDUs of BISes in the BIG.
                                                                                                                bit 0: The transmitter PHY of packets is LE 1M.
                                                                                                                bit 1: The transmitter PHY of packets is LE 2M.
                                                                                                                bit 2: The transmitter PHY of packets is LE Coded. */
	.packing = 1,                                                                                           /*< Indicate the preferred method of arranging subevents of multiple BISes.
                                                                                                                0x00: Sequential
                                                                                                                0x01: Interleaved */
	.framing = 0,                                                                                           /*< Indicate the format for sending BIS Data PDUs.
                                                                                                                0x00: Unframed
                                                                                                                0x01: Framed */
	.encryption = 0,                                                                                        /*< Identify the encryption mode of the BISes.
                                                                                                                0x00: Unencrypted
                                                                                                                0x01: Encrypted */
	.broadcast_code = {0}                                                                                       //{0x82, 0x31, 0x5C, 0x28, 0xF6, 0x1B, 0x19, 0x58, 0x2D, 0x5E, 0x20, 0xD1, 0x23, 0x33, 0xD0, 0x8E}
	/* Default value of TSPX_broadcast_code in PTS 8.0.0 is 8ED03323D1205E2D58191BF6285C3182 */
};

app_lea_iso_data_path_list_t g_app_lea_iso_path_list = {0};
static struct list_head g_app_lea_group_list_head = {0};
static void *g_app_lea_group_list_mtx = NULL;
static struct list_head g_app_lea_device_list_head = {0};
static void *g_app_lea_device_list_mtx = NULL;
static struct list_head g_app_lea_scan_dev_list_head = {0};
static void *g_app_lea_scan_dev_list_mtx = NULL;
static struct list_head g_app_lea_bass_scan_dev_list_head = {0};
static void *g_app_lea_bass_scan_dev_list_mtx = NULL;
static struct list_head g_app_lea_sync_dev_list_head = {0};
static void *g_app_lea_sync_dev_list_mtx = NULL;

const app_lea_audio_support_capablity app_lea_audio_support_capablity_table[] = {
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_8K,  RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS,   1, 26},//CODEC_CFG_ITEM_8_1
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_8K,  RTK_BT_LE_FRAME_DURATION_CFG_10_MS,    1, 30},//CODEC_CFG_ITEM_8_2
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K, RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS,   1, 30},//CODEC_CFG_ITEM_16_1
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K, RTK_BT_LE_FRAME_DURATION_CFG_10_MS,    1, 40},//CODEC_CFG_ITEM_16_2
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_24K, RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS,   1, 45},//CODEC_CFG_ITEM_24_1
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_24K, RTK_BT_LE_FRAME_DURATION_CFG_10_MS,    1, 60},//CODEC_CFG_ITEM_24_2
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_32K, RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS,   1, 60},//CODEC_CFG_ITEM_32_1
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_32K, RTK_BT_LE_FRAME_DURATION_CFG_10_MS,    1, 80},//CODEC_CFG_ITEM_32_2
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_44_1K, RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS,  1, 97}, //CODEC_CFG_ITEM_441_1
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_44_1K, RTK_BT_LE_FRAME_DURATION_CFG_10_MS,   1, 130}, //CODEC_CFG_ITEM_441_2
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K, RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS,   1, 75},//CODEC_CFG_ITEM_48_1
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K, RTK_BT_LE_FRAME_DURATION_CFG_10_MS,    1, 100},//CODEC_CFG_ITEM_48_2
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K, RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS,   1, 90},//CODEC_CFG_ITEM_48_3
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K, RTK_BT_LE_FRAME_DURATION_CFG_10_MS,    1, 120},//CODEC_CFG_ITEM_48_4
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K, RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS,   1, 117},//CODEC_CFG_ITEM_48_5
	{RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K, RTK_BT_LE_FRAME_DURATION_CFG_10_MS,    1, 155},//CODEC_CFG_ITEM_48_6
};

//used to save return pac_id
static uint8_t app_lea_sink_pac_id = 0;
static uint8_t app_lea_source_pac_id = 0;
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
extern void *cap_ini_ext_scan_timer;
extern void *cap_comm_ext_scan_timer;
extern int16_t cap_ini_ext_scan_time_remaining;
extern int16_t cap_comm_ext_scan_time_remaining;
bool csip_discover_flag = false;
#endif
//codec audio channel has only left and right channels, but LE audio channel has 28 combinations.
//Therefore, B is replaced by A
uint32_t app_bt_le_audio_translate_le_chnl_to_audio_chnl(uint32_t audio_channel_allocation)
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

uint8_t app_bt_le_audio_get_lea_chnl_num(uint32_t audio_channel_allocation)
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

uint32_t app_bt_le_audio_get_channel_location(uint32_t pacs_audio_loc, uint8_t channel_num, uint32_t used_loc)
{
	uint32_t audio_loc = 0;
	uint16_t idx = 0;
	uint8_t num = 0;
	pacs_audio_loc &= ~used_loc;

	while (pacs_audio_loc) {
		if (pacs_audio_loc & 0x01) {
			audio_loc |= (1 << idx);
			num++;
		}
		pacs_audio_loc >>= 1;
		idx++;
		if (num >= channel_num) {
			break;
		}
	}
	if (num != channel_num) {
		BT_LOGE("[APP] %s: get failed, channel_num %d, used_loc 0x%x", __func__, channel_num, (unsigned int)used_loc);
		return 0;
	}
	return audio_loc;
}

uint32_t app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(uint8_t sample_frequency)
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

uint16_t app_bt_le_audio_lea_app_conf(void *conf, uint8_t sound_channel, uint8_t le_audio_cap_role, uint8_t le_audio_bap_role,
									  uint8_t *pac_source_codec, uint8_t pac_source_codec_len, uint8_t *pac_sink_codec, uint8_t pac_sink_codec_len, app_lea_audio_cap_param_t *p_cap_param)
{
	rtk_bt_le_audio_app_conf_t *p_app_conf = (rtk_bt_le_audio_app_conf_t *)conf;

	p_app_conf->cap_param.cap_role = (rtk_bt_le_audio_cap_role_t)le_audio_cap_role;
	p_app_conf->bap_role = (rtk_bt_le_audio_bap_role_t)le_audio_bap_role;

	if ((p_app_conf->bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER) || (p_app_conf->bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK)) {
		p_app_conf->pacs_param.sink_available_contexts = RTK_BT_LE_AUDIO_CONTEXT_UNSPECIFIED | RTK_BT_LE_AUDIO_CONTEXT_MEDIA | RTK_BT_LE_AUDIO_CONTEXT_CONVERSATIONAL;
		p_app_conf->pacs_param.source_available_contexts = RTK_BT_LE_AUDIO_CONTEXT_MEDIA | RTK_BT_LE_AUDIO_CONTEXT_CONVERSATIONAL;
		p_app_conf->pacs_param.p_sink_pac_id = &app_lea_sink_pac_id;
		p_app_conf->pacs_param.pac_sink_codec_len = pac_sink_codec_len;
		p_app_conf->pacs_param.p_pac_sink_codec = pac_sink_codec;
		p_app_conf->pacs_param.p_source_pac_id = &app_lea_source_pac_id;
		p_app_conf->pacs_param.pac_source_codec_len = pac_source_codec_len;
		p_app_conf->pacs_param.p_pac_source_codec = pac_source_codec;
		switch (sound_channel) {
		case RTK_BT_LE_AUDIO_LEFT: {
			p_app_conf->pacs_param.sink_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL;
			p_app_conf->pacs_param.source_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL;
			p_app_conf->ascs_param.source_ase_num = 1;
			p_app_conf->ascs_param.sink_ase_num = 1;
			break;
		}
		case RTK_BT_LE_AUDIO_RIGHT: {
			p_app_conf->pacs_param.sink_audio_location = RTK_BT_LE_AUDIO_LOCATION_FR;
			p_app_conf->pacs_param.source_audio_location = RTK_BT_LE_AUDIO_LOCATION_FR;
			p_app_conf->ascs_param.source_ase_num = 1;
			p_app_conf->ascs_param.sink_ase_num = 1;
			break;
		}
		case RTK_BT_LE_AUDIO_STEREO: {
			p_app_conf->pacs_param.sink_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR;
			p_app_conf->pacs_param.source_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR;
			p_app_conf->ascs_param.source_ase_num = 1;
			p_app_conf->ascs_param.sink_ase_num = 1;
			break;
		}
		default:
			BT_LOGE("[APP] unsupport sound_channel, use left instead %d\r\n", sound_channel);
			p_app_conf->pacs_param.sink_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL;
			p_app_conf->pacs_param.source_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL;
			p_app_conf->ascs_param.source_ase_num = 1;
			p_app_conf->ascs_param.sink_ase_num = 1;
			break;
		}
	}
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
	if (p_app_conf->cap_param.cap_role & RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR) {
		if (!p_cap_param) {
			BT_LOGE("[APP] p_cap_param should not be NULL when initializing RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR \r\n");
		}
		p_app_conf->cap_param.mcs_param.general_mcs = true;
		p_app_conf->cap_param.mcs_param.char_media_control_point_support = true;
		p_app_conf->cap_param.mcs_param.char_media_control_point_opcodes_supported_optional_property_notify = true;
		p_app_conf->cap_param.mcs_param.char_media_player_name_optional_property_notify = true;
		p_app_conf->cap_param.mcs_param.char_track_title_optional_property_notify = true;
		p_app_conf->cap_param.mcs_param.char_track_duration_optional_property_notify = true;
		p_app_conf->cap_param.mcs_param.char_track_position_optional_property_notify = true;
		p_app_conf->cap_param.mcs_param.char_playing_order_support = true;
		p_app_conf->cap_param.mcs_param.char_playing_order_optional_property_notify = true;
		p_app_conf->cap_param.mcs_param.char_playing_orders_supported_support = true;
		p_app_conf->cap_param.mcs_param.char_playback_speed_support = true;
		p_app_conf->cap_param.mcs_param.char_playback_speed_optional_property_notify = true;
		p_app_conf->cap_param.mcs_param.char_seeking_speed_support = true;
		p_app_conf->cap_param.mcs_param.char_seeking_speed_optional_property_notify = true;
		p_app_conf->cap_param.mcs_param.char_media_player_icon_url_support = true;
		p_app_conf->cap_param.mcs_param.media_state = RTK_BT_LE_AUDIO_MCS_MEDIA_STATE_PAUSED;
		p_app_conf->cap_param.mcs_param.playing_orders_supported = 0x3FF;
		p_app_conf->cap_param.mcs_param.media_control_point_opcodes_supported = 0x1FFFFF;
		p_app_conf->cap_param.mcs_param.p_mcs_service_id = p_cap_param->p_mcs_service_id;
	}
#endif
	if (p_app_conf->cap_param.cap_role == RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR) {
		if (!p_cap_param) {
			BT_LOGE("[APP] p_cap_param should not be NULL when initializing RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR \r\n");
		}
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
		p_app_conf->cap_param.csis_param.csis_size = 2;
		p_app_conf->cap_param.csis_param.csis_rank = 1;
		p_app_conf->cap_param.csis_param.csis_feature = RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_LOCK_EXIST | RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_SIZE_EXIST |
														RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_RANK_EXIST | RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_SIRK_NOTIFY_SUPPORT;
		memcpy(p_app_conf->cap_param.csis_param.csis_sirk, p_cap_param->p_csis_sirk, p_cap_param->csis_sirk_len);
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
		p_app_conf->cap_param.aics_param.aics_vcs_num = p_cap_param->aics_vcs_num;
		p_app_conf->cap_param.aics_param.p_aics_vcs_tbl = p_cap_param->p_aics_vcs_tbl;
		p_app_conf->cap_param.aics_param.aics_mics_num = p_cap_param->aics_mics_num;
		p_app_conf->cap_param.aics_param.p_aics_mics_tbl = p_cap_param->p_aics_mics_tbl;
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
		p_app_conf->cap_param.vocs_param.vocs_num = p_cap_param->vocs_num;
		p_app_conf->cap_param.vocs_param.p_vocs_feature_tbl = p_cap_param->p_vocs_feature_tbl;
#endif
	}
	return RTK_BT_OK;
}

void app_bt_le_audio_update_adv(void *app_conf, uint8_t *device_name, uint8_t *adv_data, uint8_t *adv_data_len)
{
	uint16_t idx = *adv_data_len;
	rtk_bt_le_audio_app_conf_t *p_app_conf = (rtk_bt_le_audio_app_conf_t *)app_conf;
	uint16_t sink_available_contexts = p_app_conf->pacs_param.sink_available_contexts;
	uint16_t source_available_contexts = p_app_conf->pacs_param.source_available_contexts;
	rtk_bt_le_audio_bap_role_t bap_role = p_app_conf->bap_role;
	uint16_t tmas_role = 0;
	uint8_t gmap_role = 0;
	tmas_role |= p_app_conf->tmap_role;
	gmap_role |= p_app_conf->gmap_role;
	//set adv device name
	uint8_t device_name_len = strlen((const char *)device_name);
	//BT_LOGA("[APP] device_name_len %d *adv_data_len %d\r\n", device_name_len,*adv_data_len);
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
#if defined(CONFIG_BT_TMAP_SUPPORT) && CONFIG_BT_TMAP_SUPPORT
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
#if defined(CONFIG_BT_GMAP_SUPPORT) && CONFIG_BT_GMAP_SUPPORT
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
	if (rtk_bt_le_audio_csis_gen_rsi(p_app_conf->cap_param.csis_param.csis_sirk, prsi_data) == RTK_BT_OK) {
		adv_data[idx] = RTK_BT_LE_CSI_RSI_LEN + 1;
		idx++;
		adv_data[idx] = RTK_BT_LE_GAP_ADTYPE_RSI;
		idx++;
		memcpy(adv_data + idx, prsi_data, RTK_BT_LE_CSI_RSI_LEN);
		idx += RTK_BT_LE_CSI_RSI_LEN;
	}
#endif
	if (idx > RTK_LE_AUDIO_ADV_LEN_MAX) {
		BT_LOGE("[APP] %s adv len too long, adv len = %d\r\n", __func__, idx);
	}
	*adv_data_len = idx;
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

void app_bt_le_audio_iso_data_path_init(void)
{
	app_lea_iso_data_path_list_t *p_list = &g_app_lea_iso_path_list;
	memset(p_list, 0, sizeof(app_lea_iso_data_path_list_t));

	INIT_LIST_HEAD(&p_list->head);
	if (p_list->mtx == NULL) {
		osif_mutex_create(&p_list->mtx);
	}
	if (rtk_bt_audio_init()) {
		BT_LOGE("[APP] %s rtk_bt_audio_init fail \r\n", __func__);
	}
#if defined(CONFIG_BT_APP_DEBUG) && CONFIG_BT_APP_DEBUG
	if (g_app_printf_mtx == NULL) {
		osif_mutex_create(&g_app_printf_mtx);
	}
#endif
}

void app_bt_le_audio_iso_data_path_deinit(void)
{
	app_lea_iso_data_path_list_t *p_list = &g_app_lea_iso_path_list;
	if (p_list->mtx != NULL) {
		osif_mutex_delete(p_list->mtx);
		p_list->mtx = NULL;
	}
	if (rtk_bt_audio_deinit()) {
		BT_LOGE("[APP] %s rtk_bt_audio_deinit fail \r\n", __func__);
	}
#if defined(CONFIG_BT_APP_DEBUG) && CONFIG_BT_APP_DEBUG
	if (g_app_printf_mtx != NULL) {
		osif_mutex_delete(g_app_printf_mtx);
		g_app_printf_mtx = NULL;
	}
#endif
}

uint8_t app_bt_le_audio_iso_data_path_get_num(uint8_t path_direction)
{
	uint8_t iso_data_path_num = 0;
	app_lea_iso_data_path_list_t *p_list = &g_app_lea_iso_path_list;

	if (path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
		iso_data_path_num = p_list->tx_iso_path_num;
	} else if (path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_RX) {
		iso_data_path_num = p_list->rx_iso_path_num;
	} else {
		BT_LOGE("[APP] %s: path_direction %d not support\r\n", __func__, path_direction);
	}

	return iso_data_path_num;
}

app_lea_iso_data_path_t *app_bt_le_audio_iso_data_path_find(uint16_t iso_conn_handle, uint8_t path_direction)
{
	app_lea_iso_data_path_t *p_iso_path = NULL, *p_iso_path_tmp = NULL;
	app_lea_iso_data_path_list_t *p_list = &g_app_lea_iso_path_list;
	void *pmtx = p_list->mtx;
	struct list_head *iterator = NULL, *phead = &p_list->head;

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_iso_path_tmp = list_entry(iterator, app_lea_iso_data_path_t, list);
			if (p_iso_path_tmp == NULL) {
				continue;
			}
			if (p_iso_path_tmp->iso_conn_handle == iso_conn_handle && p_iso_path_tmp->path_direction == path_direction) {
				p_iso_path = p_iso_path_tmp;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return p_iso_path;
}

app_lea_iso_data_path_t *app_bt_le_audio_iso_data_path_find_by_idx(uint8_t idx, uint8_t path_direction)
{
	uint8_t i = 0, iso_data_path_num = 0;
	app_lea_iso_data_path_t *p_iso_path = NULL, *p_iso_path_tmp = NULL;
	app_lea_iso_data_path_list_t *p_list = &g_app_lea_iso_path_list;
	struct list_head *iterator = NULL, *phead = &p_list->head;
	void *pmtx = p_list->mtx;

	if (path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
		iso_data_path_num = p_list->tx_iso_path_num;
	} else if (path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_RX) {
		iso_data_path_num = p_list->rx_iso_path_num;
	} else {
		BT_LOGE("[APP] %s: path_direction %d not support\r\n", __func__, path_direction);
	}
	if (idx >= iso_data_path_num) {
		BT_LOGE("[APP] %s: idx(%d) >= iso_data_path_num(%d)\r\n", __func__, idx, iso_data_path_num);
		return NULL;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_iso_path_tmp = list_entry(iterator, app_lea_iso_data_path_t, list);
			if (p_iso_path_tmp == NULL || p_iso_path_tmp->path_direction != path_direction) {
				continue;
			}
			if (i++ == idx) {
				p_iso_path = p_iso_path_tmp;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return p_iso_path;
}

uint16_t app_bt_le_audio_iso_data_path_add(uint8_t iso_mode, uint8_t iso_idx, uint8_t path_direction, uint16_t iso_conn_handle, void *codec_cfg)
{
	rtk_bt_le_audio_cfg_codec_t *p_codec = (rtk_bt_le_audio_cfg_codec_t *)codec_cfg;
	app_lea_iso_data_path_t *p_iso_path = NULL;
	float left_hw_volume, right_hw_volume = 0;

	app_lea_iso_data_path_list_t *p_list = &g_app_lea_iso_path_list;
	void *pmtx = p_list->mtx;
	struct list_head *phead = &p_list->head;

	if (NULL != app_bt_le_audio_iso_data_path_find(iso_conn_handle, path_direction)) {
		BT_LOGA("[APP] %s iso_conn_handle (0x%x) alreay add\r\n", __func__, iso_conn_handle);
		return RTK_BT_OK;
	}
	p_iso_path = (app_lea_iso_data_path_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(app_lea_iso_data_path_t));
	if (!p_iso_path) {
		BT_LOGE("[APP] %s osif_mem_alloc (len=%d) fail\r\n", __func__, sizeof(app_lea_iso_data_path_t));
		return RTK_BT_FAIL;
	}
	memset(p_iso_path, 0, sizeof(app_lea_iso_data_path_t));
	p_iso_path->iso_mode = iso_mode;
	p_iso_path->iso_idx = iso_idx;
	p_iso_path->path_direction = path_direction;
	p_iso_path->iso_conn_handle = iso_conn_handle;
	p_iso_path->pkt_seq_num = 0;
	p_iso_path->iso_data_tx_queue_num = 0;
	p_iso_path->iso_data_rx_queue_num = 0;
	memcpy(&p_iso_path->codec, p_codec, sizeof(rtk_bt_le_audio_cfg_codec_t));
	p_iso_path->codec_entity = app_bt_le_audio_lc3_codec_entity_add(p_codec);
	p_iso_path->is_add_new_entity = true;
	BT_LOGD("[APP] %s:add lc3_codec: type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu 0x%x, octets_per_codec_frame 0x%x, audio_channel_allocation 0x%x)\r\n",
			__func__, p_codec->type_exist, p_codec->frame_duration,
			p_codec->sample_frequency, p_codec->codec_frame_blocks_per_sdu,
			p_codec->octets_per_codec_frame, (unsigned int)p_codec->audio_channel_allocation);
	if (path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_RX) {
#if 0
		uint8_t channels = 0;
		uint32_t rate = 0;

		rate = app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(p_codec->sample_frequency);
		channels = app_bt_le_audio_get_lea_chnl_num(p_codec->audio_channel_allocation);
		p_iso_path->track_hdl = app_bt_le_audio_track_add(p_codec);
		p_iso_path->track_hdl->audio_sync_flag = true;
		p_iso_path->track_hdl->sdu_interval = p_codec->frame_duration * 1000; // micro seconds
		p_iso_path->track_hdl->pres_us = 20000;
		p_iso_path->track_hdl->audio_delay_start_buff.size = RTK_BT_AUDIO_DELAY_START_BUFFER_COUNT * p_codec->frame_duration * (rate * channels * (16 / 8)) / 1000;
		p_iso_path->track_hdl->audio_delay_start_buff.buff = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_iso_path->track_hdl->audio_delay_start_buff.size);
		memset((void *)p_iso_path->track_hdl->audio_delay_start_buff.buff, 0, p_iso_path->track_hdl->audio_delay_start_buff.size);
#endif
		p_iso_path->track_hdl = app_bt_le_audio_track_add(p_codec);
	}
	if (path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_RX && p_iso_path->track_hdl != NULL) {
		left_hw_volume = DEFAULT_AUDIO_HARDWARE_LEFT_VOLUME;
		right_hw_volume = DEFAULT_AUDIO_HARDWARE_RIGHT_VOLUME;
		rtk_bt_audio_track_set_hardware_volume(left_hw_volume, right_hw_volume);
		BT_LOGA("[APP] %s: set hardware left volume (%.2f), right volume (%.2f)\r\n", __func__, left_hw_volume, right_hw_volume);
	}
#if defined(RTK_BLE_AUDIO_RECORD_SUPPORT) && RTK_BLE_AUDIO_RECORD_SUPPORT
	// can init only one record
	if (path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
		if (!p_iso_path->record_hdl) {
			p_iso_path->record_hdl = app_bt_le_audio_record_add(p_codec);
		}
	}
#endif
	INIT_LIST_HEAD(&p_iso_path->iso_data_head);
	//BT_LOGA("[APP] %s p_iso_path 0x%08x!\r\n", __func__,p_iso_path);
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_add_tail(&p_iso_path->list, phead);/* insert list */
	if (path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
		p_list->tx_iso_path_num++;
	} else if (path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_RX) {
		p_list->rx_iso_path_num++;
	} else {
		BT_LOGE("[APP] %s: path_direction %d not support\r\n", __func__, path_direction);
	}
	osif_mutex_give(pmtx);
	BT_LOGA("[APP] add iso_conn_handle(0x%x) path_direction(%d) ok, iso_idx = %d\r\n", iso_conn_handle, path_direction, iso_idx);

	return RTK_BT_OK;
}

uint16_t app_bt_le_audio_iso_data_path_remove(uint16_t iso_conn_handle, uint8_t path_direction)
{
	uint16_t ret = RTK_BT_OK;
	app_lea_iso_data_path_t *p_iso_path = NULL;
	app_lea_iso_data_path_list_t *p_list = &g_app_lea_iso_path_list;
	void *pmtx = p_list->mtx;

	p_iso_path = app_bt_le_audio_iso_data_path_find(iso_conn_handle, path_direction);
	if (p_iso_path == NULL) {
		BT_LOGE("[APP] %s iso_conn_handle (0x%x) not find\r\n", __func__, iso_conn_handle);
		return RTK_BT_FAIL;
	}
	//wait processing end
	while (p_iso_path->is_processing) {
		BT_LOGE("[APP] %s iso_conn_handle (0x%x) path_direction %d is_processing, wait\r\n", __func__, p_iso_path->iso_conn_handle, path_direction);
		osif_delay(1);
	}
	//start remove path
	p_iso_path->is_removing = true;
	if (p_iso_path->is_add_new_entity) {
		ret = app_bt_le_audio_lc3_codec_entity_remove(p_iso_path->codec_entity);
		if (ret != RTK_BT_OK) {
			BT_LOGE("[APP] %s app_bt_le_audio_lc3_codec_entity_remove fail, ret = 0x%x\r\n", __func__, ret);
		}
		//init track if sink (rx)
		if (p_iso_path->path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_RX) {
			ret = app_bt_le_audio_track_remove(p_iso_path->track_hdl);
			if (ret != RTK_BT_OK) {
				BT_LOGE("[APP] %s app_bt_le_audio_track_remove fail, ret = 0x%x\r\n", __func__, ret);
			}
		}
#if defined(RTK_BLE_AUDIO_RECORD_SUPPORT) && RTK_BLE_AUDIO_RECORD_SUPPORT
		//init record if source (tx)
		if (p_iso_path->path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
			ret = app_bt_le_audio_record_remove(p_iso_path->record_hdl);
			if (ret != RTK_BT_OK) {
				BT_LOGE("[APP] %s app_bt_le_audio_record_remove fail, ret = 0x%x\r\n", __func__, ret);
			}
		}
#endif
		p_iso_path->is_add_new_entity = false;
	}
	/* waiting tx rx queue */
	while (p_iso_path->iso_data_tx_queue_num || p_iso_path->iso_data_rx_queue_num) {
		osif_delay(1);
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_del_init(&p_iso_path->list);
	if (path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
		if (p_list->tx_iso_path_num > 0) {
			p_list->tx_iso_path_num--;
		} else {
			BT_LOGE("[APP] %s: p_list->tx_iso_path_num is already %d, something is wrong\r\n", __func__, p_list->tx_iso_path_num);
		}
	} else if (path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_RX) {
		if (p_list->rx_iso_path_num > 0) {
			p_list->rx_iso_path_num--;
		} else {
			BT_LOGE("[APP] %s: p_list->tx_iso_path_num is already %d, something is wrong\r\n", __func__, p_list->tx_iso_path_num);
		}
	} else {
		BT_LOGE("[APP] %s: path_direction %d not support\r\n", __func__, path_direction);
	}
	if (p_iso_path->p_encode_data) {
		osif_mem_free(p_iso_path->p_encode_data);
	}
	memset(p_iso_path, 0, sizeof(app_lea_iso_data_path_t));
	osif_mem_free(p_iso_path);
	osif_mutex_give(pmtx);
	BT_LOGA("[APP] remove iso_conn_handle(0x%x) path_direction %d ok \r\n", iso_conn_handle, path_direction);

	return RTK_BT_OK;
}

app_bt_le_audio_device_info_t *app_bt_le_audio_device_list_find(uint16_t conn_handle)
{
	app_bt_le_audio_device_info_t *p_device_info = NULL, *p_device_info_tmp = NULL;
	struct list_head *phead = &g_app_lea_device_list_head;
	void *pmtx = g_app_lea_device_list_mtx;
	struct list_head *iterator = NULL;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return fail: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_device_info_tmp = list_entry(iterator, app_bt_le_audio_device_info_t, list);
			if (p_device_info_tmp == NULL) {
				continue;
			}
			if (p_device_info_tmp->conn_handle == conn_handle) {
				p_device_info = p_device_info_tmp;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return p_device_info;
}

app_bt_le_audio_device_info_t *app_bt_le_audio_device_list_add(uint16_t conn_handle)
{
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	struct list_head *phead = &g_app_lea_device_list_head;
	void *pmtx = g_app_lea_device_list_mtx;

	//check if the device in device list
	p_device_info = app_bt_le_audio_device_list_find(conn_handle);
	if (p_device_info) {
		BT_LOGD("[APP] %s conn_handle (%d) alreay in device_list , skip add action\r\n", __func__, conn_handle);
		return p_device_info;
	}
	//check if the device in group list
	p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(conn_handle);
	if (p_device_info) {
		BT_LOGD("[APP] %s conn_handle (%d) alreay in group_list, skip add action\r\n", __func__, conn_handle);
		return p_device_info;
	}
	//the new one
	p_device_info = (app_bt_le_audio_device_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(app_bt_le_audio_device_info_t));
	if (p_device_info == NULL) {
		BT_LOGE("[APP] %s osif_mem_alloc (len=%d) fail\r\n", __func__, sizeof(app_bt_le_audio_device_info_t));
		return NULL;
	}
	memset(p_device_info, 0, sizeof(app_bt_le_audio_device_info_t));
	p_device_info->conn_handle = conn_handle;
	//BT_LOGA("[APP] %s p_group_info 0x%08x p_device_info=%08x!\r\n", __func__,p_group_info,p_device_info);
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_add_tail(&p_device_info->list, phead);/* insert list */
	osif_mutex_give(pmtx);
	BT_LOGD("[APP] add conn_handle(%d) in device list ok\r\n", conn_handle);

	return p_device_info;
}

uint16_t app_bt_le_audio_device_list_remove(uint16_t conn_handle)
{
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	void *pmtx = g_app_lea_device_list_mtx;

	p_device_info = app_bt_le_audio_device_list_find(conn_handle);
	if (p_device_info == NULL) {
		BT_LOGD("[APP] %s conn_handle (%d) not find, skip remove action\r\n", __func__, conn_handle);
		return RTK_BT_OK;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_del_init(&p_device_info->list);
	osif_mutex_give(pmtx);
	if (p_device_info->brs_char_tbl) {
		osif_mem_free(p_device_info->brs_char_tbl);
	}
	memset(p_device_info, 0, sizeof(app_bt_le_audio_device_info_t));
	osif_mem_free(p_device_info);
	BT_LOGD("[APP] remove conn_handle(%d) from device list ok\r\n", conn_handle);

	return RTK_BT_OK;
}

uint16_t app_bt_le_audio_device_list_remove_all(void)
{
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	struct list_head *phead = &g_app_lea_device_list_head;
	void *pmtx = g_app_lea_device_list_mtx;
	struct list_head *plist = NULL, *pnext = NULL;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	//delete and free all group in group list
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_device_info = (app_bt_le_audio_device_info_t *)plist;
		if (p_device_info && p_device_info->conn_handle) {
			app_bt_le_audio_device_list_remove(p_device_info->conn_handle);
		}
		plist = pnext;
	}

	return RTK_BT_OK;
}

void app_bt_le_audio_group_list_init(void)
{
	INIT_LIST_HEAD(&g_app_lea_group_list_head);
	if (g_app_lea_group_list_mtx == NULL) {
		osif_mutex_create(&g_app_lea_group_list_mtx);
		//BT_LOGA("[APP] %s g_app_lea_group_list_mtx = %08x\r\n",__func__,g_app_lea_group_list_mtx);
	}
	INIT_LIST_HEAD(&g_app_lea_device_list_head);
	if (g_app_lea_device_list_mtx == NULL) {
		osif_mutex_create(&g_app_lea_device_list_mtx);
		//BT_LOGA("[APP] %s g_app_lea_device_list_mtx = %08x\r\n",__func__,g_app_lea_device_list_mtx);
	}
}

void app_bt_le_audio_group_list_deinit(void)
{
	app_bt_le_audio_group_list_remove_all();
	INIT_LIST_HEAD(&g_app_lea_group_list_head);

	if (g_app_lea_group_list_mtx != NULL) {
		osif_mutex_delete(g_app_lea_group_list_mtx);
		g_app_lea_group_list_mtx = NULL;
	}
	app_bt_le_audio_device_list_remove_all();
	INIT_LIST_HEAD(&g_app_lea_device_list_head);
	if (g_app_lea_device_list_mtx != NULL) {
		osif_mutex_delete(g_app_lea_device_list_mtx);
		g_app_lea_device_list_mtx = NULL;
	}
}

uint8_t app_bt_le_audio_group_list_get_num(void)
{
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	struct list_head *iterator = NULL;
	struct list_head *phead = &g_app_lea_group_list_head;
	void *pmtx = g_app_lea_group_list_mtx;
	uint8_t group_num = 0;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_group_info = list_entry(iterator, app_bt_le_audio_group_info_t, list);
			if (p_group_info == NULL) {
				continue;
			}
			group_num ++;
		}
	}
	osif_mutex_give(pmtx);

	return group_num;
}

app_bt_le_audio_group_info_t *app_bt_le_audio_group_list_find(rtk_bt_le_audio_group_handle_t group_handle)
{
	app_bt_le_audio_group_info_t *p_group_info = NULL, *p_group_info_tmp = NULL;
	struct list_head *iterator = NULL;
	struct list_head *phead = &g_app_lea_group_list_head;
	void *pmtx = g_app_lea_group_list_mtx;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_group_info_tmp = list_entry(iterator, app_bt_le_audio_group_info_t, list);
			if (p_group_info_tmp == NULL) {
				continue;
			}
			if (p_group_info_tmp->group_handle == group_handle) {
				p_group_info = p_group_info_tmp;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return p_group_info;
}

app_bt_le_audio_group_info_t *app_bt_le_audio_group_list_find_by_conn_handle(uint16_t conn_handle)
{
	app_bt_le_audio_group_info_t *p_group_info = NULL, *p_group_info_tmp = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	struct list_head *iterator = NULL;
	struct list_head *phead = &g_app_lea_group_list_head;
	void *pmtx = g_app_lea_group_list_mtx;
	struct list_head *pdevice_head = NULL;
	void *pdevice_mtx = NULL;
	struct list_head *device_iterator = NULL;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_group_info_tmp = list_entry(iterator, app_bt_le_audio_group_info_t, list);
			if (p_group_info_tmp == NULL) {
				continue;
			}
			pdevice_head = &p_group_info_tmp->device_info_head;
			pdevice_mtx = g_app_lea_device_list_mtx;
			if (pdevice_head == NULL || pdevice_mtx == NULL) {
				continue;
			}
			osif_mutex_take(pdevice_mtx, BT_TIMEOUT_FOREVER);
			if (!list_empty(pdevice_head)) {
				list_for_each(device_iterator, pdevice_head) {
					p_device_info = list_entry(device_iterator, app_bt_le_audio_device_info_t, list);
					if (p_device_info == NULL) {
						continue;
					}
					if (p_device_info->conn_handle == conn_handle) {
						p_group_info = p_group_info_tmp;
						break;
					}
				}
			}
			osif_mutex_give(pdevice_mtx);
		}
	}
	osif_mutex_give(pmtx);

	return p_group_info;
}

rtk_bt_le_audio_group_handle_t app_bt_le_audio_group_list_find_by_group_idx(uint8_t group_idx)
{
	rtk_bt_le_audio_group_handle_t group_handle = NULL;
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	struct list_head *phead = &g_app_lea_group_list_head;
	void *pmtx = g_app_lea_group_list_mtx;
	struct list_head *iterator = NULL;
	uint8_t group_num = 0, i = 0;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return fail: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}
	group_num = app_bt_le_audio_group_list_get_num();
	if (group_idx >= group_num) {
		BT_LOGE("[APP] %s: group_idx(%d) >= group_num(%d)\r\n", __func__, group_idx, group_num);
		return NULL;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_group_info = list_entry(iterator, app_bt_le_audio_group_info_t, list);
			if (p_group_info == NULL) {
				continue;
			}
			if (i++ == group_idx) {
				group_handle = p_group_info->group_handle;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return group_handle;
}

app_bt_le_audio_device_info_t *app_bt_le_audio_group_list_find_dev_by_idx(rtk_bt_le_audio_group_handle_t group_handle, uint8_t dev_idx)
{
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL, *p_device_info_tmp = NULL;
	struct list_head *phead = NULL;
	void *pmtx = NULL;
	struct list_head *iterator = NULL;
	uint8_t i = 0;

	p_group_info = app_bt_le_audio_group_list_find(group_handle);
	if (p_group_info == NULL) {
		BT_LOGE("[APP] %s group_handle (%08x) not find ,skip\r\n", __func__, group_handle);
		return NULL;
	}
	if (dev_idx >= p_group_info->dev_num) {
		BT_LOGE("[APP] %s: dev_idx(%d) >= dev_num(%d)\r\n", __func__, dev_idx, p_group_info->dev_num);
		return NULL;
	}
	phead = &p_group_info->device_info_head;
	pmtx = g_app_lea_device_list_mtx;
	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return fail: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_device_info_tmp = list_entry(iterator, app_bt_le_audio_device_info_t, list);
			if (p_device_info_tmp == NULL) {
				continue;
			}
			if (i++ == dev_idx) {
				p_device_info = p_device_info_tmp;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return p_device_info;
}

app_bt_le_audio_device_info_t *app_bt_le_audio_group_list_find_dev_by_conn_handle(uint16_t conn_handle)
{
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL, *p_device_info_tmp = NULL;
	struct list_head *phead = &g_app_lea_group_list_head;
	void *pmtx = g_app_lea_group_list_mtx;
	struct list_head *iterator = NULL, *device_iterator = NULL;
	struct list_head *pdevice_head = NULL;
	void *pdevice_mtx = NULL;
	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return fail: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_group_info = list_entry(iterator, app_bt_le_audio_group_info_t, list);
			if (p_group_info == NULL) {
				continue;
			}
			pdevice_head = &p_group_info->device_info_head;
			pdevice_mtx = g_app_lea_device_list_mtx;
			if (pdevice_head == NULL || pdevice_mtx == NULL) {
				BT_LOGE("[APP] %s return fail: pdevice_head=%08x,pdevice_mtx=%08x\r\n", __func__, pdevice_head, pdevice_mtx);
				continue;
			}
			osif_mutex_take(pdevice_mtx, BT_TIMEOUT_FOREVER);
			if (!list_empty(pdevice_head)) {
				list_for_each(device_iterator, pdevice_head) {
					p_device_info_tmp = list_entry(device_iterator, app_bt_le_audio_device_info_t, list);
					if (p_device_info_tmp == NULL) {
						continue;
					}
					if (p_device_info_tmp->conn_handle == conn_handle) {
						p_device_info = p_device_info_tmp;
						break;
					}
				}
			}
			osif_mutex_give(pdevice_mtx);
		}
	}
	osif_mutex_give(pmtx);

	return p_device_info;
}

app_bt_le_audio_group_info_t *app_bt_le_audio_group_list_add_dev(rtk_bt_le_audio_group_handle_t group_handle, rtk_bt_le_audio_device_handle_t device_handle,
																 uint16_t conn_handle)
{
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	struct list_head *p_group_head = &g_app_lea_group_list_head;
	void *p_group_mtx = g_app_lea_group_list_mtx;
	//struct list_head *p_device_list_head = &g_app_lea_device_list_head;
	struct list_head *p_device_head = NULL;
	void *p_device_mtx = NULL;

	if (p_group_head == NULL || p_group_mtx == NULL) {
		BT_LOGE("[APP] %s return fail: p_group_head=%08x,pmtx=%08x\r\n", __func__, p_group_head, p_group_mtx);
		return NULL;
	}
	p_group_info = app_bt_le_audio_group_list_find(group_handle);
	if (p_group_info) {
		BT_LOGA("[APP] %s group_handle (%08x) alreay in group_list, skip add action\r\n", __func__, group_handle);
		goto ADD_DEV;
	}
	p_group_info = (app_bt_le_audio_group_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(app_bt_le_audio_group_info_t));
	if (!p_group_info) {
		BT_LOGE("[APP] %s osif_mem_alloc (len=%d) fail\r\n", __func__, sizeof(app_bt_le_audio_group_info_t));
		return NULL;
	}
	memset(p_group_info, 0, sizeof(app_bt_le_audio_group_info_t));
	p_group_info->group_handle = group_handle;
	INIT_LIST_HEAD(&p_group_info->device_info_head);
	osif_mutex_take(p_group_mtx, BT_TIMEOUT_FOREVER);
	list_add_tail(&p_group_info->list, p_group_head);/* insert list */
	osif_mutex_give(p_group_mtx);
	BT_LOGD("[APP] add group_handle(%08x) in group list ok \r\n", group_handle);

ADD_DEV:
	p_device_head = &p_group_info->device_info_head;
	p_device_mtx = g_app_lea_device_list_mtx;
	if (p_device_head == NULL || p_device_mtx == NULL) {
		BT_LOGE("[APP] %s return fail: p_device_head=%08x,p_device_mtx=%08x\r\n", __func__, p_device_head, p_device_mtx);
		return NULL;
	}
	p_device_info = app_bt_le_audio_device_list_find(conn_handle);
	osif_mutex_take(p_device_mtx, BT_TIMEOUT_FOREVER);
	//the new one
	if (p_device_info == NULL) {
		p_device_info = (app_bt_le_audio_device_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(app_bt_le_audio_device_info_t));
		if (p_device_info == NULL) {
			BT_LOGE("[APP] %s osif_mem_alloc (len=%d) fail\r\n", __func__, sizeof(app_bt_le_audio_device_info_t));
			osif_mutex_give(p_device_mtx);
			return NULL;
		}
		memset(p_device_info, 0, sizeof(app_bt_le_audio_device_info_t));
	} else {
		//remove the device from device list
		list_del_init(&p_device_info->list);
		BT_LOGA("[APP] %s remove the device(conn_handle %d) from device list and insert it to group list\r\n", __func__, conn_handle);
	}
	p_device_info->conn_handle = conn_handle;
	p_device_info->device_handle = device_handle;
	//insert the device to deviveinfo list of this group handle
	list_add_tail(&p_device_info->list, p_device_head);/* insert list */
	p_group_info->dev_num++;
	osif_mutex_give(p_device_mtx);
	BT_LOGD("[APP] add device_handle(%08x) in group_handle(%08x) ok ,dev_num = %d \r\n", device_handle, group_handle, p_group_info->dev_num);

	return p_group_info;
}

uint16_t app_bt_le_audio_group_list_remove_dev_by_conn_handle(rtk_bt_le_audio_group_handle_t group_handle, uint16_t conn_handle)
{
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	struct list_head *phead = &g_app_lea_group_list_head;
	void *pmtx = g_app_lea_group_list_mtx;
	struct list_head *pdevice_head = NULL;
	struct list_head *plist = NULL, *pnext = NULL;
	void *pdevice_mtx = NULL;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_group_info = app_bt_le_audio_group_list_find(group_handle);
	if (p_group_info == NULL) {
		BT_LOGE("[APP] %s group_handle (%08x) not find ,skip\r\n", __func__, group_handle);
		return RTK_BT_ERR_MISMATCH;
	}
	pdevice_head = &p_group_info->device_info_head;
	pdevice_mtx = g_app_lea_device_list_mtx;
	if (pdevice_head == NULL || pdevice_mtx == NULL) {
		BT_LOGE("[APP] %s return: pdevice_head=%08x,pdevice_mtx=%08x\r\n", __func__, pdevice_head, pdevice_mtx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	//delete and free all device info in this group
	osif_mutex_take(pdevice_mtx, BT_TIMEOUT_FOREVER);
	plist = pdevice_head->next;
	while (plist != pdevice_head) {
		pnext = plist->next;
		p_device_info = (app_bt_le_audio_device_info_t *)plist;
		if (p_device_info) {
			if (p_device_info->conn_handle == conn_handle) {
				list_del_init(&p_device_info->list);
				p_group_info->dev_num--;
				if (p_device_info->brs_char_tbl) {
					osif_mem_free(p_device_info->brs_char_tbl);
				}
				memset(p_device_info, 0, sizeof(app_bt_le_audio_device_info_t));
				osif_mem_free(p_device_info);
				break;
			}
		}
		plist = pnext;
	}
	osif_mutex_give(pdevice_mtx);
	BT_LOGD("[APP] remove conn_handle(%d) in group_handle(%08x) ok, dev_num(%d)\r\n", conn_handle, group_handle, p_group_info->dev_num);

	return RTK_BT_OK;
}

uint16_t app_bt_le_audio_group_list_remove_dev(rtk_bt_le_audio_group_handle_t group_handle, rtk_bt_le_audio_device_handle_t device_handle)
{
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	struct list_head *phead = &g_app_lea_group_list_head;
	void *pmtx = g_app_lea_group_list_mtx;
	struct list_head *pdevice_head = NULL;
	struct list_head *plist = NULL, *pnext = NULL;
	void *pdevice_mtx = NULL;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_group_info = app_bt_le_audio_group_list_find(group_handle);
	if (p_group_info == NULL) {
		BT_LOGE("[APP] %s group_handle (%08x) not find ,skip\r\n", __func__, group_handle);
		return RTK_BT_ERR_MISMATCH;
	}
	pdevice_head = &p_group_info->device_info_head;
	pdevice_mtx = g_app_lea_device_list_mtx;
	if (pdevice_head == NULL || pdevice_mtx == NULL) {
		BT_LOGE("[APP] %s return: pdevice_head=%08x,pdevice_mtx=%08x\r\n", __func__, pdevice_head, pdevice_mtx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	//delete and free all device info in this group
	osif_mutex_take(pdevice_mtx, BT_TIMEOUT_FOREVER);
	plist = pdevice_head->next;
	while (plist != pdevice_head) {
		pnext = plist->next;
		p_device_info = (app_bt_le_audio_device_info_t *)plist;
		if (p_device_info) {
			if (p_device_info->device_handle == device_handle) {
				list_del_init(&p_device_info->list);
				p_group_info->dev_num--;
				if (p_device_info->brs_char_tbl) {
					osif_mem_free(p_device_info->brs_char_tbl);
				}
				memset(p_device_info, 0, sizeof(app_bt_le_audio_device_info_t));
				osif_mem_free(p_device_info);
				break;
			}
		}
		plist = pnext;
	}
	osif_mutex_give(pdevice_mtx);
	BT_LOGD("[APP] remove device_handle(%08x) in group_handle(%08x) ok, dev_num(%d)\r\n", device_handle, group_handle, p_group_info->dev_num);

	return RTK_BT_OK;
}

uint16_t app_bt_le_audio_group_list_remove(rtk_bt_le_audio_group_handle_t group_handle)
{
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	struct list_head *phead = &g_app_lea_group_list_head;
	void *pmtx = g_app_lea_group_list_mtx;
	struct list_head *pdevice_head = NULL;
	struct list_head *plist = NULL, *pnext = NULL;
	void *pdevice_mtx = NULL;
	uint8_t group_num = 0;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_group_info = app_bt_le_audio_group_list_find(group_handle);
	if (p_group_info) {
		pdevice_head = &p_group_info->device_info_head;
		pdevice_mtx = g_app_lea_device_list_mtx;
		if (pdevice_head == NULL || pdevice_mtx == NULL) {
			BT_LOGE("[APP] %s return: pdevice_head=%08x,pdevice_mtx=%08x\r\n", __func__, pdevice_head, pdevice_mtx);
			return RTK_BT_ERR_PARAM_INVALID;
		}
		//delete and free all device info in this group
		osif_mutex_take(pdevice_mtx, BT_TIMEOUT_FOREVER);
		plist = pdevice_head->next;
		while (plist != pdevice_head) {
			pnext = plist->next;
			p_device_info = (app_bt_le_audio_device_info_t *)plist;
			if (p_device_info) {
				list_del_init(&p_device_info->list);
				p_group_info->dev_num--;
				if (p_device_info->brs_char_tbl) {
					osif_mem_free(p_device_info->brs_char_tbl);
				}
				memset(p_device_info, 0, sizeof(app_bt_le_audio_device_info_t));
				osif_mem_free(p_device_info);
			}
			plist = pnext;
		}
		osif_mutex_give(pdevice_mtx);
		//delete and free this group info
		osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
		list_del_init(&p_group_info->list);
		osif_mutex_give(pmtx);
		group_num = app_bt_le_audio_group_list_get_num();
		BT_LOGD("[APP] remove group_handle(0x%08x)ok, group_num(%d)\r\n", group_handle, group_num);
		memset(p_group_info, 0, sizeof(app_bt_le_audio_group_info_t));
		osif_mem_free(p_group_info);
	}

	return RTK_BT_OK;
}

uint16_t app_bt_le_audio_group_list_remove_all(void)
{
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	struct list_head *phead = &g_app_lea_group_list_head;
	void *pmtx = g_app_lea_group_list_mtx;
	struct list_head *plist = NULL, *pnext = NULL;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	//delete and free all group in group list
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_group_info = (app_bt_le_audio_group_info_t *)plist;
		if (p_group_info && p_group_info->group_handle) {
			app_bt_le_audio_group_list_remove(p_group_info->group_handle);
		}
		plist = pnext;
	}

	return RTK_BT_OK;
}

uint16_t app_bt_le_audio_new_device_add_in_group(uint16_t conn_handle, rtk_bt_le_audio_group_handle_t *p_group_handle)
{
	uint16_t ret = RTK_BT_OK;
	app_bt_le_audio_group_info_t *p_group_info = NULL;
	rtk_bt_le_audio_group_handle_t group_handle = NULL;
	rtk_bt_le_audio_device_handle_t device_handle = NULL;

	//save new conn_handle if it support le audio
	p_group_info = app_bt_le_audio_group_list_find_by_conn_handle(conn_handle);
	if (p_group_info != NULL) {
		BT_LOGA("[APP] %s conn_handle (%d) already add \r\n", __func__, conn_handle);
		*p_group_handle = p_group_info->group_handle;
		return RTK_BT_OK;
	}
	//allocate group if group_handle is NULL
	if (*p_group_handle == NULL) {
		ret = rtk_bt_le_audio_group_allocate(&group_handle);
		BT_LOGD("[APP] %s: allocate group_handle %s (group_handle=%08x) \r\n", __func__, (RTK_BT_OK != ret) ? "fail" : "ok", group_handle);
		if (ret != RTK_BT_OK || group_handle == NULL) {
			return ret;
		}
	} else {
		group_handle = *p_group_handle;
		ret = rtk_bt_le_audio_group_find_device_by_conn_handle(group_handle, conn_handle, &device_handle);
		if (RTK_BT_OK != ret) {
			BT_LOGE("[APP] %s conn_handle %d get device_handle fail, add new\r\n", __func__, conn_handle);
		}
	}
	//allocate device if device_handle is NULL
	if (device_handle == NULL) {
		ret = rtk_bt_le_audio_group_add_device(group_handle, conn_handle, &device_handle);
		BT_LOGD("[APP] %s: add device in group %s (group_handle=%08x,device_handle=%08x) \r\n", __func__, (RTK_BT_OK != ret) ? "fail" : "ok", group_handle,
				device_handle);
		if (RTK_BT_OK != ret) {
			return ret;
		}
	}
	p_group_info = app_bt_le_audio_group_list_add_dev(group_handle, device_handle, conn_handle);
	if (p_group_info == NULL) {
		BT_LOGE("[APP] %s conn_handle %d app_bt_le_audio_group_list_add_dev fail \r\n", __func__, conn_handle);
		return RTK_BT_FAIL;
	}
	p_group_info->set_mem_size = 0;
	*p_group_handle = group_handle;

	return RTK_BT_OK;
}

//for normal scan
app_bt_le_audio_scan_dev_info_t *app_bt_le_audio_scan_dev_list_find(rtk_bt_le_addr_t adv_addr)
{
	app_bt_le_audio_scan_dev_info_t *p_scan_dev_info = NULL, *p_scan_dev_info_tmp = NULL;
	struct list_head *phead = &g_app_lea_scan_dev_list_head;
	void *pmtx = g_app_lea_scan_dev_list_mtx;
	struct list_head *iterator = NULL;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return fail: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_scan_dev_info_tmp = list_entry(iterator, app_bt_le_audio_scan_dev_info_t, list);
			if (p_scan_dev_info_tmp == NULL) {
				continue;
			}

			if (0 == memcmp(&p_scan_dev_info_tmp->scan_res_ind.addr, &adv_addr, sizeof(rtk_bt_le_addr_t))) {
				p_scan_dev_info = p_scan_dev_info_tmp;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return p_scan_dev_info;
}
app_bt_le_audio_scan_dev_info_t *app_bt_le_audio_scan_dev_list_find_by_idx(uint8_t scan_dev_idx)
{
	app_bt_le_audio_scan_dev_info_t *p_scan_dev_info = NULL, *p_scan_dev_info_tmp = NULL;
	struct list_head *phead = &g_app_lea_scan_dev_list_head;
	void *pmtx = g_app_lea_scan_dev_list_mtx;
	struct list_head *iterator = NULL;
	uint8_t i = 0;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return fail: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_scan_dev_info_tmp = list_entry(iterator, app_bt_le_audio_scan_dev_info_t, list);
			if (p_scan_dev_info_tmp == NULL) {
				continue;
			}

			if (i++ == scan_dev_idx) {
				p_scan_dev_info = p_scan_dev_info_tmp;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return p_scan_dev_info;
}

uint16_t app_bt_le_audio_scan_dev_list_add(app_bt_le_audio_scan_dev_info_t *p_info)
{
	app_bt_le_audio_scan_dev_info_t *p_scan_dev_info = NULL;
	rtk_bt_le_addr_t adv_addr = {0};
	struct list_head *phead = &g_app_lea_scan_dev_list_head;
	void *pmtx = g_app_lea_scan_dev_list_mtx;

	if (!p_info) {
		BT_LOGE("[APP] %s p_info is NULL \r\n", __func__);
		return 1;
	}
	memcpy((void *)&adv_addr, (void *)&p_info->scan_res_ind.addr, sizeof(rtk_bt_le_addr_t));
	//check if the scan_dev in scan_dev list
	if (app_bt_le_audio_scan_dev_list_find(adv_addr)) {
		BT_LOGD("[APP] %s device(%x %x %x %x %x %x) alreay in scan_dev_list , skip add action\r\n", __func__,
				adv_addr.addr_val[5], adv_addr.addr_val[4], adv_addr.addr_val[3], adv_addr.addr_val[2], adv_addr.addr_val[1], adv_addr.addr_val[0]);
		return 1;
	}

	//the new one
	p_scan_dev_info = (app_bt_le_audio_scan_dev_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(app_bt_le_audio_scan_dev_info_t));
	if (p_scan_dev_info == NULL) {
		BT_LOGE("[APP] %s osif_mem_alloc (len=%d) fail\r\n", __func__, sizeof(app_bt_le_audio_scan_dev_info_t));
		return 1;
	} else {
		memcpy((void *)p_scan_dev_info, (void *)p_info, sizeof(app_bt_le_audio_scan_dev_info_t));
	}

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_add_tail(&p_scan_dev_info->list, phead);/* insert list */
	osif_mutex_give(pmtx);

	BT_LOGD("[APP] add device(%x %x %x %x %x %x Name %s ) in scan_dev list ok\r\n",
			adv_addr.addr_val[5], adv_addr.addr_val[4], adv_addr.addr_val[3], adv_addr.addr_val[2], adv_addr.addr_val[1], adv_addr.addr_val[0],
			p_scan_dev_info->local_name);

	return 0;

}

uint16_t app_bt_le_audio_scan_dev_list_remove(rtk_bt_le_addr_t adv_addr)
{
	app_bt_le_audio_scan_dev_info_t *p_scan_dev_info = NULL;
	void *pmtx = g_app_lea_scan_dev_list_mtx;

	p_scan_dev_info = app_bt_le_audio_scan_dev_list_find(adv_addr);
	if (p_scan_dev_info == NULL) {
		BT_LOGD("[APP] %s device(%x %x %x %x %x %x) not find in scan_dev_list , skip remove action\r\n", __func__,
				adv_addr.addr_val[5], adv_addr.addr_val[4], adv_addr.addr_val[3], adv_addr.addr_val[2], adv_addr.addr_val[1], adv_addr.addr_val[0]);
		return RTK_BT_OK;
	}

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_del_init(&p_scan_dev_info->list);
	osif_mutex_give(pmtx);

	memset(p_scan_dev_info, 0, sizeof(app_bt_le_audio_scan_dev_info_t));
	osif_mem_free(p_scan_dev_info);

	BT_LOGD("[APP] remove device(%x %x %x %x %x %x) from scan_dev list ok\r\n",
			adv_addr.addr_val[5], adv_addr.addr_val[4], adv_addr.addr_val[3], adv_addr.addr_val[2], adv_addr.addr_val[1], adv_addr.addr_val[0]);

	return RTK_BT_OK;
}

uint16_t app_bt_le_audio_scan_dev_list_remove_all(void)
{
	app_bt_le_audio_scan_dev_info_t *p_scan_dev_info = NULL;
	struct list_head *phead = &g_app_lea_scan_dev_list_head;
	struct list_head *plist = NULL, *pnext = NULL;

	if (phead == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x\r\n", __func__, phead);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	//delete and free in scan_dev list
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_scan_dev_info = (app_bt_le_audio_scan_dev_info_t *)plist;
		if (p_scan_dev_info) {
			app_bt_le_audio_scan_dev_list_remove(p_scan_dev_info->scan_res_ind.addr);
		}

		plist = pnext;
	}

	return RTK_BT_OK;
}
//for broadcast scan
uint8_t app_bt_le_audio_bass_scan_dev_list_get_num(void)
{
	app_bt_le_audio_bass_scan_dev_info_t *p_bscan_dev_info = NULL;
	struct list_head *iterator = NULL;
	struct list_head *phead = &g_app_lea_bass_scan_dev_list_head;
	void *pmtx = g_app_lea_bass_scan_dev_list_mtx;
	uint8_t bass_scan_dev_num = 0;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_bscan_dev_info = list_entry(iterator, app_bt_le_audio_bass_scan_dev_info_t, list);
			if (p_bscan_dev_info == NULL) {
				continue;
			}

			bass_scan_dev_num ++ ;
		}
	}
	osif_mutex_give(pmtx);
	return bass_scan_dev_num;
}

app_bt_le_audio_bass_scan_dev_info_t *app_bt_le_audio_bass_scan_dev_list_find(rtk_bt_le_addr_t adv_addr)
{
	app_bt_le_audio_bass_scan_dev_info_t *p_bscan_dev_info = NULL, *p_bscan_dev_info_tmp = NULL;
	struct list_head *phead = &g_app_lea_bass_scan_dev_list_head;
	void *pmtx = g_app_lea_bass_scan_dev_list_mtx;
	struct list_head *iterator = NULL;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return fail: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_bscan_dev_info_tmp = list_entry(iterator, app_bt_le_audio_bass_scan_dev_info_t, list);
			if (p_bscan_dev_info_tmp == NULL) {
				continue;
			}

			if (0 == memcmp(&p_bscan_dev_info_tmp->adv_addr, &adv_addr, sizeof(rtk_bt_le_addr_t))) {
				p_bscan_dev_info = p_bscan_dev_info_tmp;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return p_bscan_dev_info;
}

app_bt_le_audio_bass_scan_dev_info_t *app_bt_le_audio_bass_scan_dev_list_find_by_idx(uint8_t bscan_dev_idx)
{
	app_bt_le_audio_bass_scan_dev_info_t *p_bscan_dev_info = NULL, *p_bscan_dev_info_tmp = NULL;
	struct list_head *phead = &g_app_lea_bass_scan_dev_list_head;
	void *pmtx = g_app_lea_bass_scan_dev_list_mtx;
	struct list_head *iterator = NULL;
	uint8_t i = 0;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return fail: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_bscan_dev_info_tmp = list_entry(iterator, app_bt_le_audio_bass_scan_dev_info_t, list);
			if (p_bscan_dev_info_tmp == NULL) {
				continue;
			}

			if (i++ == bscan_dev_idx) {
				p_bscan_dev_info = p_bscan_dev_info_tmp;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return p_bscan_dev_info;
}

app_bt_le_audio_bass_scan_dev_info_t *app_bt_le_audio_bass_scan_dev_list_add(rtk_bt_le_addr_t adv_addr)
{
	app_bt_le_audio_bass_scan_dev_info_t *p_bscan_dev_info = NULL;
	struct list_head *phead = &g_app_lea_bass_scan_dev_list_head;
	void *pmtx = g_app_lea_bass_scan_dev_list_mtx;
	char addr_str[32] = {0};
	rtk_bt_le_addr_to_str(&adv_addr, addr_str, sizeof(addr_str));

	//check if the scan_dev in scan_dev list
	p_bscan_dev_info = app_bt_le_audio_bass_scan_dev_list_find(adv_addr);
	if (p_bscan_dev_info) {
		BT_LOGD("[APP] %s addr_str(%s) alreay in scan_dev_list , skip add action\r\n", __func__, addr_str);
		return p_bscan_dev_info;
	}

	//the new one
	p_bscan_dev_info = (app_bt_le_audio_bass_scan_dev_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(app_bt_le_audio_bass_scan_dev_info_t));
	if (p_bscan_dev_info == NULL) {
		BT_LOGE("[APP] %s osif_mem_alloc (len=%d) fail\r\n", __func__, sizeof(app_bt_le_audio_bass_scan_dev_info_t));
		return NULL;
	}
	memset(p_bscan_dev_info, 0, sizeof(app_bt_le_audio_bass_scan_dev_info_t));
	memcpy(&p_bscan_dev_info->adv_addr, &adv_addr, sizeof(rtk_bt_le_addr_t));

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_add_tail(&p_bscan_dev_info->list, phead);/* insert list */
	osif_mutex_give(pmtx);

	BT_LOGD("[APP] add addr_str(%s) in bass_scan_dev list ok\r\n", addr_str);

	return p_bscan_dev_info;

}

uint16_t app_bt_le_audio_bass_scan_dev_list_remove(rtk_bt_le_addr_t adv_addr)
{
	app_bt_le_audio_bass_scan_dev_info_t *p_bscan_dev_info = NULL;
	void *pmtx = g_app_lea_bass_scan_dev_list_mtx;
	char addr_str[32] = {0};
	rtk_bt_le_addr_to_str(&adv_addr, addr_str, sizeof(addr_str));

	p_bscan_dev_info = app_bt_le_audio_bass_scan_dev_list_find(adv_addr);
	if (p_bscan_dev_info == NULL) {
		BT_LOGD("[APP] %s addr_str(%s) not find, skip remove action\r\n", __func__, addr_str);
		return RTK_BT_OK;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_del_init(&p_bscan_dev_info->list);
	osif_mutex_give(pmtx);
	memset(p_bscan_dev_info, 0, sizeof(app_bt_le_audio_bass_scan_dev_info_t));
	osif_mem_free(p_bscan_dev_info);

	BT_LOGD("[APP] remove addr_str(%s) from scan_dev list ok\r\n", addr_str);

	return RTK_BT_OK;
}

uint16_t app_bt_le_audio_bass_scan_dev_list_remove_all(void)
{
	app_bt_le_audio_bass_scan_dev_info_t *p_bscan_dev_info = NULL;
	struct list_head *phead = &g_app_lea_bass_scan_dev_list_head;
	void *pmtx = g_app_lea_bass_scan_dev_list_mtx;
	struct list_head *plist = NULL, *pnext = NULL;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	//delete and free in scan_dev list
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_bscan_dev_info = (app_bt_le_audio_bass_scan_dev_info_t *)plist;
		if (p_bscan_dev_info) {
			app_bt_le_audio_bass_scan_dev_list_remove(p_bscan_dev_info->adv_addr);
		}

		plist = pnext;
	}

	return RTK_BT_OK;
}

uint8_t app_bt_le_audio_sync_dev_list_get_num(void)
{
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	struct list_head *iterator = NULL;
	struct list_head *phead = &g_app_lea_sync_dev_list_head;
	void *pmtx = g_app_lea_sync_dev_list_mtx;
	uint8_t bass_sync_dev_num = 0;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_sync_dev_info = list_entry(iterator, app_bt_le_audio_sync_dev_info_t, list);
			if (p_sync_dev_info == NULL) {
				continue;
			}

			bass_sync_dev_num ++ ;
		}
	}
	osif_mutex_give(pmtx);
	return bass_sync_dev_num;
}

app_bt_le_audio_sync_dev_info_t *app_bt_le_audio_sync_dev_list_find(rtk_bt_le_audio_sync_handle_t sync_handle)
{
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL, *p_sync_handle_info_tmp = NULL;
	struct list_head *phead = &g_app_lea_sync_dev_list_head;
	void *pmtx = g_app_lea_sync_dev_list_mtx;
	struct list_head *iterator = NULL;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return fail: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_sync_handle_info_tmp = list_entry(iterator, app_bt_le_audio_sync_dev_info_t, list);
			if (p_sync_handle_info_tmp == NULL) {
				continue;
			}

			if (p_sync_handle_info_tmp->sync_handle == sync_handle) {
				p_sync_dev_info = p_sync_handle_info_tmp;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return p_sync_dev_info;
}

app_bt_le_audio_sync_dev_info_t *app_bt_le_audio_sync_dev_list_find_by_idx(uint8_t sync_handle_idx)
{
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL, *p_sync_handle_info_tmp = NULL;
	struct list_head *phead = &g_app_lea_sync_dev_list_head;
	void *pmtx = g_app_lea_sync_dev_list_mtx;
	struct list_head *iterator = NULL;
	uint8_t i = 0;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return fail: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_sync_handle_info_tmp = list_entry(iterator, app_bt_le_audio_sync_dev_info_t, list);
			if (p_sync_handle_info_tmp == NULL || p_sync_handle_info_tmp->sync_handle == NULL) {
				continue;
			}

			if (i++ == sync_handle_idx) {
				p_sync_dev_info = p_sync_handle_info_tmp;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return p_sync_dev_info;
}
app_bt_le_audio_sync_dev_info_t *app_bt_le_audio_sync_dev_list_find_by_addr(rtk_bt_le_addr_t source_adv_addr)
{
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL, *p_sync_handle_info_tmp = NULL;
	struct list_head *phead = &g_app_lea_sync_dev_list_head;
	void *pmtx = g_app_lea_sync_dev_list_mtx;
	struct list_head *iterator = NULL;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return fail: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_sync_handle_info_tmp = list_entry(iterator, app_bt_le_audio_sync_dev_info_t, list);
			if (p_sync_handle_info_tmp == NULL || p_sync_handle_info_tmp->sync_handle == NULL) {
				continue;
			}

			if (0 == memcmp(&p_sync_handle_info_tmp->adv_addr, &source_adv_addr, sizeof(rtk_bt_le_addr_t))) {
				p_sync_dev_info = p_sync_handle_info_tmp;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return p_sync_dev_info;
}

app_bt_le_audio_sync_dev_info_t *app_bt_le_audio_sync_dev_list_find_by_source_id(uint8_t source_id)
{
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL, *p_sync_handle_info_tmp = NULL;
	struct list_head *phead = &g_app_lea_sync_dev_list_head;
	void *pmtx = g_app_lea_sync_dev_list_mtx;
	struct list_head *iterator = NULL;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return fail: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return NULL;
	}

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	if (!list_empty(phead)) {
		list_for_each(iterator, phead) {
			p_sync_handle_info_tmp = list_entry(iterator, app_bt_le_audio_sync_dev_info_t, list);
			if (p_sync_handle_info_tmp == NULL || p_sync_handle_info_tmp->sync_handle == NULL) {
				continue;
			}

			if (p_sync_handle_info_tmp->source_id == source_id) {
				p_sync_dev_info = p_sync_handle_info_tmp;
				break;
			}
		}
	}
	osif_mutex_give(pmtx);

	return p_sync_dev_info;
}

app_bt_le_audio_sync_dev_info_t *app_bt_le_audio_sync_dev_list_add(rtk_bt_le_audio_sync_handle_t sync_handle)
{
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	struct list_head *phead = &g_app_lea_sync_dev_list_head;
	void *pmtx = g_app_lea_sync_dev_list_mtx;

	if (pmtx == NULL) {
		BT_LOGE("[APP] %s: g_app_lea_sync_dev_list_mtx not init!\r\n", __func__);
		return NULL;
	}

	//check if the sync_handle in sync_handle list
	p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(sync_handle);
	if (p_sync_dev_info) {
		BT_LOGD("[APP] %s sync_handle(%08x) alreay in sync_handle_list , skip add action\r\n", __func__, sync_handle);
		return p_sync_dev_info;
	}

	//the new one
	p_sync_dev_info = (app_bt_le_audio_sync_dev_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(app_bt_le_audio_sync_dev_info_t));
	if (p_sync_dev_info == NULL) {
		BT_LOGE("[APP] %s osif_mem_alloc (len=%d) fail\r\n", __func__, sizeof(app_bt_le_audio_sync_dev_info_t));
		return NULL;
	}
	memset(p_sync_dev_info, 0, sizeof(app_bt_le_audio_sync_dev_info_t));
	p_sync_dev_info->sync_handle = sync_handle;

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_add_tail(&p_sync_dev_info->list, phead);/* insert list */
	osif_mutex_give(pmtx);

	BT_LOGD("[APP] add sync_handle(%08x) in sync_handle list ok\r\n", sync_handle);

	return p_sync_dev_info;
}

uint16_t app_bt_le_audio_sync_dev_list_remove(rtk_bt_le_audio_sync_handle_t sync_handle)
{
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	void *pmtx = g_app_lea_sync_dev_list_mtx;

	p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(sync_handle);
	if (p_sync_dev_info == NULL) {
		BT_LOGD("[APP] %s sync_handle(%08x) not find, skip remove action\r\n", __func__, sync_handle);
		return RTK_BT_OK;
	}

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	list_del_init(&p_sync_dev_info->list);
	osif_mutex_give(pmtx);

	memset(p_sync_dev_info, 0, sizeof(app_bt_le_audio_sync_dev_info_t));
	osif_mem_free(p_sync_dev_info);

	BT_LOGD("[APP] remove sync_handle(%08x) from sync_handle list ok\r\n", sync_handle);

	return RTK_BT_OK;
}

uint16_t app_bt_le_audio_sync_dev_list_remove_all(void)
{
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	struct list_head *phead = &g_app_lea_sync_dev_list_head;
	void *pmtx = g_app_lea_sync_dev_list_mtx;
	struct list_head *plist = NULL, *pnext = NULL;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	//delete and free in sync_handle list
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_sync_dev_info = (app_bt_le_audio_sync_dev_info_t *)plist;
		if (p_sync_dev_info) {
			app_bt_le_audio_sync_dev_list_remove(p_sync_dev_info->sync_handle);
		}

		plist = pnext;
	}

	return RTK_BT_OK;
}

void app_bt_le_audio_scan_dev_list_init(void)
{
	INIT_LIST_HEAD(&g_app_lea_scan_dev_list_head);

	if (g_app_lea_scan_dev_list_mtx == NULL) {
		osif_mutex_create(&g_app_lea_scan_dev_list_mtx);
	}
}

void app_bt_le_audio_scan_dev_list_deinit(void)
{
	app_bt_le_audio_scan_dev_list_remove_all();

	INIT_LIST_HEAD(&g_app_lea_scan_dev_list_head);

	if (g_app_lea_scan_dev_list_mtx != NULL) {
		osif_mutex_delete(g_app_lea_scan_dev_list_mtx);
		g_app_lea_scan_dev_list_mtx = NULL;
	}
}

void app_bt_le_audio_broadcast_dev_list_init(void)
{
	INIT_LIST_HEAD(&g_app_lea_bass_scan_dev_list_head);

	if (g_app_lea_bass_scan_dev_list_mtx == NULL) {
		osif_mutex_create(&g_app_lea_bass_scan_dev_list_mtx);
	}

	INIT_LIST_HEAD(&g_app_lea_sync_dev_list_head);

	if (g_app_lea_sync_dev_list_mtx == NULL) {
		osif_mutex_create(&g_app_lea_sync_dev_list_mtx);
	}
}
void app_bt_le_audio_broadcast_dev_list_deinit(void)
{
	app_bt_le_audio_bass_scan_dev_list_remove_all();
	INIT_LIST_HEAD(&g_app_lea_bass_scan_dev_list_head);

	if (g_app_lea_bass_scan_dev_list_mtx != NULL) {
		osif_mutex_delete(g_app_lea_bass_scan_dev_list_mtx);
		g_app_lea_bass_scan_dev_list_mtx = NULL;
	}

	app_bt_le_audio_sync_dev_list_remove_all();
	INIT_LIST_HEAD(&g_app_lea_sync_dev_list_head);

	if (g_app_lea_sync_dev_list_mtx != NULL) {
		osif_mutex_delete(g_app_lea_sync_dev_list_mtx);
		g_app_lea_sync_dev_list_mtx = NULL;
	}
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
				if (length <= (RTK_BLE_EXT_ADV_MAX_LOCAL_NAME_LENGTH + 1)) {
					memcpy((void *)p_scan_dev_info->local_name, (void *)p_buffer, length - 1);
				}
			}
			break;

			case RTK_BT_LE_GAP_ADTYPE_SERVICE_DATA: {

				uint16_t uuid = ((uint16_t)(*p_buffer) << 0) + ((uint16_t)(*(p_buffer + 1)) << 8);
				p_buffer += 2;

				if (uuid == RTK_BT_LE_GATT_UUID_ASCS) {
					p_scan_dev_info->adv_data_flags |= RTK_BLE_AUDIO_ADV_DATA_ASCS_BIT;

					p_scan_dev_info->ascs_announcement_type = (uint8_t)(*p_buffer);
					p_buffer += 1;
					p_scan_dev_info->ascs_sink_available_contexts = ((uint16_t)(*p_buffer) << 0) + ((uint16_t)(*(p_buffer + 1)) << 8);
					p_buffer += 2;
					p_scan_dev_info->ascs_source_available_contexts = ((uint16_t)(*p_buffer) << 0) + ((uint16_t)(*(p_buffer + 1)) << 8);
					p_buffer += 2;
				} else if (uuid == RTK_BT_LE_GATT_UUID_CAS) {
					p_scan_dev_info->adv_data_flags |= RTK_BLE_AUDIO_ADV_DATA_CAP_BIT;

					p_scan_dev_info->cap_announcement_type = (uint8_t)(*p_buffer);
					p_buffer += 1;
				} else if (uuid == RTK_BT_LE_GATT_UUID_BASS) {
					p_scan_dev_info->adv_data_flags |= RTK_BLE_AUDIO_ADV_DATA_BASS_BIT;
				}
			}
			break;

			case RTK_BT_LE_GAP_ADTYPE_RSI: {
				p_scan_dev_info->adv_data_flags |= RTK_BLE_AUDIO_ADV_DATA_RSI_BIT;
			}
			break;

			default:
				break;
			}
		}
		pos += length;
	}
}

uint16_t app_bt_le_audio_scan_report_handle(rtk_bt_le_ext_scan_res_ind_t *scan_res_ind)
{
	app_bt_le_audio_scan_dev_info_t scan_dev_info = {0};

	if (NULL == app_bt_le_audio_scan_dev_list_find(scan_res_ind->addr)) {
		app_bt_le_audio_scan_parse_report(scan_res_ind->len, scan_res_ind->data, &scan_dev_info);
		if (scan_dev_info.adv_data_flags) {
			memcpy(&scan_dev_info.scan_res_ind, scan_res_ind, sizeof(rtk_bt_le_ext_scan_res_ind_t));
			if (app_bt_le_audio_scan_dev_list_add(&scan_dev_info)) {
				BT_LOGE("[APP] add device(%x %x %x %x %x %x >> Name %s) in scan_dev list fail \r\n",
						scan_res_ind->addr.addr_val[5], scan_res_ind->addr.addr_val[4], scan_res_ind->addr.addr_val[3],
						scan_res_ind->addr.addr_val[2], scan_res_ind->addr.addr_val[1], scan_res_ind->addr.addr_val[0],
						scan_dev_info.local_name);
			} else {
				BT_LOGA("[APP] add device(%x %x %x %x %x %x >> Name %s) in scan_dev list \r\n",
						scan_res_ind->addr.addr_val[5], scan_res_ind->addr.addr_val[4], scan_res_ind->addr.addr_val[3],
						scan_res_ind->addr.addr_val[2], scan_res_ind->addr.addr_val[1], scan_res_ind->addr.addr_val[0],
						scan_dev_info.local_name);
				BT_AT_PRINT("+BLEBAP:unicast,client,escan,%02x:%02x:%02x:%02x:%02x:%02x,%s\r\n",
							scan_res_ind->addr.addr_val[5], scan_res_ind->addr.addr_val[4], scan_res_ind->addr.addr_val[3],
							scan_res_ind->addr.addr_val[2], scan_res_ind->addr.addr_val[1], scan_res_ind->addr.addr_val[0],
							scan_dev_info.local_name);

			}
		}
	}
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
	if (csip_discover_flag) {
		if (scan_dev_info.adv_data_flags & RTK_BLE_AUDIO_ADV_DATA_RSI_BIT) {
			if (RTK_BT_ERR_LOWER_STACK_API == rtk_bt_le_audio_csis_set_coordinator_check_adv_rsi(scan_res_ind->addr, scan_res_ind->len, scan_res_ind->data)) {
				BT_LOGE("[APP] %s: The scanned device is not in the same coordinator set\r\n", __func__);
				app_bt_le_audio_scan_dev_list_remove(scan_res_ind->addr);
			}
		}
	}
#endif
	return RTK_BT_OK;
}

uint16_t app_bt_le_audio_scan_report_show(void)
{
	app_bt_le_audio_scan_dev_info_t *p_scan_dev_info = NULL;
	struct list_head *phead = &g_app_lea_scan_dev_list_head;
	void *pmtx = g_app_lea_scan_dev_list_mtx;
	struct list_head *plist = NULL, *pnext = NULL;
	uint8_t scan_dev_num = 0;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	BT_LOGA("[APP] show scan device list: \r\n");

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_scan_dev_info = (app_bt_le_audio_scan_dev_info_t *)plist;
		if (p_scan_dev_info) {
			BT_LOGA("[APP] RemoteBd[%d] = [%02x:%02x:%02x:%02x:%02x:%02x], type %d\r\n", scan_dev_num,
					p_scan_dev_info->scan_res_ind.addr.addr_val[5], p_scan_dev_info->scan_res_ind.addr.addr_val[4], p_scan_dev_info->scan_res_ind.addr.addr_val[3],
					p_scan_dev_info->scan_res_ind.addr.addr_val[2], p_scan_dev_info->scan_res_ind.addr.addr_val[1], p_scan_dev_info->scan_res_ind.addr.addr_val[0],
					p_scan_dev_info->scan_res_ind.addr.type);
			scan_dev_num++;
		}

		plist = pnext;
	}
	osif_mutex_give(pmtx);
	BT_LOGA("[APP] scan device list total num: %d\r\n", scan_dev_num);
	return RTK_BT_OK;
}

uint16_t app_bt_le_audio_bass_scan_report_handle(rtk_bt_le_ext_scan_res_ind_t *scan_res_ind)
{
	uint8_t *p_service_data = NULL;
	uint16_t service_data_len = 0;
	app_bt_le_audio_bass_scan_dev_info_t bscan_dev_info = {0}, *p_bscan_dev_info = NULL;
	char addr_str[32] = {0};

	if (app_bt_le_audio_adv_filter_service_data(scan_res_ind->len,
												scan_res_ind->data,
												RTK_BT_LE_BROADCAST_AUDIO_ANNOUNCEMENT_SRV_UUID,
												&p_service_data,
												&service_data_len)) {
		bscan_dev_info.adv_sid = scan_res_ind->adv_sid;
		memcpy(&bscan_dev_info.adv_addr, &scan_res_ind->addr, sizeof(rtk_bt_le_addr_t));
		memcpy(bscan_dev_info.broadcast_id, p_service_data, service_data_len);
		p_bscan_dev_info = app_bt_le_audio_bass_scan_dev_list_find(bscan_dev_info.adv_addr);
		if (!p_bscan_dev_info) {
			p_bscan_dev_info = app_bt_le_audio_bass_scan_dev_list_add(bscan_dev_info.adv_addr);
			if (!p_bscan_dev_info) {
				BT_LOGE("[APP] broadcast scan find new device (addr: %s, broadcast_id: %x %x %x), but add fail! \r\n", addr_str, bscan_dev_info.broadcast_id[0],
						bscan_dev_info.broadcast_id[1], bscan_dev_info.broadcast_id[2]);
				return RTK_BT_OK;
			}
		} else {
			/* check parameters */
			if (p_bscan_dev_info->adv_sid == bscan_dev_info.adv_sid && \
				!memcmp((void *)p_bscan_dev_info->broadcast_id, (void *)&bscan_dev_info.broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN)) {
				p_bscan_dev_info = NULL;
			}
		}
		if (p_bscan_dev_info) {
			p_bscan_dev_info->adv_sid = bscan_dev_info.adv_sid;
			memcpy(p_bscan_dev_info->broadcast_id, bscan_dev_info.broadcast_id, RTK_BT_LE_AUDIO_BROADCAST_ID_LEN);
			rtk_bt_le_addr_to_str(&scan_res_ind->addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] broadcast scan add new device in list (addr: %s, adv_sid %x, broadcast_id: %x %x %x) \r\n",
					addr_str,
					p_bscan_dev_info->adv_sid,
					bscan_dev_info.broadcast_id[0],
					bscan_dev_info.broadcast_id[1],
					bscan_dev_info.broadcast_id[2]);
			BT_AT_PRINT("+BLEBAP:broadcast,sink,escan,%s,%x,%x %x %x\r\n",
						addr_str,
						p_bscan_dev_info->adv_sid,
						bscan_dev_info.broadcast_id[0],
						bscan_dev_info.broadcast_id[1],
						bscan_dev_info.broadcast_id[2]);
		}
		return RTK_BT_OK;
	}

	return RTK_BT_ERR_MISMATCH;
}

uint16_t app_bt_le_audio_bass_scan_report_show(void)
{
	app_bt_le_audio_bass_scan_dev_info_t *p_bscan_dev_info = NULL;
	struct list_head *phead = &g_app_lea_bass_scan_dev_list_head;
	void *pmtx = g_app_lea_bass_scan_dev_list_mtx;
	struct list_head *plist = NULL, *pnext = NULL;
	uint8_t bscan_dev_num = 0;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	BT_LOGA("[APP] show bass scan device list: \r\n");

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_bscan_dev_info = (app_bt_le_audio_bass_scan_dev_info_t *)plist;
		if (p_bscan_dev_info) {
			BT_LOGA("[APP] RemoteBd[%d] = [%02x:%02x:%02x:%02x:%02x:%02x], type %d, broadcast_id [%02x:%02x:%02x]\r\n", bscan_dev_num,
					p_bscan_dev_info->adv_addr.addr_val[5], p_bscan_dev_info->adv_addr.addr_val[4], p_bscan_dev_info->adv_addr.addr_val[3],
					p_bscan_dev_info->adv_addr.addr_val[2], p_bscan_dev_info->adv_addr.addr_val[1], p_bscan_dev_info->adv_addr.addr_val[0],
					p_bscan_dev_info->adv_addr.type,
					p_bscan_dev_info->broadcast_id[0], p_bscan_dev_info->broadcast_id[1], p_bscan_dev_info->broadcast_id[2]);
			bscan_dev_num++;
		}
		plist = pnext;
	}
	osif_mutex_give(pmtx);
	BT_LOGA("[APP] bass scan device list total num: %d\r\n", bscan_dev_num);
	return RTK_BT_OK;
}

uint16_t app_bt_le_audio_bass_pa_show(void)
{
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	struct list_head *phead = &g_app_lea_sync_dev_list_head;
	void *pmtx = g_app_lea_sync_dev_list_mtx;
	struct list_head *plist = NULL, *pnext = NULL;
	uint8_t sync_dev_num = 0;

	if (phead == NULL || pmtx == NULL) {
		BT_LOGE("[APP] %s return: phead=%08x,pmtx=%08x\r\n", __func__, phead, pmtx);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	BT_LOGA("[APP] show BAAS sync device list: \r\n");

	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	plist = phead->next;
	while (plist != phead) {
		pnext = plist->next;
		p_sync_dev_info = (app_bt_le_audio_sync_dev_info_t *)plist;
		if (p_sync_dev_info) {
			BT_LOGA("[APP] sync dev[%d] : [%02x:%02x:%02x:%02x:%02x:%02x], type %d, source_id %d, pa_sync_state %d, big_sync_state %d\r\n", sync_dev_num,
					p_sync_dev_info->adv_addr.addr_val[5], p_sync_dev_info->adv_addr.addr_val[4], p_sync_dev_info->adv_addr.addr_val[3],
					p_sync_dev_info->adv_addr.addr_val[2], p_sync_dev_info->adv_addr.addr_val[1], p_sync_dev_info->adv_addr.addr_val[0],
					p_sync_dev_info->adv_addr.type, p_sync_dev_info->source_id, p_sync_dev_info->pa_sync_state, p_sync_dev_info->big_sync_state);

			sync_dev_num++;
		}
		plist = pnext;
	}
	osif_mutex_give(pmtx);
	BT_LOGA("[APP] sync device list total num: %d\r\n", sync_dev_num);

	return RTK_BT_OK;
}

rtk_bt_audio_track_t *app_bt_le_audio_track_add(rtk_bt_le_audio_cfg_codec_t *p_codec)
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
		left_volume = DEFAULT_AUDIO_TRACK_LEFT_VOLUME;
	}
	if (audio_chnl & RTK_BT_LE_AUDIO_LOCATION_FR) {
		right_volume = DEFAULT_AUDIO_TRACK_RIGHT_VOLUME;
	}
#else
	channels = 2;//default init two channel each track
	left_volume = DEFAULT_AUDIO_TRACK_LEFT_VOLUME;
	right_volume = DEFAULT_AUDIO_TRACK_RIGHT_VOLUME;
#endif

	return rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_LC3, (float)left_volume, (float)right_volume,
								  channels, rate, BT_AUDIO_FORMAT_PCM_16_BIT, duration, NULL, true);
}

uint16_t app_bt_le_audio_track_remove(void *audio_track_hdl)
{
	return rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_LC3, audio_track_hdl);
}

rtk_bt_audio_record_t *app_bt_le_audio_record_add(rtk_bt_le_audio_cfg_codec_t *p_codec)
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

	leaudio_record_hdl = rtk_bt_audio_record_add(RTK_BT_AUDIO_CODEC_LC3, channels, rate, buffer_bytes);
	if (!leaudio_record_hdl) {
		BT_LOGA("[APP] %s failed ! delete record\r\n", __func__);
		rtk_bt_audio_record_del(RTK_BT_AUDIO_CODEC_LC3, leaudio_record_hdl);
		leaudio_record_hdl = NULL;
	}
	// // set default mic bst gain 20DB
	// if (rtk_bt_audio_record_set_mic_bst_gain(RTK_BT_AUDIO_AMIC1, RTK_BT_AUDIO_MICBST_GAIN_20DB)) {
	//     BT_LOGA("[APP] %s set le audio default mic bst gain 20DB success !\r\n", __func__);
	// }

	return leaudio_record_hdl;

}

uint16_t app_bt_le_audio_record_remove(void *audio_record_hdl)
{
	return rtk_bt_audio_record_del(RTK_BT_AUDIO_CODEC_LC3, audio_record_hdl);
}

void *app_bt_le_audio_lc3_codec_entity_add(rtk_bt_le_audio_cfg_codec_t *p_codec)
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
	BT_LOGA("[APP] %s audio_channel_allocation=0x%x, sample_frequency=0x%x\r\n", __func__, (unsigned int)lc3_codec_cfg.audio_channel_allocation,
			lc3_codec_cfg.sample_frequency);

	audio_codec_conf.codec_index = RTK_BT_AUDIO_CODEC_LC3;
	audio_codec_conf.param = (void *)&lc3_codec_cfg;
	audio_codec_conf.param_len = sizeof(lc3_codec_cfg);
	return rtk_bt_audio_codec_add(&audio_codec_conf);
}

uint16_t app_bt_le_audio_lc3_codec_entity_remove(void *codec_entity)
{
	return rtk_bt_audio_codec_remove(RTK_BT_AUDIO_CODEC_LC3, codec_entity);
}

bool app_bt_le_audio_broadcast_source_get_codec_from_level2(uint8_t *p_codec, uint8_t codec_len, rtk_bt_le_audio_cfg_codec_t *p_bis_codec_cfg)
{
	uint8_t pos = 0, *p_buffer = NULL;
	uint8_t length = 0;
	uint8_t type = 0;

	BT_DUMPD(__func__, p_codec, codec_len);

	p_buffer = p_codec;
	pos = 0;
	while (pos < codec_len) {
		/* Codec_Specific_Capabilities TLV length, one octet. */
		length = (uint8_t) * (p_buffer + pos);
		/* Codec_Specific_Capabilities TLV Type, one octet. */
		type = (uint8_t) * (p_buffer + pos + 1);
		switch (type) {
		case RTK_BT_LE_CODEC_CFG_TYPE_SAMPLING_FREQUENCY: {
			p_bis_codec_cfg->sample_frequency = (uint8_t) * (p_buffer + pos + 2);
		}
		break;
		case RTK_BT_LE_CODEC_CFG_TYPE_FRAME_DURATION: {
			p_bis_codec_cfg->frame_duration = (uint8_t) * (p_buffer + pos + 2);
		}
		break;
		case RTK_BT_LE_CODEC_CFG_TYPE_OCTET_PER_CODEC_FRAME: {
			p_bis_codec_cfg->octets_per_codec_frame = (((uint16_t) * (p_buffer + pos + 2)) << 0) + (((uint16_t) * (p_buffer + pos + 3)) << 8);
		}
		break;
		case RTK_BT_LE_CODEC_CFG_TYPE_BLOCKS_PER_SDU: {
			p_bis_codec_cfg->codec_frame_blocks_per_sdu = (uint8_t) * (p_buffer + pos + 2);
		}
		break;
		default:
			break;
		}
		pos += (length + 1);
	}

	return false;
}

bool app_bt_le_audio_broadcast_source_set_codec_to_level2(uint8_t *p_codec, uint8_t codec_len, uint8_t type, uint16_t value)
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

uint16_t app_bt_le_audio_bass_brs_char_init(app_bt_le_audio_device_info_t *p_device_info, uint8_t brs_char_num)
{
	uint16_t ret = RTK_BT_OK, size = 0;
	rtk_bt_le_audio_bass_brs_data_t brs_data = {0};
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;

	if (p_device_info->brs_char_tbl) {
		osif_mem_free(p_device_info->brs_char_tbl);
		p_device_info->brs_char_tbl = NULL;
		p_device_info->brs_char_num = 0;
	}
	if (brs_char_num) {
		size = brs_char_num * sizeof(app_bt_le_audio_bass_brs_char_info_t);
		p_device_info->brs_char_tbl = (app_bt_le_audio_bass_brs_char_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, size);
		if (p_device_info->brs_char_tbl == NULL) {
			BT_LOGE("[APP] %s osif_mem_alloc fail, size=%d\r\n", __func__, size);
			return RTK_BT_ERR_NO_MEMORY;
		}
		memset(p_device_info->brs_char_tbl, 0, size);
		p_device_info->brs_char_num = brs_char_num;
		for (uint8_t i = 0; i < brs_char_num; i++) {
			p_device_info->brs_char_tbl[i].instance_id = i;
			ret = rtk_bt_le_audio_bass_get_brs_data(p_device_info->conn_handle, i, false, &brs_data);
			if (ret != RTK_BT_OK) {
				BT_LOGE("[APP] %s get_brs_data fail (ret 0x%x) for instance_id %d\n", __func__, i, ret);
				continue;
			}
			if (brs_data.brs_is_used) {
				p_device_info->brs_char_tbl[i].brs_is_used = true;
				p_device_info->brs_char_tbl[i].source_id = brs_data.source_id;
				p_sync_dev_info = app_bt_le_audio_sync_dev_list_find_by_addr(brs_data.source_address);
				if (p_sync_dev_info == NULL) {
					BT_LOGE("[APP] %s bass_sync_dev_list_find_by_addr fail for instance_id %d\n", __func__, i);
					continue;
				}
				p_device_info->brs_char_tbl[i].sync_handle = p_sync_dev_info->sync_handle;

				if (brs_data.bis_info_size) {
					brs_data.p_cp_bis_info = osif_mem_alloc(RAM_TYPE_DATA_ON, brs_data.bis_info_size);
					if (!brs_data.p_cp_bis_info) {
						BT_LOGE("[APP] %s osif_mem_alloc (len=%d) fail\r\n", __func__, brs_data.bis_info_size);
						continue;
					}
					ret = rtk_bt_le_audio_bass_get_brs_data(p_device_info->conn_handle, i, true, &brs_data);
					if (ret != RTK_BT_OK) {
						BT_LOGE("[APP] %s get_brs_data fail (ret 0x%x) for instance_id %d\n", __func__, i, ret);
						continue;
					}
					if (brs_data.num_subgroups) {
						BT_LOGD("[APP] rtk_bt_le_audio_bass_get_brs_data num_subgroups %d, bis_info_size %d, bis_sync_state 0x%x, metadata_len %d\r\n",
								brs_data.num_subgroups, brs_data.bis_info_size, (unsigned int)brs_data.p_cp_bis_info[0].bis_sync_state,
								brs_data.p_cp_bis_info[0].metadata_len);
						BT_DUMPD("", brs_data.p_cp_bis_info[0].p_metadata, brs_data.p_cp_bis_info[0].metadata_len);
					}
				}
			}
		}
	}

	return RTK_BT_OK;
}

uint16_t app_bt_le_audio_bass_brs_char_update(app_bt_le_audio_device_info_t *p_device_info, uint8_t char_instance_id,
											  rtk_bt_le_audio_bass_brs_data_t *p_brs_data)
{
	uint8_t i = 0;
	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;

	if (p_device_info == NULL || p_brs_data == NULL) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	for (i = 0; i < p_device_info->brs_char_num; i++) {
		if (p_device_info->brs_char_tbl[i].instance_id == char_instance_id) {
			break;
		}
	}
	if (i == p_device_info->brs_char_num) {
		BT_LOGE("[APP] %s not find brs_char_tbl for char_instance_id %d\r\n", __func__, char_instance_id);
		return RTK_BT_ERR_NO_ENTRY;
	}
	if (p_device_info->brs_char_tbl[i].brs_is_used != p_brs_data->brs_is_used) {
		if (p_brs_data->brs_is_used) {
			p_device_info->brs_char_tbl[i].brs_is_used = true;
			p_device_info->brs_char_tbl[i].source_id = p_brs_data->source_id;
			p_sync_dev_info = app_bt_le_audio_sync_dev_list_find_by_addr(p_brs_data->source_address);
			if (p_sync_dev_info == NULL) {
				BT_LOGE("[APP] %s bass_sync_dev_list_find_by_addr fail for char_instance_id %d\n", __func__, char_instance_id);
				return RTK_BT_ERR_NO_ENTRY;
			}
			p_device_info->brs_char_tbl[i].sync_handle = p_sync_dev_info->sync_handle;
		} else {
			p_device_info->brs_char_tbl[i].brs_is_used = false;
			p_device_info->brs_char_tbl[i].source_id = 0;
			p_device_info->brs_char_tbl[i].sync_handle = NULL;
			BT_LOGA("[APP] %s p_brs_data->brs_is_used %d,so clear the brs_char_tbl \r\n", __func__, p_brs_data->brs_is_used);
		}
	}

	return RTK_BT_OK;
}
#endif
