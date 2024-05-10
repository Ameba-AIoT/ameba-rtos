/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_power_level_server.c
* @brief    Source file for generic power level server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-7-27
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "generic_power_level.h"
#include "delay_execution.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif

typedef struct
{
    uint8_t tid;
    uint16_t target_power;
    generic_transition_time_t trans_time;
    uint32_t delay_time;
#if MODEL_ENABLE_DELAY_MSG_RSP
    uint32_t delay_pub_time;
#endif
} generic_power_level_info_t;

typedef struct
{
    bool state_changed;
    bool use_transition;
} generic_power_level_process_result_t;

int16_t generic_power_level_to_generic_level(uint16_t power)
{
    return power - 32768;
}

uint16_t generic_level_to_power_level(int16_t level)
{
    return level + 32768;
}

static mesh_msg_send_cause_t generic_power_level_server_send(mesh_model_info_p pmodel_info,
                                                             uint16_t dst, uint8_t *pmsg, uint16_t msg_len,
                                                             uint16_t app_key_index, uint32_t delay_time)
{
    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    if (dst != 0)
    {
        mesh_msg.dst = dst;
        mesh_msg.app_key_index = app_key_index;
    }
    mesh_msg.delay_time = delay_time;
    return access_send(&mesh_msg);
}

static mesh_msg_send_cause_t generic_power_level_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                      uint16_t app_key_index,
                                                      uint16_t present_power, bool optional, uint16_t target_power,
                                                      generic_transition_time_t remaining_time, uint32_t delay_time)
{
    generic_power_level_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_LEVEL_STAT);
    uint16_t msg_len;
    if (optional)
    {
        msg_len = sizeof(generic_power_level_stat_t);
        msg.target_power = target_power;
        msg.remaining_time = remaining_time;
    }
    else
    {
        msg_len = MEMBER_OFFSET(generic_power_level_stat_t, target_power);
    }
    msg.present_power = present_power;
    return generic_power_level_server_send(pmodel_info, dst, (uint8_t *)&msg, msg_len, app_key_index,
                                           delay_time);
}

mesh_msg_send_cause_t generic_power_level_delay_publish(const mesh_model_info_p pmodel_info,
                                                        uint16_t power, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        generic_transition_time_t remaining_time = {0, 0};
        ret = generic_power_level_stat(pmodel_info, 0, 0, power, FALSE, power, remaining_time, delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t generic_power_level_publish(const mesh_model_info_p pmodel_info,
                                                  uint16_t power)
{
    return generic_power_level_delay_publish(pmodel_info, power, 0);
}

#if !MODEL_ENABLE_DELAY_MSG_RSP
static void generic_power_level_state_change_publish(const mesh_model_info_p pmodel_info,
                                                     uint16_t power, generic_power_level_process_result_t result)
{
    if (result.use_transition)
    {
        return ;
    }

#if !MODEL_ENABLE_PUBLISH_ALL_TIME
    if (result.state_changed)
#endif
    {
        generic_power_level_publish(pmodel_info, power);
    }
}
#endif

static mesh_msg_send_cause_t generic_power_last_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                     uint16_t app_key_index, uint16_t power, uint32_t delay_time)
{
    generic_power_last_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_LAST_STAT);
    msg.power = power;
    return generic_power_level_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg),
                                           app_key_index, delay_time);
}

mesh_msg_send_cause_t generic_power_default_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                 uint16_t app_key_index, uint16_t power, uint32_t delay_time)
{
    generic_power_default_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_DEFAULT_STAT);
    msg.power = power;
    return generic_power_level_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg),
                                           app_key_index, delay_time);
}

mesh_msg_send_cause_t generic_power_range_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                               uint16_t app_key_index, generic_stat_t stat, uint16_t range_min, uint16_t range_max,
                                               uint32_t delay_time)
{
    generic_power_range_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_POWER_RANGE_STAT);
    msg.stat = stat;
    msg.range_min = range_min;
    msg.range_max = range_max;
    return generic_power_level_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg),
                                           app_key_index, delay_time);
}

