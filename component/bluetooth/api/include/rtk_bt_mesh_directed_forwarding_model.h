/**
 * @file      rtk_bt_mesh_directed_forwarding_model.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH directed forwarding model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_DIRECTED_FORWARDING_MODEL_H__
#define __RTK_BT_MESH_DIRECTED_FORWARDING_MODEL_H__

#include <stdint.h>
#include <rtk_bt_mesh_def.h>

/**
 * @enum      rtk_bt_mesh_df_client_act_t
 * @brief     BLE MESH directed forwarding client model act definition.
 */
typedef enum {
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_GET = 1,                 /*!< Send a Directed Control Get message to the server. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_SET,                     /*!< Send a Directed Control Set message to the server. */
	RTK_BT_MESH_DF_MODEL_PATH_METRIC_GET,                          /*!< Send a Path Metric Get message to the server. */
	RTK_BT_MESH_DF_MODEL_PATH_METRIC_SET,                          /*!< Send a Path Metric Set message to the server. */
	RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_GET,         /*!< Send a Discovery Table Capabilities Get message to the server. */
	RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_SET,         /*!< Send a Discovery Table Capabilities Set message to the server. */
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ADD,                     /*!< Send a Forwarding Table Add message to the server. */
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DELETE,                  /*!< Send a Forwarding Table Delete message to the server. */
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_ADD,          /*!< Send a Forwarding Table Dependents Add message to the server. */
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_DELETE,       /*!< Send a Forwarding Table Dependents Delete message to the server. */
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_GET,          /*!< Send a Forwarding Table Dependents Get message to the server. */
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_COUNT_GET,       /*!< Send a Forwarding Table Entries Count Get message to the server. */
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_GET,             /*!< Send a Forwarding Table Entries Get message to the server. */
	RTK_BT_MESH_DF_MODEL_WANTED_LANES_GET,                         /*!< Send a Wanted Lanes Get message to the server. */
	RTK_BT_MESH_DF_MODEL_WANTED_LANES_SET,                         /*!< Send a Wanted Lanes Set message to the server. */
	RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_GET,                         /*!< Send a Two Way Path Get message to the server. */
	RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_SET,                         /*!< Send a Two Way Path Set message to the server. */
	RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_GET,                   /*!< Send a Path Echo Interval Get message to the server. */
	RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_SET,                   /*!< Send a Path Echo Interval Set message to the server. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_GET,            /*!< Send a Directed Network Transmit Get message to the server. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_SET,            /*!< Send a Directed Network Transmit Set message to the server. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_GET,            /*!< Send a Directed Relay Retransmit Get message to the server. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_SET,            /*!< Send a Directed Relay Retransmit Set message to the server. */
	RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_GET,                       /*!< Send a RSSI Threshold Get message to the server. */
	RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_SET,                       /*!< Send a RSSI Threshold Set message to the server. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_PATHS_GET,                       /*!< Send a Directed Paths Get message to the server. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_GET,              /*!< Send a Directed Publish Policy Get message to the server. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_SET,              /*!< Send a Directed Publish Policy Set message to the server. */
	RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_GET,        /*!< Send a Path Discovery Timing Control Get message to the server. */
	RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_SET,        /*!< Send a Path Discovery Timing Control Set message to the server. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_GET,    /*!< Send a Directed Control Network Transmit Get message to the server. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_SET,    /*!< Send a Directed Control Network Transmit Set message to the server. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_GET,    /*!< Send a Directed Control Relay Retransmit Get message to the server. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_SET,    /*!< Send a Directed Control Relay Retransmit Set message to the server. */
	RTK_BT_MESH_DF_MODEL_ACT_MAX,                                  /*!< Reserved. */
} rtk_bt_mesh_df_client_act_t;

/**
 * @enum      rtk_bt_mesh_df_common_act_t
 * @brief     BLE MESH directed forwarding common act definition.
 */
typedef enum {
	RTK_BT_MESH_DF_PATH_DISCOVERY = 1,            /*!< Start a path discovery on this node. */
	RTK_BT_MESH_DF_PATH_SOLICITATION,             /*!< Start a path solicitation on this node. */
	RTK_BT_MESH_DF_PATH_DEPENDENTS_UPDATE,        /*!< Update the dependent nodes of a path endpoint on this node. */
	RTK_BT_MESH_DF_DIRECTED_CONTROL_STATE_GET,    /*!< Get the Directed Control state of this node. */
	RTK_BT_MESH_DF_DIRECTED_CONTROL_STATE_SET,    /*!< Set the Directed Control state of this node. */
	RTK_BT_MESH_DF_PATH_METRIC_STATE_GET,         /*!< Get the Path Metric state of this node. */
	RTK_BT_MESH_DF_PATH_METRIC_STATE_SET,         /*!< Set the Path Metric state of this node. */
	RTK_BT_MESH_DF_DISCOVERY_TABLE_CAP_GET,       /*!< Get the Discovery Table Capabilities state of this node. */
	RTK_BT_MESH_DF_DISCOVERY_TABLE_CAP_SET,       /*!< Set the Discovery Table Capabilities state of this node. */
	RTK_BT_MESH_DF_WANTED_LANES_GET,              /*!< Get the Wanted Lanes state of this node. */
	RTK_BT_MESH_DF_WANTED_LANES_SET,              /*!< Set the Wanted Lanes state of this node. */
	RTK_BT_MESH_DF_TWO_WAY_PATH_GET,              /*!< Get the Two Way Path state of this node. */
	RTK_BT_MESH_DF_TWO_WAY_PATH_SET,              /*!< Set the Two Way Path state of this node. */
	RTK_BT_MESH_DF_PATH_ECHO_INTERVAL_GET,        /*!< Get the Unicast Echo Interval and Multicast Echo Interval states of this node. */
	RTK_BT_MESH_DF_PATH_ECHO_INTERVAL_SET,        /*!< Set the Unicast Echo Interval and Multicast Echo Interval states of this node. */
	RTK_BT_MESH_DF_PUBLISH_POLICY_SET,            /*!< Set the Directed Publish Policy state of a model on this node. */
	RTK_BT_MESH_DF_PARAMETER_SET,                 /*!< Set the directed forwarding related parameters of the mesh node, which are not states of the Directed Forwarding
                                                   *   Configuration Server. */
	RTK_BT_MESH_DF_COMMON_ACT_MAX,                /*!< Reserved. */
} rtk_bt_mesh_df_common_act_t;

/**
 * @enum      rtk_bt_mesh_df_client_evt_t
 * @brief     BLE MESH directed forwarding client model event definition.
 */
typedef enum {
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_STAT = 1,                 /*!< Indicate a Directed Control Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_directed_control_status_t. */
	RTK_BT_MESH_DF_MODEL_PATH_METRIC_STAT,                          /*!< Indicate a Path Metric Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_path_metric_status_t. */
	RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_STAT,         /*!< Indicate a Discovery Table Capabilities Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_discovery_table_capabilities_status_t. */
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_STAT,                     /*!< Indicate a Forwarding Table Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_forwarding_table_status_t. */
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_STAT,          /*!< Indicate a Forwarding Table Dependents Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_forwarding_table_dependents_status_t. */
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_GET_STAT,      /*!< Indicate a Forwarding Table Dependents Get Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t.
                                                                     *   The list of unicast address ranges of the dependent nodes
                                                                     *   is located right after the structure in the event msg. */
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_COUNT_STAT,       /*!< Indicate a Forwarding Table Entries Count Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_forwarding_table_entries_count_status_t. */
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_STAT,             /*!< Indicate a Forwarding Table Entries Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_forwarding_table_entries_status_for_call_t.
                                                                     *   The Forwarding Table entries are located right after the structure in the event msg. */
	RTK_BT_MESH_DF_MODEL_WANTED_LANES_STAT,                         /*!< Indicate a Wanted Lanes Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_wanted_lanes_status_t. */
	RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_STAT,                         /*!< Indicate a Two Way Path Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_two_way_path_status_t. */
	RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_STAT,                   /*!< Indicate a Path Echo Interval Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_path_echo_interval_status_t. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_STAT,            /*!< Indicate a Directed Network Transmit Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_directed_network_transmit_status_t. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_STAT,            /*!< Indicate a Directed Relay Retransmit Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_directed_relay_retransmit_status_t. */
	RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_STAT,                       /*!< Indicate a RSSI Threshold Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_rssi_threshold_status_t. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_PATHS_STAT,                       /*!< Indicate a Directed Paths Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_directed_paths_status_t. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_STAT,              /*!< Indicate a Directed Publish Policy Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_directed_publish_policy_status_t. */
	RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_STAT,        /*!< Indicate a Path Discovery Timing Control Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_path_discovery_timing_control_status_t. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_STAT,    /*!< Indicate a Directed Control Network Transmit Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_directed_control_network_transmit_status_t. */
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_STAT,    /*!< Indicate a Directed Control Relay Retransmit Status message received from the server, with msg
                                                                     *   @ref rtk_bt_mesh_directed_control_relay_retransmit_status_t. */
	RTK_BT_MESH_DF_MODEL_EVT_MAX,                                   /*!< Reserved. */
} rtk_bt_mesh_df_client_evt_t;

/**
 * @enum      rtk_bt_mesh_df_path_action_type_t
 * @brief     BLE MESH directed forwarding path action type definition.
 */
typedef enum {
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_DISCOVERING,             /*!< The path is being discovered. */
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_DISCOVERY_FAIL,          /*!< The path discovery failed. */
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_ESTABLISHED,             /*!< The path is established. */
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_PATH_RELEASE,            /*!< The path is released. */
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_NEW_LANE_DISCOVERING,    /*!< A new lane of the path is being discovered. */
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_NEW_LANE_ESTABLISHED,    /*!< A new lane of the path is established. */
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_NEW_LANE_FAIL,           /*!< The discovery of a new lane of the path failed. */
} rtk_bt_mesh_df_path_action_type_t;

/**
 * @struct    rtk_bt_mesh_df_path_action_t
 * @brief     BLE MESH directed forwarding path action definition.
 */
typedef struct {
	rtk_bt_mesh_df_path_action_type_t action_type;    /*!< Type of the path action. */
	uint8_t master_key_index;                         /*!< Index of the NetKey of the subnet that the path belongs to.
                                                       *   Range: 0x0000 to 0x0FFF. */
	uint16_t path_src;                                /*!< Primary element address of the Path Origin. */
	uint8_t path_src_sec_elem_num;                    /*!< Number of secondary elements of the Path Origin. */
	uint16_t dp_src;                                  /*!< Primary element address of a dependent node of the Path Origin. */
	uint8_t dp_src_sec_elem_num;                      /*!< Number of secondary elements of the dependent node of the Path Origin. */
	uint16_t path_dst;                                /*!< Destination address of the path, the primary element address of the Path Target
                                                       *   or a group address or a virtual address. */
	uint8_t path_dst_sec_elem_num;                    /*!< Number of secondary elements of the Path Target. */
	uint16_t dp_dst;                                  /*!< Primary element address of a dependent node of the Path Target. */
	uint8_t dp_dst_sec_elem_num;                      /*!< Number of secondary elements of the dependent node of the Path Target. */
	uint8_t forwarding_num;                           /*!< Forwarding number of the path. */
} rtk_bt_mesh_df_path_action_t;

/**
 * @name      Forwarding Table Entries Get Filter Mask Bits
 * @brief     Bits of the Filter Mask of a Forwarding Table Entries Get message.
 * @{
 */
#define RTK_BT_MESH_FORWARDING_TABLE_FILTER_MASK_FIXED_PATHS                  0x01    /*!< Filter Mask bit 0 of a Forwarding Table Entries Get message,
                                                                                       *   return the fixed path entries. */
#define RTK_BT_MESH_FORWARDING_TABLE_FILTER_MASK_NON_FIXED_PATHS              0x02    /*!< Filter Mask bit 1 of a Forwarding Table Entries Get message,
                                                                                       *   return the non-fixed path entries. */
