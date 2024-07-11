/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

/* -------------------------------- Includes -------------------------------- */
#include <stdio.h>
#include <string.h>
#include <osif.h>

#include <rtk_bt_common.h>
#include <rtk_bt_def.h>
#include <bt_api_config.h>
#include <bt_app_config.h>
#include <rtk_bt_br_gap.h>
#include <rtk_bt_sdp.h>
#include <rtk_bt_device.h>
#include <rtk_bt_spp.h>
#include <bt_utils.h>

/* -------------------------------- Defines -------------------------------- */
#define SPP_DEFAULT_PAGESCAN_WINDOW             0x48
#define SPP_DEFAULT_PAGESCAN_INTERVAL           0x800 //0x800
#define SPP_DEFAULT_PAGE_TIMEOUT                0x4000
#define SPP_DEFAULT_INQUIRYSCAN_WINDOW          0x48
#define SPP_DEFAULT_INQUIRYSCAN_INTERVAL        0x800 //0x1000

/* Define SPP maximum link numbers, only one SPP server channel exists in the unique link based on RFCOMM */
#define SPP_MAX_LINK_NUM                        0x5

/* Define SPP maximum service numbers */
#define SPP_RFC_SPP_MAX_SERVICE_NUM             0x5

/* Define SPP default l2cap mtu size */
/* SPP application may send tx data lower than SPP mtu size at a time */
#define SPP_DEMO_SPP_DEFAULT_MTU_SIZE           1012

/* Define SPP default local tx window size */
/* SPP application should send tx data when SPP link credit is not zero */
#define SPP_DEMO_SPP_DEFAULT_CREDITS            10

/* Define SPP local server channel */
#define SPP_RFC_CHANN_NUM                       0x3

/* Define vendor SPP local server channel */
#define SPP_RFC_VENDOR_CHANN_NUM                0x4

/* Max length of device name, if device name length exceeds it, it will be truncated */
#define GAP_DEVICE_NAME_LEN                     40

/* ---------------------------- Static Variables ---------------------------- */

static uint8_t spp_demo_role;
static uint8_t rtk_bt_dev_name[GAP_DEVICE_NAME_LEN];

static rtk_bt_br_page_param_t page_param = {
	.pagescan_type = RTK_BT_BR_GAP_PAGE_SCAN_TYPE_STANDARD,
	.pagescan_interval = SPP_DEFAULT_PAGESCAN_INTERVAL,
	.pagescan_window = SPP_DEFAULT_PAGESCAN_WINDOW,
	.page_timeout = SPP_DEFAULT_PAGE_TIMEOUT,
};

static rtk_bt_br_inquiry_param_t inquiry_param = {
	.inquiryscan_type = RTK_BT_BR_GAP_PAGE_SCAN_TYPE_STANDARD,
	.inquiryscan_window = SPP_DEFAULT_INQUIRYSCAN_WINDOW,
	.inquiryscan_interval = SPP_DEFAULT_INQUIRYSCAN_INTERVAL,
	.inquiry_mode = RTK_BT_BR_GAP_INQUIRY_MODE_EXTENDED_RESULT,
};

/** @brief  SPP UUID */
static const uint8_t spp_demo_service_class_uuid128[16] = {
	0x00, 0x00, 0x11, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb
};

/** @brief  SPP Vendor UUID */
static const uint8_t rtk_vendor_spp_service_class_uuid128[16] = {
	0x6a, 0x24, 0xee, 0xab, 0x4b, 0x65, 0x46, 0x93, 0x98, 0x6b, 0x3c, 0x26, 0xc3, 0x52, 0x26, 0x4f
};

/**
 * @brief  SPP local SDP record.
 * @return void
 */
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

/**
 * @brief  Vendor SPP local SDP record.
 * @return void
 */
