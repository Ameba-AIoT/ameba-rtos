/*
*******************************************************************************
* Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <bt_api_config.h>
#include <osif.h>
#include <bt_utils.h>
#include <rtk_bt_pbp.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_bt_gap.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_le_audio.h>
#include <app_bt_le_audio_common.h>
#include <rtk_bt_def.h>
#include <rtk_bt_br_gap.h>
#include <rtk_bt_sdp.h>
#include <rtk_bt_avrcp.h>
#include <rtk_bt_tmap.h>
#include <rtk_bt_bap.h>
#include <rtk_bt_a2dp.h>
#include <bt_audio_intf.h>
#include <bt_audio_codec_wrapper.h>
#include <bt_audio_track_api.h>
#include <sbc_codec_entity.h>
#include <app_audio_data.h>
#include "kv.h"
#include <dlist.h>
#include "bt_audio_resample.h"
#include <bt_utils.h>
/* ------------------------------- Company ID ------------------------------- */
#define RTK_COMPANY_ID                                 0x005D
/* ----------------------------- Avrcp Related ------------------------------ */
#define VOL_MAX                                        0x7f
#define VOL_MIN                                        0x00
/* -------------------------------- A2DP Related Macros --------------------------------- */
#define RTK_BT_DEV_NAME                                "a2dp tmap player"
#define DEFAULT_PAGESCAN_WINDOW                        0x48
#define DEFAULT_PAGESCAN_INTERVAL                      0x800 //0x800
#define DEFAULT_PAGE_TIMEOUT                           0x2000
#define DEFAULT_INQUIRYSCAN_WINDOW                     0x48
#define DEFAULT_INQUIRYSCAN_INTERVAL                   0x800 //0x1000
#define DEFAULT_A2DP_TMAP_AUDIO_LEFT_VOLUME            0.8
#define DEFAULT_A2DP_TMAP_AUDIO_RIGHT_VOLUME           0.8
#define RTK_BT_A2DP_SRC_STREAM_MAX_CREDITS             2
/* ---------------------------- A2DP Bond Info Manage config Related Macros-------------------------- */
#define A2DP_DEMO_MAX_BOND_NUM 8
#define A2DP_DEMO_BOND_INFO_USED_VAL 0x4E
#define A2DP_DEMO_BOND_INFO_PRIORITY_UNASSIGNED 0x4F
#define A2DP_DEMO_BOND_INFO_FLUSH_EVENT 0x01
#define A2DP_DEMO_BOND_INFO_THREAD_EXIT 0xFF
/* ---------------------------- Audio resample related Macros-------------------------- */
#define RTK_BT_A2DP_TMAP_DEMO_OUPUT_SAMPLE_RATE        48000

#if (RTK_BLE_AUDIO_UNICAST_ONE_CIS_SETEO_MODE == 1) || (RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE == 1)
#define RTK_BT_A2DP_TMAP_DEMO_OUPUT_CHANNEL_NUM        2
#else
#define RTK_BT_A2DP_TMAP_DEMO_OUPUT_CHANNEL_NUM        1
#endif
/*
    A2DP sink will receive 595 bytes encode data per 14.5 ms from source, approximately 6.89 packets of 595 bytes per 100 milliseconds.
    When the sample rate is 44.1KHz, the above 595 bytes encode data will be parsing to 5 frames,
    which can be decoded to 512 bytes, 2560 bytes pcm data in total.
    In one time, we can callback 512 bytes 44.1KHZ 2 channels pcm data from rtk_bt_a2dp_decode_pcm_data_callback.

    The A2DP sink RX rate is not constant and depended on the A2DP source.But LE Aduio TX rate is constant.
    So we should leave a proper water level in pbp_convert_pcm_queue maintain average rate.
    The water level can cause extra delay, but will not run out if set proper value.
*/

// LE Audio TX water level and the unit is in milliseconds. Must set an integer multiple of 10 milliseconds.
#define LE_AUDIO_TX_WATER_LEVEL                       100
#if (RTK_BLE_AUDIO_UNICAST_ONE_CIS_SETEO_MODE == 1) || (RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE == 1)
// Actual LE Audio TX water level length
#define A2DP_TMAP_ENCODE_PCM_DATA_WATER_LEVEL         1920 * LE_AUDIO_TX_WATER_LEVEL / 10
/*
    Set enough length to store resample data.The unit is in short.
    1920 bytes is equal to 48 KHz,2 channels pcm data bytes per 10 milliseconds.
*/
#define TMAP_ENCODE_PCM_DATA_QUEUE_MAX_LEN             1920 * LE_AUDIO_TX_WATER_LEVEL / 10 * 2
#else
#define A2DP_TMAP_ENCODE_PCM_DATA_WATER_LEVEL          960 * LE_AUDIO_TX_WATER_LEVEL / 10
#define TMAP_ENCODE_PCM_DATA_QUEUE_MAX_LEN             960 * LE_AUDIO_TX_WATER_LEVEL / 10 * 2
#endif
/* Fixed length, used for temporary storage of audio data after resampled */
#define SOXR_OUT_FRAME_BUF_MAX_LEN                     480*4*4
/* ------------------------------- TMAP Related Macros ------------------------------- */
#define TMAP_ENCODE_TASK_PRIO                          4
#define TMAP_ENCODE_TASK_STACK_SIZE                    (1024 * 5)
#define A2DP_TMAP_REOPEN_TEST                          1
/* ---------------------------- Global Variables ---------------------------- */
typedef enum {
	RTK_BT_LE_AUDIO_PBP_ROLE_UNKNOWN = 0,
	RTK_BT_LE_AUDIO_A2DP_SINK_UNICAST_MEDIA_SNEDER = 0x01,           /**< a2dp sink Unicast Media Sender role. */
	RTK_BT_LE_AUDIO_A2DP_SINK_BROADCAST_MEDIA_SNEDER = 0x02,         /**< a2dp sink Broadcast Media Sender role. */
} app_bt_le_audio_a2dp_sink_tmap_role_t;

/* ---------------------------- Audio Resample Related Variables-------------------------- */
static bool a2dp_play_flag = false; // used for HW timer to judge whether to wake up LE Audio TX thread
static bool tmap_pcm_data_dequeue_flag = false;// used to indicate the tmap encode task that resample pcm data has reached sufficient number

typedef struct {
	int q_write;
	int q_read;
	void *mtx;
	short *queue;
	uint32_t queue_size;//unit: bytes
	uint32_t queue_max_len;
} a2dp_tmap_demo_queue_t;

struct a2dp_tmap_demo_task_t {
	void *hdl;
	void *sem;
	uint8_t run;
};
/* ---------------------------- TMAP Public Related Variables ---------------------------- */
static bool a2dp_tmap_demo_init_flag = false;
static uint8_t a2dp_tmap_role;
static rtk_bt_le_audio_bap_role_t bap_role;

/* ---------------------------- TMAP Unicast Media Sender Related Variables ---------------------------- */

app_bt_le_audio_initiator_info_t g_ums_info = {
	.device_name = "a2dp tmap player",
	.lea_app_conf = {0},
	.p_bap_uni_cli_info = &app_bap_uni_client_info,
	.p_bap_bro_sour_info = NULL,
	.p_bap_bro_assi_info = NULL,
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
	.p_mcp_svr_info = &app_mcp_svr_info,
#endif
	.group_handle = NULL,
	.status = RTK_BLE_AUDIO_INITIATOR_DISABLE
};

/* tmap task*/
struct app_lea_tmap_task_t {
	void *hdl;
	void *sem;
	uint8_t run;
};
static struct app_lea_tmap_task_t g_tmap_encode_task = {
	.hdl = NULL,
	.sem = NULL,
	.run = 0
};
static bool g_tmap_encode_task_enable = false;
static void *g_tmap_encode_data_sem = NULL;

#define RTK_BLE_AUDIO_USE_HW_GTIMER 1
#if defined(RTK_BLE_AUDIO_USE_HW_GTIMER) && RTK_BLE_AUDIO_USE_HW_GTIMER
#include "timer_api.h"
#define TMAP_SEND_TIMER_ID                   TIMER13
static gtimer_t g_lea_tmap_send_timer = {0};
#else
static void *g_lea_tmap_send_timer = NULL;
#endif
static uint32_t g_tmap_send_timer_interval_us = RTK_BLE_AUDIO_DEFAULT_SDU_INTERVAL_M_S_US;

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
static uint8_t app_lea_mcs_service_id = 0;
#endif

static rtk_bt_le_scan_param_t app_def_scan_param = {
	.type          = RTK_BT_LE_SCAN_TYPE_ACTIVE,
	.interval      = 0x60,
	.window        = 0x30,
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.filter_policy = RTK_BT_LE_SCAN_FILTER_ALLOW_ALL,
	.duplicate_opt = 0,
};

static uint8_t app_lea_def_metadata[] = {
	0x03,
	RTK_BT_LE_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS,
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_MEDIA),
	(uint8_t)(RTK_BT_LE_AUDIO_CONTEXT_MEDIA >> 8),
	0x2,
	RTK_BT_LE_METADATA_TYPE_CCCD_LIST,
	0
};

static uint8_t app_lea_def_metadata_len = sizeof(app_lea_def_metadata);

/* ---------------------------- TMAP Broadcast Media Sender Related Variables ---------------------------- */
app_bt_le_audio_initiator_info_t g_bms_info = {
	.device_name = "a2dp tmap player",
	.lea_app_conf = {0},
	.p_bap_uni_cli_info = NULL,
	.p_bap_bro_sour_info = &app_bap_bro_sour_info,
	.p_bap_bro_assi_info = NULL,
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
	.p_mcp_svr_info = &app_mcp_svr_info,
#endif
	.group_handle = NULL,
	.status = RTK_BLE_AUDIO_INITIATOR_DISABLE
};
/* ---------------------------- Audio Resample Related Variables-------------------------- */
/*
    1.Storage 48KHz PCM data after resample;
    2.Send it to LE audio encode task when reach a suitable water level.
*/
static short tmap_resample_queue[TMAP_ENCODE_PCM_DATA_QUEUE_MAX_LEN] = {0};

static a2dp_tmap_demo_queue_t tmap_resample_pcm_data_queue = {
	.q_write = 0,
	.q_read = 0,
	.mtx = NULL,
	.queue = NULL,
	.queue_size = 0,
	.queue_max_len = 0,
};

static rtk_bt_audio_biquad_t bq_t = {0};
static rtk_bt_audio_resample_t *g_audio_resample_t = NULL;
static uint32_t resample_out_frames, resample_in_frames = 0;
static uint32_t demo_in_rate = 44100;//input sample rate
static uint32_t demo_out_rate = RTK_BT_A2DP_TMAP_DEMO_OUPUT_SAMPLE_RATE;//output sample rate
static int8_t out_frame_buf[SOXR_OUT_FRAME_BUF_MAX_LEN] = {0};

