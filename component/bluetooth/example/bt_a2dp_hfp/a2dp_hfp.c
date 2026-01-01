/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

/* -------------------------------- Includes -------------------------------- */
#include <string.h>
#include <stdio.h>
#include <osif.h>
#include <rtk_bt_def.h>
#include <rtk_bt_device.h>
#include <rtk_bt_common.h>
#include <bt_api_config.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_br_gap.h>
#include <rtk_bt_sdp.h>
#include <rtk_bt_avrcp.h>
#include <rtk_bt_a2dp.h>
#include <rtk_bt_hfp.h>
#include <bt_audio_intf.h>
#include <bt_audio_noise_cancellation.h>
#include <bt_audio_track_api.h>
#include <bt_audio_record_api.h>
#include <app_audio_data.h>
#include <sbc_codec_entity.h>
#include <cvsd_codec_entity.h>
#include "kv.h"
#include <dlist.h>
#include <bt_utils.h>
#include <bt_audio_resample.h>

#if defined(CONFIG_BT_AUDIO_NOISE_CANCELLATION) && CONFIG_BT_AUDIO_NOISE_CANCELLATION
#define AUDIO_RECORD_CHANNELS (2) /* 1 channel for audio source, another for AEC */
#define MAX_RECORD_FRAME_SAMPLES_PER_CHANNLE (256)
#else
#define AUDIO_RECORD_CHANNELS (1)
#define MAX_RECORD_FRAME_SAMPLES_PER_CHANNLE (60)
#endif
/* this parameter will calculate samples per channel from RTK_BT_HFP_EVT_SCO_CONN_CMPL codec configuration and
will be used during transmit and recording flow */
static int record_frame_samples_per_channel = 0;
#define BT_ENCODE_FRAME_BYTES (120)

/* -------------------------------- Defines --------------------------------- */
#define RTK_BT_DEV_NAME "RTK_BT_AUDIO_DEV"
#define DEFAULT_PAGESCAN_WINDOW             0x48
#define DEFAULT_PAGESCAN_INTERVAL           0x800 //0x800
#define DEFAULT_PAGE_TIMEOUT                0x2000
#define DEFAULT_INQUIRYSCAN_WINDOW          0x48
#define DEFAULT_INQUIRYSCAN_INTERVAL        0x800 //0x1000
#define DEFAULT_AUDIO_LEFT_VOLUME           0.8
#define DEFAULT_AUDIO_RIGHT_VOLUME          0.8
#define RTK_BT_A2DP_SRC_STREAM_MAX_CREDITS   2
#define RTK_BT_DEMO_RECONNECT_TIMER_INTERVAL 5000
#define RTK_BT_DEMO_RECONNECT_COUNT          3
#define RTK_BT_A2DP_SRC_DEMO_MAX_SEND_DATA_SIZE (8 * 16 * 2 * SBC_MAX_NUM_FRAME) * (48000 / 16000) // the maximum sbc input pcm size (int16_t) (MAX_SUBBANDS * MAX_BLOCKS * MAX_NUM * SBC_MAX_NUM_FRAME)
// 48000 / 16000 means the max ratio for sbc

/* ------------------------------- Company ID ------------------------------- */
#define RTK_COMPANY_ID 0x005D
/* ----------------------------- Avrcp Related ------------------------------ */
#define VOL_MAX     0x7f
#define VOL_MIN     0x00

/* ---------------------------- Global Variables ---------------------------- */
static uint8_t curr_volume = RTK_BT_DEFAULT_ABSOLUTE_VOLUME;

/* ---------------------------- Static Variables ---------------------------- */
static uint8_t bt_audio_demo_init_flag = 0;
static void *alert_timer = NULL;
static void *alert_track_hdl = NULL;
static rtk_bt_audio_track_t *a2dp_demo_audio_track_hdl = NULL;
static rtk_bt_audio_track_t *hfp_demo_audio_track_hdl = NULL;
static rtk_bt_audio_record_t *hfp_demo_audio_record_hdl = NULL;
#if defined(CONFIG_BT_AUDIO_NOISE_CANCELLATION) && CONFIG_BT_AUDIO_NOISE_CANCELLATION
static rtk_bt_audio_record_config_table_t hfp_demo_audio_record_table = {
	.strs = NULL,
	.mic_type = RTK_BT_AUDIO_AMIC,
	.record_nums = (uint8_t)AUDIO_RECORD_CHANNELS,
	{
		/* 1 record */
		{
			.mic_channel_index = 0,
			.mic_category = RTK_BT_AUDIO_AMIC1,
			.gain = RTK_BT_AUDIO_MICBST_GAIN_0DB
		},
		/* 2 record */
		{
			.mic_channel_index = 1,
			.mic_category = RTK_BT_AUDIO_AMIC5,
			.gain = RTK_BT_AUDIO_MICBST_GAIN_0DB
		}
	}
};
#endif
static void *a2dp_demo_codec_entity = NULL;
static void *hfp_demo_codec_entity = NULL;
static uint8_t hfp_demo_role;
static uint8_t a2dp_demo_role;
static uint8_t src_a2dp_credits = RTK_BT_A2DP_SRC_STREAM_MAX_CREDITS;
static bool ring_alert_inband = false;
static uint8_t battery_power = RTK_BT_HFP_DEMO_DEFAULT_BATTERY_POWER_LEVEL;
static uint8_t remote_bd_addr[6] = {0};
static uint8_t call_curr_status = 0;
/* class of device */
static uint32_t cod_for_snk = (0x04 | (0x04 << 8) | (0x24 <<
													 16)); // MAJOR_DEVICE_CLASS_AUDIO + MINOR_DEVICE_CLASS_HEADSET + SERVICE_CLASS_AUDIO + SERVICE_CLASS_RENDERING
static uint32_t cod_for_src = (0x0C | (0x02 << 8) | (0x64 <<
													 16)); // MINOR_DEVICE_CLASS_SMART_PHONE + MAJOR_DEVICE_CLASS_PHONE + SERVICE_CLASS_AUDIO + SERVICE_CLASS_RENDERING
/* supervision_timeout */
static uint16_t superv_time = (0x1f40);
static uint16_t sco_seq_num = 0;
static float hw_volume = 0.7;
static uint16_t src_a2dp_send_data_size = 0; // unit is sizeof(int16_t)
static rtk_bt_audio_codec_conf_t audio_a2dp_codec_conf = {0};
static rtk_bt_audio_codec_conf_t audio_hfp_codec_conf = {0};
static uint32_t demo_in_rate = 48000;
static uint8_t demo_in_channels = 2;
static rtk_bt_audio_biquad_t bq_t = {0};
static rtk_bt_audio_resample_t *g_audio_resample_t = NULL;
static uint32_t resample_out_frames, resample_in_frames;
static rtk_bt_cvsd_codec_t cvsd_codec_t = {0};
static rtk_bt_sbc_codec_t sbc_codec_t = {
	{
		.sbc_enc_mode = (sbc_mode_t)SBC_MODE_STANDARD,
		.sbc_pkt_num = SBC_MAX_NUM_FRAME,
		.blocks = 16,
		.subbands = 8,
		.alloc_method = SBC_ALLOCATION_METHOD_SNR,
		.sample_rate = 44100,
		.bitpool = 0x21,
		.channel_mode = SBC_CHANNEL_MODE_DUAL_CHANNEL,
		.frame_size = 0,
	},
	{0},
};
static rtk_bt_sbc_codec_t msbc_codec_t = {
	{
		.sbc_enc_mode = (sbc_mode_t)SBC_MODE_mSBC,
		.sbc_pkt_num = 1,
		.blocks = 15,
		.subbands = 8,
		.alloc_method = SBC_ALLOCATION_METHOD_LOUDNESS,
		.sample_rate = 16000,
		.bitpool = 26,
		.channel_mode = SBC_CHANNEL_MODE_MONO,
		.frame_size = 0,
	},
	{0},
};

typedef struct {
	uint16_t type;
	uint16_t subtype;
	uint8_t bd_addr[6];
	union {
		uint32_t  param;
		void     *buf;
	} u;
} a2dp_demo_msg_t;

struct a2dp_demo_task_t {
	void *hdl;
	void *sem;
	uint8_t run;
};

#include "timer_api.h"
#define A2DP_SEND_TIMER_ID TIMER13
static gtimer_t bt_a2dp_demo_src_send_timer = {0};
static bool bt_a2dp_demo_src_send_data_enable = false;
static void *bt_a2dp_demo_src_send_sem = NULL;
static uint32_t a2dp_src_data_send_interval_us = 0;
static struct a2dp_demo_task_t a2dp_task = {
	.hdl = NULL,
	.sem = NULL,
	.run = 0,
};

/* ---------------------------- Auto reconnect config ---------------------------- */
static void *reconnect_timer = NULL;
static uint8_t a2dp_demo_auto_reconnect = 1;
static uint8_t a2dp_demo_auto_reconnect_try_count = RTK_BT_DEMO_RECONNECT_COUNT;
/* when sink disconnect due to out of range, this flag where be set up to do reconnecting */
static uint8_t a2dp_demo_disconnection_reconnecting = 0;
static char a2dp_demo_filesystem_key[] = "a2dp_reconnect_data";
/* ---------------------------- Bond Info Manage config -------------------------- */
/* Max bond number is unchangable */
#define A2DP_DEMO_MAX_BOND_NUM 8
#define A2DP_DEMO_BOND_INFO_USED_VAL 0x4E
#define A2DP_DEMO_BOND_INFO_FLUSH_EVENT 0x01
#define A2DP_DEMO_BOND_INFO_THREAD_EXIT 0xFF
static char a2dp_demo_bond_info_key[] = "a2dp_bond_table";
static uint8_t a2dp_demo_bond_num = 0;
static uint8_t a2dp_demo_bond_info_flush = 1;
static struct a2dp_demo_task_t bond_info_flush_task = {
	.hdl = NULL,
	.sem = NULL,
	.run = 0,
};
static void *a2dp_demo_bond_info_flush_msg_q = NULL;
typedef struct {
	uint8_t             bd_addr[6];                         /* Bluetooth address */
	uint8_t             name[RTK_BT_GAP_DEVICE_NAME_LEN];   /* name */
	uint8_t             name_contained;                     /* check whether this unit contains name */
	uint8_t             priority;                           /* bond info priority, range from 1~8, 1 is the highest */
	uint8_t             used_check;                         /* check whether this unit is configured */
	uint8_t             save_flag;                          /* indicate whether flash save action is need */
} a2dp_demo_bond_info_t;
static a2dp_demo_bond_info_t a2dp_demo_bond_table[A2DP_DEMO_MAX_BOND_NUM] = {0};

static rtk_bt_br_page_param_t page_param = {
	.pagescan_type = RTK_BT_BR_GAP_PAGE_SCAN_TYPE_INTERLACED,
	.pagescan_interval = DEFAULT_PAGESCAN_INTERVAL,
	.pagescan_window = DEFAULT_PAGESCAN_WINDOW,
	.page_timeout = DEFAULT_PAGE_TIMEOUT,
};

static rtk_bt_br_inquiry_param_t inquiry_param = {
	.inquiryscan_type = RTK_BT_BR_GAP_PAGE_SCAN_TYPE_INTERLACED,
	.inquiryscan_window = DEFAULT_INQUIRYSCAN_WINDOW,
	.inquiryscan_interval = DEFAULT_INQUIRYSCAN_INTERVAL,
	.inquiry_mode = RTK_BT_BR_GAP_INQUIRY_MODE_EXTENDED_RESULT,
};

static rtk_bt_hfp_ag_indicators_status_t demo_ag_call_status_t = {
	.service_indicator = RTK_BT_HFP_AG_SERVICE_STATUS_AVAILABLE,
	.call_indicator = RTK_BT_HFP_AG_NO_CALL_IN_PROGRESS,
	.call_setup_indicator = RTK_BT_HFP_AG_CALL_SETUP_STATUS_IDLE,
	.call_held_indicator = RTK_BT_HFP_AG_CALL_HELD_STATUS_IDLE,
	.signal_indicator = 5,
	.roaming_indicator = RTK_BT_HFP_AG_ROAMING_STATUS_ACTIVE,
	.batt_chg_indicator = 5,
};

#if defined(CONFIG_BT_AUDIO_NOISE_CANCELLATION) && CONFIG_BT_AUDIO_NOISE_CANCELLATION
/* for noise cancellation */
struct nc_task_t {
	void *hdl;
	void *sem;
	uint8_t run;
};

static struct nc_task_t nc_task = {
	.hdl = NULL,
	.sem = NULL,
	.run  = 0,
};
#else
#include "ringbuffer.h"
/* record read task */
struct record_task_t {
	void *hdl;
	void *sem;
	uint8_t run;
	RingBuffer *p_buffer;
};

