/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     directed_forwarding.h
* @brief    Head file for directed forwarding
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-10-26
* @version  v1.0
* *************************************************************************************
*/

#ifndef _DIRECTED_FORWARDING_H_
#define _DIRECTED_FORWARDING_H_

#include "mesh_api.h"

#if MESH_DF

BEGIN_DECLS

#define PATH_LIFETIME_12MIN                                           0
#define PATH_LIFETIME_2HOUR                                           1
#define PATH_LIFETIME_24HOUR                                          2
#define PATH_LIFETIME_10DAY                                           3

#define PATH_DISCOVERY_INTERVAL_5S                                    0
#define PATH_DISCOVERY_INTERVAL_30S                                   1

#define PATH_LANE_DISCOVERY_GUARD_INTERVAL_2S                         0
#define PATH_LANE_DISCOVERY_GUARD_INTERVAL_10S                        1

enum
{
    PUBLISH_POLICY_MANAGED_FLOODING,
    PUBLISH_POLICY_DIRECTED_FORWARDING,
} _SHORT_ENUM_;
typedef uint8_t directed_publish_policy_t;

typedef struct _df_dependent_addr_t
{
    struct _df_dependent_addr_t *pnext;
    uint16_t primary_addr;
    uint8_t secondary_elem_num;
} df_dependent_addr_t;

typedef struct
{
    uint8_t fixed_path : 1;
    uint8_t backward_path_valid : 1;
    uint8_t not_ready : 1;
    uint8_t rfu : 5;
    uint16_t origin_addr;
    uint8_t origin_secondary_elem_num;
    plt_list_t dependent_origin_list;
    uint16_t target_addr;
    uint8_t target_secondary_elem_num;
    plt_list_t dependent_target_list;
    uint8_t forwarding_num;
    uint8_t lane_counter;
    uint16_t bearer_toward_path_origin;
    uint16_t bearer_toward_path_target;
} forwarding_table_entry_t;

enum
{
    PATH_ECHO_TIMER_STATE_ECHO_REQ = 0x00,
    PATH_ECHO_TIMER_STATE_ECHO_REPLY = 0x01,
} _SHORT_ENUM_;
typedef uint8_t df_path_echo_timer_state;

enum
{
    PATH_IN_USE_TIMER_STATE_USE = 0x00,
    PATH_IN_USE_TIMER_STATE_MONITOR = 0x01,
} _SHORT_ENUM_;
typedef uint8_t df_path_in_use_timer_state;

typedef struct _forwarding_table_t
{
    struct _forwarding_table_t *pnext;
    uint16_t fixed_path_index;  // only used for fixed path
    uint16_t df_key_index;
    uint16_t master_key_index;
    forwarding_table_entry_t entry;
    plt_timer_t lifetime_timer;
    df_path_in_use_timer_state in_use_timer_state;
    plt_timer_t in_use_timer;
    df_path_echo_timer_state echo_timer_state;
    plt_timer_t echo_timer;
    bool new_lane_established;
    uint32_t lifetime_period;
    uint32_t lifetime_start_count;
} forwarding_table_t;

extern plt_list_t forwarding_table_list;

enum
{
    DIRECTED_CONTROL_STATE_DISABLE = 0x00,
    DIRECTED_CONTROL_STATE_ENABLE = 0x01,
    DIRECTED_CONTROL_STATE_NOT_SUPPORTED = 0x02,
    DIRECTED_CONTROL_STATE_IGNORE = 0xFF,
} _SHORT_ENUM_;
typedef uint8_t directed_control_state_t;

enum
{
    PATH_METRIC_TYPE_NODE_COUNT = 0,
} _SHORT_ENUM_;
typedef uint8_t path_metric_type_t;

typedef struct
{
    uint8_t directed_forwarding;
    uint8_t directed_relay;
    uint8_t directed_proxy;
    uint8_t directed_proxy_use_directed_default;
    uint8_t directed_friend;
} _PACKED4_ directed_control_t;

