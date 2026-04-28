/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <osif.h>

#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_def.h>
#include <bt_utils.h>
#include "rtk_client_config.h"
#include "rtk_hids_gp_client.h"
#include "rtk_gcs_client.h"  /* For general_client_discover_res_hdl() */

/* Enable detailed discovery logging */
#define HIDS_GP_CLIENT_SHOW_DETAIL   1

/* ============================================================================
 *                           Definitions
 * ============================================================================ */

/** @brief Log tag for Gamepad HID Client */
#define TAG_HIDS_GP_CLIENT  "[HIDS_GP_CLIENT]"

/** @brief Maximum number of connections supported */
#define HIDS_GP_MAX_LINKS    RTK_BLE_GAP_MAX_LINKS

/* ============================================================================
 *                           Type Definitions
 * ============================================================================ */

/**
 * @brief Discovery state machine
 */
typedef enum {
	HIDS_GP_DISC_IDLE = 0,       /**< Idle state */
	HIDS_GP_DISC_SRV,           /**< Discovering service */
	HIDS_GP_DISC_CHAR,          /**< Discovering characteristics */
	HIDS_GP_DISC_CHAR_DONE,     /**< Characteristic discovery done */
	HIDS_GP_DISC_DESC,          /**< Discovering descriptors */
	HIDS_GP_DISC_DONE,          /**< Discovery complete */
} hids_gp_disc_state_t;

/**
 * @brief Report information structure
 */
typedef struct {
	uint16_t char_val_handle;   /**< Characteristic value handle */
	uint16_t cccd_handle;       /**< CCCD handle */
	uint16_t report_ref_handle; /**< Report Reference descriptor handle */
	uint8_t properties;         /**< Characteristic properties */
	uint8_t report_id;          /**< Report ID */
	uint8_t report_type;        /**< 1=Input, 2=Output, 3=Feature */
	uint8_t report_ref_read;    /**< Report Reference read flag */
	uint16_t bit_length;        /**< Report bit length */
} hids_gp_report_info_t;

/**
 * @brief Gamepad HID Client database per connection
 */
typedef struct {
	uint8_t conn_id;                    /**< Connection ID */
	hids_gp_disc_state_t disc_state;    /**< Discovery state */
	uint16_t start_handle;              /**< HID service start handle */
	uint16_t end_handle;                /**< HID service end handle */

	/* Characteristic handles */
	uint16_t protocol_mode_handle;     /**< Protocol Mode characteristic handle */
	uint16_t report_map_handle;        /**< Report Map characteristic handle */
	uint16_t hid_info_handle;          /**< HID Information characteristic handle */
	uint16_t control_point_handle;    /**< HID Control Point characteristic handle */

	/* Report information */
	hids_gp_report_info_t reports[HIDS_GP_MAX_REPORTS];
	uint8_t num_reports;               /**< Number of reports found */

	/* Report Reference tracking */
	uint8_t report_refs_pending;       /**< Pending Report Reference reads */
	bool report_refs_done;             /**< All Report References read */

	/* Client features */
	bool protocol_mode_support;        /**< Device supports Protocol Mode */
	bool report_map_read;              /**< Report Map has been read */
	bool notification_enabled;         /**< Input Report notification enabled */
} hids_gp_client_db_t;

/**
 * @brief Gamepad HID client callback
 * @param[in] conn_id Connection ID
 * @param[in] event Event type
 * @param[in] data Event data
 * @param[in] len Data length
 */
typedef void (*hids_gp_client_cb_t)(uint8_t conn_id, uint8_t event, void *data, uint16_t len);

/* ============================================================================
 *                           Local Variables
 * ============================================================================ */

/** @brief Gamepad HID client database */
static hids_gp_client_db_t *hids_gp_database[HIDS_GP_MAX_LINKS] = {0};

/** @brief Application callback */
static hids_gp_client_cb_t app_callback = NULL;

/* ============================================================================
 *                           Local Functions
 * ============================================================================ */

/**
 * @brief    Print report summary table
 * @param[in] p_db Pointer to connection database
 */
