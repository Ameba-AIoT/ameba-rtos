/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_default_transition_time_server.c
* @brief    Source file for generic default transition time server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-7-9
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "generic_default_transition_time.h"


/**
 * @brief replay generic default transition time status
 * @param[in] pmodel_info: gdtt server model information
 * @param[in] dst: remote address
 * @param[in] app_key_index: mesh message used app key index
 * @param[in] trans_time: curent transition time
 * @return send status
 */
static mesh_msg_send_cause_t generic_default_transition_time_stat(mesh_model_info_p pmodel_info,
                                                                  uint16_t dst, uint16_t app_key_index,
                                                                  generic_transition_time_t trans_time)
{
    generic_default_transition_time_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_DEFAULT_TRANSITION_TIME_STAT);
    msg.trans_time = trans_time;

    mesh_msg_t mesh_msg = {0};
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = (uint8_t *)&msg;
    mesh_msg.msg_len = sizeof(generic_default_transition_time_stat_t);
    if (0 != dst)
    {
        mesh_msg.dst = dst;
        mesh_msg.app_key_index = app_key_index;
    }
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t generic_default_transition_time_publish(const mesh_model_info_p pmodel_info,
                                                              generic_transition_time_t trans_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = generic_default_transition_time_stat(pmodel_info, 0, 0, trans_time);
    }

    return ret;
}

static generic_transition_time_t get_present_transition_time(mesh_model_info_p pmodel_info)
{
    generic_default_transition_time_server_get_t get_data = {{0, 0}};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, GENERIC_DEFAULT_TRANSITION_TIME_SERVER_GET, &get_data);
    }

    return get_data.trans_time;
}

/**
 * @brief generic default transition time receive callback
 * @param[in] pmesh_msg: received mesh message
 * @return process status
 */
static bool generic_default_transition_time_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_t *pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_DEFAULT_TRANSITION_TIME_GET:
        if (pmesh_msg->msg_len == sizeof(generic_default_transition_time_get_t))
        {
            generic_default_transition_time_stat(pmesh_msg->pmodel_info, pmesh_msg->src,
                                                 pmesh_msg->app_key_index, get_present_transition_time(pmodel_info));
        }
        break;
    case MESH_MSG_GENERIC_DEFAULT_TRANSITION_TIME_SET:
    case MESH_MSG_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNACK:
        if (pmesh_msg->msg_len == sizeof(generic_default_transition_time_set_t))
        {
            generic_default_transition_time_set_t *pmsg = (generic_default_transition_time_set_t *)pbuffer;
            if (IS_GENERIC_TRANSITION_STEPS_VALID(pmsg->trans_time.num_steps))
            {
                generic_default_transition_time_server_set_t get_data = {{0, 0}};
                if (NULL != pmodel_info->model_data_cb)
                {
                    generic_default_transition_time_server_set_t set_data;
                    set_data.trans_time = pmsg->trans_time;
                    pmodel_info->model_data_cb(pmodel_info,
                                               GENERIC_DEFAULT_TRANSITION_TIME_SERVER_GET, &get_data);
                    pmodel_info->model_data_cb(pmodel_info,
                                               GENERIC_DEFAULT_TRANSITION_TIME_SERVER_SET, &set_data);
                }
                if (MESH_MSG_GENERIC_DEFAULT_TRANSITION_TIME_SET == pmesh_msg->access_opcode)
                {
                    generic_default_transition_time_stat(pmodel_info, pmesh_msg->src,
                                                         pmesh_msg->app_key_index, pmsg->trans_time);
                }

                if ((get_data.trans_time.num_steps != pmsg->trans_time.num_steps) ||
                    (get_data.trans_time.step_resolution != pmsg->trans_time.step_resolution))
                {
                    /* state change publish */
                    generic_default_transition_time_publish(pmodel_info, pmsg->trans_time);
                }
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

static int32_t generic_default_transition_time_server_publish(mesh_model_info_p pmodel_info,
                                                              bool retrans)
{
    UNUSED(retrans);
    generic_default_transition_time_stat(pmodel_info, 0, 0, get_present_transition_time(pmodel_info));
    return 0;
}

bool generic_default_transition_time_server_reg(uint8_t element_index,
                                                mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_default_transition_time_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_default_transition_time_server_reg: missing data process callback!");
        }
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = generic_default_transition_time_server_publish;
    }

    return mesh_model_reg(element_index, pmodel_info);
}

