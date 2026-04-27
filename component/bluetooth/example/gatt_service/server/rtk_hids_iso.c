/**
 * @file      rtk_hids_iso.c
 * @author    atonio_liu@realsil.com.cn
 * @brief     HOGP v1.1 ULL HID ISO Service implementation
 * @copyright Copyright (c) 2026. Realtek Semiconductor Corporation. All rights reserved.
 * @note     This file contains GATT Service handling only.
 *           HID ISO protocol data processing is in hid_iso_protocol.c
 */

#include <osif.h>
#include <string.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_def.h>
#include <basic_types.h>
#include <bt_debug.h>
#include <rtk_service_config.h>
#include "rtk_hids_iso.h"
#include "rtk_hids_gamepad.h"

/* ============================================================================
 *                           Local Definitions
 * ============================================================================ */

/** @brief Log tag */
#define TAG_HIDS_ISO     "[HOGP_HIDS_ISO] "

/** @brief Service ID for HID ISO Service */
#define HID_ISO_SRV_ID              BT_BLE_SERVICE_HID_ISO

/** @brief HID ISO Properties value size */
#define HID_ISO_PROPERTIES_LEN      sizeof(hid_iso_properties_t)

/* ============================================================================
 *                           Attribute Index Definitions
 * ============================================================================ */

enum {
	HIDS_ISO_IDX_SVC = 0,                   /* 0 - Service Declaration */
	HIDS_ISO_IDX_PROPERTIES_CHAR,           /* 1 - HID ISO Properties Characteristic Declaration */
	HIDS_ISO_IDX_PROPERTIES_VAL,            /* 2 - HID ISO Properties Value */
	HIDS_ISO_IDX_OPERATION_MODE_CHAR,       /* 3 - LE HID Operation Mode Characteristic Declaration */
	HIDS_ISO_IDX_OPERATION_MODE_VAL,        /* 4 - LE HID Operation Mode Value */
	HIDS_ISO_IDX_OPERATION_MODE_CCC,        /* 5 - LE HID Operation Mode CCCD */
	HIDS_ISO_IDX_MAX,
};

/* ============================================================================
 *                           Local Variables
 * ============================================================================ */

/** @brief HID ISO Service instance properties
 * @note Report interval bitmap per HOGP v1.1 spec Section 6.5.1.1.2:
 *       Bit 0 = 1ms, Bit 1 = 2ms, Bit 2 = 3ms, Bit 3 = 4ms, Bit 4 = 5ms,
 *       Bit 5 = 1.25ms, Bit 6 = 2.5ms, Bit 7 = 3.75ms, Bit 8 = 7.5ms
 * @note Mandatory: Must support at least 5ms or 7.5ms (Bit 4 or Bit 8)
 * @note Hybrid Mode ISO Reports per HOGP v1.1 spec Section 6.5.1.1.3:
 *       - Report ID 0 (No Report ID, Input): additional_info bit 0 = 0 (Input Report)
 *       - Report ID 1 (Input): additional_info bit 0 = 0 (Input Report)
 *       - Report ID 2 (Output): additional_info bit 0 = 1 (Output Report)
 */
static hid_iso_properties_t iso_properties = {
	.features = 0x01,                    /* Bit 0: Device Mode Change Supported */
	.supported_report_intervals = 0x01DC, /* Support 2.5ms, 3ms, 3.75ms, 4ms, 5ms, 7.5ms
                                         * Bit 0 (1ms) = 0
                                         * Bit 1 (2ms) = 0
                                         * Bit 2 (3ms) = 1
                                         * Bit 3 (4ms) = 1
                                         * Bit 4 (5ms) = 1  (Mandatory - supported)
                                         * Bit 5 (1.25ms) = 0
                                         * Bit 6 (2.5ms) = 1
                                         * Bit 7 (3.75ms) = 1
                                         * Bit 8 (7.5ms) = 1
                                         * Bitmap = 0b0000000111101100 = 0x01DC
                                         */
	.max_sdu_size_input = 64,
	.preferred_sdu_size_input = 32,
	.max_sdu_size_output = 32,
	.preferred_sdu_size_output = 16,
	.hybrid_mode_iso_reports = {
		{ .report_id = 0x00,                     .additional_info = 0x00 },  /* No Report ID, Input (bit0=0) */
		{ .report_id = GAMEPAD_REPORT_ID_INPUT,  .additional_info = 0x00 },  /* Input Report (bit0=0) */
		{ .report_id = GAMEPAD_REPORT_ID_OUTPUT, .additional_info = 0x01 },  /* Output Report (bit0=1) */
	},
};