static struct record_task_t record_task = {
	.hdl = NULL,
	.sem = NULL,
	.run = 0,
	.p_buffer = NULL,
};
#endif

#include "timer_api.h"
#define HFP_SCO_SEND_TIMER_ID TIMER12
static gtimer_t bt_hfp_demo_sco_send_timer = {0};
static bool bt_hfp_demo_sco_send_data_enable = false;
static void *bt_hfp_demo_sco_send_sem = NULL;
static uint32_t hfp_sco_data_send_interval_us = 0;

/* for hfp hf client read record data */
struct hfp_task_t {
	void *hdl;
	void *sem;
	uint8_t run;
};

static struct hfp_task_t hfp_task = {
	.hdl = NULL,
	.sem = NULL,
	.run = 0,
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

static const uint8_t hfp_sdp_record[] = {
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

static const uint8_t hfp_ag_sdp_record[] = {
	//total length
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x34,//52 bytes belowed.

	//attribute SDP_ATTR_SRV_CLASS_ID_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,          //0x35
	0x06,                                   //6bytes
	RTK_BT_SDP_UUID16_HDR,                     //0x19
	(uint8_t)(RTK_BT_UUID_HANDSFREE_AUDIO_GATEWAY >> 8), //0x111F
	(uint8_t)(RTK_BT_UUID_HANDSFREE_AUDIO_GATEWAY),
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
	RTK_BT_RFC_HFP_AG_CHANN_NUM,  //0x11

	//attribute RTK_BT_SDP_ATTR_PROFILE_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROFILE_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROFILE_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,          //0x35
	0x08,                                   //8 bytes
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,      //0x35
	0x06,                               //6 bytes
	RTK_BT_SDP_UUID16_HDR,                 //0x19
	(uint8_t)(RTK_BT_UUID_HANDSFREE_AUDIO_GATEWAY >> 8), //0x111F
	(uint8_t)(RTK_BT_UUID_HANDSFREE_AUDIO_GATEWAY),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,           //0x09
	(uint8_t)(0x0109 >> 8),     //version number default hf1.9
	(uint8_t)(0x0109),
	/*
	    //attribute RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST
	    RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	    (uint8_t)(RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST >> 8),
	    (uint8_t)RTK_BT_SDP_ATTR_BROWSE_GROUP_LIST,
	    RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	    0x03,
	    RTK_BT_SDP_UUID16_HDR,
	    (uint8_t)(RTK_BT_UUID_PUBLIC_BROWSE_GROUP >> 8),
	    (uint8_t)RTK_BT_UUID_PUBLIC_BROWSE_GROUP,
	*/
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
	//attribute SDP_ATTR_EXT_NETWORK
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)((RTK_BT_SDP_ATTR_EXT_NETWORK) >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_EXT_NETWORK),
	RTK_BT_SDP_UNSIGNED_ONE_BYTE,
	(uint8_t)(0x01),

	//attribute RTK_BT_SDP_ATTR_SUPPORTED_FEATURES
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)((RTK_BT_SDP_ATTR_SUPPORTED_FEATURES) >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_SUPPORTED_FEATURES),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x012F >> 8),
	(uint8_t)(0x012F)
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

