/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_battery_client.c
* @brief    Source file for generic battery client model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-6-26
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "generic_battery.h"

/**
 * @brief convert array to time
 * @param buf - array to convert
 * @return time converted
 */
static __INLINE uint32_t array_to_time(uint8_t *buf)
{
    diag_assert(NULL != buf);
    uint32_t time = 0;
    for (int i = 2; i >= 0; --i)
    {
        time <<= 8;
        time |= buf[i];
    }

    return time;
}

static mesh_msg_send_cause_t generic_battery_client_send(const mesh_model_info_p pmodel_info,
                                                         uint16_t dst, uint16_t app_key_index,
                                                         uint8_t *pmsg, uint16_t msg_len)
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

mesh_msg_send_cause_t generic_battery_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                          uint16_t app_key_index)
{
    generic_battery_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_BATTERY_GET);
    return generic_battery_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

static bool generic_battery_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_BATTERY_STAT:
        if (pmesh_msg->msg_len == sizeof(generic_battery_stat_t))
        {
            generic_battery_stat_p pmsg = (generic_battery_stat_p)pbuffer;
            uint32_t discharge = array_to_time(pmsg->time_to_discharge);
            uint32_t charge = array_to_time(pmsg->time_to_charge);

            generic_battery_client_status_t status_data = {pmesh_msg->src, 0, 0, 0, {0, 0, 0, 0}};
            status_data.battery_level = pmsg->battery_level;
            status_data.time_to_discharge = discharge;
            status_data.time_to_charge = charge;
            status_data.flags = pmsg->flags;
            if (NULL != pmesh_msg->pmodel_info->model_data_cb)
            {
                pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, GENERIC_BATTERY_CLIENT_STATUS,
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

bool generic_battery_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_BATTERY_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_battery_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_battery_client_reg: missing model data process callback!");
        }
    }
    return mesh_model_reg(element_index, pmodel_info);
}

