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
/* br gap */
#include <rtk_bt_br_gap.h>
#include <rtk_bt_sdp.h>
#include <rtk_bt_avrcp.h>
#include <rtk_bt_a2dp.h>
#include <bt_audio_intf.h>
#include <bt_audio_track_api.h>
#include <app_audio_data.h>
#include <sbc_codec_entity.h>
// #include <aac_codec_entity.h>
/* le gap */
#include <rtk_bt_gap.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_gattc.h>
#include <rtk_service_config.h>
#include <rtk_bas.h>
#include <rtk_hrs.h>
#include <rtk_simple_ble_service.h>
#include <rtk_dis.h>
#include <rtk_ias.h>
#include <rtk_hids_kb.h>
#include <rtk_gls.h>
#include <rtk_long_uuid_service.h>
#include <rtk_client_config.h>
#include <rtk_gcs_client.h>
#include <rtk_bas_client.h>
#include <rtk_simple_ble_client.h>
#include <rtk_gaps_client.h>
#include "kv.h"
#include <dlist.h>
#include <bt_utils.h>

/* -------------------------------- Defines --------------------------------- */
#define RTK_BT_DEV_NAME                      "RTK_BT_CONCURRENT_DEV"
#define DEFAULT_PAGESCAN_WINDOW              0x48
#define DEFAULT_PAGESCAN_INTERVAL            0x800 //0x800
#define DEFAULT_PAGE_TIMEOUT                 0x2000
#define DEFAULT_INQUIRYSCAN_WINDOW           0x48
#define DEFAULT_INQUIRYSCAN_INTERVAL         0x800 //0x1000
#define DEFAULT_AUDIO_LEFT_VOLUME            0.8
#define DEFAULT_AUDIO_RIGHT_VOLUME           0.8
#define RTK_BT_A2DP_SRC_STREAM_MAX_CREDITS   2
#define RTK_BT_DEMO_RECONNECT_TIMER_INTERVAL 5000
#define RTK_BT_DEMO_RECONNECT_COUNT          3
/* ------------------------------- Company ID ------------------------------- */
#define RTK_COMPANY_ID 0x005D
/* ----------------------------- Avrcp Related ------------------------------ */
#define VOL_MAX     0x7f
#define VOL_MIN     0x00

/* ---------------------------------- A2DP -----------------------------------*/
/* ---------------------------- Global Variables ---------------------------- */
static uint8_t curr_volume = RTK_BT_DEFAULT_ABSOLUTE_VOLUME;

/* ---------------------------- Static Variables ---------------------------- */
static uint8_t bt_concurrent_demo_init_flag = 0;
static bool app_a2dp_src_send_flag = false;
static rtk_bt_audio_track_t *a2dp_demo_audio_track_hdl = NULL;
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
// static rtk_bt_aac_codec_t aac_codec_t = {
//              {
//                  .bit_rate = 128 * 1024,
//                  .channels = 2,
//                  .object_type = RTK_AAC_AOT_MP2_AAC_LC,
//                  .sample_freq = 44100,
//                  .vbr = 0,
//              },
//              {0},
// };

typedef struct {
	uint16_t type;
	uint16_t subtype;
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

static struct a2dp_demo_task_t a2dp_task = {
	.hdl = NULL,
	.sem = NULL,
	.run = 0,
};

static uint8_t remote_bd_addr[6] = {0};
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
#define A2DP_DEMO_BOND_INFO_PRIORITY_UNASSIGNED 0x4F
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
} a2dp_demo_bond_info_t;
static a2dp_demo_bond_info_t a2dp_demo_bond_table[A2DP_DEMO_MAX_BOND_NUM] = {0};
/* ------------------------------------ End -------------------------------------- */
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

// static rtk_bt_a2dp_media_codec_aac_t codec_aac = {
//  .object_type_mask = 0x80,
//  .sampling_frequency_mask = 0x0180,
//  .channel_number_mask = 0x0c,
//  .vbr_supported = 1,
//  .bit_rate = 0,
// };
/* ---------------------------------- a2dp(end) -----------------------------------*/

/* ---------------------------------- Scatternet -----------------------------------*/
#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
static uint8_t ext_adv_data[] = {
	// Flags
	0x02,
	RTK_BT_LE_GAP_ADTYPE_FLAGS,
	RTK_BT_LE_GAP_ADTYPE_FLAGS_LIMITED | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
	// Local name
	0x12,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'B', 'T', '_', 'S', 'C', 'A', 'T', 'T', 'E', 'R', 'N', 'E', 'T',
	// Manufacturer Specific Data
	0xc3,
	RTK_BT_LE_GAP_ADTYPE_MANUFACTURER_SPECIFIC,
	0xc0, 0x00,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
	0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
	0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,
	0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5,
	0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6,
	0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
	0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
	0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9,
	0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa,
	0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb,
};

static rtk_bt_le_ext_adv_param_t ext_adv_param = {
	.adv_event_prop = RTK_BT_LE_EXT_ADV_EXTENDED_ADV_CONN_UNDIRECTED,
	.primary_adv_interval_min = 320,
	.primary_adv_interval_max = 320,
	.primary_adv_channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.own_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},
	.peer_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},//;{0x8A, 0xAA, 0xAA, 0x4C, 0xE0, 0x00},
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
	.tx_power = 0x7F,
	.primary_adv_phy = RTK_BT_LE_PHYS_PRIM_ADV_1M,
	.secondary_adv_max_skip = 0,
	.secondary_adv_phy = RTK_BT_LE_PHYS_2M,
	.adv_sid = 0,
};
#else
static uint8_t adv_data[] = {
	0x02, //AD len
	RTK_BT_LE_GAP_ADTYPE_FLAGS, //AD types
	RTK_BT_LE_GAP_ADTYPE_FLAGS_GENERAL | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, //AD data
	0x12,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'B', 'T', '_', 'S', 'C', 'A', 'T', 'T', 'E', 'R', 'N', 'E', 'T',
};

