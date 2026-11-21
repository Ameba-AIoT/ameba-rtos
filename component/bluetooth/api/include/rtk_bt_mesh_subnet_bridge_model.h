/**
 * @file      rtk_bt_mesh_subnet_bridge_model.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH subnet bridge model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_SUBNET_BRIDGE_MODEL_H__
#define __RTK_BT_MESH_SUBNET_BRIDGE_MODEL_H__

#include <rtk_bt_mesh_def.h>

/************************************** Data Types *************************************/

/**
 * @enum      rtk_bt_mesh_subnet_bridge_client_model_act_t
 * @brief     BLE MESH subnet bridge client model act definition.
 */
typedef enum {
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_GET = 1,                    /*!< Send a Subnet Bridge Get message to the server. */
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_SET,                        /*!< Send a Subnet Bridge Set message to the server. */
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_ADD,         /*!< Send a Bridging Table Add message to the server. */
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_REMOVE,      /*!< Send a Bridging Table Remove message to the server. */
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGED_SUBNETS_GET,        /*!< Send a Bridged Subnets Get message to the server. */
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_GET,         /*!< Send a Bridging Table Get message to the server. */
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_SIZE_GET,    /*!< Send a Bridging Table Size Get message to the server. */
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_ACT_MAX,                    /*!< Reserved. */
} rtk_bt_mesh_subnet_bridge_client_model_act_t;

/**
 * @struct    rtk_bt_mesh_subnet_bridge_get_t
 * @brief     BLE MESH subnet bridge get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Subnet Bridge Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_subnet_bridge_get_t;

/**
 * @enum      rtk_bt_mesh_subnet_bridge_state_t
 * @brief     BLE MESH subnet bridge state definition.
 */
enum {
	RTK_BT_MESH_SUBNET_BRIDGE_STATE_DISABLE,    /*!< Subnet bridge functionality is disabled. */
	RTK_BT_MESH_SUBNET_BRIDGE_STATE_ENABLE,     /*!< Subnet bridge functionality is enabled. */
} _SHORT_ENUM_;
typedef uint8_t rtk_bt_mesh_subnet_bridge_state_t;

/**
 * @struct    rtk_bt_mesh_subnet_bridge_set_t
 * @brief     BLE MESH subnet bridge set act message structure.
 */
typedef struct {
	uint16_t dst;                               /*!< Unicast address of the Subnet Bridge Server. */
	uint16_t net_key_index;                     /*!< Index of the NetKey of the subnet the message is sent through.
                                                 *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_subnet_bridge_state_t state;    /*!< Subnet Bridge state to set. */
} rtk_bt_mesh_subnet_bridge_set_t;

/**
 * @struct    rtk_bt_mesh_bridging_table_add_t
 * @brief     BLE MESH subnet bridge table add act message structure.
 */
typedef struct {
	uint16_t dst;               /*!< Unicast address of the Subnet Bridge Server. */
	uint16_t net_key_index;     /*!< Index of the NetKey of the subnet the message is sent through.
                                 *   Range: 0x0000 to 0x0FFF. */
	uint8_t directions;         /*!< Allowed directions for the bridged traffic.
                                 *   0x01: Bridging is allowed only for messages with addr1 as the source address and addr2 as the destination address.
                                 *   0x02: Bridging is allowed for messages with addr1 as the source address and addr2 as the destination address,
                                 *         and for messages with addr2 as the source address and addr1 as the destination address.
                                 *   0x00, 0x03 to 0xFF: Prohibited. */
	uint16_t net_key_index1;    /*!< Index of the NetKey of the first subnet.
                                 *   Range: 0x0000 to 0x0FFF.
                                 *   It shall be different from net_key_index2. */
	uint16_t net_key_index2;    /*!< Index of the NetKey of the second subnet.
                                 *   Range: 0x0000 to 0x0FFF. */
	uint16_t addr1;             /*!< Address of the node in the first subnet.
                                 *   It shall be a unicast address and different from addr2. */
	uint16_t addr2;             /*!< Address of the node in the second subnet.
                                 *   If directions is 0x01, the unassigned address and the all-nodes address are Prohibited.
                                 *   If directions is 0x02, it shall be a unicast address. */
} rtk_bt_mesh_bridging_table_add_t;

/**
 * @struct    rtk_bt_mesh_bridging_table_remove_t
 * @brief     BLE MESH subnet bridge table remove act message structure.
 */