#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
static void hids_gp_client_print_report_summary(hids_gp_client_db_t *p_db)
{
	BT_LOGA(TAG_HIDS_GP_CLIENT "================ Report Summary ================\r\n");
	BT_LOGA(TAG_HIDS_GP_CLIENT "  Index |  Type   | Report ID\r\n");
	BT_LOGA(TAG_HIDS_GP_CLIENT "  ------+---------+-----------\r\n");

	for (uint8_t i = 0; i < p_db->num_reports; i++) {
		const char *type_str;
		switch (p_db->reports[i].report_type) {
		case HID_INPUT_TYPE:
			type_str = "Input  ";
			break;
		case HID_OUTPUT_TYPE:
			type_str = "Output ";
			break;
		case HID_FEATURE_TYPE:
			type_str = "Feature";
			break;
		default:
			type_str = "Unknown";
			break;
		}

		BT_LOGA(TAG_HIDS_GP_CLIENT "    %d   | %s |     %d\r\n",
				i, type_str, p_db->reports[i].report_id);
	}
	BT_LOGA(TAG_HIDS_GP_CLIENT "================================================\r\n");
}

#endif /* #if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB */

/**
 * @brief    Get connection database by connection handle
 * @param[in] conn_handle Connection handle
 * @return   Pointer to database or NULL if not found
 */
static hids_gp_client_db_t *get_conn_db(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return NULL;
	}

	if (conn_id >= HIDS_GP_MAX_LINKS) {
		return NULL;
	}

	return hids_gp_database[conn_id];
}

/**
 * @brief    Get or create connection database
 * @param[in] conn_handle Connection handle
 * @return   Pointer to database or NULL if failed
 */
static hids_gp_client_db_t *get_or_create_conn_db(uint16_t conn_handle)
{
	uint8_t conn_id;
	hids_gp_client_db_t *p_db;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return NULL;
	}

	if (conn_id >= HIDS_GP_MAX_LINKS) {
		return NULL;
	}

	p_db = hids_gp_database[conn_id];
	if (!p_db) {
		p_db = (hids_gp_client_db_t *)osif_mem_alloc(RAM_TYPE_DATA_ON,
													 sizeof(hids_gp_client_db_t));
		if (!p_db) {
			BT_LOGE(TAG_HIDS_GP_CLIENT "Failed to allocate memory for conn_id %d\r\n", conn_id);
			return NULL;
		}
		memset(p_db, 0, sizeof(hids_gp_client_db_t));
		p_db->conn_id = conn_id;
		hids_gp_database[conn_id] = p_db;
		BT_LOGA(TAG_HIDS_GP_CLIENT "Allocated database for conn_id %d\r\n", conn_id);
	}

	return p_db;
}

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Handle HID service discovery result
 * @param[in] p_data Discovery result data
 */
static void hids_gp_handle_service_disc(const rtk_bt_gattc_discover_ind_t *p_data)
{
	hids_gp_client_db_t *p_db = get_conn_db(p_data->conn_handle);
	if (!p_db || p_db->disc_state != HIDS_GP_DISC_SRV) {
		return;
	}

	/* Check if discovery completed successfully */
	if (p_data->status == RTK_BT_STATUS_DONE) {
		if (p_db->start_handle == 0 && p_db->end_handle == 0) {
			/* Service not found */
			BT_LOGE(TAG_HIDS_GP_CLIENT "HID service not found\r\n");
			p_db->disc_state = HIDS_GP_DISC_IDLE;
			goto log_exit;
		}

		/* Start characteristic discovery */
		rtk_bt_gattc_discover_param_t disc_param = {0};
		disc_param.profile_id = HIDS_GP_CLIENT_PROFILE_ID;
		disc_param.conn_handle = p_data->conn_handle;
		disc_param.type = RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL;
		disc_param.disc_char_all.start_handle = p_db->start_handle;
		disc_param.disc_char_all.end_handle = p_db->end_handle;

		if (rtk_bt_gattc_discover(&disc_param) == RTK_BT_OK) {
			BT_LOGA(TAG_HIDS_GP_CLIENT "Characteristic discovery started, handles 0x%04x-0x%04x\r\n",
					p_db->start_handle, p_db->end_handle);
			p_db->disc_state = HIDS_GP_DISC_CHAR;
		} else {
			BT_LOGE(TAG_HIDS_GP_CLIENT "Failed to start characteristic discovery\r\n");
			p_db->disc_state = HIDS_GP_DISC_IDLE;
		}
		goto log_exit;
	}

	if (p_data->status != RTK_BT_STATUS_CONTINUE) {
		BT_LOGE(TAG_HIDS_GP_CLIENT "HID service discovery failed\r\n");
		p_db->disc_state = HIDS_GP_DISC_IDLE;
		goto log_exit;
	}

	/* Save service handles from discovery result */
	p_db->start_handle = p_data->disc_primary_by_uuid_per.start_handle;
	p_db->end_handle = p_data->disc_primary_by_uuid_per.end_handle;
	/* Don't change state here - wait for DONE event like BAS Client does */

	BT_LOGA(TAG_HIDS_GP_CLIENT "Found HID Service: handles 0x%04x - 0x%04x\r\n",
			p_db->start_handle, p_db->end_handle);
	goto log_exit;

log_exit:
#if defined(HIDS_GP_CLIENT_SHOW_DETAIL) && HIDS_GP_CLIENT_SHOW_DETAIL
	general_client_discover_res_hdl((void *)p_data);
#endif
	return;
}