static const uint8_t a2dp_src_sdp_record[] = {
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x39,//0x55,
	//attribute SDP_ATTR_SRV_CLASS_ID_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_AUDIO_SOURCE >> 8),
	(uint8_t)(RTK_BT_UUID_AUDIO_SOURCE),

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

static rtk_bt_a2dp_media_codec_sbc_t codec_sbc = {
	.sampling_frequency_mask = 0xf0,
	.channel_mode_mask = 0x0f,
	.block_length_mask = 0xf0,
	.subbands_mask = 0x0C,
	.allocation_method_mask = 0x03,
	.min_bitpool = 2,
	.max_bitpool = 53,
};

static uint32_t pcm_offset = 0;
static uint16_t a2dp_demo_send_data_seq = 0;
static uint32_t demo_read_size = 0;
static int16_t in_frame_buf[RTK_BT_A2DP_SRC_DEMO_MAX_SEND_DATA_SIZE] = {0};
static int16_t out_frame_buf[RTK_BT_A2DP_SRC_DEMO_MAX_SEND_DATA_SIZE] = {0};
static void app_a2dp_src_send_data(void)
{
	static rtk_bt_a2dp_stream_data_send_t data_send_t = {0};
	struct enc_codec_buffer *penc_codec_buffer_t = NULL;
	short *pdata = NULL;
	uint32_t out_frames = 0;

	if (src_a2dp_credits) {
		if (pcm_offset < (birds_sing_size / 2)) {
			pdata = (short *)(birds_sing + pcm_offset);
			if ((pcm_offset + (demo_read_size / 2)) < birds_sing_size / 2) {
				out_frames = rtk_bt_audio_resample_entry(g_audio_resample_t, &bq_t,
														 (uint8_t *)pdata,
														 resample_in_frames,
														 (uint8_t *)out_frame_buf,
														 demo_in_rate,
														 sbc_codec_t.encoder_t.sample_rate);
				pcm_offset += (demo_read_size / 2);
			} else {
				memset((void *)in_frame_buf, 0, demo_read_size);
				memcpy((void *)in_frame_buf, (void *)pdata, ((birds_sing_size / 2) - pcm_offset) * 2);
				out_frames = rtk_bt_audio_resample_entry(g_audio_resample_t, &bq_t,
														 (uint8_t *)in_frame_buf,
														 resample_in_frames,
														 (uint8_t *)out_frame_buf,
														 demo_in_rate,
														 sbc_codec_t.encoder_t.sample_rate);
				pcm_offset = 0;
			}
			penc_codec_buffer_t = rtk_bt_audio_data_encode(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_codec_entity, (int16_t *)out_frame_buf,
														   out_frames * g_audio_resample_t->out_frame_size);
			if (penc_codec_buffer_t) {
				memcpy((void *)data_send_t.bd_addr, (void *)remote_bd_addr, 6);
				data_send_t.seq_num = a2dp_demo_send_data_seq++;
				data_send_t.frame_buf = (uint8_t *)penc_codec_buffer_t->pbuffer;
				data_send_t.frame_num = (uint8_t)penc_codec_buffer_t->frame_num;
				data_send_t.time_stamp += data_send_t.frame_num * sbc_codec_t.encoder_t.subbands * sbc_codec_t.encoder_t.blocks;
				data_send_t.len = (uint16_t)(penc_codec_buffer_t->frame_num * penc_codec_buffer_t->frame_size);
				data_send_t.flush = false;
				if (rtk_bt_a2dp_data_send(&data_send_t)) {
					BT_LOGE("[A2DP] data send fail \r\n");
				} else {
					src_a2dp_credits --;
				}
				rtk_bt_audio_free_encode_buffer(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_codec_entity, penc_codec_buffer_t);
			} else {
				BT_LOGE("[A2DP SRC Demo]: Encode fail \r\n");
			}
		}
	} else {
		// BT_LOGE("[A2DP] waiting src_a2dp_credits \r\n");
	}
}

static void a2dp_task_entry(void *ctx)
{
	(void)ctx;
	a2dp_task.run = 1;
	osif_sem_give(a2dp_task.sem);

	while (a2dp_task.run) {
		if (bt_a2dp_demo_src_send_sem) {
			osif_sem_take(bt_a2dp_demo_src_send_sem, BT_TIMEOUT_FOREVER);
		}
		app_a2dp_src_send_data();
	}

	osif_sem_give(a2dp_task.sem);
	osif_task_delete(NULL);
}

static void bt_a2dp_demo_src_send_timer_handler(void *arg)
{
	(void)arg;

	if (a2dp_task.run) {
		if (bt_a2dp_demo_src_send_sem) {
			osif_sem_give(bt_a2dp_demo_src_send_sem);
		}
	}
}

static a2dp_demo_bond_info_t *a2dp_demo_find_bond_info_by_mac_addr(uint8_t *bd_addr)
{
	a2dp_demo_bond_info_t *pbond_info = NULL;

	if (!bd_addr) {
		return NULL;
	}
	for (uint8_t i = 0; i < A2DP_DEMO_MAX_BOND_NUM; i ++) {
		/* check bond info unit meaningful */
		if (A2DP_DEMO_BOND_INFO_USED_VAL == a2dp_demo_bond_table[i].used_check) {
			if (!memcmp((void *)a2dp_demo_bond_table[i].bd_addr, (void *)bd_addr, 6)) {
				pbond_info = &a2dp_demo_bond_table[i];
				break;
			}
		}
	}

	return pbond_info;
}

static a2dp_demo_bond_info_t *a2dp_demo_find_bond_info_by_priority(uint8_t bond_priority)
{
	a2dp_demo_bond_info_t *pbond_info = NULL;

	if ((bond_priority < 1) || (bond_priority > 8)) {
		return NULL;
	}
	for (uint8_t i = 0; i < A2DP_DEMO_MAX_BOND_NUM; i ++) {
		/* check bond info unit meaningful */
		if (A2DP_DEMO_BOND_INFO_USED_VAL == a2dp_demo_bond_table[i].used_check) {
			if (bond_priority == a2dp_demo_bond_table[i].priority) {
				pbond_info = &a2dp_demo_bond_table[i];
				break;
			}
		}
	}

	return pbond_info;
}

static a2dp_demo_bond_info_t *get_free_bond_table_unit(void)
{
	a2dp_demo_bond_info_t *pbond_info = NULL;

	for (uint8_t i = 0; i < A2DP_DEMO_MAX_BOND_NUM; i ++) {
		/* check free unit */
		if (A2DP_DEMO_BOND_INFO_USED_VAL != a2dp_demo_bond_table[i].used_check) {
			pbond_info = &a2dp_demo_bond_table[i];
			pbond_info->used_check = A2DP_DEMO_BOND_INFO_USED_VAL;
			break;
		}
	}

	/* if bond units are full, get the lowest priority bond info for overwriting */
	if (!pbond_info) {
		pbond_info = a2dp_demo_find_bond_info_by_priority(8);
	}

	return pbond_info;
}

/* once application modify bond info, this api should bt invoke to sync bond info table */
static uint16_t a2dp_demo_flush_bond_info(uint8_t type, uint8_t *bd_addr)
{
	a2dp_demo_msg_t io_msg;

	io_msg.type = type;
	if (bd_addr) {
		memcpy(io_msg.bd_addr, bd_addr, 6);
	}
	if (false == osif_msg_send(a2dp_demo_bond_info_flush_msg_q, &io_msg, 0)) {
		return 1;
	}

	return 0;
}


static void a2dp_demo_bond_info_dump(void)
{
	a2dp_demo_bond_info_t *pbond_info = NULL;

	BT_LOGA("[A2DP Demo] Bond Info List\r\n");
	for (uint8_t i = 0; i < a2dp_demo_bond_num; i ++) {
		pbond_info = a2dp_demo_find_bond_info_by_priority(i + 1);
		if (pbond_info && (pbond_info->used_check == A2DP_DEMO_BOND_INFO_USED_VAL)) {
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

static void a2dp_demo_bond_flush_thread(void *ctx)
{
	(void)ctx;
	a2dp_demo_msg_t io_msg;
	uint8_t bd_addr[6] = {0};
	a2dp_demo_bond_info_t *pbond_info = NULL;
	uint8_t temp_bond_num = 0;
	uint8_t need_save_bond_info = 0;
	uint16_t ret = 1;

	osif_sem_give(bond_info_flush_task.sem);
	while (bond_info_flush_task.run) {
		if (osif_msg_recv(a2dp_demo_bond_info_flush_msg_q, &io_msg, BT_TIMEOUT_FOREVER) == true) {
			if (A2DP_DEMO_BOND_INFO_THREAD_EXIT == io_msg.type) {
				bond_info_flush_task.run = 0;
				continue;
			}
			/* do flush action */
			/* 1. get bond number */
			if (rtk_bt_br_gap_bond_num_get(&temp_bond_num)) {
				BT_LOGE("[A2DP Demo] Get Bond Number fail\r\n");
				continue;
			}
			BT_LOGA("[A2DP Demo] Get Bond Number %d \r\n", temp_bond_num);
			if (temp_bond_num > A2DP_DEMO_MAX_BOND_NUM) {
				temp_bond_num = A2DP_DEMO_MAX_BOND_NUM;
			}
			/* 2. get bond mac address accord to bond number */
			for (uint8_t i = 0; i < temp_bond_num; i ++) {
				/* gap bond priority start from 1, so i + 1 */
				ret = rtk_bt_br_gap_bond_addr_get(i + 1, bd_addr);
				if (ret) {
					BT_LOGE("[A2DP Demo] Get bond addr failed! err: 0x%x", ret);
					continue;
				}
				pbond_info = a2dp_demo_find_bond_info_by_mac_addr(bd_addr);
				if (pbond_info) {
					if (pbond_info->priority != (i + 1)) {
						pbond_info->priority = i + 1;
						need_save_bond_info = 1;
					}
				} else {
					pbond_info = get_free_bond_table_unit();
					if (!pbond_info) {
						BT_LOGE("[A2DP Demo] Get free bond table unit fail \r\n");
						continue;
					}
					memcpy((void *)pbond_info->bd_addr, (void *)bd_addr, 6);
					pbond_info->name_contained = 0;
					pbond_info->priority = i + 1;
					memset((void *)pbond_info->name, 0, RTK_BT_GAP_DEVICE_NAME_LEN);
					need_save_bond_info = 1;
				}
			}
			pbond_info = a2dp_demo_find_bond_info_by_mac_addr(io_msg.bd_addr);
			if (pbond_info && !pbond_info->name_contained) {
				rtk_bt_br_gap_get_remote_name(io_msg.bd_addr);
			} else if (pbond_info && pbond_info->save_flag) {
				need_save_bond_info = 1;
				pbond_info->save_flag = 0;
			}
			if (need_save_bond_info) {
				if (rt_kv_set(a2dp_demo_bond_info_key, (void *)a2dp_demo_bond_table, sizeof(a2dp_demo_bond_table)) == sizeof(a2dp_demo_bond_table)) {
					BT_LOGA("[A2DP Demo] Save a2dp demo bond info table success \r\n");
					a2dp_demo_bond_info_dump();
				} else {
					BT_LOGE("[A2DP Demo] Fail to save a2dp demo bond info table \r\n");
				}
			}
			a2dp_demo_bond_num = temp_bond_num;
		} else {
			BT_LOGE("[A2DP Demo] Bond info flush thread recv msg fail\r\n");
		}
		osif_delay(3);
	}

	osif_sem_give(bond_info_flush_task.sem);
	osif_task_delete(NULL);
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
		if (a2dp_demo_bond_info_flush) {
			a2dp_demo_bond_info_t *pbond_info = NULL;
			pbond_info = a2dp_demo_find_bond_info_by_mac_addr(p_name_rsp->bd_addr);
			if (pbond_info) {
				memcpy((void *)pbond_info->name, (void *)p_name_rsp->name, RTK_BT_GAP_DEVICE_NAME_LEN);
				pbond_info->name_contained = 1;
				pbond_info->save_flag = 1;
				a2dp_demo_flush_bond_info(A2DP_DEMO_BOND_INFO_FLUSH_EVENT, p_name_rsp->bd_addr);
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
		if (a2dp_demo_auto_reconnect) {
			/* write reconnect info to file system */
			if (rt_kv_set(a2dp_demo_filesystem_key, (void *)bd_addr, 6) == 6) {
				BT_LOGA("[A2DP Demo] Save a2dp demo reconnect data success \r\n");
			} else {
				BT_LOGE("[A2DP Demo] Fail to save a2dp reconnect data \r\n");
			}
			a2dp_demo_auto_reconnect_try_count = 0;
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
		rtk_bt_br_link_key_req_t *p_link_key_req_t = (rtk_bt_br_link_key_req_t *)param;
		if (a2dp_demo_bond_info_flush && p_link_key_req_t->found) {
			a2dp_demo_flush_bond_info(A2DP_DEMO_BOND_INFO_FLUSH_EVENT, p_link_key_req_t->bd_addr);
		}
		BT_LOGA("[BR GAP] Link Key Request received and found is 0x%x \r\n", p_link_key_req_t->found);
		break;
	}

	case RTK_BT_BR_GAP_LINK_KEY_INFO: {
		rtk_bt_br_bond_key_t *pbond_key_t = (rtk_bt_br_bond_key_t *)param;
		uint8_t *bd_addr = pbond_key_t->bd_addr;
		if (a2dp_demo_bond_info_flush) {
			a2dp_demo_flush_bond_info(A2DP_DEMO_BOND_INFO_FLUSH_EVENT, pbond_key_t->bd_addr);
		}
		BT_LOGA("[BR GAP] Set link key of %02x:%02x:%02x:%02x:%02x:%02x \r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		break;
	}

	case RTK_BT_BR_GAP_LINK_RSSI_INFO: {
		rtk_bt_br_link_read_rssi_rsp *prssi_rsp_t = (rtk_bt_br_link_read_rssi_rsp *)param;
		if (!prssi_rsp_t->cause) {
			BT_LOGA("[BR GAP] Read rssi %d of %02x:%02x:%02x:%02x:%02x:%02x \r\n", prssi_rsp_t->rssi,
					prssi_rsp_t->bd_addr[5], prssi_rsp_t->bd_addr[4], prssi_rsp_t->bd_addr[3],
					prssi_rsp_t->bd_addr[2], prssi_rsp_t->bd_addr[1], prssi_rsp_t->bd_addr[0]);
		} else {
			BT_LOGA("[BR GAP] Read rssi fail, cause 0x%x, %02x:%02x:%02x:%02x:%02x:%02x \r\n", prssi_rsp_t->cause,
					prssi_rsp_t->bd_addr[5], prssi_rsp_t->bd_addr[4], prssi_rsp_t->bd_addr[3],
					prssi_rsp_t->bd_addr[2], prssi_rsp_t->bd_addr[1], prssi_rsp_t->bd_addr[0]);
		}
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
		if (a2dp_demo_auto_reconnect) {
			if ((RTK_BT_ERR_HCI_GROUP | RTK_BT_HCI_ERR_REMOTE_USER_TERMINATE) == p_acl_disc_event->cause) {
				BT_LOGA("[A2DP Demo] Deleting a2dp demo reconnect config \r\n");
				if (rt_kv_delete(a2dp_demo_filesystem_key) == 0) {
					BT_LOGA("[A2DP Demo] Delete a2dp demo reconnect config success \r\n");
				}
			} else if ((RTK_BT_ERR_HCI_GROUP | RTK_BT_HCI_ERR_CONN_TIMEOUT) == p_acl_disc_event->cause) {
				BT_LOGA("[A2DP Demo] Start reconnect \r\n");
				a2dp_demo_auto_reconnect_try_count = RTK_BT_DEMO_RECONNECT_COUNT;
				a2dp_demo_disconnection_reconnecting = 1;
				/* avoid another device inquiry req and paging our dut */
				rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_CONNECTABLE);
				osif_timer_start(&reconnect_timer);
			}
		}
		break;
	}

	default:
		BT_LOGE("%s, Unknown GAP RTK_BLE_EVT: %d\r\n", __func__, evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t rtk_bt_sdp_app_callback(uint8_t evt_code, void *param, uint32_t len)
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

static rtk_bt_evt_cb_ret_t rtk_bt_avrcp_app_callback(uint8_t evt_code, void *param, uint32_t len)
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
		audio_track_volume = 1.0 * curr_volume / VOL_MAX;
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
		if (curr_volume < VOL_MAX) {
			curr_volume++;
		} else {
			curr_volume = VOL_MAX;
		}
		BT_LOGA("[AVRCP] Volume up to %d \r\n", curr_volume);
		break;
	}

	case RTK_BT_AVRCP_EVT_VOLUME_DOWN: {
		if (curr_volume > VOL_MIN) {
			curr_volume--;
		} else {
			curr_volume = VOL_MIN;
		}
		BT_LOGA("[AVRCP] Volume down to %d \r\n", curr_volume);
		break;
	}

	case RTK_BT_AVRCP_EVT_REG_VOLUME_CHANGED: {
		rtk_bt_avrcp_volume_change_req_t *p_volume_t = (rtk_bt_avrcp_volume_change_req_t *)param;
		uint8_t volume = p_volume_t->volume;
		float audio_track_volume = 0;

		curr_volume = volume;
		audio_track_volume = 1.0 * curr_volume / VOL_MAX;
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
			if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
				if (bt_a2dp_demo_src_send_data_enable) {
					if (bt_a2dp_demo_src_send_timer.handler) {
						gtimer_start_periodical(&bt_a2dp_demo_src_send_timer, a2dp_src_data_send_interval_us, (void *)bt_a2dp_demo_src_send_timer_handler, NULL);
					} else {
						BT_LOGE("[APP] %s: timer not init\r\n", __func__);
					}
				}
			}
			break;
		}

		case RTK_BT_AVRCP_STATUS_PAUSED: {
			BT_LOGA("[AVRCP]: Paused \r\n");
			if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
				if (bt_a2dp_demo_src_send_data_enable) {
					if (bt_a2dp_demo_src_send_timer.handler) {
						gtimer_stop(&bt_a2dp_demo_src_send_timer);
					} else {
						BT_LOGE("[APP] %s: timer not init\r\n", __func__);
					}
				}
			} else {
				if (a2dp_demo_disconnection_reconnecting) {
					rtk_bt_avrcp_play(remote_bd_addr);
					a2dp_demo_disconnection_reconnecting = 0;
				}
			}
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
		rtk_bt_avrcp_digital_interface_command_t *p_command_t = (rtk_bt_avrcp_digital_interface_command_t *)param;

		BT_LOGA("[AVRCP]: Play req \r\n");
		/* for a2dp source */
		rtk_bt_avrcp_play_status_change_req(p_command_t->bd_addr, RTK_BT_AVRCP_STATUS_PLAYING);
		rtk_bt_a2dp_start(p_command_t->bd_addr);
		break;
	}

	case RTK_BT_AVRCP_EVT_PAUSE_REQ_EVENT: {
		rtk_bt_avrcp_digital_interface_command_t *p_command_t = (rtk_bt_avrcp_digital_interface_command_t *)param;

		BT_LOGA("[AVRCP]: Pause req \r\n");
		/* for a2dp source */
		rtk_bt_avrcp_play_status_change_req(p_command_t->bd_addr, RTK_BT_AVRCP_STATUS_PAUSED);
		rtk_bt_a2dp_suspend(p_command_t->bd_addr);
		break;
	}

	case RTK_BT_AVRCP_EVT_STOP_REQ_EVENT: {
		rtk_bt_avrcp_digital_interface_command_t *p_command_t = (rtk_bt_avrcp_digital_interface_command_t *)param;

		BT_LOGA("[AVRCP]: Stop req \r\n");
		/* for a2dp source */
		rtk_bt_avrcp_play_status_change_req(p_command_t->bd_addr, RTK_BT_AVRCP_STATUS_STOPPED);
		rtk_bt_a2dp_suspend(p_command_t->bd_addr);
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
	psbc_decoder_t->sbc_dec_mode = (sbc_mode_t)SBC_MODE_STANDARD;
	a2dp_demo_audio_track_hdl = rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_SBC, (float)DEFAULT_AUDIO_LEFT_VOLUME, (float)DEFAULT_AUDIO_RIGHT_VOLUME, channels,
													   psbc_decoder_t->sampling_frequency, BT_AUDIO_FORMAT_PCM_16_BIT, 0, NULL, true);
	if (!a2dp_demo_audio_track_hdl) {
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
	psbc_encoder_t->sbc_enc_mode = (sbc_mode_t)SBC_MODE_STANDARD;
	BT_LOGA("[A2DP] Parsing encode struct:subbands is %d, blocks is %d, channel mode is %d \r\n", psbc_encoder_t->subbands, psbc_encoder_t->blocks,
			psbc_encoder_t->channel_mode);

	return 0;
}

static uint16_t app_bt_pcm_data_resample_engine_alloc(rtk_bt_audio_resample_t **pp_sample_t, uint32_t sample_rate_in, uint8_t in_channels,
													  uint32_t sample_rate_out, uint8_t out_channels)
{
	double sample_ratio;

	sample_ratio = (double)sample_rate_out / (double)sample_rate_in;
	//calculate the expected input and output according to the sample rate
	/* resample out frames should be matched with the sbc encode size */
	resample_out_frames = (sbc_codec_t.encoder_t.blocks * sbc_codec_t.encoder_t.subbands * out_channels * sbc_codec_t.encoder_t.sbc_pkt_num) * 2 /
						  (16 / 8 * out_channels);
	resample_in_frames = (uint32_t)(resample_out_frames * (float)sample_rate_in / (float)sample_rate_out);
	/* init bq filter */
	rtk_bt_audio_bq_config(&bq_t, RTK_BT_AUDIO_LPF, 1.0, (unsigned long)((1.0 / sample_ratio / 2) * (double)sample_rate_in),
						   (unsigned long)sample_rate_in, 0.2);
	/* init sinc resample */
	*pp_sample_t = rtk_bt_audio_resample_alloc((float)sample_rate_in, (float)sample_rate_out, in_channels, out_channels, resample_in_frames);
	demo_read_size = resample_in_frames * in_channels * 16 / 8;
	BT_LOGA("[A2DP] Resample allocate: rate in is %d, channel is %d, rate out is %d, channel is %d \r\n", sample_rate_in, in_channels, sample_rate_out,
			out_channels);
	BT_LOGA("[A2DP] resample_out_frames is %d, resample_in_frames is %d, demo_read_size is %d \r\n", resample_out_frames, resample_in_frames, demo_read_size);

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

static void rtk_bt_a2dp_demo_src_send_data_control(bool enable)
{
	BT_LOGA("[APP] %s %d\r\n", __func__, enable);
	if (enable) {
		if (bt_a2dp_demo_src_send_data_enable == true) {
			BT_LOGE("[APP] %s: send data is alreay enabled\r\n", __func__);
			return;
		}
		bt_a2dp_demo_src_send_data_enable = true;
		if (bt_a2dp_demo_src_send_sem == NULL) {
			osif_sem_create(&bt_a2dp_demo_src_send_sem, 0, BT_TIMEOUT_FOREVER);
		}
		if (a2dp_task.hdl == NULL) {
			if (false == osif_sem_create(&a2dp_task.sem, 0, 1)) {
				BT_LOGE("[A2DP Demo] Create a2dp_task.sem fail \r\n");
				goto fail;
			}
			if (false == osif_task_create(&a2dp_task.hdl, "a2dp_task",
										  a2dp_task_entry, NULL,
										  2048, 4)) {
				BT_LOGE("[A2DP Demo] Create a2dp_task fail \r\n");
				goto fail;
			}
			osif_sem_take(a2dp_task.sem, BT_TIMEOUT_FOREVER);
		}
		if (!bt_a2dp_demo_src_send_timer.handler) {
			a2dp_src_data_send_interval_us = ((resample_in_frames * demo_in_channels * 16 / 8) * 1000) / (demo_in_rate * demo_in_channels * (16 / 8) / 1000) + 1;
			BT_LOGA("[A2DP Demo] rtk_bt_a2dp_demo_src_send_data_control send interval(us) is %d \r\n", a2dp_src_data_send_interval_us);
			if (bt_a2dp_demo_src_send_timer.handler == NULL) {
				gtimer_init(&bt_a2dp_demo_src_send_timer, A2DP_SEND_TIMER_ID);
				gtimer_start_periodical(&bt_a2dp_demo_src_send_timer, a2dp_src_data_send_interval_us, (void *)bt_a2dp_demo_src_send_timer_handler, NULL);
			}
		}
	} else {
		if (bt_a2dp_demo_src_send_data_enable == false) {
			BT_LOGE("[APP] %s: send data is alreay disabled\r\n", __func__);
			return;
		}
		bt_a2dp_demo_src_send_data_enable = false;
		if (bt_a2dp_demo_src_send_timer.handler) {
			gtimer_stop(&bt_a2dp_demo_src_send_timer);
			gtimer_deinit(&bt_a2dp_demo_src_send_timer);
			bt_a2dp_demo_src_send_timer.handler = NULL;
		}
		if (a2dp_task.hdl) {
			a2dp_task.run = 0;
			osif_sem_give(bt_a2dp_demo_src_send_sem);
			osif_sem_take(a2dp_task.sem, BT_TIMEOUT_FOREVER);
			osif_sem_delete(a2dp_task.sem);
			memset((void *)&a2dp_task, 0, sizeof(struct a2dp_demo_task_t));
		}
		if (bt_a2dp_demo_src_send_sem) {
			osif_sem_delete(bt_a2dp_demo_src_send_sem);
			bt_a2dp_demo_src_send_sem = NULL;
		}
	}
	return;

fail:
	if (bt_a2dp_demo_src_send_timer.handler) {
		gtimer_stop(&bt_a2dp_demo_src_send_timer);
		gtimer_deinit(&bt_a2dp_demo_src_send_timer);
		bt_a2dp_demo_src_send_timer.handler = NULL;
	}
	if (a2dp_task.hdl) {
		a2dp_task.run = 0;
		osif_sem_give(bt_a2dp_demo_src_send_sem);
		osif_sem_take(a2dp_task.sem, BT_TIMEOUT_FOREVER);
		osif_sem_delete(a2dp_task.sem);
		memset((void *)&a2dp_task, 0, sizeof(struct a2dp_demo_task_t));
	}
	if (a2dp_task.sem) {
		osif_sem_delete(a2dp_task.sem);
		a2dp_task.sem = NULL;
	}
	if (bt_a2dp_demo_src_send_sem) {
		osif_sem_delete(bt_a2dp_demo_src_send_sem);
		bt_a2dp_demo_src_send_sem = NULL;
	}
}

static rtk_bt_evt_cb_ret_t rtk_bt_a2dp_app_callback(uint8_t evt_code, void *param, uint32_t len)
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
		src_a2dp_credits = RTK_BT_A2DP_SRC_STREAM_MAX_CREDITS;
		memcpy((void *)remote_bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[A2DP] A2DP connection completion with %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				remote_bd_addr[5], remote_bd_addr[4], remote_bd_addr[3], remote_bd_addr[2], remote_bd_addr[1], remote_bd_addr[0]);
		BT_AT_PRINT("+BTA2DP:conn,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					remote_bd_addr[5], remote_bd_addr[4], remote_bd_addr[3], remote_bd_addr[2], remote_bd_addr[1], remote_bd_addr[0]);
	}
	break;

	case RTK_BT_A2DP_EVT_DISCONN_CMPL: {
		rtk_bt_a2dp_disconn_ind_t *disconn_ind = (rtk_bt_a2dp_disconn_ind_t *)param;
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
			rtk_bt_a2dp_demo_src_send_data_control(false);
			rtk_bt_avrcp_play_status_change_req(disconn_ind->bd_addr, RTK_BT_AVRCP_STATUS_STOPPED);
		}
		rtk_bt_audio_codec_remove(audio_a2dp_codec_conf.codec_index, a2dp_demo_codec_entity);
		memset((void *)&audio_a2dp_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
		if (g_audio_resample_t) {
			app_bt_pcm_data_resample_engine_destroy(&g_audio_resample_t);
		}
		if (!hfp_demo_audio_track_hdl) {
			rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_audio_track_hdl);
		} else {
			rtk_bt_audio_deinit();
		}
		a2dp_demo_audio_track_hdl = NULL;
		a2dp_demo_codec_entity = NULL;
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
		if ((pa2dp_codec->codec_type & RTK_BT_AUDIO_CODEC_SBC) == 0) {
			BT_LOGE("[A2DP] Not support codec %d \r\n", pa2dp_codec->codec_type);
			break;
		}
		if (rtk_bt_audio_init()) {
			BT_LOGE("[A2DP] rtk_bt_audio_init fail \r\n");
			break;
		}
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SNK) {
			if (pa2dp_codec->codec_type == RTK_BT_AUDIO_CODEC_SBC) {
				ret = rtk_bt_a2dp_sbc_parse_decoder_struct(pa2dp_codec, &sbc_codec_t.decoder_t);
				audio_a2dp_codec_conf.codec_index = RTK_BT_AUDIO_CODEC_SBC;
				audio_a2dp_codec_conf.param = (void *)&sbc_codec_t;
				audio_a2dp_codec_conf.param_len = sizeof(sbc_codec_t);
			}
			if (ret) {
				BT_LOGE("[A2DP] RTK_BT_A2DP_EVT_CONFIG_CMPL Fail \r\n");
				if (!hfp_demo_audio_track_hdl) {
					rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_audio_track_hdl);
				} else {
					rtk_bt_audio_deinit();
				}
				a2dp_demo_audio_track_hdl = NULL;
				a2dp_demo_codec_entity = NULL;
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
		a2dp_demo_codec_entity = rtk_bt_audio_codec_add(&audio_a2dp_codec_conf);
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
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
			// set BR/EDR tpoll to 3.75ms
			uint16_t tpoll = 0x06;
			if (RTK_BT_OK == rtk_bt_br_gap_set_link_qos(p_stream_open_t->bd_addr, RTK_BT_BR_QOS_TYPE_BEST_EFFORT, tpoll)) {
				BT_LOGA("[A2DP] set link qos with %02x:%02x:%02x:%02x:%02x:%02x success, tpoll:0x%x \r\n",
						p_stream_open_t->bd_addr[5], p_stream_open_t->bd_addr[4], p_stream_open_t->bd_addr[3], p_stream_open_t->bd_addr[2], p_stream_open_t->bd_addr[1],
						p_stream_open_t->bd_addr[0],
						tpoll);
			}
			/* calculate pkt number and update */
			sbc_codec_t.encoder_t.sbc_pkt_num = p_stream_open_t->max_pkt_len / sbc_codec_t.encoder_t.frame_size;
			rtk_bt_audio_codec_update(&audio_a2dp_codec_conf, a2dp_demo_codec_entity);
			src_a2dp_send_data_size = sbc_codec_t.encoder_t.sbc_pkt_num * sbc_codec_t.encoder_t.blocks * sbc_codec_t.encoder_t.subbands * ((
										  sbc_codec_t.encoder_t.channel_mode == 0) ? 1 : 2);
			/* default audio stream source is 48000 sample rate, 2 channels */
			app_bt_pcm_data_resample_engine_alloc(&g_audio_resample_t, demo_in_rate, demo_in_channels, sbc_codec_t.encoder_t.sample_rate,
												  ((sbc_codec_t.encoder_t.channel_mode != 0) ? 2 : 1));
			BT_LOGA("[A2DP] sbc_pkt_num is update to %d, src_a2dp_send_data_size is %d \r\n", sbc_codec_t.encoder_t.sbc_pkt_num, src_a2dp_send_data_size);
		}
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_START_IND: {
		rtk_bt_a2dp_stream_start_t *pa2dp_stream = (rtk_bt_a2dp_stream_start_t *)param;

		BT_LOGA("[A2DP] BT_EVENT_A2DP_STREAM_START_IND with %02x:%02x:%02x:%02x:%02x:%02x \r\n",
				pa2dp_stream->bd_addr[5], pa2dp_stream->bd_addr[4], pa2dp_stream->bd_addr[3], pa2dp_stream->bd_addr[2], pa2dp_stream->bd_addr[1],
				pa2dp_stream->bd_addr[0]);
		BT_AT_PRINT("+BTA2DP:start,%02x:%02x:%02x:%02x:%02x:%02x,%d\r\n",
					pa2dp_stream->bd_addr[5], pa2dp_stream->bd_addr[4], pa2dp_stream->bd_addr[3], pa2dp_stream->bd_addr[2], pa2dp_stream->bd_addr[1],
					pa2dp_stream->bd_addr[0]);
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SNK) {
			if (a2dp_demo_audio_track_hdl) {
				rtk_bt_audio_track_resume(a2dp_demo_audio_track_hdl->audio_track_hdl);
			}
		} else if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
			if (bt_a2dp_demo_src_send_data_enable) {
				if (bt_a2dp_demo_src_send_timer.handler) {
					gtimer_start_periodical(&bt_a2dp_demo_src_send_timer, a2dp_src_data_send_interval_us, (void *)bt_a2dp_demo_src_send_timer_handler, NULL);
				} else {
					BT_LOGE("[APP] %s: timer not init\r\n", __func__);
				}
			} else {
				rtk_bt_a2dp_demo_src_send_data_control(true);
			}
			rtk_bt_avrcp_play_status_change_req(pa2dp_stream->bd_addr, RTK_BT_AVRCP_STATUS_PLAYING);
		}
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_START_RSP: {
		memcpy((void *)bd_addr, param, 6);

		BT_LOGA("[A2DP] RTK_BT_A2DP_EVT_STREAM_START_RSP from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		if (bt_a2dp_demo_src_send_data_enable) {
			if (bt_a2dp_demo_src_send_timer.handler) {
				gtimer_start_periodical(&bt_a2dp_demo_src_send_timer, a2dp_src_data_send_interval_us, (void *)bt_a2dp_demo_src_send_timer_handler, NULL);
			} else {
				BT_LOGE("[APP] %s: timer not init\r\n", __func__);
			}
		} else {
			rtk_bt_a2dp_demo_src_send_data_control(true);
		}
		rtk_bt_avrcp_play_status_change_req(bd_addr, RTK_BT_AVRCP_STATUS_PLAYING);
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_STOP: {
		rtk_bt_a2dp_stream_stop_t *p_stream_stop_t = (rtk_bt_a2dp_stream_stop_t *)param;

		BT_LOGA("[A2DP] Stream stop from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_stream_stop_t->bd_addr[5], p_stream_stop_t->bd_addr[4], p_stream_stop_t->bd_addr[3],
				p_stream_stop_t->bd_addr[2], p_stream_stop_t->bd_addr[1], p_stream_stop_t->bd_addr[0]);
		BT_AT_PRINT("+BTA2DP:stop,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					p_stream_stop_t->bd_addr[5], p_stream_stop_t->bd_addr[4], p_stream_stop_t->bd_addr[3],
					p_stream_stop_t->bd_addr[2], p_stream_stop_t->bd_addr[1], p_stream_stop_t->bd_addr[0]);
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
			if (bt_a2dp_demo_src_send_timer.handler) {
				gtimer_stop(&bt_a2dp_demo_src_send_timer);
			} else {
				BT_LOGE("[APP] %s: timer not init\r\n", __func__);
			}
			rtk_bt_avrcp_play_status_change_req(p_stream_stop_t->bd_addr, RTK_BT_AVRCP_STATUS_PAUSED);
		}
		if (a2dp_demo_audio_track_hdl) {
			rtk_bt_audio_track_pause(a2dp_demo_audio_track_hdl->audio_track_hdl);
		}
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_CLOSE: {
		rtk_bt_a2dp_stream_close_t *p_stream_close_t = (rtk_bt_a2dp_stream_close_t *)param;

		BT_LOGA("[A2DP] Stream close from %02x:%02x:%02x:%02x:%02x:%02x, cause 0x%x \r\n",
				p_stream_close_t->bd_addr[5], p_stream_close_t->bd_addr[4], p_stream_close_t->bd_addr[3],
				p_stream_close_t->bd_addr[2], p_stream_close_t->bd_addr[1], p_stream_close_t->bd_addr[0],
				p_stream_close_t->cause);
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
			rtk_bt_a2dp_demo_src_send_data_control(false);
			rtk_bt_avrcp_play_status_change_req(p_stream_close_t->bd_addr, RTK_BT_AVRCP_STATUS_STOPPED);
		}
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_DATA_IND: { //BT api shall not be called here
		rtk_bt_a2dp_stream_data_ind_t *pdata_in = (rtk_bt_a2dp_stream_data_ind_t *)param;

		if (rtk_bt_audio_recvd_data_in(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_audio_track_hdl, a2dp_demo_codec_entity, pdata_in->data, pdata_in->length, 0)) {
			BT_LOGE("[A2DP] Stream Data Receiving FAIL %d \r\n", RTK_BT_AUDIO_CODEC_SBC);
		}
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_DATA_RSP: {
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
			src_a2dp_credits++;
			if (src_a2dp_credits > RTK_BT_A2DP_SRC_STREAM_MAX_CREDITS) {
				src_a2dp_credits = RTK_BT_A2DP_SRC_STREAM_MAX_CREDITS;
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

static uint16_t rtk_bt_hfp_cvsd_parse_decoder_struct(rtk_bt_hfp_codec_t *phfp_codec, rtk_bt_cvsd_decode_t *pcvsd_decoder_t)
{
#if defined(CONFIG_BT_AUDIO_NOISE_CANCELLATION) && CONFIG_BT_AUDIO_NOISE_CANCELLATION
	/* copy left channel to right channel
	bt audio noise cancellation mic loop back need right channel */
	pcvsd_decoder_t->channel_num = 2;
	pcvsd_decoder_t->sample_rate = phfp_codec->cvsd.sample_rate;
	pcvsd_decoder_t->frame_duration = phfp_codec->cvsd.frame_duration;
	hfp_demo_audio_track_hdl = rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_CVSD, (float)DEFAULT_AUDIO_LEFT_VOLUME, (float)DEFAULT_AUDIO_RIGHT_VOLUME,
													  pcvsd_decoder_t->channel_num,
													  pcvsd_decoder_t->sample_rate, BT_AUDIO_FORMAT_PCM_16_BIT, 0, NULL, true);
	record_frame_samples_per_channel = 256;
#else
	pcvsd_decoder_t->channel_num = phfp_codec->cvsd.channel_num;
	pcvsd_decoder_t->sample_rate = phfp_codec->cvsd.sample_rate;
	pcvsd_decoder_t->frame_duration = phfp_codec->cvsd.frame_duration;
	hfp_demo_audio_track_hdl = rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_CVSD, (float)DEFAULT_AUDIO_LEFT_VOLUME, (float)0, pcvsd_decoder_t->channel_num,
													  pcvsd_decoder_t->sample_rate, BT_AUDIO_FORMAT_PCM_16_BIT, 0, NULL, true);
	if (phfp_codec->cvsd.frame_duration == RTK_BT_AUDIO_CVSD_FRAME_DURATION_7_5_MS) {
		record_frame_samples_per_channel = 60; // (8000 / 1000) * 7.5
	} else {
		record_frame_samples_per_channel = 30; // (8000 / 1000) * 3.75
	}
#endif
	if (!hfp_demo_audio_track_hdl) {
		BT_LOGE("[HFP] bt audio track add fail \r\n");
		return 1;
	}
	BT_LOGA("cvsd audio record sample_rate %d, %d channels, %d samples\n",
			pcvsd_decoder_t->sample_rate, AUDIO_RECORD_CHANNELS, record_frame_samples_per_channel);
#if defined(CONFIG_BT_AUDIO_NOISE_CANCELLATION) && CONFIG_BT_AUDIO_NOISE_CANCELLATION
	if (rtk_bt_audio_record_config(&hfp_demo_audio_record_table)) {
		BT_LOGE("[HFP] rtk_bt_audio_record_config fail \r\n");
		rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_CVSD, hfp_demo_audio_track_hdl);
		hfp_demo_audio_track_hdl = NULL;
		return 1;
	}
	/* AIVoice use 16k record */
	hfp_demo_audio_record_hdl = rtk_bt_audio_record_add(RTK_BT_AUDIO_CODEC_CVSD, AUDIO_RECORD_CHANNELS, 16000,
														record_frame_samples_per_channel * AUDIO_RECORD_CHANNELS * sizeof(int16_t), 0x7f);
#else
	hfp_demo_audio_record_hdl = rtk_bt_audio_record_add(RTK_BT_AUDIO_CODEC_CVSD, AUDIO_RECORD_CHANNELS, pcvsd_decoder_t->sample_rate,
														record_frame_samples_per_channel * AUDIO_RECORD_CHANNELS * sizeof(int16_t), 0x7f);
#endif
	if (!hfp_demo_audio_record_hdl) {
		BT_LOGE("[HFP] bt audio record add fail \r\n");
		rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_CVSD, hfp_demo_audio_track_hdl);
		hfp_demo_audio_track_hdl = NULL;
		return 1;
	}

	return 0;
}

static uint16_t rtk_bt_hfp_sbc_parse_decoder_struct(rtk_bt_hfp_codec_t *phfp_codec, rtk_bt_sbc_decode_t *psbc_decoder_t)
{
#if defined(CONFIG_BT_AUDIO_NOISE_CANCELLATION) && CONFIG_BT_AUDIO_NOISE_CANCELLATION
	/* copy left channel to right channel
	bt audio noise cancellation mic loop back need right channel */
	psbc_decoder_t->sbc_dec_mode = (sbc_mode_t)SBC_MODE_mSBC;
	psbc_decoder_t->sampling_frequency = phfp_codec->msbc.sample_rate;
	psbc_decoder_t->channel_mode = SBC_CHANNEL_MODE_DUAL_CHANNEL;
	psbc_decoder_t->block_length = phfp_codec->msbc.block_length;
	psbc_decoder_t->subbands = phfp_codec->msbc.subbands;
	psbc_decoder_t->allocation_method = phfp_codec->msbc.allocation_method;
	psbc_decoder_t->min_bitpool = phfp_codec->msbc.bitpool;
	psbc_decoder_t->max_bitpool = phfp_codec->msbc.bitpool;
	hfp_demo_audio_track_hdl = rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_mSBC, (float)DEFAULT_AUDIO_LEFT_VOLUME, (float)DEFAULT_AUDIO_RIGHT_VOLUME, 2,
													  psbc_decoder_t->sampling_frequency, BT_AUDIO_FORMAT_PCM_16_BIT, 0, NULL, true);
	record_frame_samples_per_channel = 256;
#else
	psbc_decoder_t->sbc_dec_mode = (sbc_mode_t)SBC_MODE_mSBC;
	psbc_decoder_t->sampling_frequency = phfp_codec->msbc.sample_rate;
	psbc_decoder_t->channel_mode = phfp_codec->msbc.channel_mode;
	psbc_decoder_t->block_length = phfp_codec->msbc.block_length;
	psbc_decoder_t->subbands = phfp_codec->msbc.subbands;
	psbc_decoder_t->allocation_method = phfp_codec->msbc.allocation_method;
	psbc_decoder_t->min_bitpool = phfp_codec->msbc.bitpool;
	psbc_decoder_t->max_bitpool = phfp_codec->msbc.bitpool;
	hfp_demo_audio_track_hdl = rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_mSBC, (float)DEFAULT_AUDIO_LEFT_VOLUME, (float)0, 1,
													  psbc_decoder_t->sampling_frequency, BT_AUDIO_FORMAT_PCM_16_BIT, 0, NULL, true);
	record_frame_samples_per_channel = (1 * psbc_decoder_t->block_length * psbc_decoder_t->subbands * 1 * 2) / (16 / 8);
#endif
	if (!hfp_demo_audio_track_hdl) {
		BT_LOGE("[HFP] bt audio track add fail \r\n");
		return 1;
	}
	BT_LOGA("msbc audio record sample_rate %d, %d channels, %d samples\n",
			psbc_decoder_t->sampling_frequency, AUDIO_RECORD_CHANNELS, record_frame_samples_per_channel);
#if defined(CONFIG_BT_AUDIO_NOISE_CANCELLATION) && CONFIG_BT_AUDIO_NOISE_CANCELLATION
	if (rtk_bt_audio_record_config(&hfp_demo_audio_record_table)) {
		BT_LOGE("[HFP] rtk_bt_audio_record_config fail \r\n");
		rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_mSBC, hfp_demo_audio_track_hdl);
		hfp_demo_audio_track_hdl = NULL;
		return 1;
	}
	/* AIVoice use 16k record */
	hfp_demo_audio_record_hdl = rtk_bt_audio_record_add(RTK_BT_AUDIO_CODEC_mSBC, AUDIO_RECORD_CHANNELS, 16000,
														record_frame_samples_per_channel * AUDIO_RECORD_CHANNELS * sizeof(int16_t), 0x7f);
#else
	hfp_demo_audio_record_hdl = rtk_bt_audio_record_add(RTK_BT_AUDIO_CODEC_mSBC, AUDIO_RECORD_CHANNELS, psbc_decoder_t->sampling_frequency,
														record_frame_samples_per_channel * AUDIO_RECORD_CHANNELS * sizeof(int16_t), 0x7f);
#endif
	if (!hfp_demo_audio_record_hdl) {
		BT_LOGE("[HFP] bt audio record add fail \r\n");
		rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_mSBC, hfp_demo_audio_track_hdl);
		hfp_demo_audio_track_hdl = NULL;
		return 1;
	}

	return 0;
}

