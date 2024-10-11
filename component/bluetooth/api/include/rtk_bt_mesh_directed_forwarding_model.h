/**
 * @file      rtk_bt_mesh_light_model.h
 * @author    quinn_yang@realsil.com.cn
 * @brief     Bluetooth LE MESH directed forwarding model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <rtk_bt_mesh_def.h>

/**
 * @typedef   rtk_bt_mesh_df_client_api_t
 * @brief     BLE MESH directed forwarding client model act definition.
 */
typedef enum {
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_GET = 1,
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_SET,
	RTK_BT_MESH_DF_MODEL_PATH_METRIC_GET,
	RTK_BT_MESH_DF_MODEL_PATH_METRIC_SET,
	RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_GET,
	RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_SET,
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ADD,
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DELETE,
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_ADD,
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_DELETE,
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_GET,
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_COUNT_GET,
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_GET,
	RTK_BT_MESH_DF_MODEL_WANTED_LANES_GET,
	RTK_BT_MESH_DF_MODEL_WANTED_LANES_SET,
	RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_GET,
	RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_SET,
	RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_GET,
	RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_SET,
	RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_GET,
	RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_SET,
	RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_GET,
	RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_SET,
	RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_GET,
	RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_SET,
	RTK_BT_MESH_DF_MODEL_DIRECTED_PATHS_GET,
	RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_GET,
	RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_SET,
	RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_GET,
	RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_SET,
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_GET,
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_SET,
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_GET,
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_SET,
	RTK_BT_MESH_DF_MODEL_ACT_MAX,
} rtk_bt_mesh_df_client_act_t;

/**
 * @typedef   rtk_bt_mesh_df_client_evt_t
 * @brief     BLE MESH directed forwarding act definition.
 */
typedef enum {
	RTK_BT_MESH_DF_PATH_DISCOVERY = 1,
	RTK_BT_MESH_DF_PATH_SOLICITATION,
	RTK_BT_MESH_DF_PATH_DEPENDENTS_UPDATE,
	RTK_BT_MESH_DF_DIRECTED_CONTROL_STATE_GET,
	RTK_BT_MESH_DF_DIRECTED_CONTROL_STATE_SET,
	RTK_BT_MESH_DF_PATH_METRIC_STATE_GET,
	RTK_BT_MESH_DF_PATH_METRIC_STATE_SET,
	RTK_BT_MESH_DF_DISCOVERY_TABLE_CAP_GET,
	RTK_BT_MESH_DF_DISCOVERY_TABLE_CAP_SET,
	RTK_BT_MESH_DF_WANTED_LANES_GET,
	RTK_BT_MESH_DF_WANTED_LANES_SET,
	RTK_BT_MESH_DF_TWO_WAY_PATH_GET,
	RTK_BT_MESH_DF_TWO_WAY_PATH_SET,
	RTK_BT_MESH_DF_PATH_ECHO_INTERVAL_GET,
	RTK_BT_MESH_DF_PATH_ECHO_INTERVAL_SET,
	RTK_BT_MESH_DF_PUBLISH_POLICY_SET,
	RTK_BT_MESH_DF_PARAMETER_SET, //mesh node related
	RTK_BT_MESH_DF_COMMON_ACT_MAX,
} rtk_bt_mesh_df_common_act_t;

/**
 * @typedef   rtk_bt_mesh_df_client_evt_t
 * @brief     BLE MESH directed forwarding client model event definition.
 */
typedef enum {
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_STAT = 1,
	RTK_BT_MESH_DF_MODEL_PATH_METRIC_STAT,
	RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_STAT,
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_STAT,
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_STAT,
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_GET_STAT,
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_COUNT_STAT,
	RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_STAT,
	RTK_BT_MESH_DF_MODEL_WANTED_LANES_STAT,
	RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_STAT,
	RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_STAT,
	RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_STAT,
	RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_STAT,
	RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_STAT,
	RTK_BT_MESH_DF_MODEL_DIRECTED_PATHS_STAT,
	RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_STAT,
	RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_STAT,
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_STAT,
	RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_STAT,
	RTK_BT_MESH_DF_MODEL_EVT_MAX,
} rtk_bt_mesh_df_client_evt_t;

/**
 * @typedef   rtk_bt_mesh_df_path_action_type_t
 * @brief     BLE MESH directed forwarding path action type definition.
 */
typedef enum {
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_DISCOVERING,
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_DISCOVERY_FAIL,
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_ESTABLISHED,
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_PATH_RELEASE,
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_NEW_LANE_DISCOVERING,
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_NEW_LANE_ESTABLISHED,
	RTK_BT_MESH_DF_PATH_ACTION_TYPE_NEW_LANE_FAIL,
} rtk_bt_mesh_df_path_action_type_t;

/**
 * @typedef   rtk_bt_mesh_df_path_action_t
 * @brief     BLE MESH directed forwarding path actiondefinition.
 */
typedef struct {
	rtk_bt_mesh_df_path_action_type_t action_type;
	uint8_t master_key_index;
	uint16_t path_src;
	uint8_t path_src_sec_elem_num;
	uint16_t dp_src;
	uint8_t dp_src_sec_elem_num;
	uint16_t path_dst;
	uint8_t path_dst_sec_elem_num;
	uint16_t dp_dst;
	uint8_t dp_dst_sec_elem_num;
	uint8_t forwarding_num;
} rtk_bt_mesh_df_path_action_t;

#define RTK_BT_MESH_FORWARDING_TABLE_FILTER_MASK_FIXED_PATHS                  0x01
#define RTK_BT_MESH_FORWARDING_TABLE_FILTER_MASK_NON_FIXED_PATHS              0x02
#define RTK_BT_MESH_FORWARDING_TABLE_FILTER_MASK_PATH_ORIGIN_MATCH            0x04
#define RTK_BT_MESH_FORWARDING_TABLE_FILTER_MASK_DST_MATCH                    0x08

#define RTK_BT_MESH_FORWARDING_TABLE_DEPENDENTS_LIST_MASK_PATH_ORIGIN         0x01
#define RTK_BT_MESH_FORWARDING_TABLE_DEPENDENTS_LIST_MASK_PATH_TARGET         0x02

/**
 * @typedef   rtk_bt_mesh_stack_evt_df_t
 * @brief     BLE MESH df action evt structure.
 */
typedef struct {
	uint8_t type;
	rtk_bt_mesh_df_path_action_t path_action;
} rtk_bt_mesh_stack_evt_df_t;

/**
 * @typedef   rtk_bt_mesh_directed_control_t
 * @brief     BLE MESH directed control structure.
 */
typedef struct {
	uint8_t directed_forwarding;
	uint8_t directed_relay;
	uint8_t directed_proxy;
	uint8_t directed_proxy_use_directed_default;
	uint8_t directed_friend;
} _PACKED4_ rtk_bt_mesh_directed_control_t;

