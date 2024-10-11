/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     directed_forwarding_client.c
* @brief    Source file for directed forwarding client.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2019-12-26
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "directed_forwarding_model.h"
#include <rtk_stack_mesh_internal.h>
#include <rtk_bt_mesh_directed_forwarding_model.h>
#include <mesh_data_dump.h>

#if MESH_DF
mesh_model_info_t df_client;

static mesh_msg_send_cause_t directed_forwarding_client_send(uint16_t dst, uint16_t net_key_index,
                                                             void *pmsg, uint16_t msg_len, uint32_t delay_time)
{
    (void)delay_time;
    mesh_msg_t mesh_msg = {0};
    mesh_msg.pmodel_info = &df_client;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.akf = 0;
    mesh_msg.net_key_index = net_key_index;
    mesh_msg.dst = dst;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t directed_control_get(uint16_t dst, uint16_t net_key_index,
                                           uint16_t net_key_index_df)
{
    directed_control_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DIRECTED_CONTROL_GET);
    msg.net_key_index = net_key_index_df;

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(directed_control_get_t), 0);
}

mesh_msg_send_cause_t directed_control_set(uint16_t dst, uint16_t net_key_index,
                                           uint16_t net_key_index_df, directed_control_t ctl)
{
    directed_control_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DIRECTED_CONTROL_SET);
    msg.net_key_index = net_key_index_df;
    msg.directed_ctl = ctl;

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(directed_control_set_t), 0);
}

mesh_msg_send_cause_t path_metric_get(uint16_t dst, uint16_t net_key_index,
                                      uint16_t net_key_index_df)
{
    path_metric_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PATH_METRIC_GET);
    msg.net_key_index = net_key_index_df;

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(directed_control_get_t), 0);
}

mesh_msg_send_cause_t path_metric_set(uint16_t dst, uint16_t net_key_index,
                                      uint16_t net_key_index_df,
                                      path_metric_t metric)
{
    path_metric_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PATH_METRIC_SET);
    msg.net_key_index = net_key_index_df;
    msg.path_metric = metric;

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(path_metric_set_t), 0);
}

mesh_msg_send_cause_t discovery_table_capabilities_get(uint16_t dst, uint16_t net_key_index,
                                                       uint16_t net_key_index_df)
{
    discovery_table_capabilities_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DISCOVERY_TABLE_CAPABILITIES_GET);
    msg.net_key_index = net_key_index_df;

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(discovery_table_capabilities_get_t),
                                           0);
}

mesh_msg_send_cause_t discovery_table_capabilities_set(uint16_t dst, uint16_t net_key_index,
                                                       uint16_t net_key_index_df, uint8_t max_concurrent_init)
{
    discovery_table_capabilities_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DISCOVERY_TABLE_CAPABILITIES_SET);
    msg.net_key_index = net_key_index_df;
    msg.max_concurrent_init = max_concurrent_init;

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(discovery_table_capabilities_set_t),
                                           0);
}

mesh_msg_send_cause_t forwarding_table_add(uint16_t dst, uint16_t net_key_index,
                                           uint16_t net_key_index_df, const forwarding_table_param_t *ptable_param)
{
    forwarding_table_add_t msg;
    uint8_t *pdata = msg.variable_data;
    mesh_addr_range_t *paddr = (mesh_addr_range_t *)pdata;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FORWARDING_TABLE_ADD);
    msg.net_key_index = net_key_index_df;
    msg.prohibited = 0;
    msg.unicast_dst_flag = ptable_param->unicast_dst;
    msg.backward_path_valid_flag = ptable_param->backward_path_valid;

    if (ptable_param->path_origin.len_present_access)
    {
        *paddr = ptable_param->path_origin;
        pdata += 3;
    }
    else
    {
        paddr->addr = ptable_param->path_origin.addr;
        pdata += 2;
    }

    if (ptable_param->unicast_dst)
    {
        paddr = (mesh_addr_range_t *)pdata;
        if (ptable_param->path_target.len_present_access)
        {
            *paddr = ptable_param->path_target;
            pdata += 3;
        }
        else
        {
            paddr->addr = ptable_param->path_target.addr;
            pdata += 2;
        }
    }
    else
    {
        LE_WORD2EXTRN(pdata, ptable_param->multicast_dst);
        pdata += 2;
    }

    LE_WORD2EXTRN(pdata, ptable_param->bearer_toward_path_origin);
    pdata += 2;
    LE_WORD2EXTRN(pdata, ptable_param->bearer_toward_path_target);
    pdata += 2;

    printi("forwarding_table_add: unicast dst flag %d, backward path valid flag %d, data",
           msg.unicast_dst_flag, msg.backward_path_valid_flag);
    dprinti(msg.variable_data, pdata - msg.variable_data);
    return directed_forwarding_client_send(dst, net_key_index, &msg, pdata - (uint8_t *)&msg, 0);
}

mesh_msg_send_cause_t forwarding_table_delete(uint16_t dst, uint16_t net_key_index,
                                              uint16_t net_key_index_df, uint16_t path_origin, uint16_t path_target)
{
    forwarding_table_delete_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FORWARDING_TABLE_DELETE);
    msg.net_key_index = net_key_index_df;
    msg.path_origin = path_origin;
    msg.dst = path_target;

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(forwarding_table_delete_t),
                                           0);
}