#if defined(CONFIG_BT_AUDIO_NOISE_CANCELLATION) && CONFIG_BT_AUDIO_NOISE_CANCELLATION
static int16_t record_buffer[MAX_RECORD_FRAME_SAMPLES_PER_CHANNLE * AUDIO_RECORD_CHANNELS] = {0};
static int16_t nc_buffer[MAX_RECORD_FRAME_SAMPLES_PER_CHANNLE * AUDIO_RECORD_CHANNELS] = {0};

static void nc_task_entry(void *ctx)
{
	(void)ctx;
	static int read_size = 0;

	nc_task.run = 1;
	osif_sem_give(nc_task.sem);
	while (nc_task.run) {
		read_size = rtk_bt_audio_record_data_get(audio_hfp_codec_conf.codec_index, hfp_demo_audio_record_hdl, hfp_demo_codec_entity, (void *)record_buffer,
												 record_frame_samples_per_channel * AUDIO_RECORD_CHANNELS * sizeof(int16_t),
												 true);
		rtk_bt_audio_noise_cancellation_feed(record_buffer, read_size);
	}
	osif_sem_give(nc_task.sem);
	osif_task_delete(NULL);
}

static void hfp_task_entry(void *ctx)
{
	(void)ctx;
	struct enc_codec_buffer *penc_codec_buffer_t = NULL;
	rtk_bt_hfp_sco_data_send_t sco_data_t = {0};

	hfp_task.run = 1;
	osif_sem_give(hfp_task.sem);

	while (hfp_task.run) {
		if (bt_hfp_demo_sco_send_sem) {
			osif_sem_take(bt_hfp_demo_sco_send_sem, BT_TIMEOUT_FOREVER);
		}
		if (rtk_bt_audio_noise_cancellation_data_get(nc_buffer,
													 audio_hfp_codec_conf.codec_index == RTK_BT_AUDIO_CODEC_CVSD ? BT_ENCODE_FRAME_BYTES : (2 * BT_ENCODE_FRAME_BYTES))) {
			penc_codec_buffer_t = rtk_bt_audio_data_encode(audio_hfp_codec_conf.codec_index, hfp_demo_codec_entity,
														   nc_buffer, audio_hfp_codec_conf.codec_index == RTK_BT_AUDIO_CODEC_CVSD ? BT_ENCODE_FRAME_BYTES : (2 * BT_ENCODE_FRAME_BYTES));
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
			rtk_bt_audio_free_encode_buffer(audio_hfp_codec_conf.codec_index, hfp_demo_codec_entity, penc_codec_buffer_t);
		} else {
			BT_LOGE("[HFP]noise cancellation data get fail \r\n");
		}
	}
	osif_sem_give(hfp_task.sem);
	osif_task_delete(NULL);
}
#else
/* max record_frame_samples_per_channel for hfp without noise cancellation is 120(mSBC) */
static int16_t voice_buffer[120 * AUDIO_RECORD_CHANNELS] = {0};
static int16_t record_buffer[120 * AUDIO_RECORD_CHANNELS] = {0};
static int16_t temp_buffer[120 * AUDIO_RECORD_CHANNELS] = {0};

