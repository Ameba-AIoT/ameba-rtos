/**
 * @file      rtk_bt_mesh_private_beacon_model.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH private beacon model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_PRIVATE_BEACON_MODEL_H__
#define __RTK_BT_MESH_PRIVATE_BEACON_MODEL_H__

/************************************** Data Types *************************************/

/**
 * @enum      rtk_bt_mesh_private_beacon_client_model_act_t
 * @brief     BLE MESH private beacon client model act definition.
 */
typedef enum {
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GET = 1,              /*!< Send a Private Beacon Get message to the server. */
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_SET,                  /*!< Send a Private Beacon Set message to the server. */
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GATT_PROXY_GET,       /*!< Send a Private GATT Proxy Get message to the server. */
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_GATT_PROXY_SET,       /*!< Send a Private GATT Proxy Set message to the server. */
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_NODE_IDENTITY_GET,    /*!< Send a Private Node Identity Get message to the server. */
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_NODE_IDENTITY_SET,    /*!< Send a Private Node Identity Set message to the server. */
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_ACT_MAX,              /*!< Reserved. */
} rtk_bt_mesh_private_beacon_client_model_act_t;

/**
 * @struct    rtk_bt_mesh_private_beacon_get_t
 * @brief     BLE MESH private beacon get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Private Beacon Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_private_beacon_get_t;

/**
 * @struct    rtk_bt_mesh_private_beacon_set_t
 * @brief     BLE MESH private beacon set act message structure.
 */
typedef struct {
	uint16_t dst;                            /*!< Unicast address of the Private Beacon Server. */
	uint16_t net_key_index;                  /*!< Index of the NetKey of the subnet the message is sent through.
                                              *   Range: 0x0000 to 0x0FFF. */
	uint8_t private_beacon;                  /*!< New Private Beacon state.
                                              *   0x00: The node does not broadcast Mesh Private beacons.
                                              *   0x01: The node broadcasts Mesh Private beacons.
                                              *   0x02 to 0xFF: Prohibited. */
	bool has_random_update;                  /*!< Whether the Random Update Interval Steps field is carried in the message.
                                              *   - true: random_update_interval_steps is carried.
                                              *   - false: random_update_interval_steps is not carried. */
	uint8_t random_update_interval_steps;    /*!< New Random Update Interval Steps state, in units of 10 seconds.
                                              *   0x00: The Random field is updated for every Mesh Private beacon.
                                              *   0x01 to 0xFF: The Random field is updated at an interval of 10 * random_update_interval_steps seconds.
                                              *   Only used when has_random_update is true. */
} rtk_bt_mesh_private_beacon_set_t;

/**
 * @struct    rtk_bt_mesh_private_gatt_proxy_get_t
 * @brief     BLE MESH private gatt proxy get act message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Private Beacon Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_private_gatt_proxy_get_t;

/**
 * @struct    rtk_bt_mesh_private_gatt_proxy_set_t
 * @brief     BLE MESH private gatt proxy set act message structure.
 */
typedef struct {
	uint16_t dst;                  /*!< Unicast address of the Private Beacon Server. */
	uint16_t net_key_index;        /*!< Index of the NetKey of the subnet the message is sent through.
                                    *   Range: 0x0000 to 0x0FFF. */
	uint8_t private_gatt_proxy;    /*!< New Private GATT Proxy state.
                                    *   0x00: The Private Proxy functionality is disabled.
                                    *   0x01: The Private Proxy functionality is enabled.
                                    *   0x02 to 0xFF: Prohibited. */
} rtk_bt_mesh_private_gatt_proxy_set_t;

/**
 * @struct    rtk_bt_mesh_private_node_identity_get_t
 * @brief     BLE MESH private node identity get act message structure.
 */
