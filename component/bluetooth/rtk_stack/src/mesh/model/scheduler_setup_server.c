/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     scheduler_setup_server.c
* @brief    Source file for scheduler setup server model.
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


extern mesh_msg_send_cause_t scheduler_action_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                                     uint16_t app_key_index, scheduler_register_t scheduler,
                                                     uint32_t delay_time);
extern mesh_msg_send_cause_t scheduler_delay_publish(const mesh_model_info_p pmodel_info,
                                                     scheduler_register_t scheduler, uint32_t delay_time);


typedef struct
{
    bool state_changed;
} scheduler_process_result_t;

static bool scheduler_check_parameter(const scheduler_register_t *pscheduler)
{
    if (!IS_SCHEDULER_INDEX_VALID(pscheduler->index))
    {
        return FALSE;
    }

    if (!IS_SCHEDULER_YEAR_VALIE(pscheduler->year))
    {
        return FALSE;
    }

    if (!IS_SCHEDULER_HOUR_VALID(pscheduler->hour))
    {
        return FALSE;
    }

    return TRUE;
}

static bool scheduler_is_same(const scheduler_register_t *psrc, const scheduler_register_t *pdst)
{
    return ((psrc->index == pdst->index) &&
            (psrc->year == pdst->year) &&
            (psrc->month == pdst->month) &&
            (psrc->day == pdst->day) &&
            (psrc->hour == pdst->hour) &&
            (psrc->minute == pdst->minute) &&
            (psrc->second == pdst->second) &&
            (psrc->day_of_week == pdst->day_of_week) &&
            (psrc->action == pdst->action) &&
            (psrc->num_steps == pdst->num_steps) &&
            (psrc->step_resolution == pdst->step_resolution) &&
            (psrc->scene_number == pdst->scene_number));
}

#if !MODEL_ENABLE_DELAY_MSG_RSP
void scheduler_state_change_publish(const mesh_model_info_p pmodel_info,
                                    scheduler_register_t scheduler, scheduler_process_result_t result)
{
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
    if (result.state_changed)
#endif
    {
        scheduler_publish(pmodel_info, scheduler);
    }
}
#endif

static bool scheduler_setup_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_SCHEDULER_ACTION_SET:
    case MESH_MSG_SCHEDULER_ACTION_SET_UNACK:
        if (pmesh_msg->msg_len == sizeof(scheduler_action_set_t))
        {
            scheduler_action_set_t *pmsg = (scheduler_action_set_t *)pbuffer;
            if (scheduler_check_parameter(&pmsg->scheduler))
            {
                scheduler_server_get_action_t get_data;
                memset(&get_data, 0, sizeof(scheduler_server_get_action_t));
                get_data.index = pmsg->scheduler.index;
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, SCHEDULER_SERVER_GET_ACTION, &get_data);
                    scheduler_server_set_action_t set_data;
                    memset(&set_data, 0, sizeof(scheduler_server_set_action_t));
                    set_data = pmsg->scheduler;
                    pmodel_info->model_data_cb(pmodel_info, SCHEDULER_SERVER_SET_ACTION, &set_data);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (MESH_MSG_SCHEDULER_ACTION_SET == pmesh_msg->access_opcode)
                {
                    scheduler_action_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, pmsg->scheduler,
                                            delay_rsp_time);
                }

                scheduler_process_result_t result =
                {
                    .state_changed = FALSE,
                };
                if (!scheduler_is_same(&get_data.scheduler, &pmsg->scheduler))
                {
                    result.state_changed = TRUE;
                }

#if MODEL_ENABLE_DELAY_MSG_RSP
                generic_transition_time_t trans_time = {0, 0};
                bool ack = (pmesh_msg->access_opcode == MESH_MSG_SCHEDULER_ACTION_SET_UNACK) ? FALSE : TRUE;
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                if (result.state_changed)
#endif
                {
                    (void)result.state_changed; /* fix warning */
                    uint32_t delay_pub_time = delay_msg_get_trans_delay(0, trans_time, delay_rsp_time, TRUE, ack);
                    scheduler_delay_publish(pmodel_info, pmsg->scheduler, delay_pub_time);
                }
#else
                scheduler_state_change_publish(pmodel_info, pmsg->scheduler, result);
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

bool scheduler_setup_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_SCHEDULER_SETUP_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = scheduler_setup_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("scheduler_server_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}


