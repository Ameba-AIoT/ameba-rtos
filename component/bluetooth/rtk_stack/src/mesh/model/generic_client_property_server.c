/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_client_property_server.c
* @brief    Source file for generic client property server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2019-06-28
* @version  v1.0
* *************************************************************************************
*/
#include "generic_property.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif

static mesh_msg_send_cause_t generic_client_property_send(mesh_model_info_p pmodel_info,
                                                          uint16_t dst,
                                                          uint16_t app_key_index,
                                                          uint8_t *pmsg, uint16_t msg_len, uint16_t delay_time)
{
    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    if (0 != dst)
    {
        mesh_msg.dst = dst;
        mesh_msg.app_key_index = app_key_index;
    }
    mesh_msg.delay_time = delay_time;
    return access_send(&mesh_msg);
}

static mesh_msg_send_cause_t generic_client_properties_status(mesh_model_info_p pmodel_info,
                                                              uint16_t dst, uint16_t app_key_index,
                                                              const uint16_t *pproperty_ids,
                                                              uint16_t num_ids, uint16_t delay_time)
{
    uint16_t msg_len = sizeof(generic_client_properties_status_t) + 2 * num_ids;
    generic_client_properties_status_t *pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_GENERIC_CLIENT_PROPERTIES_STATUS);
    for (uint16_t i = 0; i < num_ids; ++i)
    {
        pmsg->property_ids[i] = pproperty_ids[i];
    }
    mesh_msg_send_cause_t ret = generic_client_property_send(pmodel_info, dst, app_key_index,
                                                             (uint8_t *)pmsg,
                                                             msg_len, delay_time);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

static bool generic_client_property_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_CLIENT_PROPERTIES_GET:
        if (pmesh_msg->msg_len == sizeof(generic_client_properties_get_t))
        {
            generic_client_properties_get_t *pmsg = (generic_client_properties_get_t *)pbuffer;
            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            generic_client_property_server_get_t get_data = {pmsg->property_id, NULL, 0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, GENERIC_CLIENT_PROPERTY_SERVER_GET, &get_data);
            }
            generic_client_properties_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                             get_data.pproperty_ids, get_data.num_ids, delay_rsp_time);
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool generic_client_property_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_CLIENT_PROPERTY_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_client_property_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_admin_property_server_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}




