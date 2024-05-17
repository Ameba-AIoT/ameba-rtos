/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     light_lightness_server.c
* @brief    Source file for light lightness server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-7-30
* @version  v1.0
* *************************************************************************************
*/

#include <math.h>
#include "light_lightness.h"
#include "delay_execution.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif


typedef struct
{
    uint8_t tid;
    uint16_t target_lightness;
    uint16_t target_lightness_linear;
    generic_transition_time_t trans_time;
    uint32_t delay_time;
    generic_transition_time_t trans_time_linear;
    uint32_t delay_time_linear;
#if MODEL_ENABLE_DELAY_MSG_RSP
    uint32_t delay_pub_time;
    uint32_t delay_pub_time_linear;
#endif
} light_lightness_info_t;

typedef struct
{
    bool state_changed;
    bool use_transition;
} light_lightness_process_result_t;

static bool lightness_period_pub_enabled = TRUE;
static bool lightness_linear_period_pub_enabled = TRUE;


uint16_t light_lightness_linear_to_actual(uint16_t lightness_linear)
{
    return (uint16_t)(65535 * sqrt(lightness_linear / 65535.0));
}

uint16_t light_lightness_actual_to_linear(uint16_t lightness_actual)
{
    return (uint16_t)(lightness_actual / 65535.0 * lightness_actual);
}

int16_t light_lightness_to_generic_level(uint16_t lightness)
{
    return lightness - 32768;
}

uint16_t generic_level_to_light_lightness(int16_t level)
{
    return level + 32768;
}

int16_t light_lightness_linear_to_generic_level(uint16_t lightness)
{
    return light_lightness_to_generic_level(light_lightness_linear_to_actual(lightness));
}

uint16_t generic_level_to_light_lightness_linear(int16_t level)
{
    return light_lightness_actual_to_linear(generic_level_to_light_lightness(level));
}

static mesh_msg_send_cause_t light_lightness_server_send(mesh_model_info_p pmodel_info,
                                                         uint16_t dst, uint8_t *pmsg, uint16_t msg_len, uint16_t app_key_index,
                                                         uint32_t delay_time)
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
    mesh_msg.delay_time = delay_time;
    return access_send(&mesh_msg);
}

static mesh_msg_send_cause_t light_lightness_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                  uint16_t app_key_index, uint16_t present_lightness, bool optional, uint16_t target_lightness,
                                                  generic_transition_time_t remaining_time, bool linear, uint32_t delay_time)
{
    light_lightness_stat_t msg;
    if (linear)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_LINEAR_STAT);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_STAT);
    }
    uint16_t msg_len;
    if (optional)
    {
        msg_len = sizeof(light_lightness_stat_t);
        msg.target_lightness = target_lightness;
        msg.remaining_time = remaining_time;
    }
    else
    {
        msg_len = MEMBER_OFFSET(light_lightness_stat_t, target_lightness);
    }
    msg.present_lightness = present_lightness;
    return light_lightness_server_send(pmodel_info, dst, (uint8_t *)&msg, msg_len, app_key_index,
                                       delay_time);
}

static mesh_msg_send_cause_t light_lightness_delay_publish(const mesh_model_info_p pmodel_info,
                                                           uint16_t lightness, bool linear, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        generic_transition_time_t remaining_time = {};
        ret = light_lightness_stat(pmodel_info, 0, 0, lightness, FALSE, lightness, remaining_time, linear,
                                   delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t light_lightness_publish(const mesh_model_info_p pmodel_info,
                                              uint16_t lightness)
{
    return light_lightness_delay_publish(pmodel_info, lightness, FALSE, 0);
}

#if !MODEL_ENABLE_DELAY_MSG_RSP
static void light_lightness_state_change_publish(const mesh_model_info_p pmodel_info,
                                                 uint16_t lightness, bool linear, light_lightness_process_result_t result)
{
    if (result.use_transition)
    {
        return ;
    }

#if !MODEL_ENABLE_PUBLISH_ALL_TIME
    if (result.state_changed)
#endif
    {
        light_lightness_delay_publish(pmodel_info, lightness, linear, 0);
    }
}
#endif

mesh_msg_send_cause_t light_lightness_linear_publish(const mesh_model_info_p pmodel_info,
                                                     uint16_t lightness)
{
    return light_lightness_delay_publish(pmodel_info, lightness, TRUE, 0);
}

static mesh_msg_send_cause_t light_lightness_last_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                       uint16_t app_key_index, uint16_t lightness, uint32_t delay_time)
{
    light_lightness_last_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_LAST_STAT);
    msg.lightness = lightness;
    return light_lightness_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg), app_key_index,
                                       delay_time);
}