static uint8_t scan_rsp_data[] = {
	0x3,
	RTK_BT_LE_GAP_ADTYPE_APPEARANCE, //GAP_ADTYPE_APPEARANCE
	LO_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
	HI_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
};

static rtk_bt_le_adv_param_t def_adv_param = {
	.interval_min = 200, //units of 0.625ms
	.interval_max = 250,
	.type = RTK_BT_LE_ADV_TYPE_IND,
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.peer_addr = {
		.type = (rtk_bt_le_addr_type_t)0,
		.addr_val = {0},
	},
	.channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};
#endif

static rtk_bt_le_scan_param_t scan_param = {
	.type          = RTK_BT_LE_SCAN_TYPE_ACTIVE,
	.interval      = 0x60,
	.window        = 0x30,
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.filter_policy = RTK_BT_LE_SCAN_FILTER_ALLOW_ALL,
	.duplicate_opt = 0,
};

static rtk_bt_le_security_param_t sec_param = {
	.io_cap = RTK_IO_CAP_NO_IN_NO_OUT,
	.oob_data_flag = 0,
	.bond_flag = 1,
	.mitm_flag = 0,
	.sec_pair_flag = 0,
	.use_fixed_key = 0,
	.fixed_key = 000000,
};

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
#define PRIVACY_USE_DIR_ADV_WHEN_BONDED    0
static bool privacy_enable = false;
static bool privacy_whitelist = true;
static uint8_t privacy_irk[RTK_BT_LE_GAP_IRK_LEN] = "abcdef0123456789";
#endif

#if defined(RTK_BT_POWER_CONTROL_SUPPORT) && RTK_BT_POWER_CONTROL_SUPPORT
#define BT_POWER_TEST_MODE         0
#if defined(BT_POWER_TEST_MODE) && BT_POWER_TEST_MODE
#include "rtk_bt_power_control.h"

#define BT_POWER_TEST_WAKE_TIME    5    //Unit:s

static void *bt_power_test_wake_timer_hdl = NULL;

static void bt_power_test_wake_timeout_handler(void *arg)
{
	(void)arg;
	rtk_bt_release_wakelock();
}

static void bt_power_test_suspend(void)
{
	BT_LOGA("[BT_PS] Enter bt_power_test_suspend\r\n");
}

static void bt_power_test_resume(void)
{
	BT_LOGA("[BT_PS] Enter bt_power_test_resume\r\n");

	if (BT_POWER_TEST_WAKE_TIME != 0) {
		osif_timer_restart(&bt_power_test_wake_timer_hdl, BT_POWER_TEST_WAKE_TIME * 1000);
	} else {
		rtk_bt_release_wakelock();
	}
}

static void bt_power_test_init(void)
{
	if (BT_POWER_TEST_WAKE_TIME != 0) {
		osif_timer_create(&bt_power_test_wake_timer_hdl, "bt_power_test_wake_timer", NULL, BT_POWER_TEST_WAKE_TIME * 1000, false,
						  bt_power_test_wake_timeout_handler);
		if (bt_power_test_wake_timer_hdl == NULL) {
			BT_LOGE("[BT_PS] bt_power_test_wake_timer create failed!\r\n");
			return;
		}
	}

	rtk_bt_power_save_init((rtk_bt_ps_callback)bt_power_test_suspend, (rtk_bt_ps_callback)bt_power_test_resume);
}

static void bt_power_test_deinit(void)
{
	rtk_bt_power_save_deinit();

	if (BT_POWER_TEST_WAKE_TIME != 0) {
		if (bt_power_test_wake_timer_hdl) {
			osif_timer_delete(&bt_power_test_wake_timer_hdl);
			bt_power_test_wake_timer_hdl = NULL;
		}
	}
}
#endif
#endif

static void app_server_disconnect(uint16_t conn_handle)
{
	simple_ble_srv_disconnect(conn_handle);
	bas_disconnect(conn_handle);
	hrs_disconnect(conn_handle);
	gls_disconnect(conn_handle);
}

static void app_server_deinit(void)
{
	simple_ble_srv_status_deinit();
	bas_status_deinit();
	hrs_status_deinit();
	gls_status_deinit();
}

static rtk_bt_evt_cb_ret_t scatternet_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
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

