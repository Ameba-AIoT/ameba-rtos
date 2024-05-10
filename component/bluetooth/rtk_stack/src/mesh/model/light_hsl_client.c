/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     light_hsl_client.c
* @brief    Source file for light hsl client model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2017-12-22
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "light_hsl.h"

static mesh_msg_send_cause_t light_hsl_client_send(const mesh_model_info_p pmodel_info,
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

mesh_msg_send_cause_t light_hsl_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                    uint16_t app_key_index)
{
    light_hsl_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_GET);
    return light_hsl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_hsl_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                    uint16_t app_key_index, uint16_t lightness, uint16_t hue, uint16_t saturation, uint8_t tid,
                                    bool optional, generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    light_hsl_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(light_hsl_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(light_hsl_set_t, trans_time);
    }
    msg.lightness = lightness;
    msg.hue = hue;
    msg.saturation = saturation;
    msg.tid = tid;
    return light_hsl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

mesh_msg_send_cause_t light_hsl_target_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                           uint16_t app_key_index)
{
    light_hsl_target_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_TARGET_GET);
    return light_hsl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_hsl_hue_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                        uint16_t app_key_index)
{
    light_hsl_hue_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_HUE_GET);
    return light_hsl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_hsl_hue_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                        uint16_t app_key_index, uint16_t hue, uint8_t tid, bool optional,
                                        generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    light_hsl_hue_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_HUE_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_HUE_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(light_hsl_hue_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(light_hsl_hue_set_t, trans_time);
    }
    msg.hue = hue;
    msg.tid = tid;
    return light_hsl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

mesh_msg_send_cause_t light_hsl_saturation_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                               uint16_t app_key_index)
{
    light_hsl_saturation_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_SATURATION_GET);
    return light_hsl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_hsl_saturation_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                               uint16_t app_key_index, uint16_t saturation, uint8_t tid, bool optional,
                                               generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    light_hsl_saturation_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_SATURATION_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_SATURATION_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(light_hsl_saturation_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(light_hsl_saturation_set_t, trans_time);
    }
    msg.saturation = saturation;
    msg.tid = tid;
    return light_hsl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

mesh_msg_send_cause_t light_hsl_default_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                            uint16_t app_key_index)
{
    light_hsl_default_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_DEFAULT_GET);
    return light_hsl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_hsl_default_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                            uint16_t app_key_index, uint16_t lightness, uint16_t hue, uint16_t saturation, bool ack)
{
    light_hsl_default_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_DEFAULT_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_DEFAULT_SET_UNACK);
    }
    msg.lightness = lightness;
    msg.hue = hue;
    msg.saturation = saturation;
    return light_hsl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_hsl_range_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                          uint16_t app_key_index)
{
    light_hsl_range_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_RANGE_GET);
    return light_hsl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_hsl_range_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                          uint16_t app_key_index, uint16_t hue_range_min, uint16_t hue_range_max,
                                          uint16_t saturation_range_min, uint16_t saturation_range_max, bool ack)
{
    light_hsl_range_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_RANGE_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_RANGE_SET_UNACK);
    }
    msg.hue_range_min = hue_range_min;
    msg.hue_range_max = hue_range_max;
    msg.saturation_range_min = saturation_range_min;
    msg.saturation_range_max = saturation_range_max;
    return light_hsl_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

static bool light_hsl_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_LIGHT_HSL_STAT:
        {
            light_hsl_stat_t *pmsg = (light_hsl_stat_t *)pbuffer;
            light_hsl_client_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.optional = FALSE;
            status_data.lightness = pmsg->lightness;
            status_data.hue = pmsg->hue;
            status_data.saturation = pmsg->saturation;
            if (pmesh_msg->msg_len == sizeof(light_hsl_stat_t))
            {
                status_data.optional = TRUE;
                status_data.remaining_time = pmsg->remaining_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_HSL_CLIENT_STATUS,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_HSL_TARGET_STAT:
        {
            light_hsl_target_stat_t *pmsg = (light_hsl_target_stat_t *)pbuffer;
            light_hsl_client_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.optional = FALSE;
            status_data.lightness = pmsg->lightness;
            status_data.hue = pmsg->hue;
            status_data.saturation = pmsg->saturation;
            if (pmesh_msg->msg_len == sizeof(light_hsl_target_stat_t))
            {
                status_data.optional = TRUE;
                status_data.remaining_time = pmsg->remaining_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_HSL_CLIENT_STATUS_TARGET,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_HSL_HUE_STAT:
        {
            light_hsl_hue_stat_t *pmsg = (light_hsl_hue_stat_t *)pbuffer;
            light_hsl_client_status_hue_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.optional = FALSE;
            status_data.present_hue = pmsg->present_hue;
            if (pmesh_msg->msg_len == sizeof(light_hsl_hue_stat_t))
            {
                status_data.optional = TRUE;
                status_data.target_hue = pmsg->target_hue;
                status_data.remaining_time = pmsg->remaining_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_HSL_CLIENT_STATUS_HUE,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_HSL_SATURATION_STAT:
        {
            light_hsl_saturation_stat_t *pmsg = (light_hsl_saturation_stat_t *)pbuffer;
            light_hsl_client_status_saturation_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.optional = FALSE;
            status_data.present_saturation = pmsg->present_saturation;
            if (pmesh_msg->msg_len == sizeof(light_hsl_saturation_stat_t))
            {
                status_data.optional = TRUE;
                status_data.target_saturation = pmsg->target_saturation;
                status_data.remaining_time = pmsg->remaining_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_HSL_CLIENT_STATUS_SATURATION,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_HSL_DEFAULT_STAT:
        if (pmesh_msg->msg_len == sizeof(light_hsl_default_stat_t))
        {
            light_hsl_default_stat_t *pmsg = (light_hsl_default_stat_t *)pbuffer;
            light_hsl_client_status_default_t default_data = {pmesh_msg->src, pmsg->lightness, pmsg->hue, pmsg->saturation};

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_HSL_CLIENT_STATUS_DEFAULT,
                                                      &default_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_HSL_RANGE_STAT:
        if (pmesh_msg->msg_len == sizeof(light_hsl_range_stat_t))
        {
            light_hsl_range_stat_t *pmsg = (light_hsl_range_stat_t *)pbuffer;
            light_hsl_client_status_range_t range_data = {pmesh_msg->src, pmsg->stat, pmsg->hue_range_min, pmsg->hue_range_max, pmsg->saturation_range_min, pmsg->saturation_range_max};

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info,
                                                      LIGHT_HSL_CLIENT_STATUS_RANGE,
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

bool light_hsl_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_LIGHT_HSL_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = light_hsl_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("light_hsl_client_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}

