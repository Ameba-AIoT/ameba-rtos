/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     directed_forwarding_model.h
* @brief    Head file for directed forwarding models.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2019-12-12
* @version  v1.0
* *************************************************************************************
*/

#ifndef _DIRECTED_FORWARDING_MODEL_H_
#define _DIRECTED_FORWARDING_MODEL_H_

#include "directed_forwarding.h"

#if MESH_DF

BEGIN_DECLS

/**
 * @addtogroup DIRECTED_FORWARDING_MODEL
 * @{
 */

/**
 * @defgroup DIRECTED_FORWARDING_ACCESS_OPCODE Access Opcode
 * @brief Mesh message access opcode
 * @{
 */
#define MESH_MSG_DIRECTED_CONTROL_GET                           0x807B
#define MESH_MSG_DIRECTED_CONTROL_SET                           0x807C
#define MESH_MSG_DIRECTED_CONTROL_STATUS                        0x807D
#define MESH_MSG_PATH_METRIC_GET                                0x807E
#define MESH_MSG_PATH_METRIC_SET                                0x807F
#define MESH_MSG_PATH_METRIC_STATUS                             0x8080
#define MESH_MSG_DISCOVERY_TABLE_CAPABILITIES_GET               0x8081
#define MESH_MSG_DISCOVERY_TABLE_CAPABILITIES_SET               0x8082
#define MESH_MSG_DISCOVERY_TABLE_CAPABILITIES_STATUS            0x8083
#define MESH_MSG_FORWARDING_TABLE_ADD                           0x8084
#define MESH_MSG_FORWARDING_TABLE_DELETE                        0x8085
#define MESH_MSG_FORWARDING_TABLE_STATUS                        0x8086
#define MESH_MSG_FORWARDING_TABLE_DEPENDENTS_ADD                0x8087
#define MESH_MSG_FORWARDING_TABLE_DEPENDENTS_DELETE             0x8088
#define MESH_MSG_FORWARDING_TABLE_DEPENDENTS_STATUS             0x8089
#define MESH_MSG_FORWARDING_TABLE_DEPENDENTS_GET                0x808A
#define MESH_MSG_FORWARDING_TABLE_DEPENDENTS_GET_STATUS         0x808B
#define MESH_MSG_FORWARDING_TABLE_ENTRIES_COUNT_GET             0x808C
#define MESH_MSG_FORWARDING_TABLE_ENTRIES_COUNT_STATUS          0x808D
#define MESH_MSG_FORWARDING_TABLE_ENTRIES_GET                   0x808E
#define MESH_MSG_FORWARDING_TABLE_ENTRIES_STATUS                0x808F
#define MESH_MSG_WANTED_LANES_GET                               0x8090
#define MESH_MSG_WANTED_LANES_SET                               0x8091
#define MESH_MSG_WANTED_LANES_STATUS                            0x8092
#define MESH_MSG_TWO_WAY_PATH_GET                               0x8093
#define MESH_MSG_TWO_WAY_PATH_SET                               0x8094
#define MESH_MSG_TWO_WAY_PATH_STATUS                            0x8095
#define MESH_MSG_PATH_ECHO_INTERVAL_GET                         0x8096
#define MESH_MSG_PATH_ECHO_INTERVAL_SET                         0x8097
#define MESH_MSG_PATH_ECHO_INTERVAL_STATUS                      0x8098
#define MESH_MSG_DIRECTED_NETWORK_TRANSMIT_GET                  0x8099
#define MESH_MSG_DIRECTED_NETWORK_TRANSMIT_SET                  0x809A
#define MESH_MSG_DIRECTED_NETWORK_TRANSMIT_STATUS               0x809B
#define MESH_MSG_DIRECTED_RELAY_RETRANSMIT_GET                  0x809C
#define MESH_MSG_DIRECTED_RELAY_RETRANSMIT_SET                  0x809D
#define MESH_MSG_DIRECTED_RELAY_RETRANSMIT_STATUS               0x809E
#define MESH_MSG_RSSI_THRESHOLD_GET                             0x809F
#define MESH_MSG_RSSI_THRESHOLD_SET                             0x80A0
#define MESH_MSG_RSSI_THRESHOLD_STATUS                          0x80A1
#define MESH_MSG_DIRECTED_PATHS_GET                             0x80A2
#define MESH_MSG_DIRECTED_PATHS_STATUS                          0x80A3
#define MESH_MSG_DIRECTED_PUBLISH_POLICY_GET                    0x80A4
#define MESH_MSG_DIRECTED_PUBLISH_POLICY_SET                    0x80A5
#define MESH_MSG_DIRECTED_PUBLISH_POLICY_STATUS                 0x80A6
#define MESH_MSG_PATH_DISCOVERY_TIMING_CONTROL_GET              0x80A7
#define MESH_MSG_PATH_DISCOVERY_TIMING_CONTROL_SET              0x80A8
#define MESH_MSG_PATH_DISCOVERY_TIMING_CONTROL_STATUS           0x80A9
#define MESH_MSG_DIRECTED_CONTROL_NETWORK_TRANSMIT_GET          0x80AB
#define MESH_MSG_DIRECTED_CONTROL_NETWORK_TRANSMIT_SET          0x80AC
#define MESH_MSG_DIRECTED_CONTROL_NETWORK_TRANSMIT_STATUS       0x80AD
#define MESH_MSG_DIRECTED_CONTROL_RELAY_RETRANSMIT_GET          0x80AE
#define MESH_MSG_DIRECTED_CONTROL_RELAY_RETRANSMIT_SET          0x80AF
#define MESH_MSG_DIRECTED_CONTROL_RELAY_RETRANSMIT_STATUS       0x80B0
/** @} */