#define RTK_BT_MESH_FORWARDING_TABLE_FILTER_MASK_PATH_ORIGIN_MATCH            0x04    /*!< Filter Mask bit 2 of a Forwarding Table Entries Get message,
                                                                                       *   return the path entries with a matching Path Origin. */
#define RTK_BT_MESH_FORWARDING_TABLE_FILTER_MASK_DST_MATCH                    0x08    /*!< Filter Mask bit 3 of a Forwarding Table Entries Get message,
                                                                                       *   return the path entries with a matching Destination. */
/** @} */

/**
 * @name      Forwarding Table Dependents Get Dependents List Mask Bits
 * @brief     Bits of the Dependents List Mask of a Forwarding Table Dependents Get message.
 * @{
 */
#define RTK_BT_MESH_FORWARDING_TABLE_DEPENDENTS_LIST_MASK_PATH_ORIGIN         0x01    /*!< Dependents List Mask bit 0 of a Forwarding Table Dependents Get message,
                                                                                       *   return the dependent nodes of the Path Origin. */
#define RTK_BT_MESH_FORWARDING_TABLE_DEPENDENTS_LIST_MASK_PATH_TARGET         0x02    /*!< Dependents List Mask bit 1 of a Forwarding Table Dependents Get message,
                                                                                       *   return the dependent nodes of the Path Target. */
/** @} */

/**
 * @struct    rtk_bt_mesh_stack_evt_df_t
 * @brief     BLE MESH df action evt structure.
 */
typedef struct {
	uint8_t type;                                /*!< Type of the directed forwarding event.
                                                  *   0x00: A path action, see path_action. */
	rtk_bt_mesh_df_path_action_t path_action;    /*!< Path action reported when type is 0x00. */
} rtk_bt_mesh_stack_evt_df_t;

/**
 * @struct    rtk_bt_mesh_directed_control_t
 * @brief     BLE MESH directed control structure.
 */
typedef struct {
	uint8_t directed_forwarding;                    /*!< Directed Forwarding state of the subnet.
                                                     *   0x00: Directed forwarding is disabled.
                                                     *   0x01: Directed forwarding is enabled.
                                                     *   0x02 to 0xFF: Prohibited. */
	uint8_t directed_relay;                         /*!< Directed Relay state of the subnet.
                                                     *   0x00: Directed relay is disabled.
                                                     *   0x01: Directed relay is enabled.
                                                     *   0x02 to 0xFF: Prohibited. */
	uint8_t directed_proxy;                         /*!< Directed Proxy state of the subnet.
                                                     *   0x00: Directed proxy is disabled.
                                                     *   0x01: Directed proxy is enabled.
                                                     *   0x02: Directed proxy is not supported.
                                                     *   0x03 to 0xFF: Prohibited. */
	uint8_t directed_proxy_use_directed_default;    /*!< Directed Proxy Use Directed Default state of the subnet.
                                                     *   0x00: Use_Directed is set to Disabled when a connection is created.
                                                     *   0x01: Use_Directed is set to Enabled when a connection is created.
                                                     *   0x02: Directed proxy is not supported or is disabled.
                                                     *   0x03 to 0xFF: Prohibited. */
	uint8_t directed_friend;                        /*!< Directed Friend state of the subnet.
                                                     *   0x00: Directed friend is disabled.
                                                     *   0x01: Directed friend is enabled.
                                                     *   0x02: Directed friend is not supported.
                                                     *   0x03 to 0xFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_directed_control_t;

/**
 * @struct    rtk_bt_mesh_path_metric_t
 * @brief     BLE MESH path metric structure.
 */
typedef struct {
	uint8_t metric_type : 3;    /*!< Path Metric Type state, 3 bits.
                                 *   0b000: Node Count, the node uses the Node Count metric.
                                 *   0b001 to 0b111: Reserved for Future Use. */
	uint8_t lifetime : 2;       /*!< Path Lifetime state, 2 bits.
                                 *   0b00: Path lifetime is 12 minutes.
                                 *   0b01: Path lifetime is 2 hours.
                                 *   0b10: Path lifetime is 24 hours.
                                 *   0b11: Path lifetime is 10 days. */
	uint8_t prohibited : 3;     /*!< Prohibited, 3 bits. */
} _PACKED4_ rtk_bt_mesh_path_metric_t;

/**
 * @struct    rtk_bt_mesh_discovery_table_capabilities_t
 * @brief     BLE MESH discovery table capabilities structure.
 */
typedef struct {
	uint8_t max_concurrent_init;                  /*!< Max Concurrent Init state, the maximum number of concurrent Directed Forwarding Initialization procedures.
                                                   *   0x00: Prohibited.
                                                   *   It shall be at most max_discovery_table_entries_count. */
	uint8_t max_discovery_table_entries_count;    /*!< Max Discovery Table Entries Count state, the maximum number of Discovery Table entries supported by the node.
                                                   *   It is at least 2. */
} _PACKED4_ rtk_bt_mesh_discovery_table_capabilities_t;

/**
 * @struct    rtk_bt_mesh_forwarding_table_entries_count_t
 * @brief     BLE MESH forwarding table entries count structure.
 */
typedef struct {
	uint16_t forwarding_table_update_id;      /*!< Forwarding Table Update Identifier state, it changes after each change to the Forwarding Table Entries state. */
	uint16_t fixed_path_entries_count;        /*!< Number of fixed path entries in the Forwarding Table. */
	uint16_t non_fixed_path_entries_count;    /*!< Number of non-fixed path entries in the Forwarding Table. */
} _PACKED4_ rtk_bt_mesh_forwarding_table_entries_count_t;

/**
 * @struct    rtk_bt_mesh_directed_paths_t
 * @brief     BLE MESH directed paths structure.
 */
typedef struct {
	uint16_t directed_node_paths;      /*!< Directed Node Paths state, the minimum number of paths the node supports as a Path Origin or a Path Target.
                                        *   It is at least 20. */
	uint16_t directed_relay_paths;     /*!< Directed Relay Paths state, the minimum number of paths the node supports as an intermediate Directed Relay node.
                                        *   It is at least 20. */
	uint16_t directed_proxy_paths;     /*!< Directed Proxy Paths state, the minimum number of paths the node supports as a Directed Proxy node.
                                        *   It is at least 20 if directed proxy is supported, otherwise 0. */
	uint16_t directed_friend_paths;    /*!< Directed Friend Paths state, the minimum number of paths the node supports as a Directed Friend node.
                                        *   It is at least 20 if directed friend is supported, otherwise 0. */
} _PACKED4_ rtk_bt_mesh_directed_paths_t;

/**
 * @struct    rtk_bt_mesh_path_discovery_timing_control_t
 * @brief     BLE MESH path discovery timing control structure.
 */
typedef struct {
	uint16_t path_monitoring_interval;            /*!< Path Monitoring Interval state, in seconds. */
	uint16_t path_discovery_retry_interval;       /*!< Path Discovery Retry Interval state, in seconds. */
	uint8_t path_discovery_interval : 1;          /*!< Path Discovery Interval state, 1 bit.
                                                   *   0b0: Path discovery interval is 5 seconds.
                                                   *   0b1: Path discovery interval is 30 seconds. */
	uint8_t lane_discovery_guard_interval : 1;    /*!< Lane Discovery Guard Interval state, 1 bit.
                                                   *   0b0: Lane discovery guard interval is 2 seconds.
                                                   *   0b1: Lane discovery guard interval is 10 seconds. */
	uint8_t prohibited : 6;                       /*!< Prohibited, 6 bits. */
} _PACKED4_ rtk_bt_mesh_path_discovery_timing_control_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_directed_control_get_t
 * @brief     BLE MESH directed control get message structure.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;    /*!< Index of the NetKey of the subnet that the Directed Control state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_directed_control_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_directed_control_set_t
 * @brief     BLE MESH directed control set message structure.
 */
typedef struct {
	uint16_t dst;                                   /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;                         /*!< Index of the NetKey of the subnet the message is sent through.
                                                     *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;                      /*!< Index of the NetKey of the subnet that the Directed Control state belongs to.
                                                     *   Range: 0x0000 to 0x0FFF. */
	uint8_t directed_forwarding;                    /*!< New Directed Forwarding state.
                                                     *   0x00: Disable directed forwarding.
                                                     *   0x01: Enable directed forwarding.
                                                     *   0x02 to 0xFF: Prohibited. */
	uint8_t directed_relay;                         /*!< New Directed Relay state.
                                                     *   0x00: Disable directed relay.
                                                     *   0x01: Enable directed relay.
                                                     *   0x02 to 0xFF: Prohibited. */
	uint8_t directed_proxy;                         /*!< New Directed Proxy state.
                                                     *   0x00: Disable directed proxy.
                                                     *   0x01: Enable directed proxy.
                                                     *   0x02 to 0xFE: Prohibited.
                                                     *   0xFF: Do Not Process, the field is not processed. */
	uint8_t directed_proxy_use_directed_default;    /*!< New Directed Proxy Use Directed Default state.
                                                     *   0x00: The default value of Use_Directed of the Directed Proxy Server is Disabled.
                                                     *   0x01: The default value of Use_Directed of the Directed Proxy Server is Enabled.
                                                     *   0x02 to 0xFE: Prohibited.
                                                     *   0xFF: Do Not Process, the field is not processed. */
	uint8_t directed_friend;                        /*!< New Directed Friend state.
                                                     *   0x00: Disable directed friend.
                                                     *   0x01: Enable directed friend.
                                                     *   0x02 to 0xFE: Prohibited.
                                                     *   0xFF: Do Not Process, the field is not processed. */
} rtk_bt_mesh_df_client_act_directed_control_set_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_path_metric_get_t
 * @brief     BLE MESH path metric get message structure.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;    /*!< Index of the NetKey of the subnet that the Path Metric state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_path_metric_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_path_metric_set_t
 * @brief     BLE MESH path metric set message structure.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;    /*!< Index of the NetKey of the subnet that the Path Metric state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint8_t metric_type;          /*!< New Path Metric Type state, 3 bits.
                                   *   0b000: Node Count, the node uses the Node Count metric.
                                   *   0b001 to 0b111: Reserved for Future Use. */
	uint8_t lifetime;             /*!< New Path Lifetime state, 2 bits.
                                   *   0b00: Path lifetime is 12 minutes.
                                   *   0b01: Path lifetime is 2 hours.
                                   *   0b10: Path lifetime is 24 hours.
                                   *   0b11: Path lifetime is 10 days. */
} rtk_bt_mesh_df_client_act_path_metric_set_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_discovery_table_capabilities_get_t
 * @brief     BLE MESH discovery table capabilities get message structure.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;    /*!< Index of the NetKey of the subnet that the Discovery Table Capabilities state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_discovery_table_capabilities_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_discovery_table_capabilities_set_t
 * @brief     BLE MESH discovery table capabilities set message structure.
 */
typedef struct {
	uint16_t dst;                   /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;         /*!< Index of the NetKey of the subnet the message is sent through.
                                     *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;      /*!< Index of the NetKey of the subnet that the Discovery Table Capabilities state belongs to.
                                     *   Range: 0x0000 to 0x0FFF. */
	uint8_t max_concurrent_init;    /*!< New Max Concurrent Init state, the maximum number of concurrent Directed Forwarding Initialization procedures.
                                     *   0x00: Prohibited.
                                     *   It shall be at most the Max Discovery Table Entries Count state of the server. */
} rtk_bt_mesh_df_client_act_discovery_table_capabilities_set_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_forwarding_table_add_t
 * @brief     BLE MESH forwarding table add message structure.
 */
