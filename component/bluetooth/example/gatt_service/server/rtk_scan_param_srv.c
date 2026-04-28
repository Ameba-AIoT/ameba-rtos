/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_service_config.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_def.h>
#include <bt_utils.h>
#include "rtk_scan_param_srv.h"

/* ============================================================================
 *                           Definitions
 * ============================================================================ */

#define SCAN_PARAM_SRV_UUID                    0x1813
#define SCAN_INTERVAL_WINDOW_UUID              0x2A4F
#define SCAN_REFRESH_UUID                      0x2A31

/* Use local UUID declarations for GATT attribute table */
#define SCAN_PARAM_SRV_UUID_DECL               BT_UUID_DECLARE_16(SCAN_PARAM_SRV_UUID)
#define SCAN_INTERVAL_WINDOW_UUID_DECL         BT_UUID_DECLARE_16(SCAN_INTERVAL_WINDOW_UUID)
#define SCAN_REFRESH_UUID_DECL                 BT_UUID_DECLARE_16(SCAN_REFRESH_UUID)

#define SCAN_INTERVAL_WINDOW_INDEX             2
#define SCAN_REFRESH_INDEX                     4
#define SCAN_REFRESH_CCCD_INDEX                5

/* ============================================================================
 *                           Local Variables
 * ============================================================================ */

/** @brief Scan parameters storage per connection */
static scan_interval_window_t scan_params_storage[RTK_BLE_GAP_MAX_LINKS] = {0};

/** @brief Scan Refresh CCCD notification enable map */
static uint8_t scan_refresh_cccd_ntf_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};

/** @brief Application callback */
static scan_param_cb_t app_callback = NULL;

/** @brief Flag indicating if service has been added */
static bool scan_param_srv_added = false;

/* ============================================================================
 *                           GATT Attribute Table
 * ============================================================================ */

static rtk_bt_gatt_attr_t scan_param_srv_attrs[] = {
	/* 0: Primary Service: Scan Parameters Service */
	RTK_BT_GATT_PRIMARY_SERVICE(SCAN_PARAM_SRV_UUID_DECL),

	/* 1, 2: Characteristic: Scan Interval Window
	 * Per Scan Parameters Service spec: WriteWithoutResponse only.
	 */
	RTK_BT_GATT_CHARACTERISTIC(SCAN_INTERVAL_WINDOW_UUID_DECL,
							   RTK_BT_GATT_CHRC_WRITE_WITHOUT_RESP,
							   RTK_BT_GATT_PERM_WRITE),

	/* 3, 4: Characteristic: Scan Refresh */
	RTK_BT_GATT_CHARACTERISTIC(SCAN_REFRESH_UUID_DECL,
							   RTK_BT_GATT_CHRC_NOTIFY,
							   RTK_BT_GATT_PERM_NONE),
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
};

static struct rtk_bt_gatt_service scan_param_srv = RTK_BT_GATT_SERVICE(scan_param_srv_attrs, SCAN_PARAM_SRV_ID);

/* ============================================================================
 *                           Local Functions
 * ============================================================================ */

/**
 * @brief    Validate scan parameters
 * @param[in] p_params Pointer to scan parameters
 * @return   true if valid, false otherwise
 */
static bool validate_scan_params(const scan_interval_window_t *p_params)
{
	if (p_params->le_scan_interval < SCAN_INTERVAL_MIN ||
		p_params->le_scan_interval > SCAN_INTERVAL_MAX) {
		return false;
	}

	if (p_params->le_scan_window < SCAN_WINDOW_MIN ||
		p_params->le_scan_window > SCAN_WINDOW_MAX) {
		return false;
	}

	/* Scan Window shall be less than or equal to Scan Interval */
	if (p_params->le_scan_window > p_params->le_scan_interval) {
		return false;
	}

	return true;
}

/* ============================================================================
 *                           Service Callback
 * ============================================================================ */

