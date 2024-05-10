/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     light_lc_setup_server.c
* @brief    Source file for light lc setup server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2019-07-18
* @version  v1.0
* *************************************************************************************
*/
#include "light_lc.h"


static mesh_msg_send_cause_t light_lc_client_send(const mesh_model_info_p pmodel_info,
                                                  uint16_t dst,
                                                  uint16_t app_key_index, uint8_t *pmsg, uint16_t msg_len)
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
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t light_lc_mode_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                        uint16_t app_key_index)
{
    light_lc_mode_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LC_MODE_GET);
    return light_lc_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_lc_mode_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                        uint16_t app_key_index, uint8_t mode, bool ack)
{
    light_lc_mode_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LC_MODE_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LC_MODE_SET_UNACK);
    }
    msg.mode = mode;
    return light_lc_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_lc_om_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                      uint16_t app_key_index)
{
    light_lc_om_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LC_OM_GET);
    return light_lc_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_lc_om_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                      uint16_t app_key_index, uint8_t mode, bool ack)
{
    light_lc_om_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LC_OM_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LC_OM_SET_UNACK);
    }
    msg.mode = mode;
    return light_lc_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_lc_light_on_off_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                uint16_t app_key_index)
{
    light_lc_light_on_off_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LC_LIGHT_ON_OFF_GET);
    return light_lc_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_lc_light_on_off_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                uint16_t app_key_index, uint8_t light_on_off, uint8_t tid, bool optional,
                                                generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
    light_lc_light_on_off_set_t msg;
    uint32_t len;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LC_LIGHT_ON_OFF_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LC_LIGHT_ON_OFF_SET_UNACK);
    }

    if (optional)
    {
        len = sizeof(light_lc_light_on_off_set_t);
        msg.trans_time = trans_time;
        msg.delay = delay;
    }
    else
    {
        len = MEMBER_OFFSET(light_lc_light_on_off_set_t, trans_time);
    }
    msg.light_on_off = light_on_off;
    msg.tid = tid;
    return light_lc_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, len);
}

mesh_msg_send_cause_t light_lc_property_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                            uint16_t app_key_index, uint16_t property_id)
{
    light_lc_property_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LC_PROPERTY_GET);
    msg.property_id = property_id;
    return light_lc_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t light_lc_property_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                            uint16_t app_key_index, uint16_t property_id,
                                            uint8_t *pvalue, uint16_t value_len, bool ack)
{
    uint16_t msg_len = sizeof(light_lc_property_set_t) + value_len;
    light_lc_property_set_t *pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    if (ack)
    {
        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_LIGHT_LC_PROPERTY_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_LIGHT_LC_PROPERTY_SET_UNACK);
    }
    pmsg->property_id = property_id;
    memcpy(pmsg->property_value, pvalue, value_len);
    mesh_msg_send_cause_t ret = light_lc_client_send(pmodel_info, dst, app_key_index, (uint8_t *)pmsg,
                                                     msg_len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

static bool light_lc_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_LIGHT_LC_MODE_STATUS:
        if (pmesh_msg->msg_len == sizeof(light_lc_mode_status_t))
        {
            light_lc_mode_status_t *pmsg = (light_lc_mode_status_t *)pbuffer;
            light_lc_client_mode_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.mode = pmsg->mode;

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_LC_CLIENT_MODE_STATUS,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_LC_OM_STATUS:
        if (pmesh_msg->msg_len == sizeof(light_lc_om_status_t))
        {
            light_lc_om_status_t *pmsg = (light_lc_om_status_t *)pbuffer;
            light_lc_client_om_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.mode = pmsg->mode;

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_LC_CLIENT_OM_STATUS,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_LC_LIGHT_ON_OFF_STATUS:
        {
            light_lc_light_on_off_status_t *pmsg = (light_lc_light_on_off_status_t *)pbuffer;
            light_lc_client_light_on_off_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.present_on_off = pmsg->present_light_on_off;
            status_data.optional = FALSE;
            if (pmesh_msg->msg_len == sizeof(light_lc_light_on_off_status_t))
            {
                status_data.optional = TRUE;
                status_data.target_on_off = pmsg->target_light_on_off;
                status_data.remaining_time = pmsg->trans_time;
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_LC_CLIENT_LIGHT_ON_OFF_STATUS,
                                                      &status_data);
            }
        }
        break;
    case MESH_MSG_LIGHT_LC_PROPERTY_STATUS:
        {
            light_lc_property_status_t *pmsg = (light_lc_property_status_t *)pbuffer;
            uint8_t value_len = pmesh_msg->msg_len - sizeof(light_lc_property_status_t);
            light_lc_client_property_status_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.property_id = pmsg->property_id;
            status_data.property_value = 0;
            for (uint8_t i = value_len; i > 0; --i)
            {
                status_data.property_value <<= 8;
                status_data.property_value |= pmsg->property_value[i - 1];
            }

            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_LC_CLIENT_PROPERTY_STATUS,
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

bool light_lc_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_LIGHT_LC_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = light_lc_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("light_lc_client_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}

