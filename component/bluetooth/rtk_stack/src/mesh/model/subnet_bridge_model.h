/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     subnet_bridge_model.h
* @brief    Head file for private beacon models.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2021-01-08
* @version  v1.0
* *************************************************************************************
*/

#ifndef _SUBNET_BRIDGE_MODEL_H_
#define _SUBNET_BRIDGE_MODEL_H_


#include "mesh_api.h"

#if MESH_SBR

BEGIN_DECLS

/**
 * @addtogroup SUBNET_BRIDGE
 * @{
 */

/**
 * @defgroup SUBNET_BRIDGE_ACCESS_OPCODE Access Opcode
 * @brief Mesh message access opcode
 * @{
 */
#define MESH_MSG_SUBNET_BRIDGE_GET                          0x80B1
#define MESH_MSG_SUBNET_BRIDGE_SET                          0x80B2
#define MESH_MSG_SUBNET_BRIDGE_STATUS                       0x80B3
#define MESH_MSG_BRIDGING_TABLE_ADD                         0x80B4
#define MESH_MSG_BRIDGING_TABLE_REMOVE                      0x80B5
#define MESH_MSG_BRIDGING_TABLE_STATUS                      0x80B6
#define MESH_MSG_BRIDGED_SUBNETS_GET                        0x80B7
#define MESH_MSG_BRIDGED_SUBNETS_LIST                       0x80B8
#define MESH_MSG_BRIDGING_TABLE_GET                         0x80B9
#define MESH_MSG_BRIDGING_TABLE_LIST                        0x80BA
#define MESH_MSG_BRIDGING_TABLE_SIZE_GET                    0x80BB
#define MESH_MSG_BRIDGING_TABLE_SIZE_STATUS                 0x80BC
/** @} */

/**
 * @defgroup SUBNET_BRIDGE_MODEL_ID Model ID
 * @brief Mesh model id
 * @{
 */
#define MESH_MODEL_SUBNET_BRIDGE_SERVER                     0x0008FFFF
#define MESH_MODEL_SUBNET_BRIDGE_CLIENT                     0x0009FFFF
/** @} */

/**
 * @defgroup SUBNET_BRIDGE_MESH_MSG Mesh Msg
 * @brief Mesh message types used by models
 * @{
 */
typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_SUBNET_BRIDGE_GET)];
} _PACKED4_ subnet_bridge_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_SUBNET_BRIDGE_SET)];
    subnet_bridge_state_t state;
} _PACKED4_ subnet_bridge_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_SUBNET_BRIDGE_STATUS)];
    subnet_bridge_state_t state;
} _PACKED4_ subnet_bridge_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_BRIDGING_TABLE_ADD)];
    uint32_t directions : 8;
    uint32_t net_key_index1 : 12;
    uint32_t net_key_index2 : 12;
    uint16_t addr1;
    uint16_t addr2;
} _PACKED4_ bridging_table_add_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_BRIDGING_TABLE_REMOVE)];
    uint8_t net_key_index[3];
    uint16_t addr1;
    uint16_t addr2;
} _PACKED4_ bridging_table_remove_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_BRIDGING_TABLE_STATUS)];
    uint8_t status;
    uint32_t directions : 8;
    uint32_t net_key_index1 : 12;
    uint32_t net_key_index2 : 12;
    uint16_t addr1;
    uint16_t addr2;
} _PACKED4_ bridging_table_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_BRIDGED_SUBNETS_GET)];
    uint16_t filter : 2;
    uint16_t prohibited : 2;
    uint16_t net_key_index : 12;
    uint8_t start_index;
} _PACKED4_ bridged_subnets_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_BRIDGED_SUBNETS_LIST)];
    uint16_t filter : 2;
    uint16_t prohibited : 2;
    uint16_t net_key_index : 12;
    uint8_t start_index;
    uint8_t bridged_subnets_list[0];
} _PACKED4_ bridged_subnets_list_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_BRIDGING_TABLE_GET)];
    uint8_t net_key_index[3];
    uint16_t start_index;
} _PACKED4_ bridging_table_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_BRIDGING_TABLE_LIST)];
    uint32_t status : 8;
    uint32_t net_key_index1 : 12;
    uint32_t net_key_index2 : 12;
    uint16_t start_index;
    uint8_t bridged_addrs_list[0];
} _PACKED4_ bridging_table_list_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_BRIDGING_TABLE_SIZE_GET)];
} _PACKED4_ bridging_table_size_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_BRIDGING_TABLE_SIZE_STATUS)];
    uint16_t bridging_table_size;
} _PACKED4_ bridging_table_size_status_t;