typedef struct {
	uint16_t dst;                     /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;           /*!< Index of the NetKey of the subnet the message is sent through.
                                       *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;        /*!< Index of the NetKey of the subnet that the Forwarding Table state belongs to.
                                       *   Range: 0x0000 to 0x0FFF. */
	uint8_t unicast_dst_flag;         /*!< Whether the destination of the path is a unicast address.
                                       *   1: The destination is a unicast address, target_addr and target_element_num give the Path Target.
                                       *   0: The destination is a group address or a virtual address, target_addr gives the destination. */
	uint8_t backward_path_valid;      /*!< Whether the backward path is validated.
                                       *   1: The backward path is validated.
                                       *   0: The backward path is not validated.
                                       *   If unicast_dst_flag is 0, backward_path_valid shall be 0. */
	uint16_t origin_addr;             /*!< Unicast address of the Path Origin, the first address of the unicast address range. */
	uint16_t origin_element_num;      /*!< Number of addresses of the unicast address range of the Path Origin, starting from origin_addr.
                                       *   Range: 1 to 255. */
	uint16_t target_addr;             /*!< Unicast address of the Path Target, the first address of the unicast address range, if unicast_dst_flag is 1.
                                       *   Group address or virtual address of the destination if unicast_dst_flag is 0.
                                       *   The all-directed-forwarding-nodes, all-nodes and all-relays fixed group addresses are Prohibited. */
	uint16_t target_element_num;      /*!< Number of addresses of the unicast address range of the Path Target, starting from target_addr.
                                       *   It is only used for a unicast destination.
                                       *   Range: 1 to 255. */
	uint16_t bearer_toward_origin;    /*!< Index of the bearer used for directed forwarding toward the Path Origin.
                                       *   It is a bit field identifying a bearer or a combination of bearers.
                                       *   Bit 0: The advertising bearer.
                                       *   Bit 1: The GATT bearer.
                                       *   Bits 2 to 15: RFU. */
	uint16_t bearer_toward_target;    /*!< Index of the bearer used for directed forwarding toward the Path Target.
                                       *   The format is the same as bearer_toward_origin. */
} rtk_bt_mesh_df_client_act_forwarding_table_add_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_forwarding_table_delete_t
 * @brief     BLE MESH forwarding table delete message structure.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;    /*!< Index of the NetKey of the subnet that the Forwarding Table state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t path_origin;         /*!< Primary element address of the Path Origin.
                                   *   It shall be a unicast address. */
	uint16_t path_target;         /*!< Destination address of the path entry.
                                   *   The unassigned address and the all-directed-forwarding-nodes,
                                   *   all-nodes and all-relays fixed group addresses are Prohibited.
                                   *   It shall be different from path_origin. */
} rtk_bt_mesh_df_client_act_forwarding_table_delete_t;

/**
 * @struct    rtk_bt_mesh_addr_range_add_t
 * @brief     BLE MESH unicast address range of a dependent node structure.
 */
typedef struct {
	uint16_t addr;          /*!< Primary element address of the dependent node. */
	uint8_t element_num;    /*!< Number of elements of the dependent node, starting from addr.
                             *   Range: 1 to 255. */
} rtk_bt_mesh_addr_range_add_t;

/**
 * @def       DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE
 * @brief     Maximum number of entries of an address list, that is each dependent node list of a Forwarding Table Dependents message
 *            and the destination addresses of a Path Solicitation.
 */
#define DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE 10

/**
 * @struct    rtk_bt_mesh_df_client_act_forwarding_table_dependents_add_t
 * @brief     BLE MESH forwarding table dependents add message structure.
 */
typedef struct {
	uint16_t dst;                                                                     /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;                                                           /*!< Index of the NetKey of the subnet the message is sent through.
                                                                                       *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;                                                        /*!< Index of the NetKey of the subnet that the Forwarding Table
                                                                                       *   state belongs to.
                                                                                       *   Range: 0x0000 to 0x0FFF. */
	uint16_t path_origin;                                                             /*!< Primary element address of the Path Origin.
                                                                                       *   It shall be a unicast address. */
	uint16_t path_target;                                                             /*!< Destination address of the path entry.
                                                                                       *   The unassigned address and the all-directed-forwarding-nodes,
                                                                                       *   all-nodes and all-relays fixed group addresses are Prohibited.
                                                                                       *   It shall be different from path_origin. */
	uint8_t do_list_size;                                                             /*!< Number of entries in do_range.
                                                                                       *   do_list_size and dt_list_size shall not both be 0.
                                                                                       *   Range: 0 to @ref DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE. */
	uint8_t dt_list_size;                                                             /*!< Number of entries in dt_range.
                                                                                       *   It shall be 0 if path_target is not a unicast address.
                                                                                       *   Range: 0 to @ref DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE. */
	rtk_bt_mesh_addr_range_add_t do_range[DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE];    /*!< Unicast address ranges of the dependent nodes of the Path Origin,
                                                                                       *   do_list_size entries.
                                                                                       *   Its addresses shall differ from path_origin and path_target. */
	rtk_bt_mesh_addr_range_add_t dt_range[DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE];    /*!< Unicast address ranges of the dependent nodes of the Path Target,
                                                                                       *   dt_list_size entries.
                                                                                       *   Its addresses shall differ from path_origin, path_target,
                                                                                       *   and the addresses of do_range. */
} rtk_bt_mesh_df_client_act_forwarding_table_dependents_add_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_forwarding_table_dependents_delete_t
 * @brief     BLE MESH forwarding table dependents delete message structure.
 */
typedef struct {
	uint16_t dst;                                                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;                                       /*!< Index of the NetKey of the subnet the message is sent through.
                                                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;                                    /*!< Index of the NetKey of the subnet that the Forwarding Table state belongs to.
                                                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t path_origin;                                         /*!< Primary element address of the Path Origin.
                                                                   *   It shall be a unicast address. */
	uint16_t path_target;                                         /*!< Destination address of the path entry.
                                                                   *   The unassigned address and the all-directed-forwarding-nodes,
                                                                   *   all-nodes and all-relays fixed group addresses are Prohibited.
                                                                   *   It shall be different from path_origin. */
	uint8_t do_list_size;                                         /*!< Number of entries in do_range.
                                                                   *   do_list_size and dt_list_size shall not both be 0.
                                                                   *   Range: 0 to @ref DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE. */
	uint8_t dt_list_size;                                         /*!< Number of entries in dt_range.
                                                                   *   It shall be 0 if path_target is not a unicast address.
                                                                   *   Range: 0 to @ref DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE. */
	uint16_t do_range[DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE];    /*!< Primary element addresses of the dependent nodes of the Path Origin to delete, do_list_size
                                                                   *   entries.
                                                                   *   They shall differ from path_origin and path_target. */
	uint16_t dt_range[DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE];    /*!< Primary element addresses of the dependent nodes of the Path Target to delete, dt_list_size
                                                                   *   entries.
                                                                   *   They shall differ from path_origin, path_target and the addresses of do_range. */
} rtk_bt_mesh_df_client_act_forwarding_table_dependents_delete_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_forwarding_table_dependents_get_t
 * @brief     BLE MESH forwarding table dependents get message structure.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;    /*!< Index of the NetKey of the subnet that the Forwarding Table state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint8_t list_mask;            /*!< Filter applied to the lists of unicast address ranges of the dependent nodes, a combination of the bits below.
                                   *   Bit 0: Whether to return the list of the Path Origin.
                                   *   Bit 1: Whether to return the list of the Path Target.
                                   *   For each bit:
                                   *   0: The list is not returned.
                                   *   1: The list is returned.
                                   *   Bits 0 and 1 shall not both be 0.
                                   *   Bit 1 shall be 0 if path_target is a group address or a virtual address. */
	uint8_t fixed_path_flag;      /*!< Whether to return the unicast address ranges of the dependent nodes of a fixed path entry
                                   *   or of a non-fixed path entry.
                                   *   0: Return the unicast address ranges of the dependent nodes of the non-fixed path entry.
                                   *   1: Return the unicast address ranges of the dependent nodes of the fixed path entry. */
	uint16_t start_index;         /*!< Start offset in units of unicast address ranges to read. */
	uint16_t path_origin;         /*!< Primary element address of the Path Origin.
                                   *   It shall be a unicast address. */
	uint16_t path_target;         /*!< Destination address of the path entry.
                                   *   The unassigned address and the all-directed-forwarding-nodes,
                                   *   all-nodes and all-relays fixed group addresses are Prohibited.
                                   *   It shall be different from path_origin. */
	uint8_t update_id_exist;      /*!< Whether update_id is carried in the message.
                                   *   1: update_id is carried.
                                   *   0: update_id is not carried. */
	uint16_t update_id;           /*!< Last saved Forwarding Table Update Identifier state.
                                   *   Only used when update_id_exist is true. */
} rtk_bt_mesh_df_client_act_forwarding_table_dependents_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_forwarding_table_entries_count_get_t
 * @brief     BLE MESH forwarding table entries count get message structure.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;    /*!< Index of the NetKey of the subnet that the Forwarding Table state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_forwarding_table_entries_count_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_forwarding_table_entries_get_t
 * @brief     BLE MESH forwarding table entries get message structure.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;    /*!< Index of the NetKey of the subnet that the Forwarding Table state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint8_t filter_mask;          /*!< Filter applied to the Forwarding Table entries, a combination of the bits below.
                                   *   Bit 0: Whether to return the fixed path entries.
                                   *   Bit 1: Whether to return the non-fixed path entries.
                                   *   Bit 2: Whether to return the path entries with a matching path_origin.
                                   *   Bit 3: Whether to return the path entries with a matching path_target.
                                   *   For each bit:
                                   *   0: The path entries are not returned.
                                   *   1: The path entries are returned.
                                   *   Bits 0 and 1 shall not both be 0. */
	uint16_t start_index;         /*!< Start offset in units of Forwarding Table entries to read. */
	uint16_t path_origin;         /*!< Primary element address of the Path Origin to match.
                                   *   Only used when bit 2 of filter_mask is 1.
                                   *   It shall be a unicast address. */
	uint16_t path_target;         /*!< Destination address to match.
                                   *   Only used when bit 3 of filter_mask is 1.
                                   *   The unassigned address and the all-directed-forwarding-nodes,
                                   *   all-nodes and all-relays fixed group addresses are Prohibited.
                                   *   If path_origin is also used, it shall be different from path_origin. */
	uint8_t update_id_flag;       /*!< Whether update_id is carried in the message.
                                   *   1: update_id is carried.
                                   *   0: update_id is not carried. */
	uint16_t update_id;           /*!< Last saved Forwarding Table Update Identifier state.
                                   *   Only used when update_id_flag is true. */
} rtk_bt_mesh_df_client_act_forwarding_table_entries_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_wanted_lanes_get_t
 * @brief     BLE MESH wanted lanes get message structure.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;    /*!< Index of the NetKey of the subnet that the Wanted Lanes state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_wanted_lanes_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_wanted_lanes_set_t
 * @brief     BLE MESH wanted lanes set message structure.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;    /*!< Index of the NetKey of the subnet that the Wanted Lanes state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint8_t wanted_lanes;         /*!< Wanted Lanes state to set, the number of lanes to be discovered for each path in the subnet.
                                   *   0x00: Prohibited. */
} rtk_bt_mesh_df_client_act_wanted_lanes_set_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_two_way_path_get_t
 * @brief     BLE MESH two way path get message structure.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;    /*!< Index of the NetKey of the subnet that the Two Way Path state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_two_way_path_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_two_way_path_set_t
 * @brief     BLE MESH two way path set message structure.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;    /*!< Index of the NetKey of the subnet that the Two Way Path state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint8_t two_way_path;         /*!< Two Way Path state to set.
                                   *   0b0: Each path is considered one-way, it is used to forward messages only from the Path Origin to the Path Target.
                                   *   0b1: Each path is considered two-way, it is used to forward messages from the Path Origin to the Path Target
                                   *   and from the Path Target to the Path Origin. */
} rtk_bt_mesh_df_client_act_two_way_path_set_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_path_echo_interval_get_t
 * @brief     BLE MESH path echo interval get message structure.
 */
