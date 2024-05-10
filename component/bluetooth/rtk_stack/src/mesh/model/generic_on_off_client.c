/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_on_off_client.c
* @brief    Source file for generic on off client model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2017-12-22
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "generic_on_off.h"

static mesh_msg_send_cause_t generic_on_off_client_send(const mesh_model_info_p pmodel_info,
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

mesh_msg_send_cause_t generic_on_off_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                         uint16_t app_key_index)
{
    generic_on_off_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_ON_OFF_GET);
    return generic_on_off_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t generic_on_off_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                         uint16_t app_key_index, generic_on_off_t on_off,
                                         uint8_t tid, bool optional,
                                         generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    generic_on_off_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_ON_OFF_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_ON_OFF_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(generic_on_off_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(generic_on_off_set_t, trans_time);
    }
    msg.on_off = on_off;
    msg.tid = tid;
    return generic_on_off_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

static bool generic_on_off_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_ON_OFF_STAT:
        {
            generic_on_off_stat_p pmsg = (generic_on_off_stat_p)pbuffer;
            generic_on_off_client_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.present_on_off = pmsg->present_on_off;
            status_data.optional = FALSE;
            if (pmesh_msg->msg_len == sizeof(generic_on_off_stat_t))
            {
                status_data.optional = TRUE;
                status_data.target_on_off = pmsg->target_on_off;
                status_data.remaining_time = pmsg->remaining_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, GENERIC_ON_OFF_CLIENT_STATUS,
                                                      &status_data);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool generic_on_off_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_ON_OFF_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_on_off_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic on off client reg: missing data process callback!");
        }
    }
    return mesh_model_reg(element_index, pmodel_info);
}