static rtk_bt_evt_cb_ret_t ble_scatternet_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	char le_addr[30] = {0};
	char *role;

	switch (evt_code) {
	case RTK_BT_LE_GAP_EVT_ADV_START_IND: {
		rtk_bt_le_adv_start_ind_t *adv_start_ind = (rtk_bt_le_adv_start_ind_t *)param;
		if (!adv_start_ind->err) {
			BT_LOGA("[APP] ADV started: adv_type %d  \r\n", adv_start_ind->adv_type);
		} else {
			BT_LOGE("[APP] ADV start failed, err 0x%x \r\n", adv_start_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:adv,start,%d,%d\r\n", (adv_start_ind->err == 0) ? 0 : -1, adv_start_ind->adv_type);
		break;
	}

	case RTK_BT_LE_GAP_EVT_ADV_STOP_IND: {
		rtk_bt_le_adv_stop_ind_t *adv_stop_ind = (rtk_bt_le_adv_stop_ind_t *)param;
		if (!adv_stop_ind->err) {
			BT_LOGA("[APP] ADV stopped: reason 0x%x \r\n", adv_stop_ind->stop_reason);
		} else {
			BT_LOGE("[APP] ADV stop failed, err 0x%x \r\n", adv_stop_ind->err);
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

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
	case RTK_BT_LE_GAP_EVT_PA_IND: {
		rtk_bt_le_pa_ind_t *pa_ind = (rtk_bt_le_pa_ind_t *)param;
		if (pa_ind->state == RTK_BT_LE_PA_STATE_IDLE) {
			if (pa_ind->cause) {
				BT_LOGE("[APP] Periodic ADV(%d) stopped failed, err 0x%x\r\n", pa_ind->adv_handle, pa_ind->cause);
			} else {
				BT_LOGA("[APP] Periodic ADV(%d) stopped\r\n", pa_ind->adv_handle);
			}
		} else if (pa_ind->state == RTK_BT_LE_PA_STATE_ADVERTISING) {
			if (pa_ind->cause) {
				BT_LOGE("[APP] Periodic ADV(%d) started failed, err 0x%x\r\n", pa_ind->adv_handle, pa_ind->cause);
			} else {
				BT_LOGA("[APP] Periodic ADV(%d) started\r\n", pa_ind->adv_handle);
			}
		} else if (pa_ind->state == RTK_BT_LE_PA_STATE_WAIT_EXT_ADV_ADVERTISING) {
			BT_LOGE("[APP] Periodic ADV(%d) waiting for ext adv start, cause 0x%x\r\n", pa_ind->adv_handle, pa_ind->cause);
		}
		BT_AT_PRINT("+BLEGAP:pa_adv,%s,%d,%d\r\n",
					((pa_ind->state == RTK_BT_LE_PA_STATE_IDLE) ? "stop" : ((pa_ind->state == RTK_BT_LE_PA_STATE_ADVERTISING) ? "start" : "wait_eadv")),
					((pa_ind->state == RTK_BT_LE_PA_STATE_WAIT_EXT_ADV_ADVERTISING) ? 0 : ((pa_ind->cause == 0) ? 0 : -1)),
					pa_ind->adv_handle);
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
		BT_LOGA("[APP] Ext Scan info, [Device]: %s, AD evt type: 0x%x, RSSI: %i, PHY: 0x%x, TxPower: %d, Len: %d\r\n",
				le_addr, scan_res_ind->evt_type, scan_res_ind->rssi,
				(scan_res_ind->primary_phy << 4) | scan_res_ind->secondary_phy,
				scan_res_ind->tx_power, scan_res_ind->len);
		BT_AT_PRINT("+BLEGAP:escan,%s,0x%x,%i,0x%x,%d,%d\r\n",
					le_addr, scan_res_ind->evt_type, scan_res_ind->rssi,
					(scan_res_ind->primary_phy << 4) | scan_res_ind->secondary_phy,
					scan_res_ind->tx_power, scan_res_ind->len);
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
					conn_ind->conn_handle, role, le_addr);
#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
			/* gattc action */
			if (RTK_BT_LE_ROLE_MASTER == conn_ind->role &&
				RTK_BT_OK == general_client_attach_conn(conn_ind->conn_handle) &&
				RTK_BT_OK == bas_client_attach_conn(conn_ind->conn_handle) &&
				RTK_BT_OK == gaps_client_attach_conn(conn_ind->conn_handle) &&
				RTK_BT_OK == simple_ble_client_attach_conn(conn_ind->conn_handle)) {
				BT_LOGA("[APP] GATTC Profiles attach connection success, conn_handle: %d\r\n",
						conn_ind->conn_handle);
			}
#endif
			/* central action */
			if (RTK_BT_LE_ROLE_MASTER == conn_ind->role &&
				rtk_bt_le_sm_is_device_bonded(&conn_ind->peer_addr)) {
				BT_LOGA("[APP] Bonded device, start link encryption procedure\r\n");
				rtk_bt_le_sm_start_security(conn_ind->conn_handle);
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

		if (RTK_BT_LE_ROLE_SLAVE == disconn_ind->role) {
			/* gap action */
#if !defined(RTK_BLE_5_0_AE_ADV_SUPPORT) || !RTK_BLE_5_0_AE_ADV_SUPPORT
			rtk_bt_le_gap_dev_state_t dev_state;
			rtk_bt_le_adv_param_t adv_param = {0};
			if (rtk_bt_le_gap_get_dev_state(&dev_state) == RTK_BT_OK &&
				dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_IDLE) {
				memcpy(&adv_param, &def_adv_param, sizeof(rtk_bt_le_adv_param_t));
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
				if (privacy_enable) {
					uint8_t bond_size = 0;
					adv_param.own_addr_type = 2;
					BT_APP_PROCESS(rtk_bt_le_sm_get_bond_num(&bond_size));
					if (bond_size != 0) {
#if defined(PRIVACY_USE_DIR_ADV_WHEN_BONDED) && PRIVACY_USE_DIR_ADV_WHEN_BONDED
						rtk_bt_le_bond_info_t bond_info = {0};
						uint8_t bond_num = 1;
						rtk_bt_le_sm_get_bond_info(&bond_info, &bond_num);
						adv_param.type = RTK_BT_LE_ADV_TYPE_DIRECT_IND_LOW;
						adv_param.peer_addr.type = 0;
						memcpy(adv_param.peer_addr.addr_val, bond_info.ident_addr.addr_val, RTK_BD_ADDR_LEN);
						adv_param.own_addr_type = 2;
#endif /* PRIVACY_USE_DIR_ADV_WHEN_BONDED */
						if (privacy_whitelist) {
							adv_param.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST;
						} else {
							adv_param.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
						}
					}
				}
#endif /* RTK_BLE_PRIVACY_SUPPORT */
				BT_LOGA("[APP] Reconnect ADV starting, adv type:%d,  own_addr_type: %d, filter_policy: %d\r\n"
						, adv_param.type,  adv_param.own_addr_type, adv_param.filter_policy);
				BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&adv_param));
			}
#endif /* RTK_BLE_5_0_AE_ADV_SUPPORT */
			/* gatts action */
			app_server_disconnect(disconn_ind->conn_handle);
		}
		/* gattc action */
		if (RTK_BT_LE_ROLE_MASTER == disconn_ind->role &&
			RTK_BT_OK == general_client_detach_conn(disconn_ind->conn_handle) &&
			RTK_BT_OK == bas_client_detach_conn(disconn_ind->conn_handle) &&
			RTK_BT_OK == gaps_client_detach_conn(disconn_ind->conn_handle) &&
			RTK_BT_OK == simple_ble_client_detach_conn(disconn_ind->conn_handle)) {
			BT_LOGA("[APP] GATTC Profiles detach connection success, conn_handle: %d\r\n",
					disconn_ind->conn_handle);
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

	case RTK_BT_LE_GAP_EVT_REMOTE_CONN_UPDATE_REQ_IND: { //BT api shall not be called here
		rtk_bt_le_remote_conn_update_req_ind_t *rmt_update_req =
			(rtk_bt_le_remote_conn_update_req_ind_t *)param;
		BT_LOGA("[APP] Remote device request a change in conn param, conn_handle: %d, "      \
				"conn_interval_max: 0x%x, conn_interval_min: 0x%x, conn_latency: 0x%x, "      \
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
		rtk_bt_le_auth_pair_cfm_ind_t *pair_cfm_ind =
			(rtk_bt_le_auth_pair_cfm_ind_t *)param;
		BT_LOGA("[APP] Just work pairing need user to confirm, conn_handle: %d!\r\n",
				pair_cfm_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:pair_cfm,%d\r\n", pair_cfm_ind->conn_handle);
		rtk_bt_le_pair_cfm_t pair_cfm_param = {0};
		uint16_t ret = 0;
		pair_cfm_param.conn_handle = pair_cfm_ind->conn_handle;
		pair_cfm_param.confirm = 1;
		ret = rtk_bt_le_sm_pairing_confirm(&pair_cfm_param);
		if (RTK_BT_OK == ret) {
			BT_LOGA("[APP] Just work pairing auto confirm succcess\r\n");
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_DISPLAY_IND: {
		rtk_bt_le_auth_key_display_ind_t *key_dis_ind =
			(rtk_bt_le_auth_key_display_ind_t *)param;
		BT_LOGA("[APP] Auth passkey display: %d, conn_handle:%d\r\n",
				key_dis_ind->passkey,
				key_dis_ind->conn_handle);
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
				key_cfm_ind->passkey,
				key_cfm_ind->conn_handle);
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
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_BOND_MODIFY_IND: {
		rtk_bt_le_bond_modify_ind_t *bond_mdf_ind =
			(rtk_bt_le_bond_modify_ind_t *)param;
		char ident_addr[30] = {0};
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->remote_addr), le_addr, sizeof(le_addr));
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->ident_addr), ident_addr, sizeof(ident_addr));
		BT_LOGA("[APP] Bond info modified, op: %d, addr: %s, ident_addr: %s\r\n",
				bond_mdf_ind->op, le_addr, ident_addr);
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

	case RTK_BT_LE_GAP_EVT_RESOLV_LIST_PENDING_IND: {
		rtk_bt_le_resolv_list_pending_ind_t *p_ind = (rtk_bt_le_resolv_list_pending_ind_t *)param;
		BT_LOGA("[APP] WARN: Resolving list modification is pending because of adv(%d) scan(%d) connect(%d)!!!\r\n",
				(p_ind->reason & RTK_BT_LE_RESOLV_LIST_PEND_BY_ADV) ? 1 : 0,
				(p_ind->reason & RTK_BT_LE_RESOLV_LIST_PEND_BY_SCAN) ? 1 : 0,
				(p_ind->reason & RTK_BT_LE_RESOLV_LIST_PEND_BY_CONNECT) ? 1 : 0);
		BT_AT_PRINT("+BLEGAP:resolv_list_pending,%d,%d,%d\r\n",
					(p_ind->reason & RTK_BT_LE_RESOLV_LIST_PEND_BY_ADV) ? 1 : 0,
					(p_ind->reason & RTK_BT_LE_RESOLV_LIST_PEND_BY_SCAN) ? 1 : 0,
					(p_ind->reason & RTK_BT_LE_RESOLV_LIST_PEND_BY_CONNECT) ? 1 : 0);
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

static uint16_t app_get_gatts_app_id(uint8_t event, void *data)
{
	uint16_t app_id = 0xFFFF;

	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *p_reg_ind = (rtk_bt_gatts_reg_ind_t *)data;
		app_id = p_reg_ind->app_id;
		break;
	}
	case RTK_BT_GATTS_EVT_READ_IND: {
		rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
		app_id = p_read_ind->app_id;
		break;
	}
	case RTK_BT_GATTS_EVT_WRITE_IND: {
		rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
		app_id = p_write_ind->app_id;
		break;
	}
	case RTK_BT_GATTS_EVT_CCCD_IND: {
		rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;
		app_id = p_cccd_ind->app_id;
		break;
	}
	case RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND:
	case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ind_ntf = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		app_id = p_ind_ntf->app_id;
		break;
	}
	default:
		break;
	}

	return app_id;
}

static rtk_bt_evt_cb_ret_t ble_scatternet_gatts_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;
	uint16_t app_id = 0xFFFF;

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

	app_id = app_get_gatts_app_id(event, data);
	switch (app_id) {
	case SIMPLE_BLE_SRV_ID:
		simple_ble_service_callback(event, data);
		break;
	case DEVICE_INFO_SRV_ID:
		device_info_srv_callback(event, data);
		break;
	case HEART_RATE_SRV_ID:
		heart_rate_srv_callback(event, data);
		break;
	case BATTERY_SRV_ID:
		battery_service_callback(event, data);
		break;
	case IMMEDIATE_ALERT_SRV_ID:
		immediate_alert_srv_callback(event, data);
		break;
	case GLUCOSE_SRV_ID:
		glucose_srv_callback(event, data);
		break;
	case HID_SRV_ID:
		hid_srv_callback(event, data);
		break;
	case LONG_UUID_SRV_ID:
		long_uuid_service_callback(event, data);
		break;
	default:
		break;
	}
	return RTK_BT_EVT_CB_OK;
}

static uint16_t app_get_gattc_profile_id(uint8_t event, void *data)
{
	uint16_t profile_id = 0xFFFF;

	switch (event) {
	case RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND: {
		rtk_bt_gattc_discover_ind_t *disc_res = (rtk_bt_gattc_discover_ind_t *)data;
		profile_id = disc_res->profile_id;
		break;
	}
	case RTK_BT_GATTC_EVT_READ_RESULT_IND: {
		rtk_bt_gattc_read_ind_t *read_res = (rtk_bt_gattc_read_ind_t *)data;
		profile_id = read_res->profile_id;
		break;
	}
	case RTK_BT_GATTC_EVT_WRITE_RESULT_IND: {
		rtk_bt_gattc_write_ind_t *write_res = (rtk_bt_gattc_write_ind_t *)data;
		profile_id = write_res->profile_id;
		break;
	}
	case RTK_BT_GATTC_EVT_NOTIFY_IND:
	case RTK_BT_GATTC_EVT_INDICATE_IND: {
		rtk_bt_gattc_cccd_value_ind_t *cccd_ind = (rtk_bt_gattc_cccd_value_ind_t *)data;
		profile_id = cccd_ind->profile_id;
		break;
	}
	case RTK_BT_GATTC_EVT_CCCD_ENABLE_IND:
	case RTK_BT_GATTC_EVT_CCCD_DISABLE_IND: {
		rtk_bt_gattc_cccd_update_ind_t *cccd_update = (rtk_bt_gattc_cccd_update_ind_t *)data;
		profile_id = cccd_update->profile_id;
		break;
	}
	default:
		break;
	}

	return profile_id;
}

static rtk_bt_evt_cb_ret_t ble_scatternet_gattc_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;
	uint16_t profile_id = 0xFFFF;

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

#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	if (RTK_BT_GATTC_EVT_DISCOVER_ALL_STATE_IND == event) {
		rtk_bt_gattc_discover_all_state_ind_t *p_ind = (rtk_bt_gattc_discover_all_state_ind_t *)data;
		BT_LOGA("[APP] GATTC discover all finished: conn_handle: %d, is_success: %d, load_from_storage: %d\r\n",
				p_ind->conn_handle, p_ind->is_success, p_ind->load_from_storage);

		return RTK_BT_EVT_CB_OK;
	}

	if (RTK_BT_GATTC_EVT_GATT_SERVICE_INFO_IND == event) {
		rtk_bt_gattc_gatt_service_info_ind_t *p_ind = (rtk_bt_gattc_gatt_service_info_ind_t *)data;
		BT_LOGA("[APP] GATTC discover GATT service info: conn_handle: %d, is_found: %d, load_from_storage: %d\r\n",
				p_ind->conn_handle, p_ind->is_found, p_ind->load_from_storage);
		if ((p_ind->char_flag & RTK_BT_GATT_SVC_SERVER_SUPPORTED_FEATURES_FLAG) &&
			(p_ind->server_features[0] & RTK_BT_GATTS_SERVER_SUPPORTED_FEATURES_EATT_BIT)) {
			BT_LOGA("[APP] Server supported features: 0x%02x, support EATT.\r\n", p_ind->server_features[0]);
		}

		return RTK_BT_EVT_CB_OK;
	}
#endif

	profile_id = app_get_gattc_profile_id(event, data);
	switch (profile_id) {
	case GCS_CLIENT_PROFILE_ID:
		general_client_app_callback(event, data);
		break;
	case GAPS_CLIENT_PROFILE_ID:
		gaps_client_app_callback(event, data);
		break;
	case BAS_CLIENT_PROFILE_ID:
		bas_client_app_callback(event, data);
		break;
	case SIMPLE_BLE_CLIENT_PROFILE_ID:
		simple_ble_client_app_callback(event, data);
		break;
	default:
		break;
	}

	return RTK_BT_EVT_CB_OK;
}
/* ---------------------------------- Scatternet(end) -----------------------------------*/

#if defined(CONFIG_BT_AUDIO_SOURCE_OUTBAND) && CONFIG_BT_AUDIO_SOURCE_OUTBAND
static int16_t pcm_buffer[512] = {0};
static uint16_t a2dp_demo_send_data_seq = 0;

static void app_a2dp_src_send_data(void)
{
	rtk_bt_a2dp_stream_data_send_t data_send_t = {0};
	struct enc_codec_buffer *penc_codec_buffer_t = NULL;

	if (src_a2dp_credits) {
		if (demo_uart_read((uint8_t *)pcm_buffer)) {
			// BT_LOGA("[A2DP SRC Demo]: uart buffer read success \r\n");
			penc_codec_buffer_t = rtk_bt_audio_data_encode(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_codec_entity, (int16_t *)pcm_buffer, (uint32_t)1024);
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
				BT_LOGE("[A2DP SRC Demo]: Encode fail \r\n");
			}
		} else {
			// BT_LOGE("[A2DP SRC Demo]: uart buffer length is not enough \r\n");
		}
	} else {
		// BT_LOGE("[A2DP] waiting src_a2dp_credits \r\n");
	}
}
#else
static uint32_t pcm_offset = 0;
static uint16_t a2dp_demo_send_data_seq = 0;

