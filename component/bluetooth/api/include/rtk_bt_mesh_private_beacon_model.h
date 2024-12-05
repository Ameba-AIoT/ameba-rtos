/**
 * @file      rtk_bt_mesh_private_beacon_model.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH private beacon model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */


#ifndef __RTK_BT_MESH_PRIVATE_BEACON_MODEL_H__
#define __RTK_BT_MESH_PRIVATE_BEACON_MODEL_H__

/* ------------------------------- Data Types ------------------------------- */

/**
 * @typedef   rtk_bt_mesh_private_beacon_client_model_act_t
 * @brief     BLE MESH private beacon client model act definition.
 */
typedef enum {
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GET = 1,
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_SET,
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GATT_PROXY_GET,
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GATT_PROXY_SET,
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_NODE_IDENTITY_GET,
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_NODE_IDENTITY_SET,
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_ACT_MAX,
} rtk_bt_mesh_private_beacon_client_model_act_t;

/**
 * @typedef   rtk_bt_mesh_private_beacon_get_t
 * @brief     BLE MESH private beacon get act message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
} rtk_bt_mesh_private_beacon_get_t;

/**
 * @typedef   rtk_bt_mesh_private_beacon_set_t
 * @brief     BLE MESH private beacon set act message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint8_t private_beacon;
	bool has_random_update;
	uint8_t random_update_interval_steps;
} rtk_bt_mesh_private_beacon_set_t;

/**
 * @typedef   rtk_bt_mesh_private_gatt_proxy_get_t
 * @brief     BLE MESH private gatt proxy get act message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
} rtk_bt_mesh_private_gatt_proxy_get_t;

/**
 * @typedef   rtk_bt_mesh_private_gatt_proxy_set_t
 * @brief     BLE MESH private gatt proxy set act message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint8_t private_gatt_proxy;
} rtk_bt_mesh_private_gatt_proxy_set_t;

/**
 * @typedef   rtk_bt_mesh_private_node_identity_get_t
 * @brief     BLE MESH private node identity get act message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t sub_net_key_index;
} rtk_bt_mesh_private_node_identity_get_t;

/**
 * @typedef   rtk_bt_mesh_private_node_identity_set_t
 * @brief     BLE MESH private node identity set act message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t sub_net_key_index;
	uint8_t private_identity;
} rtk_bt_mesh_private_node_identity_set_t;

/**
 * @typedef   rtk_bt_mesh_datatrans_model_evt_t
 * @brief     BLE MESH datatrans model evt definition.
 */
typedef enum {
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_STATUS = 1,
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_GATT_PROXY_STATUS,
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_NODE_IDENTITY_STATUS,
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_MAX,
} rtk_bt_mesh_private_beacon_client_model_evt_t;

/**
 * @typedef   rtk_bt_mesh_private_beacon_status_t
 * @brief     BLE MESH private beacon client model status message structure.
 */
typedef struct {
	uint8_t private_beacon;
	uint8_t random_update_interval_steps;
} _PACKED4_ rtk_bt_mesh_private_beacon_status_t;

/**
 * @typedef   rtk_bt_mesh_private_gatt_proxy_status_t
 * @brief     BLE MESH private beacon client model gatt proxy status message structure.
 */
typedef struct {
	uint8_t private_gatt_proxy;
} _PACKED4_ rtk_bt_mesh_private_gatt_proxy_status_t;

/**
 * @typedef   rtk_bt_mesh_private_node_identity_status_t
 * @brief     BLE MESH private beacon client model node identity status message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index;
	uint8_t private_identity;
} _PACKED4_ rtk_bt_mesh_private_node_identity_status_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  ble_mesh_private_beacon_model BT LE Mesh Private Beacon Model APIs
 * @brief     BT LE Mesh Private Beacon Model related function APIs
 * @ingroup   ble_mesh_api_group
 * @{
 */

/**
 * @brief     Send private beacon get message to private beacon server, will cause event @ref RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GET
 * @param[in] private_beacon_get: private beacon get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_private_beacon_get(rtk_bt_mesh_private_beacon_get_t *private_beacon_get);

/**
 * @brief     Send private beacon set message to private beacon server, will cause event @ref RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_SET
 * @param[in] private_beacon_set: private beacon set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_private_beacon_set(rtk_bt_mesh_private_beacon_set_t *private_beacon_set);

/**
 * @brief     Send private gatt proxy get message to private beacon server, will cause event @ref RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GATT_PROXY_GET
 * @param[in] private_gatt_proxy_get: private gatt proxy get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_private_gatt_proxy_get(rtk_bt_mesh_private_gatt_proxy_get_t *private_gatt_proxy_get);

/**
 * @brief     Send private gatt proxy set message to private beacon server, will cause event @ref RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GATT_PROXY_SET
 * @param[in] private_gatt_proxy_set: private gatt proxy set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_private_gatt_proxy_set(rtk_bt_mesh_private_gatt_proxy_set_t *private_gatt_proxy_set);

/**
 * @brief     Send private node identity get message to private beacon server, will cause event @ref RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_NODE_IDENTITY_GET
 * @param[in] private_node_identity_get: private node identity get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_private_node_identity_get(rtk_bt_mesh_private_node_identity_get_t *private_node_identity_get);

/**
 * @brief     Send private node identity set message to private beacon server, will cause event @ref RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_NODE_IDENTITY_SET
 * @param[in] private_node_identity_set: private node identity set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_private_node_identity_set(rtk_bt_mesh_private_node_identity_set_t *private_node_identity_set);

/**
 * @}
 */
#endif