mesh_msg_send_cause_t forwarding_table_dependents_add(uint16_t dst, uint16_t net_key_index,
                                                      uint16_t net_key_index_df, const forwarding_table_dependents_add_param_t *pdepend_param)
{
    uint8_t msg_len = sizeof(forwarding_table_dependents_add_t) + pdepend_param->addr_range_len;
    forwarding_table_dependents_add_t *pmsg = plt_zalloc(msg_len, RAM_TYPE_DATA_ON);
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_FORWARDING_TABLE_DEPENDENTS_ADD);
    pmsg->net_key_index = net_key_index_df;
    pmsg->path_origin = pdepend_param->path_origin;
    pmsg->dst = pdepend_param->dst;
    pmsg->dependent_origin_list_size = pdepend_param->dependent_origin_list_size;
    pmsg->dependent_target_list_size = pdepend_param->dependent_target_list_size;
    memcpy(pmsg->unicast_addr_range, pdepend_param->paddr_range, pdepend_param->addr_range_len);

    mesh_msg_send_cause_t ret = directed_forwarding_client_send(dst, net_key_index, pmsg, msg_len,  0);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t forwarding_table_dependents_delete(uint16_t dst, uint16_t net_key_index,
                                                         uint16_t net_key_index_df, const forwarding_table_dependents_del_param_t *pdepend_param)
{
    uint8_t msg_len = sizeof(forwarding_table_dependents_delete_t) + pdepend_param->addr_range_len;
    forwarding_table_dependents_delete_t *pmsg = plt_zalloc(msg_len, RAM_TYPE_DATA_ON);
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_FORWARDING_TABLE_DEPENDENTS_DELETE);
    pmsg->net_key_index = net_key_index_df;
    pmsg->path_origin = pdepend_param->path_origin;
    pmsg->dst = pdepend_param->dst;
    pmsg->dependent_origin_list_size = pdepend_param->dependent_origin_list_size;
    pmsg->dependent_target_list_size = pdepend_param->dependent_target_list_size;
    memcpy(pmsg->unicast_addrs, pdepend_param->paddrs, pdepend_param->addr_range_len);

    mesh_msg_send_cause_t ret = directed_forwarding_client_send(dst, net_key_index, pmsg, msg_len, 0);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t forwarding_table_dependents_get(uint16_t dst, uint16_t net_key_index,
                                                      uint16_t net_key_index_df, uint8_t dependents_list_mask,
                                                      uint8_t fixed_path_flag, uint16_t start_index,
                                                      uint16_t path_origin, uint16_t path_target,
                                                      bool update_id_exist, uint16_t update_id)
{
    forwarding_table_dependents_get_t msg;
    uint8_t msg_len = sizeof(forwarding_table_dependents_get_t);
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FORWARDING_TABLE_DEPENDENTS_GET);
    msg.net_key_index = net_key_index_df;
    msg.dependents_list_mask = dependents_list_mask;
    msg.fixed_path_flag = fixed_path_flag;
    msg.prohibited = 0;
    msg.start_index = start_index;
    msg.path_origin = path_origin;
    msg.dst = path_target;
    msg.forwarding_table_update_id = update_id;
    if (!update_id_exist)
    {
        msg_len -= sizeof(uint16_t);
    }

    return directed_forwarding_client_send(dst, net_key_index, &msg, msg_len, 0);
}

mesh_msg_send_cause_t forwarding_table_entries_count_get(uint16_t dst, uint16_t net_key_index,
                                                         uint16_t net_key_index_df)
{
    forwarding_table_entries_count_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FORWARDING_TABLE_ENTRIES_COUNT_GET);
    msg.net_key_index = net_key_index_df;

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(forwarding_table_entries_count_get_t),
                                           0);
}

mesh_msg_send_cause_t forwarding_table_entries_get(uint16_t dst, uint16_t net_key_index,
                                                   uint16_t net_key_index_df, uint8_t filter_mask, uint16_t start_index, uint8_t *pdata,
                                                   uint16_t data_len)
{
    uint8_t msg_len = sizeof(forwarding_table_entries_get_t) + data_len;
    forwarding_table_entries_get_t *pmsg = plt_zalloc(msg_len, RAM_TYPE_DATA_ON);
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_FORWARDING_TABLE_ENTRIES_GET);
    pmsg->net_key_index = net_key_index_df;
    pmsg->filter_mask = filter_mask;
    pmsg->start_index = start_index;
    memcpy(pmsg->data, pdata, data_len);

    mesh_msg_send_cause_t ret = directed_forwarding_client_send(dst, net_key_index, pmsg, msg_len, 0);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t wanted_lanes_get(uint16_t dst, uint16_t net_key_index,
                                       uint16_t net_key_index_df)
{
    wanted_lanes_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_WANTED_LANES_GET);
    msg.net_key_index = net_key_index_df;

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(wanted_lanes_get_t),
                                           0);
}

mesh_msg_send_cause_t wanted_lanes_set(uint16_t dst, uint16_t net_key_index,
                                       uint16_t net_key_index_df, uint8_t wanted_lanes)
{
    wanted_lanes_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_WANTED_LANES_SET);
    msg.net_key_index = net_key_index_df;
    msg.wanted_lanes = wanted_lanes;

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(wanted_lanes_set_t),
                                           0);
}

mesh_msg_send_cause_t two_way_path_get(uint16_t dst, uint16_t net_key_index,
                                       uint16_t net_key_index_df)
{
    two_way_path_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TWO_WAY_PATH_GET);
    msg.net_key_index = net_key_index_df;

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(two_way_path_get_t),
                                           0);
}