/**
 * @defgroup DIRECTED_FORWARDING_MODEL_ID Model ID
 * @brief Mesh model id
 * @{
 */
#define MESH_MODEL_DIRECTED_FORWARDING_SERVER           0x0006FFFF
#define MESH_MODEL_DIRECTED_FORWARDING_CLIENT           0x0007FFFF
/** @} */

/**
 * @defgroup DIRECTED_FORWARDING_MESH_MSG Mesh Msg
 * @brief Mesh message types used by models
 * @{
 */
typedef struct
{
    uint16_t fixed_path_flag : 1;
    uint16_t unicast_dst_flag : 1;
    uint16_t backward_path_validated_flag : 1;
    uint16_t bearer_toward_path_origin_indicator : 1;
    uint16_t bearer_toward_path_target_indicator : 1;
    uint16_t dependent_origin_list_size_indicator : 2;
    uint16_t dependent_target_list_size_indicator : 2;
    uint16_t prohibited : 7;
} _PACKED4_ forwarding_table_entry_header_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_GET)];
    uint16_t net_key_index;
} _PACKED4_ directed_control_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_SET)];
    uint16_t net_key_index;
    directed_control_t directed_ctl;
} _PACKED4_ directed_control_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_STATUS)];
    uint8_t status;
    uint16_t net_key_index;
    directed_control_t directed_ctl;
} _PACKED4_ directed_control_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_PATH_METRIC_GET)];
    uint16_t net_key_index;
} _PACKED4_ path_metric_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_PATH_METRIC_SET)];
    uint16_t net_key_index;
    path_metric_t path_metric;
} _PACKED4_ path_metric_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_PATH_METRIC_STATUS)];
    uint8_t status;
    uint16_t net_key_index;
    path_metric_t path_metric;
} _PACKED4_ path_metric_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DISCOVERY_TABLE_CAPABILITIES_GET)];
    uint16_t net_key_index;
} _PACKED4_ discovery_table_capabilities_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DISCOVERY_TABLE_CAPABILITIES_SET)];
    uint16_t net_key_index;
    uint8_t max_concurrent_init;
} _PACKED4_ discovery_table_capabilities_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DISCOVERY_TABLE_CAPABILITIES_STATUS)];
    uint8_t status;
    uint16_t net_key_index;
    discovery_table_capabilities_t capabilities;
} _PACKED4_ discovery_table_capabilities_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_ADD)];
    uint16_t net_key_index: 12;
    uint16_t prohibited: 2;
    uint16_t unicast_dst_flag: 1;
    uint16_t backward_path_valid_flag: 1;
    uint8_t variable_data[10];
} _PACKED4_ forwarding_table_add_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_DELETE)];
    uint16_t net_key_index;
    uint16_t path_origin;
    uint16_t dst;
} _PACKED4_ forwarding_table_delete_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_STATUS)];
    uint8_t status;
    uint16_t net_key_index;
    uint16_t path_origin;
    uint16_t dst;
} _PACKED4_ forwarding_table_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_DEPENDENTS_ADD)];
    uint16_t net_key_index;
    uint16_t path_origin;
    uint16_t dst;
    uint8_t dependent_origin_list_size;
    uint8_t dependent_target_list_size;
    uint8_t unicast_addr_range[0];
} _PACKED4_ forwarding_table_dependents_add_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_DEPENDENTS_DELETE)];
    uint16_t net_key_index;
    uint16_t path_origin;
    uint16_t dst;
    uint8_t dependent_origin_list_size;
    uint8_t dependent_target_list_size;
    uint8_t unicast_addrs[0];
} _PACKED4_ forwarding_table_dependents_delete_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_DEPENDENTS_STATUS)];
    uint8_t status;
    uint16_t net_key_index;
    uint16_t path_origin;
    uint16_t dst;
} _PACKED4_ forwarding_table_dependents_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_ENTRIES_COUNT_GET)];
    uint16_t net_key_index;
} _PACKED4_ forwarding_table_entries_count_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_ENTRIES_COUNT_STATUS)];
    uint8_t status;
    uint16_t net_key_index;
    forwarding_table_entries_count_t count;
} _PACKED4_ forwarding_table_entries_count_status_t;

