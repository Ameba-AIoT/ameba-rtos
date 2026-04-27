/**
 * @file      rtk_hids_iso.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     HOGP v1.1 ULL HID ISO Service header file
 * @copyright Copyright (c) 2026. Realtek Semiconductor Corporation. All rights reserved.
 * @note     This file contains GATT Service definitions only.
 *           HID ISO protocol data processing is in hid_iso_protocol.h
 */

#ifndef __RTK_HIDS_ISO_H__
#define __RTK_HIDS_ISO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <rtk_bt_att_defs.h>

/* ============================================================================
 *                           HID ISO Service UUIDs (v1.1)
 * ============================================================================ */

/* Raw UUID values */
#define HID_ISO_UUID_SRV                        0x185C
#define HID_ISO_UUID_PROPERTIES                 0x2C23
#define HID_ISO_UUID_OPERATION_MODE             0x2C24

/** @brief HID ISO Service UUID (HOGP v1.1) */
#ifndef RTK_BT_UUID_HID_ISO_SRV
#define RTK_BT_UUID_HID_ISO_SRV                 BT_UUID_DECLARE_16(HID_ISO_UUID_SRV)
#endif

/** @brief HID ISO Properties Characteristic UUID */
#ifndef RTK_BT_UUID_HID_ISO_PROPERTIES
#define RTK_BT_UUID_HID_ISO_PROPERTIES          BT_UUID_DECLARE_16(HID_ISO_UUID_PROPERTIES)
#endif

/** @brief LE HID Operation Mode Characteristic UUID */
#ifndef RTK_BT_UUID_LE_HID_OPERATION_MODE
#define RTK_BT_UUID_LE_HID_OPERATION_MODE       BT_UUID_DECLARE_16(HID_ISO_UUID_OPERATION_MODE)
#endif

/* ============================================================================
 *                           Operation Mode Opcodes
 * ============================================================================ */

/** @brief Opcode for selecting Hybrid Mode */
#define OPCODE_SELECT_HYBRID_MODE              0x01

/** @brief Opcode for selecting Default Mode */
#define OPCODE_SELECT_DEFAULT_MODE             0x02

/* ============================================================================
 *                           Operation Mode Error Codes
 * ============================================================================ */

/** @brief Error: Opcode outside supported range */
#define ERROR_CODE_OPCODE_NOT_SUPPORTED        0x81

/** @brief Error: Already in the requested mode */
#define ERROR_CODE_ALREADY_IN_MODE             0x82

/** @brief Error: Requested feature not supported */
#define ERROR_CODE_FEATURE_NOT_SUPPORTED       0x83

/* ============================================================================
 *                           Type Definitions
 * ============================================================================ */

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
#define HID_ISO_MAX_HYBRID_REPORTS              3

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
	uint8_t  hybrid_mode_reports_enable[];  /**< Report ID list for ISO (variable length) */
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

/**
 * @brief Operation mode change callback function type
 * @param[in] conn_id Connection ID
 * @param[in] new_mode New operation mode
 * @param[in] cig_id CIG ID for ISO
 * @param[in] cis_id CIS ID for ISO
 */
typedef void (*hid_iso_mode_change_cb_t)(
	uint8_t conn_id,
	hid_operation_mode_t new_mode,
	uint8_t cig_id,
	uint8_t cis_id
);

/* ============================================================================
 *                           Function Declarations (GATT Service Layer Only)
 * ============================================================================ */

/**
 * @brief    HID ISO Service callback
 * @param[in] event Event type
 * @param[in] data Event data
 */
void hid_iso_srv_callback(uint8_t event, void *data);

/**
 * @brief    Add HID ISO Service
 * @param[in] p_properties Pointer to HID ISO properties (NULL to use defaults)
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hid_iso_srv_add(const hid_iso_properties_t *p_properties);

/**
 * @brief    Register mode change callback
 * @param[in] callback Callback function for mode change notifications
 */
void hid_iso_srv_register_mode_change_cb(hid_iso_mode_change_cb_t callback);

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
												 const le_hid_operation_mode_params_t *p_params, uint16_t params_len);

/**
 * @brief    Send Default Mode indication
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 * @note     Per HOGP v1.1 spec Section 6.5.2, Default Mode indication only contains opcode (1 byte)
 */
uint16_t hid_iso_srv_send_default_mode_indication(uint16_t conn_handle);

/**
 * @brief    Get HID ISO Properties value
 * @return   Pointer to HID ISO Properties structure
 */
hid_iso_properties_t *hid_iso_srv_get_properties(void);

/**
 * @brief    Set current device operation mode
 * @param[in] mode New operation mode
 * @note     Called by application after CIS establishment (HYBRID) or teardown (DEFAULT)
 *           to sync the service layer state
 */
void hid_iso_srv_set_device_mode(hid_operation_mode_t mode);

/**
 * @brief    Handle disconnect for HID ISO Service
 * @param[in] conn_handle Connection handle
 */
void hid_iso_srv_disconnect(uint16_t conn_handle);

/**
 * @brief    Deinitialize HID ISO Service
 */
void hid_iso_srv_deinit(void);

/**
 * @brief    Get current hybrid mode Report IDs
 * @param[out] p_num_reports Pointer to store number of Report IDs
 * @param[out] p_report_ids Array to store Report IDs (must be at least 8 bytes)
 * @param[in] max_reports Maximum number of Report IDs to return
 * @return   Number of Report IDs actually stored, 0 if not in hybrid mode
 * @note     Should be called after mode change callback indicates Hybrid Mode
 */
uint8_t hid_iso_srv_get_hybrid_report_ids(uint8_t *p_report_ids, uint8_t max_reports);

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
										bool *p_conf_enabled, bool *p_rep_enabled);

#ifdef __cplusplus
}
#endif

#endif /* __RTK_HIDS_ISO_H__ */