/** @brief Per-connection CCCD state for indication */
static uint8_t operation_mode_cccd_ind_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};

/** @brief Current LE HID Operation Mode parameters (raw buffer for variable-length data)
 * @note le_hid_operation_mode_params_t contains a flexible array member,
 *       so sizeof does not include hybrid_mode_reports_enable[]. Use raw buffer
 *       to ensure full data is stored without truncation. */
#define CURRENT_OP_MODE_BUF_SIZE  (sizeof(le_hid_operation_mode_params_t) + HID_ISO_MAX_HYBRID_REPORTS)
static uint8_t current_operation_mode_buf[CURRENT_OP_MODE_BUF_SIZE] = {0};

/** @brief Actual length of current_operation_mode (including variable Report ID array) */
static uint16_t current_operation_mode_len = 0;

/** @brief Current device operation mode */
static hid_operation_mode_t current_device_mode = HID_OPERATION_MODE_DEFAULT;

/** @brief Operation mode change callback */
static hid_iso_mode_change_cb_t mode_change_cb = NULL;

/** @brief Per-report enable state stored from Host Operation Mode write
 * @note Indexed by hybrid_mode_iso_reports[] index (0..HID_ISO_MAX_HYBRID_REPORTS-1)
 */
static struct {
	bool confirmation_enabled;
	bool repetition_enabled;
} reports_enable_state[HID_ISO_MAX_HYBRID_REPORTS] = {0};

/* ============================================================================
 *                           Service Attribute Table
 * ============================================================================ */

static rtk_bt_gatt_attr_t hid_iso_srv_attrs[] = {
	/* 0 - Primary Service: HID ISO */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_HID_ISO_SRV),

	/* 1, 2 - HID ISO Properties Characteristic (Read) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_HID_ISO_PROPERTIES,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ_ENCRYPT),

	/* 3, 4, 5 - LE HID Operation Mode Characteristic (Write + Indicate + CCCD) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_LE_HID_OPERATION_MODE,
							   RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_INDICATE,
							   RTK_BT_GATT_PERM_READ_ENCRYPT | RTK_BT_GATT_PERM_WRITE_ENCRYPT),
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ_ENCRYPT | RTK_BT_GATT_PERM_WRITE_ENCRYPT),
};

/** @brief Service structure */
static struct rtk_bt_gatt_service hid_iso_srv = RTK_BT_GATT_SERVICE(hid_iso_srv_attrs, HID_ISO_SRV_ID);

/* ============================================================================
 *                           Local Functions
 * ============================================================================ */

/**
 * @brief    Handle read requests
 * @param[in] data Read indication data
 */
static void hid_iso_read_hdl(void *data)
{
	rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
	rtk_bt_gatts_read_resp_param_t read_resp = {0};
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(p_read_ind->conn_handle, &conn_id) != RTK_BT_OK) {
		BT_LOGE(TAG_HIDS_ISO "Failed to get conn_id for conn_handle 0x%04x\r\n",
				p_read_ind->conn_handle);
		return;
	}

	read_resp.app_id = HID_ISO_SRV_ID;
	read_resp.conn_handle = p_read_ind->conn_handle;
	read_resp.cid = p_read_ind->cid;
	read_resp.index = p_read_ind->index;

	switch (p_read_ind->index) {
	case HIDS_ISO_IDX_PROPERTIES_VAL:
		read_resp.data = (uint8_t *)&iso_properties;
		read_resp.len = sizeof(hid_iso_properties_t);
		BT_LOGA(TAG_HIDS_ISO "Read HID ISO Properties: features=0x%02x, intervals=0x%04x, len=%d",
				iso_properties.features, iso_properties.supported_report_intervals,
				sizeof(hid_iso_properties_t));
		break;

	default:
		BT_LOGE(TAG_HIDS_ISO "Read from unknown index: %d\r\n", p_read_ind->index);
		break;
	}

	rtk_bt_gatts_read_resp(&read_resp);
}