typedef struct {
	uint16_t dst;                 /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;       /*!< Index of the NetKey of the subnet the message is sent through.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;    /*!< Index of the NetKey of the subnet that the Path Echo Interval state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_path_echo_interval_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_path_echo_interval_set_t
 * @brief     BLE MESH path echo interval set message structure.
 */
typedef struct {
	uint16_t dst;                       /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;             /*!< Index of the NetKey of the subnet the message is sent through.
                                         *   Range: 0x0000 to 0x0FFF. */
	uint16_t net_key_index_df;          /*!< Index of the NetKey of the subnet that the Path Echo Interval state belongs to.
                                         *   Range: 0x0000 to 0x0FFF. */
	uint8_t unicast_echo_interval;      /*!< Unicast Echo Interval state to set, or no change.
                                         *   0x00: The Directed Forwarding Echo procedure is not executed for a destination that is a unicast address.
                                         *   0x01 to 0x63: Interval expressed as percentage of the time corresponding to the Path Lifetime state.
                                         *   0x64 to 0xFE: Prohibited.
                                         *   0xFF: No change in the state.
                                         *   unicast_echo_interval and multicast_echo_interval shall not both be 0xFF. */
	uint8_t multicast_echo_interval;    /*!< Multicast Echo Interval state to set, or no change.
                                         *   0x00: The Directed Forwarding Echo procedure is not executed for a destination that is a group address or a virtual address.
                                         *   0x01 to 0x63: Interval expressed as percentage of the time corresponding to the Path Lifetime state.
                                         *   0x64 to 0xFE: Prohibited.
                                         *   0xFF: No change in the state. */
} rtk_bt_mesh_df_client_act_path_echo_interval_set_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_directed_network_transmit_get_t
 * @brief     BLE MESH directed network transmit get message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_directed_network_transmit_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_directed_network_transmit_set_t
 * @brief     BLE MESH directed network transmit set message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t count;             /*!< Directed Network Transmit Count to set, controls the number of transmissions.
                                *   Number of transmissions = count + 1.
                                *   Range: 0x00 to 0x07. */
	uint8_t steps;             /*!< Directed Network Transmit Interval Steps to set, the number of 10-millisecond steps between transmissions.
                                *   Transmission interval = (steps + 1) * 10 ms.
                                *   Range: 0x00 to 0x1F. */
} rtk_bt_mesh_df_client_act_directed_network_transmit_set_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_directed_relay_retransmit_get_t
 * @brief     BLE MESH directed relay retransmit get message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_directed_relay_retransmit_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_directed_relay_retransmit_set_t
 * @brief     BLE MESH directed relay retransmit set message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t count;             /*!< Directed Relay Retransmit Count to set, controls the number of transmissions.
                                *   Number of transmissions = count + 1.
                                *   Range: 0x00 to 0x07. */
	uint8_t steps;             /*!< Directed Relay Retransmit Interval Steps to set, the number of 10-millisecond steps between transmissions.
                                *   Transmission interval = (steps + 1) * 10 ms.
                                *   Range: 0x00 to 0x1F. */
} rtk_bt_mesh_df_client_act_directed_relay_retransmit_set_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_rssi_threshold_get_t
 * @brief     BLE MESH rssi threshold get message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_rssi_threshold_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_rssi_threshold_set_t
 * @brief     BLE MESH rssi threshold set message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t rssi_margin;       /*!< RSSI Margin state to set, the margin above the default RSSI threshold for which the link quality with a neighbor node is considered
                                *   acceptable.
                                *   Unit: dB.
                                *   Range: 0x00 to 0x32.
                                *   0x33 to 0xFF: Prohibited. */
} rtk_bt_mesh_df_client_act_rssi_threshold_set_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_directed_paths_get_t
 * @brief     BLE MESH directed paths get message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_directed_paths_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_directed_publish_policy_get_t
 * @brief     BLE MESH directed publish policy get message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t elem_addr;        /*!< Unicast address of the element that supports the publishing model. */
	uint32_t model_id;         /*!< Identifier of the model.
                                *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                                *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
} rtk_bt_mesh_df_client_act_directed_publish_policy_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_directed_publish_policy_set_t
 * @brief     BLE MESH directed publish policy set message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t policy;            /*!< Directed Publish Policy state to set.
                                *   0x00: The policy is to use managed flooding when publishing.
                                *   0x01: The policy is to use directed forwarding when publishing.
                                *   0x02 to 0xFF: Reserved for future use. */
	uint16_t elem_addr;        /*!< Unicast address of the element that supports the publishing model. */
	uint32_t model_id;         /*!< Identifier of the model.
                                *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                                *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
} rtk_bt_mesh_df_client_act_directed_publish_policy_set_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_path_discovery_timing_control_get_t
 * @brief     BLE MESH path discovery timing control get message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_path_discovery_timing_control_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_path_discovery_timing_control_set_t
 * @brief     BLE MESH path discovery timing control set message structure.
 */
typedef struct {
	uint16_t dst;                   /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;         /*!< Index of the NetKey of the subnet the message is sent through.
                                     *   Range: 0x0000 to 0x0FFF. */
	uint16_t monitor_interval;      /*!< Path Monitoring Interval state to set, the time interval during which the node collects
                                     *   information to determine whether or not to execute a Directed Forwarding Initialization
                                     *   procedure for a destination before a path to that destination expires.
                                     *   Unit: second. */
	uint16_t dis_retry_interval;    /*!< Path Discovery Retry Interval state to set, the time interval after a Directed Forwarding Initialization procedure fails
                                     *   before starting a new one for the same destination.
                                     *   Unit: second. */
	uint8_t dis_interval;           /*!< Path Discovery Interval state to set, the initial value of the Path Discovery timer.
                                     *   0b0: 5 seconds.
                                     *   0b1: 30 seconds. */
	uint8_t lan_guard;              /*!< Lane Discovery Guard Interval state to set, the interval between the Path Discovery timer expiry and sending
                                     *   a PATH_REQUEST for a new lane discovery for the same path.
                                     *   0b0: 2 seconds.
                                     *   0b1: 10 seconds. */
} rtk_bt_mesh_df_client_act_path_discovery_timing_control_set_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_directed_control_network_transmit_get_t
 * @brief     BLE MESH directed control network transmit get message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_directed_control_network_transmit_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t
 * @brief     BLE MESH directed control network transmit set message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t count;             /*!< Directed Control Network Transmit Count to set, controls the number of transmissions.
                                *   Number of transmissions = count + 1.
                                *   Range: 0x00 to 0x07. */
	uint8_t steps;             /*!< Directed Control Network Transmit Interval Steps to set, the number of 10-millisecond steps between transmissions.
                                *   Transmission interval = (steps + 1) * 10 ms.
                                *   Range: 0x00 to 0x1F. */
} rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_get_t
 * @brief     BLE MESH directed control relay retransmit get message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_get_t;

/**
 * @struct    rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_set_t
 * @brief     BLE MESH directed control relay retransmit set message structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Directed Forwarding Configuration Server. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet the message is sent through.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t count;             /*!< Directed Control Relay Retransmit Count to set, controls the number of transmissions.
                                *   Number of transmissions = count + 1.
                                *   Range: 0x00 to 0x07. */
	uint8_t steps;             /*!< Directed Control Relay Retransmit Interval Steps to set, the number of 10-millisecond steps between transmissions.
                                *   Transmission interval = (steps + 1) * 10 ms.
                                *   Range: 0x00 to 0x1F. */
} rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_set_t;

/**************************************** Common ***************************************/
/**
 * @def       RTK_BT_MESH_MSG_DF_PATH_ACTION
 * @brief     Value of the type of @ref rtk_bt_mesh_stack_evt_df_t that indicates a path action.
 */
#define RTK_BT_MESH_MSG_DF_PATH_ACTION            0x00

/**
 * @struct    rtk_bt_mesh_df_act_df_path_discovery_t
 * @brief     BLE MESH directed forwarding path discovery message structure.
 */
typedef struct {
	uint16_t master_key_index;     /*!< Index of the NetKey of the subnet to discover the path in.
                                    *   Range: 0x0000 to 0x0FFF. */
	uint16_t target_addr;          /*!< Address of the Path Target to discover the path to. */
	uint16_t dependent_addr;       /*!< Primary element address of the dependent node of the Path Origin.
                                    *   The unassigned address if there is no dependent node. */
	uint8_t dependent_elem_num;    /*!< Number of elements of the dependent node, starting from dependent_addr.
                                    *   It must be greater than 0 if dependent_addr is not the unassigned address. */
} rtk_bt_mesh_df_act_df_path_discovery_t;

/**
 * @struct    rtk_bt_mesh_df_act_df_path_solicitation_t
 * @brief     BLE MESH directed forwarding path solicitation message structure.
 */
typedef struct {
	uint16_t master_key_index;                                /*!< Index of the NetKey of the subnet to solicit the path discovery in.
                                                               *   Range: 0x0000 to 0x0FFF. */
	uint16_t addr_num;                                        /*!< Number of addresses in addr.
                                                               *   It shall not exceed @ref DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE. */
	uint16_t addr[DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE];    /*!< Destination addresses to request the path discovery for, addr_num entries.
                                                               *   Unicast addresses, group addresses and virtual addresses are allowed.
                                                               *   The all-directed-forwarding-nodes, all-nodes and all-relays fixed group addresses are Prohibited. */
} rtk_bt_mesh_df_act_df_path_solicitation_t;

/**
 * @struct    rtk_bt_mesh_df_act_df_path_dependents_update_t
 * @brief     BLE MESH directed forwarding path dependents update message structure.
 */
typedef struct {
	uint8_t type;                  /*!< Type of the update.
                                    *   0: The dependent node address is removed.
                                    *   1: The dependent node address is added. */
	uint16_t endpoint_addr;        /*!< Primary element address of the Path Origin or the Path Target that the dependent node belongs to. */
	uint16_t dependent_addr;       /*!< Primary element address of the dependent node. */
	uint8_t dependent_elem_num;    /*!< Number of elements of the dependent node, starting from dependent_addr.
                                    *   Range: 1 to 255. */
} rtk_bt_mesh_df_act_df_path_dependents_update_t;

/**
 * @struct    rtk_bt_mesh_df_control_state_setting_t
 * @brief     BLE MESH directed forwarding control state setting message structure.
 */
typedef struct {
	uint16_t master_key_index;             /*!< Index of the NetKey of the subnet that the Directed Control state belongs to.
                                            *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_directed_control_t ctl;    /*!< Directed Control state of the subnet. */
} rtk_bt_mesh_df_control_state_setting_t;

/**
 * @struct    rtk_bt_mesh_df_path_metric_state_setting_t
 * @brief     BLE MESH directed forwarding path metric state setting message structure.
 */
typedef struct {
	uint16_t master_key_index;           /*!< Index of the NetKey of the subnet that the Path Metric state belongs to.
                                          *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_path_metric_t metric;    /*!< Path Metric state of the subnet. */
} rtk_bt_mesh_df_path_metric_state_setting_t;

/**
 * @struct    rtk_bt_mesh_df_discovery_table_cap_state_setting_t
 * @brief     BLE MESH directed forwarding discovery table capabilities state setting message structure.
 */
typedef struct {
	uint16_t master_key_index;                         /*!< Index of the NetKey of the subnet that the Discovery Table Capabilities state belongs to.
                                                        *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_discovery_table_capabilities_t cap;    /*!< Discovery Table Capabilities state of the subnet. */
} rtk_bt_mesh_df_discovery_table_cap_state_setting_t;

/**
 * @struct    rtk_bt_mesh_df_wanted_lanes_state_setting_t
 * @brief     BLE MESH directed forwarding wanted lanes state setting message structure.
 */
typedef struct {
	uint16_t master_key_index;    /*!< Index of the NetKey of the subnet that the Wanted Lanes state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint8_t wanted_lanes;         /*!< Wanted Lanes state, the number of lanes to be discovered for each path in the subnet.
                                   *   0x00: Prohibited. */
} rtk_bt_mesh_df_wanted_lanes_state_setting_t;