typedef struct
{
    uint8_t metric_type : 3;
    uint8_t lifetime : 2;
    uint8_t prohibited : 3;
} _PACKED4_ path_metric_t;

typedef struct
{
    uint8_t max_concurrent_init;
    uint8_t max_discovery_table_entries_count;
} _PACKED4_ discovery_table_capabilities_t;

typedef struct
{
    uint16_t directed_node_paths;
    uint16_t directed_relay_paths;
    uint16_t directed_proxy_paths;
    uint16_t directed_friend_paths;
} _PACKED4_ directed_paths_t;

typedef struct
{
    uint8_t unicast_dst : 1;
    uint8_t backward_path_valid : 1;
    uint8_t prohibited : 6;
    mesh_addr_range_t path_origin;
    union
    {
        mesh_addr_range_t path_target;
        uint16_t multicast_dst;
    };
    uint16_t bearer_toward_path_origin;
    uint16_t bearer_toward_path_target;
} forwarding_table_param_t;

typedef struct
{
    uint16_t path_origin;
    uint16_t dst;
    uint8_t dependent_origin_list_size;
    uint8_t dependent_target_list_size;
    uint16_t addr_range_len;
    uint8_t *paddr_range;
} _PACKED4_ forwarding_table_dependents_add_param_t;

typedef struct
{
    uint16_t path_origin;
    uint16_t dst;
    uint8_t dependent_origin_list_size;
    uint8_t dependent_target_list_size;
    uint16_t addr_range_len;
    uint8_t *paddrs;
} _PACKED4_ forwarding_table_dependents_del_param_t;

typedef struct
{
    uint16_t forwarding_table_update_id;
    uint16_t fixed_path_entries_count;
    uint16_t non_fixed_path_entries_count;
} _PACKED4_ forwarding_table_entries_count_t;

typedef enum
{
    DF_RESULT_SUCCESS,
    DF_RESULT_INVALID_NET_KEY_INDEX,
    DF_RESULT_INVALID_ADDRESS,
    DF_RESULT_INVALID_STATE,
    DF_RESULT_OUT_OF_MEMORY,
    DF_RESULT_RESOURCE_INSUFFICIENT,
    DF_RESULT_BUSY
} df_result_t;

#define MESH_MSG_DF_PATH_ACTION            0x00

typedef enum
{
    DF_PATH_ACTION_TYPE_DISCOVERING,
    DF_PATH_ACTION_TYPE_DISCOVERY_FAIL,
    DF_PATH_ACTION_TYPE_ESTABLISHED,
    DF_PATH_ACTION_TYPE_PATH_RELEASE,
    DF_PATH_ACTION_TYPE_NEW_LANE_DISCOVERING,
    DF_PATH_ACTION_TYPE_NEW_LANE_ESTABLISHED,
    DF_PATH_ACTION_TYPE_NEW_LANE_FAIL,
} df_path_action_type_t;

typedef struct
{
    df_path_action_type_t action_type;
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
} df_path_action_t;

typedef uint16_t (*pf_df_cb_t)(uint8_t type, void *pdata);

/**
 * @brief get directed forwarding control state
 *
 * @param[in] master_key_index: master key index
 * @return directed_control_t
 */
directed_control_t df_control_get(uint16_t master_key_index);

/**
 * @brief set directed forwarding control state
 *
 * @param[in] master_key_index: master key index
 * @param[in] ctl: @ref directed_control_t
 * @return df index
 */
uint16_t df_control_set(uint16_t master_key_index, directed_control_t ctl);

/**
 * @brief get path metric
 *
 * @param[in] master_key_index: master key index
 * @return path_metric_t
 */
path_metric_t df_path_metric_get(uint16_t master_key_index);

/**
 * @brief set path metric
 *
 * @param[in] master_key_index: master key index
 * @param[in] metric: @ref path_metric_t
 * @return uint16_t
 */
uint16_t df_path_metric_set(uint16_t master_key_index, path_metric_t metric);

/**
 * @brief get discovery table cap
 *
 * @param[in] master_key_index: master key index
 * @return discovery_table_capabilities_t
 */
