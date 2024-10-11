/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     light_hsl_server.c
* @brief    Source file for light hsl server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-8-1
* @version  v1.0
* *************************************************************************************
*/

#include "light_hsl.h"
#include "delay_execution.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif


typedef struct
{
    uint8_t tid;
    uint16_t target_lightness;
    uint16_t target_hue;
    uint16_t target_saturation;
    generic_transition_time_t trans_time;
    uint32_t delay_time;
#if MODEL_ENABLE_DELAY_MSG_RSP
    uint32_t delay_pub_time;
#endif
} light_hsl_info_t;

typedef struct
{
    bool state_changed;
    bool use_transition;
} light_hsl_process_result_t;


static mesh_msg_send_cause_t light_hsl_server_send(mesh_model_info_p pmodel_info, uint16_t dst,
                                                   uint8_t *pmsg, uint16_t msg_len, uint16_t app_key_index, uint32_t delay_time)
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

static mesh_msg_send_cause_t light_hsl_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                            uint16_t app_key_index, uint16_t lightness, uint16_t hue, uint16_t saturation, bool optional,
                                            generic_transition_time_t remaining_time, uint32_t delay_time)
{
    light_hsl_stat_t msg;
    uint32_t len;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_STAT);
    msg.lightness = lightness;
    msg.hue = hue;
    msg.saturation = saturation;
    if (optional)
    {
        len = sizeof(light_hsl_stat_t);
        msg.remaining_time = remaining_time;
    }
    else
    {
        len = MEMBER_OFFSET(light_hsl_stat_t, remaining_time);
    }
    return light_hsl_server_send(pmodel_info, dst, (uint8_t *)&msg, len, app_key_index, delay_time);
}

mesh_msg_send_cause_t light_hsl_delay_publish(const mesh_model_info_p pmodel_info,
                                              uint16_t lightness,
                                              uint16_t hue, uint16_t saturation, uint32_t delay_time)
{
    UNUSED(delay_time);
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        generic_transition_time_t trans_time = {0, 0};
        ret = light_hsl_stat(pmodel_info, 0, 0, lightness, hue, saturation, FALSE, trans_time, 0);
    }

    return ret;
}

mesh_msg_send_cause_t light_hsl_publish(const mesh_model_info_p pmodel_info, uint16_t lightness,
                                        uint16_t hue, uint16_t saturation)
{
    return light_hsl_delay_publish(pmodel_info, lightness, hue, saturation, 0);
}

#if !MODEL_ENABLE_DELAY_MSG_RSP
static void light_hsl_state_change_publish(const mesh_model_info_p pmodel_info, uint16_t lightness,
                                           uint16_t hue, uint16_t saturation, light_hsl_process_result_t result)
{
    if (result.use_transition)
    {
        return ;
    }

#if !MODEL_ENABLE_PUBLISH_ALL_TIME
    if (result.state_changed)
#endif
    {
        light_hsl_publish(pmodel_info, lightness, hue, saturation);
    }
}
#endif

static mesh_msg_send_cause_t light_hsl_target_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                   uint16_t app_key_index, uint16_t lightness, uint16_t hue, uint16_t saturation, bool optional,
                                                   generic_transition_time_t remaining_time, uint32_t delay_time)
{
    light_hsl_target_stat_t msg;
    uint32_t len;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_TARGET_STAT);
    msg.lightness = lightness;
    msg.hue = hue;
    msg.saturation = saturation;
    if (optional)
    {
        len = sizeof(light_hsl_target_stat_t);
        msg.remaining_time = remaining_time;
    }
    else
    {
        len = MEMBER_OFFSET(light_hsl_target_stat_t, remaining_time);
    }
    return light_hsl_server_send(pmodel_info, dst, (uint8_t *)&msg, len, app_key_index, delay_time);
}

mesh_msg_send_cause_t light_hsl_default_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                             uint16_t app_key_index, uint16_t lightness, uint16_t hue, uint16_t saturation,
                                             uint32_t delay_time)
{
    light_hsl_default_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_DEFAULT_STAT);
    msg.lightness = lightness;
    msg.hue = hue;
    msg.saturation = saturation;
    return light_hsl_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg), app_key_index,
                                 delay_time);
}