/**
 * @struct    rtk_bt_mesh_df_two_way_path_state_setting_t
 * @brief     BLE MESH directed forwarding two way path state setting message structure.
 */
typedef struct {
	uint16_t master_key_index;    /*!< Index of the NetKey of the subnet that the Two Way Path state belongs to.
                                   *   Range: 0x0000 to 0x0FFF. */
	uint8_t two_way_path;         /*!< Two Way Path state.
                                   *   0b0: Each path is considered one-way and is used only from the Path Origin to the Path Target.
                                   *   0b1: Each path is considered two-way and is used in both directions. */
} rtk_bt_mesh_df_two_way_path_state_setting_t;

/**
 * @struct    rtk_bt_mesh_df_echo_interval_state_setting_t
 * @brief     BLE MESH directed forwarding echo interval state setting message structure.
 */
typedef struct {
	uint16_t master_key_index;          /*!< Index of the NetKey of the subnet that the Path Echo Interval state belongs to.
                                         *   Range: 0x0000 to 0x0FFF. */
	uint8_t unicast_echo_interval;      /*!< Unicast Echo Interval state.
                                         *   0x00: The Directed Forwarding Echo procedure is not executed for a destination that is a unicast address.
                                         *   0x01 to 0x63: Interval expressed as percentage of the time of the Path Lifetime state.
                                         *   0x64 to 0xFF: Prohibited. */
	uint8_t multicast_echo_interval;    /*!< Multicast Echo Interval state.
                                         *   0x00: The Directed Forwarding Echo procedure is not executed for a destination that is a group address or a virtual address.
                                         *   0x01 to 0x63: Interval expressed as percentage of the time of the Path Lifetime state.
                                         *   0x64 to 0xFF: Prohibited. */
} rtk_bt_mesh_df_echo_interval_state_setting_t;

/**
 * @struct    rtk_bt_mesh_df_publish_policy_setting_t
 * @brief     BLE MESH directed forwarding model publish policy state setting message structure.
 */
typedef struct {
	uint8_t element_index;    /*!< Index of the element of this node that contains the model. */
	uint32_t model_id;        /*!< Identifier of the model.
                               *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                               *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
	uint8_t policy;           /*!< Directed Publish Policy state of the model, see @ref rtk_bt_mesh_directed_publish_policy_t. */
} rtk_bt_mesh_df_publish_policy_setting_t;

/**************************************** Event ****************************************/
/**
 * @struct    rtk_bt_mesh_forwarding_table_entry_header_t
 * @brief     BLE MESH forwarding table entry header structure.
 */
typedef struct {
	uint16_t fixed_path_flag : 1;                         /*!< Whether the entry is a fixed path entry.
                                                           *   0: The entry is a non-fixed path entry.
                                                           *   1: The entry is a fixed path entry. */
	uint16_t unicast_dst_flag : 1;                        /*!< Whether the destination of the path is a unicast address.
                                                           *   0: The destination is a group address or a virtual address.
                                                           *   1: The destination is a unicast address. */
	uint16_t backward_path_validated_flag : 1;            /*!< Whether the backward path has been validated.
                                                           *   0: The backward path has not been validated.
                                                           *   1: The backward path has been validated. */
	uint16_t bearer_toward_path_origin_indicator : 1;     /*!< Whether the node is the Path Origin for the entry.
                                                           *   0: The node is not the Path Origin, the Bearer_Toward_Path_Origin field is absent.
                                                           *   1: The node is the Path Origin, the Bearer_Toward_Path_Origin field is present. */
	uint16_t bearer_toward_path_target_indicator : 1;     /*!< Whether the node is the Path Target for the entry.
                                                           *   0: The node is not the Path Target, the Bearer_Toward_Path_Target field is absent.
                                                           *   1: The node is the Path Target, the Bearer_Toward_Path_Target field is present. */
	uint16_t dependent_origin_list_size_indicator : 2;    /*!< Number of octets used to represent the length of the Dependent_Origin_List field.
                                                           *   0b00: The list size is zero, the Dependent_Origin_List_Size field is absent.
                                                           *   0b01: The Dependent_Origin_List_Size field is 1 octet.
                                                           *   0b10: The Dependent_Origin_List_Size field is 2 octets.
                                                           *   0b11: Prohibited. */
	uint16_t dependent_target_list_size_indicator : 2;    /*!< Number of octets used to represent the length of the Dependent_Target_List field.
                                                           *   0b00: The list size is zero, the Dependent_Target_List_Size field is absent.
                                                           *   0b01: The Dependent_Target_List_Size field is 1 octet.
                                                           *   0b10: The Dependent_Target_List_Size field is 2 octets.
                                                           *   0b11: Prohibited. */
	uint16_t prohibited : 7;                              /*!< Prohibited bits of the entry header format. */
} _PACKED4_ rtk_bt_mesh_forwarding_table_entry_header_t;

/**
 * @struct    rtk_bt_mesh_directed_control_status_t
 * @brief     BLE MESH directed control status evt message structure.
 */
typedef struct {
	uint8_t status;                                 /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index;                         /*!< Index of the NetKey of the subnet that the Directed Control state belongs to.
                                                     *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_directed_control_t directed_ctl;    /*!< Current Directed Control state of the subnet. */
} _PACKED4_ rtk_bt_mesh_directed_control_status_t;

/**
 * @struct    rtk_bt_mesh_path_metric_status_t
 * @brief     BLE MESH path metric status evt message structure.
 */
typedef struct {
	uint8_t status;                           /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index;                   /*!< Index of the NetKey of the subnet that the Path Metric state belongs to.
                                               *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_path_metric_t path_metric;    /*!< Current Path Metric state of the node. */
} _PACKED4_ rtk_bt_mesh_path_metric_status_t;

/**
 * @struct    rtk_bt_mesh_discovery_table_capabilities_status_t
 * @brief     BLE MESH discovery table capabilities status evt message structure.
 */
typedef struct {
	uint8_t status;                                             /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index;                                     /*!< Index of the NetKey of the subnet that the Discovery Table Capabilities state belongs to.
                                                                 *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_discovery_table_capabilities_t capabilities;    /*!< Current Discovery Table Capabilities state of the node. */
} _PACKED4_ rtk_bt_mesh_discovery_table_capabilities_status_t;

/**
 * @struct    rtk_bt_mesh_forwarding_table_status_t
 * @brief     BLE MESH forwarding table status evt message structure.
 */
typedef struct {
	uint8_t status;            /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet that the Forwarding Table state belongs to.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t path_origin;      /*!< Primary element address of the Path Origin of the path entries added or deleted in the most recent operation. */
	uint16_t dst;              /*!< Destination address of the path entries added or deleted in the most recent operation. */
} _PACKED4_ rtk_bt_mesh_forwarding_table_status_t;

/**
 * @struct    rtk_bt_mesh_forwarding_table_dependents_status_t
 * @brief     BLE MESH forwarding table dependents status evt message structure.
 */
typedef struct {
	uint8_t status;            /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet that the Forwarding Table state belongs to.
                                *   Range: 0x0000 to 0x0FFF. */
	uint16_t path_origin;      /*!< Primary element address of the Path Origin of the path entry whose dependent node list was changed in the most recent operation. */
	uint16_t dst;              /*!< Destination address of the path entry whose dependent node list was changed in the most recent operation. */
} _PACKED4_ rtk_bt_mesh_forwarding_table_dependents_status_t;

/**
 * @struct    rtk_bt_mesh_forwarding_table_entries_count_status_t
 * @brief     BLE MESH forwarding table entries count status evt message structure.
 */
typedef struct {
	uint8_t status;                                        /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index;                                /*!< Index of the NetKey of the subnet that the Forwarding Table state belongs to.
                                                            *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_forwarding_table_entries_count_t count;    /*!< Information about the Forwarding Table state of the node.
                                                            *   It is only valid when status is success. */
} _PACKED4_ rtk_bt_mesh_forwarding_table_entries_count_status_t;

/**
 * @struct    rtk_bt_mesh_forwarding_table_entries_status_t
 * @brief     BLE MESH forwarding table entries status evt message structure.
 */
typedef struct {
	uint8_t status;                /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index: 12;    /*!< Index of the NetKey of the subnet that the Forwarding Table state belongs to.
                                    *   Range: 0x0000 to 0x0FFF. */
	uint16_t filter_mask: 4;       /*!< Filter applied to the reported Forwarding Table entries.
                                    *   Bit 0: Fixed path entries are returned.
                                    *   Bit 1: Non-fixed path entries are returned.
                                    *   Bit 2: Path entries with a matching Path Origin are returned.
                                    *   Bit 3: Path entries with a matching Destination are returned.
                                    *   Bits 0 and 1 are not both 0. */
	uint16_t start_index;          /*!< Start offset in units of Forwarding Table entries of the reported entries. */
	uint8_t data[0];               /*!< Data that follows the fixed fields of the message. This structure only indicates the format of the message,
                                    *   the event reports @ref rtk_bt_mesh_forwarding_table_entries_status_for_call_t. */
} _PACKED4_ rtk_bt_mesh_forwarding_table_entries_status_t;

/**
 * @struct    rtk_bt_mesh_forwarding_table_entries_status_for_call_t
 * @brief     BLE MESH forwarding table entries status for call app evt message structure.
 */
typedef struct {
	uint8_t status;                /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index: 12;    /*!< Index of the NetKey of the subnet that the Forwarding Table state belongs to.
                                    *   Range: 0x0000 to 0x0FFF. */
	uint16_t filter_mask: 4;       /*!< Filter applied to the reported Forwarding Table entries.
                                    *   Bit 0: Whether the fixed path entries are returned.
                                    *   Bit 1: Whether the non-fixed path entries are returned.
                                    *   Bit 2: Whether the path entries with a matching Path Origin are returned.
                                    *   Bit 3: Whether the path entries with a matching Destination are returned.
                                    *   For each bit:
                                    *   0: The path entries are not returned.
                                    *   1: The path entries are returned.
                                    *   Bits 0 and 1 are not both 0. */
	uint16_t start_index;          /*!< Start offset in units of Forwarding Table entries of the reported entries. */
	uint16_t data_len;             /*!< Length in bytes of the data located right after the structure in the event data. */
	uint8_t *data;                 /*!< Data of the message following Start_Index.
                                    *   This member only marks where the data starts,
                                    *   the data is located right after the structure in the event data, its length is data_len.
                                    *   It is made of the following fields in order:
                                    *   - Path_Origin, 2 bytes, only if bit 2 of filter_mask is 1.
                                    *   - Destination, 2 bytes, only if bit 3 of filter_mask is 1.
                                    *   - Forwarding Table Update Identifier, 2 bytes, only if status is success.
                                    *   - Forwarding Table entries, each starts with @ref rtk_bt_mesh_forwarding_table_entry_header_t. */
} _PACKED4_ rtk_bt_mesh_forwarding_table_entries_status_for_call_t;

/**
 * @struct    rtk_bt_mesh_forwarding_table_dependents_get_status_t
 * @brief     BLE MESH forwarding table dependents get status evt message structure.
 */
typedef struct {
	uint8_t status;                      /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index: 12;          /*!< Index of the NetKey of the subnet that the Forwarding Table state belongs to.
                                          *   Range: 0x0000 to 0x0FFF. */
	uint16_t dependents_list_mask: 2;    /*!< Filter applied to the lists of unicast address ranges of dependent nodes.
                                          *   Bit 0: The list of the dependent nodes of the Path Origin is returned.
                                          *   Bit 1: The list of the dependent nodes of the Path Target is returned.
                                          *   Bits 0 and 1 are not both 0. */
	uint16_t fixed_path_flag: 1;         /*!< Which path entry the unicast address ranges are returned from.
                                          *   0: A non-fixed path entry.
                                          *   1: A fixed path entry. */
	uint16_t prohibited: 1;              /*!< Prohibited bit of the message format. */
	uint16_t start_index;                /*!< Start offset in units of unicast address ranges of the reported lists. */
	uint16_t path_origin;                /*!< Primary element address of the Path Origin of the matched path entry. */
	uint16_t dst;                        /*!< Destination address of the matched path entry. */
	uint8_t data[0];                     /*!< Data that follows the fixed fields of the message. This structure only indicates the format of the message,
                                          *   the event reports @ref rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t. */
} _PACKED4_ rtk_bt_mesh_forwarding_table_dependents_get_status_t;

