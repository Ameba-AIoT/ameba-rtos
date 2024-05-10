/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     time_server.c
* @brief    Source file for time server model.
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


int16_t tai_utc_delta_convert(uint16_t delta)
{
    return delta - 0xff;
}

int16_t time_zone_offset_convert(uint8_t time_zone_offset)
{
    return (int16_t)time_zone_offset - 0x40;
}

void increase_uncertainty(uint8_t *uncertainty, uint8_t delta)
{
    if ((MAX_UNCERTAINTY - *uncertainty) > delta)
    {
        *uncertainty += delta;
    }
    else
    {
        *uncertainty = MAX_UNCERTAINTY;
    }
}

static mesh_msg_send_cause_t time_server_send(mesh_model_info_p pmodel_info,
                                              uint16_t dst, void *pmsg, uint16_t msg_len, uint16_t app_key_index,
                                              uint32_t delay_time)
{
    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    mesh_msg.delay_time = delay_time;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t time_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                  uint16_t app_key_index, tai_time_t time, uint32_t delay_time)
{
    time_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TIME_STATUS);

    uint8_t zero_tai_seconds[5] = {0, 0, 0, 0, 0};
    uint8_t *temp = msg.tai_seconds;
    *((tai_time_t *)(temp)) = time;
    uint16_t msg_len = sizeof(time_status_t);
    if (0 == memcmp(time.tai_seconds, zero_tai_seconds, 5))
    {
        msg_len = MEMBER_OFFSET(time_status_t, subsecond);
    }

    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = (uint8_t *)&msg;
    mesh_msg.msg_len = msg_len;
    if (0 != dst)
    {
        mesh_msg.dst = dst;
        mesh_msg.app_key_index = app_key_index;
    }
    else
    {
        /* spec need time publish ttl to be 0 */
        mesh_msg.ttl = 0;
    }
    mesh_msg.delay_time = delay_time;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t time_zone_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                       uint16_t app_key_index, time_zone_t time_zone, uint32_t delay_time)
{
    time_zone_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TIME_ZONE_STATUS);
    uint8_t *temp = (uint8_t *) & (msg.time_zone_offset_current);
    *((time_zone_t *)temp) = time_zone;

    return time_server_send(pmodel_info, dst, &msg, sizeof(time_zone_status_t), app_key_index,
                            delay_time);
}

mesh_msg_send_cause_t tai_utc_delta_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                           uint16_t app_key_index, tai_utc_delta_t time_tai_utc_delta, uint32_t delay_time)
{
    tai_utc_delta_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_TAI_UTC_DELTA_STATUS);

    msg.tai_utc_delta_current = time_tai_utc_delta.tai_utc_delta_current;
    msg.padding1 = 0;
    msg.tai_utc_delta_new = time_tai_utc_delta.tai_utc_delta_new;
    msg.padding2 = 0;
    memcpy(msg.tai_of_delta_change, time_tai_utc_delta.tai_of_delta_change, 5);

    return time_server_send(pmodel_info, dst, &msg, sizeof(tai_utc_delta_status_t), app_key_index,
                            delay_time);
}

mesh_msg_send_cause_t time_delay_publish(const mesh_model_info_p pmodel_info,
                                         tai_time_t time, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = time_status(pmodel_info, 0, 0, time, delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t time_publish(const mesh_model_info_p pmodel_info,
                                   tai_time_t time)
{
    return time_delay_publish(pmodel_info, time, 0);
}

static tai_time_t get_present_time(const mesh_model_info_p pmodel_info)
{
    time_server_get_t get_data;
    memset(&get_data, 0, sizeof(time_server_get_t));

    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, TIME_SERVER_GET, &get_data);
    }

    return get_data;
}

static bool time_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_TIME_GET:
        if (pmesh_msg->msg_len == sizeof(time_get_t))
        {
            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            time_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, get_present_time(pmodel_info),
                        delay_rsp_time);
        }
        break;
    case MESH_MSG_TIME_STATUS:
        if (pmesh_msg->msg_len == sizeof(time_status_t))
        {
            time_status_t *pmsg = (time_status_t *)pbuffer;
            time_server_get_role_t get_role = {TIME_ROLE_NONE};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, TIME_SERVER_GET_ROLE, &get_role);
            }

            time_server_status_set_t status_set;
            memcpy(status_set.tai_seconds, pmsg->tai_seconds, 5);
            status_set.subsecond = pmsg->subsecond;
            status_set.uncertainty = pmsg->uncertainty;
            status_set.tai_utc_delta = pmsg->tai_utc_delta;
            status_set.time_zone_offset = pmsg->time_zone_offset;
            if ((TIME_ROLE_RELAY == get_role.role) ||
                (TIME_ROLE_CLIENT == get_role.role))
            {
                /* update time */
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, TIME_SERVER_STATUS_SET, &status_set);
                }
            }

            if (TIME_ROLE_RELAY == get_role.role)
            {
                /* publish time */
                uint8_t *temp = pmsg->tai_seconds;
                time_publish(pmodel_info, *((tai_time_t *)temp));
            }
        }
        break;
    case MESH_MSG_TIME_ZONE_GET:
        if (pmesh_msg->msg_len == sizeof(time_zone_get_t))
        {
            time_server_get_zone_t get_zone;
            memset(&get_zone, 0, sizeof(time_server_get_zone_t));

            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, TIME_SERVER_GET_ZONE, &get_zone);
            }
            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            time_zone_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, get_zone, delay_rsp_time);
        }
        break;
    case MESH_MSG_TAI_UTC_DELTA_GET:
        if (pmesh_msg->msg_len == sizeof(tai_utc_delta_get_t))
        {
            time_server_get_tai_utc_delta_t get_data;
            memset(&get_data, 0, sizeof(time_server_get_tai_utc_delta_t));

            if (NULL != pmodel_info->model_data_cb)
            {
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
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

static int32_t time_server_publish(mesh_model_info_p pmodel_info, bool retrans)
{
    UNUSED(retrans);
    time_status(pmodel_info, 0, 0, get_present_time(pmodel_info), 0);
    return 0;
}

bool time_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_TIME_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = time_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("time_server_reg: missing model data process callback!");
        }
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = time_server_publish;
    }

    return mesh_model_reg(element_index, pmodel_info);
}

