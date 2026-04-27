/**
 * @file      rtk_scan_param_client.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     Scan Parameters Service Client header file
 * @copyright Copyright (c) 2026. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_SCAN_PARAM_CLIENT_H__
#define __RTK_SCAN_PARAM_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 *                           Scan Parameters Service UUIDs
 * ============================================================================ */

/** @brief Scan Parameters Service UUID */
#define RTK_BT_UUID_SCAN_PARAM_SRV           0x1813

/** @brief Scan Interval Window Characteristic UUID */
#define RTK_BT_UUID_SCAN_INTERVAL_WINDOW     0x2A4F

/** @brief Scan Refresh Characteristic UUID */
#define RTK_BT_UUID_SCAN_REFRESH             0x2A31

/* ============================================================================
 *                           Definitions
 * ============================================================================ */

/** @brief Minimum LE Scan Interval (2.5 ms) */
#define SCAN_INTERVAL_MIN                    0x0004

/** @brief Maximum LE Scan Interval (10.24 s) */
#define SCAN_INTERVAL_MAX                    0x4000

/** @brief Minimum LE Scan Window (2.5 ms) */
#define SCAN_WINDOW_MIN                      0x0004

/** @brief Maximum LE Scan Window (10.24 s) */
#define SCAN_WINDOW_MAX                      0x4000

/** @brief Scan Refresh Info Request value */
#define SCAN_REFRESH_INFO_REQ                0x00

/* ============================================================================
 *                           Type Definitions
 * ============================================================================ */

#ifndef RTK_SCAN_PARAM_TYPES_DEFINED
#define RTK_SCAN_PARAM_TYPES_DEFINED

/**
 * @brief Scan Interval Window structure
 * @note As defined in Scan Parameters Service Specification v1.0
 *       Time = N * 0.625 ms
 *
 * @important Server Assumption (per spec Section 9):
 *       The Server should assume that the stored values represent the "worst case"
 *       for the Client's scanning behavior:
 *       - Maximum LE_Scan_Interval (least frequent scanning)
 *       - Minimum LE_Scan_Window (shortest scanning duration)
 *       This allows the Server to optimize advertising timing accordingly.
 */
typedef struct {
	uint16_t le_scan_interval;  /**< LE_Scan_Interval (Range: 0x0004-0x4000) */
	uint16_t le_scan_window;    /**< LE_Scan_Window (Range: 0x0004-0x4000) */
} __attribute__((packed)) scan_interval_window_t;

#endif /* RTK_SCAN_PARAM_TYPES_DEFINED */

/**
 * @brief Scan Parameters Client callback event types
 */
typedef enum {
	SCAN_PARAM_CLIENT_EVT_DISCOVERY_COMPLETE,     /**< Service discovery complete */
	SCAN_PARAM_CLIENT_EVT_SCAN_REFRESH_NOTIFY,    /**< Scan Refresh notification received */
} scan_param_client_evt_t;

/**
 * @brief Scan Parameters Client callback function type
 * @param[in] conn_id Connection ID
 * @param[in] event Event type
 * @param[in] data Event data
 * @param[in] len Data length
 */
typedef void (*scan_param_client_cb_t)(uint8_t conn_id, uint8_t event, void *data, uint16_t len);

/* ============================================================================
 *                           Function Declarations
 * ============================================================================ */

/**
 * @brief    Initialize Scan Parameters Client
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_client_add(void);

/**
 * @brief    Attach connection to Scan Parameters Client
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_client_attach_conn(uint16_t conn_handle);

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Start discovery of Scan Parameters Service
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_client_discover(uint16_t conn_handle);
#endif

/**
 * @brief    Write Scan Interval Window
 * @param[in] conn_handle Connection handle
 * @param[in] p_params Pointer to scan parameters
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_client_write_scan_interval_window(uint16_t conn_handle,
													  const scan_interval_window_t *p_params);

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
/**
 * @brief    Enable/Disable Scan Refresh notification
 * @param[in] conn_handle Connection handle
 * @param[in] enable true to enable, false to disable
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_client_set_refresh_notify(uint16_t conn_handle, bool enable);
#endif

/**
 * @brief    Register application callback
 * @param[in] callback Callback function
 */
void scan_param_client_register_callback(scan_param_client_cb_t callback);

/**
 * @brief    Handle connection disconnect
 * @param[in] conn_handle Connection handle
 */
void scan_param_client_disconnect(uint16_t conn_handle);

/**
 * @brief    Deinitialize Scan Parameters Client
 */
void scan_param_client_deinit(void);

/**
 * @brief    Scan Parameters Client unified GATTC app callback (for uniform dispatch)
 * @param[in] event GATTC event code
 * @param[in] data Event data
 * @return    RTK_BT_EVT_CB_OK
 */
rtk_bt_evt_cb_ret_t scan_param_client_app_callback(uint8_t event, void *data);

#ifdef __cplusplus
}
#endif

#endif /* __RTK_SCAN_PARAM_CLIENT_H__ */
