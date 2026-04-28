/**
 * @file      rtk_hids_iso_client.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     HOGP v1.1 ULL HID ISO Service Client header file
 * @copyright Copyright (c) 2026. Realtek Semiconductor Corporation. All rights reserved.
 * @note      This file defines types that are shared with the server.
 *            If both server and client headers are included, the server's
 *            definitions take precedence to avoid conflicts.
 */

#ifndef __RTK_HIDS_ISO_CLIENT_H__
#define __RTK_HIDS_ISO_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 *                           HID ISO Service UUIDs (HOGP v1.1)
 * ============================================================================ */

/** @brief HID ISO Service UUID (HOGP v1.1) */
#ifndef RTK_BT_UUID_HID_ISO_SRV
#define RTK_BT_UUID_HID_ISO_SRV                 0x185C
#endif

/** @brief HID ISO Properties Characteristic UUID */
#ifndef RTK_BT_UUID_HID_ISO_PROPERTIES
#define RTK_BT_UUID_HID_ISO_PROPERTIES          0x2C23
#endif

/** @brief LE HID Operation Mode Characteristic UUID */
#ifndef RTK_BT_UUID_LE_HID_OPERATION_MODE
#define RTK_BT_UUID_LE_HID_OPERATION_MODE       0x2C24
#endif

/* ============================================================================
 *                           Operation Mode Opcodes
 * ============================================================================ */

/** @brief Opcode for selecting Hybrid Mode */
#ifndef OPCODE_SELECT_HYBRID_MODE
#define OPCODE_SELECT_HYBRID_MODE              0x01
#endif

/** @brief Opcode for selecting Default Mode */
#ifndef OPCODE_SELECT_DEFAULT_MODE
#define OPCODE_SELECT_DEFAULT_MODE             0x02
#endif

/* ============================================================================
 *                           Operation Mode Error Codes
 * ============================================================================ */

/** @brief Error: Opcode outside supported range */
#ifndef ERROR_CODE_OPCODE_NOT_SUPPORTED
#define ERROR_CODE_OPCODE_NOT_SUPPORTED        0x81
#endif

/** @brief Error: Already in the requested mode */
#ifndef ERROR_CODE_ALREADY_IN_MODE
#define ERROR_CODE_ALREADY_IN_MODE             0x82
#endif

/** @brief Error: Requested feature not supported */
#ifndef ERROR_CODE_FEATURE_NOT_SUPPORTED
#define ERROR_CODE_FEATURE_NOT_SUPPORTED       0x83
#endif

/* ============================================================================
 *                           HID Control Point Values
 * ============================================================================ */

/** @brief Suspend command */
#ifndef HID_CONTROL_POINT_SUSPEND
#define HID_CONTROL_POINT_SUSPEND              0x01
#endif

/** @brief Exit Suspend command */
#ifndef HID_CONTROL_POINT_EXIT_SUSPEND
#define HID_CONTROL_POINT_EXIT_SUSPEND         0x02
#endif

/* ============================================================================
 *                           Type Definitions
 *                           NOTE: These types are shared with server.
 *                                 Only define if not already defined.
 * ============================================================================ */

/* Check if this is being included along with the server header */
#if !defined(__RTK_HIDS_ISO_H__)

/**
 * @brief Hybrid Mode Report Info structure
 * @note Per HOGP v1.1 spec Section 6.5.1.1.3
 */
typedef struct {
	uint8_t  report_id;                     /**< Report ID */
	uint8_t  additional_info;               /**< Additional Info bitmap */
	/* Bit 0: Report Type (0=Input, 1=Output) */
	/* Bit 1: Confirmation Supported */
	/* Bit 2: Repetition Supported */
	/* Bits 3-7: RFU */
} hid_iso_hybrid_report_info_t;

/**
 * @brief Maximum number of Hybrid Mode ISO Reports supported
 * @note 3 reports: No Report ID (0x00) + Input Report (0x01) + Output Report (0x02)
 */
#ifndef HID_ISO_MAX_HYBRID_REPORTS
#define HID_ISO_MAX_HYBRID_REPORTS              3
#endif

