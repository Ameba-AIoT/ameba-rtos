/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     light_ctl_server.c
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


typedef struct
{
    uint8_t tid;
    uint16_t target_lightness;
    uint16_t target_temperature;
    uint16_t target_delta_uv;
    generic_transition_time_t trans_time;
    uint32_t delay_time;
#if MODEL_ENABLE_DELAY_MSG_RSP
    uint32_t delay_pub_time;
#endif
} light_ctl_info_t;

typedef struct
{
    bool state_changed;
    bool use_transition;
} light_ctl_process_result_t;


int16_t light_ctl_temperature_to_generic_level(uint16_t temperature)
{
    return DIVISION_ROUND((temperature - LIGHT_CTL_TEMPERATURE_LOWER_LIMIT) * 65535,
                          LIGHT_CTL_TEMPERATURE_DELTA) -
           32768;
}

uint16_t generic_level_to_light_ctl_temperature(int16_t level)
{
    return LIGHT_CTL_TEMPERATURE_LOWER_LIMIT + DIVISION_ROUND((level + 32768) *
                                                              LIGHT_CTL_TEMPERATURE_DELTA, 65535);
}

float light_ctl_delta_uv_to_represented_delta_uv(int16_t delta_uv)
{
    return delta_uv / 32768.0;
}

int16_t light_represented_delta_uv_to_ctl_delta_uv(float delta_uv)
{
    return delta_uv * 32768;
}

static mesh_msg_send_cause_t light_ctl_server_send(mesh_model_info_p pmodel_info, uint16_t dst,
                                                   uint8_t *pmsg, uint16_t msg_len, uint16_t app_key_index,
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

static mesh_msg_send_cause_t light_ctl_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                            uint16_t app_key_index, uint16_t present_lightness, uint16_t present_temperature, bool optional,
                                            uint16_t target_lightness, uint16_t target_temperature, generic_transition_time_t remaining_time,
                                            uint32_t delay_time)
{
    light_ctl_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_STAT);
    uint16_t msg_len;
    if (optional)
    {
        msg_len = sizeof(light_ctl_stat_t);
        msg.target_lightness = target_lightness;
        msg.target_temperature = target_temperature;
        msg.remaining_time = remaining_time;
    }
    else
    {
        msg_len = MEMBER_OFFSET(light_ctl_stat_t, target_lightness);
    }
    msg.present_lightness = present_lightness;
    msg.present_temperature = present_temperature;
    return light_ctl_server_send(pmodel_info, dst, (uint8_t *)&msg, msg_len, app_key_index, delay_time);
}

mesh_msg_send_cause_t light_ctl_delay_publish(const mesh_model_info_p pmodel_info,
                                              uint16_t lightness,
                                              uint16_t temperature, uint32_t delay_time)
{
    UNUSED(delay_time);
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        generic_transition_time_t trans_time = {0, 0};
        ret = light_ctl_stat(pmodel_info, 0, 0, lightness, temperature, FALSE, 0, 0, trans_time, 0);
    }

    return ret;
}

mesh_msg_send_cause_t light_ctl_publish(const mesh_model_info_p pmodel_info, uint16_t lightness,
                                        uint16_t temperature)
{
    return light_ctl_delay_publish(pmodel_info, lightness, temperature, 0);
}

#if !MODEL_ENABLE_DELAY_MSG_RSP
static void light_ctl_state_change_publish(const mesh_model_info_p pmodel_info, uint16_t lightness,
                                           uint16_t temperature, light_ctl_process_result_t result)
{
    if (result.use_transition)
    {
        return ;
    }

#if !MODEL_ENABLE_PUBLISH_ALL_TIME
    if (result.state_changed)
#endif
    {
        light_ctl_publish(pmodel_info, lightness, temperature);
    }
}
#endif

/* this message is also used by light ctl temperature model */
mesh_msg_send_cause_t light_ctl_temperature_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                 uint16_t app_key_index, uint16_t present_temperature, uint16_t present_delta_uv, bool optional,
                                                 uint16_t target_temperature, uint16_t target_delta_uv, generic_transition_time_t remaining_time,
                                                 uint32_t delay_time)
{
    light_ctl_temperature_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_TEMPERATURE_STAT);
    uint16_t msg_len;
    if (optional)
    {
        msg_len = sizeof(light_ctl_temperature_stat_t);
        msg.target_temperature = target_temperature;
        msg.target_delta_uv = target_delta_uv;
        msg.remaining_time = remaining_time;
    }
    else
    {
        msg_len = MEMBER_OFFSET(light_ctl_temperature_stat_t, target_temperature);
    }
    msg.present_temperature = present_temperature;
    msg.present_delta_uv = present_delta_uv;
    return light_ctl_server_send(pmodel_info, dst, (uint8_t *)&msg, msg_len, app_key_index, delay_time);
}

