/**
 * @file      rtk_bt_mesh_common.h
 * @author    pico_zhai@realsil.com.cn
 * @brief     Bluetooth LE MESH stack api data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_COMMON_H__
#define __RTK_BT_MESH_COMMON_H__

#include <stdint.h>

#include <rtk_bt_le_gap.h>
#include <rtk_bt_mesh_def.h>

/************************************** Data Types *************************************/

/**
 * @enum      rtk_bt_mesh_stack_act_t
 * @brief     BLE MESH stack action definition, the action code passed to the stack by the mesh common APIs.
 */
typedef enum {
	RTK_BT_MESH_STACK_ACT_NODE_RESET = 1,           /*!< Reset the node, see @ref rtk_bt_mesh_stack_node_reset. */
	RTK_BT_MESH_STACK_ACT_SET_PROV_PARAM,           /*!< Set the provisioning parameters, see @ref rtk_bt_mesh_stack_prov_param_set. */
	RTK_BT_MESH_STACK_ACT_DEV_INFO_SWITCH,          /*!< Switch the device information display on or off, see @ref rtk_bt_mesh_stack_set_device_info_display. */
	RTK_BT_MESH_STACK_ACT_PB_ADV_CON,               /*!< Open a PB-ADV link to an unprovisioned device, see @ref rtk_bt_mesh_stack_prov_pb_adv_con. */
	RTK_BT_MESH_STACK_ACT_PB_ADV_DISCON,            /*!< Close the PB-ADV link, see @ref rtk_bt_mesh_stack_prov_pb_adv_discon. */
	RTK_BT_MESH_STACK_ACT_PB_GATT_CON,              /*!< Establish a BLE connection for PB-GATT or Mesh Proxy, see @ref rtk_bt_mesh_stack_prov_pb_gatt_con. */
	RTK_BT_MESH_STACK_ACT_PB_GATT_DISCON,           /*!< Disconnect the BLE connection for PB-GATT or Mesh Proxy, see @ref rtk_bt_mesh_stack_prov_pb_gatt_discon. */
	RTK_BT_MESH_STACK_ACT_START_PROV,               /*!< Start provisioning a device, see @ref rtk_bt_mesh_stack_start_provisioning. */
	RTK_BT_MESH_STACK_ACT_METHOD_CHOOSE_FOR_PROV,   /*!< Choose the provisioning method, see @ref rtk_bt_mesh_stack_method_choose_for_provisioning. */
	RTK_BT_MESH_STACK_ACT_SET_RANDOM_VALUE,         /*!< Set the random value used for authentication, see @ref rtk_bt_mesh_stack_set_random_value_for_authentication. */
	RTK_BT_MESH_STACK_ACT_GET_HB_SUB_RESULT,        /*!< Get the Heartbeat subscription result, see @ref rtk_bt_mesh_stack_get_heartbeat_subscribe_result. */
	RTK_BT_MESH_STACK_ACT_SET_MODEL_SUB,            /*!< Set the subscription of a model of the local node, see @ref rtk_bt_mesh_stack_set_model_subscribe. */
	RTK_BT_MESH_STACK_ACT_PROV_SERVICE_DISCOVERY,   /*!< Discover the Mesh Provisioning Service, see @ref rtk_bt_mesh_stack_prov_service_discovery. */
	RTK_BT_MESH_STACK_ACT_PROV_SERVICE_SET_NOTIFY,  /*!< Enable or disable the notification of the Mesh Provisioning Service, see
                                                     *   @ref rtk_bt_mesh_stack_prov_service_set_notify. */
	RTK_BT_MESH_STACK_ACT_PROXY_SERVICE_DISCOVERY,  /*!< Discover the Mesh Proxy Service, see @ref rtk_bt_mesh_stack_proxy_service_discovery. */
	RTK_BT_MESH_STACK_ACT_PROXY_SERVICE_SET_NOTIFY, /*!< Enable or disable the notification of the Mesh Proxy Service, see
                                                     *   @ref rtk_bt_mesh_stack_proxy_service_set_notify. */
	RTK_BT_MESH_STACK_ACT_USER_LIST_INFO,           /*!< List the information of the local node, see @ref rtk_bt_mesh_stack_user_list_info. */
	RTK_BT_MESH_STACK_ACT_FN_INIT,                  /*!< Initialize the Friend Node feature, see @ref rtk_bt_mesh_stack_fn_init. */
	RTK_BT_MESH_STACK_ACT_FN_DEINIT,                /*!< Deinitialize the Friend Node feature, see @ref rtk_bt_mesh_stack_fn_deinit. */
	RTK_BT_MESH_STACK_ACT_LPN_INIT,                 /*!< Initialize the Low Power Node feature, see @ref rtk_bt_mesh_stack_lpn_init. */
	RTK_BT_MESH_STACK_ACT_LPN_REQ,                  /*!< Request a friendship, see @ref rtk_bt_mesh_stack_lpn_req. */
	RTK_BT_MESH_STACK_ACT_LPN_SUB,                  /*!< Update the Friend Subscription List, see @ref rtk_bt_mesh_stack_lpn_sub. */
	RTK_BT_MESH_STACK_ACT_LPN_CLEAR,                /*!< Terminate the friendship, see @ref rtk_bt_mesh_stack_lpn_clear. */
	RTK_BT_MESH_STACK_ACT_LPN_DEINIT,               /*!< Deinitialize the Low Power Node feature, see @ref rtk_bt_mesh_stack_lpn_deinit. */
	RTK_BT_MESH_STACK_ACT_SEND_ONE_SHOT_ADV,        /*!< Send an one shot adv, see @ref rtk_bt_mesh_stack_send_one_shot_adv. */
	RTK_BT_MESH_STACK_ACT_SET_RETRANS_PARAM,        /*!< Set the retransmission parameters, see @ref rtk_bt_mesh_stack_retrans_param_set. */
	RTK_BT_MESH_STACK_ACT_SET_SERVICE_ADV,          /*!< Switch the connectable mesh service advertising on or off, see @ref rtk_bt_mesh_stack_set_service_adv. */
#if defined(RTK_BLE_MESH_BASED_ON_CODED_PHY) && RTK_BLE_MESH_BASED_ON_CODED_PHY
	RTK_BT_MESH_STACK_ACT_SET_TX_PHY,               /*!< Set the PHY used to transmit mesh advertising, see @ref rtk_bt_mesh_stack_set_tx_phy. */
#endif
	RTK_BT_MESH_STACK_ACT_MAX,                      /*!< Reserved. */
} rtk_bt_mesh_stack_act_t;

/**
 * @enum      rtk_bt_mesh_stack_event_t
 * @brief     BLE MESH stack event definition, the event code reported to the application by the mesh common part.
 */