mesh_msg_send_cause_t two_way_path_set(uint16_t dst, uint16_t net_key_index,
                                       uint16_t net_key_index_df, uint8_t two_way_path)
{
    two_way_path_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TWO_WAY_PATH_SET);
    msg.net_key_index = net_key_index_df;
    msg.two_way_path.two_way_path = two_way_path;
    msg.two_way_path.prohibited = 0;

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(two_way_path_set_t),
                                           0);
}

mesh_msg_send_cause_t path_echo_interval_get(uint16_t dst, uint16_t net_key_index,
                                             uint16_t net_key_index_df)
{
    path_echo_interval_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PATH_ECHO_INTERVAL_GET);
    msg.net_key_index = net_key_index_df;

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(path_echo_interval_get_t),
                                           0);
}

mesh_msg_send_cause_t path_echo_interval_set(uint16_t dst, uint16_t net_key_index,
                                             uint16_t net_key_index_df, uint8_t unicast_echo_interval,
                                             uint8_t multicast_echo_interval)
{
    path_echo_interval_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PATH_ECHO_INTERVAL_SET);
    msg.net_key_index = net_key_index_df;
    msg.unicast_echo_interval = unicast_echo_interval;
    msg.multicast_echo_interval = multicast_echo_interval;

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(path_echo_interval_set_t),
                                           0);
}

mesh_msg_send_cause_t directed_network_transmit_get(uint16_t dst, uint16_t net_key_index)
{
    directed_network_transmit_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DIRECTED_NETWORK_TRANSMIT_GET);

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(directed_network_transmit_get_t),
                                           0);
}

mesh_msg_send_cause_t directed_network_transmit_set(uint16_t dst, uint16_t net_key_index,
                                                    uint8_t count, uint8_t interval_steps)
{
    directed_network_transmit_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DIRECTED_NETWORK_TRANSMIT_SET);
    msg.network_transmit.directed_network_transmit_count = count;
    msg.network_transmit.directed_network_transmit_interval_steps = interval_steps;

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(directed_network_transmit_set_t),
                                           0);
}

mesh_msg_send_cause_t directed_relay_retransmit_get(uint16_t dst, uint16_t net_key_index)
{
    directed_relay_retransmit_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DIRECTED_RELAY_RETRANSMIT_GET);

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(directed_relay_retransmit_get_t),
                                           0);
}

mesh_msg_send_cause_t directed_relay_retransmit_set(uint16_t dst, uint16_t net_key_index,
                                                    uint8_t count, uint8_t interval_steps)
{
    directed_relay_retransmit_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DIRECTED_RELAY_RETRANSMIT_SET);
    msg.relay_retransmit.directed_relay_retransmit_count = count;
    msg.relay_retransmit.directed_relay_retransmit_interval_steps = interval_steps;

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(directed_relay_retransmit_set_t),
                                           0);
}

mesh_msg_send_cause_t rssi_threshold_get(uint16_t dst, uint16_t net_key_index)
{
    rssi_threshold_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_RSSI_THRESHOLD_GET);

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(rssi_threshold_get_t),
                                           0);
}

mesh_msg_send_cause_t rssi_threshold_set(uint16_t dst, uint16_t net_key_index,
                                         uint8_t rssi_margin)
{
    rssi_threshold_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_RSSI_THRESHOLD_SET);
    msg.rssi_margin = rssi_margin;

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(rssi_threshold_set_t),
                                           0);
}

mesh_msg_send_cause_t directed_paths_get(uint16_t dst, uint16_t net_key_index)
{
    directed_paths_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DIRECTED_PATHS_GET);

    return directed_forwarding_client_send(dst, net_key_index, &msg, sizeof(directed_paths_get_t),
                                           0);
}

mesh_msg_send_cause_t directed_publish_policy_get(uint16_t dst, uint16_t net_key_index,
                                                  uint16_t elem_addr, uint32_t model_id)
{
    directed_publish_policy_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DIRECTED_PUBLISH_POLICY_GET);
    msg.element_addr = elem_addr;
    msg.model_id = MESH_MODEL_CONVERT(model_id);

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(directed_publish_policy_get_t) - (MESH_IS_SIG_MODEL(model_id) ? 2 : 0),
                                           0);
}

mesh_msg_send_cause_t directed_publish_policy_set(uint16_t dst, uint16_t net_key_index,
                                                  directed_publish_policy_t publish_policy,
                                                  uint16_t elem_addr, uint32_t model_id)
{
    directed_publish_policy_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DIRECTED_PUBLISH_POLICY_SET);
    msg.publish_policy = publish_policy;
    msg.element_addr = elem_addr;
    msg.model_id = MESH_MODEL_CONVERT(model_id);

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(directed_publish_policy_set_t) - (MESH_IS_SIG_MODEL(model_id) ? 2 : 0),
                                           0);
}

mesh_msg_send_cause_t path_discovery_timing_control_get(uint16_t dst, uint16_t net_key_index)
{
    path_discovery_timing_control_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PATH_DISCOVERY_TIMING_CONTROL_GET);

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(path_discovery_timing_control_get_t),
                                           0);
}

mesh_msg_send_cause_t path_discovery_timing_control_set(uint16_t dst, uint16_t net_key_index,
                                                        path_discovery_timing_control_t timing_ctl)
{
    path_discovery_timing_control_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PATH_DISCOVERY_TIMING_CONTROL_SET);
    msg.timing_ctl = timing_ctl;

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(path_discovery_timing_control_set_t),
                                           0);
}

mesh_msg_send_cause_t directed_control_network_transmit_get(uint16_t dst, uint16_t net_key_index)
{
    directed_control_network_transmit_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DIRECTED_CONTROL_NETWORK_TRANSMIT_GET);

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(directed_control_network_transmit_get_t),
                                           0);
}

