/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_default_transition_time_client.c
* @brief    Source file for generic default transition time client model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-7-9
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "generic_default_transition_time.h"

mesh_msg_send_cause_t generic_default_transition_time_client_send(const mesh_model_info_p
                                                                  pmodel_info, uint16_t dst,
                                                                  uint16_t app_key_index, uint8_t *pmsg, uint16_t msg_len)
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

mesh_msg_send_cause_t generic_default_transition_time_get(const mesh_model_info_p
                                                          pmodel_info,
                                                          uint16_t dst, uint16_t app_key_index)
{
    generic_default_transition_time_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_DEFAULT_TRANSITION_TIME_GET);
    return generic_default_transition_time_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                                                       sizeof(msg));
}

mesh_msg_send_cause_t generic_default_transition_time_set(const mesh_model_info_p
                                                          pmodel_info,
                                                          uint16_t dst, uint16_t app_key_index,
                                                          generic_transition_time_t trans_time, bool ack)
{
    generic_default_transition_time_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_DEFAULT_TRANSITION_TIME_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNACK);
    }
    msg.trans_time = trans_time;
    return generic_default_transition_time_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                                                       sizeof(msg));
}

static bool generic_default_transition_time_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_DEFAULT_TRANSITION_TIME_STAT:
        if (pmesh_msg->msg_len == sizeof(generic_default_transition_time_stat_t))
        {
            generic_default_transition_time_stat_t *pmsg = (generic_default_transition_time_stat_t *)pbuffer;
            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                generic_default_transition_time_client_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.trans_time = pmsg->trans_time;
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info,
                                                      GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_STATUS, &status_data);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool generic_default_transition_time_client_reg(uint8_t element_index,
                                                mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_default_transition_time_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_default_transition_time_client_reg: missing data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}

