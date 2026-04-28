/**
 * @file      rtk_scan_param_client.c
 * @author    atonio_liu@realsil.com.cn
 * @brief     Scan Parameters Service Client implementation
 * @copyright Copyright (c) 2026. Realtek Semiconductor Corporation. All rights reserved.
 *
 * This file implements the Scan Parameters Service Client as defined in the
 * Scan Parameters Service Specification v1.0.
 *
 * The Scan Parameters Client writes its scan parameters to the Server
 * (Broadcaster) to allow the Server to optimize its advertising and
 * connection parameters.
 */

#include <stdio.h>
#include <string.h>
#include <osif.h>

#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_def.h>
#include <bt_debug.h>
#include "rtk_client_config.h"
#include "rtk_scan_param_client.h"
#include "rtk_gcs_client.h"  /* For general_client_discover_res_hdl() */

/* Enable detailed discovery logging */
#define SCAN_PARAM_CLIENT_SHOW_DETAIL   1

/* ============================================================================
 *                           Definitions
 * ============================================================================ */

/** @brief Log tag for Scan Parameters Client */
#define TAG_SCAN_PARAM_CLIENT  "[SCAN_PARAM_CLIENT]"

/** @brief Maximum number of connections supported */
#define SCAN_PARAM_MAX_LINKS    RTK_BLE_GAP_MAX_LINKS

/* Scan Parameters Service UUID */
#define SCAN_PARAM_SRV_UUID    0x1813

/* Characteristic UUIDs */
#define SCAN_INTERVAL_WINDOW_UUID    0x2A4F
#define SCAN_REFRESH_UUID            0x2A31

/* ============================================================================
 *                           Type Definitions
 * ============================================================================ */

/**
 * @brief Discovery state machine
 */
typedef enum {
	SCAN_PARAM_DISC_IDLE = 0,       /**< Idle state */
	SCAN_PARAM_DISC_SRV,            /**< Discovering service */
	SCAN_PARAM_DISC_CHAR,           /**< Discovering characteristics */
	SCAN_PARAM_DISC_CHAR_DONE,      /**< Character discovery done */
	SCAN_PARAM_DISC_DESC,           /**< Discovering descriptors */
	SCAN_PARAM_DISC_DONE,           /**< Discovery complete */
} scan_param_disc_state_t;

/**
 * @brief Scan Parameters Client database per connection
 */
typedef struct {
	uint16_t conn_handle;                   /**< Connection handle */
	scan_param_disc_state_t disc_state;     /**< Discovery state */
	uint16_t start_handle;                  /**< Service start handle */
	uint16_t end_handle;                    /**< Service end handle */

	/* Characteristic handles */
	uint16_t scan_interval_window_handle;   /**< Scan Interval Window handle */
	uint16_t scan_refresh_handle;           /**< Scan Refresh handle */
	uint16_t scan_refresh_cccd_handle;      /**< Scan Refresh CCCD handle */

	/* Service flags */
	bool service_found;                     /**< Service discovered flag */
	bool refresh_notify_en;                 /**< Refresh notification enabled */
} scan_param_client_db_t;

/* ============================================================================
 *                           Local Variables
 * ============================================================================ */

/** @brief Scan Parameters client database */
static scan_param_client_db_t *scan_param_database[SCAN_PARAM_MAX_LINKS] = {0};

/** @brief Application callback */
static scan_param_client_cb_t app_callback = NULL;

/* ============================================================================
 *                           Local Functions
 * ============================================================================ */

/**
 * @brief    Get connection database by connection handle
 * @param[in] conn_handle Connection handle
 * @return   Pointer to database or NULL if not found
 */
static scan_param_client_db_t *get_conn_db(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return NULL;
	}

	if (conn_id >= SCAN_PARAM_MAX_LINKS) {
		return NULL;
	}

	return scan_param_database[conn_id];
}

/**
 * @brief    Get or create connection database
 * @param[in] conn_handle Connection handle
 * @return   Pointer to database or NULL if failed
 */