/**
 * @brief    Handle characteristic discovery result
 * @param[in] p_data Discovery result data
 */
static void hids_gp_handle_char_disc(const rtk_bt_gattc_discover_ind_t *p_data)
{
	hids_gp_client_db_t *p_db = get_conn_db(p_data->conn_handle);
	if (!p_db || p_db->disc_state != HIDS_GP_DISC_CHAR) {
		return;
	}

	/* Check for completion or failure */
	if (p_data->status == RTK_BT_STATUS_DONE) {
		/* All characteristics discovered */
		p_db->disc_state = HIDS_GP_DISC_CHAR_DONE;
		BT_LOGA(TAG_HIDS_GP_CLIENT "Characteristic discovery complete, found %d reports\r\n",
				p_db->num_reports);

		/* Start descriptor discovery */
		rtk_bt_gattc_discover_param_t disc_param = {0};
		disc_param.profile_id = HIDS_GP_CLIENT_PROFILE_ID;
		disc_param.conn_handle = p_data->conn_handle;
		disc_param.type = RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL;
		disc_param.disc_descriptor.start_handle = p_db->start_handle;
		disc_param.disc_descriptor.end_handle = p_db->end_handle;

		if (rtk_bt_gattc_discover(&disc_param) == RTK_BT_OK) {
			p_db->disc_state = HIDS_GP_DISC_DESC;
		} else {
			BT_LOGE(TAG_HIDS_GP_CLIENT "Failed to start descriptor discovery\r\n");
			p_db->disc_state = HIDS_GP_DISC_IDLE;
		}
		goto log_exit;
	}

	if (p_data->status == RTK_BT_STATUS_FAIL) {
		BT_LOGE(TAG_HIDS_GP_CLIENT "Characteristic discovery failed\r\n");
		p_db->disc_state = HIDS_GP_DISC_IDLE;
		goto log_exit;
	}

	if (p_data->status != RTK_BT_STATUS_CONTINUE) {
		return;
	}

	/* Process discovered characteristic from union */
	uint8_t properties = p_data->disc_char_all_per.properties;
	uint16_t value_handle = p_data->disc_char_all_per.value_handle;
	uint16_t uuid = *(uint16_t *)p_data->disc_char_all_per.uuid;

	switch (uuid) {
	case 0x2A4E: /* Protocol Mode */
		p_db->protocol_mode_handle = value_handle;
		p_db->protocol_mode_support = true;
		BT_LOGA(TAG_HIDS_GP_CLIENT "Found Protocol Mode characteristic: 0x%04x\r\n", value_handle);
		break;

	case 0x2A4B: /* Report Map */
		p_db->report_map_handle = value_handle;
		BT_LOGA(TAG_HIDS_GP_CLIENT "Found Report Map characteristic: 0x%04x\r\n", value_handle);
		break;

	case 0x2A4A: /* HID Information */
		p_db->hid_info_handle = value_handle;
		BT_LOGA(TAG_HIDS_GP_CLIENT "Found HID Information characteristic: 0x%04x\r\n", value_handle);
		break;

	case 0x2A4C: /* HID Control Point */
		p_db->control_point_handle = value_handle;
		BT_LOGA(TAG_HIDS_GP_CLIENT "Found HID Control Point characteristic: 0x%04x\r\n", value_handle);
		break;

	case 0x2A4D: /* Report */
		if (p_db->num_reports < HIDS_GP_MAX_REPORTS) {
			hids_gp_report_info_t *p_report = &p_db->reports[p_db->num_reports];
			p_report->char_val_handle = value_handle;
			p_report->properties = properties;
			p_db->num_reports++;
			BT_LOGA(TAG_HIDS_GP_CLIENT "Found Report characteristic: 0x%04x, props: 0x%02x\r\n",
					value_handle, properties);
		}
		break;

	default:
		break;
	}
	goto log_exit;

log_exit:
#if defined(HIDS_GP_CLIENT_SHOW_DETAIL) && HIDS_GP_CLIENT_SHOW_DETAIL
	general_client_discover_res_hdl((void *)p_data);
#endif
	return;
}

