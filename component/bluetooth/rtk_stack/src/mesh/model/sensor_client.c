/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     sensor_client.c
* @brief    Source file for sensor client model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-9-12
* @version  v1.0
* *************************************************************************************
*/

#include "sensor.h"

#define PROPERTY_ID_LEN                   2
#define SETTING_PROPERTY_ID_LEN           2
#define TRIGGER_DELTA_UNITLESS_LEN        2

static mesh_msg_send_cause_t sensor_client_send(const mesh_model_info_p pmodel_info,
                                                uint16_t dst, uint16_t app_key_index, uint8_t *pmsg, uint16_t msg_len)
{
    mesh_msg_t mesh_msg = {0};
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t sensor_descriptor_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                            uint16_t app_key_index, uint16_t property_id)
{
    sensor_descriptor_get_t msg;
    uint16_t msg_len = sizeof(msg);
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SENSOR_DESCRIPTOR_GET);
    if (0 == property_id)
    {
        msg_len -= PROPERTY_ID_LEN;
    }
    msg.property_id = property_id;

    return sensor_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, msg_len);
}

mesh_msg_send_cause_t sensor_cadence_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                         uint16_t app_key_index, uint16_t property_id)
{
    sensor_cadence_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SENSOR_CADENCE_GET);
    msg.property_id = property_id;

    return sensor_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t sensor_cadence_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                         uint16_t app_key_index, uint16_t property_id, const sensor_cadence_t *cadence, bool ack)
{
    mesh_msg_send_cause_t ret;
    sensor_cadence_set_t *pmsg = NULL;
    uint8_t trigger_len = 0;
    if (SENSOR_TRIGGER_TYPE_CHARACTERISTIC == cadence->status_trigger_type)
    {
        trigger_len = cadence->raw_value_len;
    }
    else
    {
        trigger_len = TRIGGER_DELTA_UNITLESS_LEN;
    }
    uint16_t msg_len = sizeof(sensor_cadence_set_t) + sizeof(uint8_t) + trigger_len * 2 +
                       cadence->raw_value_len * 2;
    pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    if (ack)
    {
        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_CADENCE_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_CADENCE_SET_UNACK);
    }
    pmsg->property_id = property_id;
    pmsg->fast_cadence_period_divisor = cadence->fast_cadence_period_divisor;
    pmsg->status_trigger_type = cadence->status_trigger_type;
    uint8_t *pdata = pmsg->cadence;

    for (uint8_t i = 0; i < trigger_len; ++i)
    {
        *pdata ++ = ((uint8_t *)cadence->status_trigger_delta_down)[i];
    }

    for (uint8_t i = 0; i < trigger_len; ++i)
    {
        *pdata ++ = ((uint8_t *)cadence->status_trigger_delta_up)[i];
    }

    *pdata ++ = cadence->status_min_interval;

    for (uint8_t i = 0; i < cadence->raw_value_len; ++i)
    {
        *pdata ++ = ((uint8_t *)cadence->fast_cadence_low)[i];
    }

    for (uint8_t i = 0; i < cadence->raw_value_len; ++i)
    {
        *pdata ++ = ((uint8_t *)cadence->fast_cadence_high)[i];
    }


    ret = sensor_client_send(pmodel_info, dst, app_key_index, (uint8_t *)pmsg, msg_len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);
    return ret;
}

mesh_msg_send_cause_t sensor_settings_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                          uint16_t app_key_index, uint16_t property_id)
{
    sensor_settings_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SENSOR_SETTINGS_GET);
    msg.property_id = property_id;

    return sensor_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t sensor_setting_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                         uint16_t app_key_index, uint16_t property_id, uint16_t setting_property_id)
{
    sensor_setting_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SENSOR_SETTING_GET);
    msg.property_id = property_id;
    msg.setting_property_id = setting_property_id;

    return sensor_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t sensor_setting_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                         uint16_t app_key_index, uint16_t property_id, uint16_t setting_property_id,
                                         uint8_t setting_raw_len, const void *setting_raw, bool ack)
{
    mesh_msg_send_cause_t ret;
    sensor_setting_set_t *pmsg = NULL;
    uint16_t msg_len = sizeof(sensor_setting_set_t) + setting_raw_len;
    pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    uint8_t *pdata = pmsg->setting;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_SETTING_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_SETTING_SET_UNACK);
    }
    pmsg->property_id = property_id;
    pmsg->setting_property_id = setting_property_id;

    for (uint8_t i = 0; i < setting_raw_len; ++i)
    {
        *pdata ++ = ((const uint8_t *)setting_raw)[i];
    }

    ret = sensor_client_send(pmodel_info, dst, app_key_index, (uint8_t *)pmsg, msg_len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);
    return ret;
}