static uint16_t get_present_power_level(const mesh_model_info_p pmodel_info)
{
    generic_power_level_server_get_t get_data = {0};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, GENERIC_POWER_LEVEL_SERVER_GET, &get_data);
    }

    return get_data.power;
}

static int32_t generic_power_level_trans_step_change(const mesh_model_info_p pmodel_info,
                                                     uint32_t type,
                                                     generic_transition_time_t total_time,
                                                     generic_transition_time_t remaining_time)
{
	(void) type; //avoid warning
    int32_t ret = MODEL_SUCCESS;
    generic_power_level_server_set_t set_data;
    generic_power_level_info_t *ppower_level_info = pmodel_info->pargs;
    if (NULL == ppower_level_info)
    {
        return 0;
    }
    set_data.power = ppower_level_info->target_power;
    set_data.total_time = total_time;
    set_data.remaining_time = remaining_time;

    if (NULL != pmodel_info->model_data_cb)
    {
        ret = pmodel_info->model_data_cb(pmodel_info, GENERIC_POWER_LEVEL_SERVER_SET, &set_data);
    }

    if (0 == remaining_time.num_steps)
    {
        uint16_t present_level = get_present_power_level(pmodel_info);
#if MODEL_ENABLE_DELAY_MSG_RSP
        generic_power_level_delay_publish(pmodel_info, present_level, ppower_level_info->delay_pub_time);
#else
        generic_power_level_publish(pmodel_info, present_level);
#endif
    }

    return ret;
}

static uint16_t generic_power_level_process(const mesh_model_info_p pmodel_info,
                                            uint16_t target_power,
                                            generic_transition_time_t trans_time,
                                            generic_power_level_process_result_t *presult)
{
    uint16_t power_before_set = 0;
    uint16_t power_after_set = 0;

    /* get generic power level before set */
    power_before_set = get_present_power_level(pmodel_info);
    power_after_set = power_before_set;

    int32_t ret = MODEL_SUCCESS;
    generic_power_level_server_set_t trans_set_data;
    trans_set_data.power = target_power;
    trans_set_data.total_time = trans_time;
    trans_set_data.remaining_time = trans_time;

    if (NULL != pmodel_info->model_data_cb)
    {
        ret = pmodel_info->model_data_cb(pmodel_info, GENERIC_POWER_LEVEL_SERVER_SET, &trans_set_data);
    }

    if (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE != trans_time.num_steps)
    {
        if ((ret >= 0) && (MODEL_STOP_TRANSITION != ret))
        {
            if (NULL != presult)
            {
                presult->use_transition = TRUE;
            }
            generic_transition_timer_start(pmodel_info, GENERIC_TRANSITION_TYPE_POWER_LEVEL, trans_time,
                                           generic_power_level_trans_step_change);
        }
#if MODEL_ENABLE_USER_STOP_TRANSITION_NOTIFICATION
        else if (MODEL_STOP_TRANSITION == ret)
        {
            if (NULL != pmodel_info->model_data_cb)
            {
                ret = pmodel_info->model_data_cb(pmodel_info, GENERIC_POWER_LEVEL_SERVER_SET, &trans_set_data);
            }
        }
#endif
    }
    else
    {
        /* get level after set */
        power_after_set = get_present_power_level(pmodel_info);
    }

    if (power_before_set != power_after_set)
    {
        if (NULL != presult)
        {
            presult->state_changed = TRUE;
        }
    }

    if (0 != target_power)
    {
        if (NULL != pmodel_info->model_data_cb)
        {
            generic_power_level_server_set_last_t set_last = {target_power};
            pmodel_info->model_data_cb(pmodel_info, GENERIC_POWER_LEVEL_SERVER_SET_LAST, &set_last);
        }
    }

    return power_after_set;
}

