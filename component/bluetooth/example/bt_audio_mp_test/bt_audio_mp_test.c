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
#include <rtk_bt_spp.h>
#include <bt_audio_intf.h>
#include <bt_audio_track_api.h>
#include <bt_audio_record_api.h>
#include <app_audio_data.h>
#include <sbc_codec_entity.h>
#include <cvsd_codec_entity.h>
#include <bt_utils.h>

/* -------------------------------- Defines --------------------------------- */
#define RTK_BT_DEV_NAME "RTK_BT_MP_TEST_DEV"
#define DEFAULT_PAGESCAN_WINDOW             0x48
#define DEFAULT_PAGESCAN_INTERVAL           0x800 //0x800
#define DEFAULT_PAGE_TIMEOUT                0x2000
#define DEFAULT_INQUIRYSCAN_WINDOW          0x48
#define DEFAULT_INQUIRYSCAN_INTERVAL        0x800 //0x1000
#define DEFAULT_AUDIO_LEFT_VOLUME           0.8
#define DEFAULT_AUDIO_RIGHT_VOLUME          0.8

#define SPP_RFC_CHANN_NUM                   0x03 //SPP local server channel

#define SPP_MAX_LINK_NUM                    0x2 //SPP maximum link numbers
#define SPP_RFC_SPP_MAX_SERVICE_NUM         0x3 //SPP maximum service numbers
#define SPP_DEFAULT_MTU_SIZE                1012
#define SPP_DEFAULT_CREDITS                 10

#define CMD_MIC_SWITCH                      0x030E
#define CMD_SET_BST_GAIN                    0x040A

#define AMIC1                               0x01
#define AMIC2                               0x02
#define AMIC_OFF                            0x03

/* ------------------------------- Company ID ------------------------------- */
#define RTK_COMPANY_ID 0x005D

/* ----------------------------- Avrcp Related ------------------------------ */
#define VOL_MAX     0x7f
#define VOL_MIN     0x00

/* ---------------------------- Global Variables ---------------------------- */
static uint8_t curr_volume = 0x05;

/* ---------------------------- Static Variables ---------------------------- */
static uint8_t bt_audio_mp_test_init_flag = 0;
static void *alert_timer = NULL;
static void *alert_track_hdl = NULL;
static rtk_bt_audio_track_t *a2dp_demo_audio_track_hdl = NULL;
static rtk_bt_audio_track_t *hfp_demo_audio_track_hdl = NULL;
static rtk_bt_audio_record_t *hfp_demo_audio_record_hdl = NULL;
static void *a2dp_demo_codec_entity = NULL;
static void *hfp_demo_codec_entity = NULL;
static uint8_t hfp_demo_role;
static uint8_t a2dp_demo_role;
static uint8_t spp_demo_role;
static bool ring_alert_inband = false;
static uint8_t remote_bd_addr[6] = {0};
static uint8_t call_curr_status = 0;
/* class of device */
static uint32_t cod = (0x18 | (0x04 << 8) | (0x24 << 16));
/* supervision_timeout */
static uint16_t superv_time = (0x1f40);
static uint16_t sco_seq_num = 0;
static uint8_t local_server_chann;
static rtk_bt_audio_codec_conf_t audio_a2dp_codec_conf = {0};
static rtk_bt_audio_codec_conf_t audio_hfp_codec_conf = {0};
static uint8_t evt_MicSwitch[7] = {0xAA, 0x01, 0x03, 0x00, 0x0F, 0x09, 0x00};
static uint8_t evt_BstG[7] = {0xAA, 0x01, 0x03, 0x00, 0x0A, 0x08, 0x00};
static rtk_bt_cvsd_codec_t cvsd_codec_t = {0};
static rtk_bt_sbc_codec_t sbc_codec_t = {
	{
		.sbc_enc_mode = (sbc_channel_mode_t)SBC_MODE_STANDARD,
		.blocks = 16,
		.subbands = 8,
		.alloc_method = SBC_ALLOCATION_METHOD_SNR,
		.sample_rate = 16000,
		.bitpool = 100,
		.channel_mode = SBC_CHANNEL_MODE_DUAL_CHANNEL,
	},
	{0},
};

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