typedef struct
{
    uint16_t addr1;
    uint16_t addr2;
    uint8_t directions;
} _PACKED4_ bridged_address_entry_t;
/** @} */

/**
 * @defgroup SUBNET_BRIDGE_SERVER_API Server API
 * @brief Functions declaration
 * @{
 */

/**
 * @brief register subnet bridge server model
 * @param[in] element_index: element index that model registered to
 * @retval true: register success
 * @retval false: register failed
 */
bool subnet_bridge_server_reg(uint8_t element_index);

/** @} */

/**
 * @defgroup SUBNET_BRIDGE_CLIENT_API Client API
 * @brief Functions declaration
 * @{
 */

/**
 * @brief register subnet bridge client
 * @param[in] element_index: element index that model registered to
 * @retval TRUE: register success
 * @retval FALSE: register failed
 */
bool subnet_bridge_client_reg(uint8_t element_index);

/**
 * @brief subnet bridget get
 * @param[in] dst: destination address
 * @param[in] net_key_index: network key index used in this message
 * @return send result
 */
mesh_msg_send_cause_t subnet_bridge_get(uint16_t dst, uint16_t net_key_index);

/**
 * @brief subnet bridge set
 * @param[in] dst: destination address
 * @param[in] net_key_index: network key index used in this message
 * @param[in] state: subnet bridge new state
 * @return send result
 */
mesh_msg_send_cause_t subnet_bridge_set(uint16_t dst, uint16_t net_key_index,
                                        subnet_bridge_state_t state);

/**
 * @brief add bridging table
 * @param[in] dst: destination address
 * @param[in] net_key_index: network key index used in this message
 * @param[in] directions: subnet bridge directions
 * @param[in] net_key_index1_g: global network key index in subnet bridge
 * @param[in] net_key_index2_g: global network key index in subnet bridge
 * @param[in] addr1: address in subnet bridge
 * @param[in] addr2: address in subnet bridge
 * @return send result
 */
mesh_msg_send_cause_t bridging_table_add(uint16_t dst, uint16_t net_key_index, uint8_t directions,
                                         uint16_t net_key_index1, uint16_t net_key_index2, uint16_t addr1, uint16_t addr2);

/**
 * @brief remove bridging table
 * @param[in] dst: destination address
 * @param[in] net_key_index: network key index used in this message
 * @param[in] net_key_index1: network key index in bridging table
 * @param[in] net_key_index2: network key index in bridging table
 * @param[in] addr1: address in bridging table
 * @param[in] addr2: address in bridging table
 * @return send result
 */
mesh_msg_send_cause_t bridging_table_remove(uint16_t dst, uint16_t net_key_index,
                                            uint16_t net_key_index1, uint16_t net_key_index2, uint16_t addr1, uint16_t addr2);

/**
 * @brief get bridged subnets
 * @param[in] dst: destination address
 * @param[in] net_key_index: network key index used in this message
 * @param[in] filter: filter used in bridged table
 * @param[in] filter_net_key_index: filter network key index1 and index2
 * @param[in] start_index: start index in bridging table list
 * @return send result
 */
mesh_msg_send_cause_t bridged_subnets_get(uint16_t dst, uint16_t net_key_index,
                                          subnets_filter_t filter, uint16_t filter_net_key_index, uint8_t start_index);

/**
 * @brief get bridging table
 * @param[in] dst: destination address
 * @param[in] net_key_index: network key index used in this message
 * @param[in] net_key_index1: network key index in bridging table
 * @param[in] net_key_index2: network key index in bridging table
 * @param[in] start_index: start index in bridging table list
 * @return send result
 */
mesh_msg_send_cause_t bridging_table_get(uint16_t dst, uint16_t net_key_index,
                                         uint16_t net_key_index1, uint16_t net_key_index2, uint16_t start_index);

/**
 * @brief get bridging table size
 * @param[in] dst: destination address
 * @param[in] net_key_index: network key index used in this message
 * @return send result
 */
mesh_msg_send_cause_t bridging_table_size_get(uint16_t dst, uint16_t net_key_index);
/** @} */
/** @} */

END_DECLS

#endif

#endif /* _SUBNET_BRIDGE_MODEL_H_ */
