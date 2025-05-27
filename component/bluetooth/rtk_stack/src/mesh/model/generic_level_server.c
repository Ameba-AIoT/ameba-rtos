/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_level_server.c
* @brief    Source file for generic level server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-7-13
* @version  v1.0
* *************************************************************************************
*/

#include "generic_level.h"
#include "generic_types.h"
#include "delay_execution.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif

typedef struct
{
    uint8_t tid;
    int16_t target_level;
    generic_transition_time_t trans_time;
    uint32_t delay_time;
    int16_t move_delta;
    int32_t delta_level;
    bool max_move;
#if MODEL_ENABLE_DELAY_MSG_RSP
    uint32_t delay_pub_time;
#endif
} generic_level_info_t;

typedef struct
{
    bool state_changed;
    bool use_transition;
} generic_level_process_result_t;

#define GENERIC_LEVEL_MAX     0x7FFF

static mesh_msg_send_cause_t generic_level_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                                uint16_t app_key_index, int16_t present_level,
                                                bool optional, int16_t target_level,
                                                generic_transition_time_t remaining_time, uint32_t delay_time)
{
    generic_level_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_LEVEL_STAT);
    uint16_t msg_len;
    if (optional)
    {
        msg_len = sizeof(generic_level_stat_t);
        msg.target_level = target_level;
        msg.remaining_time = remaining_time;
    }
    else
    {
        msg_len = MEMBER_OFFSET(generic_level_stat_t, target_level);
    }
    msg.present_level = present_level;

    mesh_msg_t mesh_msg = {0};
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = (uint8_t *)&msg;
    mesh_msg.msg_len = msg_len;
    if (0 != dst)
    {
        mesh_msg.dst = dst;
        mesh_msg.app_key_index = app_key_index;
    }
    mesh_msg.delay_time = delay_time;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t generic_level_delay_publish(const mesh_model_info_p pmodel_info,
                                                  int16_t present_level, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        generic_transition_time_t trans_time = {0, 0};
        ret = generic_level_stat(pmodel_info, 0, 0, present_level, FALSE, present_level, trans_time,
                                 delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t generic_level_publish(const mesh_model_info_p pmodel_info,
                                            int16_t present_level)
{
    return generic_level_delay_publish(pmodel_info, present_level, 0);
}

#if !MODEL_ENABLE_DELAY_MSG_RSP
static void generic_level_state_change_publish(const mesh_model_info_p pmodel_info,
                                               int16_t present_level, generic_level_process_result_t result)
{
    if (result.use_transition)
    {
        return ;
    }

#if !MODEL_ENABLE_PUBLISH_ALL_TIME
    if (result.state_changed)
#endif
    {
        generic_level_publish(pmodel_info, present_level);
    }
}
#endif

static int16_t get_present_level(const mesh_model_info_p pmodel_info)
{
    generic_level_server_get_t get_data = {0};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, GENERIC_LEVEL_SERVER_GET, &get_data);
    }

    return get_data.level;
}

static int32_t generic_level_trans_step_change(const mesh_model_info_p pmodel_info, uint32_t type,
                                               generic_transition_time_t total_time,
                                               generic_transition_time_t remaining_time)
{
    UNUSED(type);
    int32_t ret = MODEL_SUCCESS;
    generic_level_server_set_t set_data;
    generic_level_info_t *plevel_info = pmodel_info->pargs;
    if (NULL == plevel_info)
    {
        return 0;
    }
    set_data.level = plevel_info->target_level;
    set_data.total_time = total_time;
    set_data.remaining_time = remaining_time;

    if (NULL != pmodel_info->model_data_cb)
    {
        ret = pmodel_info->model_data_cb(pmodel_info, GENERIC_LEVEL_SERVER_SET, &set_data);
    }

    if (0 == remaining_time.num_steps)
    {
        int16_t present_level = get_present_level(pmodel_info);
#if MODEL_ENABLE_DELAY_MSG_RSP
        generic_level_delay_publish(pmodel_info, present_level, plevel_info->delay_pub_time);
#else
        generic_level_publish(pmodel_info, present_level);
#endif
    }

    return ret;
}