typedef enum {
	RTK_BT_MESH_STACK_EVT_DEVICE_INFO_UDB_DISPLAY = 1,      /*!< Indicate an Unprovisioned Device Beacon is received while the device information display is on,
                                                             *   with msg @ref rtk_bt_mesh_stack_evt_dev_info_udb_t. */
	RTK_BT_MESH_STACK_EVT_DEVICE_INFO_SNB_DISPLAY,          /*!< Indicate a Secure Network Beacon is received while the device information display is on, with msg
                                                             *   @ref rtk_bt_mesh_stack_evt_dev_info_snb_t. */
	RTK_BT_MESH_STACK_EVT_DEVICE_INFO_PROV_DISPLAY,         /*!< Indicate a Mesh Provisioning Service advertising packet is received while the device information
                                                             *   display is on, with msg @ref rtk_bt_mesh_stack_evt_dev_info_provision_adv_t. */
	RTK_BT_MESH_STACK_EVT_DEVICE_INFO_PROXY_DISPLAY,        /*!< Indicate a Mesh Proxy Service advertising packet is received while the device information display is
                                                             *   on, with msg @ref rtk_bt_mesh_stack_evt_dev_info_proxy_adv_t. */
	RTK_BT_MESH_STACK_EVT_PB_ADV_LINK_STATE,                /*!< Indicate the state of the PB-ADV link changes, with msg @ref rtk_bt_mesh_prov_generic_cb_type_t. */
	RTK_BT_MESH_STACK_EVT_SET_AUTH_VALUE_FOR_STATIC_OOB,    /*!< Indicate the Static OOB authentication value is set during provisioning, with msg
                                                             *   @ref rtk_bt_mesh_stack_set_auth_value_for_static_oob. */
	RTK_BT_MESH_STACK_EVT_SET_AUTH_VALUE_FOR_OUTPUT_OOB,    /*!< Indicate the random value to be output by the device is generated during Output OOB provisioning,
                                                             *   with msg @ref rtk_bt_mesh_stack_set_auth_value_for_oob_data. */
	RTK_BT_MESH_STACK_EVT_SET_AUTH_VALUE_FOR_INPUT_OOB,     /*!< Indicate the random value to be displayed by the provisioner and entered on the device is generated
                                                             *   during Input OOB provisioning, with msg @ref rtk_bt_mesh_stack_set_auth_value_for_oob_data. */
	RTK_BT_MESH_STACK_EVT_NOTIFY_FOR_OUTPUT_OOB_VALUE,      /*!< Indicate the value output by the device is to be entered during Output OOB provisioning, there is no
                                                             *   msg. */
	RTK_BT_MESH_STACK_EVT_NOTIFY_FOR_INPUT_OOB_VALUE,       /*!< Indicate the value displayed by the provisioner is to be entered on the device during Input OOB
                                                             *   provisioning, there is no msg. */
	RTK_BT_MESH_STACK_EVT_CAPABILITY_NOT_SUPPORT_OF_DEVICE, /*!< Indicate the capabilities of the device do not meet the expected provisioning method, with msg
                                                             *   @ref rtk_bt_mesh_stack_prov_capabilities_t. */
	RTK_BT_MESH_STACK_EVT_PROV_COMPLETE,                    /*!< Indicate the provisioning procedure completes, with msg @ref rtk_bt_mesh_stack_evt_prov_complete_t. */
	RTK_BT_MESH_STACK_EVT_PROV_FAIL,                        /*!< Indicate the provisioning procedure fails, with msg @ref rtk_bt_mesh_stack_evt_prov_fail_t. */
	RTK_BT_MESH_STACK_EVT_DEVICE_START_PROVED,              /*!< Indicate the device receives the Provisioning Start PDU, there is no msg. */
	RTK_BT_MESH_STACK_EVT_PROV_SERVICE_DIS_RESULT,          /*!< Indicate the result of the Mesh Provisioning Service discovery, with msg
                                                             *   @ref rtk_bt_mesh_stack_evt_prov_dis_t. */
	RTK_BT_MESH_STACK_EVT_PROXY_SERVICE_DIS_RESULT,         /*!< Indicate the result of the Mesh Proxy Service discovery, with msg
                                                             *   @ref rtk_bt_mesh_stack_evt_proxy_dis_t. */
	RTK_BT_MESH_STACK_EVT_UNPROVISIONED_DEVICE,             /*!< Indicate the device is unprovisioned, there is no msg. */
	RTK_BT_MESH_STACK_EVT_PROVISIONED_DEVICE,               /*!< Indicate the device is provisioned, with msg @ref rtk_bt_mesh_stack_evt_provisioned_device_t. */
	RTK_BT_MESH_STACK_EVT_SCAN_SWITCH,                      /*!< Indicate the scan is switched on or off, with msg @ref rtk_bt_mesh_stack_evt_scan_switch_t. */
	RTK_BT_MESH_STACK_EVT_START_ADV,                        /*!< Indicate the result of starting the advertising, with msg @ref rtk_bt_mesh_stack_evt_start_adv_t. */
	RTK_BT_MESH_STACK_EVT_STOP_ADV,                         /*!< Indicate the result of stopping the advertising, with msg @ref rtk_bt_mesh_stack_evt_stop_adv_t. */
	RTK_BT_MESH_STACK_EVT_LIST_INFO,                        /*!< Indicate the information of the local node requested by @ref rtk_bt_mesh_stack_user_list_info. The msg
                                                             *   is a byte stream of items, each item is the type of @ref rtk_bt_mesh_stack_user_list_type_t (1
                                                             *   byte), the length of the value (1 byte) and the value. */
	RTK_BT_MESH_STACK_EVT_FN_CB,                            /*!< Indicate the state of a friendship changes on the Friend Node, with msg
                                                             *   @ref rtk_bt_mesh_stack_evt_fn_t. */
	RTK_BT_MESH_STACK_EVT_LPN_CB,                           /*!< Indicate the state of a friendship changes on the Low Power Node, with msg
                                                             *   @ref rtk_bt_mesh_stack_evt_lpn_t. */
	RTK_BT_MESH_STACK_EVT_HB_PUB_TIMER_STATE,               /*!< Indicate the state of the Heartbeat publication timer changes, with msg
                                                             *   @ref rtk_bt_mesh_stack_hb_data_timer_state_t. */
	RTK_BT_MESH_STACK_EVT_HB_SUB_TIMER_STATE,               /*!< Indicate the state of the Heartbeat subscription timer changes, with msg
                                                             *   @ref rtk_bt_mesh_stack_hb_data_timer_state_t. */
	RTK_BT_MESH_STACK_EVT_HB_PUB_COUNT_UPDATE,              /*!< Indicate the Heartbeat publication count is updated, with msg
                                                             *   @ref rtk_bt_mesh_stack_hb_pub_count_update_t. */
	RTK_BT_MESH_STACK_EVT_HB_SUB_PERIOD_UPDATE,             /*!< Indicate the Heartbeat subscription period is updated, with msg
                                                             *   @ref rtk_bt_mesh_stack_hb_sub_period_update_t. */
	RTK_BT_MESH_STACK_EVT_HB_SUB_RECEIVE,                   /*!< Indicate a Heartbeat message is received for the Heartbeat subscription, with msg
                                                             *   @ref rtk_bt_mesh_stack_hb_data_sub_receive_t. */
	RTK_BT_MESH_STACK_EVT_RETRANS_PARAM_SETTING_RESULT,     /*!< Indicate the retransmission parameters are set by @ref rtk_bt_mesh_stack_retrans_param_set, with msg
                                                             *   @ref rtk_bt_mesh_stack_set_retrans_param_t that carries the values in effect. */
	RTK_BT_MESH_STACK_EVT_DF_CB,                            /*!< Indicate a directed forwarding path action, with msg @ref rtk_bt_mesh_stack_evt_df_t. */
	RTK_BT_MESH_STACK_EVT_MAX,                              /*!< Reserved. */

} rtk_bt_mesh_stack_event_t;

/**
 * @enum      rtk_bt_mesh_stack_api_result_t
 * @brief     BLE MESH stack api result definition.
 */
typedef enum {
	RTK_BT_MESH_STACK_API_SUCCESS = 0,    /*!< The operation succeeds. */
	RTK_BT_MESH_STACK_API_FAIL,           /*!< The operation fails. */
} rtk_bt_mesh_stack_api_result_t;

/**
 * @enum      rtk_bt_mesh_stack_user_list_type_t
 * @brief     BLE MESH stack user list type definition, the type of an item of the node information list.
 */
typedef enum {
	RTK_BT_MESH_STACK_USER_LIST_MESH_STATE = 1,        /*!< State of the node. */
	RTK_BT_MESH_STACK_USER_LIST_DEV_UUID,              /*!< Device UUID of the node. */
	RTK_BT_MESH_STACK_USER_LIST_DEV_ADDR,              /*!< Bluetooth device address of the node. */
	RTK_BT_MESH_STACK_USER_LIST_DEV_KEY,               /*!< A device key stored on the node. */
	RTK_BT_MESH_STACK_USER_LIST_NET_KEY,               /*!< A NetKey stored on the node, the first byte of the value is
                                                        *   of the type @ref rtk_bt_mesh_stack_net_key_type. */
	RTK_BT_MESH_STACK_USER_LIST_APP_KEY,               /*!< An AppKey stored on the node. */
	RTK_BT_MESH_STACK_USER_LIST_NORMAL_VALS,           /*!< General values of the node: the IV Update flag, the IV Index, the sequence number,
                                                        *   the unicast address, the number of elements and the number of models. */
	RTK_BT_MESH_STACK_USER_LIST_ELEMENT_INFO,          /*!< Information of an element of the node. */
	RTK_BT_MESH_STACK_USER_LIST_MODEL_INFO,            /*!< Information of a model of the node. */
	RTK_BT_MESH_STACK_USER_LIST_MODEL_APP_KEY,         /*!< AppKey bound to a model of the node. */
	RTK_BT_MESH_STACK_USER_LIST_MODEL_PUB_INFO,        /*!< Publication information of a model of the node. */
	RTK_BT_MESH_STACK_USER_LIST_MODEL_SUB_INFO,        /*!< Subscription information of a model of the node. */
	RTK_BT_MESH_STACK_USER_LIST_DF_PATH_INFO,          /*!< Information of a directed forwarding path. */
	RTK_BT_MESH_STACK_USER_LIST_SUBNET_BRIDGE_INFO,    /*!< Information of a Subnet Bridge entry. */
} rtk_bt_mesh_stack_user_list_type_t;

/**
 * @enum      rtk_bt_mesh_stack_net_key_type
 * @brief     BLE MESH net key type, the kind of security material derived from a NetKey.
 */
typedef enum {
	RTK_BT_MESH_NET_KEY_MASTER = 1,    /*!< Managed flooding security material. */
	RTK_BT_MESH_NET_KEY_FN,            /*!< Friendship security material. */
	RTK_BT_MESH_NET_KEY_DF,            /*!< Directed security material. */
} rtk_bt_mesh_stack_net_key_type;

/**
 * @enum      rtk_bt_mesh_stack_role_t
 * @brief     BLE MESH device role type definition, the role of the node selected by the application when the mesh stack is enabled.
 */
typedef enum {
	RTK_BT_MESH_ROLE_DEVICE,         /*!< The node works as a device, which is provisioned by a provisioner. */
	RTK_BT_MESH_ROLE_PROVISIONER     /*!< The node works as a provisioner, which provisions devices. */
} rtk_bt_mesh_stack_role_t;

/**
 * @struct    rtk_bt_mesh_stack_act_set_prov_param_t
 * @brief     BLE MESH provisioning parameter set structure.
 */
typedef struct {
	uint16_t unicast_addr;    /*!< Unicast address of the primary element of this node.
                               *   0: Use the default value 0x0200 | the lowest byte of the Bluetooth address. */
	uint8_t net_key[16];      /*!< NetKey of this node, which is added with NetKey Index 0.
                               *   All 0: Use the default NetKey, whose last 6 bytes are replaced by the Bluetooth address. */
	uint8_t app_key[16];      /*!< AppKey of this node, which is bound to the NetKey and added with AppKey Index 0.
                               *   All 0: Use the default AppKey, whose last 6 bytes are replaced by the Bluetooth address. */
	uint8_t dev_key[16];      /*!< DevKey of this node.
                               *   All 0: Use the default DevKey, whose last 6 bytes are replaced by the Bluetooth address. */
} rtk_bt_mesh_stack_act_set_prov_param_t;

/**
 * @struct    rtk_bt_mesh_stack_act_device_info_set_t
 * @brief     BLE MESH device information display switch structure.
 */
typedef struct {
	uint8_t dev_info_dis;    /*!< Whether to report the information of the received unprovisioned device beacons, Secure Network Beacons,
                              *   provisioning advertisements and Mesh Proxy advertisements.
                              *   0: Do not report.
                              *   Others: Report. */
} rtk_bt_mesh_stack_act_device_info_set_t;