/**
 * @typedef   rtk_bt_mesh_path_metric_t
 * @brief     BLE MESH path metric structure.
 */
typedef struct {
	uint8_t metric_type : 3;
	uint8_t lifetime : 2;
	uint8_t prohibited : 3;
} _PACKED4_ rtk_bt_mesh_path_metric_t;

/**
 * @typedef   rtk_bt_mesh_discovery_table_capabilities_t
 * @brief     BLE MESH discovery table capabilities structure.
 */
typedef struct {
	uint8_t max_concurrent_init;
	uint8_t max_discovery_table_entries_count;
} _PACKED4_ rtk_bt_mesh_discovery_table_capabilities_t;

/**
 * @typedef   rtk_bt_mesh_forwarding_table_entries_count_t
 * @brief     BLE MESH forwarding table entries count structure.
 */
typedef struct {
	uint16_t forwarding_table_update_id;
	uint16_t fixed_path_entries_count;
	uint16_t non_fixed_path_entries_count;
} _PACKED4_ rtk_bt_mesh_forwarding_table_entries_count_t;

/**
 * @typedef   rtk_bt_mesh_directed_paths_t
 * @brief     BLE MESH directed paths structure.
 */
typedef struct {
	uint16_t directed_node_paths;
	uint16_t directed_relay_paths;
	uint16_t directed_proxy_paths;
	uint16_t directed_friend_paths;
} _PACKED4_ rtk_bt_mesh_directed_paths_t;

/**
 * @typedef   rtk_bt_mesh_path_discovery_timing_control_t
 * @brief     BLE MESH path discovery timing control structure.
 */
typedef struct {
	uint16_t path_monitoring_interval;
	uint16_t path_discovery_retry_interval;
	uint8_t path_discovery_interval : 1;
	uint8_t lane_discovery_guard_interval : 1;
	uint8_t prohibited : 6;
} _PACKED4_ rtk_bt_mesh_path_discovery_timing_control_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_directed_control_get_t
 * @brief     BLE MESH direced control get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
} rtk_bt_mesh_df_client_act_directed_control_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_directed_control_set_t
 * @brief     BLE MESH direced control set message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
	uint8_t directed_forwarding;
	uint8_t directed_relay;
	uint8_t directed_proxy;
	uint8_t directed_proxy_use_directed_default;
	uint8_t directed_friend;
} rtk_bt_mesh_df_client_act_directed_control_set_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_path_metric_get_t
 * @brief     BLE MESH path metric get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
} rtk_bt_mesh_df_client_act_path_metric_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_path_metric_set_t
 * @brief     BLE MESH path metric set message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
	uint8_t metric_type;
	uint8_t lifetime;
} rtk_bt_mesh_df_client_act_path_metric_set_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_discovery_table_capabilities_get_t
 * @brief     BLE MESH discovery table capabilities get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
} rtk_bt_mesh_df_client_act_discovery_table_capabilities_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_discovery_table_capabilities_set_t
 * @brief     BLE MESH discovery table capabilities set message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
	uint8_t max_concurrent_init;
} rtk_bt_mesh_df_client_act_discovery_table_capabilities_set_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_forwarding_table_add_t
 * @brief     BLE MESH forwarding table add message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
	uint8_t unicast_dst_flag;
	uint8_t backward_path_valid;
	uint16_t origin_addr;
	uint16_t origin_element_num;
	uint16_t target_addr;
	uint16_t target_element_num;
	uint16_t bearer_toward_origin;
	uint16_t bearer_toward_target;
} rtk_bt_mesh_df_client_act_forwarding_table_add_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_forwarding_table_delete_t
 * @brief     BLE MESH forwarding table delete message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
	uint16_t path_origin;
	uint16_t path_target;
} rtk_bt_mesh_df_client_act_forwarding_table_delete_t;

typedef struct {
	uint16_t addr;
	uint8_t element_num;
} rtk_bt_mesh_addr_range_add_t;

#define DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE 10