/**
 * @brief    Handle descriptor discovery result
 * @param[in] p_data Discovery result data
 */
static void hids_gp_handle_desc_disc(const rtk_bt_gattc_discover_ind_t *p_data)
{
	hids_gp_client_db_t *p_db = get_conn_db(p_data->conn_handle);
	if (!p_db || p_db->disc_state != HIDS_GP_DISC_DESC) {
		return;
	}

	/* Check for completion or failure */
	if (p_data->status == RTK_BT_STATUS_DONE) {
		/* All descriptors discovered, notify application */
		p_db->disc_state = HIDS_GP_DISC_DONE;

		BT_LOGA(TAG_HIDS_GP_CLIENT "HID Service discovery complete\r\n");
		BT_LOGA(TAG_HIDS_GP_CLIENT "  - Protocol Mode support: %s\r\n",
				p_db->protocol_mode_support ? "Yes" : "No");
		BT_LOGA(TAG_HIDS_GP_CLIENT "  - Number of reports: %d\r\n", p_db->num_reports);

		/* Auto-read Report References */
		if (p_db->num_reports > 0) {
			BT_LOGA(TAG_HIDS_GP_CLIENT "Reading Report References...\r\n");
			hids_gp_client_read_report_refs(p_data->conn_handle);
		}

		/* Notify application */
		if (app_callback) {
			app_callback(p_db->conn_id, HIDS_GP_CLIENT_EVT_DISCOVERY_COMPLETE, NULL, 0);
		}
		goto log_exit;
	}

	if (p_data->status == RTK_BT_STATUS_FAIL) {
		BT_LOGE(TAG_HIDS_GP_CLIENT "Descriptor discovery failed\r\n");
		p_db->disc_state = HIDS_GP_DISC_DONE;
		goto log_exit;
	}

	if (p_data->status != RTK_BT_STATUS_CONTINUE) {
		return;
	}

	/* Process discovered descriptor from union */
	uint16_t desc_handle = p_data->disc_descriptor_per.handle;
	uint16_t uuid = *(uint16_t *)p_data->disc_descriptor_per.uuid;

	if (uuid == 0x2902) { /* CCCD */
		/* Find the report this CCCD belongs to */
		for (uint8_t j = 0; j < p_db->num_reports; j++) {
			if (desc_handle > p_db->reports[j].char_val_handle &&
				(j + 1 >= p_db->num_reports ||
				 desc_handle < p_db->reports[j + 1].char_val_handle)) {
				p_db->reports[j].cccd_handle = desc_handle;
				BT_LOGA(TAG_HIDS_GP_CLIENT "Found CCCD for report %d: 0x%04x\r\n",
						j, desc_handle);
				break;
			}
		}
	} else if (uuid == 0x2908) { /* Report Reference */
		/* Find the report this Report Reference belongs to */
		for (uint8_t j = 0; j < p_db->num_reports; j++) {
			if (desc_handle > p_db->reports[j].char_val_handle &&
				(j + 1 >= p_db->num_reports ||
				 desc_handle < p_db->reports[j + 1].char_val_handle)) {
				p_db->reports[j].report_ref_handle = desc_handle;
				BT_LOGA(TAG_HIDS_GP_CLIENT "Found Report Reference for report %d: 0x%04x\r\n",
						j, desc_handle);
				break;
			}
		}
	}
	goto log_exit;

log_exit:
#if defined(HIDS_GP_CLIENT_SHOW_DETAIL) && HIDS_GP_CLIENT_SHOW_DETAIL
	general_client_discover_res_hdl((void *)p_data);
#endif
	return;
}

