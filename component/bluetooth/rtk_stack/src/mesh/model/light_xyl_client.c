/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     light_xyl_client.c
* @brief    Source file for light xyl client model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2019-07-11
* @version  v1.0
* *************************************************************************************
*/
#include "light_xyl.h"

static mesh_msg_send_cause_t light_xyl_client_send(const mesh_model_info_p pmodel_info,
                                                   uint16_t dst,
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

mesh_msg_send_cause_t light_xyl_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                    uint16_t app_key_index)
{
    light_xyl_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_GET);
    return light_xyl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_xyl_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                    uint16_t app_key_index, light_xyl_t xyl, uint8_t tid,
                                    bool optional, generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    light_xyl_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(light_xyl_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(light_xyl_set_t, trans_time);
    }
    msg.xyl = xyl;
    msg.tid = tid;
    return light_xyl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

mesh_msg_send_cause_t light_xyl_target_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                           uint16_t app_key_index)
{
    light_xyl_target_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_TARGET_GET);
    return light_xyl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_xyl_default_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                            uint16_t app_key_index)
{
    light_xyl_default_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_DEFAULT_GET);
    return light_xyl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_xyl_default_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                            uint16_t app_key_index, light_xyl_t xyl, bool ack)
{
    light_xyl_default_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_DEFAULT_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_DEFAULT_SET_UNACK);
    }
    msg.xyl = xyl;
    return light_xyl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_xyl_range_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                          uint16_t app_key_index)
{
    light_xyl_range_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_RANGE_GET);
    return light_xyl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_xyl_range_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                          uint16_t app_key_index, uint16_t xyl_x_range_min, uint16_t xyl_x_range_max,
                                          uint16_t xyl_y_range_min, uint16_t xyl_y_range_max, bool ack)
{
    light_xyl_range_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_RANGE_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_RANGE_SET_UNACK);
    }
    msg.xyl_x_range_min = xyl_x_range_min;
    msg.xyl_x_range_max = xyl_x_range_max;
    msg.xyl_y_range_min = xyl_y_range_min;
    msg.xyl_y_range_max = xyl_y_range_max;
    return light_xyl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

static bool light_xyl_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_LIGHT_XYL_STATUS:
        {
            light_xyl_status_t *pmsg = (light_xyl_status_t *)pbuffer;
            light_xyl_client_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.optional = FALSE;
            status_data.xyl = pmsg->xyl;
            if (pmesh_msg->msg_len == sizeof(light_xyl_status_t))
            {
                status_data.optional = TRUE;
                status_data.remaining_time = pmsg->remaining_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_XYL_CLIENT_STATUS,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_XYL_TARGET_STATUS:
        {
            light_xyl_target_status_t *pmsg = (light_xyl_target_status_t *)pbuffer;
            light_xyl_client_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.optional = FALSE;
            status_data.xyl = pmsg->xyl;
            if (pmesh_msg->msg_len == sizeof(light_xyl_target_status_t))
            {
                status_data.optional = TRUE;
                status_data.remaining_time = pmsg->remaining_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_XYL_CLIENT_STATUS_TARGET,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_XYL_DEFAULT_STATUS:
        if (pmesh_msg->msg_len == sizeof(light_xyl_default_status_t))
        {
            light_xyl_default_status_t *pmsg = (light_xyl_default_status_t *)pbuffer;
            light_xyl_client_status_default_t default_data;
            default_data.src = pmesh_msg->src;
            default_data.xyl = pmsg->xyl;

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_XYL_CLIENT_STATUS_DEFAULT,
                                                      &default_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_XYL_RANGE_STATUS:
        if (pmesh_msg->msg_len == sizeof(light_xyl_range_status_t))
        {
            light_xyl_range_status_t *pmsg = (light_xyl_range_status_t *)pbuffer;
            light_xyl_client_status_range_t range_data = {pmesh_msg->src, pmsg->status_code, pmsg->xyl_x_range_min, pmsg->xyl_x_range_max, pmsg->xyl_y_range_min, pmsg->xyl_y_range_max};

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info,
                                                      LIGHT_XYL_CLIENT_STATUS_RANGE,
                                                      &range_data);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool light_xyl_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_LIGHT_XYL_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = light_xyl_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("light_xyl_client_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}