static scan_param_client_db_t *get_or_create_conn_db(uint16_t conn_handle)
{
	uint8_t conn_id;
	scan_param_client_db_t *p_db;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return NULL;
	}

	if (conn_id >= SCAN_PARAM_MAX_LINKS) {
		return NULL;
	}

	p_db = scan_param_database[conn_id];
	if (!p_db) {
		p_db = (scan_param_client_db_t *)osif_mem_alloc(RAM_TYPE_DATA_ON,
														sizeof(scan_param_client_db_t));
		if (!p_db) {
			return NULL;
		}
		memset(p_db, 0, sizeof(scan_param_client_db_t));
		p_db->conn_handle = conn_handle;
		scan_param_database[conn_id] = p_db;
	}

	return p_db;
}

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Handle GATT discover result
 * @param[in] data Discover result data
 */
static void scan_param_client_discover_hdl(void *data)
{
	rtk_bt_gattc_discover_ind_t *p_ind = (rtk_bt_gattc_discover_ind_t *)data;
	scan_param_client_db_t *p_db = get_conn_db(p_ind->conn_handle);

	if (p_db == NULL) {
		return;
	}

	/* Handle DONE status */
	if (p_ind->status == RTK_BT_STATUS_DONE) {
		switch (p_ind->type) {
		case RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID: {
			if (!p_db->service_found) {
				BT_LOGE(TAG_SCAN_PARAM_CLIENT "Scan Parameters Service not found\r\n");
				p_db->disc_state = SCAN_PARAM_DISC_IDLE;
				goto log_exit;
			}

			/* Start characteristic discovery */
			rtk_bt_gattc_discover_param_t param = {0};
			param.profile_id = SCAN_PARAM_CLIENT_PROFILE_ID;
			param.conn_handle = p_ind->conn_handle;
			param.type = RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL;
			param.disc_char_all.start_handle = p_db->start_handle;
			param.disc_char_all.end_handle = p_db->end_handle;

			if (rtk_bt_gattc_discover(&param) == RTK_BT_OK) {
				p_db->disc_state = SCAN_PARAM_DISC_CHAR;
			} else {
				p_db->disc_state = SCAN_PARAM_DISC_IDLE;
			}
			break;
		}

		case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL: {
			/* Start descriptor discovery */
			rtk_bt_gattc_discover_param_t param = {0};
			param.profile_id = SCAN_PARAM_CLIENT_PROFILE_ID;
			param.conn_handle = p_ind->conn_handle;
			param.type = RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL;
			param.disc_descriptor.start_handle = p_db->scan_refresh_handle + 1;
			param.disc_descriptor.end_handle = p_db->end_handle;

			if (rtk_bt_gattc_discover(&param) == RTK_BT_OK) {
				p_db->disc_state = SCAN_PARAM_DISC_DESC;
			} else {
				p_db->disc_state = SCAN_PARAM_DISC_IDLE;
			}
			break;
		}

		case RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL:
			/* All discovery complete */
			p_db->disc_state = SCAN_PARAM_DISC_DONE;
			BT_LOGA(TAG_SCAN_PARAM_CLIENT "Discovery complete\r\n");

			/* Notify application */
			if (app_callback) {
				uint8_t conn_id;
				if (rtk_bt_le_gap_get_conn_id(p_ind->conn_handle, &conn_id) == RTK_BT_OK) {
					app_callback(conn_id, SCAN_PARAM_CLIENT_EVT_DISCOVERY_COMPLETE, NULL, 0);
				}
			}
			break;

		default:
			break;
		}
		goto log_exit;
	}

	/* Handle FAIL status */
	if (p_ind->status == RTK_BT_STATUS_FAIL) {
		BT_LOGE(TAG_SCAN_PARAM_CLIENT "Discovery failed, err: 0x%x\r\n", p_ind->err_code);
		p_db->disc_state = SCAN_PARAM_DISC_IDLE;
		goto log_exit;
	}

	/* Handle CONTINUE status */
	if (p_ind->status != RTK_BT_STATUS_CONTINUE) {
		return;
	}

	/* Process discovery result */
	if (p_ind->type == RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID) {
		/* Service found */
		p_db->start_handle = p_ind->disc_primary_by_uuid_per.start_handle;
		p_db->end_handle = p_ind->disc_primary_by_uuid_per.end_handle;
		p_db->service_found = true;
		p_db->disc_state = SCAN_PARAM_DISC_CHAR;

		BT_LOGA(TAG_SCAN_PARAM_CLIENT "Scan Parameters Service found: 0x%04x-0x%04x\r\n",
				p_db->start_handle, p_db->end_handle);
	} else if (p_ind->type == RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL) {
		/* Process characteristic */
		uint16_t uuid = 0;
		if (p_ind->disc_char_all_per.uuid_type == BT_UUID_TYPE_16) {
			uuid = *(uint16_t *)p_ind->disc_char_all_per.uuid;
		}

		if (uuid == SCAN_INTERVAL_WINDOW_UUID) {
			p_db->scan_interval_window_handle = p_ind->disc_char_all_per.value_handle;
			BT_LOGA(TAG_SCAN_PARAM_CLIENT "Scan Interval Window: 0x%04x\r\n",
					p_db->scan_interval_window_handle);
		} else if (uuid == SCAN_REFRESH_UUID) {
			p_db->scan_refresh_handle = p_ind->disc_char_all_per.value_handle;
			BT_LOGA(TAG_SCAN_PARAM_CLIENT "Scan Refresh: 0x%04x\r\n",
					p_db->scan_refresh_handle);
		}
	} else if (p_ind->type == RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL) {
		/* Process descriptor - check for CCCD */
		uint16_t uuid = 0;
		if (p_ind->disc_descriptor_per.uuid_type == BT_UUID_TYPE_16) {
			uuid = *(uint16_t *)p_ind->disc_descriptor_per.uuid;
		}

		if (uuid == BT_UUID_GATT_CCC_VAL) {
			p_db->scan_refresh_cccd_handle = p_ind->disc_descriptor_per.handle;
			BT_LOGA(TAG_SCAN_PARAM_CLIENT "Scan Refresh CCCD: 0x%04x\r\n",
					p_db->scan_refresh_cccd_handle);
		}
	}
	goto log_exit;

log_exit:
#if defined(SCAN_PARAM_CLIENT_SHOW_DETAIL) && SCAN_PARAM_CLIENT_SHOW_DETAIL
	general_client_discover_res_hdl(data);
#endif
	return;
}

