/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

/* Provisioning Manager role. See delegated_prov.h for the design. */

/* App-level opt-in, symmetric with the Server. See delegated_prov_server.c. */
#ifndef MESH_DELEGATED_PROV
#define MESH_DELEGATED_PROV                 0
#endif

#if MESH_DELEGATED_PROV

#include <string.h>
#include "delegated_prov.h"
#include "mesh_node.h"
#include "mesh_flash.h"

/* -------------------------------------------------------------------- tx helper */

/* akf = 0: message is protected by the target's device key, no AppKey */
static mesh_msg_send_cause_t dprov_client_send(const mesh_model_info_p pmodel_info,
                                               uint16_t dst, uint16_t net_key_index,
                                               uint8_t *pmsg, uint16_t msg_len)
{
    mesh_msg_t mesh_msg = {0};
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.akf = 0;
    mesh_msg.net_key_index = net_key_index;
    return access_send(&mesh_msg);
}

/* ----------------------------------------------------------------- public API */

mesh_msg_send_cause_t dprov_client_grant(const mesh_model_info_p pmodel_info, uint16_t dst,
                                         uint16_t net_key_index, uint16_t addr_base,
                                         uint16_t addr_count, uint8_t inactive_period,
                                         uint16_t report_dst, uint8_t prefix_len,
                                         const uint8_t *uuid_prefix)
{
    dprov_grant_t *pmsg;
    mesh_msg_send_cause_t ret;
    uint16_t msg_len;

    /* overflow-safe range check: whole range must stay in the unicast space */
    if ((0 == addr_count) || (prefix_len > 16) || ((prefix_len > 0) && (NULL == uuid_prefix)) ||
        !MESH_IS_UNICAST_ADDR(addr_base) ||
        ((uint16_t)(addr_count - 1) > (uint16_t)(0x7FFF - addr_base)))
    {
        return MESH_MSG_SEND_CAUSE_INVALID_ACCESS_PARAMETER;
    }

    msg_len = sizeof(dprov_grant_t) + prefix_len;

    pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_DPROV_GRANT);
    pmsg->addr_base = addr_base;
    pmsg->addr_count = addr_count;
    pmsg->inactive_period = inactive_period;
    pmsg->report_dst = report_dst;
    if (prefix_len)
    {
        memcpy(pmsg->uuid_prefix, uuid_prefix, prefix_len);
    }

    ret = dprov_client_send(pmodel_info, dst, net_key_index, (uint8_t *)pmsg, msg_len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t dprov_client_cap_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                           uint16_t net_key_index)
{
    dprov_cap_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DPROV_CAP_GET);
    return dprov_client_send(pmodel_info, dst, net_key_index, (uint8_t *)&msg,
                             sizeof(dprov_cap_get_t));
}

mesh_msg_send_cause_t dprov_client_revoke(const mesh_model_info_p pmodel_info, uint16_t dst,
                                          uint16_t net_key_index)
{
    dprov_revoke_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DPROV_REVOKE);
    return dprov_client_send(pmodel_info, dst, net_key_index, (uint8_t *)&msg,
                             sizeof(dprov_revoke_t));
}

mesh_msg_send_cause_t dprov_client_report_confirm(const mesh_model_info_p pmodel_info,
                                                  uint16_t dst, uint16_t net_key_index,
                                                  uint16_t unicast_addr, dprov_stat_t status)
{
    dprov_node_report_confirm_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DPROV_NODE_REPORT_CONFIRM);
    msg.unicast_addr = unicast_addr;
    msg.status = status;
    return dprov_client_send(pmodel_info, dst, net_key_index, (uint8_t *)&msg,
                             sizeof(dprov_node_report_confirm_t));
}

/* -------------------------------------------------------------------- receive */

static bool dprov_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_DPROV_GRANT_STATUS:
        if (pmesh_msg->msg_len == sizeof(dprov_grant_status_t))
        {
            dprov_grant_status_t *pmsg = (dprov_grant_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                dprov_client_grant_status_t cb_data;
                cb_data.src = pmesh_msg->src;
                cb_data.status = pmsg->status;
                pmodel_info->model_data_cb(pmodel_info, DPROV_CLIENT_GRANT_STATUS, &cb_data);
            }
        }
        break;
    case MESH_MSG_DPROV_NODE_REPORT:
        if (pmesh_msg->msg_len == sizeof(dprov_node_report_t))
        {
            dprov_node_report_t *pmsg = (dprov_node_report_t *)pbuffer;
            dprov_stat_t status;
            int idx;

            /* dev_key_add: >=0 fresh, -1 duplicate (re-ack silently), -2 no room */
            idx = dev_key_add(pmsg->unicast_addr, pmsg->element_count, pmsg->dev_key);
            if (idx >= 0)
            {
                uint16_t flash_idx = (uint16_t)idx;
                mesh_flash_store(MESH_FLASH_PARAMS_DEV_KEY, &flash_idx);

                status = DPROV_STAT_SUCCESS;
                if (NULL != pmodel_info->model_data_cb)
                {
                    dprov_client_node_report_t cb_data = {0};
                    cb_data.src = pmesh_msg->src;
                    cb_data.unicast_addr = pmsg->unicast_addr;
                    cb_data.element_count = pmsg->element_count;
                    memcpy(cb_data.dev_key, pmsg->dev_key, 16);
                    pmodel_info->model_data_cb(pmodel_info, DPROV_CLIENT_NODE_REPORT, &cb_data);
                    memset(&cb_data, 0, sizeof(cb_data));
                }
            }
            else if (-1 == idx)
            {
                status = DPROV_STAT_SUCCESS;
                printw("dprov: duplicate report from 0x%04x re-acked", pmsg->unicast_addr);
            }
            else
            {
                status = DPROV_STAT_NO_RESOURCE;
                printe("dprov: no dev_key slot to archive report for 0x%04x",
                       pmsg->unicast_addr);
            }

            dprov_client_report_confirm(pmodel_info, pmesh_msg->src,
                                        pmesh_msg->net_key_index, pmsg->unicast_addr, status);
        }
        break;
    case MESH_MSG_DPROV_SERVER_STATUS:
        if (pmesh_msg->msg_len == sizeof(dprov_server_status_t))
        {
            dprov_server_status_t *pmsg = (dprov_server_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                dprov_client_server_status_t cb_data;
                cb_data.src = pmesh_msg->src;
                cb_data.reason = pmsg->reason;
                cb_data.addr_next = pmsg->addr_next;
                cb_data.addr_end = pmsg->addr_end;
                pmodel_info->model_data_cb(pmodel_info, DPROV_CLIENT_SERVER_STATUS, &cb_data);
            }
        }
        break;
    case MESH_MSG_DPROV_CAP_STATUS:
        if (pmesh_msg->msg_len == sizeof(dprov_cap_status_t))
        {
            dprov_cap_status_t *pmsg = (dprov_cap_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                dprov_client_cap_status_t cb_data;
                cb_data.src = pmesh_msg->src;
                cb_data.willing = pmsg->willing;
                pmodel_info->model_data_cb(pmodel_info, DPROV_CLIENT_CAP_STATUS, &cb_data);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }

    return ret;
}

bool dprov_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_DPROV_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = dprov_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("dprov client reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}

#endif /* MESH_DELEGATED_PROV */
