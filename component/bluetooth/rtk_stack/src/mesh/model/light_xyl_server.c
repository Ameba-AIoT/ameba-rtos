/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     light_xyl_server.c
* @brief    Source file for light xyl server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2019-07-09
* @version  v1.0
* *************************************************************************************
*/

#include <math.h>
#include "light_xyl.h"
#include "delay_execution.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif


typedef struct
{
    uint8_t tid;
    light_xyl_t target_xyl;
    generic_transition_time_t trans_time;
    uint32_t delay_time;
#if MODEL_ENABLE_DELAY_MSG_RSP
    uint32_t delay_pub_time;
#endif
} light_xyl_info_t;

typedef struct
{
    bool state_changed;
    bool use_transition;
} light_xyl_process_result_t;

double light_xyl_x_to_cie1931_x(uint16_t xyl_x)
{
    return xyl_x / 65535.0;
}

uint16_t light_cie1931_x_to_xyl_x(double cie1931_x)
{
    return cie1931_x * 65535;
}

double light_xyl_y_to_cie1931_y(uint16_t xyl_y)
{
    return xyl_y / 65535.0;
}

uint16_t light_cie1931_y_to_xyl_y(double cie1931_y)
{
    return cie1931_y * 65535;
}

uint16_t light_intensity_to_xyl_lightness(uint16_t intensity)
{
    return (uint16_t)(65535 * sqrt(intensity / 65535.0));
}

uint16_t light_xyl_lightness_to_internsity(uint16_t lightness)
{
    return (uint16_t)(lightness / 65535.0 * lightness);
}

static mesh_msg_send_cause_t light_xyl_server_send(mesh_model_info_p pmodel_info,
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

static mesh_msg_send_cause_t light_xyl_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                              uint16_t app_key_index, light_xyl_t xyl,
                                              bool optional, generic_transition_time_t remaining_time,
                                              uint32_t delay_time)
{
    light_xyl_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_STATUS);
    uint16_t msg_len;
    if (optional)
    {
        msg_len = sizeof(light_xyl_status_t);
        msg.remaining_time = remaining_time;
    }
    else
    {
        msg_len = MEMBER_OFFSET(light_xyl_status_t, remaining_time);
    }
    msg.xyl = xyl;
    return light_xyl_server_send(pmodel_info, dst, (uint8_t *)&msg, msg_len, app_key_index,
                                 delay_time);
}

static mesh_msg_send_cause_t light_xyl_target_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                                     uint16_t app_key_index, light_xyl_t xyl,
                                                     bool optional, generic_transition_time_t remaining_time,
                                                     uint32_t delay_time)
{
    light_xyl_target_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_TARGET_STATUS);
    uint16_t msg_len;
    if (optional)
    {
        msg_len = sizeof(light_xyl_target_status_t);
        msg.remaining_time = remaining_time;
    }
    else
    {
        msg_len = MEMBER_OFFSET(light_xyl_target_status_t, remaining_time);
    }
    msg.xyl = xyl;
    return light_xyl_server_send(pmodel_info, dst, (uint8_t *)&msg, msg_len, app_key_index,
                                 delay_time);
}

static mesh_msg_send_cause_t light_xyl_delay_publish(const mesh_model_info_p pmodel_info,
                                                     light_xyl_t xyl,
                                                     uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        generic_transition_time_t remaining_time = {};
        ret = light_xyl_status(pmodel_info, 0, 0, xyl, FALSE, remaining_time, delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t light_xyl_publish(const mesh_model_info_p pmodel_info, light_xyl_t xyl)
{
    return light_xyl_delay_publish(pmodel_info, xyl, 0);
}

#if !MODEL_ENABLE_DELAY_MSG_RSP
static void light_xyl_state_change_publish(const mesh_model_info_p pmodel_info, light_xyl_t xyl,
                                           light_xyl_process_result_t result)
{
    if (result.use_transition)
    {
        return ;
    }

#if !MODEL_ENABLE_PUBLISH_ALL_TIME
    if (result.state_changed)
#endif
    {
        light_xyl_delay_publish(pmodel_info, xyl, 0);
    }
}
#endif

mesh_msg_send_cause_t light_xyl_default_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                               uint16_t app_key_index, light_xyl_t xyl, uint32_t delay_time)
{
    light_xyl_default_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_DEFAULT_STATUS);
    msg.xyl = xyl;
    return light_xyl_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg), app_key_index,
                                 delay_time);
}

