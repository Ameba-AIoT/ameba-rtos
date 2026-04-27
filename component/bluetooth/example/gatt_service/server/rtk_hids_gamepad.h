/**
 * @file      rtk_hids_gamepad.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     Gamepad HID Service header file
 * @copyright Copyright (c) 2026. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_HIDS_GAMEPAD_H__
#define __RTK_HIDS_GAMEPAD_H__

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

/** @brief HID Protocol Modes (Fixed: Report Protocol Mode only, per HOGP PTS) */
#define GAMEPAD_PROTOCOL_MODE_REPORT 0x01

/** @brief HID Control Point Commands */
#define HID_CONTROL_POINT_SUSPEND       0x01
#define HID_CONTROL_POINT_EXIT_SUSPEND  0x02

/** @brief HID Report Types (for Report Reference Descriptor) */
#define HID_INPUT_TYPE   1
#define HID_OUTPUT_TYPE  2
#define HID_FEATURE_TYPE 3

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
 * @brief HID Information structure
 * @note As defined in HID Service specification
 */
typedef struct {
	uint16_t bcd_hid;      /**< HID version (e.g., 0x0111 for v1.11) */
	uint8_t b_country_code;/**< Country code (0x00 = Not localized) */
	uint8_t flags;         /**< Remote wake / Normally connectable flags */
} __attribute__((packed)) hid_info_t;

/**
 * @brief Gamepad HID Service callback event types
 */
typedef enum {
	GAMEPAD_HID_EVT_NONE = 0,
	GAMEPAD_HID_EVT_PROTOCOL_MODE_CHANGE,   /**< Protocol mode changed */
	GAMEPAD_HID_EVT_CONTROL_POINT,          /**< Control point command received */
	GAMEPAD_HID_EVT_OUTPUT_REPORT_WRITE,    /**< Output report written */
	GAMEPAD_HID_EVT_CCCD_ENABLE,            /**< Notification enabled */
	GAMEPAD_HID_EVT_CCCD_DISABLE,           /**< Notification disabled */
} gamepad_hid_evt_t;

/**
 * @brief Gamepad HID Service callback data
 */
typedef struct {
	gamepad_hid_evt_t event;                /**< Event type */
	uint8_t conn_id;                        /**< Connection ID */
	union {
		uint8_t protocol_mode;              /**< New protocol mode */
		uint8_t control_point;              /**< Control point command */
		gamepad_output_report_t output;     /**< Output report data */
	} data;
} gamepad_hid_cb_data_t;

/**
 * @brief Gamepad HID Service callback function type
 * @param[in] p_cb_data Pointer to callback data
 */
typedef void (*gamepad_hid_cb_t)(gamepad_hid_cb_data_t *p_cb_data);

/* ============================================================================
 *                           Function Declarations
 * ============================================================================ */

/**
 * @brief    Add Gamepad HID Service
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t gamepad_hid_srv_add(void);

/**
 * @brief    Register Gamepad HID Service callback
 * @param[in] callback Callback function
 */
void gamepad_hid_srv_register_callback(gamepad_hid_cb_t callback);

/**
 * @brief    Send Input Report notification
 * @param[in] conn_handle Connection handle
 * @param[in] p_report Pointer to input report data
 * @return   RTK_BT_OK on success, error code otherwise
 */
uint16_t gamepad_hid_srv_send_input_report(uint16_t conn_handle,
										   const gamepad_input_report_t *p_report);

/**
 * @brief    Get current Protocol Mode (always returns Report Protocol Mode)
 * @return   Current protocol mode
 */
uint8_t gamepad_hid_srv_get_protocol_mode(void);

/**
 * @brief    Check if notification is enabled
 * @param[in] conn_handle Connection handle
 * @return   true if enabled, false otherwise
 */
bool gamepad_hid_srv_is_notify_enabled(uint16_t conn_handle);

/**
 * @brief    Handle disconnect for Gamepad HID Service
 * @param[in] conn_handle Connection handle
 */
void gamepad_hid_srv_disconnect(uint16_t conn_handle);

/**
 * @brief    Gamepad HID Service callback function
 * @param[in] event GATTS event code
 * @param[in] data Event data
 */
void gamepad_hid_srv_callback(uint8_t event, void *data);

/**
 * @brief    Deinitialize Gamepad HID Service
 */
void gamepad_hid_srv_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __RTK_HIDS_GAMEPAD_H__ */
