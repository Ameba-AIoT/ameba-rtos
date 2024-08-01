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
#include <rtk_bt_br_gap.h>
#include <rtk_bt_sdp.h>
#include <rtk_bt_hfp.h>
#include <rtk_bt_pbap.h>
#include <bt_audio_intf.h>
#include <bt_audio_track_api.h>
#include <bt_audio_record_api.h>
#include <app_audio_data.h>
#include <cvsd_codec_entity.h>
#include <bt_utils.h>

/* -------------------------------- Defines --------------------------------- */
#define RTK_BT_DEV_NAME "RTK_BT_HFP_HF"
#define DEFAULT_PAGESCAN_WINDOW             0x48
#define DEFAULT_PAGESCAN_INTERVAL           0x800 //0x800
#define DEFAULT_PAGE_TIMEOUT                0x2000
#define DEFAULT_INQUIRYSCAN_WINDOW          0x48
#define DEFAULT_INQUIRYSCAN_INTERVAL        0x800 //0x1000
#define DEFAULT_AUDIO_LEFT_VOLUME           1
#define DEFAULT_AUDIO_RIGHT_VOLUME          1

/* ------------------------------- Company ID ------------------------------- */
#define RTK_COMPANY_ID 0x005D

/* ---------------------------- Global Variables ---------------------------- */

/* ---------------------------- Static Variables ---------------------------- */
static uint8_t hfp_demo_init_flag = 0;
static void *alert_timer = NULL;
static void *alert_track_hdl = NULL;
static rtk_bt_audio_track_t *hfp_demo_audio_track_hdl = NULL;
static rtk_bt_audio_record_t *hfp_demo_audio_record_hdl = NULL;
static void *hfp_demo_codec_entity = NULL;
static uint8_t hfp_demo_role;
bool ring_alert_inband = false;
static uint8_t battery_power = RTK_BT_HFP_DEMO_DEFAULT_BATTERY_POWER_LEVEL;
static rtk_bt_audio_codec_conf_t audio_codec_conf = {0};
static rtk_bt_cvsd_codec_t cvsd_codec_t = {0};
static uint8_t remote_bd_addr[6] = {0};
static uint8_t call_curr_status = 0;
/* class of device */
static uint32_t cod = (0x18 | (0x04 << 8) | (0x24 << 16));
/* supervision_timeout */
static uint16_t superv_time = (0x1f40);
static uint16_t sco_seq_num = 0;

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

/* for hfp hf client read record data */
struct hfp_task_t {
	void *hdl;
	void *sem;
	uint8_t run;
};

