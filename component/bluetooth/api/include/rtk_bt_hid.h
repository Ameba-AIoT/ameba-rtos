/**
 * @file      rtk_bt_hid.h
 * @brief     Bluetooth BR/EDR HID type and function definition.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_HID_H__
#define __RTK_BT_HID_H__

#include <rtk_bt_common.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define RTK_BT_HID_MAX_DESCRIPTOR_LENGTH 400       /*!< HID maximum descriptor length. */
#define RTK_BT_HID_MAX_INPUT_DATA_LENGTH 20        /*!< HID maximum input data length. */

/**
 * @typedef   rtk_bt_hid_role_t
 * @brief     Bluetooth BR/EDR HID device role.
 */
typedef enum {
	RTK_BT_HID_ROLE_GAMEPAD = 0x00,    /*!< HID gamepad device. */
	RTK_BT_HID_ROLE_MOUSE = 0x01,      /*!< HID mouse device. */
} rtk_bt_hid_role_t;

/************************************** Data Types *************************************/

/**
 * @struct    rtk_bt_hid_des_t
 * @brief     Bluetooth BR/EDR HID report descriptor.
 */
typedef struct {
	uint8_t des[RTK_BT_HID_MAX_DESCRIPTOR_LENGTH];    /*!< HID report descriptor data. */
	uint32_t len;                                     /*!< Length of the descriptor data in octets.
                                                       *   Range: 0 to @ref RTK_BT_HID_MAX_DESCRIPTOR_LENGTH. */
} rtk_bt_hid_des_t;

/**
 * @struct    rtk_bt_hid_input_data_t
 * @brief     Bluetooth BR/EDR HID interrupt input report data.
 */
typedef struct {
	uint8_t bd_addr[6];                                /*!< Bluetooth address of the remote device, 6 octets. */
	uint8_t data[RTK_BT_HID_MAX_INPUT_DATA_LENGTH];    /*!< Input report data. */
	uint32_t len;                                      /*!< Length of the input report data in octets.
                                                        *   Range: 0 to @ref RTK_BT_HID_MAX_INPUT_DATA_LENGTH. */
} rtk_bt_hid_input_data_t;

/**
 * @struct    rtk_bt_hid_conn_ind_t
 * @brief     Bluetooth BR/EDR HID connection information.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device, 6 octets. */
} rtk_bt_hid_conn_ind_t;

/**
 * @struct    rtk_bt_hid_disconn_ind_t
 * @brief     Bluetooth BR/EDR HID disconnection information.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Bluetooth address of the remote device, 6 octets. */
	uint16_t cause;        /*!< Disconnection cause code. */
} rtk_bt_hid_disconn_ind_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_hid BT HID APIs
 * @brief     BT HID related function APIs
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Add the HID report descriptor that is published in the HID SDP record.
 * @param[in] des: HID report descriptor data.
 *                 Must not be NULL.
 * @param[in] length: Length of the descriptor data in octets.
 *                    Range: 0 to @ref RTK_BT_HID_MAX_DESCRIPTOR_LENGTH.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall be called before BT is enabled.
 *            - Only one descriptor can be added.
 */
uint16_t rtk_hid_descriptor_add(void *des, uint32_t length);

/**
 * @brief     Initiate a HID connection to the remote device. The result is reported asynchronously via event @ref RTK_BT_HID_EVT_CONN_CMPL.
 * @param[in] bd_addr: Bluetooth address of the remote device, 6 octets.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hid_connect(uint8_t *bd_addr);

/**
 * @brief     Disconnect the HID connection with the remote device. The result is reported asynchronously via event @ref RTK_BT_HID_EVT_DISCONN_CMPL.
 * @param[in] bd_addr: Bluetooth address of the remote device, 6 octets.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hid_disconnect(uint8_t *bd_addr);

/**
 * @brief     Send the response to a HID get report request from the remote device.
 * @param[in] bd_addr: Bluetooth address of the remote device, 6 octets.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hid_get_report_rsp(uint8_t *bd_addr);

/**
 * @brief     Send an input report to the remote device over the HID interrupt channel.
 * @param[in] bd_addr: Bluetooth address of the remote device, 6 octets.
 *                     Must not be NULL.
 * @param[in] data: Input report data.
 *                  Must not be NULL.
 * @param[in] len: Length of the input report data in octets.
 *                 Range: 0 to @ref RTK_BT_HID_MAX_INPUT_DATA_LENGTH.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hid_input_data_send(uint8_t *bd_addr, uint8_t *data, uint32_t len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_HID_H__ */