static const uint8_t rtk_vendor_spp_sdp_record[] = {
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x4E,
	//attribute SDP_ATTR_SRV_CLASS_ID_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x11,
	RTK_BT_SDP_UUID128_HDR,
	0x6a, 0x24, 0xee, 0xab, 0x4b, 0x65, 0x46, 0x93, 0x98, 0x6b, 0x3c, 0x26, 0xc3, 0x52, 0x26, 0x4f,

	//attribute SDP_ATTR_PROTO_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROTO_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROTO_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x0c,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_L2CAP >> 8),
	(uint8_t)RTK_BT_UUID_L2CAP,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x05,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_RFCOMM >> 8),
	(uint8_t)RTK_BT_UUID_RFCOMM,
	RTK_BT_SDP_UNSIGNED_ONE_BYTE,
	SPP_RFC_VENDOR_CHANN_NUM,

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

	//attribute SDP_ATTR_SRV_NAME
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)((RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET),
	RTK_BT_SDP_STRING_HDR,
	0x0B,
	'r', 't', 'k', '_', 'v', 'n', 'd', '_', 's', 'p', 'p'
};

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

	uint8_t spp_tx_data[8] = {0x01, 0x02, 0x03, 0x04, 0x0A, 0x0B, 0x0C, 0x0D};

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

