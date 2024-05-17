/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     light_lc_server.c
* @brief    Source file for light lc server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2019-07-17
* @version  v1.0
* *************************************************************************************
*/
#include "light_lc.h"
#include "delay_execution.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif
#include "model_property.h"

typedef struct
{
    generic_on_off_t target_light_on_off;
    uint8_t tid;
    generic_transition_time_t trans_time;
    uint32_t delay_time;
#if MODEL_ENABLE_DELAY_MSG_RSP
    uint32_t delay_pub_time;
#endif
} light_lc_info_t;

typedef struct
{
    bool state_changed;
    bool use_transition;
} light_lc_process_result_t;

static mesh_msg_send_cause_t light_lc_server_send(mesh_model_info_p pmodel_info,
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

static mesh_msg_send_cause_t light_lc_mode_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                                  uint16_t app_key_index, uint8_t mode, uint32_t delay_time)
{
    light_lc_mode_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LC_MODE_STATUS);
    msg.mode = mode;
    return light_lc_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(light_lc_mode_status_t),
                                app_key_index,
                                delay_time);
}

static mesh_msg_send_cause_t light_lc_om_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                                uint16_t app_key_index, uint8_t mode, uint32_t delay_time)
{
    light_lc_om_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LC_OM_STATUS);
    msg.mode = mode;
    return light_lc_server_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg), app_key_index,
                                delay_time);
}

static mesh_msg_send_cause_t light_lc_light_on_off_status(mesh_model_info_p pmodel_info,
                                                          uint16_t dst,
                                                          uint16_t app_key_index, generic_on_off_t present, bool optional,
                                                          generic_on_off_t target, generic_transition_time_t trans_time,
                                                          uint32_t delay_time)
{
    light_lc_light_on_off_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_LC_LIGHT_ON_OFF_STATUS);
    uint16_t msg_len = 0;
    if (optional)
    {
        msg.present_light_on_off = present;
        msg.target_light_on_off = target;
        msg.trans_time = trans_time;
        msg_len = sizeof(msg);
    }
    else
    {
        msg.present_light_on_off = present;
        msg_len = MEMBER_OFFSET(light_lc_light_on_off_status_t, target_light_on_off);
    }
    return light_lc_server_send(pmodel_info, dst, (uint8_t *)&msg, msg_len, app_key_index,
                                delay_time);
}

static uint8_t get_present_lc_mode(const mesh_model_info_p pmodel_info)
{
    light_lc_server_get_mode_t get_data = {0};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, LIGHT_LC_SERVER_GET_MODE, &get_data);
    }

    return get_data.mode;
}

static uint8_t get_present_lc_om(const mesh_model_info_p pmodel_info)
{
    light_lc_server_get_om_t get_data = {0};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, LIGHT_LC_SERVER_GET_OM, &get_data);
    }

    return get_data.mode;
}

static generic_on_off_t get_present_light_on_off(const mesh_model_info_p pmodel_info)
{
    light_lc_server_get_light_on_off_t get_data = {GENERIC_OFF};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, LIGHT_LC_SERVER_GET_LIGHT_ON_OFF, &get_data);
    }

    return get_data.on_off;
}