/**
 * @typedef   rtk_bt_mesh_df_client_act_forwarding_table_dependents_add_t
 * @brief     BLE MESH forwarding table dependents add message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
	uint16_t path_origin;
	uint16_t path_target;
	uint8_t do_list_size;
	uint8_t dt_list_size;
	rtk_bt_mesh_addr_range_add_t do_range[DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE];
	rtk_bt_mesh_addr_range_add_t dt_range[DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE];
} rtk_bt_mesh_df_client_act_forwarding_table_dependents_add_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_forwarding_table_dependents_delete_t
 * @brief     BLE MESH forwarding table dependents delete message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
	uint16_t path_origin;
	uint16_t path_target;
	uint8_t do_list_size;
	uint8_t dt_list_size;
	uint16_t do_range[DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE];
	uint16_t dt_range[DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE];
} rtk_bt_mesh_df_client_act_forwarding_table_dependents_delete_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_forwarding_table_dependents_get_t
 * @brief     BLE MESH forwarding table dependents get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
	uint8_t list_mask;
	uint8_t fixed_path_flag;
	uint16_t start_index;
	uint16_t path_origin;
	uint16_t path_target;
	uint8_t update_id_exist;
	uint16_t update_id;
} rtk_bt_mesh_df_client_act_forwarding_table_dependents_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_forwarding_table_entries_count_get_t
 * @brief     BLE MESH forwarding table entries count get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
} rtk_bt_mesh_df_client_act_forwarding_table_entries_count_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_forwarding_table_entries_get_t
 * @brief     BLE MESH forwarding table entries get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
	uint8_t filter_mask;
	uint16_t start_index;
	uint16_t path_origin;
	uint16_t path_target;
	uint8_t update_id_flag; //check whether have update_id
	uint16_t update_id;
} rtk_bt_mesh_df_client_act_forwarding_table_entries_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_wanted_lanes_get_t
 * @brief     BLE MESH wanted lanes get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
} rtk_bt_mesh_df_client_act_wanted_lanes_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_wanted_lanes_set_t
 * @brief     BLE MESH wanted lanes set message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
	uint8_t wanted_lanes;
} rtk_bt_mesh_df_client_act_wanted_lanes_set_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_two_way_path_get_t
 * @brief     BLE MESH two way path get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
} rtk_bt_mesh_df_client_act_two_way_path_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_two_way_path_set_t
 * @brief     BLE MESH two way path set message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
	uint8_t two_way_path;
} rtk_bt_mesh_df_client_act_two_way_path_set_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_path_echo_interval_get_t
 * @brief     BLE MESH path echo interval get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
} rtk_bt_mesh_df_client_act_path_echo_interval_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_path_echo_interval_set_t
 * @brief     BLE MESH path echo interval set message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t net_key_index_df;
	uint8_t unicast_echo_interval;
	uint8_t multicast_echo_interval;
} rtk_bt_mesh_df_client_act_path_echo_interval_set_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_directed_network_transmit_get_t
 * @brief     BLE MESH directed network transmit get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
} rtk_bt_mesh_df_client_act_directed_network_transmit_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_directed_network_transmit_set_t
 * @brief     BLE MESH directed network transmit set message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint8_t count;
	uint8_t steps;
} rtk_bt_mesh_df_client_act_directed_network_transmit_set_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_directed_relay_retransmit_get_t
 * @brief     BLE MESH directed relay retransmit get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
} rtk_bt_mesh_df_client_act_directed_relay_retransmit_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_directed_relay_retransmit_set_t
 * @brief     BLE MESH directed relay retransmit set message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint8_t count;
	uint8_t steps;
} rtk_bt_mesh_df_client_act_directed_relay_retransmit_set_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_rssi_threshold_get_t
 * @brief     BLE MESH rssi threshold get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
} rtk_bt_mesh_df_client_act_rssi_threshold_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_rssi_threshold_set_t
 * @brief     BLE MESH rssi threshold set message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint8_t rssi_margin;
} rtk_bt_mesh_df_client_act_rssi_threshold_set_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_directed_paths_get_t
 * @brief     BLE MESH directed paths get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
} rtk_bt_mesh_df_client_act_directed_paths_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_directed_publish_policy_get_t
 * @brief     BLE MESH directed publish policy get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t elem_addr;
	uint32_t model_id;
} rtk_bt_mesh_df_client_act_directed_publish_policy_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_directed_publish_policy_set_t
 * @brief     BLE MESH directed publish policy set message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint8_t policy;
	uint16_t elem_addr;
	uint32_t model_id;
} rtk_bt_mesh_df_client_act_directed_publish_policy_set_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_path_discovery_timing_control_get_t
 * @brief     BLE MESH path discovery timing control get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
} rtk_bt_mesh_df_client_act_path_discovery_timing_control_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_path_discovery_timing_control_set_t
 * @brief     BLE MESH path discovery timing control set message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint16_t monitor_interval;
	uint16_t dis_retry_interval;
	uint8_t dis_interval;
	uint8_t lan_guard;
} rtk_bt_mesh_df_client_act_path_discovery_timing_control_set_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_directed_control_network_transmit_get_t
 * @brief     BLE MESH directed control network transmit get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
} rtk_bt_mesh_df_client_act_directed_control_network_transmit_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t
 * @brief     BLE MESH directed control network transmit set message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint8_t count;
	uint8_t steps;
} rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_get_t
 * @brief     BLE MESH directed control relay retransmit get message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
} rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_get_t;

/**
 * @typedef   rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_set_t
 * @brief     BLE MESH directed control relay retransmit set message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t net_key_index;
	uint8_t count;
	uint8_t steps;
} rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_set_t;

/*****common*****/
#define RTK_BT_MESH_MSG_DF_PATH_ACTION            0x00

/**
 * @typedef   rtk_bt_mesh_df_act_df_path_discovery_t
 * @brief     BLE MESH directed forwarding path discovery message structure.
 */
typedef struct {
	uint16_t master_key_index;
	uint16_t target_addr;
	uint16_t dependent_addr;
	uint8_t dependent_elem_num;
} rtk_bt_mesh_df_act_df_path_discovery_t;

/**
 * @typedef   rtk_bt_mesh_df_act_df_path_solicitation_t
 * @brief     BLE MESH directed forwarding path solicitation message structure.
 */
typedef struct {
	uint16_t master_key_index;
	uint16_t addr_num;
	uint16_t addr[DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE];
} rtk_bt_mesh_df_act_df_path_solicitation_t;

/**
 * @typedef   rtk_bt_mesh_df_act_df_path_dependents_update_t
 * @brief     BLE MESH directed forwarding path dependents update message structure.
 */
typedef struct {
	uint8_t type;
	uint16_t endpoint_addr;
	uint16_t dependent_addr;
	uint8_t dependent_elem_num;
} rtk_bt_mesh_df_act_df_path_dependents_update_t;

/**
 * @typedef   rtk_bt_mesh_df_control_state_setting_t
 * @brief     BLE MESH directed forwarding control state setting message structure.
 */
typedef struct {
	uint16_t master_key_index;
	rtk_bt_mesh_directed_control_t ctl;
} rtk_bt_mesh_df_control_state_setting_t;

/**
 * @typedef   rtk_bt_mesh_df_path_metric_state_setting_t
 * @brief     BLE MESH directed forwarding path metric state setting message structure.
 */
typedef struct {
	uint16_t master_key_index;
	rtk_bt_mesh_path_metric_t metric;
} rtk_bt_mesh_df_path_metric_state_setting_t;

/**
 * @typedef   rtk_bt_mesh_df_discovery_table_cap_state_setting_t
 * @brief     BLE MESH directed forwarding discovery table capabilities state setting message structure.
 */
typedef struct {
	uint16_t master_key_index;
	rtk_bt_mesh_discovery_table_capabilities_t cap;
} rtk_bt_mesh_df_discovery_table_cap_state_setting_t;

/**
 * @typedef   rtk_bt_mesh_df_wanted_lanes_state_setting_t
 * @brief     BLE MESH directed forwarding wanted lanes state setting message structure.
 */
typedef struct {
	uint16_t master_key_index;
	uint8_t wanted_lanes;
} rtk_bt_mesh_df_wanted_lanes_state_setting_t;

/**
 * @typedef   rtk_bt_mesh_df_two_way_path_state_setting_t
 * @brief     BLE MESH directed forwarding two way path state setting message structure.
 */
typedef struct {
	uint16_t master_key_index;
	uint8_t two_way_path;
} rtk_bt_mesh_df_two_way_path_state_setting_t;

/**
 * @typedef   rtk_bt_mesh_df_echo_interval_state_setting_t
 * @brief     BLE MESH directed forwarding echo interval state setting message structure.
 */
typedef struct {
	uint16_t master_key_index;
	uint8_t unicast_echo_interval;
	uint8_t multicast_echo_interval;
} rtk_bt_mesh_df_echo_interval_state_setting_t;

/**
 * @typedef   rtk_bt_mesh_df_publish_policy_setting_t
 * @brief     BLE MESH directed forwarding model publish policy state setting message structure.
 */
typedef struct {
	uint8_t element_index;
	uint32_t model_id;
	uint8_t policy;
} rtk_bt_mesh_df_publish_policy_setting_t;

/*****evt*****/
/**
 * @typedef   rtk_bt_mesh_forwarding_table_entry_header_t
 * @brief     BLE MESH forwarding table entry header structure.
 */
