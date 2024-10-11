/**
 * @file      rtk_bt_rfc.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth RFC part type and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_RFC_H__
#define __RTK_BT_RFC_H__

#include <rtk_bt_common.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* ------------------------------- Macros ------------------------------- */
#define RTK_BT_RFC_DEMO_DEFAULT_MTU_SIZE 1012
#define RTK_BT_RFC_DEMO_DEFAULT_CREDITS 10
/* ------------------------------- Data Types ------------------------------- */

/**
 * @typedef   rtk_bt_rfc_uuid_type_t
 * @brief     rfc uuid type
 */
typedef enum {
	RFC_UUID16,                             /*!< UUID type 16 bits */
	RFC_UUID32,                             /*!< UUID type 32 bits */
	RFC_UUID128                             /*!< UUID type 128 bits */
} rtk_bt_rfc_uuid_type_t;

/**
 * @typedef   rtk_bt_rfc_uuid_data_t
 * @brief     rfc uuid data
 */
typedef union {
	uint16_t    uuid_16;                    /*!< UUID data in 16 bits */
	uint32_t    uuid_32;                    /*!< UUID data in 32 bits */
	uint8_t     uuid_128[16];               /*!< UUID data in 128 bits */
} rtk_bt_rfc_uuid_data_t;

/**
 * @struct    rtk_bt_rfc_attr_info_t
 * @brief     Bluetooth RFC SDP attribute information struct
 */
typedef struct {
	uint8_t                     bd_addr[6];                 /**< Address */
	rtk_bt_rfc_uuid_type_t      srv_class_uuid_type;        /**< Service class UUID type */
	rtk_bt_rfc_uuid_data_t      srv_class_uuid_data;        /**< Service class UUID data */
	uint16_t                    profile_version;            /**< Remote profile version */
	uint16_t                    protocol_version;           /**< Remote protocol version */
	uint8_t                     server_channel;             /**< Server channel number */
	uint16_t                    supported_feat;             /**< Supported features */
	uint16_t                    l2c_psm;                    /**< L2CAP PSM value */
	uint8_t                     supported_repos;            /**< Supported repositories */
	uint32_t                    pbap_supported_feat;        /**< PBAP supported features */
} rtk_bt_rfc_attr_info_t;

/**
 * @struct    rtk_bt_rfc_data_send_t
 * @brief     Bluetooth RFC data send struct.
 */
typedef struct {
	uint8_t bd_addr[6];                                /*!< address */
	uint8_t server_chann;                              /*!< channel number */
	uint8_t *p_data;                                   /*!< pointer to the data.*/
	uint16_t data_len;                                 /*!< data length */
} rtk_bt_rfc_data_send_t;

/**
 * @struct    rtk_bt_rfc_conn_ind_t
 * @brief     rfc connection indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	uint8_t local_server_chann;                         /*!< channel number */
	uint16_t frame_size;                                /*!< frame size */
} rtk_bt_rfc_conn_ind_t;

/**
 * @struct    rtk_bt_rfc_conn_cmpl_t
 * @brief     rfc connection completion indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	uint8_t local_server_chann;                         /*!< channel number */
	uint16_t frame_size;                                /*!< frame size */
	uint8_t remain_credits;                             /*!< remain credits */
} rtk_bt_rfc_conn_cmpl_t;

/**
 * @struct    rtk_bt_rfc_disconn_cmpl_t
 * @brief     rfc disconnection completion indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	uint8_t local_server_chann;                         /*!< channel number */
	uint16_t cause;                                     /*!< disconnection cause */
} rtk_bt_rfc_disconn_cmpl_t;

/**
 * @struct    rtk_bt_rfc_data_ind_t
 * @brief     rfc data indication.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	uint8_t local_server_chann;                         /*!< channel number */
	uint8_t remain_credits;                             /*!< remain credits */
	uint16_t length;                                    /*!< data length */
	uint8_t *buf;                                       /*!< pointer of the data */
} rtk_bt_rfc_data_ind_t;

/**
 * @struct    rtk_bt_rfc_credit_info_t
 * @brief     rfc receive credits infomation.
 */
typedef struct {
	uint8_t local_server_chann;                         /*!< channel number */
	uint8_t remain_credits;                             /*!< remain credits */
} rtk_bt_rfc_credit_info_t;

/**
 * @struct    rtk_bt_rfc_credits_give_t
 * @brief     rfc give credits struct.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	uint8_t local_server_chann;                         /*!< channel number */
	uint8_t credits;                                    /*!< The number of credit */
} rtk_bt_rfc_credits_give_t;

/* ------------------------- Functions Declaration ------------------------- */
/**
 * @defgroup  bt_rfc BT RFC APIs
 * @brief     BT RFC related function APIs
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Send RFC connect request.
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_rfc_connect(uint8_t *bd_addr);

/**
 * @brief     Send an RFC disconnection request
 * @param[in] bd_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_rfc_disconnect(uint8_t *bd_addr);

/**
 * @brief     Send data to peer device on specific rfc channel
 * @param[in] p_data_send_t: RFC send data struct pointer
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_rfc_data_send(rtk_bt_rfc_data_send_t *p_data_send_t);

/**
 * @brief     Send credits to peer device on specific rfc channel
 * @param[in] p_credits_give_t: RFC give credits struct pointer
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_rfc_credits_give(rtk_bt_rfc_credits_give_t *p_credits_give_t);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_SPP_H__ */