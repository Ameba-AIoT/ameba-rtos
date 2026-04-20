/**
 * @file      rtk_hids_iso_client.c
 * @author    atonio_liu@realsil.com.cn
 * @brief     HOGP v1.1 ULL HID ISO Service Client implementation
 * @copyright Copyright (c) 2026. Realtek Semiconductor Corporation. All rights reserved.
 */

#include <osif.h>
#include <string.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_def.h>
#include <basic_types.h>
#include <bt_debug.h>
#include "rtk_client_config.h"
#include "rtk_hids_iso_client.h"
#include "rtk_gcs_client.h"  /* For general_client_discover_res_hdl() */

/* ============================================================================
 *                           Local Definitions
 * ============================================================================ */

/** @brief Log tag */
#define TAG_HIDS_ISO_CLIENT     "[HOGP_HIDS_ISO_CLIENT] "

/* ============================================================================
 *                           Local Variables
 * ============================================================================ */

/** @brief HID ISO Client context per connection */
static hid_iso_client_ctx_t hid_iso_client_ctx[RTK_BLE_GAP_MAX_LINKS] = {0};

/** @brief Operation mode change callback */
static hid_iso_client_mode_change_cb_t mode_change_cb = NULL;

/**
 * @brief Cached Operation Mode parameters per connection (for Host-initiated write response)
 * @note  Used to trigger mode_change_cb after Write success for Host-initiated scenario
 *        Per HOGP v1.1 spec Section 5.2.1: "After sending the Select Hybrid Operation
 *        Mode Opcode and receiving a successful response, the HID Host shall configure
 *        the CIS to be used for HID ISO traffic and initiate the Connected Isochronous
 *        Stream Creation procedure."
 */
static struct {
	uint8_t opcode;
	uint8_t cig_id;
	uint8_t cis_id;
	bool valid;
} last_op_mode_params[RTK_BLE_GAP_MAX_LINKS] = {0};

/**
 * @brief Cached indication data per connection (for Device-initiated scenario)
 * @note  Used to preserve full indication parameters when Host needs to echo back
 *        Per HOGP v1.1 spec Section 5.2.2: Host shall use the same parameters from indication
 */
#define HID_ISO_MAX_INDICATION_LEN  16
static struct {
	uint8_t data[HID_ISO_MAX_INDICATION_LEN];
	uint16_t len;
	bool valid;
} last_indication_data[RTK_BLE_GAP_MAX_LINKS] = {0};

/* ============================================================================
 *                           Local Functions
 * ============================================================================ */

/**
 * @brief    Find client context by connection handle
 * @param[in] conn_handle Connection handle
 * @return   Pointer to context, NULL if not found
 */
static hid_iso_client_ctx_t *hid_iso_client_find_ctx(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return NULL;
	}

	if (hid_iso_client_ctx[conn_id].conn_handle == conn_handle &&
		hid_iso_client_ctx[conn_id].used) {
		return &hid_iso_client_ctx[conn_id];
	}

	return NULL;
}

/**
 * @brief    Get free client context
 * @param[in] conn_handle Connection handle
 * @return   Pointer to context, NULL if no free slot
 */
static hid_iso_client_ctx_t *hid_iso_client_get_free_ctx(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return NULL;
	}

	if (!hid_iso_client_ctx[conn_id].used) {
		memset(&hid_iso_client_ctx[conn_id], 0, sizeof(hid_iso_client_ctx_t));
		hid_iso_client_ctx[conn_id].used = true;
		hid_iso_client_ctx[conn_id].conn_handle = conn_handle;
		return &hid_iso_client_ctx[conn_id];
	}

	return NULL;
}

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Handle GATT discover result
 * @param[in] data Discover result data
 */