typedef struct {
	uint16_t fixed_path_flag : 1;
	uint16_t unicast_dst_flag : 1;
	uint16_t backward_path_validated_flag : 1;
	uint16_t bearer_toward_path_origin_indicator : 1;
	uint16_t bearer_toward_path_target_indicator : 1;
	uint16_t dependent_origin_list_size_indicator : 2;
	uint16_t dependent_target_list_size_indicator : 2;
	uint16_t prohibited : 7;
} _PACKED4_ rtk_bt_mesh_forwarding_table_entry_header_t;

/**
 * @typedef   rtk_bt_mesh_directed_control_status_t
 * @brief     BLE MESH directed control status evt message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index;
	rtk_bt_mesh_directed_control_t directed_ctl;
} _PACKED4_ rtk_bt_mesh_directed_control_status_t;

/**
 * @typedef   rtk_bt_mesh_path_metric_status_t
 * @brief     BLE MESH path metric status evt message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index;
	rtk_bt_mesh_path_metric_t path_metric;
} _PACKED4_ rtk_bt_mesh_path_metric_status_t;

/**
 * @typedef   rtk_bt_mesh_discovery_table_capabilities_status_t
 * @brief     BLE MESH discovery table capabilities status evt message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index;
	rtk_bt_mesh_discovery_table_capabilities_t capabilities;
} _PACKED4_ rtk_bt_mesh_discovery_table_capabilities_status_t;

/**
 * @typedef   rtk_bt_mesh_forwarding_table_status_t
 * @brief     BLE MESH forwarding table status evt message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index;
	uint16_t path_origin;
	uint16_t dst;
} _PACKED4_ rtk_bt_mesh_forwarding_table_status_t;

/**
 * @typedef   rtk_bt_mesh_forwarding_table_dependents_status_t
 * @brief     BLE MESH forwarding table dependents status evt message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index;
	uint16_t path_origin;
	uint16_t dst;
} _PACKED4_ rtk_bt_mesh_forwarding_table_dependents_status_t;

/**
 * @typedef   rtk_bt_mesh_forwarding_table_dependents_status_t
 * @brief     BLE MESH forwarding table entries count status evt message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index;
	rtk_bt_mesh_forwarding_table_entries_count_t count;
} _PACKED4_ rtk_bt_mesh_forwarding_table_entries_count_status_t;

/**
 * @typedef   rtk_bt_mesh_forwarding_table_entries_status_t
 * @brief     BLE MESH forwarding table entries status evt message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index: 12;
	uint16_t filter_mask: 4;
	uint16_t start_index;
	uint8_t data[0];// indicate this structure maybe modifyed
} _PACKED4_ rtk_bt_mesh_forwarding_table_entries_status_t;

/**
 * @typedef   rtk_bt_mesh_forwarding_table_entries_status_for_call_t
 * @brief     BLE MESH forwarding table entries status for call app evt message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index: 12;
	uint16_t filter_mask: 4;
	uint16_t start_index;
	uint16_t data_len;
	uint8_t *data; //for indicate, the pointer is stack pointer
} _PACKED4_ rtk_bt_mesh_forwarding_table_entries_status_for_call_t;

/**
 * @typedef   rtk_bt_mesh_forwarding_table_dependents_get_status_t
 * @brief     BLE MESH forwarding table dependents get status evt message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index: 12;
	uint16_t dependents_list_mask: 2;
	uint16_t fixed_path_flag: 1;
	uint16_t prohibited: 1;
	uint16_t start_index;
	uint16_t path_origin;
	uint16_t dst;
	uint8_t data[0];// indicate this structure maybe modifyed
} _PACKED4_ rtk_bt_mesh_forwarding_table_dependents_get_status_t;

/**
 * @typedef   rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t
 * @brief     BLE MESH forwarding table dependents get status for call app evt message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index: 12;
	uint16_t dependents_list_mask: 2;
	uint16_t fixed_path_flag: 1;
	uint16_t prohibited: 1;
	uint16_t start_index;
	uint16_t path_origin;
	uint16_t dst;
	uint16_t data_len;
	uint8_t *data; // for indicate, the pointer is stack pointer
} _PACKED4_ rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t;

/**
 * @typedef   rtk_bt_mesh_wanted_lanes_status_t
 * @brief     BLE MESH wanted lanes status evt message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index;
	uint8_t wanted_lanes;
} _PACKED4_ rtk_bt_mesh_wanted_lanes_status_t;

/**
 * @typedef   rtk_bt_mesh_two_way_path_t
 * @brief     BLE MESH two way path structure.
 */
typedef struct {
	uint8_t two_way_path: 1;
	uint8_t prohibited: 7;
} _PACKED4_ rtk_bt_mesh_two_way_path_t;

/**
 * @typedef   rtk_bt_mesh_two_way_path_status_t
 * @brief     BLE MESH two way path status evt message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index;
	rtk_bt_mesh_two_way_path_t two_way_path;
} _PACKED4_ rtk_bt_mesh_two_way_path_status_t;

/**
 * @typedef   rtk_bt_mesh_path_echo_interval_status_t
 * @brief     BLE MESH path echo interval status evt message structure.
 */
typedef struct {
	uint8_t status;
	uint16_t net_key_index;
	uint8_t unicast_echo_interval;
	uint8_t multicast_echo_interval;
} _PACKED4_ rtk_bt_mesh_path_echo_interval_status_t;

/**
 * @typedef   rtk_bt_mesh_directed_network_transmit_t
 * @brief     BLE MESH directed network transmit structure.
 */
typedef struct {
	uint8_t directed_network_transmit_count: 3;
	uint8_t directed_network_transmit_interval_steps: 5;
} _PACKED4_ rtk_bt_mesh_directed_network_transmit_t;

/**
 * @typedef   rtk_bt_mesh_directed_network_transmit_status_t
 * @brief     BLE MESH directed network transmit status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_directed_network_transmit_t network_transmit;
} _PACKED4_ rtk_bt_mesh_directed_network_transmit_status_t;

/**
 * @typedef   rtk_bt_mesh_directed_relay_retransmit_t
 * @brief     BLE MESH directed relay retransmit structure.
 */
typedef struct {
	uint8_t directed_relay_retransmit_count: 3;
	uint8_t directed_relay_retransmit_interval_steps: 5;
} _PACKED4_ rtk_bt_mesh_directed_relay_retransmit_t;

/**
 * @typedef   rtk_bt_mesh_directed_relay_retransmit_status_t
 * @brief     BLE MESH directed relay retransmit status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_directed_relay_retransmit_t relay_retransmit;
} _PACKED4_ rtk_bt_mesh_directed_relay_retransmit_status_t;

/**
 * @typedef   rtk_bt_mesh_rssi_threshold_t
 * @brief     BLE MESH rssi threshold structure.
 */
