/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_power_on_off_client.c
* @brief    Source file for generic power on off client model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2017-12-22
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "generic_power_on_off.h"

static mesh_msg_send_cause_t generic_power_on_off_client_send(const mesh_model_info_p pmodel_info,
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

mesh_msg_send_cause_t generic_on_power_up_get(const mesh_model_info_p pmodel_info,
                                              uint16_t dst,
                                              uint16_t app_key_index)
{
    generic_on_power_up_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_ON_POWER_UP_GET);
    return generic_power_on_off_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                                            sizeof(msg));
}

mesh_msg_send_cause_t generic_on_power_up_set(const mesh_model_info_p pmodel_info,
                                              uint16_t dst,
                                              uint16_t app_key_index,
                                              generic_on_power_up_t on_power_up, bool ack)
{
    generic_on_power_up_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_ON_POWER_UP_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_ON_POWER_UP_SET_UNACK);
    }
    msg.on_power_up = on_power_up;
    return generic_power_on_off_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                                            sizeof(msg));
}

static bool generic_power_on_off_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_ON_POWER_UP_STAT:
        if (pmesh_msg->msg_len == sizeof(generic_on_power_up_stat_t))
        {
            generic_on_power_up_stat_p pmsg = (generic_on_power_up_stat_p)pbuffer;
            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                generic_power_on_off_client_status_t status_data = {pmesh_msg->src, pmsg->on_power_up};
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, GENERIC_POWER_ON_OFF_CLIENT_STATUS,
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

bool generic_power_on_off_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_POWER_ON_OFF_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_power_on_off_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_power_on_off_client_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}

