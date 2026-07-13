/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_def.h>
#include <rtk_service_config.h>
#include <bt_utils.h>

#include "rtk_hids_gamepad.h"

/* ============================================================================
 *                           Definitions
 * ============================================================================ */

/** @brief Log tag for Gamepad HID Service */
#define TAG_GAMEPAD_HID  "[HIDS_GAMEPAD] "

/** @brief HID Service UUIDs */
#define HID_SRV_UUID                         0x1812
#define PROTOCOL_MODE_CHAR_UUID              0x2A4E
#define REPORT_CHAR_UUID                     0x2A4D
#define REPORT_MAP_CHAR_UUID                 0x2A4B
#define HID_INFO_CHAR_UUID                   0x2A4A
#define HID_CONTROL_POINT_CHAR_UUID          0x2A4C
#define REPORT_REFERENCE_DSCRP_UUID          0x2908

/** @brief UUID declarations using BT_UUID_DECLARE_16 */
#define RTK_BT_UUID_HID_SRV                  BT_UUID_DECLARE_16(HID_SRV_UUID)
#define RTK_BT_UUID_PROTOCOL_MODE_CHAR       BT_UUID_DECLARE_16(PROTOCOL_MODE_CHAR_UUID)
#define RTK_BT_UUID_REPORT_CHAR              BT_UUID_DECLARE_16(REPORT_CHAR_UUID)
#define RTK_BT_UUID_REPORT_MAP_CHAR          BT_UUID_DECLARE_16(REPORT_MAP_CHAR_UUID)
#define RTK_BT_UUID_HID_INFO_CHAR            BT_UUID_DECLARE_16(HID_INFO_CHAR_UUID)
#define RTK_BT_UUID_HID_CONTROL_POINT_CHAR   BT_UUID_DECLARE_16(HID_CONTROL_POINT_CHAR_UUID)
#define RTK_BT_UUID_REPORT_REFERENCE_DSCRP   BT_UUID_DECLARE_16(REPORT_REFERENCE_DSCRP_UUID)

/** @brief Attribute indices
 * @note GATT attribute table index mapping:
 *       - RTK_BT_GATT_PRIMARY_SERVICE = 1 entry (index 0)
 *       - RTK_BT_GATT_CHARACTERISTIC = 2 entries (declaration + value)
 *       - RTK_BT_GATT_CCC = 1 entry
 *       - RTK_BT_GATT_DESCRIPTOR = 1 entry
 */
#define PROTOCOL_MODE_CHAR_VAL_INDEX         2   /* Index 1=decl, 2=val */
#define REPORT_MAP_CHAR_VAL_INDEX            4   /* Index 3=decl, 4=val */
#define HID_INFO_CHAR_VAL_INDEX              6   /* Index 5=decl, 6=val */
#define HID_CONTROL_POINT_CHAR_VAL_INDEX     8   /* Index 7=decl, 8=val */
#define INPUT_REPORT_CHAR_VAL_INDEX          10  /* Index 9=decl, 10=val */
#define INPUT_REPORT_CCCD_INDEX              11  /* CCCD for Input Report */
#define OUTPUT_REPORT_CHAR_VAL_INDEX         14  /* Index 13=decl, 14=val - FIXED: was 13 */

/* ============================================================================
 *                           Local Variables
 * ============================================================================ */

/**
 * @brief Gamepad HID Report Descriptor
 * @note Defines a gamepad with 16 buttons, 4 analog axes, and 1 hat switch
 */
