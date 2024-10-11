/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     scheduler_server.c
* @brief    Source file for scheduler server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-9-14
* @version  v1.0
* *************************************************************************************
*/

#include "scheduler.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif

static mesh_msg_send_cause_t scheduler_server_send(mesh_model_info_p pmodel_info,
                                                   uint16_t dst, void *pmsg, uint16_t msg_len, uint16_t app_key_index,
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

mesh_msg_send_cause_t scheduler_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                       uint16_t app_key_index, uint16_t schedulers,
                                       uint32_t delay_time)
{
    scheduler_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCHEDULER_STATUS);
    msg.schedulers = schedulers;

    return scheduler_server_send(pmodel_info, dst, &msg, sizeof(msg), app_key_index, delay_time);
}

mesh_msg_send_cause_t scheduler_action_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                              uint16_t app_key_index, bool optional, scheduler_register_t scheduler,
                                              uint32_t delay_time)
{
    scheduler_action_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SCHEDULER_ACTION_STATUS);
    msg.scheduler = scheduler;

    uint16_t msg_len = sizeof(msg);
    if (!optional)
    {
        msg_len = MEMBER_OFFSET(scheduler_action_status_t, scheduler) + 1;
        msg.scheduler.year = 0;
    }

    return scheduler_server_send(pmodel_info, dst, &msg, msg_len, app_key_index, delay_time);
}

mesh_msg_send_cause_t scheduler_delay_publish(const mesh_model_info_p pmodel_info,
                                              scheduler_register_t scheduler, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = scheduler_action_status(pmodel_info, 0, 0, true, scheduler, delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t scheduler_publish(const mesh_model_info_p pmodel_info,
                                        scheduler_register_t scheduler)
{
    return scheduler_delay_publish(pmodel_info, scheduler, 0);
}

static bool scheduler_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_SCHEDULER_GET:
        if (pmesh_msg->msg_len == sizeof(scheduler_get_t))
        {
            scheduler_server_get_t get_data = {0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, SCHEDULER_SERVER_GET, &get_data);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            scheduler_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, get_data.schedulers,
                             delay_rsp_time);
        }
        break;
    case MESH_MSG_SCHEDULER_ACTION_GET:
        if (pmesh_msg->msg_len == sizeof(scheduler_action_get_t))
        {
            scheduler_action_get_t *pmsg = (scheduler_action_get_t *)pbuffer;
            if (IS_SCHEDULER_INDEX_VALID(pmsg->index))
            {
                scheduler_server_get_action_t get_data;
                memset(&get_data, 0, sizeof(get_data));
                if (NULL != pmodel_info->model_data_cb)
                {
                    get_data.index = pmsg->index;
                    pmodel_info->model_data_cb(pmodel_info, SCHEDULER_SERVER_GET_ACTION, &get_data);
                    get_data.scheduler.index = pmsg->index;
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                scheduler_action_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                        get_data.scheduler.action != SCHEDULER_NO_ACTION,
                                        get_data.scheduler, delay_rsp_time);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

static int32_t scheduler_server_publish(mesh_model_info_p pmodel_info, bool retrans)
{
    UNUSED(retrans);
    scheduler_server_get_action_t get_data;
    scheduler_server_get_action_t get_data_zero;
    memset(&get_data_zero, 0, sizeof(scheduler_server_get_action_t));
    if (NULL != pmodel_info->model_data_cb)
    {
        for (uint8_t i = 0; i < 15; ++i)
        {
            memset(&get_data, 0, sizeof(scheduler_server_get_action_t));
            get_data.index = i;
            get_data_zero.index = i;
            pmodel_info->model_data_cb(pmodel_info, SCHEDULER_SERVER_GET_ACTION, &get_data);
            if (0 != memcmp(&get_data, &get_data_zero, sizeof(scheduler_server_get_action_t)))
            {
                scheduler_action_status(pmodel_info, 0, 0, true, get_data.scheduler, 0);
            }
        }
    }

    return 0;
}

bool scheduler_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_SCHEDULER_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = scheduler_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("scheduler_server_reg: missing model data process callback!");
        }
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = scheduler_server_publish;
    }

    return mesh_model_reg(element_index, pmodel_info);
}

