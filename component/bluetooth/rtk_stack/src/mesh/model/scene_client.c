/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     scene_client.c
* @brief    Source file for scene client model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-8-29
* @version  v1.0
* *************************************************************************************
*/

#include "scene.h"

static mesh_msg_send_cause_t scene_client_send(const mesh_model_info_p pmodel_info,
                                               uint16_t dst, uint16_t app_key_index, uint8_t *pmsg, uint16_t msg_len)
{
    mesh_msg_t mesh_msg = {0};
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t scene_store(const mesh_model_info_p pmodel_info, uint16_t dst,
                                  uint16_t app_key_index, uint16_t scene_number, bool ack)
{
    scene_store_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCENE_STORE);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCENE_STORE_UNACK);
    }
    msg.scene_number = scene_number;

    return scene_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t scene_recall(const mesh_model_info_p pmodel_info, uint16_t dst,
                                   uint16_t app_key_index, uint16_t scene_number, uint8_t tid, bool optional,
                                   generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    scene_recall_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCENE_RECALL);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCENE_RECALL_UNACK);
    }

    if (optional)
    {
        len = sizeof(scene_recall_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(scene_recall_t, trans_time);
    }
    msg.scene_number = scene_number;
    msg.tid = tid;

    return scene_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);

}

mesh_msg_send_cause_t scene_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                uint16_t app_key_index)
{
    scene_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCENE_GET);

    return scene_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t scene_register_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                         uint16_t app_key_index)
{
    scene_register_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCENE_REGISTER_GET);

    return scene_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t scene_delete(const mesh_model_info_p pmodel_info, uint16_t dst,
                                   uint16_t app_key_index, uint16_t scene_number, bool ack)
{
    scene_delete_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCENE_DELETE);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCENE_DELETE_UNACK);
    }
    msg.scene_number = scene_number;

    return scene_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

static bool scene_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_SCENE_STATUS:
        if (pmesh_msg->msg_len == sizeof(scene_status_t) ||
            pmesh_msg->msg_len == MEMBER_OFFSET(scene_status_t, target_scene))
        {
            scene_status_t *pmsg = (scene_status_t *)pbuffer;
            scene_client_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.optional = FALSE;
            status_data.status = pmsg->status;
            status_data.current_scene = pmsg->current_scene;
            if (pmesh_msg->msg_len == sizeof(scene_status_t))
            {
                status_data.optional = TRUE;
                status_data.target_scene = pmsg->target_scene;
                status_data.remaining_time = pmsg->remaining_time;
            }
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, SCENE_CLIENT_STATUS,
                                           &status_data);
            }
        }
        break;
    case MESH_MSG_SCENE_REGISTER_STATUS:
        {
            scene_register_status_t *pmsg = (scene_register_status_t *)pbuffer;
            //avoid compile warning
            void *pointer = (void *)pmsg->scenes;
            scene_client_register_status_t status_data = {pmesh_msg->src, (scene_status_code_t)(pmsg->status), pmsg->current_scene, (uint16_t *)pointer, 0};
            status_data.scene_array_len = (pmesh_msg->msg_len - MEMBER_OFFSET(scene_register_status_t,
                                                                              scenes)) / sizeof(uint16_t);
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, SCENE_CLIENT_REGISTER_STATUS, &status_data);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool scene_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_SCENE_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = scene_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("scene_client_reg: missing data process callback!");
        }
    }
    return mesh_model_reg(element_index, pmodel_info);
}