#define FORWARDING_TABLE_FILTER_MASK_FIXED_PATHS                  0x01
#define FORWARDING_TABLE_FILTER_MASK_NON_FIXED_PATHS              0x02
#define FORWARDING_TABLE_FILTER_MASK_PATH_ORIGIN_MATCH            0x04
#define FORWARDING_TABLE_FILTER_MASK_DST_MATCH                    0x08
typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_ENTRIES_GET)];
    uint16_t net_key_index: 12;
    uint16_t filter_mask: 4;
    uint16_t start_index;
    uint8_t data[0];
} _PACKED4_ forwarding_table_entries_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_ENTRIES_STATUS)];
    uint8_t status;
    uint16_t net_key_index: 12;
    uint16_t filter_mask: 4;
    uint16_t start_index;
    uint8_t data[0];
} _PACKED4_ forwarding_table_entries_status_t;

//RTK porting: Add new structure for call app layer
typedef struct
{
    uint8_t status;
    uint16_t net_key_index: 12;
    uint16_t filter_mask: 4;
    uint16_t start_index;
    uint16_t data_len;
    uint8_t *data;
} _PACKED4_ forwarding_table_entries_status_for_call_t;

#define FORWARDING_TABLE_DEPENDENTS_LIST_MASK_PATH_ORIGIN         0x01
#define FORWARDING_TABLE_DEPENDENTS_LIST_MASK_PATH_TARGET         0x02
typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_DEPENDENTS_GET)];
    uint16_t net_key_index: 12;
    uint16_t dependents_list_mask: 2;
    uint16_t fixed_path_flag: 1;
    uint16_t prohibited: 1;
    uint16_t start_index;
    uint16_t path_origin;
    uint16_t dst;
    uint16_t forwarding_table_update_id;
} _PACKED4_ forwarding_table_dependents_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_DEPENDENTS_GET_STATUS)];
    uint8_t status;
    uint16_t net_key_index: 12;
    uint16_t dependents_list_mask: 2;
    uint16_t fixed_path_flag: 1;
    uint16_t prohibited: 1;
    uint16_t start_index;
    uint16_t path_origin;
    uint16_t dst;
    uint8_t data[0];
} _PACKED4_ forwarding_table_dependents_get_status_t;

//RTK porting: Add new structure for call app layer
typedef struct
{
    uint8_t status;
    uint16_t net_key_index: 12;
    uint16_t dependents_list_mask: 2;
    uint16_t fixed_path_flag: 1;
    uint16_t prohibited: 1;
    uint16_t start_index;
    uint16_t path_origin;
    uint16_t dst;
    uint16_t data_len;
    uint8_t *data;
} _PACKED4_ forwarding_table_dependents_get_status_for_call_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_WANTED_LANES_GET)];
    uint16_t net_key_index;
} _PACKED4_ wanted_lanes_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_WANTED_LANES_SET)];
    uint16_t net_key_index;
    uint8_t wanted_lanes;
} _PACKED4_ wanted_lanes_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_WANTED_LANES_STATUS)];
    uint8_t status;
    uint16_t net_key_index;
    uint8_t wanted_lanes;
} _PACKED4_ wanted_lanes_status_t;