static void app_a2dp_src_send_data(void)
{
	rtk_bt_a2dp_stream_data_send_t data_send_t = {0};
	struct enc_codec_buffer *penc_codec_buffer_t = NULL;
	short *pdata = NULL;

	if (src_a2dp_credits) {
		if (pcm_offset < (birds_sing_size / 2)) {
			if (pcm_offset + 512 < birds_sing_size / 2) {
				pdata = (short *)(birds_sing + pcm_offset);
				penc_codec_buffer_t = rtk_bt_audio_data_encode(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_codec_entity, pdata, 1024);
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
					BT_LOGE("[A2DP SRC Demo]: Encode fail \r\n");
				}
			} else {
				pdata = (short *)(birds_sing + pcm_offset);
				penc_codec_buffer_t = rtk_bt_audio_data_encode(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_codec_entity, pdata, ((birds_sing_size / 2) - pcm_offset) * 2);
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
					BT_LOGE("[A2DP SRC Demo]: Encode fail \r\n");
				}
			}
			pcm_offset += 512;
		} else {
			pcm_offset = 0;
		}
	} else {
		// BT_LOGE("[A2DP] waiting src_a2dp_credits \r\n");
	}
}
#endif

static void a2dp_task_entry(void *ctx)
{
	(void)ctx;
	osif_sem_give(a2dp_task.sem);

	while (a2dp_task.run) {
		if (app_a2dp_src_send_flag) {
			app_a2dp_src_send_data();
		}
		osif_delay(3);
	}

	osif_sem_give(a2dp_task.sem);
	osif_task_delete(NULL);
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
				if (a2dp_task.run) {
					app_a2dp_src_send_flag = true;
				}
			}
			break;
		}

		case RTK_BT_AVRCP_STATUS_PAUSED: {
			BT_LOGA("[AVRCP]: Paused \r\n");
			if (a2dp_demo_role == RTK_BT_A2DP_ROLE_SRC) {
				if (a2dp_task.run) {
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
		if (a2dp_task.run) {
			app_a2dp_src_send_flag = true;
		}
		break;
	}

	case RTK_BT_AVRCP_EVT_PAUSE_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Pause req \r\n");
		if (a2dp_task.run) {
			app_a2dp_src_send_flag = false;
		}
		break;
	}

	case RTK_BT_AVRCP_EVT_STOP_REQ_EVENT: {
		BT_LOGA("[AVRCP]: Stop req \r\n");
		if (a2dp_task.run) {
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
	psbc_encoder_t->sbc_enc_mode = (sbc_channel_mode_t)SBC_MODE_STANDARD;

	return 0;
}

// static uint16_t rtk_bt_a2dp_aac_parse_decoder_struct(rtk_bt_a2dp_codec_t *pa2dp_codec, rtk_bt_aac_decode_t *paac_decoder_t)
// {
//  if (pa2dp_codec->aac.channel_number == RTK_BT_A2DP_AAC_CHANNEL_NUMBER_1) {
//      paac_decoder_t->channels = 1;
//  } else {
//      paac_decoder_t->channels = 2;
//  }
//  switch (pa2dp_codec->aac.sampling_frequency) {
//      case RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_8KHZ:
//          paac_decoder_t->sample_freq = 8000;
//          break;
//      case RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_16KHZ:
//          paac_decoder_t->sample_freq = 16000;
//          break;
//      case RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_32KHZ:
//          paac_decoder_t->sample_freq = 32000;
//          break;
//      case RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_44_1KHZ:
//          paac_decoder_t->sample_freq = 44100;
//          break;
//      case RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_48KHZ:
//          paac_decoder_t->sample_freq = 48000;
//          break;
//      default:
//          BT_LOGE("[A2DP] RTK Audio not support frequency 11.025, 12, 22.05, 24, 64, 88.2, 96KHz \r\n");
//          return 1;
//  }
//  switch (pa2dp_codec->aac.object_type) {
//      case RTK_BT_A2DP_AAC_OBJECT_TYPE_MPEG_2_AAC_LC:
//          paac_decoder_t->object_type = RTK_AAC_AOT_MP2_AAC_LC;
//          break;

//      case RTK_BT_A2DP_AAC_OBJECT_TYPE_MPEG_4_AAC_LC:
//          paac_decoder_t->object_type = RTK_AAC_AOT_AAC_LC;
//          break;

//      case RTK_BT_A2DP_AAC_OBJECT_TYPE_MPEG_4_AAC_LTP:
//          paac_decoder_t->object_type = RTK_AAC_AOT_AAC_LTP;
//          break;

//      case RTK_BT_A2DP_AAC_OBJECT_TYPE_MPEG_4_AAC_SCALABLE:
//          paac_decoder_t->object_type = RTK_AAC_AOT_AAC_SCAL;
//          break;

//      default:
//          BT_LOGE("[A2DP] RTK Audio only support object type MP2_AAC_LC/MP4_AAC_LC/MP4_AAC_LTP/MP4_AAC_SCALABLE \r\n");
//          return 1;
//  }
//  paac_decoder_t->bit_rate = pa2dp_codec->aac.bit_rate;
//  paac_decoder_t->vbr = pa2dp_codec->aac.vbr_supported;
//  rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_AAC, (float)DEFAULT_AUDIO_LEFT_VOLUME, (float)DEFAULT_AUDIO_RIGHT_VOLUME, channels,
//                          psbc_decoder_t->sampling_frequency, BT_AUDIO_FORMAT_PCM_16_BIT, 0, NULL, true);

//  return 0;
// }

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
			if (a2dp_task.hdl) {
				BT_LOGA("[A2DP Demo] Delete Test Demo \r\n");
				app_a2dp_src_send_flag = false;
				a2dp_task.run = 0;
				if (false == osif_sem_take(a2dp_task.sem, 0xffffffff)) {
					BT_LOGE("[A2DP Demo] take a2dp_task.sem fail \r\n");
					break;
				}
				osif_sem_delete(a2dp_task.sem);
				memset((void *)&a2dp_task, 0, sizeof(struct a2dp_demo_task_t));
			}
		}
		rtk_bt_audio_codec_remove(audio_codec_conf.codec_index, a2dp_demo_codec_entity);
		memset((void *)&audio_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
		rtk_bt_audio_deinit();
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
		} else {
			if (pa2dp_codec->codec_type == RTK_BT_AUDIO_CODEC_SBC) {
				ret = rtk_bt_a2dp_sbc_parse_encoder_struct(pa2dp_codec, &sbc_codec_t.encoder_t);
				audio_codec_conf.codec_index = RTK_BT_AUDIO_CODEC_SBC;
				audio_codec_conf.param = (void *)&sbc_codec_t;
				audio_codec_conf.param_len = sizeof(sbc_codec_t);
			}
		}
		a2dp_demo_codec_entity = rtk_bt_audio_codec_add(&audio_codec_conf);
		// bt_a2dp_sink_event_handle(A2DP_SINK_STREAM_DATA_HANDLE_INIT, A2DP_SBC_CODEC, 0, NULL);
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
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_START_RSP: {
		rtk_bt_a2dp_stream_start_t *pa2dp_stream = (rtk_bt_a2dp_stream_start_t *)param;

		BT_LOGA("[A2DP] BT_EVENT_A2DP_STREAM_START_IND active_a2dp_idx %d, streaming_fg %d \r\n",
				pa2dp_stream->active_a2dp_link_index, pa2dp_stream->stream_cfg);

		{
			if (a2dp_task.hdl) {
				BT_LOGA("[A2DP Demo] Start Test Demo \r\n");
			} else {
				BT_LOGA("[A2DP Demo] Create Test Demo \r\n");
				if (false == osif_sem_create(&a2dp_task.sem, 0, 1)) {
					BT_LOGE("[A2DP Demo] Create a2dp_task.sem fail \r\n");
					break;
				}
				a2dp_task.run = 1;
				if (false == osif_task_create(&a2dp_task.hdl, "a2dp_task",
											  a2dp_task_entry, NULL,
											  2048, 4)) {
					osif_sem_delete(a2dp_task.sem);
					a2dp_task.run = 0;
					BT_LOGE("[A2DP Demo] Create a2dp_task fail \r\n");
					break;
				}
				osif_sem_take(a2dp_task.sem, 0xffffffff);
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
		if (a2dp_demo_audio_track_hdl) {
			rtk_bt_audio_track_pause(a2dp_demo_audio_track_hdl->audio_track_hdl);
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
 * @brief  process a2dp main init/ deinit.
 * @param  enable[in]: 1: init. 0 deinit
 * @return none.
 */
int bt_a2dp_scatternet_main(uint8_t role, uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_br_bd_addr_t bd_addr = {0};
	bool adv_filter_whitelist = false;
	char addr_str[30] = {0};
	char dev_name[30] = {0};
#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
	uint8_t adv_handle;
#else
	rtk_bt_le_adv_param_t adv_param = {0};
#endif
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
	uint8_t bond_size = 0;
#endif

	if (1 == enable) {
		if (bt_concurrent_demo_init_flag) {
			BT_LOGE("%s Already init! \r\n", __func__);
			return -1;
		}

		//set GAP configuration
		bt_app_conf.app_profile_support =   RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_GATTC | \
											RTK_BT_PROFILE_A2DP | \
											RTK_BT_PROFILE_AVRCP | \
											RTK_BT_PROFILE_SDP;
		bt_app_conf.mtu_size = 180;
		bt_app_conf.master_init_mtu_req = true;
		bt_app_conf.slave_init_mtu_req = false;
		bt_app_conf.prefer_all_phy = 0;
		bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
		bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
		bt_app_conf.max_tx_octets = 0x40;
		bt_app_conf.max_tx_time = 0x200;
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
		memcpy(bt_app_conf.irk, privacy_irk, RTK_BT_LE_GAP_IRK_LEN);
#endif
		bt_app_conf.user_def_service = false;
		bt_app_conf.cccd_not_check = false;
		bt_app_conf.a2dp_role = role;

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
		if (RTK_BT_A2DP_ROLE_SNK == role) {
			BT_APP_PROCESS(rtk_sdp_record_add((void *)a2dp_sink_sdp_record, sizeof(a2dp_sink_sdp_record)));
			a2dp_demo_role = RTK_BT_A2DP_ROLE_SNK;
		} else if (RTK_BT_A2DP_ROLE_SRC == role) {
			BT_APP_PROCESS(rtk_sdp_record_add((void *)a2dp_src_sdp_record, sizeof(a2dp_src_sdp_record)));
			a2dp_demo_role = RTK_BT_A2DP_ROLE_SRC;
		} else {
			BT_LOGE("[APP] Wrong A2DP role %d \r\n", role);
			return 1;
		}
		BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_ct_sdp_record, sizeof(avrcp_ct_sdp_record)));
		BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_tg_sdp_record, sizeof(avrcp_tg_sdp_record)));

		/* a2dp codec add (must be invoked before rtk_bt_enable) */
		BT_APP_PROCESS(rtk_bt_a2dp_codec_cfg((uint8_t)RTK_BT_AUDIO_CODEC_SBC, (void *)&codec_sbc, (uint32_t)sizeof(rtk_bt_a2dp_media_codec_sbc_t)));
		// BT_APP_PROCESS(rtk_bt_a2dp_codec_cfg((uint8_t)RTK_BT_AUDIO_CODEC_AAC, (void *)&codec_aac, (uint32_t)sizeof(rtk_bt_a2dp_media_codec_sbc_t)));

		/* Enable BT */
		BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
		BT_APP_PROCESS(rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_VISIBLE_CONNECTABLE));

		BT_APP_PROCESS(rtk_bt_br_gap_get_bd_addr(&bd_addr));
		rtk_bt_br_addr_to_str(bd_addr.addr, addr_str, sizeof(addr_str));
		BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

		/* le gap related */
		{
			BT_APP_PROCESS(rtk_bt_le_gap_set_scan_param(&scan_param));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_COMMON_GP_GAP,
														scatternet_gap_app_callback));
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP,
														ble_scatternet_gap_app_callback));
			memcpy(dev_name, (const uint8_t *)RTK_BT_DEV_NAME, strlen((const char *)RTK_BT_DEV_NAME));
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name((uint8_t *)dev_name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_HEART_RATE_BELT));

			BT_APP_PROCESS(rtk_bt_le_sm_set_security_param(&sec_param));