typedef struct {
	int8_t default_rssi_threshold;
	uint8_t rssi_margin;
} _PACKED4_ rtk_bt_mesh_rssi_threshold_t;

/**
 * @typedef   rtk_bt_mesh_rssi_threshold_status_t
 * @brief     BLE MESH rssi threshold status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_rssi_threshold_t threshold;
} _PACKED4_ rtk_bt_mesh_rssi_threshold_status_t;

/**
 * @typedef   rtk_bt_mesh_directed_paths_status_t
 * @brief     BLE MESH directed paths status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_directed_paths_t directed_paths;
} _PACKED4_ rtk_bt_mesh_directed_paths_status_t;

/**
 * @typedef   rtk_bt_mesh_directed_publish_policy_t
 * @brief     BLE MESH directed publish policy structure.
 */
enum {
	RTK_BT_MESH_PUBLISH_POLICY_MANAGED_FLOODING,
	RTK_BT_MESH_PUBLISH_POLICY_DIRECTED_FORWARDING,
} _SHORT_ENUM_;
typedef uint8_t rtk_bt_mesh_directed_publish_policy_t;

/**
 * @typedef   rtk_bt_mesh_directed_publish_policy_status_t
 * @brief     BLE MESH directed publish policy status evt message structure.
 */
typedef struct {
	uint8_t status;
	rtk_bt_mesh_directed_publish_policy_t publish_policy;
	uint16_t element_addr;
	uint32_t model_id;
} _PACKED4_ rtk_bt_mesh_directed_publish_policy_status_t;

/**
 * @typedef   rtk_bt_mesh_path_discovery_timing_control_status_t
 * @brief     BLE MESH path discovery timing control status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_path_discovery_timing_control_t timing_ctl;
} _PACKED4_ rtk_bt_mesh_path_discovery_timing_control_status_t;

/**
 * @typedef   rtk_bt_mesh_directed_control_network_transmit_t
 * @brief     BLE MESH directed control network transmit structure.
 */
typedef struct {
	uint8_t directed_control_network_transmit_count : 3;
	uint8_t directed_control_network_transmit_interval_steps: 5;
} _PACKED4_ rtk_bt_mesh_directed_control_network_transmit_t;

/**
 * @typedef   rtk_bt_mesh_directed_control_network_transmit_status_t
 * @brief     BLE MESH directed control network transmit status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_directed_control_network_transmit_t control_network_transmit;
} _PACKED4_ rtk_bt_mesh_directed_control_network_transmit_status_t;

/**
 * @typedef   rtk_bt_mesh_directed_control_relay_retransmit_t
 * @brief     BLE MESH directed control relay_retransmit structure.
 */
typedef struct {
	uint8_t directed_control_relay_retransmit_count : 3;
	uint8_t directed_control_relay_retransmit_interval_steps: 5;
} _PACKED4_ rtk_bt_mesh_directed_control_relay_retransmit_t;

/**
 * @typedef   rtk_bt_mesh_directed_control_relay_retransmit_status_t
 * @brief     BLE MESH directed control relay_retransmit status evt message structure.
 */
typedef struct {
	rtk_bt_mesh_directed_control_relay_retransmit_t control_relay_retransmit;
} _PACKED4_ rtk_bt_mesh_directed_control_relay_retransmit_status_t;


#define DF_NETWORK_TRANSMIT_SETTING             0x0001
#define DF_RELAY_TRANSMIT_SETTING               0x0002
#define DF_CTL_NETWORK_TRANSMIT_SETTING         0x0004
#define DF_CTL_RELAY_TRANSMIT_SETTING           0x0008
#define DF_TIMING_CTL_SETTING                   0x0010
#define DF_RSSI_SETTING                         0x0020
#define DF_DIRECTED_PATHS_SETTING               0x0040
#define DF_DEPENDENT_ADDR_SIZE_SETTING          0x0080
#define DF_FIXED_PATH_SIZE_SETTING              0x0100
#define DF_NON_FIXED_PATH_SIZE_SETTING          0x0200

/**
 * @typedef   rtk_bt_mesh_directed_control_param_setting_t
 * @brief     BLE MESH directed control parameter setting message structure.
 */
