/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_power_on_off_server.c
* @brief    Source file for generic power on off server model.
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


mesh_msg_send_cause_t generic_on_power_up_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                               uint16_t app_key_index, generic_on_power_up_t on_power_up, uint32_t delay_time)
{
    generic_on_power_up_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_ON_POWER_UP_STAT);
    msg.on_power_up = on_power_up;

    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = (uint8_t *)&msg;
    mesh_msg.msg_len = sizeof(generic_on_power_up_stat_t);
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    mesh_msg.delay_time = delay_time;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t generic_on_power_up_publish(const mesh_model_info_p pmodel_info,
                                                  generic_on_power_up_t on_power_up)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = generic_on_power_up_stat(pmodel_info, 0, 0, on_power_up, 0);
    }

    return ret;
}

static generic_on_power_up_t get_present_power_on_off(const mesh_model_info_p pmodel_info)
{
    generic_power_on_off_server_get_t get_data = {GENERIC_ON_POWER_UP_OFF};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, GENERIC_POWER_ON_OFF_SERVER_GET,
                                   &get_data);
    }
    return get_data.on_power_up;
}

static bool generic_power_on_off_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    //uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_ON_POWER_UP_GET:
        if (pmesh_msg->msg_len == sizeof(generic_on_power_up_get_t))
        {
            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            generic_on_power_up_stat(pmesh_msg->pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                     get_present_power_on_off(pmesh_msg->pmodel_info), delay_rsp_time);
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

static int32_t generic_power_on_off_server_publish(const mesh_model_info_p pmodel_info,
                                                   bool retrans)
{
	(void) retrans; //avoid warning
    generic_on_power_up_stat(pmodel_info, 0, 0, get_present_power_on_off(pmodel_info), 0);
    return 0;
}

bool generic_power_on_off_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_POWER_ON_OFF_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_power_on_off_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_power_on_off_server_reg: missing model data process callback!");
        }
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = generic_power_on_off_server_publish;
    }

    return mesh_model_reg(element_index, pmodel_info);
}
