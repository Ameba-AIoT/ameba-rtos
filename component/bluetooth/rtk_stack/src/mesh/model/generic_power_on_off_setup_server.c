/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_power_on_off_setup_server.c
* @brief    Source file for generic power on off setup server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-7-17
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "generic_power_on_off.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif


extern mesh_msg_send_cause_t generic_on_power_up_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                      uint16_t app_key_index, generic_on_power_up_t on_power_up, uint32_t delay_time);


static bool generic_power_on_off_setup_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_ON_POWER_UP_SET:
    case MESH_MSG_GENERIC_ON_POWER_UP_SET_UNACK:
        if (pmesh_msg->msg_len == sizeof(generic_on_power_up_set_t))
        {
            generic_on_power_up_set_p pmsg = (generic_on_power_up_set_p)pbuffer;
            if (IS_GENERIC_ON_POWER_UP_VALID(pmsg->on_power_up))
            {
                generic_power_on_off_server_set_t set_data = {pmsg->on_power_up};
                if (NULL != pmesh_msg->pmodel_info->model_data_cb)
                {
                    pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, GENERIC_POWER_ON_OFF_SERVER_SET,
                                                          &set_data);
                }

                if (MESH_MSG_GENERIC_ON_POWER_UP_SET == pmesh_msg->access_opcode)
                {
                    uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                    delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                    generic_on_power_up_stat(pmesh_msg->pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                             pmsg->on_power_up, delay_rsp_time);
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


bool generic_power_on_off_setup_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_POWER_ON_OFF_SETUP_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_power_on_off_setup_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_power_on_off_setup_server_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}
