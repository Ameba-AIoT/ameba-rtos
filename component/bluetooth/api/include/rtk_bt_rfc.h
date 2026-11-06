/**
 * @file      rtk_bt_rfc.h
 * @author    sherman_sun@realsil.com.cn
 * @brief     Bluetooth RFCOMM type and function definitions.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_RFC_H__
#define __RTK_BT_RFC_H__

#include <rtk_bt_common.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**************************************** Macros ***************************************/
/**
 * @def       RTK_BT_RFC_DEMO_DEFAULT_MTU_SIZE
 * @brief     Default RFCOMM MTU size, in octets, used when a connection is requested.
 */
#define RTK_BT_RFC_DEMO_DEFAULT_MTU_SIZE 1012
/**
 * @def       RTK_BT_RFC_DEMO_DEFAULT_CREDITS
 * @brief     Default number of RFCOMM credits given to the remote device when a connection is requested.
 */
#define RTK_BT_RFC_DEMO_DEFAULT_CREDITS 10
/************************************** Data Types *************************************/

/**
 * @typedef   rtk_bt_rfc_uuid_type_t
 * @brief     Bluetooth RFCOMM UUID type.
 */
typedef enum {
	RFC_UUID16,    /*!< 16-bit UUID. */
	RFC_UUID32,    /*!< 32-bit UUID. */
	RFC_UUID128    /*!< 128-bit UUID. */
} rtk_bt_rfc_uuid_type_t;

/**
 * @typedef   rtk_bt_rfc_uuid_data_t
 * @brief     Bluetooth RFCOMM UUID data.
 */
typedef union {
	uint16_t    uuid_16;                    /*!< 16-bit UUID value. */
	uint32_t    uuid_32;                    /*!< 32-bit UUID value. */
	uint8_t     uuid_128[16];               /*!< 128-bit UUID value. */
} rtk_bt_rfc_uuid_data_t;

/**
 * @struct    rtk_bt_rfc_attr_info_t
 * @brief     Bluetooth RFCOMM SDP attribute information of the remote device.
 */
typedef struct {
	uint8_t                     bd_addr[6];             /*!< Remote device Bluetooth address. */
	rtk_bt_rfc_uuid_type_t      srv_class_uuid_type;    /*!< Service class UUID type. */
	rtk_bt_rfc_uuid_data_t      srv_class_uuid_data;    /*!< Service class UUID data. */
	uint16_t                    profile_version;        /*!< Profile version of the remote service. */
	uint16_t                    protocol_version;       /*!< Protocol version of the remote service. */
	uint8_t                     server_channel;         /*!< Server channel number of the remote service. */
	uint16_t                    supported_feat;         /*!< Features supported by the remote service. */
	uint16_t                    l2c_psm;                /*!< L2CAP PSM of the remote service. */
	uint8_t                     supported_repos;        /*!< Repositories supported by the remote service. */
	uint32_t                    pbap_supported_feat;    /*!< PBAP features supported by the remote service. */
} rtk_bt_rfc_attr_info_t;

/**
 * @struct    rtk_bt_rfc_data_send_t
 * @brief     Parameters for sending RFCOMM data on a server channel.
 */
typedef struct {
	uint8_t bd_addr[6];      /*!< Remote device Bluetooth address. */
	uint8_t server_chann;    /*!< Local server channel number. */
	uint8_t *p_data;         /*!< Data to send. */
	uint16_t data_len;       /*!< Length of the data to send, in octets. */
} rtk_bt_rfc_data_send_t;

/**
 * @struct    rtk_bt_rfc_conn_ind_t
 * @brief     RFCOMM incoming connection indication.
 */
typedef struct {
	uint8_t bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t local_server_chann;    /*!< Local server channel number. */
	uint16_t frame_size;           /*!< Maximum RFCOMM frame size, in octets. */
} rtk_bt_rfc_conn_ind_t;

/**
 * @struct    rtk_bt_rfc_conn_cmpl_t
 * @brief     RFCOMM connection completion information.
 */
typedef struct {
	uint8_t bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t local_server_chann;    /*!< Local server channel number. */
	uint16_t frame_size;           /*!< Maximum RFCOMM frame size, in octets. */
	uint8_t remain_credits;        /*!< Remaining RFCOMM TX credits. */
} rtk_bt_rfc_conn_cmpl_t;

/**
 * @struct    rtk_bt_rfc_disconn_cmpl_t
 * @brief     RFCOMM disconnection completion information.
 */
typedef struct {
	uint8_t bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t local_server_chann;    /*!< Local server channel number. */
	uint16_t cause;                /*!< Disconnection reason code. */
} rtk_bt_rfc_disconn_cmpl_t;

/**
 * @struct    rtk_bt_rfc_data_ind_t
 * @brief     RFCOMM data received from the remote device.
 */
typedef struct {
	uint8_t bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t local_server_chann;    /*!< Local server channel number. */
	uint8_t remain_credits;        /*!< Remaining RFCOMM RX credits. */
	uint16_t length;               /*!< Length of the received data, in octets. */
	uint8_t *buf;                  /*!< Received data. */
} rtk_bt_rfc_data_ind_t;

/**
 * @struct    rtk_bt_rfc_credit_info_t
 * @brief     RFCOMM credits information.
 */
typedef struct {
	uint8_t local_server_chann;    /*!< Local server channel number. */
	uint8_t remain_credits;        /*!< Remaining RFCOMM TX credits. */
} rtk_bt_rfc_credit_info_t;

/**
 * @struct    rtk_bt_rfc_credits_give_t
 * @brief     Parameters for giving RFCOMM credits to the remote device.
 */
typedef struct {
	uint8_t bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t local_server_chann;    /*!< Local server channel number. */
	uint8_t credits;               /*!< Number of RFCOMM credits the local device gives to the remote device. */
} rtk_bt_rfc_credits_give_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_rfc BT RFC APIs
 * @brief     BT RFC related function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Connect to the RFCOMM service of a remote device.
 *            The service of the remote device is discovered first, and the connection is then established on the discovered server channel.
 *            The result is reported asynchronously via event @ref RTK_BT_RFC_EVT_CONN_CMPL.
 * @param[in] bd_addr: Remote device Bluetooth address, 6 octets.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_rfc_connect(uint8_t *bd_addr);

/**
 * @brief     Disconnect the RFCOMM connection with a remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_RFC_EVT_DISCONN_CMPL.
 * @param[in] bd_addr: Remote device Bluetooth address, 6 octets.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - It returns @ref RTK_BT_FAIL if there is no link to the remote device.
 */
uint16_t rtk_bt_rfc_disconnect(uint8_t *bd_addr);

/**
 * @brief     Send data to the remote device on a server channel.
 * @param[in] p_data_send_t: RFCOMM data sending parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - It returns @ref RTK_BT_FAIL if there is no link to the remote device.
 */
uint16_t rtk_bt_rfc_data_send(rtk_bt_rfc_data_send_t *p_data_send_t);

/**
 * @brief     Give RFCOMM credits to the remote device on a server channel, allowing it to send more data.
 * @param[in] p_credits_give_t: RFCOMM credits giving parameters.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - It returns @ref RTK_BT_FAIL if there is no link to the remote device.
 */
uint16_t rtk_bt_rfc_credits_give(rtk_bt_rfc_credits_give_t *p_credits_give_t);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_RFC_H__ */