mesh_msg_send_cause_t light_lightness_default_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                   uint16_t app_key_index, uint16_t lightness, uint32_t delay_time)
{
    light_lightness_default_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_STAT);
    msg.lightness = lightness;
    return light_lightness_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg), app_key_index,
                                       delay_time);
}

mesh_msg_send_cause_t light_lightness_range_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                 uint16_t app_key_index, generic_stat_t stat, uint16_t range_min, uint16_t range_max,
                                                 uint32_t delay_time)
{
    light_lightness_range_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LIGHTNESS_RANGE_STAT);
    msg.stat = stat;
    msg.range_min = range_min;
    msg.range_max = range_max;
    return light_lightness_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg), app_key_index,
                                       delay_time);
}

static uint16_t get_present_lightness(const mesh_model_info_p pmodel_info, bool linear)
{
    light_lightness_server_get_t get_data = {0};
    uint32_t get_code = LIGHT_LIGHTNESS_SERVER_GET;
    if (linear)
    {
        get_code = LIGHT_LIGHTNESS_SERVER_GET_LINEAR;
    }
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, get_code, &get_data);
    }

    return get_data.lightness;
}

static int32_t light_lightness_trans_step_change(const mesh_model_info_p pmodel_info,
                                                 uint32_t type,
                                                 generic_transition_time_t total_time,
                                                 generic_transition_time_t remaining_time)
{
    int32_t ret = MODEL_SUCCESS;
    light_lightness_server_set_t set_data;
    light_lightness_info_t *plightness_info = pmodel_info->pargs;
    if (NULL == plightness_info)
    {
        return 0;
    }

    if (type == GENERIC_TRANSITION_TYPE_LIGHT_LIGHTNESS)
    {
        set_data.lightness = plightness_info->target_lightness;
    }
    else
    {
        set_data.lightness = plightness_info->target_lightness_linear;
    }
    set_data.total_time = total_time;
    set_data.remaining_time = remaining_time;

    if (NULL != pmodel_info->model_data_cb)
    {
        if (type == GENERIC_TRANSITION_TYPE_LIGHT_LIGHTNESS)
        {
            ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_LIGHTNESS_SERVER_SET, &set_data);
        }
        else
        {
            ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_LIGHTNESS_SERVER_SET_LINEAR, &set_data);
        }
    }

    if (0 == remaining_time.num_steps)
    {
        bool linear;
        uint32_t delay_rsp_time = 0;
        if (GENERIC_TRANSITION_TYPE_LIGHT_LIGHTNESS == type)
        {
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = plightness_info->delay_pub_time;
#endif
            linear = FALSE;
        }
        else
        {
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = plightness_info->delay_pub_time_linear;
#endif
            linear = TRUE;
        }

        uint16_t present_lightness = get_present_lightness(pmodel_info, linear);
#if MODEL_ENABLE_DELAY_MSG_RSP
        light_lightness_delay_publish(pmodel_info, present_lightness, linear, delay_rsp_time);
#else
        light_lightness_delay_publish(pmodel_info, present_lightness, linear, delay_rsp_time);
#endif
    }

    return ret;
}