static mesh_msg_send_cause_t light_lc_mode_delay_publish(const mesh_model_info_p pmodel_info,
                                                         uint8_t mode, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = light_lc_mode_status(pmodel_info, 0, 0, mode, delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t light_lc_mode_publish(const mesh_model_info_p pmodel_info,
                                            uint8_t mode)
{
    return light_lc_mode_delay_publish(pmodel_info, mode, 0);
}

static mesh_msg_send_cause_t light_lc_om_delay_publish(const mesh_model_info_p pmodel_info,
                                                       uint8_t om, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = light_lc_om_status(pmodel_info, 0, 0, om, delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t light_lc_om_publish(const mesh_model_info_p pmodel_info,
                                          uint8_t om)
{
    return light_lc_om_delay_publish(pmodel_info, om, 0);
}

static mesh_msg_send_cause_t light_lc_light_on_off_delay_publish(const mesh_model_info_p
                                                                 pmodel_info,
                                                                 generic_on_off_t on_off, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        generic_transition_time_t trans_time = {0, 0};
        ret = light_lc_light_on_off_status(pmodel_info, 0, 0, on_off, FALSE, on_off, trans_time,
                                           delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t light_lc_light_on_off_publish(const mesh_model_info_p pmodel_info,
                                                    generic_on_off_t on_off)
{
    return light_lc_light_on_off_delay_publish(pmodel_info, on_off, 0);
}

#if !MODEL_ENABLE_DELAY_MSG_RSP
static void light_lc_mode_state_change_publish(const mesh_model_info_p pmodel_info,
                                               uint8_t mode, light_lc_process_result_t result)
{
    if (result.use_transition)
    {
        return ;
    }

#if !MODEL_ENABLE_PUBLISH_ALL_TIME
    if (result.state_changed)
#endif
    {
        light_lc_mode_publish(pmodel_info, mode);
    }
}
#endif

#if !MODEL_ENABLE_DELAY_MSG_RSP
static void light_lc_om_state_change_publish(const mesh_model_info_p pmodel_info,
                                             uint8_t mode, light_lc_process_result_t result)
{
    if (result.use_transition)
    {
        return ;
    }

#if !MODEL_ENABLE_PUBLISH_ALL_TIME
    if (result.state_changed)
#endif
    {
        light_lc_om_publish(pmodel_info, mode);
    }
}
#endif


#if !MODEL_ENABLE_DELAY_MSG_RSP
static void light_lc_light_on_off_state_change_publish(const mesh_model_info_p pmodel_info,
                                                       generic_on_off_t on_off, light_lc_process_result_t result)
{
    if (result.use_transition)
    {
        return ;
    }

#if !MODEL_ENABLE_PUBLISH_ALL_TIME
    if (result.state_changed)
#endif
    {
        light_lc_light_on_off_publish(pmodel_info, on_off);
    }
}
#endif

static int32_t light_lc_light_on_off_trans_step_change(const mesh_model_info_p pmodel_info,
                                                       uint32_t type,
                                                       generic_transition_time_t total_time,
                                                       generic_transition_time_t remaining_time)
{
	(void) type; //avoid warning
    int32_t ret = MODEL_SUCCESS;
    light_lc_server_set_light_on_off_t set_data;
    light_lc_info_t *plc_info = pmodel_info->pargs;
    if (NULL == plc_info)
    {
        return 0;
    }
    set_data.light_on_off = plc_info->target_light_on_off;
    set_data.total_time = total_time;
    set_data.remaining_time = remaining_time;
    if (NULL != pmodel_info->model_data_cb)
    {
        ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_LC_SERVER_SET_LIGHT_ON_OFF, &set_data);
    }

    if (0 == remaining_time.num_steps)
    {
        generic_on_off_t present_on_off = get_present_light_on_off(pmodel_info);
#if MODEL_ENABLE_DELAY_MSG_RSP
        light_lc_light_on_off_delay_publish(pmodel_info, present_on_off, plc_info->delay_pub_time);
#else
        light_lc_light_on_off_publish(pmodel_info, present_on_off);
#endif
    }

    return ret;
}

static generic_on_off_t light_lc_light_on_off_process(const mesh_model_info_p pmodel_info,
                                                      generic_on_off_t target_on_off,
                                                      generic_transition_time_t trans_time,
                                                      light_lc_process_result_t *presult)
{
    generic_on_off_t on_off_before_set;
    generic_on_off_t on_off_after_set;

    /* get generic on/off before set */
    on_off_before_set = get_present_light_on_off(pmodel_info);
    on_off_after_set = on_off_before_set;

    int32_t ret = MODEL_SUCCESS;
    light_lc_server_set_light_on_off_t trans_set_data;
    trans_set_data.total_time = trans_time;
    trans_set_data.remaining_time = trans_time;
    trans_set_data.light_on_off = target_on_off;

    if (NULL != pmodel_info->model_data_cb)
    {
        ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_LC_SERVER_SET_LIGHT_ON_OFF, &trans_set_data);
    }

    if (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE != trans_time.num_steps)
    {
        if ((ret >= 0) && (MODEL_STOP_TRANSITION != ret))
        {
            if (NULL != presult)
            {
                presult->use_transition = TRUE;
            }
            generic_transition_timer_start(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_LC, trans_time,
                                           light_lc_light_on_off_trans_step_change);
        }
#if MODEL_ENABLE_USER_STOP_TRANSITION_NOTIFICATION
        else if (MODEL_STOP_TRANSITION == ret)
        {
            if (NULL != pmodel_info->model_data_cb)
            {
                ret = pmodel_info->model_data_cb(pmodel_info, LIGHT_LC_SERVER_SET_LIGHT_ON_OFF, &trans_set_data);
            }
        }
#endif
    }
    else
    {
        /* get on/off after set */
        on_off_after_set = get_present_light_on_off(pmodel_info);
    }

    if (on_off_before_set != on_off_after_set)
    {
        if (NULL != presult)
        {
            presult->state_changed = TRUE;
        }
    }

    return on_off_after_set;
}

static int32_t light_lc_light_on_off_delay_execution(mesh_model_info_t *pmodel_info,
                                                     uint32_t delay_type)
{
    switch (delay_type)
    {
    case DELAY_EXECUTION_TYPE_LIGHT_LC:
        {
            light_lc_info_t *plc_info = pmodel_info->pargs;
            if (NULL == plc_info)
            {
                return 0;
            }
            plc_info->delay_time = 0;
            light_lc_process_result_t result =
            {
                .state_changed = FALSE,
                .use_transition = FALSE
            };
            generic_on_off_t present_on_off = light_lc_light_on_off_process(pmodel_info,
                                                                            plc_info->target_light_on_off,
                                                                            plc_info->trans_time, &result);
#if MODEL_ENABLE_DELAY_MSG_RSP
            if (!result.use_transition)
            {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                if (result.state_changed)
#endif
                {
                    light_lc_light_on_off_delay_publish(pmodel_info, present_on_off, plc_info->delay_pub_time);
                }
            }
#else
            light_lc_light_on_off_state_change_publish(pmodel_info, present_on_off, result);
#endif
        }
        break;
    default:
        break;
    }

    return 0;
}

static bool light_lc_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_LIGHT_LC_MODE_GET:
        if (pmesh_msg->msg_len == sizeof(light_lc_mode_get_t))
        {
            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_lc_mode_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                 get_present_lc_mode(pmodel_info), delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_LC_MODE_SET:
    case MESH_MSG_LIGHT_LC_MODE_SET_UNACK:
        {
            light_lc_mode_set_t *pmsg = (light_lc_mode_set_t *)pbuffer;
            uint8_t mode_before_set = get_present_lc_mode(pmodel_info);
            if (NULL != pmodel_info->model_data_cb)
            {
                light_lc_server_set_mode_t set_data = {pmsg->mode};
                pmodel_info->model_data_cb(pmodel_info, LIGHT_LC_SERVER_SET_MODE, &set_data);
            }

            uint8_t mode_after_set = get_present_lc_mode(pmodel_info);
            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            if (pmesh_msg->access_opcode == MESH_MSG_LIGHT_LC_MODE_SET)
            {
                light_lc_mode_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                     mode_after_set, delay_rsp_time);
            }

            light_lc_process_result_t result =
            {
                .state_changed = FALSE,
                .use_transition = FALSE
            };
            (void)result;

            if (mode_before_set != mode_after_set)
            {
                result.state_changed = TRUE;
            }
#if MODEL_ENABLE_DELAY_MSG_RSP
            bool ack = (pmesh_msg->access_opcode == MESH_MSG_LIGHT_LC_MODE_SET_UNACK) ? FALSE : TRUE;
            generic_transition_time_t trans_time = {0, 0};
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
            if (result.state_changed)
#endif
            {
                uint32_t delay_pub_time = delay_msg_get_trans_delay(0, trans_time, delay_rsp_time, TRUE,
                                                                    ack);
                light_lc_mode_delay_publish(pmodel_info, mode_after_set, delay_pub_time);
            }
#else
            light_lc_mode_state_change_publish(pmodel_info, mode_after_set, result);
#endif

            if (0 == pmsg->mode)
            {
                /* stop transition */
                generic_transition_timer_stop(pmesh_msg->pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_LC);
            }
        }
        break;
    case MESH_MSG_LIGHT_LC_OM_GET:
        if (pmesh_msg->msg_len == sizeof(light_lc_om_get_t))
        {
            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_lc_om_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                               get_present_lc_om(pmodel_info), delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_LC_OM_SET:
    case MESH_MSG_LIGHT_LC_OM_SET_UNACK:
        {
            light_lc_om_set_t *pmsg = (light_lc_om_set_t *)pbuffer;
            uint8_t mode_before_set = get_present_lc_om(pmodel_info);
            if (NULL != pmodel_info->model_data_cb)
            {
                light_lc_server_set_om_t set_data = {pmsg->mode};
                pmodel_info->model_data_cb(pmodel_info, LIGHT_LC_SERVER_SET_OM, &set_data);
            }
            uint8_t mode_after_set = get_present_lc_om(pmodel_info);

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            if (pmesh_msg->access_opcode == MESH_MSG_LIGHT_LC_OM_SET)
            {
                light_lc_om_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                   get_present_lc_om(pmodel_info), delay_rsp_time);
            }

            light_lc_process_result_t result =
            {
                .state_changed = FALSE,
                .use_transition = FALSE
            };
            (void)result;

            if (mode_before_set != mode_after_set)
            {
                result.state_changed = TRUE;
            }
#if MODEL_ENABLE_DELAY_MSG_RSP
            bool ack = (pmesh_msg->access_opcode == MESH_MSG_LIGHT_LC_OM_SET_UNACK) ? FALSE : TRUE;
            generic_transition_time_t trans_time = {0, 0};
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
            if (result.state_changed)
#endif
            {
                uint32_t delay_pub_time = delay_msg_get_trans_delay(0, trans_time, delay_rsp_time, TRUE,
                                                                    ack);
                light_lc_om_delay_publish(pmodel_info, mode_after_set, delay_pub_time);
            }
#else
            light_lc_om_state_change_publish(pmodel_info, mode_after_set, result);
#endif
        }
        break;
    case MESH_MSG_LIGHT_LC_LIGHT_ON_OFF_GET:
        if (pmesh_msg->msg_len == sizeof(light_lc_light_on_off_get_t))
        {
            /* get present on/off status */
            generic_on_off_t present_on_off = get_present_light_on_off(pmodel_info);
            /* get target on/off status*/
            light_lc_info_t *plc_info = pmodel_info->pargs;
            /* get remaining time */
            generic_transition_time_t remaining_time;
            if (plc_info->delay_time > 0)
            {
                remaining_time = plc_info->trans_time;
            }
            else
            {
                remaining_time = generic_transition_time_get(pmesh_msg->pmodel_info,
                                                             GENERIC_TRANSITION_TYPE_LIGHT_LC);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            light_lc_light_on_off_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                         present_on_off,
                                         (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == remaining_time.num_steps) ? FALSE : TRUE,
                                         plc_info->target_light_on_off, remaining_time, delay_rsp_time);
        }
        break;
    case MESH_MSG_LIGHT_LC_LIGHT_ON_OFF_SET:
    case MESH_MSG_LIGHT_LC_LIGHT_ON_OFF_SET_UNACK:
        {
            light_lc_light_on_off_set_t *pmsg = (light_lc_light_on_off_set_t *)pbuffer;
            generic_transition_time_t trans_time = {GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE, 0};
            uint32_t delay_time = 0;
            bool trans_exist = FALSE;
            if (pmesh_msg->msg_len == MEMBER_OFFSET(light_lc_light_on_off_set_t, trans_time))
            {
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, LIGHT_LC_SERVER_GET_DEFAULT_TRANSITION_TIME,
                                               &trans_time);
                }
            }
            else if (pmesh_msg->msg_len == sizeof(light_lc_light_on_off_set_t))
            {
                trans_exist = TRUE;
                trans_time = pmsg->trans_time;
                delay_time = pmsg->delay * DELAY_EXECUTION_STEP_RESOLUTION;
            }
            if (IS_GENERIC_ON_OFF_VALID(pmsg->light_on_off) &&
                IS_GENERIC_TRANSITION_STEPS_VALID(trans_time.num_steps))
            {
                light_lc_info_t *plc_info = pmodel_info->pargs;
                plc_info->tid = pmsg->tid;
                plc_info->target_light_on_off = pmsg->light_on_off;
                if (!trans_exist && GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == trans_time.num_steps)
                {
                    /* calculate transition time */
                    light_lc_server_get_sm_transition_time_t get_data;
                    get_data.light_on_off = pmsg->light_on_off;
                    get_data.sm_trans_time.num_steps = GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE;
                    get_data.sm_trans_time.step_resolution = 0;
                    if (NULL != pmodel_info->model_data_cb)
                    {
                        pmodel_info->model_data_cb(pmodel_info, LIGHT_LC_SERVER_GET_SM_TRANSITION_TIME, &get_data);
                    }
                    trans_time = get_data.sm_trans_time;
                }
                plc_info->trans_time = trans_time;
                plc_info->delay_time = delay_time;

                generic_on_off_t present_on_off;
                light_lc_process_result_t result =
                {
                    .state_changed = FALSE,
                    .use_transition = FALSE
                };

                if (delay_time > 0)
                {
                    result.use_transition = TRUE;
                    present_on_off = get_present_light_on_off(pmodel_info);
                    delay_execution_timer_start(pmodel_info, DELAY_EXECUTION_TYPE_LIGHT_LC, delay_time,
                                                light_lc_light_on_off_delay_execution);
                }
                else
                {
                    present_on_off = light_lc_light_on_off_process(pmodel_info, plc_info->target_light_on_off,
                                                                   trans_time,
                                                                   &result);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (pmesh_msg->access_opcode == MESH_MSG_LIGHT_LC_LIGHT_ON_OFF_SET)
                {
                    light_lc_light_on_off_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                                 present_on_off,
                                                 (GENERIC_TRANSITION_NUM_STEPS_IMMEDIATE == trans_time.num_steps) ? FALSE : TRUE,
                                                 plc_info->target_light_on_off, trans_time, delay_rsp_time);
                }

#if MODEL_ENABLE_DELAY_MSG_RSP
                bool ack = (pmesh_msg->access_opcode == MESH_MSG_LIGHT_LC_LIGHT_ON_OFF_SET_UNACK) ? FALSE : TRUE;
                if (!result.use_transition)
                {
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
                    if (result.state_changed)
#endif
                    {
                        uint32_t delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time, TRUE,
                                                                            ack);
                        light_lc_light_on_off_delay_publish(pmodel_info, present_on_off, delay_pub_time);
                    }
                }
                else
                {
                    plc_info->delay_pub_time = delay_msg_get_trans_delay(delay_time, trans_time, delay_rsp_time,
                                                                         FALSE, ack);
                }
#else
                light_lc_light_on_off_state_change_publish(pmodel_info, present_on_off, result);
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

static int32_t light_lc_server_publish(mesh_model_info_p pmodel_info, bool retrans)
{
	(void) retrans; //avoid warning
    generic_transition_time_t remaining_time;
    light_lc_light_on_off_status(pmodel_info, 0, 0, get_present_light_on_off(pmodel_info), FALSE,
                                 GENERIC_OFF,
                                 remaining_time, 0);
    return 0;
}

#if MESH_MODEL_ENABLE_DEINIT
static void light_lc_server_deinit(mesh_model_info_t *pmodel_info)
{
    if (pmodel_info->model_receive == light_lc_server_receive)
    {
        /* stop delay execution */
        delay_execution_timer_stop(pmodel_info, DELAY_EXECUTION_TYPE_LIGHT_LC);
        /* stop step transition */
        generic_transition_timer_stop(pmodel_info, GENERIC_TRANSITION_TYPE_LIGHT_LC);

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

bool light_lc_server_reg(uint8_t element_index, mesh_model_info_t *pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_LIGHT_LC_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        light_lc_info_t *plc_info = plt_malloc(sizeof(light_lc_info_t), RAM_TYPE_DATA_ON);
        if (NULL == plc_info)
        {
            printe("light_lc_server_reg: fail to allocate memory for the new model extension data!");
            return FALSE;
        }
        memset(plc_info, 0, sizeof(light_lc_info_t));
        pmodel_info->pargs = plc_info;

        pmodel_info->model_receive = light_lc_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("light_lc_server_reg: missing model data process callback!");
        }
#if MESH_MODEL_ENABLE_DEINIT
        pmodel_info->model_deinit = light_lc_server_deinit;
#endif
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = light_lc_server_publish;
    }

    generic_transition_time_init();
    delay_execution_init();
    return mesh_model_reg(element_index, pmodel_info);
}