/** @brief  SPP UUID */
static const uint8_t spp_demo_service_class_uuid128[16] = {
	0x00, 0x00, 0x11, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb
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
	(uint8_t)(0x0107 >> 8),     //version number default hf1.7
	(uint8_t)(0x0107),

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
	(uint8_t)(0x003F >> 8),
	(uint8_t)(0x003F)
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

static const uint8_t spp_sdp_record[] = {
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x4C,
	//attribute SDP_ATTR_SRV_CLASS_ID_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_SERIAL_PORT >> 8),
	(uint8_t)(RTK_BT_UUID_SERIAL_PORT),

	//attribute SDP_ATTR_PROTO_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROTO_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROTO_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x0c,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_L2CAP >> 8),//Protocol ID: L2CAP
	(uint8_t)RTK_BT_UUID_L2CAP,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x05,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_RFCOMM >> 8),//Protocol ID: RFCOMM
	(uint8_t)RTK_BT_UUID_RFCOMM,
	RTK_BT_SDP_UNSIGNED_ONE_BYTE,
	SPP_RFC_CHANN_NUM,// Protocol Specific Parameter: Server channel

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
	(uint8_t)(RTK_BT_UUID_SERIAL_PORT >> 8),//Supported Profiles:Serial Port Profile
	(uint8_t)RTK_BT_UUID_SERIAL_PORT,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,// Profile Version
	0x01,//version 1.2
	0x02,

	//attribute SDP_ATTR_SRV_NAME
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)((RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET),
	RTK_BT_SDP_STRING_HDR,
	0x0B,
	0x73, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x20, 0x70, 0x6f, 0x72, 0x74 //"serial port"
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
							  "TotalTrack:", "Genre:", "PlayingTime:"
							 };
		rtk_bt_avrcp_element_attr_info_t *p_attr_t = (rtk_bt_avrcp_element_attr_info_t *)param;

		if (p_attr_t->state == 0) {
			BT_LOGA("[AVRCP] Get element attr information successfully from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
					p_attr_t->bd_addr[5], p_attr_t->bd_addr[4], p_attr_t->bd_addr[3], p_attr_t->bd_addr[2], p_attr_t->bd_addr[1], p_attr_t->bd_addr[0]);
			for (uint8_t i = 0; i < p_attr_t->num_of_attr; i ++) {
				if (p_attr_t->attr[i].length) {
					memset((void *)temp_buff, 0, 50);
					uint16_t len = p_attr_t->attr[i].length + strlen(attr[p_attr_t->attr[i].attribute_id]) + 1;
					snprintf((char *)temp_buff, len, "%s%s\r\n", attr[p_attr_t->attr[i].attribute_id], p_attr_t->attr[i].p_buf);
					BT_LOGA("[AVRCP] %s \r\n", temp_buff);
				}
			}
		} else {
			BT_LOGA("[AVRCP] Get element attr information fail from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
					p_attr_t->bd_addr[5], p_attr_t->bd_addr[4], p_attr_t->bd_addr[3], p_attr_t->bd_addr[2], p_attr_t->bd_addr[1], p_attr_t->bd_addr[0]);
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

		if (p_data_t->data_end) {
			BT_LOGA("[AVRCP] Get art cover successfully \r\n");
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

static rtk_bt_a2dp_media_codec_sbc_t codec_sbc = {
	.sampling_frequency_mask = 0xf0,
	.channel_mode_mask = 0x0f,
	.block_length_mask = 0xf0,
	.subbands_mask = 0x0C,
	.allocation_method_mask = 0x03,
	.min_bitpool = 2,
	.max_bitpool = 53,
};

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
		memcpy((void *)bd_addr, conn_ind->bd_addr, 6);
		memcpy((void *)remote_bd_addr, conn_ind->bd_addr, 6);
		BT_LOGA("[A2DP] A2DP connection completion with %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
		BT_AT_PRINT("+BTA2DP:conn,%02x:%02x:%02x:%02x:%02x:%02x\r\n",
					remote_bd_addr[5], remote_bd_addr[4], remote_bd_addr[3], remote_bd_addr[2], remote_bd_addr[1], remote_bd_addr[0]);
	}
	break;

	case RTK_BT_A2DP_EVT_DISCONN_CMPL: {
		rtk_bt_a2dp_disconn_ind_t *disconn_ind = (rtk_bt_a2dp_disconn_ind_t *)param;
		rtk_bt_audio_codec_remove(audio_a2dp_codec_conf.codec_index, a2dp_demo_codec_entity);
		memset((void *)&audio_a2dp_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
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
		if (a2dp_demo_audio_track_hdl) {
			rtk_bt_audio_track_resume(a2dp_demo_audio_track_hdl->audio_track_hdl);
		}
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_START_RSP: {
		memcpy((void *)bd_addr, param, 6);

		BT_LOGA("[A2DP] RTK_BT_A2DP_EVT_STREAM_START_RSP from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
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
	}
	break;

	case RTK_BT_A2DP_EVT_STREAM_DATA_IND: { //BT api shall not be called here
		rtk_bt_a2dp_stream_data_ind_t *pdata_in = (rtk_bt_a2dp_stream_data_ind_t *)param;

		if (rtk_bt_audio_recvd_data_in(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_audio_track_hdl, a2dp_demo_codec_entity, pdata_in->data, pdata_in->length, 0)) {
			BT_LOGE("[A2DP] Stream Data Receiving FAIL %d \r\n", RTK_BT_AUDIO_CODEC_SBC);
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
	pcvsd_decoder_t->channel_num = phfp_codec->cvsd.channel_num;
	pcvsd_decoder_t->sample_rate = phfp_codec->cvsd.sample_rate;
	pcvsd_decoder_t->frame_duration = phfp_codec->cvsd.frame_duration;
	hfp_demo_audio_track_hdl = rtk_bt_audio_track_add(RTK_BT_AUDIO_CODEC_CVSD, (float)DEFAULT_AUDIO_LEFT_VOLUME, (float)0, pcvsd_decoder_t->channel_num,
													  pcvsd_decoder_t->sample_rate, BT_AUDIO_FORMAT_PCM_16_BIT, 0, NULL, true);
	if (!hfp_demo_audio_track_hdl) {
		BT_LOGE("[HFP] bt audio track add fail \r\n");
		return 1;
	}
	hfp_demo_audio_record_hdl = rtk_bt_audio_record_add(RTK_BT_AUDIO_CODEC_CVSD, 1, 8000, 0, 0x7f);
	if (!hfp_demo_audio_record_hdl) {
		BT_LOGE("[HFP] bt audio record add fail \r\n");
		rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_CVSD, hfp_demo_audio_track_hdl);
		hfp_demo_audio_track_hdl = NULL;
		return 1;
	}

	return 0;
}

static bool reacord_flag = true;

static void hfp_task_entry(void *ctx)
{
	(void)ctx;
	static int read_size = 0;
	int16_t voice_buffer[30] = {0};
	struct enc_codec_buffer *penc_codec_buffer_t = NULL;
	rtk_bt_hfp_sco_data_send_t sco_data_t = {0};

	osif_sem_give(hfp_task.sem);

	while (hfp_task.run) {
		if (reacord_flag) {
			read_size = rtk_bt_audio_record_data_get(RTK_BT_AUDIO_CODEC_CVSD, hfp_demo_audio_record_hdl, hfp_demo_codec_entity, (void *)voice_buffer, 30 * sizeof(int16_t),
													 true);
		} else {
			read_size = 0;
		}
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
			rtk_bt_audio_free_encode_buffer(audio_hfp_codec_conf.codec_index, hfp_demo_codec_entity, penc_codec_buffer_t);
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
		BT_LOGA("[HFP] caller number is %s \r\n", p_hfp_caller_id_ind->number);
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
		if ((phfp_codec->codec_type & (RTK_BT_AUDIO_CODEC_CVSD /* | RTK_BT_AUDIO_CODEC_mSBC */)) == 0) {
			BT_LOGE("[HFP] Not support codec %d \r\n", phfp_codec->codec_type);
			break;
		}
		if (rtk_bt_audio_init()) {
			BT_LOGE("[HFP] rtk_bt_audio_init fail \r\n");
			break;
		}
		if (hfp_demo_role == RTK_BT_AUDIO_HFP_ROLE_HF) {
			if (phfp_codec->codec_type == RTK_BT_AUDIO_CODEC_CVSD) {
				ret = rtk_bt_hfp_cvsd_parse_decoder_struct(phfp_codec, &cvsd_codec_t.decoder_t);
				audio_hfp_codec_conf.codec_index = RTK_BT_AUDIO_CODEC_CVSD;
				audio_hfp_codec_conf.param = (void *)&cvsd_codec_t;
				audio_hfp_codec_conf.param_len = sizeof(cvsd_codec_t);
			}
			if (ret) {
				BT_LOGE("[HFP] RTK_BT_HFP_EVT_SCO_CONN_CMPL Fail \r\n");
				if (a2dp_demo_audio_track_hdl) {
					rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_CVSD, hfp_demo_audio_track_hdl);
					rtk_bt_audio_record_del(RTK_BT_AUDIO_CODEC_CVSD, hfp_demo_audio_record_hdl);
					rtk_bt_avrcp_play(remote_bd_addr);
				} else {
					rtk_bt_audio_deinit();
				}
				hfp_demo_audio_track_hdl = NULL;
				hfp_demo_audio_record_hdl = NULL;
				break;
			}
		}
		hfp_demo_codec_entity = rtk_bt_audio_codec_add(&audio_hfp_codec_conf);
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
		BT_LOGA("[HFP] HFP receive sco disconnected from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
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
		rtk_bt_audio_codec_remove(audio_hfp_codec_conf.codec_index, hfp_demo_codec_entity);
		memset((void *)&audio_hfp_codec_conf, 0, sizeof(rtk_bt_audio_codec_conf_t));
		if (a2dp_demo_audio_track_hdl) {
			rtk_bt_audio_track_del(RTK_BT_AUDIO_CODEC_CVSD, hfp_demo_audio_track_hdl);
			rtk_bt_audio_record_del(RTK_BT_AUDIO_CODEC_CVSD, hfp_demo_audio_record_hdl);
		} else {
			rtk_bt_audio_deinit();
		}
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

/**
    * @brief     SPP Tx function for test.
    * @param[in] rmt_bd_addr remote device BT address
    * @param[in] server_chann local server channel
    * @return    void
    */
static void spp_tx_test(uint8_t *rmt_bd_addr, uint8_t server_chann)
{
	BT_LOGA("[SPP] SPP TX test to %02x:%02x:%02x:%02x:%02x:%02x, server channel: 0x%x\r\n",
			rmt_bd_addr[5], rmt_bd_addr[4], rmt_bd_addr[3], rmt_bd_addr[2], rmt_bd_addr[1], rmt_bd_addr[0], server_chann);

	rtk_bt_spp_send_data_t send_data_t = {0};

	uint8_t spp_tx_data[8] = {0xAA, 0x01, 0x04, 0x00, 0x04, 0x00, 0x00, 0x6A};

	memcpy((void *)send_data_t.bd_addr, (void *)rmt_bd_addr, 6);
	send_data_t.local_server_chann = server_chann;
	//memcpy((void *)send_data_t.data, (void *)spp_tx_data, sizeof(spp_tx_data));
	send_data_t.data = spp_tx_data;
	send_data_t.len = sizeof(spp_tx_data);

	if (rtk_bt_spp_send_data(&send_data_t) != RTK_BT_OK) {
		BT_LOGE("[SPP] Send SPP tx test data falied\r\n");
		return ;
	}
	BT_LOGA("[SPP] Send SPP tx test data success\r\n");
}

/**
    * @brief     SPP Rx function for test.
    * @param[in] bd_addr remote device BT address
    * @param[in] buf the data buffer received
    * @param[in] len data buffer length
    * @return    void
    */
static void spp_rx_test(uint8_t *bd_addr, uint8_t *buf, uint16_t len)
{
	BT_LOGA("[SPP] SPP RX test from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
			bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	for (uint16_t i = 0; i < len; i++) {
		BT_LOGA("[SPP] Receive SPP tx test data: buf[%d] 0x%02x\r\n", i, buf[i]);
	}
}

static void bt_spp_parsing_recv_cmd(uint8_t *recv_buf, uint16_t buf_len)
{
	if (buf_len <= 6) {
		BT_LOGE("[SPP] parsing cmd failed: cmd length is too short !!!\r\n");
		return ;
	}

	uint8_t cmd_buf[RTK_BT_SPP_MAX_DATA_IND_LENGTH] = {0};
	memcpy((void *)cmd_buf, (void *)recv_buf, buf_len);

	// Sync word
	uint8_t sync_word = cmd_buf[0];
	if (sync_word != 0xAA) {
		BT_LOGE("[SPP] parsing cmd failed: incompatible Sync word !!!\r\n");
		return ;
	}
	// Sequence number
	// uint8_t seqn = cmd_buf[1];

	// payload length =  cmd/evt opcode len(fixed 2 bytes) + param_len
	uint8_t len_lowbyte = cmd_buf[2];
	uint8_t len_highbyte = cmd_buf[3];
	uint16_t payload_len = ((uint16_t)len_highbyte << 8) + (uint16_t)len_lowbyte;
	// Sync word + Sequence number + payload length = 4 bytes
	if (payload_len != buf_len - 4) {
		BT_LOGE("[SPP] parsing cmd failed: wrong payload length !!!\r\n");
		return ;
	}
	// param_len = payload length - cmd/evt opcode len(fixed 2 bytes)
	uint16_t param_len = ((uint16_t)len_highbyte << 8) + (uint16_t)len_lowbyte - 2;

	// cmd opcode
	uint8_t op_lowbyte = cmd_buf[4];
	uint8_t op_highbyte = cmd_buf[5];
	uint16_t cmd_op = ((uint16_t)op_highbyte << 8) + (uint16_t)op_lowbyte;

	// cmd parameter
	uint16_t param = 0;

	if (param_len > 2) {
		BT_LOGE("[SPP] parsing cmd failed: param length is too long !!!\r\n");
		return ;
	}

	if (param_len == 1) {
		param = (uint16_t)cmd_buf[6];
	} else if (param_len == 2) {
		param = (uint16_t)cmd_buf[7] + ((uint16_t)cmd_buf[6] << 8);
	} else if (param_len == 0) {
		BT_LOGE("[SPP] parsing cmd failed: param length is 0 !!!\r\n");
		return ;
	}

	switch (cmd_op) {
	case CMD_MIC_SWITCH: {
		if (param != AMIC1 && param != AMIC2 && param != AMIC_OFF) {
			BT_LOGE("[SPP] parsing cmd failed: CMD_MIC_SWITCH wrong parameter !!!\r\n");
			return ;
		}
		reacord_flag = false;
		rtk_bt_audio_record_stop(hfp_demo_audio_record_hdl);
		BT_LOGA("[SPP] record stop.. \r\n");
		if (param == AMIC1) {
			if (rtk_bt_audio_record_set_parameters(hfp_demo_audio_record_hdl, "ch0_sel_amic=1")) {
				BT_LOGE("[SPP] Set Mic failed !!! \r\n");
				return;
			}
			BT_LOGA("[SPP] Set MIC: %d success \r\n", param);
			evt_MicSwitch[6] = AMIC1;
			rtk_bt_audio_record_start(hfp_demo_audio_record_hdl);
			reacord_flag = true;
			BT_LOGA("[SPP] record start.. \r\n");
		} else if (param == AMIC2) {
			if (rtk_bt_audio_record_set_parameters(hfp_demo_audio_record_hdl, "ch0_sel_amic=2")) {
				BT_LOGE("[SPP] Set MIC failed !!! \r\n");
				return;
			}
			BT_LOGA("[SPP] Set MIC: %d success \r\n", param);
			evt_MicSwitch[6] = AMIC2;
			rtk_bt_audio_record_start(hfp_demo_audio_record_hdl);
			reacord_flag = true;
			BT_LOGA("[SPP] record start.. \r\n");
		} else if (param == AMIC_OFF) {
			BT_LOGA("[SPP] Turn off the MIC..\r\n");
			evt_MicSwitch[6] = 0;
		}
		// send SPP event to peer device
		rtk_bt_spp_send_data_t send_data_t = {0};
		memcpy((void *)send_data_t.bd_addr, (void *)remote_bd_addr, 6);
		send_data_t.local_server_chann = local_server_chann;
		send_data_t.data = evt_MicSwitch;
		send_data_t.len = sizeof(evt_MicSwitch);
		if (rtk_bt_spp_send_data(&send_data_t) != RTK_BT_OK) {
			BT_LOGE("[SPP] Send CMD_MIC_SWITCH event falied\r\n");
			return ;
		}
		BT_LOGA("[SPP] Send CMD_MIC_SWITCH event success\r\n");
		break;
	}

	case CMD_SET_BST_GAIN: {
		// param1: mic index
		uint8_t mic_index = (uint8_t)(param >> 8);
		// param2: bst gain value
		uint8_t bstG_val = (uint8_t)param;

		if (mic_index != AMIC1 && mic_index != AMIC2) {
			BT_LOGE("[SPP] parsing cmd failed: CMD_SET_BST_GAIN wrong parameter !!!\r\n");
			return ;
		}

		if (bstG_val > 8) {
			BT_LOGE("[SPP] CMD_SET_BST_GAIN failed: exceed the max value !!!\r\n ");
			return ;
		}
		// stop audio record
		reacord_flag = false;
		rtk_bt_audio_record_stop(hfp_demo_audio_record_hdl);
		BT_LOGA("[SPP] record stop.. \r\n");

		if (mic_index == AMIC1) {
			evt_BstG[6] = AMIC1;
			if (rtk_bt_audio_record_set_mic_bst_gain(RTK_BT_AUDIO_AMIC1, (uint32_t)bstG_val)) {
				BT_LOGE("[SPP] Set MicBstGain failed !!! \r\n");
				return;
			}
			BT_LOGA("[SPP] Set Mic %d BstGain: %d db success !!! \r\n", mic_index, bstG_val *= 5);
		} else if (mic_index == AMIC2) {
			evt_BstG[6] = AMIC2;
			if (rtk_bt_audio_record_set_mic_bst_gain(RTK_BT_AUDIO_AMIC2, (uint32_t)bstG_val)) {
				BT_LOGE("[SPP] Set MicBstGain failed !!! \r\n");
				return;
			}
			BT_LOGA("[SPP] Set Mic %d BstGain: %d db success !!! \r\n", mic_index, bstG_val *= 5);
		}
		// start audio record
		rtk_bt_audio_record_start(hfp_demo_audio_record_hdl);
		reacord_flag = true;
		BT_LOGA("[SPP] record start.. \r\n");

		// send SPP event to peer device
		rtk_bt_spp_send_data_t send_data_t = {0};
		memcpy((void *)send_data_t.bd_addr, (void *)remote_bd_addr, 6);
		send_data_t.local_server_chann = local_server_chann;
		send_data_t.data = evt_BstG;
		send_data_t.len = sizeof(evt_BstG);
		if (rtk_bt_spp_send_data(&send_data_t) != RTK_BT_OK) {
			BT_LOGE("[SPP] Send CMD_SET_BST_GAIN event falied\r\n");
			return ;
		}
		BT_LOGA("[SPP] Send CMD_SET_BST_GAIN event success\r\n");
		break;
	}

	default:
		BT_LOGE("[SPP] %s, Unknown SPP cmd: 0x%x\r\n", __func__, cmd_op);
		break;
	}
}

/**
    * @brief     SPP Audio Mic Switch rx test
    * @param[in] bd_addr remote device BT address
    * @param[in] server_chann local server channel
    * @param[in] buf the spp cmd data buffer received
    * @param[in] len data buffer length
    * @return    void
    */
static void SPP_Audio_MIC_Switch_rx_test(uint8_t *bd_addr, uint8_t server_chann, uint8_t *buf, uint16_t len)
{
	BT_LOGA("[SPP] SPP_Audio_Mic_Switch_rx_test from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
			bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);

	memcpy((void *)remote_bd_addr, bd_addr, 6);
	local_server_chann = server_chann;
	bt_spp_parsing_recv_cmd(buf, len);
}

static rtk_bt_evt_cb_ret_t rtk_bt_spp_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	(void)param;

	uint8_t bd_addr[6];

	switch (evt_code) {
	case RTK_BT_SPP_EVT_SDP_ATTR_INFO: {
		rtk_bt_spp_attr_info_t *p_info = (rtk_bt_spp_attr_info_t *)param;
		memcpy((void *)bd_addr, p_info->bd_addr, 6);
		BT_LOGA("[SPP] SPP sdp attr info from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);

		/* Print UUID*/
		if (p_info->srv_class_uuid_type == SPP_UUID128) {
			BT_LOGA("[SPP] UUID type is uuid128\r\n");
			uint8_t *uuid_128 = p_info->srv_class_uuid_data.uuid_128;
			BT_DUMPA("[SPP] uuid128 is : ", uuid_128, 16);
		} else if (p_info->srv_class_uuid_type == SPP_UUID16) {
			uint16_t uuid_16 = p_info->srv_class_uuid_data.uuid_16;
			BT_LOGA("[SPP] UUID type is uuid16\r\n");
			BT_LOGA("[SPP] uuid16 is : 0x%x\r\n", uuid_16);
		} else if (p_info->srv_class_uuid_type == SPP_UUID32) {
			uint32_t uuid_32 = p_info->srv_class_uuid_data.uuid_32;
			BT_LOGA("[SPP] UUID type is uuid32\r\n");
			BT_LOGA("[SPP] uuid32 is : 0x%x\r\n", uuid_32);
		}

	}
	break;

	case RTK_BT_SPP_EVT_SDP_DISCOV_CMPL: {
		rtk_bt_spp_sdp_discov_cmpl_t *p_discov = (rtk_bt_spp_sdp_discov_cmpl_t *)param;
		memcpy((void *)bd_addr, p_discov->bd_addr, 6);
		BT_LOGA("[SPP] SPP sdp discov cmpl from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);
	}
	break;

	case RTK_BT_SPP_EVT_CONN_IND: {
		rtk_bt_spp_conn_ind_t *p_conn_ind = (rtk_bt_spp_conn_ind_t *)param;
		memcpy((void *)bd_addr, p_conn_ind->bd_addr, 6);
		BT_LOGA("[SPP] Receive SPP connection from %02x:%02x:%02x:%02x:%02x:%02x, server channel: 0x%x, frame size: %d\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0],
				p_conn_ind->local_server_chann, p_conn_ind->frame_size);
	}
	break;

	case RTK_BT_SPP_EVT_CONN_COMPL: {
		rtk_bt_spp_conn_cmpl_t *p_conn_cmpl = (rtk_bt_spp_conn_cmpl_t *)param;
		memcpy((void *)bd_addr, p_conn_cmpl->bd_addr, 6);
		BT_LOGA("[SPP] SPP connection completion with %02x:%02x:%02x:%02x:%02x:%02x, server channel: 0x%x, frame size: %d, link credit: %d\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0],
				p_conn_cmpl->local_server_chann, p_conn_cmpl->frame_size,
				p_conn_cmpl->link_credit);
		BT_AT_PRINT("+BTSPP:conn,%02x:%02x:%02x:%02x:%02x:%02x,0x%x,%d,%d\r\n",
					bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0],
					p_conn_cmpl->local_server_chann, p_conn_cmpl->frame_size,
					p_conn_cmpl->link_credit);


		/* Then tx data */
		if (spp_demo_role == RTK_BT_SPP_ROLE_SERVER) {
			if (p_conn_cmpl->link_credit) {
				uint8_t server_chann = p_conn_cmpl->local_server_chann;
				for (uint16_t i = 0; i < 1; i++) {
					spp_tx_test(bd_addr, server_chann);
				}
			}

			// /* disconnect spp channel*/
			// rtk_bt_spp_disconn_req_t p_disconn_req_t = {0};
			// memcpy((void *)p_disconn_req_t.bd_addr, p_conn_cmpl->bd_addr, 6);
			// p_disconn_req_t.local_server_chann = p_conn_cmpl->local_server_chann;
			// if (rtk_bt_spp_disconnect(&p_disconn_req_t) != RTK_BT_OK) {
			//     BT_LOGE("[SPP] SPP server send spp disconnection request failed\r\n");
			//     break;
			// }
			// BT_LOGA("[SPP] SPP server send spp disconnection request to channel 0x%x success!!\r\n", p_conn_cmpl->local_server_chann);

			// /* disconnect all spp channel*/
			// if (rtk_bt_spp_disconnect_all(p_conn_cmpl->bd_addr) != RTK_BT_OK) {
			//     BT_LOGE("[SPP] SPP server send spp disconnection all request failed\r\n");
			//     break;
			// }
			// BT_LOGA("[SPP] SPP server send spp disconnection request to all channel success!!\r\n");

		}
	}
	break;

	case RTK_BT_SPP_EVT_DISCONN_CMPL: {
		rtk_bt_spp_disconn_ind_t *p_disconn_ind = (rtk_bt_spp_disconn_ind_t *)param;
		memcpy((void *)bd_addr, p_disconn_ind->bd_addr, 6);
		BT_LOGA("[SPP] SPP disconnection completion with %02x:%02x:%02x:%02x:%02x:%02x, server channel: 0x%x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0],
				p_disconn_ind->local_server_chann);
		BT_AT_PRINT("+BTSPP:disconn,%02x:%02x:%02x:%02x:%02x:%02x,0x%x\r\n",
					bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0],
					p_disconn_ind->local_server_chann);
	}
	break;

	case RTK_BT_SPP_EVT_CREDIT_RCVD: {
		rtk_bt_spp_credit_rcvd_t *p_credit_rcvd = (rtk_bt_spp_credit_rcvd_t *)param;
		memcpy((void *)bd_addr, p_credit_rcvd->bd_addr, 6);
		BT_LOGA("[SPP] Receive %d link credit from %02x:%02x:%02x:%02x:%02x:%02x, server channel: 0x%x\r\n", p_credit_rcvd->link_credit,
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2],
				bd_addr[1], bd_addr[0], p_credit_rcvd->local_server_chann);
	}
	break;

	case RTK_BT_SPP_EVT_DATA_IND: {
		rtk_bt_spp_data_ind_t *p_data_ind = (rtk_bt_spp_data_ind_t *)param;

		memcpy((void *)bd_addr, p_data_ind->bd_addr, 6);
		BT_LOGA("[SPP] Receive data from %02x:%02x:%02x:%02x:%02x:%02x, server channel: 0x%x\r\n",
				bd_addr[5], bd_addr[4], bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0],
				p_data_ind->local_server_chann);

		/* Then rx test */
		spp_rx_test(bd_addr, p_data_ind->data, p_data_ind->len);

		/* Receive one packet, then send one SPP link credit */
		rtk_bt_spp_credits_give_t credits_give = {0};
		memcpy((void *)credits_give.bd_addr, p_data_ind->bd_addr, 6);
		credits_give.local_server_chann = p_data_ind->local_server_chann;
		credits_give.credits = 1;
		if (rtk_bt_spp_credits_give(&credits_give)) {
			BT_LOGE("[SPP] Send %d link credits to server channel 0x%x failed\r\n",
					credits_give.credits, credits_give.local_server_chann);
			break;
		}
		BT_LOGA("[SPP] Send %d link credits to server channel 0x%x success\r\n",
				credits_give.credits, credits_give.local_server_chann);

		SPP_Audio_MIC_Switch_rx_test(bd_addr, p_data_ind->local_server_chann, p_data_ind->data, p_data_ind->len);
	}
	break;

	default:
		BT_LOGE("%s, Unknown RTK_BT_BR_SPP_EVT: %d\r\n", __func__, evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_hfp_hf_conf_t demo_hf_conf = {
	.link_num = 1,
	.rfc_hfp_chann_num = RTK_BT_RFC_HFP_CHANN_NUM,
	.rfc_hsp_chann_num = RTK_BT_RFC_HSP_CHANN_NUM,
	.hf_supported_features = RTK_BT_HFP_HF_LOCAL_THREE_WAY_CALLING |
	RTK_BT_HFP_HF_LOCAL_CLI_PRESENTATION_CAPABILITY |
	RTK_BT_HFP_HF_LOCAL_VOICE_RECOGNITION_ACTIVATION |
	RTK_BT_HFP_HF_LOCAL_ESCO_S4_SETTINGS |
	RTK_BT_HFP_HF_LOCAL_REMOTE_VOLUME_CONTROL
};

/**
    * @brief  process bt audio mp test init/ deinit.
    * @param  enable[in]: 1: init. 0 deinit
    * @return none.
    */
int bt_audio_mp_test_main(uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_br_bd_addr_t bd_addr = {0};
	char addr_str[30] = {0};
	char dev_name[30] = {0};

	if (1 == enable) {
		if (bt_audio_mp_test_init_flag) {
			BT_LOGE("%s Already init! \r\n", __func__);
			return -1;
		}

		//set GAP configuration
		bt_app_conf.app_profile_support =   RTK_BT_PROFILE_A2DP | \
											RTK_BT_PROFILE_AVRCP | \
											RTK_BT_PROFILE_HFP | \
											RTK_BT_PROFILE_PBAP | \
											RTK_BT_PROFILE_SPP | \
											RTK_BT_PROFILE_SDP;
		bt_app_conf.mtu_size = 180;
		bt_app_conf.max_tx_octets = 0x40;
		bt_app_conf.max_tx_time = 0x200;
		bt_app_conf.a2dp_role = RTK_BT_A2DP_ROLE_SNK;
		bt_app_conf.hfp_role = RTK_BT_AUDIO_HFP_ROLE_HF;
		bt_app_conf.spp_role = RTK_BT_SPP_ROLE_CLIENT;

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
		BT_APP_PROCESS(rtk_sdp_record_add((void *)hfp_sdp_record, sizeof(hfp_sdp_record)));
		BT_APP_PROCESS(rtk_bt_hfp_cfg((uint8_t)RTK_BT_AUDIO_HFP_ROLE_HF, (void *)&demo_hf_conf));
		BT_APP_PROCESS(rtk_sdp_record_add((void *)a2dp_sink_sdp_record, sizeof(a2dp_sink_sdp_record)));
		BT_APP_PROCESS(rtk_sdp_record_add((void *)spp_sdp_record, sizeof(spp_sdp_record)));
		a2dp_demo_role = RTK_BT_A2DP_ROLE_SNK;
		hfp_demo_role = RTK_BT_AUDIO_HFP_ROLE_HF;
		spp_demo_role = RTK_BT_SPP_ROLE_CLIENT;
		BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_ct_sdp_record, sizeof(avrcp_ct_sdp_record)));
		BT_APP_PROCESS(rtk_sdp_record_add((void *)avrcp_tg_sdp_record, sizeof(avrcp_tg_sdp_record)));

		/* a2dp codec add (must be invoked before rtk_bt_enable) */
		BT_APP_PROCESS(rtk_bt_a2dp_codec_cfg((uint8_t)RTK_BT_AUDIO_CODEC_SBC, (void *)&codec_sbc, (uint32_t)sizeof(rtk_bt_a2dp_media_codec_sbc_t)));

		/* spp init config should be invoked before bt enable */
		BT_APP_PROCESS(rtk_bt_spp_init_cfg(SPP_MAX_LINK_NUM, SPP_RFC_SPP_MAX_SERVICE_NUM, SPP_DEFAULT_MTU_SIZE, SPP_DEFAULT_CREDITS));
		/* service register config should be invoked before bt enable */
		BT_APP_PROCESS(rtk_bt_service_register_cfg((void *)spp_demo_service_class_uuid128, sizeof(spp_demo_service_class_uuid128), SPP_RFC_CHANN_NUM));
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
		/* Initilize spp part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_SPP, rtk_bt_spp_app_callback));
		bt_audio_mp_test_init_flag = 1;
	} else if (0 == enable) {
		if (!bt_audio_mp_test_init_flag) {
			BT_LOGE("%s No need deinit! \r\n", __func__);
			return -1;
		}
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
			rtk_bt_audio_codec_remove(audio_hfp_codec_conf.codec_index, hfp_demo_codec_entity);
		}
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
		bt_audio_mp_test_init_flag = 0;
	}


	return 0;
}