static uint16_t light_lightness_process(const mesh_model_info_p pmodel_info,
                                        uint16_t target_lightness, bool linear,
                                        generic_transition_time_t trans_time,
                                        light_lightness_process_result_t *presult)
{
    uint16_t lightness_before_set = 0;
    uint16_t lightness_after_set = 0;
    uint32_t trans_set_code = LIGHT_LIGHTNESS_SERVER_SET;
    uint32_t trans_timer_code = GENERIC_TRANSITION_TYPE_LIGHT_LIGHTNESS;

    if (linear)
    {
        trans_set_code = LIGHT_LIGHTNESS_SERVER_SET_LINEAR;
        trans_timer_code = GENERIC_TRANSITION_TYPE_LIGHT_LIGHTNESS_LINEAR;
    }

    /* get lightness before set */
    lightness_before_set = get_present_lightness(pmodel_info, linear);
    lightness_after_set = lightness_before_set;

    int32_t ret = MODEL_SUCCESS;
    light_lightness_server_set_t trans_set_data;
    trans_set_data.lightness = target_lightness;
    trans_set_data.total_time = trans_time;
    trans_set_data.remaining_time = trans_time;

    if (NULL != pmodel_info->model_data_cb)
    {
        ret = pmodel_info->model_data_cb(pmodel_info, trans_set_code, &trans_set_data);
    }

    if (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE != trans_time.num_steps)
    {
        if ((ret >= 0) && (MODEL_STOP_TRANSITION != ret))
        {
            if (NULL != presult)
            {
                presult->use_transition = TRUE;
            }
            generic_transition_timer_start(pmodel_info, trans_timer_code, trans_time,
                                           light_lightness_trans_step_change);
        }
#if MODEL_ENABLE_USER_STOP_TRANSITION_NOTIFICATION
        else if (MODEL_STOP_TRANSITION == ret)
        {
            if (NULL != pmodel_info->model_data_cb)
            {
                ret = pmodel_info->model_data_cb(pmodel_info, trans_set_code, &trans_set_data);
            }
        }
#endif
    }
    else
    {
        /* get lightness after set */
        lightness_after_set = get_present_lightness(pmodel_info, linear);
    }

    if (lightness_before_set != lightness_after_set)
    {
        if (NULL != presult)
        {
            presult->state_changed = TRUE;
        }
    }

    if (0 != target_lightness)
    {
        if (NULL != pmodel_info->model_data_cb)
        {
            if (linear)
            {
                target_lightness = light_lightness_linear_to_actual(target_lightness);
            }
            light_lightness_server_set_last_t set_last = {target_lightness};
            pmodel_info->model_data_cb(pmodel_info, LIGHT_LIGHTNESS_SERVER_SET_LAST, &set_last);
        }
    }

    return lightness_after_set;
}

static int32_t light_lightness_delay_execution(mesh_model_info_t *pmodel_info, uint32_t delay_type)
{
    switch (delay_type)
    {
    case DELAY_EXECUTION_TYPE_LIGHT_LIGHTNESS:
    case DELAY_EXECUTION_TYPE_LIGHT_LIGHTNESS_LINEAR:
        {
            light_lightness_info_t *plightness_info = pmodel_info->pargs;
            if (NULL == plightness_info)
            {
                return 0;
            }

            light_lightness_process_result_t result =
            {
                .state_changed = FALSE,
                .use_transition = FALSE
            };
            bool linear;
            generic_transition_time_t trans_time;
            uint16_t target_lightness;
            if (DELAY_EXECUTION_TYPE_LIGHT_LIGHTNESS == delay_type)
            {
                plightness_info->delay_time = 0;
                linear = FALSE;
                target_lightness = plightness_info->target_lightness;
                trans_time = plightness_info->trans_time;
            }
            else
            {
                plightness_info->delay_time_linear = 0;
                linear = TRUE;
                target_lightness = plightness_info->target_lightness_linear;
                trans_time = plightness_info->trans_time_linear;
            }

            uint16_t present_lightness = light_lightness_process(pmodel_info, target_lightness, linear,
                                                                 trans_time, &result);
#if MODEL_ENABLE_DELAY_MSG_RSP
            if (!result.use_transition)
            {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                if (result.state_changed)
#endif
                {
                    uint32_t delay_rsp_time;
                    if (DELAY_EXECUTION_TYPE_LIGHT_LIGHTNESS == delay_type)
                    {
                        delay_rsp_time = plightness_info->delay_pub_time;
                    }
                    else
                    {
                        delay_rsp_time = plightness_info->delay_pub_time_linear;
                    }
                    light_lightness_delay_publish(pmodel_info, present_lightness, linear, delay_rsp_time);
                }
            }
#else
            light_lightness_state_change_publish(pmodel_info, present_lightness, linear, result);
#endif
        }
        break;
    default:
        break;
    }

    return 0;
}

