/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     subnet_bridge.h
* @brief    Head file for subnet bridge.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2021-01-08
* @version  v1.0
* *************************************************************************************
*/

/* Define to prevent recursive inclusion */
#ifndef _SUBNET_BRIDGE_H
#define _SUBNET_BRIDGE_H

/* Add Includes here */
#include "platform_types.h"
#include "mesh_config.h"

BEGIN_DECLS

#if MESH_SBR
/** @addtogroup SUBNET_BRIDGE
  * @{
  */

/** @defgroup Subnet_Bridge_Exported_Macros Exported Macros
  * @{
  */
/** @} */

/** @defgroup Subnet_Bridge_Exported_Types Exported Types
  * @{
  */
enum
{
    SUBNET_BRIDGE_STATE_DISABLE,
    SUBNET_BRIDGE_STATE_ENABLE,
} _SHORT_ENUM_;
typedef uint8_t subnet_bridge_state_t;

enum
{
    BRIDGING_DIRECTIONS_PROHIBITED,
    BRIDGING_DIRECTIONS_ONE_WAY,
    BRIDGING_DIRECTIONS_TWO_WAY,
} _SHORT_ENUM_;
typedef uint8_t bridging_directions_t;

typedef struct
{
    bool used;
    uint8_t directions;
    uint16_t net_key_index1_g;
    uint16_t net_key_index1;
    uint16_t net_key_index2_g;
    uint16_t net_key_index2;
    uint16_t addr1;
    uint16_t addr2;
} bridging_table_t;

enum
{
    SUBNETS_FILTER_ALL_PAIRS,
    SUBNETS_FILTER_NET_KEY_INDEX1,
    SUBNETS_FILTER_NET_KEY_INDEX2,
    SUBNETS_FILTER_NET_KEY_INDEX_EITHER,
} _SHORT_ENUM_;
typedef uint8_t subnets_filter_t;
/** @} */

/** @defgroup Subnet_Bridge_DATA_CALLBACKS Data Callbacks
  * @{
  */
/** @} */

/** @defgroup Subnet_Bridge_Exported_Functions Exported Functions
  * @{
  */
/**
 * @brief initialize subnet bridge
 * @param[in] bridging_table_size: maximum bridging table size
 * @return initialize result
 */
bool subnet_bridge_init(uint16_t bridging_table_size);

/**
 * @brief deinitialize subnet bridge
 *
 */
void subnet_bridge_deinit(void);

/**
 * @brief get subnet bridge table
 * @param[in] net_key_index1: network key index in bridging table
 * @param[in] net_key_index2: network key index in bridging table
 * @param[in] addr1: address in bridging table
 * @param[in] addr2: address in bridging table
 * @return bridging table pointer
 */
bridging_table_t *subnet_bridge_table_get(uint16_t net_key_index1, uint16_t net_key_index2,
                                          uint16_t addr1, uint16_t addr2);

/**
 * @brief get subnet bridge table index
 * @param[in] net_key_index1: network key index in bridging table
 * @param[in] net_key_index2: network key index in bridging table
 * @param[in] addr1: address in bridging table
 * @param[in] addr2: address in bridging table
 * @return bridging table index
 */
uint16_t subnet_bridge_table_get_index(uint16_t net_key_index1, uint16_t net_key_index2,
                                       uint16_t addr1, uint16_t addr2);

/**
 * @brief get bridget subnet network key index
 * @param[in] src: message source address
 * @param[in] dst: message destination address
 * @param[in] net_key_index: network key index used in message
 * @return bridged subnet network key index
 */
uint16_t subnet_bridge_get_bridged_net_key_index(uint16_t src, uint16_t dst,
                                                 uint16_t net_key_index);

/**
 * @brief get subnet bridge table by index
 * @param[in] index: bridging table index
 * @return bridging table pointer
 */
bridging_table_t *subnet_bridge_table_get_by_index(uint16_t index);

