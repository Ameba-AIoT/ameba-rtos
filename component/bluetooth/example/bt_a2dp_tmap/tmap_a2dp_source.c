/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <bt_api_config.h>
#include <osif.h>

#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_bt_gap.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_le_audio.h>
#include <rtk_bt_tmap.h>
#include <rtk_bt_br_gap.h>
#include <rtk_bt_a2dp.h>
#include <rtk_bt_sdp.h>
#include <rtk_bt_avrcp.h>
#include <app_bt_le_audio_common.h>
#include <bt_audio_intf.h>
#include <bt_audio_codec_wrapper.h>
#include <bt_audio_track_api.h>
#include <sbc_codec_entity.h>
#include <bt_audio_resample.h>
#include "kv.h"
#include <dlist.h>
#include <bt_utils.h>

static bool tmap_a2dp_demo_init_flag = false;
/* ---------------------------- Audio Resample Related Variables-------------------------- */
/* A2DP source tx water level and the unit is in milliseconds. */
#define A2DP_SOURCE_TX_WATER_LEVEL                         10
/* A2DP source tx sample rate and channel*/
#define RTK_BT_A2DP_SOURCE_OUPUT_SAMPLE_RATE               44100
#define RTK_BT_A2DP_SOURCE_OUPUT_CHANNEL_NUM               2
/* Actual a2dp source tx water level length */
#define A2DP_SOURCE_ENCODE_PCM_DATA_WATER_LEVEL            1764 * A2DP_SOURCE_TX_WATER_LEVEL / 10
/*
    Set enough length to store resample data.The unit is in short.
    1764 bytes is equal to 44.1 KHz,2 channels pcm data bytes per 10 milliseconds.
*/
#define A2DP_ENCODE_PCM_DATA_QUEUE_MAX_LEN                 1764 * 100 / 10 * 2
/* Fixed length, used for temporary storage of audio data after resampled */
#define RESAMPLE_OUT_FRAME_BUF_MAX_LEN                     480*4*4

/*
    1.Storage 44.1KHz pcm data after resample;
    2.Send it to A2DP source encode task when reach a suitable water level.
*/
static short a2dp_src_queue[A2DP_ENCODE_PCM_DATA_QUEUE_MAX_LEN] = {0};
/* Indicate the a2dp encode task that resample pcm data has reached sufficient number */
static bool a2dp_src_dequeue_flag = false;

typedef struct {
	int q_write;
	int q_read;
	void *mtx;
	short *queue;
	uint32_t queue_size;//unit: bytes
	uint32_t queue_max_len;
} a2dp_tmap_demo_queue_t;