#endif /* #if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB */

/**
 * @brief    Handle GATT write result
 * @param[in] data Write result data
 */
static void scan_param_client_write_result_hdl(void *data)
{
	rtk_bt_gattc_write_ind_t *p_ind = (rtk_bt_gattc_write_ind_t *)data;

	if (p_ind->status != RTK_BT_STATUS_DONE) {
		BT_LOGE(TAG_SCAN_PARAM_CLIENT "Write failed, handle: 0x%04x, err: 0x%x\r\n",
				p_ind->handle, p_ind->err_code);
	} else {
		BT_LOGA(TAG_SCAN_PARAM_CLIENT "Write succeed, handle: 0x%04x\r\n", p_ind->handle);
	}
}

/**
 * @brief    Handle notification
 * @param[in] data Notification data
 */
static void scan_param_client_notify_hdl(void *data)
{
	rtk_bt_gattc_cccd_value_ind_t *p_ind = (rtk_bt_gattc_cccd_value_ind_t *)data;
	scan_param_client_db_t *p_db = get_conn_db(p_ind->conn_handle);

	if (!p_db) {
		BT_LOGE(TAG_SCAN_PARAM_CLIENT "p_db is NULL\r\n");
		return;
	}

	/* Check if this is Scan Refresh notification */
	if (p_ind->value_handle == p_db->scan_refresh_handle) {
		uint8_t refresh_value = 0;
		if (p_ind->len > 0) {
			refresh_value = p_ind->value[0];
		}

		BT_LOGA(TAG_SCAN_PARAM_CLIENT "Scan Refresh notification: 0x%02x\r\n", refresh_value);

		/* Notify application */
		if (app_callback) {
			uint8_t conn_id;
			if (rtk_bt_le_gap_get_conn_id(p_ind->conn_handle, &conn_id) == RTK_BT_OK) {
				app_callback(conn_id, SCAN_PARAM_CLIENT_EVT_SCAN_REFRESH_NOTIFY,
							 &refresh_value, sizeof(uint8_t));
			}
		}
	} else {
		BT_LOGE(TAG_SCAN_PARAM_CLIENT "mismatch!!  handle: 0x%x, scan_refresh_handle 0x%x\r\n",
				p_ind->value_handle, p_db->scan_refresh_handle);
	}
}