mesh_msg_send_cause_t light_xyl_range_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                             uint16_t app_key_index, generic_stat_t status, uint16_t xyl_x_range_min, uint16_t xyl_x_range_max,
                                             uint16_t xyl_y_range_min, uint16_t xyl_y_range_max, uint32_t delay_time)
{
    light_xyl_range_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_XYL_RANGE_STATUS);
    msg.status_code = status;
    msg.xyl_x_range_min = xyl_x_range_min;
    msg.xyl_x_range_max = xyl_x_range_max;
    msg.xyl_y_range_min = xyl_y_range_min;
    msg.xyl_y_range_max = xyl_y_range_max;
    return light_xyl_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg), app_key_index,
                                 delay_time);
}

static light_xyl_t get_present_xyl(const mesh_model_info_p pmodel_info)
{
    light_xyl_server_get_t get_data = {0, 0, 0};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, LIGHT_XYL_SERVER_GET, &get_data);
    }

    return get_data;
}

static int32_t light_xyl_trans_step_change(const mesh_model_info_p pmodel_info,
                                           uint32_t type,
                                           generic_transition_time_t total_time,
                                           generic_transition_time_t remaining_time)
{
    UNUSED(type);
    int32_t ret = MODEL_SUCCESS;
    light_xyl_server_set_t set_data;
    light_xyl_info_t *pxyl_info = pmodel_info->pargs;
    if (NULL == pxyl_info)
    {
        return 0;
    }

    set_data.xyl = pxyl_info->target_xyl;
    set_data.total_time = total_time;
    set_data.remaining_time = remaining_time;

    if (NULL != pmodel_info->model_data_cb)
    {
        ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_XYL_SERVER_SET, &set_data);
    }

    if (0 == remaining_time.num_steps)
    {
        uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
        delay_rsp_time = pxyl_info->delay_pub_time;
#endif
        light_xyl_t present_xyl = get_present_xyl(pmodel_info);
        light_xyl_delay_publish(pmodel_info, present_xyl, delay_rsp_time);
    }

    return ret;
}

static light_xyl_t light_xyl_process(const mesh_model_info_p pmodel_info,
                                     light_xyl_t target_xyl,
                                     generic_transition_time_t trans_time,
                                     light_xyl_process_result_t *presult)
{
    light_xyl_t xyl_before_set = {0, 0, 0};
    light_xyl_t xyl_after_set = {0, 0, 0};

    /* get xyl before set */
    xyl_before_set = get_present_xyl(pmodel_info);
    xyl_after_set = xyl_before_set;

    int32_t ret = MODEL_SUCCESS;
    light_xyl_server_set_t trans_set_data;
    trans_set_data.xyl = target_xyl;
    trans_set_data.total_time = trans_time;
    trans_set_data.remaining_time = trans_time;

    if (NULL != pmodel_info->model_data_cb)
    {
        ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_XYL_SERVER_SET, &trans_set_data);
    }

    if (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE != trans_time.num_steps)
    {
        if ((ret >= 0) && (MODEL_STOP_TRANSITION != ret))
        {
            if (NULL != presult)
            {
                presult->use_transition = TRUE;
            }
            generic_transition_timer_start(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_XYL, trans_time,
                                           light_xyl_trans_step_change);
        }
#if MODEL_ENABLE_USER_STOP_TRANSITION_NOTIFICATION
        else if (MODEL_STOP_TRANSITION == ret)
        {
            if (NULL != pmodel_info->model_data_cb)
            {
                ret = pmodel_info->model_data_cb(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_XYL, &trans_set_data);
            }
        }
#endif
    }
    else
    {
        /* get xyl after set */
        xyl_after_set = get_present_xyl(pmodel_info);
    }

    if ((xyl_before_set.xyl_lightness != xyl_after_set.xyl_lightness) &&
        (xyl_before_set.xyl_x != xyl_after_set.xyl_x) &&
        (xyl_before_set.xyl_y != xyl_after_set.xyl_y))
    {
        if (NULL != presult)
        {
            presult->state_changed = TRUE;
        }
    }

    return xyl_after_set;
}