/* ---------------------------- A2DP Bond Info Manage config -------------------------- */
/* Max bond number is unchangable */
static char a2dp_demo_bond_info_key[] = "a2dp_bond_table";
static uint8_t a2dp_demo_bond_num = 0;
static uint8_t a2dp_demo_bond_info_flush = 0;
static struct a2dp_tmap_demo_task_t bond_info_flush_task = {
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
} a2dp_demo_bond_info_t;
static a2dp_demo_bond_info_t a2dp_demo_bond_table[A2DP_DEMO_MAX_BOND_NUM] = {0};
/* ---------------------------- A2DP Related Variables-------------------------- */
typedef struct {
	uint16_t type;
	uint16_t subtype;
	union {
		uint32_t  param;
		void     *buf;
	} u;
} a2dp_demo_msg_t;

static rtk_bt_audio_track_t *a2dp_demo_audio_track_hdl = NULL;
static void *a2dp_demo_codec_entity = NULL;
static uint8_t a2dp_demo_role;
static uint8_t src_a2dp_credits = RTK_BT_A2DP_SRC_STREAM_MAX_CREDITS;
static uint8_t curr_volume = RTK_BT_DEFAULT_ABSOLUTE_VOLUME;

static rtk_bt_audio_codec_conf_t audio_codec_conf = {0};
static rtk_bt_sbc_codec_t sbc_codec_t = {
	{
		.sbc_enc_mode = (sbc_channel_mode_t)SBC_MODE_STANDARD,
		.blocks = 16,
		.subbands = 8,
		.alloc_method = SBC_ALLOCATION_METHOD_SNR,
#if defined(CONFIG_BT_AUDIO_SOURCE_OUTBAND) && CONFIG_BT_AUDIO_SOURCE_OUTBAND
		.sample_rate = 48000,
#else
		.sample_rate = 44100,
#endif
		.bitpool = 0x21,
		.channel_mode = SBC_CHANNEL_MODE_DUAL_CHANNEL,
	},
	{0},
};

static uint8_t remote_bd_addr[6] = {0};

static rtk_bt_a2dp_media_codec_sbc_t codec_sbc = {
#if defined(CONFIG_BT_AUDIO_SOURCE_OUTBAND) && CONFIG_BT_AUDIO_SOURCE_OUTBAND
	.sampling_frequency_mask = RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_48KHZ,
#else
	.sampling_frequency_mask = 0xf0,
#endif
	.channel_mode_mask = 0x0f,
	.block_length_mask = 0xf0,
	.subbands_mask = 0x0C,
	.allocation_method_mask = 0x03,
	.min_bitpool = 2,
	.max_bitpool = 53,
};
/* class of device */
static uint32_t cod = (0x18 | (0x04 << 8) | (0x24 << 16));
/* supervision_timeout */
static uint16_t superv_time = (0x1f40);

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

/********************************************* a2dp sink parameter end***************************************************/

/********************************************* audio resample related function***************************************************/

static bool a2dp_tmap_demo_dequeue_num_is_enough(a2dp_tmap_demo_queue_t *p_queue, uint32_t dequeue_size)
{
	uint32_t queue_size = 0;

	queue_size = p_queue->queue_size;

	if (queue_size < dequeue_size) {
		return false;
	}

	return true;
}

static bool a2dp_tmap_demo_enqueue_is_full(a2dp_tmap_demo_queue_t *p_queue, uint32_t enqueue_size)
{
	uint32_t queue_size = 0;
	uint32_t queue_max_len = 0;

	queue_size = p_queue->queue_size;
	queue_max_len = p_queue->queue_max_len;

	if ((queue_size + enqueue_size) > 2 * queue_max_len) {
		return true;
	}

	return false;

}

static bool a2dp_tmap_demo_queue_is_empty(a2dp_tmap_demo_queue_t *p_queue)
{
	if (p_queue->q_write == -1) {
		return true;
	}
	return false;

}

//enqueue_len: unit is bytes
static uint16_t a2dp_tmap_demo_pcm_data_enqueue(a2dp_tmap_demo_queue_t *p_queue, int8_t *in_buf, uint32_t enqueue_size)
{
	uint32_t p_write = 0;
	uint32_t p_read = 0;
	uint32_t buf_write = 0;
	uint32_t queue_max_len = 0;

	short *p_que = NULL;
	p_que = p_queue->queue;
	queue_max_len = p_queue->queue_max_len;

	if (a2dp_tmap_demo_enqueue_is_full(p_queue, enqueue_size)) {
		BT_LOGE("[APP] %s No enough space! queue size now is %d\r\n", __func__, (int)(p_queue->queue_size));
		return RTK_BT_FAIL;
	}

	//enqueue
	if (a2dp_tmap_demo_queue_is_empty(p_queue)) {
		if (enqueue_size) {
			p_queue->q_write += enqueue_size / 2;
			p_queue->q_read = 0;
			memcpy((void *)p_que, (void *)in_buf, enqueue_size);
			p_queue->queue_size += enqueue_size;
			BT_LOGD("[APP] %s queue is empty!\r\n", __func__);
			return RTK_BT_OK;
		}
	}

	p_write = (p_queue->q_write) % queue_max_len;
	p_read = (p_queue->q_read) % queue_max_len;

	if (p_write >= p_read) {
		if (p_write + enqueue_size / 2 <= queue_max_len - 1) {
			//[p_wite,enqueue_size-1] buf is enough
			memcpy(p_que + (p_write + 1) % queue_max_len, (void *)in_buf, enqueue_size);
		} else {
			//[p_write,enqueue_size-1] buf is not enough
			//[0, p_write]
			uint32_t right_size = 0;
			uint32_t left_size = 0;
			right_size = 2 * (queue_max_len - 1 - p_write);
			left_size = enqueue_size - right_size;

			memcpy(p_que + (p_write + 1) % queue_max_len, in_buf, right_size);
			buf_write += right_size;
			memcpy(p_que, in_buf + buf_write, left_size);
		}
	} else {
		memcpy(p_que + (p_write + 1) % queue_max_len, (void *)in_buf, enqueue_size);
	}
	p_queue->q_write = (p_queue->q_write + enqueue_size / 2) % queue_max_len;
	p_queue->queue_size += enqueue_size;

	BT_LOGD("[APP] %s enqueue %u success! queue_size: %u q_write: %d q_read: %d\r\n", __func__, enqueue_size, p_queue->queue_size, p_queue->q_write,
			p_queue->q_read);
	return RTK_BT_OK;
}

