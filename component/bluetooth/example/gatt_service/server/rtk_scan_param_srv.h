/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __RTK_SCAN_PARAM_SRV_H__
#define __RTK_SCAN_PARAM_SRV_H__

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
 * @brief Scan Refresh values
 */
typedef enum {
	SCAN_REFRESH_INFO_REQ_ENUM = 0x00  /**< Host requests refresh of scan parameters */
} scan_refresh_t;

/**
 * @brief Scan Parameters Service callback event types
 */
typedef enum {
	SCAN_PARAM_EVT_NONE = 0,
	SCAN_PARAM_EVT_SCAN_INTERVAL_WINDOW_WRITE,  /**< Scan Interval Window written */
	SCAN_PARAM_EVT_SCAN_REFRESH_CCCD_ENABLE,    /**< Scan Refresh notification enabled */
	SCAN_PARAM_EVT_SCAN_REFRESH_CCCD_DISABLE,   /**< Scan Refresh notification disabled */
} scan_param_evt_t;

/**
 * @brief Scan Parameters Service callback data
 */
typedef struct {
	scan_param_evt_t event;                  /**< Event type */
	uint8_t conn_id;                         /**< Connection ID */
	scan_interval_window_t scan_params;      /**< Scan parameters (for write event) */
} scan_param_cb_data_t;

/**
 * @brief Scan Parameters Service callback function type
 * @param[in] p_cb_data Pointer to callback data
 */
typedef void (*scan_param_cb_t)(scan_param_cb_data_t *p_cb_data);

/* ============================================================================
 *                           Function Declarations
 * ============================================================================ */

/**
* @brief    Scan Parameters Service callback
* @param[in] event Event type
* @param[in] data Event data
*/
void scan_param_srv_callback(uint8_t event, void *data);

/**
 * @brief    Add Scan Parameters Service
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_srv_add(void);

/**
 * @brief    Register Scan Parameters Service callback
 * @param[in] callback Callback function
 */
void scan_param_srv_register_callback(scan_param_cb_t callback);

/**
 * @brief    Request scan parameters refresh
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 * @note     Send notification to client to refresh scan parameters
 */
uint16_t scan_param_srv_request_refresh(uint16_t conn_handle);

/**
 * @brief    Get current scan parameters
 * @param[in] conn_handle Connection handle
 * @param[out] p_params Pointer to store scan parameters
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t scan_param_srv_get_scan_params(uint16_t conn_handle, scan_interval_window_t *p_params);

/**
 * @brief    Check if Scan Refresh notification is enabled
 * @param[in] conn_handle Connection handle
 * @return   true if enabled, false otherwise
 */
bool scan_param_srv_is_refresh_notify_enabled(uint16_t conn_handle);

/**
 * @brief    Handle connect for Scan Parameters Service
 * @param[in] conn_handle Connection handle
 * @return   RTK_BT_OK on success, error code otherwise
 * @note     Should be called when a LE connection is established
 */
uint16_t scan_param_srv_connect(uint16_t conn_handle);

/**
 * @brief    Handle disconnect for Scan Parameters Service
 * @param[in] conn_handle Connection handle
 */
void scan_param_srv_disconnect(uint16_t conn_handle);

/**
 * @brief    Deinitialize Scan Parameters Service
 */
void scan_param_srv_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __RTK_SCAN_PARAM_SRV_H__ */