static int16_t generic_level_process(const mesh_model_info_p pmodel_info, uint32_t type,
                                     int16_t move_delta,
                                     int16_t target_level, generic_transition_time_t trans_time,
                                     generic_level_process_result_t *presult)
{
    int16_t level_before_set = 0;
    int16_t level_after_set = 0;

    /* get generic level before set */
    level_before_set = get_present_level(pmodel_info);
    level_after_set = level_before_set;

    int32_t ret = MODEL_SUCCESS;
    generic_level_server_set_t trans_set_data;
    generic_level_server_set_move_t move_set_data;
    switch (type)
    {
    case GENERIC_LEVEL_SERVER_SET:
        {
            trans_set_data.level = target_level;
            trans_set_data.total_time = trans_time;
            trans_set_data.remaining_time = trans_time;

            if (NULL != pmodel_info->model_data_cb)
            {
                ret = pmodel_info->model_data_cb(pmodel_info, GENERIC_LEVEL_SERVER_SET, &trans_set_data);
            }
        }
        break;
    case GENERIC_LEVEL_SERVER_SET_MOVE:
        {
            move_set_data.move_delta = move_delta;
            move_set_data.target_level = target_level;
            move_set_data.total_time = trans_time;
            move_set_data.remaining_time = trans_time;

            if (NULL != pmodel_info->model_data_cb)
            {
                ret = pmodel_info->model_data_cb(pmodel_info, GENERIC_LEVEL_SERVER_SET_MOVE, &move_set_data);
            }
            break;
        }
    default:
        break;
    }
    if ((GENERIC_TRANSITION_NUM_STEPS_UNKNOWN != trans_time.num_steps) &&
        (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE != trans_time.num_steps))
    {
        if ((ret >= 0) && (MODEL_STOP_TRANSITION != ret))
        {
            if (NULL != presult)
            {
                presult->use_transition = TRUE;
            }
            generic_transition_timer_start(pmodel_info, GENERIC_TRANSITION_TYPE_LEVEL, trans_time,
                                           generic_level_trans_step_change);
        }
#if MODEL_ENABLE_USER_STOP_TRANSITION_NOTIFICATION
        else if (MODEL_STOP_TRANSITION == ret)
        {
            switch (type)
            {
            case GENERIC_LEVEL_SERVER_SET:
                {
                    if (NULL != pmodel_info->model_data_cb)
                    {
                        ret = pmodel_info->model_data_cb(pmodel_info, GENERIC_LEVEL_SERVER_SET, &trans_set_data);
                    }
                }
                break;
            case GENERIC_LEVEL_SERVER_SET_MOVE:
                {
                    if (NULL != pmodel_info->model_data_cb)
                    {
                        ret = pmodel_info->model_data_cb(pmodel_info, GENERIC_LEVEL_SERVER_SET_MOVE, &move_set_data);
                    }
                    break;
                }
            default:
                break;
            }
        }
#endif
    }
    else
    {
        /* get level after set */
        level_after_set = get_present_level(pmodel_info);
    }

    if (level_before_set != level_after_set)
    {
        if (NULL != presult)
        {
            presult->state_changed = TRUE;
        }
    }

    return level_after_set;
}

static int32_t generic_level_delay_execution(mesh_model_info_t *pmodel_info, uint32_t delay_type)
{

    switch (delay_type)
    {
    case DELAY_EXECUTION_TYPE_LEVEL:
    case DELAY_EXECUTION_TYPE_LEVEL_MOVE:
        {
            generic_level_info_t *plevel_info = pmodel_info->pargs;
            if (NULL == plevel_info)
            {
                return 0;
            }
            plevel_info->delay_time = 0;
            int16_t present_level = 0;
            generic_level_process_result_t result =
            {
                .state_changed = FALSE,
                .use_transition = FALSE
            };
            if (DELAY_EXECUTION_TYPE_LEVEL == delay_type)
            {
                present_level = generic_level_process(pmodel_info, GENERIC_LEVEL_SERVER_SET, 0,
                                                      plevel_info->target_level, plevel_info->trans_time, &result);
            }
            else
            {
                present_level = generic_level_process(pmodel_info, GENERIC_LEVEL_SERVER_SET_MOVE,
                                                      plevel_info->move_delta, plevel_info->target_level,
                                                      plevel_info->trans_time, &result);
            }
#if MODEL_ENABLE_DELAY_MSG_RSP
            if (!result.use_transition)
            {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                if (result.state_changed)
#endif
                {
                    generic_level_delay_publish(pmodel_info, present_level, plevel_info->delay_pub_time);
                }
            }
#else
            generic_level_state_change_publish(pmodel_info, present_level, result);
#endif
        }
        break;
    default:
        break;
    }

    return 0;
}