static const uint8_t hids_gamepad_report_descriptor[] = {
	/* Usage Page (Generic Desktop) */
	0x05, 0x01,
	/* Usage (Game Pad) */
	0x09, 0x05,
	/* Collection (Application) */
	0xA1, 0x01,

	/* Report ID 1: Input Report */
	0x85, GAMEPAD_REPORT_ID_INPUT,

	/* Usage Page (Button) */
	0x05, 0x09,
	/* Usage Minimum (Button 1) */
	0x19, 0x01,
	/* Usage Maximum (Button 16) */
	0x29, 0x10,
	/* Logical Minimum (0) */
	0x15, 0x00,
	/* Logical Maximum (1) */
	0x25, 0x01,
	/* Report Count (16) */
	0x95, 0x10,
	/* Report Size (1) */
	0x75, 0x01,
	/* Input (Data, Variable, Absolute) */
	0x81, 0x02,

	/* Usage Page (Generic Desktop) */
	0x05, 0x01,
	/* Usage (X) */
	0x09, 0x30,
	/* Usage (Y) */
	0x09, 0x31,
	/* Usage (Z) */
	0x09, 0x32,
	/* Usage (Rz) */
	0x09, 0x35,
	/* Logical Minimum (-127) */
	0x15, 0x81,
	/* Logical Maximum (127) */
	0x25, 0x7F,
	/* Report Size (8) */
	0x75, 0x08,
	/* Report Count (4) */
	0x95, 0x04,
	/* Input (Data, Variable, Absolute) */
	0x81, 0x02,

	/* Usage (Hat Switch) */
	0x09, 0x39,
	/* Logical Minimum (0) */
	0x15, 0x00,
	/* Logical Maximum (7) */
	0x25, 0x07,
	/* Physical Minimum (0) */
	0x35, 0x00,
	/* Physical Maximum (315) */
	0x46, 0x3B, 0x01,
	/* Unit (Degrees) */
	0x65, 0x14,
	/* Report Size (4) */
	0x75, 0x04,
	/* Report Count (1) */
	0x95, 0x01,
	/* Input (Data, Variable, Absolute, Null) */
	0x81, 0x42,

	/* Padding (4 bits) */
	0x75, 0x04,
	0x95, 0x01,
	0x81, 0x01,

	/* Padding (8 bits for alignment) */
	0x75, 0x08,
	0x95, 0x01,
	0x81, 0x01,

	/* End Collection */
	0xC0,

	/* Usage Page (Generic Desktop) */
	0x05, 0x01,
	/* Usage (Game Pad) */
	0x09, 0x05,
	/* Collection (Application) */
	0xA1, 0x01,

	/* Report ID 2: Output Report */
	0x85, GAMEPAD_REPORT_ID_OUTPUT,

	/* Usage Page (Generic Desktop) */
	0x05, 0x01,
	/* Usage (0x00 - Reserved) */
	0x09, 0x00,
	/* Logical Minimum (0) */
	0x15, 0x00,
	/* Logical Maximum (255) */
	0x26, 0xFF, 0x00,
	/* Report Size (8) */
	0x75, 0x08,
	/* Report Count (1) */
	0x95, 0x01,
	/* Output (Data, Variable, Absolute) */
	0x91, 0x02,

	/* End Collection */
	0xC0,
};

/** @brief HID Information default value */
static const hid_info_t gamepad_hid_information = {
	.bcd_hid = 0x0111,       /* HID version 1.11 */
	.b_country_code = 0x00,  /* Not localized */
	.flags = 0x02            /* Normally connectable */
};

/** @brief Report Reference values */
static uint16_t input_report_ref = HID_INPUT_TYPE << 8 | GAMEPAD_REPORT_ID_INPUT;
static uint16_t output_report_ref = HID_OUTPUT_TYPE << 8 | GAMEPAD_REPORT_ID_OUTPUT;

/** @brief Protocol mode */
static uint8_t gamepad_protocol_mode = GAMEPAD_PROTOCOL_MODE_REPORT;

/** @brief Control point (suspend mode) */
static uint8_t gamepad_suspend_mode = 0;

/** @brief Output report storage */
static gamepad_output_report_t gamepad_output_report = {0};

/** @brief Input report CCCD notification enable map */
static uint8_t input_report_cccd_ntf_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};

/** @brief Application callback */
static gamepad_hid_cb_t app_callback = NULL;

/* ============================================================================
 *                           GATT Attribute Table
 * ============================================================================ */