static void hid_iso_client_discover_result_hdl(void *data)
{
	rtk_bt_gattc_discover_ind_t *p_ind = (rtk_bt_gattc_discover_ind_t *)data;
	hid_iso_client_ctx_t *p_ctx = hid_iso_client_find_ctx(p_ind->conn_handle);

	if (p_ctx == NULL) {
		return;
	}

	/* Handle DONE status - trigger next discovery step */
	if (p_ind->status == RTK_BT_STATUS_DONE) {
		switch (p_ind->type) {
		case RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID: {
			/* Service discovery complete */
			if (p_ctx->srv_start_handle == 0 && p_ctx->srv_end_handle == 0) {
				BT_LOGE(TAG_HIDS_ISO_CLIENT "HID ISO Service not found\r\n");
				p_ctx->disc_state = HID_ISO_DISC_IDLE;
				goto log_exit;
			}

			/* Start characteristic discovery */
			rtk_bt_gattc_discover_param_t param = {0};
			param.profile_id = HID_ISO_CLIENT_PROFILE_ID;
			param.conn_handle = p_ind->conn_handle;
			param.type = RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL;
			param.disc_char_all.start_handle = p_ctx->srv_start_handle;
			param.disc_char_all.end_handle = p_ctx->srv_end_handle;

			if (rtk_bt_gattc_discover(&param) == RTK_BT_OK) {
				p_ctx->disc_state = HID_ISO_DISC_CHAR;
			} else {
				BT_LOGE(TAG_HIDS_ISO_CLIENT "Failed to start characteristic discovery\r\n");
				p_ctx->disc_state = HID_ISO_DISC_IDLE;
			}
			break;
		}

		case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL: {
			/* Characteristic discovery complete */
			p_ctx->disc_state = HID_ISO_DISC_CHAR_DONE;

			/* Start descriptor discovery */
			rtk_bt_gattc_discover_param_t param = {0};
			param.profile_id = HID_ISO_CLIENT_PROFILE_ID;
			param.conn_handle = p_ind->conn_handle;
			param.type = RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL;
			param.disc_descriptor.start_handle = p_ctx->srv_start_handle;
			param.disc_descriptor.end_handle = p_ctx->srv_end_handle;

			if (rtk_bt_gattc_discover(&param) == RTK_BT_OK) {
				p_ctx->disc_state = HID_ISO_DISC_DESC;
			} else {
				BT_LOGE(TAG_HIDS_ISO_CLIENT "Failed to start descriptor discovery\r\n");
				p_ctx->disc_state = HID_ISO_DISC_IDLE;
			}
			break;
		}

		case RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL:
			/* Descriptor discovery complete */
			p_ctx->disc_state = HID_ISO_DISC_DONE;
			BT_LOGA(TAG_HIDS_ISO_CLIENT "HID ISO Service discovery complete\r\n");
			break;

		default:
			break;
		}
		goto log_exit;
	}

	/* Handle FAIL status */
	if (p_ind->status == RTK_BT_STATUS_FAIL) {
		BT_LOGE(TAG_HIDS_ISO_CLIENT "Discovery failed, type: %d, err: 0x%x\r\n",
				p_ind->type, p_ind->err_code);
		p_ctx->disc_state = HID_ISO_DISC_IDLE;
		goto log_exit;
	}

	/* Handle CONTINUE status - process discovery data */
	if (p_ind->status != RTK_BT_STATUS_CONTINUE) {
		return;
	}

	if (p_ind->type == RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID) {
		/* HID ISO Service found */
		p_ctx->srv_start_handle = p_ind->disc_primary_by_uuid_per.start_handle;
		p_ctx->srv_end_handle = p_ind->disc_primary_by_uuid_per.end_handle;
		p_ctx->service_found = true;
		BT_LOGA(TAG_HIDS_ISO_CLIENT "HID ISO Service found: start=0x%04x, end=0x%04x\r\n",
				p_ind->disc_primary_by_uuid_per.start_handle,
				p_ind->disc_primary_by_uuid_per.end_handle);
	} else if (p_ind->type == RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL) {
		/* Check characteristic UUID */
		uint16_t uuid = 0;
		if (p_ind->disc_char_all_per.uuid_type == BT_UUID_TYPE_16) {
			uuid = *(uint16_t *)p_ind->disc_char_all_per.uuid;
		}

		switch (uuid) {
		case RTK_BT_UUID_HID_ISO_PROPERTIES:
			p_ctx->properties_handle = p_ind->disc_char_all_per.value_handle;
			BT_LOGA(TAG_HIDS_ISO_CLIENT "HID ISO Properties handle: 0x%04x\r\n",
					p_ind->disc_char_all_per.value_handle);
			break;
		case RTK_BT_UUID_LE_HID_OPERATION_MODE:
			p_ctx->operation_mode_handle = p_ind->disc_char_all_per.value_handle;
			BT_LOGA(TAG_HIDS_ISO_CLIENT "Operation Mode handle: 0x%04x\r\n",
					p_ind->disc_char_all_per.value_handle);
			break;
		default:
			break;
		}
	} else if (p_ind->type == RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL) {
		/* Check for CCCD descriptor */
		uint16_t uuid = 0;
		if (p_ind->disc_descriptor_per.uuid_type == BT_UUID_TYPE_16) {
			uuid = *(uint16_t *)p_ind->disc_descriptor_per.uuid;
		}

		if (uuid == BT_UUID_GATT_CCC_VAL) {
			/* Found CCCD - assign to operation mode CCCD if near operation mode handle */
			if (p_ctx->operation_mode_handle != 0 &&
				p_ind->disc_descriptor_per.handle > p_ctx->operation_mode_handle &&
				(p_ctx->operation_mode_cccd_handle == 0 ||
				 p_ind->disc_descriptor_per.handle < p_ctx->operation_mode_cccd_handle)) {
				p_ctx->operation_mode_cccd_handle = p_ind->disc_descriptor_per.handle;
				BT_LOGA(TAG_HIDS_ISO_CLIENT "Operation Mode CCCD handle: 0x%04x\r\n",
						p_ind->disc_descriptor_per.handle);
			}
		}
	}
	goto log_exit;

log_exit:
#if defined(HID_ISO_CLIENT_SHOW_DETAIL) && HID_ISO_CLIENT_SHOW_DETAIL
	general_client_discover_res_hdl(data);
#endif
	return;
}