static a2dp_tmap_demo_queue_t a2dp_src_pcm_data_queue = {
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
/* Audio resample input sample rate, will be reconfig when iso data path created after PA sync or ASE configured */
static uint32_t demo_in_rate = 48000;
/* A2DP source output sample rate */
static uint32_t demo_out_rate = RTK_BT_A2DP_SOURCE_OUPUT_SAMPLE_RATE;
static int8_t out_frame_buf[RESAMPLE_OUT_FRAME_BUF_MAX_LEN] = {0};
/* ---------------------------- TMAP Broadcast Media Receiver parameter -------------------- */
static app_bt_le_audio_acceptor_info_t g_tmap_bmr_info = {
	.sound_channel = RTK_BT_LE_AUDIO_STEREO,
	.device_name = "TMAP BMR A2DP SRC",
	.csis_neighbor_addr = {0},
	.lea_app_conf = {0},
	.p_bap_uni_ser_info = NULL,
	.p_bap_bro_sink_info = &app_bap_bro_sink_info,
	.status = 0
};
static rtk_bt_le_audio_bis_info_t sync_bis_info = {0};
/* ---------------------------- TMAP Unicast Media Receiver parameter ---------------------- */
static app_bt_le_audio_acceptor_info_t g_tmap_umr_info = {
	.sound_channel = RTK_BT_LE_AUDIO_STEREO,
	.device_name = "TMAP UMR A2DP SRC",
	.csis_neighbor_addr = {0},
	.lea_app_conf = {0},
	.p_bap_uni_ser_info = &app_bap_uni_server_info,
	.p_bap_bro_sink_info = NULL,
	.status = RTK_BLE_AUDIO_ACCEPTOR_DISABLE
};
/* ---------------------------- LE Audio Related Variables------------------------------------- */
static rtk_bt_le_audio_tmap_role_t tmap_role;
static bool g_tmap_decode_enable = false;

static uint8_t tmap_ext_adv_handle = 0xFF;
static rtk_bt_le_ext_adv_param_t tmap_def_ext_adv_param = {
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
/* Define TMAP extended adv data default length */
#define LE_AUDIO_TMAP_EXT_ADV_DATA_DEFAULT_LEN         7
static uint8_t tmap_def_adv_data[RTK_LE_AUDIO_ADV_LEN_MAX] = {
	0x02, //AD len
	RTK_BT_LE_GAP_ADTYPE_FLAGS, //AD types
	RTK_BT_LE_GAP_ADTYPE_FLAGS_GENERAL | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, //AD data
	0x03,
	RTK_BT_LE_GAP_ADTYPE_APPEARANCE, //GAP_ADTYPE_APPEARANCE
	(uint8_t)(RTK_BT_LE_GAP_APPEARANCE_HEADSET),
	(uint8_t)(RTK_BT_LE_GAP_APPEARANCE_HEADSET >> 8)
};
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
static uint8_t app_lea_default_csis_sirk[] = {0x63, 0x68, 0x65, 0x6e, 0x67, 0x2d, 0x63, 0x61, 0x69, 0x73, 0x37, 0x38, 0x32, 0x53, 0xe8, 0x10};
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
static uint8_t app_lea_vocs_features[RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM] = { RTK_BT_LE_AUDIO_VOCS_AUDIO_LOCATION_WRITE_WITHOUT_RSP_SUPPORT | \
																		   RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_WRITE_WITHOUT_RSP_SUPPORT | \
																		   RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_WRITE_WITHOUT_RSP_SUPPORT | \
																		   RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_NOTIFY_SUPPORT
																		 };
#endif
static uint8_t app_tmap_pac_sink_codec[] = {
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
static uint8_t app_tmap_pac_sink_codec_len = sizeof(app_tmap_pac_sink_codec) / sizeof(uint8_t);
/* ---------------------------- A2DP Related Variables-------------------------- */
/* -------------------------------- Defines --------------------------------- */
#define RTK_BT_DEV_NAME                      "TMAP_A2DP_Source"
#define DEFAULT_PAGESCAN_WINDOW              0x48
#define DEFAULT_PAGESCAN_INTERVAL            0x800 //0x800
#define DEFAULT_PAGE_TIMEOUT                 0x2000
#define DEFAULT_INQUIRYSCAN_WINDOW           0x48
#define DEFAULT_INQUIRYSCAN_INTERVAL         0x800 //0x1000
#define RTK_BT_A2DP_SRC_STREAM_MAX_CREDITS   4
#define RTK_BT_DEMO_RECONNECT_TIMER_INTERVAL 5000
#define RTK_BT_DEMO_RECONNECT_COUNT          3
/* ------------------------------- Company ID ------------------------------- */
#define RTK_COMPANY_ID 0x005D
/* ----------------------------- Avrcp Related ------------------------------ */
#define VOL_MAX     0x7f
#define VOL_MIN     0x00
/* ---------------------------- Global Variables ---------------------------- */
static uint8_t curr_volume = RTK_BT_DEFAULT_ABSOLUTE_VOLUME;
static uint8_t a2dp_demo_role;
struct tmap_a2dp_demo_task_t {
	void *hdl;
	void *sem;
	uint8_t run;
};
static struct tmap_a2dp_demo_task_t a2dp_send_task = {
	.hdl = NULL,
	.sem = NULL,
	.run = 0,
};
static rtk_bt_a2dp_media_codec_sbc_t codec_sbc = {
	.sampling_frequency_mask = RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_44_1KHZ,
	.channel_mode_mask = 0x0f,
	.block_length_mask = 0xf0,
	.subbands_mask = 0x0C,
	.allocation_method_mask = 0x03,
	.min_bitpool = 2,
	.max_bitpool = 53,
};
static bool app_a2dp_src_send_flag = false;
static void *a2dp_demo_codec_entity = NULL;
static uint8_t a2dp_demo_role;
static uint8_t src_a2dp_credits = RTK_BT_A2DP_SRC_STREAM_MAX_CREDITS;
static rtk_bt_audio_codec_conf_t audio_codec_conf = {0};
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
static uint8_t remote_bd_addr[6] = {0};
/* ---------------------------- Auto reconnect config ---------------------------- */
static void *reconnect_timer = NULL;
static uint8_t a2dp_demo_auto_reconnect = 0;
static uint8_t a2dp_demo_auto_reconnect_try_count = RTK_BT_DEMO_RECONNECT_COUNT;
/* when sink disconnect due to out of range, this flag where be set up to do reconnecting */
static uint8_t a2dp_demo_disconnection_reconnecting = 0;
static char a2dp_demo_filesystem_key[] = "a2dp_reconnect_data";
/* ---------------------------- Bond Info Manage config -------------------------- */
/* Max bond number is unchangable */
#define A2DP_DEMO_MAX_BOND_NUM 8
#define A2DP_DEMO_BOND_INFO_USED_VAL 0x4E
#define A2DP_DEMO_BOND_INFO_PRIORITY_UNASSIGNED 0x4F
#define A2DP_DEMO_BOND_INFO_FLUSH_EVENT 0x01
#define A2DP_DEMO_BOND_INFO_THREAD_EXIT 0xFF

typedef struct {
	uint16_t type;
	uint16_t subtype;
	union {
		uint32_t  param;
		void     *buf;
	} u;
} a2dp_demo_msg_t;

static char a2dp_demo_bond_info_key[] = "a2dp_bond_table";
static uint8_t a2dp_demo_bond_num = 0;
static uint8_t a2dp_demo_bond_info_flush = 0;
static struct tmap_a2dp_demo_task_t bond_info_flush_task = {
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

/*********************************************a2dp function**********************************/
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

/********************************************* Audio resample related function***************************************************/
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
static uint16_t rtk_bt_le_audio_decode_pcm_data_callback(void *p_pcm_data, uint16_t p_len, void *pentity, void *track)
{
	(void)pentity;
	(void)track;
	void *p_enqueue_mtx = NULL;
	uint32_t time_stamp_before, time_stamp_after = 0;
	uint32_t enqueue_size = 0;
	uint32_t out_frames = 0;

	if (!app_a2dp_src_send_flag) {
		if (0 == (fail_cnt % 200)) {
			BT_LOGE("[APP] A2DP stream not start!\r\n");
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
	p_enqueue_mtx = a2dp_src_pcm_data_queue.mtx;
	enqueue_size = out_frames * g_audio_resample_t->out_frame_size;
	if (!p_enqueue_mtx) {
		BT_LOGE("[APP] %s p_enqueue_mtx is NULL\r\n", __func__);
		return RTK_BT_FAIL;
	}
	osif_mutex_take(p_enqueue_mtx, BT_TIMEOUT_FOREVER);
	if (RTK_BT_OK != a2dp_tmap_demo_pcm_data_enqueue(&a2dp_src_pcm_data_queue, out_frame_buf, enqueue_size)) {
		BT_LOGE("[APP] %s a2dp_src_pcm_data_queue is full!\r\n", __func__);
		osif_mutex_give(p_enqueue_mtx);
		return RTK_BT_FAIL;
	}
	if (a2dp_src_pcm_data_queue.queue_size >= A2DP_SOURCE_ENCODE_PCM_DATA_WATER_LEVEL) {
		a2dp_src_dequeue_flag = true;
	}
	osif_mutex_give(p_enqueue_mtx);

	return RTK_BT_OK;
}

static uint16_t rtk_bt_tmap_a2dp_source_audio_resample_alloc(rtk_bt_audio_resample_t **pp_sample_t, uint32_t sample_rate_in, uint8_t in_channels)
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
	*pp_sample_t = rtk_bt_audio_resample_alloc((float)demo_in_rate, (float)demo_out_rate, in_channels, RTK_BT_A2DP_SOURCE_OUPUT_CHANNEL_NUM, resample_in_frames);

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
/********************************************* Audio resample related function end ***************************************************/

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
				if (a2dp_send_task.run) {
					app_a2dp_src_send_flag = true;
				}
			}
			break;
		}

		case RTK_BT_AVRCP_STATUS_PAUSED: {
			BT_LOGA("[AVRCP]: Paused \r\n");
			if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
				if (a2dp_send_task.run) {
					app_a2dp_src_send_flag = false;
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

	case RTK_BT_AVRCP_EVT_PLAY_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Play req \r\n");
		if (a2dp_send_task.run) {
			app_a2dp_src_send_flag = true;
		}
		break;
	}

	case RTK_BT_AVRCP_EVT_PAUSE_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Pause req \r\n");
		if (a2dp_send_task.run) {
			app_a2dp_src_send_flag = false;
		}
		break;
	}

	case RTK_BT_AVRCP_EVT_STOP_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Stop req \r\n");
		if (a2dp_send_task.run) {
			app_a2dp_src_send_flag = false;
		}
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

static short pcm_data_output_buf[1024];
static uint16_t a2dp_demo_send_data_seq = 0;

static void app_a2dp_src_send_data(void)
{
	rtk_bt_a2dp_stream_data_send_t data_send_t = {0};
	struct enc_codec_buffer *penc_codec_buffer_t = NULL;
	void *pmtx = NULL;
	uint32_t queue_size = 0;
	uint32_t encode_byte = 1024;
	uint16_t ret = 0;

	if (src_a2dp_credits) {
		// dequeue
		pmtx = a2dp_src_pcm_data_queue.mtx;
		if (pmtx == NULL) {
			BT_LOGE("[APP] %s:pmtx is NULL\r\n", __func__);
		}
		while (a2dp_src_pcm_data_queue.queue_size < encode_byte && a2dp_send_task.run) {
			osif_delay(2);
		}
		osif_mutex_take(pmtx, BT_TIMEOUT_FOREVER);
		ret = a2dp_tmap_demo_pcm_data_dequeue(&a2dp_src_pcm_data_queue, (int8_t *)pcm_data_output_buf, encode_byte);
		queue_size = a2dp_src_pcm_data_queue.queue_size;
		osif_mutex_give(pmtx);
		BT_LOGD("[APP] %s: %u, queue_size: %u\r\n", __func__, queue_size);

		//if a2dp source data is not enough
		if (RTK_BT_OK != ret) {
			BT_LOGE("[APP] %s: dequeue failed\r\n", __func__);
			return ;
		}
		//SBC encode
		penc_codec_buffer_t = rtk_bt_audio_data_encode(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_codec_entity, pcm_data_output_buf, encode_byte);
		//data send
		if (penc_codec_buffer_t) {
			memset((void *)&data_send_t, 0, sizeof(rtk_bt_a2dp_stream_data_send_t));
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
			BT_LOGE("[A2DP SRC]: Encode fail \r\n");
		}
	} else {
		BT_LOGD("[A2DP] waiting src_a2dp_credits \r\n");
	}
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
		BT_LOGA("[A2DP] encode sample rate: 16000 \r\n");
		break;
	case RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_32KHZ:
		psbc_encoder_t->sample_rate = (uint32_t)32000;
		BT_LOGA("[A2DP] encode sample rate: 32000 \r\n");
		break;
	case RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_44_1KHZ:
		psbc_encoder_t->sample_rate = (uint32_t)44100;
		BT_LOGA("[A2DP] encode sample rate: 44100 \r\n");
		break;
	case RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_48KHZ:
		psbc_encoder_t->sample_rate = (uint32_t)48000;
		BT_LOGA("[A2DP] encode sample rate: 48000 \r\n");
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

static void a2dp_send_task_entry(void *ctx)
{
	(void)ctx;
	osif_sem_give(a2dp_send_task.sem);

	while (a2dp_send_task.run) {
		if (app_a2dp_src_send_flag && a2dp_src_dequeue_flag) {
			app_a2dp_src_send_data();
		}
		osif_delay(3);
	}

	osif_sem_give(a2dp_send_task.sem);
	osif_task_delete(NULL);
}

static rtk_bt_evt_cb_ret_t rtk_bt_a2dp_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	uint8_t bd_addr[6];
	void *p_mtx;
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
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
			if (a2dp_send_task.hdl) {
				app_a2dp_src_send_flag = false;
				a2dp_send_task.run = 0;
				if (false == osif_sem_take(a2dp_send_task.sem, 0xffffffff)) {
					BT_LOGE("[A2DP Demo] take a2dp_send_task.sem fail \r\n");
					break;
				}
				osif_sem_delete(a2dp_send_task.sem);
				memset((void *)&a2dp_send_task, 0, sizeof(struct tmap_a2dp_demo_task_t));
				BT_LOGA("[A2DP] Delete a2dp send task \r\n");
			}
		}
		rtk_bt_audio_codec_remove(audio_codec_conf.codec_index, a2dp_demo_codec_entity);
		memset((void *)&audio_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
		//rtk_bt_audio_deinit();
		a2dp_demo_codec_entity = NULL;
		memcpy((void *)bd_addr, disconn_ind->bd_addr, 6);
		BT_LOGA("[A2DP] A2DP disconnection completion with %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		BT_AT_PRINT("+BTA2DP:disconn,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_A2DP_EVT_CONFIG_CMPL: {
		rtk_bt_a2dp_codec_t *pa2dp_codec = (rtk_bt_a2dp_codec_t *)param;
		if ((pa2dp_codec->codec_type & RTK_BT_AUDIO_CODEC_SBC/* | RTK_BT_AUDIO_CODEC_AAC */) == 0) {
			BT_LOGE("[A2DP] Not support codec %d \r\n", pa2dp_codec->codec_type);
			break;
		}
		if (rtk_bt_audio_init()) {
			BT_LOGE("[A2DP] rtk_bt_audio_init fail \r\n");
			break;
		}
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
			if (pa2dp_codec->codec_type == RTK_BT_AUDIO_CODEC_SBC) {
				rtk_bt_a2dp_sbc_parse_encoder_struct(pa2dp_codec, &sbc_codec_t.encoder_t);
				audio_codec_conf.codec_index = RTK_BT_AUDIO_CODEC_SBC;
				audio_codec_conf.param = (void *)&sbc_codec_t;
				audio_codec_conf.param_len = sizeof(sbc_codec_t);
			}
		}
		a2dp_demo_codec_entity = rtk_bt_audio_codec_add(&audio_codec_conf);
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
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_START_RSP: {
		rtk_bt_a2dp_stream_start_t *pa2dp_stream = (rtk_bt_a2dp_stream_start_t *)param;

		BT_LOGA("[A2DP] RTK_BT_A2DP_EVT_STREAM_START_RSP active_a2dp_idx %d, streaming_fg %d \r\n",
				pa2dp_stream->active_a2dp_link_index, pa2dp_stream->stream_cfg);
		// flush queue before a2dp src send
		p_mtx = a2dp_src_pcm_data_queue.mtx;
		if (!p_mtx) {
			BT_LOGE("[APP] %s:p_mtx is NULL\r\n", __func__);
			break;
		}
		osif_mutex_take(p_mtx, BT_TIMEOUT_FOREVER);
		dequeue_size = a2dp_src_pcm_data_queue.queue_size;
		if (dequeue_size) {
			a2dp_tmap_demo_queue_pcm_data_flush(&a2dp_src_pcm_data_queue);
		}
		osif_mutex_give(p_mtx);
		{
			if (a2dp_send_task.hdl) {
				BT_LOGA("[A2DP] Start a2dp send task \r\n");
			} else {
				BT_LOGA("[A2DP] Create a2dp send task \r\n");
				if (false == osif_sem_create(&a2dp_send_task.sem, 0, 1)) {
					BT_LOGE("[A2DP ] Create a2dp_send_task.sem fail \r\n");
					break;
				}
				a2dp_send_task.run = 1;
				if (false == osif_task_create(&a2dp_send_task.hdl, "a2dp_send_task",
											  a2dp_send_task_entry, NULL,
											  2048, 4)) {
					osif_sem_delete(a2dp_send_task.sem);
					a2dp_send_task.run = 0;
					BT_LOGE("[A2DP] Create a2dp_send_task fail \r\n");
					break;
				}
				osif_sem_take(a2dp_send_task.sem, 0xffffffff);
			}
			app_a2dp_src_send_flag = true;
		}
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_STOP: {
		rtk_bt_a2dp_conn_ind_t *conn_ind = (rtk_bt_a2dp_conn_ind_t *)param;
		memcpy((void *)bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[A2DP] Stream stop from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		BT_AT_PRINT("+BTA2DP:stop,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
			app_a2dp_src_send_flag = false;
		}
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_CLOSE: {
		rtk_bt_a2dp_conn_ind_t *conn_ind = (rtk_bt_a2dp_conn_ind_t *)param;
		memcpy((void *)bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[A2DP] Stream close from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
			app_a2dp_src_send_flag = false;
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

/*********************************************tmap broadcast media receiver function************************/
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

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
static uint16_t app_bt_le_audio_vocs_set_param(uint16_t sound_channel)
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
	if (p_iso_path->status_ok_cnt % 500 == 0) {
		BT_LOGE("[APP] iso_conn_handle 0x%x,iso_data_rx_queue_num %d, rx_ok_cnt %d, rx_fail_cnt %d, rx_lost_cnt %d, FreeHeap %d\r\n",
				p_iso_path->iso_conn_handle, p_iso_path->iso_data_rx_queue_num, (int)p_iso_path->status_ok_cnt, (int)p_iso_path->status_fail_cnt,
				(int)p_iso_path->status_lost_cnt, osif_mem_peek(RAM_TYPE_DATA_ON));
	}
}

static void app_bt_le_audio_tmap_decode_data_control(bool enable)
{
	BT_LOGD("[APP] %s %d\r\n", __func__, enable);
	if (enable) {
		if (g_tmap_decode_enable == true) {
			BT_LOGE("[APP] %s: decode is alreay enabled\r\n", __func__);
			return;
		}
		g_tmap_decode_enable = true;
	} else {
		if (g_tmap_decode_enable == false) {
			BT_LOGE("[APP] %s: decode is alreay disabled\r\n", __func__);
			return;
		}
		g_tmap_decode_enable = false;
	}
}

static rtk_bt_audio_track_t *app_bt_tmap_a2dp_source_track_add(rtk_bt_le_audio_cfg_codec_t *p_codec)
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

	channels = app_bt_le_audio_get_lea_chnl_num(p_codec->audio_channel_allocation);
	audio_chnl = app_bt_le_audio_translate_le_chnl_to_audio_chnl(p_codec->audio_channel_allocation);
	if (audio_chnl & RTK_BT_LE_AUDIO_LOCATION_FL) {
		left_volume = DEFAULT_AUDIO_TRACK_LEFT_VOLUME;
	}
	if (audio_chnl & RTK_BT_LE_AUDIO_LOCATION_FR) {
		right_volume = DEFAULT_AUDIO_TRACK_RIGHT_VOLUME;
	}

	return rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_LC3, (float)left_volume, (float)right_volume,
								  channels, rate, BT_AUDIO_FORMAT_PCM_16_BIT, duration, (pcm_data_cb)rtk_bt_le_audio_decode_pcm_data_callback, false);
}

static uint16_t app_bt_tmap_a2dp_source_iso_data_path_add(uint8_t iso_mode, uint8_t iso_idx, uint8_t path_direction, uint16_t iso_conn_handle, void *codec_cfg)
{
	rtk_bt_le_audio_cfg_codec_t *p_codec = (rtk_bt_le_audio_cfg_codec_t *)codec_cfg;
	app_lea_iso_data_path_t *p_iso_path = NULL;

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
		p_iso_path->track_hdl = app_bt_tmap_a2dp_source_track_add(p_codec);
	}

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

static uint16_t app_bt_le_audio_tmap_broadcast_media_receiver_setup_data_path(rtk_bt_le_audio_sync_handle_t sync_handle)
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
	if (bis_info.num_bis >= 2) {
		BT_LOGE("[APP] num bis: %d exceed 1 !\r\n", bis_info.num_bis);
		return RTK_BT_FAIL;
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
			BT_LOGD("[APP] rtk_bt_le_audio_sync_get_bis_sync_codec_cfg ok (sync_handle %08x, bis_idx %d,", sync_handle, bis_idx);
			BT_LOGA("[APP] type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu 0x%x, octets_per_codec_frame 0x%x, audio_channel_allocation 0x%x)\r\n",
					bis_codec_cfg.type_exist, bis_codec_cfg.frame_duration,
					bis_codec_cfg.sample_frequency, bis_codec_cfg.codec_frame_blocks_per_sdu,
					bis_codec_cfg.octets_per_codec_frame, (unsigned int)bis_codec_cfg.audio_channel_allocation);
			/* audio resample struct init */
			uint32_t sample_rate_in = 0;
			uint8_t channels = 0;
			sample_rate_in = app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(bis_codec_cfg.sample_frequency);
			channels = app_bt_le_audio_get_lea_chnl_num(bis_codec_cfg.audio_channel_allocation);
			rtk_bt_tmap_a2dp_source_audio_resample_alloc(&g_audio_resample_t, sample_rate_in, channels);
			/* add audio track && register pcm data callback  */
			app_bt_tmap_a2dp_source_iso_data_path_add(RTK_BT_LE_AUDIO_BIS_MODE, bis_idx, RTK_BLE_AUDIO_ISO_DATA_PATH_RX, bis_conn_handle, &bis_codec_cfg);
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

static uint16_t app_bt_le_audio_tmap_broadcast_media_receiver_remove_data_path(rtk_bt_le_audio_sync_handle_t sync_handle)
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
	//set up iso data path
	for (i = 0; i < sync_bis_info.num_bis; i++) {
		bis_conn_handle = sync_bis_info.bis_conn_info[i].bis_conn_handle;
		app_bt_le_audio_iso_data_path_remove(bis_conn_handle, RTK_BLE_AUDIO_ISO_DATA_PATH_RX);
		if (g_audio_resample_t) {
			rtk_bt_a2dp_tmap_demo_audio_resample_destroy(&g_audio_resample_t);
		}
	}
	return ret;
}

static uint16_t app_bt_le_audio_tmap_broadcast_media_receiver_init(app_bt_le_audio_acceptor_info_t *p_info)
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

static uint16_t app_bt_le_audio_tmap_broadcast_media_receiver_deinit(app_bt_le_audio_acceptor_info_t *p_info)
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

/************************************** TMAP Unicast Media Receiver function *****************************/
static uint16_t app_bt_le_audio_tmap_unicast_media_receiver_init(void)
{

	/* intialize iso data path */
	app_bt_le_audio_iso_data_path_init();

	return RTK_BT_OK;
}

static uint16_t app_bt_le_audio_tmap_unicast_media_receiver_deinit(void)
{

	/* deintialize iso data path */
	app_bt_le_audio_iso_data_path_deinit();

	return RTK_BT_OK;
}

/************************************** TMAP Unicast Media Receiver end **********************************/

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
		if (tmap_role & RTK_BT_LE_AUDIO_TMAP_ROLE_BMR) {
			app_bt_le_audio_bass_scan_report_handle(scan_res_ind);
		}
		break;
	}
#endif

	case RTK_BT_LE_GAP_EVT_SCAN_STOP_IND: {
		rtk_bt_le_scan_stop_ind_t *scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)param;
		if (!scan_stop_ind->err) {
			BT_LOGA("[APP] Scan stopped, reason: 0x%x\r\n", scan_stop_ind->stop_reason);
			if (tmap_role & RTK_BT_LE_AUDIO_TMAP_ROLE_BMR) {
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
			if (tmap_role & RTK_BT_LE_AUDIO_TMAP_ROLE_UMR) {
				g_tmap_umr_info.status = RTK_BLE_AUDIO_ACCEPTOR_CONN;
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
		if (tmap_role & RTK_BT_LE_AUDIO_TMAP_ROLE_UMR) {
			//restart ext adv
			rtk_bt_le_gap_start_ext_adv(tmap_ext_adv_handle, 0, 0);
			if (tmap_role & RTK_BT_LE_AUDIO_TMAP_ROLE_UMR) {
				g_tmap_umr_info.status = RTK_BLE_AUDIO_ACCEPTOR_DISC;
			}
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

static uint8_t app_tmap_pac_source_codec[] = {
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

static rtk_bt_evt_cb_ret_t app_bt_le_audio_callback(uint8_t evt_code, void *data, uint32_t len)
{
	uint16_t ret = RTK_BT_OK;
	(void)len;

	app_bt_le_audio_sync_dev_info_t *p_sync_dev_info = NULL;
	app_bt_le_audio_bap_unicast_server_info_t *p_bap_uni_ser_info = g_tmap_umr_info.p_bap_uni_ser_info;
	switch (evt_code) {
	/********************************************* tmap broadcast media receiver event********************************************/
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
			if (g_tmap_decode_enable) {
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
		BT_AT_PRINT("+BLEBAP:broadcast,sink,sync_state,%p,0x%x,0x%x,0x%x\r\n",
					param->sync_handle, param->sync_state, param->action, param->cause);

		p_sync_dev_info = app_bt_le_audio_sync_dev_list_find(param->sync_handle);
		if (!p_sync_dev_info) {
			BT_LOGE("[APP] %s not find sync dev info for sync_handle %08x\r\n", __func__, param->sync_handle);
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

	// broadcast sink && scan delegator event
	case RTK_BT_LE_AUDIO_EVT_PA_BIG_INFO_IND: {
		rtk_bt_le_audio_pa_biginfo_ind_t *param = (rtk_bt_le_audio_pa_biginfo_ind_t *)data;
		if (tmap_role & RTK_BT_LE_AUDIO_TMAP_ROLE_BMR) {
			BT_LOGA("[APP] broadcast sink pa biginfo indication: sync_handle: %08x, num_bis 0x%x, sync_id 0x%x, pa_sync_handle 0x%x, encryption 0x%x\r\n",
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
			app_bt_le_audio_bap_broadcast_sink_info_t *p_bsink_info = g_tmap_bmr_info.p_bap_bro_sink_info;
			if (p_bsink_info->big_info_ind_sem) {
				osif_sem_give(p_bsink_info->big_info_ind_sem);
			}
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
			//setup data path
			g_tmap_bmr_info.status = RTK_BLE_AUDIO_ACCEPTOR_BIG_SYNC;
			ret = app_bt_le_audio_tmap_broadcast_media_receiver_setup_data_path(param->sync_handle);
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
			app_bt_le_audio_tmap_decode_data_control(true);
		} else if (param->sync_state == RTK_BT_LE_AUDIO_BIG_SYNC_STATE_TERMINATED) {
			BT_LOGA("[APP] broadcast sink big sync termiated\r\n");
			ret = app_bt_le_audio_tmap_broadcast_media_receiver_remove_data_path(param->sync_handle);
			BT_LOGA("[APP] app_bt_le_audio_tmap_broadcast_media_receiver_remove_data_path %s after big sync terminated! ret: 0x%x\r\n",
					((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
			g_tmap_bmr_info.status = RTK_BLE_AUDIO_ACCEPTOR_BIG_SYNC_TERMINATE;
			ret = rtk_bt_le_audio_sync_release(param->sync_handle);
			BT_LOGA("[APP] rtk_bt_le_audio_sync_release %s, ret: 0x%x\r\n", ((RTK_BT_OK != ret) ? "fail" : "ok"), ret);
			//deinit rx thread
			app_bt_le_audio_tmap_decode_data_control(false);
			ret = app_bt_le_audio_sync_dev_list_remove(p_sync_dev_info->sync_handle);
			BT_LOGA("[APP] %s: app_bt_le_audio_sync_dev_list_remove %s, ret: 0x%x\r\n",
					__func__, ((RTK_BT_OK != ret) ? "fail" : "ok"), ret);

			a2dp_src_dequeue_flag = false;
		}
		break;
	}
	/********************************************* tmap broadcast media receiver event end *************************************************/

	/********************************************* tmap unicast media receiver event *************************************************/
	case RTK_BT_LE_AUDIO_EVT_ASCS_CP_CODEC_CFG_IND: {
		rtk_bt_le_audio_ascs_cp_codec_cfg_ind_t *param = (rtk_bt_le_audio_ascs_cp_codec_cfg_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CP_CODEC_CFG_IND(conn_handle %d, ase_id 0x%x\r\n",
				param->conn_handle, param->ase_id);
		BT_LOGD("[APP] type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu 0x%x, octets_per_codec_frame 0x%x, audio_channel_allocation 0x%x)\r\n",
				param->codec_cfg.type_exist, param->codec_cfg.frame_duration, param->codec_cfg.sample_frequency, param->codec_cfg.codec_frame_blocks_per_sdu,
				param->codec_cfg.octets_per_codec_frame, (unsigned int)param->codec_cfg.audio_channel_allocation);
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_ASCS_CP_QOS_CFG_IND: {
		rtk_bt_le_audio_ascs_cp_qos_cfg_ind_t *param = (rtk_bt_le_audio_ascs_cp_qos_cfg_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CP_QOS_CFG_IND(conn_handle %d, ase_id 0x%x, cig_id 0x%x, cis_id 0x%x,",
				param->conn_handle, param->ase_id, param->cig_id, param->cis_id);
		BT_LOGD("[APP] framing 0x%x, phy 0x%x, retransmission_number 0x%x, max_sdu 0x%x, max_transport_latency 0x%x, sdu_interval  0x%x, presentation_delay 0x%x)\r\n",
				param->framing, param->phy, param->retransmission_number, param->max_sdu,
				param->max_transport_latency, (unsigned int)param->sdu_interval, (unsigned int)param->presentation_delay);
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_ASCS_CP_ENABLE_IND: {
		rtk_bt_le_audio_ascs_cp_enable_ind_t *param = (rtk_bt_le_audio_ascs_cp_enable_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CP_ENABLE_IND(conn_handle %d, ase_num 0x%x\r\n",
				param->conn_handle, param->ase_num);
#if 0
		BT_DUMPA("ase id: ", param->ase_id, param->ase_num);
#endif
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_ASCS_CP_DISABLE_IND: {
		rtk_bt_le_audio_ascs_cp_disable_ind_t *param = (rtk_bt_le_audio_ascs_cp_disable_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CP_DISABLE_IND(conn_handle %d, ase_num 0x%x\r\n",
				param->conn_handle, param->ase_num);
#if 0
		BT_DUMPA("ase id: ", param->ase_id, param->ase_num);
#endif
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_ASCS_GET_PREFER_QOS_IND: {
		rtk_bt_le_audio_ascs_get_prefer_qos_ind_t *param = (rtk_bt_le_audio_ascs_get_prefer_qos_ind_t *) data;
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
		rtk_bt_le_audio_ascs_ase_state_ind_t *param = (rtk_bt_le_audio_ascs_ase_state_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_ASE_STATE_IND(conn_handle %d, ase_id 0x%x, audio_role 0x%x, ase_state 0x%x)\r\n",
				param->conn_handle, param->ase_id, param->audio_role, param->ase_state);
		switch (param->ase_state) {
		case RTK_BLE_AUDIO_ASCS_ASE_STATE_IDLE:
			BT_LOGA("[APP] (ASCS_ASE_STATE_IDLE)\r\n");
			if (param->audio_role == RTK_BLE_AUDIO_SINK) {
				// ASE Streaming state --> ASE Releasing state --> ASE Idle state
				if (p_bap_uni_ser_info->enabling_sink_ase_num > p_bap_uni_ser_info->streaming_sink_ase_num) {
					p_bap_uni_ser_info->enabling_sink_ase_num--;
				} else if (p_bap_uni_ser_info->enabling_sink_ase_num < p_bap_uni_ser_info->streaming_sink_ase_num) {
					BT_LOGE("[APP] enabling_sink_ase_num is already %d, something is wrong!!\r\n", p_bap_uni_ser_info->enabling_sink_ase_num);
					break;
				}
			} else if (param->audio_role == RTK_BLE_AUDIO_SOURCE) {
				if (p_bap_uni_ser_info->enabling_source_ase_num > p_bap_uni_ser_info->streaming_source_ase_num) {
					p_bap_uni_ser_info->enabling_source_ase_num--;
				} else if (p_bap_uni_ser_info->enabling_source_ase_num < p_bap_uni_ser_info->streaming_source_ase_num) {
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
			if (param->audio_role == RTK_BLE_AUDIO_SINK) {
				// ASE Streaming state --> ASE Qos Configured state, but not enter ASE releasing state
				if (p_bap_uni_ser_info->streaming_sink_ase_num > 0 && \
					p_bap_uni_ser_info->streaming_sink_ase_num == p_bap_uni_ser_info->enabling_sink_ase_num) {
					p_bap_uni_ser_info->enabling_sink_ase_num--;
					// BAP unicast client stop stream
					a2dp_src_dequeue_flag = false;
				}
			} else if (param->audio_role == RTK_BLE_AUDIO_SOURCE) {
				if (p_bap_uni_ser_info->streaming_source_ase_num > 0 && \
					p_bap_uni_ser_info->streaming_source_ase_num == p_bap_uni_ser_info->enabling_source_ase_num) {
					p_bap_uni_ser_info->enabling_source_ase_num--;
				}
			}
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
			if (param->audio_role == RTK_BLE_AUDIO_SINK) {
				if (p_bap_uni_ser_info->streaming_sink_ase_num < p_bap_uni_ser_info->enabling_sink_ase_num) {
					// ASE Codec Configured state--> ASE QoS Configured --> ASE Enabling state--> ASE Streaming state
					p_bap_uni_ser_info->streaming_sink_ase_num++;
				} else if (p_bap_uni_ser_info->streaming_sink_ase_num == p_bap_uni_ser_info->enabling_sink_ase_num) {
					// ASE Streaming state--> ASE QoS Configured --> ASE Enabling state--> ASE Streaming state
					// do nothing
				} else {
					BT_LOGE("[APP] streaming_sink_ase_num(%d) > enabling_sink_ase_num(%d), something is wrong!!\r\n", p_bap_uni_ser_info->streaming_sink_ase_num,
							p_bap_uni_ser_info->enabling_sink_ase_num);
					break;
				}
				if (p_bap_uni_ser_info->streaming_sink_ase_num == p_bap_uni_ser_info->enabling_sink_ase_num) {
					//init rx thread
					g_tmap_umr_info.status = RTK_BLE_AUDIO_ACCEPTOR_START;
					app_bt_le_audio_tmap_decode_data_control(true);
				}
			} else if (param->audio_role == RTK_BLE_AUDIO_SOURCE) {
				if (p_bap_uni_ser_info->streaming_source_ase_num < p_bap_uni_ser_info->enabling_source_ase_num) {
					p_bap_uni_ser_info->streaming_source_ase_num++;
				} else if (p_bap_uni_ser_info->streaming_source_ase_num == p_bap_uni_ser_info->enabling_source_ase_num) {
					// ASE Streaming state--> ASE QoS Configured --> ASE Enbabling state--> ASE Streaming state
					// do nothing
				} else {
					BT_LOGE("[APP] streaming_source_ase_num(%d) > enabling_source_ase_num(%d), something is wrong!!\r\n", p_bap_uni_ser_info->streaming_source_ase_num,
							p_bap_uni_ser_info->enabling_source_ase_num);
					break;
				}
				if (p_bap_uni_ser_info->streaming_source_ase_num == p_bap_uni_ser_info->enabling_source_ase_num) {
					//init tx thread in RTK_BT_LE_AUDIO_EVT_ASCS_SETUP_DATA_PATH_IND
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
					app_bt_le_audio_tmap_decode_data_control(false);
				}
				// BAP unicast client stop stream
				a2dp_src_dequeue_flag = false;
				g_tmap_umr_info.status = RTK_BLE_AUDIO_ACCEPTOR_STOP;
			} else if (param->audio_role == RTK_BLE_AUDIO_SOURCE) {
				if (p_bap_uni_ser_info->streaming_source_ase_num > 0) {
					p_bap_uni_ser_info->streaming_source_ase_num--;
				} else {
					BT_LOGE("[APP] streaming_source_ase_num is already %d, something is wrong!!\r\n", p_bap_uni_ser_info->streaming_source_ase_num);
					break;
				}
				if (p_bap_uni_ser_info->streaming_source_ase_num == 0) {
					//deinit tx thread
					//app_bt_le_audio_tmap_encode_data_control(false);
				}
				g_tmap_umr_info.status = RTK_BLE_AUDIO_ACCEPTOR_STOP;
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
		BT_AT_PRINT("+BLEBAP:unicast,server,ascs,setup_data_path,%d,0x%x,0x%x,0x%x\r\n",
					param->conn_handle, param->ase_id, param->path_direction, param->cis_conn_handle);
		BT_LOGD("[APP] type_exist 0x%x, frame_duration 0x%x, sample_frequency 0x%x, codec_frame_blocks_per_sdu 0x%x, octets_per_codec_frame 0x%x, audio_channel_allocation 0x%x)\r\n",
				param->codec_cfg.type_exist, param->codec_cfg.frame_duration, param->codec_cfg.sample_frequency, param->codec_cfg.codec_frame_blocks_per_sdu,
				param->codec_cfg.octets_per_codec_frame, (unsigned int)param->codec_cfg.audio_channel_allocation);
		/* audio resample struct init */
		uint32_t sample_rate_in = 0;
		uint8_t channels = 0;
		sample_rate_in = app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(param->codec_cfg.sample_frequency);
		channels = app_bt_le_audio_get_lea_chnl_num(param->codec_cfg.audio_channel_allocation);
		rtk_bt_tmap_a2dp_source_audio_resample_alloc(&g_audio_resample_t, sample_rate_in, channels);
		/* add audio track && register pcm data callback  */
		app_bt_tmap_a2dp_source_iso_data_path_add(RTK_BT_LE_AUDIO_CIS_MODE, 0, param->path_direction, param->cis_conn_handle, &param->codec_cfg);
		break;
	}
	case RTK_BT_LE_AUDIO_EVT_ASCS_REMOVE_DATA_PATH_IND: {
		rtk_bt_le_audio_ascs_remove_data_path_ind_t *param = (rtk_bt_le_audio_ascs_remove_data_path_ind_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_REMOVE_DATA_PATH_IND(conn_handle %d, ase_id 0x%x, path_direction 0x%x, cis_conn_handle 0x%x\r\n",
				param->conn_handle, param->ase_id, param->path_direction, param->cis_conn_handle);
		BT_AT_PRINT("+BLEBAP:unicast,server,ascs,remove_data_path,%d,0x%x,0x%x,0x%x\r\n",
					param->conn_handle, param->ase_id, param->path_direction, param->cis_conn_handle);

		//remove the iso conn handle when tx data path remove
		app_bt_le_audio_iso_data_path_remove(param->cis_conn_handle, param->path_direction);
		if (g_audio_resample_t) {
			rtk_bt_a2dp_tmap_demo_audio_resample_destroy(&g_audio_resample_t);
		}
		g_tmap_umr_info.status = RTK_BLE_AUDIO_ACCEPTOR_STOP;

		break;
	}
	case RTK_BT_LE_AUDIO_EVT_ASCS_CIS_CONN_INFO: {
		rtk_bt_le_audio_ascs_cis_conn_info_t *param = (rtk_bt_le_audio_ascs_cis_conn_info_t *) data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_ASCS_CIS_CONN_INFO: conn_handle %d, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x\r\n",
				param->conn_handle, param->cis_conn_handle, param->cig_id, param->cis_id);
		break;
	}
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
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	case RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_OFFSET_STATE_IND: {
		rtk_bt_le_audio_vocs_write_offset_state_ind_t *param = (rtk_bt_le_audio_vocs_write_offset_state_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_VOCS_WRITE_OFFSET_STATE_IND: conn_handle 0x%x, srv_instance_id %d, volume_offset %d\r\n",
				param->conn_handle, param->srv_instance_id, param->volume_offset);
		BT_AT_PRINT("+BLECAP:acceptor,vocs,write_offset_state,0x%x,%d,%d\r\n",
					param->conn_handle, param->srv_instance_id, param->volume_offset);
		if (param->srv_instance_id == RTK_BT_LE_AUDIO_VOCS_IDX) {
			// set volume offset
			// volume_offset should in range [-255, 255]
			float left_volume = 0, right_volume = 0;
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
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
	case RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_DISCOVERY_DONE_IND: {
		rtk_bt_le_audio_mcp_client_discovery_done_ind_t *param = (rtk_bt_le_audio_mcp_client_discovery_done_ind_t *)data;
		BT_LOGA("[APP] RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_DISCOVERY_DONE_IND(conn_handle %d,is_found %d, load_from_ftl %d, general_mcs %d, srv_num %d\r\n",
				param->conn_handle, param->is_found, param->load_from_ftl, param->general_mcs, param->srv_num);
		BT_AT_PRINT("+BLECAP:acceptor,mcp,dis,%d,%d,%d,%d,%d\r\n",
					param->conn_handle, param->is_found, param->load_from_ftl, param->general_mcs, param->srv_num);
		if (param->is_found && param->general_mcs) {
			rtk_bt_le_audio_mcs_client_cfg_cccd(param->conn_handle, 0, true, 0xFFFFFFFF, true);
		}
		if (param->general_mcs) {
			BT_LOGA("[APP] TX power report: conn_handle %d, type %d, txpower %d\r\n", param->conn_handle);
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
				BT_LOGA("[APP] media_state %s\r\n", RTK_BLE_AUDIO_MCS_STATE_TRANSLATE(param->data.media_state));
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
			BT_LOGA("media_state %s\r\n", RTK_BLE_AUDIO_MCS_STATE_TRANSLATE(param->data.media_state));
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
	/********************************************* tmap unicast media receiver event end *********************************************/
	default: {
		BT_LOGD("[APP] %s default evt_code 0x%04x\r\n", __func__, evt_code);
		ret = RTK_BT_ERR_UNHANDLED;
		break;
	}
	}

	return RTK_BT_EVT_CB_OK;
}

int bt_tmap_a2dp_source_main(uint8_t role, uint8_t enable)
{
	if (1 == enable) {
		if (tmap_a2dp_demo_init_flag) {
			BT_LOGE("%s Already init! \r\n", __func__);
			return -1;
		}

		tmap_role = (rtk_bt_le_audio_tmap_role_t)role;
		rtk_bt_br_bd_addr_t bd_addr = {0};
		char dev_name[30] = {0};
		char addr_str[30] = {0};
		uint32_t a2dp_tx_water_level = 0;

		switch (role) {
		/* TMAP Broadcast Media Receiver init*/
		case RTK_BT_LE_AUDIO_TMAP_ROLE_BMR: {
			rtk_bt_app_conf_t bt_app_conf = {0};

			app_bt_le_audio_bap_broadcast_sink_info_t *p_bmr_bap_info = g_tmap_bmr_info.p_bap_bro_sink_info;
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &g_tmap_bmr_info.lea_app_conf;
			app_lea_audio_cap_param_t cap_param_t = {0};
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
			cap_param_t.p_csis_sirk = app_lea_default_csis_sirk;
			cap_param_t.csis_sirk_len = RTK_BT_LE_CSIS_SIRK_LEN;
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
			cap_param_t.vocs_num = RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM;
			cap_param_t.p_vocs_feature_tbl = app_lea_vocs_features;
#endif
			//set lea_app_conf, must called before rtk_bt_enable
			app_bt_le_audio_lea_app_conf((void *)p_lea_app_conf, g_tmap_bmr_info.sound_channel,
										 RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR,
										 RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK,
										 NULL,
										 0,
										 app_tmap_pac_sink_codec,
										 app_tmap_pac_sink_codec_len,
										 &cap_param_t);

			p_bmr_bap_info->config_sink_audio_location = p_lea_app_conf->pacs_param.sink_audio_location;
			/* tmap role init */
			{
				p_lea_app_conf->tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_BMR;
			}
			//set GAP configuration
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_LEAUDIO | \
											  RTK_BT_PROFILE_GATTS | \
											  RTK_BT_PROFILE_A2DP | \
											  RTK_BT_PROFILE_AVRCP | \
											  RTK_BT_PROFILE_SDP;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			bt_app_conf.le_audio_app_conf = g_tmap_bmr_info.lea_app_conf;
			/* a2dp role init */
			a2dp_demo_role = RTK_BT_A2DP_ROLE_SRC;
			bt_app_conf.a2dp_role = a2dp_demo_role;

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
			BT_APP_PROCESS(rtk_sdp_record_add((void *)a2dp_src_sdp_record, sizeof(a2dp_src_sdp_record)));
			BT_APP_PROCESS(rtk_sdp_record_add((void *)did_sdp_record, sizeof(did_sdp_record)));
			BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_ct_sdp_record, sizeof(avrcp_ct_sdp_record)));

			/* a2dp codec add (must be invoked before rtk_bt_enable) */
			BT_APP_PROCESS(rtk_bt_a2dp_codec_cfg((uint8_t)RTK_BT_AUDIO_CODEC_SBC, (void *)&codec_sbc, (uint32_t)sizeof(rtk_bt_a2dp_media_codec_sbc_t)));

			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_LOGA("[APP] p_sink_pac_id = %d\r\n", *bt_app_conf.le_audio_app_conf.pacs_param.p_sink_pac_id);

			BT_APP_PROCESS(rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_VISIBLE_CONNECTABLE));

			BT_APP_PROCESS(rtk_bt_br_gap_get_bd_addr(&bd_addr));
			rtk_bt_br_addr_to_str(bd_addr.addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

			/* Initilize BR GAP part */
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

#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
			app_bt_le_audio_update_csis_sirk((void *)p_lea_app_conf,
											 g_tmap_bmr_info.sound_channel,
											 bd_addr.addr,
											 g_tmap_bmr_info.csis_neighbor_addr);
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

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
			app_bt_le_audio_vocs_set_param(g_tmap_bmr_info.sound_channel);
#endif
			/* Initilize LE GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_le_audio_gap_callback));
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(g_tmap_bmr_info.device_name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_HEADSET));

			uint8_t tmap_def_adv_data_len = LE_AUDIO_TMAP_EXT_ADV_DATA_DEFAULT_LEN;
			app_bt_le_audio_update_adv((void *)p_lea_app_conf, g_tmap_bmr_info.device_name, tmap_def_adv_data, &tmap_def_adv_data_len);

			/* intialize le audio part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_AUDIO, app_bt_le_audio_callback));

			app_bt_le_audio_tmap_broadcast_media_receiver_init(&g_tmap_bmr_info);

			/* pcm queue init */
			a2dp_tmap_demo_queue_init(&a2dp_src_pcm_data_queue, a2dp_src_queue, sizeof(a2dp_src_queue) / 2);
			a2dp_tx_water_level = A2DP_SOURCE_TX_WATER_LEVEL;
			BT_LOGA("[APP] A2DP source tx water level: %d ms, a2dp stream max credits: %d \r\n", a2dp_tx_water_level, src_a2dp_credits);
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
				if (false == osif_sem_take(bond_info_flush_task.sem, 0xffffffff)) {
					goto failed;
				}
			}
			g_tmap_bmr_info.status = RTK_BLE_AUDIO_ACCEPTOR_ENABLE;

			tmap_a2dp_demo_init_flag = true;
			break;
		}
		/* TMAP Unicast Media Receiver init */
		case RTK_BT_LE_AUDIO_TMAP_ROLE_UMR: {

			app_bt_le_audio_acceptor_info_t *p_tmap_umr_info = &g_tmap_umr_info;
			rtk_bt_le_audio_app_conf_t *p_lea_app_conf = &p_tmap_umr_info->lea_app_conf;
			app_bt_le_audio_bap_unicast_server_info_t *p_bap_uni_ser_info = p_tmap_umr_info->p_bap_uni_ser_info;

			rtk_bt_app_conf_t bt_app_conf = {0};
			app_lea_audio_cap_param_t cap_param_t = {0};
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
			cap_param_t.p_csis_sirk = app_lea_default_csis_sirk;
			cap_param_t.csis_sirk_len = RTK_BT_LE_CSIS_SIRK_LEN;
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
			cap_param_t.vocs_num = RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM;
			cap_param_t.p_vocs_feature_tbl = app_lea_vocs_features;
#endif
			//set lea_app_conf, must called before rtk_bt_enable
			app_bt_le_audio_lea_app_conf((void *)p_lea_app_conf,
										 p_tmap_umr_info->sound_channel,
										 RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR,
										 RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER,
										 app_tmap_pac_source_codec,
										 sizeof(app_tmap_pac_source_codec),
										 app_tmap_pac_sink_codec,
										 sizeof(app_tmap_pac_sink_codec),
										 &cap_param_t);
			p_bap_uni_ser_info->config_sink_ase_num = p_lea_app_conf->ascs_param.sink_ase_num;
			p_bap_uni_ser_info->config_source_ase_num = p_lea_app_conf->ascs_param.source_ase_num;
			BT_LOGA("[APP] config_sink_ase_num = %d, config_source_ase_num = %d\r\n", p_bap_uni_ser_info->config_sink_ase_num,
					p_bap_uni_ser_info->config_source_ase_num);
			/* tmap role init */
			{
				p_lea_app_conf->tmap_role = RTK_BT_LE_AUDIO_TMAP_ROLE_UMR;
			}
			//set GAP configuration
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTC | \
											  RTK_BT_PROFILE_GATTS | \
											  RTK_BT_PROFILE_LEAUDIO | \
											  RTK_BT_PROFILE_A2DP | \
											  RTK_BT_PROFILE_AVRCP | \
											  RTK_BT_PROFILE_SDP;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			bt_app_conf.le_audio_app_conf = p_tmap_umr_info->lea_app_conf;

			/* a2dp role init */
			a2dp_demo_role = RTK_BT_A2DP_ROLE_SRC;
			bt_app_conf.a2dp_role = a2dp_demo_role;

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
			BT_APP_PROCESS(rtk_sdp_record_add((void *)a2dp_src_sdp_record, sizeof(a2dp_src_sdp_record)));
			BT_APP_PROCESS(rtk_sdp_record_add((void *)did_sdp_record, sizeof(did_sdp_record)));

			BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_ct_sdp_record, sizeof(avrcp_ct_sdp_record)));

			/* a2dp codec add (must be invoked before rtk_bt_enable) */
			BT_APP_PROCESS(rtk_bt_a2dp_codec_cfg((uint8_t)RTK_BT_AUDIO_CODEC_SBC, (void *)&codec_sbc, (uint32_t)sizeof(rtk_bt_a2dp_media_codec_sbc_t)));

			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_LOGA("[APP] p_sink_pac_id = %d,p_source_pac_id = %d\r\n", *bt_app_conf.le_audio_app_conf.pacs_param.p_sink_pac_id,
					*bt_app_conf.le_audio_app_conf.pacs_param.p_source_pac_id);


			BT_APP_PROCESS(rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_VISIBLE_CONNECTABLE));

			BT_APP_PROCESS(rtk_bt_br_gap_get_bd_addr(&bd_addr));
			rtk_bt_br_addr_to_str(bd_addr.addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

			/* Initilize BR GAP part */
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

#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
			app_bt_le_audio_update_csis_sirk((void *)p_lea_app_conf,
											 p_tmap_umr_info->sound_channel,
											 bd_addr.addr,
											 p_tmap_umr_info->csis_neighbor_addr);
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

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
			app_bt_le_audio_vocs_set_param(g_tmap_umr_info.sound_channel);
#endif
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_le_audio_gap_callback));
			/* Initilize GATTS part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTS, app_bt_le_audio_tmap_umr_gatts_app_callback));
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(p_tmap_umr_info->device_name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_HEADSET));
			uint8_t tmap_def_adv_data_len = LE_AUDIO_TMAP_EXT_ADV_DATA_DEFAULT_LEN;
			app_bt_le_audio_update_adv((void *)p_lea_app_conf, p_tmap_umr_info->device_name, tmap_def_adv_data, &tmap_def_adv_data_len);
			//1. create ext adv
			BT_APP_PROCESS(rtk_bt_le_gap_create_ext_adv(&tmap_def_ext_adv_param, &tmap_ext_adv_handle));
			BT_LOGA("[APP] def_ext_adv_param.adv_handle = 0x%x\r\n", tmap_ext_adv_handle);
			//2. set ext adv data
			BT_APP_PROCESS(rtk_bt_le_gap_set_ext_adv_data(tmap_ext_adv_handle, (uint8_t *)tmap_def_adv_data, tmap_def_adv_data_len));
			//3. start ext adv
			BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(tmap_ext_adv_handle, 0, 0));
			/* register le audio event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_AUDIO, app_bt_le_audio_callback));

			app_bt_le_audio_tmap_unicast_media_receiver_init();

			/* pcm queue init */
			a2dp_tmap_demo_queue_init(&a2dp_src_pcm_data_queue, a2dp_src_queue, sizeof(a2dp_src_queue) / 2);
			a2dp_tx_water_level = A2DP_SOURCE_TX_WATER_LEVEL;
			BT_LOGA("[APP] A2DP source tx water level: %d ms, a2dp stream max credits: %d \r\n", a2dp_tx_water_level, src_a2dp_credits);
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
				if (false == osif_sem_take(bond_info_flush_task.sem, 0xffffffff)) {
					goto failed;
				}
			}
			p_tmap_umr_info->status = RTK_BLE_AUDIO_ACCEPTOR_ENABLE;

			tmap_a2dp_demo_init_flag = true;
			break;
		}

		default:
			BT_LOGE("[APP] unknown role 0x%x\r\n", role);
			break;

		}
	} else if (0 == enable) {
		if (!tmap_a2dp_demo_init_flag) {
			BT_LOGE("%s No need deinit! \r\n", __func__);
			return -1;
		}

		switch (role) {
		/* TMAP Broadcast Media Receiver deinit */
		case RTK_BT_LE_AUDIO_TMAP_ROLE_BMR: {
			if (g_tmap_bmr_info.status == RTK_BLE_AUDIO_ACCEPTOR_DISABLE) {
				BT_LOGE("[APP] %s: already disabled \r\n", __func__);
				return -1;
			}
			if (g_tmap_bmr_info.status == RTK_BLE_AUDIO_ACCEPTOR_BIG_SYNC) {
				BT_LOGE("[APP] %s: please terminate sync with broadcast stream first before disable \r\n", __func__);
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
			/* a2dp source demo send task delete */
			if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
				if (a2dp_send_task.hdl) {
					BT_LOGA("[A2DP] Delete a2dp send task \r\n");
					app_a2dp_src_send_flag = false;
					a2dp_send_task.run = 0;
					if (false == osif_sem_take(a2dp_send_task.sem, 0xffffffff)) {
						BT_LOGE("[A2DP] take a2dp_send_task.sem fail \r\n");
						return -1;
					}
					osif_sem_delete(a2dp_send_task.sem);
					memset((void *)&a2dp_send_task, 0, sizeof(struct tmap_a2dp_demo_task_t));
				}
			}

			app_bt_le_audio_tmap_broadcast_media_receiver_deinit(&g_tmap_bmr_info);
			a2dp_tmap_demo_queue_deinit(&a2dp_src_pcm_data_queue);

			BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_GAP));
			BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_SDP));
			BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_AVRCP));
			BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_A2DP));

			BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_GAP));
			BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_AUDIO));

			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			if (g_audio_resample_t) {
				BT_APP_PROCESS(rtk_bt_a2dp_tmap_demo_audio_resample_destroy(&g_audio_resample_t));
			}
			a2dp_demo_codec_entity = NULL;
			a2dp_demo_send_data_seq = 0;
			memset((void *)&audio_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
			g_tmap_bmr_info.status = RTK_BLE_AUDIO_ACCEPTOR_DISABLE;
			tmap_a2dp_demo_init_flag = false;
			break;
		}
		/* TMAP Unicast Media Receiver deinit */
		case RTK_BT_LE_AUDIO_TMAP_ROLE_UMR: {
			if (g_tmap_umr_info.status == RTK_BLE_AUDIO_ACCEPTOR_DISABLE) {
				BT_LOGE("[APP] %s: already disabled\r\n", __func__);
				return -1;
			}
			if (g_tmap_umr_info.status == RTK_BLE_AUDIO_ACCEPTOR_START) {
				BT_LOGE("[APP] %s: please stop unicast stream first before disable \r\n", __func__);
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
			/* a2dp source demo send task delete */
			if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
				if (a2dp_send_task.hdl) {
					BT_LOGA("[A2DP Demo] Delete Test Demo \r\n");
					app_a2dp_src_send_flag = false;
					a2dp_send_task.run = 0;
					if (false == osif_sem_take(a2dp_send_task.sem, 0xffffffff)) {
						BT_LOGE("[A2DP Demo] take a2dp_send_task.sem fail \r\n");
						return -1;
					}
					osif_sem_delete(a2dp_send_task.sem);
					memset((void *)&a2dp_send_task, 0, sizeof(struct tmap_a2dp_demo_task_t));
				}
			}

			// stop and remove ea
			rtk_bt_le_gap_stop_ext_adv(tmap_ext_adv_handle);

			app_bt_le_audio_tmap_unicast_media_receiver_deinit();
			a2dp_tmap_demo_queue_deinit(&a2dp_src_pcm_data_queue);

			BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_GAP));
			BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_SDP));
			BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_AVRCP));
			BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_A2DP));

			BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_GAP));
			BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_GATTS));
			BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_AUDIO));

			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			if (g_audio_resample_t) {
				BT_APP_PROCESS(rtk_bt_a2dp_tmap_demo_audio_resample_destroy(&g_audio_resample_t));
			}
			a2dp_demo_codec_entity = NULL;
			a2dp_demo_send_data_seq = 0;
			memset((void *)&audio_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
			g_tmap_umr_info.status = RTK_BLE_AUDIO_ACCEPTOR_DISABLE;
			tmap_a2dp_demo_init_flag = false;
			break;
		}

		default:
			BT_LOGE("[APP] unknown role 0x%x\r\n", role);
			break;

		}
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