/**
 * @brief    Handle read result
 * @param[in] p_data Read result data
 */
static void hids_gp_handle_read_result(const rtk_bt_gattc_read_ind_t *p_data)
{
	hids_gp_client_db_t *p_db = get_conn_db(p_data->conn_handle);
	if (!p_db) {
		return;
	}

	/* Check for read continuation vs completion */
	if (p_data->status == RTK_BT_STATUS_DONE) {
		/* Read operation complete */
		return;
	}

	if (p_data->status == RTK_BT_STATUS_FAIL || p_data->err_code != 0) {
		BT_LOGE(TAG_HIDS_GP_CLIENT "Read failed, handle: 0x%04x, err: 0x%04x\r\n",
				p_data->by_handle.handle, p_data->err_code);
		return;
	}

	if (p_data->status != RTK_BT_STATUS_CONTINUE) {
		return;
	}

	/* Handle Report Map read */
	if (p_data->by_handle.handle == p_db->report_map_handle) {
		BT_LOGA(TAG_HIDS_GP_CLIENT "Report Map read, length: %d\r\n", p_data->by_handle.len);
		p_db->report_map_read = true;

		/* Notify application with Report Map data */
		if (app_callback) {
			app_callback(p_db->conn_id, HIDS_GP_CLIENT_EVT_REPORT_MAP,
						 (void *)p_data->by_handle.value, p_data->by_handle.len);
		}
		return;
	}

	/* Handle Report Reference descriptor read */
	for (uint8_t i = 0; i < p_db->num_reports; i++) {
		if (p_data->by_handle.handle == p_db->reports[i].report_ref_handle) {
			/* Report Reference format: 2 bytes - Report ID (1 byte) + Report Type (1 byte) */
			if (p_data->by_handle.len >= 2) {
				p_db->reports[i].report_id = p_data->by_handle.value[0];
				p_db->reports[i].report_type = p_data->by_handle.value[1];
				p_db->reports[i].report_ref_read = 1;
				BT_LOGA(TAG_HIDS_GP_CLIENT "Report %d Reference: ID=%d, Type=%d\r\n",
						i, p_db->reports[i].report_id, p_db->reports[i].report_type);
			}

			/* Check if all Report References have been read */
			if (p_db->report_refs_pending > 0) {
				p_db->report_refs_pending--;
				if (p_db->report_refs_pending == 0) {
					p_db->report_refs_done = true;
					hids_gp_client_print_report_summary(p_db);
				}
			}
			return;
		}
	}
}

#endif /* #if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB */

/**
 * @brief    Handle write result
 * @param[in] p_data Write result data
 */
static void hids_gp_handle_write_result(const rtk_bt_gattc_write_ind_t *p_data)
{
	hids_gp_client_db_t *p_db = get_conn_db(p_data->conn_handle);
	if (!p_db) {
		return;
	}

	if (p_data->status != RTK_BT_STATUS_DONE || p_data->err_code != 0) {
		BT_LOGE(TAG_HIDS_GP_CLIENT "Write failed, handle: 0x%04x, err: 0x%04x\r\n",
				p_data->handle, p_data->err_code);
		return;
	}

	/* Handle Protocol Mode write */
	if (p_data->handle == p_db->protocol_mode_handle) {
		BT_LOGA(TAG_HIDS_GP_CLIENT "Protocol Mode write success\r\n");

		if (app_callback) {
			app_callback(p_db->conn_id, HIDS_GP_CLIENT_EVT_PROTOCOL_MODE, NULL, 0);
		}
	}
}

/**
 * @brief    Handle notification/indication
 * @param[in] p_data Notification data
 */
static void hids_gp_handle_notification(const rtk_bt_gattc_cccd_value_ind_t *p_data)
{
	hids_gp_client_db_t *p_db = get_conn_db(p_data->conn_handle);
	if (!p_db) {
		return;
	}

	/* Find which report this notification belongs to */
	for (uint8_t i = 0; i < p_db->num_reports; i++) {
		if (p_data->value_handle == p_db->reports[i].char_val_handle) {
			BT_LOGA(TAG_HIDS_GP_CLIENT "Input Report %d received, length: %d\r\n",
					i, p_data->len);

			/* Notify application */
			if (app_callback) {
				hids_gp_client_input_report_t report;
				report.report_id = p_db->reports[i].report_id;
				report.data_len = p_data->len;
				memcpy(report.data, p_data->value,
					   (p_data->len > sizeof(report.data)) ? sizeof(report.data) : p_data->len);

				app_callback(p_db->conn_id, HIDS_GP_CLIENT_EVT_INPUT_REPORT,
							 &report, sizeof(report));
			}
			break;
		}
	}
#if defined(HIDS_GP_CLIENT_SHOW_DETAIL) && HIDS_GP_CLIENT_SHOW_DETAIL
	general_client_notify_hdl((void *)p_data);
#endif
}