#if !defined(RTK_BLE_5_0_AE_ADV_SUPPORT) || !RTK_BLE_5_0_AE_ADV_SUPPORT
			memcpy(&adv_param, &def_adv_param, sizeof(rtk_bt_le_adv_param_t));
#endif
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
			if (privacy_enable) {
				BT_APP_PROCESS(rtk_bt_le_gap_privacy_init(privacy_whitelist));
#if !defined(RTK_BLE_5_0_AE_ADV_SUPPORT) || !RTK_BLE_5_0_AE_ADV_SUPPORT
				/* If privacy on, default use RPA adv, even not bonded */
				adv_param.own_addr_type = 2;
#endif
				BT_APP_PROCESS(rtk_bt_le_sm_get_bond_num(&bond_size));
				if (bond_size != 0) {
#if (defined(PRIVACY_USE_DIR_ADV_WHEN_BONDED) && PRIVACY_USE_DIR_ADV_WHEN_BONDED) && (!defined(RTK_BLE_5_0_AE_ADV_SUPPORT) || !RTK_BLE_5_0_AE_ADV_SUPPORT)
					rtk_bt_le_bond_info_t bond_info = {0};
					uint8_t bond_num = 1;
					rtk_bt_le_sm_get_bond_info(&bond_info, &bond_num);
					adv_param.type = RTK_BT_LE_ADV_TYPE_DIRECT_IND_LOW;
					adv_param.peer_addr.type = 0;
					memcpy(adv_param.peer_addr.addr_val, bond_info.ident_addr.addr_val, RTK_BD_ADDR_LEN);
					adv_param.own_addr_type = 2;
#endif
					if (privacy_whitelist) {
						adv_filter_whitelist = true;
					}
				}
			}