#endif /* #if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB */

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Handle GATT read result
 * @param[in] data Read result data
 */
static void hid_iso_client_read_result_hdl(void *data)
{
	rtk_bt_gattc_read_ind_t *p_ind = (rtk_bt_gattc_read_ind_t *)data;
	hid_iso_client_ctx_t *p_ctx = hid_iso_client_find_ctx(p_ind->conn_handle);

	if (p_ctx == NULL || p_ind->status != RTK_BT_STATUS_CONTINUE) {
		return;
	}

	if (p_ind->type == RTK_BT_GATT_CHAR_READ_BY_HANDLE) {
		if (p_ind->by_handle.handle == p_ctx->properties_handle) {
			/* Parse HID ISO Properties */
			if (p_ind->by_handle.len >= sizeof(hid_iso_properties_t)) {
				uint8_t i;
				memcpy(&p_ctx->iso_properties, p_ind->by_handle.value,
					   sizeof(hid_iso_properties_t));
				BT_LOGA(TAG_HIDS_ISO_CLIENT "HID ISO Properties:\r\n");
				BT_LOGA(TAG_HIDS_ISO_CLIENT "  features=0x%02x, intervals=0x%04x\r\n",
						p_ctx->iso_properties.features,
						p_ctx->iso_properties.supported_report_intervals);
				BT_LOGA(TAG_HIDS_ISO_CLIENT "  max_sdu_in=%d, pref_sdu_in=%d\r\n",
						p_ctx->iso_properties.max_sdu_size_input,
						p_ctx->iso_properties.preferred_sdu_size_input);
				BT_LOGA(TAG_HIDS_ISO_CLIENT "  max_sdu_out=%d, pref_sdu_out=%d\r\n",
						p_ctx->iso_properties.max_sdu_size_output,
						p_ctx->iso_properties.preferred_sdu_size_output);
				for (i = 0; i < HID_ISO_MAX_HYBRID_REPORTS; i++) {
					BT_LOGA(TAG_HIDS_ISO_CLIENT "    hybrid_report[%d]: id=0x%02x, info=0x%02x\r\n",
							i,
							p_ctx->iso_properties.hybrid_mode_iso_reports[i].report_id,
							p_ctx->iso_properties.hybrid_mode_iso_reports[i].additional_info);
				}
			}
		}
	}
}

