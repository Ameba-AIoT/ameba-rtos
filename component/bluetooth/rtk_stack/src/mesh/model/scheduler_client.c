/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     scheduler_client.c
* @brief    Source file for scheduler client model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-9-17
* @version  v1.0
* *************************************************************************************
*/

#include "scheduler.h"

static mesh_msg_send_cause_t scheduler_client_send(const mesh_model_info_p pmodel_info,
                                                   uint16_t dst, uint16_t app_key_index, uint8_t *pmsg,
                                                   uint16_t msg_len)
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

mesh_msg_send_cause_t scheduler_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                    uint16_t app_key_index)
{
    scheduler_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCHEDULER_GET);
    return scheduler_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t scheduler_action_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                           uint16_t app_key_index, uint8_t index)
{
    scheduler_action_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCHEDULER_ACTION_GET);
    msg.index = index;
    return scheduler_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t scheduler_action_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                           uint16_t app_key_index, scheduler_register_t scheduler,
                                           bool ack)
{
    scheduler_action_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCHEDULER_ACTION_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCHEDULER_ACTION_SET_UNACK);
    }
    msg.scheduler = scheduler;
    return scheduler_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

static bool scheduler_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_SCHEDULER_STATUS:
        if (pmesh_msg->msg_len == sizeof(scheduler_status_t))
        {
            scheduler_status_t *pmsg = (scheduler_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                scheduler_client_status_t status_data = {pmesh_msg->src, 0};
                status_data.schedulers = pmsg->schedulers;
                pmodel_info->model_data_cb(pmodel_info, SCHEDULER_CLIENT_STATUS, &status_data);
            }
        }
        break;
    case MESH_MSG_SCHEDULER_ACTION_STATUS:
        if (pmesh_msg->msg_len == sizeof(scheduler_action_status_t))
        {
            scheduler_action_status_t *pmsg = (scheduler_action_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                scheduler_client_status_action_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.scheduler = pmsg->scheduler;
                pmodel_info->model_data_cb(pmodel_info, SCHEDULER_CLIENT_STATUS_ACTION, &status_data);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool scheduler_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_SCHEDULER_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = scheduler_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("scheduler_client_reg: missing data process callback!");
        }
    }
    return mesh_model_reg(element_index, pmodel_info);
}