/**
 * @brief    Handle write requests
 * @param[in] data Write indication data
 */
static void hid_iso_write_hdl(void *data)
{
	rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
	rtk_bt_gatts_write_resp_param_t write_resp = {0};
	uint8_t conn_id;
	uint8_t error_code = 0;

	if (rtk_bt_le_gap_get_conn_id(p_write_ind->conn_handle, &conn_id) != RTK_BT_OK) {
		BT_LOGE(TAG_HIDS_ISO "Failed to get conn_id for conn_handle 0x%04x\r\n",
				p_write_ind->conn_handle);
		return;
	}

	/* Prepare response */
	write_resp.app_id = HID_ISO_SRV_ID;
	write_resp.conn_handle = p_write_ind->conn_handle;
	write_resp.cid = p_write_ind->cid;
	write_resp.index = p_write_ind->index;

	switch (p_write_ind->index) {
	case HIDS_ISO_IDX_OPERATION_MODE_VAL:
		/* Handle LE HID Operation Mode write */
		if (p_write_ind->len < 1) {
			error_code = ERROR_CODE_FEATURE_NOT_SUPPORTED;
			BT_LOGE(TAG_HIDS_ISO "Operation Mode write too short: len=%d\r\n",
					p_write_ind->len);
			break;
		}

		{
			uint8_t opcode = p_write_ind->value[0];

			/* Validate opcode range */
			if (opcode != OPCODE_SELECT_HYBRID_MODE &&
				opcode != OPCODE_SELECT_DEFAULT_MODE) {
				/* Opcode outside supported range */
				error_code = ERROR_CODE_OPCODE_NOT_SUPPORTED;
				BT_LOGE(TAG_HIDS_ISO "Invalid opcode: 0x%02x\r\n", opcode);
				break;
			}

			/* Check for already in mode error */
			if (opcode == OPCODE_SELECT_HYBRID_MODE) {
				if (current_device_mode == HID_OPERATION_MODE_HYBRID) {
					error_code = ERROR_CODE_ALREADY_IN_MODE;
					BT_LOGE(TAG_HIDS_ISO "Already in Hybrid mode\r\n");
					break;
				}
				if (current_device_mode == HID_OPERATION_MODE_HYBRID_PENDING) {
					/* This should be Host confirmation write from Device-initiated flow
					 * Allow it - the state is already HYBRID_PENDING */
					BT_LOGA(TAG_HIDS_ISO "Host confirmation for Hybrid Mode (already pending)\r\n");
					/* Continue to process the write */
				}
			}

			if (opcode == OPCODE_SELECT_DEFAULT_MODE) {
				if (current_device_mode == HID_OPERATION_MODE_DEFAULT) {
					error_code = ERROR_CODE_ALREADY_IN_MODE;
					BT_LOGE(TAG_HIDS_ISO "Already in Default mode\r\n");
					break;
				}
				if (current_device_mode == HID_OPERATION_MODE_DEFAULT_PENDING) {
					/* This should be Host confirmation write from Device-initiated flow
					 * Allow it - the state is already DEFAULT_PENDING */
					BT_LOGA(TAG_HIDS_ISO "Host confirmation for Default Mode (already pending)\r\n");
					/* Continue to process the write */
				}
			}

			/* For Hybrid Mode, validate parameters */
			if (opcode == OPCODE_SELECT_HYBRID_MODE) {
				/* Minimum parameter length for Hybrid Mode:
				 * Opcode(1) + CIG_ID(1) + CIS_ID(1) + Report_Interval_Bitmap(2) + SDU sizes(2) + Enable(1-2)
				 * = 8 bytes minimum
				 */
				if (p_write_ind->len < 8) {
					error_code = ERROR_CODE_FEATURE_NOT_SUPPORTED;
					BT_LOGE(TAG_HIDS_ISO "Hybrid Mode parameters too short: len=%d (min 8)\r\n",
							p_write_ind->len);
					break;
				}

				/* Validate report interval bitmap */
				uint16_t interval_bitmap;
				memcpy(&interval_bitmap, &p_write_ind->value[3], sizeof(uint16_t));
				/* Validate exactly one report interval is selected (power of 2) */
				if (interval_bitmap == 0 || (interval_bitmap & (interval_bitmap - 1)) != 0) {
					error_code = ERROR_CODE_FEATURE_NOT_SUPPORTED;
					BT_LOGE(TAG_HIDS_ISO "Multiple or zero report intervals requested: 0x%04x\r\n", interval_bitmap);
					break;
				}
				if ((interval_bitmap & iso_properties.supported_report_intervals) != interval_bitmap) {
					error_code = ERROR_CODE_FEATURE_NOT_SUPPORTED;
					BT_LOGE(TAG_HIDS_ISO "Requested intervals (0x%04x) not fully supported (supported: 0x%04x)\r\n",
							interval_bitmap, iso_properties.supported_report_intervals);
					break;
				} else {
					BT_LOGA(TAG_HIDS_ISO "Requested intervals (0x%04x)  (supported: 0x%04x)\r\n",
							interval_bitmap, iso_properties.supported_report_intervals);
				}
				/* Per HOGP v1.1 spec Section 2.7:
				 * Report intervals less than 2ms shall only be used with LE 2M PHY
				 * Intervals < 2ms: Bit 0 (1ms), Bit 5 (1.25ms) */
				if (interval_bitmap & 0x0021) {  /* Check Bit 0 and Bit 5 */
					/* TODO: Verify current PHY is 2M
					 * Use rtk_bt_le_gap_get_phy() or similar API to check PHY
					 * For now, log a warning as this is application-dependent */
					error_code = ERROR_CODE_FEATURE_NOT_SUPPORTED;
					BT_LOGE(TAG_HIDS_ISO "Report interval < 2ms requested, LE 2M PHY required\r\n");
					break;
				}

				/* Validate SDU sizes */
				uint8_t sdu_input = p_write_ind->value[5];
				uint8_t sdu_output = p_write_ind->value[6];
				if (sdu_input > iso_properties.max_sdu_size_input ||
					sdu_output > iso_properties.max_sdu_size_output) {
					error_code = ERROR_CODE_FEATURE_NOT_SUPPORTED;
					BT_LOGE(TAG_HIDS_ISO "SDU size exceeds maximum: in=%d (max %d), out=%d (max %d)\r\n",
							sdu_input, iso_properties.max_sdu_size_input,
							sdu_output, iso_properties.max_sdu_size_output);
					break;
				}

				/* Validate Report Info Index in Hybrid Mode ISO Reports Enable
				 * Per HOGP v1.1 spec Section 6.5.2.1.2.1, each entry is 1 byte:
				 * - Bit 7: Repetition Enable
				 * - Bit 6: Confirmation Enable
				 * - Bits 2-0: Report Info Index (index into hybrid_mode_iso_reports[])
				 * - Other bits: RFU */
				if (p_write_ind->len > 7) {
					uint8_t num_reports = p_write_ind->len - 7;  /* Each entry is 1 byte */
					/* Validate number of report entries does not exceed supported count */
					if (num_reports > HID_ISO_MAX_HYBRID_REPORTS) {
						error_code = ERROR_CODE_FEATURE_NOT_SUPPORTED;
						BT_LOGE(TAG_HIDS_ISO "Too many report entries: %d (max %d)\r\n",
								num_reports, HID_ISO_MAX_HYBRID_REPORTS);
						break;
					}
					for (uint8_t i = 0; i < num_reports; i++) {
						uint8_t report_entry = p_write_ind->value[7 + i];
						uint8_t report_info_index = report_entry & 0x07;

						/* Validate Report Info Index against hybrid_mode_iso_reports array */
						if (report_info_index >= HID_ISO_MAX_HYBRID_REPORTS) {
							error_code = ERROR_CODE_FEATURE_NOT_SUPPORTED;
							BT_LOGE(TAG_HIDS_ISO "Invalid Report Info Index: %d (max %d)\r\n",
									report_info_index, HID_ISO_MAX_HYBRID_REPORTS - 1);
							break;
						}

						/* Store Confirmation and Repetition enable bits
						 * Per HOGP v1.1 spec Section 6.5.2.1.2.1:
						 * Bit 7: Repetition Enable, Bit 6: Confirmation Enable */
						reports_enable_state[report_info_index].confirmation_enabled =
							(report_entry & 0x40) ? true : false;
						reports_enable_state[report_info_index].repetition_enabled =
							(report_entry & 0x80) ? true : false;

						BT_LOGA(TAG_HIDS_ISO "Report Info Index %d: Report ID 0x%02x, Conf=%d, Rep=%d\r\n",
								report_info_index,
								iso_properties.hybrid_mode_iso_reports[report_info_index].report_id,
								reports_enable_state[report_info_index].confirmation_enabled,
								reports_enable_state[report_info_index].repetition_enabled);
					}
				}
				/* If Report Info Index validation failed, skip memcpy and mode change */
				if (error_code != 0) {
					break;
				}
			}

			/* Copy operation mode parameters (full length including variable Report ID array) */
			current_operation_mode_len = p_write_ind->len;
			memcpy(current_operation_mode_buf, p_write_ind->value,
				   p_write_ind->len <= CURRENT_OP_MODE_BUF_SIZE ?
				   p_write_ind->len : CURRENT_OP_MODE_BUF_SIZE);

			le_hid_operation_mode_params_t *p_op_mode =
				(le_hid_operation_mode_params_t *)current_operation_mode_buf;
			BT_LOGA(TAG_HIDS_ISO "Operation Mode write: opcode=0x%02x, cig_id=%d, cis_id=%d\r\n",
					opcode, p_op_mode->cig_id, p_op_mode->cis_id);

			/* Update device mode and notify application via callback */
			if (mode_change_cb != NULL) {
				hid_operation_mode_t new_mode;
				if (opcode == OPCODE_SELECT_HYBRID_MODE) {
					new_mode = HID_OPERATION_MODE_HYBRID_PENDING;
				} else {
					new_mode = HID_OPERATION_MODE_DEFAULT;
				}
				current_device_mode = new_mode;
				mode_change_cb(conn_id, new_mode,
							   p_op_mode->cig_id,
							   p_op_mode->cis_id);
			}
		}
		break;

	default:
		BT_LOGE(TAG_HIDS_ISO "Write to unknown index: %d\r\n", p_write_ind->index);
		break;
	}

	/* Send write response */
	write_resp.type = p_write_ind->type;
	write_resp.err_code = error_code;
	rtk_bt_gatts_write_resp(&write_resp);
}