#endif /* #if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB */

/**
 * @brief    Handle GATT write result
 * @param[in] data Write result data
 * @note     Per HOGP v1.1 spec Section 5.2.1: "After sending the Select Hybrid Operation
 *           Mode Opcode and receiving a successful response, the HID Host shall configure
 *           the CIS to be used for HID ISO traffic and initiate the Connected Isochronous
 *           Stream Creation procedure."
 */
static void hid_iso_client_write_result_hdl(void *data)
{
	rtk_bt_gattc_write_ind_t *p_ind = (rtk_bt_gattc_write_ind_t *)data;
	hid_iso_client_ctx_t *p_ctx = hid_iso_client_find_ctx(p_ind->conn_handle);

	if (p_ctx == NULL) {
		return;
	}

	if (p_ind->status != RTK_BT_STATUS_DONE) {
		BT_LOGE(TAG_HIDS_ISO_CLIENT "Write failed: handle=0x%04x, err=%d\r\n",
				p_ind->handle, p_ind->err_code);
	} else {
		BT_LOGA(TAG_HIDS_ISO_CLIENT "Write success: handle=0x%04x\r\n", p_ind->handle);

		/* Per HOGP v1.1 spec Section 5.2.1: Host-initiated operation mode change
		 * After receiving a successful write response, the Host shall initiate
		 * CIS creation. This is different from Device-initiated scenario where
		 * the Device sends an indication. */
		if (p_ind->handle == p_ctx->operation_mode_handle && mode_change_cb != NULL) {
			uint8_t conn_id;
			if (rtk_bt_le_gap_get_conn_id(p_ind->conn_handle, &conn_id) == RTK_BT_OK) {
				if (last_op_mode_params[conn_id].valid) {
					hid_operation_mode_t mode = HID_OPERATION_MODE_DEFAULT;
					if (last_op_mode_params[conn_id].opcode == OPCODE_SELECT_HYBRID_MODE) {
						mode = HID_OPERATION_MODE_HYBRID;
					}
					BT_LOGA(TAG_HIDS_ISO_CLIENT "Host-initiated mode change: opcode=0x%02x, cig=%d, cis=%d\r\n",
							last_op_mode_params[conn_id].opcode,
							last_op_mode_params[conn_id].cig_id,
							last_op_mode_params[conn_id].cis_id);
					mode_change_cb(conn_id, mode,
								   last_op_mode_params[conn_id].cig_id,
								   last_op_mode_params[conn_id].cis_id);
				}
			}
		}
	}
}

/**
 * @brief    Handle GATT indication
 * @param[in] data Indication data
 */
