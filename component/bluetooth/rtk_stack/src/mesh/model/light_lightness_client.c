/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     light_lightness_client.c
* @brief    Source file for light lightness client model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2017-12-22
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "light_lightness.h"

static mesh_msg_send_cause_t light_lightness_client_send(const mesh_model_info_p pmodel_info,
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

mesh_msg_send_cause_t light_lightness_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                          uint16_t app_key_index)
{
    light_lightness_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_GET);
    return light_lightness_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_lightness_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                          uint16_t app_key_index, uint16_t lightness, uint8_t tid, bool optional,
                                          generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    light_lightness_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(light_lightness_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(light_lightness_set_t, trans_time);
    }
    msg.lightness = lightness;
    msg.tid = tid;
    return light_lightness_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

mesh_msg_send_cause_t light_lightness_linear_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                 uint16_t app_key_index)
{
    light_lightness_linear_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_LINEAR_GET);
    return light_lightness_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_lightness_linear_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                 uint16_t app_key_index, uint16_t lightness, uint8_t tid, bool optional,
                                                 generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    light_lightness_linear_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(light_lightness_linear_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(light_lightness_linear_set_t, trans_time);
    }
    msg.lightness = lightness;
    msg.tid = tid;
    return light_lightness_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

mesh_msg_send_cause_t light_lightness_last_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                               uint16_t app_key_index)
{
    light_lightness_last_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_LAST_GET);
    return light_lightness_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_lightness_default_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                  uint16_t app_key_index)
{
    light_lightness_default_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_GET);
    return light_lightness_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_lightness_default_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                  uint16_t app_key_index, uint16_t lightness, bool ack)
{
    light_lightness_default_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK);
    }
    msg.lightness = lightness;
    return light_lightness_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_lightness_range_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                uint16_t app_key_index)
{
    light_lightness_range_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_RANGE_GET);
    return light_lightness_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_lightness_range_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                uint16_t app_key_index, uint16_t range_min, uint16_t range_max, bool ack)
{
    light_lightness_range_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_RANGE_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_RANGE_SET_UNACK);
    }
    msg.range_min = range_min;
    msg.range_max = range_max;
    return light_lightness_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

static bool light_lightness_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_LIGHT_LIGHTNESS_STAT:
        {
            light_lightness_stat_t *pmsg = (light_lightness_stat_t *)pbuffer;
            light_lightness_client_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.optional = FALSE;
            status_data.present_lightness = pmsg->present_lightness;
            if (pmesh_msg->msg_len == sizeof(light_lightness_stat_t))
            {
                status_data.optional = TRUE;
                status_data.target_lightness = pmsg->target_lightness;
                status_data.remaining_time = pmsg->remaining_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_LIGHTNESS_CLIENT_STATUS,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_LIGHTNESS_LINEAR_STAT:
        {
            light_lightness_linear_stat_t *pmsg = (light_lightness_linear_stat_t *)pbuffer;
            light_lightness_client_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.optional = FALSE;
            status_data.present_lightness = pmsg->present_lightness;
            if (pmesh_msg->msg_len == sizeof(light_lightness_linear_stat_t))
            {
                status_data.optional = TRUE;
                status_data.target_lightness = pmsg->target_lightness;
                status_data.remaining_time = pmsg->remaining_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_LIGHTNESS_CLIENT_STATUS_LINEAR,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_LIGHTNESS_LAST_STAT:
        if (pmesh_msg->msg_len == sizeof(light_lightness_last_stat_t))
        {
            light_lightness_last_stat_t *pmsg = (light_lightness_last_stat_t *)pbuffer;
            light_lightness_client_status_last_t status_last = {pmesh_msg->src, pmsg->lightness};
            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_LIGHTNESS_CLIENT_STATUS_LAST,
                                                      &status_last);
            }
        }
        break;
    case MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_STAT:
        if (pmesh_msg->msg_len == sizeof(light_lightness_default_stat_t))
        {
            light_lightness_default_stat_t *pmsg = (light_lightness_default_stat_t *)pbuffer;
            light_lightness_client_status_default_t status_default = {pmesh_msg->src, pmsg->lightness};
            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_LIGHTNESS_CLIENT_STATUS_DEFAULT,
                                                      &status_default);
            }
        }
        break;
    case MESH_MSG_LIGHT_LIGHTNESS_RANGE_STAT:
        if (pmesh_msg->msg_len == sizeof(light_lightness_range_stat_t))
        {
            light_lightness_range_stat_t *pmsg = (light_lightness_range_stat_t *)pbuffer;
            light_lightness_client_status_range_t status_range = {pmesh_msg->src, pmsg->stat, pmsg->range_min, pmsg->range_max};
            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_LIGHTNESS_CLIENT_STATUS_RANGE,
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

bool light_lightness_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_LIGHT_LIGHTNESS_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = light_lightness_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("light_lightness_client_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}