/**
 * @brief HID ISO Properties structure
 * @note As defined in HOGP v1.1 specification Section 6.5.1.1.3
 *       Uses fixed array instead of flexible array member for static initialization
 */
typedef struct {
	uint8_t  features;                      /**< Bit 0: Device Mode Change Supported */
	uint16_t supported_report_intervals;    /**< Bitmap: bit0=1ms, bit1=2ms, ... */
	uint8_t  max_sdu_size_input;            /**< Max input SDU size */
	uint8_t  preferred_sdu_size_input;      /**< Preferred input SDU size */
	uint8_t  max_sdu_size_output;           /**< Max output SDU size */
	uint8_t  preferred_sdu_size_output;     /**< Preferred output SDU size */
	hid_iso_hybrid_report_info_t hybrid_mode_iso_reports[HID_ISO_MAX_HYBRID_REPORTS]; /**< Report IDs for ISO in Hybrid Mode */
} __attribute__((packed)) hid_iso_properties_t;

/**
 * @brief LE HID Operation Mode parameters structure
 * @note Variable length structure for mode switching
 * @note Use __attribute__((packed)) to ensure no padding between fields for correct GATT transmission
 */
typedef struct {
	uint8_t  opcode;                        /**< Operation opcode */
	uint8_t  cig_id;                        /**< CIG ID */
	uint8_t  cis_id;                        /**< CIS ID */
	uint16_t report_interval_bitmap;        /**< Report interval bitmap */
	uint8_t  current_sdu_size_input;        /**< Current input SDU size */
	uint8_t  current_sdu_size_output;       /**< Current output SDU size */
	uint8_t  hybrid_mode_reports_enable[];  /**< Each byte: Bit7=Rep Enable, Bit6=Conf Enable, Bits2-0=Report Info Index */
} __attribute__((packed)) le_hid_operation_mode_params_t;

/**
 * @brief HID operation mode enumeration
 * @note Used by both HID Device and Report Host
 */
typedef enum {
	HID_OPERATION_MODE_DEFAULT = 0,         /**< Default Mode (GATT only) */
	HID_OPERATION_MODE_DEFAULT_PENDING,     /**< Default Mode pending (waiting for CIS teardown) */
	HID_OPERATION_MODE_HYBRID_PENDING,      /**< Hybrid Mode pending (waiting for CIS) */
	HID_OPERATION_MODE_HYBRID,              /**< Hybrid Mode active (GATT + ISO) */
} hid_operation_mode_t;

#endif /* !defined(__RTK_HIDS_ISO_H__) */

/* ============================================================================
 *                           Configuration
 * ============================================================================ */

/**
 * @brief Enable detailed discovery logging via general_client_discover_res_hdl()
 */
#define HID_ISO_CLIENT_SHOW_DETAIL   1

/* ============================================================================
 *                           Client-Specific Type Definitions
 * ============================================================================ */

/**
 * @brief HID ISO Client discovery state machine
 */
typedef enum {
	HID_ISO_DISC_IDLE = 0,       /**< Idle state */
	HID_ISO_DISC_SRV,            /**< Discovering service */
	HID_ISO_DISC_CHAR,           /**< Discovering characteristics */
	HID_ISO_DISC_CHAR_DONE,      /**< Characteristic discovery done */
	HID_ISO_DISC_DESC,           /**< Discovering descriptors */
	HID_ISO_DISC_DONE,           /**< Discovery complete */
} hid_iso_disc_state_t;

/**
 * @brief HID ISO Client context structure
 */
typedef struct {
	bool used;                               /**< Context in use flag */
	uint16_t conn_handle;                    /**< Connection handle */
	hid_iso_disc_state_t disc_state;         /**< Discovery state */
	bool service_found;                      /**< HID ISO Service discovered flag */
	uint16_t srv_start_handle;               /**< HID ISO Service start handle */
	uint16_t srv_end_handle;                 /**< HID ISO Service end handle */
	uint16_t properties_handle;              /**< HID ISO Properties handle */
	uint16_t operation_mode_handle;          /**< Operation Mode handle */
	uint16_t operation_mode_cccd_handle;     /**< Operation Mode CCCD handle */
	hid_iso_properties_t iso_properties;     /**< Cached ISO properties */
} hid_iso_client_ctx_t;