typedef struct {
	uint16_t dst;               /*!< Unicast address of the Subnet Bridge Server. */
	uint16_t net_key_index;     /*!< Index of the NetKey of the subnet the message is sent through.
                                 *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index1;    /*!< Index of the NetKey of the first subnet.
                                 *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index2;    /*!< Index of the NetKey of the second subnet.
                                 *   Range: 0x0000 to 0x0FFF. */
	uint16_t addr1;             /*!< Address of the node in the first subnet.
                                 *   The unassigned address or a unicast address. */
	uint16_t addr2;             /*!< Address of the node in the second subnet.
                                 *   The all-nodes address is Prohibited. */
} rtk_bt_mesh_bridging_table_remove_t;

/**
 * @enum      rtk_bt_mesh_subnets_filter_t
 * @brief     BLE MESH bridged subnets filter definition.
 */
enum {
	RTK_BT_MESH_SUBNETS_FILTER_ALL_PAIRS,               /*!< 0b00: Report all pairs of NetKey Indexes extracted from the Bridging Table entries. */
	RTK_BT_MESH_SUBNETS_FILTER_NET_KEY_INDEX1,          /*!< 0b01: Report the pairs in which the NetKey Index of the first subnet matches the filter NetKey Index. */
	RTK_BT_MESH_SUBNETS_FILTER_NET_KEY_INDEX2,          /*!< 0b10: Report the pairs in which the NetKey Index of the second subnet matches the filter NetKey Index. */
	RTK_BT_MESH_SUBNETS_FILTER_NET_KEY_INDEX_EITHER,    /*!< 0b11: Report the pairs in which either NetKey Index matches the filter NetKey Index. */
} _SHORT_ENUM_;
typedef uint8_t rtk_bt_mesh_subnets_filter_t;

/**
 * @struct    rtk_bt_mesh_bridged_subnets_get_t
 * @brief     BLE MESH bridged subnets get act message structure.
 */
typedef struct {
	uint16_t dst;                           /*!< Unicast address of the Subnet Bridge Server. */
	uint16_t net_key_index;                 /*!< Index of the NetKey of the subnet the message is sent through.
                                             *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_subnets_filter_t filter;    /*!< Filter applied when reporting the pairs of NetKey Indexes. */
	uint16_t filter_net_key_index;          /*!< Index of the NetKey used for filtering.
                                             *   Range: 0x0000 to 0x0FFF.
                                             *   Not used when filter is the all pairs filter. */
	uint8_t start_index;                    /*!< Start offset in units of pairs of NetKey Indexes to read. */
} rtk_bt_mesh_bridged_subnets_get_t;

/**
 * @struct    rtk_bt_mesh_bridging_table_get_t
 * @brief     BLE MESH bridging table get act message structure.
 */
typedef struct {
	uint16_t dst;               /*!< Unicast address of the Subnet Bridge Server. */
	uint16_t net_key_index;     /*!< Index of the NetKey of the subnet the message is sent through.
                                 *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index1;    /*!< Index of the NetKey of the first subnet.
                                 *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index2;    /*!< Index of the NetKey of the second subnet.
                                 *   Range: 0x0000 to 0x0FFF. */
	uint16_t start_index;       /*!< Start offset in units of Bridging Table entries to read. */
} rtk_bt_mesh_bridging_table_get_t;

/**
 * @struct    rtk_bt_mesh_bridging_table_size_get_t
 * @brief     BLE MESH bridging table size get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Subnet Bridge Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_bridging_table_size_get_t;

/**
 * @enum      rtk_bt_mesh_subnet_bridge_client_model_evt_t
 * @brief     BLE MESH subnet bridge client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_STATUS = 1,               /*!< Indicate a Subnet Bridge Status message received from the server, with msg
                                                                      *   @ref rtk_bt_mesh_subnet_bridge_status_t. */
	RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_STATUS,    /*!< Indicate a Bridging Table Status message received from the server, with msg
                                                                      *   @ref rtk_bt_mesh_bridging_table_status_t. */
	RTK_BT_MESH_SUBNET_BRIDGE_BRIDGED_SUBNETS_CLIENT_MODEL_LIST,     /*!< Indicate a Bridged Subnets List message received from the server, with msg
                                                                      *   @ref rtk_bt_mesh_bridged_subnets_list_t. */
	RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_LIST,      /*!< Indicate a Bridging Table List message received from the server, with msg
                                                                      *   @ref rtk_bt_mesh_bridging_table_list_t. */
	RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_SIZE,      /*!< Indicate a Bridging Table Size Status message received from the server, with msg
                                                                      *   @ref rtk_bt_mesh_bridging_table_size_status_t. */
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_EVT_MAX,                  /*!< Reserved. */
} rtk_bt_mesh_subnet_bridge_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_subnet_bridge_status_t
 * @brief     BLE MESH subnet bridge client model status message structure.
 */