typedef struct
{
    uint8_t two_way_path: 1;
    uint8_t prohibited: 7;
} _PACKED4_ two_way_path_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_TWO_WAY_PATH_GET)];
    uint16_t net_key_index;
} _PACKED4_ two_way_path_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_TWO_WAY_PATH_SET)];
    uint16_t net_key_index;
    two_way_path_t two_way_path;
} _PACKED4_ two_way_path_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_TWO_WAY_PATH_STATUS)];
    uint8_t status;
    uint16_t net_key_index;
    two_way_path_t two_way_path;
} _PACKED4_ two_way_path_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_PATH_ECHO_INTERVAL_GET)];
    uint16_t net_key_index;
} _PACKED4_ path_echo_interval_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_PATH_ECHO_INTERVAL_SET)];
    uint16_t net_key_index;
    uint8_t unicast_echo_interval;
    uint8_t multicast_echo_interval;
} _PACKED4_ path_echo_interval_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_PATH_ECHO_INTERVAL_STATUS)];
    uint8_t status;
    uint16_t net_key_index;
    uint8_t unicast_echo_interval;
    uint8_t multicast_echo_interval;
} _PACKED4_ path_echo_interval_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_NETWORK_TRANSMIT_GET)];
} _PACKED4_ directed_network_transmit_get_t;

typedef struct
{
    uint8_t directed_network_transmit_count: 3;
    uint8_t directed_network_transmit_interval_steps: 5;
} _PACKED4_ directed_network_transmit_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_NETWORK_TRANSMIT_SET)];
    directed_network_transmit_t network_transmit;
} _PACKED4_ directed_network_transmit_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_NETWORK_TRANSMIT_STATUS)];
    directed_network_transmit_t network_transmit;
} _PACKED4_ directed_network_transmit_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_RELAY_RETRANSMIT_GET)];
} _PACKED4_ directed_relay_retransmit_get_t;

typedef struct
{
    uint8_t directed_relay_retransmit_count: 3;
    uint8_t directed_relay_retransmit_interval_steps: 5;
} _PACKED4_ directed_relay_retransmit_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_RELAY_RETRANSMIT_SET)];
    directed_relay_retransmit_t relay_retransmit;
} _PACKED4_ directed_relay_retransmit_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_RELAY_RETRANSMIT_STATUS)];
    directed_relay_retransmit_t relay_retransmit;
} _PACKED4_ directed_relay_retransmit_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_RSSI_THRESHOLD_GET)];
} _PACKED4_ rssi_threshold_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_RSSI_THRESHOLD_SET)];
    uint8_t rssi_margin;
} _PACKED4_ rssi_threshold_set_t;

typedef struct
{
    int8_t default_rssi_threshold;
    uint8_t rssi_margin;
} _PACKED4_ rssi_threshold_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_RSSI_THRESHOLD_STATUS)];
    rssi_threshold_t threshold;
} _PACKED4_ rssi_threshold_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_PATHS_GET)];
} _PACKED4_ directed_paths_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_PATHS_STATUS)];
    directed_paths_t directed_paths;
} _PACKED4_ directed_paths_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_PUBLISH_POLICY_GET)];
    uint16_t element_addr;
    uint32_t model_id;
} _PACKED4_ directed_publish_policy_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_PUBLISH_POLICY_SET)];
    directed_publish_policy_t publish_policy;
    uint16_t element_addr;
    uint32_t model_id;
} _PACKED4_ directed_publish_policy_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_PUBLISH_POLICY_STATUS)];
    uint8_t status;
    directed_publish_policy_t publish_policy;
    uint16_t element_addr;
    uint32_t model_id;
} _PACKED4_ directed_publish_policy_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_PATH_DISCOVERY_TIMING_CONTROL_GET)];
} _PACKED4_ path_discovery_timing_control_get_t;

typedef struct
{
    uint16_t path_monitoring_interval;
    uint16_t path_discovery_retry_interval;
    uint8_t path_discovery_interval : 1;
    uint8_t lane_discovery_guard_interval : 1;
    uint8_t prohibited : 6;
} _PACKED4_ path_discovery_timing_control_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_PATH_DISCOVERY_TIMING_CONTROL_SET)];
    path_discovery_timing_control_t timing_ctl;
} _PACKED4_ path_discovery_timing_control_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_PATH_DISCOVERY_TIMING_CONTROL_STATUS)];
    path_discovery_timing_control_t timing_ctl;
} _PACKED4_ path_discovery_timing_control_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_NETWORK_TRANSMIT_GET)];
} _PACKED4_ directed_control_network_transmit_get_t;

