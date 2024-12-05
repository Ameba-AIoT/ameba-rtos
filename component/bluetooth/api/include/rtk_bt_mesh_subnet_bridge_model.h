/**
 * @file      rtk_bt_mesh_subnet_bridge_model.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH subnet bridge model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */


#ifndef __RTK_BT_MESH_SUBNET_BRIDGE_MODEL_H__
#define __RTK_BT_MESH_SUBNET_BRIDGE_MODEL_H__

#include <rtk_bt_mesh_def.h>

/* ------------------------------- Data Types ------------------------------- */

/**
 * @typedef   rtk_bt_mesh_subnet_bridge_client_model_act_t
 * @brief     BLE MESH subnet bridge client model act definition.
 */
typedef enum {
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_GET = 1,
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_SET,
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_ADD,
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_REMOVE,
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGED_SUBNETS_GET,
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_GET,
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_SIZE_GET,
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_ACT_MAX,
} rtk_bt_mesh_subnet_bridge_client_model_act_t;

/**
 * @typedef   rtk_bt_mesh_sensor_descriptor_get_t
 * @brief     BLE MESH subnet bridge get act message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
} rtk_bt_mesh_subnet_bridge_get_t;

enum {
	RTK_BT_MESH_SUBNET_BRIDGE_STATE_DISABLE,
	RTK_BT_MESH_SUBNET_BRIDGE_STATE_ENABLE,
} _SHORT_ENUM_;
typedef uint8_t rtk_bt_mesh_subnet_bridge_state_t;

/**
 * @typedef   rtk_bt_mesh_sensor_descriptor_set_t
 * @brief     BLE MESH subnet bridge set act message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	rtk_bt_mesh_subnet_bridge_state_t state;
} rtk_bt_mesh_subnet_bridge_set_t;

/**
 * @typedef   rtk_bt_mesh_bridging_table_add_t
 * @brief     BLE MESH subnet bridge table add act message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint8_t directions;
	uint16_t net_key_index1;
	uint16_t net_key_index2;
	uint16_t addr1;
	uint16_t addr2;
} rtk_bt_mesh_bridging_table_add_t;

/**
 * @typedef   rtk_bt_mesh_bridging_table_remove_t
 * @brief     BLE MESH subnet bridge table remove act message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index1;
	uint16_t net_key_index2;
	uint16_t addr1;
	uint16_t addr2;
} rtk_bt_mesh_bridging_table_remove_t;

enum {
	RTK_BT_MESH_SUBNETS_FILTER_ALL_PAIRS,
	RTK_BT_MESH_SUBNETS_FILTER_NET_KEY_INDEX1,
	RTK_BT_MESH_SUBNETS_FILTER_NET_KEY_INDEX2,
	RTK_BT_MESH_SUBNETS_FILTER_NET_KEY_INDEX_EITHER,
} _SHORT_ENUM_;
typedef uint8_t rtk_bt_mesh_subnets_filter_t;

/**
 * @typedef   rtk_bt_mesh_bridged_subnets_get_t
 * @brief     BLE MESH bridged subnets get act message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	rtk_bt_mesh_subnets_filter_t filter;
	uint16_t filter_net_key_index;
	uint8_t start_index;
} rtk_bt_mesh_bridged_subnets_get_t;

/**
 * @typedef   rtk_bt_mesh_bridging_table_get_t
 * @brief     BLE MESH bridging table get act message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index1;
	uint16_t net_key_index2;
	uint16_t start_index;
} rtk_bt_mesh_bridging_table_get_t;

/**
 * @typedef   rtk_bt_mesh_bridging_table_size_get_t
 * @brief     BLE MESH bridging table size get act message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
} rtk_bt_mesh_bridging_table_size_get_t;

/**
 * @typedef   rtk_bt_mesh_subnet_bridge_client_model_evt_t
 * @brief     BLE MESH subnet bridge client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_STATUS = 1,
	RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_STATUS,
	RTK_BT_MESH_SUBNET_BRIDGE_BRIDGED_SUBNETS_CLIENT_MODEL_LIST,
	RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_LIST,
	RTK_BT_MESH_SUBNET_BRIDGE_BRIDGING_TABLE_CLIENT_MODEL_SIZE,
	RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_EVT_MAX,
} rtk_bt_mesh_subnet_bridge_client_model_evt_t;

/**
 * @typedef   rtk_bt_mesh_subnet_bridge_status_t
 * @brief     BLE MESH subnet bridge client model status message structure.
 */
