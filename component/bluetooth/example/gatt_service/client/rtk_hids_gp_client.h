/**
 * @file      rtk_hids_gp_client.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     Gamepad HID Service Client header file
 * @copyright Copyright (c) 2026. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_HIDS_GP_CLIENT_H__
#define __RTK_HIDS_GP_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 *                           Definitions
 * ============================================================================ */

/** @brief Gamepad Report ID */
#define GAMEPAD_REPORT_ID_INPUT     0x01
#define GAMEPAD_REPORT_ID_OUTPUT    0x02

/** @brief Gamepad Input Report length */
#define GAMEPAD_INPUT_REPORT_LEN    8

/** @brief Gamepad Output Report length */
#define GAMEPAD_OUTPUT_REPORT_LEN   1

/** @brief HID Protocol Modes */
#define GAMEPAD_PROTOCOL_MODE_BOOT   0x00
#define GAMEPAD_PROTOCOL_MODE_REPORT 0x01

/** @brief HID Control Point Commands */
#define HID_CONTROL_POINT_SUSPEND       0x01
#define HID_CONTROL_POINT_EXIT_SUSPEND  0x02

/** @brief HID Service UUID */
#define HID_SRV_UUID                        0x1812

/** @brief HID Report Types (for Report Reference Descriptor) */
#define HID_INPUT_TYPE   1
#define HID_OUTPUT_TYPE  2
#define HID_FEATURE_TYPE 3

/** @brief Maximum report data length */
#define HIDS_GP_MAX_REPORT_DATA_LEN    64

/** @brief Maximum number of reports per device */
#define HIDS_GP_MAX_REPORTS            10

/* ============================================================================
 *                           Type Definitions
 * ============================================================================ */

#ifndef RTK_HIDS_GP_TYPES_DEFINED
#define RTK_HIDS_GP_TYPES_DEFINED

/**
 * @brief Gamepad Input Report structure
 * @note 8 bytes total
 */
typedef struct {
	uint16_t buttons;      /**< 16 buttons (Bit 0-15: Button 1-16) */
	int8_t x;              /**< X axis (-127 to 127) */
	int8_t y;              /**< Y axis (-127 to 127) */
	int8_t z;              /**< Z axis (-127 to 127) */
	int8_t rz;             /**< Rz axis (-127 to 127) */
	uint8_t hat_switch;    /**< D-pad/Hat switch (0-7, 8=center/released) */
	uint8_t reserved;      /**< Reserved/padding */
} __attribute__((packed)) gamepad_input_report_t;

/**
 * @brief Gamepad Output Report structure
 * @note 1 byte for vibration/LED control
 */
typedef struct {
	uint8_t vibration;     /**< Vibration intensity (0-255) */
} __attribute__((packed)) gamepad_output_report_t;

#endif /* RTK_HIDS_GP_TYPES_DEFINED */

/**
 * @brief Input report data structure for callback
 */
typedef struct {
	uint8_t report_id;                              /**< Report ID */
	uint16_t data_len;                              /**< Data length */
	uint8_t data[HIDS_GP_MAX_REPORT_DATA_LEN];      /**< Report data */
} hids_gp_client_input_report_t;

/**
 * @brief Gamepad HID client callback event types
 */
typedef enum {
	HIDS_GP_CLIENT_EVT_DISCOVERY_COMPLETE,    /**< Service discovery complete */
	HIDS_GP_CLIENT_EVT_INPUT_REPORT,          /**< Input report received */
	HIDS_GP_CLIENT_EVT_REPORT_MAP,            /**< Report Map read complete */
	HIDS_GP_CLIENT_EVT_PROTOCOL_MODE,         /**< Protocol mode changed */
} hids_gp_client_evt_t;

/**
 * @brief Gamepad HID client callback function type
 * @param[in] conn_id Connection ID
 * @param[in] event Event type
 * @param[in] data Event data
 * @param[in] len Data length
 */
typedef void (*hids_gp_client_cb_t)(uint8_t conn_id, uint8_t event, void *data, uint16_t len);

/* ============================================================================
 *                           Function Declarations
 * ============================================================================ */

/**
 * @brief    Initialize Gamepad HID Client
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_add(void);

/**
 * @brief    Attach connection to HIDS GP Client
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_attach_conn(uint16_t conn_handle);

/**
 * @brief    Detach connection from HIDS GP Client
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_detach_conn(uint16_t conn_handle);

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Start discovery of Gamepad HID Service
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_discover(uint16_t conn_handle);
#endif

/**
 * @brief    Set Protocol Mode
 * @param[in] conn_handle Connection handle
 * @param[in] mode Protocol mode (0=Boot, 1=Report)
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_set_protocol_mode(uint16_t conn_handle, uint8_t mode);

/**
 * @brief    Read Report Map
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_read_report_map(uint16_t conn_handle);

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Read Report Reference descriptors for all reports
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 * @note     This function reads Report Reference descriptors to get Report ID and type
 *           for each discovered report. Should be called after discovery is complete.
 */
uint16_t hids_gp_client_read_report_refs(uint16_t conn_handle);
#endif

/**
 * @brief    Get report info by Report ID
 * @param[in] conn_handle Connection handle
 * @param[in] report_id Report ID to find
 * @param[out] p_report_idx Pointer to store report index
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_get_report_by_id(uint16_t conn_handle, uint8_t report_id, uint8_t *p_report_idx);

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Enable/Disable Input Report notification
 * @param[in] conn_handle Connection handle
 * @param[in] report_idx Report index (from discovery)
 * @param[in] enable true to enable, false to disable
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_set_notify(uint16_t conn_handle, uint8_t report_idx, bool enable);
#endif

/**
 * @brief    Write Output Report
 * @param[in] conn_handle Connection handle
 * @param[in] report_idx Report index
 * @param[in] p_data Pointer to data
 * @param[in] len Data length
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t hids_gp_client_write_output_report(uint16_t conn_handle, uint8_t report_idx,
											const uint8_t *p_data, uint16_t len);

/**
 * @brief    Register application callback
 * @param[in] callback Callback function
 */
void hids_gp_client_register_callback(hids_gp_client_cb_t callback);

/**
 * @brief    Handle connection disconnect
 * @param[in] conn_handle Connection handle
 */
void hids_gp_client_disconnect(uint16_t conn_handle);

/**
 * @brief    Deinitialize Gamepad HID Client
 */
void hids_gp_client_deinit(void);

/**
 * @brief    HIDS GP Client unified GATTC app callback (for uniform dispatch)
 * @param[in] event GATTC event code
 * @param[in] data Event data
 * @return    RTK_BT_EVT_CB_OK
 */
rtk_bt_evt_cb_ret_t hids_gp_client_app_callback(uint8_t event, void *data);

#ifdef __cplusplus
}
#endif

#endif /* __RTK_HIDS_GP_CLIENT_H__ */