static int32_t generic_power_level_delay_execution(mesh_model_info_t *pmodel_info,
                                                   uint32_t delay_type)
{
    switch (delay_type)
    {
    case DELAY_EXECUTION_TYPE_POWER_LEVEL:
        {
            generic_power_level_info_t *ppower_level_info = pmodel_info->pargs;
            if (NULL == ppower_level_info)
            {
                return 0;
            }
            ppower_level_info->delay_time = 0;
            generic_power_level_process_result_t result =
            {
                .state_changed = FALSE,
                .use_transition = FALSE
            };
            uint16_t present_power = generic_power_level_process(pmodel_info, ppower_level_info->target_power,
                                                                 ppower_level_info->trans_time, &result);
#if MODEL_ENABLE_DELAY_MSG_RSP
            if (!result.use_transition)
            {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                if (result.state_changed)
#endif
                {
                    generic_power_level_delay_publish(pmodel_info, present_power, ppower_level_info->delay_pub_time);
                }
            }
#else
            generic_power_level_state_change_publish(pmodel_info, present_power, result);
#endif
        }
        break;
    default:
        break;
    }

    return 0;
}

static bool generic_power_level_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_POWER_LEVEL_GET:
        if (pmesh_msg->msg_len == sizeof(generic_power_level_get_t))
        {
            generic_power_level_info_t *ppower_level_info = pmodel_info->pargs;
            generic_transition_time_t remaining_time;
            if (ppower_level_info->delay_time > 0)
            {
                remaining_time = ppower_level_info->trans_time;
            }
            else
            {
                remaining_time = generic_transition_time_get(pmesh_msg->pmodel_info,
                                                             GENERIC_TRANSITION_TYPE_POWER_LEVEL);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            generic_power_level_stat(pmesh_msg->pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                     get_present_power_level(pmodel_info),
                                     (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == remaining_time.num_steps) ? FALSE : TRUE,
                                     ppower_level_info->target_power, remaining_time, delay_rsp_time);
        }
        break;
    case MESH_MSG_GENERIC_POWER_LEVEL_SET:
    case MESH_MSG_GENERIC_POWER_LEVEL_SET_UNACK:
        {
            generic_power_level_set_t *pmsg = (generic_power_level_set_t *)pbuffer;
            generic_transition_time_t trans_time = {GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE, 0};
            uint32_t delay_time = 0;
            if (pmesh_msg->msg_len == MEMBER_OFFSET(generic_power_level_set_t, trans_time))
            {
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, GENERIC_POWER_LEVEL_SERVER_GET_DEFAULT_TRANSITION_TIME,
                                               &trans_time);
                }
            }
            else if (pmesh_msg->msg_len == sizeof(generic_power_level_set_t))
            {
                trans_time = pmsg->trans_time;
                delay_time = pmsg->delay * DELAY_EXECUTION_STEP_RESOLUTION;
            }
            if (IS_GENERIC_TRANSITION_STEPS_VALID(trans_time.num_steps))
            {
                generic_power_level_server_get_range_t range = {0, 0};
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, GENERIC_POWER_LEVEL_SERVER_GET_RANGE, &range);
                }

                generic_power_level_info_t *ppower_level_info = pmodel_info->pargs;
                if ((0 != range.range_min) && (0 != range.range_max))
                {
                    ppower_level_info->target_power = CLAMP(pmsg->power, range.range_min, range.range_max);
                }
                else
                {
                    ppower_level_info->target_power = pmsg->power;
                }
                ppower_level_info->tid = pmsg->tid;
                ppower_level_info->trans_time = trans_time;
                ppower_level_info->delay_time = delay_time;

                uint16_t present_power;
                generic_power_level_process_result_t result =
                {
                    .state_changed = FALSE,
                    .use_transition = FALSE
                };

                if (delay_time > 0)
                {
                    result.use_transition = TRUE;
                    present_power = get_present_power_level(pmodel_info);
                    delay_execution_timer_start(pmodel_info, DELAY_EXECUTION_TYPE_POWER_LEVEL, delay_time,
                                                generic_power_level_delay_execution);
                }
                else
                {
                    present_power = generic_power_level_process(pmodel_info,
                                                                ppower_level_info->target_power,
                                                                trans_time, &result);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (pmesh_msg->access_opcode == MESH_MSG_GENERIC_POWER_LEVEL_SET)
                {
                    generic_power_level_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                             present_power,
                                             (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == trans_time.num_steps) ? FALSE : TRUE,
                                             ppower_level_info->target_power, trans_time, delay_rsp_time);
                }