static void record_task_entry(void *ctx)
{
	(void)ctx;
	static uint32_t read_size = 0;

	record_task.p_buffer = RingBuffer_Create(NULL, record_frame_samples_per_channel * AUDIO_RECORD_CHANNELS * sizeof(int16_t) * 4, LOCAL_RINGBUFF, 1);
	record_task.run = 1;
	osif_sem_give(record_task.sem);

	while (record_task.run) {
		read_size = rtk_bt_audio_record_data_get(audio_hfp_codec_conf.codec_index, hfp_demo_audio_record_hdl, hfp_demo_codec_entity, (void *)voice_buffer,
												 record_frame_samples_per_channel * AUDIO_RECORD_CHANNELS * sizeof(int16_t),
												 true);
		if (record_task.p_buffer) {
			if (RingBuffer_Space(record_task.p_buffer) >= read_size) {
				RingBuffer_Write(record_task.p_buffer, (uint8_t *)voice_buffer, read_size);
			} else {
				BT_LOGE("[HFP]record_task.p_buffer overlap occured \r\n");
				RingBuffer_Read(record_task.p_buffer, (void *)temp_buffer, read_size);
				RingBuffer_Write(record_task.p_buffer, (uint8_t *)voice_buffer, read_size);
			}
		} else {
			BT_LOGE("[HFP]record_task.p_buffer create fail \r\n");
			osif_delay(100);
		}
	}
	RingBuffer_Destroy(record_task.p_buffer);
	osif_sem_give(record_task.sem);
	osif_task_delete(NULL);
}