typedef struct {
	rtk_bt_mesh_subnet_bridge_state_t state;    /*!< Current Subnet Bridge state of the server. */
} _PACKED4_ rtk_bt_mesh_subnet_bridge_status_t;

/**
 * @struct    rtk_bt_mesh_bridging_table_status_t
 * @brief     BLE MESH subnet bridge client model bridging table status message structure.
 */
typedef struct {
	uint8_t status;                  /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint32_t directions : 8;         /*!< Current allowed directions for the bridged traffic.
                                      *   0x00: Bridging is not allowed for messages between addr1 and addr2.
                                      *   0x01: Bridging is allowed only for messages with addr1 as the source address and addr2 as the destination address.
                                      *   0x02: Bridging is allowed for messages with addr1 as the source address and addr2 as the destination address,
                                      *         and for messages with addr2 as the source address and addr1 as the destination address.
                                      *   0x03 to 0xFF: Prohibited. */
	uint32_t net_key_index1 : 12;    /*!< Index of the NetKey of the first subnet.
                                      *   Range: 0x0000 to 0x0FFF. */
	uint32_t net_key_index2 : 12;    /*!< Index of the NetKey of the second subnet.
                                      *   Range: 0x0000 to 0x0FFF. */
	uint16_t addr1;                  /*!< Address of the node in the first subnet.
                                      *   The unassigned address or a unicast address. */
	uint16_t addr2;                  /*!< Address of the node in the second subnet.
                                      *   The all-nodes address is Prohibited. */
} _PACKED4_ rtk_bt_mesh_bridging_table_status_t;

/**
 * @struct    rtk_bt_mesh_bridged_subnets_list_t
 * @brief     BLE MESH subnet bridge client model bridged subnets list message structure.
 */
typedef struct {
	uint16_t filter : 2;                /*!< Filter applied to the set of pairs of NetKey Indexes.
                                         *   0b00: All pairs of NetKey Indexes are reported.
                                         *   0b01: Only the pairs whose first NetKey Index matches net_key_index are reported.
                                         *   0b10: Only the pairs whose second NetKey Index matches net_key_index are reported.
                                         *   0b11: Only the pairs in which either NetKey Index matches net_key_index are reported. */
	uint16_t prohibited : 2;            /*!< Prohibited bits of the message format. */
	uint16_t net_key_index : 12;        /*!< Index of the NetKey used for filtering, ignored when filter is 0b00.
                                         *   Range: 0x0000 to 0x0FFF. */
	uint8_t start_index;                /*!< Start offset in units of pairs of NetKey Indexes of the reported set. */
	uint8_t bridged_subnets_list[0];    /*!< Filtered set of pairs of NetKey Indexes, it may be empty.
                                         *   This member only marks where the data starts,
                                         *   the pairs are located right after the structure in the event data,
                                         *   their total length in bytes is the length of the event data minus the size of the structure.
                                         *   Each pair takes 3 bytes, little-endian: bits 0 to 11 are the NetKey Index of the first subnet,
                                         *   bits 12 to 23 are the NetKey Index of the second subnet. */
} _PACKED4_ rtk_bt_mesh_bridged_subnets_list_t;

/**
 * @struct    rtk_bt_mesh_bridged_address_entry_t
 * @brief     BLE MESH subnet bridge client model bridged address entry structure.
 */