/**
 * @brief    Handle CCCD change indications
 * @param[in] data CCCD indication data
 */
static void hid_iso_cccd_hdl(void *data)
{
	rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(p_cccd_ind->conn_handle, &conn_id) != RTK_BT_OK) {
		BT_LOGE(TAG_HIDS_ISO "Failed to get conn_id for conn_handle 0x%04x\r\n",
				p_cccd_ind->conn_handle);
		return;
	}

	switch (p_cccd_ind->index) {
	case HIDS_ISO_IDX_OPERATION_MODE_CCC:
		if (p_cccd_ind->value & RTK_BT_GATT_CCC_INDICATE) {
			operation_mode_cccd_ind_en_map[conn_id] = 1;
			BT_LOGA(TAG_HIDS_ISO "Operation Mode indication enabled, conn_id: %d\r\n", conn_id);
		} else {
			operation_mode_cccd_ind_en_map[conn_id] = 0;
			BT_LOGA(TAG_HIDS_ISO "Operation Mode indication disabled, conn_id: %d\r\n", conn_id);
		}
		break;

	default:
		BT_LOGE(TAG_HIDS_ISO "CCCD for unknown index: %d\r\n", p_cccd_ind->index);
		break;
	}
}

/* ============================================================================
 *                           Public Functions
 * ============================================================================ */

