/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_power_level_client.c
* @brief    Source file for generic power level client model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2017-12-22
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "generic_power_level.h"

static mesh_msg_send_cause_t generic_power_level_client_send(const mesh_model_info_p pmodel_info,
                                                             uint16_t dst, uint16_t app_key_index,
                                                             uint8_t *pmsg, uint16_t msg_len)
{
    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t generic_power_level_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                              uint16_t app_key_index)
{
    generic_power_level_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_LEVEL_GET);
    return generic_power_level_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                                           sizeof(msg));
}

mesh_msg_send_cause_t generic_power_level_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                              uint16_t app_key_index,
                                              uint16_t power, uint8_t tid, bool optional,
                                              generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    generic_power_level_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_LEVEL_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_LEVEL_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(generic_power_level_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(generic_power_level_set_t, trans_time);
    }
    msg.power = power;
    msg.tid = tid;
    return generic_power_level_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

mesh_msg_send_cause_t generic_power_last_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                             uint16_t app_key_index)
{
    generic_power_last_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_LAST_GET);
    return generic_power_level_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                                           sizeof(msg));
}

mesh_msg_send_cause_t generic_power_default_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                uint16_t app_key_index)
{
    generic_power_default_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_DEFAULT_GET);
    return generic_power_level_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                                           sizeof(msg));
}

mesh_msg_send_cause_t generic_power_default_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                uint16_t app_key_index, uint16_t power, bool ack)
{
    generic_power_default_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_DEFAULT_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_DEFAULT_SET_UNACK);
    }
    msg.power = power;
    return generic_power_level_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                                           sizeof(msg));
}

mesh_msg_send_cause_t generic_power_range_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                              uint16_t app_key_index)
{
    generic_power_range_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_RANGE_GET);
    return generic_power_level_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                                           sizeof(msg));
}

mesh_msg_send_cause_t generic_power_range_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                              uint16_t app_key_index, uint16_t range_min, uint16_t range_max, bool ack)
{
    generic_power_range_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_RANGE_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_RANGE_SET_UNACK);
    }
    msg.range_min = range_min;
    msg.range_max = range_max;
    return generic_power_level_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                                           sizeof(msg));
}

static bool generic_power_level_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_POWER_LEVEL_STAT:
        {
            generic_power_level_stat_p pmsg = (generic_power_level_stat_p)pbuffer;
            generic_power_level_client_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.optional = FALSE;
            status_data.present_power = pmsg->present_power;
            if (pmesh_msg->msg_len == sizeof(generic_power_level_stat_t))
            {
                status_data.optional = TRUE;
                status_data.target_power = pmsg->target_power;
                status_data.remaining_time = pmsg->remaining_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, GENERIC_POWER_LEVEL_CLIENT_STATUS,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_GENERIC_POWER_LAST_STAT:
        if (pmesh_msg->msg_len == sizeof(generic_power_last_stat_t))
        {
            generic_power_last_stat_p pmsg = (generic_power_last_stat_p)pbuffer;
            generic_power_level_client_status_simple_t status_last = {pmesh_msg->src, pmsg->power};
            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info,
                                                      GENERIC_POWER_LEVEL_CLIENT_STATUS_LAST, &status_last);
            }

        }
        break;
    case MESH_MSG_GENERIC_POWER_DEFAULT_STAT:
        if (pmesh_msg->msg_len == sizeof(generic_power_default_stat_t))
        {
            generic_power_default_stat_p pmsg = (generic_power_default_stat_p)pbuffer;
            generic_power_level_client_status_simple_t status_default = {pmesh_msg->src, pmsg->power};
            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info,
                                                      GENERIC_POWER_LEVEL_CLIENT_STATUS_DEFAULT,
                                                      &status_default);
            }

        }
        break;
    case MESH_MSG_GENERIC_POWER_RANGE_STAT:
        if (pmesh_msg->msg_len == sizeof(generic_power_range_stat_t))
        {
            generic_power_range_stat_p pmsg = (generic_power_range_stat_p)pbuffer;
            generic_power_level_client_status_range_t status_range = {pmesh_msg->src, pmsg->stat, pmsg->range_min, pmsg->range_max};
            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info,
                                                      GENERIC_POWER_LEVEL_CLIENT_STATUS_RANGE,
                                                      &status_range);
            }

        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool generic_power_level_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_POWER_LEVEL_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_power_level_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_power_level_client_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}