static uint16_t a2dp_tmap_demo_pcm_data_dequeue(a2dp_tmap_demo_queue_t *p_queue, int8_t *out_buf, uint32_t dequeue_size)
{
	uint32_t p_write = 0;
	uint32_t p_read = 0;
	uint32_t buf_write = 0;
	uint32_t queue_max_len = 0;

	short *p_que = NULL;
	p_que = p_queue->queue;
	queue_max_len = p_queue->queue_max_len;

	if (a2dp_tmap_demo_queue_is_empty(p_queue)) {
		BT_LOGE("[APP] %s dequeue failed !!! queue is empty\r\n", __func__);
		return RTK_BT_FAIL;
	}

	if (!a2dp_tmap_demo_dequeue_num_is_enough(p_queue, dequeue_size)) {
		BT_LOGD("[APP] %s dequeue num is not enough!!!\r\n", __func__);
		return RTK_BT_FAIL;
	}

	if (!out_buf) {
		BT_LOGE("[APP] %s out buf is NULL!\r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (!dequeue_size) {
		BT_LOGE("[APP] %s dequeue_size is 0!\r\n", __func__);
		return RTK_BT_FAIL;
	}

	p_write = (p_queue->q_write) % queue_max_len;
	p_read = (p_queue->q_read) % queue_max_len;

	if (p_write >= p_read) {
		memcpy((void *)out_buf, (void *)(p_que + p_read), dequeue_size);
	} else {
		//2*(queue_max_len - 1 - p_read+1) >= dequeue_size
		if (p_read + dequeue_size / 2 <= queue_max_len) {
			//buf is enough
			memcpy(out_buf, p_que + p_read, dequeue_size);
		} else {
			uint32_t right_size = 0;
			uint32_t left_size = 0;
			right_size = 2 * ((queue_max_len - 1) - p_read + 1);
			left_size = dequeue_size - right_size;
			memcpy(out_buf + buf_write, p_que + p_read, right_size);
			buf_write += right_size;
			memcpy(out_buf + buf_write, (void *)p_que, left_size);
		}
	}
	p_queue->q_read = (p_queue->q_read + dequeue_size / 2) % queue_max_len;
	p_queue->queue_size -= dequeue_size;

	BT_LOGD("[APP] %s dequeue %u success! queue_size: %u q_write: %d q_read: %d\r\n", __func__, dequeue_size, p_queue->queue_size, p_queue->q_write,
			p_queue->q_read);
	return RTK_BT_OK;

}

static uint16_t a2dp_tmap_demo_queue_pcm_data_flush(a2dp_tmap_demo_queue_t *p_queue)
{
	if (p_queue != NULL) {
		p_queue->q_write = -1;
		p_queue->q_read = -1;
		p_queue->queue_size = 0;
		BT_LOGD("[APP] %s success\r\n", __func__);
		return RTK_BT_OK;
	}
	BT_LOGE("[APP] %s failed \r\n", __func__);
	return RTK_BT_FAIL;
}

static uint16_t a2dp_tmap_demo_queue_init(a2dp_tmap_demo_queue_t *p_queue, short *queue, uint32_t queue_len)
{
	if (p_queue != NULL) {
		p_queue->q_write = -1;
		p_queue->q_read = -1;
		p_queue->queue = queue;
		p_queue->queue_size = 0;
		p_queue->queue_max_len = queue_len;
		if (p_queue->mtx == NULL) {
			osif_mutex_create(&p_queue->mtx);
		}
		BT_LOGA("[APP] %s queue init success\r\n", __func__);
		return RTK_BT_OK;
	}
	BT_LOGE("[APP] %s queue init failed\r\n", __func__);
	return RTK_BT_FAIL;
}

static uint16_t a2dp_tmap_demo_queue_deinit(a2dp_tmap_demo_queue_t *p_queue)
{
	if (p_queue != NULL) {
		p_queue->q_write = -1;
		p_queue->q_read = -1;
		p_queue->queue = NULL;
		p_queue->queue_size = 0;
		p_queue->queue_max_len = 0;
		if (p_queue->mtx) {
			osif_mutex_delete(p_queue->mtx);
			p_queue->mtx = NULL;
		}
		BT_LOGA("[APP] %s queue deinit success\r\n", __func__);
		return RTK_BT_OK;
	}
	BT_LOGE("[APP] %s: p_queue is NULL\r\n", __func__);
	return RTK_BT_FAIL;
}
static uint32_t fail_cnt;
static uint16_t rtk_bt_a2dp_decode_pcm_data_callback(void *p_pcm_data, uint16_t p_len, void *pentity, void *track)
{
	(void)pentity;
	(void)track;
	void *p_enqueue_mtx = NULL;
	uint32_t time_stamp_before, time_stamp_after = 0;
	uint32_t enqueue_size = 0;
	uint32_t out_frames = 0;

	if (((a2dp_tmap_role == RTK_BT_LE_AUDIO_A2DP_SINK_UNICAST_MEDIA_SNEDER) && g_ums_info.status != RTK_BLE_AUDIO_INITIATOR_UNICAST_START) || \
		((a2dp_tmap_role == RTK_BT_LE_AUDIO_A2DP_SINK_BROADCAST_MEDIA_SNEDER) && g_bms_info.status != RTK_BLE_AUDIO_INITIATOR_BROADCAST_START)) {
		if (0 == (fail_cnt % 200)) {
			BT_LOGE("[APP] TMAP unicast or broadcast stream has not started!\r\n");
		}
		fail_cnt++;
		return RTK_BT_FAIL;
	}
	//1.resample process
	time_stamp_before = osif_sys_time_get();
	out_frames = rtk_bt_audio_resample_entry(g_audio_resample_t, &bq_t,
											 (uint8_t *)p_pcm_data,
											 p_len / (g_audio_resample_t->in_frame_size),
											 (uint8_t *)out_frame_buf,
											 demo_in_rate,
											 demo_out_rate);
	time_stamp_after = osif_sys_time_get();
	BT_LOGD("[APP] %s: time_stamp before :%u,time_stamp after:%u,delt_time:%d\r\n", __func__, time_stamp_before, time_stamp_after,
			(int)(time_stamp_after - time_stamp_before));
	BT_LOGD("[APP] in_frames:%u out_frames:%u\r\n",  p_len / (g_audio_resample_t->in_frame_size), out_frames);
	//2.enqueue
	p_enqueue_mtx = tmap_resample_pcm_data_queue.mtx;
	enqueue_size = out_frames * g_audio_resample_t->out_frame_size;
	if (!p_enqueue_mtx) {
		BT_LOGE("[APP] %s p_enqueue_mtx is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	}
	osif_mutex_take(p_enqueue_mtx, BT_TIMEOUT_FOREVER);
	if (RTK_BT_OK != a2dp_tmap_demo_pcm_data_enqueue(&tmap_resample_pcm_data_queue, out_frame_buf, enqueue_size)) {
		BT_LOGE("[APP] %s tmap_resample_pcm_data_queue is full!\r\n", __func__);
		osif_mutex_give(p_enqueue_mtx);
		return RTK_BT_FAIL;
	}
	if (tmap_resample_pcm_data_queue.queue_size >= A2DP_TMAP_ENCODE_PCM_DATA_WATER_LEVEL) {
		tmap_pcm_data_dequeue_flag = true;
	}
	osif_mutex_give(p_enqueue_mtx);

	return RTK_BT_OK;
}

static uint16_t rtk_bt_a2dp_tmap_demo_audio_resample_alloc(rtk_bt_audio_resample_t **pp_sample_t, uint32_t sample_rate_in, uint8_t in_channels)
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
	*pp_sample_t = rtk_bt_audio_resample_alloc((float)demo_in_rate, (float)demo_out_rate, in_channels, RTK_BT_A2DP_TMAP_DEMO_OUPUT_CHANNEL_NUM, resample_in_frames);

	return RTK_BT_OK;
}

static uint16_t rtk_bt_a2dp_tmap_demo_audio_resample_destroy(rtk_bt_audio_resample_t **pp_sample_t)
{
	if (*pp_sample_t) {
		rtk_bt_audio_resample_free(*pp_sample_t);
		*pp_sample_t = NULL;
	}

	return RTK_BT_OK;
}

/********************************************* audio resample related function end***************************************************/

/********************************************* a2dp bond related function ***************************************************/
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
static uint16_t a2dp_demo_flush_bond_info(uint8_t type)
{
	a2dp_demo_msg_t io_msg;
	io_msg.type = type;

	if (false == osif_msg_send(a2dp_demo_bond_info_flush_msg_q, &io_msg, 0)) {
		return 1;
	}

	return 0;
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
		if (osif_msg_recv(a2dp_demo_bond_info_flush_msg_q, &io_msg, 0xFFFFFFFF) == true) {
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
			/* 2. get bond mac address accord to bond number */
			for (uint8_t i = 0; i < temp_bond_num; i ++) {
				/* gap bond priority start frmo 1, so i + 1 */
				ret = rtk_bt_br_gap_bond_addr_get(i + 1, bd_addr);
				if (ret) {
					BT_LOGE("[A2DP Demo] Get bond addr failed! err: 0x%x", ret);
					continue;
				}
				BT_LOGA("[A2DP Demo] Get Bond addr %02x:%02x:%02x:%02x:%02x:%02x \r\n",
						bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
				/* 3. sync with a2dp_demo_bond_table */
				pbond_info = a2dp_demo_find_bond_info_by_mac_addr(bd_addr);
				if (!pbond_info) {
					/* allocate new bond info unit */
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
					/* 4. try to fetch remote device name */
					rtk_bt_br_gap_get_remote_name(bd_addr);
				} else {
					/* update bond info unit */
					if (pbond_info->priority != (i + 1)) {
						if (A2DP_DEMO_BOND_INFO_PRIORITY_UNASSIGNED != pbond_info->priority) {
							a2dp_demo_bond_info_t *ptemp_bond_info = NULL;
							/* get previous priority bond info unit */
							ptemp_bond_info = a2dp_demo_find_bond_info_by_priority(i + 1);
							if (ptemp_bond_info) {
								/* this bond info should be mask for further configuration */
								ptemp_bond_info->priority = A2DP_DEMO_BOND_INFO_PRIORITY_UNASSIGNED;
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
			if (temp_bond_num < a2dp_demo_bond_num) {
				for (uint8_t i = 0; i < (a2dp_demo_bond_num - temp_bond_num); i ++) {
					pbond_info = a2dp_demo_find_bond_info_by_priority(temp_bond_num + i + 1);
					if (pbond_info) {
						memset((void *)pbond_info, 0, sizeof(a2dp_demo_bond_info_t));
						need_save_bond_info = 1;
					}
				}
			}
			/* 6. check whether need save a2dp_demo_bond_table */
			if (need_save_bond_info) {
				/* foreach table to check whether there is A2DP_DEMO_BOND_INFO_PRIORITY_UNASSIGNED bond unit(need to be deleted) */
				for (uint8_t i = 0; i < A2DP_DEMO_MAX_BOND_NUM; i ++) {
					if ((A2DP_DEMO_BOND_INFO_USED_VAL == a2dp_demo_bond_table[i].used_check) &&
						(A2DP_DEMO_BOND_INFO_PRIORITY_UNASSIGNED == a2dp_demo_bond_table[i].priority)) {
						memset((void *)&a2dp_demo_bond_table[i], 0, sizeof(a2dp_demo_bond_info_t));
					}
				}
				if (rt_kv_set(a2dp_demo_bond_info_key, (void *)a2dp_demo_bond_table, sizeof(a2dp_demo_bond_table)) == sizeof(a2dp_demo_bond_table)) {
					BT_LOGA("[A2DP Demo] Save a2dp demo bond info table success \r\n");
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

static void a2dp_demo_bond_info_dump(void)
{
	a2dp_demo_bond_info_t *pbond_info = NULL;

	BT_LOGA("[A2DP Demo] Bond Info List\r\n");
	for (uint8_t i = 0; i < a2dp_demo_bond_num; i ++) {
		pbond_info = a2dp_demo_find_bond_info_by_priority(i + 1);
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

/********************************************* a2dp bond related function end ***************************************************/

/********************************************* a2dp sink function ***************************************************/
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
				if (rt_kv_set(a2dp_demo_bond_info_key, (void *)a2dp_demo_bond_table, sizeof(a2dp_demo_bond_table)) == sizeof(a2dp_demo_bond_table)) {
					BT_LOGA("[A2DP Demo] Save a2dp demo bond info table success \r\n");
					a2dp_demo_bond_info_dump();
				} else {
					BT_LOGE("[A2DP Demo] Fail to save a2dp demo bond info table \r\n");
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
		BT_LOGA("[BR GAP] ACL connection success %02x:%02x:%02x:%02x:%02x:%02x \r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		break;
	}

	case RTK_BT_BR_GAP_LINK_KEY_REQ: {
		uint8_t found = *(uint8_t *)param;
		if (a2dp_demo_bond_info_flush && found) {
			a2dp_demo_flush_bond_info(A2DP_DEMO_BOND_INFO_FLUSH_EVENT);
		}
		BT_LOGA("[BR GAP] Link Key Request received and found is 0x%x \r\n", found);
		break;
	}

	case RTK_BT_BR_GAP_LINK_KEY_INFO: {
		rtk_bt_br_bond_key_t *pbond_key_t = (rtk_bt_br_bond_key_t *)param;
		uint8_t *bd_addr = pbond_key_t->bd_addr;
		if (a2dp_demo_bond_info_flush) {
			a2dp_demo_flush_bond_info(A2DP_DEMO_BOND_INFO_FLUSH_EVENT);
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

		if (a2dp_demo_bond_info_flush) {
			a2dp_demo_flush_bond_info(A2DP_DEMO_BOND_INFO_FLUSH_EVENT);
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
	a2dp_demo_audio_track_hdl = rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_SBC,
													   (float)DEFAULT_A2DP_TMAP_AUDIO_LEFT_VOLUME,
													   (float)DEFAULT_A2DP_TMAP_AUDIO_RIGHT_VOLUME,
													   channels,
													   psbc_decoder_t->sampling_frequency,
													   BT_AUDIO_FORMAT_PCM_16_BIT,
													   0,
													   (pcm_data_cb)rtk_bt_a2dp_decode_pcm_data_callback, false);
	if (!a2dp_demo_audio_track_hdl) {
		BT_LOGE("[A2DP] bt audio track add fail \r\n");
		return 1;
	}

	return 0;
}
static void app_bt_le_audio_tmap_encode_data_control(bool enable);
static rtk_bt_evt_cb_ret_t app_bt_a2dp_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	uint8_t bd_addr[6];
	void *p_enqueue_mtx;
	uint32_t dequeue_size;

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
		rtk_bt_audio_codec_remove(audio_codec_conf.codec_index, a2dp_demo_codec_entity);
		rtk_bt_audio_track_del(audio_codec_conf.codec_index, a2dp_demo_audio_track_hdl);
		memset((void *)&audio_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
		if (g_audio_resample_t) {
			rtk_bt_a2dp_tmap_demo_audio_resample_destroy(&g_audio_resample_t);
		}
		p_enqueue_mtx = tmap_resample_pcm_data_queue.mtx;
		osif_mutex_take(p_enqueue_mtx, BT_TIMEOUT_FOREVER);
		dequeue_size = tmap_resample_pcm_data_queue.queue_size;
		if (dequeue_size) {
			a2dp_tmap_demo_queue_pcm_data_flush(&tmap_resample_pcm_data_queue);
			tmap_pcm_data_dequeue_flag = false;
		}
		osif_mutex_give(p_enqueue_mtx);
		// rtk_bt_audio_deinit();
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
		if ((pa2dp_codec->codec_type & RTK_BT_AUDIO_CODEC_SBC/* | RTK_BT_AUDIO_CODEC_AAC */) == 0) {
			BT_LOGE("[A2DP] Not support codec %d \r\n", pa2dp_codec->codec_type);
			break;
		}
		if (rtk_bt_audio_init()) {
			BT_LOGE("[A2DP] rtk_bt_audio_init fail \r\n");
			break;
		}
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SNK) {
			if (pa2dp_codec->codec_type == RTK_BT_AUDIO_CODEC_SBC) {
				// add audio track
				ret = rtk_bt_a2dp_sbc_parse_decoder_struct(pa2dp_codec, &sbc_codec_t.decoder_t);
				audio_codec_conf.codec_index = RTK_BT_AUDIO_CODEC_SBC;
				audio_codec_conf.param = (void *)&sbc_codec_t;
				audio_codec_conf.param_len = sizeof(sbc_codec_t);
			} /* else if (pa2dp_codec->codec_type == RTK_BT_AUDIO_CODEC_AAC) {
ret = rtk_bt_a2dp_aac_parse_decoder_struct(pa2dp_codec, &aac_codec_t.decoder_t);
audio_codec_conf.codec_index = RTK_BT_AUDIO_CODEC_AAC;
audio_codec_conf.param = (void *)&aac_codec_t;
audio_codec_conf.param_len = sizeof(aac_codec_t);
} */
			if (ret) {
				BT_LOGE("[A2DP] RTK_BT_A2DP_EVT_CONFIG_CMPL Fail \r\n");
				rtk_bt_audio_deinit();
				a2dp_demo_audio_track_hdl = NULL;
				a2dp_demo_codec_entity = NULL;
				break;
			}
		}
		a2dp_demo_codec_entity = rtk_bt_audio_codec_add(&audio_codec_conf);
		// bt_a2dp_sink_event_handle(A2DP_SINK_STREAM_DATA_HANDLE_INIT, A2DP_SBC_CODEC, 0, NULL);
		/* pcm convert engine init */
		rtk_bt_a2dp_tmap_demo_audio_resample_alloc(&g_audio_resample_t, sbc_codec_t.decoder_t.sampling_frequency, ((sbc_codec_t.decoder_t.channel_mode != 0) ? 2 : 1));
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
		if (a2dp_demo_audio_track_hdl) {
			rtk_bt_audio_track_resume(a2dp_demo_audio_track_hdl->audio_track_hdl);
		}
#if defined(A2DP_TMAP_REOPEN_TEST) && A2DP_TMAP_REOPEN_TEST
		if (a2dp_tmap_role == RTK_BT_LE_AUDIO_A2DP_SINK_UNICAST_MEDIA_SNEDER) {
			if (g_ums_info.status == RTK_BLE_AUDIO_INITIATOR_UNICAST_STOP) {
				//flush pcm data
				p_enqueue_mtx = tmap_resample_pcm_data_queue.mtx;
				osif_mutex_take(p_enqueue_mtx, BT_TIMEOUT_FOREVER);
				dequeue_size = tmap_resample_pcm_data_queue.queue_size;
				if (dequeue_size) {
					a2dp_tmap_demo_queue_pcm_data_flush(&tmap_resample_pcm_data_queue);
					tmap_pcm_data_dequeue_flag = false;
				}
				osif_mutex_give(p_enqueue_mtx);
				//restart iso data path
				rtk_bt_bap_unicast_client_start(0, 1);
			}
		}
#endif
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
		if (a2dp_demo_audio_track_hdl) {
			rtk_bt_audio_track_pause(a2dp_demo_audio_track_hdl->audio_track_hdl);
		}
#if defined(A2DP_TMAP_REOPEN_TEST) && A2DP_TMAP_REOPEN_TEST
		if (a2dp_tmap_role == RTK_BT_LE_AUDIO_A2DP_SINK_UNICAST_MEDIA_SNEDER) {
			if (g_ums_info.status == RTK_BLE_AUDIO_INITIATOR_UNICAST_START) {
				// delete timer handle and task handle
				//app_bt_le_audio_tmap_encode_data_control(false);
				// stop unicast session
				rtk_bt_bap_unicast_client_stop(0);
				// release unicast session
				rtk_bt_bap_unicast_client_release(0);
			}
		}
#endif
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_CLOSE: {
		rtk_bt_a2dp_conn_ind_t *conn_ind = (rtk_bt_a2dp_conn_ind_t *)param;
		a2dp_play_flag = false;
		memcpy((void *)bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[A2DP] Stream close from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		//BT_LOGA("%s: free heap is %d \r\n", __func__, osif_mem_peek(RAM_TYPE_DATA_ON));
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_DATA_IND: { //BT api shall not be called here
		rtk_bt_a2dp_stream_data_ind_t *pdata_in = (rtk_bt_a2dp_stream_data_ind_t *)param;
		a2dp_play_flag = true;
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
/********************************************* a2dp sink function end*************************************************/

/********************************************* tmap unicast media sender function *****************************************/
static void app_bt_le_audio_tmap_send_timer_handler(void *arg)
{
	(void)arg;
	uint8_t i = 0, tx_iso_data_path_num = 0;
	uint32_t sample_rate = 0;
	uint32_t frame_duration_us = 0;
	uint16_t frame_num = 0;
	tx_iso_data_path_num = app_bt_le_audio_iso_data_path_get_num(RTK_BLE_AUDIO_ISO_DATA_PATH_TX);
	app_lea_iso_data_path_t *p_iso_path = NULL;
	if (g_tmap_encode_task.run) {
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
		if (tmap_pcm_data_dequeue_flag && a2dp_play_flag) {
			if (g_tmap_encode_data_sem) {
				osif_sem_give(g_tmap_encode_data_sem);
			}
		}
	}
}

static void app_bt_le_audio_tmap_send_timer_update(uint32_t time_interval_us)
{
	BT_LOGA("[APP] %s: time_interval_us=%d\r\n", __func__, (int)time_interval_us);
	g_tmap_send_timer_interval_us = time_interval_us;
#if defined(RTK_BLE_AUDIO_USE_HW_GTIMER) && RTK_BLE_AUDIO_USE_HW_GTIMER
	if (g_lea_tmap_send_timer.handler) {
		gtimer_reload(&g_lea_tmap_send_timer, time_interval_us);
	} else {
		BT_LOGD("[APP] %s: timer not init\r\n", __func__);
	}
#else
	if (g_lea_tmap_send_timer) {
		if (false == osif_timer_stop(&g_lea_tmap_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_stop fail \r\n", __func__);
		}
		if (false == osif_timer_restart(&g_lea_tmap_send_timer, time_interval_us / 1000)) {
			BT_LOGE("[APP] %s osif_timer_start fail\r\n", __func__);
		}
	}
#endif
}

static void app_bt_le_audio_tmap_send_timer_init(void)
{
#if defined(RTK_BLE_AUDIO_USE_HW_GTIMER) && RTK_BLE_AUDIO_USE_HW_GTIMER
	BT_LOGD("[APP] %s hw timer id %d,time_interval_us = %d\r\n", __func__, TMAP_SEND_TIMER_ID, (int)g_tmap_send_timer_interval_us);
	if (g_lea_tmap_send_timer.handler == NULL) {
		gtimer_init(&g_lea_tmap_send_timer, TMAP_SEND_TIMER_ID);
		gtimer_start_periodical(&g_lea_tmap_send_timer, g_tmap_send_timer_interval_us, (void *)app_bt_le_audio_tmap_send_timer_handler, NULL);
	}
#else
	BT_LOGD("[APP] %s sw time_interval_us = %d\r\n", __func__, (int)g_tmap_send_timer_interval_us);
	if (g_tmap_send_timer_interval_us % 1000 != 0) {
		BT_LOGE("[APP] %s error: interval %d(us) cannot use sw timer, please use hw timer instead\r\n", __func__, (int)g_tmap_send_timer_interval_us);
		return;
	}
	if (!g_lea_tmap_send_timer) {
		if (false == osif_timer_create(&g_lea_tmap_send_timer,
									   "le_audio_tmap_send_timer",
									   1,
									   g_tmap_send_timer_interval_us / 1000,
									   true,
									   app_bt_le_audio_tmap_send_timer_handler)) {
			BT_LOGE("[APP] %s osif_timer_create fail\r\n", __func__);
			return;
		}
	}
	if (false == osif_timer_start(&g_lea_tmap_send_timer)) {
		BT_LOGE("[APP] %s osif_timer_start fail\r\n", __func__);
		return;
	}
#endif
}

static void app_bt_le_audio_tmap_send_timer_deinit(void)
{
#if defined(RTK_BLE_AUDIO_USE_HW_GTIMER) && RTK_BLE_AUDIO_USE_HW_GTIMER
	if (g_lea_tmap_send_timer.handler) {
		gtimer_stop(&g_lea_tmap_send_timer);
		gtimer_deinit(&g_lea_tmap_send_timer);
		g_lea_tmap_send_timer.handler = NULL;
	}
#else
	if (g_lea_tmap_send_timer) {
		if (false == osif_timer_stop(&g_lea_tmap_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_stop fail \r\n", __func__);
		}
		if (false == osif_timer_delete(&g_lea_tmap_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_delete fail \r\n", __func__);
		}
		g_lea_tmap_send_timer = NULL;
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
		p_iso_path->status_ok_cnt ++;
	} else {
		BT_LOGE("[APP] %s failed, iso_conn_handle 0x%x, seq_num %d, ret 0x%x\r\n", __func__, p_iso_path->iso_conn_handle, p_iso_path->pkt_seq_num, ret);
		BT_DUMPD("", p_data, data_len);
		p_iso_path->status_fail_cnt++;
	}

	app_bt_le_audio_iso_data_tx_statistics(p_iso_path);

	return ret;
}

static uint16_t app_bt_le_audio_encode_resample_data(app_lea_iso_data_path_t *p_iso_path, uint8_t out_channels)
{
	uint32_t time_stamp_before, queue_size = 0;;
	uint8_t bytes_per_num = (DEFAULT_PCM_BIT_WIDTH / 8);
	uint32_t encode_byte = 0, pcm_frame_size = 0;
	uint32_t sample_rate = 0, frame_duration_us = 0;
	void *pmtx = NULL;
	rtk_bt_le_audio_cfg_codec_t *p_codec = NULL;
	struct enc_codec_buffer *penc_codec_buffer_t = NULL;
	uint16_t ret = 0;

	if (!p_iso_path) {
		BT_LOGE("[APP] %s p_iso_path is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	} else {
		p_iso_path->is_processing = true;
		p_iso_path->iso_data_tx_queue_num++;
		p_codec = &p_iso_path->codec;
	}
	if (!p_codec) {
		BT_LOGE("[APP] %s p_codec is NULL\r\n", __func__);
		goto exit;
	}
	sample_rate = app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(p_codec->sample_frequency); // default 48K
	frame_duration_us = (p_codec->frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500;
	pcm_frame_size = sample_rate * frame_duration_us / 1000 / 1000;
	encode_byte = pcm_frame_size * out_channels * bytes_per_num;

	if (p_iso_path->p_encode_data == NULL) {
		p_iso_path->p_encode_data = (short *)osif_mem_alloc(RAM_TYPE_DATA_ON, encode_byte);
		if (p_iso_path->p_encode_data == NULL) {
			BT_LOGE("[APP] %s p_iso_path->p_encode_data alloc fail\r\n", __func__);
			goto exit;
		}
		memset(p_iso_path->p_encode_data, 0, encode_byte);
	}
	// dequeue
	pmtx = tmap_resample_pcm_data_queue.mtx;
	if (pmtx == NULL) {
		BT_LOGE("[APP] %s:pmtx is NULL\r\n", __func__);
	}
	time_stamp_before = osif_sys_time_get();
	osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
	ret = a2dp_tmap_demo_pcm_data_dequeue(&tmap_resample_pcm_data_queue, (int8_t *)p_iso_path->p_encode_data, encode_byte);
	queue_size = tmap_resample_pcm_data_queue.queue_size;
	osif_mutex_give(pmtx);
	BT_LOGD("[APP] %s: time_stamp before: %u, queue_size: %u\r\n", __func__, time_stamp_before, queue_size);
	if (RTK_BT_OK != ret) {
		BT_LOGE("[APP] %s: dequeue num %d is not enough set buf 0!\r\n", __func__, (int)encode_byte);
		memset(p_iso_path->p_encode_data, 0, encode_byte);
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

static void app_bt_le_audio_tmap_encode_task_entry(void *ctx)
{
	(void)ctx;
	app_lea_iso_data_path_t *p_iso_path = NULL;
	uint8_t i = 0, tx_iso_data_path_num = 0;

	g_tmap_encode_task.run = 1;
	//give sem each 10ms in
	while (g_tmap_encode_task.run) {
		if (g_tmap_encode_data_sem) {
			osif_sem_take(g_tmap_encode_data_sem, BT_TIMEOUT_FOREVER);
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
			{
				uint8_t output_channels = 0;
				/* get encoding lc3 data channels */
				if (app_bt_le_audio_get_lea_chnl_num(p_iso_path->codec.audio_channel_allocation) > 1) {
					output_channels = 2;
				} else {
					output_channels = 1;
				}
				if (RTK_BT_OK != app_bt_le_audio_encode_resample_data(p_iso_path, output_channels)) {
					BT_LOGE("[APP] %s app_bt_le_audio_send_a2dp_data fail\r\n", __func__);
					continue;
				}
			}
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
				rtk_bt_audio_free_encode_buffer(RTK_BT_AUDIO_CODEC_LC3, p_iso_path->codec_entity, p_iso_path->p_enc_codec_buffer_t);
				p_iso_path->p_enc_codec_buffer_t = NULL;
				p_iso_path->iso_data_tx_queue_num --;
				p_iso_path->is_processing = false;
			}
		}
	}
	BT_LOGA("[APP] %s task_delete\r\n", __func__);
	osif_sem_give(g_tmap_encode_task.sem);
	g_tmap_encode_task.run = 0;
	g_tmap_encode_task.hdl = NULL;
	osif_task_delete(NULL);
}

static void app_bt_le_audio_tmap_encode_data_control(bool enable)
{
	if (enable) {
		if (g_tmap_encode_task_enable == true) {
			BT_LOGE("[APP] %s: encode task is alreay enabled\r\n", __func__);
			return ;
		}

		g_tmap_encode_task_enable = true;

		if (g_tmap_encode_data_sem == NULL) {
			osif_sem_create(&g_tmap_encode_data_sem, 0, 0XFFFFFFFF);
		}

		if (g_tmap_encode_task.sem == NULL) {
			osif_sem_create(&g_tmap_encode_task.sem, 0, 1);
		}

		if (g_tmap_encode_task.hdl == NULL) {
			if (false == osif_task_create(&g_tmap_encode_task.hdl, ((const char *)"tmap_encode_task"), app_bt_le_audio_tmap_encode_task_entry,
										  NULL, TMAP_ENCODE_TASK_STACK_SIZE, TMAP_ENCODE_TASK_PRIO)) {
				BT_LOGE("[APP] %s xTaskCreate(lea_encode_task) failed\r\n", __func__);
				return ;
			}
		}
		app_bt_le_audio_tmap_send_timer_init();
	} else {
		if (g_tmap_encode_task_enable == false) {
			BT_LOGE("[APP] %s: encode task is alreay disabled\r\n", __func__);
			return ;
		}
		g_tmap_encode_task_enable = false;
		app_bt_le_audio_tmap_send_timer_deinit();
		if (g_tmap_encode_task.hdl) {
			g_tmap_encode_task.run = 0;
			osif_sem_give(g_tmap_encode_data_sem);
			osif_sem_take(g_tmap_encode_task.sem, BT_TIMEOUT_FOREVER);
			osif_sem_delete(g_tmap_encode_task.sem);
			g_tmap_encode_task.hdl = NULL;
			g_tmap_encode_task.sem = NULL;
		}
		if (g_tmap_encode_data_sem) {
			osif_sem_delete(g_tmap_encode_data_sem);
			g_tmap_encode_data_sem = NULL;
		}
	}
}

static uint16_t app_bt_le_audio_tmap_unicast_media_sender_init(app_bt_le_audio_initiator_info_t *p_ums_info)
{
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
	rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &p_ums_info->lea_app_conf;
	app_bt_le_audio_mcp_server_info_t *p_mcp_srv_info = p_ums_info->p_mcp_svr_info;

	if (p_lea_app_conf->cap_param.mcs_param.p_mcs_service_id) {
		p_mcp_srv_info->mcs_service_id = *p_lea_app_conf->cap_param.mcs_param.p_mcs_service_id;
		BT_LOGA("[APP] mcs_service_id = %d\r\n", p_mcp_srv_info->mcs_service_id);
	}

	p_mcp_srv_info->media_player_name.p_media_player_name = (uint8_t *)RTK_BLE_AUDIO_MCS_MEDIA_PLAYER_NAME;
	p_mcp_srv_info->media_player_name.media_player_name_len = sizeof(RTK_BLE_AUDIO_MCS_MEDIA_PLAYER_NAME);
	p_mcp_srv_info->track_title.p_track_title = (uint8_t *)RTK_BLE_AUDIO_MCS_TRACK_TITLE;
	p_mcp_srv_info->track_title.track_title_len = sizeof(RTK_BLE_AUDIO_MCS_TRACK_TITLE);
	p_mcp_srv_info->track_duration = RTK_BT_LE_AUDIO_MCS_TRACK_DURATION_CHAR_VALUE_UNKNOWN;
	p_mcp_srv_info->track_position = 0;//0: starting position , UNAVAILABLE: RTK_BT_LE_AUDIO_MCS_TRACK_POSITION_CHAR_VALUE_UNAVAILABLE.
	p_mcp_srv_info->playback_speed = 0;
	p_mcp_srv_info->seeking_speed = 0;
	p_mcp_srv_info->playing_order = RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_SINGLE_ONCE;
	p_mcp_srv_info->content_control_id = 0;
#else
	(void) p_ums_info;
#endif
	/* intialize iso data path */
	app_bt_le_audio_iso_data_path_init();
	/* intialize group list */
	app_bt_le_audio_group_list_init();
	app_bt_le_audio_scan_dev_list_init();

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_tmap_unicast_media_sender_deinit(app_bt_le_audio_initiator_info_t *p_ums_info)
{
	(void)p_ums_info;
	/* deintialize group list */
	app_bt_le_audio_scan_dev_list_deinit();
	app_bt_le_audio_group_list_deinit();
	/* deintialize iso data path */
	app_bt_le_audio_iso_data_path_deinit();

	return RTK_BT_OK;
}
/********************************************* tmap unicast media sender function end*****************************************/

/********************************************* tmap broadcast media sender function ******************************************/
static uint16_t app_bt_le_audio_tmap_broadcast_media_sender_setup_data_path(app_bt_le_audio_bap_broadcast_source_info_t *p_bsrc_info)
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

static uint16_t app_bt_le_audio_tmap_broadcast_media_sender_init(void)
{
	/* intialize iso data path */
	app_bt_le_audio_iso_data_path_init();

	/* intialize group list */
	app_bt_le_audio_group_list_init();
	app_bt_le_audio_scan_dev_list_init();

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_tmap_broadcast_media_sender_deinit(void)
{
	/* deintialize group list */
	app_bt_le_audio_scan_dev_list_deinit();
	app_bt_le_audio_group_list_deinit();

	/* deintialize iso data path */
	app_bt_le_audio_iso_data_path_deinit();

	return RTK_BT_OK;
}
/********************************************* tmap broadcast media sender function end ******************************************/
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
		BT_LOGE("[APP] Unkown common gap cb evt type: %d\r\n", evt_code);
		break;
	}

	return ret;
}

static rtk_bt_evt_cb_ret_t app_le_audio_gap_callback(uint8_t evt_code, void *param, uint32_t len)
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
		if (RTK_BT_LE_AUDIO_A2DP_SINK_UNICAST_MEDIA_SNEDER == a2dp_tmap_role || \
			RTK_BT_LE_AUDIO_A2DP_SINK_BROADCAST_MEDIA_SNEDER == a2dp_tmap_role) {
			app_bt_le_audio_scan_report_handle(scan_res_ind);
		}

		break;
	}
#endif

	case RTK_BT_LE_GAP_EVT_SCAN_STOP_IND: {
		rtk_bt_le_scan_stop_ind_t *scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)param;
		if (!scan_stop_ind->err) {
			BT_LOGA("[APP] Scan stopped, reason: 0x%x\r\n", scan_stop_ind->stop_reason);
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
			if (a2dp_tmap_role == RTK_BT_LE_AUDIO_A2DP_SINK_UNICAST_MEDIA_SNEDER) {
				if (g_ums_info.status != RTK_BLE_AUDIO_INITIATOR_UNICAST_START) {
					g_ums_info.status = RTK_BLE_AUDIO_INITIATOR_CONNECT;
				}
				uint16_t ret = RTK_BT_OK;
				if ((ret = rtk_bt_le_sm_start_security(conn_ind->conn_handle)) != RTK_BT_OK) {
					BT_LOGE("[APP] GAP start security flow failed! err: 0x%x", ret);
					break;
				}
			} else if (a2dp_tmap_role == RTK_BT_LE_AUDIO_A2DP_SINK_BROADCAST_MEDIA_SNEDER) {
				if (g_bms_info.status != RTK_BLE_AUDIO_INITIATOR_BROADCAST_START) {
					g_bms_info.status = RTK_BLE_AUDIO_INITIATOR_CONNECT;
				}
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
		if (a2dp_tmap_role == RTK_BT_LE_AUDIO_A2DP_SINK_UNICAST_MEDIA_SNEDER) {
			app_bt_le_audio_device_list_remove(disconn_ind->conn_handle);
			g_ums_info.status = RTK_BLE_AUDIO_INITIATOR_DISCONNECT;
		} else if (a2dp_tmap_role == RTK_BT_LE_AUDIO_A2DP_SINK_BROADCAST_MEDIA_SNEDER) {
			app_bt_le_audio_device_list_remove(disconn_ind->conn_handle);
			g_bms_info.status = RTK_BLE_AUDIO_INITIATOR_DISCONNECT;
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
			if (RTK_BT_LE_AUDIO_A2DP_SINK_UNICAST_MEDIA_SNEDER == a2dp_tmap_role || \
				RTK_BT_LE_AUDIO_A2DP_SINK_BROADCAST_MEDIA_SNEDER == a2dp_tmap_role) {
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

static rtk_bt_evt_cb_ret_t app_bt_le_audio_gattc_callback(uint8_t event, void *data, uint32_t len)
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


static rtk_bt_evt_cb_ret_t app_bt_le_audio_callback(uint8_t evt_code, void *data, uint32_t len)
{
	uint16_t ret = RTK_BT_OK;
	(void)len;

	app_bt_le_audio_group_info_t *p_group_info = NULL;
	app_bt_le_audio_device_info_t *p_device_info = NULL;
	app_bt_le_audio_bap_broadcast_source_info_t *p_bsrc_info = g_bms_info.p_bap_bro_sour_info;

	switch (evt_code) {
	/********************************************* tmap unicast media sender event *********************************************************/
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

	case RTK_BT_LE_AUDIO_EVT_BAP_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_bap_discovery_done_ind_t *param = (rtk_bt_le_audio_bap_discovery_done_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BAP_DISCOVERY_DONE_IND(conn_handle %d, pacs_is_found %d,sink_pac_num %d, source_pac_num %d\r\n",
				param->conn_handle, param->pacs_is_found, param->sink_pac_num, param->source_pac_num);
		BT_LOGA("[APP] ascs_is_found 0x%x, sink_ase_num %d, source_ase_num %d, bass_is_found %d, brs_char_num %d)\r\n",
				param->ascs_is_found, param->sink_ase_num, param->source_ase_num, param->bass_is_found, param->brs_char_num);
		//check wheather it support le audio bass
		if (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI) {
			if (param->bass_is_found == 0) {
				BT_LOGE("[APP] conn_handle(%d) not support bass\r\n", param->conn_handle);
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
			if (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI) {
				if (param->brs_char_num) {
					app_bt_le_audio_bass_brs_char_init(p_device_info, param->brs_char_num);
				}
			}
			if (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI) {
				app_bt_le_audio_new_device_add_in_group(param->conn_handle, &g_ums_info.group_handle);
			}
			if (bap_role & RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI) {
				app_bt_le_audio_new_device_add_in_group(param->conn_handle, &g_bms_info.group_handle);
			}
		}
		BT_LOGA("[APP] TX power report: conn_handle %d, type %d, txpower %d\r\n", param->conn_handle);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BAP_STATE_IND: {
		rtk_bt_le_audio_bap_state_ind_t *param = (rtk_bt_le_audio_bap_state_ind_t *) data;
		BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_BAP_STATE_IND(cause 0x%x, group_handle %08x,stream_session_handle %08x, curr_action %d, stream_state = %d, result= 0x%x)\r\n",
				param->cause, param->group_handle, param->stream_session_handle, param->curr_action, param->stream_state, param->result);
		p_group_info = app_bt_le_audio_group_list_find(param->group_handle);
		if (p_group_info == NULL) {
			BT_LOGE("[APP] %s: p_group_info is NULL\r\n", __func__);
			break;
		}
		p_group_info->bap_state = param->stream_state;
		if (param->stream_state == RTK_BT_LE_AUDIO_STREAM_STATE_STREAMING || param->stream_state == RTK_BT_LE_AUDIO_STREAM_STATE_PARTIAL_STREAMING) {
			app_bt_le_audio_tmap_encode_data_control(true);
			if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION) {
				//init rx thread
				//app_bt_le_audio_tmap_decode_data_control(true);
			}
			g_ums_info.status = RTK_BLE_AUDIO_INITIATOR_UNICAST_START;
		} else if (param->stream_state == RTK_BT_LE_AUDIO_STREAM_STATE_STOPPING) {
			app_bt_le_audio_tmap_encode_data_control(false);
			if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION) {
				//deinit rx thread
				//app_bt_le_audio_tmap_decode_data_control(false);
			}
			g_ums_info.status = RTK_BLE_AUDIO_INITIATOR_UNICAST_STOP;

		} else if (param->stream_state == RTK_BT_LE_AUDIO_STREAM_STATE_IDLE) {
			BT_LOGD("[APP] %s: RTK_BT_LE_AUDIO_STREAM_STATE_IDLE \r\n", __func__);
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BAP_START_QOS_CFG_IND: {
		rtk_bt_le_audio_bap_start_qos_cfg_ind_t *param = (rtk_bt_le_audio_bap_start_qos_cfg_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BAP_START_QOS_CFG_IND(group handle %08x, session handle %08x, sink presentation delay(%u-%u),source presentation delay(%u-%u), sink latency(%u), source latency(%u),\r\n",
				param->group_handle, param->stream_session_handle,
				(unsigned int)param->sink_presentation_delay_min, (unsigned int)param->sink_presentation_delay_max,
				(unsigned int)param->source_presentation_delay_min, (unsigned int)param->source_presentation_delay_max,
				(unsigned int)param->sink_transport_latency_max, (unsigned int)param->source_transport_latency_max);
		BT_LOGD("[APP] sink_preferred_presentation_delay(%u-%u), source_preferred_presentation_delay(%u-%u)\r\n",
				(unsigned int)param->sink_preferred_presentation_delay_min, (unsigned int)param->sink_preferred_presentation_delay_max,
				(unsigned int)param->source_preferred_presentation_delay_min, (unsigned int)param->source_preferred_presentation_delay_max);
		//config session_qos when received RTK_BT_LE_AUDIO_EVT_BAP_START_QOS_CFG_IND, change the config if need
		p_group_info = app_bt_le_audio_group_list_find(param->group_handle);
		if (p_group_info) {
			memcpy(param->p_session_qos, &p_group_info->session_qos, sizeof(rtk_bt_le_audio_unicast_session_qos_t));
			memcpy(param->p_snk_ase_qos, &bap_def_ase_qos, sizeof(rtk_bt_le_audio_unicast_ase_qos_t));
		}
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_BAP_START_METADATA_CFG_IND: {
		rtk_bt_le_audio_bap_start_metadata_cfg_ind_t *param = (rtk_bt_le_audio_bap_start_metadata_cfg_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BAP_START_METADATA_CFG_IND(group handle %08x, session handle %08x, dev_handle %08x, ase_id %d)\r\n",
				param->group_handle, param->stream_session_handle, param->device_handle, param->ase_id);
		*param->p_metadata_len = app_lea_def_metadata_len;
		memcpy(param->p_metadata, app_lea_def_metadata, app_lea_def_metadata_len);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_BAP_SETUP_DATA_PATH_IND: {
		rtk_bt_le_audio_bap_setup_data_path_ind_t *param = (rtk_bt_le_audio_bap_setup_data_path_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_BAP_SETUP_DATA_PATH_IND(group handle %08x, session handle %08x, dev_handle %08x, ase_id %d,path_direction=%d,cis_conn_handle=0x%x,\r\n",
				param->group_handle, param->stream_session_handle, param->device_handle, param->ase_id, param->path_direction, param->cis_conn_handle);
		BT_AT_PRINT("+BLEBAP:unicast,client,start,%p,%p,%p,%d,%d,0x%x\r\n",
					param->group_handle, param->stream_session_handle, param->device_handle,
					param->ase_id, param->path_direction, param->cis_conn_handle);
		BT_LOGD("[APP] type_exist 0x%x, frame_duration %d, sample_frequency %d, codec_frame_blocks_per_sdu %d,octets_per_codec_frame=%d,audio_channel_allocation=0x%x)\r\n",
				param->codec_parsed_data.type_exist, param->codec_parsed_data.frame_duration, param->codec_parsed_data.sample_frequency,
				param->codec_parsed_data.codec_frame_blocks_per_sdu, param->codec_parsed_data.octets_per_codec_frame,
				(unsigned int)param->codec_parsed_data.audio_channel_allocation);

		//add the iso data path when data path setup
		if (!app_bt_le_audio_iso_data_path_add(RTK_BT_LE_AUDIO_CIS_MODE, 0, param->path_direction, param->cis_conn_handle, &param->codec_parsed_data)) {
			if (param->path_direction == RTK_BLE_AUDIO_ISO_DATA_PATH_TX) {
				app_bt_le_audio_tmap_send_timer_update((param->codec_parsed_data.frame_duration == RTK_BT_LE_FRAME_DURATION_CFG_10_MS) ? 10000 : 7500);
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
		//remove the iso conn handle when tx data path remove
		app_bt_le_audio_iso_data_path_remove(param->cis_conn_handle, param->path_direction);
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_GROUP_DEV_MSG_IND: {
		rtk_bt_le_audio_group_dev_msg_ind_t *param = (rtk_bt_le_audio_group_dev_msg_ind_t *)data;
		BT_LOGD("[APP] RTK_BT_LE_AUDIO_EVT_GROUP_DEV_MSG_IND:msg_type 0x%x, cause 0x%x, group_handle %08x, dev_handle %08x\r\n",
				param->msg_type, param->cause, param->group_handle, param->device_handle);
		p_group_info = app_bt_le_audio_group_list_find(param->group_handle);
		if (p_group_info == NULL) {
			BT_LOGE("[APP] %s: p_group_info is NULL\r\n", __func__);
			break;
		}

		switch (param->msg_type) {
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_CONN: {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_CONN\r\n");
			break;
		}
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_DISCONN: {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_DISCONN\r\n");
			app_bt_le_audio_group_list_remove_dev(param->group_handle, param->device_handle);
			rtk_bt_le_audio_group_handle_t *p_group_handle;
			if (a2dp_tmap_role == RTK_BT_LE_AUDIO_A2DP_SINK_UNICAST_MEDIA_SNEDER) {
				p_group_handle = &g_ums_info.group_handle;
			} else if (a2dp_tmap_role == RTK_BT_LE_AUDIO_A2DP_SINK_BROADCAST_MEDIA_SNEDER) {
				p_group_handle = &g_bms_info.group_handle;
			}
			if (p_group_info->dev_num == 0) {
				// release stream session when group released
				if (p_group_info->stream_session_handle) {
					rtk_bt_le_audio_stream_session_release(p_group_info->stream_session_handle);
					BT_LOGA("%s: stream_session_handle:0x%x released\r\n", __func__, p_group_info->stream_session_handle);
					p_group_info->stream_session_handle = NULL;
				} else {
					BT_LOGE("%s: stream_session_handle is NULL \r\n", __func__);
				}
				rtk_bt_le_audio_group_release(*p_group_handle);
				BT_LOGA("%s: group handle 0x%x deleted \r\n", __func__, *p_group_handle);
				app_bt_le_audio_group_list_remove(*p_group_handle);
				*p_group_handle = NULL;
				// stop stream when group released
				app_bt_le_audio_tmap_encode_data_control(false);
				if (p_group_info->play_mode == RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION) {
					//app_bt_le_audio_tmap_decode_data_control(false);
				}
			}
			break;
		}
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_REMOVE: {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_REMOVE\r\n");
			ret = rtk_bt_le_audio_group_delete_device(param->group_handle, param->device_handle);
			app_bt_le_audio_group_list_remove_dev(param->group_handle, param->device_handle);
			BT_LOGD("[APP] %s: delete device in group %s (group_handle=%08x,device_handle=%08x) \r\n", __func__, (RTK_BT_OK != ret) ? "fail" : "ok",
					param->group_handle, param->device_handle);
			break;
		}
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_CLEAR: {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_CLEAR\r\n");
			break;
		}
		case RTK_BT_LE_AUDIO_GROUP_MSG_DEV_EMPTY: {
			BT_LOGA("[APP] RTK_BT_LE_AUDIO_GROUP_MSG_DEV_EMPTY\r\n");
			app_bt_le_audio_group_list_remove(param->group_handle);
			break;
		}
		default:
			BT_LOGE("[APP] %s unsupport group dev msg 0x%x\r\n", __func__, param->msg_type);
			break;
		}
		break;
	}
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
			cfm_param.param.media_player_name.p_media_player_name = g_ums_info.p_mcp_svr_info->media_player_name.p_media_player_name;
			cfm_param.param.media_player_name.media_player_name_len = g_ums_info.p_mcp_svr_info->media_player_name.media_player_name_len;
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE:
			BT_LOGA("[APP] read track title\r\n");
			cfm_param.param.track_title.p_track_title = g_ums_info.p_mcp_svr_info->track_title.p_track_title;
			cfm_param.param.track_title.track_title_len = g_ums_info.p_mcp_svr_info->track_title.track_title_len;
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION:
			BT_LOGA("[APP] read track duration\r\n");
			cfm_param.param.track_duration = g_ums_info.p_mcp_svr_info->track_duration;
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION:
			BT_LOGA("[APP] read track position\r\n");
			cfm_param.param.track_position = g_ums_info.p_mcp_svr_info->track_position;
			break;
		case RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID:
			BT_LOGA("[APP] read content control id\r\n");
			cfm_param.param.content_control_id = g_ums_info.p_mcp_svr_info->content_control_id;
			break;
		default:
			BT_LOGE("[APP] %s unprocess char_uuid 0x%x for MCS server\r\n", __func__, p_ind->char_uuid);
			break;
		}
		ret = rtk_bt_le_audio_mcs_server_read_confirm(p_ind->conn_handle, p_ind->cid, p_ind->service_id, p_ind->char_uuid, p_ind->offset, &cfm_param.param);
		break;
	}

#endif
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
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
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_STATE_IND: {
		app_bt_le_audio_device_info_t *p_device_info = NULL;
		rtk_bt_le_audio_vcs_client_volume_state_ind_t *param = (rtk_bt_le_audio_vcs_client_volume_state_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_STATE_IND: conn_handle %d, is_notify 0x%x, volume_setting 0x%x, mute 0x%x, change_counter 0x%x\r\n",
				param->conn_handle, param->is_notify, param->volume_state.volume_setting, param->volume_state.mute, param->volume_state.change_counter);
		BT_AT_PRINT("+BLECAP:commander,vcp,volume_state,%d,0x%x,0x%x,0x%x,0x%x\r\n",
					param->conn_handle, param->is_notify, param->volume_state.volume_setting,
					param->volume_state.mute, param->volume_state.change_counter);

		p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(param->conn_handle);
		if (p_device_info) {
			memcpy(&p_device_info->vcs_volume_state, &param->volume_state, sizeof(rtk_bt_le_audio_vcs_volume_state_t));
		}
		break;
	}

	case RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_FLAG_IND: {
		app_bt_le_audio_device_info_t *p_device_info = NULL;
		rtk_bt_le_audio_vcs_client_volume_flag_ind_t *param = (rtk_bt_le_audio_vcs_client_volume_flag_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VCS_CLIENT_VOLUME_FLAG_IND: conn_handle %d, is_notify 0x%x,volume_flags 0x%x\r\n",
				param->conn_handle, param->is_notify, param->volume_flags);
		BT_AT_PRINT("+BLECAP:commander,vcp,volume_flag,%d,0x%x,0x%x\r\n",
					param->conn_handle, param->is_notify, param->volume_flags);

		p_device_info = app_bt_le_audio_group_list_find_dev_by_conn_handle(param->conn_handle);
		if (p_device_info) {
			p_device_info->vcs_volume_flags = param->volume_flags;
		}
		break;
	}
#endif
	/********************************************* tmap unicast media receiver event end *********************************************/
	/********************************************* tmap broadcast media sender event *************************************************/
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
			if (param->cause == 0) {
				ret = app_bt_le_audio_tmap_broadcast_media_sender_setup_data_path(p_bsrc_info);
				if (ret != RTK_BT_OK) {
					BT_LOGE("[APP] app_bt_le_audio_tmap_broadcast_media_sender_setup_data_path fail,ret = 0x%x\r\n", ret);
					break;
				}
				app_bt_le_audio_tmap_encode_data_control(true);
			}
		} else if (param->broadcast_source_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING_STARTING) {
			BT_LOGA("[APP] broadcast source streaming starting \r\n");
			g_bms_info.status = RTK_BLE_AUDIO_INITIATOR_BROADCAST_START;
		} else if (param->broadcast_source_state == RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING_STOPPING) {
			BT_LOGA("[APP] broadcast source streaming stopping \r\n");
			g_bms_info.status = RTK_BLE_AUDIO_INITIATOR_BROADCAST_STOP;
		}
		break;
	}
	/********************************************* tmap broadcast media sender event end *********************************************/
	default: {
		BT_LOGD("[APP] %s default evt_code 0x%04x\r\n", __func__, evt_code);
		ret = RTK_BT_ERR_UNHANDLED;
		break;
	}
	}

	return RTK_BT_EVT_CB_OK;
}

/**
 * @brief  process a2dp sink tmap main init/ deinit.
 * @param  role[in] 1:ums 2:bms
 * @param  enable[in] 1:init 2:deinit
 * @return none.
 */
int bt_a2dp_sink_tmap_main(uint8_t role, uint8_t enable)
{
	if (1 == enable) {
		if (a2dp_tmap_demo_init_flag) {
			BT_LOGE("%s Already init! \r\n", __func__);
			return -1;
		}

		switch (role) {
		case RTK_BT_LE_AUDIO_TMAP_ROLE_UMS: {
			/* TMAP unicast media sender part */
			uint16_t tx_water_level = 0;
			app_bt_le_audio_initiator_info_t *p_ums_info = &g_ums_info;
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &p_ums_info->lea_app_conf;
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_br_bd_addr_t br_addr = {0};
			char dev_name[30] = {0};
			app_lea_audio_cap_param_t cap_param_t = {0};
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
			cap_param_t.p_mcs_service_id = &app_lea_mcs_service_id;
#endif
			a2dp_tmap_role = RTK_BT_LE_AUDIO_A2DP_SINK_UNICAST_MEDIA_SNEDER;
			bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI;
			//set lea_app_conf, must called before rtk_bt_enable
			app_bt_le_audio_lea_app_conf((void *)p_lea_app_conf,
										 0,
										 RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR | RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER,
										 RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI,
										 NULL,
										 0,
										 NULL,
										 0,
										 &cap_param_t);
			/* tmap role init */
			p_lea_app_conf->tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_UMS;
			/* A2DP part */
			{
				a2dp_demo_role = RTK_BT_A2DP_ROLE_SNK;
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
				/* a2dp sink recor add */
				BT_APP_PROCESS(rtk_sdp_record_add((void *)a2dp_sink_sdp_record, sizeof(a2dp_sink_sdp_record)));
				BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_ct_sdp_record, sizeof(avrcp_ct_sdp_record)));
				BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_tg_sdp_record, sizeof(avrcp_tg_sdp_record)));
				/* a2dp codec add (must be invoked before rtk_bt_enable) */
				BT_APP_PROCESS(rtk_bt_a2dp_codec_cfg((uint8_t)RTK_BT_AUDIO_CODEC_SBC, (void *)&codec_sbc, (uint32_t)sizeof(rtk_bt_a2dp_media_codec_sbc_t)));
			}
			//set GAP configuration
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_A2DP | \
											  RTK_BT_PROFILE_AVRCP | \
											  RTK_BT_PROFILE_SDP | \
											  RTK_BT_PROFILE_GATTS | \
											  RTK_BT_PROFILE_GATTC | \
											  RTK_BT_PROFILE_LEAUDIO;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			bt_app_conf.a2dp_role = a2dp_demo_role;
			bt_app_conf.le_audio_app_conf = p_ums_info->lea_app_conf;

			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));

			/* Enable BR/EDR inquiry scan and page scan */
			BT_APP_PROCESS(rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_VISIBLE_CONNECTABLE));
			BT_APP_PROCESS(rtk_bt_br_gap_get_bd_addr(&br_addr));

			BT_APP_PROCESS(rtk_bt_le_gap_set_scan_param(&app_def_scan_param));
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_COMMON_GP_GAP, app_le_audio_common_gap_app_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_le_audio_gap_callback));
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(p_ums_info->device_name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_GENERIC_MEDIA_PLAYER));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_GAP, br_gap_app_callback));

			/* A2DP part */
			{
				/* mix RTK_BT_DEV_NAME with bt mac address */
				strcpy(dev_name, RTK_BT_DEV_NAME);
				snprintf(&dev_name[strlen(RTK_BT_DEV_NAME)], 7, "(%02X%02X)", br_addr.addr[1], br_addr.addr[0]);
				BT_APP_PROCESS(rtk_bt_br_gap_set_device_name((const uint8_t *)dev_name));
				/* Initilize SDP part */
				BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_SDP, rtk_bt_sdp_app_callback));
				/* Initialize avrcp part */
				BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_AVRCP, app_bt_avrcp_callback));
				/* Initialize a2dp part */
				BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_A2DP, app_bt_a2dp_callback));
			}

			/* Initilize GATT (client) part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTC, app_bt_le_audio_gattc_callback));
			/* Register le audio event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_AUDIO, app_bt_le_audio_callback));

			app_bt_le_audio_tmap_unicast_media_sender_init(p_ums_info);

			/* pcm queue init */
			a2dp_tmap_demo_queue_init(&tmap_resample_pcm_data_queue, tmap_resample_queue, sizeof(tmap_resample_queue) / 2);
			tx_water_level = LE_AUDIO_TX_WATER_LEVEL;
			BT_LOGA("[APP] TMAP UMS tx_water_level: %d ms\r\n", tx_water_level);
			/* resample task init */
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
				if (false == osif_sem_take(bond_info_flush_task.sem, 0xffffffff)) {
					goto failed;
				}
			}
			p_ums_info->status = RTK_BLE_AUDIO_INITIATOR_ENABLE;
			a2dp_tmap_demo_init_flag = true;
			break;

		}
		// Broadcast Media Sender(bms)
		case RTK_BT_LE_AUDIO_TMAP_ROLE_BMS: {
			uint16_t tx_water_level = 0;
			/* TMAP broadcast media sender part */
			app_bt_le_audio_initiator_info_t *p_bms_info = &g_bms_info;
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &p_bms_info->lea_app_conf;
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_br_bd_addr_t br_addr = {0};
			char dev_name[30] = {0};
			app_lea_audio_cap_param_t cap_param_t = {0};
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
			cap_param_t.p_mcs_service_id = &app_lea_mcs_service_id;
#endif
			a2dp_tmap_role = RTK_BT_LE_AUDIO_A2DP_SINK_BROADCAST_MEDIA_SNEDER;
			bap_role = RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SOUR | RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI;
			//set lea_app_conf, must called before rtk_bt_enable
			app_bt_le_audio_lea_app_conf((void *)p_lea_app_conf,
										 0,
										 RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR | RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER,
										 RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SOUR | RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI,
										 NULL,
										 0,
										 NULL,
										 0,
										 &cap_param_t);

			/* bap broadcast part */
			{
				bap_broadcast_source_cfg.cfg_qos_type = RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY;
			}
			/* tmap role init */
			{
				p_lea_app_conf->tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_BMS;
			}
			/* A2DP part */
			{
				a2dp_demo_role = RTK_BT_A2DP_ROLE_SNK;
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
				/* a2dp sink recor add */
				BT_APP_PROCESS(rtk_sdp_record_add((void *)a2dp_sink_sdp_record, sizeof(a2dp_sink_sdp_record)));
				BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_ct_sdp_record, sizeof(avrcp_ct_sdp_record)));
				BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_tg_sdp_record, sizeof(avrcp_tg_sdp_record)));
				/* a2dp codec add (must be invoked before rtk_bt_enable) */
				BT_APP_PROCESS(rtk_bt_a2dp_codec_cfg((uint8_t)RTK_BT_AUDIO_CODEC_SBC, (void *)&codec_sbc, (uint32_t)sizeof(rtk_bt_a2dp_media_codec_sbc_t)));
			}
			/* set GAP configuration */
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_A2DP | \
											  RTK_BT_PROFILE_AVRCP | \
											  RTK_BT_PROFILE_SDP | \
											  RTK_BT_PROFILE_GATTS | \
											  RTK_BT_PROFILE_GATTC | \
											  RTK_BT_PROFILE_LEAUDIO;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			bt_app_conf.a2dp_role = a2dp_demo_role;
			bt_app_conf.le_audio_app_conf = g_bms_info.lea_app_conf;

			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));

			/* Enable BR/EDR inquiry scan and page scan */
			BT_APP_PROCESS(rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_VISIBLE_CONNECTABLE));
			BT_APP_PROCESS(rtk_bt_br_gap_get_bd_addr(&br_addr));

			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_le_audio_gap_callback));
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(g_bms_info.device_name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_GENERIC_MEDIA_PLAYER));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_GAP, br_gap_app_callback));
			/* A2DP part */
			{
				/* mix RTK_BT_DEV_NAME with bt mac address */
				strcpy(dev_name, RTK_BT_DEV_NAME);
				snprintf(&dev_name[strlen(RTK_BT_DEV_NAME)], 7, "(%02X%02X)", br_addr.addr[1], br_addr.addr[0]);
				BT_APP_PROCESS(rtk_bt_br_gap_set_device_name((const uint8_t *)dev_name));
				/* Initilize SDP part */
				BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_SDP, rtk_bt_sdp_app_callback));
				/* Initialize avrcp part */
				BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_AVRCP, app_bt_avrcp_callback));
				/* Initialize a2dp part */
				BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_A2DP, app_bt_a2dp_callback));
			}
			/* Initilize GATT (client) part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTC, app_bt_le_audio_gattc_callback));
			/* Register le audio event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_AUDIO, app_bt_le_audio_callback));

			app_bt_le_audio_tmap_broadcast_media_sender_init();

			/* pcm queue init */
			a2dp_tmap_demo_queue_init(&tmap_resample_pcm_data_queue, tmap_resample_queue, sizeof(tmap_resample_queue) / 2);
			tx_water_level = LE_AUDIO_TX_WATER_LEVEL;
			BT_LOGA("[APP] TMAP BMS tx_water_level: %d ms\r\n", tx_water_level);
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
				if (false == osif_sem_take(bond_info_flush_task.sem, 0xffffffff)) {
					goto failed;
				}
			}
			p_bms_info->status = RTK_BLE_AUDIO_INITIATOR_ENABLE;
			a2dp_tmap_demo_init_flag = true;

			break;
		}

		default:
			BT_LOGE("%s unknown role:0x%x\r\n", __func__, role);
			break;
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

	} else if (0 == enable) {
		if (!a2dp_tmap_demo_init_flag) {
			BT_LOGE("%s No need deinit! \r\n", __func__);
			return -1;
		}
		switch (role) {
		// Unicast Media Sender(ums)
		case RTK_BT_LE_AUDIO_TMAP_ROLE_UMS: {

			if (g_ums_info.status == RTK_BLE_AUDIO_INITIATOR_DISABLE) {
				BT_LOGE("[APP] %s: already disabled\r\n", __func__);
				return -1;
			}
			if (g_ums_info.status == RTK_BLE_AUDIO_INITIATOR_UNICAST_START) {
				BT_LOGE("[APP] %s: please stop unicast first before disable \r\n", __func__);
				return -1;
			}
			if (a2dp_demo_bond_info_flush) {
				/* indicate a2dp demo bond info flush thread to kill itself */
				a2dp_demo_flush_bond_info(A2DP_DEMO_BOND_INFO_THREAD_EXIT);
				if (false == osif_sem_take(bond_info_flush_task.sem, 0xffffffff)) {
					return -1;
				}
				osif_sem_delete(bond_info_flush_task.sem);
				osif_msg_queue_delete(a2dp_demo_bond_info_flush_msg_q);
				bond_info_flush_task.sem = NULL;
				bond_info_flush_task.hdl = NULL;
				a2dp_demo_bond_info_flush_msg_q = NULL;
			}
			/* TMAP Unicast Media Sender */
			{
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_GAP));
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_GAP));
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_SDP));
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_AVRCP));
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_A2DP));
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_COMMON_GP_GAP));
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_GATTC));
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_AUDIO));

				app_bt_le_audio_tmap_unicast_media_sender_deinit(&g_ums_info);

				a2dp_tmap_demo_queue_deinit(&tmap_resample_pcm_data_queue);
			}
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			g_ums_info.status = RTK_BLE_AUDIO_INITIATOR_DISABLE;
			if (g_audio_resample_t) {
				BT_APP_PROCESS(rtk_bt_a2dp_tmap_demo_audio_resample_destroy(&g_audio_resample_t));
			}
			a2dp_demo_audio_track_hdl = NULL;
			a2dp_demo_codec_entity = NULL;
			memset((void *)&audio_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
			a2dp_tmap_demo_init_flag = false;
			break;
		}
		// Broadcast Media Sender(bms)
		case RTK_BT_LE_AUDIO_TMAP_ROLE_BMS: {
			if (g_bms_info.status == RTK_BLE_AUDIO_INITIATOR_DISABLE) {
				BT_LOGE("[APP] %s: already disabled\r\n", __func__);
				return -1;
			}
			if (g_bms_info.status == RTK_BLE_AUDIO_INITIATOR_BROADCAST_START) {
				BT_LOGE("[APP] %s: please stop broadcast first before disable \r\n", __func__);
				return -1;
			}
			if (a2dp_demo_bond_info_flush) {
				/* indicate a2dp demo bond info flush thread to kill itself */
				a2dp_demo_flush_bond_info(A2DP_DEMO_BOND_INFO_THREAD_EXIT);
				if (false == osif_sem_take(bond_info_flush_task.sem, 0xffffffff)) {
					return -1;
				}
				osif_sem_delete(bond_info_flush_task.sem);
				osif_msg_queue_delete(a2dp_demo_bond_info_flush_msg_q);
				bond_info_flush_task.sem = NULL;
				bond_info_flush_task.hdl = NULL;
				a2dp_demo_bond_info_flush_msg_q = NULL;
			}
			/* TMAP Broadcast Media Sender */
			{
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_GAP));
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_GAP));
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_AVRCP));
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_A2DP));
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_GATTC));
				BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_AUDIO));

				app_bt_le_audio_tmap_broadcast_media_sender_deinit();

				a2dp_tmap_demo_queue_deinit(&tmap_resample_pcm_data_queue);
			}
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			g_bms_info.status = RTK_BLE_AUDIO_INITIATOR_DISABLE;
			if (g_audio_resample_t) {
				BT_APP_PROCESS(rtk_bt_a2dp_tmap_demo_audio_resample_destroy(&g_audio_resample_t));
			}
			a2dp_demo_audio_track_hdl = NULL;
			a2dp_demo_codec_entity = NULL;
			memset((void *)&audio_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
			a2dp_tmap_demo_init_flag = false;
			break;
		}

		default:
			BT_LOGE("%s unknown role:0x%x\r\n", __func__, role);
			break;
		}
		return 0;
	}

	BT_LOGE("%s Error parameter 0x%x ! \r\n", __func__, enable);
	return -1;

}