/**
 * @brief    HID ISO Service callback
 * @param[in] event Event type
 * @param[in] data Event data
 */
void hid_iso_srv_callback(uint8_t event, void *data)
{
	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE:
		BT_LOGA(TAG_HIDS_ISO "HID ISO Service registered\r\n");
		break;

	case RTK_BT_GATTS_EVT_READ_IND:
		hid_iso_read_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_WRITE_IND:
		hid_iso_write_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_CCCD_IND:
		hid_iso_cccd_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND:
		BT_LOGA(TAG_HIDS_ISO "Indication complete\r\n");
		break;

	default:
		break;
	}
}

/**
 * @brief    Add HID ISO Service
 * @param[in] p_properties Pointer to HID ISO properties (NULL to use defaults)
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_srv_add(const hid_iso_properties_t *p_properties)
{
	if (p_properties != NULL) {
		memcpy(&iso_properties, p_properties, sizeof(hid_iso_properties_t));
	}

	hid_iso_srv.type = GATT_SERVICE_OVER_BLE;
	hid_iso_srv.server_info = 0;
	hid_iso_srv.user_data = NULL;
	hid_iso_srv.register_status = 0;

	return rtk_bt_gatts_register_service(&hid_iso_srv);
}

/**
 * @brief    Register mode change callback
 * @param[in] callback Callback function for mode change notifications
 */