static bool generic_level_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_LEVEL_GET:
        if (pmesh_msg->msg_len == sizeof(generic_level_get_t))
        {
            generic_level_info_t *plevel_info = pmodel_info->pargs;

            generic_transition_time_t remaining_time;
            if (plevel_info->delay_time > 0)
            {
                remaining_time = plevel_info->trans_time;
            }
            else
            {
                remaining_time = generic_transition_time_get(pmesh_msg->pmodel_info,
                                                             GENERIC_TRANSITION_TYPE_LEVEL);
            }

            if (plevel_info->max_move)
            {
                remaining_time.num_steps = GENERIC_TRANSITION_NUM_STEPS_UNKNOWN;
                remaining_time.step_resolution = GENERIC_TRANSITION_STEP_RESOLUTION_10MINUTES;
            }

            int16_t present_level = get_present_level(pmodel_info);

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            generic_level_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                               present_level,
                               (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == remaining_time.num_steps) ? FALSE : TRUE,
                               plevel_info->target_level, remaining_time, delay_rsp_time);
        }
        break;
    case MESH_MSG_GENERIC_LEVEL_SET:
    case MESH_MSG_GENERIC_LEVEL_SET_UNACK:
        {
            generic_level_set_t *pmsg = (generic_level_set_t *)pbuffer;
            generic_transition_time_t trans_time = {GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE, 0};
            uint32_t delay_time = 0;
            if (pmesh_msg->msg_len == MEMBER_OFFSET(generic_level_set_t, trans_time))
            {
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, GENERIC_LEVEL_SERVER_GET_DEFAULT_TRANSITION_TIME,
                                               &trans_time);
                }
            }
            else if (pmesh_msg->msg_len == sizeof(generic_level_set_t))
            {
                trans_time = pmsg->trans_time;
                delay_time = pmsg->delay * DELAY_EXECUTION_STEP_RESOLUTION;
            }
            if (IS_GENERIC_TRANSITION_STEPS_VALID(trans_time.num_steps))
            {
                generic_level_info_t *plevel_info = pmodel_info->pargs;
                plevel_info->target_level = pmsg->level;
                plevel_info->tid = pmsg->tid;
                plevel_info->delay_time = delay_time;
                plevel_info->trans_time = trans_time;

                int16_t present_level;
                generic_level_process_result_t result =
                {
                    .state_changed = FALSE,
                    .use_transition = FALSE
                };

                if (delay_time > 0)
                {
                    result.use_transition = TRUE;
                    present_level = get_present_level(pmodel_info);
                    delay_execution_timer_start(pmodel_info, DELAY_EXECUTION_TYPE_LEVEL, delay_time,
                                                generic_level_delay_execution);
                }
                else
                {
                    present_level = generic_level_process(pmodel_info, GENERIC_LEVEL_SERVER_SET, 0,
                                                          plevel_info->target_level, trans_time, &result);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (pmesh_msg->access_opcode == MESH_MSG_GENERIC_LEVEL_SET)
                {
                    generic_level_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                       present_level,
                                       (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == trans_time.num_steps) ? FALSE : TRUE,
                                       plevel_info->target_level, trans_time, delay_rsp_time);
                }
#if MODEL_ENABLE_DELAY_MSG_RSP
                bool ack = (pmesh_msg->access_opcode == MESH_MSG_GENERIC_LEVEL_SET_UNACK) ? FALSE : TRUE;
                if (!result.use_transition)
                {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                    if (result.state_changed)
#endif
                    {
                        uint32_t delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time, TRUE,
                                                                            ack);
                        generic_level_delay_publish(pmodel_info, present_level, delay_pub_time);
                    }
                }
                else
                {
                    plevel_info->delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time,
                                                                            FALSE, ack);
                }
#else
                generic_level_state_change_publish(pmodel_info, present_level, result);