static void hid_iso_client_indicate_hdl(void *data)
{
	rtk_bt_gattc_cccd_value_ind_t *p_ind = (rtk_bt_gattc_cccd_value_ind_t *)data;
	hid_iso_client_ctx_t *p_ctx = hid_iso_client_find_ctx(p_ind->conn_handle);
	uint8_t conn_id;

	if (p_ctx == NULL) {
		return;
	}

	if (p_ind->value_handle == p_ctx->operation_mode_handle) {
		/* Parse Operation Mode indication */
		if (p_ind->len >= 1) {
			le_hid_operation_mode_params_t *p_params = (le_hid_operation_mode_params_t *)p_ind->value;
			BT_LOGA(TAG_HIDS_ISO_CLIENT "Operation Mode indication: opcode=0x%02x, len=%d\r\n",
					p_params->opcode, p_ind->len);

			/* Cache complete indication data for Device-initiated scenario
			 * Per HOGP v1.1 spec Section 5.2.2: Host shall use these parameters */
			if (rtk_bt_le_gap_get_conn_id(p_ind->conn_handle, &conn_id) == RTK_BT_OK) {
				uint16_t copy_len = p_ind->len;
				if (copy_len > HID_ISO_MAX_INDICATION_LEN) {
					copy_len = HID_ISO_MAX_INDICATION_LEN;
				}
				memcpy(last_indication_data[conn_id].data, p_ind->value, copy_len);
				last_indication_data[conn_id].len = copy_len;
				last_indication_data[conn_id].valid = true;
				BT_LOGA(TAG_HIDS_ISO_CLIENT "Cached indication data: conn_id=%d, len=%d\r\n",
						conn_id, copy_len);
			}

			/* Notify application */
			if (mode_change_cb != NULL) {
				if (rtk_bt_le_gap_get_conn_id(p_ind->conn_handle, &conn_id) == RTK_BT_OK) {
					hid_operation_mode_t mode = HID_OPERATION_MODE_DEFAULT;
					uint8_t cig_id = 0;
					uint8_t cis_id = 0;

					if (p_params->opcode == OPCODE_SELECT_HYBRID_MODE) {
						mode = HID_OPERATION_MODE_HYBRID;
						/* Hybrid Mode indication has cig_id/cis_id fields (min len=3) */
						if (p_ind->len >= 3) {
							cig_id = p_params->cig_id;
							cis_id = p_params->cis_id;
						}
					}
					/* Default Mode indication only has opcode (len=1), no cig_id/cis_id */
					mode_change_cb(conn_id, mode, cig_id, cis_id);
				}
			}
		}
	}
}

/* ============================================================================
 *                           Public Functions
 * ============================================================================ */

/**
 * @brief    HID ISO Client unified GATTC app callback (for uniform dispatch)
 * @param[in] event GATTC event code
 * @param[in] data Event data
 * @return    RTK_BT_EVT_CB_OK
 */