/* ============================================================================
 *                           Public Functions
 * ============================================================================ */

/**
 * @brief    Initialize Gamepad HID Client
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_add(void)
{
#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	rtk_bt_gattc_uuid_t hids_uuid = {
		.is_uuid16 = true,
		.p.uuid16 = HID_SRV_UUID
	};
	return rtk_bt_gattc_register_profile(HIDS_GP_CLIENT_PROFILE_ID, hids_uuid);
#else
	return rtk_bt_gattc_register_profile(HIDS_GP_CLIENT_PROFILE_ID);
#endif
}

/**
 * @brief    Attach connection to HIDS GP Client
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_attach_conn(uint16_t conn_handle)
{
	hids_gp_client_db_t *p_db = get_or_create_conn_db(conn_handle);
	if (!p_db) {
		return RTK_BT_ERR_NO_MEMORY;
	}

	BT_LOGA(TAG_HIDS_GP_CLIENT "Connection attached: conn_handle=0x%x\r\n", conn_handle);
	return RTK_BT_OK;
}

/**
 * @brief    Detach connection from HIDS GP Client
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_detach_conn(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (conn_id >= HIDS_GP_MAX_LINKS) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	/* Clear the connection state without freeing memory */
	if (hids_gp_database[conn_id]) {
		memset(hids_gp_database[conn_id], 0, sizeof(hids_gp_client_db_t));
		hids_gp_database[conn_id]->conn_id = conn_id;
	}

	return RTK_BT_OK;
}

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Start discovery of Gamepad HID Service
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_discover(uint16_t conn_handle)
{
	hids_gp_client_db_t *p_db;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	if (conn_id >= HIDS_GP_MAX_LINKS) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	p_db = hids_gp_database[conn_id];
	if (!p_db) {
		p_db = (hids_gp_client_db_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(hids_gp_client_db_t));
		if (!p_db) {
			return RTK_BT_ERR_NO_MEMORY;
		}
		memset(p_db, 0, sizeof(hids_gp_client_db_t));
		hids_gp_database[conn_id] = p_db;
	}

	p_db->conn_id = conn_id;
	p_db->disc_state = HIDS_GP_DISC_SRV;

	/* Start service discovery */
	rtk_bt_gattc_discover_param_t disc_param = {0};
	uint16_t uuid = HID_SRV_UUID;

	disc_param.profile_id = HIDS_GP_CLIENT_PROFILE_ID;
	disc_param.conn_handle = conn_handle;
	disc_param.type = RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID;
	memcpy(disc_param.disc_primary_by_uuid.uuid, &uuid, BT_UUID_SIZE_16);

	return rtk_bt_gattc_discover(&disc_param);
}
#endif /* #if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB */

/**
 * @brief    Set Protocol Mode
 * @param[in] conn_handle Connection handle
 * @param[in] mode Protocol mode (0=Boot, 1=Report)
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_set_protocol_mode(uint16_t conn_handle, uint8_t mode)
{
	hids_gp_client_db_t *p_db = get_conn_db(conn_handle);
	if (!p_db || !p_db->protocol_mode_support) {
		return RTK_BT_ERR_UNSUPPORTED;
	}

	rtk_bt_gattc_write_param_t write_param = {0};
	write_param.profile_id = HIDS_GP_CLIENT_PROFILE_ID;
	write_param.conn_handle = conn_handle;
	write_param.type = RTK_BT_GATT_CHAR_WRITE_NO_RSP;
	write_param.handle = p_db->protocol_mode_handle;
	write_param.length = 1;
	write_param.data = &mode;

	return rtk_bt_gattc_write(&write_param);
}

/**
 * @brief    Read Report Map
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_read_report_map(uint16_t conn_handle)
{
	hids_gp_client_db_t *p_db = get_conn_db(conn_handle);
	if (!p_db || !p_db->report_map_handle) {
		return RTK_BT_ERR_UNSUPPORTED;
	}

	rtk_bt_gattc_read_param_t read_param = {0};
	read_param.profile_id = HIDS_GP_CLIENT_PROFILE_ID;
	read_param.conn_handle = conn_handle;
	read_param.type = RTK_BT_GATT_CHAR_READ_BY_HANDLE;
	read_param.by_handle.handle = p_db->report_map_handle;

	return rtk_bt_gattc_read(&read_param);
}

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Read Report Reference descriptors for all reports
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 * @note     This function reads Report Reference descriptors to get Report ID and type
 *           for each discovered report. Should be called after discovery is complete.
 */