mesh_msg_send_cause_t light_hsl_range_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                           uint16_t app_key_index, generic_stat_t stat, uint16_t hue_range_min, uint16_t hue_range_max,
                                           uint16_t saturation_range_min, uint16_t saturation_range_max, uint32_t delay_time)
{
    UNUSED(stat);
    light_hsl_range_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_HSL_RANGE_STAT);
    msg.stat = stat;
    msg.hue_range_min = hue_range_min;
    msg.hue_range_max = hue_range_max;
    msg.saturation_range_min = saturation_range_min;
    msg.saturation_range_max = saturation_range_max;
    return light_hsl_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg), app_key_index,
                                 delay_time);
}

static light_hsl_server_get_t get_present_hsl(const mesh_model_info_p pmodel_info)
{
    light_hsl_server_get_t get_data = {0, 0, 0};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, LIGHT_HSL_SERVER_GET, &get_data);
    }

    return get_data;
}

static int32_t light_hsl_trans_step_change(const mesh_model_info_p pmodel_info,
                                           uint32_t type,
                                           generic_transition_time_t total_time,
                                           generic_transition_time_t remaining_time)
{
    UNUSED(type);
    int32_t ret = 0;
    light_hsl_server_set_t trans_set_data;
    light_hsl_info_t *phsl_info = pmodel_info->pargs;
    if (NULL == phsl_info)
    {
        return 0;
    }
    trans_set_data.lightness = phsl_info->target_lightness;
    trans_set_data.hue = phsl_info->target_hue;
    trans_set_data.saturation = phsl_info->target_saturation;
    trans_set_data.total_time = total_time;
    trans_set_data.remaining_time = remaining_time;

    if (NULL != pmodel_info->model_data_cb)
    {
        ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_HSL_SERVER_SET, &trans_set_data);
    }

    if (0 == remaining_time.num_steps)
    {
        light_hsl_server_get_t get_data = {0, 0, 0};
        get_data = get_present_hsl(pmodel_info);
#if MODEL_ENABLE_DELAY_MSG_RSP
        light_hsl_delay_publish(pmodel_info, get_data.lightness, get_data.hue, get_data.saturation,
                                phsl_info->delay_pub_time);
#else
        light_hsl_publish(pmodel_info, get_data.lightness, get_data.hue, get_data.saturation);
#endif
    }

    return ret;
}

static light_hsl_server_get_t light_hsl_process(const mesh_model_info_p pmodel_info,
                                                uint16_t target_lightness, uint16_t target_hue,
                                                uint16_t target_saturation,
                                                generic_transition_time_t trans_time,
                                                light_hsl_process_result_t *presult)
{
    light_hsl_server_get_t hsl_before_set = {0};
    light_hsl_server_get_t hsl_after_set = {0};

    /* get hsl before set */
    hsl_before_set = get_present_hsl(pmodel_info);
    hsl_after_set = hsl_before_set;

    int32_t ret = MODEL_SUCCESS;
    light_hsl_server_set_t trans_set_data;
    trans_set_data.lightness = target_lightness;
    trans_set_data.hue = target_hue;
    trans_set_data.saturation = target_saturation;
    trans_set_data.total_time = trans_time;
    trans_set_data.remaining_time = trans_time;

    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, LIGHT_HSL_SERVER_SET, &trans_set_data);
    }

    if (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE != trans_time.num_steps)
    {
        if ((ret >= 0) && (MODEL_STOP_TRANSITION != ret))
        {
            if (NULL != presult)
            {
                presult->use_transition = TRUE;
            }
            generic_transition_timer_start(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_HSL, trans_time,
                                           light_hsl_trans_step_change);
        }
#if MODEL_ENABLE_USER_STOP_TRANSITION_NOTIFICATION
        else if (MODEL_STOP_TRANSITION == ret)
        {
            if (NULL != pmodel_info->model_data_cb)
            {
                ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_HSL_SERVER_SET, &trans_set_data);
            }
        }
#endif
    }
    else
    {
        /* get hsl after set */
        hsl_after_set = get_present_hsl(pmodel_info);
    }

    if ((hsl_after_set.lightness != hsl_before_set.lightness) ||
        (hsl_after_set.hue != hsl_before_set.hue) ||
        (hsl_after_set.saturation != hsl_before_set.saturation))
    {
        if (NULL != presult)
        {
            presult->state_changed = TRUE;
        }
    }

    return hsl_after_set;
}