void scan_param_srv_callback(uint8_t event, void *data)
{
	switch (event) {
	case RTK_BT_GATTS_EVT_READ_IND: {
		/* Scan Interval Window is WRITE_ONLY per Scan Parameters Service spec.
		 * CCCD reads are handled internally by the stack. */
		rtk_bt_gatts_read_ind_t *p_read = (rtk_bt_gatts_read_ind_t *)data;
		rtk_bt_gatts_read_resp_param_t read_resp = {0};

		read_resp.app_id = p_read->app_id;
		read_resp.conn_handle = p_read->conn_handle;
		read_resp.cid = p_read->cid;
		read_resp.index = p_read->index;
		read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;

		rtk_bt_gatts_read_resp(&read_resp);
		break;
	}

	case RTK_BT_GATTS_EVT_WRITE_IND: {
		rtk_bt_gatts_write_ind_t *p_write = (rtk_bt_gatts_write_ind_t *)data;
		rtk_bt_gatts_write_resp_param_t write_resp = {0};
		scan_param_cb_data_t cb_data;
		uint8_t conn_id;

		if (rtk_bt_le_gap_get_conn_id(p_write->conn_handle, &conn_id) != RTK_BT_OK) {
			BT_LOGE("[SCAN_PARAM_SRV] rtk_bt_le_gap_get_conn_id:%d failed\r\n", conn_id);
			return;
		}

		/* Prepare response */
		write_resp.app_id = p_write->app_id;
		write_resp.conn_handle = p_write->conn_handle;
		write_resp.cid = p_write->cid;
		write_resp.index = p_write->index;
		write_resp.type = p_write->type;

		if (p_write->index == SCAN_INTERVAL_WINDOW_INDEX) {
			/* Write Scan Interval Window value */
			if (p_write->len != sizeof(scan_interval_window_t)) {
				BT_LOGE("[SCAN_PARAM_SRV] Invalid scan interval window length: %d\r\n", p_write->len);
				write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE;
				rtk_bt_gatts_write_resp(&write_resp);
				break;
			}

			scan_interval_window_t *p_params = (scan_interval_window_t *)p_write->value;

			if (!validate_scan_params(p_params)) {
				BT_LOGE("[SCAN_PARAM_SRV] Invalid scan parameters: interval=0x%04x, window=0x%04x\r\n",
						p_params->le_scan_interval, p_params->le_scan_window);
				write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE;
				rtk_bt_gatts_write_resp(&write_resp);
				break;
			}

			/* Store scan parameters */
			memcpy(&scan_params_storage[conn_id], p_params, sizeof(scan_interval_window_t));

			BT_LOGA("[SCAN_PARAM_SRV] Scan Interval Window: interval=0x%04x (%d ms), window=0x%04x (%d ms) \r\n",
					p_params->le_scan_interval,
					(int)(p_params->le_scan_interval * 0.625),
					p_params->le_scan_window,
					(int)(p_params->le_scan_window * 0.625));

			/* Notify application */
			if (app_callback) {
				cb_data.event = SCAN_PARAM_EVT_SCAN_INTERVAL_WINDOW_WRITE;
				cb_data.conn_id = conn_id;
				memcpy(&cb_data.scan_params, p_params, sizeof(scan_interval_window_t));
				app_callback(&cb_data);
			}

			rtk_bt_gatts_write_resp(&write_resp);
		} else if (p_write->index == SCAN_REFRESH_CCCD_INDEX) {
			/* Write Scan Refresh CCCD */
			if (p_write->len != 2) {
				rtk_bt_gatts_write_resp(&write_resp);
				break;
			}

			uint16_t cccd_value = p_write->value[0] | (p_write->value[1] << 8);

			if (cccd_value == RTK_BT_GATT_CCC_NOTIFY) {
				scan_refresh_cccd_ntf_en_map[conn_id] = 1;
				BT_LOGA("[SCAN_PARAM_SRV] Scan Refresh notifications enabled\r\n");

				if (app_callback) {
					cb_data.event = SCAN_PARAM_EVT_SCAN_REFRESH_CCCD_ENABLE;
					cb_data.conn_id = conn_id;
					app_callback(&cb_data);
				}
			} else if (cccd_value == 0x0000) {
				scan_refresh_cccd_ntf_en_map[conn_id] = 0;
				BT_LOGA("[SCAN_PARAM_SRV] Scan Refresh notifications disabled\r\n");

				if (app_callback) {
					cb_data.event = SCAN_PARAM_EVT_SCAN_REFRESH_CCCD_DISABLE;
					cb_data.conn_id = conn_id;
					app_callback(&cb_data);
				}
			}

			rtk_bt_gatts_write_resp(&write_resp);
		} else {
			rtk_bt_gatts_write_resp(&write_resp);
		}
		break;
	}

	case RTK_BT_GATTS_EVT_CCCD_IND: {
		rtk_bt_gatts_cccd_ind_t *p_cccd = (rtk_bt_gatts_cccd_ind_t *)data;
		scan_param_cb_data_t cb_data;
		uint8_t conn_id;

		if (rtk_bt_le_gap_get_conn_id(p_cccd->conn_handle, &conn_id) != RTK_BT_OK) {
			return;
		}

		if (p_cccd->index == SCAN_REFRESH_CCCD_INDEX) {
			if (p_cccd->value & RTK_BT_GATT_CCC_NOTIFY) {
				scan_refresh_cccd_ntf_en_map[conn_id] = 1;
				BT_LOGA("[SCAN_PARAM_SRV] Scan Refresh notifications enabled\r\n");

				if (app_callback) {
					cb_data.event = SCAN_PARAM_EVT_SCAN_REFRESH_CCCD_ENABLE;
					cb_data.conn_id = conn_id;
					app_callback(&cb_data);
				}
			} else {
				scan_refresh_cccd_ntf_en_map[conn_id] = 0;
				BT_LOGA("[SCAN_PARAM_SRV] Scan Refresh notifications disabled\r\n");

				if (app_callback) {
					cb_data.event = SCAN_PARAM_EVT_SCAN_REFRESH_CCCD_DISABLE;
					cb_data.conn_id = conn_id;
					app_callback(&cb_data);
				}
			}
		}
		break;
	}

	default:
		break;
	}
}