static rtk_bt_gatt_attr_t gamepad_hid_attrs[] = {
	/* 0: Primary Service: HID Service */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_HID_SRV),

	/* 1, 2: Characteristic: Protocol Mode (READ_ONLY, fixed Report Protocol Mode per HOGP v1.1 PTS) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_PROTOCOL_MODE_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ_ENCRYPT),

	/* 3, 4: Characteristic: Report Map */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_REPORT_MAP_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ_ENCRYPT),
	/* gamepad is no input no output iocap, so, it can't realize authen,
	just use permission encrypt, if use permission authen, host will disconnect the link */

	/* 5, 6: Characteristic: HID Information */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_HID_INFO_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ_ENCRYPT),

	/* 7, 8: Characteristic: HID Control Point (WRITE_ENCRYPT per HOGP spec) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_HID_CONTROL_POINT_CHAR,
							   RTK_BT_GATT_CHRC_WRITE_WITHOUT_RESP,
							   RTK_BT_GATT_PERM_WRITE_ENCRYPT),

	/* 9, 10: Characteristic: Input Report */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_REPORT_CHAR,
							   RTK_BT_GATT_CHRC_READ | RTK_BT_GATT_CHRC_NOTIFY,
							   RTK_BT_GATT_PERM_READ_ENCRYPT),
	/* 11: CCCD for Input Report */
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
	/* 12: Report Reference for Input Report */
	RTK_BT_GATT_DESCRIPTOR(RTK_BT_UUID_REPORT_REFERENCE_DSCRP,
						   RTK_BT_GATT_PERM_READ_ENCRYPT, &input_report_ref, 2, RTK_BT_GATT_INTERNAL),

	/* 13, 14: Characteristic: Output Report */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_REPORT_CHAR,
							   RTK_BT_GATT_CHRC_READ | RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_WRITE_WITHOUT_RESP,
							   RTK_BT_GATT_PERM_READ_ENCRYPT | RTK_BT_GATT_PERM_WRITE_ENCRYPT),
	/* 15: Report Reference for Output Report */
	RTK_BT_GATT_DESCRIPTOR(RTK_BT_UUID_REPORT_REFERENCE_DSCRP,
						   RTK_BT_GATT_PERM_READ_ENCRYPT, &output_report_ref, 2, RTK_BT_GATT_INTERNAL),
};

static struct rtk_bt_gatt_service gamepad_hid_srv = RTK_BT_GATT_SERVICE(gamepad_hid_attrs, HID_SRV_ID);

/* ============================================================================
 *                           Local Functions
 * ============================================================================ */

/**
 * @brief    Handle read request
 * @param[in] data Read indication data
 */
static void gamepad_hid_read_hdl(void *data)
{
	uint16_t ret = 0;
	rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
	rtk_bt_gatts_read_resp_param_t read_resp = {0};

	read_resp.app_id = p_read_ind->app_id;
	read_resp.conn_handle = p_read_ind->conn_handle;
	read_resp.cid = p_read_ind->cid;
	read_resp.index = p_read_ind->index;

	switch (p_read_ind->index) {
	case PROTOCOL_MODE_CHAR_VAL_INDEX:
		read_resp.data = &gamepad_protocol_mode;
		read_resp.len = sizeof(gamepad_protocol_mode);
		break;

	case REPORT_MAP_CHAR_VAL_INDEX: {
		uint16_t total_len = sizeof(hids_gamepad_report_descriptor);
		uint16_t offset = p_read_ind->offset;
		uint16_t actual_len = total_len - offset;

		if (offset >= total_len) {
			read_resp.err_code = RTK_BT_ATT_ERR_INVALID_OFFSET;
			break;
		}

		read_resp.data = (uint8_t *)hids_gamepad_report_descriptor + offset;
		read_resp.len = actual_len;
		break;
	}

	case HID_INFO_CHAR_VAL_INDEX:
		read_resp.data = (uint8_t *)&gamepad_hid_information;
		read_resp.len = sizeof(hid_info_t);
		break;

	case OUTPUT_REPORT_CHAR_VAL_INDEX:
		read_resp.data = (uint8_t *)&gamepad_output_report;
		read_resp.len = sizeof(gamepad_output_report_t);
		break;

	default:
		read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
		break;
	}

	ret = rtk_bt_gatts_read_resp(&read_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA(TAG_GAMEPAD_HID "Read response succeed, index: %d\r\n", p_read_ind->index);
	} else {
		BT_LOGE(TAG_GAMEPAD_HID "Read response failed, err: 0x%x\r\n", ret);
	}
}

/**
 * @brief    Handle write request
 * @param[in] data Write indication data
 */
