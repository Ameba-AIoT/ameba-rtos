/**
 * @file      rtk_bt_spp.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     Bluetooth SPP type and function definitions.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_SPP_H__
#define __RTK_BT_SPP_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**************************************** Macros ***************************************/
/**
 * @def       SPP_CAHNN_DB_MAX_LINK_NUM
 * @brief     Number of entries in the SPP channel database.
 * @note
 *            - The maximum number of SPP channels shall not exceed this value.
 */
#define SPP_CAHNN_DB_MAX_LINK_NUM               5

/**
 * @def       RTK_BT_SPP_MAX_DATA_IND_LENGTH
 * @brief     Maximum length, in octets, of the data buffer of a received SPP data indication.
 */
#define RTK_BT_SPP_MAX_DATA_IND_LENGTH          512

/**
 * @def       RTK_BT_SPP_MAX_SRV_CLASS_UUID_LENGTH
 * @brief     Maximum length, in octets, of an SPP service class UUID.
 */
#define RTK_BT_SPP_MAX_SRV_CLASS_UUID_LENGTH    16

/************************************** Data Types *************************************/
/**
 * @typedef   rtk_bt_spp_role_t
 * @brief     Bluetooth SPP role.
 */
typedef enum {
	RTK_BT_SPP_ROLE_CLIENT = 0x00,    /*!< SPP client. */
	RTK_BT_SPP_ROLE_SERVER = 0x01,    /*!< SPP server. */
} rtk_bt_spp_role_t;

/**
 * @typedef   rtk_bt_spp_uuid_type_t
 * @brief     Bluetooth SPP UUID type.
 */
typedef enum {
	SPP_UUID16,                       /*!< 16-bit UUID. */
	SPP_UUID32,                       /*!< 32-bit UUID. */
	SPP_UUID128                       /*!< 128-bit UUID. */
} rtk_bt_spp_uuid_type_t;

/**
 * @typedef   rtk_bt_spp_uuid_data_t
 * @brief     Bluetooth SPP UUID data.
 */
typedef union {
	uint16_t    uuid_16;              /*!< 16-bit UUID value. */
	uint32_t    uuid_32;              /*!< 32-bit UUID value. */
	uint8_t     uuid_128[16];         /*!< 128-bit UUID value. */
} rtk_bt_spp_uuid_data_t;

/**
 * @struct    T_SPP_CHANN_DB
 * @brief     SPP channel database entry, one per registered service channel.
 */
typedef struct {
	bool                    used;                   /*!< Whether this entry is in use.
                                                     *   - true: The entry is in use.
                                                     *   - false: The entry is free. */
	uint8_t                 spp_chann_num;          /*!< Local SPP server channel number registered for this entry. */
	rtk_bt_spp_uuid_data_t  service_uuid;           /*!< Service UUID. */
	bool                    is_spp_sdp_ok;          /*!< Whether the SDP discovery result of the remote service has been received.
                                                     *   - true: The SDP discovery result has been received.
                                                     *   - false: The SDP discovery result has not been received. */
	bool                    is_on_connection;       /*!< Whether an SPP connection is established on this channel.
                                                     *   - true: The connection is established.
                                                     *   - false: There is no connection. */
	uint8_t                 local_server_chann;     /*!< Local SPP server channel number. */
	uint8_t                 remote_server_chann;    /*!< Remote SPP server channel number. */
} T_SPP_CHANN_DB;

/**
 * @struct    rtk_bt_spp_attr_info_t
 * @brief     Bluetooth SPP SDP attribute information of the remote device.
 */
typedef struct {
	uint8_t                     bd_addr[6];             /*!< Remote device Bluetooth address. */
	rtk_bt_spp_uuid_type_t      srv_class_uuid_type;    /*!< Service class UUID type. */
	rtk_bt_spp_uuid_data_t      srv_class_uuid_data;    /*!< Service class UUID data. */
	uint16_t                    profile_version;        /*!< Profile version of the remote service. */
	uint16_t                    protocol_version;       /*!< Protocol version of the remote service. */
	uint8_t                     server_channel;         /*!< Server channel number of the remote service. */
	uint16_t                    supported_feat;         /*!< Features supported by the remote service. */
	uint16_t                    l2c_psm;                /*!< L2CAP PSM of the remote service. */
	uint8_t                     supported_repos;        /*!< Repositories supported by the remote service. */
	uint32_t                    pbap_supported_feat;    /*!< PBAP features supported by the remote service. */
} rtk_bt_spp_attr_info_t;

