/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     light_ctl_client.c
* @brief    Source file for light ctl client model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2017-12-22
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "light_ctl.h"

static mesh_msg_send_cause_t light_ctl_client_send(mesh_model_info_p pmodel_info, uint16_t dst,
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

mesh_msg_send_cause_t light_ctl_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                    uint16_t app_key_index)
{
    light_ctl_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_GET);
    return light_ctl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_ctl_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                    uint16_t app_key_index, uint16_t lightness, uint16_t temperature, int16_t delta_uv, uint8_t tid,
                                    bool optional, generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    light_ctl_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(light_ctl_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(light_ctl_set_t, trans_time);
    }
    msg.lightness = lightness;
    msg.temperature = temperature;
    msg.delta_uv = delta_uv;
    msg.tid = tid;
    return light_ctl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

mesh_msg_send_cause_t light_ctl_temperature_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                uint16_t app_key_index)
{
    light_ctl_temperature_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_TEMPERATURE_GET);
    return light_ctl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_ctl_temperature_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                uint16_t app_key_index, uint16_t temperature, int16_t delta_uv, uint8_t tid, bool optional,
                                                generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    light_ctl_temperature_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_TEMPERATURE_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_TEMPERATURE_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(light_ctl_temperature_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(light_ctl_temperature_set_t, trans_time);
    }
    msg.temperature = temperature;
    msg.delta_uv = delta_uv;
    msg.tid = tid;
    return light_ctl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

mesh_msg_send_cause_t light_ctl_temperature_range_get(const mesh_model_info_p pmodel_info,
                                                      uint16_t dst,
                                                      uint16_t app_key_index)
{
    light_ctl_temperature_range_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_GET);
    return light_ctl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_ctl_temperature_range_set(const mesh_model_info_p pmodel_info,
                                                      uint16_t dst,
                                                      uint16_t app_key_index, uint16_t range_min, uint16_t range_max, bool ack)
{
    light_ctl_temperature_range_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK);
    }

    msg.range_min = range_min;
    msg.range_max = range_max;
    return light_ctl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_ctl_default_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                            uint16_t app_key_index)
{
    light_ctl_default_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_DEFAULT_GET);
    return light_ctl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_ctl_default_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                            uint16_t app_key_index, uint16_t lightness, uint16_t temperature, int16_t delta_uv, bool ack)
{
    light_ctl_default_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_DEFAULT_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_DEFAULT_SET_UNACK);
    }
    msg.lightness = lightness;
    msg.temperature = temperature;
    msg.delta_uv = delta_uv;
    return light_ctl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

static bool light_ctl_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_LIGHT_CTL_STAT:
        {
            light_ctl_stat_t *pmsg = (light_ctl_stat_t *)pbuffer;
            light_ctl_client_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.optional = FALSE;
            status_data.present_lightness = pmsg->present_lightness;
            status_data.present_temperature = pmsg->present_temperature;
            if (pmesh_msg->msg_len == sizeof(light_ctl_stat_t))
            {
                status_data.optional = TRUE;
                status_data.target_lightness = pmsg->target_lightness;
                status_data.target_temperature = pmsg->target_temperature;
                status_data.remaining_time = pmsg->remaining_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_CTL_CLIENT_STATUS,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_CTL_TEMPERATURE_STAT:
        {
            light_ctl_temperature_stat_t *pmsg = (light_ctl_temperature_stat_t *)pbuffer;
            light_ctl_client_status_temperature_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.optional = FALSE;
            status_data.present_temperature = pmsg->present_temperature;
            status_data.present_delta_uv = pmsg->present_delta_uv;
            if (pmesh_msg->msg_len == sizeof(light_ctl_stat_t))
            {
                status_data.optional = TRUE;
                status_data.target_temperature = pmsg->target_temperature;
                status_data.target_delta_uv = pmsg->target_delta_uv;
                status_data.remaining_time = pmsg->remaining_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_CTL_CLIENT_STATUS_TEMPERATURE,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_STAT:
        if (pmesh_msg->msg_len == sizeof(light_ctl_temperature_range_stat_t))
        {
            light_ctl_temperature_range_stat_t *pmsg = (light_ctl_temperature_range_stat_t *)pbuffer;
            light_ctl_client_status_temperature_range_t range_data = {pmesh_msg->src, pmsg->stat, pmsg->range_min, pmsg->range_max};

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info,
                                                      LIGHT_CTL_CLIENT_STATUS_TEMPERATURE_RANGE,
                                                      &range_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_CTL_DEFAULT_STAT:
        if (pmesh_msg->msg_len == sizeof(light_ctl_default_stat_t))
        {
            light_ctl_default_stat_t *pmsg = (light_ctl_default_stat_t *)pbuffer;
            light_ctl_client_status_default_t default_data = {pmesh_msg->src, pmsg->lightness, pmsg->temperature, pmsg->delta_uv};

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_CTL_CLIENT_STATUS_DEFAULT,
                                                      &default_data);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool light_ctl_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_LIGHT_CTL_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = light_ctl_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("light_ctl_client_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}