static void gamepad_hid_write_hdl(void *data)
{
	uint16_t ret = 0;
	rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
	rtk_bt_gatts_write_resp_param_t write_resp = {0};
	gamepad_hid_cb_data_t cb_data;
	void *p_value = p_write_ind->value;
	uint16_t len = p_write_ind->len;

	write_resp.app_id = p_write_ind->app_id;
	write_resp.conn_handle = p_write_ind->conn_handle;
	write_resp.cid = p_write_ind->cid;
	write_resp.index = p_write_ind->index;
	write_resp.type = p_write_ind->type;

	if (!len || !p_value) {
		BT_LOGE(TAG_GAMEPAD_HID "Write value is empty!\r\n");
		write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
		goto send_rsp;
	}

	switch (p_write_ind->index) {
	case HID_CONTROL_POINT_CHAR_VAL_INDEX:
		if (len >= 1) {
			uint8_t command = *(uint8_t *)p_value;
			if (command == HID_CONTROL_POINT_SUSPEND || command == HID_CONTROL_POINT_EXIT_SUSPEND) {
				gamepad_suspend_mode = command;
				BT_LOGA(TAG_GAMEPAD_HID "Control Point: %s\r\n",
						command == HID_CONTROL_POINT_SUSPEND ? "Suspend" : "Exit Suspend");

				if (app_callback) {
					cb_data.event = GAMEPAD_HID_EVT_CONTROL_POINT;
					cb_data.conn_id = 0;
					cb_data.data.control_point = command;
					app_callback(&cb_data);
				}
			} else {
				BT_LOGE(TAG_GAMEPAD_HID "Invalid control point command: 0x%02x\r\n", command);
			}
		}
		break;

	case OUTPUT_REPORT_CHAR_VAL_INDEX:
		if (len >= sizeof(gamepad_output_report_t)) {
			memcpy(&gamepad_output_report, p_value, sizeof(gamepad_output_report_t));
			BT_LOGA(TAG_GAMEPAD_HID "Output Report received: conn_handle=0x%04x, len=%d, data=0x%02x (vibration=%d)\r\n",
					p_write_ind->conn_handle, len, gamepad_output_report.vibration, gamepad_output_report.vibration);

			if (app_callback) {
				cb_data.event = GAMEPAD_HID_EVT_OUTPUT_REPORT_WRITE;
				cb_data.conn_id = 0;
				memcpy(&cb_data.data.output, &gamepad_output_report, sizeof(gamepad_output_report_t));
				app_callback(&cb_data);
			}
		}
		break;

	default:
		write_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
		break;
	}

send_rsp:
	/* Don't send response for WRITE_NO_RSP */
	if (RTK_BT_GATTS_WRITE_NO_RESP == p_write_ind->type ||
		RTK_BT_GATTS_WRITE_NO_RESP_SIGNED == p_write_ind->type) {
		return;
	}

	ret = rtk_bt_gatts_write_resp(&write_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA(TAG_GAMEPAD_HID "Write response succeed, index: %d\r\n", p_write_ind->index);
	} else {
		BT_LOGE(TAG_GAMEPAD_HID "Write response failed, err: 0x%x\r\n", ret);
	}
}

/**
 * @brief    Handle CCCD update
 * @param[in] data CCCD indication data
 */
static void gamepad_hid_cccd_hdl(void *data)
{
	rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;
	gamepad_hid_cb_data_t cb_data;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(p_cccd_ind->conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	switch (p_cccd_ind->index) {
	case INPUT_REPORT_CCCD_INDEX:
		if (p_cccd_ind->value & RTK_BT_GATT_CCC_NOTIFY) {
			input_report_cccd_ntf_en_map[conn_id] = 1;
			BT_LOGA(TAG_GAMEPAD_HID "Input Report notifications enabled\r\n");

			if (app_callback) {
				cb_data.event = GAMEPAD_HID_EVT_CCCD_ENABLE;
				cb_data.conn_id = conn_id;
				app_callback(&cb_data);
			}
		} else {
			input_report_cccd_ntf_en_map[conn_id] = 0;
			BT_LOGA(TAG_GAMEPAD_HID "Input Report notifications disabled\r\n");

			if (app_callback) {
				cb_data.event = GAMEPAD_HID_EVT_CCCD_DISABLE;
				cb_data.conn_id = conn_id;
				app_callback(&cb_data);
			}
		}
		break;

	default:
		break;
	}
}

/* ============================================================================
 *                           Service Callback
 * ============================================================================ */

void gamepad_hid_srv_callback(uint8_t event, void *data)
{
	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *p_reg_ind = (rtk_bt_gatts_reg_ind_t *)data;
		if (RTK_BT_OK == p_reg_ind->reg_status) {
			BT_LOGA(TAG_GAMEPAD_HID "Register service succeed!\r\n");
		} else {
			BT_LOGE(TAG_GAMEPAD_HID "Register service failed, err: 0x%x\r\n", p_reg_ind->reg_status);
		}
		break;
	}

	case RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ntf_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		if (RTK_BT_OK == p_ntf_ind->err_code) {
			BT_LOGA(TAG_GAMEPAD_HID "Notify succeed!\r\n");
		} else {
			BT_LOGE(TAG_GAMEPAD_HID "Notify failed, err: 0x%x\r\n", p_ntf_ind->err_code);
		}
		break;
	}

	case RTK_BT_GATTS_EVT_READ_IND:
		gamepad_hid_read_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_WRITE_IND:
		gamepad_hid_write_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_CCCD_IND:
		gamepad_hid_cccd_hdl(data);
		break;

	default:
		break;
	}
}