static bool light_lightness_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_LIGHT_LIGHTNESS_GET:
        if (pmesh_msg->msg_len == sizeof(light_lightness_get_t))
        {
            light_lightness_info_t *plightness_info = pmodel_info->pargs;
            generic_transition_time_t remaining_time;
            if (plightness_info->delay_time > 0)
            {
                remaining_time = plightness_info->trans_time;
            }
            else
            {
                remaining_time = generic_transition_time_get(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_LIGHTNESS);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_lightness_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                 get_present_lightness(pmodel_info, FALSE),
                                 (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == remaining_time.num_steps) ? FALSE : TRUE,
                                 plightness_info->target_lightness, remaining_time, FALSE, delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_LIGHTNESS_SET:
    case MESH_MSG_LIGHT_LIGHTNESS_SET_UNACK:
        {
            light_lightness_set_t *pmsg = (light_lightness_set_t *)pbuffer;
            generic_transition_time_t trans_time = {GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE, 0};
            uint32_t delay_time = 0;
            if (pmesh_msg->msg_len == MEMBER_OFFSET(light_lightness_set_t, trans_time))
            {
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_LIGHTNESS_SERVER_GET_DEFAULT_TRANSITION_TIME,
                                               &trans_time);
                }
            }
            else if (pmesh_msg->msg_len == sizeof(light_lightness_set_t))
            {
                trans_time = pmsg->trans_time;
                delay_time = pmsg->delay * DELAY_EXECUTION_STEP_RESOLUTION;
            }
            if (IS_GENERIC_TRANSITION_STEPS_VALID(trans_time.num_steps))
            {
                light_lightness_info_t *plightness_info = pmodel_info->pargs;
                light_lightness_server_get_range_t range = {0, 0};
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_LIGHTNESS_SERVER_GET_RANGE, &range);
                }
                if ((0 != range.range_min) && (0 != range.range_max))
                {
                    /* need to clamp lightness */
                    plightness_info->target_lightness = CLAMP(pmsg->lightness, range.range_min, range.range_max);
                }
                else
                {
                    plightness_info->target_lightness = pmsg->lightness;
                }
                plightness_info->tid = pmsg->tid;
                plightness_info->trans_time = trans_time;
                plightness_info->delay_time = delay_time;

                uint16_t present_lightness;
                light_lightness_process_result_t result =
                {
                    .state_changed = FALSE,
                    .use_transition = FALSE
                };
                if (delay_time > 0)
                {
                    result.use_transition = TRUE;
                    present_lightness = get_present_lightness(pmodel_info, FALSE);
                    delay_execution_timer_start(pmodel_info, DELAY_EXECUTION_TYPE_LIGHT_LIGHTNESS, delay_time,
                                                light_lightness_delay_execution);
                }
                else
                {
                    present_lightness = light_lightness_process(pmodel_info, plightness_info->target_lightness,
                                                                FALSE, trans_time, &result);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (pmesh_msg->access_opcode == MESH_MSG_LIGHT_LIGHTNESS_SET)
                {
                    light_lightness_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                         present_lightness,
                                         (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == trans_time.num_steps) ? FALSE : TRUE,
                                         plightness_info->target_lightness, trans_time, FALSE, delay_rsp_time);
                }