/**
 * @struct    rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t
 * @brief     BLE MESH forwarding table dependents get status for call app evt message structure.
 */
typedef struct {
	uint8_t status;                      /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index: 12;          /*!< Index of the NetKey of the subnet that the Forwarding Table state belongs to.
                                          *   Range: 0x0000 to 0x0FFF. */
	uint16_t dependents_list_mask: 2;    /*!< Filter applied to the lists of unicast address ranges of dependent nodes.
                                          *   Bit 0: The list of the dependent nodes of the Path Origin is returned.
                                          *   Bit 1: The list of the dependent nodes of the Path Target is returned.
                                          *   Bits 0 and 1 are not both 0. */
	uint16_t fixed_path_flag: 1;         /*!< Which path entry the unicast address ranges are returned from.
                                          *   0: A non-fixed path entry.
                                          *   1: A fixed path entry. */
	uint16_t prohibited: 1;              /*!< Prohibited bit of the message format. */
	uint16_t start_index;                /*!< Start offset in units of unicast address ranges of the reported lists. */
	uint16_t path_origin;                /*!< Primary element address of the Path Origin of the matched path entry. */
	uint16_t dst;                        /*!< Destination address of the matched path entry. */
	uint16_t data_len;                   /*!< Length in bytes of the data located right after the structure in the event data. */
	uint8_t *data;                       /*!< Data of the message following Destination.
                                          *   This member only marks where the data starts,
                                          *   the data is located right after the structure in the event data, its length is data_len.
                                          *   It is made of the following fields in order:
                                          *   - Forwarding Table Update Identifier, 2 bytes, only if status is success or obsolete information.
                                          *   - Dependent_Origin_Unicast_Addr_Range_List_Size, 1 byte, only if status is success.
                                          *   - Dependent_Target_Unicast_Addr_Range_List_Size, 1 byte, only if status is success.
                                          *   - Dependent_Origin_Unicast_Addr_Range_List, only if its size is greater than 0.
                                          *   - Dependent_Target_Unicast_Addr_Range_List, only if its size is greater than 0. */
} _PACKED4_ rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t;

/**
 * @struct    rtk_bt_mesh_wanted_lanes_status_t
 * @brief     BLE MESH wanted lanes status evt message structure.
 */
typedef struct {
	uint8_t status;            /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index;    /*!< Index of the NetKey of the subnet that the Wanted Lanes state belongs to.
                                *   Range: 0x0000 to 0x0FFF. */
	uint8_t wanted_lanes;      /*!< Current Wanted Lanes state of the node.
                                *   0x00: Prohibited. */
} _PACKED4_ rtk_bt_mesh_wanted_lanes_status_t;

/**
 * @struct    rtk_bt_mesh_two_way_path_t
 * @brief     BLE MESH two way path structure.
 */
typedef struct {
	uint8_t two_way_path: 1;    /*!< Two Way Path state.
                                 *   0b0: Each path is considered one-way and is used only from the Path Origin to the Path Target.
                                 *   0b1: Each path is considered two-way and is used in both directions. */
	uint8_t prohibited: 7;      /*!< Prohibited bits of the message format. */
} _PACKED4_ rtk_bt_mesh_two_way_path_t;

/**
 * @struct    rtk_bt_mesh_two_way_path_status_t
 * @brief     BLE MESH two way path status evt message structure.
 */
typedef struct {
	uint8_t status;                             /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index;                     /*!< Index of the NetKey of the subnet that the Two Way Path state belongs to.
                                                 *   Range: 0x0000 to 0x0FFF. */
	rtk_bt_mesh_two_way_path_t two_way_path;    /*!< Current Two Way Path state of the node. */
} _PACKED4_ rtk_bt_mesh_two_way_path_status_t;

/**
 * @struct    rtk_bt_mesh_path_echo_interval_status_t
 * @brief     BLE MESH path echo interval status evt message structure.
 */
typedef struct {
	uint8_t status;                     /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	uint16_t net_key_index;             /*!< Index of the NetKey of the subnet that the Path Echo Interval state belongs to.
                                         *   Range: 0x0000 to 0x0FFF. */
	uint8_t unicast_echo_interval;      /*!< Current Unicast Echo Interval state of the node.
                                         *   0x00: The Directed Forwarding Echo procedure is not executed for a destination that is a unicast address.
                                         *   0x01 to 0x63: Interval expressed as percentage of the time of the Path Lifetime state.
                                         *   0x64 to 0xFF: Prohibited. */
	uint8_t multicast_echo_interval;    /*!< Current Multicast Echo Interval state of the node.
                                         *   0x00: The Directed Forwarding Echo procedure is not executed for a destination that is a group address or a virtual address.
                                         *   0x01 to 0x63: Interval expressed as percentage of the time of the Path Lifetime state.
                                         *   0x64 to 0xFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_path_echo_interval_status_t;

/**
 * @struct    rtk_bt_mesh_directed_network_transmit_t
 * @brief     BLE MESH directed network transmit structure.
 */
typedef struct {
	uint8_t directed_network_transmit_count: 3;             /*!< Directed Network Transmit Count, controls the number of transmissions of each Network PDU
                                                             *   with CTL equal to 0 that originates from either a Path Origin or a Path Target and that is secured using
                                                             *   directed security material.
                                                             *   Number of transmissions = count + 1.
                                                             *   Range: 0x00 to 0x07. */
	uint8_t directed_network_transmit_interval_steps: 5;    /*!< Directed Network Transmit Interval Steps, the number of 10-millisecond steps between
                                                             *   transmissions of each Network PDU with CTL equal to 0 that originates from either a Path Origin or a Path
                                                             *   Target and that is secured using directed security material.
                                                             *   Transmission interval = (steps + 1) * 10 ms.
                                                             *   Range: 0x00 to 0x1F. */
} _PACKED4_ rtk_bt_mesh_directed_network_transmit_t;

/**
 * @struct    rtk_bt_mesh_directed_network_transmit_status_t
 * @brief     BLE MESH directed network transmit status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_directed_network_transmit_t network_transmit;    /*!< Current Directed Network Transmit state of the node. */
} _PACKED4_ rtk_bt_mesh_directed_network_transmit_status_t;

/**
 * @struct    rtk_bt_mesh_directed_relay_retransmit_t
 * @brief     BLE MESH directed relay retransmit structure.
 */
typedef struct {
	uint8_t directed_relay_retransmit_count: 3;             /*!< Directed Relay Retransmit Count, controls the number of transmissions of each relayed Network
                                                             *   PDU with CTL equal to 0 that is relayed by a Directed Relay node and secured with directed security
                                                             *   material.
                                                             *   Number of transmissions = count + 1.
                                                             *   Range: 0x00 to 0x07. */
	uint8_t directed_relay_retransmit_interval_steps: 5;    /*!< Directed Relay Retransmit Interval Steps, the number of 10-millisecond steps between
                                                             *   retransmissions of each Network PDU with CTL equal to 0 that is relayed by a Directed Relay node
                                                             *   and secured with directed security material.
                                                             *   Transmission interval = (steps + 1) * 10 ms.
                                                             *   Range: 0x00 to 0x1F. */
} _PACKED4_ rtk_bt_mesh_directed_relay_retransmit_t;

/**
 * @struct    rtk_bt_mesh_directed_relay_retransmit_status_t
 * @brief     BLE MESH directed relay retransmit status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_directed_relay_retransmit_t relay_retransmit;    /*!< Current Directed Relay Retransmit state of the node. */
} _PACKED4_ rtk_bt_mesh_directed_relay_retransmit_status_t;

/**
 * @struct    rtk_bt_mesh_rssi_threshold_t
 * @brief     BLE MESH rssi threshold structure.
 */
typedef struct {
	int8_t default_rssi_threshold;    /*!< Default RSSI Threshold, the default RSSI threshold in dBm.
                                       *   Range: -128 to 127. */
	uint8_t rssi_margin;              /*!< RSSI Margin, the margin in dB above the default RSSI threshold for which the link quality with a neighbor node is acceptable.
                                       *   0x00 to 0x32: Margin above the default RSSI threshold, in dB.
                                       *   0x33 to 0xFF: Prohibited. */
} _PACKED4_ rtk_bt_mesh_rssi_threshold_t;

/**
 * @struct    rtk_bt_mesh_rssi_threshold_status_t
 * @brief     BLE MESH rssi threshold status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_rssi_threshold_t threshold;    /*!< Current RSSI Threshold state of the node. */
} _PACKED4_ rtk_bt_mesh_rssi_threshold_status_t;

/**
 * @struct    rtk_bt_mesh_directed_paths_status_t
 * @brief     BLE MESH directed paths status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_directed_paths_t directed_paths;    /*!< Current Directed Paths state of the node. */
} _PACKED4_ rtk_bt_mesh_directed_paths_status_t;

/**
 * @enum      rtk_bt_mesh_directed_publish_policy_t
 * @brief     BLE MESH directed publish policy structure.
 */
enum {
	RTK_BT_MESH_PUBLISH_POLICY_MANAGED_FLOODING,       /*!< Managed Flooding, the policy is to use managed flooding when publishing. */
	RTK_BT_MESH_PUBLISH_POLICY_DIRECTED_FORWARDING,    /*!< Directed Forwarding, the policy is to use directed forwarding when publishing. */
} _SHORT_ENUM_;
typedef uint8_t rtk_bt_mesh_directed_publish_policy_t;

/**
 * @struct    rtk_bt_mesh_directed_publish_policy_status_t
 * @brief     BLE MESH directed publish policy status evt message structure.
 */
typedef struct {
	uint8_t status;                                          /*!< Status code for the requesting message, see @ref rtk_bt_mesh_msg_stat_t. */
	rtk_bt_mesh_directed_publish_policy_t publish_policy;    /*!< Current Directed Publish Policy state of the model. */
	uint16_t element_addr;                                   /*!< Unicast address of the element that contains the model. */
	uint32_t model_id;                                       /*!< Identifier of the model.
                                                              *   A SIG model: the low 16 bits are 0xFFFF and the high 16 bits are the SIG Model ID.
                                                              *   A vendor model: the low 16 bits are the Company ID and the high 16 bits are the Vendor Model ID. */
} _PACKED4_ rtk_bt_mesh_directed_publish_policy_status_t;

/**
 * @struct    rtk_bt_mesh_path_discovery_timing_control_status_t
 * @brief     BLE MESH path discovery timing control status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_path_discovery_timing_control_t timing_ctl;    /*!< Current Path Discovery Timing Control state of the node. */
} _PACKED4_ rtk_bt_mesh_path_discovery_timing_control_status_t;

/**
 * @struct    rtk_bt_mesh_directed_control_network_transmit_t
 * @brief     BLE MESH directed control network transmit structure.
 */
typedef struct {
	uint8_t directed_control_network_transmit_count: 3;             /*!< Directed Control Network Transmit Count, controls the number of transmissions of each
                                                                     *   Network PDU with CTL equal to 1 that originates from either a Path Origin or a Path Target
                                                                     *   and that is secured using directed security material.
                                                                     *   Number of transmissions = count + 1.
                                                                     *   Range: 0x00 to 0x07. */
	uint8_t directed_control_network_transmit_interval_steps: 5;    /*!< Directed Control Network Transmit Interval Steps, the number of 10-millisecond steps
                                                                     *   between transmissions of each Network PDU with CTL equal to 1 that originates from either a Path
                                                                     *   Origin or a Path Target and that is secured using directed security material.
                                                                     *   Transmission interval = (steps + 1) * 10 ms.
                                                                     *   Range: 0x00 to 0x1F. */
} _PACKED4_ rtk_bt_mesh_directed_control_network_transmit_t;