void hid_iso_srv_register_mode_change_cb(hid_iso_mode_change_cb_t callback)
{
	mode_change_cb = callback;
}

/**
 * @brief    Send Hybrid Mode indication
 * @param[in] conn_handle Connection handle
 * @param[in] p_params Pointer to hybrid mode parameters (must include opcode + all params)
 * @param[in] params_len Total length including variable report array (7 + num_reports)
 * @return   RTK_BT_OK on success, error code otherwise
 * @note     Per HOGP v1.1 spec Section 6.5.2, Hybrid Mode indication length:
 *           Opcode(1) + CIG_ID(1) + CIS_ID(1) + Interval(2) + SDU_in(1) + SDU_out(1) + Reports(N)
 *           = 7 + N bytes
 */
uint16_t hid_iso_srv_send_hybrid_mode_indication(uint16_t conn_handle,
												 const le_hid_operation_mode_params_t *p_params,
												 uint16_t params_len)
{
	uint8_t conn_id;
	rtk_bt_gatts_ntf_and_ind_param_t ind_param = {0};

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		BT_LOGE(TAG_HIDS_ISO "Failed to get conn_id for conn_handle 0x%04x\r\n", conn_handle);
		return RTK_BT_FAIL;
	}

	/* Check if indication is enabled */
	if (!operation_mode_cccd_ind_en_map[conn_id]) {
		BT_LOGE(TAG_HIDS_ISO "Operation Mode indication not enabled for conn_id %d\r\n", conn_id);
		return RTK_BT_FAIL;
	}

	/* Validate parameters */
	if (p_params == NULL || params_len < 7) {
		BT_LOGE(TAG_HIDS_ISO "Invalid Hybrid Mode params: len=%d (min 7)\r\n", params_len);
		return RTK_BT_FAIL;
	}

	/* Cache indication parameters for Host confirmation validation (full length including Reports) */
	memcpy(current_operation_mode_buf, p_params,
		   params_len <= CURRENT_OP_MODE_BUF_SIZE ?
		   params_len : CURRENT_OP_MODE_BUF_SIZE);
	current_operation_mode_len = params_len;

	/* Set state to HYBRID_PENDING before sending indication
	 * Per HOGP v1.1: Device requests mode change, waits for Host confirmation */
	current_device_mode = HID_OPERATION_MODE_HYBRID_PENDING;

	ind_param.app_id = HID_ISO_SRV_ID;
	ind_param.conn_handle = conn_handle;
	ind_param.index = HIDS_ISO_IDX_OPERATION_MODE_VAL;
	ind_param.data = (void *)p_params;
	ind_param.len = params_len;

	BT_LOGA(TAG_HIDS_ISO "Send Hybrid Mode indication: cig_id=%d, cis_id=%d, len=%d, state=%d\r\n",
			p_params->cig_id, p_params->cis_id, params_len, current_device_mode);

	return rtk_bt_gatts_indicate(&ind_param);
}