mesh_msg_send_cause_t light_ctl_temperature_range_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                       uint16_t app_key_index, generic_stat_t stat, uint16_t range_min, uint16_t range_max,
                                                       uint32_t delay_time)
{
    light_ctl_temperature_range_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_STAT);
    msg.stat = stat;
    msg.range_min = range_min;
    msg.range_max = range_max;
    return light_ctl_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg), app_key_index,
                                 delay_time);
}

mesh_msg_send_cause_t light_ctl_default_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                             uint16_t app_key_index, uint16_t lightness, uint16_t temperature, int16_t delta_uv,
                                             uint32_t delay_time)
{
    light_ctl_default_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CTL_DEFAULT_STAT);
    msg.lightness = lightness;
    msg.temperature = temperature;
    msg.delta_uv = delta_uv;
    return light_ctl_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg), app_key_index,
                                 delay_time);
}

static light_ctl_server_get_t get_present_ctl(const mesh_model_info_p pmodel_info)
{
    light_ctl_server_get_t get_data = {0, 0};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_GET, &get_data);
    }

    return get_data;
}

static int32_t light_ctl_trans_step_change(const mesh_model_info_p pmodel_info,
                                           uint32_t type,
                                           generic_transition_time_t total_time,
                                           generic_transition_time_t remaining_time)
{
    UNUSED(type);
    int32_t ret = MODEL_SUCCESS;
    light_ctl_server_set_t trans_set_data;
    light_ctl_info_t *pctl_info = pmodel_info->pargs;
    if (NULL == pctl_info)
    {
        return 0;
    }
    trans_set_data.lightness = pctl_info->target_lightness;
    trans_set_data.temperature = pctl_info->target_temperature;
    trans_set_data.delta_uv = pctl_info->target_delta_uv;
    trans_set_data.total_time = total_time;
    trans_set_data.remaining_time = remaining_time;

    if (NULL != pmodel_info->model_data_cb)
    {
        ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_SET, &trans_set_data);
    }

    if (0 == remaining_time.num_steps)
    {
        light_ctl_server_get_t get_data = {0, 0};
        get_data = get_present_ctl(pmodel_info);
#if MODEL_ENABLE_DELAY_MSG_RSP
        light_ctl_delay_publish(pmodel_info, get_data.lightness, get_data.temperature,
                                pctl_info->delay_pub_time);
#else
        light_ctl_publish(pmodel_info, get_data.lightness, get_data.temperature);
#endif
    }

    return ret;
}

static light_ctl_server_get_t light_ctl_process(const mesh_model_info_p pmodel_info,
                                                uint16_t target_lightness,
                                                uint16_t target_temperature, int16_t target_delta_uv,
                                                generic_transition_time_t trans_time,
                                                light_ctl_process_result_t *presult)
{
    light_ctl_server_get_t ctl_before_set = {0, 0};
    light_ctl_server_get_t ctl_after_set = {0, 0};

    /* get ctl before set */
    ctl_before_set = get_present_ctl(pmodel_info);
    ctl_after_set = ctl_before_set;

    int32_t ret = MODEL_SUCCESS;
    light_ctl_server_set_t trans_set_data;
    trans_set_data.lightness = target_lightness;
    trans_set_data.temperature = target_temperature;
    trans_set_data.delta_uv = target_delta_uv;
    trans_set_data.total_time = trans_time;
    trans_set_data.remaining_time = trans_time;

    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_SET, &trans_set_data);
    }

    if (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE != trans_time.num_steps)
    {
        if ((ret >= 0) && (MODEL_STOP_TRANSITION != ret))
        {
            if (NULL != presult)
            {
                presult->use_transition = TRUE;
            }
            generic_transition_timer_start(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_CTL, trans_time,
                                           light_ctl_trans_step_change);
        }
#if MODEL_ENABLE_USER_STOP_TRANSITION_NOTIFICATION
        else if (MODEL_STOP_TRANSITION == ret)
        {
            if (NULL != pmodel_info->model_data_cb)
            {
                ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_SET, &trans_set_data);
            }
        }