/**
 * @struct    rtk_bt_mesh_directed_control_network_transmit_status_t
 * @brief     BLE MESH directed control network transmit status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_directed_control_network_transmit_t control_network_transmit;    /*!< Current Directed Control Network Transmit state of the node. */
} _PACKED4_ rtk_bt_mesh_directed_control_network_transmit_status_t;

/**
 * @struct    rtk_bt_mesh_directed_control_relay_retransmit_t
 * @brief     BLE MESH directed control relay_retransmit structure.
 */
typedef struct {
	uint8_t directed_control_relay_retransmit_count: 3;             /*!< Directed Control Relay Retransmit Count, controls the number of transmissions of each
                                                                     *   relayed Network PDU with CTL equal to 1 that is relayed by a Directed Relay node and secured
                                                                     *   with directed security material.
                                                                     *   Number of transmissions = count + 1.
                                                                     *   Range: 0x00 to 0x07. */
	uint8_t directed_control_relay_retransmit_interval_steps: 5;    /*!< Directed Control Relay Retransmit Interval Steps, the number of 10-millisecond steps
                                                                     *   between retransmissions of each Network PDU with CTL equal to 1 that is relayed by a Directed
                                                                     *   Relay node and secured with directed security material.
                                                                     *   Transmission interval = (steps + 1) * 10 ms.
                                                                     *   Range: 0x00 to 0x1F. */
} _PACKED4_ rtk_bt_mesh_directed_control_relay_retransmit_t;

/**
 * @struct    rtk_bt_mesh_directed_control_relay_retransmit_status_t
 * @brief     BLE MESH directed control relay_retransmit status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_directed_control_relay_retransmit_t control_relay_retransmit;    /*!< Current Directed Control Relay Retransmit state of the node. */
} _PACKED4_ rtk_bt_mesh_directed_control_relay_retransmit_status_t;

/**
 * @name      Directed Control Parameter Setting Bits
 * @brief     Bits of bit_check of @ref rtk_bt_mesh_directed_control_param_setting_t, each bit selects one parameter to set.
 * @{
 */
#define DF_NETWORK_TRANSMIT_SETTING             0x0001    /*!< Set the Directed Network Transmit state, uses network_transmit. */
#define DF_RELAY_TRANSMIT_SETTING               0x0002    /*!< Set the Directed Relay Retransmit state, uses relay_retransmit. */
#define DF_CTL_NETWORK_TRANSMIT_SETTING         0x0004    /*!< Set the Directed Control Network Transmit state, uses control_network_transmit. */
#define DF_CTL_RELAY_TRANSMIT_SETTING           0x0008    /*!< Set the Directed Control Relay Retransmit state, uses control_relay_retransmit. */
#define DF_TIMING_CTL_SETTING                   0x0010    /*!< Set the Path Discovery Timing Control state, uses timing_ctl. */
#define DF_RSSI_SETTING                         0x0020    /*!< Set the RSSI Threshold state, uses threshold. */
#define DF_DIRECTED_PATHS_SETTING               0x0040    /*!< Set the Directed Paths state, uses directed_paths. */
#define DF_DEPENDENT_ADDR_SIZE_SETTING          0x0080    /*!< Set the maximum number of dependent node addresses per path, uses dependent_addr_size. */
#define DF_FIXED_PATH_SIZE_SETTING              0x0100    /*!< Set the maximum number of fixed paths, uses df_fixed_path_size. */
#define DF_NON_FIXED_PATH_SIZE_SETTING          0x0200    /*!< Set the maximum number of non-fixed paths, uses df_non_fixed_path_size. */
/** @} */

/**
 * @struct    rtk_bt_mesh_directed_control_param_setting_t
 * @brief     BLE MESH directed control parameter setting message structure.
 */
