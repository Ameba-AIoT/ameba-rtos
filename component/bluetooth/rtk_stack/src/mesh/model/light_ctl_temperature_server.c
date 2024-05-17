/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_on_off_server.c
* @brief    Source file for generic on off server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-8-1
* @version  v1.0
* *************************************************************************************
*/

#include "light_ctl.h"
#include "delay_execution.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif

extern mesh_msg_send_cause_t light_ctl_temperature_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                        uint16_t app_key_index, uint16_t present_temperature, uint16_t present_delta_uv, bool optional,
                                                        uint16_t target_temperature, uint16_t target_delta_uv, generic_transition_time_t remaining_time,
                                                        uint32_t delay_time);

typedef struct
{
    uint8_t tid;
    uint16_t target_temperature;
    uint16_t target_delta_uv;
    generic_transition_time_t trans_time;
    uint32_t delay_time;
#if MODEL_ENABLE_DELAY_MSG_RSP
    uint32_t delay_pub_time;
#endif
} light_ctl_temperature_info_t, *light_ctl_temperature_info_p;

typedef struct
{
    bool state_changed;
    bool use_transition;
} light_ctl_temperature_process_result_t;


static light_ctl_server_get_temperature_t get_present_temperature(const mesh_model_info_p
                                                                  pmodel_info)
{
    light_ctl_server_get_temperature_t get_data = {0};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_GET_TEMPERATURE, &get_data);
    }

    return get_data;
}

mesh_msg_send_cause_t light_ctl_temperature_delay_publish(const mesh_model_info_p pmodel_info,
                                                          uint16_t temperature, int16_t delta_uv,
                                                          uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        generic_transition_time_t trans_time = {0, 0};
        ret = light_ctl_temperature_stat(pmodel_info, 0, 0, temperature, delta_uv, FALSE, 0, 0,
                                         trans_time, delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t light_ctl_temperature_publish(const mesh_model_info_p pmodel_info,
                                                    uint16_t temperature, int16_t delta_uv)
{
    return light_ctl_temperature_delay_publish(pmodel_info, temperature, delta_uv, 0);
}

#if !MODEL_ENABLE_DELAY_MSG_RSP
static void light_ctl_temperature_state_change_publish(const mesh_model_info_p pmodel_info,
                                                       uint16_t temperature, int16_t delta_uv,
                                                       light_ctl_temperature_process_result_t result)
{
    if (result.use_transition)
    {
        return ;
    }

#if !MODEL_ENABLE_PUBLISH_ALL_TIME
    if (result.state_changed)
#endif
    {
        light_ctl_temperature_publish(pmodel_info, temperature, delta_uv);
    }
}
#endif

static int32_t light_ctl_temperature_trans_step_change(const mesh_model_info_p pmodel_info,
                                                       uint32_t type,
                                                       generic_transition_time_t total_time,
                                                       generic_transition_time_t remaining_time)
{
    UNUSED(type);
    int32_t ret = 0;
    light_ctl_server_set_temperature_t trans_set_data;
    light_ctl_temperature_info_p ptemp_info = pmodel_info->pargs;
    trans_set_data.temperature = ptemp_info->target_temperature;
    trans_set_data.delta_uv = ptemp_info->target_delta_uv;
    trans_set_data.total_time = total_time;
    trans_set_data.remaining_time = remaining_time;

    if (NULL != pmodel_info->model_data_cb)
    {
        ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_SET_TEMPERATURE,
                                         &trans_set_data);
    }

    if (0 == remaining_time.num_steps)
    {
        light_ctl_server_get_temperature_t get_data = {0, 0};
        get_data = get_present_temperature(pmodel_info);
#if MODEL_ENABLE_DELAY_MSG_RSP
        light_ctl_temperature_delay_publish(pmodel_info, get_data.temperature, get_data.delta_uv,
                                            ptemp_info->delay_pub_time);
#else
        light_ctl_temperature_publish(pmodel_info, get_data.temperature, get_data.delta_uv);
#endif
    }

    return ret;
}

static light_ctl_server_get_temperature_t light_ctl_temperature_process(
    const mesh_model_info_p pmodel_info,
    uint16_t target_temperature, int16_t target_delta_uv,
    generic_transition_time_t trans_time,
    light_ctl_temperature_process_result_t *presult)
{
    light_ctl_server_get_temperature_t temperature_before_set = {0, 0};
    light_ctl_server_get_temperature_t temperature_after_set = {0, 0};

    /* get temperature before set */
    temperature_before_set = get_present_temperature(pmodel_info);
    temperature_after_set = temperature_before_set;

    int32_t ret = MODEL_SUCCESS;
    light_ctl_server_set_temperature_t trans_set_data;
    trans_set_data.temperature = target_temperature;
    trans_set_data.delta_uv = target_delta_uv;
    trans_set_data.total_time = trans_time;
    trans_set_data.remaining_time = trans_time;

    if (NULL != pmodel_info->model_data_cb)
    {
        ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_SET_TEMPERATURE,
                                         &trans_set_data);
    }

    if (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE != trans_time.num_steps)
    {
        if ((ret >= 0) && (MODEL_STOP_TRANSITION != ret))
        {
            if (NULL != presult)
            {
                presult->use_transition = TRUE;
            }
            generic_transition_timer_start(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_CTL_TEMPERATURE,
                                           trans_time,
                                           light_ctl_temperature_trans_step_change);
        }
#if MODEL_ENABLE_USER_STOP_TRANSITION_NOTIFICATION
        else if (MODEL_STOP_TRANSITION == ret)
        {
            if (NULL != pmodel_info->model_data_cb)
            {
                ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_SET_TEMPERATURE, &trans_set_data);
            }
        }
#endif
    }
    else
    {
        /* get temperature set */
        temperature_after_set = get_present_temperature(pmodel_info);
    }

    if ((temperature_after_set.temperature != temperature_before_set.temperature) ||
        (temperature_after_set.delta_uv != temperature_before_set.delta_uv))
    {
        if (NULL != presult)
        {
            presult->state_changed = TRUE;
        }
    }

    return temperature_after_set;
}