static void hfp_task_entry(void *ctx)
{
	(void)ctx;
	static int read_size = 0;
	struct enc_codec_buffer *penc_codec_buffer_t = NULL;
	rtk_bt_hfp_sco_data_send_t sco_data_t = {0};

	hfp_task.run = 1;
	osif_sem_give(hfp_task.sem);

	while (hfp_task.run) {
		if (bt_hfp_demo_sco_send_sem) {
			osif_sem_take(bt_hfp_demo_sco_send_sem, BT_TIMEOUT_FOREVER);
		}
		if (record_task.p_buffer) {
			if (RingBuffer_Available(record_task.p_buffer) >= record_frame_samples_per_channel * AUDIO_RECORD_CHANNELS * sizeof(int16_t)) {
				RingBuffer_Read(record_task.p_buffer, (void *)record_buffer, record_frame_samples_per_channel * AUDIO_RECORD_CHANNELS * sizeof(int16_t));
				read_size = record_frame_samples_per_channel * AUDIO_RECORD_CHANNELS * sizeof(int16_t);
			} else {
				read_size = 0;
			}
		}
		if (read_size) {
			penc_codec_buffer_t = rtk_bt_audio_data_encode(audio_hfp_codec_conf.codec_index, hfp_demo_codec_entity,
														   record_buffer, read_size);
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
			rtk_bt_audio_free_encode_buffer(audio_hfp_codec_conf.codec_index, hfp_demo_codec_entity, penc_codec_buffer_t);
		} else {
			BT_LOGE("[HFP]record ringbuffer read fail \r\n");
		}
	}
	osif_sem_give(hfp_task.sem);
	osif_task_delete(NULL);
}
#endif

static void bt_hfp_demo_sco_send_timer_handler(void *arg)
{
	(void)arg;

	if (hfp_task.run) {
		if (bt_hfp_demo_sco_send_sem) {
			osif_sem_give(bt_hfp_demo_sco_send_sem);
		}
	}
}