typedef struct
{
    uint8_t directed_control_network_transmit_count : 3;
    uint8_t directed_control_network_transmit_interval_steps: 5;
} _PACKED4_ directed_control_network_transmit_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_NETWORK_TRANSMIT_SET)];
    directed_control_network_transmit_t control_network_transmit;
} _PACKED4_ directed_control_network_transmit_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_NETWORK_TRANSMIT_STATUS)];
    directed_control_network_transmit_t control_network_transmit;
} _PACKED4_ directed_control_network_transmit_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_RELAY_RETRANSMIT_GET)];
} _PACKED4_ directed_control_relay_retransmit_get_t;

typedef struct
{
    uint8_t directed_control_relay_retransmit_count : 3;
    uint8_t directed_control_relay_retransmit_interval_steps: 5;
} _PACKED4_ directed_control_relay_retransmit_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_RELAY_RETRANSMIT_SET)];
    directed_control_relay_retransmit_t control_relay_retransmit;
} _PACKED4_ directed_control_relay_retransmit_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_RELAY_RETRANSMIT_STATUS)];
    directed_control_relay_retransmit_t control_relay_retransmit;
} _PACKED4_ directed_control_relay_retransmit_status_t;

/** @} */

/**
 * @defgroup DIRECTED_FORWARDING_SERVER_API Server API
 * @brief Functions declaration
 * @{
 */

/**
 * @brief register directed forwarding server
 * @param[in] element_index: element index that model in
 * @retval TRUE: register success
 * @retval FALSE: register failed
 */
bool directed_forwarding_server_reg(uint8_t element_index);

/** @} */

/**
 * @defgroup DIRECTED_FORWARDING_CLIENT_API Client API
 * @brief Functions declaration
 * @{
 */

/**
 * @brief register directed forwarding client
 * @param[in] element_index: element index that model in
 * @retval TRUE: register success
 * @retval FALSE: register failed
 */
bool directed_forwarding_client_reg(uint8_t element_index);

/**
 * @brief get directed control
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that control exists
 * @return send status
 */
mesh_msg_send_cause_t directed_control_get(uint16_t dst, uint16_t net_key_index,
                                           uint16_t net_key_index_df);

/**
 * @brief set directed control
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that control need to changed
 * @param[in] pctl: desired directed control value
 * @return send status
 */
mesh_msg_send_cause_t directed_control_set(uint16_t dst, uint16_t net_key_index,
                                           uint16_t net_key_index_df, directed_control_t ctl);

/**
 * @brief get path metric
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that path metric exists
 * @return send status
 */
mesh_msg_send_cause_t path_metric_get(uint16_t dst, uint16_t net_key_index,
                                      uint16_t net_key_index_df);

/**
 * @brief set path metric
 * @param[in] dst: remote address
 * @param[in] app_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that path metric need to changed
 * @param[in] metric: desired path metric value
 * @return send status
 */
mesh_msg_send_cause_t path_metric_set(uint16_t dst, uint16_t net_key_index,
                                      uint16_t net_key_index_df,
                                      path_metric_t metric);

/**
 * @brief get discovery table capabilities
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that capabilities exists
 * @return send status
 */
mesh_msg_send_cause_t discovery_table_capabilities_get(uint16_t dst, uint16_t net_key_index,
                                                       uint16_t net_key_index_df);

/**
 * @brief set discovery table capabilities
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that capabilities need to changed
 * @param[in] max_concurrent_int: desired max concurrent init value
 * @return send status
 */
mesh_msg_send_cause_t discovery_table_capabilities_set(uint16_t dst, uint16_t net_key_index,
                                                       uint16_t net_key_index_df, uint8_t max_concurrent_init);

/**
 * @brief add forwarding table
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that forwarding table need to changed
 * @param[in] ptable_param: forwarding table need to add
 * @return send status
 */
mesh_msg_send_cause_t forwarding_table_add(uint16_t dst, uint16_t net_key_index,
                                           uint16_t net_key_index_df, const forwarding_table_param_t *ptable_param);

/**
 * @brief delete forwarding table
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that forwarding table need to changed
 * @param[in] path_origin: path origin address in forwarding table
 * @param[in] dst: destination address in forwarding table
 * @return send status
 */