#endif /* RTK_BLE_PRIVACY_SUPPORT */

			/* gatts related */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTS,
														ble_scatternet_gatts_app_callback));
			BT_APP_PROCESS(simple_ble_srv_add());
			BT_APP_PROCESS(device_info_srv_add());
			BT_APP_PROCESS(heart_rate_srv_add());
			BT_APP_PROCESS(battery_srv_add());
			BT_APP_PROCESS(immediate_alert_srv_add());
			BT_APP_PROCESS(glucose_srv_add());
			BT_APP_PROCESS(long_uuid_srv_add());
			/* gattc related */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTC,
														ble_scatternet_gattc_app_callback));
			BT_APP_PROCESS(general_client_add());
			BT_APP_PROCESS(bas_client_add());
			BT_APP_PROCESS(gaps_client_add());
			BT_APP_PROCESS(simple_ble_client_add());

#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
			if (adv_filter_whitelist) {
				ext_adv_param.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST;
			}
			BT_APP_PROCESS(rtk_bt_le_gap_create_ext_adv(&ext_adv_param, &adv_handle));
			BT_APP_PROCESS(rtk_bt_le_gap_set_ext_adv_data(adv_handle, ext_adv_data, sizeof(ext_adv_data)));
			BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(adv_handle, 0, 0));
