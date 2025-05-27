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
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif
#include "model_property.h"

#define IS_LC_TIME_PROPERTY(value)   ((value) >= 0x36) && ((value) <= 0x3c))

static mesh_msg_send_cause_t light_lc_setup_server_send(mesh_model_info_p pmodel_info,
                                                        uint16_t dst, uint8_t *pmsg, uint16_t msg_len, uint16_t app_key_index,
                                                        uint32_t delay_time)
{
    mesh_msg_t mesh_msg = {0};
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

static mesh_msg_send_cause_t light_lc_property_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                                      uint16_t app_key_index, uint16_t property_id, const uint8_t *pproperty_value, uint16_t len,
                                                      uint32_t delay_time)
{
    uint16_t msg_len = sizeof(light_lc_property_status_t) + len;
    light_lc_property_status_t *pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_LIGHT_LC_PROPERTY_STATUS);
    pmsg->property_id = property_id;
    memcpy(pmsg->property_value, pproperty_value, len);

    mesh_msg_send_cause_t ret = light_lc_setup_server_send(pmodel_info, dst, (uint8_t *)pmsg, msg_len,
                                                           app_key_index, delay_time);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

static mesh_msg_send_cause_t light_lc_property_delay_publish(const mesh_model_info_p pmodel_info,
                                                             uint16_t property_id, const uint8_t *pproperty_value,
                                                             uint16_t len, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = light_lc_property_status(pmodel_info, 0, 0, property_id, pproperty_value, len,
                                       delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t light_lc_property_publish(const mesh_model_info_p pmodel_info,
                                                uint16_t property_id, const uint8_t *pproperty_value,
                                                uint16_t len)
{
    return light_lc_property_delay_publish(pmodel_info, property_id, pproperty_value, len, 0);
}

bool light_lc_property_value_check(uint16_t property_id, const uint8_t *value, uint16_t value_len)
{
    UNUSED(value);
    if (MODEL_PROPERTY_INVALID == property_id)
    {
        return FALSE;
    }

    uint16_t wanted_len = 0;
    if (property_id == MODEL_PROPERTY_LIGHT_CONTORL_REGULATOR_ACCURACY)
    {
        wanted_len = 1;
    }
    if (property_id >= MODEL_PROPERTY_LIGHT_CONTORL_LIGHTNESS_ON &&
        property_id <= MODEL_PROPERTY_LIGHT_CONTORL_LIGHTNESS_STANDBY)
    {
        wanted_len = 2;
    }
    else if ((property_id >= MODEL_PROPERTY_LIGHT_CONTORL_TIME_FADE &&
              property_id <= MODEL_PROPERTY_LIGHT_CONTROL_TIME_RUN_ON) ||
             (property_id >= MODEL_PROPERTY_LIGHT_CONTORL_AMBIENT_LUXLEVEL_ON &&
              property_id <= MODEL_PROPERTY_LIGHT_CONTORL_AMBIENT_LUXLEVEL_STANDBY))
    {
        wanted_len = 3;
    }
    else if (property_id >= MODEL_PROPERTY_LIGHT_CONTORL_REGULATOR_KID &&
             property_id <= MODEL_PROPERTY_LIGHT_CONTORL_REGULATOR_KPU)
    {
        wanted_len = 4;
    }

    return value_len == wanted_len;
}

static bool light_lc_setup_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_LIGHT_LC_PROPERTY_GET:
        if (pmesh_msg->msg_len == sizeof(light_lc_property_get_t))
        {
            light_lc_property_get_t *pmsg = (light_lc_property_get_t *)pbuffer;
            if (MODEL_PROPERTY_INVALID != pmsg->property_id)
            {
                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                light_lc_server_get_property_t get_data = {pmsg->property_id, 0, 0};
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_LC_SERVER_GET_PROPERTY, &get_data);
                }
                light_lc_property_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                         get_data.property_id, (uint8_t *)&get_data.property_value, get_data.value_len,
                                         delay_rsp_time);
            }
        }
        break;
    case MESH_MSG_LIGHT_LC_PROPERTY_SET:
    case MESH_MSG_LIGHT_LC_PROPERTY_SET_UNACK:
        {
            light_lc_property_set_t *pmsg = (light_lc_property_set_t *)pbuffer;
            uint16_t value_len = pmesh_msg->msg_len - MEMBER_OFFSET(light_lc_property_set_t, property_value);

            if (light_lc_property_value_check(pmsg->property_id, pmsg->property_value, value_len))
            {
                bool property_changed = FALSE;
                if (NULL != pmesh_msg->pmodel_info->model_data_cb)
                {
                    light_lc_server_get_property_t property_before_set = {pmsg->property_id, 0, 0};
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_LC_SERVER_GET_PROPERTY, &property_before_set);

                    light_lc_server_set_property_t set_data;
                    set_data.property_id = pmsg->property_id;
                    set_data.property_value = 0;
                    for (uint8_t i = value_len; i > 0; --i)
                    {
                        set_data.property_value <<= 8;
                        set_data.property_value |= pmsg->property_value[i - 1];
                    }

                    pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, LIGHT_LC_SERVER_SET_PROPERTY,
                                                          &set_data);

                    light_lc_server_get_property_t property_after_set = {pmsg->property_id, 0, 0};
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_LC_SERVER_GET_PROPERTY, &property_after_set);

                    /* compare property before and after set */
                    if (property_before_set.property_value != property_after_set.property_value)
                    {
                        property_changed = TRUE;
                    }
                }
                (void)property_changed;

                uint32_t delay_rsp_time = 0;
                if (MESH_MSG_LIGHT_LC_PROPERTY_SET == pmesh_msg->access_opcode)
                {
#if MODEL_ENABLE_DELAY_MSG_RSP
                    delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                    light_lc_property_status(pmesh_msg->pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                             pmsg->property_id, pmsg->property_value, value_len, delay_rsp_time);
                }

#if MODEL_ENABLE_DELAY_MSG_RSP
                bool ack = (pmesh_msg->access_opcode == MESH_MSG_LIGHT_LC_PROPERTY_SET) ? FALSE : TRUE;
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                if (property_changed)
#endif
                {
                    generic_transition_time_t trans_time = {0, 0};
                    uint32_t delay_pub_time = delay_msg_get_trans_delay(0, trans_time, delay_rsp_time, TRUE,
                                                                        ack);
                    light_lc_property_delay_publish(pmesh_msg->pmodel_info, pmsg->property_id, pmsg->property_value,
                                                    value_len, delay_pub_time);
                }
#else
                light_lc_property_publish(pmesh_msg->pmodel_info, pmsg->property_id, pmsg->property_value,
                                          value_len);
#endif
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool light_lc_setup_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_LIGHT_LC_SETUP_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = light_lc_setup_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("light_xyl_setup_server_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}