/**
 * @struct    rtk_bt_mesh_stack_act_pb_adv_con_t
 * @brief     BLE MESH PB-ADV link open structure.
 */
typedef struct {
	uint8_t uuid[16];    /*!< Device UUID of the unprovisioned device to open the PB-ADV link with. */
} rtk_bt_mesh_stack_act_pb_adv_con_t;

/**
 * @union     rtk_bt_mesh_stack_act_pb_gatt_con_t
 * @brief     BLE MESH GATT connection establish structure.
 */
typedef union {
	rtk_bt_le_create_conn_param_t conn_param;    /*!< Parameters of the BLE connection to establish, by the peer address. */
} rtk_bt_mesh_stack_act_pb_gatt_con_t;

/**
 * @union     rtk_bt_mesh_stack_act_pb_gatt_discon_t
 * @brief     BLE MESH GATT disconnection structure.
 */
typedef union {
	uint16_t conn_handle;    /*!< Connection handle of the BLE connection to disconnect.
                              *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_mesh_stack_act_pb_gatt_discon_t;

/**
 * @struct    rtk_bt_mesh_stack_act_start_prov_t
 * @brief     BLE MESH start provisioning structure.
 */
typedef struct {
	uint8_t attn_dur;    /*!< Attention Duration of the Provisioning Invite PDU, the time in seconds that the elements of the unprovisioned device
                          *   attract human attention. */
} rtk_bt_mesh_stack_act_start_prov_t;

/**
 * @struct    rtk_bt_mesh_stack_act_prov_dis_t
 * @brief     BLE MESH Mesh Provisioning Service discovery structure.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the BLE connection to discover the Mesh Provisioning Service on.
                              *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_mesh_stack_act_prov_dis_t;

/**
 * @enum      rtk_bt_mesh_stack_prov_dis_result
 * @brief     BLE MESH Mesh Provisioning Service discovery result definition.
 */
typedef enum {
	RTK_BT_MESH_PROV_DISC_DONE,         /*!< The discovery is done and the Mesh Provisioning Service is found. */
	RTK_BT_MESH_PROV_DISC_FAIL,         /*!< The discovery failed. */
	RTK_BT_MESH_PROV_DISC_NOT_FOUND,    /*!< The discovery is done but the Mesh Provisioning Service is not found. */
} rtk_bt_mesh_stack_prov_dis_result;

/**
 * @struct    rtk_bt_mesh_stack_evt_prov_dis_t
 * @brief     BLE MESH Mesh Provisioning Service discovery event structure.
 */
typedef struct {
	rtk_bt_mesh_stack_prov_dis_result dis_result;    /*!< Result of the discovery. */
} rtk_bt_mesh_stack_evt_prov_dis_t;

/**
 * @struct    rtk_bt_mesh_stack_act_prov_set_notify_t
 * @brief     BLE MESH Mesh Provisioning Data Out notification enable structure.
 */
typedef struct {
	bool is_enable;    /*!< Whether to enable the notification of the Mesh Provisioning Data Out characteristic.
                        *   - true: Enable.
                        *   - false: Disable. */
} rtk_bt_mesh_stack_act_prov_set_notify_t;

/**
 * @struct    rtk_bt_mesh_stack_act_proxy_dis_t
 * @brief     BLE MESH Mesh Proxy Service discovery structure.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the BLE connection to discover the Mesh Proxy Service on.
                              *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_mesh_stack_act_proxy_dis_t;

/**
 * @enum      rtk_bt_mesh_stack_proxy_dis_result
 * @brief     BLE MESH Mesh Proxy Service discovery result definition.
 */
typedef enum {
	RTK_BT_MESH_PROXY_DISC_DONE,         /*!< The discovery is done and the Mesh Proxy Service is found. */
	RTK_BT_MESH_PROXY_DISC_FAIL,         /*!< The discovery failed. */
	RTK_BT_MESH_PROXY_DISC_NOT_FOUND,    /*!< The discovery is done but the Mesh Proxy Service is not found. */
} rtk_bt_mesh_stack_proxy_dis_result;

/**
 * @struct    rtk_bt_mesh_stack_evt_proxy_dis_t
 * @brief     BLE MESH Mesh Proxy Service discovery event structure.
 */
typedef struct {
	rtk_bt_mesh_stack_proxy_dis_result dis_result;    /*!< Result of the discovery. */
} rtk_bt_mesh_stack_evt_proxy_dis_t;

/**
 * @struct    rtk_bt_mesh_stack_act_proxy_set_notify_t
 * @brief     BLE MESH Mesh Proxy Data Out notification enable structure.
 */
typedef struct {
	bool is_enable;    /*!< Whether to enable the notification of the Mesh Proxy Data Out characteristic.
                        *   - true: Enable.
                        *   - false: Disable. */
} rtk_bt_mesh_stack_act_proxy_set_notify_t;

/**
 * @struct    rtk_bt_mesh_stack_evt_scan_switch_t
 * @brief     BLE MESH scan switch event structure.
 */
typedef struct {
	bool scan_enable;    /*!< Whether the scan has been started or stopped.
                          *   - true: The scan has been started.
                          *   - false: The scan has been stopped. */
} rtk_bt_mesh_stack_evt_scan_switch_t;

/**
 * @struct    rtk_bt_mesh_stack_act_send_adv_t
 * @brief     BLE MESH one shot advertising send structure.
 */
typedef struct {
	uint8_t adv_raw_data[RTK_BT_LE_MAX_ADV_DATA_LEN];    /*!< Raw advertising data to send. */
	uint8_t adv_data_len;                                /*!< Length in bytes of the valid data in adv_raw_data. */
	rtk_bt_le_adv_type_t adv_type;                       /*!< Advertising type used to send the data. */
	rtk_bt_le_addr_type_t addr_type;                     /*!< Own address type used to send the data. */
} rtk_bt_mesh_stack_act_send_adv_t;

/**
 * @struct    rtk_bt_mesh_stack_evt_start_adv_t
 * @brief     BLE MESH start advertising event structure.
 */
typedef struct {
	uint16_t result;    /*!< Result of starting the advertising, see @ref rtk_bt_mesh_stack_api_result_t. */
} rtk_bt_mesh_stack_evt_start_adv_t;

/**
 * @struct    rtk_bt_mesh_stack_evt_stop_adv_t
 * @brief     BLE MESH stop advertising event structure.
 */
typedef struct {
	uint16_t result;    /*!< Result of stopping the advertising, see @ref rtk_bt_mesh_stack_api_result_t. */
} rtk_bt_mesh_stack_evt_stop_adv_t;

/**
 * @struct    rtk_bt_mesh_stack_act_fn_init_t
 * @brief     BLE MESH Friend Node initialization structure.
 */
typedef struct {
	uint8_t lpn_num;        /*!< Maximum number of Low Power nodes that the Friend node supports at the same time. */
	uint8_t queue_size;     /*!< Size of the Friend Queue for each Low Power node, the number of messages that the Friend node can store. */
	uint8_t rx_window;      /*!< ReceiveWindow of the Friend Offer message, in units of 1 millisecond.
                             *   Range: 0x01 to 0xFF. */
} rtk_bt_mesh_stack_act_fn_init_t;

/**
 * @enum      rtk_bt_mesh_stack_evt_fn_cb_type
 * @brief     BLE MESH Friend Node event callback type definition.
 */
typedef enum {
	RTK_BT_MESH_FN_CB_TYPE_ESTABLISHING,              /*!< Friendship with the Low Power node is being established. */
	RTK_BT_MESH_FN_CB_TYPE_ESTABLISH_FAIL_NO_POLL,    /*!< Friendship establishment failed because no Friend Poll was received from the Low Power node. */
	RTK_BT_MESH_FN_CB_TYPE_ESTABLISH_SUCCESS,         /*!< Friendship with the Low Power node has been established. */
	RTK_BT_MESH_FN_CB_TYPE_FRND_LOST,                 /*!< Friendship with the Low Power node has been lost. */
} rtk_bt_mesh_stack_evt_fn_cb_type;

/**
 * @struct    rtk_bt_mesh_stack_evt_fn_t
 * @brief     BLE MESH Friend Node event structure.
 */
typedef struct {
	rtk_bt_mesh_stack_evt_fn_cb_type cb_type;    /*!< Type of the friendship event. */
	uint16_t lpn_addr;                           /*!< Unicast address of the Low Power node. */
} rtk_bt_mesh_stack_evt_fn_t;

/**
 * @struct    rtk_bt_mesh_stack_act_lpn_init_t
 * @brief     BLE MESH Low Power node init param structure.
 */
typedef struct {
	uint8_t fn_num;    /*!< Maximum number of friendships that the Low Power node can establish, one for each Friend node. */
} rtk_bt_mesh_stack_act_lpn_init_t;

/**
 * @struct    rtk_bt_mesh_stack_act_lpn_req_t
 * @brief     BLE MESH Low Power node request a friendship param structure.
 */
typedef struct {
	uint8_t fn_index;          /*!< Index of the friendship to establish, managed by the application.
                                *   It shall be less than fn_num of @ref rtk_bt_mesh_stack_act_lpn_init_t. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet to establish the friendship on.
                                *   At most one friendship can be established on a NetKey.
                                *   Range: 0x0000 to 0x0FFF. */
	uint32_t poll_interval;    /*!< Interval between two Friend Poll messages, in units of 100 milliseconds.
                                *   It is expected to be less than poll_timeout. */
	uint32_t poll_timeout;     /*!< Initial value of the PollTimeout timer, in units of 100 milliseconds.
                                *   Range: 0x00000A to 0x34BBFF. */
	uint8_t frnd_rx_delay;     /*!< Receive Delay requested from the Friend node, in milliseconds.
                                *   Range: 0x0A to 0xFF. */
	uint8_t frnd_rx_widen;     /*!< Extra time, in milliseconds, that the Low Power node widens its receive window by.
                                *   Range: 0x00 to 0xFF. */
} rtk_bt_mesh_stack_act_lpn_req_t;

/**
 * @struct    rtk_bt_mesh_stack_act_lpn_sub_t
 * @brief     BLE MESH Low Power node subscribe add or remove param structure.
 */
typedef struct {
	uint8_t fn_index;    /*!< Index of the friendship whose Friend Subscription List is modified. */
	uint16_t addr;       /*!< Group address or virtual address to add to or remove from the Friend Subscription List. */
	bool add_rm;         /*!< Whether to add or remove the address.
                          *   - true: Add the address.
                          *   - false: Remove the address. */
} rtk_bt_mesh_stack_act_lpn_sub_t;

/**
 * @struct    rtk_bt_mesh_stack_act_lpn_clear_t
 * @brief     BLE MESH Low Power node clear friendship param structure.
 */
typedef struct {
	uint8_t fn_index;    /*!< Index of the friendship to clear. */
} rtk_bt_mesh_stack_act_lpn_clear_t;

/**
 * @enum      rtk_bt_mesh_stack_lpn_req_result_type
 * @brief     BLE MESH Low Power node request a friendship result type.
 */
typedef enum {
	RTK_BT_MESH_LPN_REQ_REASON_SUCCESS,                                 /*!< The request is accepted, the result of the friendship establishment is reported by event
                                                                         *   @ref RTK_BT_MESH_STACK_EVT_LPN_CB. */
	RTK_BT_MESH_LPN_REQ_REASON_REQ_PARAMS_INVALID,                      /*!< The parameters of the request are invalid. */
	RTK_BT_MESH_LPN_REQ_REASON_NET_KEY_INDEX_INVALID,                   /*!< The NetKey index of the request is invalid. */
	RTK_BT_MESH_LPN_REQ_REASON_FRND_ON_SAME_NET_KEY_INDEX,              /*!< A friendship has already been established on the NetKey of the request. */
	RTK_BT_MESH_LPN_REQ_REASON_FRND_INDEX_INVALID,                      /*!< The friendship index of the request is invalid. */
	RTK_BT_MESH_LPN_REQ_REASON_FRND_ALREADY_EXIST_OR_ESTABLISING,       /*!< The friendship of the request already exists or is being established. */
	RTK_BT_MESH_LPN_REQ_REASON_RESOURCE_INSUFFICIENT,                   /*!< The resources are insufficient to establish the friendship. */
	RTK_BT_MESH_LPN_REQ_REASON_PROXY_SERVER_DO_NOT_SUPPORT_LPN,         /*!< The request is rejected because the node has a BLE link. */
	RTK_BT_MESH_LPN_REQ_REASON_UNKNOWN,                                 /*!< Unknown reason. */
} rtk_bt_mesh_stack_lpn_req_result_type;

/**
 * @enum      rtk_bt_mesh_stack_evt_lpn_cb_type
 * @brief     BLE MESH friendship Low Power node event cb type.
 */
typedef enum {
	RTK_BT_MESH_LPN_CB_TYPE_ESTABLISH_SUCCESS,        /*!< The friendship is established. */
	RTK_BT_MESH_LPN_CB_TYPE_ESTABLISH_FAIL_NO_OFFER,  /*!< The friendship is not established because no Friend Offer is received. */
	RTK_BT_MESH_LPN_CB_TYPE_ESTABLISH_FAIL_NO_UPDATE, /*!< The friendship is not established because no Friend Update is received. */
	RTK_BT_MESH_LPN_CB_TYPE_FRIENDSHIP_LOST,          /*!< The friendship is lost or cleared. */
	RTK_BT_MESH_LPN_CB_TYPE_UNKNOWN,                  /*!< Unknown type. */
} rtk_bt_mesh_stack_evt_lpn_cb_type;

/**
 * @struct    rtk_bt_mesh_stack_evt_lpn_t
 * @brief     BLE MESH friendship Low Power node event structure.
 */
typedef struct {
	rtk_bt_mesh_stack_evt_lpn_cb_type cb_type;    /*!< Type of the friendship event. */
	uint16_t fn_addr;                             /*!< Unicast address of the Friend node. */
} rtk_bt_mesh_stack_evt_lpn_t;

/**
 * @enum      rtk_bt_mesh_stack_prov_cb_fail_type_t
 * @brief     BLE MESH prov fail reason type.
 */
typedef enum {
	RTK_BT_MESH_PROV_CB_FAIL_TYPE_BEARER_LOSS,          /*!< The PB-ADV link is closed or the LE link is disconnected. */
	RTK_BT_MESH_PROV_CB_FAIL_TYPE_BEARER_TIMEOUT,       /*!< A PB-ADV transaction times out. */
	RTK_BT_MESH_PROV_CB_FAIL_TYPE_BEARER_IDLE,          /*!< The PB-ADV link is idle for too long. */
	RTK_BT_MESH_PROV_CB_FAIL_TYPE_PROCEDURE_TIMEOUT,    /*!< The provisioning procedure times out. */
	RTK_BT_MESH_PROV_CB_FAIL_TYPE_PROCEDURE_ABNORMAL    /*!< The provisioning procedure fails with an error. */
} rtk_bt_mesh_stack_prov_cb_fail_type_t;

/**
 * @struct    rtk_bt_mesh_stack_evt_prov_fail_t
 * @brief     BLE MESH provisioning fail reason structure.
 */
typedef struct {
	rtk_bt_mesh_stack_prov_cb_fail_type_t fail_reason;    /*!< Reason of the provisioning failure. */
} rtk_bt_mesh_stack_evt_prov_fail_t;

/**
 * @struct    rtk_bt_mesh_stack_evt_provisioned_device_t
 * @brief     BLE MESH provisioned device info structure.
 */
typedef struct {
	uint16_t unicast_addr;    /*!< Unicast address of the primary element of this node. */
} rtk_bt_mesh_stack_evt_provisioned_device_t;

/**
 * @struct    rtk_bt_mesh_stack_device_info_common_data
 * @brief     BLE MESH device beacon common info structure.
 */
typedef struct {
	uint8_t bt_addr[6];      /*!< Bluetooth address of the device that sent the advertising packet. */
	uint8_t bt_addr_type;    /*!< Type of the Bluetooth address, see @ref rtk_bt_le_addr_type_t.
                              *   0xFF: The Bluetooth address and RSSI are not available. */
	int8_t rssi;             /*!< RSSI of the received advertising packet, in dBm. */
} rtk_bt_mesh_stack_device_info_common_data;

/**
 * @struct    rtk_bt_mesh_stack_evt_dev_info_udb_t
 * @brief     BLE MESH device unprovisioned device beacon info structure.
 */
typedef struct {
	rtk_bt_mesh_stack_device_info_common_data dev_info;    /*!< Information of the device that sent the Unprovisioned Device beacon. */
	uint8_t dev_uuid[16];                                  /*!< Device UUID of the unprovisioned device. */
	uint8_t oob_info[2];                                   /*!< OOB Information of the unprovisioned device, in the byte order of the beacon (big-endian). */
	uint8_t uri_hash[4];                                   /*!< Hash of the URI advertised by the unprovisioned device, in the byte order of the beacon (big-endian).
                                                            *   It is an optional field of the beacon, it is only meaningful when the beacon carries a URI Hash. */
} rtk_bt_mesh_stack_evt_dev_info_udb_t;

/**
 * @struct    rtk_bt_mesh_stack_evt_dev_info_snb_t
 * @brief     BLE MESH device secure network beacon info structure.
 */
typedef struct {
	rtk_bt_mesh_stack_device_info_common_data dev_info;    /*!< Information of the device that sent the Secure Network beacon. */
	uint8_t key_refresh_flag: 1;                           /*!< Bit 0 of the Flags field, the Key Refresh Flag.
                                                            *   0: False.
                                                            *   1: True. */
	uint8_t iv_update_flag: 1;                             /*!< Bit 1 of the Flags field, the IV Update Flag.
                                                            *   0: Normal operation.
                                                            *   1: IV Update in progress. */
	uint8_t rfu: 6;                                        /*!< Reserved for future use. */
	uint8_t net_id[8];                                     /*!< Network ID of the subnet. */
	uint8_t iv_index[4];                                   /*!< Current IV Index. */
	uint8_t auth[8];                                       /*!< Authentication Value of the beacon. */
} rtk_bt_mesh_stack_evt_dev_info_snb_t;

/**
 * @struct    rtk_bt_mesh_stack_evt_prov_complete_t
 * @brief     BLE MESH device provisioning complete info structure.
 */
typedef struct {
	uint16_t unicast_addr;    /*!< Unicast address of the primary element of the provisioned node. */
	bool dkri_flag;           /*!< Whether the completed procedure is a Device Key Refresh Interface procedure.
                               *   - true: The procedure is a Device Key Refresh Interface procedure, see dkri.
                               *   - false: The procedure is a provisioning procedure. */
	uint8_t dkri;             /*!< Type of the Device Key Refresh Interface procedure, see @ref rtk_bt_mesh_rmt_prov_dkri_procedure_t.
                               *   It is only valid when dkri_flag is true. */
} rtk_bt_mesh_stack_evt_prov_complete_t;

/**
 * @struct    rtk_bt_mesh_stack_prov_oob_info_t
 * @brief     BLE MESH device oob info structure.
 */
typedef struct {
	uint16_t other: 1;             /*!< Bit 0: Other.
                                    *   0: Not available.
                                    *   1: Available. */
	uint16_t electronic_uri: 1;    /*!< Bit 1: Electronic / URI.
                                    *   0: Not available.
                                    *   1: Available. */
	uint16_t qr_code: 1;           /*!< Bit 2: 2D machine-readable code.
                                    *   0: Not available.
                                    *   1: Available. */
	uint16_t bar_code: 1;          /*!< Bit 3: Bar code.
                                    *   0: Not available.
                                    *   1: Available. */
	uint16_t nfc: 1;               /*!< Bit 4: Near Field Communication (NFC).
                                    *   0: Not available.
                                    *   1: Available. */
	uint16_t number: 1;            /*!< Bit 5: Number.
                                    *   0: Not available.
                                    *   1: Available. */
	uint16_t string: 1;            /*!< Bit 6: String.
                                    *   0: Not available.
                                    *   1: Available. */
	uint16_t rfu: 4;               /*!< Bits 7 to 10.
                                    *   Bit 7: Support for certificate-based provisioning.
                                    *   Bit 8: Support for provisioning records.
                                    *   Bits 9 and 10: Reserved for future use. */
	uint16_t on_box: 1;            /*!< Bit 11: On box.
                                    *   0: Not available.
                                    *   1: Available. */
	uint16_t inside_box: 1;        /*!< Bit 12: Inside box.
                                    *   0: Not available.
                                    *   1: Available. */
	uint16_t on_paper: 1;          /*!< Bit 13: On piece of paper.
                                    *   0: Not available.
                                    *   1: Available. */
	uint16_t inside_manual: 1;     /*!< Bit 14: Inside manual.
                                    *   0: Not available.
                                    *   1: Available. */
	uint16_t on_device: 1;         /*!< Bit 15: On device.
                                    *   0: Not available.
                                    *   1: Available. */
} rtk_bt_mesh_stack_prov_oob_info_t;

/**
 * @struct    rtk_bt_mesh_stack_evt_dev_info_provision_adv_t
 * @brief     BLE MESH device provisioin adv info structure.
 */
typedef struct {
	rtk_bt_mesh_stack_device_info_common_data dev_info;    /*!< Information of the device that sent the Mesh Provisioning Service advertising packet. */
	uint8_t dev_uuid[16];                                  /*!< Device UUID of the unprovisioned device. */
	rtk_bt_mesh_stack_prov_oob_info_t oob_info;            /*!< OOB Information of the unprovisioned device. */
} rtk_bt_mesh_stack_evt_dev_info_provision_adv_t;

/**
 * @enum      proxy_adv_type_t
 * @brief     BLE MESH Mesh Proxy Service advertising identification type definition.
 */
enum {
	RTK_BT_MESH_PROXY_ADV_TYPE_NET_ID = 0,                  /*!< Advertising with Network ID. */
	RTK_BT_MESH_PROXY_ADV_TYPE_NODE_IDENTITY = 1,           /*!< Advertising with Node Identity. */
#if defined(BT_MESH_ENABLE_PRIVATE_BEACON) && BT_MESH_ENABLE_PRIVATE_BEACON
	RTK_BT_MESH_PROXY_ADV_TYPE_PRIVATE_NET_ID = 2,          /*!< Advertising with Private Network Identity. */
	RTK_BT_MESH_PROXY_ADV_TYPE_PRIVATE_NODE_IDENTITY = 3,   /*!< Advertising with Private Node Identity. */
#endif
} _SHORT_ENUM_;
typedef uint8_t proxy_adv_type_t;

/**
 * @struct    rtk_bt_mesh_stack_mesh_service_data_proxy_t
 * @brief     BLE MESH Mesh Proxy Service data structure.
 */
typedef struct {
	proxy_adv_type_t type;       /*!< Identification type of the advertising, see @ref proxy_adv_type_t. */
	union {
		uint8_t net_id[8];       /*!< Network ID, valid when type is @ref RTK_BT_MESH_PROXY_ADV_TYPE_NET_ID. */
		struct {
			uint8_t hash[8];     /*!< Hash, a function of the random number and the identity information.
                                  *   Valid when type is any other value. */
			uint8_t random[8];   /*!< 64-bit random number.
                                  *   Valid when type is any other value. */
		};
	};
} rtk_bt_mesh_stack_mesh_service_data_proxy_t;

/**
 * @struct    rtk_bt_mesh_stack_evt_dev_info_proxy_adv_t
 * @brief     BLE MESH device proxy data info structure.
 */
typedef struct {
	rtk_bt_mesh_stack_device_info_common_data dev_info;    /*!< Information of the device that sent the Mesh Proxy Service advertising packet. */
	uint16_t len;                                          /*!< Length in bytes of the valid data of proxy, including the type.
                                                            *   It is 9 for Network ID and 17 for the other identification types. */
	rtk_bt_mesh_stack_mesh_service_data_proxy_t proxy;     /*!< Service data of the advertising packet. */
} rtk_bt_mesh_stack_evt_dev_info_proxy_adv_t;

/**
 * @enum      rtk_bt_mesh_stack_hb_timer_state_t
 * @brief     BLE MESH heartbeat timer state definition.
 */
typedef enum {
	RTK_BT_MESH_STACK_HB_TIMER_STATE_START,    /*!< The heartbeat timer is started. */
	RTK_BT_MESH_STACK_HB_TIMER_STATE_STOP      /*!< The heartbeat timer is stopped. */
} rtk_bt_mesh_stack_hb_timer_state_t;

/**
 * @struct    rtk_bt_mesh_stack_hb_data_timer_state_t
 * @brief     BLE MESH heartbeat timer state event structure.
 */
typedef struct {
	rtk_bt_mesh_stack_hb_timer_state_t timer_state;    /*!< State of the heartbeat timer. */
	uint16_t period;                                   /*!< Period of the heartbeat timer. */
} rtk_bt_mesh_stack_hb_data_timer_state_t;

/**
 * @struct    rtk_bt_mesh_stack_hb_pub_count_update_t
 * @brief     BLE MESH heartbeat publication count update event structure.
 */
typedef struct {
	uint16_t count;    /*!< Heartbeat Publication Count state.
                        *   0x0000: Heartbeat messages are not being sent periodically.
                        *   0x0001 to 0xFFFE: Number of Heartbeat messages that remain to be sent.
                        *   0xFFFF: Heartbeat messages are being sent indefinitely. */
} rtk_bt_mesh_stack_hb_pub_count_update_t;

/**
 * @struct    rtk_bt_mesh_stack_hb_sub_period_update_t
 * @brief     BLE MESH heartbeat subscription period update event structure.
 */
typedef struct {
	uint16_t period;    /*!< Heartbeat Subscription Period state, in seconds.
                         *   0x0000: Heartbeat messages are not processed.
                         *   Others: Heartbeat messages are processed, the value is the remaining period. */
} rtk_bt_mesh_stack_hb_sub_period_update_t;

/**
 * @union     rtk_bt_mesh_stack_hb_pub_features_t
 * @brief     BLE MESH heartbeat Features field structure.
 */
typedef union {
	uint16_t raw;            /*!< Raw value of the Features field. */
	struct {
		uint16_t relay: 1;    /*!< Bit 0: Whether the Relay feature is in use.
                               *   0: The feature is not in use.
                               *   1: The feature is in use. */
		uint16_t proxy: 1;    /*!< Bit 1: Whether the Proxy feature is in use.
                               *   0: The feature is not in use.
                               *   1: The feature is in use. */
		uint16_t frnd: 1;     /*!< Bit 2: Whether the Friend feature is in use.
                               *   0: The feature is not in use.
                               *   1: The feature is in use. */
		uint16_t lpn: 1;      /*!< Bit 3: Whether the Low Power feature is in use.
                               *   0: The feature is not in use.
                               *   1: The feature is in use. */
		uint16_t rfu: 12;     /*!< Bits 4 to 15: Reserved for future use. */
	};
} _PACKED4_ rtk_bt_mesh_stack_hb_pub_features_t;

/**
 * @struct    rtk_bt_mesh_stack_hb_data_sub_receive_t
 * @brief     BLE MESH received heartbeat message event structure.
 */
typedef struct {
	uint16_t src;                                        /*!< Unicast address of the node that sent the Heartbeat message. */
	uint8_t init_ttl;                                    /*!< InitTTL field, the initial TTL used when sending the Heartbeat message.
                                                          *   Range: 0x00 to 0x7F. */
	rtk_bt_mesh_stack_hb_pub_features_t features;        /*!< Features field, the features that the sending node currently uses. */
	uint8_t ttl;                                         /*!< TTL field of the received Network PDU.
                                                          *   Hops = init_ttl - ttl + 1. */
} rtk_bt_mesh_stack_hb_data_sub_receive_t;

/**
 * @struct    rtk_bt_mesh_hb_sub_t
 * @brief     BLE MESH heartbeat subscription result structure.
 */
typedef struct {
	uint16_t src;         /*!< Heartbeat Subscription Source state, the source address of the Heartbeat messages that the node processes.
                           *   The unassigned address: Heartbeat messages are not being processed. */
	uint16_t dst;         /*!< Heartbeat Subscription Destination state, the destination address of the Heartbeat messages.
                           *   The unassigned address: Heartbeat messages are not being processed. */
	uint16_t period;      /*!< Heartbeat Subscription Period state, in seconds.
                           *   0x0000: Heartbeat messages are not being processed.
                           *   0x0001 to 0xFFFF: Remaining period for processing Heartbeat messages. */
	uint16_t count;       /*!< Heartbeat Subscription Count state.
                           *   0x0000 to 0xFFFE: Number of Heartbeat messages received.
                           *   0xFFFF: More than 0xFFFE messages have been received. */
	uint8_t min_hops;     /*!< Heartbeat Subscription Min Hops state, the minimum hops of the received Heartbeat messages.
                           *   Range: 0x00 to 0x7F. */
	uint8_t max_hops;     /*!< Heartbeat Subscription Max Hops state, the maximum hops of the received Heartbeat messages.
                           *   Range: 0x00 to 0x7F. */
} rtk_bt_mesh_hb_sub_t;

/**
 * @struct    rtk_bt_mesh_set_model_subscribe_t
 * @brief     BLE MESH set model subscribe structure.
 */
typedef struct {
	uint8_t element_index;    /*!< Index of the element of this node that contains the model. */
	uint32_t model_id;        /*!< Identifier of the model.
                               *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                               *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
	uint16_t sub_addr;        /*!< Group address or virtual address to add to the Subscription List of the model. */
} rtk_bt_mesh_set_model_subscribe_t;

/**
 * @enum      rtk_bt_mesh_prov_generic_cb_type_t
 * @brief     BLE MESH PB ADV link state type.
 */
typedef enum {
	RTK_BT_MESH_PB_GENERIC_CB_LINK_OPENED,         /*!< The PB-ADV link is opened. */
	RTK_BT_MESH_PB_GENERIC_CB_LINK_OPEN_FAILED,    /*!< The PB-ADV link fails to be opened. */
	RTK_BT_MESH_PB_GENERIC_CB_LINK_CLOSED,         /*!< The PB-ADV link is closed. */
	RTK_BT_MESH_PB_GENERIC_CB_MSG_ACKED,           /*!< A provisioning message is acknowledged. */
	RTK_BT_MESH_PB_GENERIC_CB_MSG,                 /*!< A provisioning message is received. */
} rtk_bt_mesh_prov_generic_cb_type_t;

/**
 * @struct    rtk_bt_mesh_stack_act_set_random_value_t
 * @brief     BLE MESH random value entered by the user for the Output OOB or Input OOB authentication.
 */
typedef struct {
	uint32_t random;    /*!< Number displayed by the remote device and entered by the user. */
} rtk_bt_mesh_stack_act_set_random_value_t;

/**
 * @struct    rtk_bt_mesh_stack_set_auth_value_for_static_oob
 * @brief     BLE MESH setting result of the Static OOB authentication value.
 */
typedef struct {
	bool status;         /*!< Whether the Static OOB authentication value has been set successfully.
                          *   - true: The value has been set.
                          *   - false: Setting the value failed. */
	uint8_t data[16];    /*!< Static OOB authentication value that has been set, 16 bytes.
                          *   It is only filled when status is true. */
} rtk_bt_mesh_stack_set_auth_value_for_static_oob;

/**
 * @struct    rtk_bt_mesh_stack_set_auth_value_for_oob_data
 * @brief     BLE MESH setting result of the Output OOB or Input OOB authentication value.
 */
typedef struct {
	bool status;        /*!< Whether the authentication value has been set successfully.
                         *   - true: The value has been set.
                         *   - false: Setting the value failed. */
	uint32_t random;    /*!< Random number used as the authentication value, which is output to the user to enter into the remote device.
                         *   It is only filled when status is true. */
} rtk_bt_mesh_stack_set_auth_value_for_oob_data;

/**
 * @enum      rtk_bt_mesh_stack_prov_cap_algorithm_t
 * @brief     BLE MESH provisioning algorithms supported by the unprovisioned device.
 */
typedef enum {
	RTK_BT_MESH_PROV_CAP_ALGO_FIPS_P256_ELLIPTIC_CURVE = BIT0,             /*!< The BTM_ECDH_P256_CMAC_AES128_AES_CCM algorithm is supported. */
#if defined(BT_MESH_ENABLE_EPA_PROVISION) && BT_MESH_ENABLE_EPA_PROVISION
	RTK_BT_MESH_PROV_CAP_ALGO_BTM_ECDH_P256_HMAC_SHA256_AES_CCM = BIT1,    /*!< The BTM_ECDH_P256_HMAC_SHA256_AES_CCM algorithm is supported. */
#endif
} rtk_bt_mesh_stack_prov_cap_algorithm_t;

/**
 * @enum      rtk_bt_mesh_stack_prov_cap_public_key_t
 * @brief     BLE MESH Public Key types supported by the unprovisioned device.
 */
typedef enum {
	RTK_BT_MESH_PROV_CAP_PUBLIC_KEY_OOB = BIT0    /*!< The Public Key OOB information is available. */
} rtk_bt_mesh_stack_prov_cap_public_key_t;

/**
 * @enum      rtk_bt_mesh_stack_prov_cap_static_oob_t
 * @brief     BLE MESH OOB types supported by the unprovisioned device.
 */
typedef enum {
	RTK_BT_MESH_PROV_CAP_NOT_SUPPORT_STATIC_OOB = 0,    /*!< No bit is set, the Static OOB information is not available. */
	RTK_BT_MESH_PROV_CAP_SUPPORT_STATIC_OOB = BIT0,     /*!< The Static OOB information is available. */
#if defined(BT_MESH_ENABLE_EPA_PROVISION) && BT_MESH_ENABLE_EPA_PROVISION
	RTK_BT_MESH_PROV_CAP_ONLY_OOB = BIT1,               /*!< Only OOB authenticated provisioning is supported.
                                                         *   If it is set, bit 0 of algorithm shall be 0, and bit 0 of static_oob is 1
                                                         *   or output_oob_size is not 0x00 or input_oob_size is not 0x00. */
#endif
} rtk_bt_mesh_stack_prov_cap_static_oob_t;

/**
 * @enum      rtk_bt_mesh_stack_prov_cap_output_oob_action_t
 * @brief     BLE MESH Output OOB actions supported by the unprovisioned device.
 */
typedef enum {
	RTK_BT_MESH_PROV_CAP_OUTPUT_OOB_ACTION_NOT_ENABLE = 0,               /*!< No bit is set, no Output OOB action is supported. */
	RTK_BT_MESH_PROV_CAP_OUTPUT_OOB_ACTION_BLINK = BIT0,                 /*!< Blink is supported. */
	RTK_BT_MESH_PROV_CAP_OUTPUT_OOB_ACTION_BEEP = BIT1,                  /*!< Beep is supported. */
	RTK_BT_MESH_PROV_CAP_OUTPUT_OOB_ACTION_VIBRATE = BIT2,               /*!< Vibrate is supported. */
	RTK_BT_MESH_PROV_CAP_OUTPUT_OOB_ACTION_OUTPUT_NUMERIC = BIT3,        /*!< Output Numeric is supported. */
	RTK_BT_MESH_PROV_CAP_OUTPUT_OOB_ACTION_OUTPUT_ALPHANUMERIC = BIT4    /*!< Output Alphanumeric is supported. */
} rtk_bt_mesh_stack_prov_cap_output_oob_action_t;

/**
 * @enum      rtk_bt_mesh_stack_prov_cap_input_oob_action_t
 * @brief     BLE MESH Input OOB actions supported by the unprovisioned device.
 */
typedef enum {
	RTK_BT_MESH_PROV_CAP_INPUT_OOB_ACTION_NOT_ENABLE = 0,                  /*!< No bit is set, no Input OOB action is supported. */
	RTK_BT_MESH_PROV_CAP_INPUT_OOB_ACTION_BIT_PUSH = BIT0,                 /*!< Push is supported. */
	RTK_BT_MESH_PROV_CAP_INPUT_OOB_ACTION_BIT_TWIST = BIT1,                /*!< Twist is supported. */
	RTK_BT_MESH_PROV_CAP_INPUT_OOB_ACTION_BIT_INPUT_NUMERIC = BIT2,        /*!< Input Numeric is supported. */
	RTK_BT_MESH_PROV_CAP_INPUT_OOB_ACTION_BIT_INPUT_ALPHANUMERIC = BIT3    /*!< Input Alphanumeric is supported. */
} rtk_bt_mesh_stack_prov_cap_input_oob_action_t;

/**
 * @struct    rtk_bt_mesh_stack_prov_capabilities_t
 * @brief     BLE MESH provisioning capabilities of the unprovisioned device.
 */
typedef struct {
	uint8_t element_num;           /*!< Number of elements supported by the unprovisioned device.
                                    *   0x00: Prohibited.
                                    *   0x01 to 0xFF: Number of elements of the unprovisioned device. */
	uint16_t algorithm;            /*!< Supported algorithms, a combination of the bits of @ref rtk_bt_mesh_stack_prov_cap_algorithm_t.
                                    *   The bits that are not defined there are RFU. */
	uint8_t public_key;            /*!< Supported Public Key types, a combination of the bits of @ref rtk_bt_mesh_stack_prov_cap_public_key_t.
                                    *   The bits that are not defined there are RFU. */
	uint8_t static_oob;            /*!< Supported OOB types, a combination of the bits of @ref rtk_bt_mesh_stack_prov_cap_static_oob_t.
                                    *   The bits that are not defined there are RFU. */
	uint8_t output_oob_size;       /*!< Maximum size of the Output OOB supported.
                                    *   0x00: The Output OOB is not supported.
                                    *   0x01 to 0x20: Maximum size of the Output OOB supported.
                                    *   0x21 to 0xFF: RFU. */
	uint16_t output_oob_action;    /*!< Supported Output OOB actions, a combination of the bits of @ref rtk_bt_mesh_stack_prov_cap_output_oob_action_t.
                                    *   The bits that are not defined there are RFU. */
	uint8_t input_oob_size;        /*!< Maximum size of the Input OOB supported.
                                    *   0x00: The Input OOB is not supported.
                                    *   0x01 to 0x20: Maximum size of the Input OOB supported.
                                    *   0x21 to 0xFF: RFU. */
	uint16_t input_oob_action;     /*!< Supported Input OOB actions, a combination of the bits of @ref rtk_bt_mesh_stack_prov_cap_input_oob_action_t.
                                    *   The bits that are not defined there are RFU. */
} _PACKED4_ rtk_bt_mesh_stack_prov_capabilities_t;

/**
 * @enum      rtk_bt_mesh_prov_start_algorithm_t
 * @brief     BLE MESH algorithm used by the provisioner for provisioning.
 */
typedef enum {
	RTK_BT_MESH_PROV_START_FIPS_P256_ELLIPTIC_CURVE,             /*!< The BTM_ECDH_P256_CMAC_AES128_AES_CCM algorithm is used. */
#if defined(BT_MESH_ENABLE_EPA_PROVISION) && BT_MESH_ENABLE_EPA_PROVISION
	RTK_BT_MESH_PROV_START_BTM_ECDH_P256_HMAC_SHA256_AES_CCM,    /*!< The BTM_ECDH_P256_HMAC_SHA256_AES_CCM algorithm is used. */
#endif
} rtk_bt_mesh_prov_start_algorithm_t;

/**
 * @enum      rtk_bt_mesh_prov_start_public_key_t
 * @brief     BLE MESH Public Key type used by the provisioner for provisioning.
 */
typedef enum {
	RTK_BT_MESH_PROV_START_NO_OOB_PUBLIC_KEY,    /*!< No OOB Public Key is used. */
	RTK_BT_MESH_PROV_START_OOB_PUBLIC_KEY        /*!< OOB Public Key is used. */
} rtk_bt_mesh_prov_start_public_key_t;

/**
 * @enum      rtk_bt_mesh_prov_auth_method_t
 * @brief     BLE MESH Authentication Method used by the provisioner for provisioning.
 */
typedef enum {
	RTK_BT_MESH_PROV_AUTH_METHOD_NO_OOB,        /*!< Authentication with No OOB, no OOB authentication is used. */
	RTK_BT_MESH_PROV_AUTH_METHOD_STATIC_OOB,    /*!< Authentication with Static OOB, Static OOB authentication is used. */
	RTK_BT_MESH_PROV_AUTH_METHOD_OUTPUT_OOB,    /*!< Authentication with Output OOB, Output OOB authentication is used. */
	RTK_BT_MESH_PROV_AUTH_METHOD_INPUT_OOB      /*!< Authentication with Input OOB, Input OOB authentication is used. */
} rtk_bt_mesh_prov_auth_method_t;

/**
 * @enum      rtk_bt_mesh_prov_start_output_oob_action_t
 * @brief     BLE MESH Output OOB Action selected by the provisioner for provisioning.
 */
typedef enum {
	RTK_BT_MESH_PROV_START_OUTPUT_OOB_ACTION_BLINK,                 /*!< Blink. */
	RTK_BT_MESH_PROV_START_OUTPUT_OOB_ACTION_BEEP,                  /*!< Beep. */
	RTK_BT_MESH_PROV_START_OUTPUT_OOB_ACTION_VIBRATE,               /*!< Vibrate. */
	RTK_BT_MESH_PROV_START_OUTPUT_OOB_ACTION_OUTPUT_NUMERIC,        /*!< Output Numeric. */
	RTK_BT_MESH_PROV_START_OUTPUT_OOB_ACTION_OUTPUT_ALPHANUMERIC    /*!< Output Alphanumeric. */
} rtk_bt_mesh_prov_start_output_oob_action_t;

/**
 * @enum      rtk_bt_mesh_prov_start_input_oob_action_t
 * @brief     BLE MESH Input OOB Action selected by the provisioner for provisioning.
 */
typedef enum {
	RTK_BT_MESH_PROV_START_INPUT_OOB_ACTION_PUSH,                 /*!< Push. */
	RTK_BT_MESH_PROV_START_INPUT_OOB_ACTION_TWIST,                /*!< Twist. */
	RTK_BT_MESH_PROV_START_INPUT_OOB_ACTION_INPUT_NUMERIC,        /*!< Input Numeric. */
	RTK_BT_MESH_PROV_START_INPUT_OOB_ACTION_INPUT_ALPHANUMERIC    /*!< Input Alphanumeric. */
} rtk_bt_mesh_prov_start_input_oob_action_t;

/**
 * @union     rtk_bt_mesh_prov_auth_action_t
 * @brief     BLE MESH Authentication Action used by the provisioner for provisioning.
 */
typedef union {
	uint8_t oob_action;                                              /*!< Authentication Action as a raw value.
                                                                      *   It shall be 0x00 for Authentication with No OOB and Authentication with Static OOB. */
	rtk_bt_mesh_prov_start_output_oob_action_t output_oob_action;    /*!< Output OOB Action, used for Authentication with Output OOB. */
	rtk_bt_mesh_prov_start_input_oob_action_t input_oob_action;      /*!< Input OOB Action, used for Authentication with Input OOB. */
} _PACKED4_ rtk_bt_mesh_prov_auth_action_t;

/**
 * @union     rtk_bt_mesh_prov_auth_size_t
 * @brief     BLE MESH Authentication Size used by the provisioner for provisioning.
 */
typedef union {
	uint8_t oob_size;           /*!< Authentication Size as a raw value.
                                 *   It shall be 0x00 for Authentication with No OOB and Authentication with Static OOB. */
	uint8_t output_oob_size;    /*!< Size of the Output OOB used, for Authentication with Output OOB.
                                 *   0x00: Prohibited.
                                 *   0x01 to 0x20: The Output OOB size to be used.
                                 *   0x21 to 0xFF: RFU. */
	uint8_t input_oob_size;     /*!< Size of the Input OOB used, for Authentication with Input OOB.
                                 *   0x00: Prohibited.
                                 *   0x01 to 0x20: The Input OOB size to be used.
                                 *   0x21 to 0xFF: RFU. */
} _PACKED4_ rtk_bt_mesh_prov_auth_size_t;

/**
 * @struct    rtk_bt_mesh_stack_prov_start_t
 * @brief     BLE MESH provisioning method selected by the provisioner.
 */
typedef struct {
	rtk_bt_mesh_prov_start_algorithm_t algorithm;      /*!< Algorithm used for provisioning.
                                                        *   0x00: BTM_ECDH_P256_CMAC_AES128_AES_CCM.
                                                        *   0x01: BTM_ECDH_P256_HMAC_SHA256_AES_CCM.
                                                        *   0x02 to 0xFF: RFU. */
	rtk_bt_mesh_prov_start_public_key_t public_key;    /*!< Public Key type used for provisioning.
                                                        *   0x00: No OOB Public Key is used.
                                                        *   0x01: OOB Public Key is used.
                                                        *   0x02 to 0xFF: RFU. */
	rtk_bt_mesh_prov_auth_method_t auth_method;        /*!< Authentication Method used for provisioning.
                                                        *   0x00: Authentication with No OOB.
                                                        *   0x01: Authentication with Static OOB.
                                                        *   0x02: Authentication with Output OOB.
                                                        *   0x03: Authentication with Input OOB.
                                                        *   0x04 to 0xFF: Prohibited. */
	rtk_bt_mesh_prov_auth_action_t auth_action;        /*!< Authentication Action, the Output OOB Action for Authentication with Output OOB,
                                                        *   the Input OOB Action for Authentication with Input OOB and 0x00 otherwise. */
	rtk_bt_mesh_prov_auth_size_t auth_size;            /*!< Authentication Size, the size of the Output OOB for Authentication with Output OOB,
                                                        *   the size of the Input OOB for Authentication with Input OOB and 0x00 otherwise. */
} _PACKED4_ rtk_bt_mesh_stack_prov_start_t;

/**
 * @struct    rtk_bt_mesh_stack_set_retrans_param_t
 * @brief     BLE MESH set retransmit parameter.
 */
typedef struct {
	uint8_t ttl;                    /*!< Default TTL of the node, used when sending messages.
                                     *   0x00 or 0x02 to 0x7F: Default TTL.
                                     *   0x01 or 0x80 to 0xFF: Prohibited. */
	uint8_t relay_retrans_count;    /*!< Relay Retransmit Count, controls the number of retransmissions of each relayed Network PDU.
                                     *   Number of transmissions of a relayed Network PDU = count + 1.
                                     *   Range: 0x00 to 0x07. */
	uint8_t net_retrans_count;      /*!< Network Transmit Count, controls the number of transmissions of each Network PDU originating from the node.
                                     *   Number of transmissions = count + 1.
                                     *   Range: 0x00 to 0x07. */
	uint8_t trans_retrans_count;    /*!< Number of retransmissions of each segment of a segmented message.
                                     *   Range: 0x00 to 0x0A. */
} rtk_bt_mesh_stack_set_retrans_param_t;

/**
 * @struct    rtk_bt_mesh_stack_act_set_service_adv_t
 * @brief     BLE MESH switch the connectable mesh service ADV on/off.
 */
typedef struct {
	uint8_t enable;    /*!< Whether to advertise the mesh service on the GATT bearer.
                        *   0: Stop the advertising.
                        *   1: Start the advertising. */
} rtk_bt_mesh_stack_act_set_service_adv_t;

#if defined(RTK_BLE_MESH_BASED_ON_CODED_PHY) && RTK_BLE_MESH_BASED_ON_CODED_PHY
/**
 * @enum      rtk_bt_mesh_stack_act_set_tx_phy_t
 * @brief     BLE MESH set tx phy data structure when use BLE 5.0 AE, the value is passed to the gap scheduler as the advertising type of mesh packets.
 */
typedef enum {
	RTK_BT_MESH_GAP_SCHED_AE_ADV_TYPE_LEGACY_ON_1M =    0x01,    /*!< Legacy advertising on the LE 1M PHY. */
	RTK_BT_MESH_GAP_SCHED_AE_ADV_TYPE_LEGACY_ON_S2 =    0x02,    /*!< Legacy advertising on the LE Coded PHY with S=2. */
	RTK_BT_MESH_GAP_SCHED_AE_ADV_TYPE_LEGACY_ON_S8 =    0x04,    /*!< Legacy advertising on the LE Coded PHY with S=8. */
	RTK_BT_MESH_GAP_SCHED_AE_ADV_TYPE_EXTEND_ON_1M_1M = 0x08,    /*!< Extended advertising with the primary PHY of LE 1M and the secondary PHY of LE 1M. */
	RTK_BT_MESH_GAP_SCHED_AE_ADV_TYPE_EXTEND_ON_1M_2M = 0x10,    /*!< Extended advertising with the primary PHY of LE 1M and the secondary PHY of LE 2M. */
} rtk_bt_mesh_stack_act_set_tx_phy_t;
#endif

/**
 * @enum      rtk_bt_mesh_bearer_field_t
 * @brief     BLE MESH bearer field definition, a bit field identifying a bearer or a combination of bearers.
 */
enum {
	RTK_BT_MESH_BEARER_FIELD_UNASSIGNED = 0,         /*!< No bearer. */
	RTK_BT_MESH_BEARER_FIELD_ADV = BIT0,             /*!< The advertising bearer. */
	RTK_BT_MESH_BEARER_FIELD_GATT = BIT1,            /*!< The GATT bearer. */
	RTK_BT_MESH_BEARER_FIELD_OTHER = BIT14,          /*!< Another bearer, not the advertising bearer or the GATT bearer. */
	RTK_BT_MESH_BEARER_FIELD_LOOPBACK = BIT15,       /*!< The loopback bearer. */
	RTK_BT_MESH_BEARER_FIELD_ALL = 0xFFFF,           /*!< All the bearers. */
} _SHORT_ENUM_;
typedef uint16_t rtk_bt_mesh_bearer_field_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  ble_mesh_common BT LE Mesh Common APIs
 * @brief     BT LE Mesh Common related function APIs
 * @ingroup   ble_mesh_api_group
 * @{
 */

/**
 * @brief     Set the network parameters of this node: the unicast address, the NetKey, the AppKey and the DevKey.
 *            A zero unicast address or an all-zero key in the parameter is replaced by a default value.
 * @param[in] init_setting: Network parameters to set.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_prov_param_set(rtk_bt_mesh_stack_act_set_prov_param_t *init_setting);

#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT

/**
 * @brief     Open a PB-ADV link with an unprovisioned device.
 *            The state of the link is reported asynchronously via event @ref RTK_BT_MESH_STACK_EVT_PB_ADV_LINK_STATE.
 * @param[in] pbadvcon: Unprovisioned device to open the PB-ADV link with.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_prov_pb_adv_con(rtk_bt_mesh_stack_act_pb_adv_con_t *pbadvcon);

/**
 * @brief     Close the PB-ADV link.
 *            The state of the link is reported asynchronously via event @ref RTK_BT_MESH_STACK_EVT_PB_ADV_LINK_STATE.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_prov_pb_adv_discon(void);

/**
 * @brief     Establish a BLE connection to a device that provides the Mesh Provisioning Service or the Mesh Proxy Service.
 * @param[in] pgatt_con: Connection parameters.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_prov_pb_gatt_con(rtk_bt_mesh_stack_act_pb_gatt_con_t *pgatt_con);

/**
 * @brief     Disconnect the BLE connection established by @ref rtk_bt_mesh_stack_prov_pb_gatt_con.
 * @param[in] pgatt_discon: Connection to disconnect.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_prov_pb_gatt_discon(rtk_bt_mesh_stack_act_pb_gatt_discon_t *pgatt_discon);

/**
 * @brief     Start provisioning the device on the opened link by sending a Provisioning Invite PDU.
 * @param[in] start_prov: Provisioning Invite parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_start_provisioning(rtk_bt_mesh_stack_act_start_prov_t *start_prov);

/**
 * @brief     Choose the provisioning method that the provisioner expects to use for provisioning.
 * @param[in] method: Provisioning method, as in the Provisioning Start PDU.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Call it before @ref rtk_bt_mesh_stack_start_provisioning if the default method is not wanted.
 */
uint16_t rtk_bt_mesh_stack_method_choose_for_provisioning(rtk_bt_mesh_stack_prov_start_t *method);

/**
 * @brief     Discover the Mesh Provisioning Service on a BLE connection.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_STACK_EVT_PROV_SERVICE_DIS_RESULT.
 * @param[in] prov_dis: Connection to discover the service on.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_prov_service_discovery(rtk_bt_mesh_stack_act_prov_dis_t *prov_dis);

/**
 * @brief     Enable or disable the notification of the Mesh Provisioning Data Out characteristic.
 * @param[in] prov_notify: Notification setting.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_prov_service_set_notify(rtk_bt_mesh_stack_act_prov_set_notify_t *prov_notify);

/**
 * @brief     Discover the Mesh Proxy Service on a BLE connection.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_STACK_EVT_PROXY_SERVICE_DIS_RESULT.
 * @param[in] proxy_dis: Connection to discover the service on.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_proxy_service_discovery(rtk_bt_mesh_stack_act_proxy_dis_t *proxy_dis);

/**
 * @brief     Enable or disable the notification of the Mesh Proxy Data Out characteristic.
 * @param[in] proxy_notify: Notification setting.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_proxy_service_set_notify(rtk_bt_mesh_stack_act_proxy_set_notify_t *proxy_notify);

#endif /* RTK_BLE_MESH_PROVISIONER_SUPPORT */

#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT

/**
 * @brief     Initialize the Low Power node feature.
 * @param[in] lpn_init: Low Power node initialization parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_lpn_init(rtk_bt_mesh_stack_act_lpn_init_t *lpn_init);

/**
 * @brief     Request a friendship with a Friend node by sending a Friend Request message.
 *            The result of the friendship establishment is reported asynchronously via event @ref RTK_BT_MESH_STACK_EVT_LPN_CB.
 * @param[in] lpn_req: Friend Request parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref RTK_BT_MESH_LPN_REQ_REASON_SUCCESS: The request is accepted.
 *            - Others: The request is rejected, see @ref rtk_bt_mesh_stack_lpn_req_result_type and @ref rtk_bt_err_common.
 */
uint16_t rtk_bt_mesh_stack_lpn_req(rtk_bt_mesh_stack_act_lpn_req_t *lpn_req);

/**
 * @brief     Add an address to or remove an address from the Friend Subscription List of the Friend node.
 * @param[in] lpn_sub: Address and the operation.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_lpn_sub(rtk_bt_mesh_stack_act_lpn_sub_t *lpn_sub);

/**
 * @brief     Clear a friendship.
 *            The end of the friendship is reported asynchronously via event @ref RTK_BT_MESH_STACK_EVT_LPN_CB.
 * @param[in] lpn_clear: Friendship to clear.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_lpn_clear(rtk_bt_mesh_stack_act_lpn_clear_t *lpn_clear);

/**
 * @brief     Deinitialize the Low Power node feature.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_lpn_deinit(void);

/**
 * @brief     Switch the connectable advertising of the mesh service on or off.
 * @param[in] param: Advertising switch.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - An unprovisioned device switches the PB-GATT advertising, a provisioned node switches the Mesh Proxy advertising.
 */
uint16_t rtk_bt_mesh_stack_set_service_adv(rtk_bt_mesh_stack_act_set_service_adv_t *param);

#endif /* RTK_BLE_MESH_DEVICE_SUPPORT */

/**
 * @brief     Reset this node: clear the mesh configuration, including the provisioning data.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_node_reset(void);

/**
 * @brief     Switch the reporting of the information of the received unprovisioned device beacons, Secure Network Beacons,
 *            provisioning advertisements and Mesh Proxy advertisements.
 * @param[in] p_data: Switch setting.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_set_device_info_display(rtk_bt_mesh_stack_act_device_info_set_t *p_data);

/**
 * @brief     Get the information of this node: the mesh state, the device UUID, the Bluetooth address, the keys, the elements and the models.
 *            The information is reported asynchronously via event @ref RTK_BT_MESH_STACK_EVT_LIST_INFO.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_user_list_info(void);

/**
 * @brief     Send one advertising packet with the given advertising data, advertising type and own address type.
 * @param[in] adv_param: Advertising data and type.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_send_one_shot_adv(rtk_bt_mesh_stack_act_send_adv_t *adv_param);

/**
 * @brief     Set the authentication value used by the Output OOB or Input OOB provisioning method.
 * @param[in] value: Authentication value.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_set_random_value_for_authentication(rtk_bt_mesh_stack_act_set_random_value_t *value);

/**
 * @brief     Get the current Heartbeat Subscription state of this node.
 * @param[out] hb_sub: Heartbeat Subscription state read.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_get_heartbeat_subscribe_result(rtk_bt_mesh_hb_sub_t *hb_sub);

/**
 * @brief     Add a subscription address to a model of this node.
 * @param[in] model_sub: Model and the address to subscribe.
 *                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_set_model_subscribe(rtk_bt_mesh_set_model_subscribe_t *model_sub);

/**
 * @brief     Initialize the Friend node feature.
 * @param[in] friend_init: Friend node initialization parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_fn_init(rtk_bt_mesh_stack_act_fn_init_t *friend_init);

/**
 * @brief     Deinitialize the Friend node feature.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_fn_deinit(void);

/**
 * @brief     Set the retransmission parameters of this node: the TTL, the network, relay and transport retransmit counts.
 *            The values in effect after the setting are reported asynchronously via event @ref RTK_BT_MESH_STACK_EVT_RETRANS_PARAM_SETTING_RESULT.
 * @param[in] param: Retransmission parameters.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_retrans_param_set(rtk_bt_mesh_stack_set_retrans_param_t *param);

#if defined(RTK_BLE_MESH_BASED_ON_CODED_PHY) && RTK_BLE_MESH_BASED_ON_CODED_PHY
/**
 * @brief     Set the advertising type, and with it the PHY, used for the Mesh advertising.
 * @param[in] tx_phy: Advertising type.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_stack_set_tx_phy(rtk_bt_mesh_stack_act_set_tx_phy_t *tx_phy);
#endif

/**
 * @}
 */

#endif /* __RTK_BT_MESH_COMMON_H__ */