static rtk_bt_evt_cb_ret_t br_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)param;
	(void)len;
	switch (evt_code) {

	case RTK_BT_BR_GAP_INQUIRY_RESULT: {
		rtk_bt_br_inquiry_result_t *p_result = (rtk_bt_br_inquiry_result_t *)param;
		BT_LOGA("[BR GAP] Scan %02x:%02x:%02x:%02x:%02x:%02x Name %s \r\n",
				p_result->bd_addr[5], p_result->bd_addr[4], p_result->bd_addr[3], p_result->bd_addr[2],
				p_result->bd_addr[1], p_result->bd_addr[0], p_result->name);
		BT_AT_PRINT("+BRGAP:inquiry_result,%02x:%02x:%02x:%02x:%02x:%02x,%s\r\n",
					p_result->bd_addr[5], p_result->bd_addr[4], p_result->bd_addr[3], p_result->bd_addr[2], p_result->bd_addr[1], p_result->bd_addr[0],
					p_result->name);
		// if spp server role, and the remote device name is "spp client", send spp connect request (stop inquiry and start sdp discov)
		// if (spp_demo_role == RTK_BT_SPP_ROLE_SERVER) {
		//  if (memcmp("spp client", p_result->name, 10) == 0)
		//  {
		//      memcpy((void *)client_bd_addr, p_result->bd_addr, 6);
		//      if (rtk_bt_spp_connect(client_bd_addr))
		//      {
		//          BT_LOGE("[BR GAP] Send spp connect request failed\r\n");
		//          break;
		//      }
		//      BT_LOGA("[BR GAP] Send spp connect request success\r\n");
		//      BT_LOGA("[BR GAP] wait for sdp discv info...\r\n");
		//  }
		// }
		break;
	}

	case RTK_BT_BR_GAP_ACL_CONN_IND: {
		uint8_t *bd_addr = (uint8_t *)param;
		BT_LOGA("[BR GAP] ACL connection indication %02x:%02x:%02x:%02x:%02x:%02x \r\n",
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

	case RTK_BT_BR_GAP_ACL_CONN_SUCCESS: {
		uint8_t *bd_addr = (uint8_t *)param;
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

	default:
		BT_LOGE("%s, Unknown RTK_BT_BR_GAP_EVT: %d\r\n", __func__, evt_code);
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

static rtk_bt_evt_cb_ret_t rtk_bt_spp_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)param;
	(void)len;

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

		// if (spp_demo_role == RTK_BT_SPP_ROLE_CLIENT)
		// {
		//     //if SPP client role, can tx data
		//     spp_tx_test(bd_addr, p_data_ind->local_server_chann);
		// }

		//spp_tx_test(bd_addr, p_data_ind->local_server_chann);
	}
	break;

	default:
		BT_LOGE("%s, Unknown RTK_BT_BR_SPP_EVT: %d\r\n", __func__, evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

/**
 * @brief  process spp main init/ deinit.
 * @param  role[in]: 1:spp server. 0: spp client
 * @param  enable[in]: 1: init. 0 deinit.
 * @return none.
 */
int bt_spp_main(uint8_t role, uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_br_bd_addr_t bd_addr = {0};
	char addr_str[30] = {0};

	if (1 == enable) {
		//set SPP demo role
		if (RTK_BT_SPP_ROLE_CLIENT == role) {
			spp_demo_role = RTK_BT_SPP_ROLE_CLIENT;
			bt_app_conf.spp_role = RTK_BT_SPP_ROLE_CLIENT;
			BT_LOGA("[APP] spp demo role : SPP Client\r\n");
			memcpy((void *)rtk_bt_dev_name, (void *)"spp client", 10);
		} else if (RTK_BT_SPP_ROLE_SERVER == role) {
			spp_demo_role = RTK_BT_SPP_ROLE_SERVER;
			bt_app_conf.spp_role = RTK_BT_SPP_ROLE_SERVER;
			memcpy((void *)rtk_bt_dev_name, (void *)"spp server", 10);
			BT_LOGA("[APP] spp demo role : SPP Server\r\n");
		} else {
			BT_LOGE("[APP] Wrong SPP role: %d \r\n", role);
			return 1;
		}

		//set GAP configuration
		bt_app_conf.app_profile_support = RTK_BT_PROFILE_SDP | \
										  RTK_BT_PROFILE_SPP;
		bt_app_conf.mtu_size = 180;
		bt_app_conf.prefer_all_phy = 0;
		bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
		bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
		bt_app_conf.max_tx_octets = 0x40;
		bt_app_conf.max_tx_time = 0x200;

		rtk_bt_br_gap_default_param_t dft_param;
		dft_param.mtu_size = 180;
		BT_APP_PROCESS(rtk_bt_br_gap_set_default_param(&dft_param));
		BT_APP_PROCESS(rtk_bt_br_gap_set_inquiry_param(&inquiry_param));
		BT_APP_PROCESS(rtk_bt_br_gap_set_page_param(&page_param));

		/* sdp record add should be invoked before bt enable */
		BT_APP_PROCESS(rtk_sdp_record_add((void *)spp_sdp_record, sizeof(spp_sdp_record)));
		BT_APP_PROCESS(rtk_sdp_record_add((void *)rtk_vendor_spp_sdp_record, sizeof(rtk_vendor_spp_sdp_record)));

		/* spp init config should be invoked before bt enable */
		BT_APP_PROCESS(rtk_bt_spp_init_cfg(SPP_MAX_LINK_NUM, SPP_RFC_SPP_MAX_SERVICE_NUM, SPP_DEMO_SPP_DEFAULT_MTU_SIZE, SPP_DEMO_SPP_DEFAULT_CREDITS));

		/* service register config should be invoked before bt enable */
		BT_APP_PROCESS(rtk_bt_service_register_cfg((void *)spp_demo_service_class_uuid128, sizeof(spp_demo_service_class_uuid128), SPP_RFC_CHANN_NUM));
		BT_APP_PROCESS(rtk_bt_service_register_cfg((void *)rtk_vendor_spp_service_class_uuid128, sizeof(rtk_vendor_spp_service_class_uuid128),
												   SPP_RFC_VENDOR_CHANN_NUM));

		/* Enable BT */
		BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
		BT_APP_PROCESS(rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_VISIBLE_CONNECTABLE));

		BT_APP_PROCESS(rtk_bt_br_gap_get_bd_addr(&bd_addr));
		rtk_bt_br_addr_to_str(bd_addr.addr, addr_str, sizeof(addr_str));
		BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

		/* Initialize GAP part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_GAP, br_gap_app_callback));
		BT_APP_PROCESS(rtk_bt_br_gap_set_device_name((const uint8_t *)rtk_bt_dev_name));
		/* Initilize SDP part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_SDP, rtk_bt_sdp_app_callback));
		/* Initilaize SPP part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_SPP, rtk_bt_spp_app_callback));

		// if role is SPP server, start inquiry and then wait for inquiry result callback
		if (spp_demo_role == RTK_BT_SPP_ROLE_SERVER) {
			BT_LOGA("[APP] SPP server start inquiry...\r\n");
			rtk_bt_br_inquiry_start_t inquiry_start_param = {0};
			inquiry_start_param.limited_inquiry = false;
			inquiry_start_param.timeout = 10;

			if (rtk_bt_br_gap_start_inquiry(&inquiry_start_param)) {
				BT_LOGE("[APP] Start inquiry failed\r\n");
			}
			BT_LOGA("[APP] Start inquiry success\r\n");
		}
	} else if (0 == enable) {
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_GAP));
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_SDP));
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_SPP));
		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
	}

	return 0;
}