/**
 * @brief Operation mode change callback function type (Client)
 * @param[in] conn_id Connection ID
 * @param[in] new_mode New operation mode
 * @param[in] cig_id CIG ID for ISO
 * @param[in] cis_id CIS ID for ISO
 */
typedef void (*hid_iso_client_mode_change_cb_t)(
	uint8_t conn_id,
	hid_operation_mode_t new_mode,
	uint8_t cig_id,
	uint8_t cis_id
);

/* ============================================================================
 *                           Function Declarations
 * ============================================================================ */

/**
 * @brief    HID ISO Client unified GATTC app callback (for uniform dispatch)
 * @param[in] event GATTC event code
 * @param[in] data Event data
 * @return    RTK_BT_EVT_CB_OK
 */
rtk_bt_evt_cb_ret_t hids_iso_client_app_callback(uint8_t event, void *data);

/**
 * @brief    Initialize HID ISO Service Client
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_add(void);

/**
 * @brief    Attach connection to HID ISO Client
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_attach_conn(uint16_t conn_handle);

/**
 * @brief    Detach connection from HID ISO Client
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_detach_conn(uint16_t conn_handle);

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Start HID ISO Service discovery
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_discover(uint16_t conn_handle);

/**
 * @brief    Discover HID ISO Service characteristics
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_discover_char(uint16_t conn_handle);

/**
 * @brief    Discover HID ISO Service descriptors
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_discover_desc(uint16_t conn_handle);

/**
 * @brief    Read HID ISO Properties
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_read_properties(uint16_t conn_handle);
#endif

/**
 * @brief    Write LE HID Operation Mode
 * @param[in] conn_handle Connection handle
 * @param[in] p_params Pointer to operation mode parameters
 * @param[in] params_len Total length of parameters (including variable-length hybrid_mode_reports_enable)
 * @return   RTK_BT_OK on success, error code otherwise
 * @note     Per HOGP v1.1 spec Section 6.5.2:
 *           - Default Mode: params_len = 1 (opcode only)
 *           - Hybrid Mode: params_len = 7 + num_reports (each report entry is 1 byte)
 */
uint16_t hid_iso_client_write_operation_mode(uint16_t conn_handle,
											 const le_hid_operation_mode_params_t *p_params, uint16_t params_len);

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Enable Operation Mode indication
 * @param[in] conn_handle Connection handle
 * @param[in] enable Enable/disable indication
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_client_enable_indication(uint16_t conn_handle, bool enable);
#endif

/**
 * @brief    Register mode change callback
 * @param[in] callback Callback function
 */
void hid_iso_client_register_mode_change_cb(hid_iso_client_mode_change_cb_t callback);

/**
 * @brief    Get HID ISO Client context
 * @param[in] conn_handle Connection handle
 * @return   Pointer to context, NULL if not found
 */
hid_iso_client_ctx_t *hid_iso_client_get_ctx(uint16_t conn_handle);

/**
 * @brief    Check if HID ISO Service is discovered
 * @param[in] conn_handle Connection handle
 * @return   true if service found, false otherwise
 */
bool hid_iso_client_is_service_found(uint16_t conn_handle);

/**
 * @brief    Deinitialize HID ISO Service Client
 */
void hid_iso_client_deinit(void);

/**
 * @brief    Get last received indication parameters
 * @param[in] conn_handle Connection handle
 * @param[out] p_len Pointer to store indication data length
 * @return   Pointer to indication data, NULL if not available
 * @note     Used by Report Host to retrieve Device-initiated indication parameters
 *           The returned data should be used for write response to Device
 */
const uint8_t *hid_iso_client_get_last_indication(uint16_t conn_handle, uint16_t *p_len);

#ifdef __cplusplus
}
#endif

#endif /* __RTK_HIDS_ISO_CLIENT_H__ */
