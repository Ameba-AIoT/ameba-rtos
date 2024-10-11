/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     health_client.c
* @brief    Source file for health client model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2017-3-10
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "mesh_api.h"
#include "health.h"

static mesh_msg_send_cause_t health_client_send(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                uint16_t app_key_index, uint8_t *pmsg,
                                                uint16_t len)
{
    mesh_msg_t mesh_msg = {0};
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = len;
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t health_fault_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                       uint16_t app_key_index, uint16_t company_id)
{
    health_fault_get_t msg;
    msg.company_id = company_id;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_HEALTH_FAULT_GET);
    return health_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                              sizeof(health_fault_get_t));
}

mesh_msg_send_cause_t health_fault_clear(const mesh_model_info_p pmodel_info, uint16_t dst,
                                         uint16_t app_key_index, uint16_t company_id,
                                         bool ack)
{
    health_fault_clear_t msg;
    msg.company_id = company_id;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_HEALTH_FAULT_CLEAR);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_HEALTH_FAULT_CLEAR_UNACK);
    }
    return health_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                              sizeof(health_fault_clear_t));
}

mesh_msg_send_cause_t health_fault_test(const mesh_model_info_p pmodel_info, uint16_t dst,
                                        uint16_t app_key_index, uint8_t test_id,
                                        uint16_t company_id, bool ack)
{
    health_fault_test_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_HEALTH_FAULT_TEST);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_HEALTH_FAULT_TEST_UNACK);
    }
    msg.test_id = test_id;
    msg.company_id = company_id;
    return health_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                              sizeof(health_fault_test_t));
}

mesh_msg_send_cause_t health_period_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                        uint16_t app_key_index)
{
    health_period_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_HEALTH_PERIOD_GET);
    return health_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                              sizeof(health_period_get_t));
}

mesh_msg_send_cause_t health_period_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                        uint16_t app_key_index,
                                        uint8_t fast_period_divisor, bool ack)
{
    health_period_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_HEALTH_PERIOD_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_HEALTH_PERIOD_SET_UNACK);
    }
    msg.fast_period_divisor = fast_period_divisor;
    return health_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                              sizeof(health_period_set_t));
}

mesh_msg_send_cause_t health_attn_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                      uint16_t app_key_index)
{
    health_attn_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_HEALTH_ATTN_GET);
    return health_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                              sizeof(health_attn_get_t));
}

mesh_msg_send_cause_t health_attn_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                      uint16_t app_key_index, uint8_t attn, bool ack)
{
    health_attn_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_HEALTH_ATTN_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_HEALTH_ATTN_SET_UNACK);
    }
    msg.attn = attn;
    return health_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                              sizeof(health_attn_set_t));
}

static bool health_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_HEALTH_ATTN_STAT:
        if (pmesh_msg->msg_len == sizeof(health_attn_stat_t))
        {
            health_attn_stat_p pmsg = (health_attn_stat_p)pbuffer;
            health_client_status_attention_t status_attn = {pmsg->attn};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, HEALTH_CLIENT_STATUS_ATTENTION, &status_attn);
            }
        }
        break;
    case MESH_MSG_HEALTH_CURT_STAT:
        {
            health_curt_stat_p pmsg = (health_curt_stat_p)pbuffer;
            health_client_status_t status_data = {pmesh_msg->src, pmsg->test_id, pmsg->company_id, pmsg->fault_array, 0};
            status_data.fault_array_len = pmesh_msg->msg_len - MEMBER_OFFSET(health_curt_stat_t, fault_array);
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, HEALTH_CLIENT_STATUS_CURRENT, &status_data);
            }
        }
        break;

    case MESH_MSG_HEALTH_FAULT_STAT:
        {
            health_fault_stat_p pmsg = (health_fault_stat_p)pbuffer;
            health_client_status_t status_data = {pmesh_msg->src, pmsg->test_id, pmsg->company_id, pmsg->fault_array, 0};
            status_data.fault_array_len = pmesh_msg->msg_len - MEMBER_OFFSET(health_fault_stat_t, fault_array);
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, HEALTH_CLIENT_STATUS_REGISTERED, &status_data);
            }
        }
        break;
    case MESH_MSG_HEALTH_PERIOD_STAT:
        if (pmesh_msg->msg_len == sizeof(health_period_stat_t))
        {
            health_period_stat_p pmsg = (health_period_stat_p)pbuffer;
            health_client_status_period_t status_period = {pmsg->fast_period_divisor};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, HEALTH_CLIENT_STATUS_PERIOD, &status_period);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool health_client_reg(uint16_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_HEALTH_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = health_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("hralth_client_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}
