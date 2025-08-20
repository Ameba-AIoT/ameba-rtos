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
#include <rtk_bt_le_gap.h>
#include <rtk_bt_br_gap.h>
#include <rtk_bt_sdp.h>
#include <rtk_bt_device.h>
#include <rtk_bt_rfc.h>
#include <bt_utils.h>

/* -------------------------------- Defines -------------------------------- */
#define RTK_BT_DEV_NAME                         "RTK_BT_RFC_DEVICE"
#define RFC_DEFAULT_PAGESCAN_WINDOW             0x48
#define RFC_DEFAULT_PAGESCAN_INTERVAL           0x800 //0x800
#define RFC_DEFAULT_PAGE_TIMEOUT                0x4000
#define RFC_DEFAULT_INQUIRYSCAN_WINDOW          0x48
#define RFC_DEFAULT_INQUIRYSCAN_INTERVAL        0x800 //0x1000
/* Define RFC local server channel */
#define RTK_BT_RFC_DEMO_CHANN_NUM                       0x3
/* ---------------------------- Static Variables ---------------------------- */
static rtk_bt_br_page_param_t page_param = {
	.pagescan_type = RTK_BT_BR_GAP_PAGE_SCAN_TYPE_STANDARD,
	.pagescan_interval = RFC_DEFAULT_PAGESCAN_INTERVAL,
	.pagescan_window = RFC_DEFAULT_PAGESCAN_WINDOW,
	.page_timeout = RFC_DEFAULT_PAGE_TIMEOUT,
};

static rtk_bt_br_inquiry_param_t inquiry_param = {
	.inquiryscan_type = RTK_BT_BR_GAP_PAGE_SCAN_TYPE_STANDARD,
	.inquiryscan_window = RFC_DEFAULT_INQUIRYSCAN_WINDOW,
	.inquiryscan_interval = RFC_DEFAULT_INQUIRYSCAN_INTERVAL,
	.inquiry_mode = RTK_BT_BR_GAP_INQUIRY_MODE_EXTENDED_RESULT,
};

/**
 * @brief  Vendor local SDP record.
 * @return void
 */
static const uint8_t rtk_vendor_rfc_sdp_record[] = {
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
	RTK_BT_RFC_DEMO_CHANN_NUM,

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
	'r', 't', 'k', '_', 'v', 'n', 'd', '_', 'r', 'f', 'c'
};

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
		BT_LOGA("[BR GAP] ACL disc cause 0x%x \r\n", p_acl_disc_event->cause);
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

static rtk_bt_evt_cb_ret_t rtk_bt_rfc_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;

	switch (evt_code) {
	case RTK_BT_RFC_EVT_RFC_ATTR_INFO: {
		const char *uuid_type[] = {"UUID16", "UUID32", "UUID128"};
		rtk_bt_rfc_attr_info_t *p_info = (rtk_bt_rfc_attr_info_t *)param;
		BT_LOGA("[RFC] SDP Scan %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_info->bd_addr[5], p_info->bd_addr[4],
				p_info->bd_addr[3], p_info->bd_addr[2],
				p_info->bd_addr[1], p_info->bd_addr[0]);
		BT_LOGA("[RFC] UUID type is %s \r\n", uuid_type[p_info->srv_class_uuid_type]);
		/* Print UUID*/
		if (p_info->srv_class_uuid_type == RFC_UUID128) {
			uint8_t *uuid_128 = p_info->srv_class_uuid_data.uuid_128;
			BT_DUMPA("[RFC] uuid128 is : ", uuid_128, 16);
		} else if (p_info->srv_class_uuid_type == RFC_UUID16) {
			uint16_t uuid_16 = p_info->srv_class_uuid_data.uuid_16;
			BT_LOGA("[RFC] uuid16 is : 0x%x\r\n", uuid_16);
		} else if (p_info->srv_class_uuid_type == RFC_UUID32) {
			uint32_t uuid_32 = p_info->srv_class_uuid_data.uuid_32;
			BT_LOGA("[RFC] uuid32 is : 0x%x\r\n", uuid_32);
		}

	}
	break;

	case RTK_BT_RFC_EVT_CONN_IND: {
		rtk_bt_rfc_conn_ind_t *p_rfc_conn_ind = (rtk_bt_rfc_conn_ind_t *)param;
		BT_LOGA("[RFC] receive connection indication from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_rfc_conn_ind->bd_addr[5], p_rfc_conn_ind->bd_addr[4],
				p_rfc_conn_ind->bd_addr[3], p_rfc_conn_ind->bd_addr[2],
				p_rfc_conn_ind->bd_addr[1], p_rfc_conn_ind->bd_addr[0]);
		BT_LOGA("[RFC] Local server chann is 0x%02x, frame size is 0x%d \r\n", p_rfc_conn_ind->local_server_chann, p_rfc_conn_ind->frame_size);
	}
	break;

	case RTK_BT_RFC_EVT_CONN_CMPL: {
		rtk_bt_rfc_conn_cmpl_t *p_rfc_conn_cmpl_ind = (rtk_bt_rfc_conn_cmpl_t *)param;
		BT_LOGA("[RFC] connection completion from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_rfc_conn_cmpl_ind->bd_addr[5], p_rfc_conn_cmpl_ind->bd_addr[4],
				p_rfc_conn_cmpl_ind->bd_addr[3], p_rfc_conn_cmpl_ind->bd_addr[2],
				p_rfc_conn_cmpl_ind->bd_addr[1], p_rfc_conn_cmpl_ind->bd_addr[0]);
		BT_LOGA("[RFC] Local server chann is 0x%02x, frame size is 0x%d, remain_credits is 0x%d \r\n",
				p_rfc_conn_cmpl_ind->local_server_chann,
				p_rfc_conn_cmpl_ind->frame_size,
				p_rfc_conn_cmpl_ind->remain_credits);
	}
	break;

	case RTK_BT_RFC_EVT_DISCONN_CMPL: {
		rtk_bt_rfc_disconn_cmpl_t *p_rfc_disconn_cmpl_ind = (rtk_bt_rfc_disconn_cmpl_t *)param;
		BT_LOGA("[RFC] disconnection completion with %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_rfc_disconn_cmpl_ind->bd_addr[5], p_rfc_disconn_cmpl_ind->bd_addr[4],
				p_rfc_disconn_cmpl_ind->bd_addr[3], p_rfc_disconn_cmpl_ind->bd_addr[2],
				p_rfc_disconn_cmpl_ind->bd_addr[1], p_rfc_disconn_cmpl_ind->bd_addr[0]);
		BT_LOGA("[RFC] Local server chann is 0x%02x, cause is 0x%04x \r\n",
				p_rfc_disconn_cmpl_ind->local_server_chann,
				p_rfc_disconn_cmpl_ind->cause);
	}
	break;

	case RTK_BT_RFC_EVT_DATA_IND: {
		rtk_bt_rfc_data_ind_t *p_rfc_data_ind = (rtk_bt_rfc_data_ind_t *)param;
		BT_LOGA("[RFC] receive data from %02x:%02x:%02x:%02x:%02x:%02x\r\n",
				p_rfc_data_ind->bd_addr[5], p_rfc_data_ind->bd_addr[4],
				p_rfc_data_ind->bd_addr[3], p_rfc_data_ind->bd_addr[2],
				p_rfc_data_ind->bd_addr[1], p_rfc_data_ind->bd_addr[0]);
		if (p_rfc_data_ind->length) {
			BT_DUMPA("[RFC] data: ", p_rfc_data_ind->buf, p_rfc_data_ind->length);
			osif_mem_free(p_rfc_data_ind->buf);
		}
	}
	break;

	case RTK_BT_RFC_EVT_CREDITS_INFO: {
		rtk_bt_rfc_credit_info_t *p_rfc_credit_info = (rtk_bt_rfc_credit_info_t *)param;
		BT_LOGA("[RFC] server channel:0x%x, remain credits:%d\r\n", p_rfc_credit_info->local_server_chann, p_rfc_credit_info->remain_credits);
	}
	break;

	default: {
		BT_LOGE("%s, Unknown RTK_BT_RFC_EVT: %d\r\n", __func__, evt_code);
		break;
	}
	}

	return RTK_BT_EVT_CB_OK;
}

