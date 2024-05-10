/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_location_client.c
* @brief    Source file for generic on off client model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2019-06-25
* @version  v1.0
* *************************************************************************************
*/
#include "generic_location.h"

static mesh_msg_send_cause_t generic_location_client_send(const mesh_model_info_p pmodel_info,
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

mesh_msg_send_cause_t generic_location_global_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                  uint16_t app_key_index)
{
    generic_location_global_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_LOCATION_GLOBAL_GET);
    return generic_location_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t generic_location_global_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                  uint16_t app_key_index, generic_location_global_t global,
                                                  bool ack)
{
    generic_location_global_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_LOCATION_GLOBAL_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_LOCATION_GLOBAL_SET_UNACK);
    }

    msg.global = global;
    return generic_location_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t generic_location_local_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                 uint16_t app_key_index)
{
    generic_location_local_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_LOCATION_LOCAL_GET);
    return generic_location_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t generic_location_local_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                 uint16_t app_key_index, generic_location_local_t local,
                                                 bool ack)
{
    generic_location_local_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_LOCATION_LOCAL_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_LOCATION_LOCAL_SET_UNACK);
    }

    msg.local = local;
    return generic_location_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

static bool generic_location_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_LOCATION_GLOBAL_STATUS:
        if (pmesh_msg->msg_len == sizeof(generic_location_global_status_t))
        {
            generic_location_global_status_t *pmsg = (generic_location_global_status_t *)pbuffer;
            generic_location_client_status_global_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.global = pmsg->global;

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, GENERIC_LOCATION_CLIENT_STATUS_GLOBAL,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_GENERIC_LOCATION_LOCAL_STATUS:
        if (pmesh_msg->msg_len == sizeof(generic_location_local_status_t))
        {
            generic_location_local_status_t *pmsg = (generic_location_local_status_t *)pbuffer;
            generic_location_client_status_local_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.local = pmsg->local;

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, GENERIC_LOCATION_CLIENT_STATUS_LOCAL,
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

bool generic_location_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_LOCATION_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_location_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_location_client_reg: missing data process callback!");
        }
    }
    return mesh_model_reg(element_index, pmodel_info);
}