/**
 * @brief    Send Default Mode indication
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 * @note     Per HOGP v1.1 spec Section 6.5.2, Default Mode indication only contains opcode (1 byte)
 */
uint16_t hid_iso_srv_send_default_mode_indication(uint16_t conn_handle)
{
	uint8_t conn_id;
	rtk_bt_gatts_ntf_and_ind_param_t ind_param = {0};
	uint8_t opcode = OPCODE_SELECT_DEFAULT_MODE;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		BT_LOGE(TAG_HIDS_ISO "Failed to get conn_id for conn_handle 0x%04x\r\n", conn_handle);
		return RTK_BT_FAIL;
	}

	/* Check if indication is enabled */
	if (!operation_mode_cccd_ind_en_map[conn_id]) {
		BT_LOGE(TAG_HIDS_ISO "Operation Mode indication not enabled for conn_id %d\r\n", conn_id);
		return RTK_BT_FAIL;
	}

	/* Set state to DEFAULT_PENDING before sending indication
	 * Per HOGP v1.1: Device requests mode change, waits for Host confirmation */
	current_device_mode = HID_OPERATION_MODE_DEFAULT_PENDING;

	/* Clear cached operation mode params since Default Mode has no parameters */
	memset(current_operation_mode_buf, 0, CURRENT_OP_MODE_BUF_SIZE);
	current_operation_mode_len = 1;  /* Only opcode */

	ind_param.app_id = HID_ISO_SRV_ID;
	ind_param.conn_handle = conn_handle;
	ind_param.index = HIDS_ISO_IDX_OPERATION_MODE_VAL;
	ind_param.data = &opcode;
	ind_param.len = 1;  /* Only opcode for Default Mode */

	BT_LOGA(TAG_HIDS_ISO "Send Default Mode indication: len=1, state=%d\r\n", current_device_mode);

	return rtk_bt_gatts_indicate(&ind_param);
}

/**
 * @brief    Get HID ISO Properties value
 * @return   Pointer to HID ISO Properties structure
 */
hid_iso_properties_t *hid_iso_srv_get_properties(void)
{
	return &iso_properties;
}

/**
 * @brief    Set current device operation mode
 * @param[in] mode New operation mode
 */
void hid_iso_srv_set_device_mode(hid_operation_mode_t mode)
{
	current_device_mode = mode;
	BT_LOGA(TAG_HIDS_ISO "Device mode set to: %d\r\n", mode);
}

/**
 * @brief    Handle disconnect for HID ISO Service
 * @param[in] conn_handle Connection handle
 */
void hid_iso_srv_disconnect(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) == RTK_BT_OK) {
		operation_mode_cccd_ind_en_map[conn_id] = 0;
	}
}

/**
 * @brief    Get current hybrid mode Report IDs
 * @return Number of Report IDs actually stored
 * @param[out] p_report_ids Array to store Report IDs (must be at least max_reports bytes)
 * @param[in] max_reports Maximum number of Report IDs to return
 * @return   Number of Report IDs actually stored, 0 if not in hybrid mode
 * @note     Should be called after mode change callback indicates Hybrid Mode
 * @note     Per HOGP v1.1 spec Section 6.5.2.1.2.1, each entry in hybrid_mode_reports_enable
 *           is 1 byte: Bit 7 = Rep, Bit 6 = Conf, Bits 2-0 = Report Info Index
 */