#if MODEL_ENABLE_DELAY_MSG_RSP
                bool ack = (pmesh_msg->access_opcode == MESH_MSG_LIGHT_LIGHTNESS_SET_UNACK) ? FALSE : TRUE;
                if (!result.use_transition)
                {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                    if (result.state_changed)
#endif
                    {
                        uint32_t delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time, TRUE,
                                                                            ack);
                        light_lightness_delay_publish(pmodel_info, present_lightness, FALSE, delay_pub_time);
                    }
                }
                else
                {
                    plightness_info->delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time,
                                                                                FALSE, ack);
                }
#else
                light_lightness_state_change_publish(pmodel_info, present_lightness, FALSE, result);
#endif
            }
        }
        break;
    case MESH_MSG_LIGHT_LIGHTNESS_LINEAR_GET:
        if (pmesh_msg->msg_len == sizeof(light_lightness_linear_get_t))
        {
            light_lightness_info_t *plightness_info = pmodel_info->pargs;
            generic_transition_time_t remaining_time;
            if (plightness_info->delay_time_linear)
            {
                remaining_time = plightness_info->trans_time_linear;
            }
            else
            {
                remaining_time = generic_transition_time_get(pmodel_info,
                                                             GENERIC_TRANSITION_TYPE_LIGHT_LIGHTNESS_LINEAR);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_lightness_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                 get_present_lightness(pmodel_info, TRUE),
                                 (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == remaining_time.num_steps) ? FALSE : TRUE,
                                 plightness_info->target_lightness_linear, remaining_time, TRUE, delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET:
    case MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET_UNACK:
        {
            light_lightness_linear_set_t *pmsg = (light_lightness_linear_set_t *)pbuffer;
            generic_transition_time_t trans_time = {GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE, 0};
            uint32_t delay_time = 0;
            if (pmesh_msg->msg_len == MEMBER_OFFSET(light_lightness_linear_set_t, trans_time))
            {
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_LIGHTNESS_SERVER_GET_DEFAULT_TRANSITION_TIME,
                                               &trans_time);
                }
            }
            else if (pmesh_msg->msg_len == sizeof(light_lightness_linear_set_t))
            {
                trans_time = pmsg->trans_time;
                delay_time = pmsg->delay * DELAY_EXECUTION_STEP_RESOLUTION;
            }
            if (IS_GENERIC_TRANSITION_STEPS_VALID(trans_time.num_steps))
            {
                light_lightness_info_t *plightness_info = pmodel_info->pargs;
                plightness_info->target_lightness_linear = pmsg->lightness;
                plightness_info->tid = pmsg->tid;
                plightness_info->trans_time_linear = trans_time;
                plightness_info->delay_time_linear = delay_time;

                uint16_t present_linear;
                light_lightness_process_result_t result =
                {
                    .state_changed = FALSE,
                    .use_transition = FALSE
                };
                if (delay_time > 0)
                {
                    result.use_transition = TRUE;
                    present_linear = get_present_lightness(pmodel_info, TRUE);
                    delay_execution_timer_start(pmodel_info, DELAY_EXECUTION_TYPE_LIGHT_LIGHTNESS_LINEAR, delay_time,
                                                light_lightness_delay_execution);
                }
                else
                {
                    present_linear = light_lightness_process(pmodel_info,
                                                             plightness_info->target_lightness_linear,
                                                             TRUE, trans_time, &result);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (pmesh_msg->access_opcode == MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET)
                {
                    light_lightness_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                         present_linear,
                                         (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == trans_time.num_steps) ? FALSE : TRUE,
                                         plightness_info->target_lightness_linear, trans_time, TRUE, delay_rsp_time);
                }
#if MODEL_ENABLE_DELAY_MSG_RSP
                bool ack = (pmesh_msg->access_opcode == MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET_UNACK) ? FALSE : TRUE;
                if (!result.use_transition)
                {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                    if (result.state_changed)
#endif
                    {
                        uint32_t delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time, TRUE,
                                                                            ack);
                        light_lightness_delay_publish(pmodel_info, present_linear, TRUE, delay_pub_time);
                    }
                }
                else
                {
                    plightness_info->delay_pub_time_linear = delay_msg_get_trans_delay(delay_time, trans_time,
                                                                                       delay_rsp_time, FALSE, ack);
                }
#else
                light_lightness_state_change_publish(pmodel_info, present_linear, TRUE, result);
#endif
            }
        }
        break;
    case MESH_MSG_LIGHT_LIGHTNESS_LAST_GET:
        if (pmesh_msg->msg_len == sizeof(light_lightness_last_get_t))
        {
            light_lightness_server_get_t last_data = {0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, LIGHT_LIGHTNESS_SERVER_GET_LAST,
                                           &last_data);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_lightness_last_stat(pmodel_info, pmesh_msg->src,
                                      pmesh_msg->app_key_index, last_data.lightness, delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_GET:
        if (pmesh_msg->msg_len == sizeof(light_lightness_default_get_t))
        {
            light_lightness_server_get_t default_data = {0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, LIGHT_LIGHTNESS_SERVER_GET_DEFAULT,
                                           &default_data);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_lightness_default_stat(pmodel_info, pmesh_msg->src,
                                         pmesh_msg->app_key_index, default_data.lightness, delay_rsp_time);

        }
        break;
    case MESH_MSG_LIGHT_LIGHTNESS_RANGE_GET:
        if (pmesh_msg->msg_len == sizeof(light_lightness_range_get_t))
        {
            light_lightness_server_get_range_t range_data = {0, 0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, LIGHT_LIGHTNESS_SERVER_GET_RANGE,
                                           &range_data);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_lightness_range_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                       GENERIC_STAT_SUCCESS, range_data.range_min, range_data.range_max,
                                       delay_rsp_time);
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

static int32_t light_lightness_server_publish(mesh_model_info_p pmodel_info, bool retrans)
{
    UNUSED(retrans);
    generic_transition_time_t remaining_time = {};
    if (lightness_period_pub_enabled)
    {
        light_lightness_stat(pmodel_info, 0, 0, get_present_lightness(pmodel_info, FALSE), FALSE, 0,
                             remaining_time, FALSE, 0);
    }

    if (lightness_linear_period_pub_enabled)
    {
        light_lightness_stat(pmodel_info, 0, 0, get_present_lightness(pmodel_info, TRUE), FALSE, 0,
                             remaining_time, TRUE, 0);
    }
    return 0;
}

#if MESH_MODEL_ENABLE_DEINIT
static void light_lightness_server_deinit(mesh_model_info_t *pmodel_info)
{
    if (pmodel_info->model_receive == light_lightness_server_receive)
    {
        /* stop delay execution */
        delay_execution_timer_stop(pmodel_info, DELAY_EXECUTION_TYPE_LIGHT_LIGHTNESS);
        delay_execution_timer_stop(pmodel_info, DELAY_EXECUTION_TYPE_LIGHT_LIGHTNESS_LINEAR);
        /* stop step transition */
        generic_transition_timer_stop(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_LIGHTNESS);
        generic_transition_timer_stop(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_LIGHTNESS_LINEAR);

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

void light_lightness_period_pub_enable(bool lightness, bool lightness_linear)
{
    lightness_period_pub_enabled = lightness;
    lightness_linear_period_pub_enabled = lightness_linear;
}

bool light_lightness_server_reg(uint8_t element_index, mesh_model_info_t *pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_LIGHT_LIGHTNESS_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        light_lightness_info_t *plightness_info = plt_malloc(sizeof(light_lightness_info_t),
                                                             RAM_TYPE_DATA_ON);
        if (NULL == plightness_info)
        {
            printe("light_lightness_server_reg: fail to allocate memory for the new model extension data!");
            return FALSE;
        }
        memset(plightness_info, 0, sizeof(light_lightness_info_t));
        pmodel_info->pargs = plightness_info;

        pmodel_info->model_receive = light_lightness_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("light_lightness_server_reg: missing model data process callback!");
        }
#if MESH_MODEL_ENABLE_DEINIT
        pmodel_info->model_deinit = light_lightness_server_deinit;
#endif
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = light_lightness_server_publish;
    }

    generic_transition_time_init();
    delay_execution_init();
    return mesh_model_reg(element_index, pmodel_info);
}