typedef struct {
	uint16_t bit_check; //if need set related parameter,the related bit should be set 1
	rtk_bt_mesh_directed_network_transmit_t network_transmit;
	rtk_bt_mesh_directed_relay_retransmit_t relay_retransmit;
	rtk_bt_mesh_directed_control_network_transmit_t control_network_transmit;
	rtk_bt_mesh_directed_control_relay_retransmit_t control_relay_retransmit;
	rtk_bt_mesh_path_discovery_timing_control_t timing_ctl;
	rtk_bt_mesh_rssi_threshold_t threshold;
	rtk_bt_mesh_directed_paths_t directed_paths;
	uint16_t dependent_addr_size;
	uint16_t df_fixed_path_size;
	uint16_t df_non_fixed_path_size;
} _PACKED4_ rtk_bt_mesh_directed_control_param_setting_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  ble_mesh_directed_forwarding_model BT LE Mesh Directed Forwarding Model APIs
 * @brief     BT LE Mesh Directed Forwarding Model related function APIs
 * @ingroup   ble_mesh_api_group
 * @{
 */

/**
 * @fn        uint16_t rtk_bt_mesh_directed_control_get(rtk_bt_mesh_df_client_act_directed_control_get_t *control_get)
 * @brief     Send directed control get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_GET
 * @param[in] control_get: directed control get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_directed_control_get(rtk_bt_mesh_df_client_act_directed_control_get_t *control_get);

/**
 * @fn        uint16_t rtk_bt_mesh_directed_control_set(rtk_bt_mesh_df_client_act_directed_control_set_t *control_set)
 * @brief     Send directed control set message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_SET
 * @param[in] control_set: directed control set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_directed_control_set(rtk_bt_mesh_df_client_act_directed_control_set_t *control_set);

/**
 * @fn        uint16_t rtk_bt_mesh_path_metric_get(rtk_bt_mesh_df_client_act_path_metric_get_t *path_metric_get)
 * @brief     Send path metric get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_PATH_METRIC_GET
 * @param[in] path_metric_get: path metric get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_path_metric_get(rtk_bt_mesh_df_client_act_path_metric_get_t *path_metric_get);

/**
 * @fn        uint16_t rtk_bt_mesh_path_metric_set(rtk_bt_mesh_df_client_act_path_metric_set_t *path_metric_set)
 * @brief     Send path metric set message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_PATH_METRIC_SET
 * @param[in] path_metric_set: path metric set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_path_metric_set(rtk_bt_mesh_df_client_act_path_metric_set_t *path_metric_set);

/**
 * @fn        uint16_t rtk_bt_mesh_discovery_table_capabilities_get(rtk_bt_mesh_df_client_act_discovery_table_capabilities_get_t *dis_tab_cap_get)
 * @brief     Send discovery table capabilities get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_GET
 * @param[in] dis_tab_cap_get: discovery table capabilities get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_discovery_table_capabilities_get(rtk_bt_mesh_df_client_act_discovery_table_capabilities_get_t *dis_tab_cap_get);

/**
 * @fn        uint16_t rtk_bt_mesh_discovery_table_capabilities_set(rtk_bt_mesh_df_client_act_discovery_table_capabilities_set_t *dis_tab_cap_set)
 * @brief     Send discovery table capabilities set message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_SET
 * @param[in] dis_tab_cap_set: discovery table capabilities set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_discovery_table_capabilities_set(rtk_bt_mesh_df_client_act_discovery_table_capabilities_set_t *dis_tab_cap_set);

/**
 * @fn        uint16_t rtk_bt_mesh_forwarding_table_add(rtk_bt_mesh_df_client_act_forwarding_table_add_t *for_tab_add)
 * @brief     Send forwarding table add message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ADD
 * @param[in] for_tab_add: forwarding table add message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_forwarding_table_add(rtk_bt_mesh_df_client_act_forwarding_table_add_t *for_tab_add);

/**
 * @fn        uint16_t rtk_bt_mesh_forwarding_table_delete(rtk_bt_mesh_df_client_act_forwarding_table_delete_t *for_tab_del)
 * @brief     Send forwarding table delete message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DELETE
 * @param[in] for_tab_del: forwarding table delete message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_forwarding_table_delete(rtk_bt_mesh_df_client_act_forwarding_table_delete_t *for_tab_del);

/**
 * @fn        uint16_t rtk_bt_mesh_forwarding_table_dependents_add(rtk_bt_mesh_df_client_act_forwarding_table_dependents_add_t *for_tab_dep_add)
 * @brief     Send forwarding table dependents add message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_ADD
 * @param[in] for_tab_dep_add: forwarding table dependents add message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_forwarding_table_dependents_add(rtk_bt_mesh_df_client_act_forwarding_table_dependents_add_t *for_tab_dep_add);

/**
 * @fn        uint16_t rtk_bt_mesh_forwarding_table_dependents_delete(rtk_bt_mesh_df_client_act_forwarding_table_dependents_delete_t *for_tab_dep_del)
 * @brief     Send forwarding table dependents delete message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_DELETE
 * @param[in] for_tab_dep_del: forwarding table dependents delete message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_forwarding_table_dependents_delete(rtk_bt_mesh_df_client_act_forwarding_table_dependents_delete_t *for_tab_dep_del);

/**
 * @fn        uint16_t rtk_bt_mesh_forwarding_table_dependents_get(rtk_bt_mesh_df_client_act_forwarding_table_dependents_get_t *for_tab_dep_get)
 * @brief     Send forwarding table dependents get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_GET
 * @param[in] for_tab_dep_get: forwarding table dependents get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_forwarding_table_dependents_get(rtk_bt_mesh_df_client_act_forwarding_table_dependents_get_t *for_tab_dep_get);

/**
 * @fn        uint16_t rtk_bt_mesh_forwarding_table_entries_count_get(rtk_bt_mesh_df_client_act_forwarding_table_entries_count_get_t *for_tab_entries_count_get)
 * @brief     Send forwarding table entries count get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_COUNT_GET
 * @param[in] for_tab_entries_count_get: forwarding table entries count get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_forwarding_table_entries_count_get(rtk_bt_mesh_df_client_act_forwarding_table_entries_count_get_t *for_tab_entries_count_get);

/**
 * @fn        uint16_t rtk_bt_mesh_forwarding_table_entries_get(rtk_bt_mesh_df_client_act_forwarding_table_entries_get_t *for_tab_entries_get)
 * @brief     Send forwarding table entries get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_GET
 * @param[in] for_tab_entries_get: forwarding table entries get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_forwarding_table_entries_get(rtk_bt_mesh_df_client_act_forwarding_table_entries_get_t *for_tab_entries_get);

/**
 * @fn        uint16_t rtk_bt_mesh_wanted_lanes_get(rtk_bt_mesh_df_client_act_wanted_lanes_get_t *wanted_lanes_get)
 * @brief     Send wanted lanes get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_WANTED_LANES_GET
 * @param[in] wanted_lanes_get: wanted lanes get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_wanted_lanes_get(rtk_bt_mesh_df_client_act_wanted_lanes_get_t *wanted_lanes_get);

/**
 * @fn        uint16_t rtk_bt_mesh_wanted_lanes_set(rtk_bt_mesh_df_client_act_wanted_lanes_set_t *wanted_lanes_set)
 * @brief     Send wanted lanes set message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_WANTED_LANES_SET
 * @param[in] wanted_lanes_set: wanted lanes set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_wanted_lanes_set(rtk_bt_mesh_df_client_act_wanted_lanes_set_t *wanted_lanes_set);

/**
 * @fn        uint16_t rtk_bt_mesh_two_way_path_get(rtk_bt_mesh_df_client_act_two_way_path_get_t *two_way_path_get)
 * @brief     Send two way path get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_GET
 * @param[in] two_way_path_get: two way path get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_two_way_path_get(rtk_bt_mesh_df_client_act_two_way_path_get_t *two_way_path_get);

/**
 * @fn        uint16_t rtk_bt_mesh_two_way_path_set(rtk_bt_mesh_df_client_act_two_way_path_set_t *two_way_path_set)
 * @brief     Send two way path set message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_SET
 * @param[in] two_way_path_set: two way path set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_two_way_path_set(rtk_bt_mesh_df_client_act_two_way_path_set_t *two_way_path_set);

/**
 * @fn        uint16_t rtk_bt_mesh_path_echo_interval_get(rtk_bt_mesh_df_client_act_path_echo_interval_get_t *path_echo_interval_get)
 * @brief     Send path echo interval get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_GET
 * @param[in] path_echo_interval_get: path echo interval get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_path_echo_interval_get(rtk_bt_mesh_df_client_act_path_echo_interval_get_t *path_echo_interval_get);

/**
 * @fn        uint16_t rtk_bt_mesh_path_echo_interval_set(rtk_bt_mesh_df_client_act_path_echo_interval_set_t *path_echo_interval_set)
 * @brief     Send path echo interval set message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_SET
 * @param[in] path_echo_interval_set: path echo interval set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_path_echo_interval_set(rtk_bt_mesh_df_client_act_path_echo_interval_set_t *path_echo_interval_set);

/**
 * @fn        uint16_t rtk_bt_mesh_directed_network_transmit_get(rtk_bt_mesh_df_client_act_directed_network_transmit_get_t *net_trans_get)
 * @brief     Send network transmit get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_GET
 * @param[in] net_trans_get: network transmit get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_directed_network_transmit_get(rtk_bt_mesh_df_client_act_directed_network_transmit_get_t *net_trans_get);

/**
 * @fn        uint16_t rtk_bt_mesh_directed_network_transmit_set(rtk_bt_mesh_df_client_act_directed_network_transmit_set_t *net_trans_set)
 * @brief     Send network transmit get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_SET
 * @param[in] net_trans_set: network transmit set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_directed_network_transmit_set(rtk_bt_mesh_df_client_act_directed_network_transmit_set_t *net_trans_set);

/**
 * @fn        uint16_t rtk_bt_mesh_directed_relay_retransmit_get(rtk_bt_mesh_df_client_act_directed_relay_retransmit_get_t *relay_retrans_get)
 * @brief     Send relay retransmit get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_GET
 * @param[in] relay_retrans_get: relay retransmit get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_directed_relay_retransmit_get(rtk_bt_mesh_df_client_act_directed_relay_retransmit_get_t *relay_retrans_get);

/**
 * @fn        uint16_t rtk_bt_mesh_directed_relay_retransmit_set(rtk_bt_mesh_df_client_act_directed_relay_retransmit_set_t *relay_retrans_set)
 * @brief     Send relay retransmit set message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_SET
 * @param[in] relay_retrans_set: relay retransmit set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_directed_relay_retransmit_set(rtk_bt_mesh_df_client_act_directed_relay_retransmit_set_t *relay_retrans_set);

/**
 * @fn        uint16_t rtk_bt_mesh_rssi_threshold_get(rtk_bt_mesh_df_client_act_rssi_threshold_get_t *rssi_get)
 * @brief     Send rssi threshold get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_GET
 * @param[in] rssi_get: rssi threshold get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_rssi_threshold_get(rtk_bt_mesh_df_client_act_rssi_threshold_get_t *rssi_get);

/**
 * @fn        uint16_t rtk_bt_mesh_rssi_threshold_set(rtk_bt_mesh_df_client_act_rssi_threshold_set_t *rssi_set)
 * @brief     Send rssi threshold set message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_SET
 * @param[in] rssi_set: rssi threshold set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_rssi_threshold_set(rtk_bt_mesh_df_client_act_rssi_threshold_set_t *rssi_set);

/**
 * @fn        uint16_t rtk_bt_mesh_directed_paths_get(rtk_bt_mesh_df_client_act_directed_paths_get_t *path_get)
 * @brief     Send directed path get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_PATHS_GET
 * @param[in] path_get: directed path get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_directed_paths_get(rtk_bt_mesh_df_client_act_directed_paths_get_t *path_get);

/**
 * @fn        uint16_t rtk_bt_mesh_directed_publish_policy_get(rtk_bt_mesh_df_client_act_directed_publish_policy_get_t *policy_get)
 * @brief     Send directed publish policy get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_GET
 * @param[in] policy_get: directed publish policy get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_directed_publish_policy_get(rtk_bt_mesh_df_client_act_directed_publish_policy_get_t *policy_get);

/**
 * @fn        uint16_t rtk_bt_mesh_directed_publish_policy_set(rtk_bt_mesh_df_client_act_directed_publish_policy_set_t *policy_set)
 * @brief     Send directed publish policy set message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_SET
 * @param[in] policy_set: directed publish policy set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_directed_publish_policy_set(rtk_bt_mesh_df_client_act_directed_publish_policy_set_t *policy_set);

/**
 * @fn        uint16_t rtk_bt_mesh_discovery_timing_control_get(rtk_bt_mesh_df_client_act_path_discovery_timing_control_get_t *timing_ctl_get)
 * @brief     Send discovery timing control get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_GET
 * @param[in] timing_ctl_get: discovery timing control get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_discovery_timing_control_get(rtk_bt_mesh_df_client_act_path_discovery_timing_control_get_t *timing_ctl_get);

/**
 * @fn        uint16_t rtk_bt_mesh_discovery_timing_control_set(rtk_bt_mesh_df_client_act_path_discovery_timing_control_set_t *timing_ctl_set)
 * @brief     Send discovery timing control set message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_SET
 * @param[in] timing_ctl_set: discovery timing control set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_discovery_timing_control_set(rtk_bt_mesh_df_client_act_path_discovery_timing_control_set_t *timing_ctl_set);

/**
 * @fn        uint16_t rtk_bt_mesh_directed_control_network_transmit_get(rtk_bt_mesh_df_client_act_directed_control_network_transmit_get_t *ctl_net_trans_get)
 * @brief     Send directed control network transmit get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_GET
 * @param[in] ctl_net_trans_get: directed control network transmit get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_directed_control_network_transmit_get(rtk_bt_mesh_df_client_act_directed_control_network_transmit_get_t *ctl_net_trans_get);

/**
 * @fn        uint16_t rtk_bt_mesh_directed_control_network_transmit_set(rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t *ctl_net_trans_set)
 * @brief     Send directed control network transmit set message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_SET
 * @param[in] ctl_net_trans_set: directed control network transmit set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_directed_control_network_transmit_set(rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t *ctl_net_trans_set);

/**
 * @fn        uint16_t rtk_bt_mesh_directed_control_relay_retransmit_get(rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_get_t *ctl_relay_retrans_get)
 * @brief     Send directed control relay retransmit get message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_GET
 * @param[in] ctl_relay_retrans_get: directed control relay retransmit get message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_directed_control_relay_retransmit_get(rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_get_t *ctl_relay_retrans_get);

/**
 * @fn        uint16_t rtk_bt_mesh_directed_control_relay_retransmit_set(rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_set_t *ctl_relay_retrans_set)
 * @brief     Send directed control relay retransmit set message to directed forwarding server, will cause event @ref RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_SET
 * @param[in] ctl_relay_retrans_set: directed control relay retransmit set message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_directed_control_relay_retransmit_set(rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_set_t *ctl_relay_retrans_set);

/**
 * @fn        uint16_t rtk_bt_mesh_df_path_discovery(rtk_bt_mesh_df_act_df_path_discovery_t *path_discovery)
 * @brief     Send directed forwarding path discovery message to directed forwarding devices, will cause event @ref RTK_BT_MESH_DF_PATH_DISCOVERY
 * @param[in] path_discovery: directed forwarding path discovery message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_path_discovery(rtk_bt_mesh_df_act_df_path_discovery_t *path_discovery);

/**
 * @fn        uint16_t rtk_bt_mesh_df_path_solicitation(rtk_bt_mesh_df_act_df_path_solicitation_t *path_solicitation)
 * @brief     Send directed forwarding path solicitation message to directed forwarding devices, will cause event @ref RTK_BT_MESH_DF_PATH_SOLICITATION
 * @param[in] path_solicitation: directed forwarding path solicitation message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_path_solicitation(rtk_bt_mesh_df_act_df_path_solicitation_t *path_solicitation);

/**
 * @fn        uint16_t rtk_bt_mesh_df_path_dependents_update(rtk_bt_mesh_df_act_df_path_dependents_update_t *path_dep_update)
 * @brief     Send directed forwarding path dependents update message to directed forwarding devices, will cause event @ref RTK_BT_MESH_DF_PATH_SOLICITATION
 * @param[in] path_dep_update: directed forwarding path dependents update message structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_path_dependents_update(rtk_bt_mesh_df_act_df_path_dependents_update_t *path_dep_update);

/**********param setting**********/

/**
 * @fn        uint16_t rtk_bt_mesh_df_control_state_get(rtk_bt_mesh_df_control_state_setting_t *ctl)
 * @brief     Send directed forwarding control state get message to get own df control state
 * @param[in] ctl: directed forwarding control state setting structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_control_state_get(rtk_bt_mesh_df_control_state_setting_t *ctl);

/**
 * @fn        uint16_t rtk_bt_mesh_df_control_state_set(rtk_bt_mesh_df_control_state_setting_t *ctl)
 * @brief     Send directed forwarding control state set message to set own df control state
 * @param[in] ctl: directed forwarding control state setting structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_control_state_set(rtk_bt_mesh_df_control_state_setting_t *ctl);

/**
 * @fn        uint16_t rtk_bt_mesh_df_path_metric_state_get(rtk_bt_mesh_df_path_metric_state_setting_t *pm)
 * @brief     Send directed forwarding path metric get message to get own df path metric state
 * @param[in] pm: directed forwarding path metric state setting structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_path_metric_state_get(rtk_bt_mesh_df_path_metric_state_setting_t *pm);

/**
 * @fn        uint16_t rtk_bt_mesh_df_path_metric_state_set(rtk_bt_mesh_df_path_metric_state_setting_t *pm)
 * @brief     Send directed forwarding path metric set message to set own df path metric state
 * @param[in] pm: directed forwarding path metric state setting structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_path_metric_state_set(rtk_bt_mesh_df_path_metric_state_setting_t *pm);

/**
 * @fn        uint16_t rtk_bt_mesh_df_discovery_table_capablities_state_get(rtk_bt_mesh_df_discovery_table_cap_state_setting_t *dtcs)
 * @brief     Send directed forwarding discovery table capablities get message to get own discovery table capablities state
 * @param[in] dtcs: directed forwarding discovery table capablities state setting structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_discovery_table_capablities_state_get(rtk_bt_mesh_df_discovery_table_cap_state_setting_t *dtcs);

/**
 * @fn        uint16_t rtk_bt_mesh_df_discovery_table_capablities_state_set(rtk_bt_mesh_df_discovery_table_cap_state_setting_t *dtcs)
 * @brief     Send directed forwarding discovery table capablities set message to set own discovery table capablities state
 * @param[in] dtcs: directed forwarding discovery table capablities state setting structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_discovery_table_capablities_state_set(rtk_bt_mesh_df_discovery_table_cap_state_setting_t *dtcs);

/**
 * @fn        uint16_t rtk_bt_mesh_df_wanted_lanes_state_get(rtk_bt_mesh_df_wanted_lanes_state_setting_t *wls)
 * @brief     Send directed forwarding wanted lanes get message to get own wanted lane state
 * @param[in] wls: directed forwarding wanted lane state setting structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_wanted_lanes_state_get(rtk_bt_mesh_df_wanted_lanes_state_setting_t *wls);

/**
 * @fn        uint16_t rtk_bt_mesh_df_wanted_lanes_state_set(rtk_bt_mesh_df_wanted_lanes_state_setting_t *wls)
 * @brief     Send directed forwarding wanted lanes set message to set own wanted lane state
 * @param[in] wls: directed forwarding wanted lane state setting structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_wanted_lanes_state_set(rtk_bt_mesh_df_wanted_lanes_state_setting_t *wls);

/**
 * @fn        uint16_t rtk_bt_mesh_df_two_way_path_state_get(rtk_bt_mesh_df_two_way_path_state_setting_t *twps)
 * @brief     Send directed forwarding two way path get message to get own two way path state
 * @param[in] twps: directed forwarding two way path state setting structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_two_way_path_state_get(rtk_bt_mesh_df_two_way_path_state_setting_t *twps);

/**
 * @fn        uint16_t rtk_bt_mesh_df_two_way_path_state_set(rtk_bt_mesh_df_two_way_path_state_setting_t *twps)
 * @brief     Send directed forwarding two way path set message to set own two way path state
 * @param[in] twps: directed forwarding two way path state setting structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_two_way_path_state_set(rtk_bt_mesh_df_two_way_path_state_setting_t *twps);

/**
 * @fn        uint16_t rtk_bt_mesh_df_path_echo_interval_state_get(rtk_bt_mesh_df_echo_interval_state_setting_t *peis)
 * @brief     Send directed forwarding path echo interval get message to get own path echo interval state
 * @param[in] peis: directed forwarding path echo interval state setting structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_path_echo_interval_state_get(rtk_bt_mesh_df_echo_interval_state_setting_t *peis);

/**
 * @fn        uint16_t rtk_bt_mesh_df_path_echo_interval_state_set(rtk_bt_mesh_df_echo_interval_state_setting_t *peis)
 * @brief     Send directed forwarding path echo interval set message to set own path echo interval state
 * @param[in] peis: directed forwarding path echo interval state setting structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_path_echo_interval_state_set(rtk_bt_mesh_df_echo_interval_state_setting_t *peis);

/**
 * @fn        uint16_t rtk_bt_mesh_df_publish_policy_set(rtk_bt_mesh_df_publish_policy_setting_t *policy)
 * @brief     Send directed forwarding model publish policy set message to set own model publish policy state
 * @param[in] policy: directed forwarding model publish policy state setting structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_publish_policy_set(rtk_bt_mesh_df_publish_policy_setting_t *policy);

/**
 * @fn        uint16_t rtk_bt_mesh_df_parameter_set(rtk_bt_mesh_directed_control_param_setting_t *param)
 * @brief     Send directed forwarding parameter set message to set own mesh node releated df parameter
 * @param[in] param: directed forwarding parameter structure
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_df_parameter_set(rtk_bt_mesh_directed_control_param_setting_t *param);

/**
 * @}
 */