/* ============================================================================
 *                           Public Functions
 * ============================================================================ */

/**
 * @brief    Initialize Scan Parameters Client
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_client_add(void)
{
#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	rtk_bt_gattc_uuid_t scan_param_uuid = {
		.is_uuid16 = true,
		.p.uuid16 = SCAN_PARAM_SRV_UUID
	};
	return rtk_bt_gattc_register_profile(SCAN_PARAM_CLIENT_PROFILE_ID, scan_param_uuid);
#else
	return rtk_bt_gattc_register_profile(SCAN_PARAM_CLIENT_PROFILE_ID);
#endif
}

/**
 * @brief    Attach connection to Scan Parameters Client
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_client_attach_conn(uint16_t conn_handle)
{
	scan_param_client_db_t *p_db = get_or_create_conn_db(conn_handle);
	if (!p_db) {
		return RTK_BT_ERR_NO_MEMORY;
	}

	BT_LOGA(TAG_SCAN_PARAM_CLIENT "Connection attached: conn_handle=0x%x\r\n", conn_handle);
	return RTK_BT_OK;
}

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Start discovery of Scan Parameters Service
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_client_discover(uint16_t conn_handle)
{
	scan_param_client_db_t *p_db = get_or_create_conn_db(conn_handle);
	if (!p_db) {
		return RTK_BT_ERR_NO_MEMORY;
	}

	p_db->disc_state = SCAN_PARAM_DISC_SRV;

	/* Start service discovery */
	rtk_bt_gattc_discover_param_t disc_param = {0};
	disc_param.profile_id = SCAN_PARAM_CLIENT_PROFILE_ID;
	disc_param.conn_handle = conn_handle;
	disc_param.type = RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID;
	disc_param.disc_primary_by_uuid.uuid_type = BT_UUID_TYPE_16;
	*(uint16_t *)disc_param.disc_primary_by_uuid.uuid = SCAN_PARAM_SRV_UUID;

	return rtk_bt_gattc_discover(&disc_param);
}
#endif /* #if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB */

