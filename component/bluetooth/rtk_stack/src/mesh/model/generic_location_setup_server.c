/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_location_setup_server.c
* @brief    Source file for generic location setup server model.
* @details  Data types and external functions declaration.
* @author   heactor_huang
* @date     2019-06-24
* @version  v1.0
* *************************************************************************************
*/
#include "generic_location.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif

extern mesh_msg_send_cause_t generic_location_global_status(mesh_model_info_p pmodel_info,
                                                            uint16_t dst, uint16_t app_key_index,
                                                            generic_location_global_t global,
                                                            uint16_t delay_time);
extern mesh_msg_send_cause_t generic_location_local_status(mesh_model_info_p pmodel_info,
                                                           uint16_t dst, uint16_t app_key_index,
                                                           generic_location_local_t local,
                                                           uint16_t delay_time);
extern generic_location_global_t get_present_global(mesh_model_info_p pmodel_info);
extern generic_location_local_t get_present_local(mesh_model_info_p pmodel_info);

typedef struct
{
    bool state_changed;
} generic_location_process_result_t;

static generic_location_global_t generic_location_global_process(const mesh_model_info_p
                                                                 pmodel_info,
                                                                 generic_location_global_t global,
                                                                 generic_location_process_result_t *presult)
{
    generic_location_global_t global_before_set;
    generic_location_global_t global_after_set;

    /* get generic location before set */
    global_before_set = get_present_global(pmodel_info);
    global_after_set = global_before_set;

    generic_location_server_set_global_t set_data;
    set_data = global;

    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, GENERIC_LOCATION_SERVER_SET_GLOBAL, &set_data);
    }

    /* get generic location global after set */
    global_after_set = get_present_global(pmodel_info);

    if ((global_before_set.global_latitude != global_after_set.global_latitude) ||
        (global_before_set.global_longitude != global_after_set.global_longitude) ||
        (global_before_set.global_altitude != global_after_set.global_altitude))
    {
        if (NULL != presult)
        {
            presult->state_changed = TRUE;
        }
    }

    return global_after_set;
}

static generic_location_local_t generic_location_local_process(const mesh_model_info_p pmodel_info,
                                                               generic_location_local_t local,
                                                               generic_location_process_result_t *presult)
{
    generic_location_local_t local_before_set;
    generic_location_local_t local_after_set;

    /* get generic location before set */
    local_before_set = get_present_local(pmodel_info);
    local_after_set = local_before_set;

    generic_location_server_set_local_t set_data;
    set_data = local;

    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, GENERIC_LOCATION_SERVER_SET_LOCAL, &set_data);
    }

    /* get generic location local after set */
    local_after_set = get_present_local(pmodel_info);

    if ((local_before_set.local_north != local_after_set.local_north) ||
        (local_before_set.local_east != local_after_set.local_east) ||
        (local_before_set.local_altitude != local_after_set.local_altitude) ||
        (local_before_set.floor_num != local_after_set.floor_num) ||
        (local_before_set.uncertainty.uncertainty != local_after_set.uncertainty.uncertainty))
    {
        if (NULL != presult)
        {
            presult->state_changed = TRUE;
        }
    }

    return local_after_set;
}

static mesh_msg_send_cause_t generic_location_global_delay_publish(const mesh_model_info_p
                                                                   pmodel_info,
                                                                   generic_location_global_t global, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = generic_location_global_status(pmodel_info, 0, 0, global, delay_time);
    }

    return ret;
}

#if !MODEL_ENABLE_DELAY_MSG_RSP
static void generic_location_global_state_change_publish(const mesh_model_info_p pmodel_info,
                                                         generic_location_global_t global,
                                                         generic_location_process_result_t result)
{
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
    if (result.state_changed)
#endif
    {
        generic_location_global_delay_publish(pmodel_info, global, 0);
    }
}
#endif

static mesh_msg_send_cause_t generic_location_local_delay_publish(const mesh_model_info_p
                                                                  pmodel_info,
                                                                  generic_location_local_t local, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = generic_location_local_status(pmodel_info, 0, 0, local, delay_time);
    }

    return ret;
}

#if !MODEL_ENABLE_DELAY_MSG_RSP
static void generic_location_local_state_change_publish(const mesh_model_info_p pmodel_info,
                                                        generic_location_local_t local,
                                                        generic_location_process_result_t result)
{
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
    if (result.state_changed)
#endif
    {
        generic_location_local_delay_publish(pmodel_info, local, 0);
    }
}
#endif

static bool generic_location_setup_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_LOCATION_GLOBAL_SET:
    case MESH_MSG_GENERIC_LOCATION_GLOBAL_SET_UNACK:
        if (pmesh_msg->msg_len == sizeof(generic_location_global_set_t))
        {
            generic_location_global_set_t *pmsg = (generic_location_global_set_t *)pbuffer;

            generic_location_process_result_t result = {FALSE};
            generic_location_global_t global = generic_location_global_process(pmodel_info, pmsg->global,
                                                                               &result);

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif

            if (pmesh_msg->access_opcode == MESH_MSG_GENERIC_LOCATION_GLOBAL_SET)
            {
                generic_location_global_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                               get_present_global(pmodel_info), delay_rsp_time);
            }

#if MODEL_ENABLE_DELAY_MSG_RSP
            bool ack = (pmesh_msg->access_opcode == MESH_MSG_GENERIC_LOCATION_GLOBAL_SET_UNACK) ? FALSE : TRUE;
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
            if (result.state_changed)
#endif
            {
                generic_transition_time_t trans_time = {0, 0};
                uint32_t delay_pub_time = delay_msg_get_trans_delay(0, trans_time, delay_rsp_time, TRUE,
                                                                    ack);
                generic_location_global_delay_publish(pmodel_info, global, delay_pub_time);
            }
#else
            generic_location_global_state_change_publish(pmodel_info, global, result);
#endif
        }
        break;
    case MESH_MSG_GENERIC_LOCATION_LOCAL_SET:
    case MESH_MSG_GENERIC_LOCATION_LOCAL_SET_UNACK:
        if (pmesh_msg->msg_len == sizeof(generic_location_local_set_t))
        {
            generic_location_local_set_t *pmsg = (generic_location_local_set_t *)pbuffer;

            generic_location_process_result_t result = {FALSE};
            generic_location_local_t local = generic_location_local_process(pmodel_info, pmsg->local, &result);

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif

            if (pmesh_msg->access_opcode == MESH_MSG_GENERIC_LOCATION_LOCAL_SET)
            {
                generic_location_local_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                              get_present_local(pmodel_info), delay_rsp_time);
            }

#if MODEL_ENABLE_DELAY_MSG_RSP
            bool ack = (pmesh_msg->access_opcode == MESH_MSG_GENERIC_LOCATION_LOCAL_SET_UNACK) ? FALSE : TRUE;
#if !MODEL_ENABLE_PUBLISH_ALL_TIME
            if (result.state_changed)
#endif
            {
                generic_transition_time_t trans_time = {0, 0};
                uint32_t delay_pub_time = delay_msg_get_trans_delay(0, trans_time, delay_rsp_time, TRUE,
                                                                    ack);
                generic_location_local_delay_publish(pmodel_info, local, delay_pub_time);
            }
#else
            generic_location_local_state_change_publish(pmodel_info, local, result);
#endif
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool generic_location_setup_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_LOCATION_SETUP_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_location_setup_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_location_setup_server_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}