#endif
            }
        }
        break;
    case MESH_MSG_GENERIC_DELTA_SET:
    case MESH_MSG_GENERIC_DELTA_SET_UNACK:
        {
            generic_delta_set_t *pmsg = (generic_delta_set_t *)pbuffer;
            generic_transition_time_t trans_time = {GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE, 0};
            uint32_t delay_time = 0;
            if (pmesh_msg->msg_len == MEMBER_OFFSET(generic_delta_set_t, trans_time))
            {
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, GENERIC_LEVEL_SERVER_GET_DEFAULT_TRANSITION_TIME,
                                               &trans_time);
                }
            }
            else if (pmesh_msg->msg_len == sizeof(generic_delta_set_t))
            {
                trans_time = pmsg->trans_time;
                delay_time = pmsg->delay * DELAY_EXECUTION_STEP_RESOLUTION;
            }

            if (IS_GENERIC_TRANSITION_STEPS_VALID(trans_time.num_steps))
            {
                generic_level_info_t *plevel_info = pmodel_info->pargs;
                generic_level_server_get_t get_data = {0};
                if (plevel_info->tid == pmsg->tid)
                {
                    plevel_info->target_level = plevel_info->target_level - plevel_info->delta_level +
                                                pmsg->delta_level;
                }
                else
                {
                    /* get level */
                    if (NULL != pmodel_info->model_data_cb)
                    {
                        pmodel_info->model_data_cb(pmodel_info, GENERIC_LEVEL_SERVER_GET, &get_data);
                    }
                    plevel_info->target_level = get_data.level + pmsg->delta_level;
                }
                plevel_info->tid = pmsg->tid;
                plevel_info->delta_level = pmsg->delta_level;
                plevel_info->delay_time = delay_time;
                plevel_info->trans_time = trans_time;
                int16_t present_level;
                generic_level_process_result_t result =
                {
                    .state_changed = FALSE,
                    .use_transition = FALSE
                };

                if (delay_time > 0)
                {
                    result.use_transition = TRUE;
                    present_level = get_present_level(pmodel_info);
                    delay_execution_timer_start(pmodel_info, DELAY_EXECUTION_TYPE_LEVEL, delay_time,
                                                generic_level_delay_execution);
                }
                else
                {
                    present_level = generic_level_process(pmodel_info, GENERIC_LEVEL_SERVER_SET, 0,
                                                          plevel_info->target_level, trans_time, &result);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (pmesh_msg->access_opcode == MESH_MSG_GENERIC_DELTA_SET)
                {
                    generic_level_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                       present_level,
                                       (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == trans_time.num_steps) ? FALSE : TRUE,
                                       plevel_info->target_level, trans_time, delay_rsp_time);
                }
#if MODEL_ENABLE_DELAY_MSG_RSP
                uint32_t delay_pub_time = 0;
                bool ack = (pmesh_msg->access_opcode == MESH_MSG_GENERIC_DELTA_SET_UNACK) ? FALSE : TRUE;
                if (!result.use_transition)
                {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                    if (result.state_changed)
#endif
                    {
                        delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time, TRUE, ack);
                        generic_level_delay_publish(pmodel_info, present_level, delay_pub_time);
                    }
                }
                else
                {
                    plevel_info->delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time,
                                                                            FALSE, ack);
                }
#else
                generic_level_state_change_publish(pmodel_info, present_level, result);