#else
			BT_APP_PROCESS(rtk_bt_le_gap_set_adv_data(adv_data, sizeof(adv_data)));
			BT_APP_PROCESS(rtk_bt_le_gap_set_scan_rsp_data(scan_rsp_data, sizeof(scan_rsp_data)));
			if (adv_filter_whitelist) {
				adv_param.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST;
			}
			BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&adv_param));
#endif

#if (defined(BT_POWER_TEST_MODE) && BT_POWER_TEST_MODE) && (defined(RTK_BT_POWER_CONTROL_SUPPORT) && RTK_BT_POWER_CONTROL_SUPPORT)
			bt_power_test_init();
#endif
		}

		/* bredr gap related */
		{
#if defined(CONFIG_BT_AUDIO_SOURCE_OUTBAND) && CONFIG_BT_AUDIO_SOURCE_OUTBAND
			demo_uart_init();
#endif
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
			bt_concurrent_demo_init_flag = 1;
		}
	} else if (0 == enable) {
		if (!bt_concurrent_demo_init_flag) {
			BT_LOGE("%s No need deinit! \r\n", __func__);
			return -1;
		}
#if (defined(BT_POWER_TEST_MODE) && BT_POWER_TEST_MODE) && (defined(RTK_BT_POWER_CONTROL_SUPPORT) && RTK_BT_POWER_CONTROL_SUPPORT)
		bt_power_test_deinit();
#endif
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
			if (a2dp_task.hdl) {
				BT_LOGA("[A2DP Demo] Delete Test Demo \r\n");
				app_a2dp_src_send_flag = false;
				a2dp_task.run = 0;
				if (false == osif_sem_take(a2dp_task.sem, 0xffffffff)) {
					BT_LOGE("[A2DP Demo] take a2dp_task.sem fail \r\n");
					return -1;
				}
				osif_sem_delete(a2dp_task.sem);
				memset((void *)&a2dp_task, 0, sizeof(struct a2dp_demo_task_t));
			}
		}
		app_server_deinit();
		BT_APP_PROCESS(general_client_delete());
		BT_APP_PROCESS(bas_client_delete());
		BT_APP_PROCESS(gaps_client_delete());
		BT_APP_PROCESS(simple_ble_client_delete());

		/* no need to unreg callback here, it is done in rtk_bt_disable */
		// BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_GAP));
		// BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_GATTS));
		// BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_GATTC));

		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_GAP));
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_SDP));
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_AVRCP));
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_A2DP));
		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
		/* audio related resources release */
		rtk_bt_audio_deinit();
		a2dp_demo_audio_track_hdl = NULL;
		a2dp_demo_codec_entity = NULL;
		memset((void *)&audio_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
		bt_concurrent_demo_init_flag = 0;
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