typedef struct {
	uint16_t dst;                  /*!< Unicast address of the Private Beacon Server. */
	uint16_t net_key_index;        /*!< Index of the NetKey of the subnet the message is sent through.
                                    *   Range: 0x0000 to 0x0FFF. */
	uint16_t sub_net_key_index;    /*!< Index of the network key of the subnet whose Private Node Identity state is requested.
                                    *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_private_node_identity_get_t;

/**
 * @struct    rtk_bt_mesh_private_node_identity_set_t
 * @brief     BLE MESH private node identity set act message structure.
 */
typedef struct {
	uint16_t dst;                  /*!< Unicast address of the Private Beacon Server. */
	uint16_t net_key_index;        /*!< Index of the NetKey of the subnet the message is sent through.
                                    *   Range: 0x0000 to 0x0FFF. */
	uint16_t sub_net_key_index;    /*!< Index of the network key of the subnet whose Private Node Identity state is set.
                                    *   Range: 0x0000 to 0x0FFF. */
	uint8_t private_identity;      /*!< New Private Node Identity state.
                                    *   0x00: Advertising with Private Node Identity for the subnet is stopped.
                                    *   0x01: Advertising with Private Node Identity for the subnet is running.
                                    *   0x02 to 0xFF: Prohibited. */
} rtk_bt_mesh_private_node_identity_set_t;

/**
 * @enum      rtk_bt_mesh_private_beacon_client_model_evt_t
 * @brief     BLE MESH private beacon client model evt definition.
 */
typedef enum {
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_STATUS = 1,              /*!< Indicate a Private Beacon Status message received from the server, with msg
                                                                          *   @ref rtk_bt_mesh_private_beacon_status_t. */
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_GATT_PROXY_STATUS,       /*!< Indicate a Private GATT Proxy Status message received from the server, with msg
                                                                          *   @ref rtk_bt_mesh_private_gatt_proxy_status_t. */
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_NODE_IDENTITY_STATUS,    /*!< Indicate a Private Node Identity Status message received from the server, with msg
                                                                          *   @ref rtk_bt_mesh_private_node_identity_status_t. */
	RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_MAX,                     /*!< Reserved. */
} rtk_bt_mesh_private_beacon_client_model_evt_t;

/**
 * @struct    rtk_bt_mesh_private_beacon_status_t
 * @brief     BLE MESH private beacon client model status message structure.
 */
typedef struct {
	uint8_t private_beacon;                  /*!< Current Private Beacon state reported by the server.
                                              *   0x00: The node does not broadcast Mesh Private beacons.
                                              *   0x01: The node broadcasts Mesh Private beacons.
                                              *   0x02 to 0xFF: Prohibited. */
	uint8_t random_update_interval_steps;    /*!< Current Random Update Interval Steps state reported by the server, in units of 10 seconds.
                                              *   0x00: The Random field is updated for every Mesh Private beacon.
                                              *   0x01 to 0xFF: The Random field is updated at an interval of 10 * random_update_interval_steps seconds. */
} _PACKED4_ rtk_bt_mesh_private_beacon_status_t;

/**
 * @struct    rtk_bt_mesh_private_gatt_proxy_status_t
 * @brief     BLE MESH private beacon client model gatt proxy status message structure.
 */
typedef struct {
	uint8_t private_gatt_proxy;    /*!< Current Private GATT Proxy state reported by the server.
                                    *   0x00: The Private Proxy functionality is supported and disabled.
                                    *   0x01: The Private Proxy functionality is supported and enabled.
                                    *   0x02: The Private Proxy functionality is not supported.
                                    *   0x03 to 0xFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_private_gatt_proxy_status_t;

/**
 * @struct    rtk_bt_mesh_private_node_identity_status_t
 * @brief     BLE MESH private beacon client model node identity status message structure.
 */
typedef struct {
	uint8_t status;              /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index;      /*!< Index of the network key of the subnet.
                                  *   Range: 0x0000 to 0x0FFF. */
	uint8_t private_identity;    /*!< Current Private Node Identity state of the subnet reported by the server.
                                  *   0x00: Advertising with Private Node Identity for the subnet is stopped.
                                  *   0x01: Advertising with Private Node Identity for the subnet is running.
                                  *   0x02: Advertising with Private Node Identity is not supported.
                                  *   0x03 to 0xFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_private_node_identity_status_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  ble_mesh_private_beacon_model BT LE Mesh Private Beacon Model APIs
 * @brief     BT LE Mesh Private Beacon Model related function APIs
 * @ingroup   ble_mesh_api_group
 * @{
 */

/**
 * @brief     Send a Private Beacon Get message to a Private Beacon Server to read its Private Beacon state and Random Update Interval Steps state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_STATUS.
 * @param[in] private_beacon_get: Private Beacon Get message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_private_beacon_get(rtk_bt_mesh_private_beacon_get_t *private_beacon_get);

/**
 * @brief     Send a Private Beacon Set message to a Private Beacon Server to set its Private Beacon state and Random Update Interval Steps state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_STATUS.
 * @param[in] private_beacon_set: Private Beacon Set message parameters.
 *                                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_private_beacon_set(rtk_bt_mesh_private_beacon_set_t *private_beacon_set);

/**
 * @brief     Send a Private GATT Proxy Get message to a Private Beacon Server to read its Private GATT Proxy state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_GATT_PROXY_STATUS.
 * @param[in] private_gatt_proxy_get: Private GATT Proxy Get message parameters.
 *                                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_private_gatt_proxy_get(rtk_bt_mesh_private_gatt_proxy_get_t *private_gatt_proxy_get);

/**
 * @brief     Send a Private GATT Proxy Set message to a Private Beacon Server to set its Private GATT Proxy state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_GATT_PROXY_STATUS.
 * @param[in] private_gatt_proxy_set: Private GATT Proxy Set message parameters.
 *                                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_private_gatt_proxy_set(rtk_bt_mesh_private_gatt_proxy_set_t *private_gatt_proxy_set);

/**
 * @brief     Send a Private Node Identity Get message to a Private Beacon Server to read its Private Node Identity state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_NODE_IDENTITY_STATUS.
 * @param[in] private_node_identity_get: Private Node Identity Get message parameters.
 *                                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_private_node_identity_get(rtk_bt_mesh_private_node_identity_get_t *private_node_identity_get);

/**
 * @brief     Send a Private Node Identity Set message to a Private Beacon Server to set its Private Node Identity state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_PRIVATE_BEACON_CLIENT_MODEL_EVT_NODE_IDENTITY_STATUS.
 * @param[in] private_node_identity_set: Private Node Identity Set message parameters.
 *                                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_private_node_identity_set(rtk_bt_mesh_private_node_identity_set_t *private_node_identity_set);

/**
 * @}
 */
#endif /* __RTK_BT_MESH_PRIVATE_BEACON_MODEL_H__ */
