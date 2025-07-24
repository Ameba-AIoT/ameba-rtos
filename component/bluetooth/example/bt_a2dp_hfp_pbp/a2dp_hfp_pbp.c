/*
*******************************************************************************
* Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

/* -------------------------------- Includes -------------------------------- */
#include <stdio.h>
#include <string.h>
#include <bt_api_config.h>
#include <osif.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_le_audio_def.h>
#include <rtk_bt_bap.h>
#include <rtk_bt_cap.h>
#include <rtk_bt_def.h>
#include <rtk_bt_br_gap.h>
#include <rtk_bt_sdp.h>
#include <rtk_bt_avrcp.h>
#include <rtk_bt_a2dp.h>
#include <rtk_bt_hfp.h>
#include <rtk_bt_pbap.h>
#include <bt_audio_track_api.h>
#include <bt_audio_record_api.h>
#include <bt_audio_intf.h>
#include <bt_audio_codec_wrapper.h>
#include <lc3_codec_entity.h>
#include <sbc_codec_entity.h>
#include <cvsd_codec_entity.h>
// #include <aac_codec_entity.h>
#include "kv.h"
#include <dlist.h>
#include "bt_audio_resample.h"
#include <bt_utils.h>
#include <app_queue_mgr.h>
#include <app_audio_data.h>

#if defined(RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE) && RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE
#define APP_BT_A2DP_PBP_DEMO_OUPUT_CHANNEL_NUM          2
#else
#define APP_BT_A2DP_PBP_DEMO_OUPUT_CHANNEL_NUM          1
#endif
#if (RTK_BT_LE_AUDIO_COMPO_DEMO_AUDIO_STREAM_SAMPLE_RATE == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K)
#define APP_BT_A2DP_PBP_DEMO_OUPUT_SAMPLE_RATE          48000
#elif (RTK_BT_LE_AUDIO_COMPO_DEMO_AUDIO_STREAM_SAMPLE_RATE == RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K)
#define APP_BT_A2DP_PBP_DEMO_OUPUT_SAMPLE_RATE          16000
#else
#define APP_BT_A2DP_PBP_DEMO_OUPUT_SAMPLE_RATE          48000
#endif
#define APP_BT_LE_AUDIO_DEFAULT_SDU_INTERVAL_M_S_US     10000
#define APP_LE_AUDIO_DEMO_DATA_PATH_NUM                 2
/*
    A2DP sink will receive 595 bytes encode data per 14.5 ms from a2dp source, approximately 6.89 packets of 595 bytes per 100 milliseconds.
    When the sample rate is 44.1KHz, the above 595 bytes encode data will be parsing to 5 frames,
    which can be decoded to 512 bytes, 2560 bytes pcm data in total.
    In one time, we can get 512 bytes 44.1KHZ 2 channels pcm callback data from rtk_bt_audio_decode_pcm_data_callback.

    The A2DP sink RX rate is not constant and depended on the A2DP source.But LE Audio TX rate is constant.
    So we should leave a proper water level in app_pcm_data_mgr_queue maintain average rate.
    The water level can cause extra delay, but will not run out if set proper value.
*/
#if defined(RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE) && RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE
/* PBP broadcast source TX water level and the unit is in milliseconds. Must set an integer multiple of 10 milliseconds.*/
#define APP_LE_AUDIO_PBP_BROADCAST_TX_WATER_LEVEL_MS            500
/* Actual PBP broadcast TX water level length */
#define APP_BT_PCM_DATA_QUEUE_WATER_LEVEL                       1920 * APP_LE_AUDIO_PBP_BROADCAST_TX_WATER_LEVEL_MS / 10
/* Set enough length to store resample data.The unit is in short. 1920 bytes is equal to 48 KHz,2 channels pcm data bytes per 10 milliseconds. */
#define APP_BT_PBP_SOURCE_PCM_DATA_MAX_LEN                      1920 * APP_LE_AUDIO_PBP_BROADCAST_TX_WATER_LEVEL_MS / 10
#else
#define APP_LE_AUDIO_PBP_BROADCAST_TX_WATER_LEVEL_MS            300
#define APP_BT_PCM_DATA_QUEUE_WATER_LEVEL                       960 * APP_LE_AUDIO_PBP_BROADCAST_TX_WATER_LEVEL_MS / 10
#define APP_BT_PBP_SOURCE_PCM_DATA_MAX_LEN                      960 * APP_LE_AUDIO_PBP_BROADCAST_TX_WATER_LEVEL_MS / 10
#endif
/* Fixed length, used for temporary storage of audio data after resampled */
#define APP_RESAMPLE_OUTPUT_FRAME_BUF_MAX_LEN                   480*4*4
#define APP_BT_DEFAULT_PAGESCAN_WINDOW                          0x48
#define APP_A2DP_SINK_HFP_HF_PBP_DEVICE_NAME                    "A2DP HFP Auracast"
#define APP_BT_LE_AUDIO_BASS_SCAN_TYPE                          2
#define APP_BT_DEFAULT_PAGESCAN_INTERVAL                        0x800 //0x800
#define APP_BT_DEFAULT_PAGE_TIMEOUT                             0x2000
#define APP_BT_DEFAULT_INQUIRYSCAN_WINDOW                       0x48
#define APP_BT_DEFAULT_INQUIRYSCAN_INTERVAL                     0x800 //0x1000
#define APP_BT_DEFAULT_A2DP_PBP_AUDIO_LEFT_VOLUME               0.8
#define APP_BT_DEFAULT_A2DP_PBP_AUDIO_RIGHT_VOLUME              0.8
#define APP_BT_A2DP_SRC_STREAM_MAX_CREDITS                      2
#define APP_BT_PBP_BROADCAST_SOURCE_ENCODE_TASK_PRIO            5
#define APP_BT_PBP_BROADCAST_SOURCE_ENCODE_TASK_STACK_SIZE     (1024 * 5)
#define RTK_COMPANY_ID                                          0x005D
#define APP_BT_VOL_MAX                                          0x7f
#define APP_BT_VOL_MIN                                          0x00
#define APP_LE_AUDIO_DEFAULT_PCM_BIT_WIDTH                      16
#define APP_BT_RECONNECT_TIMER_INTERVAL                         5000
#define APP_BT_RECONNECT_COUNT                                  3
/* Max bond number is unchangable */
#define A2DP_BT_MAX_BOND_NUM                                    8
#define A2DP_BT_BOND_INFO_USED_VAL                              0x4E
#define A2DP_BT_BOND_INFO_PRIORITY_UNASSIGNED                   0x4F
#define A2DP_BT_BOND_INFO_FLUSH_EVENT                           0x01
#define A2DP_BT_BOND_INFO_THREAD_EXIT                           0xFF
/* ---------------------------- Struct definition --------------------------- */
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
struct app_demo_task_t {
	void *hdl;
	void *sem;
	uint8_t run;
};
typedef struct {
	uint16_t type;
	uint16_t subtype;
	union {
		uint32_t  param;
		void     *buf;
	} u;
} app_demo_msg_t;
typedef struct {
	uint8_t             bd_addr[6];                         /* Bluetooth address */
	uint8_t             name[RTK_BT_GAP_DEVICE_NAME_LEN];   /* name */
	uint8_t             name_contained;                     /* check whether this unit contains name */
	uint8_t             priority;                           /* bond info priority, range from 1~8, 1 is the highest */
	uint8_t             used_check;                         /* check whether this unit is configured */
} a2dp_bond_info_t;
typedef struct {
	uint16_t count;
	void *mtx;
	struct list_head head;
} app_le_audio_list_t;
typedef struct {
	struct list_head list;
	uint8_t scan_type;
	rtk_bt_le_ext_scan_res_ind_t scan_res_ind;          /**< @ref rtk_bt_le_ext_scan_res_ind_t */
	union {
		struct {
			uint8_t adv_sid;
			uint8_t broadcast_id[RTK_BT_LE_AUDIO_BROADCAST_ID_LEN];
		} bass_scan_info_t;
	};
} app_bt_le_audio_scan_dev_info_t;
/* ---------------------------- Static Variables ---------------------------- */
/* Auto reconnect config*/
static void *reconnect_timer = NULL;
static uint8_t a2dp_auto_reconnect = 1;
static uint8_t a2dp_auto_reconnect_try_count = APP_BT_RECONNECT_COUNT;
/* when sink disconnect due to out of range, this flag where be set up to do reconnecting */
static uint8_t a2dp_disconnection_reconnecting = 0;
static char a2dp_filesystem_key[] = "a2dp_reconnect_data";
/* Bond Info Manage config*/
static char a2dp_bond_info_key[] = "a2dp_bond_table";
static uint8_t a2dp_bond_num = 0;
static uint8_t a2dp_bond_info_flush = 1;
static struct app_demo_task_t bond_info_flush_task = {
	.hdl = NULL,
	.sem = NULL,
	.run = 0,
};
static void *a2dp_bond_info_flush_msg_q = NULL;
static a2dp_bond_info_t a2dp_bond_table[A2DP_BT_MAX_BOND_NUM] = {0};
/* HFP task*/
static struct app_demo_task_t hfp_task = {
	.hdl = NULL,
	.sem = NULL,
	.run  = 0,
};
/* pbp broadcast source task*/
static struct app_demo_task_t app_bt_pbp_bsrc_encode_task = {
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
static uint32_t g_pbp_bsrc_send_timer_interval_us = APP_BT_LE_AUDIO_DEFAULT_SDU_INTERVAL_M_S_US;
/*
    1.Storage 44.KHZ pcm data from a2dp decode task
    2.Send it to pbp convert task when reach a suitable water level.
*/
static short pcm_data_buf[APP_BT_PBP_SOURCE_PCM_DATA_MAX_LEN] = {0};
static app_bt_queue_t app_pcm_data_mgr_queue = {
	.q_write = 0,
	.q_read = 0,
	.mtx = NULL,
	.queue = NULL,
	.queue_size = 0,
	.queue_max_len = 0,
};
/* used to drop packet in pcm data fifo for balance A2DP rx and BIS tx */
static void *pkt_drop_mtx = NULL;
static bool pkt_drop_flag = false;
static uint16_t pkt_drop_num = 0;
/* used to indicate LE Audio thread that the pcm data has reached sufficient number */
static bool pbp_broadcast_dequeue_flag = false;
/* used for HW timer to judge whether send semaphone to wake up LE Audio TX thread */
static bool a2dp_play_flag = false;
static bool demo_init_flag = false;
/* init le audio app config */
static uint8_t bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_UNKNOWN;
static uint8_t cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_UNKNOWN;
static uint8_t bt_le_audio_sink_pac_id = 0xFF;
static uint8_t bt_le_audio_source_pac_id = 0xFF;
static app_le_audio_list_t scan_dev_queue;
static rtk_bt_le_audio_sync_handle_t big_sync_handle = NULL;
static rtk_bt_le_audio_app_conf_t bt_le_audio_app_conf = {0};
static app_bt_le_audio_data_path_t app_le_audio_data_path[APP_LE_AUDIO_DEMO_DATA_PATH_NUM] = {0};
static uint8_t curr_volume = RTK_BT_DEFAULT_ABSOLUTE_VOLUME;
static uint32_t fail_cnt;
static bool lea_broadcast_start = false;
static rtk_bt_audio_biquad_t bq_t = {0};
static rtk_bt_audio_resample_t *g_audio_resample_t = NULL;
static uint32_t resample_out_frames, resample_in_frames = 0;
static uint32_t demo_in_rate = 44100;//input sample rate
static uint32_t demo_out_rate = APP_BT_A2DP_PBP_DEMO_OUPUT_SAMPLE_RATE;//output sample rate
static int8_t out_frame_buf[APP_RESAMPLE_OUTPUT_FRAME_BUF_MAX_LEN] = {0};
static rtk_bt_audio_track_t *a2dp_audio_track_hdl = NULL;
static void *a2dp_codec_entity = NULL;
static uint8_t a2dp_role;
static uint8_t src_a2dp_credits = APP_BT_A2DP_SRC_STREAM_MAX_CREDITS;
static rtk_bt_audio_codec_conf_t audio_a2dp_codec_conf = {0};
static rtk_bt_audio_codec_conf_t audio_hfp_codec_conf = {0};
static uint8_t remote_bd_addr[6] = {0};
static uint8_t hfp_role;
static bool ring_alert_inband = false;
static uint8_t battery_power = RTK_BT_HFP_DEMO_DEFAULT_BATTERY_POWER_LEVEL;
static uint8_t call_curr_status = 0;
static void *alert_timer = NULL;
static void *alert_track_hdl = NULL;
static rtk_bt_audio_track_t *hfp_audio_track_hdl = NULL;
static rtk_bt_audio_record_t *hfp_audio_record_hdl = NULL;
static void *hfp_codec_entity = NULL;
static rtk_bt_cvsd_codec_t cvsd_codec_t = {0};
static rtk_bt_hfp_hf_conf_t bt_hfp_hf_conf = {
	.link_num = 1,
	.rfc_hfp_chann_num = RTK_BT_RFC_HFP_CHANN_NUM,
	.rfc_hsp_chann_num = RTK_BT_RFC_HSP_CHANN_NUM,
	.hf_supported_features = RTK_BT_HFP_HF_LOCAL_THREE_WAY_CALLING |
	RTK_BT_HFP_HF_LOCAL_CLI_PRESENTATION_CAPABILITY |
	RTK_BT_HFP_HF_LOCAL_VOICE_RECOGNITION_ACTIVATION |
	RTK_BT_HFP_HF_LOCAL_ESCO_S4_SETTINGS |
	RTK_BT_HFP_HF_LOCAL_REMOTE_VOLUME_CONTROL,
	.hf_supported_codecs = RTK_BT_HFP_HF_CODEC_TYPE_CVSD
};
static uint16_t sco_seq_num = 0;
/* class of device */
static uint32_t cod = (0x18 | (0x04 << 8) | (0x24 << 16));
/* supervision_timeout */
static uint16_t superv_time = (0x1f40);
static rtk_bt_a2dp_media_codec_sbc_t codec_sbc = {
	.sampling_frequency_mask = 0xf0,
	.channel_mode_mask = 0x0f,
	.block_length_mask = 0xf0,
	.subbands_mask = 0x0C,
	.allocation_method_mask = 0x03,
	.min_bitpool = 2,
	.max_bitpool = 53,
};
static rtk_bt_sbc_codec_t sbc_codec_t = {
	{
		.sbc_enc_mode = (sbc_channel_mode_t)SBC_MODE_STANDARD,
		.blocks = 16,
		.subbands = 8,
		.alloc_method = SBC_ALLOCATION_METHOD_SNR,
		.sample_rate = 44100,
		.bitpool = 0x21,
		.channel_mode = SBC_CHANNEL_MODE_DUAL_CHANNEL,
	},
	{0},
};
static rtk_bt_br_page_param_t page_param = {
	.pagescan_type = RTK_BT_BR_GAP_PAGE_SCAN_TYPE_INTERLACED,
	.pagescan_interval = APP_BT_DEFAULT_PAGESCAN_INTERVAL,
	.pagescan_window = APP_BT_DEFAULT_PAGESCAN_WINDOW,
	.page_timeout = APP_BT_DEFAULT_PAGE_TIMEOUT,
};
static rtk_bt_br_inquiry_param_t inquiry_param = {
	.inquiryscan_type = RTK_BT_BR_GAP_PAGE_SCAN_TYPE_INTERLACED,
	.inquiryscan_window = APP_BT_DEFAULT_INQUIRYSCAN_WINDOW,
	.inquiryscan_interval = APP_BT_DEFAULT_INQUIRYSCAN_INTERVAL,
	.inquiry_mode = RTK_BT_BR_GAP_INQUIRY_MODE_EXTENDED_RESULT,
};
static const uint8_t did_sdp_record[] = {
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x4D,
	//attribute SDP_ATTR_SRV_CLASS_ID_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_PNP_INFORMATION >> 8),
	(uint8_t)(RTK_BT_UUID_PNP_INFORMATION),

	//attribute SDP_ATTR_BROWSE_GROUP_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_PUBLIC_BROWSE_GROUP >> 8),
	(uint8_t)RTK_BT_UUID_PUBLIC_BROWSE_GROUP,

	//attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x09,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_LANG_ENGLISH >> 8),
	(uint8_t)RTK_BT_SDP_LANG_ENGLISH,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_CHARACTER_UTF8 >> 8),
	(uint8_t)RTK_BT_SDP_CHARACTER_UTF8,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_BASE_LANG_OFFSET >> 8),
	(uint8_t)RTK_BT_SDP_BASE_LANG_OFFSET,

	//attribute SDP_ATTR_PROFILE_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROFILE_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROFILE_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x08,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_PNP_INFORMATION >> 8),
	(uint8_t)RTK_BT_UUID_PNP_INFORMATION,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	0x01,//version 1.3
	0x03,

	//attribute SDP_ATTR_DIP_SPECIFICATION_ID
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_DIP_SPECIFICATION_ID >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_DIP_SPECIFICATION_ID,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	0x01,
	0x03,

	//attribute SDP_ATTR_DIP_VENDOR_ID
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_DIP_VENDOR_ID >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_DIP_VENDOR_ID,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	//0x00,//0x005D : RealTek
	//0x5D,
	(uint8_t)(RTK_COMPANY_ID >> 8),
	(uint8_t)RTK_COMPANY_ID,

	//attribute SDP_ATTR_DIP_PRODUCT_ID
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_DIP_PRODUCT_ID >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_DIP_PRODUCT_ID,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	0x22,//8763
	0x3B,

	//attribute SDP_ATTR_DIP_PRODUCT_VERSION
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_DIP_PRODUCT_VERSION >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_DIP_PRODUCT_VERSION,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	0x01,// 1.0.0
	0x00,

	//attribute SDP_ATTR_DIP_PRIMARY_RECORD
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_DIP_PRIMARY_RECORD >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_DIP_PRIMARY_RECORD,
	RTK_BT_SDP_BOOL_ONE_BYTE,
	true,

	//attribute SDP_ATTR_DIP_VENDOR_ID_SOURCE
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_DIP_VENDOR_ID_SOURCE >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_DIP_VENDOR_ID_SOURCE,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	0x00,//Bluetooth SIG
	0x01
};

