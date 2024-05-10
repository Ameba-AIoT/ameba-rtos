/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     time_setup_server.c
* @brief    Source file for time setup server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-8-22
* @version  v1.0
* *************************************************************************************
*/

#include "time_model.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif

extern mesh_msg_send_cause_t time_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                         uint16_t app_key_index, tai_time_t time_info, uint32_t delay_time);
extern mesh_msg_send_cause_t time_zone_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                              uint16_t app_key_index, time_zone_t time_zone, uint32_t delay_time);
extern mesh_msg_send_cause_t tai_utc_delta_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                                  uint16_t app_key_index, tai_utc_delta_t tai_utc_delta, uint32_t delay_time);


mesh_msg_send_cause_t time_role_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                       uint16_t app_key_index, time_role_t role, uint32_t delay_time)
{
    time_role_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TIME_ROLE_STATUS);

    uint16_t msg_len = sizeof(time_role_status_t);
    msg.role = role;

    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = (uint8_t *)&msg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    mesh_msg.delay_time = delay_time;

    return access_send(&mesh_msg);
}

static bool time_setup_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_TIME_SET:
        if (pmesh_msg->msg_len == sizeof(time_set_t))
        {
            time_set_t *pmsg = (time_set_t *)pbuffer;

            time_server_set_t set_data;
            uint8_t *temp = pmsg->tai_seconds;
            set_data = *((tai_time_t *)temp);
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, TIME_SERVER_SET, &set_data);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            time_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, set_data, delay_rsp_time);
        }
        break;
    case MESH_MSG_TIME_ZONE_SET:
        if (pmesh_msg->msg_len == sizeof(time_zone_set_t))
        {
            time_zone_set_t *pmsg = (time_zone_set_t *)pbuffer;

            time_server_set_zone_t set_data;
            set_data.time_zone_offset_new = pmsg->time_zone_offset_new;
            memcpy(set_data.tai_of_zone_change, pmsg->tai_of_zone_change, 5);
            time_server_get_zone_t get_zone;
            memset(&get_zone, 0, sizeof(time_server_get_zone_t));
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, TIME_SERVER_SET_ZONE, &set_data);
                pmodel_info->model_data_cb(pmodel_info, TIME_SERVER_GET_ZONE, &get_zone);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            time_zone_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, get_zone, delay_rsp_time);
        }
        break;
    case MESH_MSG_TAI_UTC_DELTA_SET:
        if (pmesh_msg->msg_len == sizeof(tai_utc_delta_set_t))
        {
            tai_utc_delta_set_t *pmsg = (tai_utc_delta_set_t *)pbuffer;

            time_server_set_tai_utc_delta_t set_data;
            set_data.tai_utc_delta_new = pmsg->tai_utc_delta_new;
            memcpy(set_data.tai_of_delta_change, pmsg->tai_of_delta_change, 5);
            time_server_get_tai_utc_delta_t get_data;
            memset(&get_data, 0, sizeof(time_server_get_tai_utc_delta_t));
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, TIME_SERVER_SET_TAI_UTC_DELTA, &set_data);
                pmodel_info->model_data_cb(pmodel_info, TIME_SERVER_GET_TAI_UTC_DELTA, &get_data);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            tai_utc_delta_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, get_data,
                                 delay_rsp_time);
        }
        break;
    case MESH_MSG_TIME_ROLE_GET:
        if (pmesh_msg->msg_len == sizeof(time_role_get_t))
        {
            time_server_get_role_t get_role = {TIME_ROLE_NONE};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, TIME_SERVER_GET_ROLE, &get_role);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            time_role_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                             get_role.role, delay_rsp_time);
        }
        break;
    case MESH_MSG_TIME_ROLE_SET:
        if (pmesh_msg->msg_len == sizeof(time_role_set_t))
        {
            time_role_set_t *pmsg = (time_role_set_t *)pbuffer;
            if (IS_TIME_ROLE_VALID(pmsg->role))
            {
                time_server_set_role_t set_role = {pmsg->role};
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, TIME_SERVER_SET_ROLE, &set_role);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                time_role_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                 set_role.role, delay_rsp_time);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool time_setup_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_TIME_SETUP_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = time_setup_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("time_setup_server_reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}