uint8_t hid_iso_srv_get_hybrid_report_ids(uint8_t *p_report_ids, uint8_t max_reports)
{
	/* Fixed fields: opcode(1) + cig_id(1) + cis_id(1) + interval(2) + sdu_in(1) + sdu_out(1) = 7 bytes */
	uint8_t fixed_len = 7;

	if (current_operation_mode_len < fixed_len) {
		return 0;
	}

	/* Calculate number of report entries (each entry is 1 byte) */
	uint8_t report_entries_len = current_operation_mode_len - fixed_len;
	uint8_t num_reports = report_entries_len;  /* Each entry is 1 byte */

	/* Limit to maximum requested */
	if (num_reports > max_reports) {
		num_reports = max_reports;
	}

	/* Base structure size without flexible array member */
	size_t base_struct_size = offsetof(le_hid_operation_mode_params_t, hybrid_mode_reports_enable);
	uint8_t available_entries = (current_operation_mode_len > base_struct_size) ?
								(current_operation_mode_len - base_struct_size) : 0;

	/* Limit to available entries */
	if (num_reports > available_entries) {
		num_reports = available_entries;
	}

	/* Extract Report IDs from the hybrid_mode_reports_enable array
	 * Each entry is 1 byte: Bits 2-0 = Report Info Index */
	if (num_reports > 0 && current_operation_mode_len > base_struct_size) {
		uint8_t *p_base = current_operation_mode_buf;
		uint8_t *p_report_data = p_base + base_struct_size;
		for (uint8_t i = 0; i < num_reports; i++) {
			uint8_t report_entry = p_report_data[i];  /* Each entry is 1 byte */
			uint8_t report_info_index = report_entry & 0x07;

			/* Validate Report Info Index */
			if (report_info_index < HID_ISO_MAX_HYBRID_REPORTS) {
				/* Get actual Report ID from hybrid_mode_iso_reports array */
				p_report_ids[i] = iso_properties.hybrid_mode_iso_reports[report_info_index].report_id;
			} else {
				BT_LOGE(TAG_HIDS_ISO "Invalid Report Info Index %d at position %d\r\n",
						report_info_index, i);
				p_report_ids[i] = 0;  /* Invalid index */
			}
		}
	}


	return num_reports;
}

/**
 * @brief    Get Confirmation/Repetition enable state per Report Info Index
 * @param[in] report_info_index Index into hybrid_mode_iso_reports[]
 * @param[out] p_conf_enabled Pointer to store Confirmation enable state
 * @param[out] p_rep_enabled Pointer to store Repetition enable state
 * @return   RTK_BT_OK on success, RTK_BT_FAIL if invalid index
 * @note     Should be called after mode change callback to determine
 *           which features the Host has enabled per report
 */
uint16_t hid_iso_srv_get_reports_enable(uint8_t report_info_index,
										bool *p_conf_enabled, bool *p_rep_enabled)
{
	if (report_info_index >= HID_ISO_MAX_HYBRID_REPORTS ||
		p_conf_enabled == NULL || p_rep_enabled == NULL) {
		return RTK_BT_FAIL;
	}

	*p_conf_enabled = reports_enable_state[report_info_index].confirmation_enabled;
	*p_rep_enabled = reports_enable_state[report_info_index].repetition_enabled;
	return RTK_BT_OK;
}

/**
 * @brief    Deinitialize HID ISO Service
 */
void hid_iso_srv_deinit(void)
{
	memset(reports_enable_state, 0, sizeof(reports_enable_state));
	memset(operation_mode_cccd_ind_en_map, 0, sizeof(operation_mode_cccd_ind_en_map));
	memset(current_operation_mode_buf, 0, CURRENT_OP_MODE_BUF_SIZE);
	current_operation_mode_len = 0;
	current_device_mode = HID_OPERATION_MODE_DEFAULT;
	mode_change_cb = NULL;
}