/**
 * @struct    rtk_bt_spp_init_cfg_t
 * @brief     SPP initialization configuration.
 */
typedef struct {
	uint8_t spp_max_link_num;           /*!< Maximum number of simultaneous SPP links. */
	uint8_t spp_rfc_max_service_num;    /*!< Maximum number of SPP services. */
	uint16_t default_mtu_size;          /*!< Default MTU size of the link, in octets. */
	uint8_t default_credits;            /*!< Default number of credits of the link. */
} rtk_bt_spp_init_cfg_t;

/**
 * @struct    rtk_bt_spp_srv_cfg_t
 * @brief     SPP service registration configuration.
 */
typedef struct {
	uint8_t local_server_chann;                                         /*!< Local server channel number. */
	uint32_t length;                                                    /*!< Length of the service class UUID data, in octets. */
	uint8_t srv_class_uuid128[RTK_BT_SPP_MAX_SRV_CLASS_UUID_LENGTH];    /*!< Service class UUID data. */
} rtk_bt_spp_srv_cfg_t;

/**
 * @struct    rtk_bt_spp_sdp_discov_cmpl_t
 * @brief     SPP SDP discovery completion information.
 */
typedef struct {
	uint8_t bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint16_t cause;                /*!< Result code of the SDP discovery. */
} rtk_bt_spp_sdp_discov_cmpl_t;

/**
 * @struct    rtk_bt_spp_conn_ind_t
 * @brief     SPP incoming connection indication.
 */
typedef struct {
	uint8_t bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t local_server_chann;    /*!< Local server channel number. */
	uint16_t frame_size;           /*!< Maximum SPP frame size, in octets. */
} rtk_bt_spp_conn_ind_t;

/**
 * @struct    rtk_bt_spp_disconn_ind_t
 * @brief     SPP disconnection information.
 */
typedef struct {
	uint8_t bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t local_server_chann;    /*!< Local server channel number. */
	uint16_t cause;                /*!< Disconnection reason code. */
} rtk_bt_spp_disconn_ind_t;

/**
 * @struct    rtk_bt_spp_conn_cmpl_t
 * @brief     SPP connection completion information.
 */
typedef struct {
	uint8_t bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t local_server_chann;    /*!< Local server channel number. */
	uint8_t  link_credit;          /*!< Remaining SPP TX link credits. */
	uint16_t frame_size;           /*!< Maximum SPP frame size, in octets. */
} rtk_bt_spp_conn_cmpl_t;

/**
 * @struct    rtk_bt_spp_disconn_cmpl_t
 * @brief     SPP disconnection completion information.
 */
typedef struct {
	uint8_t bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t local_server_chann;    /*!< Local server channel number. */
	uint16_t cause;                /*!< Disconnection reason code. */
} rtk_bt_spp_disconn_cmpl_t;

/**
 * @struct    rtk_bt_spp_credit_rcvd_t
 * @brief     SPP link credits received from the remote device.
 */
typedef struct {
	uint8_t bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t local_server_chann;    /*!< Local server channel number. */
	uint8_t link_credit;           /*!< Number of SPP link credits received. */
} rtk_bt_spp_credit_rcvd_t;

/**
 * @struct    rtk_bt_spp_data_ind_t
 * @brief     SPP data received from the remote device.
 */
typedef struct {
	uint8_t bd_addr[6];                              /*!< Remote device Bluetooth address. */
	uint8_t local_server_chann;                      /*!< Local server channel number. */
	uint8_t data[RTK_BT_SPP_MAX_DATA_IND_LENGTH];    /*!< Received data buffer. */
	uint16_t len;                                    /*!< Length of the received data, in octets. */
} rtk_bt_spp_data_ind_t;

/**
 * @struct    rtk_bt_spp_data_rsp_t
 * @brief     SPP data response information for data sent with the acknowledge flag set.
 */
typedef struct {
	uint8_t bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t local_server_chann;    /*!< Local server channel number. */
} rtk_bt_spp_data_rsp_t;

/**
 * @struct    rtk_bt_spp_send_data_t
 * @brief     Parameters for sending SPP data on a server channel.
 */
typedef struct {
	uint8_t  bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t  local_server_chann;    /*!< Local server channel number. */
	uint8_t  *data;                 /*!< Data buffer to send. */
	uint16_t len;                   /*!< Length of the data to send, in octets. */
} rtk_bt_spp_send_data_t;