/**
 * @brief    Write Scan Interval Window
 * @param[in] conn_handle Connection handle
 * @param[in] p_params Pointer to scan parameters
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_client_write_scan_interval_window(uint16_t conn_handle,
													  const scan_interval_window_t *p_params)
{
	scan_param_client_db_t *p_db = get_conn_db(conn_handle);
	if (!p_db || !p_db->scan_interval_window_handle) {
		return RTK_BT_ERR_UNSUPPORTED;
	}

	if (p_params == NULL) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	/* Validate parameters */
	if (p_params->le_scan_interval < SCAN_INTERVAL_MIN ||
		p_params->le_scan_interval > SCAN_INTERVAL_MAX) {
		BT_LOGE(TAG_SCAN_PARAM_CLIENT "Invalid scan interval: 0x%04x\r\n",
				p_params->le_scan_interval);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (p_params->le_scan_window < SCAN_WINDOW_MIN ||
		p_params->le_scan_window > SCAN_WINDOW_MAX) {
		BT_LOGE(TAG_SCAN_PARAM_CLIENT "Invalid scan window: 0x%04x\r\n",
				p_params->le_scan_window);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (p_params->le_scan_window > p_params->le_scan_interval) {
		BT_LOGE(TAG_SCAN_PARAM_CLIENT "Scan window > interval\r\n");
		return RTK_BT_ERR_PARAM_INVALID;
	}

	rtk_bt_gattc_write_param_t write_param = {0};
	write_param.profile_id = SCAN_PARAM_CLIENT_PROFILE_ID;
	write_param.conn_handle = conn_handle;
	/* Use WRITE_NO_RESP as per Scan Parameters Service Specification v1.0 */
	write_param.type = RTK_BT_GATT_CHAR_WRITE_NO_RSP;
	write_param.handle = p_db->scan_interval_window_handle;
	write_param.length = sizeof(scan_interval_window_t);
	write_param.data = (void *)p_params;

	BT_LOGA(TAG_SCAN_PARAM_CLIENT "Writing scan params: interval=%d ms, window=%d ms \r\n",
			(int)(p_params->le_scan_interval * 0.625), (int)(p_params->le_scan_window * 0.625));

	return rtk_bt_gattc_write(&write_param);
}

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Enable/Disable Scan Refresh notification
 * @param[in] conn_handle Connection handle
 * @param[in] enable true to enable, false to disable
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_client_set_refresh_notify(uint16_t conn_handle, bool enable)
{
	scan_param_client_db_t *p_db = get_conn_db(conn_handle);
	if (!p_db || !p_db->scan_refresh_cccd_handle) {
		return RTK_BT_ERR_UNSUPPORTED;
	}

	rtk_bt_gattc_update_cccd_param_t cccd_param = {0};
	cccd_param.profile_id = SCAN_PARAM_CLIENT_PROFILE_ID;
	cccd_param.conn_handle = conn_handle;
	cccd_param.char_val_handle = p_db->scan_refresh_handle;
	cccd_param.cccd_handle = p_db->scan_refresh_cccd_handle;
	cccd_param.bnotify = true;

	if (enable) {
		p_db->refresh_notify_en = true;
		BT_LOGA(TAG_SCAN_PARAM_CLIENT "Enabling Scan Refresh notifications\r\n");
		return rtk_bt_gattc_enable_notify_or_indicate(&cccd_param);
	} else {
		p_db->refresh_notify_en = false;
		BT_LOGA(TAG_SCAN_PARAM_CLIENT "Disabling Scan Refresh notifications\r\n");
		return rtk_bt_gattc_disable_notify_or_indicate(&cccd_param);
	}
}
#endif /* #if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB */

/**
 * @brief    Register application callback
 * @param[in] callback Callback function
 */
void scan_param_client_register_callback(scan_param_client_cb_t callback)
{
	app_callback = callback;
}

/**
 * @brief    Handle connection disconnect
 * @param[in] conn_handle Connection handle
 */
void scan_param_client_disconnect(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	if (conn_id >= SCAN_PARAM_MAX_LINKS) {
		return;
	}

	if (scan_param_database[conn_id]) {
		osif_mem_free(scan_param_database[conn_id]);
		scan_param_database[conn_id] = NULL;
	}
}

/**
 * @brief    Deinitialize Scan Parameters Client
 */
void scan_param_client_deinit(void)
{
	for (uint8_t i = 0; i < SCAN_PARAM_MAX_LINKS; i++) {
		if (scan_param_database[i]) {
			osif_mem_free(scan_param_database[i]);
			scan_param_database[i] = NULL;
		}
	}
	app_callback = NULL;
	BT_LOGA(TAG_SCAN_PARAM_CLIENT "Scan Parameters Client deinitialized\r\n");
}

/**
 * @brief    Scan Parameters Client unified GATTC app callback (for uniform dispatch)
 * @param[in] event GATTC event code
 * @param[in] data Event data
 * @return    RTK_BT_EVT_CB_OK
 */
rtk_bt_evt_cb_ret_t scan_param_client_app_callback(uint8_t event, void *data)
{
	switch (event) {
#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
	case RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND:
		scan_param_client_discover_hdl(data);
		break;
#endif

	case RTK_BT_GATTC_EVT_WRITE_RESULT_IND:
		scan_param_client_write_result_hdl(data);
		break;

	case RTK_BT_GATTC_EVT_NOTIFY_IND:
		scan_param_client_notify_hdl(data);
		break;

	default:
		break;
	}

	return RTK_BT_EVT_CB_OK;
}