typedef struct {
	uint16_t addr1;        /*!< Address of the node in the first subnet. */
	uint16_t addr2;        /*!< Address of the node in the second subnet. */
	uint8_t directions;    /*!< Allowed directions for the bridged traffic.
                            *   0x01: Bridging is allowed only for messages with addr1 as the source address and addr2 as the destination address.
                            *   0x02: Bridging is allowed for messages with addr1 as the source address and addr2 as the destination address,
                            *         and for messages with addr2 as the source address and addr1 as the destination address.
                            *   0x00, 0x03 to 0xFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_bridged_address_entry_t;

/**
 * @struct    rtk_bt_mesh_bridging_table_list_t
 * @brief     BLE MESH subnet bridge client model bridging table list message structure.
 */
typedef struct {
	uint32_t status : 8;              /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint32_t net_key_index1 : 12;     /*!< Index of the NetKey of the first subnet.
                                       *   Range: 0x0000 to 0x0FFF. */
	uint32_t net_key_index2 : 12;     /*!< Index of the NetKey of the second subnet.
                                       *   Range: 0x0000 to 0x0FFF. */
	uint16_t start_index;             /*!< Start offset in units of Bridging Table entries of the reported list. */
	uint8_t bridged_addrs_list[0];    /*!< List of bridged addresses and allowed traffic directions, it may be empty.
                                       *   It is only present when status is success.
                                       *   This member only marks where the data starts,
                                       *   the entries are located right after the structure in the event data,
                                       *   their total length in bytes is the length of the event data minus the size of the structure.
                                       *   Each entry is formatted as @ref rtk_bt_mesh_bridged_address_entry_t. */
} _PACKED4_ rtk_bt_mesh_bridging_table_list_t;

/**
 * @struct    rtk_bt_mesh_bridging_table_size_status_t
 * @brief     BLE MESH subnet bridge client model bridging table size status message structure.
 */
typedef struct {
	uint16_t bridging_table_size;    /*!< Bridging Table Size state, the maximum number of Bridging Table entries the server supports.
                                      *   It is at least 16. */
} _PACKED4_ rtk_bt_mesh_bridging_table_size_status_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  ble_mesh_subnet_bridge_model BT LE Mesh Subnet Bridge Model APIs
 * @brief     BT LE Mesh Subnet Bridge Model related function APIs
 * @ingroup   ble_mesh_api_group
 * @{
 */

/**
 * @brief     Send a Subnet Bridge Get message to a Subnet Bridge Server to read its Subnet Bridge state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_STATUS.
 * @param[in] subnet_bridge_get: Subnet Bridge Get message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_subnet_bridge_get(rtk_bt_mesh_subnet_bridge_get_t *subnet_bridge_get);

/**
 * @brief     Send a Subnet Bridge Set message to a Subnet Bridge Server to set its Subnet Bridge state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_STATUS.
 * @param[in] subnet_bridge_set: Subnet Bridge Set message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_subnet_bridge_set(rtk_bt_mesh_subnet_bridge_set_t *subnet_bridge_set);

/**
 * @brief     Send a Bridging Table Add message to a Subnet Bridge Server to add an entry to its Bridging Table.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_STATUS.
 * @param[in] bridging_table_add: Bridging Table Add message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_bridging_table_add(rtk_bt_mesh_bridging_table_add_t *bridging_table_add);

/**
 * @brief     Send a Bridging Table Remove message to a Subnet Bridge Server to remove entries from its Bridging Table.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_STATUS.
 * @param[in] bridging_table_remove: Bridging Table Remove message parameters.
 *                                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_bridging_table_remove(rtk_bt_mesh_bridging_table_remove_t *bridging_table_remove);

/**
 * @brief     Send a Bridged Subnets Get message to a Subnet Bridge Server to read the filtered pairs of NetKey Indexes of its Bridging Table.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SUBNET_BRIDGE_BRIDGED_SUBNETS_CLIENT_MODEL_LIST.
 * @param[in] bridged_subnets_get: Bridged Subnets Get message parameters.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_bridged_subnets_get(rtk_bt_mesh_bridged_subnets_get_t *bridged_subnets_get);

/**
 * @brief     Send a Bridging Table Get message to a Subnet Bridge Server to read the addresses and allowed directions of its Bridging Table entries.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_LIST.
 * @param[in] bridging_table_get: Bridging Table Get message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_bridging_table_get(rtk_bt_mesh_bridging_table_get_t *bridging_table_get);

/**
 * @brief     Send a Bridging Table Size Get message to a Subnet Bridge Server to read its Bridging Table Size state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_SIZE.
 * @param[in] bridging_table_size_get: Bridging Table Size Get message parameters.
 *                                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_bridging_table_size_get(rtk_bt_mesh_bridging_table_size_get_t *bridging_table_size_get);

/**
 * @}
 */


#endif /* __RTK_BT_MESH_SUBNET_BRIDGE_MODEL_H__ */