mesh_msg_send_cause_t forwarding_table_delete(uint16_t dst, uint16_t net_key_index,
                                              uint16_t net_key_index_df, uint16_t path_origin, uint16_t path_target);

/**
 * @brief add forwarding table dependents
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that forwarding table dependents need to changed
 * @param[in] pdepend_param: forwarding table dependents need to add
 * @param[in] addr_len: address array length
 * @return send status
 */
mesh_msg_send_cause_t forwarding_table_dependents_add(uint16_t dst, uint16_t net_key_index,
                                                      uint16_t net_key_index_df, const forwarding_table_dependents_add_param_t *pdepend_param);

/**
 * @brief delete forwarding table dependents
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that forwarding table dependents to changed
 * @param[in] path_origin: path origin address in forwarding table dependents
 * @param[in] dst: destination address in forwarding table dependents
 * @return send status
 */
mesh_msg_send_cause_t forwarding_table_dependents_delete(uint16_t dst, uint16_t net_key_index,
                                                         uint16_t net_key_index_df, const forwarding_table_dependents_del_param_t *pdepend_param);

/**
 * @brief get forwarding table dependents
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that forwarding table dependents to changed
 * @param[in] dependents_list_mask: Filter applied to the lists of unicast address ranges for dependent nodes
 * @param[in] fixed_path_flag: Flag indicating whether or not to return the unicast address ranges
 * @param[in] start_index: Start offset in units of unicast address ranges
 * @param[in] path_origin: Primary element address of the Path Origin
 * @param[in] path_target: Destination address
 * @param[in] update_id_exist: Whether or not Forwarding Table Update Counter state exists
 * @param[in] update_id: Last saved Forwarding Table Update Counter state
 * @return send status
 */
mesh_msg_send_cause_t forwarding_table_dependents_get(uint16_t dst, uint16_t net_key_index,
                                                      uint16_t net_key_index_df, uint8_t dependents_list_mask,
                                                      uint8_t fixed_path_flag, uint16_t start_index,
                                                      uint16_t path_origin, uint16_t path_target,
                                                      bool update_id_exist, uint16_t update_id);

/**
 * @brief get forwarding table entries count
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that forwarding table exists
 * @return send status
 */
mesh_msg_send_cause_t forwarding_table_entries_count_get(uint16_t dst, uint16_t net_key_index,
                                                         uint16_t net_key_index_df);
/**
 * @brief get forwarding table entries
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that forwarding table exists
 * @param[in] filter_mask: filter mask
 * @param[in] start_index: start index
 * @param[in] pdata: variable data
 * @param[in] data_len: length of variable data
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t forwarding_table_entries_get(uint16_t dst, uint16_t net_key_index,
                                                   uint16_t net_key_index_df, uint8_t filter_mask, uint16_t start_index, uint8_t *pdata,
                                                   uint16_t data_len);
/**
 * @brief get wanted lanes
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that wanted lanes exists
 * @return send status
 */
mesh_msg_send_cause_t wanted_lanes_get(uint16_t dst, uint16_t net_key_index,
                                       uint16_t net_key_index_df);

/**
 * @brief set wanted lanes
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that wanted lanes exists
 * @param[in] wanted_lanes: wanted lanes value
 * @return send status
 */
mesh_msg_send_cause_t wanted_lanes_set(uint16_t dst, uint16_t net_key_index,
                                       uint16_t net_key_index_df, uint8_t wanted_lanes);

/**
 * @brief get two way path
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that two way path exists
 * @return send status
 */
mesh_msg_send_cause_t two_way_path_get(uint16_t dst, uint16_t net_key_index,
                                       uint16_t net_key_index_df);

/**
 * @brief set two way path
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that two way path exists
 * @param[in] two_way_path: two way path value
 * @return send status
 */
mesh_msg_send_cause_t two_way_path_set(uint16_t dst, uint16_t net_key_index,
                                       uint16_t net_key_index_df, uint8_t two_way_path);

/**
 * @brief get path echo interval
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that two way path exists
 * @return send status
 */
mesh_msg_send_cause_t path_echo_interval_get(uint16_t dst, uint16_t net_key_index,
                                             uint16_t net_key_index_df);

/**
 * @brief set path echo interval
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_key_index_df: network that two way path exists
 * @param[in] unicast_echo_interval: unicast echo interval value
 * @param[in] multicast_echo_interval: multicast echo interval value
 * @return send status
 */