static const uint8_t avrcp_tg_sdp_record[] = {
	//Total length
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x38,//0x46,//0x5F,

	//Attribute SDP_ATTR_SRV_CLASS_ID_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03, //Attribute length: 6 bytes
	//Service Class #0: A/V Remote Control Target
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_AV_REMOTE_CONTROL_TARGET >> 8),
	(uint8_t)(RTK_BT_UUID_AV_REMOTE_CONTROL_TARGET),

	//Attribute SDP_ATTR_PROTO_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROTO_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROTO_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x10, //Attribute length: 12 bytes
	//Protocol #0: L2CAP
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06, //Element length: 3 bytes
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_L2CAP >> 8),
	(uint8_t)(RTK_BT_UUID_L2CAP),
	//Parameter #0 for Protocol #0: PSM = AVCTP
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_PSM_AVCTP >> 8),
	(uint8_t)RTK_BT_PSM_AVCTP,
	//Protocol #1: AVCTP
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06, //Element length: 5 bytes
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_AVCTP >> 8),
	(uint8_t)(RTK_BT_UUID_AVCTP),
	//Parameter #0 for Protocol #1: 0x0104 (v1.4)
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0104 >> 8),
	(uint8_t)(0x0104),

	//Attribute SDP_ATTR_PROFILE_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROFILE_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROFILE_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x08, //Attribute length: 8 bytes
	//Profile #0: A/V Remote Control
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06, //Element length: 6 bytes
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_AV_REMOTE_CONTROL >> 8),
	(uint8_t)(RTK_BT_UUID_AV_REMOTE_CONTROL),
	//Parameter #0 for Profile #0: 0x0106 (v1.6)
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0106 >> 8),
	(uint8_t)(0x0106),

	//Attribute SDP_ATTR_SUPPORTED_FEATURES
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)((RTK_BT_SDP_ATTR_SUPPORTED_FEATURES) >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_SUPPORTED_FEATURES),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0002 >> 8), //Category 2 Amplifier
	(uint8_t)(0x0002),

	//Attribute SDP_ATTR_BROWSE_GROUP_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_PUBLIC_BROWSE_GROUP >> 8),
	(uint8_t)RTK_BT_UUID_PUBLIC_BROWSE_GROUP
	/*
	    //Attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST...it is used for SDP_ATTR_SRV_NAME
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
	    (uint8_t)RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
	    RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	    0x09,
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_LANG_ENGLISH >> 8),
	    (uint8_t)RTK_BT_SDP_LANG_ENGLISH,
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_CHARACTER_UTF8 >> 8),
	    (uint8_t)RTK_BT_SDP_CHARACTER_UTF8,
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_BASE_LANG_OFFSET >> 8),
	    (uint8_t)RTK_BT_SDP_BASE_LANG_OFFSET,

	    //Attribute SDP_ATTR_PROVIDER_NAME
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)((RTK_BT_SDP_ATTR_PROVIDER_NAME + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	    (uint8_t)(RTK_BT_SDP_ATTR_PROVIDER_NAME + RTK_BT_SDP_BASE_LANG_OFFSET),
	    RTK_BT_SDP_STRING_HDR,
	    0x07, //Attribute length: 7 bytes
	    0x52, 0x65, 0x61, 0x6C, 0x54, 0x65, 0x6B, //RealTek

	    //Attribute SDP_ATTR_SRV_NAME
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)((RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	    (uint8_t)(RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET),
	    SDP_STRING_HDR,
	    0x08, //Attribute length: 8 bytes
	    0x41, 0x56, 0x52, 0x43, 0x50, 0x20, 0x54, 0x47, //AVRCP TG
	*/
};

static const uint8_t avrcp_ct_sdp_record[] = {
	//Total length
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x3B,//0x49,//0x62,

	//Attribute SDP_ATTR_SRV_CLASS_ID_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06, //Attribute length: 6 bytes
	//Service Class #0: A/V Remote Control
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_AV_REMOTE_CONTROL >> 8),
	(uint8_t)(RTK_BT_UUID_AV_REMOTE_CONTROL),
	//Service Class #1: A/V Remote Control Controller
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_AV_REMOTE_CONTROL_CONTROLLER >> 8),
	(uint8_t)(RTK_BT_UUID_AV_REMOTE_CONTROL_CONTROLLER),

	//Attribute SDP_ATTR_PROTO_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROTO_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROTO_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x10, //Attribute length: 12 bytes
	//Protocol #0: L2CAP
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06, //Element length: 3 bytes
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_L2CAP >> 8),
	(uint8_t)(RTK_BT_UUID_L2CAP),
	//Parameter #0 for Protocol #0: PSM = AVCTP
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_PSM_AVCTP >> 8),
	(uint8_t)RTK_BT_PSM_AVCTP,
	//Protocol #1: AVCTP
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06, //Element length: 5 bytes
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_AVCTP >> 8),
	(uint8_t)(RTK_BT_UUID_AVCTP),
	//Parameter #0 for Protocol #1: 0x0104 (v1.4)
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0104 >> 8),
	(uint8_t)(0x0104),

	//Attribute SDP_ATTR_PROFILE_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROFILE_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROFILE_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x08, //Attribute length: 8 bytes
	//Profile #0: A/V Remote Control
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06, //Element length: 6 bytes
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_AV_REMOTE_CONTROL >> 8),
	(uint8_t)(RTK_BT_UUID_AV_REMOTE_CONTROL),
	//Parameter #0 for Profile #0: 0x0106 (v1.6)
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0106 >> 8),
	(uint8_t)(0x0106),

	//Attribute SDP_ATTR_SUPPORTED_FEATURES
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)((RTK_BT_SDP_ATTR_SUPPORTED_FEATURES) >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_SUPPORTED_FEATURES),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0001 >> 8), //Category 1 Player / Recorder
	(uint8_t)(0x0001),

	//Attribute SDP_ATTR_BROWSE_GROUP_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_PUBLIC_BROWSE_GROUP >> 8),
	(uint8_t)RTK_BT_UUID_PUBLIC_BROWSE_GROUP
	/*
	    //Attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST...it is used for SDP_ATTR_SRV_NAME
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
	    (uint8_t)RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
	    RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	    0x09,
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_LANG_ENGLISH >> 8),
	    (uint8_t)RTK_BT_SDP_LANG_ENGLISH,
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_CHARACTER_UTF8 >> 8),
	    (uint8_t)RTK_BT_SDP_CHARACTER_UTF8,
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_BASE_LANG_OFFSET >> 8),
	    (uint8_t)RTK_BT_SDP_BASE_LANG_OFFSET,

	    //Attribute SDP_ATTR_PROVIDER_NAME
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)((RTK_BT_SDP_ATTR_PROVIDER_NAME + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	    (uint8_t)(RTK_BT_SDP_ATTR_PROVIDER_NAME + RTK_BT_SDP_BASE_LANG_OFFSET),
	    RTK_BT_SDP_STRING_HDR,
	    0x07, //Attribute length: 7 bytes
	    0x52, 0x65, 0x61, 0x6C, 0x54, 0x65, 0x6B, //RealTek

	    //Attribute SDP_ATTR_SRV_NAME
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)((RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	    (uint8_t)(RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET),
	    RTK_BT_SDP_STRING_HDR,
	    0x08, //Attribute length: 8 bytes
	    0x41, 0x56, 0x52, 0x43, 0x50, 0x20, 0x43, 0x54, //AVRCP CT
	*/
};

static const uint8_t a2dp_sink_sdp_record[] = {
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x39,//0x55,
	//attribute SDP_ATTR_SRV_CLASS_ID_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_AUDIO_SINK >> 8),
	(uint8_t)(RTK_BT_UUID_AUDIO_SINK),

	//attribute SDP_ATTR_PROTO_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROTO_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROTO_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x10,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_L2CAP >> 8),
	(uint8_t)(RTK_BT_UUID_L2CAP),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_PSM_AVDTP >> 8),
	(uint8_t)(RTK_BT_PSM_AVDTP),
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_AVDTP >> 8),
	(uint8_t)(RTK_BT_UUID_AVDTP),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	0x01,
	0x03,

	//attribute SDP_ATTR_BROWSE_GROUP_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_PUBLIC_BROWSE_GROUP >> 8),
	(uint8_t)(RTK_BT_UUID_PUBLIC_BROWSE_GROUP),
	/*
	    //attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
	    (uint8_t)RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
	    RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	    0x09,
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_LANG_ENGLISH >> 8),
	    (uint8_t)RTK_BT_SDP_LANG_ENGLISH,
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_CHARACTER_UTF8 >> 8),
	    (uint8_t)RTK_BT_SDP_CHARACTER_UTF8,
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_BASE_LANG_OFFSET >> 8),
	    (uint8_t)RTK_BT_SDP_BASE_LANG_OFFSET,
	*/
	//attribute SDP_ATTR_PROFILE_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROFILE_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROFILE_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x08,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_ADVANCED_AUDIO_DISTRIBUTION >> 8),
	(uint8_t)(RTK_BT_UUID_ADVANCED_AUDIO_DISTRIBUTION),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	0x01,//version 1.3
	0x03,

	//attribute SDP_ATTR_SUPPORTED_FEATURES
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_SUPPORTED_FEATURES >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_SUPPORTED_FEATURES,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	0x00,
	0x03
	/*
	    //attribute SDP_ATTR_SRV_NAME
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)((RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	    (uint8_t)(RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET),
	    RTK_BT_SDP_STRING_HDR,
	    0x09,
	    0x61, 0x32, 0x64, 0x70, 0x5f, 0x73, 0x69, 0x6e, 0x6b //a2dp_sink
	*/
};

static const uint8_t hfp_hf_sdp_record[] = {
	//total length
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x4B,//0x37,//0x59,

	//attribute RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,          //0x35
	0x06,                                   //6bytes
	RTK_BT_SDP_UUID16_HDR,                     //0x19
	(uint8_t)(RTK_BT_UUID_HANDSFREE >> 8), //0x111E
	(uint8_t)(RTK_BT_UUID_HANDSFREE),
	RTK_BT_SDP_UUID16_HDR,                     //0x19
	(uint8_t)(RTK_BT_UUID_GENERIC_AUDIO >> 8),  //0x1203
	(uint8_t)(RTK_BT_UUID_GENERIC_AUDIO),

	//attribute RTK_BT_SDP_ATTR_PROTO_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROTO_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROTO_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,          //0x35
	0x0C,                                   //12bytes
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,      //0x35
	0x03,                               //3bytes
	RTK_BT_SDP_UUID16_HDR,                 //0x19
	(uint8_t)(RTK_BT_UUID_L2CAP >> 8),     //0x0100
	(uint8_t)(RTK_BT_UUID_L2CAP),
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,      //0x35
	0x05,                               //5bytes
	RTK_BT_SDP_UUID16_HDR,                 //0x19
	(uint8_t)(RTK_BT_UUID_RFCOMM >> 8),   //0x0003
	(uint8_t)(RTK_BT_UUID_RFCOMM),
	RTK_BT_SDP_UNSIGNED_ONE_BYTE,           //0x08
	RTK_BT_RFC_HFP_CHANN_NUM,  //0x02

	//attribute RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_PUBLIC_BROWSE_GROUP >> 8),
	(uint8_t)RTK_BT_UUID_PUBLIC_BROWSE_GROUP,
	/*
	    //attribute RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST...it is used for RTK_BT_SDP_ATTR_SRV_NAME
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
	    (uint8_t)RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
	    RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	    0x09,
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_LANG_ENGLISH >> 8),
	    (uint8_t)RTK_BT_SDP_LANG_ENGLISH,
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_CHARACTER_UTF8 >> 8),
	    (uint8_t)RTK_BT_SDP_CHARACTER_UTF8,
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_BASE_LANG_OFFSET >> 8),
	    (uint8_t)RTK_BT_SDP_BASE_LANG_OFFSET,

	    //attribute RTK_BT_SDP_ATTR_SRV_NAME
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)((RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	    (uint8_t)(RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET),
	    RTK_BT_SDP_STRING_HDR,                             //0x25 text string
	    0x0F,                                   //15 bytes
	    0x48, 0x61, 0x6e, 0x64, 0x73, 0x2d, 0x66, 0x72, 0x65, 0x65,
	    0x20, 0x75, 0x6e, 0x69, 0x74, //"Hands-free unit"
	*/

	//attribute RTK_BT_SDP_ATTR_PROFILE_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROFILE_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROFILE_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,          //0x35
	0x08,                                   //8 bytes
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,      //0x35
	0x06,                               //6 bytes
	RTK_BT_SDP_UUID16_HDR,                 //0x19
	(uint8_t)(RTK_BT_UUID_HANDSFREE >> 8), //0x111E
	(uint8_t)(RTK_BT_UUID_HANDSFREE),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,           //0x09
	(uint8_t)(0x0109 >> 8),     //version number default hf1.9
	(uint8_t)(0x0109),

	//Attribute SDP_ATTR_SRV_NAME
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)((RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET),
	RTK_BT_SDP_STRING_HDR,
	0x0F,
	'H', 'a', 'n', 'd', 's', '-', 'F', 'r', 'e', 'e', ' ', 'u', 'n', 'i', 't',

	//attribute SDP_ATTR_SUPPORTED_FEATURES
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)((RTK_BT_SDP_ATTR_SUPPORTED_FEATURES) >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_SUPPORTED_FEATURES),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x013F >> 8),
	(uint8_t)(0x013F)
};

static const uint8_t pbap_pce_sdp_record[] = {
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x36,
	//attribute SDP_ATTR_SRV_CLASS_ID_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_PBAP_PCE >> 8),
	(uint8_t)(RTK_BT_UUID_PBAP_PCE),

	//attribute SDP_ATTR_BROWSE_GROUP_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_PUBLIC_BROWSE_GROUP >> 8),
	(uint8_t)RTK_BT_UUID_PUBLIC_BROWSE_GROUP,

	//attribute SDP_ATTR_PROFILE_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROFILE_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROFILE_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x08,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_PBAP >> 8),
	(uint8_t)RTK_BT_UUID_PBAP,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	0x01,//version 1.2
	0x02,

	//attribute SDP_ATTR_SRV_NAME
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)((RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET),
	RTK_BT_SDP_STRING_HDR,
	0x14,
	0x50, 0x68, 0x6f, 0x6e, 0x65, 0x62, 0x6f, 0x6f, 0x6b, 0x20, 0x41, 0x63, 0x63, 0x65, 0x73, 0x73, 0x20, 0x50, 0x43, 0x45 //"Phonebook Access PCE"
};

