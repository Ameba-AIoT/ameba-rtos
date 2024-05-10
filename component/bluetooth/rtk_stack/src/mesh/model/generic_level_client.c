/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_level_client.c
* @brief    Source file for generic level client model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2017-12-22
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "generic_level.h"

static mesh_msg_send_cause_t generic_level_client_send(const mesh_model_info_p pmodel_info,
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

mesh_msg_send_cause_t generic_level_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                        uint16_t app_key_index)
{
    generic_level_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_LEVEL_GET);
    return generic_level_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t generic_level_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                        uint16_t app_key_index,
                                        int16_t level, uint8_t tid, bool optional,
                                        generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    generic_level_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_LEVEL_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_LEVEL_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(generic_level_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(generic_level_set_t, trans_time);
    }
    msg.level = level;
    msg.tid = tid;
    return generic_level_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

mesh_msg_send_cause_t generic_delta_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                        uint16_t app_key_index,
                                        int32_t delta_level, uint8_t tid, bool optional,
                                        generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    generic_delta_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_DELTA_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_DELTA_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(generic_delta_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(generic_delta_set_t, trans_time);
    }
    msg.delta_level = delta_level;
    msg.tid = tid;
    return generic_level_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

mesh_msg_send_cause_t generic_move_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                       uint16_t app_key_index,
                                       int16_t delta_level, uint8_t tid, bool optional,
                                       generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    generic_move_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_MOVE_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_MOVE_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(generic_move_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(generic_move_set_t, trans_time);
    }
    msg.delta_level = delta_level;
    msg.tid = tid;
    return generic_level_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

static bool generic_level_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_LEVEL_STAT:
        {
            generic_level_stat_p pmsg = (generic_level_stat_p)pbuffer;
            generic_level_client_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.present_level = pmsg->present_level;
            status_data.optional = FALSE;
            if (pmesh_msg->msg_len == sizeof(generic_level_stat_t))
            {
                status_data.optional = TRUE;
                status_data.target_level = pmsg->target_level;
                status_data.remaining_time = pmsg->remaining_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, GENERIC_LEVEL_CLIENT_STATUS,
                                                      &status_data);
            }
            break;
        }
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool generic_level_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_LEVEL_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_level_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_level_client_reg: missing model data process callback!");
        }

    }
    return mesh_model_reg(element_index, pmodel_info);
}