static int32_t light_hsl_delay_execution(mesh_model_info_t *pmodel_info, uint32_t delay_type)
{
    switch (delay_type)
    {
    case DELAY_EXECUTION_TYPE_LIGHT_HSL:
        {
            light_hsl_info_t *phsl_info = pmodel_info->pargs;
            if (NULL == phsl_info)
            {
                return 0;
            }
            phsl_info->delay_time = 0;
            light_hsl_process_result_t result =
            {
                .state_changed = FALSE,
                .use_transition = FALSE
            };
            light_hsl_server_get_t present_hsl = light_hsl_process(pmodel_info, phsl_info->target_lightness,
                                                                   phsl_info->target_hue,
                                                                   phsl_info->target_saturation, phsl_info->trans_time, &result);
#if MODEL_ENABLE_DELAY_MSG_RSP
            if (!result.use_transition)
            {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                if (result.state_changed)
#endif
                {
                    light_hsl_delay_publish(pmodel_info, present_hsl.lightness, present_hsl.hue, present_hsl.saturation,
                                            phsl_info->delay_pub_time);
                }
            }
#else
            light_hsl_state_change_publish(pmodel_info, present_hsl.lightness, present_hsl.hue,
                                           present_hsl.saturation, result);
#endif
        }
        break;
    default:
        break;
    }

    return 0;
}

static bool light_hsl_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_LIGHT_HSL_GET:
        if (pmesh_msg->msg_len == sizeof(light_hsl_get_t))
        {
            light_hsl_info_t *phsl_info = pmodel_info->pargs;
            generic_transition_time_t remaining_time;
            if (phsl_info->delay_time > 0)
            {
                remaining_time = phsl_info->trans_time;
            }
            else
            {
                remaining_time = generic_transition_time_get(pmodel_info,
                                                             GENERIC_TRANSITION_TYPE_LIGHT_HSL);
            }

            light_hsl_server_get_t get_data = get_present_hsl(pmodel_info);

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_hsl_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                           get_data.lightness, get_data.hue,
                           get_data.saturation,
                           (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == remaining_time.num_steps) ? FALSE : TRUE,
                           remaining_time, delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_HSL_SET:
    case MESH_MSG_LIGHT_HSL_SET_UNACK:
        {
            light_hsl_set_t *pmsg = (light_hsl_set_t *)pbuffer;
            generic_transition_time_t trans_time = {GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE, 0};
            uint32_t delay_time = 0;
            if (pmesh_msg->msg_len == MEMBER_OFFSET(light_hsl_set_t, trans_time))
            {
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_HSL_SERVER_GET_DEFAULT_TRANSITION_TIME, &trans_time);
                }
            }
            else if (pmesh_msg->msg_len == sizeof(light_hsl_set_t))
            {
                trans_time = pmsg->trans_time;
                delay_time = pmsg->delay * DELAY_EXECUTION_STEP_RESOLUTION;
            }
            if (IS_GENERIC_TRANSITION_STEPS_VALID(trans_time.num_steps))
            {
                light_hsl_server_get_range_t range = {0, 0, 0, 0};
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_HSL_SERVER_GET_RANGE, &range);
                }

                light_hsl_info_t *phsl_info = pmodel_info->pargs;
                phsl_info->target_lightness = pmsg->lightness;
                if ((0 != range.hue_range_min) && (0 != range.hue_range_max))
                {
                    phsl_info->target_hue = CLAMP(pmsg->hue, range.hue_range_min, range.hue_range_max);
                }
                else
                {
                    phsl_info->target_hue = pmsg->hue;
                }

                if ((0 != range.saturation_range_min) && (0 != range.saturation_range_max))
                {
                    phsl_info->target_saturation = CLAMP(pmsg->saturation, range.saturation_range_min,
                                                         range.saturation_range_max);
                }
                else
                {
                    phsl_info->target_saturation = pmsg->saturation;
                }
                phsl_info->tid = pmsg->tid;
                phsl_info->trans_time = trans_time;
                phsl_info->delay_time = delay_time;

                light_hsl_server_get_t present_hsl;
                light_hsl_process_result_t result =
                {
                    .state_changed = FALSE,
                    .use_transition = FALSE
                };
                if (delay_time > 0)
                {
                    result.use_transition = TRUE;
                    present_hsl = get_present_hsl(pmodel_info);
                    delay_execution_timer_start(pmodel_info, DELAY_EXECUTION_TYPE_LIGHT_HSL, delay_time,
                                                light_hsl_delay_execution);
                }
                else
                {
                    present_hsl = light_hsl_process(pmodel_info, phsl_info->target_lightness,
                                                    phsl_info->target_hue,
                                                    phsl_info->target_saturation, trans_time,
                                                    &result);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (pmesh_msg->access_opcode == MESH_MSG_LIGHT_HSL_SET)
                {
                    light_hsl_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                   present_hsl.lightness, present_hsl.hue,
                                   present_hsl.saturation,
                                   (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == trans_time.num_steps) ? FALSE : TRUE,
                                   trans_time, delay_rsp_time);
                }