mesh_msg_send_cause_t sensor_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                 uint16_t app_key_index, uint16_t property_id)
{
    sensor_get_t msg;
    uint16_t msg_len = sizeof(msg);
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SENSOR_GET);
    if (0 == property_id)
    {
        msg_len -= PROPERTY_ID_LEN;
    }
    msg.property_id = property_id;

    return sensor_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, msg_len);
}

mesh_msg_send_cause_t sensor_column_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                        uint16_t app_key_index, uint16_t property_id, uint8_t raw_value_x_len, const void *raw_value_x)
{
    mesh_msg_send_cause_t ret;
    sensor_column_get_t *pmsg = NULL;
    uint16_t msg_len = sizeof(sensor_column_get_t) + raw_value_x_len;
    pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_COLUMN_GET);
    pmsg->property_id = property_id;

    uint8_t *pdata = pmsg->raw_value_x;
    for (uint8_t i = 0; i < raw_value_x_len; ++i)
    {
        *pdata ++ = ((const uint8_t *)raw_value_x)[i];
    }

    ret = sensor_client_send(pmodel_info, dst, app_key_index, (uint8_t *)pmsg, msg_len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);
    return ret;
}

mesh_msg_send_cause_t sensor_series_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                        uint16_t app_key_index, uint16_t property_id, uint8_t raw_value_x_len, const void *raw_value_x1,
                                        const void *raw_value_x2)
{
    mesh_msg_send_cause_t ret;
    sensor_series_get_t *pmsg = NULL;
    uint16_t msg_len = sizeof(sensor_series_get_t) + raw_value_x_len * 2;
    pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_SERIES_GET);
    pmsg->property_id = property_id;

    uint8_t *pdata = pmsg->raw_value_x;
    for (uint8_t i = 0; i < raw_value_x_len; ++i)
    {
        *pdata ++ = ((const uint8_t *)raw_value_x1)[i];
    }

    for (uint8_t i = 0; i < raw_value_x_len; ++i)
    {
        *pdata ++ = ((const uint8_t *)raw_value_x2)[i];
    }

    ret = sensor_client_send(pmodel_info, dst, app_key_index, (uint8_t *)pmsg, msg_len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);
    return ret;
}