/* source pac */
static uint8_t bt_le_audio_pac_source_codec[] = {
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
static uint8_t bt_le_audio_pac_sink_codec[] = {
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

/* ---------------------------- LE Audio scan list manage ---------------------------- */
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
			if (APP_BT_LE_AUDIO_BASS_SCAN_TYPE == scan_type) {
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

static uint16_t app_bt_le_audio_scan_report_handle(rtk_bt_le_ext_scan_res_ind_t *scan_res_ind, uint8_t scan_type)
{
	uint16_t ret = 1;

	if (!scan_res_ind) {
		BT_LOGE("[APP] %s input scan_res_ind is NULL \r\n", __func__);
		return ret;
	}
	switch (scan_type) {

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

/* ---------------------------- Audio PCM converter  ---------------------------- */
static uint16_t app_bt_pcm_data_resample_engine_alloc(rtk_bt_audio_resample_t **pp_sample_t, uint32_t sample_rate_in, uint8_t in_channels)
{
	double sample_ratio;

	demo_in_rate = sample_rate_in;
	sample_ratio = (double)demo_out_rate / (double)demo_in_rate;
	//calculate the expected input and output according to the sample rate
	resample_out_frames = demo_out_rate * 10 / 1000;
	resample_in_frames = (uint32_t)(resample_out_frames * (float)demo_in_rate / (float)demo_out_rate);
	/* init bq filter */
	rtk_bt_audio_bq_config(&bq_t, RTK_BT_AUDIO_LPF, RTK_BT_AUDIO_FILTER_DEFAULT_GAIN, (unsigned long)((1.0 / sample_ratio / 2) * (double)demo_in_rate),
						   (unsigned long)demo_in_rate, RTK_BT_AUDIO_FILTER_DEFAULT_BANDWIDTH);
	/* init sinc resample */
	*pp_sample_t = rtk_bt_audio_resample_alloc((float)demo_in_rate, (float)demo_out_rate, in_channels, APP_BT_A2DP_PBP_DEMO_OUPUT_CHANNEL_NUM, resample_in_frames);

	return RTK_BT_OK;
}

static uint16_t app_bt_pcm_data_resample_engine_destroy(rtk_bt_audio_resample_t **pp_sample_t)
{
	if (*pp_sample_t) {
		rtk_bt_audio_resample_free(*pp_sample_t);
		*pp_sample_t = NULL;
	}

	return RTK_BT_OK;
}
/*
    1.Used to callback 44.1khz pcm data from bt_audio_intf.c
    2.resample pcm data
    3.enqueue pcm data
*/
static uint16_t rtk_bt_audio_decode_pcm_data_callback(void *p_pcm_data, uint16_t p_len, void *pentity, void *track)
{
	(void)pentity;
	(void)track;
	uint32_t enqueue_size = 0;
#if 0
	uint32_t time_stamp_before, time_stamp_after = 0;
#endif
	uint32_t out_frames = 0;
	/* if pbp broadcast has not started, stop resample */
	if (false == lea_broadcast_start) {
#if defined(RTK_BLE_AUDIO_BROADCAST_LOCAL_PLAY_SUPPORT) && RTK_BLE_AUDIO_BROADCAST_LOCAL_PLAY_SUPPORT
		/* play local music here*/
		rtk_bt_audio_track_play(a2dp_audio_track_hdl->audio_track_hdl, p_pcm_data, p_len);
#endif
		if (0 == (fail_cnt % 500)) {
			BT_LOGE("[APP] PBP broadcast has not started!\r\n");
		}
		fail_cnt++;
		goto exit;
	}
	/* 1. resample */
#if 0
	time_stamp_before = osif_sys_time_get();
#endif
	out_frames = rtk_bt_audio_resample_entry(g_audio_resample_t, &bq_t,
											 (uint8_t *)p_pcm_data,
											 p_len / (g_audio_resample_t->in_frame_size),
											 (uint8_t *)out_frame_buf,
											 demo_in_rate,
											 demo_out_rate);
#if 0
	time_stamp_after = osif_sys_time_get();
	BT_LOGD("[APP] %s: time_stamp before :%u,time_stamp after:%u,delt_time:%d\r\n", __func__, time_stamp_before, time_stamp_after,
			(int)(time_stamp_after - time_stamp_before));
	BT_LOGD("[APP] in_frames:%u out_frames:%u \r\n", p_len / (g_audio_resample_t->in_frame_size), out_frames);
#endif
	/* 2. enqueue */
	{
		if (app_pcm_data_mgr_queue.mtx == NULL) {
			BT_LOGE("[APP] %s p_enqueue_mtx is NULL\r\n", __func__);
			goto exit;
		}
		enqueue_size = out_frames * g_audio_resample_t->out_frame_size;
		BT_LOGD("[APP] %s enqueue_size\r\n", __func__, enqueue_size);
		osif_mutex_take(app_pcm_data_mgr_queue.mtx, BT_TIMEOUT_FOREVER);
		if (RTK_BT_OK != app_queue_mgr_enqueue(&app_pcm_data_mgr_queue, out_frame_buf, enqueue_size)) {
			/* queue is full , flush queue*/
			app_queue_mgr_flush_queue(&app_pcm_data_mgr_queue);
			pbp_broadcast_dequeue_flag = false;
			BT_LOGE("[APP] %s: flush queue\r\n", __func__);
			osif_mutex_give(app_pcm_data_mgr_queue.mtx);
			goto exit;
		}
		if (app_pcm_data_mgr_queue.queue_size >= APP_BT_PCM_DATA_QUEUE_WATER_LEVEL) {
			pbp_broadcast_dequeue_flag = true;
		}
		osif_mutex_give(app_pcm_data_mgr_queue.mtx);
	}
	/* flush queue if BIS broadcast stop */
	if (!lea_broadcast_start) {
		osif_mutex_take(app_pcm_data_mgr_queue.mtx, BT_TIMEOUT_FOREVER);
		app_queue_mgr_flush_queue(&app_pcm_data_mgr_queue);
		pbp_broadcast_dequeue_flag = false;
		osif_mutex_give(app_pcm_data_mgr_queue.mtx);
	}

exit:
	return 1;
}

static a2dp_bond_info_t *a2dp_find_bond_info_by_mac_addr(uint8_t *bd_addr)
{
	a2dp_bond_info_t *pbond_info = NULL;

	if (!bd_addr) {
		return NULL;
	}
	for (uint8_t i = 0; i < A2DP_BT_MAX_BOND_NUM; i ++) {
		/* check bond info unit meaningful */
		if (A2DP_BT_BOND_INFO_USED_VAL == a2dp_bond_table[i].used_check) {
			if (!memcmp((void *)a2dp_bond_table[i].bd_addr, (void *)bd_addr, 6)) {
				pbond_info = &a2dp_bond_table[i];
				break;
			}
		}
	}

	return pbond_info;
}

static a2dp_bond_info_t *a2dp_find_bond_info_by_priority(uint8_t bond_priority)
{
	a2dp_bond_info_t *pbond_info = NULL;

	if ((bond_priority < 1) || (bond_priority > 8)) {
		return NULL;
	}
	for (uint8_t i = 0; i < A2DP_BT_MAX_BOND_NUM; i ++) {
		/* check bond info unit meaningful */
		if (A2DP_BT_BOND_INFO_USED_VAL == a2dp_bond_table[i].used_check) {
			if (bond_priority == a2dp_bond_table[i].priority) {
				pbond_info = &a2dp_bond_table[i];
				break;
			}
		}
	}

	return pbond_info;
}

static a2dp_bond_info_t *get_free_bond_table_unit(void)
{
	a2dp_bond_info_t *pbond_info = NULL;

	for (uint8_t i = 0; i < A2DP_BT_MAX_BOND_NUM; i ++) {
		/* check free unit */
		if (A2DP_BT_BOND_INFO_USED_VAL != a2dp_bond_table[i].used_check) {
			pbond_info = &a2dp_bond_table[i];
			pbond_info->used_check = A2DP_BT_BOND_INFO_USED_VAL;
			break;
		}
	}

	/* if bond units are full, get the lowest priority bond info for overwriting */
	if (!pbond_info) {
		pbond_info = a2dp_find_bond_info_by_priority(8);
	}

	return pbond_info;
}

/* once application modify bond info, this api should bt invoke to sync bond info table */
static uint16_t a2dp_flush_bond_info(uint8_t type)
{
	app_demo_msg_t io_msg;
	io_msg.type = type;

	if (false == osif_msg_send(a2dp_bond_info_flush_msg_q, &io_msg, 0)) {
		return 1;
	}

	return 0;
}

static void a2dp_bond_flush_thread(void *ctx)
{
	(void)ctx;
	app_demo_msg_t io_msg;
	uint8_t bd_addr[6] = {0};
	a2dp_bond_info_t *pbond_info = NULL;
	uint8_t temp_bond_num = 0;
	uint8_t need_save_bond_info = 0;
	uint16_t ret = 1;

	osif_sem_give(bond_info_flush_task.sem);
	while (bond_info_flush_task.run) {
		if (osif_msg_recv(a2dp_bond_info_flush_msg_q, &io_msg, 0xFFFFFFFF) == true) {
			if (A2DP_BT_BOND_INFO_THREAD_EXIT == io_msg.type) {
				bond_info_flush_task.run = 0;
				continue;
			}
			/* do flush action */
			/* 1. get bond number */
			if (rtk_bt_br_gap_bond_num_get(&temp_bond_num)) {
				BT_LOGE("[A2DP] Get Bond Number fail\r\n");
				continue;
			}
			BT_LOGA("[A2DP] Get Bond Number %d \r\n", temp_bond_num);
			/* 2. get bond mac address accord to bond number */
			for (uint8_t i = 0; i < temp_bond_num; i ++) {
				/* gap bond priority start frmo 1, so i + 1 */
				ret = rtk_bt_br_gap_bond_addr_get(i + 1, bd_addr);
				if (ret) {
					BT_LOGE("[A2DP] Get bond addr failed! err: 0x%x", ret);
					continue;
				}
				BT_LOGA("[A2DP] Get Bond addr %02x:%02x:%02x:%02x:%02x:%02x \r\n",
						bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
				/* 3. sync with a2dp_bond_table */
				pbond_info = a2dp_find_bond_info_by_mac_addr(bd_addr);
				if (!pbond_info) {
					/* allocate new bond info unit */
					pbond_info = get_free_bond_table_unit();
					if (!pbond_info) {
						BT_LOGE("[A2DP] Get free bond table unit fail \r\n");
						continue;
					}
					memcpy((void *)pbond_info->bd_addr, (void *)bd_addr, 6);
					pbond_info->name_contained = 0;
					pbond_info->priority = i + 1;
					memset((void *)pbond_info->name, 0, RTK_BT_GAP_DEVICE_NAME_LEN);
					need_save_bond_info = 1;
					/* 4. try to fetch remote device name */
					rtk_bt_br_gap_get_remote_name(bd_addr);
				} else {
					/* update bond info unit */
					if (pbond_info->priority != (i + 1)) {
						if (A2DP_BT_BOND_INFO_PRIORITY_UNASSIGNED != pbond_info->priority) {
							a2dp_bond_info_t *ptemp_bond_info = NULL;
							/* get previous priority bond info unit */
							ptemp_bond_info = a2dp_find_bond_info_by_priority(i + 1);
							if (ptemp_bond_info) {
								/* this bond info should be mask for further configuration */
								ptemp_bond_info->priority = A2DP_BT_BOND_INFO_PRIORITY_UNASSIGNED;
							}
						}
						need_save_bond_info = 1;
						pbond_info->priority = i + 1;
					}
					if (!pbond_info->name_contained) {
						/* 4. try to fetch remote device name */
						rtk_bt_br_gap_get_remote_name(bd_addr);
					}
				}
			}
			/* 5. Maybe the loweset priority bond unit is delete, which cannot be detected by step3 */
			if (temp_bond_num < a2dp_bond_num) {
				for (uint8_t i = 0; i < (a2dp_bond_num - temp_bond_num); i ++) {
					pbond_info = a2dp_find_bond_info_by_priority(temp_bond_num + i + 1);
					if (pbond_info) {
						memset((void *)pbond_info, 0, sizeof(a2dp_bond_info_t));
						need_save_bond_info = 1;
					}
				}
			}
			/* 6. check whether need save a2dp_bond_table */
			if (need_save_bond_info) {
				/* foreach table to check whether there is A2DP_BT_BOND_INFO_PRIORITY_UNASSIGNED bond unit(need to be deleted) */
				for (uint8_t i = 0; i < A2DP_BT_MAX_BOND_NUM; i ++) {
					if ((A2DP_BT_BOND_INFO_USED_VAL == a2dp_bond_table[i].used_check) &&
						(A2DP_BT_BOND_INFO_PRIORITY_UNASSIGNED == a2dp_bond_table[i].priority)) {
						memset((void *)&a2dp_bond_table[i], 0, sizeof(a2dp_bond_info_t));
					}
				}
				if (rt_kv_set(a2dp_bond_info_key, (void *)a2dp_bond_table, sizeof(a2dp_bond_table)) == sizeof(a2dp_bond_table)) {
					BT_LOGA("[A2DP] Save a2dp demo bond info table success \r\n");
				} else {
					BT_LOGE("[A2DP] Fail to save a2dp demo bond info table \r\n");
				}
			}
			a2dp_bond_num = temp_bond_num;
		} else {
			BT_LOGE("[A2DP] Bond info flush thread recv msg fail\r\n");
		}
		osif_delay(3);
	}

	osif_sem_give(bond_info_flush_task.sem);
	osif_task_delete(NULL);
}

static void a2dp_bond_info_dump(void)
{
	a2dp_bond_info_t *pbond_info = NULL;

	BT_LOGA("[A2DP] Bond Info List\r\n");
	for (uint8_t i = 0; i < a2dp_bond_num; i ++) {
		pbond_info = a2dp_find_bond_info_by_priority(i + 1);
		if (pbond_info) {
			BT_LOGA(">> %d. ", (int)(i + 1));
			BT_LOGA(" mac addr %02x:%02x:%02x:%02x:%02x:%02x  ", pbond_info->bd_addr[5],
					pbond_info->bd_addr[4],
					pbond_info->bd_addr[3],
					pbond_info->bd_addr[2],
					pbond_info->bd_addr[1],
					pbond_info->bd_addr[0]);
			BT_LOGA(" name ");
			if (pbond_info->name_contained) {
				BT_LOGA(" %s", pbond_info->name);
			}
			BT_LOGA(" \r\n");
		}
	}
}

static void a2dp_reconnect_timer_handle(void *arg)
{
	(void)arg;

	if (a2dp_auto_reconnect_try_count --) {
		BT_LOGA("[A2DP]: try reconnect to %02x:%02x:%02x:%02x:%02x:%02x\r\n", remote_bd_addr[5], remote_bd_addr[4], remote_bd_addr[3], remote_bd_addr[2],
				remote_bd_addr[1], remote_bd_addr[0]);
		rtk_bt_a2dp_connect(remote_bd_addr);
		osif_timer_restart(&reconnect_timer, APP_BT_RECONNECT_TIMER_INTERVAL);
	} else {
		/* auto reconnect fail and enable inquiry scan and page scan */
		rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_VISIBLE_CONNECTABLE);
	}
}

static rtk_bt_evt_cb_ret_t br_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	(void)param;

	switch (evt_code) {

	/* RTK_BT_BR_GAP_ACT_SET_DEVICE_NAME */
	/* RTK_BT_BR_GAP_ACT_SET_PAGE_PARAM */
	/* RTK_BT_BR_GAP_ACT_SET_INQUIRY_PARAM */
	/* are Sync */

	case RTK_BT_BR_GAP_INQUIRY_RESULT: {
		rtk_bt_br_inquiry_result_t *p_result = (rtk_bt_br_inquiry_result_t *)param;
		BT_LOGA("[BR GAP] Scan %02x:%02x:%02x:%02x:%02x:%02x Name %s \r\n",
				p_result->bd_addr[5], p_result->bd_addr[4], p_result->bd_addr[3], p_result->bd_addr[2], p_result->bd_addr[1], p_result->bd_addr[0],
				p_result->name);
		BT_AT_PRINT("+BRGAP:inquiry_result,%02x:%02x:%02x:%02x:%02x:%02x,%s\r\n",
					p_result->bd_addr[5], p_result->bd_addr[4], p_result->bd_addr[3], p_result->bd_addr[2], p_result->bd_addr[1], p_result->bd_addr[0],
					p_result->name);
		break;
	}

	case RTK_BT_BR_GAP_REMOTE_NAME_RSP: {
		rtk_bt_br_remote_name_rsp_t *p_name_rsp = (rtk_bt_br_remote_name_rsp_t *)param;
		if (a2dp_bond_info_flush) {
			a2dp_bond_info_t *pbond_info = NULL;
			pbond_info = a2dp_find_bond_info_by_mac_addr(p_name_rsp->bd_addr);
			if (pbond_info) {
				memcpy((void *)pbond_info->name, (void *)p_name_rsp->name, RTK_BT_GAP_DEVICE_NAME_LEN);
				pbond_info->name_contained = 1;
				if (rt_kv_set(a2dp_bond_info_key, (void *)a2dp_bond_table, sizeof(a2dp_bond_table)) == sizeof(a2dp_bond_table)) {
					BT_LOGA("[A2DP] Save a2dp demo bond info table success \r\n");
					a2dp_bond_info_dump();
				} else {
					BT_LOGE("[A2DP] Fail to save a2dp demo bond info table \r\n");
				}
			}
		}
		break;
	}

	case RTK_BT_BR_GAP_ACL_CONN_IND: {
		uint8_t *bd_addr = (uint8_t *)param;
		BT_LOGA("[BR GAP] ACL connection indication %02x:%02x:%02x:%02x:%02x:%02x \r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		break;
	}

	case RTK_BT_BR_GAP_ACL_CONN_SUCCESS: {
		uint8_t *bd_addr = (uint8_t *)param;
		if (a2dp_auto_reconnect) {
			/* write reconnect info to file system */
			if (rt_kv_set(a2dp_filesystem_key, (void *)bd_addr, 6) == 6) {
				BT_LOGA("[A2DP] Save a2dp demo reconnect data success \r\n");
			} else {
				BT_LOGE("[A2DP] Fail to save a2dp reconnect data \r\n");
			}
			a2dp_auto_reconnect_try_count = 0;
			osif_timer_stop(&reconnect_timer);
		}
		BT_LOGA("[BR GAP] ACL connection success %02x:%02x:%02x:%02x:%02x:%02x \r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		break;
	}

	case RTK_BT_BR_GAP_ACL_CONN_FAIL: {
		rtk_bt_br_acl_conn_fail_t *p_fail_rsp = (rtk_bt_br_acl_conn_fail_t *)param;
		BT_LOGA("[BR GAP] ACL connection fail with %02x:%02x:%02x:%02x:%02x:%02x, cause is 0x%x \r\n",
				p_fail_rsp->bd_addr[5], p_fail_rsp->bd_addr[4], p_fail_rsp->bd_addr[3], p_fail_rsp->bd_addr[2], p_fail_rsp->bd_addr[1], p_fail_rsp->bd_addr[0],
				p_fail_rsp->cause);
		break;
	}

	case RTK_BT_BR_GAP_ACL_SNIFF: {
		rtk_bt_br_acl_sniff_t *p_sniff = (rtk_bt_br_acl_sniff_t *)param;
		BT_LOGA("[BR GAP] ACL sniff mode from %02x:%02x:%02x:%02x:%02x:%02x \r\n",
				p_sniff->bd_addr[5], p_sniff->bd_addr[4],
				p_sniff->bd_addr[3], p_sniff->bd_addr[2],
				p_sniff->bd_addr[1], p_sniff->bd_addr[0]);
		BT_LOGA("[BR GAP] ACL sniff interval 0x%x \r\n", p_sniff->interval);
		break;
	}

	case RTK_BT_BR_GAP_ACL_ACTIVE: {
		uint8_t *bd_addr = (uint8_t *)param;
		BT_LOGA("[BR GAP] ACL active %02x:%02x:%02x:%02x:%02x:%02x \r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		break;
	}

	case RTK_BT_BR_GAP_LINK_KEY_REQ: {
		uint8_t found = *(uint8_t *)param;
		if (a2dp_bond_info_flush && found) {
			a2dp_flush_bond_info(A2DP_BT_BOND_INFO_FLUSH_EVENT);
		}
		BT_LOGA("[BR GAP] Link Key Request received and found is 0x%x \r\n", found);
		break;
	}

	case RTK_BT_BR_GAP_LINK_KEY_INFO: {
		rtk_bt_br_bond_key_t *pbond_key_t = (rtk_bt_br_bond_key_t *)param;
		uint8_t *bd_addr = pbond_key_t->bd_addr;
		if (a2dp_bond_info_flush) {
			a2dp_flush_bond_info(A2DP_BT_BOND_INFO_FLUSH_EVENT);
		}
		BT_LOGA("[BR GAP] Set link key of %02x:%02x:%02x:%02x:%02x:%02x \r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		break;
	}

	case RTK_BT_BR_GAP_ACL_DISCONN: {
		rtk_bt_br_acl_disc_t *p_acl_disc_event = (rtk_bt_br_acl_disc_t *)param;
		BT_LOGA("[BR GAP] ACL disconnection %02x:%02x:%02x:%02x:%02x:%02x \r\n",
				p_acl_disc_event->bd_addr[5], p_acl_disc_event->bd_addr[4],
				p_acl_disc_event->bd_addr[3], p_acl_disc_event->bd_addr[2],
				p_acl_disc_event->bd_addr[1], p_acl_disc_event->bd_addr[0]);
		BT_LOGA("[BR GAP] ACL disc cause 0x%x \r\n", p_acl_disc_event->cause);
		BT_AT_PRINT("+BRGAP:disc,%02x:%02x:%02x:%02x:%02x:%02x,0x%x\r\n",
					p_acl_disc_event->bd_addr[5], p_acl_disc_event->bd_addr[4],
					p_acl_disc_event->bd_addr[3], p_acl_disc_event->bd_addr[2],
					p_acl_disc_event->bd_addr[1], p_acl_disc_event->bd_addr[0],
					p_acl_disc_event->cause);
		/* cause 0x113 -> Remote User Terminate*/
		/* cause 0x108 -> Conn Timeout*/
		if (a2dp_auto_reconnect) {
			if ((RTK_BT_ERR_HCI_GROUP | RTK_BT_HCI_ERR_REMOTE_USER_TERMINATE) == p_acl_disc_event->cause) {
				BT_LOGA("[A2DP] Deleting a2dp demo reconnect config \r\n");
				if (rt_kv_delete(a2dp_filesystem_key) == 0) {
					BT_LOGA("[A2DP] Delete a2dp demo reconnect config success \r\n");
				}
			} else if ((RTK_BT_ERR_HCI_GROUP | RTK_BT_HCI_ERR_CONN_TIMEOUT) == p_acl_disc_event->cause) {
				BT_LOGA("[A2DP] Start reconnect \r\n");
				a2dp_auto_reconnect_try_count = APP_BT_RECONNECT_COUNT;
				a2dp_disconnection_reconnecting = 1;
				/* avoid another device inquiry req and paging our dut */
				rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_CONNECTABLE);
				osif_timer_start(&reconnect_timer);
			}
		}
		if (a2dp_bond_info_flush) {
			a2dp_flush_bond_info(A2DP_BT_BOND_INFO_FLUSH_EVENT);
		}
		break;
	}

	default:
		BT_LOGE("%s, Unknown GAP RTK_BLE_EVT: %d\r\n", __func__, evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t app_bt_sdp_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;

	switch (evt_code) {
	case RTK_BT_SDP_EVT_DID_ATTR_INFO: {
		rtk_sdp_did_attr_info *p_info = (rtk_sdp_did_attr_info *)param;
		BT_LOGA("[SDP] SDP DID ATTR %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_info->bd_addr[5], p_info->bd_addr[4],
				p_info->bd_addr[3], p_info->bd_addr[2],
				p_info->bd_addr[1], p_info->bd_addr[0]);
		BT_LOGA("[SDP] specific_id 0x%04x, vendor_id 0x%04x, vendor_src 0x%04x, product_id 0x%04x, version 0x%04x \r\n",
				p_info->specific_id,
				p_info->vendor_id,
				p_info->vendor_src,
				p_info->product_id,
				p_info->version);
		break;
	}

	case RTK_BT_SDP_EVT_DISCOV_CMPL: {
		rtk_sdp_discov_cmpl *p_info = (rtk_sdp_discov_cmpl *)param;
		BT_LOGA("[SDP] discovery complete cause 0x%x \r\n", p_info->cause);
		break;
	}

	default: {
		BT_LOGE("%s, Unknown SDP RTK_BLE_EVT: %d\r\n", __func__, evt_code);
		break;
	}
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t app_bt_avrcp_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	uint8_t bd_addr[6];

	switch (evt_code) {
	case RTK_BT_AVRCP_EVT_CONN_IND: {
		memcpy((void *)bd_addr, param, 6);
		BT_LOGA("[AVRCP] Receive AVRCP connection from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		break;
	}

	case RTK_BT_AVRCP_EVT_ELEMENT_ATTR_INFO: {
		uint8_t temp_buff[50];
		const char *attr[] = {"", "Title:", "Artist:", "Album:", "Track:",
							  "TotalTrack:", "Genre:", "PlayingTime:", "CoverArt:"
							 };
		rtk_bt_avrcp_element_attr_info_t *p_attr_t = (rtk_bt_avrcp_element_attr_info_t *)param;

		if (p_attr_t->state == 0) {
			BT_LOGA("[AVRCP] Get element attr information successfully from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
					p_attr_t->bd_addr[5], p_attr_t->bd_addr[4], p_attr_t->bd_addr[3], p_attr_t->bd_addr[2], p_attr_t->bd_addr[1], p_attr_t->bd_addr[0]);
			for (uint8_t i = 0; i < p_attr_t->num_of_attr; i ++) {
				if (p_attr_t->attr[i].length) {
					memset((void *)temp_buff, 0, 50);
					if (RTK_BT_AVRCP_ELEM_ATTR_DEFAULT_COVER_ART == p_attr_t->attr[i].attribute_id) {
						uint8_t image_handle[16] = {0};
						for (uint8_t j = 0; j < p_attr_t->attr[i].length; j++) {
							image_handle[2 * j + 1] = p_attr_t->attr[i].p_buf[j];
						}
						BT_LOGA("[AVRCP] Get cover art image handle ");
						for (uint8_t i = 0; i < 16; i ++) {
							BT_LOGA(" 0x%02x ", image_handle[i]);
						}
						BT_LOGA("\r\n");
						continue;
					} else {
						uint16_t len = p_attr_t->attr[i].length + strlen(attr[p_attr_t->attr[i].attribute_id]) + 1;
						snprintf((char *)temp_buff, len, "%s%s\r\n", attr[p_attr_t->attr[i].attribute_id], p_attr_t->attr[i].p_buf);
						BT_LOGA("[AVRCP] %s \r\n", temp_buff);
					}
					osif_mem_free(p_attr_t->attr[i].p_buf);
				}
			}
		} else {
			BT_LOGA("[AVRCP] Get element attr information fail from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
					p_attr_t->bd_addr[5], p_attr_t->bd_addr[4], p_attr_t->bd_addr[3], p_attr_t->bd_addr[2], p_attr_t->bd_addr[1], p_attr_t->bd_addr[0]);
		}
		if (p_attr_t->num_of_attr) {
			osif_mem_free(p_attr_t->attr);
		}
		break;
	}

	case RTK_BT_AVRCP_EVT_APP_SETTING_ATTRS_LIST_RSP: {
		uint8_t temp_buff[10];
		rtk_bt_avrcp_app_setting_attrs_list_t *p_list_t = (rtk_bt_avrcp_app_setting_attrs_list_t *)param;
		const char *attr[] = {"", "EQ", "Repeat Mode", "Shuffle", "Scan"};

		if (p_list_t->state == 0) {
			BT_LOGA("[AVRCP] Get app settings attrs information successfully from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
					p_list_t->bd_addr[5], p_list_t->bd_addr[4], p_list_t->bd_addr[3], p_list_t->bd_addr[2], p_list_t->bd_addr[1], p_list_t->bd_addr[0]);
			for (uint8_t i = 0; i < p_list_t->num_of_attr; i ++) {
				memset((void *)temp_buff, 0, 10);
				snprintf((char *)temp_buff, len, "%s\r\n", attr[p_list_t->p_attr_id[i]]);
				BT_LOGA("[AVRCP] %s \r\n", temp_buff);
			}
		}
		break;
	}

	case RTK_BT_AVRCP_EVT_APP_SETTING_VALUES_LIST_RSP: {
		rtk_bt_avrcp_app_setting_values_list_t *p_values_t = (rtk_bt_avrcp_app_setting_values_list_t *)param;

		if (p_values_t->state == 0) {
			BT_LOGA("[AVRCP] Get app settings values information successfully from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
					p_values_t->bd_addr[5], p_values_t->bd_addr[4], p_values_t->bd_addr[3], p_values_t->bd_addr[2], p_values_t->bd_addr[1], p_values_t->bd_addr[0]);
			for (uint8_t i = 0; i < p_values_t->num_of_value; i ++) {
				BT_LOGA("[AVRCP] value 0x%02x \r\n", p_values_t->p_value[i]);
			}
		}
		break;
	}

	case RTK_BT_AVRCP_EVT_APP_SETTING_GET_RSP: {
		uint8_t temp_buff[20];
		rtk_bt_avrcp_app_setting_get_rsp_t *p_rsp_t = (rtk_bt_avrcp_app_setting_get_rsp_t *)param;
		const char *attr[] = {"", "EQ:", "Repeat Mode:", "Shuffle:", "Scan:"};

		if (p_rsp_t->state == 0) {
			BT_LOGA("[AVRCP] Get app settings response information successfully from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
					p_rsp_t->bd_addr[5], p_rsp_t->bd_addr[4], p_rsp_t->bd_addr[3], p_rsp_t->bd_addr[2], p_rsp_t->bd_addr[1], p_rsp_t->bd_addr[0]);
			for (uint8_t i = 0; i < p_rsp_t->num_of_attr; i ++) {
				memset((void *)temp_buff, 0, 20);
				snprintf((char *)temp_buff, len, "%s 0x%x\r\n", attr[p_rsp_t->p_app_setting[i].attr], p_rsp_t->p_app_setting[i].value);
				BT_LOGA("[AVRCP] %s \r\n", temp_buff);
			}
		}
		break;
	}

	case RTK_BT_AVRCP_EVT_COVER_ART_DATA_IND: {
		rtk_bt_avrcp_cover_art_data_ind_t *p_data_t = (rtk_bt_avrcp_cover_art_data_ind_t *)param;

		for (uint16_t i = 0; i < p_data_t->data_len; i ++) {
			if (i % 10 == 0) {
				BT_LOGA("\r\n");
			}
			BT_LOGA(" 0x%02x ", p_data_t->p_data[i]);
		}
		if (p_data_t->data_end) {
			BT_LOGA("[AVRCP] Data End -> Get art cover successfully \r\n");
		}
		break;
	}

	case RTK_BT_AVRCP_EVT_GET_PLAY_STATUS_RSP_EVENT: {
		rtk_bt_avrcp_get_play_status_rsp_t *p_rsp_t = (rtk_bt_avrcp_get_play_status_rsp_t *)param;

		if (p_rsp_t->state == 0) {
			BT_LOGA("[AVRCP] Get play status successfully from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
					p_rsp_t->bd_addr[5], p_rsp_t->bd_addr[4], p_rsp_t->bd_addr[3], p_rsp_t->bd_addr[2], p_rsp_t->bd_addr[1], p_rsp_t->bd_addr[0]);
			BT_LOGA("[AVRCP] play status is 0x%x, total song length is %d, position is %d \r\n",
					p_rsp_t->play_status, p_rsp_t->length_ms, p_rsp_t->position_ms);
		}
		break;
	}

	case RTK_BT_AVRCP_EVT_ABSOLUTE_VOLUME_SET: {
		rtk_bt_avrcp_absolute_volume_set_t *p_avrcp_absolute_volume_set_t = (rtk_bt_avrcp_absolute_volume_set_t *)param;
		uint8_t volume = p_avrcp_absolute_volume_set_t->volume;
		float audio_track_volume = 0;

		curr_volume = volume;
		audio_track_volume = 1.0 * curr_volume / APP_BT_VOL_MAX;
		rtk_bt_audio_track_set_hardware_volume(audio_track_volume, audio_track_volume);
		BT_LOGA("[AVRCP] absolute volume set %d \r\n", (int)(audio_track_volume * 100));
		break;
	}

	case RTK_BT_AVRCP_EVT_VOLUME_CHANGED: {
		rtk_bt_avrcp_volume_t *pvolume_t = (rtk_bt_avrcp_volume_t *)param;
		uint8_t volume = pvolume_t->volume;

		curr_volume = volume;
		BT_LOGA("[AVRCP] Volume changed %d \r\n", curr_volume);
		break;
	}

	case RTK_BT_AVRCP_EVT_VOLUME_UP: {
		if (curr_volume < APP_BT_VOL_MAX) {
			curr_volume++;
		} else {
			curr_volume = APP_BT_VOL_MAX;
		}
		BT_LOGA("[AVRCP] Volume up to %d \r\n", curr_volume);
		break;
	}

	case RTK_BT_AVRCP_EVT_VOLUME_DOWN: {
		if (curr_volume > APP_BT_VOL_MIN) {
			curr_volume--;
		} else {
			curr_volume = APP_BT_VOL_MIN;
		}
		BT_LOGA("[AVRCP] Volume down to %d \r\n", curr_volume);
		break;
	}

	case RTK_BT_AVRCP_EVT_REG_VOLUME_CHANGED: {
		rtk_bt_avrcp_volume_change_req_t *p_volume_t = (rtk_bt_avrcp_volume_change_req_t *)param;
		uint8_t volume = p_volume_t->volume;
		float audio_track_volume = 0;

		curr_volume = volume;
		audio_track_volume = 1.0 * curr_volume / APP_BT_VOL_MAX;
		rtk_bt_audio_track_set_hardware_volume(audio_track_volume, audio_track_volume);
		BT_LOGA("[AVRCP] volume request set %d \r\n", (int)(audio_track_volume * 100));
		BT_AT_PRINT("+BTAVRCP:volume_change,%d\r\n", (int)(audio_track_volume * 100));
		break;
	}

	case RTK_BT_AVRCP_EVT_CONN_CMPL: {
		BT_LOGA("[AVRCP] Connection Completion \r\n");
		rtk_bt_avrcp_conn_cmpl_t *conn_cmpl = (rtk_bt_avrcp_conn_cmpl_t *)param;
		BT_LOGA("[AVRCP] AVRCP connection completion with %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				conn_cmpl->bd_addr[5], conn_cmpl->bd_addr[4], conn_cmpl->bd_addr[3], conn_cmpl->bd_addr[2], conn_cmpl->bd_addr[1], conn_cmpl->bd_addr[0]);
		BT_AT_PRINT("+BTAVRCP:conn,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					conn_cmpl->bd_addr[5], conn_cmpl->bd_addr[4], conn_cmpl->bd_addr[3],
					conn_cmpl->bd_addr[2], conn_cmpl->bd_addr[1], conn_cmpl->bd_addr[0]);
		break;
	}

	case RTK_BT_AVRCP_EVT_DISCONN_CMPL: {
		BT_LOGA("[AVRCP] Disconnection Completion \r\n");
		rtk_bt_avrcp_disconn_cmpl_t *disconn_cmpl = (rtk_bt_avrcp_disconn_cmpl_t *)param;
		BT_LOGA("[AVRCP] AVRCP disconnection completion with %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				disconn_cmpl->bd_addr[5], disconn_cmpl->bd_addr[4], disconn_cmpl->bd_addr[3], disconn_cmpl->bd_addr[2], disconn_cmpl->bd_addr[1], disconn_cmpl->bd_addr[0]);
		BT_AT_PRINT("+BTAVRCP:disconn,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					disconn_cmpl->bd_addr[5], disconn_cmpl->bd_addr[4], disconn_cmpl->bd_addr[3],
					disconn_cmpl->bd_addr[2], disconn_cmpl->bd_addr[1], disconn_cmpl->bd_addr[0]);
		break;
	}

	case RTK_BT_AVRCP_EVT_PLAY_STATUS_CHANGED_REG_REQ: {
		rtk_bt_avrcp_sub_event_t *psub_event = (rtk_bt_avrcp_sub_event_t *)param;
		BT_AT_PRINT("+BTAVRCP:status,%u\r\n", psub_event->avrcp_play_status);
		switch (psub_event->avrcp_play_status) {
		case RTK_BT_AVRCP_STATUS_STOPPED: {
			BT_LOGA("[AVRCP]: Stopped \r\n");
			break;
		}

		case RTK_BT_AVRCP_STATUS_PLAYING: {
			BT_LOGA("[AVRCP]: Playing \r\n");
			break;
		}

		case RTK_BT_AVRCP_STATUS_PAUSED: {
			BT_LOGA("[AVRCP]: Paused \r\n");
			break;
		}

		case RTK_BT_AVRCP_STATUS_FWD_SEEK: {
			BT_LOGA("[AVRCP]: FWD_SEEK \r\n");
			break;
		}

		case RTK_BT_AVRCP_STATUS_REV_SEEK: {
			BT_LOGA("[AVRCP]: REV_SEEK \r\n");
			break;
		}

		case RTK_BT_AVRCP_STATUS_FAST_FWD: {
			BT_LOGA("[AVRCP]: FAST_FWD \r\n");
			break;
		}

		case RTK_BT_AVRCP_STATUS_REWIND: {
			BT_LOGA("[AVRCP]: REWIND \r\n");
			break;
		}

		case 0xFF: {
			BT_LOGE("[AVRCP]: ERROR \r\n");
		}
		break;

		default: {
			BT_LOGE("[AVRCP]: Default 0x%x \r\n", psub_event->avrcp_play_status);
		}
		break;
		}
		break;
	}

	case RTK_BT_AVRCP_EVT_TRACK_CHANGED: {
		rtk_bt_avrcp_track_changed_t *p_track_t = (rtk_bt_avrcp_track_changed_t *)param;
		BT_LOGA("[AVRCP]: Track changed id 0x%x \r\n", p_track_t->track_id);
		break;
	}

	case RTK_BT_AVRCP_EVT_PLAY_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Play req \r\n");
		break;
	}

	case RTK_BT_AVRCP_EVT_PAUSE_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Pause req \r\n");
		break;
	}

	case RTK_BT_AVRCP_EVT_STOP_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Stop req \r\n");
		break;
	}

	case RTK_BT_AVRCP_EVT_FORWARD_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Forward req \r\n");
		break;
	}

	case RTK_BT_AVRCP_EVT_BACKWARD_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Backward req \r\n");
		break;
	}

	case RTK_BT_AVRCP_EVT_FAST_FORWARD_START_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Fast forward start req \r\n");
		break;
	}

	case RTK_BT_AVRCP_EVT_FAST_FORWARD_STOP_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Fast forward stop req \r\n");
		break;
	}

	case RTK_BT_AVRCP_EVT_REWIND_START_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Rewind start req \r\n");
		break;
	}

	case RTK_BT_AVRCP_EVT_REWIND_STOP_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Rewind stop req \r\n");
		break;
	}

	default: {
		BT_LOGE("%s, Unknown GAP RTK_BLE_EVT: %d\r\n", __func__, evt_code);
		break;
	}
	}

	return RTK_BT_EVT_CB_OK;
}

static uint16_t rtk_bt_a2dp_sbc_parse_decoder_struct(rtk_bt_a2dp_codec_t *pa2dp_codec, rtk_bt_sbc_decode_t *psbc_decoder_t)
{
	uint32_t channels = 0;
	bool play_flag = false;
	if (pa2dp_codec->sbc.allocation_method == RTK_BT_A2DP_SBC_ALLOCATION_METHOD_LOUDNESS) {
		psbc_decoder_t->allocation_method = SBC_ALLOCATION_METHOD_LOUDNESS; // align to sbc codec lib
	} else {
		psbc_decoder_t->allocation_method = SBC_ALLOCATION_METHOD_SNR; // align to sbc codec lib
	}
	if (pa2dp_codec->sbc.subbands == RTK_BT_A2DP_SBC_SUBBANDS_4) {
		psbc_decoder_t->subbands = 4; // align to sbc codec lib
	} else {
		psbc_decoder_t->subbands = 8; // align to sbc codec lib
	}
	switch (pa2dp_codec->sbc.channel_mode) {
	case RTK_BT_A2DP_SBC_CHANNEL_MODE_MONO:
		psbc_decoder_t->channel_mode = SBC_CHANNEL_MODE_MONO;
		channels = 1;
		break;
	case RTK_BT_A2DP_SBC_CHANNEL_MODE_DUAL_CHANNEL:
		psbc_decoder_t->channel_mode = SBC_CHANNEL_MODE_DUAL_CHANNEL;
		channels = 2;
		break;
	case RTK_BT_A2DP_SBC_CHANNEL_MODE_STEREO:
		psbc_decoder_t->channel_mode = SBC_CHANNEL_MODE_STEREO;
		channels = 2;
		break;
	case RTK_BT_A2DP_SBC_CHANNEL_MODE_JOINT_STEREO:
		psbc_decoder_t->channel_mode = SBC_CHANNEL_MODE_JOINT_STEREO;
		channels = 2;
		break;
	default:
		BT_LOGE("[A2DP] RTK Audio not support channel_mode %x \r\n", pa2dp_codec->sbc.channel_mode);
		return 1;
	}
	switch (pa2dp_codec->sbc.sampling_frequency) {
	case RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_16KHZ:
		psbc_decoder_t->sampling_frequency = (uint32_t)16000;
		break;
	case RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_32KHZ:
		psbc_decoder_t->sampling_frequency = (uint32_t)32000;
		break;
	case RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_44_1KHZ:
		psbc_decoder_t->sampling_frequency = (uint32_t)44100;
		break;
	case RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_48KHZ:
		psbc_decoder_t->sampling_frequency = (uint32_t)48000;
		break;
	default:
		BT_LOGE("[A2DP] RTK Audio not support frequency %x \r\n", pa2dp_codec->sbc.sampling_frequency);
		return 1;
	}
	switch (pa2dp_codec->sbc.block_length) {
	case RTK_BT_A2DP_SBC_BLOCK_LENGTH_4:
		psbc_decoder_t->block_length = 4;
		break;
	case RTK_BT_A2DP_SBC_BLOCK_LENGTH_8:
		psbc_decoder_t->block_length = 8;
		break;
	case RTK_BT_A2DP_SBC_BLOCK_LENGTH_12:
		psbc_decoder_t->block_length = 12;
		break;
	case RTK_BT_A2DP_SBC_BLOCK_LENGTH_16:
		psbc_decoder_t->block_length = 16;
		break;
	default:
		BT_LOGE("[A2DP] RTK Audio not support frequency %x \r\n", pa2dp_codec->sbc.sampling_frequency);
		return 1;
	}
	psbc_decoder_t->min_bitpool = pa2dp_codec->sbc.min_bitpool;
	psbc_decoder_t->max_bitpool = pa2dp_codec->sbc.max_bitpool;
	psbc_decoder_t->sbc_dec_mode = (sbc_channel_mode_t)SBC_MODE_STANDARD;
#if defined(RTK_BLE_AUDIO_BROADCAST_LOCAL_PLAY_SUPPORT) && RTK_BLE_AUDIO_BROADCAST_LOCAL_PLAY_SUPPORT
	play_flag = true;
#endif
	a2dp_audio_track_hdl = rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_SBC,
												  (float)APP_BT_DEFAULT_A2DP_PBP_AUDIO_LEFT_VOLUME,
												  (float)APP_BT_DEFAULT_A2DP_PBP_AUDIO_RIGHT_VOLUME,
												  channels,
												  psbc_decoder_t->sampling_frequency,
												  BT_AUDIO_FORMAT_PCM_16_BIT,
												  0,
												  (pcm_data_cb)rtk_bt_audio_decode_pcm_data_callback,
												  play_flag);
	if (!a2dp_audio_track_hdl) {
		BT_LOGE("[A2DP] bt audio track add fail \r\n");
		return 1;
	}

	return 0;
}

static uint16_t rtk_bt_a2dp_sbc_parse_encoder_struct(rtk_bt_a2dp_codec_t *pa2dp_codec, rtk_bt_sbc_encode_t *psbc_encoder_t)
{
	if (pa2dp_codec->sbc.allocation_method == RTK_BT_A2DP_SBC_ALLOCATION_METHOD_LOUDNESS) {
		psbc_encoder_t->alloc_method = SBC_ALLOCATION_METHOD_LOUDNESS; // align to sbc codec lib
	} else {
		psbc_encoder_t->alloc_method = SBC_ALLOCATION_METHOD_SNR; // align to sbc codec lib
	}
	if (pa2dp_codec->sbc.subbands == RTK_BT_A2DP_SBC_SUBBANDS_4) {
		psbc_encoder_t->subbands = 4; // align to sbc codec lib
	} else {
		psbc_encoder_t->subbands = 8; // align to sbc codec lib
	}
	switch (pa2dp_codec->sbc.channel_mode) {
	case RTK_BT_A2DP_SBC_CHANNEL_MODE_MONO:
		psbc_encoder_t->channel_mode = SBC_CHANNEL_MODE_MONO;
		break;
	case RTK_BT_A2DP_SBC_CHANNEL_MODE_DUAL_CHANNEL:
		psbc_encoder_t->channel_mode = SBC_CHANNEL_MODE_DUAL_CHANNEL;
		break;
	case RTK_BT_A2DP_SBC_CHANNEL_MODE_STEREO:
		psbc_encoder_t->channel_mode = SBC_CHANNEL_MODE_STEREO;
		break;
	case RTK_BT_A2DP_SBC_CHANNEL_MODE_JOINT_STEREO:
		psbc_encoder_t->channel_mode = SBC_CHANNEL_MODE_JOINT_STEREO;
		break;
	default:
		BT_LOGE("[A2DP] RTK Audio not support channel_mode %x \r\n", pa2dp_codec->sbc.channel_mode);
		return 1;
	}
	switch (pa2dp_codec->sbc.sampling_frequency) {
	case RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_16KHZ:
		psbc_encoder_t->sample_rate = (uint32_t)16000;
		break;
	case RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_32KHZ:
		psbc_encoder_t->sample_rate = (uint32_t)32000;
		break;
	case RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_44_1KHZ:
		psbc_encoder_t->sample_rate = (uint32_t)44100;
		break;
	case RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_48KHZ:
		psbc_encoder_t->sample_rate = (uint32_t)48000;
		break;
	default:
		BT_LOGE("[A2DP] RTK Audio not support frequency %x \r\n", pa2dp_codec->sbc.sampling_frequency);
		return 1;
	}
	switch (pa2dp_codec->sbc.block_length) {
	case RTK_BT_A2DP_SBC_BLOCK_LENGTH_4:
		psbc_encoder_t->blocks = 4;
		break;
	case RTK_BT_A2DP_SBC_BLOCK_LENGTH_8:
		psbc_encoder_t->blocks = 8;
		break;
	case RTK_BT_A2DP_SBC_BLOCK_LENGTH_12:
		psbc_encoder_t->blocks = 12;
		break;
	case RTK_BT_A2DP_SBC_BLOCK_LENGTH_16:
		psbc_encoder_t->blocks = 16;
		break;
	default:
		BT_LOGE("[A2DP] RTK Audio not support block_length %x \r\n", pa2dp_codec->sbc.block_length);
		return 1;
	}
	psbc_encoder_t->bitpool = pa2dp_codec->sbc.max_bitpool;
	psbc_encoder_t->sbc_enc_mode = (sbc_channel_mode_t)SBC_MODE_STANDARD;

	return 0;
}

static rtk_bt_evt_cb_ret_t app_bt_a2dp_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	uint8_t bd_addr[6];
	switch (evt_code) {

	case RTK_BT_A2DP_EVT_SDP_ATTR_INFO: {
		rtk_bt_a2dp_sdp_attr_info_t *p_info = (rtk_bt_a2dp_sdp_attr_info_t *)param;
		BT_LOGA("[A2DP] SDP Scan %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_info->bd_addr[5], p_info->bd_addr[4],
				p_info->bd_addr[3], p_info->bd_addr[2],
				p_info->bd_addr[1], p_info->bd_addr[0]);
	}
	break;

	case RTK_BT_A2DP_EVT_CONN_IND: {
		rtk_bt_a2dp_conn_ind_t *conn_ind = (rtk_bt_a2dp_conn_ind_t *)param;
		memcpy((void *)bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[A2DP] Receive A2DP connection from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_A2DP_EVT_CONN_CMPL: {
		rtk_bt_a2dp_conn_ind_t *conn_ind = (rtk_bt_a2dp_conn_ind_t *)param;
		src_a2dp_credits = APP_BT_A2DP_SRC_STREAM_MAX_CREDITS;
		memcpy((void *)remote_bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[A2DP] A2DP connection completion with %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				remote_bd_addr[5], remote_bd_addr[4], remote_bd_addr[3], remote_bd_addr[2], remote_bd_addr[1], remote_bd_addr[0]);
		BT_AT_PRINT("+BTA2DP:conn,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					remote_bd_addr[5], remote_bd_addr[4], remote_bd_addr[3], remote_bd_addr[2], remote_bd_addr[1], remote_bd_addr[0]);
		// set BR/EDR tpoll to 3.75ms
		uint16_t tpoll = 0x06;
		if (RTK_BT_OK == rtk_bt_br_gap_set_link_qos(remote_bd_addr, RTK_BT_BR_QOS_TYPE_BEST_EFFORT, tpoll)) {
			BT_LOGA("[A2DP] set link qos with %02x:%02x:%02x:%02x:%02x:%02x success, tpoll:0x%x \r\n",
					remote_bd_addr[5], remote_bd_addr[4], remote_bd_addr[3], remote_bd_addr[2], remote_bd_addr[1], remote_bd_addr[0],
					tpoll);
		}
	}
	break;

	case RTK_BT_A2DP_EVT_DISCONN_CMPL: {
		rtk_bt_a2dp_disconn_ind_t *disconn_ind = (rtk_bt_a2dp_disconn_ind_t *)param;
		rtk_bt_audio_codec_remove(audio_a2dp_codec_conf.codec_index, a2dp_codec_entity);
		rtk_bt_audio_track_del(audio_a2dp_codec_conf.codec_index, a2dp_audio_track_hdl);
		memset((void *)&audio_a2dp_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
		if (g_audio_resample_t) {
			app_bt_pcm_data_resample_engine_destroy(&g_audio_resample_t);
		}
		/* flush pcm data queue */
		{
			osif_mutex_take(app_pcm_data_mgr_queue.mtx, BT_TIMEOUT_FOREVER);
			app_queue_mgr_flush_queue(&app_pcm_data_mgr_queue);
			pbp_broadcast_dequeue_flag = false;
			osif_mutex_give(app_pcm_data_mgr_queue.mtx);
		}
		a2dp_audio_track_hdl = NULL;
		a2dp_codec_entity = NULL;
		memcpy((void *)bd_addr, disconn_ind->bd_addr, 6);
		BT_LOGA("[A2DP] A2DP disconnection completion with %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		BT_AT_PRINT("+BTA2DP:disconn,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_A2DP_EVT_CONFIG_CMPL: {
		uint16_t ret = 1;
		rtk_bt_a2dp_codec_t *pa2dp_codec = (rtk_bt_a2dp_codec_t *)param;
		if ((pa2dp_codec->codec_type & RTK_BT_AUDIO_CODEC_SBC/* | RTK_BT_AUDIO_CODEC_AAC */) == 0) {
			BT_LOGE("[A2DP] Not support codec %d \r\n", pa2dp_codec->codec_type);
			break;
		}

		if (a2dp_role == RTK_BT_A2DP_ROLE_SNK) {
			if (pa2dp_codec->codec_type == RTK_BT_AUDIO_CODEC_SBC) {
				ret = rtk_bt_a2dp_sbc_parse_decoder_struct(pa2dp_codec, &sbc_codec_t.decoder_t);
				audio_a2dp_codec_conf.codec_index = RTK_BT_AUDIO_CODEC_SBC;
				audio_a2dp_codec_conf.param = (void *)&sbc_codec_t;
				audio_a2dp_codec_conf.param_len = sizeof(sbc_codec_t);
			}
			if (ret) {
				BT_LOGE("[A2DP] RTK_BT_A2DP_EVT_CONFIG_CMPL Fail \r\n");
				//rtk_bt_audio_deinit();
				a2dp_audio_track_hdl = NULL;
				a2dp_codec_entity = NULL;
				break;
			}
		} else {
			if (pa2dp_codec->codec_type == RTK_BT_AUDIO_CODEC_SBC) {
				ret = rtk_bt_a2dp_sbc_parse_encoder_struct(pa2dp_codec, &sbc_codec_t.encoder_t);
				audio_a2dp_codec_conf.codec_index = RTK_BT_AUDIO_CODEC_SBC;
				audio_a2dp_codec_conf.param = (void *)&sbc_codec_t;
				audio_a2dp_codec_conf.param_len = sizeof(sbc_codec_t);
			}
		}
		a2dp_codec_entity = rtk_bt_audio_codec_add(&audio_a2dp_codec_conf);
		// bt_a2dp_sink_event_handle(A2DP_SINK_STREAM_DATA_HANDLE_INIT, A2DP_SBC_CODEC, 0, NULL);
		/* pcm convert engine init */
		app_bt_pcm_data_resample_engine_alloc(&g_audio_resample_t, sbc_codec_t.decoder_t.sampling_frequency, ((sbc_codec_t.decoder_t.channel_mode != 0) ? 2 : 1));
		BT_LOGA("[A2DP] Configure Complete CODEC %d \r\n", pa2dp_codec->codec_type);
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_OPEN: {
		BT_LOGA("[A2DP] A2DP STREAM is opened \r\n");
		rtk_bt_a2dp_stream_open_t *p_stream_open_t = (rtk_bt_a2dp_stream_open_t *)param;
		BT_LOGA("[A2DP] A2DP stream open with %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_stream_open_t->bd_addr[5], p_stream_open_t->bd_addr[4], p_stream_open_t->bd_addr[3], p_stream_open_t->bd_addr[2], p_stream_open_t->bd_addr[1],
				p_stream_open_t->bd_addr[0]);
		BT_LOGA("[A2DP] A2DP stream open max packet length is %d \r\n", (int)p_stream_open_t->max_pkt_len);
		BT_AT_PRINT("+BTA2DP:stream_open,%02x:%02x:%02x:%02x:%02x:%02x,%d\r\n",
					p_stream_open_t->bd_addr[5], p_stream_open_t->bd_addr[4], p_stream_open_t->bd_addr[3],
					p_stream_open_t->bd_addr[2], p_stream_open_t->bd_addr[1], p_stream_open_t->bd_addr[0],
					(int)p_stream_open_t->max_pkt_len);
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_START_IND: {
		rtk_bt_a2dp_stream_start_t *pa2dp_stream = (rtk_bt_a2dp_stream_start_t *)param;

		BT_LOGA("[A2DP] BT_EVENT_A2DP_STREAM_START_IND active_a2dp_idx %d, streaming_fg %d \r\n",
				pa2dp_stream->active_a2dp_link_index, pa2dp_stream->stream_cfg);
		BT_AT_PRINT("+BTA2DP:start,%d,%d\r\n",
					pa2dp_stream->active_a2dp_link_index, pa2dp_stream->stream_cfg);
		a2dp_play_flag = true;
		if (a2dp_audio_track_hdl) {
			rtk_bt_audio_track_resume(a2dp_audio_track_hdl->audio_track_hdl);
		}
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_START_RSP: {
		rtk_bt_a2dp_stream_start_t *pa2dp_stream = (rtk_bt_a2dp_stream_start_t *)param;
		BT_LOGA("[A2DP] RTK_BT_A2DP_EVT_STREAM_START_RSP active_a2dp_idx %d, streaming_fg %d \r\n",
				pa2dp_stream->active_a2dp_link_index, pa2dp_stream->stream_cfg);

	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_STOP: {
		rtk_bt_a2dp_conn_ind_t *conn_ind = (rtk_bt_a2dp_conn_ind_t *)param;
		memcpy((void *)bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[A2DP] Stream stop from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		BT_AT_PRINT("+BTA2DP:stop,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		a2dp_play_flag = false;
		pbp_broadcast_dequeue_flag = false;
		/* pkt drop flag reset*/
		if (pkt_drop_flag) {
			osif_mutex_take(pkt_drop_mtx, BT_TIMEOUT_FOREVER);
			pkt_drop_flag = false;
			pkt_drop_num = 0;
			osif_mutex_give(pkt_drop_mtx);
		}
		if (a2dp_audio_track_hdl) {
			rtk_bt_audio_track_pause(a2dp_audio_track_hdl->audio_track_hdl);
		}
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_CLOSE: {
		rtk_bt_a2dp_conn_ind_t *conn_ind = (rtk_bt_a2dp_conn_ind_t *)param;
		a2dp_play_flag = false;
		pbp_broadcast_dequeue_flag = false;
		/* pkt drop flag reset*/
		if (pkt_drop_flag) {
			osif_mutex_take(pkt_drop_mtx, BT_TIMEOUT_FOREVER);
			pkt_drop_flag = false;
			pkt_drop_num = 0;
			osif_mutex_give(pkt_drop_mtx);
		}
		memcpy((void *)bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[A2DP] Stream close from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_DATA_IND: { //BT api shall not be called here
		rtk_bt_a2dp_stream_data_ind_t *pdata_in = (rtk_bt_a2dp_stream_data_ind_t *)param;
		a2dp_play_flag = true;
		if (rtk_bt_audio_recvd_data_in(RTK_BT_AUDIO_CODEC_SBC, a2dp_audio_track_hdl, a2dp_codec_entity, pdata_in->data, pdata_in->length, 0)) {
			BT_LOGE("[A2DP] Stream Data Receiving FAIL %d \r\n", RTK_BT_AUDIO_CODEC_SBC);
		}
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_DATA_RSP: {
		if (a2dp_role == RTK_BT_A2DP_ROLE_SRC) {
			src_a2dp_credits++;
			if (src_a2dp_credits > APP_BT_A2DP_SRC_STREAM_MAX_CREDITS) {
				src_a2dp_credits = APP_BT_A2DP_SRC_STREAM_MAX_CREDITS;
			}
		}
	}
	break;

	default: {
		BT_LOGE("[A2DP]: default evt_code 0x%04x \r\n", evt_code);
	}
	break;
	}

	return RTK_BT_EVT_CB_OK;
}

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
#if defined(RTK_BLE_AUDIO_BROADCAST_LOCAL_PLAY_SUPPORT) && RTK_BLE_AUDIO_BROADCAST_LOCAL_PLAY_SUPPORT
				app_le_audio_data_path[i].p_track_hdl = app_bt_le_audio_track_add(&app_le_audio_data_path[i].codec_t);
				if (!app_le_audio_data_path[i].p_track_hdl) {
					BT_LOGE("[APP] %s track add fail \r\n", __func__);
					app_bt_le_audio_lc3_codec_entity_remove(app_le_audio_data_path[i].p_codec_entity);
					goto error;
				}
#endif
			}
			return 0;
error:
			memset((void *)&app_le_audio_data_path[i], 0, sizeof(app_bt_le_audio_data_path_t));
			break;
		}
	}
	BT_LOGE("[APP] %s: app_bt_le_audio_add_data_path fail, APP_LE_AUDIO_DEMO_DATA_PATH_NUM not enough! \r\n", __func__);
	return 1;
}

static uint16_t app_bt_le_audio_track_remove(void *audio_track_hdl)
{
	return rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_LC3, audio_track_hdl);
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
			if (app_le_audio_data_path[i].p_track_hdl) {
				app_bt_le_audio_track_remove(app_le_audio_data_path[i].p_track_hdl);
			}
			if (app_le_audio_data_path[i].p_codec_entity) {
				app_bt_le_audio_lc3_codec_entity_remove(app_le_audio_data_path[i].p_codec_entity);
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
			if (app_le_audio_data_path[i].p_track_hdl) {
				app_bt_le_audio_track_remove(app_le_audio_data_path[i].p_track_hdl);
			}
			if (app_le_audio_data_path[i].p_codec_entity) {
				app_bt_le_audio_lc3_codec_entity_remove(app_le_audio_data_path[i].p_codec_entity);
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

static void app_bt_pbp_bsrc_send_timer_handler(void *arg)
{
	(void)arg;
	bool pbp_tx_flag = false;
	uint32_t sample_rate = 0;
	uint32_t frame_duration_us = 0;
	uint16_t frame_num = 0;
	pbp_tx_flag = pbp_broadcast_dequeue_flag && a2dp_play_flag;
	if (app_bt_pbp_bsrc_encode_task.run) {
		for (uint16_t i = 0; i < APP_LE_AUDIO_DEMO_DATA_PATH_NUM; i ++) {
			if (app_le_audio_data_path[i].used && (app_le_audio_data_path[i].path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX)) {
				sample_rate = app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(app_le_audio_data_path[i].codec_t.sample_frequency);
				frame_duration_us = (app_le_audio_data_path[i].codec_t.frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500;
				frame_num = app_le_audio_data_path[i].codec_t.codec_frame_blocks_per_sdu;
				app_le_audio_data_path[i].pkt_seq_num ++;
				app_le_audio_data_path[i].time_stamp += sample_rate * frame_duration_us * frame_num / 1000 / 1000;
			}
		}
		if (pbp_tx_flag) {
			if (g_pbp_brc_encode_data_sem) {
				osif_sem_give(g_pbp_brc_encode_data_sem);
			}
		}
	}
}

static void app_bt_pbp_bsrc_send_timer_init(void)
{
#if defined(RTK_BLE_AUDIO_USE_HW_GTIMER) && RTK_BLE_AUDIO_USE_HW_GTIMER
	BT_LOGD("[APP] %s hw timer id %d,time_interval_us = %d\r\n", __func__, PBP_BRAODCAST_SOURCE_SEND_TIMER_ID, (int)g_pbp_bsrc_send_timer_interval_us);
	if (g_lea_pbp_bsrc_send_timer.handler == NULL) {
		gtimer_init(&g_lea_pbp_bsrc_send_timer, PBP_BRAODCAST_SOURCE_SEND_TIMER_ID);
		gtimer_start_periodical(&g_lea_pbp_bsrc_send_timer, g_pbp_bsrc_send_timer_interval_us, (void *)app_bt_pbp_bsrc_send_timer_handler, NULL);
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
									   app_bt_pbp_bsrc_send_timer_handler)) {
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

static uint16_t app_bt_le_audio_encode_data(app_bt_le_audio_data_path_t *p_iso_path, uint8_t out_channels)
{
#if 0
	uint32_t time_stamp_before;
#endif
	uint8_t ret = RTK_BT_OK;
	uint8_t bytes_per_num = (APP_LE_AUDIO_DEFAULT_PCM_BIT_WIDTH / 8);
	uint32_t encode_byte = 0, pcm_frame_size = 0;
	uint32_t sample_rate = 0, frame_duration_us = 0;
	rtk_bt_le_audio_cfg_codec_t *p_codec = NULL;
	uint16_t i, j = 0;
	int8_t drop_buf[1920] = {0};

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
	sample_rate = app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(p_codec->sample_frequency); // default 48K
	frame_duration_us = (p_codec->frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500;
	pcm_frame_size = sample_rate * frame_duration_us / 1000 / 1000;
	encode_byte = pcm_frame_size * out_channels * bytes_per_num;
	if (p_iso_path->p_encode_data == NULL) {
		p_iso_path->p_encode_data = (short *)osif_mem_alloc(RAM_TYPE_DATA_ON, encode_byte);
		if (p_iso_path->p_encode_data == NULL) {
			BT_LOGE("[APP] %s p_iso_path->p_encode_data alloc fail\r\n", __func__);
			return RTK_BT_FAIL;
		}
		memset(p_iso_path->p_encode_data, 0, encode_byte);
	}
#if 0
	time_stamp_before = osif_sys_time_get();
#endif
	/* dequeue from pcm data queue */
	if (NULL == app_pcm_data_mgr_queue.mtx) {
		BT_LOGE("[APP] %s: mtx is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	}
	osif_mutex_take(app_pcm_data_mgr_queue.mtx, BT_TIMEOUT_FOREVER);
	ret = app_queue_mgr_dequeue(&app_pcm_data_mgr_queue, (int8_t *)p_iso_path->p_encode_data, encode_byte);
	osif_mutex_give(app_pcm_data_mgr_queue.mtx);
#if 0
	BT_LOGA("[APP] queue_size: %u, cur_water_level:%d\r\n", app_pcm_data_mgr_queue.queue_size, app_pcm_data_mgr_queue.queue_size / 192);
#endif
	if (RTK_BT_OK != ret) {
		BT_LOGE("[APP] %s: dequeue num %d is not enough, set buf default 0!\r\n", __func__, (int)encode_byte);
		memset(p_iso_path->p_encode_data, 0, encode_byte);
		osif_mutex_take(pkt_drop_mtx, BT_TIMEOUT_FOREVER);
		pkt_drop_flag = true;
		pkt_drop_num++;
		osif_mutex_give(pkt_drop_mtx);
	}
	p_iso_path->encode_byte = encode_byte;
	/* encode */
	p_iso_path->p_enc_codec_buffer_t = rtk_bt_audio_data_encode(RTK_BT_AUDIO_CODEC_LC3, p_iso_path->p_codec_entity, p_iso_path->p_encode_data,
																p_iso_path->encode_byte);
	if (!p_iso_path->p_enc_codec_buffer_t) {
		BT_LOGE("[APP] %s rtk_bt_audio_data_encode fail\r\n", __func__);
		return RTK_BT_FAIL;
	}
	/* Discard packet in order to match A2DP rx and BIS tx traffic */
	if (pkt_drop_flag) {
		i = pkt_drop_num;
		for (j = 0; j < i; j++) {
			osif_mutex_take(app_pcm_data_mgr_queue.mtx, BT_TIMEOUT_FOREVER);
			ret = app_queue_mgr_dequeue(&app_pcm_data_mgr_queue, drop_buf, encode_byte);
			osif_mutex_give(app_pcm_data_mgr_queue.mtx);
			if (ret) {
				break;
			}
			osif_mutex_take(pkt_drop_mtx, BT_TIMEOUT_FOREVER);
			pkt_drop_num--;
			osif_mutex_give(pkt_drop_mtx);
			BT_LOGA("[APP] %s: drop packet success, current pkt_drop_num:%d \r\n", __func__, pkt_drop_num);
		}
		if (!pkt_drop_num) {
			osif_mutex_take(pkt_drop_mtx, BT_TIMEOUT_FOREVER);
			pkt_drop_flag = false;
			osif_mutex_give(pkt_drop_mtx);
		}
	}

	return RTK_BT_OK;
}

static void app_bt_pbp_bsrc_encode_task_entry(void *ctx)
{
	(void)ctx;
	uint16_t ret = RTK_BT_FAIL;
	app_bt_pbp_bsrc_encode_task.run = 1;
	//give sem each 10ms in
	while (app_bt_pbp_bsrc_encode_task.run) {
		if (g_pbp_brc_encode_data_sem) {
			osif_sem_take(g_pbp_brc_encode_data_sem, BT_TIMEOUT_FOREVER);
		}
		BT_LOGD("[APP] %s sys_time %d\r\n", __func__, (int)osif_sys_time_get());
		/* foreach iso tx app data path */
		for (uint16_t i = 0; i < APP_LE_AUDIO_DEMO_DATA_PATH_NUM; i ++) {
			if (app_le_audio_data_path[i].used && (app_le_audio_data_path[i].path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX)) {
				/* encode data */
				{
					uint8_t output_channels = 0;
					/* get encoding lc3 data channels */
					if (app_bt_le_audio_get_lea_chnl_num(app_le_audio_data_path[i].codec_t.audio_channel_allocation) > 1) {
						output_channels = 2;
					} else {
						output_channels = 1;
					}
					if (RTK_BT_OK != app_bt_le_audio_encode_data(&app_le_audio_data_path[i], output_channels)) {
						BT_LOGE("[APP] %s app_bt_le_audio_encode_data fail\r\n", __func__);
						continue;
					}
				}
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
#if defined(RTK_BLE_AUDIO_BROADCAST_LOCAL_PLAY_SUPPORT) && RTK_BLE_AUDIO_BROADCAST_LOCAL_PLAY_SUPPORT
					if (rtk_bt_audio_recvd_data_in(RTK_BT_AUDIO_CODEC_LC3,
												   app_le_audio_data_path[i].p_track_hdl,
												   app_le_audio_data_path[i].p_codec_entity,
												   app_le_audio_data_path[i].p_enc_codec_buffer_t->pbuffer,
												   app_le_audio_data_path[i].p_enc_codec_buffer_t->frame_size,
												   0)) {
						BT_LOGE("[APP] %s: Stream Data Play Fail! \r\n", __func__);
					}
#endif
					if (app_le_audio_data_path[i].p_enc_codec_buffer_t) {
						rtk_bt_audio_free_encode_buffer(RTK_BT_AUDIO_CODEC_LC3, app_le_audio_data_path[i].p_codec_entity, app_le_audio_data_path[i].p_enc_codec_buffer_t);
					}
					app_le_audio_data_path[i].p_enc_codec_buffer_t = NULL;
				}
			}
		}
	}
	BT_LOGD("[APP] %s task_delete\r\n", __func__);
	osif_sem_give(app_bt_pbp_bsrc_encode_task.sem);
	app_bt_pbp_bsrc_encode_task.run = 0;
	app_bt_pbp_bsrc_encode_task.hdl = NULL;
	osif_task_delete(NULL);
}

static void app_bt_le_audio_send_timer_update(uint32_t time_interval_us)
{
	BT_LOGA("[APP] %s: time_interval_us=%d\r\n", __func__, (int)time_interval_us);
	g_pbp_bsrc_send_timer_interval_us = time_interval_us;
#if defined(RTK_BLE_AUDIO_USE_HW_GTIMER) && RTK_BLE_AUDIO_USE_HW_GTIMER
	if (g_lea_pbp_bsrc_send_timer.handler) {
		gtimer_reload(&g_lea_pbp_bsrc_send_timer, time_interval_us);
	} else {
		BT_LOGE("[APP] %s: timer not init\r\n", __func__);
	}
#else
	if (g_lea_pbp_bsrc_send_timer) {
		if (false == osif_timer_stop(&g_lea_pbp_bsrc_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_stop fail \r\n", __func__);
		}
		if (false == osif_timer_restart(&g_lea_pbp_bsrc_send_timer, time_interval_us / 1000)) {
			BT_LOGE("[APP] %s osif_timer_start fail\r\n", __func__);
		}
	}
#endif
}

static void app_bt_le_audio_encode_data_control(bool enable)
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

		if (app_bt_pbp_bsrc_encode_task.sem == NULL) {
			osif_sem_create(&app_bt_pbp_bsrc_encode_task.sem, 0, 1);
		}

		if (app_bt_pbp_bsrc_encode_task.hdl == NULL) {
			if (false == osif_task_create(&app_bt_pbp_bsrc_encode_task.hdl, ((const char *)"pbp_bsrc_encode_task"), app_bt_pbp_bsrc_encode_task_entry,
										  NULL, APP_BT_PBP_BROADCAST_SOURCE_ENCODE_TASK_STACK_SIZE, APP_BT_PBP_BROADCAST_SOURCE_ENCODE_TASK_PRIO)) {
				BT_LOGE("[APP] %s xTaskCreate(lea_encode_task) failed\r\n", __func__);
				return ;
			}
		}
		app_bt_pbp_bsrc_send_timer_init();
	} else {
		if (g_pbp_brc_task_enable == false) {
			BT_LOGE("[APP] %s: encode task is alreay disabled\r\n", __func__);
			return ;
		}
		g_pbp_brc_task_enable = false;
		app_bt_le_audio_pbp_bsrc_send_timer_deinit();
		if (app_bt_pbp_bsrc_encode_task.hdl) {
			app_bt_pbp_bsrc_encode_task.run = 0;
			osif_sem_give(g_pbp_brc_encode_data_sem);
			osif_sem_take(app_bt_pbp_bsrc_encode_task.sem, BT_TIMEOUT_FOREVER);
			osif_sem_delete(app_bt_pbp_bsrc_encode_task.sem);
			app_bt_pbp_bsrc_encode_task.hdl = NULL;
			app_bt_pbp_bsrc_encode_task.sem = NULL;
		}
		if (g_pbp_brc_encode_data_sem) {
			osif_sem_delete(g_pbp_brc_encode_data_sem);
			g_pbp_brc_encode_data_sem = NULL;
		}
	}
}

static rtk_bt_evt_cb_ret_t app_bt_bap_callback(uint8_t evt_code, void *data, uint32_t len)
{
	(void)len;

	switch (evt_code) {
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
			// BIS sink play suspend when A2DP local play
			if (!a2dp_play_flag) {
				if (app_bt_le_audio_data_received(p_bt_direct_iso->iso_conn_handle, RTK_BLE_AUDIO_ISO_DATA_PATH_RX,
												  p_bt_direct_iso->p_buf + p_bt_direct_iso->offset,
												  p_bt_direct_iso->iso_sdu_len)) {
					BT_LOGE("[APP] %s app le audio data parsing fail \r\n", __func__);
					break;
				}
			}
		}
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_PA_SYNC_STATE_IND: {
		rtk_bt_le_audio_pa_sync_state_ind_t *param = (rtk_bt_le_audio_pa_sync_state_ind_t *)data;
		BT_LOGD("[APP] broadcast %s pa sync state change: sync_handle: %08x, sync_state 0x%x, action 0x%x, cause: 0x%x\r\n",
				(bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) ? "sink" : "assistant", param->sync_handle, param->sync_state, param->action, param->cause);
		BT_AT_PRINT("+BLEBAP:broadcast,%s,sync_state,%p,0x%x,0x%x,0x%x\r\n",
					(bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) ? "sink" : "assistant",
					param->sync_handle, param->sync_state, param->action, param->cause);
		if (param->sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZED) {
			BT_LOGA("[APP] broadcast %s pa sync synchronized\r\n", (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) ? "sink" : "assistant");
		} else if (param->sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZING_WAIT_SCANNING) {
			BT_LOGA("[APP] broadcast %s pa sync synchronizing wait scanning \r\n", (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) ? "sink" : "assistant");
		} else if (param->sync_state == RTK_BT_LE_AUDIO_PA_SYNC_STATE_TERMINATED) {
			BT_LOGA("[APP] broadcast %s pa sync terminated \r\n", (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) ? "sink" : "assistant");
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BIG_SETUP_DATA_PATH: {
		rtk_bt_le_audio_big_setup_data_path_ind_t *param = (rtk_bt_le_audio_big_setup_data_path_ind_t *)data;
		BT_LOGA("[APP] broadcast sink big setup data path ind: bis_conn_handle: %08x, idx 0x%x, cause: 0x%x\r\n",
				param->bis_conn_handle, param->bis_idx, param->cause);
		// A2DP local play suspend
		if (a2dp_audio_track_hdl) {
			rtk_bt_audio_track_pause(a2dp_audio_track_hdl->audio_track_hdl);
			rtk_bt_avrcp_pause(remote_bd_addr);
		}
		big_sync_handle = param->sync_handle;
		app_bt_le_audio_add_data_path(param->iso_chann_t.iso_conn_handle,
									  param->iso_chann_t.p_iso_chann,
									  param->iso_chann_t.path_direction,
									  param->codec_t);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BIG_REMOVE_DATA_PATH: {
		rtk_bt_le_audio_big_remove_data_path_ind_t *param = (rtk_bt_le_audio_big_remove_data_path_ind_t *)data;
		BT_LOGA("[APP] broadcast sink big remove data path ind: bis_conn_handle: 0x%x, idx 0x%x, cause: 0x%x\r\n",
				param->bis_conn_handle, param->bis_idx, param->cause);
		if (big_sync_handle && (big_sync_handle == param->sync_handle)) {
			big_sync_handle = NULL;
		}
		app_bt_le_audio_remove_data_path(param->bis_conn_handle, param->path_direction);
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
		/* stop escan previously to avoid insufficent RF bandwidth */
		rtk_bt_le_audio_ext_scan_act(false);
		lea_broadcast_start = true;
		app_bt_le_audio_add_data_path(param->iso_chann_t.iso_conn_handle,
									  param->iso_chann_t.p_iso_chann,
									  param->iso_chann_t.path_direction,
									  param->codec_t);
		if (param->iso_chann_t.path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
			app_bt_le_audio_encode_data_control(true);
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
		lea_broadcast_start = false;
		if (param->path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
			app_bt_le_audio_encode_data_control(false);
		}
		app_bt_le_audio_remove_data_path(param->bis_conn_handle, param->path_direction);
		/* flush pcm data queue */
		{
			osif_mutex_take(app_pcm_data_mgr_queue.mtx, BT_TIMEOUT_FOREVER);
			app_queue_mgr_flush_queue(&app_pcm_data_mgr_queue);
			pbp_broadcast_dequeue_flag = false;
			osif_mutex_give(app_pcm_data_mgr_queue.mtx);
		}
		break;
	}

	default:
		BT_LOGE("[APP] default evt_code 0x%04x\r\n", evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

/* Le Gap APP Callback */
static rtk_bt_evt_cb_ret_t app_bt_le_audio_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	char *role;
	char le_addr[30] = {0};

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
	case RTK_BT_LE_GAP_EVT_DATA_LEN_CHANGE_IND: {
		rtk_bt_le_data_len_change_ind_t *data_len_change =
			(rtk_bt_le_data_len_change_ind_t *)param;
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
		if (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) {
			app_bt_le_audio_scan_report_handle(scan_res_ind, APP_BT_LE_AUDIO_BASS_SCAN_TYPE);
		}
		break;
	}
#endif

	case RTK_BT_LE_GAP_EVT_SCAN_STOP_IND: {
		rtk_bt_le_scan_stop_ind_t *scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)param;
		if (!scan_stop_ind->err) {
			BT_LOGA("[APP] Scan stopped, reason: 0x%x\r\n", scan_stop_ind->stop_reason);
			if (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK) {
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
					conn_ind->conn_handle, role, le_addr);
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
			BT_LOGA("[APP] Conn param is updated, conn_handle: %d, conn_interval: 0x%x, "       \
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

	default:
		BT_LOGE("[APP] Unknown gap cb evt type: %d\r\n", evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static uint16_t rtk_bt_hfp_cvsd_parse_decoder_struct(rtk_bt_hfp_codec_t *phfp_codec, rtk_bt_cvsd_decode_t *pcvsd_decoder_t)
{
	pcvsd_decoder_t->channel_num = phfp_codec->cvsd.channel_num;
	pcvsd_decoder_t->sample_rate = phfp_codec->cvsd.sample_rate;
	pcvsd_decoder_t->frame_duration = phfp_codec->cvsd.frame_duration;
	hfp_audio_track_hdl = rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_CVSD, (float)APP_BT_DEFAULT_A2DP_PBP_AUDIO_LEFT_VOLUME, (float)0, pcvsd_decoder_t->channel_num,
												 pcvsd_decoder_t->sample_rate, BT_AUDIO_FORMAT_PCM_16_BIT, 0, NULL, true);
	if (!hfp_audio_track_hdl) {
		BT_LOGE("[HFP] bt audio track add fail \r\n");
		return 1;
	}
	hfp_audio_record_hdl = rtk_bt_audio_record_add(RTK_BT_AUDIO_CODEC_CVSD, 1, 8000, 0, 0x7f);
	if (!hfp_audio_record_hdl) {
		BT_LOGE("[HFP] bt audio record add fail \r\n");
		rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_CVSD, hfp_audio_track_hdl);
		hfp_audio_track_hdl = NULL;
		return 1;
	}

	return 0;
}

static void hfp_task_entry(void *ctx)
{
	(void)ctx;
	static int read_size = 0;
	int16_t voice_buffer[30] = {0};
	struct enc_codec_buffer *penc_codec_buffer_t = NULL;
	rtk_bt_hfp_sco_data_send_t sco_data_t = {0};

	osif_sem_give(hfp_task.sem);

	while (hfp_task.run) {
		read_size = rtk_bt_audio_record_data_get(RTK_BT_AUDIO_CODEC_CVSD, hfp_audio_record_hdl, hfp_codec_entity, (void *)voice_buffer, 30 * sizeof(int16_t),
												 true);
		osif_delay(1);
		// BT_LOGA("get size %d \r\n", read_size);
		if (read_size) {
			penc_codec_buffer_t = rtk_bt_audio_data_encode(RTK_BT_AUDIO_CODEC_CVSD, hfp_codec_entity, voice_buffer, read_size / 2);
			if (!penc_codec_buffer_t) {
				BT_LOGE("[HFP]get encode buffer fail \r\n");
				continue;
			}
			memcpy((void *)sco_data_t.bd_addr, (void *)remote_bd_addr, 6);
			sco_data_t.buf = (uint8_t *)penc_codec_buffer_t->pbuffer;
			sco_data_t.len = (uint8_t)penc_codec_buffer_t->frame_size;
			sco_data_t.seq_num = (uint8_t)sco_seq_num;
			rtk_bt_hfp_data_send(&sco_data_t);
			sco_seq_num ++;
			rtk_bt_audio_free_encode_buffer(audio_hfp_codec_conf.codec_index, hfp_codec_entity, penc_codec_buffer_t);
		}
	}

	osif_sem_give(hfp_task.sem);
	osif_task_delete(NULL);
}

static void hfp_alert_timer_handle(void *arg)
{
	(void)arg;
	uint32_t pcm_offset = 0;

	while (pcm_offset < birds_sing_size / 2) {
		if (pcm_offset + 512 < birds_sing_size / 2) {
			if (alert_track_hdl) {
				rtk_bt_audio_track_play(alert_track_hdl, (void *)(birds_sing + pcm_offset), (uint16_t)1024);
			}
		} else {
			if (alert_track_hdl) {
				rtk_bt_audio_track_play(alert_track_hdl, (void *)(birds_sing + pcm_offset), (uint16_t)(((birds_sing_size / 2) - pcm_offset) * 2));
			}
		}
		pcm_offset += 512;
	}
	if (alert_timer) {
		osif_timer_start(&alert_timer);
	}
}

static void app_hfp_ring_alert_start(void)
{
	BT_LOGA("[HFP] Create alert timer \r\n");
	if (false == osif_timer_create(&alert_timer, "hfp_alert_timer", NULL,
								   1000,
								   false,
								   hfp_alert_timer_handle)) {
		BT_LOGE("[HFP] Create alert timer fail \r\n");
		return;
	}
	alert_track_hdl = rtk_bt_audio_track_init((uint32_t)2, (uint32_t)48000, BT_AUDIO_FORMAT_PCM_16_BIT, 1024, 0, 0);
	if (!alert_track_hdl) {
		BT_LOGE("[HFP]alert track init fail \r\n");
		osif_timer_delete(&alert_timer);
		return;
	}
	rtk_bt_audio_track_start(alert_track_hdl, 1, 1);
	osif_timer_start(&alert_timer);
}

static void app_hfp_ring_alert_stop(void)
{
	void *hdl_temp = NULL;

	if (alert_timer) {
		hdl_temp = alert_timer;
		alert_timer = NULL;
		osif_timer_stop(&hdl_temp);
		osif_timer_delete(&hdl_temp);
	}
	if (alert_track_hdl) {
		hdl_temp = alert_track_hdl;
		alert_track_hdl = NULL;
		rtk_bt_audio_track_stop(hdl_temp);
		rtk_bt_audio_track_deinit(hdl_temp);
	}
}

static rtk_bt_evt_cb_ret_t app_bt_hfp_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	uint8_t bd_addr[6];

	switch (evt_code) {

	case RTK_BT_HFP_EVT_SDP_ATTR_INFO: {
		rtk_bt_hfp_sdp_attr_info_t *p_info = (rtk_bt_hfp_sdp_attr_info_t *)param;
		BT_LOGA("[HFP] SDP Scan %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_info->bd_addr[5], p_info->bd_addr[4],
				p_info->bd_addr[3], p_info->bd_addr[2],
				p_info->bd_addr[1], p_info->bd_addr[0]);
	}
	break;

	case RTK_BT_HFP_EVT_SUPPORTED_FEATURES_IND: {
		rtk_bt_hfp_ag_features_ind_t *p_features_ind = (rtk_bt_hfp_ag_features_ind_t *)param;
		if (p_features_ind->capability & RTK_BT_HFP_AG_LOCAL_CAPABILITY_INBAND_RINGING) {
			ring_alert_inband = true;
		} else {
			ring_alert_inband = false;
		}
		BT_LOGA("[HFP] ring alert inband is %d \r\n", ring_alert_inband);
	}
	break;

	case RTK_BT_HFP_EVT_CONN_IND: {
		rtk_bt_hfp_conn_ind_t *conn_ind = (rtk_bt_hfp_conn_ind_t *)param;
		memcpy((void *)bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive HFP connection from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_HFP_EVT_CONN_CMPL: {
		rtk_bt_hfp_conn_ind_t *conn_ind = (rtk_bt_hfp_conn_ind_t *)param;
		memcpy((void *)bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive HFP connection completion from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_HFP_EVT_CALL_STATUS: {
		rtk_bt_hfp_call_status_ind_t *p_hfp_call_status_ind = (rtk_bt_hfp_call_status_ind_t *)param;
		memcpy((void *)bd_addr, p_hfp_call_status_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive HFP call status change from %d to %d from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_hfp_call_status_ind->prev_status,
				p_hfp_call_status_ind->curr_status,
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		call_curr_status = p_hfp_call_status_ind->curr_status;
		if ((RTK_BT_HFP_CALL_INCOMING != p_hfp_call_status_ind->prev_status) && (RTK_BT_HFP_CALL_INCOMING == p_hfp_call_status_ind->curr_status)) {
			if (!ring_alert_inband) {
				BT_LOGA("[HFP] ring alert outband active \r\n");
				app_hfp_ring_alert_start();
			}
		} else if ((RTK_BT_HFP_CALL_INCOMING == p_hfp_call_status_ind->prev_status) && (RTK_BT_HFP_CALL_INCOMING != p_hfp_call_status_ind->curr_status)) {
			if (!ring_alert_inband) {
				BT_LOGA("[HFP] ring alert outband inactive \r\n");
				app_hfp_ring_alert_stop();
			}
		}
	}
	break;

	case RTK_BT_HFP_EVT_CALLER_ID_IND: {
		rtk_bt_hfp_caller_id_ind_t *p_hfp_caller_id_ind = (rtk_bt_hfp_caller_id_ind_t *)param;
		memcpy((void *)bd_addr, p_hfp_caller_id_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive HFP caller id indication from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		BT_LOGA("[HFP] caller number is %s type is %d \r\n", p_hfp_caller_id_ind->number, p_hfp_caller_id_ind->type);
	}
	break;

	case RTK_BT_HFP_EVT_DISCONN_CMPL: {
		rtk_bt_hfp_disconn_ind_t *disconn_ind = (rtk_bt_hfp_disconn_ind_t *)param;
		memcpy((void *)bd_addr, disconn_ind->bd_addr, 6);
		BT_LOGA("[HFP] HFP disconnection completion with %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_HFP_EVT_SCO_CONN_IND: {
		rtk_bt_hfp_conn_ind_t *conn_ind = (rtk_bt_hfp_conn_ind_t *)param;
		memcpy((void *)bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive HFP SCO connection from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_HFP_EVT_HF_BATTERY_IND: {
		rtk_bt_hfp_hf_battery_ind_t *p_hfp_batt_ind = (rtk_bt_hfp_hf_battery_ind_t *)param;
		memcpy((void *)bd_addr, p_hfp_batt_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive HFP BATTERY IND REQ from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		*p_hfp_batt_ind->ret_info = battery_power;
	}
	break;

	case RTK_BT_HFP_EVT_HF_SPEAKER_VOL_CHANGED_IND: {
		float audio_track_volume = 0;
		rtk_bt_hfp_hf_speaker_volume_changed_ind_t *p_param_t = (rtk_bt_hfp_hf_speaker_volume_changed_ind_t *)param;
		memcpy((void *)bd_addr, p_param_t->bd_addr, 6);
		audio_track_volume = 1.0 * p_param_t->volume / 15;
		BT_LOGA("[HFP] Receive speaker volume changed from %02x:%02x:%02x:%02x:%02x:%02x, %.2f \r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0], audio_track_volume);
		rtk_bt_audio_track_set_hardware_volume(audio_track_volume, audio_track_volume);
	}
	break;

	case RTK_BT_HFP_EVT_HF_MIC_VOL_CHANGED_IND: {
		uint32_t audio_mic_volume = 0;
		rtk_bt_hfp_hf_mic_volume_changed_ind_t *p_param_t = (rtk_bt_hfp_hf_mic_volume_changed_ind_t *)param;
		memcpy((void *)bd_addr, p_param_t->bd_addr, 6);
		audio_mic_volume = 0xaf * p_param_t->volume / 15;
		BT_LOGA("[HFP] Receive mic volume changed from %02x:%02x:%02x:%02x:%02x:%02x, 0x%x \r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0], (unsigned int)audio_mic_volume);
		rtk_bt_audio_record_set_capture_volume(4, audio_mic_volume);
	}
	break;

	case RTK_BT_HFP_EVT_SCO_CONN_CMPL: {
		uint16_t ret = 1;
		rtk_bt_hfp_codec_t *phfp_codec = (rtk_bt_hfp_codec_t *)param;

		BT_LOGA("[HFP] Receive HFP SCO connection completion with %02x:%02x:%02x:%02x:%02x:%02x \r\n",
				phfp_codec->bd_addr[5], phfp_codec->bd_addr[4], phfp_codec->bd_addr[3], phfp_codec->bd_addr[2], phfp_codec->bd_addr[1], phfp_codec->bd_addr[0]);
		BT_AT_PRINT("+BTHFP:sco_conn,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					phfp_codec->bd_addr[5], phfp_codec->bd_addr[4], phfp_codec->bd_addr[3],
					phfp_codec->bd_addr[2], phfp_codec->bd_addr[1], phfp_codec->bd_addr[0]);
		/* stop escan previously to avoid insufficent RF bandwidth */
		rtk_bt_le_audio_ext_scan_act(false);
		/* pause a2dp firstly */
		if (a2dp_audio_track_hdl) {
			rtk_bt_avrcp_pause(remote_bd_addr);
		}
		/* if has sync to BIG, terminate it */
		if (big_sync_handle) {
			rtk_bt_le_audio_broadcast_big_sync_terminate_by_handle(big_sync_handle);
		}
		if ((phfp_codec->codec_type & (RTK_BT_AUDIO_CODEC_CVSD /* | RTK_BT_AUDIO_CODEC_mSBC */)) == 0) {
			BT_LOGE("[HFP] Not support codec %d \r\n", phfp_codec->codec_type);
			break;
		}
		if (rtk_bt_audio_init()) {
			BT_LOGE("[HFP] rtk_bt_audio_init fail \r\n");
			break;
		}
		if (phfp_codec->codec_type == RTK_BT_AUDIO_CODEC_CVSD) {
			ret = rtk_bt_hfp_cvsd_parse_decoder_struct(phfp_codec, &cvsd_codec_t.decoder_t);
			audio_hfp_codec_conf.codec_index = RTK_BT_AUDIO_CODEC_CVSD;
			audio_hfp_codec_conf.param = (void *)&cvsd_codec_t;
			audio_hfp_codec_conf.param_len = sizeof(cvsd_codec_t);
		}
		if (ret) {
			BT_LOGE("[HFP] RTK_BT_HFP_EVT_SCO_CONN_CMPL Fail \r\n");
			hfp_audio_track_hdl = NULL;
			hfp_audio_record_hdl = NULL;
			break;
		}
		hfp_codec_entity = rtk_bt_audio_codec_add(&audio_hfp_codec_conf);
		BT_LOGA("[HFP] Configure Complete CODEC %d \r\n", phfp_codec->codec_type);
		/* config audio record thread */
		{
			BT_LOGA("[HFP] Create Record Demo \r\n");
			if (false == osif_sem_create(&hfp_task.sem, 0, 1)) {
				BT_LOGE("[HFP] Create Record Demo Fail\r\n");
				return 1;
			}
			hfp_task.run = 1;
			if (false == osif_task_create(&hfp_task.hdl, "hfp_task",
										  hfp_task_entry, NULL,
										  4096, 4)) {
				osif_sem_delete(hfp_task.sem);
				return 1;
			}
			osif_sem_take(hfp_task.sem, 0xffffffff);
		}
	}
	break;

	case RTK_BT_HFP_EVT_SCO_DATA_IND: {
		rtk_bt_hfp_sco_data_ind_t *pdata_in = (rtk_bt_hfp_sco_data_ind_t *)param;

		if (rtk_bt_audio_recvd_data_in(RTK_BT_AUDIO_CODEC_CVSD, hfp_audio_track_hdl, hfp_codec_entity, pdata_in->data, pdata_in->length, 0)) {
			BT_LOGE("[HFP] SCO Data Receiving FAIL %d \r\n", RTK_BT_AUDIO_CODEC_CVSD);
		}
	}
	break;

	case RTK_BT_HFP_EVT_SCO_DISCONNCTED_IND: {
		rtk_bt_hfp_disconn_ind_t *disconn_ind = (rtk_bt_hfp_disconn_ind_t *)param;
		memcpy((void *)bd_addr, disconn_ind->bd_addr, 6);
		BT_LOGE("[HFP] HFP receive sco disconnected from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		BT_AT_PRINT("+BTHFP:sco_disconn,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		hfp_task.run = 0;
		if (false == osif_sem_take(hfp_task.sem, 0xffffffffUL)) {
			return 1;
		}
		osif_sem_delete(hfp_task.sem);
		hfp_task.hdl = NULL;
		hfp_task.sem = NULL;
		rtk_bt_audio_codec_remove(audio_hfp_codec_conf.codec_index, hfp_codec_entity);
		memset((void *)&audio_hfp_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
		rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_CVSD, hfp_audio_track_hdl);
		rtk_bt_audio_record_del(RTK_BT_AUDIO_CODEC_CVSD, hfp_audio_record_hdl);
		hfp_audio_track_hdl = NULL;
		hfp_audio_record_hdl = NULL;
		hfp_codec_entity = NULL;
		call_curr_status = 0;
	}
	break;

	default: {
		BT_LOGE("[HFP]: default evt_code 0x%04x \r\n", evt_code);
	}
	break;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t app_bt_pbap_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;

	switch (evt_code) {

	case RTK_BT_PBAP_EVT_CONN_CMPL: {
		rtk_bt_pbap_conn_ind_t *p_pbap_conn_ind = (rtk_bt_pbap_conn_ind_t *)param;
		BT_LOGA("[PBAP] Receive PBAP connection completion from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_pbap_conn_ind->bd_addr[5], p_pbap_conn_ind->bd_addr[4], p_pbap_conn_ind->bd_addr[3], p_pbap_conn_ind->bd_addr[2], p_pbap_conn_ind->bd_addr[1],
				p_pbap_conn_ind->bd_addr[0]);
	}
	break;

	case RTK_BT_PBAP_EVT_CONN_FAIL: {
		rtk_bt_pbap_conn_fail_ind_t *p_pbap_conn_fail_ind = (rtk_bt_pbap_conn_fail_ind_t *)param;
		BT_LOGA("[PBAP] PBAP connection fail with %02x:%02x:%02x:%02x:%02x:%02x, cause is 0x%02x \r\n",
				p_pbap_conn_fail_ind->bd_addr[5], p_pbap_conn_fail_ind->bd_addr[4], p_pbap_conn_fail_ind->bd_addr[3], p_pbap_conn_fail_ind->bd_addr[2],
				p_pbap_conn_fail_ind->bd_addr[1], p_pbap_conn_fail_ind->bd_addr[0],
				p_pbap_conn_fail_ind->cause);
	}
	break;

	case RTK_BT_PBAP_EVT_DISCONN_CMPL: {
		rtk_bt_pbap_disconn_cmpl_t *p_pbap_disc_cmpl_ind = (rtk_bt_pbap_disconn_cmpl_t *)param;
		BT_LOGA("[PBAP] PBAP disconnect completion with %02x:%02x:%02x:%02x:%02x:%02x, cause is 0x%02x \r\n",
				p_pbap_disc_cmpl_ind->bd_addr[5], p_pbap_disc_cmpl_ind->bd_addr[4], p_pbap_disc_cmpl_ind->bd_addr[3], p_pbap_disc_cmpl_ind->bd_addr[2],
				p_pbap_disc_cmpl_ind->bd_addr[1], p_pbap_disc_cmpl_ind->bd_addr[0],
				p_pbap_disc_cmpl_ind->cause);
	}
	break;

	case RTK_BT_PBAP_EVT_SET_PHONE_BOOK_CMPL: {
		rtk_bt_pbap_set_pb_cmpl_t *p_pbap_set_pb_cmpl = (rtk_bt_pbap_set_pb_cmpl_t *)param;
		BT_LOGA("[PBAP] PBAP set phone book completion with %02x:%02x:%02x:%02x:%02x:%02x, result is %d, path is 0x%02x \r\n",
				p_pbap_set_pb_cmpl->bd_addr[5], p_pbap_set_pb_cmpl->bd_addr[4], p_pbap_set_pb_cmpl->bd_addr[3], p_pbap_set_pb_cmpl->bd_addr[2], p_pbap_set_pb_cmpl->bd_addr[1],
				p_pbap_set_pb_cmpl->bd_addr[0],
				(int)p_pbap_set_pb_cmpl->result,
				(uint8_t)p_pbap_set_pb_cmpl->path);
	}
	break;

	case RTK_BT_PBAP_EVT_GET_PHONE_BOOK_CMPL: {
		rtk_bt_pbap_get_pb_cmpl_t *p_pbap_get_pb_cmpl = (rtk_bt_pbap_get_pb_cmpl_t *)param;
		BT_LOGA("[PBAP] PBAP get phone book completion with %02x:%02x:%02x:%02x:%02x:%02x, data_len is %d, pb_size is %d, new_missed_calls is %d, data_end %d \r\n",
				p_pbap_get_pb_cmpl->bd_addr[5], p_pbap_get_pb_cmpl->bd_addr[4], p_pbap_get_pb_cmpl->bd_addr[3], p_pbap_get_pb_cmpl->bd_addr[2], p_pbap_get_pb_cmpl->bd_addr[1],
				p_pbap_get_pb_cmpl->bd_addr[0],
				(int)p_pbap_get_pb_cmpl->data_len,
				(int)p_pbap_get_pb_cmpl->pb_size,
				(int)p_pbap_get_pb_cmpl->new_missed_calls,
				(int)p_pbap_get_pb_cmpl->data_end);
		if (p_pbap_get_pb_cmpl->data_len) {
			BT_LOGA(" phone book detail \r\n ");
			BT_LOGA(" %s \r\n", p_pbap_get_pb_cmpl->p_data);
			osif_mem_free(p_pbap_get_pb_cmpl->p_data);
		}
	}
	break;

	case RTK_BT_PBAP_EVT_CALLER_ID_NAME: {
		rtk_bt_pbap_caller_id_name_t *p_pbap_caller_id_name = (rtk_bt_pbap_caller_id_name_t *)param;
		if (p_pbap_caller_id_name->name_len) {
			BT_LOGA("[PBAP] PBAP pull caller id name with %02x:%02x:%02x:%02x:%02x:%02x, name is %s \r\n",
					p_pbap_caller_id_name->bd_addr[5], p_pbap_caller_id_name->bd_addr[4], p_pbap_caller_id_name->bd_addr[3], p_pbap_caller_id_name->bd_addr[2],
					p_pbap_caller_id_name->bd_addr[1], p_pbap_caller_id_name->bd_addr[0],
					(char *)p_pbap_caller_id_name->name_ptr);
			osif_mem_free(p_pbap_caller_id_name->name_ptr);
		}
	}
	break;

	case RTK_BT_PBAP_EVT_GET_PHONE_BOOK_SIZE_CMPL: {
		rtk_bt_pbap_get_pb_size_cmpl *p_pbap_get_pb_size = (rtk_bt_pbap_get_pb_size_cmpl *)param;
		BT_LOGA("[PBAP] PBAP get phone book size with %02x:%02x:%02x:%02x:%02x:%02x, pb_size is %d, new_missed_calls is %d \r\n",
				p_pbap_get_pb_size->bd_addr[5], p_pbap_get_pb_size->bd_addr[4], p_pbap_get_pb_size->bd_addr[3], p_pbap_get_pb_size->bd_addr[2], p_pbap_get_pb_size->bd_addr[1],
				p_pbap_get_pb_size->bd_addr[0],
				(int)p_pbap_get_pb_size->pb_size,
				(int)p_pbap_get_pb_size->new_missed_calls);
	}
	break;

	default: {
		BT_LOGE("[PBAP]: default evt_code 0x%04x \r\n", evt_code);
	}
	break;
	}

	return RTK_BT_EVT_CB_OK;
}

int bt_a2dp_hfp_pbp_main(uint8_t enable)
{
	if (1 == enable) {
		if (demo_init_flag) {
			BT_LOGE("%s Already init! \r\n", __func__);
			return -1;
		}
		rtk_bt_app_conf_t bt_app_conf = {0};
		rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &bt_le_audio_app_conf;
		rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
		char addr_str[30] = {0};
		char dev_name[30] = {0};
		uint16_t tx_water_level = 0;
		/* LE Audio app configuration */
		{
			p_lea_app_conf->bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SOUR |
									   RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK |
									   RTK_BT_LE_AUDIO_BAP_ROLE_SCAN_DELE;
			p_lea_app_conf->cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR |
									   RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR;
			p_lea_app_conf->pacs_param.sink_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR;
			p_lea_app_conf->pacs_param.source_audio_location = RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR;
			p_lea_app_conf->pacs_param.sink_available_contexts = RTK_BT_LE_AUDIO_CONTEXT_UNSPECIFIED | RTK_BT_LE_AUDIO_CONTEXT_MEDIA;
			p_lea_app_conf->pacs_param.sink_supported_contexts = RTK_BT_LE_AUDIO_CONTEXT_UNSPECIFIED | RTK_BT_LE_AUDIO_CONTEXT_MEDIA;
			p_lea_app_conf->pacs_param.source_available_contexts = RTK_BT_LE_AUDIO_CONTEXT_MEDIA;
			p_lea_app_conf->pacs_param.source_supported_contexts = RTK_BT_LE_AUDIO_CONTEXT_MEDIA;
			p_lea_app_conf->pacs_param.p_sink_pac_id = &bt_le_audio_sink_pac_id;
			p_lea_app_conf->pacs_param.p_pac_sink_codec = bt_le_audio_pac_sink_codec;
			p_lea_app_conf->pacs_param.pac_sink_codec_len = sizeof(bt_le_audio_pac_sink_codec);
			p_lea_app_conf->pacs_param.p_source_pac_id = &bt_le_audio_source_pac_id;
			p_lea_app_conf->pacs_param.p_pac_source_codec = bt_le_audio_pac_source_codec;
			p_lea_app_conf->pacs_param.pac_source_codec_len = sizeof(bt_le_audio_pac_source_codec);
			memset((void *)p_lea_app_conf->device_name, 0, RTK_BT_GAP_DEVICE_NAME_LEN);
			memcpy((void *)p_lea_app_conf->device_name, (uint8_t *)APP_A2DP_SINK_HFP_HF_PBP_DEVICE_NAME,
				   strlen((const char *)APP_A2DP_SINK_HFP_HF_PBP_DEVICE_NAME));
			p_lea_app_conf->device_name_len = strlen((const char *)APP_A2DP_SINK_HFP_HF_PBP_DEVICE_NAME);
		}
		/* set GAP config */
		{
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS | \
											  RTK_BT_PROFILE_SDP | \
											  RTK_BT_PROFILE_A2DP | \
											  RTK_BT_PROFILE_HFP | \
											  RTK_BT_PROFILE_PBAP | \
											  RTK_BT_PROFILE_AVRCP | \
											  RTK_BT_PROFILE_LEAUDIO | \
											  RTK_BT_PROFILE_BAP | \
											  RTK_BT_PROFILE_CAP;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = RTK_BT_LE_PHYS_PREFER_ALL;
			bt_app_conf.prefer_tx_phy = RTK_BT_LE_PHYS_PREFER_1M | RTK_BT_LE_PHYS_PREFER_2M | RTK_BT_LE_PHYS_PREFER_CODED;
			bt_app_conf.prefer_rx_phy = RTK_BT_LE_PHYS_PREFER_1M | RTK_BT_LE_PHYS_PREFER_2M | RTK_BT_LE_PHYS_PREFER_CODED;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			bt_app_conf.a2dp_role = RTK_BT_A2DP_ROLE_SNK;
			bt_app_conf.hfp_role = RTK_BT_AUDIO_HFP_ROLE_HF;
			memcpy((void *)&bt_app_conf.le_audio_app_conf, (void *)p_lea_app_conf, sizeof(rtk_bt_le_audio_app_conf_t));
		}
		/* A2DP sink part */
		{
			rtk_bt_br_gap_default_param_t dft_param;
			dft_param.mtu_size = 180;
			BT_APP_PROCESS(rtk_bt_br_gap_set_default_param(&dft_param));
			/* set class of device */
			BT_APP_PROCESS(rtk_bt_br_gap_set_cod(&cod));
			/* set supervision_timeout */
			BT_APP_PROCESS(rtk_bt_br_gap_set_supvisiontimeout(&superv_time));
			BT_APP_PROCESS(rtk_bt_br_gap_set_inquiry_param(&inquiry_param));
			BT_APP_PROCESS(rtk_bt_br_gap_set_page_param(&page_param));
			/* sdp record add (must be invoked before rtk_bt_enable) */
			BT_APP_PROCESS(rtk_sdp_record_add((void *)did_sdp_record, sizeof(did_sdp_record)));
			/* a2dp sink record add */
			BT_APP_PROCESS(rtk_sdp_record_add((void *)a2dp_sink_sdp_record, sizeof(a2dp_sink_sdp_record)));
			BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_ct_sdp_record, sizeof(avrcp_ct_sdp_record)));
			BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_tg_sdp_record, sizeof(avrcp_tg_sdp_record)));
			/* a2dp codec add (must be invoked before rtk_bt_enable) */
			BT_APP_PROCESS(rtk_bt_a2dp_codec_cfg((uint8_t)RTK_BT_AUDIO_CODEC_SBC, (void *)&codec_sbc, (uint32_t)sizeof(rtk_bt_a2dp_media_codec_sbc_t)));
		}
		/* HFP Hand Free part */
		{
			BT_APP_PROCESS(rtk_sdp_record_add((void *)hfp_hf_sdp_record, sizeof(hfp_hf_sdp_record)));
			BT_APP_PROCESS(rtk_sdp_record_add((void *)pbap_pce_sdp_record, sizeof(pbap_pce_sdp_record)));
			BT_APP_PROCESS(rtk_bt_hfp_cfg((uint8_t)RTK_BT_AUDIO_HFP_ROLE_HF, (void *)&bt_hfp_hf_conf));
		}
		/* Enable BT */
		BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
		/* Enable BR/EDR inquiry scan and page scan */
		BT_APP_PROCESS(rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_VISIBLE_CONNECTABLE));
		BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
		rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
		BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
		/* Initilize GAP part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_audio_gap_app_callback));
		BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(p_lea_app_conf->device_name));
		BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_GENERIC_MEDIA_PLAYER));
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_GAP, br_gap_app_callback));
		/* A2DP part */
		{
			/* mix RTK_BT_DEV_NAME with bt mac address */
			strcpy(dev_name, APP_A2DP_SINK_HFP_HF_PBP_DEVICE_NAME);
			snprintf(&dev_name[strlen(APP_A2DP_SINK_HFP_HF_PBP_DEVICE_NAME)], 7, "(%02X%02X)", bd_addr.addr_val[1], bd_addr.addr_val[0]);
			BT_APP_PROCESS(rtk_bt_br_gap_set_device_name((const uint8_t *)dev_name));
			/* Initilize SDP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_SDP, app_bt_sdp_callback));
			/* Initialize avrcp part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_AVRCP, app_bt_avrcp_callback));
			/* Initialize a2dp part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_A2DP, app_bt_a2dp_callback));
		}
		/* HFP Hand Free part */
		{
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_HFP, app_bt_hfp_callback));
		}
		/* Phone Book Access Profile part */
		{
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_PBAP, app_bt_pbap_callback));
		}
		/* LE Audio BAP part */
		{
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_BAP, app_bt_bap_callback));
		}
		/* LE Audio PBP Broadcast source init */
		{
			rtk_bt_le_audio_broadcast_source_create_param_t brs_create_param = {0};
			brs_create_param.cfg_codec_index = RTK_BT_LE_AUDIO_COMPO_DEMO_DEFAULT_BIS_CODEC_CFG;
			brs_create_param.cfg_qos_type = RTK_BT_LE_AUDIO_COMPO_DEMO_DEFAULT_BIS_QOS_CFG;
			brs_create_param.local_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC;
			brs_create_param.stream_audio_contexts = RTK_BT_LE_AUDIO_CONTEXT_MEDIA;
			BT_APP_PROCESS(rtk_bt_le_audio_broadcast_source_create(&brs_create_param));
		}
		/* Init scan list */
		{
			scan_dev_queue.count = 0;
			if (scan_dev_queue.mtx == NULL) {
				osif_mutex_create(&scan_dev_queue.mtx);
			}
			INIT_LIST_HEAD(&scan_dev_queue.head);
		}
		/* Audio init */
		if (rtk_bt_audio_init()) {
			BT_LOGE("[APP] rtk_bt_audio_init fail \r\n");
			return -1;
		}
		/* App audio pcm data queue init */
		{
			BT_APP_PROCESS(app_queue_mgr_init(&app_pcm_data_mgr_queue, pcm_data_buf, sizeof(pcm_data_buf) / 2));
			tx_water_level = APP_LE_AUDIO_PBP_BROADCAST_TX_WATER_LEVEL_MS;
			BT_LOGA("[APP] LE Audio PBP broadcast tx_water_level: %d ms\r\n", tx_water_level);
		}
		/* pkt drop mtx init*/
		if (pkt_drop_mtx == NULL) {
			osif_mutex_create(&pkt_drop_mtx);
		}
		hfp_role = bt_app_conf.hfp_role;
		a2dp_role = bt_app_conf.a2dp_role;
		bap_role = p_lea_app_conf->bap_role;
		cap_role = p_lea_app_conf->cap_role;
		/* Auto reconnect init */
		if (a2dp_auto_reconnect) {
			BT_LOGA("[A2DP] Create auto reaconnect timer \r\n");
			if (false == osif_timer_create(&reconnect_timer, "a2dp_demo_reconnect_timer", NULL,
										   APP_BT_RECONNECT_TIMER_INTERVAL,
										   false,
										   a2dp_reconnect_timer_handle)) {
				BT_LOGE("[APP] Create reconnect timer fail \r\n");
				a2dp_auto_reconnect = 0;
				return 1;
			} else {
				a2dp_auto_reconnect_try_count = APP_BT_RECONNECT_COUNT;
			}
			/* Load reconnect info from file system */
			if (rt_kv_get(a2dp_filesystem_key, (void *)remote_bd_addr, 6) == 6) {
				BT_LOGA("[A2DP] Load a2dp demo reconnect data success \r\n");
				/* avoid another device inquiry req and paging our dut */
				rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_CONNECTABLE);
				osif_timer_start(&reconnect_timer);
			} else {
				BT_LOGE("[A2DP] Fail to load a2dp reconnect data \r\n");
			}
		}
		/* Bond info flush thread init */
		if (a2dp_bond_info_flush) {
			if (rtk_bt_br_gap_bond_num_get(&a2dp_bond_num)) {
				BT_LOGE("[A2DP] Get Bond Number fail\r\n");
				goto failed;
			}
			/* Load reconnect info from file system */
			if (rt_kv_get(a2dp_bond_info_key, (void *)a2dp_bond_table, sizeof(a2dp_bond_table)) == sizeof(a2dp_bond_table)) {
				BT_LOGA("[A2DP] Load a2dp demo bond info table success \r\n");
				/* dump bond info */
				a2dp_bond_info_dump();
			} else {
				BT_LOGE("[A2DP] Fail to load a2dp bond table \r\n");
			}
			BT_LOGA("[A2DP] Create bond info flush thread \r\n");
			if (false == osif_sem_create(&bond_info_flush_task.sem, 0, 1)) {
				goto failed;
			}
			if (false == osif_msg_queue_create(&a2dp_bond_info_flush_msg_q, 0x0F, sizeof(app_demo_msg_t))) {
				goto failed;
			}
			bond_info_flush_task.run = 1;
			if (false == osif_task_create(&bond_info_flush_task.hdl, "a2dp_demo_bond_info_flush_thread",
										  a2dp_bond_flush_thread, NULL,
										  2048, 4)) {
				osif_sem_delete(bond_info_flush_task.sem);
				goto failed;
			}
			/* Wait until event task is running */
			if (false == osif_sem_take(bond_info_flush_task.sem, 0xffffffff)) {
				goto failed;
			}
		}
		demo_init_flag = true;
	} else if (0 == enable) {
		if (!demo_init_flag) {
			BT_LOGE("%s No need deinit! \r\n", __func__);
			return -1;
		}
		/* auto reconnect deinit */
		if (a2dp_auto_reconnect) {
			osif_timer_stop(&reconnect_timer);
			osif_timer_delete(&reconnect_timer);
			a2dp_auto_reconnect_try_count = APP_BT_RECONNECT_COUNT;
			a2dp_disconnection_reconnecting = 0;
		}
		if (a2dp_bond_info_flush) {
			/* indicate a2dp demo bond info flush thread to kill itself */
			a2dp_flush_bond_info(A2DP_BT_BOND_INFO_THREAD_EXIT);
			if (false == osif_sem_take(bond_info_flush_task.sem, 0xffffffff)) {
				return -1;
			}
			osif_sem_delete(bond_info_flush_task.sem);
			osif_msg_queue_delete(a2dp_bond_info_flush_msg_q);
			bond_info_flush_task.sem = NULL;
			bond_info_flush_task.hdl = NULL;
			a2dp_bond_info_flush_msg_q = NULL;
		}
		app_bt_le_audio_encode_data_control(false);
		if (g_audio_resample_t) {
			BT_APP_PROCESS(app_bt_pcm_data_resample_engine_destroy(&g_audio_resample_t));
		}
		/* pkt drop mtx deinit*/
		if (pkt_drop_mtx) {
			osif_mutex_delete(pkt_drop_mtx);
			pkt_drop_mtx = NULL;
		}
		app_queue_mgr_deinit(&app_pcm_data_mgr_queue);
		/* stop outband ring alert */
		app_hfp_ring_alert_stop();
		if (hfp_task.run) {
			hfp_task.run = 0;
			if (false == osif_sem_take(hfp_task.sem, 0xffffffffUL)) {
				return -1;
			}
			osif_sem_delete(hfp_task.sem);
			hfp_task.hdl = NULL;
			hfp_task.sem = NULL;
			rtk_bt_audio_codec_remove(audio_hfp_codec_conf.codec_index, hfp_codec_entity);
		}
		/* LE Audio APP data path remove */
		app_bt_le_audio_remove_data_path_all();
		/* Deinitialize Scan list */
		{
			app_bt_le_audio_scan_dev_list_remove_all();
			scan_dev_queue.count = 0;
			if (scan_dev_queue.mtx) {
				osif_mutex_delete(scan_dev_queue.mtx);
				scan_dev_queue.mtx = NULL;
			}
			INIT_LIST_HEAD(&scan_dev_queue.head);
		}
		/* APP Audio deinit */
		rtk_bt_audio_deinit();
		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
		lea_broadcast_start = false;
		pbp_broadcast_dequeue_flag = false;
		a2dp_play_flag = false;
		call_curr_status = 0;
		a2dp_audio_track_hdl = NULL;
		big_sync_handle = NULL;
		a2dp_codec_entity = NULL;
		hfp_audio_track_hdl = NULL;
		hfp_audio_record_hdl = NULL;
		hfp_codec_entity = NULL;
		bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_UNKNOWN;
		cap_role = RTK_BT_LE_AUDIO_CAP_ROLE_UNKNOWN;
		demo_init_flag = false;
	}

	return 0;

failed:
	if (bond_info_flush_task.hdl) {
		osif_task_delete(bond_info_flush_task.hdl);
	}
	if (bond_info_flush_task.sem) {
		osif_sem_delete(bond_info_flush_task.sem);
	}
	if (a2dp_bond_info_flush_msg_q) {
		osif_msg_queue_delete(a2dp_bond_info_flush_msg_q);
	}

	return -1;
}