#if MODEL_ENABLE_DELAY_MSG_RSP
                bool ack = (pmesh_msg->access_opcode == MESH_MSG_LIGHT_HSL_SET_UNACK) ? FALSE : TRUE;
                if (!result.use_transition)
                {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                    if (result.state_changed)
#endif
                    {
                        uint32_t delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time, TRUE,
                                                                            ack);
                        light_hsl_delay_publish(pmodel_info, present_hsl.lightness, present_hsl.hue, present_hsl.saturation,
                                                delay_pub_time);
                    }
                }
                else
                {
                    phsl_info->delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time, FALSE,
                                                                          ack);
                }
#else
                light_hsl_state_change_publish(pmodel_info, present_hsl.lightness, present_hsl.hue,
                                               present_hsl.saturation, result);
#endif
            }
        }
        break;
    case MESH_MSG_LIGHT_HSL_TARGET_GET:
        if (pmesh_msg->msg_len == sizeof(light_hsl_target_get_t))
        {
            light_hsl_info_t *phsl_info = pmesh_msg->pmodel_info->pargs;
            generic_transition_time_t remaining_time;
            if (phsl_info->delay_time > 0)
            {
                remaining_time = phsl_info->trans_time;
            }
            else
            {
                remaining_time = generic_transition_time_get(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_HSL);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_hsl_target_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                  phsl_info->target_lightness, phsl_info->target_hue,
                                  phsl_info->target_saturation,
                                  (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == remaining_time.num_steps) ? FALSE : TRUE,
                                  remaining_time, delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_HSL_DEFAULT_GET:
        if (pmesh_msg->msg_len == sizeof(light_hsl_default_get_t))
        {
            light_hsl_server_get_default_t get_default = {0, 0, 0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, LIGHT_HSL_SERVER_GET_DEFAULT,
                                           &get_default);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_hsl_default_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                   get_default.lightness, get_default.hue, get_default.saturation, delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_HSL_RANGE_GET:
        if (pmesh_msg->msg_len == sizeof(light_hsl_range_get_t))
        {
            light_hsl_server_get_range_t get_range = {0, 0, 0, 0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, LIGHT_HSL_SERVER_GET_RANGE,
                                           &get_range);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_hsl_range_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                 GENERIC_STAT_SUCCESS,
                                 get_range.hue_range_min, get_range.hue_range_max,
                                 get_range.saturation_range_min, get_range.saturation_range_max,
                                 delay_rsp_time);
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

static int32_t light_hsl_server_publish(mesh_model_info_p pmodel_info, bool retrans)
{
    UNUSED(retrans);
    light_hsl_server_get_t get_data = get_present_hsl(pmodel_info);
    generic_transition_time_t trans_time = {0, 0};
    light_hsl_stat(pmodel_info, 0, 0, get_data.lightness, get_data.hue, get_data.saturation, FALSE,
                   trans_time, 0);

    return 0;
}

#if MESH_MODEL_ENABLE_DEINIT
static void light_hsl_server_deinit(mesh_model_info_t *pmodel_info)
{
    if (pmodel_info->model_receive == light_hsl_server_receive)
    {
        /* stop delay execution */
        delay_execution_timer_stop(pmodel_info, DELAY_EXECUTION_TYPE_LIGHT_HSL);
        /* stop transition */
        generic_transition_timer_stop(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_HSL);

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

bool light_hsl_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_LIGHT_HSL_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        light_hsl_info_t *phsl_info = plt_malloc(sizeof(light_hsl_info_t),
                                                 RAM_TYPE_DATA_ON);
        if (NULL == phsl_info)
        {
            printe("light_hsl_server_reg: fail to allocate memory for the new model extension data!");
            return FALSE;
        }
        memset(phsl_info, 0, sizeof(light_hsl_info_t));
        pmodel_info->pargs = phsl_info;

        pmodel_info->model_receive = light_hsl_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("light_hsl_server_reg: missing model data process callback!");
        }
#if MESH_MODEL_ENABLE_DEINIT
        pmodel_info->model_deinit = light_hsl_server_deinit;
#endif
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = light_hsl_server_publish;
    }

    generic_transition_time_init();
    delay_execution_init();
    return mesh_model_reg(element_index, pmodel_info);
}