#endif
    }
    else
    {
        /* get ctl after set */
        ctl_after_set = get_present_ctl(pmodel_info);
    }


    if ((ctl_after_set.lightness != ctl_before_set.lightness) ||
        (ctl_after_set.temperature != ctl_before_set.temperature))
    {
        if (NULL != presult)
        {
            presult->state_changed = TRUE;
        }
    }

    return ctl_after_set;
}

static int32_t light_ctl_delay_execution(mesh_model_info_t *pmodel_info, uint32_t delay_type)
{
    switch (delay_type)
    {
    case DELAY_EXECUTION_TYPE_CTL:
        {
            light_ctl_info_t *pctl_info = pmodel_info->pargs;
            if (NULL == pctl_info)
            {
                return 0;
            }
            pctl_info->delay_time = 0;
            light_ctl_process_result_t result =
            {
                .state_changed = FALSE,
                .use_transition = FALSE
            };
            light_ctl_server_get_t present_ctl = light_ctl_process(pmodel_info, pctl_info->target_lightness,
                                                                   pctl_info->target_temperature, pctl_info->target_delta_uv,
                                                                   pctl_info->trans_time, &result);
#if MODEL_ENABLE_DELAY_MSG_RSP
            if (!result.use_transition)
            {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                if (result.state_changed)
#endif
                {
                    light_ctl_delay_publish(pmodel_info, present_ctl.lightness, present_ctl.temperature,
                                            pctl_info->delay_pub_time);
                }
            }
#else
            light_ctl_state_change_publish(pmodel_info, present_ctl.lightness, present_ctl.temperature, result);
#endif
        }
        break;
    default:
        break;
    }

    return 0;
}

static bool light_ctl_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_LIGHT_CTL_GET:
        if (pmesh_msg->msg_len == sizeof(light_ctl_get_t))
        {
            light_ctl_info_t *pctl_info = pmodel_info->pargs;
            generic_transition_time_t remaining_time;
            if (pctl_info->delay_time > 0)
            {
                remaining_time = pctl_info->trans_time;
            }
            else
            {
                remaining_time = generic_transition_time_get(pmesh_msg->pmodel_info,
                                                             GENERIC_TRANSITION_TYPE_LIGHT_CTL);
            }

            light_ctl_server_get_t get_data = {0, 0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_GET, &get_data);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_ctl_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                           get_data.lightness, get_data.temperature,
                           (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == remaining_time.num_steps) ? FALSE : TRUE,
                           pctl_info->target_lightness, pctl_info->target_temperature, remaining_time,
                           delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_CTL_SET:
    case MESH_MSG_LIGHT_CTL_SET_UNACK:
        {
            light_ctl_set_t *pmsg = (light_ctl_set_t *)pbuffer;
            generic_transition_time_t trans_time = {GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE, 0};
            uint32_t delay_time = 0;
            if (pmesh_msg->msg_len == MEMBER_OFFSET(light_ctl_set_t, trans_time))
            {
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_GET_DEFAULT_TRANSITION_TIME, &trans_time);
                }
            }
            else if (pmesh_msg->msg_len == sizeof(light_ctl_set_t))
            {
                trans_time = pmsg->trans_time;
                delay_time = pmsg->delay * DELAY_EXECUTION_STEP_RESOLUTION;
            }
            if (IS_GENERIC_TRANSITION_STEPS_VALID(trans_time.num_steps) &&
                IS_LIGHT_CTL_TEMPERATURE_VALID(pmsg->temperature))
            {
                light_ctl_server_get_temperature_range_t range = {0, 0};
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_GET_TEMPERATURE_RANGE, &range);
                }

                light_ctl_info_t *pctl_info = pmodel_info->pargs;
                if ((0 != range.range_min) && (0 != range.range_max))
                {
                    pctl_info->target_temperature = CLAMP(pmsg->temperature, range.range_min, range.range_max);
                }
                else
                {
                    pctl_info->target_temperature = pmsg->temperature;
                }
                pctl_info->target_lightness = pmsg->lightness;
                pctl_info->target_delta_uv = pmsg->delta_uv;
                pctl_info->tid = pmsg->tid;
                pctl_info->trans_time = trans_time;
                pctl_info->delay_time = delay_time;

                light_ctl_server_get_t present_ctl;
                light_ctl_process_result_t result =
                {
                    .state_changed = FALSE,
                    .use_transition = FALSE
                };

                if (delay_time > 0)
                {
                    result.use_transition = TRUE;
                    present_ctl = get_present_ctl(pmodel_info);
                    delay_execution_timer_start(pmodel_info, DELAY_EXECUTION_TYPE_CTL, delay_time,
                                                light_ctl_delay_execution);
                }
                else
                {
                    present_ctl = light_ctl_process(pmodel_info, pctl_info->target_lightness,
                                                    pctl_info->target_temperature, pctl_info->target_delta_uv,
                                                    trans_time, &result);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (pmesh_msg->access_opcode == MESH_MSG_LIGHT_CTL_SET)
                {
                    light_ctl_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                   present_ctl.lightness, present_ctl.temperature,
                                   (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == trans_time.num_steps) ? FALSE : TRUE,
                                   pctl_info->target_lightness, pctl_info->target_temperature, trans_time, delay_rsp_time);
                }