static void hfp_alert_timer_handle(void *arg)
{
	(void)arg;
	uint32_t pcm_offset = 0;

	while (pcm_offset < birds_sing_size / 2) {
		if (pcm_offset + 512 < birds_sing_size / 2) {
			if (alert_track_hdl) {
				rtk_bt_audio_track_play(alert_track_hdl, (void *)(birds_sing + pcm_offset), (uint32_t)1024);
			}
		} else {
			if (alert_track_hdl) {
				rtk_bt_audio_track_play(alert_track_hdl, (void *)(birds_sing + pcm_offset), (uint32_t)(((birds_sing_size / 2) - pcm_offset) * 2));
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
	rtk_bt_audio_track_set_hardware_volume(hw_volume, hw_volume);
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

static void rtk_bt_hfp_demo_sco_send_data_control(bool enable)
{
	BT_LOGA("[APP] %s %d\r\n", __func__, enable);
	if (enable) {
		if (bt_hfp_demo_sco_send_data_enable == true) {
			BT_LOGE("[APP] %s: send data is alreay enabled\r\n", __func__);
			return;
		}
		bt_hfp_demo_sco_send_data_enable = true;
		if (bt_hfp_demo_sco_send_sem == NULL) {
			osif_sem_create(&bt_hfp_demo_sco_send_sem, 0, BT_TIMEOUT_FOREVER);
		}
#if defined(CONFIG_BT_AUDIO_NOISE_CANCELLATION) && CONFIG_BT_AUDIO_NOISE_CANCELLATION
		if (nc_task.hdl == NULL) {
			rtk_bt_audio_noise_cancellation_new(audio_hfp_codec_conf.codec_index, AUDIO_RECORD_CHANNELS);
			/* create noise cancellation thread */
			if (false == osif_sem_create(&nc_task.sem, 0, 1)) {
				BT_LOGE("[HFP Demo] Create nc_task sema Fail\r\n");
				goto fail;
			}
			if (false == osif_task_create(&nc_task.hdl, "nc_task",
										  nc_task_entry, NULL,
										  6144, 4)) {
				osif_sem_delete(nc_task.sem);
				goto fail;
			}
			osif_sem_take(nc_task.sem, 0xffffffff);
		}
#else
		if (record_task.hdl == NULL) {
			if (false == osif_sem_create(&record_task.sem, 0, 1)) {
				BT_LOGE("[HFP Demo] Create record_task.sem fail \r\n");
				goto fail;
			}
			if (false == osif_task_create(&record_task.hdl, "record_task",
										  record_task_entry, NULL,
										  6144, 4)) {
				BT_LOGE("[HFP Demo] Create record_task fail \r\n");
				goto fail;
			}
			osif_sem_take(record_task.sem, BT_TIMEOUT_FOREVER);
		}
#endif
		if (hfp_task.hdl == NULL) {
			if (false == osif_sem_create(&hfp_task.sem, 0, 1)) {
				BT_LOGE("[HFP Demo] Create hfp_task.sem fail \r\n");
				goto fail;
			}
			if (false == osif_task_create(&hfp_task.hdl, "hfp_task",
										  hfp_task_entry, NULL,
										  6144, 4)) {
				BT_LOGE("[HFP Demo] Create hfp_task fail \r\n");
				goto fail;
			}
			osif_sem_take(hfp_task.sem, BT_TIMEOUT_FOREVER);
		}
		if (!bt_hfp_demo_sco_send_timer.handler) {
			if (record_frame_samples_per_channel == 30) {
				hfp_sco_data_send_interval_us = 3750; // 3.75 ms
			} else {
				hfp_sco_data_send_interval_us = 7500; // 7.5 ms
			}
			BT_LOGA("[HFP Demo] rtk_bt_hfp_demo_sco_send_data_control send interval(us) is %d \r\n", hfp_sco_data_send_interval_us);
			if (bt_hfp_demo_sco_send_timer.handler == NULL) {
				gtimer_init(&bt_hfp_demo_sco_send_timer, HFP_SCO_SEND_TIMER_ID);
				gtimer_start_periodical(&bt_hfp_demo_sco_send_timer, hfp_sco_data_send_interval_us, (void *)bt_hfp_demo_sco_send_timer_handler, NULL);
			}
		}
	} else {
		if (bt_hfp_demo_sco_send_data_enable == false) {
			BT_LOGE("[APP] %s: send data is alreay disabled\r\n", __func__);
			return;
		}
		bt_hfp_demo_sco_send_data_enable = false;
		if (bt_hfp_demo_sco_send_timer.handler) {
			gtimer_stop(&bt_hfp_demo_sco_send_timer);
			gtimer_deinit(&bt_hfp_demo_sco_send_timer);
			bt_hfp_demo_sco_send_timer.handler = NULL;
		}
		if (hfp_task.hdl) {
			hfp_task.run = 0;
			osif_sem_give(bt_hfp_demo_sco_send_sem);
			osif_sem_take(hfp_task.sem, BT_TIMEOUT_FOREVER);
			osif_sem_delete(hfp_task.sem);
			memset((void *)&hfp_task, 0, sizeof(struct hfp_task_t));
		}
#if defined(CONFIG_BT_AUDIO_NOISE_CANCELLATION) && CONFIG_BT_AUDIO_NOISE_CANCELLATION
		if (nc_task.hdl) {
			nc_task.run = 0;
			osif_sem_take(nc_task.sem, BT_TIMEOUT_FOREVER);
			osif_sem_delete(nc_task.sem);
			memset((void *)&nc_task, 0, sizeof(struct nc_task_t));
			rtk_bt_audio_noise_cancellation_destroy();
		}
#else
		if (record_task.hdl) {
			record_task.run = 0;
			osif_sem_give(bt_hfp_demo_sco_send_sem);
			osif_sem_take(record_task.sem, BT_TIMEOUT_FOREVER);
			osif_sem_delete(record_task.sem);
			memset((void *)&record_task, 0, sizeof(struct record_task_t));
		}
#endif
		if (bt_hfp_demo_sco_send_sem) {
			osif_sem_delete(bt_hfp_demo_sco_send_sem);
			bt_hfp_demo_sco_send_sem = NULL;
		}
	}
	return;

fail:
	if (bt_hfp_demo_sco_send_timer.handler) {
		gtimer_stop(&bt_hfp_demo_sco_send_timer);
		gtimer_deinit(&bt_hfp_demo_sco_send_timer);
		bt_hfp_demo_sco_send_timer.handler = NULL;
	}
#if defined(CONFIG_BT_AUDIO_NOISE_CANCELLATION) && CONFIG_BT_AUDIO_NOISE_CANCELLATION
	if (nc_task.hdl) {
		nc_task.run = 0;
		osif_sem_take(nc_task.sem, BT_TIMEOUT_FOREVER);
		osif_sem_delete(nc_task.sem);
		memset((void *)&nc_task, 0, sizeof(struct nc_task_t));
	}
#endif
	if (hfp_task.hdl) {
		hfp_task.run = 0;
		osif_sem_give(bt_hfp_demo_sco_send_sem);
		osif_sem_take(hfp_task.sem, BT_TIMEOUT_FOREVER);
		osif_sem_delete(hfp_task.sem);
		memset((void *)&hfp_task, 0, sizeof(struct hfp_task_t));
	}
	if (hfp_task.sem) {
		osif_sem_delete(hfp_task.sem);
		hfp_task.sem = NULL;
	}
	if (bt_hfp_demo_sco_send_sem) {
		osif_sem_delete(bt_hfp_demo_sco_send_sem);
		bt_hfp_demo_sco_send_sem = NULL;
	}
}

static rtk_bt_evt_cb_ret_t rtk_bt_hfp_app_callback(uint8_t evt_code, void *param, uint32_t len)
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
			/* pause a2dp firstly */
			if (a2dp_demo_audio_track_hdl) {
				rtk_bt_avrcp_pause(remote_bd_addr);
			}
		} else if ((RTK_BT_HFP_CALL_INCOMING == p_hfp_call_status_ind->prev_status) && (RTK_BT_HFP_CALL_INCOMING != p_hfp_call_status_ind->curr_status)) {
			if (!ring_alert_inband) {
				BT_LOGA("[HFP] ring alert outband inactive \r\n");
				app_hfp_ring_alert_stop();
			}
		} else if ((RTK_BT_HFP_CALL_IDLE != p_hfp_call_status_ind->prev_status) && (RTK_BT_HFP_CALL_IDLE == p_hfp_call_status_ind->curr_status)) {
			/* play a2dp */
			if (a2dp_demo_audio_track_hdl) {
				rtk_bt_avrcp_play(remote_bd_addr);
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
		memcpy((void *)remote_bd_addr, conn_ind->bd_addr, 6);
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
		/* pause a2dp firstly */
		if (a2dp_demo_audio_track_hdl) {
			rtk_bt_avrcp_pause(remote_bd_addr);
		}
		if ((phfp_codec->codec_type & (RTK_BT_AUDIO_CODEC_CVSD | RTK_BT_AUDIO_CODEC_mSBC)) == 0) {
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
		} else {
			if (audio_a2dp_codec_conf.codec_index == RTK_BT_AUDIO_CODEC_SBC) {
				/* Firstly deinit sbc codec for a2dp, then init msbc for hfp */
				rtk_bt_audio_codec_remove(audio_a2dp_codec_conf.codec_index, a2dp_demo_codec_entity);
			}
			ret = rtk_bt_hfp_sbc_parse_decoder_struct(phfp_codec, &msbc_codec_t.decoder_t);
			audio_hfp_codec_conf.codec_index = RTK_BT_AUDIO_CODEC_mSBC;
			audio_hfp_codec_conf.param = (void *)&msbc_codec_t;
			audio_hfp_codec_conf.param_len = sizeof(msbc_codec_t);
		}
		if (ret) {
			BT_LOGE("[HFP] RTK_BT_HFP_EVT_SCO_CONN_CMPL Fail \r\n");
			if (a2dp_demo_audio_track_hdl) {
				rtk_bt_avrcp_play(remote_bd_addr);
			} else {
				rtk_bt_audio_deinit();
			}
			hfp_demo_audio_track_hdl = NULL;
			hfp_demo_audio_record_hdl = NULL;
			break;
		}
		hfp_demo_codec_entity = rtk_bt_audio_codec_add(&audio_hfp_codec_conf);
		BT_LOGA("[HFP] Configure Complete CODEC %d \r\n", phfp_codec->codec_type);
		if (phfp_codec->codec_type == RTK_BT_AUDIO_CODEC_mSBC) {
			/* update sbc pkt num to 1 for HFP mSBC */
			msbc_codec_t.encoder_t.sbc_pkt_num = 1;
			rtk_bt_audio_codec_update(&audio_hfp_codec_conf, hfp_demo_codec_entity);
		}
		/* config audio record thread */
		rtk_bt_hfp_demo_sco_send_data_control(true);
	}
	break;

	case RTK_BT_HFP_EVT_SCO_DATA_IND: {
		rtk_bt_hfp_sco_data_ind_t *pdata_in = (rtk_bt_hfp_sco_data_ind_t *)param;

		if (pdata_in->status == RTK_BT_SCO_PKT_STATUS_OK) {
			if (rtk_bt_audio_recvd_data_in(audio_hfp_codec_conf.codec_index, hfp_demo_audio_track_hdl, hfp_demo_codec_entity, pdata_in->data, pdata_in->length, 0)) {
				BT_LOGE("[HFP] SCO Data Receiving FAIL %d \r\n", audio_hfp_codec_conf.codec_index);
			}
		} else {
			BT_LOGD("[HFP] received incorrect sco data, status is 0x%x, length is %d \r\n", pdata_in->status, pdata_in->length);
			if (audio_hfp_codec_conf.codec_index == RTK_BT_AUDIO_CODEC_CVSD) {
				/* CVSD has no plc, directly transmit pcm data */
				memset(pdata_in->data, 0, pdata_in->length);
				if (rtk_bt_audio_recvd_data_in(audio_hfp_codec_conf.codec_index, hfp_demo_audio_track_hdl, hfp_demo_codec_entity, pdata_in->data, pdata_in->length, 0)) {
					BT_LOGE("[HFP] SCO Data Receiving FAIL %d \r\n", audio_hfp_codec_conf.codec_index);
				}
			} else {
				/* do mSBC plc */
				if (rtk_bt_audio_recvd_data_in(audio_hfp_codec_conf.codec_index, hfp_demo_audio_track_hdl, hfp_demo_codec_entity, NULL, 0, 0)) {
					BT_LOGE("[HFP] SCO Data Receiving FAIL %d \r\n", audio_hfp_codec_conf.codec_index);
				}
			}
		}
	}
	break;

	case RTK_BT_HFP_EVT_SCO_DISCONNCTED_IND: {
		rtk_bt_hfp_disconn_ind_t *disconn_ind = (rtk_bt_hfp_disconn_ind_t *)param;
		memcpy((void *)bd_addr, disconn_ind->bd_addr, 6);
		BT_LOGA("[HFP] HFP receive sco disconnected from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		BT_AT_PRINT("+BTHFP:sco_disconn,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		rtk_bt_hfp_demo_sco_send_data_control(false);
		rtk_bt_audio_codec_remove(audio_hfp_codec_conf.codec_index, hfp_demo_codec_entity);
		if (audio_a2dp_codec_conf.codec_index == RTK_BT_AUDIO_CODEC_SBC) {
			/* reinit sbc codec for a2dp */
			a2dp_demo_codec_entity = rtk_bt_audio_codec_add(&audio_a2dp_codec_conf);
		}
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
			/* should not deinit bt audio cause it is still working on a2dp source flow */
			rtk_bt_audio_track_del(audio_hfp_codec_conf.codec_index, hfp_demo_audio_track_hdl);
			rtk_bt_audio_record_del(audio_hfp_codec_conf.codec_index, hfp_demo_audio_record_hdl);
		} else {
			if (a2dp_demo_audio_track_hdl) {
				rtk_bt_audio_track_del(audio_hfp_codec_conf.codec_index, hfp_demo_audio_track_hdl);
				rtk_bt_audio_record_del(audio_hfp_codec_conf.codec_index, hfp_demo_audio_record_hdl);
			} else {
				rtk_bt_audio_deinit();
			}
		}
		memset((void *)&audio_hfp_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
		hfp_demo_audio_track_hdl = NULL;
		hfp_demo_audio_record_hdl = NULL;
		hfp_demo_codec_entity = NULL;
		call_curr_status = 0;
	}
	break;

	case RTK_BT_HFP_EVT_AG_CONN_IND: {
		rtk_bt_hfp_conn_ind_t *conn_ind = (rtk_bt_hfp_conn_ind_t *)param;
		memcpy((void *)bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive AG connection from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_HFP_EVT_AG_CONN_CMPL: {
		rtk_bt_hfp_conn_ind_t *conn_ind = (rtk_bt_hfp_conn_ind_t *)param;
		memcpy((void *)bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive AG connection completion from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		BT_AT_PRINT("+BTHFP:conn,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_HFP_EVT_AG_DISCONN_CMPL: {
		rtk_bt_hfp_disconn_ind_t *disconn_ind = (rtk_bt_hfp_disconn_ind_t *)param;
		memcpy((void *)bd_addr, disconn_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive AG disconnection completion from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		BT_AT_PRINT("+BTHFP:disconn,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_HFP_EVT_AG_INDICATORS_STATUS_REQ: {
		rtk_bt_hfp_ag_indicators_status_req_t *p_ag_ind_status_req = (rtk_bt_hfp_ag_indicators_status_req_t *)param;
		memcpy((void *)bd_addr, p_ag_ind_status_req->bd_addr, 6);
		BT_LOGA("[HFP] Receive AG indicators req from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		/* CONFIG indicator for return value */
		memcpy((void *)p_ag_ind_status_req->ret_info, &demo_ag_call_status_t, sizeof(rtk_bt_hfp_ag_indicators_status_t));
	}
	break;

	case RTK_BT_HFP_EVT_AG_CURR_CALLS_LIST_QUERY: {
		/* query call list */
		rtk_bt_hfp_ag_curr_calls_list_query_t *p_ag_query_calls_list_ind = (rtk_bt_hfp_ag_curr_calls_list_query_t *)param;
		memcpy((void *)bd_addr, p_ag_query_calls_list_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive AG query call list req from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		/* send call list */
	}
	break;

	case RTK_BT_HFP_EVT_AG_DIAL_LAST_NUMBER: {
		rtk_bt_hfp_ag_dial_last_number_ind_t *p_ag_dial_last_num_ind = (rtk_bt_hfp_ag_dial_last_number_ind_t *)param;
		memcpy((void *)bd_addr, p_ag_dial_last_num_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive AG dial last number req from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_HFP_EVT_AG_CALL_ANSWER_REQ: {
		rtk_bt_hfp_ag_call_answer_req_ind_t *p_ag_call_answer_req_ind = (rtk_bt_hfp_ag_call_answer_req_ind_t *)param;
		memcpy((void *)bd_addr, p_ag_call_answer_req_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive AG call answer req from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_HFP_EVT_AG_CALL_TERMINATE_REQ: {
		rtk_bt_hfp_ag_call_terminate_req_ind_t *p_ag_call_terminate_req_ind = (rtk_bt_hfp_ag_call_terminate_req_ind_t *)param;
		memcpy((void *)bd_addr, p_ag_call_terminate_req_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive AG terminate req from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_HFP_EVT_AG_CALL_STATUS: {
		rtk_bt_hfp_call_status_ind_t *p_hfp_call_status_ind = (rtk_bt_hfp_call_status_ind_t *)param;
		memcpy((void *)bd_addr, p_hfp_call_status_ind->bd_addr, 6);
		BT_LOGA("[HFP] Receive AG call status change from %d to %d from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_hfp_call_status_ind->prev_status,
				p_hfp_call_status_ind->curr_status,
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		BT_AT_PRINT("+BTHFP:call_status,%d,%d,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					p_hfp_call_status_ind->prev_status,
					p_hfp_call_status_ind->curr_status,
					bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		/* stop sending a2dp source audio stream */
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
			if (bt_a2dp_demo_src_send_data_enable && (p_hfp_call_status_ind->prev_status == 0) && (p_hfp_call_status_ind->curr_status == 1)) {
				if (bt_a2dp_demo_src_send_timer.handler) {
					gtimer_stop(&bt_a2dp_demo_src_send_timer);
				} else {
					BT_LOGE("[APP] %s: timer not init\r\n", __func__);
				}
			} else if (bt_a2dp_demo_src_send_data_enable && (p_hfp_call_status_ind->prev_status != 0) && (p_hfp_call_status_ind->curr_status == 0)) {
				if (bt_a2dp_demo_src_send_timer.handler) {
					gtimer_start_periodical(&bt_a2dp_demo_src_send_timer, a2dp_src_data_send_interval_us, (void *)bt_a2dp_demo_src_send_timer_handler, NULL);
				} else {
					BT_LOGE("[APP] %s: timer not init\r\n", __func__);
				}
			}
		}
	}
	break;

	case RTK_BT_HFP_EVT_UNKNOWN_EVENT_IND: {
		rtk_bt_hfp_unknown_at_event_t *p_hfp_event_ind = (rtk_bt_hfp_unknown_at_event_t *)param;
		BT_LOGA("[HFP] Receive unknown event from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_hfp_event_ind->bd_addr[5], p_hfp_event_ind->bd_addr[4], p_hfp_event_ind->bd_addr[3], p_hfp_event_ind->bd_addr[2], p_hfp_event_ind->bd_addr[1],
				p_hfp_event_ind->bd_addr[0]);
		BT_LOGA(" %s \r\n", p_hfp_event_ind->at_cmd);
	}
	break;

	default: {
		BT_LOGE("[HFP]: default evt_code 0x%04x \r\n", evt_code);
	}
	break;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_hfp_ag_conf_t demo_ag_conf = {
	.link_num = 1,
	.rfc_hfp_ag_chann_num = RTK_BT_RFC_HFP_AG_CHANN_NUM,
	.rfc_hsp_ag_chann_num = RTK_BT_RFC_HSP_AG_CHANN_NUM,
	.ag_supported_features = RTK_BT_HFP_AG_LOCAL_CAPABILITY_3WAY |
	RTK_BT_HFP_AG_LOCAL_CAPABILITY_VOICE_RECOGNITION |
	RTK_BT_HFP_AG_LOCAL_CAPABILITY_HF_INDICATORS |
	RTK_BT_HFP_AG_LOCAL_CAPABILITY_CODEC_NEGOTIATION |
	RTK_BT_HFP_AG_LOCAL_CAPABILITY_ESCO_S4_T2_SUPPORTED,
	.ag_supported_codecs = RTK_BT_HFP_AG_CODEC_TYPE_CVSD | RTK_BT_HFP_AG_CODEC_TYPE_MSBC
};
static rtk_bt_hfp_hf_conf_t demo_hf_conf = {
	.link_num = 1,
	.rfc_hfp_chann_num = RTK_BT_RFC_HFP_CHANN_NUM,
	.rfc_hsp_chann_num = RTK_BT_RFC_HSP_CHANN_NUM,
	.hf_supported_features = RTK_BT_HFP_HF_LOCAL_THREE_WAY_CALLING |
	RTK_BT_HFP_HF_LOCAL_CLI_PRESENTATION_CAPABILITY |
	RTK_BT_HFP_HF_LOCAL_VOICE_RECOGNITION_ACTIVATION |
	RTK_BT_HFP_HF_LOCAL_ESCO_S4_SETTINGS |
	RTK_BT_HFP_HF_LOCAL_CODEC_NEGOTIATION |
	RTK_BT_HFP_HF_LOCAL_REMOTE_VOLUME_CONTROL,
	.hf_supported_codecs = RTK_BT_HFP_HF_CODEC_TYPE_CVSD | RTK_BT_HFP_AG_CODEC_TYPE_MSBC
};

static void a2dp_demo_reconnect_timer_handle(void *arg)
{
	(void)arg;

	if (a2dp_demo_auto_reconnect_try_count --) {
		BT_LOGA("[A2DP]: try reconnect to %02x:%02x:%02x:%02x:%02x:%02x\r\n", remote_bd_addr[5], remote_bd_addr[4], remote_bd_addr[3], remote_bd_addr[2],
				remote_bd_addr[1], remote_bd_addr[0]);
		rtk_bt_a2dp_connect(remote_bd_addr);
		osif_timer_restart(&reconnect_timer, RTK_BT_DEMO_RECONNECT_TIMER_INTERVAL);
	} else {
		/* auto reconnect fail and enable inquiry scan and page scan */
		rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_VISIBLE_CONNECTABLE);
	}
}

/**
    * @brief  process bt audio a2dp(SNK) hfp(HF) init/ deinit.
    * @param  enable[in]: 1: init. 0 deinit
    * @return none.
    */
int bt_audio_a2dp_hfp_main(uint8_t role, uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_br_bd_addr_t bd_addr = {0};
	char addr_str[30] = {0};
	char dev_name[30] = {0};

	if (1 == enable) {
		if (bt_audio_demo_init_flag) {
			BT_LOGE("%s Already init! \r\n", __func__);
			return -1;
		}
		//set GAP configuration
		bt_app_conf.app_profile_support =   RTK_BT_PROFILE_A2DP | \
											RTK_BT_PROFILE_AVRCP | \
											RTK_BT_PROFILE_HFP | \
											RTK_BT_PROFILE_PBAP | \
											RTK_BT_PROFILE_SDP;
		bt_app_conf.mtu_size = 180;
		bt_app_conf.max_tx_octets = 0x40;
		bt_app_conf.max_tx_time = 0x200;

		rtk_bt_br_gap_default_param_t dft_param;

		dft_param.mtu_size = 180;
		BT_APP_PROCESS(rtk_bt_br_gap_set_default_param(&dft_param));
		/* set supervision_timeout */
		BT_APP_PROCESS(rtk_bt_br_gap_set_supvisiontimeout(&superv_time));
		BT_APP_PROCESS(rtk_bt_br_gap_set_inquiry_param(&inquiry_param));
		BT_APP_PROCESS(rtk_bt_br_gap_set_page_param(&page_param));
		/* sdp record add (must be invoked before rtk_bt_enable) */
		BT_APP_PROCESS(rtk_sdp_record_add((void *)did_sdp_record, sizeof(did_sdp_record)));
		if (RTK_BT_A2DP_ROLE_SNK == role) {
			bt_app_conf.a2dp_role = RTK_BT_A2DP_ROLE_SNK;
			bt_app_conf.hfp_role = RTK_BT_AUDIO_HFP_ROLE_HF;
			/* set class of device */
			BT_APP_PROCESS(rtk_bt_br_gap_set_cod(&cod_for_snk));
			BT_APP_PROCESS(rtk_sdp_record_add((void *)a2dp_sink_sdp_record, sizeof(a2dp_sink_sdp_record)));
			BT_APP_PROCESS(rtk_sdp_record_add((void *)hfp_sdp_record, sizeof(hfp_sdp_record)));
			BT_APP_PROCESS(rtk_bt_hfp_cfg((uint8_t)RTK_BT_AUDIO_HFP_ROLE_HF, (void *)&demo_hf_conf));
			hfp_demo_role = RTK_BT_AUDIO_HFP_ROLE_HF;
			a2dp_demo_role = RTK_BT_A2DP_ROLE_SNK;
		} else if (RTK_BT_A2DP_ROLE_SRC == role) {
			bt_app_conf.a2dp_role = RTK_BT_A2DP_ROLE_SRC;
			bt_app_conf.hfp_role = RTK_BT_AUDIO_HFP_ROLE_AG;
			/* set class of device */
			BT_APP_PROCESS(rtk_bt_br_gap_set_cod(&cod_for_src));
			BT_APP_PROCESS(rtk_sdp_record_add((void *)a2dp_src_sdp_record, sizeof(a2dp_src_sdp_record)));
			BT_APP_PROCESS(rtk_sdp_record_add((void *)hfp_ag_sdp_record, sizeof(hfp_ag_sdp_record)));
			BT_APP_PROCESS(rtk_bt_hfp_cfg((uint8_t)RTK_BT_AUDIO_HFP_ROLE_AG, (void *)&demo_ag_conf));
			hfp_demo_role = RTK_BT_AUDIO_HFP_ROLE_AG;
			a2dp_demo_role = RTK_BT_A2DP_ROLE_SRC;
		} else {
			BT_LOGE("[APP] Wrong A2DP role %d \r\n", role);
			return 1;
		}
		BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_ct_sdp_record, sizeof(avrcp_ct_sdp_record)));
		BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_tg_sdp_record, sizeof(avrcp_tg_sdp_record)));

		/* a2dp codec add (must be invoked before rtk_bt_enable) */
		BT_APP_PROCESS(rtk_bt_a2dp_codec_cfg((uint8_t)RTK_BT_AUDIO_CODEC_SBC, (void *)&codec_sbc, (uint32_t)sizeof(rtk_bt_a2dp_media_codec_sbc_t)));

		/* Enable BT */
		BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
		BT_APP_PROCESS(rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_VISIBLE_CONNECTABLE));

		BT_APP_PROCESS(rtk_bt_br_gap_get_bd_addr(&bd_addr));
		rtk_bt_br_addr_to_str(bd_addr.addr, addr_str, sizeof(addr_str));
		BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

		/* Initilize GAP part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_GAP, br_gap_app_callback));
		/* mix RTK_BT_DEV_NAME with bt mac address */
		strcpy(dev_name, RTK_BT_DEV_NAME);
		snprintf(&dev_name[strlen(RTK_BT_DEV_NAME)], 7, "(%02X%02X)", bd_addr.addr[1], bd_addr.addr[0]);
		BT_APP_PROCESS(rtk_bt_br_gap_set_device_name((const uint8_t *)dev_name));
		/* Initilize SDP part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_SDP, rtk_bt_sdp_app_callback));
		/* Initilize avrcp part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_AVRCP, rtk_bt_avrcp_app_callback));
		/* Initilize a2dp part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_A2DP, rtk_bt_a2dp_app_callback));
		/* Initilize hfp part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_HFP, rtk_bt_hfp_app_callback));
		/* auto reconnect init */
		if (a2dp_demo_auto_reconnect) {
			BT_LOGA("[A2DP Demo] Create auto reaconnect timer \r\n");
			if (false == osif_timer_create(&reconnect_timer, "a2dp_demo_reconnect_timer", NULL,
										   RTK_BT_DEMO_RECONNECT_TIMER_INTERVAL,
										   false,
										   a2dp_demo_reconnect_timer_handle)) {
				BT_LOGE("[APP Demo] Create reconnect timer fail \r\n");
				a2dp_demo_auto_reconnect = 0;
				return 1;
			} else {
				a2dp_demo_auto_reconnect_try_count = RTK_BT_DEMO_RECONNECT_COUNT;
			}
			/* Load reconnect info from file system */
			if (rt_kv_get(a2dp_demo_filesystem_key, (void *)remote_bd_addr, 6) == 6) {
				BT_LOGA("[A2DP Demo] Load a2dp demo reconnect data success \r\n");
				/* avoid another device inquiry req and paging our dut */
				rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_CONNECTABLE);
				osif_timer_start(&reconnect_timer);
			} else {
				BT_LOGE("[A2DP Demo] Fail to load a2dp reconnect data \r\n");
			}
		}
		/* bond info flush thread init */
		if (a2dp_demo_bond_info_flush) {
			if (rtk_bt_br_gap_bond_num_get(&a2dp_demo_bond_num)) {
				BT_LOGE("[A2DP Demo] Get Bond Number fail\r\n");
				goto failed;
			}
			/* Load reconnect info from file system */
			if (rt_kv_get(a2dp_demo_bond_info_key, (void *)a2dp_demo_bond_table, sizeof(a2dp_demo_bond_table)) == sizeof(a2dp_demo_bond_table)) {
				BT_LOGA("[A2DP Demo] Load a2dp demo bond info table success \r\n");
				/* dump bond info */
				a2dp_demo_bond_info_dump();
			} else {
				BT_LOGE("[A2DP Demo] Fail to load a2dp bond table \r\n");
			}
			BT_LOGA("[A2DP Demo] Create bond info flush thread \r\n");
			if (false == osif_sem_create(&bond_info_flush_task.sem, 0, 1)) {
				goto failed;
			}
			if (false == osif_msg_queue_create(&a2dp_demo_bond_info_flush_msg_q, 0x0F, sizeof(a2dp_demo_msg_t))) {
				goto failed;
			}
			bond_info_flush_task.run = 1;
			if (false == osif_task_create(&bond_info_flush_task.hdl, "a2dp_demo_bond_info_flush_thread",
										  a2dp_demo_bond_flush_thread, NULL,
										  2048, 4)) {
				osif_sem_delete(bond_info_flush_task.sem);
				goto failed;
			}
			/* Wait until event task is running */
			if (false == osif_sem_take(bond_info_flush_task.sem, BT_TIMEOUT_FOREVER)) {
				goto failed;
			}
		}
		bt_audio_demo_init_flag = 1;
	} else if (0 == enable) {
		if (!bt_audio_demo_init_flag) {
			BT_LOGE("%s No need deinit! \r\n", __func__);
			return -1;
		}
		/* auto reconnect deinit */
		if (a2dp_demo_auto_reconnect) {
			osif_timer_stop(&reconnect_timer);
			osif_timer_delete(&reconnect_timer);
			a2dp_demo_auto_reconnect_try_count = RTK_BT_DEMO_RECONNECT_COUNT;
			a2dp_demo_disconnection_reconnecting = 0;
		}
		if (a2dp_demo_bond_info_flush) {
			/* indicate a2dp demo bond info flush thread to kill itself */
			a2dp_demo_flush_bond_info(A2DP_DEMO_BOND_INFO_THREAD_EXIT, NULL);
			if (false == osif_sem_take(bond_info_flush_task.sem, BT_TIMEOUT_FOREVER)) {
				return -1;
			}
			osif_sem_delete(bond_info_flush_task.sem);
			osif_msg_queue_delete(a2dp_demo_bond_info_flush_msg_q);
			bond_info_flush_task.sem = NULL;
			bond_info_flush_task.hdl = NULL;
			a2dp_demo_bond_info_flush_msg_q = NULL;
		}
		/* a2dp source demo send task delete */
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
			rtk_bt_a2dp_demo_src_send_data_control(false);
		}
		if (g_audio_resample_t) {
			app_bt_pcm_data_resample_engine_destroy(&g_audio_resample_t);
		}
		/* stop outband ring alert */
		app_hfp_ring_alert_stop();
		rtk_bt_hfp_demo_sco_send_data_control(false);
		rtk_bt_audio_codec_remove(audio_hfp_codec_conf.codec_index, hfp_demo_codec_entity);
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_GAP));
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_SDP));
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_AVRCP));
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_A2DP));
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_HFP));
		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
		/* audio related resources release */
		call_curr_status = 0;
		rtk_bt_audio_deinit();
		a2dp_demo_audio_track_hdl = NULL;
		hfp_demo_audio_track_hdl = NULL;
		hfp_demo_audio_record_hdl = NULL;
		a2dp_demo_codec_entity = NULL;
		hfp_demo_codec_entity = NULL;
		memset((void *)&audio_a2dp_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
		memset((void *)&audio_hfp_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
		bt_audio_demo_init_flag = 0;
	}

	return 0;

failed:
	if (bond_info_flush_task.hdl) {
		osif_task_delete(bond_info_flush_task.hdl);
	}
	if (bond_info_flush_task.sem) {
		osif_sem_delete(bond_info_flush_task.sem);
	}
	if (a2dp_demo_bond_info_flush_msg_q) {
		osif_msg_queue_delete(a2dp_demo_bond_info_flush_msg_q);
	}

	return -1;
}