/* ============================================================================
 *                           Public Functions
 * ============================================================================ */

/**
 * @brief    Add Scan Parameters Service
 * @return   RTK_BT_OK on success, error code otherwise
 * @note     Only one Scan Parameters Service instance is allowed per Server
 */
uint16_t scan_param_srv_add(void)
{
	/* Check if service already added (single instance constraint per spec) */
	if (scan_param_srv_added) {
		BT_LOGE("[SCAN_PARAM_SRV] Scan Parameters Service already added (only one instance allowed)\r\n");
		return RTK_BT_FAIL;
	}

	scan_param_srv.type = GATT_SERVICE_OVER_BLE;
	scan_param_srv.server_info = 0;
	scan_param_srv.user_data = NULL;
	scan_param_srv.register_status = 0;

	uint16_t ret = rtk_bt_gatts_register_service(&scan_param_srv);
	if (ret != RTK_BT_OK) {
		BT_LOGE("[SCAN_PARAM_SRV] Failed to register Scan Parameters Service\r\n");
		return ret;
	}

	scan_param_srv_added = true;
	BT_LOGA("[SCAN_PARAM_SRV] Scan Parameters Service added\r\n");

	return RTK_BT_OK;
}

/**
 * @brief    Register Scan Parameters Service callback
 * @param[in] callback Callback function
 */
void scan_param_srv_register_callback(scan_param_cb_t callback)
{
	app_callback = callback;
}

/**
 * @brief    Request scan parameters refresh
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_srv_request_refresh(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	if (!scan_refresh_cccd_ntf_en_map[conn_id]) {
		BT_LOGE("[SCAN_PARAM_SRV] Scan Refresh notification not enabled\r\n");
		return RTK_BT_FAIL;
	}

	/* Send Scan Refresh notification with value 0x00 (Info Request) */
	uint8_t refresh_value = SCAN_REFRESH_INFO_REQ;

	rtk_bt_gatts_ntf_and_ind_param_t ntf_param = {0};
	ntf_param.app_id = SCAN_PARAM_SRV_ID;
	ntf_param.conn_handle = conn_handle;
	ntf_param.index = SCAN_REFRESH_INDEX;
	ntf_param.data = &refresh_value;
	ntf_param.len = 1;
	ntf_param.seq = 0;

	uint16_t ret = rtk_bt_gatts_notify(&ntf_param);
	if (ret == RTK_BT_OK) {
		BT_LOGA("[SCAN_PARAM_SRV] Scan Refresh notification sent\r\n");
	}

	return ret;
}

/**
 * @brief    Get current scan parameters
 * @param[in] conn_handle Connection handle
 * @param[out] p_params Pointer to store scan parameters
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_srv_get_scan_params(uint16_t conn_handle, scan_interval_window_t *p_params)
{
	uint8_t conn_id;

	if (p_params == NULL) {
		return RTK_BT_FAIL;
	}

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	memcpy(p_params, &scan_params_storage[conn_id], sizeof(scan_interval_window_t));
	return RTK_BT_OK;
}

/**
 * @brief    Check if Scan Refresh notification is enabled
 * @param[in] conn_handle Connection handle
 * @return   true if enabled, false otherwise
 */
bool scan_param_srv_is_refresh_notify_enabled(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return false;
	}

	return scan_refresh_cccd_ntf_en_map[conn_id] != 0;
}

/**
 * @brief    Handle connect for Scan Parameters Service
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_srv_connect(uint16_t conn_handle)
{
	(void)conn_handle;
	/* Connection tracking is handled by conn_id in the storage arrays */
	BT_LOGA("[SCAN_PARAM_SRV] Connected, conn_handle=0x%04x\r\n", conn_handle);
	return RTK_BT_OK;
}

/**
 * @brief    Handle disconnect for Scan Parameters Service
 * @param[in] conn_handle Connection handle
 */
void scan_param_srv_disconnect(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	/* Clear scan parameters and CCCD state for this connection */
	memset(&scan_params_storage[conn_id], 0, sizeof(scan_interval_window_t));
	scan_refresh_cccd_ntf_en_map[conn_id] = 0;

	BT_LOGA("[SCAN_PARAM_SRV] Disconnected\r\n");
}

/**
 * @brief    Deinitialize Scan Parameters Service
 */
void scan_param_srv_deinit(void)
{
	memset(scan_params_storage, 0, sizeof(scan_params_storage));
	memset(scan_refresh_cccd_ntf_en_map, 0, sizeof(scan_refresh_cccd_ntf_en_map));
	scan_param_srv_added = false;
	app_callback = NULL;
	BT_LOGA("[SCAN_PARAM_SRV] Scan Parameters Service deinitialized\r\n");
}