/**
 * @brief add subnet bridge table
 * @param[in] directions: subnet bridge directions
 * @param[in] net_key_index1_g: global network key index in subnet bridge
 * @param[in] net_key_index2_g: global network key index in subnet bridge
 * @param[in] addr1: address in subnet bridge
 * @param[in] addr2: address in subnet bridge
 * @return bridging table index
 */
uint16_t subnet_bridge_table_add(uint8_t directions, uint16_t net_key_index1_g,
                                 uint16_t net_key_index2_g, uint16_t addr1, uint16_t addr2);

/**
 * @brief set bridging table by index
 * @param[in] index: bridging table index
 * @param[in] directions: subnet bridge directions
 * @param[in] net_key_index1_g: global network key index in subnet bridge
 * @param[in] net_key_index2_g: global network key index in subnet bridge
 * @param[in] addr1: address in subnet bridge
 * @param[in] addr2: address in subnet bridge
 * @return set result
 */
bool subnet_bridge_table_set(uint16_t index, uint8_t directions, uint16_t net_key_index1_g,
                             uint16_t net_key_index2_g, uint16_t addr1, uint16_t addr2);

/**
 * @brief remove bridging table
 * @param[in] net_key_index1: network key index in bridging table
 * @param[in] net_key_index2: network key index in bridging table
 * @param[in] addr1: address in bridging table
 * @param[in] addr2: address in bridging table
 * @return removed bridging table index
 */
uint16_t subnet_bridge_table_remove(uint16_t net_key_index1, uint16_t net_key_index2,
                                    uint16_t addr1, uint16_t addr2);

/**
 * @brief remove bridging table by index
 * @param[in] index: bridging table index
 */
void subnet_bridge_table_remove_by_index(uint16_t index);

/**
 * @brief calculate subnets count
 * @param[in] filter: calculate filter
 * @param[in] net_key_index: network key index in bridging table
 * @param[in] start_index: start index in bridging table
 * @return subnets count
 */
uint16_t subnet_bridge_subnets_count(subnets_filter_t filter, uint16_t net_key_index,
                                     uint8_t start_index);

/**
 * @brief list subnets
 * @param[in] filter: list filter
 * @param[in] net_key_index: network key index in bridging table
 * @param[in] start_index: start index in bridging table
 * @param[out] ptables: all subnets
 * @return subnets count
 */
uint16_t subnet_bridge_subnets_list(subnets_filter_t filter, uint16_t net_key_index,
                                    uint8_t start_index, bridging_table_t *ptables);

/**
 * @brief calculate bridging table count
 * @param[in] net_key_index1: network key index in bridging table
 * @param[in] net_key_index2: network key index in bridging table
 * @param[in] start_index: start index in bridging table
 * @return bridging table count
 */
uint16_t subnet_bridge_table_count(uint16_t net_key_index1, uint16_t net_key_index2,
                                   uint16_t start_index);

/**
 * @brief list bridging table
 * @param[in] net_key_index1: network key index in bridging table
 * @param[in] net_key_index2: network key index in bridging table
 * @param[in] start_index: start index in bridging table
 * @param[out] ptables: all bridging tables
 * @return bridging table count
 */
uint16_t subnet_bridge_table_list(uint16_t net_key_index1, uint16_t net_key_index2,
                                  uint16_t start_index, bridging_table_t *ptables);

/**
 * @brief clear subnet bridge table in RAM
 */
void subnet_bridge_table_clean(void);

/**
 * @brief clear subnet bridge table in RAM and NVM
 */
void subnet_bridge_table_clear(void);

/**
 * @brief delete network key
 * @param[in] net_key_index: network key index
 * @return deleted index
 */
uint16_t subnet_bridge_net_key_delete(uint16_t net_key_index);

/** @} */
/** @} */
#endif

END_DECLS

#endif /* _SUBNET_BRIDGE_H_ */