static struct hfp_task_t hfp_task = {
	.hdl = NULL,
	.sem = NULL,
	.run  = 0,
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

const uint8_t pbap_pce_sdp_record[] = {
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
		memcpy((void *)remote_bd_addr, bd_addr, 6);
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

	case RTK_BT_BR_GAP_ACL_DISCONN: {
		rtk_bt_br_acl_disc_t *p_acl_disc_event = (rtk_bt_br_acl_disc_t *)param;
		BT_LOGA("[BR GAP] ACL disconnection %02x:%02x:%02x:%02x:%02x:%02x \r\n",
				p_acl_disc_event->bd_addr[5], p_acl_disc_event->bd_addr[4],
				p_acl_disc_event->bd_addr[3], p_acl_disc_event->bd_addr[2],
				p_acl_disc_event->bd_addr[1], p_acl_disc_event->bd_addr[0]);
		BT_AT_PRINT("+BRGAP:disc,%02x:%02x:%02x:%02x:%02x:%02x,0x%x\r\n",
					p_acl_disc_event->bd_addr[5], p_acl_disc_event->bd_addr[4],
					p_acl_disc_event->bd_addr[3], p_acl_disc_event->bd_addr[2],
					p_acl_disc_event->bd_addr[1], p_acl_disc_event->bd_addr[0],
					p_acl_disc_event->cause);
		break;
	}

	default:
		BT_LOGE("%s, Unknown GAP RTK_BLE_EVT: %d\r\n", __func__, evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static uint16_t rtk_bt_hfp_cvsd_parse_decoder_struct(rtk_bt_hfp_codec_t *phfp_codec, rtk_bt_cvsd_decode_t *pcvsd_decoder_t)
{
	pcvsd_decoder_t->channel_num = phfp_codec->cvsd.channel_num;
	pcvsd_decoder_t->sample_rate = phfp_codec->cvsd.sample_rate;
	pcvsd_decoder_t->frame_duration = phfp_codec->cvsd.frame_duration;
	hfp_demo_audio_track_hdl = rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_CVSD, (float)DEFAULT_AUDIO_LEFT_VOLUME, (float)0, pcvsd_decoder_t->channel_num,
													  pcvsd_decoder_t->sample_rate, BT_AUDIO_FORMAT_PCM_16_BIT, 0, NULL, true);
	if (!hfp_demo_audio_track_hdl) {
		BT_LOGE("[HFP] bt audio track add fail \r\n");
		return 1;
	}
	hfp_demo_audio_record_hdl = rtk_bt_audio_record_add(RTK_BT_AUDIO_CODEC_CVSD, 1, 8000, 0);
	if (!hfp_demo_audio_record_hdl) {
		BT_LOGE("[HFP] bt audio record add fail \r\n");
		rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_CVSD, hfp_demo_audio_track_hdl);
		hfp_demo_audio_track_hdl = NULL;
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
		read_size = rtk_bt_audio_record_data_get(RTK_BT_AUDIO_CODEC_CVSD, hfp_demo_audio_record_hdl, hfp_demo_codec_entity, (void *)voice_buffer, 30 * sizeof(int16_t),
												 true);
		osif_delay(1);
		// BT_LOGA("get size %d \r\n", read_size);
		if (read_size) {
			penc_codec_buffer_t = rtk_bt_audio_data_encode(RTK_BT_AUDIO_CODEC_CVSD, hfp_demo_codec_entity, voice_buffer, read_size / 2);
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
			rtk_bt_audio_free_encode_buffer(audio_codec_conf.codec_index, hfp_demo_codec_entity, penc_codec_buffer_t);
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
	alert_track_hdl = rtk_bt_audio_track_init((uint32_t)2, (uint32_t)44100, BT_AUDIO_FORMAT_PCM_16_BIT, 1024, 0, 0);
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
			audio_codec_conf.codec_index = RTK_BT_AUDIO_CODEC_CVSD;
			audio_codec_conf.param = (void *)&cvsd_codec_t;
			audio_codec_conf.param_len = sizeof(cvsd_codec_t);
		}
		if (ret) {
			BT_LOGE("[HFP] RTK_BT_HFP_EVT_SCO_CONN_CMPL Fail \r\n");
			rtk_bt_audio_deinit();
			hfp_demo_audio_track_hdl = NULL;
			hfp_demo_audio_record_hdl = NULL;
			break;
		}
		hfp_demo_codec_entity = rtk_bt_audio_codec_add(&audio_codec_conf);
		BT_LOGA("[HFP] Configure Complete CODEC %d \r\n", phfp_codec->codec_type);
		/* config audio record thread */
		{
			BT_LOGA("[HFP Demo] Create Record Demo \r\n");
			if (false == osif_sem_create(&hfp_task.sem, 0, 1)) {
				BT_LOGE("[HFP Demo] Create Record Demo Fail\r\n");
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

		if (rtk_bt_audio_recvd_data_in(RTK_BT_AUDIO_CODEC_CVSD, hfp_demo_audio_track_hdl, hfp_demo_codec_entity, pdata_in->data, pdata_in->length, 0)) {
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
		rtk_bt_audio_codec_remove(audio_codec_conf.codec_index, hfp_demo_codec_entity);
		memset((void *)&audio_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
		rtk_bt_audio_deinit();
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
	}
	break;

	default: {
		BT_LOGE("[HFP]: default evt_code 0x%04x \r\n", evt_code);
	}
	break;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t rtk_bt_pbap_app_callback(uint8_t evt_code, void *param, uint32_t len)
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

static rtk_bt_hfp_ag_conf_t demo_ag_conf = {
	.link_num = 1,
	.rfc_hfp_ag_chann_num = RTK_BT_RFC_HFP_AG_CHANN_NUM,
	.rfc_hsp_ag_chann_num = RTK_BT_RFC_HSP_AG_CHANN_NUM,
	.ag_supported_features = RTK_BT_HFP_AG_LOCAL_CAPABILITY_3WAY |
	RTK_BT_HFP_AG_LOCAL_CAPABILITY_VOICE_RECOGNITION |
	RTK_BT_HFP_AG_LOCAL_CAPABILITY_HF_INDICATORS |
	RTK_BT_HFP_AG_LOCAL_CAPABILITY_ESCO_S4_T2_SUPPORTED,
	.ag_supported_codecs = RTK_BT_HFP_AG_CODEC_TYPE_CVSD
};
static rtk_bt_hfp_hf_conf_t demo_hf_conf = {
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

/**
 * @brief  process hfp main init/ deinit.
 * @param  enable[in]: 1: init. 0 deinit
 * @return none.
 */
int bt_hfp_main(uint8_t role, uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_br_bd_addr_t bd_addr = {0};
	char addr_str[30] = {0};
	char dev_name[30] = {0};

	if (1 == enable) {
		if (hfp_demo_init_flag) {
			BT_LOGE("%s Already init! \r\n", __func__);
			return -1;
		}

		//set GAP configuration
		if (RTK_BT_AUDIO_HFP_ROLE_HF == role) {
			bt_app_conf.app_profile_support =   RTK_BT_PROFILE_HFP | \
												RTK_BT_PROFILE_SDP | \
												RTK_BT_PROFILE_PBAP;
		} else {
			bt_app_conf.app_profile_support =   RTK_BT_PROFILE_HFP | \
												RTK_BT_PROFILE_SDP;
		}
		bt_app_conf.mtu_size = 180;
		bt_app_conf.prefer_all_phy = 0;
		bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
		bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
		bt_app_conf.max_tx_octets = 0x40;
		bt_app_conf.max_tx_time = 0x200;
		bt_app_conf.hfp_role = role;

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
		if (RTK_BT_AUDIO_HFP_ROLE_AG == role) {
			BT_APP_PROCESS(rtk_sdp_record_add((void *)hfp_ag_sdp_record, sizeof(hfp_ag_sdp_record)));
			BT_APP_PROCESS(rtk_bt_hfp_cfg((uint8_t)RTK_BT_AUDIO_HFP_ROLE_AG, (void *)&demo_ag_conf));
			hfp_demo_role = RTK_BT_AUDIO_HFP_ROLE_AG;
		} else if (RTK_BT_AUDIO_HFP_ROLE_HF == role) {
			BT_APP_PROCESS(rtk_sdp_record_add((void *)hfp_sdp_record, sizeof(hfp_sdp_record)));
			BT_APP_PROCESS(rtk_sdp_record_add((void *)pbap_pce_sdp_record, sizeof(pbap_pce_sdp_record)));
			BT_APP_PROCESS(rtk_bt_hfp_cfg((uint8_t)RTK_BT_AUDIO_HFP_ROLE_HF, (void *)&demo_hf_conf));
			hfp_demo_role = RTK_BT_AUDIO_HFP_ROLE_HF;
		} else {
			BT_LOGE("[APP] Wrong HFP role %d \r\n", role);
			return 1;
		}
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
		/* Initilize hfp part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_HFP, rtk_bt_hfp_app_callback));
		if (RTK_BT_AUDIO_HFP_ROLE_HF == role) {
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_PBAP, rtk_bt_pbap_app_callback));
		}
		hfp_demo_init_flag = 1;
	} else if (0 == enable) {
		if (!hfp_demo_init_flag) {
			BT_LOGE("%s No need deinit! \r\n", __func__);
			return -1;
		}
		/* stop outband ring alert */
		app_hfp_ring_alert_stop();
		/* audio related resources release */
		if (hfp_task.run) {
			hfp_task.run = 0;
			if (false == osif_sem_take(hfp_task.sem, 0xffffffffUL)) {
				return -1;
			}
			osif_sem_delete(hfp_task.sem);
			hfp_task.hdl = NULL;
			hfp_task.sem = NULL;
			rtk_bt_audio_codec_remove(audio_codec_conf.codec_index, hfp_demo_codec_entity);
		}
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_GAP));
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_SDP));
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_HFP));
		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
		call_curr_status = 0;
		rtk_bt_audio_deinit();
		hfp_demo_audio_track_hdl = NULL;
		hfp_demo_audio_record_hdl = NULL;
		hfp_demo_codec_entity = NULL;
		memset((void *)&audio_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
		hfp_demo_init_flag = 0;
	}

	return 0;
}