/**
 * @brief  process rfc main init/ deinit.
 * @param  enable[in]: 1: init. 0 deinit.
 * @return none.
 */
int bt_rfc_main(uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_br_bd_addr_t bd_addr = {0};
	char addr_str[30] = {0};
	char dev_name[30] = {0};

	if (1 == enable) {
		bt_app_conf.server_chann = RTK_BT_RFC_DEMO_CHANN_NUM;
		//set GAP configuration
		bt_app_conf.app_profile_support = RTK_BT_PROFILE_RFC | \
										  RTK_BT_PROFILE_SDP;
		bt_app_conf.mtu_size = 180;
		bt_app_conf.max_tx_octets = 0x40;
		bt_app_conf.max_tx_time = 0x200;

		rtk_bt_br_gap_default_param_t dft_param;
		dft_param.mtu_size = 180;
		BT_APP_PROCESS(rtk_bt_br_gap_set_default_param(&dft_param));
		BT_APP_PROCESS(rtk_bt_br_gap_set_inquiry_param(&inquiry_param));
		BT_APP_PROCESS(rtk_bt_br_gap_set_page_param(&page_param));

		/* sdp record add should be invoked before bt enable */
		BT_APP_PROCESS(rtk_sdp_record_add((void *)rtk_vendor_rfc_sdp_record, sizeof(rtk_vendor_rfc_sdp_record)));

		/* Enable BT */
		BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
		BT_APP_PROCESS(rtk_bt_br_gap_set_radio_mode(RTK_BT_BR_GAP_RADIO_MODE_VISIBLE_CONNECTABLE));

		BT_APP_PROCESS(rtk_bt_br_gap_get_bd_addr(&bd_addr));
		rtk_bt_br_addr_to_str(bd_addr.addr, addr_str, sizeof(addr_str));
		BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

		/* Initialize GAP part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_GAP, br_gap_app_callback));
		/* mix RTK_BT_DEV_NAME with bt mac address */
		strcpy(dev_name, RTK_BT_DEV_NAME);
		snprintf(&dev_name[strlen(RTK_BT_DEV_NAME)], 7, "(%02X%02X)", bd_addr.addr[1], bd_addr.addr[0]);
		BT_APP_PROCESS(rtk_bt_br_gap_set_device_name((const uint8_t *)dev_name));
		/* Initilize SDP part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_SDP, rtk_bt_sdp_app_callback));
		/* Initilaize RFC part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_BR_GP_RFC, rtk_bt_rfc_app_callback));
	} else if (0 == enable) {
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_GAP));
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_SDP));
		BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_BR_GP_RFC));
		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
	}

	return 0;
}