mesh_msg_send_cause_t directed_control_network_transmit_set(uint16_t dst, uint16_t net_key_index,
                                                            directed_control_network_transmit_t net_transmit)
{
    directed_control_network_transmit_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DIRECTED_CONTROL_NETWORK_TRANSMIT_SET);
    msg.control_network_transmit = net_transmit;

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(directed_control_network_transmit_set_t),
                                           0);
}

mesh_msg_send_cause_t directed_control_relay_retransmit_get(uint16_t dst, uint16_t net_key_index)
{
    directed_control_relay_retransmit_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DIRECTED_CONTROL_RELAY_RETRANSMIT_GET);

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(directed_control_relay_retransmit_get_t),
                                           0);
}

mesh_msg_send_cause_t directed_control_relay_retransmit_set(uint16_t dst, uint16_t net_key_index,
                                                            directed_control_relay_retransmit_t relay_retransmit)
{
    directed_control_relay_retransmit_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DIRECTED_CONTROL_RELAY_RETRANSMIT_SET);
    msg.control_relay_retransmit = relay_retransmit;

    return directed_forwarding_client_send(dst, net_key_index, &msg,
                                           sizeof(directed_control_relay_retransmit_set_t),
                                           0);
}

static bool directed_forwarding_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_DIRECTED_CONTROL_STATUS:
        if (pmesh_msg->msg_len == sizeof(directed_control_status_t))
        {
            // directed_control_status_t *pmsg = (directed_control_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_STATUS),
				sizeof(directed_control_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_STATUS));
            // data_uart_debug("receive directed control status: status %d, net_key_index 0x%04x, ctl %d-%d-%d-%d-%d\r\n",
            //                 pmsg->status, pmsg->net_key_index, pmsg->directed_ctl.directed_forwarding,
            //                 pmsg->directed_ctl.directed_relay,
            //                 pmsg->directed_ctl.directed_proxy, pmsg->directed_ctl.directed_proxy_use_directed_default,
            //                 pmsg->directed_ctl.directed_friend);
        }
        break;
    case MESH_MSG_PATH_METRIC_STATUS:
        if (pmesh_msg->msg_len == sizeof(path_metric_status_t))
        {
            // path_metric_status_t *pmsg = (path_metric_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_PATH_METRIC_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_PATH_METRIC_STATUS),
				sizeof(path_metric_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_PATH_METRIC_STATUS));
            // data_uart_debug("receive path metric status: status %d, net_key_index 0x%04x, metric %d-%d\r\n",
            //                 pmsg->status,
            //                 pmsg->net_key_index, pmsg->path_metric.metric_type, pmsg->path_metric.lifetime);
        }
        break;
    case MESH_MSG_DISCOVERY_TABLE_CAPABILITIES_STATUS:
        if (pmesh_msg->msg_len == sizeof(discovery_table_capabilities_status_t))
        {
            // discovery_table_capabilities_status_t *pmsg = (discovery_table_capabilities_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_DISCOVERY_TABLE_CAPABILITIES_STATUS),
				sizeof(discovery_table_capabilities_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_DISCOVERY_TABLE_CAPABILITIES_STATUS));
            // data_uart_debug("receive discovery table capabilities status: status %d, net_key_index 0x%04x, capabilities %d-%d\r\n",
            //                 pmsg->status, pmsg->net_key_index, pmsg->capabilities.max_concurrent_init,
            //                 pmsg->capabilities.max_discovery_table_entries_count);
        }
        break;
    case MESH_MSG_FORWARDING_TABLE_STATUS:
        if (pmesh_msg->msg_len == sizeof(forwarding_table_status_t))
        {
            // forwarding_table_status_t *pmsg = (forwarding_table_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_STATUS),
				sizeof(forwarding_table_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_STATUS));
            // data_uart_debug("receive forwarding table status: status %d, net_key_index 0x%04x, path_origin 0x%04x, dst 0x%04x\r\n",
            //                 pmsg->status, pmsg->net_key_index, pmsg->path_origin, pmsg->dst);
        }
        break;
    case MESH_MSG_FORWARDING_TABLE_DEPENDENTS_STATUS:
        if (pmesh_msg->msg_len == sizeof(forwarding_table_dependents_status_t))
        {
            // forwarding_table_dependents_status_t *pmsg = (forwarding_table_dependents_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_DEPENDENTS_STATUS),
				sizeof(forwarding_table_dependents_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_DEPENDENTS_STATUS));
            // data_uart_debug("receive forwarding table dependents status: status %d, net_key_index 0x%04x, path_origin 0x%04x, dst 0x%04x\r\n",
            //                 pmsg->status, pmsg->net_key_index, pmsg->path_origin, pmsg->dst);
        }
        break;
    case MESH_MSG_FORWARDING_TABLE_ENTRIES_COUNT_STATUS:
        if (pmesh_msg->msg_len == sizeof(forwarding_table_entries_count_status_t))
        {
            // forwarding_table_entries_count_status_t *pmsg = (forwarding_table_entries_count_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_COUNT_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_ENTRIES_COUNT_STATUS),
				sizeof(forwarding_table_entries_count_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_FORWARDING_TABLE_ENTRIES_COUNT_STATUS));
            // data_uart_debug("receive forwarding table entries count status: status %d, net_key_index 0x%04x, update id %d, fixed entries count %d, none fixed entries count %d\r\n",
            //                 pmsg->status, pmsg->net_key_index, pmsg->count.forwarding_table_update_id,
            //                 pmsg->count.fixed_path_entries_count, pmsg->count.non_fixed_path_entries_count);
        }
        break;
    case MESH_MSG_FORWARDING_TABLE_ENTRIES_STATUS:
        {
            // uint16_t path_origin = MESH_UNASSIGNED_ADDR;
            // uint16_t dst = MESH_UNASSIGNED_ADDR;
            // uint16_t forwarding_table_update_counter = 0;
            forwarding_table_entries_status_t *pmsg = (forwarding_table_entries_status_t *)pbuffer;

            forwarding_table_entries_status_for_call_t call_data;
            call_data.status = pmsg->status;
            call_data.net_key_index = pmsg->net_key_index;
            call_data.filter_mask = pmsg->filter_mask;
            call_data.start_index = pmsg->start_index;
            call_data.data_len = pmesh_msg->msg_len - sizeof(forwarding_table_entries_status_t);
            call_data.data = pmsg->data;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_STAT,
				&call_data, sizeof(forwarding_table_entries_status_for_call_t) + call_data.data_len);
                // &call_data, sizeof(forwarding_table_entries_status_for_call_t));

            // uint8_t *pdata = pmsg->data;
            // if (pmsg->filter_mask & FORWARDING_TABLE_FILTER_MASK_PATH_ORIGIN_MATCH)
            // {
            //     path_origin = LE_EXTRN2WORD(pdata);
            //     pdata += 2;
            // }

            // if (pmsg->filter_mask & FORWARDING_TABLE_FILTER_MASK_DST_MATCH)
            // {
            //     dst = LE_EXTRN2WORD(pdata);
            //     pdata += 2;
            // }

            // if (MESH_MSG_STAT_SUCCESS == pmsg->status)
            // {
            //     forwarding_table_update_counter = LE_EXTRN2WORD(pdata);
            //     pdata += 2;
            // }

            // data_uart_debug("receive forwarding table entries status: status %d, net_key_index 0x%04x, filter_mask %d, start_index %d, path_origin 0x%04x, dst 0x%04x, counter %d\r\n",
            //                 pmsg->status, pmsg->net_key_index, pmsg->filter_mask, pmsg->start_index, path_origin, dst,
            //                 forwarding_table_update_counter);

            // while ((pdata - pmsg->data + 1 + sizeof(forwarding_table_entries_status_t)) <= pmesh_msg->msg_len)
            // {
            //     forwarding_table_entry_header_t *pheader = (forwarding_table_entry_header_t *)pdata;
            //     data_uart_debug("entry header: fixed path %d, unicast_dst %d, backward_path_valid %d, bearer_toward_path_origin_indicator %d, bearer_toward_path_target_indicator %d, dependent_origin_list_size_indicator %d, dependent_target_list_size_indicator %d\r\n",
            //                     pheader->fixed_path_flag, pheader->unicast_dst_flag, pheader->backward_path_validated_flag,
            //                     pheader->bearer_toward_path_origin_indicator, pheader->bearer_toward_path_target_indicator,
            //                     pheader->dependent_origin_list_size_indicator, pheader->dependent_target_list_size_indicator);
            //     pdata += sizeof(forwarding_table_entry_header_t);

            //     if (pheader->fixed_path_flag)
            //     {
            //         uint16_t entry_path_origin;
            //         uint8_t entry_path_origin_len = 1;
            //         uint16_t entry_dependent_origin_list_size = 0;
            //         uint16_t entry_bearer_toward_path_origin = 0;
            //         uint16_t entry_path_target;
            //         uint8_t entry_path_target_len = 1;
            //         uint16_t entry_dependent_target_list_size = 0;
            //         uint16_t entry_bearer_toward_path_target = 0;

            //         mesh_addr_range_t *paddr = (mesh_addr_range_t *)pdata;
            //         entry_path_origin = paddr->range_start_access;
            //         pdata += 2;
            //         if (paddr->len_present_access)
            //         {
            //             entry_path_origin_len = paddr->range_len;
            //             pdata += 1;
            //         }

            //         if (pheader->dependent_origin_list_size_indicator == 1)
            //         {
            //             entry_dependent_origin_list_size = *(uint8_t *)pdata;
            //             pdata += 1;
            //         }
            //         else if (pheader->dependent_origin_list_size_indicator == 2)
            //         {
            //             entry_dependent_origin_list_size = LE_EXTRN2WORD(pdata);
            //             pdata += 2;
            //         }

            //         if (pheader->bearer_toward_path_origin_indicator == 1)
            //         {
            //             entry_bearer_toward_path_origin = LE_EXTRN2WORD(pdata);
            //             pdata += 2;
            //         }

            //         if (pheader->unicast_dst_flag)
            //         {
            //             mesh_addr_range_t *paddr = (mesh_addr_range_t *)pdata;
            //             entry_path_target = paddr->range_start_access;
            //             pdata += 2;
            //             if (paddr->len_present_access)
            //             {
            //                 entry_path_target_len = paddr->range_len;
            //                 pdata += 1;
            //             }
            //         }
            //         else
            //         {
            //             entry_path_target = LE_EXTRN2WORD(pdata);
            //             pdata += 2;
            //         }

            //         if (pheader->dependent_target_list_size_indicator == 1)
            //         {
            //             entry_dependent_target_list_size = *(uint8_t *)pdata;
            //             pdata += 1;
            //         }
            //         else if (pheader->dependent_target_list_size_indicator == 2)
            //         {
            //             entry_dependent_target_list_size = LE_EXTRN2WORD(pdata);
            //             pdata += 2;
            //         }

            //         if (pheader->bearer_toward_path_target_indicator == 1)
            //         {
            //             entry_bearer_toward_path_target = LE_EXTRN2WORD(pdata);
            //             pdata += 2;
            //         }

            //         data_uart_debug("fixed path entry: path_origin 0x%04x, path_origin_elem_num %d, dependent_origin_list_size %d, bearer_toward_path_origin 0x%x, path_target 0x%04x, path_target_elem_num %d, dependent_target_list_size %d, bearer_toward_path_target 0x%x\r\n",
            //                         entry_path_origin, entry_path_origin_len, entry_dependent_origin_list_size,
            //                         entry_bearer_toward_path_origin, entry_path_target, entry_path_target_len,
            //                         entry_dependent_target_list_size, entry_bearer_toward_path_target);
            //     }
            //     else
            //     {
            //         uint16_t entry_path_origin;
            //         uint8_t entry_path_origin_len = 1;
            //         uint16_t entry_dependent_origin_list_size = 0;
            //         uint16_t entry_bearer_toward_path_origin = 0;
            //         uint16_t entry_path_target;
            //         uint8_t entry_path_target_len = 1;
            //         uint16_t entry_dependent_target_list_size = 0;
            //         uint16_t entry_bearer_toward_path_target = 0;

            //         uint8_t lane_counter = *(uint8_t *)pdata;
            //         pdata += 1;
            //         uint16_t remain_time = LE_EXTRN2WORD(pdata);
            //         pdata += 2;
            //         uint8_t forwarding_num = *(uint8_t *)pdata;
            //         pdata += 1;

            //         mesh_addr_range_t *paddr = (mesh_addr_range_t *)pdata;
            //         entry_path_origin = paddr->range_start_access;
            //         pdata += 2;
            //         if (paddr->len_present_access)
            //         {
            //             entry_path_origin_len = paddr->range_len;
            //             pdata += 1;
            //         }

            //         if (pheader->dependent_origin_list_size_indicator == 1)
            //         {
            //             entry_dependent_origin_list_size = *(uint8_t *)pdata;
            //             pdata += 1;
            //         }
            //         else if (pheader->dependent_origin_list_size_indicator == 2)
            //         {
            //             entry_dependent_origin_list_size = LE_EXTRN2WORD(pdata);
            //             pdata += 2;
            //         }

            //         if (pheader->bearer_toward_path_origin_indicator == 1)
            //         {
            //             entry_bearer_toward_path_origin = LE_EXTRN2WORD(pdata);
            //             pdata += 2;
            //         }

            //         if (pheader->unicast_dst_flag)
            //         {
            //             mesh_addr_range_t *paddr = (mesh_addr_range_t *)pdata;
            //             entry_path_target = paddr->range_start_access;
            //             pdata += 2;
            //             if (paddr->len_present_access)
            //             {
            //                 entry_path_target_len = paddr->range_len;
            //                 pdata += 1;
            //             }
            //         }
            //         else
            //         {
            //             entry_path_target = LE_EXTRN2WORD(pdata);
            //             pdata += 2;
            //         }

            //         if (pheader->dependent_target_list_size_indicator == 1)
            //         {
            //             entry_dependent_target_list_size = *(uint8_t *)pdata;
            //             pdata += 1;
            //         }
            //         else if (pheader->dependent_target_list_size_indicator == 2)
            //         {
            //             entry_dependent_target_list_size = LE_EXTRN2WORD(pdata);
            //             pdata += 2;
            //         }

            //         if (pheader->bearer_toward_path_target_indicator == 1)
            //         {
            //             entry_bearer_toward_path_target = LE_EXTRN2WORD(pdata);
            //             pdata += 2;
            //         }

            //         data_uart_debug("non-fixed path entry: lane_counter %d, remain_time %d, forwarding_num %d, path_origin 0x%04x, path_origin_elem_num %d, dependent_origin_list_size %d, bearer_toward_path_origin 0x%x, path_target 0x%04x, path_target_elem_num %d, dependent_target_list_size %d, bearer_toward_path_target 0x%x\r\n",
            //                         lane_counter, remain_time, forwarding_num, entry_path_origin, entry_path_origin_len,
            //                         entry_dependent_origin_list_size, entry_bearer_toward_path_origin, entry_path_target,
            //                         entry_path_target_len, entry_dependent_target_list_size, entry_bearer_toward_path_target);
            //     }
            // }
        }
        break;
    case MESH_MSG_FORWARDING_TABLE_DEPENDENTS_GET_STATUS:
        {
            forwarding_table_dependents_get_status_t *pmsg = (forwarding_table_dependents_get_status_t *)
                                                             pbuffer;
            // uint8_t *pdata = pmsg->data;
            // uint16_t forwarding_table_update_counter = 0;
            // uint16_t dependent_origin_list_size = 0;
            // uint16_t dependent_target_list_size = 0;
            forwarding_table_dependents_get_status_for_call_t call_data;
            call_data.status = pmsg->status;
            call_data.net_key_index = pmsg->net_key_index;
            call_data.dependents_list_mask = pmsg->dependents_list_mask;
            call_data.fixed_path_flag = pmsg->fixed_path_flag;
            call_data.prohibited = pmsg->prohibited;
            call_data.start_index = pmsg->start_index;
            call_data.path_origin = pmsg->path_origin;
            call_data.dst = pmsg->dst;
            call_data.data_len = pmesh_msg->msg_len - sizeof(forwarding_table_dependents_get_status_t);
            call_data.data = pmsg->data;

            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_GET_STAT,
				&call_data, sizeof(forwarding_table_dependents_get_status_for_call_t) + call_data.data_len);
                // &call_data, sizeof(forwarding_table_dependents_get_status_for_call_t));
            // if (pmsg->status == MESH_MSG_STAT_SUCCESS || pmsg->status == MESH_MSG_STAT_OBSOLETE_INFORMATION)
            // {
            //     // forwarding_table_update_counter = LE_EXTRN2WORD(pdata);
            //     pdata += 2;
            // }

            // if (pmsg->status == MESH_MSG_STAT_SUCCESS)
            // {

            //     dependent_origin_list_size = *pdata;
            //     pdata += 1;

            //     dependent_target_list_size = *pdata;
            //     pdata += 1;
            // }

            // data_uart_debug("receive forwarding table dependents get status: status %d, net_key_index 0x%04x, dependents_list_mask 0x%02x, fixed_path_flag %d, start_index %d, path_origin 0x%04x, dst 0x%04x, forwarding_table_update_counter %d, dependents_origin_list_size %d, dependents_target_list_size %d\r\n",
            //                 pmsg->status, pmsg->net_key_index, pmsg->dependents_list_mask,
            //                 pmsg->fixed_path_flag, pmsg->start_index,
            //                 pmsg->path_origin, pmsg->dst, forwarding_table_update_counter,
            //                 dependent_origin_list_size, dependent_target_list_size);

            // uint16_t addr;
            // uint8_t addr_len;
            // while ((pdata - pmsg->data + sizeof(forwarding_table_dependents_get_status_t)) < pmesh_msg->msg_len)
            // {
            //     addr_len = 0;
            //     mesh_addr_range_t *paddr = (mesh_addr_range_t *)pdata;
            //     addr = paddr->range_start_access;
            //     pdata += 2;
            //     if (paddr->len_present_access)
            //     {
            //         addr_len = paddr->range_len;
            //         pdata += 1;
            //     }
            //     else
            //     {
            //         addr_len = 1;
            //     }
            //     data_uart_debug("dependent addr: addr 0x%04x, elem_num %d\r\n", addr, addr_len);
            // }
        }
        break;
    case MESH_MSG_WANTED_LANES_STATUS:
        if (pmesh_msg->msg_len == sizeof(wanted_lanes_status_t))
        {
            // wanted_lanes_status_t *pmsg = (wanted_lanes_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_WANTED_LANES_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_WANTED_LANES_STATUS),
				sizeof(wanted_lanes_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_WANTED_LANES_STATUS));
            // data_uart_debug("receive wanted lanes status: status %d, net_key_index 0x%04x, wanted lanes %d\r\n",
            //                 pmsg->status, pmsg->net_key_index, pmsg->wanted_lanes);
        }
        break;
    case MESH_MSG_TWO_WAY_PATH_STATUS:
        if (pmesh_msg->msg_len == sizeof(two_way_path_status_t))
        {
            // two_way_path_status_t *pmsg = (two_way_path_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_TWO_WAY_PATH_STATUS),
				sizeof(two_way_path_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_TWO_WAY_PATH_STATUS));
            // data_uart_debug("receive two way path status: status %d, net_key_index 0x%04x, two way path %d\r\n",
            //                 pmsg->status, pmsg->net_key_index, pmsg->two_way_path.two_way_path);
        }
        break;
    case MESH_MSG_PATH_ECHO_INTERVAL_STATUS:
        if (pmesh_msg->msg_len == sizeof(path_echo_interval_status_t))
        {
            // path_echo_interval_status_t *pmsg = (path_echo_interval_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_PATH_ECHO_INTERVAL_STATUS),
				sizeof(path_echo_interval_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_PATH_ECHO_INTERVAL_STATUS));
            // data_uart_debug("receive path echo interval status: status %d, net_key_index 0x%04x, unicast echo interval %d, multicast echo interval %d\r\n",
            //                 pmsg->status, pmsg->net_key_index, pmsg->unicast_echo_interval, pmsg->multicast_echo_interval);
        }
        break;
    case MESH_MSG_DIRECTED_NETWORK_TRANSMIT_STATUS:
        if (pmesh_msg->msg_len == sizeof(directed_network_transmit_status_t))
        {
            // directed_network_transmit_status_t *pmsg = (directed_network_transmit_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_NETWORK_TRANSMIT_STATUS),
				sizeof(directed_network_transmit_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_NETWORK_TRANSMIT_STATUS));
            // data_uart_debug("receive directed network transmit status: count %d, steps %d\r\n",
            //                 pmsg->network_transmit.directed_network_transmit_count,
            //                 pmsg->network_transmit.directed_network_transmit_interval_steps);
        }
        break;
    case MESH_MSG_DIRECTED_RELAY_RETRANSMIT_STATUS:
        if (pmesh_msg->msg_len == sizeof(directed_relay_retransmit_status_t))
        {
            // directed_relay_retransmit_status_t *pmsg = (directed_relay_retransmit_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_RELAY_RETRANSMIT_STATUS),
				sizeof(directed_relay_retransmit_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_RELAY_RETRANSMIT_STATUS));
            // data_uart_debug("receive directed relay retransmit status: count %d, steps %d\r\n",
            //                 pmsg->relay_retransmit.directed_relay_retransmit_count,
            //                 pmsg->relay_retransmit.directed_relay_retransmit_interval_steps);
        }
        break;
    case MESH_MSG_RSSI_THRESHOLD_STATUS:
        if (pmesh_msg->msg_len == sizeof(rssi_threshold_status_t))
        {
            // rssi_threshold_status_t *pmsg = (rssi_threshold_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_RSSI_THRESHOLD_STATUS),
				sizeof(rssi_threshold_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_RSSI_THRESHOLD_STATUS));
            // data_uart_debug("receive rssi threshold status: default_rssi_threshold %d, rssi_margin %d\r\n",
            //                 pmsg->threshold.default_rssi_threshold, pmsg->threshold.rssi_margin);
        }
        break;
    case MESH_MSG_DIRECTED_PATHS_STATUS:
        if (pmesh_msg->msg_len == sizeof(directed_paths_status_t))
        {
            // directed_paths_status_t *pmsg = (directed_paths_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_DIRECTED_PATHS_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_PATHS_STATUS),
				sizeof(directed_paths_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_PATHS_STATUS));
            // data_uart_debug("receive directed paths status: directed_node_paths %d, directed_relay_paths %d, directed_proxy_paths %d, directed_friend_paths %d\r\n",
            //                 pmsg->directed_paths.directed_node_paths, pmsg->directed_paths.directed_relay_paths,
            //                 pmsg->directed_paths.directed_proxy_paths,
            //                 pmsg->directed_paths.directed_friend_paths);
        }
        break;
    case MESH_MSG_DIRECTED_PUBLISH_POLICY_STATUS:
        {
            directed_publish_policy_status_t *pmsg = (directed_publish_policy_status_t *)pbuffer;
            uint32_t model_id;
            if (pmesh_msg->msg_len == sizeof(directed_publish_policy_status_t))
            {
                model_id = pmsg->model_id;
            }
            else if (pmesh_msg->msg_len == sizeof(directed_publish_policy_status_t) - 2)
            {
                model_id = MESH_MODEL_TRANSFORM(pmsg->model_id);
            }
            else
            {
                break;
            }
            directed_publish_policy_status_t call_data;
            call_data = *(directed_publish_policy_status_t *)pbuffer;
            call_data.model_id = model_id;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_STAT,
				&call_data.status, sizeof(directed_publish_policy_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_PUBLISH_POLICY_STATUS));
            // data_uart_debug("receive directed publish policy status: status %d, policy %d, elem_addr 0x%04x, model id 0x%08x\r\n",
            //                 pmsg->status, pmsg->publish_policy, pmsg->element_addr, model_id);
        }
        break;
    case MESH_MSG_PATH_DISCOVERY_TIMING_CONTROL_STATUS:
        if (pmesh_msg->msg_len == sizeof(path_discovery_timing_control_status_t))
        {
            // path_discovery_timing_control_status_t *pmsg = (path_discovery_timing_control_status_t *)pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_PATH_DISCOVERY_TIMING_CONTROL_STATUS),
				sizeof(path_discovery_timing_control_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_PATH_DISCOVERY_TIMING_CONTROL_STATUS));
            // data_uart_debug("receive path discovery timing control status: path_monitoring_interval %d, path_discovery_retry_interval %d, path_discovery_interval %d, lane_discovery_guard_interval %d\r\n",
            //                 pmsg->timing_ctl.path_monitoring_interval, pmsg->timing_ctl.path_discovery_retry_interval,
            //                 pmsg->timing_ctl.path_discovery_interval, pmsg->timing_ctl.lane_discovery_guard_interval);
        }
        break;
    case MESH_MSG_DIRECTED_CONTROL_NETWORK_TRANSMIT_STATUS:
        if (pmesh_msg->msg_len == sizeof(directed_control_network_transmit_status_t))
        {
            // directed_control_network_transmit_status_t *pmsg = (directed_control_network_transmit_status_t *)
            //                                                    pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_NETWORK_TRANSMIT_STATUS),
				sizeof(directed_control_network_transmit_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_NETWORK_TRANSMIT_STATUS));
            // data_uart_debug("receive directed control network transmit status: count %d, steps %d\r\n",
            //                 pmsg->control_network_transmit.directed_control_network_transmit_count,
            //                 pmsg->control_network_transmit.directed_control_network_transmit_interval_steps);
        }
        break;
    case MESH_MSG_DIRECTED_CONTROL_RELAY_RETRANSMIT_STATUS:
        if (pmesh_msg->msg_len == sizeof(directed_control_relay_retransmit_status_t))
        {
            // directed_control_relay_retransmit_status_t *pmsg = (directed_control_relay_retransmit_status_t *)
            //                                                    pbuffer;
            rtk_bt_mesh_directed_forwarding_client_model_indicate_event(RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_STAT,
				(uint8_t *) pbuffer + ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_RELAY_RETRANSMIT_STATUS),
				sizeof(directed_control_relay_retransmit_status_t) - ACCESS_OPCODE_SIZE(MESH_MSG_DIRECTED_CONTROL_RELAY_RETRANSMIT_STATUS));
            // data_uart_debug("receive directed control relay retransmit status: count %d, steps %d\r\n",
            //                 pmsg->control_relay_retransmit.directed_control_relay_retransmit_count,
            //                 pmsg->control_relay_retransmit.directed_control_relay_retransmit_interval_steps);
        }
        break;
    default:
        ret = false;
        break;
    }
    return ret;
}

bool directed_forwarding_client_reg(uint8_t element_index)
{
    df_client.model_id = MESH_MODEL_DIRECTED_FORWARDING_CLIENT;
    if (NULL == df_client.model_receive)
    {
        df_client.model_receive = directed_forwarding_client_receive;
    }
    return mesh_model_reg(element_index, &df_client);
}

#endif