static int32_t light_xyl_delay_execution(mesh_model_info_t *pmodel_info, uint32_t delay_type)
{
    switch (delay_type)
    {
    case DELAY_EXECUTION_TYPE_LIGHT_XYL:
        {
            light_xyl_info_t *pxyl_info = pmodel_info->pargs;
            if (NULL == pxyl_info)
            {
                return 0;
            }

            light_xyl_process_result_t result =
            {
                .state_changed = FALSE,
                .use_transition = FALSE
            };
            generic_transition_time_t trans_time;
            light_xyl_t target_xyl;
            pxyl_info->delay_time = 0;
            target_xyl = pxyl_info->target_xyl;
            trans_time = pxyl_info->trans_time;

            light_xyl_t present_xyl = light_xyl_process(pmodel_info, target_xyl, trans_time, &result);
#if MODEL_ENABLE_DELAY_MSG_RSP
            if (!result.use_transition)
            {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                if (result.state_changed)
#endif
                {
                    uint32_t delay_rsp_time = pxyl_info->delay_pub_time;
                    light_xyl_delay_publish(pmodel_info, present_xyl, delay_rsp_time);
                }
            }
#else
            light_xyl_state_change_publish(pmodel_info, present_xyl, result);
#endif
        }
        break;
    default:
        break;
    }

    return 0;
}

static bool light_xyl_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_LIGHT_XYL_GET:
        if (pmesh_msg->msg_len == sizeof(light_xyl_get_t))
        {
            light_xyl_info_t *pxyl_info = pmodel_info->pargs;
            generic_transition_time_t remaining_time;
            if (pxyl_info->delay_time > 0)
            {
                remaining_time = pxyl_info->trans_time;
            }
            else
            {
                remaining_time = generic_transition_time_get(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_XYL);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_xyl_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                             get_present_xyl(pmodel_info),
                             (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == remaining_time.num_steps) ? FALSE : TRUE,
                             remaining_time, delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_XYL_SET:
    case MESH_MSG_LIGHT_XYL_SET_UNACK:
        {
            light_xyl_set_t *pmsg = (light_xyl_set_t *)pbuffer;
            generic_transition_time_t trans_time = {GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE, 0};
            uint32_t delay_time = 0;
            if (pmesh_msg->msg_len == MEMBER_OFFSET(light_xyl_set_t, trans_time))
            {
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_XYL_SERVER_GET_DEFAULT_TRANSITION_TIME,
                                               &trans_time);
                }
            }
            else if (pmesh_msg->msg_len == sizeof(light_xyl_set_t))
            {
                trans_time = pmsg->trans_time;
                delay_time = pmsg->delay * DELAY_EXECUTION_STEP_RESOLUTION;
            }
            if (IS_GENERIC_TRANSITION_STEPS_VALID(trans_time.num_steps))
            {
                light_xyl_info_t *pxyl_info = pmodel_info->pargs;
                light_xyl_server_get_range_t range = {0, 0, 0, 0};
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_XYL_SERVER_GET_RANGE, &range);
                }

                pxyl_info->target_xyl.xyl_lightness = pmsg->xyl.xyl_lightness;
                if ((0 != range.xyl_x_range_min) && (0 != range.xyl_x_range_max))
                {
                    /* need to clamp xyl_x */
                    pxyl_info->target_xyl.xyl_x = CLAMP(pmsg->xyl.xyl_x, range.xyl_x_range_min, range.xyl_x_range_max);
                }
                else
                {
                    pxyl_info->target_xyl.xyl_x = pmsg->xyl.xyl_x;
                }
                if ((0 != range.xyl_y_range_min) && (0 != range.xyl_y_range_max))
                {
                    /* need to clamp xyl_y */
                    pxyl_info->target_xyl.xyl_y = CLAMP(pmsg->xyl.xyl_y, range.xyl_y_range_min, range.xyl_y_range_max);
                }
                else
                {
                    pxyl_info->target_xyl.xyl_y = pmsg->xyl.xyl_y;
                }
                pxyl_info->tid = pmsg->tid;
                pxyl_info->trans_time = trans_time;
                pxyl_info->delay_time = delay_time;

                light_xyl_t present_xyl;
                light_xyl_process_result_t result =
                {
                    .state_changed = FALSE,
                    .use_transition = FALSE
                };
                if (delay_time > 0)
                {
                    result.use_transition = TRUE;
                    present_xyl = get_present_xyl(pmodel_info);
                    delay_execution_timer_start(pmodel_info, DELAY_EXECUTION_TYPE_LIGHT_XYL, delay_time,
                                                light_xyl_delay_execution);
                }
                else
                {
                    present_xyl = light_xyl_process(pmodel_info, pxyl_info->target_xyl,
                                                    trans_time, &result);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (pmesh_msg->access_opcode == MESH_MSG_LIGHT_XYL_SET)
                {
                    light_xyl_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                     present_xyl,
                                     (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == trans_time.num_steps) ? FALSE : TRUE,
                                     trans_time, delay_rsp_time);
                }