typedef struct {
	rtk_bt_mesh_subnet_bridge_state_t state;
} _PACKED4_ rtk_bt_mesh_subnet_bridge_status_t;

/**
 * @typedef   rtk_bt_mesh_subnet_bridge_status_t
 * @brief     BLE MESH subnet bridge client model bridging table status message structure.
 */
typedef struct {
	uint8_t status;
	uint32_t directions : 8;
	uint32_t net_key_index1 : 12;
	uint32_t net_key_index2 : 12;
	uint16_t addr1;
	uint16_t addr2;
} _PACKED4_ rtk_bt_mesh_bridging_table_status_t;

/**
 * @typedef   rtk_bt_mesh_subnet_bridge_status_t
 * @brief     BLE MESH subnet bridge client model bridged subnets list message structure.
 */
typedef struct {
	uint16_t filter : 2;
	uint16_t prohibited : 2;
	uint16_t net_key_index : 12;
	uint8_t start_index;
	uint8_t bridged_subnets_list[0];
} _PACKED4_ rtk_bt_mesh_bridged_subnets_list_t;

typedef struct {
	uint16_t addr1;
	uint16_t addr2;
	uint8_t directions;
} _PACKED4_ rtk_bt_mesh_bridged_address_entry_t;

/**
 * @typedef   rtk_bt_mesh_subnet_bridge_status_t
 * @brief     BLE MESH subnet bridge client model bridging table list message structure.
 */
typedef struct {
	uint32_t status : 8;
	uint32_t net_key_index1 : 12;
	uint32_t net_key_index2 : 12;
	uint16_t start_index;
	uint8_t bridged_addrs_list[0];
} _PACKED4_ rtk_bt_mesh_bridging_table_list_t;

/**
 * @typedef   rtk_bt_mesh_subnet_bridge_status_t
 * @brief     BLE MESH subnet bridge client model bridging table size status message structure.
 */
typedef struct {
	uint16_t bridging_table_size;
} _PACKED4_ rtk_bt_mesh_bridging_table_size_status_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  ble_mesh_subnet_bridge_model BT LE Mesh Subnet Bridge Model APIs
 * @brief     BT LE Mesh Subnet Bridge Model related function APIs
 * @ingroup   ble_mesh_api_group
 * @{
 */

/**
 * @brief     Send subnet bridge get message to subnet bridge server, will cause event @ref RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_GET
 * @param[in] subnet_bridge_get: subnet bridge get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_subnet_bridge_get(rtk_bt_mesh_subnet_bridge_get_t *subnet_bridge_get);

/**
 * @brief     Send subnet bridge set message to subnet bridge server, will cause event @ref RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_SET
 * @param[in] subnet_bridge_set: subnet bridge set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_subnet_bridge_set(rtk_bt_mesh_subnet_bridge_set_t *subnet_bridge_set);

/**
 * @brief     Send bridging table add message to subnet bridge server, will cause event @ref RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_ADD
 * @param[in] bridging_table_add: bridging table add message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_bridging_table_add(rtk_bt_mesh_bridging_table_add_t *bridging_table_add);

/**
 * @brief     Send bridging table remove message to subnet bridge server, will cause event @ref RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_REMOVE
 * @param[in] bridging_table_remove: bridging table remove message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_bridging_table_remove(rtk_bt_mesh_bridging_table_remove_t *bridging_table_remove);

/**
 * @brief     Send bridged subnets get message to subnet bridge server, will cause event @ref RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGED_SUBNETS_GET
 * @param[in] bridged_subnets_get: bridged subnets get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_bridged_subnets_get(rtk_bt_mesh_bridged_subnets_get_t *bridged_subnets_get);

/**
 * @brief     Send bridging table get message to subnet bridge server, will cause event @ref RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_GET
 * @param[in] bridging_table_get: bridging table get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_bridging_table_get(rtk_bt_mesh_bridging_table_get_t *bridging_table_get);

/**
 * @brief     Send bridging table size get message to subnet bridge server, will cause event @ref RTK_BT_MESH_SUBNET_BRIDGE_CLIENT_MODEL_BRIDGING_TABLE_SIZE_GET
 * @param[in] bridging_table_size_get: bridging table size get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_bridging_table_size_get(rtk_bt_mesh_bridging_table_size_get_t *bridging_table_size_get);

/**
 * @}
 */


#endif