discovery_table_capabilities_t df_discovery_table_cap_get(uint16_t master_key_index);

/**
 * @brief set discovery table cap
 *
 * @param[in] master_key_index: master key index
 * @param[in] cap: @ref discovery_table_capabilities_t
 * @return uint16_t
 */
uint16_t df_discovery_table_cap_set(uint16_t master_key_index, discovery_table_capabilities_t cap);

/**
 * @brief get wanted lanes
 *
 * @param[in] master_key_index: master key index
 * @return uint8_t
 */
uint8_t df_wanted_lanes_get(uint16_t master_key_index);

/**
 * @brief set wanted lanes
 *
 * @param[in] master_key_index: master key index
 * @param[in] wanted_lanes: wanted lanes
 * @return uint16_t
 */
uint16_t df_wanted_lanes_set(uint16_t master_key_index, uint8_t wanted_lanes);

/**
 * @brief get two way path
 *
 * @param[in] master_key_index: master key index
 * @return uint8_t
 */
uint8_t df_two_way_path_get(uint16_t master_key_index);

/**
 * @brief set two way path
 *
 * @param[in] master_key_index: master key index
 * @param[in] two_way_path: two way path
 * @return uint16_t
 */
uint16_t df_two_way_path_set(uint16_t master_key_index, uint8_t two_way_path);

/**
 * @brief get unicast echo interval
 *
 * @param[in] master_key_index: master key index
 * @return uint8_t
 */
uint8_t df_unicast_echo_interval_get(uint16_t master_key_index);

/**
 * @brief get multicast echo interval
 *
 * @param[in] master_key_index: master key index
 * @return uint8_t
 */
uint8_t df_multicast_echo_interval_get(uint16_t master_key_index);

/**
 * @brief set path echo interval
 *
 * @param[in] master_key_index: master key index
 * @param[in] unicast_echo_interval: unicast echo interval
 * @param[in] multicast_echo_interval: multicast echo interval
 * @return uint16_t
 */
uint16_t df_path_echo_interval_set(uint16_t master_key_index, uint8_t unicast_echo_interval,
                                   uint8_t multicast_echo_interval);


/**
 * @brief get last saved forwarding table update identifier state
 *
 * @return uint16_t
 */
uint16_t df_table_update_id_get(void);

/**
 * @brief register directed forwarding callback
 *
 * @param[in] pf_df_cb: callback
 */
void df_cb_reg(pf_df_cb_t pf_df_cb);

/**
 * @brief directed forwarding discovery
 *
 * @param[in] master_key_index: master key index
 * @param[in] target_addr: target address
 * @param[in] dependent_node_addr: dependent node address
 * @param[in] dependent_node_elem_num: dependent node element number
 * @return df_result_t
 */
df_result_t df_path_discovery(uint16_t master_key_index, uint16_t target_addr,
                              uint16_t dependent_node_addr, uint8_t dependent_node_elem_num);

/**
 * @brief directed forwarding dependents update
 *
 * @param[in] type: PATH_DEPENDENT_NODE_UPDATE_TYPE_REMOVE or PATH_DEPENDENT_NODE_UPDATE_TYPE_ADD
 * @param[in] endpoint_addr: origin address or target address
 * @param[in] pdependent_addr: dependent address
 */
void df_path_dependents_update(uint8_t type, uint16_t endpoint_addr,
                               mesh_addr_range_t *pdependent_addr);

/**
 * @brief directed forwarding solicitation
 *
 * @param[in] master_key_index: master key index
 * @param[in] paddr_list: list of addresses
 * @param[in] addr_num: address num
 */
void df_path_solicitation(uint16_t master_key_index, uint16_t *paddr_list, uint16_t addr_num);

/**
 * @brief monitor test mode trigger
 *
 * @param[in] enable:
 */
void df_path_monitor_test_mode_trigger(bool enable);

END_DECLS

#endif /* MESH_DF */

#endif /** _DIRECTED_FORWARDING_H */