static int32_t light_ctl_temperature_delay_execution(mesh_model_info_t *pmodel_info,
                                                     uint32_t delay_type)
{
    switch (delay_type)
    {
    case DELAY_EXECUTION_TYPE_CTL_TEMPERATURE:
        {
            light_ctl_temperature_info_p ptemp_info = pmodel_info->pargs;
            ptemp_info->delay_time = 0;
            light_ctl_temperature_process_result_t result =
            {
                .state_changed = FALSE,
                .use_transition = FALSE
            };
            light_ctl_server_get_temperature_t present_temperature = light_ctl_temperature_process(pmodel_info,
                                                                     ptemp_info->target_temperature,
                                                                     ptemp_info->target_delta_uv, ptemp_info->trans_time, &result);
#if MODEL_ENABLE_DELAY_MSG_RSP
            if (!result.use_transition)
            {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                if (result.state_changed)
#endif
                {
                    light_ctl_temperature_delay_publish(pmodel_info, present_temperature.temperature,
                                                        present_temperature.delta_uv, ptemp_info->delay_pub_time);
                }
            }
#else
            light_ctl_temperature_state_change_publish(pmodel_info, present_temperature.temperature,
                                                       present_temperature.delta_uv, result);
#endif
        }
        break;
    default:
        break;
    }

    return 0;
}

static bool light_ctl_temperature_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_LIGHT_CTL_TEMPERATURE_GET:
        if (pmesh_msg->msg_len == sizeof(light_ctl_temperature_get_t))
        {
            light_ctl_temperature_info_p ptemp_info = pmodel_info->pargs;
            generic_transition_time_t remaining_time;
            if (ptemp_info->delay_time > 0)
            {
                remaining_time = ptemp_info->trans_time;
            }
            else
            {
                remaining_time = generic_transition_time_get(pmesh_msg->pmodel_info,
                                                             GENERIC_TRANSITION_TYPE_LIGHT_CTL_TEMPERATURE);
            }

            light_ctl_server_get_temperature_t get_data = get_present_temperature(pmodel_info);

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_ctl_temperature_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                       get_data.temperature, get_data.delta_uv,
                                       (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == remaining_time.num_steps) ? FALSE : TRUE,
                                       ptemp_info->target_temperature, ptemp_info->target_delta_uv, remaining_time, delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_CTL_TEMPERATURE_SET:
    case MESH_MSG_LIGHT_CTL_TEMPERATURE_SET_UNACK:
        {
            light_ctl_temperature_set_t *pmsg = (light_ctl_temperature_set_t *)pbuffer;
            generic_transition_time_t trans_time = {GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE, 0};
            uint32_t delay_time = 0;
            if (pmesh_msg->msg_len == MEMBER_OFFSET(light_ctl_temperature_set_t, trans_time))
            {
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_GET_DEFAULT_TRANSITION_TIME, &trans_time);
                }
            }
            else if (pmesh_msg->msg_len == sizeof(light_ctl_temperature_set_t))
            {
                trans_time = pmsg->trans_time;
                delay_time = pmsg->delay * DELAY_EXECUTION_STEP_RESOLUTION;
            }
            if (IS_GENERIC_TRANSITION_STEPS_VALID(trans_time.num_steps) &&
                IS_LIGHT_CTL_TEMPERATURE_VALID(pmsg->temperature))
            {
                light_ctl_temperature_info_t *ptemp_info = pmodel_info->pargs;
                light_ctl_server_get_temperature_range_t range = {0, 0};
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_GET_TEMPERATURE_RANGE, &range);
                }

                if ((0 != range.range_min) && (0 != range.range_max))
                {
                    ptemp_info->target_temperature = CLAMP(pmsg->temperature, range.range_min, range.range_max);
                }
                else
                {
                    ptemp_info->target_temperature = pmsg->temperature;
                }
                ptemp_info->target_delta_uv = pmsg->delta_uv;
                ptemp_info->tid = pmsg->tid;
                ptemp_info->trans_time = trans_time;
                ptemp_info->delay_time = delay_time;

                light_ctl_server_get_temperature_t present_temperature;
                light_ctl_temperature_process_result_t result =
                {
                    .state_changed = FALSE,
                    .use_transition = FALSE
                };
                if (delay_time > 0)
                {
                    result.use_transition = TRUE;
                    present_temperature = get_present_temperature(pmodel_info);
                    delay_execution_timer_start(pmodel_info, DELAY_EXECUTION_TYPE_CTL_TEMPERATURE, delay_time,
                                                light_ctl_temperature_delay_execution);
                }
                else
                {
                    present_temperature = light_ctl_temperature_process(pmodel_info, ptemp_info->target_temperature,
                                                                        ptemp_info->target_delta_uv, trans_time, &result);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (pmesh_msg->access_opcode == MESH_MSG_LIGHT_CTL_TEMPERATURE_SET)
                {
                    light_ctl_temperature_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                               present_temperature.temperature, present_temperature.delta_uv,
                                               (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == trans_time.num_steps) ? FALSE : TRUE,
                                               ptemp_info->target_temperature, ptemp_info->target_delta_uv, trans_time, delay_rsp_time);
                }