#endif
            }
        }
        break;
    case MESH_MSG_GENERIC_MOVE_SET:
    case MESH_MSG_GENERIC_MOVE_SET_UNACK:
        {
            generic_move_set_t *pmsg = (generic_move_set_t *)pbuffer;
            generic_transition_time_t trans_time = {GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE, 0};
            uint32_t delay_time = 0;
            if (pmesh_msg->msg_len == MEMBER_OFFSET(generic_move_set_t, trans_time))
            {
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, GENERIC_LEVEL_SERVER_GET_DEFAULT_TRANSITION_TIME,
                                               &trans_time);
                }
            }
            else if (pmesh_msg->msg_len == sizeof(generic_move_set_t))
            {
                trans_time = pmsg->trans_time;
                delay_time = pmsg->delay * DELAY_EXECUTION_STEP_RESOLUTION;
            }
            if (IS_GENERIC_TRANSITION_STEPS_VALID(trans_time.num_steps))
            {
                generic_level_info_t *plevel_info = pmodel_info->pargs;
                if (IS_MAX_GENERIC_TRANSITION_TIME(trans_time))
                {
                    plevel_info->target_level = GENERIC_LEVEL_MAX;
                    trans_time.num_steps = GENERIC_TRANSITION_NUM_STEPS_UNKNOWN;
                    plevel_info->max_move = TRUE;
                }
                else
                {
                    generic_level_server_get_t get_data = {0};
                    /* get level */
                    if (NULL != pmodel_info->model_data_cb)
                    {
                        pmodel_info->model_data_cb(pmodel_info, GENERIC_LEVEL_SERVER_GET, &get_data);
                    }

                    plevel_info->target_level = get_data.level + trans_time.num_steps * pmsg->delta_level;
                    plevel_info->max_move = FALSE;
                }

                plevel_info->tid = pmsg->tid;
                plevel_info->delay_time = delay_time;
                plevel_info->trans_time = trans_time;
                plevel_info->move_delta = pmsg->delta_level;

                int16_t present_level;
                generic_level_process_result_t result =
                {
                    .state_changed = FALSE,
                    .use_transition = FALSE
                };

                if (delay_time > 0)
                {
                    result.use_transition = TRUE;
                    present_level = get_present_level(pmodel_info);
                    delay_execution_timer_start(pmodel_info, DELAY_EXECUTION_TYPE_LEVEL_MOVE, delay_time,
                                                generic_level_delay_execution);
                }
                else
                {
                    present_level = generic_level_process(pmodel_info, GENERIC_LEVEL_SERVER_SET_MOVE,
                                                          pmsg->delta_level,
                                                          plevel_info->target_level,
                                                          trans_time, &result);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (pmesh_msg->access_opcode == MESH_MSG_GENERIC_MOVE_SET)
                {
                    generic_level_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                       present_level,
                                       (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == trans_time.num_steps) ? FALSE : TRUE,
                                       plevel_info->target_level, trans_time, delay_rsp_time);
                }
#if MODEL_ENABLE_DELAY_MSG_RSP
                uint32_t delay_pub_time = 0;
                bool ack = (pmesh_msg->access_opcode == MESH_MSG_GENERIC_MOVE_SET_UNACK) ? FALSE : TRUE;
                if (!result.use_transition)
                {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                    if (result.state_changed)
#endif
                    {
                        delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time, TRUE, ack);
                        generic_level_delay_publish(pmodel_info, present_level, delay_pub_time);
                    }
                }
                else
                {
                    plevel_info->delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time,
                                                                            FALSE, ack);
                }
#else
                generic_level_state_change_publish(pmodel_info, present_level, result);
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

static int32_t generic_level_server_publish(mesh_model_info_p pmodel_info, bool retrans)
{
    UNUSED(retrans);
    generic_transition_time_t trans_time = {0, 0};
    generic_level_stat(pmodel_info, 0, 0, get_present_level(pmodel_info), FALSE, 0, trans_time, 0);
    return 0;
}

#if MESH_MODEL_ENABLE_DEINIT
static void generic_level_server_deinit(mesh_model_info_t *pmodel_info)
{
    if (pmodel_info->model_receive == generic_level_server_receive)
    {
        /* stop delay execution */
        delay_execution_timer_stop(pmodel_info, DELAY_EXECUTION_TYPE_LEVEL);
        delay_execution_timer_stop(pmodel_info, DELAY_EXECUTION_TYPE_LEVEL_MOVE);
        /* stop transition */
        generic_transition_timer_stop(pmodel_info, GENERIC_TRANSITION_TYPE_LEVEL);

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

bool generic_level_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_LEVEL_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        generic_level_info_t *plevel_info = plt_malloc(sizeof(generic_level_info_t), RAM_TYPE_DATA_ON);
        if (NULL == plevel_info)
        {
            printe("generic_level_server_reg: fail to allocate memory for the new model extension data!");
            return FALSE;
        }
        memset(plevel_info, 0, sizeof(generic_level_info_t));
        pmodel_info->pargs = plevel_info;

        pmodel_info->model_receive = generic_level_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_level_server_reg: missing model data process callback!");
        }
#if MESH_MODEL_ENABLE_DEINIT
        pmodel_info->model_deinit = generic_level_server_deinit;
#endif
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = generic_level_server_publish;
    }

    generic_transition_time_init();
    delay_execution_init();
    return mesh_model_reg(element_index, pmodel_info);
}