rtk_bt_evt_cb_ret_t hids_iso_client_app_callback(uint8_t event, void *data)
{
	switch (event) {
#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
	case RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND:
		hid_iso_client_discover_result_hdl(data);
		break;

	case RTK_BT_GATTC_EVT_READ_RESULT_IND:
		hid_iso_client_read_result_hdl(data);
		break;
#endif

	case RTK_BT_GATTC_EVT_WRITE_RESULT_IND:
		hid_iso_client_write_result_hdl(data);
		break;

	case RTK_BT_GATTC_EVT_INDICATE_IND:
		hid_iso_client_indicate_hdl(data);
		break;

	default:
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

/**
 * @brief    Initialize HID ISO Service Client
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_add(void)
{
	memset(hid_iso_client_ctx, 0, sizeof(hid_iso_client_ctx));
#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	rtk_bt_gattc_uuid_t hids_iso_uuid = {
		.is_uuid16 = true,
		.p.uuid16 = RTK_BT_UUID_HID_ISO_SRV
	};
	return rtk_bt_gattc_register_profile(HID_ISO_CLIENT_PROFILE_ID, hids_iso_uuid);
#else
	return rtk_bt_gattc_register_profile(HID_ISO_CLIENT_PROFILE_ID);
#endif
}

/**
 * @brief    Attach connection to HID ISO Client
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_attach_conn(uint16_t conn_handle)
{
	hid_iso_client_ctx_t *p_ctx = hid_iso_client_get_free_ctx(conn_handle);

	if (p_ctx == NULL) {
		return RTK_BT_FAIL;
	}

	return RTK_BT_OK;
}

/**
 * @brief    Detach connection from HID ISO Client
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_detach_conn(uint16_t conn_handle)
{
	hid_iso_client_ctx_t *p_ctx = hid_iso_client_find_ctx(conn_handle);

	if (p_ctx != NULL) {
		memset(p_ctx, 0, sizeof(hid_iso_client_ctx_t));
	}

	return RTK_BT_OK;
}

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Start HID ISO Service discovery
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_discover(uint16_t conn_handle)
{
	hid_iso_client_ctx_t *p_ctx = hid_iso_client_find_ctx(conn_handle);

	if (p_ctx == NULL) {
		return RTK_BT_FAIL;
	}

	/* Reset discovery state */
	p_ctx->disc_state = HID_ISO_DISC_SRV;
	p_ctx->service_found = false;
	p_ctx->srv_start_handle = 0;
	p_ctx->srv_end_handle = 0;
	p_ctx->properties_handle = 0;
	p_ctx->operation_mode_handle = 0;
	p_ctx->operation_mode_cccd_handle = 0;

	rtk_bt_gattc_discover_param_t param = {0};
	param.profile_id = HID_ISO_CLIENT_PROFILE_ID;
	param.conn_handle = conn_handle;
	param.type = RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID;
	param.disc_primary_by_uuid.uuid_type = BT_UUID_TYPE_16;
	*(uint16_t *)param.disc_primary_by_uuid.uuid = RTK_BT_UUID_HID_ISO_SRV;

	return rtk_bt_gattc_discover(&param);
}

/**
 * @brief    Discover HID ISO Service characteristics
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_discover_char(uint16_t conn_handle)
{
	hid_iso_client_ctx_t *p_ctx = hid_iso_client_find_ctx(conn_handle);

	if (p_ctx == NULL || !p_ctx->service_found) {
		return RTK_BT_FAIL;
	}

	rtk_bt_gattc_discover_param_t param = {0};
	param.profile_id = HID_ISO_CLIENT_PROFILE_ID;
	param.conn_handle = conn_handle;
	param.type = RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL;
	param.disc_char_all.start_handle = p_ctx->srv_start_handle;
	param.disc_char_all.end_handle = p_ctx->srv_end_handle;

	return rtk_bt_gattc_discover(&param);
}

/**
 * @brief    Discover HID ISO Service descriptors
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_discover_desc(uint16_t conn_handle)
{
	hid_iso_client_ctx_t *p_ctx = hid_iso_client_find_ctx(conn_handle);

	if (p_ctx == NULL || !p_ctx->service_found) {
		return RTK_BT_FAIL;
	}

	rtk_bt_gattc_discover_param_t param = {0};
	param.profile_id = HID_ISO_CLIENT_PROFILE_ID;
	param.conn_handle = conn_handle;
	param.type = RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL;
	param.disc_descriptor.start_handle = p_ctx->srv_start_handle;
	param.disc_descriptor.end_handle = p_ctx->srv_end_handle;

	return rtk_bt_gattc_discover(&param);
}

/**
 * @brief    Read HID ISO Properties
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_read_properties(uint16_t conn_handle)
{
	hid_iso_client_ctx_t *p_ctx = hid_iso_client_find_ctx(conn_handle);

	if (p_ctx == NULL || p_ctx->properties_handle == 0) {
		return RTK_BT_FAIL;
	}

	rtk_bt_gattc_read_param_t param = {0};
	param.profile_id = HID_ISO_CLIENT_PROFILE_ID;
	param.conn_handle = conn_handle;
	param.type = RTK_BT_GATT_CHAR_READ_BY_HANDLE;
	param.by_handle.handle = p_ctx->properties_handle;

	return rtk_bt_gattc_read(&param);
}
#endif /* #if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB */

/**
 * @brief    Write LE HID Operation Mode
 * @param[in] conn_handle Connection handle
 * @param[in] p_params Pointer to operation mode parameters
 * @return   RTK_BT_OK on success, error code otherwise
 * @note     Per HOGP v1.1 spec Section 5.2.1: Parameters are cached to enable
 *           CIS creation callback after Write success response
 * @note     Per HOGP v1.1 spec Section 6.5.2, the total length is:
 *           - Opcode(1) = 1 byte for Default Mode
 *           - Opcode(1) + CIG_ID(1) + CIS_ID(1) + Interval(2) + SDU_in(1) + SDU_out(1) + Reports(N) = 7 + N bytes for Hybrid Mode
 */
uint16_t hid_iso_client_write_operation_mode(uint16_t conn_handle,
											 const le_hid_operation_mode_params_t *p_params, uint16_t params_len)
{
	hid_iso_client_ctx_t *p_ctx = hid_iso_client_find_ctx(conn_handle);
	uint8_t conn_id;

	if (p_ctx == NULL || p_ctx->operation_mode_handle == 0) {
		return RTK_BT_FAIL;
	}

	/* Cache parameters for Host-initiated mode change callback */
	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) == RTK_BT_OK) {
		last_op_mode_params[conn_id].opcode = p_params->opcode;
		last_op_mode_params[conn_id].cig_id = p_params->cig_id;
		last_op_mode_params[conn_id].cis_id = p_params->cis_id;
		last_op_mode_params[conn_id].valid = true;
	}

	rtk_bt_gattc_write_param_t param = {0};
	param.profile_id = HID_ISO_CLIENT_PROFILE_ID;
	param.conn_handle = conn_handle;
	param.type = RTK_BT_GATT_CHAR_WRITE_REQ;
	param.handle = p_ctx->operation_mode_handle;
	param.length = params_len;
	param.data = (void *)p_params;

	return rtk_bt_gattc_write(&param);
}

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Enable Operation Mode indication
 * @param[in] conn_handle Connection handle
 * @param[in] enable Enable/disable indication
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_enable_indication(uint16_t conn_handle, bool enable)
{
	hid_iso_client_ctx_t *p_ctx = hid_iso_client_find_ctx(conn_handle);

	if (p_ctx == NULL || p_ctx->operation_mode_cccd_handle == 0) {
		return RTK_BT_FAIL;
	}

	rtk_bt_gattc_update_cccd_param_t cccd_param = {0};
	cccd_param.profile_id = HID_ISO_CLIENT_PROFILE_ID;
	cccd_param.conn_handle = conn_handle;
	cccd_param.char_val_handle = p_ctx->operation_mode_handle;
	cccd_param.cccd_handle = p_ctx->operation_mode_cccd_handle;
	cccd_param.bindicate = true;

	if (enable) {
		return rtk_bt_gattc_enable_notify_or_indicate(&cccd_param);
	} else {
		return rtk_bt_gattc_disable_notify_or_indicate(&cccd_param);
	}
}
#endif /* #if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB */

/**
 * @brief    Register mode change callback
 * @param[in] callback Callback function
 */
void hid_iso_client_register_mode_change_cb(hid_iso_client_mode_change_cb_t callback)
{
	mode_change_cb = callback;
}

/**
 * @brief    Get HID ISO Client context
 * @param[in] conn_handle Connection handle
 * @return   Pointer to context, NULL if not found
 */
hid_iso_client_ctx_t *hid_iso_client_get_ctx(uint16_t conn_handle)
{
	return hid_iso_client_find_ctx(conn_handle);
}

/**
 * @brief    Check if HID ISO Service is discovered
 * @param[in] conn_handle Connection handle
 * @return   true if service found, false otherwise
 */
bool hid_iso_client_is_service_found(uint16_t conn_handle)
{
	hid_iso_client_ctx_t *p_ctx = hid_iso_client_find_ctx(conn_handle);
	return (p_ctx != NULL && p_ctx->service_found);
}

/**
 * @brief    Get last received indication data for Device-initiated scenario
 * @param[in] conn_handle Connection handle
 * @param[out] p_len Pointer to store indication data length
 * @return   Pointer to indication data, NULL if no valid indication cached
 * @note     Per HOGP v1.1 spec Section 5.2.2: Host shall use the same parameters from indication
 *           when responding to Device-initiated mode change
 */
const uint8_t *hid_iso_client_get_last_indication(uint16_t conn_handle, uint16_t *p_len)
{
	uint8_t conn_id;

	if (p_len == NULL) {
		return NULL;
	}

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return NULL;
	}

	if (!last_indication_data[conn_id].valid) {
		return NULL;
	}

	*p_len = last_indication_data[conn_id].len;
	return last_indication_data[conn_id].data;
}

/**
 * @brief    Deinitialize HID ISO Service Client
 */
void hid_iso_client_deinit(void)
{
	memset(hid_iso_client_ctx, 0, sizeof(hid_iso_client_ctx));
	memset(last_indication_data, 0, sizeof(last_indication_data));
	memset(last_op_mode_params, 0, sizeof(last_op_mode_params));
	mode_change_cb = NULL;
}