#if MODEL_ENABLE_DELAY_MSG_RSP
                bool ack = (pmesh_msg->access_opcode == MESH_MSG_LIGHT_CTL_TEMPERATURE_SET_UNACK) ? FALSE : TRUE;
                if (!result.use_transition)
                {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                    if (result.state_changed)
#endif
                    {
                        uint32_t delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time, TRUE,
                                                                            ack);
                        light_ctl_temperature_delay_publish(pmodel_info, present_temperature.temperature,
                                                            present_temperature.delta_uv, delay_pub_time);
                    }
                }
                else
                {
                    ptemp_info->delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time,
                                                                           FALSE, ack);
                }
#else
                light_ctl_temperature_state_change_publish(pmodel_info, present_temperature.temperature,
                                                           present_temperature.delta_uv, result);
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

static int32_t light_ctl_temperature_server_publish(mesh_model_info_p pmodel_info, bool retrans)
{
    UNUSED(retrans);
    light_ctl_server_get_temperature_t get_data = get_present_temperature(pmodel_info);;
    generic_transition_time_t trans_time = {0, 0};
    light_ctl_temperature_stat(pmodel_info, 0, 0, get_data.temperature, get_data.delta_uv, FALSE, 0, 0,
                               trans_time, 0);

    return 0;
}

#if MESH_MODEL_ENABLE_DEINIT
static void light_ctl_temperature_server_deinit(mesh_model_info_t *pmodel_info)
{
    if (pmodel_info->model_receive == light_ctl_temperature_server_receive)
    {
        /* stop delay execution */
        delay_execution_timer_stop(pmodel_info, DELAY_EXECUTION_TYPE_CTL_TEMPERATURE);
        /* stop transition */
        generic_transition_timer_stop(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_CTL_TEMPERATURE);

        /* now we can remove */
        if (NULL != pmodel_info->pargs)
        {
            plt_free(pmodel_info->pargs, RAM_TYPE_DATA_ON);
            pmodel_info->pargs = NULL;
        }
        pmodel_info->model_receive = NULL;
    }
}
#endif

bool light_ctl_temperature_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_LIGHT_CTL_TEMPERATURE_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        light_ctl_temperature_info_p ptemp_info = plt_malloc(sizeof(light_ctl_temperature_info_t),
                                                             RAM_TYPE_DATA_ON);
        if (NULL == ptemp_info)
        {
            printe("light_ctl_temperature_server_reg: fail to allocate memory for the new model extension data!");
            return FALSE;
        }
        memset(ptemp_info, 0, sizeof(light_ctl_temperature_info_t));
        pmodel_info->pargs = ptemp_info;

        pmodel_info->model_receive = light_ctl_temperature_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("light_ctl_temperature_server_reg: missing model data process callback!");
        }
#if MESH_MODEL_ENABLE_DEINIT
        pmodel_info->model_deinit = light_ctl_temperature_server_deinit;
#endif
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = light_ctl_temperature_server_publish;
    }

    generic_transition_time_init();
    delay_execution_init();
    return mesh_model_reg(element_index, pmodel_info);
}