/**
 * @struct    rtk_bt_spp_disconn_req_t
 * @brief     Parameters for disconnecting an SPP connection on a server channel.
 */
typedef struct {
	uint8_t  bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t  local_server_chann;    /*!< Local server channel number. */
} rtk_bt_spp_disconn_req_t;

/**
 * @struct    rtk_bt_spp_credits_give_t
 * @brief     Parameters for giving SPP link credits to the remote device.
 */
typedef struct {
	uint8_t  bd_addr[6];            /*!< Remote device Bluetooth address. */
	uint8_t  local_server_chann;    /*!< Local server channel number. */
	uint8_t  credits;               /*!< Number of SPP RX link credits the local device gives to the remote device. */
} rtk_bt_spp_credits_give_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_spp BT SPP APIs
 * @brief     BT SPP related function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Configure the SPP initialization parameters.
 * @param[in] max_link_num: Maximum number of simultaneous SPP links.
 * @param[in] max_service_num: Maximum number of SPP services.
 * @param[in] mtu_size: Default MTU size of the link, in octets.
 * @param[in] credits: Default number of credits of the link.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall be called before the BT stack is enabled, otherwise it returns @ref RTK_BT_FAIL.
 */
uint16_t rtk_bt_spp_init_cfg(uint8_t max_link_num, uint8_t max_service_num, uint16_t mtu_size, uint8_t credits);

/**
 * @brief     Register an SPP service class UUID together with its local server channel.
 * @param[in] spp_service_class_uuid: Service class UUID to register.
 *                                    Must not be NULL.
 * @param[in] length: Length of the service class UUID, in octets.
 * @param[in] local_server_chann: Local server channel number.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall be called before the BT stack is enabled, otherwise it returns @ref RTK_BT_FAIL.
 *            - It returns @ref RTK_BT_FAIL if the SPP channel database has no free entry.
 */
uint16_t rtk_bt_service_register_cfg(void *spp_service_class_uuid, uint32_t length, uint8_t local_server_chann);

/**
 * @brief     Connect to the SPP service of a remote device.
 *            The service of the remote device is discovered first, and the connection is then established on the discovered server channel.
 *            The result is reported asynchronously via event @ref RTK_BT_SPP_EVT_CONN_COMPL.
 * @param[in] bd_addr: Remote device Bluetooth address, 6 octets.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The connection proceeds only if the service class UUID of the remote service has been registered by @ref rtk_bt_service_register_cfg.
 *              Otherwise no connection is established and no event is reported.
 */
uint16_t rtk_bt_spp_connect(uint8_t *bd_addr);

/**
 * @brief     Disconnect the SPP connection on a server channel.
 *            The result is reported asynchronously via event @ref RTK_BT_SPP_EVT_DISCONN_CMPL.
 * @param[in] p_disconn_req_t: SPP disconnection parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - It returns @ref RTK_BT_FAIL if there is no link to the remote device.
 */
uint16_t rtk_bt_spp_disconnect(rtk_bt_spp_disconn_req_t *p_disconn_req_t);

/**
 * @brief     Disconnect all SPP connections with a remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_SPP_EVT_DISCONN_CMPL.
 * @param[in] bd_addr: Remote device Bluetooth address, 6 octets.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - It returns @ref RTK_BT_FAIL if there is no link to the remote device.
 *            - The event is reported once for each disconnected SPP connection.
 */
uint16_t rtk_bt_spp_disconnect_all(uint8_t *bd_addr);

/**
 * @brief     Send data to the remote device on a server channel.
 *            Each successful call consumes one SPP TX link credit.
 * @param[in] p_send_data_t: SPP data sending parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - It returns @ref RTK_BT_FAIL if there is no link to the remote device or no TX link credit is left.
 *            - New TX link credits are reported via event @ref RTK_BT_SPP_EVT_CREDIT_RCVD.
 */
uint16_t rtk_bt_spp_send_data(rtk_bt_spp_send_data_t *p_send_data_t);

/**
 * @brief     Give SPP RX link credits to the remote device, allowing it to send more data.
 * @param[in] p_credits_give_t: SPP credits giving parameters.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - It returns @ref RTK_BT_FAIL if there is no link to the remote device.
 */
uint16_t rtk_bt_spp_credits_give(rtk_bt_spp_credits_give_t *p_credits_give_t);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_SPP_H__ */