static bool sensor_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_SENSOR_DESCRIPTOR_STATUS:
        {
            sensor_descriptor_status_t *pmsg = (sensor_descriptor_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                sensor_client_status_descriptor_t status_data = {pmesh_msg->src, 0, 0, NULL};
                uint16_t descriptor_len = pmesh_msg->msg_len - sizeof(sensor_descriptor_status_t);
                if (PROPERTY_ID_LEN == descriptor_len)
                {
                    status_data.property_id = LE_EXTRN2WORD(pmsg->descriptors);
                }
                else
                {
                    status_data.num_descriptors = descriptor_len / sizeof(sensor_descriptor_t);
                    status_data.descriptors = (sensor_descriptor_t *)pmsg->descriptors;
                }

                pmodel_info->model_data_cb(pmodel_info, SENSOR_CLIENT_STATUS_DESCRIPTOR, &status_data);
            }
        }
        break;
    case MESH_MSG_SENSOR_CADENCE_STATUS:
        {
            sensor_cadence_status_t *pmsg = (sensor_cadence_status_t *)pbuffer;
            //RTK porting:avoid when pmsg->cadence == NULL
            if (NULL != pmodel_info->model_data_cb && (pmesh_msg->msg_len - sizeof(sensor_cadence_status_t)) != 0)
            {
                sensor_client_status_cadence_t status_data = {pmesh_msg->src, 0, NULL};
                status_data.property_id = pmsg->property_id;
                uint8_t *pdata = pmsg->cadence;
                sensor_cadence_t cadence;
                status_data.cadence = &cadence;
                cadence.fast_cadence_period_divisor = (*pdata & 0x7f);
                cadence.status_trigger_type = (*pdata >> 7);
                uint8_t trigger_len = 0;
                if (SENSOR_TRIGGER_TYPE_CHARACTERISTIC == cadence.status_trigger_type)
                {
                    trigger_len = (pmesh_msg->msg_len - sizeof(sensor_cadence_status_t) - sizeof(uint8_t) * 2) / 4;
                    cadence.raw_value_len = trigger_len;
                }
                else
                {
                    trigger_len = TRIGGER_DELTA_UNITLESS_LEN;
                    cadence.raw_value_len = (pmesh_msg->msg_len - sizeof(sensor_cadence_status_t) - sizeof(
                                                 uint8_t) * 2 - trigger_len * 2) / 2;
                }

                cadence.status_trigger_delta_down = pmsg->cadence + sizeof(uint8_t);
                cadence.status_trigger_delta_up = (uint8_t *)cadence.status_trigger_delta_down + trigger_len;
                cadence.status_min_interval = *((uint8_t *)cadence.status_trigger_delta_up + trigger_len);
                cadence.fast_cadence_low = (uint8_t *)cadence.status_trigger_delta_up + trigger_len + sizeof(
                                               uint8_t);
                cadence.fast_cadence_high = (uint8_t *)cadence.fast_cadence_low + cadence.raw_value_len;
                pmodel_info->model_data_cb(pmodel_info, SENSOR_CLIENT_STATUS_CADENCE, &status_data);
            //RTK porting:avoid when pmsg->cadence == NULL
            } else {
                sensor_client_status_cadence_t status_data = {pmesh_msg->src, 0, NULL};
                status_data.property_id = pmsg->property_id;
                pmodel_info->model_data_cb(pmodel_info, SENSOR_CLIENT_STATUS_CADENCE, &status_data);
            }
        }
        break;
    case MESH_MSG_SENSOR_SETTINGS_STATUS:
        {
            sensor_settings_status_t *pmsg = (sensor_settings_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                sensor_client_status_settings_t status_data = {pmesh_msg->src, 0, 0, NULL};
                status_data.property_id = pmsg->property_id;
                uint16_t ids_len = pmesh_msg->msg_len - sizeof(sensor_settings_status_t);
                status_data.num_ids = ids_len / SETTING_PROPERTY_ID_LEN;
                // avoid compile warning
                // status_data.setting_ids = pmsg->setting_ids;
                void *pointer = (void *)pmsg->setting_ids;
                status_data.setting_ids = (uint16_t *)pointer;
                pmodel_info->model_data_cb(pmodel_info, SENSOR_CLIENT_STATUS_SETTINGS, &status_data);
            }
        }
        break;
    case MESH_MSG_SENSOR_SETTING_STATUS:
        {
            sensor_setting_status_t *pmsg = (sensor_setting_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                sensor_client_status_setting_t status_data = {pmesh_msg->src, 0, 0, NULL};
                uint8_t data_len = pmesh_msg->msg_len - sizeof(sensor_setting_status_t);
                status_data.property_id = pmsg->property_id;
                status_data.setting_property_id = pmsg->setting_property_id;
                sensor_setting_t setting;
                if (data_len > 0)
                {
                    status_data.setting = &setting;
                    setting.setting_property_id = pmsg->setting_property_id;
                    setting.setting_raw_len = data_len - 1;
                    uint8_t *pdata = pmsg->setting;
                    setting.setting_access = (sensor_setting_access_t)(*pdata);
                    pdata ++;
                    setting.setting_raw = pdata;
                }
                pmodel_info->model_data_cb(pmodel_info, SENSOR_CLIENT_STATUS_SETTING, &status_data);
            }
        }
        break;
    case MESH_MSG_SENSOR_STATUS:
        {
            sensor_status_t *pmsg = (sensor_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                sensor_client_status_t status_data = {pmesh_msg->src, 0, NULL};
                status_data.marshalled_sensor_data_len = pmesh_msg->msg_len - sizeof(sensor_status_t);
                status_data.marshalled_sensor_data = pmsg->sensor_data;
                pmodel_info->model_data_cb(pmodel_info, SENSOR_CLIENT_STATUS, &status_data);
            }
        }
        break;
    case MESH_MSG_SENSOR_COLUMN_STATUS:
        {
            sensor_column_status_t *pmsg = (sensor_column_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                sensor_client_status_column_t status_data = {pmesh_msg->src, 0, 0, NULL};
                status_data.property_id = pmsg->property_id;
                status_data.column_raw_value_len = pmesh_msg->msg_len - sizeof(sensor_column_status_t);
                status_data.column_raw_value = pmsg->column_raw_value;
                pmodel_info->model_data_cb(pmodel_info, SENSOR_CLIENT_STATUS_COLUMN, &status_data);
            }
        }
        break;
    case MESH_MSG_SENSOR_SERIES_STATUS:
        {
            sensor_series_status_t *pmsg = (sensor_series_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                sensor_client_status_series_t status_data = {pmesh_msg->src, 0, 0, NULL};
                status_data.property_id = pmsg->property_id;
                status_data.series_raw_value_len = pmesh_msg->msg_len - sizeof(sensor_series_status_t);
                status_data.series_raw_value = pmsg->series_raw_value;
                pmodel_info->model_data_cb(pmodel_info, SENSOR_CLIENT_STATUS_SERIES, &status_data);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool sensor_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_SENSOR_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = sensor_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("sensor_client_reg: missing data process callback!");
        }
    }
    return mesh_model_reg(element_index, pmodel_info);
}

