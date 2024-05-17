/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     time_client.c
* @brief    Source file for time client model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-8-23
* @version  v1.0
* *************************************************************************************
*/

#include "time_model.h"

static mesh_msg_send_cause_t time_client_send(const mesh_model_info_p pmodel_info,
                                              uint16_t dst, uint16_t app_key_index, uint8_t *pmsg,
                                              uint16_t msg_len)
{
    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t time_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                               uint16_t app_key_index)
{
    time_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TIME_GET);
    return time_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t time_get_zone(const mesh_model_info_p pmodel_info, uint16_t dst,
                                    uint16_t app_key_index)
{
    time_zone_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TIME_ZONE_GET);
    return time_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t time_get_tai_utc_delta(const mesh_model_info_p pmodel_info,
                                             uint16_t dst,
                                             uint16_t app_key_index)
{
    tai_utc_delta_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TAI_UTC_DELTA_GET);
    return time_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));

}

mesh_msg_send_cause_t time_get_role(const mesh_model_info_p pmodel_info, uint16_t dst,
                                    uint16_t app_key_index)
{
    time_role_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TIME_ROLE_GET);
    return time_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t time_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                               uint16_t app_key_index, tai_time_t time)
{
    time_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TIME_SET);
    uint8_t *temp = msg.tai_seconds;
    *((tai_time_t *)(temp)) = time;
    return time_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t time_set_zone(const mesh_model_info_p pmodel_info, uint16_t dst,
                                    uint16_t app_key_index, uint8_t time_zone_offset_new,
                                    uint8_t tai_of_zone_change[5])
{
    time_zone_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TIME_ZONE_SET);
    msg.time_zone_offset_new = time_zone_offset_new;
    memcpy(msg.tai_of_zone_change, tai_of_zone_change, 5);
    return time_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t time_set_tai_utc_delta(const mesh_model_info_p pmodel_info,
                                             uint16_t dst,
                                             uint16_t app_key_index, uint16_t tai_utc_delta_new,
                                             uint8_t tai_of_delta_change[5])
{
    tai_utc_delta_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TAI_UTC_DELTA_SET);
    msg.tai_utc_delta_new = tai_utc_delta_new;
    msg.padding = 0;
    memcpy(msg.tai_of_delta_change, tai_of_delta_change, 5);
    return time_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t time_set_role(const mesh_model_info_p pmodel_info, uint16_t dst,
                                    uint16_t app_key_index, time_role_t role)
{
    time_role_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TIME_ROLE_SET);
    msg.role = role;
    return time_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

static bool time_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_TIME_STATUS:
        if (pmesh_msg->msg_len == sizeof(time_status_t) ||
            pmesh_msg->msg_len == MEMBER_OFFSET(time_status_t, subsecond))
        {
            time_status_t *pmsg = (time_status_t *)pbuffer;
            time_client_status_t status_data;
            memset(&status_data, 0, sizeof(time_client_status_t));
            if (pmesh_msg->msg_len == sizeof(time_status_t))
            {
                status_data.src = pmesh_msg->src;
                uint8_t *temp = pmsg->tai_seconds;
                status_data.tai_time = *((tai_time_t *)(temp));
            }
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, TIME_CLIENT_STATUS,
                                           &status_data);
            }
        }
        break;
    case MESH_MSG_TIME_ROLE_STATUS:
        if (pmesh_msg->msg_len == sizeof(time_role_status_t))
        {
            time_role_status_t *pmsg = (time_role_status_t *)pbuffer;
            time_client_status_role_t status_data;
            status_data.role = (time_role_t)(pmsg->role);
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, TIME_CLIENT_STATUS_ROLE,
                                           &status_data);
            }
        }
        break;
    case MESH_MSG_TIME_ZONE_STATUS:
        if (pmesh_msg->msg_len == sizeof(time_zone_status_t))
        {
            time_zone_status_t *pmsg = (time_zone_status_t *)pbuffer;
            time_client_status_zone_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.time_zone_offset_current = pmsg->time_zone_offset_current;
            status_data.time_zone_offset_new = pmsg->time_zone_offset_new;
            memcpy(status_data.tai_of_zone_change, pmsg->tai_of_zone_change, 5);
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, TIME_CLIENT_STATUS_ZONE,
                                           &status_data);
            }
        }
        break;
    case MESH_MSG_TAI_UTC_DELTA_STATUS:
        if (pmesh_msg->msg_len == sizeof(tai_utc_delta_status_t))
        {
            tai_utc_delta_status_t *pmsg = (tai_utc_delta_status_t *)pbuffer;
            time_client_status_tai_utc_delta_t status_data;
            status_data.src = pmesh_msg->src;
            status_data.tai_utc_delta_current = pmsg->tai_utc_delta_current;
            status_data.padding1 = 0;
            status_data.tai_utc_delta_new = pmsg->tai_utc_delta_new;
            status_data.padding2 = 0;
            memcpy(status_data.tai_of_delta_change, pmsg->tai_of_delta_change, 5);
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, TIME_CLIENT_STATUS_TAI_UTC_DELTA,
                                           &status_data);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool time_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_TIME_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = time_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("time_client_reg: missing data process callback!");
        }
    }
    return mesh_model_reg(element_index, pmodel_info);
}

