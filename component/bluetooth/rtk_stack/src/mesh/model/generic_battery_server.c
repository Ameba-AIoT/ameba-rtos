/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_battery_server.c
* @brief    Source file for generic battery server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-6-22
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "generic_battery.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif

/**
 * @brief convert time to array
 * @param buf - convert result output
 * @param time - time to convert
 */
static __INLINE void time_to_array(uint8_t *buf, uint32_t time)
{
    for (int i = 2; i >= 0; --i)
    {
        buf[i] = (uint8_t)(time & 0xff);
        time >>= 8;
    }
}

static mesh_msg_send_cause_t generic_battery_stat(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                  uint16_t app_key_index, uint8_t battery_level,
                                                  uint32_t time_to_discharge, uint32_t time_to_charge,
                                                  generic_battery_flags_t flags, uint32_t delay_time)
{
    generic_battery_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_BATTERY_STAT);
    uint16_t msg_len = sizeof(generic_battery_stat_t);
    uint8_t discharge[4], charge[4];
    msg.battery_level = battery_level;
    time_to_array(discharge, time_to_discharge);
    time_to_array(charge, time_to_charge);
    memcpy(msg.time_to_discharge, discharge, 3);
    memcpy(msg.time_to_charge, charge, 3);
    msg.flags = flags;

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

mesh_msg_send_cause_t generic_battery_publish(const mesh_model_info_p pmodel_info,
                                              uint8_t battery_level,
                                              uint32_t time_to_discharge, uint32_t time_to_charge,
                                              generic_battery_flags_t flags)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = generic_battery_stat(pmodel_info, 0, 0, battery_level, time_to_discharge, time_to_charge,
                                   flags, 0);
    }

    return ret;
}


/**
 * @brief default generic battery server receive function
 * @param pmesh_msg - received mesh message
 * @return process result
 */
static bool generic_battery_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    //uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_BATTERY_GET:
        if (pmesh_msg->msg_len == sizeof(generic_battery_get_t))
        {
            mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
            generic_battery_server_get_t get_data = {0, 0, 0, {0, 0, 0, 0}};
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, GENERIC_BATTERY_SERVER_GET, &get_data);
            }

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            generic_battery_stat(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, get_data.battery_level,
                                 get_data.time_to_discharge, get_data.time_to_charge, get_data.flags, delay_rsp_time);

        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

static int32_t generic_battery_server_publish(const mesh_model_info_p pmodel_info, bool retrans)
{
    // RTK porting:avoid compile warning
	(void) retrans; //avoid warning
    generic_battery_server_get_t get_data = {0, 0, 0, {0, 0, 0, 0}};
    if (NULL != pmodel_info->model_data_cb)
    {
        pmodel_info->model_data_cb(pmodel_info, GENERIC_BATTERY_SERVER_GET, &get_data);
    }

    generic_battery_stat(pmodel_info, 0, 0, get_data.battery_level, get_data.time_to_discharge,
                         get_data.time_to_charge,
                         get_data.flags, 0);

    return 0;
}

bool generic_battery_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_BATTERY_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_battery_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_battery_server_reg: missing model data process callback!");
        }
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = generic_battery_server_publish;
    }

    return mesh_model_reg(element_index, pmodel_info);
}

