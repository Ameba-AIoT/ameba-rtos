/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_power_level_setup_server.c
* @brief    Source file for generic on off server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-7-27
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "generic_power_level.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif

extern mesh_msg_send_cause_t generic_power_default_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                        uint16_t app_key_index, uint16_t power, uint32_t delay_time);
extern mesh_msg_send_cause_t generic_power_range_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                      uint16_t app_key_index, generic_stat_t stat,
                                                      uint16_t range_min, uint16_t range_max, uint32_t delay_time);

static bool generic_power_level_setup_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_POWER_DEFAULT_SET:
    case MESH_MSG_GENERIC_POWER_DEFAULT_SET_UNACK:
        if (pmesh_msg->msg_len == sizeof(generic_power_default_set_t))
        {
            generic_power_default_set_p pmsg = (generic_power_default_set_p)pbuffer;
            generic_power_level_server_set_default_t set_default = {pmsg->power};
            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info,
                                                      GENERIC_POWER_LEVEL_SERVER_SET_DEFAULT, &set_default);
            }

            if (MESH_MSG_GENERIC_POWER_DEFAULT_SET == pmesh_msg->access_opcode)
            {
                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                generic_power_default_stat(pmesh_msg->pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                           pmsg->power, delay_rsp_time);
            }
        }
        break;
    case MESH_MSG_GENERIC_POWER_RANGE_SET:
    case MESH_MSG_GENERIC_POWER_RANGE_SET_UNACK:
        if (pmesh_msg->msg_len == sizeof(generic_power_range_set_t))
        {
            generic_power_range_set_p pmsg = (generic_power_range_set_p)pbuffer;
            if ((pmsg->range_min <= pmsg->range_max) &&
                (0 != pmsg->range_min) &&
                (0 != pmsg->range_max))
            {
                generic_power_level_server_set_range_t set_range = {pmsg->range_min, pmsg->range_max};
                if (NULL != pmesh_msg->pmodel_info->model_data_cb)
                {
                    pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, GENERIC_POWER_LEVEL_SERVER_SET_RANGE,
                                                          &set_range);
                }

                if (MESH_MSG_GENERIC_POWER_RANGE_SET == pmesh_msg->access_opcode)
                {
                    uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                    delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                    generic_power_range_stat(pmesh_msg->pmodel_info, pmesh_msg->src,
                                             pmesh_msg->app_key_index, GENERIC_STAT_SUCCESS,
                                             pmsg->range_min, pmsg->range_max, delay_rsp_time);
                }
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool generic_power_level_setup_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_POWER_LEVEL_SETUP_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_power_level_setup_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_power_level_setup_server_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}