typedef struct {
	uint16_t bit_check;                                                          /*!< Bit mask of the parameters to set, only the parameters whose bit is set to 1 are
                                                                                  *   set.
                                                                                  *   Bit 0: network_transmit.
                                                                                  *   Bit 1: relay_retransmit.
                                                                                  *   Bit 2: control_network_transmit.
                                                                                  *   Bit 3: control_relay_retransmit.
                                                                                  *   Bit 4: timing_ctl.
                                                                                  *   Bit 5: threshold.
                                                                                  *   Bit 6: directed_paths.
                                                                                  *   Bit 7: dependent_addr_size.
                                                                                  *   Bit 8: df_fixed_path_size.
                                                                                  *   Bit 9: df_non_fixed_path_size. */
	rtk_bt_mesh_directed_network_transmit_t network_transmit;                    /*!< Directed Network Transmit state to set. */
	rtk_bt_mesh_directed_relay_retransmit_t relay_retransmit;                    /*!< Directed Relay Retransmit state to set. */
	rtk_bt_mesh_directed_control_network_transmit_t control_network_transmit;    /*!< Directed Control Network Transmit state to set. */
	rtk_bt_mesh_directed_control_relay_retransmit_t control_relay_retransmit;    /*!< Directed Control Relay Retransmit state to set. */
	rtk_bt_mesh_path_discovery_timing_control_t timing_ctl;                      /*!< Path Discovery Timing Control state to set. */
	rtk_bt_mesh_rssi_threshold_t threshold;                                      /*!< RSSI Threshold state to set. */
	rtk_bt_mesh_directed_paths_t directed_paths;                                 /*!< Directed Paths state to set. */
	uint16_t dependent_addr_size;                                                /*!< Maximum number of dependent node addresses per path. */
	uint16_t df_fixed_path_size;                                                 /*!< Maximum number of fixed paths. */
	uint16_t df_non_fixed_path_size;                                             /*!< Maximum number of non-fixed paths. */
} _PACKED4_ rtk_bt_mesh_directed_control_param_setting_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  ble_mesh_directed_forwarding_model BT LE Mesh Directed Forwarding Model APIs
 * @brief     BT LE Mesh Directed Forwarding Model related function APIs
 * @ingroup   ble_mesh_api_group
 * @{
 */

/**
 * @brief     Send a Directed Control Get message to a Directed Forwarding Configuration Server to read the Directed Control state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_STAT.
 * @param[in] control_get: Directed Control Get message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_directed_control_get(rtk_bt_mesh_df_client_act_directed_control_get_t *control_get);

/**
 * @brief     Send a Directed Control Set message to a Directed Forwarding Configuration Server to set the Directed Control state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_STAT.
 * @param[in] control_set: Directed Control Set message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_directed_control_set(rtk_bt_mesh_df_client_act_directed_control_set_t *control_set);

/**
 * @brief     Send a Path Metric Get message to a Directed Forwarding Configuration Server to read the Path Metric state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_PATH_METRIC_STAT.
 * @param[in] path_metric_get: Path Metric Get message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_path_metric_get(rtk_bt_mesh_df_client_act_path_metric_get_t *path_metric_get);

/**
 * @brief     Send a Path Metric Set message to a Directed Forwarding Configuration Server to set the Path Metric state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_PATH_METRIC_STAT.
 * @param[in] path_metric_set: Path Metric Set message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_path_metric_set(rtk_bt_mesh_df_client_act_path_metric_set_t *path_metric_set);

/**
 * @brief     Send a Discovery Table Capabilities Get message to a Directed Forwarding Configuration Server.
 *            It is used to read the Discovery Table Capabilities state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_STAT.
 * @param[in] dis_tab_cap_get: Discovery Table Capabilities Get message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_discovery_table_capabilities_get(rtk_bt_mesh_df_client_act_discovery_table_capabilities_get_t *dis_tab_cap_get);

/**
 * @brief     Send a Discovery Table Capabilities Set message to a Directed Forwarding Configuration Server to set the Max Concurrent Init state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_STAT.
 * @param[in] dis_tab_cap_set: Discovery Table Capabilities Set message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_discovery_table_capabilities_set(rtk_bt_mesh_df_client_act_discovery_table_capabilities_set_t *dis_tab_cap_set);

/**
 * @brief     Send a Forwarding Table Add message to a Directed Forwarding Configuration Server to add or update a fixed path entry of its Forwarding Table.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_STAT.
 * @param[in] for_tab_add: Forwarding Table Add message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_forwarding_table_add(rtk_bt_mesh_df_client_act_forwarding_table_add_t *for_tab_add);

/**
 * @brief     Send a Forwarding Table Delete message to a Directed Forwarding Configuration Server.
 *            It is used to delete all the path entries from a Path Origin to a destination from the Forwarding Table state of the server.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_STAT.
 * @param[in] for_tab_del: Forwarding Table Delete message parameters.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_forwarding_table_delete(rtk_bt_mesh_df_client_act_forwarding_table_delete_t *for_tab_del);

/**
 * @brief     Send a Forwarding Table Dependents Add message to a Directed Forwarding Configuration Server.
 *            It is used to add dependent node entries to a fixed path entry of its Forwarding Table.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_STAT.
 * @param[in] for_tab_dep_add: Forwarding Table Dependents Add message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_forwarding_table_dependents_add(rtk_bt_mesh_df_client_act_forwarding_table_dependents_add_t *for_tab_dep_add);

/**
 * @brief     Send a Forwarding Table Dependents Delete message to a Directed Forwarding Configuration Server.
 *            It is used to delete dependent node entries from a fixed path entry of its Forwarding Table.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_STAT.
 * @param[in] for_tab_dep_del: Forwarding Table Dependents Delete message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_forwarding_table_dependents_delete(rtk_bt_mesh_df_client_act_forwarding_table_dependents_delete_t *for_tab_dep_del);

/**
 * @brief     Send a Forwarding Table Dependents Get message to a Directed Forwarding Configuration Server to read the dependent nodes of a Forwarding Table entry.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_GET_STAT.
 * @param[in] for_tab_dep_get: Forwarding Table Dependents Get message parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_forwarding_table_dependents_get(rtk_bt_mesh_df_client_act_forwarding_table_dependents_get_t *for_tab_dep_get);

/**
 * @brief     Send a Forwarding Table Entries Count Get message to a Directed Forwarding Configuration Server to read the information about its Forwarding Table.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_COUNT_STAT.
 * @param[in] for_tab_entries_count_get: Forwarding Table Entries Count Get message parameters.
 *                                       Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_forwarding_table_entries_count_get(rtk_bt_mesh_df_client_act_forwarding_table_entries_count_get_t *for_tab_entries_count_get);

/**
 * @brief     Send a Forwarding Table Entries Get message to a Directed Forwarding Configuration Server to read a filtered set of its Forwarding Table entries.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_STAT.
 * @param[in] for_tab_entries_get: Forwarding Table Entries Get message parameters.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_forwarding_table_entries_get(rtk_bt_mesh_df_client_act_forwarding_table_entries_get_t *for_tab_entries_get);

/**
 * @brief     Send a Wanted Lanes Get message to a Directed Forwarding Configuration Server to read the Wanted Lanes state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_WANTED_LANES_STAT.
 * @param[in] wanted_lanes_get: Wanted Lanes Get message parameters.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_wanted_lanes_get(rtk_bt_mesh_df_client_act_wanted_lanes_get_t *wanted_lanes_get);

/**
 * @brief     Send a Wanted Lanes Set message to a Directed Forwarding Configuration Server to set the Wanted Lanes state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_WANTED_LANES_STAT.
 * @param[in] wanted_lanes_set: Wanted Lanes Set message parameters.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_wanted_lanes_set(rtk_bt_mesh_df_client_act_wanted_lanes_set_t *wanted_lanes_set);

/**
 * @brief     Send a Two Way Path Get message to a Directed Forwarding Configuration Server to read the Two Way Path state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_STAT.
 * @param[in] two_way_path_get: Two Way Path Get message parameters.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_two_way_path_get(rtk_bt_mesh_df_client_act_two_way_path_get_t *two_way_path_get);

/**
 * @brief     Send a Two Way Path Set message to a Directed Forwarding Configuration Server to set the Two Way Path state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_STAT.
 * @param[in] two_way_path_set: Two Way Path Set message parameters.
 *                              Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_two_way_path_set(rtk_bt_mesh_df_client_act_two_way_path_set_t *two_way_path_set);

/**
 * @brief     Send a Path Echo Interval Get message to a Directed Forwarding Configuration Server to read the Path Echo Interval state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_STAT.
 * @param[in] path_echo_interval_get: Path Echo Interval Get message parameters.
 *                                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_path_echo_interval_get(rtk_bt_mesh_df_client_act_path_echo_interval_get_t *path_echo_interval_get);

/**
 * @brief     Send a Path Echo Interval Set message to a Directed Forwarding Configuration Server to set the Path Echo Interval state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_STAT.
 * @param[in] path_echo_interval_set: Path Echo Interval Set message parameters.
 *                                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_path_echo_interval_set(rtk_bt_mesh_df_client_act_path_echo_interval_set_t *path_echo_interval_set);

/**
 * @brief     Send a Directed Network Transmit Get message to a Directed Forwarding Configuration Server to read its Directed Network Transmit state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_STAT.
 * @param[in] net_trans_get: Directed Network Transmit Get message parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_directed_network_transmit_get(rtk_bt_mesh_df_client_act_directed_network_transmit_get_t *net_trans_get);

/**
 * @brief     Send a Directed Network Transmit Set message to a Directed Forwarding Configuration Server to set its Directed Network Transmit state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_STAT.
 * @param[in] net_trans_set: Directed Network Transmit Set message parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_directed_network_transmit_set(rtk_bt_mesh_df_client_act_directed_network_transmit_set_t *net_trans_set);

/**
 * @brief     Send a Directed Relay Retransmit Get message to a Directed Forwarding Configuration Server to read its Directed Relay Retransmit state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_STAT.
 * @param[in] relay_retrans_get: Directed Relay Retransmit Get message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_directed_relay_retransmit_get(rtk_bt_mesh_df_client_act_directed_relay_retransmit_get_t *relay_retrans_get);

/**
 * @brief     Send a Directed Relay Retransmit Set message to a Directed Forwarding Configuration Server to set its Directed Relay Retransmit state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_STAT.
 * @param[in] relay_retrans_set: Directed Relay Retransmit Set message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_directed_relay_retransmit_set(rtk_bt_mesh_df_client_act_directed_relay_retransmit_set_t *relay_retrans_set);

/**
 * @brief     Send a RSSI Threshold Get message to a Directed Forwarding Configuration Server to read the RSSI Threshold state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_STAT.
 * @param[in] rssi_get: RSSI Threshold Get message parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_rssi_threshold_get(rtk_bt_mesh_df_client_act_rssi_threshold_get_t *rssi_get);

/**
 * @brief     Send a RSSI Threshold Set message to a Directed Forwarding Configuration Server to set the RSSI Margin state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_STAT.
 * @param[in] rssi_set: RSSI Threshold Set message parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_rssi_threshold_set(rtk_bt_mesh_df_client_act_rssi_threshold_set_t *rssi_set);

/**
 * @brief     Send a Directed Paths Get message to a Directed Forwarding Configuration Server to read the Directed Paths state of a subnet.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_PATHS_STAT.
 * @param[in] path_get: Directed Paths Get message parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_directed_paths_get(rtk_bt_mesh_df_client_act_directed_paths_get_t *path_get);

/**
 * @brief     Send a Directed Publish Policy Get message to a Directed Forwarding Configuration Server to read the Directed Publish Policy state of a model.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_STAT.
 * @param[in] policy_get: Directed Publish Policy Get message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_directed_publish_policy_get(rtk_bt_mesh_df_client_act_directed_publish_policy_get_t *policy_get);

/**
 * @brief     Send a Directed Publish Policy Set message to a Directed Forwarding Configuration Server to set the Directed Publish Policy state of a model.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_STAT.
 * @param[in] policy_set: Directed Publish Policy Set message parameters.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_directed_publish_policy_set(rtk_bt_mesh_df_client_act_directed_publish_policy_set_t *policy_set);

/**
 * @brief     Send a Path Discovery Timing Control Get message to a Directed Forwarding Configuration Server to read its Path Discovery Timing Control state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_STAT.
 * @param[in] timing_ctl_get: Path Discovery Timing Control Get message parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_discovery_timing_control_get(rtk_bt_mesh_df_client_act_path_discovery_timing_control_get_t *timing_ctl_get);

/**
 * @brief     Send a Path Discovery Timing Control Set message to a Directed Forwarding Configuration Server to set its Path Discovery Timing Control state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_STAT.
 * @param[in] timing_ctl_set: Path Discovery Timing Control Set message parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_discovery_timing_control_set(rtk_bt_mesh_df_client_act_path_discovery_timing_control_set_t *timing_ctl_set);

/**
 * @brief     Send a Directed Control Network Transmit Get message to a Directed Forwarding Configuration Server.
 *            It is used to read its Directed Control Network Transmit state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_STAT.
 * @param[in] ctl_net_trans_get: Directed Control Network Transmit Get message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_directed_control_network_transmit_get(rtk_bt_mesh_df_client_act_directed_control_network_transmit_get_t *ctl_net_trans_get);

/**
 * @brief     Send a Directed Control Network Transmit Set message to a Directed Forwarding Configuration Server to set its Directed Control Network Transmit state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_STAT.
 * @param[in] ctl_net_trans_set: Directed Control Network Transmit Set message parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_directed_control_network_transmit_set(rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t *ctl_net_trans_set);

/**
 * @brief     Send a Directed Control Relay Retransmit Get message to a Directed Forwarding Configuration Server.
 *            It is used to read its Directed Control Relay Retransmit state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_STAT.
 * @param[in] ctl_relay_retrans_get: Directed Control Relay Retransmit Get message parameters.
 *                                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_directed_control_relay_retransmit_get(rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_get_t *ctl_relay_retrans_get);

/**
 * @brief     Send a Directed Control Relay Retransmit Set message to a Directed Forwarding Configuration Server to set its Directed Control Relay Retransmit state.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_STAT.
 * @param[in] ctl_relay_retrans_set: Directed Control Relay Retransmit Set message parameters.
 *                                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_directed_control_relay_retransmit_set(rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_set_t *ctl_relay_retrans_set);

/**
 * @brief     Start a directed forwarding path discovery on this node towards a target address.
 * @param[in] path_discovery: Path discovery parameters.
 *                            Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_path_discovery(rtk_bt_mesh_df_act_df_path_discovery_t *path_discovery);

/**
 * @brief     Start a directed forwarding path solicitation on this node for a list of addresses.
 * @param[in] path_solicitation: Path solicitation parameters.
 *                               Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_path_solicitation(rtk_bt_mesh_df_act_df_path_solicitation_t *path_solicitation);

/**
 * @brief     Update the dependent nodes of a path endpoint on this node.
 * @param[in] path_dep_update: Path dependents update parameters.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_path_dependents_update(rtk_bt_mesh_df_act_df_path_dependents_update_t *path_dep_update);

/************************************ Param Setting ************************************/

/**
 * @brief     Read the Directed Control state of a subnet on this node.
 * @param[in,out] ctl: Subnet to read (master_key_index) and the Directed Control state read from it.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_control_state_get(rtk_bt_mesh_df_control_state_setting_t *ctl);

/**
 * @brief     Set the Directed Control state of a subnet on this node.
 * @param[in] ctl: Subnet (master_key_index) and the Directed Control state to set.
 *                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_control_state_set(rtk_bt_mesh_df_control_state_setting_t *ctl);

/**
 * @brief     Read the Path Metric state of a subnet on this node.
 * @param[in,out] pm: Subnet to read (master_key_index) and the Path Metric state read from it.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_path_metric_state_get(rtk_bt_mesh_df_path_metric_state_setting_t *pm);

/**
 * @brief     Set the Path Metric state of a subnet on this node.
 * @param[in] pm: Subnet (master_key_index) and the Path Metric state to set.
 *                Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_path_metric_state_set(rtk_bt_mesh_df_path_metric_state_setting_t *pm);

/**
 * @brief     Read the Discovery Table Capabilities state of a subnet on this node.
 * @param[in,out] dtcs: Subnet to read (master_key_index) and the Discovery Table Capabilities state read from it.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_discovery_table_capabilities_state_get(rtk_bt_mesh_df_discovery_table_cap_state_setting_t *dtcs);

/**
 * @brief     Set the Discovery Table Capabilities state of a subnet on this node.
 * @param[in] dtcs: Subnet (master_key_index) and the Discovery Table Capabilities state to set.
 *                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_discovery_table_capabilities_state_set(rtk_bt_mesh_df_discovery_table_cap_state_setting_t *dtcs);

/**
 * @defgroup  RTK_BT_MESH_DF_DISCOVERY_TABLE_CAPABILITIES Deprecated aliases
 * @brief     Deprecated aliases of the Discovery Table Capabilities state APIs.
 * @deprecated Use @ref rtk_bt_mesh_df_discovery_table_capabilities_state_get and
 *             @ref rtk_bt_mesh_df_discovery_table_capabilities_state_set instead.
 * @{
 */
#define rtk_bt_mesh_df_discovery_table_capablities_state_get rtk_bt_mesh_df_discovery_table_capabilities_state_get
#define rtk_bt_mesh_df_discovery_table_capablities_state_set rtk_bt_mesh_df_discovery_table_capabilities_state_set
/** @} */

/**
 * @brief     Read the Wanted Lanes state of a subnet on this node.
 * @param[in,out] wls: Subnet to read (master_key_index) and the Wanted Lanes state read from it.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_wanted_lanes_state_get(rtk_bt_mesh_df_wanted_lanes_state_setting_t *wls);

/**
 * @brief     Set the Wanted Lanes state of a subnet on this node.
 * @param[in] wls: Subnet (master_key_index) and the Wanted Lanes state to set.
 *                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_wanted_lanes_state_set(rtk_bt_mesh_df_wanted_lanes_state_setting_t *wls);

/**
 * @brief     Read the Two Way Path state of a subnet on this node.
 * @param[in,out] twps: Subnet to read (master_key_index) and the Two Way Path state read from it.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_two_way_path_state_get(rtk_bt_mesh_df_two_way_path_state_setting_t *twps);

/**
 * @brief     Set the Two Way Path state of a subnet on this node.
 * @param[in] twps: Subnet (master_key_index) and the Two Way Path state to set.
 *                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_two_way_path_state_set(rtk_bt_mesh_df_two_way_path_state_setting_t *twps);

/**
 * @brief     Read the Path Echo Interval state of a subnet on this node.
 * @param[in,out] peis: Subnet to read (master_key_index) and the Path Echo Interval state read from it.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_path_echo_interval_state_get(rtk_bt_mesh_df_echo_interval_state_setting_t *peis);

/**
 * @brief     Set the Path Echo Interval state of a subnet on this node.
 * @param[in] peis: Subnet (master_key_index) and the Path Echo Interval state to set.
 *                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_path_echo_interval_state_set(rtk_bt_mesh_df_echo_interval_state_setting_t *peis);

/**
 * @brief     Set the Directed Publish Policy state of a model of this node.
 * @param[in] policy: Model (element_index, model_id) and the Directed Publish Policy state to set.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_publish_policy_set(rtk_bt_mesh_df_publish_policy_setting_t *policy);

/**
 * @brief     Set the directed forwarding parameters of this node.
 * @param[in] param: Directed forwarding parameters to set.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_df_parameter_set(rtk_bt_mesh_directed_control_param_setting_t *param);

/**
 * @}
 */

#endif /* __RTK_BT_MESH_DIRECTED_FORWARDING_MODEL_H__ */