/* ============================================================================
 *                           Public Functions
 * ============================================================================ */

/**
 * @brief    Add Gamepad HID Service
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t gamepad_hid_srv_add(void)
{
	gamepad_hid_srv.type = GATT_SERVICE_OVER_BLE;
	gamepad_hid_srv.server_info = 0;
	gamepad_hid_srv.user_data = NULL;
	gamepad_hid_srv.register_status = 0;

	return rtk_bt_gatts_register_service(&gamepad_hid_srv);
}

/**
 * @brief    Register Gamepad HID Service callback
 * @param[in] callback Callback function
 */
void gamepad_hid_srv_register_callback(gamepad_hid_cb_t callback)
{
	app_callback = callback;
}

/**
 * @brief    Send Input Report notification
 * @param[in] conn_handle Connection handle
 * @param[in] p_report Pointer to input report data
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t gamepad_hid_srv_send_input_report(uint16_t conn_handle,
										   const gamepad_input_report_t *p_report)
{
	rtk_bt_gatts_ntf_and_ind_param_t ntf_param = {0};
	uint8_t conn_id;

	if (p_report == NULL) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	if (!input_report_cccd_ntf_en_map[conn_id]) {
		BT_LOGE(TAG_GAMEPAD_HID "Notification not enabled\r\n");
		return RTK_BT_ERR_UNSUPPORTED;
	}

	ntf_param.app_id = HID_SRV_ID;
	ntf_param.conn_handle = conn_handle;
	ntf_param.index = INPUT_REPORT_CHAR_VAL_INDEX;
	ntf_param.data = (uint8_t *)p_report;
	ntf_param.len = sizeof(gamepad_input_report_t);
	ntf_param.seq = 0;

	return rtk_bt_gatts_notify(&ntf_param);
}


/**
 * @brief    Get current Protocol Mode
 * @return   Current protocol mode
 */
uint8_t gamepad_hid_srv_get_protocol_mode(void)
{
	return gamepad_protocol_mode;
}

/**
 * @brief    Check if notification is enabled
 * @param[in] conn_handle Connection handle
 * @return   true if enabled, false otherwise
 */
bool gamepad_hid_srv_is_notify_enabled(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return false;
	}

	return input_report_cccd_ntf_en_map[conn_id] != 0;
}

/**
 * @brief    Handle disconnect for Gamepad HID Service
 * @param[in] conn_handle Connection handle
 */
void gamepad_hid_srv_disconnect(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	input_report_cccd_ntf_en_map[conn_id] = 0;
	BT_LOGA(TAG_GAMEPAD_HID "Disconnected\r\n");
}

/**
 * @brief    Deinitialize Gamepad HID Service
 */
void gamepad_hid_srv_deinit(void)
{
	memset(input_report_cccd_ntf_en_map, 0, sizeof(input_report_cccd_ntf_en_map));
	gamepad_protocol_mode = GAMEPAD_PROTOCOL_MODE_REPORT;
	gamepad_suspend_mode = 0;
	memset(&gamepad_output_report, 0, sizeof(gamepad_output_report_t));
	app_callback = NULL;
	BT_LOGA(TAG_GAMEPAD_HID "Service deinitialized\r\n");
}