#if MODEL_ENABLE_DELAY_MSG_RSP
                bool ack = (pmesh_msg->access_opcode == MESH_MSG_LIGHT_XYL_SET_UNACK) ? FALSE : TRUE;
                if (!result.use_transition)
                {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                    if (result.state_changed)
#endif
                    {
                        uint32_t delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time, TRUE,
                                                                            ack);
                        light_xyl_delay_publish(pmodel_info, present_xyl, delay_pub_time);
                    }
                }
                else
                {
                    pxyl_info->delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time,
                                                                          FALSE, ack);
                }
#else
                light_xyl_state_change_publish(pmodel_info, present_xyl, result);
#endif
            }
        }
        break;
    case MESH_MSG_LIGHT_XYL_TARGET_GET:
        if (pmesh_msg->msg_len == sizeof(light_xyl_target_get_t))
        {
            light_xyl_info_t *pxyl_info = pmodel_info->pargs;
            generic_transition_time_t remaining_time;
            if (pxyl_info->delay_time > 0)
            {
                remaining_time = pxyl_info->trans_time;
            }
            else
            {
                remaining_time = generic_transition_time_get(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_XYL);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_xyl_target_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                    pxyl_info->target_xyl,
                                    (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == remaining_time.num_steps) ? FALSE : TRUE,
                                    remaining_time, delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_XYL_DEFAULT_GET:
        if (pmesh_msg->msg_len == sizeof(light_xyl_default_get_t))
        {
            light_xyl_server_get_default_t default_data = {0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, LIGHT_XYL_SERVER_GET_DEFAULT,
                                           &default_data);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_xyl_default_status(pmodel_info, pmesh_msg->src,
                                     pmesh_msg->app_key_index, default_data, delay_rsp_time);

        }
        break;
    case MESH_MSG_LIGHT_XYL_RANGE_GET:
        if (pmesh_msg->msg_len == sizeof(light_xyl_range_get_t))
        {
            light_xyl_server_get_range_t range_data = {0, 0, 0, 0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, LIGHT_XYL_SERVER_GET_RANGE,
                                           &range_data);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_xyl_range_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                   GENERIC_STAT_SUCCESS, range_data.xyl_x_range_min, range_data.xyl_x_range_max,
                                   range_data.xyl_y_range_min, range_data.xyl_y_range_max,
                                   delay_rsp_time);
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

static int32_t light_xyl_server_publish(mesh_model_info_p pmodel_info, bool retrans)
{
    UNUSED(retrans);
    generic_transition_time_t remaining_time = {};
    light_xyl_status(pmodel_info, 0, 0, get_present_xyl(pmodel_info), FALSE,
                     remaining_time, 0);
    return 0;
}

#if MESH_MODEL_ENABLE_DEINIT
static void light_xyl_server_deinit(mesh_model_info_t *pmodel_info)
{
    if (pmodel_info->model_receive == light_xyl_server_receive)
    {
        /* stop delay execution */
        delay_execution_timer_stop(pmodel_info, DELAY_EXECUTION_TYPE_LIGHT_XYL);
        /* stop step transition */
        generic_transition_timer_stop(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_XYL);

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

bool light_xyl_server_reg(uint8_t element_index, mesh_model_info_t *pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_LIGHT_XYL_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        light_xyl_info_t *pxyl_info = plt_malloc(sizeof(light_xyl_info_t), RAM_TYPE_DATA_ON);
        if (NULL == pxyl_info)
        {
            printe("light_xyl_server_reg: fail to allocate memory for the new model extension data!");
            return FALSE;
        }
        memset(pxyl_info, 0, sizeof(light_xyl_info_t));
        pmodel_info->pargs = pxyl_info;

        pmodel_info->model_receive = light_xyl_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("light_xyl_server_reg: missing model data process callback!");
        }
#if MESH_MODEL_ENABLE_DEINIT
        pmodel_info->model_deinit = light_xyl_server_deinit;
#endif
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = light_xyl_server_publish;
    }

    generic_transition_time_init();
    delay_execution_init();
    return mesh_model_reg(element_index, pmodel_info);
}