#if MODEL_ENABLE_DELAY_MSG_RSP
                bool ack = (pmesh_msg->access_opcode == MESH_MSG_GENERIC_POWER_LEVEL_SET_UNACK) ? FALSE : TRUE;
                if (!result.use_transition)
                {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                    if (result.state_changed)
#endif
                    {
                        uint32_t delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time, TRUE,
                                                                            ack);
                        generic_power_level_delay_publish(pmodel_info, present_power, delay_pub_time);
                    }
                }
                else
                {
                    ppower_level_info->delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time,
                                                                                  delay_rsp_time, FALSE, ack);
                }
#else
                generic_power_level_state_change_publish(pmodel_info, present_power, result);
#endif
            }
        }
        break;
    case MESH_MSG_GENERIC_POWER_LAST_GET:
        if (pmesh_msg->msg_len == sizeof(generic_power_last_get_t))
        {
            generic_power_level_server_get_t last_data = {0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, GENERIC_POWER_LEVEL_SERVER_GET_LAST,
                                           &last_data);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            generic_power_last_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                    last_data.power, delay_rsp_time);
        }
        break;
    case MESH_MSG_GENERIC_POWER_DEFAULT_GET:
        if (pmesh_msg->msg_len == sizeof(generic_power_default_get_t))
        {
            generic_power_level_server_get_t default_data = {0};
            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info,
                                           GENERIC_POWER_LEVEL_SERVER_GET_DEFAULT,
                                           &default_data);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            generic_power_default_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                       default_data.power, delay_rsp_time);
        }
        break;
    case MESH_MSG_GENERIC_POWER_RANGE_GET:
        if (pmesh_msg->msg_len == sizeof(generic_power_range_get_t))
        {
            generic_power_level_server_get_range_t range_data = {0, 0};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, GENERIC_POWER_LEVEL_SERVER_GET_RANGE,
                                           &range_data);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            generic_power_range_stat(pmodel_info, pmesh_msg->src,
                                     pmesh_msg->app_key_index, GENERIC_STAT_SUCCESS,
                                     range_data.range_min, range_data.range_max, delay_rsp_time);
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

static int32_t generic_power_level_server_publish(const mesh_model_info_p pmodel_info, bool retrans)
{
	(void) retrans;
    generic_transition_time_t remaining_time = {0, 0};
    generic_power_level_stat(pmodel_info, 0, 0, get_present_power_level(pmodel_info), FALSE, 0,
                             remaining_time, 0);
    return 0;
}

#if MESH_MODEL_ENABLE_DEINIT
static void generic_power_level_server_deinit(mesh_model_info_t *pmodel_info)
{
    if (pmodel_info->model_receive == generic_power_level_server_receive)
    {
        /* stop delay execution */
        delay_execution_timer_stop(pmodel_info, DELAY_EXECUTION_TYPE_POWER_LEVEL);
        /* stop transition */
        generic_transition_timer_stop(pmodel_info, GENERIC_TRANSITION_TYPE_POWER_LEVEL);

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

bool generic_power_level_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_POWER_LEVEL_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        generic_power_level_info_t *ppower_level_info = plt_malloc(sizeof(generic_power_level_info_t),
                                                                   RAM_TYPE_DATA_ON);
        if (NULL == ppower_level_info)
        {
            printe("generic_power_level_server_reg: fail to allocate memory for the new model extension data!");
            return FALSE;
        }
        memset(ppower_level_info, 0, sizeof(generic_power_level_info_t));
        pmodel_info->pargs = ppower_level_info;

        pmodel_info->model_receive = generic_power_level_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_power_level_server_reg: missing model data process callback!");
        }
#if MESH_MODEL_ENABLE_DEINIT
        pmodel_info->model_deinit = generic_power_level_server_deinit;
#endif
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = generic_power_level_server_publish;
    }

    generic_transition_time_init();
    delay_execution_init();
    return mesh_model_reg(element_index, pmodel_info);
}