#if MODEL_ENABLE_DELAY_MSG_RSP
                bool ack = (pmesh_msg->access_opcode == MESH_MSG_LIGHT_CTL_SET_UNACK) ? FALSE : TRUE;
                if (!result.use_transition)
                {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                    if (result.state_changed)
#endif
                    {
                        uint32_t delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time, TRUE,
                                                                            ack);
                        light_ctl_delay_publish(pmodel_info, present_ctl.lightness, present_ctl.temperature,
                                                delay_pub_time);
                    }
                }
                else
                {
                    pctl_info->delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time, FALSE,
                                                                          ack);
                }
#else
                light_ctl_state_change_publish(pmodel_info, present_ctl.lightness, present_ctl.temperature, result);
#endif
            }
        }
        break;
    case MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_GET:
        if (pmesh_msg->msg_len == sizeof(light_ctl_temperature_range_get_t))
        {
            light_ctl_server_get_temperature_range_t get_range = {0, 0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info,
                                           LIGHT_CTL_SERVER_GET_TEMPERATURE_RANGE, &get_range);
            }
            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_ctl_temperature_range_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                             GENERIC_STAT_SUCCESS, get_range.range_min,
                                             get_range.range_max, delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_CTL_DEFAULT_GET:
        if (pmesh_msg->msg_len == sizeof(light_ctl_default_get_t))
        {
            light_ctl_server_get_default_t get_default = {0, 0, 0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_GET_DEFAULT,
                                           &get_default);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_ctl_default_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                   get_default.lightness, get_default.temperature,
                                   get_default.delta_uv, delay_rsp_time);
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

static int32_t light_ctl_server_publish(mesh_model_info_p pmodel_info, bool retrans)
{
    UNUSED(retrans);
    light_ctl_server_get_t get_data = {0, 0};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, LIGHT_CTL_SERVER_GET, &get_data);
    }
    generic_transition_time_t trans_time = {0, 0};
    light_ctl_stat(pmodel_info, 0, 0, get_data.lightness, get_data.temperature, FALSE, 0, 0,
                   trans_time, 0);

    return 0;
}

#if MESH_MODEL_ENABLE_DEINIT
static void light_ctl_server_deinit(mesh_model_info_t *pmodel_info)
{
    if (pmodel_info->model_receive == light_ctl_server_receive)
    {
        /* stop delay execution */
        delay_execution_timer_stop(pmodel_info, DELAY_EXECUTION_TYPE_CTL);
        /* stop transition */
        generic_transition_timer_stop(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_CTL);

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

bool light_ctl_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_LIGHT_CTL_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        light_ctl_info_t *pctl_info = plt_malloc(sizeof(light_ctl_info_t),
                                                 RAM_TYPE_DATA_ON);
        if (NULL == pctl_info)
        {
            printe("light_ctl_server_reg: fail to allocate memory for the new model extension data!");
            return FALSE;
        }
        memset(pctl_info, 0, sizeof(light_ctl_info_t));
        pmodel_info->pargs = pctl_info;

        pmodel_info->model_receive = light_ctl_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("light_ctl_server_reg: missing model data process callback!");
        }
#if MESH_MODEL_ENABLE_DEINIT
        pmodel_info->model_deinit = light_ctl_server_deinit;
#endif
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = light_ctl_server_publish;
    }

    generic_transition_time_init();
    delay_execution_init();
    return mesh_model_reg(element_index, pmodel_info);
}