uint16_t hids_gp_client_read_report_refs(uint16_t conn_handle)
{
	hids_gp_client_db_t *p_db = get_conn_db(conn_handle);
	if (!p_db || p_db->disc_state != HIDS_GP_DISC_DONE) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	/* Initialize Report Reference tracking */
	p_db->report_refs_done = false;
	p_db->report_refs_pending = 0;

	rtk_bt_gattc_read_param_t read_param = {0};
	read_param.profile_id = HIDS_GP_CLIENT_PROFILE_ID;
	read_param.conn_handle = conn_handle;
	read_param.type = RTK_BT_GATT_CHAR_READ_BY_HANDLE;

	/* Count and read Report Reference descriptor for each report */
	for (uint8_t i = 0; i < p_db->num_reports; i++) {
		if (p_db->reports[i].report_ref_handle && !p_db->reports[i].report_ref_read) {
			p_db->report_refs_pending++;
			read_param.by_handle.handle = p_db->reports[i].report_ref_handle;
			uint16_t ret = rtk_bt_gattc_read(&read_param);
			if (ret != RTK_BT_OK) {
				BT_LOGE(TAG_HIDS_GP_CLIENT "Failed to read Report Reference for report %d\r\n", i);
				p_db->report_refs_pending--;
			}
		}
	}

	/* If no pending reads, print summary immediately */
	if (p_db->report_refs_pending == 0) {
		p_db->report_refs_done = true;
		hids_gp_client_print_report_summary(p_db);
	}

	return RTK_BT_OK;
}

#endif /* #if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB */

/**
 * @brief    Get report info by Report ID
 * @param[in] conn_handle Connection handle
 * @param[in] report_id Report ID to find
 * @param[out] p_report_idx Pointer to store report index
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_get_report_by_id(uint16_t conn_handle, uint8_t report_id, uint8_t *p_report_idx)
{
	hids_gp_client_db_t *p_db = get_conn_db(conn_handle);
	if (!p_db || !p_report_idx) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	for (uint8_t i = 0; i < p_db->num_reports; i++) {
		if (p_db->reports[i].report_ref_read && p_db->reports[i].report_id == report_id) {
			*p_report_idx = i;
			return RTK_BT_OK;
		}
	}

	return RTK_BT_ERR_NO_ENTRY;
}

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Enable/Disable Input Report notification
 * @param[in] conn_handle Connection handle
 * @param[in] report_idx Report index (from discovery)
 * @param[in] enable true to enable, false to disable
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_set_notify(uint16_t conn_handle, uint8_t report_idx, bool enable)
{
	hids_gp_client_db_t *p_db = get_conn_db(conn_handle);
	if (!p_db || report_idx >= p_db->num_reports) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	hids_gp_report_info_t *p_report = &p_db->reports[report_idx];
	if (!p_report->cccd_handle) {
		return RTK_BT_ERR_UNSUPPORTED;
	}

	rtk_bt_gattc_update_cccd_param_t cccd_param = {0};
	cccd_param.profile_id = HIDS_GP_CLIENT_PROFILE_ID;
	cccd_param.conn_handle = conn_handle;
	cccd_param.char_val_handle = p_report->char_val_handle;
	cccd_param.cccd_handle = p_report->cccd_handle;
	cccd_param.bnotify = true;

	if (enable) {
		p_db->notification_enabled = true;
		return rtk_bt_gattc_enable_notify_or_indicate(&cccd_param);
	} else {
		p_db->notification_enabled = false;
		return rtk_bt_gattc_disable_notify_or_indicate(&cccd_param);
	}
}

#endif /* #if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB */

/**
 * @brief    Write Output Report
 * @param[in] conn_handle Connection handle
 * @param[in] report_idx Report index
 * @param[in] p_data Pointer to data
 * @param[in] len Data length
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_write_output_report(uint16_t conn_handle, uint8_t report_idx,
											const uint8_t *p_data, uint16_t len)
{
	hids_gp_client_db_t *p_db = get_conn_db(conn_handle);
	if (!p_db || report_idx >= p_db->num_reports) {
		BT_LOGE(TAG_HIDS_GP_CLIENT "Write output report: invalid conn_handle=0x%04x or report_idx=%d\r\n",
				conn_handle, report_idx);
		return RTK_BT_ERR_NO_ENTRY;
	}

	hids_gp_report_info_t *p_report = &p_db->reports[report_idx];

	if (!(p_report->properties & RTK_BT_GATT_CHRC_WRITE_WITHOUT_RESP)) {
		BT_LOGE(TAG_HIDS_GP_CLIENT "Write output report: properties=0x%02x does not support WRITE_WITHOUT_RESP\r\n",
				p_report->properties);
		return RTK_BT_ERR_UNSUPPORTED;
	}

	rtk_bt_gattc_write_param_t write_param = {0};
	write_param.profile_id = HIDS_GP_CLIENT_PROFILE_ID;
	write_param.conn_handle = conn_handle;
	write_param.type = RTK_BT_GATT_CHAR_WRITE_NO_RSP;
	write_param.handle = p_report->char_val_handle;
	write_param.length = len;
	write_param.data = (void *)p_data;

	return rtk_bt_gattc_write(&write_param);
}

/**
 * @brief    Register application callback
 * @param[in] callback Callback function
 */
void hids_gp_client_register_callback(hids_gp_client_cb_t callback)
{
	app_callback = callback;
}

/**
 * @brief    Handle connection disconnect
 * @param[in] conn_handle Connection handle
 */
void hids_gp_client_disconnect(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	if (conn_id >= HIDS_GP_MAX_LINKS) {
		return;
	}

	if (hids_gp_database[conn_id]) {
		osif_mem_free(hids_gp_database[conn_id]);
		hids_gp_database[conn_id] = NULL;
	}
}

/**
 * @brief    Deinitialize Gamepad HID Client
 */
void hids_gp_client_deinit(void)
{
	for (uint8_t i = 0; i < HIDS_GP_MAX_LINKS; i++) {
		if (hids_gp_database[i]) {
			osif_mem_free(hids_gp_database[i]);
			hids_gp_database[i] = NULL;
		}
	}
	app_callback = NULL;
	BT_LOGA(TAG_HIDS_GP_CLIENT "Gamepad HID Client deinitialized\r\n");
}

/**
 * @brief    HIDS GP Client unified GATTC app callback (for uniform dispatch)
 * @param[in] event GATTC event code
 * @param[in] data Event data
 * @return    RTK_BT_EVT_CB_OK
 */
rtk_bt_evt_cb_ret_t hids_gp_client_app_callback(uint8_t event, void *data)
{
	switch (event) {
#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
	case RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND: {
		rtk_bt_gattc_discover_ind_t *p_data = (rtk_bt_gattc_discover_ind_t *)data;

		switch (p_data->type) {
		case RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID:
		case RTK_BT_GATT_DISCOVER_PRIMARY_ALL:
			hids_gp_handle_service_disc(p_data);
			break;

		case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL:
			hids_gp_handle_char_disc(p_data);
			break;

		case RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL:
			hids_gp_handle_desc_disc(p_data);
			break;

		default:
			break;
		}
		break;
	}

	case RTK_BT_GATTC_EVT_READ_RESULT_IND:
		hids_gp_handle_read_result((rtk_bt_gattc_read_ind_t *)data);
		break;
#endif

	case RTK_BT_GATTC_EVT_WRITE_RESULT_IND:
		hids_gp_handle_write_result((rtk_bt_gattc_write_ind_t *)data);
		break;

	case RTK_BT_GATTC_EVT_NOTIFY_IND:
		hids_gp_handle_notification((rtk_bt_gattc_cccd_value_ind_t *)data);
		break;

	default:
		break;
	}

	return RTK_BT_EVT_CB_OK;
}
