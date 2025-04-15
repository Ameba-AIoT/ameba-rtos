/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_location_server.c
* @brief    Source file for generic location server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2019-06-10
* @version  v1.0
* *************************************************************************************
*/
#include <math.h>
#include "generic_location.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif

#define CONVERT_FACTOR (2147483647.0)

static bool location_global_period_pub_enabled = TRUE;
static bool location_local_period_pub_enabled = TRUE;

double generic_location_latitude_global_to_x(int32_t latitude_g)
{
    return latitude_g / CONVERT_FACTOR * 90;
}

int32_t generic_location_latitude_x_to_global(double latitude_x)
{
    return latitude_x / 90 * CONVERT_FACTOR;
}

double generic_location_longitude_global_to_x(int32_t longitude_g)
{
    return longitude_g / CONVERT_FACTOR * 180;
}

int32_t generic_location_longitude_x_to_global(double longitude_x)
{
    return longitude_x / 180 * CONVERT_FACTOR;
}

double generic_location_update_time_to_seconds(uint8_t update_time)
{
    update_time &= 0x0f;
    return pow(2, update_time - 3);
}

uint8_t generic_location_seconds_to_update_time(double seconds)
{
    int8_t update_time = 0;
    update_time = log(seconds) / log(2);
    update_time += 3;
    if (update_time < 0)
    {
        update_time = 0;
    }

    update_time &= 0x0f;

    return update_time;
}

double generic_location_precision_to_meters(uint8_t precision)
{
    precision &= 0x0f;
    return pow(2, precision - 3);
}

uint8_t generic_location_meters_to_precision(double meters)
{
    int8_t precision = 0;
    precision = log(meters) / log(2);
    precision += 3;
    if (precision < 0)
    {
        precision = 0;
    }

    precision &= 0x0f;

    return precision;
}

void generic_location_period_pub_enable(bool global, bool local)
{
    location_global_period_pub_enabled = global;
    location_local_period_pub_enabled = local;
}

static mesh_msg_send_cause_t generic_location_send(mesh_model_info_p pmodel_info, uint16_t dst,
                                                   uint16_t app_key_index,
                                                   uint8_t *pmsg, uint16_t msg_len, uint16_t delay_time)
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

mesh_msg_send_cause_t generic_location_global_status(mesh_model_info_p pmodel_info,
                                                     uint16_t dst, uint16_t app_key_index,
                                                     generic_location_global_t global,
                                                     uint16_t delay_time)
{
    generic_location_global_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_LOCATION_GLOBAL_STATUS);
    msg.global = global;
    return generic_location_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                                 sizeof(msg), delay_time);
}

mesh_msg_send_cause_t generic_location_local_status(mesh_model_info_p pmodel_info,
                                                    uint16_t dst, uint16_t app_key_index,
                                                    generic_location_local_t local,
                                                    uint16_t delay_time)
{
    generic_location_local_status_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_LOCATION_LOCAL_STATUS);
    msg.local = local;
    return generic_location_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                                 sizeof(msg), delay_time);
}

generic_location_global_t get_present_global(mesh_model_info_p pmodel_info)
{
    generic_location_server_get_global_t get_data = {0, 0, 0};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, GENERIC_LOCATION_SERVER_GET_GLOBAL, &get_data);
    }

    return get_data;
}

generic_location_local_t get_present_local(mesh_model_info_p pmodel_info)
{
    generic_location_server_get_local_t get_data;
    memset(&get_data, 0, sizeof(generic_location_server_get_local_t));
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, GENERIC_LOCATION_SERVER_GET_LOCAL, &get_data);
    }

    return get_data;
}

mesh_msg_send_cause_t generic_location_global_publish(const mesh_model_info_p pmodel_info,
                                                      generic_location_global_t global)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = generic_location_global_status(pmodel_info, 0, 0, global, 0);
    }

    return ret;
}

mesh_msg_send_cause_t generic_location_local_publish(const mesh_model_info_p pmodel_info,
                                                     generic_location_local_t local)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = generic_location_local_status(pmodel_info, 0, 0, local, 0);
    }

    return ret;
}

static int32_t generic_location_server_publish(mesh_model_info_p pmodel_info, bool retrans)
{
    // RTK porting:avoid compile warning
	(void) retrans; //avoid warning
    if (location_global_period_pub_enabled)
    {
        generic_location_global_status(pmodel_info, 0, 0, get_present_global(pmodel_info), 0);
    }

    if (location_local_period_pub_enabled)
    {
        generic_location_local_status(pmodel_info, 0, 0, get_present_local(pmodel_info), 0);
    }

    return 0;
}


static bool generic_location_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_LOCATION_GLOBAL_GET:
        if (pmesh_msg->msg_len == sizeof(generic_location_global_get_t))
        {
            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            generic_location_global_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                           get_present_global(pmodel_info), delay_rsp_time);
        }
        break;
    case MESH_MSG_GENERIC_LOCATION_LOCAL_GET:
        if (pmesh_msg->msg_len == sizeof(generic_location_local_get_t))
        {
            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            generic_location_local_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                          get_present_local(pmodel_info), delay_rsp_time);

        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool generic_location_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_LOCATION_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_location_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_location_server_reg: missing model data process callback!");
        }
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = generic_location_server_publish;
    }

    return mesh_model_reg(element_index, pmodel_info);
}