mesh_msg_send_cause_t path_echo_interval_set(uint16_t dst, uint16_t net_key_index,
                                             uint16_t net_key_index_df, uint8_t unicast_echo_interval,
                                             uint8_t multicast_echo_interval);

/**
 * @brief get directed network transmit
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @return send status
 */
mesh_msg_send_cause_t directed_network_transmit_get(uint16_t dst, uint16_t net_key_index);

/**
 * @brief set directed network transmit
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] count: retrans count
 * @param[in] interval_steps: (interval_steps + 1) * 10ms
 * @return send status
 */
mesh_msg_send_cause_t directed_network_transmit_set(uint16_t dst, uint16_t net_key_index,
                                                    uint8_t count, uint8_t interval_steps);

/**
 * @brief get directed relay retransmit
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @return send status
 */
mesh_msg_send_cause_t directed_relay_retransmit_get(uint16_t dst, uint16_t net_key_index);

/**
 * @brief set directed relay retransmit
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] count: retrans count
 * @param[in] interval_steps: (interval_steps + 1) * 10ms
 * @return send status
 */
mesh_msg_send_cause_t directed_relay_retransmit_set(uint16_t dst, uint16_t net_key_index,
                                                    uint8_t count, uint8_t interval_steps);

/**
 * @brief get rssi threshold
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @return send status
 */
mesh_msg_send_cause_t rssi_threshold_get(uint16_t dst, uint16_t net_key_index);

/**
 * @brief set rssi threshold
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] rssi_margin: rssi margin value
 * @return send status
 */
mesh_msg_send_cause_t rssi_threshold_set(uint16_t dst, uint16_t net_key_index,
                                         uint8_t rssi_margin);

/**
 * @brief set directed paths
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @return send status
 */
mesh_msg_send_cause_t directed_paths_get(uint16_t dst, uint16_t net_key_index);

/**
 * @brief get directed publish policy
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] elem_addr: element address
 * @param[in] model_id: model id
 * @return send status
 */
mesh_msg_send_cause_t directed_publish_policy_get(uint16_t dst, uint16_t net_key_index,
                                                  uint16_t elem_addr, uint32_t model_id);

/**
 * @brief set directed publish policy
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] publish_policy: publish policy value
 * @param[in] elem_addr: element address
 * @param[in] model_id: model id
 * @return send status
 */
mesh_msg_send_cause_t directed_publish_policy_set(uint16_t dst, uint16_t net_key_index,
                                                  directed_publish_policy_t publish_policy,
                                                  uint16_t elem_addr, uint32_t model_id);

/**
 * @brief get path discovery timing control value
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @return send status
 */
mesh_msg_send_cause_t path_discovery_timing_control_get(uint16_t dst, uint16_t net_key_index);

/**
 * @brief set path discovery timing control value
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] timing_ctl: path discovery timing control value
 * @return send status
 */
mesh_msg_send_cause_t path_discovery_timing_control_set(uint16_t dst, uint16_t net_key_index,
                                                        path_discovery_timing_control_t timing_ctl);

/**
 * @brief get directed control network transmit value
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @return send status
 */
mesh_msg_send_cause_t directed_control_network_transmit_get(uint16_t dst, uint16_t net_key_index);

/**
 * @brief set directed control network transmit value
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] net_transmit: directed control network transmit value
 * @return send status
 */
mesh_msg_send_cause_t directed_control_network_transmit_set(uint16_t dst, uint16_t net_key_index,
                                                            directed_control_network_transmit_t net_transmit);

/**
 * @brief get directed control relay retransmit value
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @return send status
 */
mesh_msg_send_cause_t directed_control_relay_retransmit_get(uint16_t dst, uint16_t net_key_index);

/**
 * @brief set directed control relay retransmit value
 * @param[in] dst: remote address
 * @param[in] net_key_index: mesh message used net key index
 * @param[in] relay_retransmit: directed control relay retransmit value
 * @return send status
 */
mesh_msg_send_cause_t directed_control_relay_retransmit_set(uint16_t dst, uint16_t net_key_index,
                                                            directed_control_relay_retransmit_t relay_retransmit);
/** @} */


END_DECLS

#endif /* MESH_DF */

#endif /* _DIRECTED_FORWARDING_MODEL_H_ */
