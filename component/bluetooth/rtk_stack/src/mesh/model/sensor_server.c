/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     sensor_server.c
* @brief    Source file for sensor server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-8-29
* @version  v1.0
* *************************************************************************************
*/

#include <math.h>
#include "sensor.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif

#define SENSOR_FAST_PERIOD_ENABLE       1

#define SENSOR_TIME_PERIOD   100
#define PROPERTY_ID_LEN      2

typedef struct
{
    sensor_db_t *sensors;
    uint16_t num_sensors;
} sensor_info_t;

typedef struct
{
    uint16_t format: 1;
    uint16_t length: 4;
    uint16_t property_id: 11;
} _PACKED4_ sensor_formata_t;

typedef struct
{
    uint8_t format: 1;
    uint8_t length: 7;
    uint16_t property_id;
} _PACKED4_ sensor_formatb_t;

uint8_t sensor_tolerance_to_percentage(uint16_t tolerance)
{
    return tolerance / 4095.0 * 100;
}

double sensor_measurement_period_to_seconds(uint8_t period)
{
    return pow(1.1, period - 64);
}

double sensor_update_interval_to_seconds(uint8_t interval)
{
    return pow(1.1, interval - 64);
}

void sensor_server_set_db(mesh_model_info_p pmodel_info, sensor_db_t *sensors, uint16_t num_sensors)
{
    sensor_info_t *pinfo = pmodel_info->pargs;
    pinfo->sensors = sensors;
    pinfo->num_sensors = num_sensors;
}

static mesh_msg_send_cause_t sensor_server_send(const mesh_model_info_p pmodel_info,
                                                uint16_t dst, uint16_t app_key_index, void *pmsg, uint16_t msg_len,
                                                uint32_t delay_time)
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

static mesh_msg_send_cause_t sensor_status_internal(const mesh_model_info_p pmodel_info,
                                                    uint16_t dst, uint16_t app_key_index,
                                                    uint16_t property_id, uint32_t delay_time)
{
    sensor_status_t *pmsg;
    mesh_msg_send_cause_t ret;
    uint16_t msg_len = sizeof(sensor_status_t);
    sensor_info_t *pinfo = pmodel_info->pargs;
    sensor_server_get_t get_data = {0, NULL};
    if (0 == property_id)
    {
        /* get all sensor data */
        for (uint16_t i = 0; i < pinfo->num_sensors; ++i)
        {
            if ((pinfo->sensors[i].sensor_raw_data_len <= 16) &&
                (pinfo->sensors[i].descriptor.property_id < 2048))
            {
                msg_len += 2;
            }
            else
            {
                msg_len += 3;
            }
            msg_len += pinfo->sensors[i].sensor_raw_data_len;
        }
        pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
        if (NULL == pmsg)
        {
            return MESH_MSG_SEND_CAUSE_NO_MEMORY;
        }
        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_STATUS);
        uint8_t *pdata = pmsg->sensor_data;
        for (uint16_t i = 0; i < pinfo->num_sensors; ++i)
        {
            if ((pinfo->sensors[i].sensor_raw_data_len <= 16) &&
                (pinfo->sensors[i].descriptor.property_id < 2048))
            {
                sensor_formata_t *pformat = (sensor_formata_t *)pdata;
                pformat->format = 0;
                pformat->length = pinfo->sensors[i].sensor_raw_data_len - 1;
                pformat->property_id = pinfo->sensors[i].descriptor.property_id;
                pdata += sizeof(sensor_formata_t);
            }
            else
            {
                sensor_formatb_t *pformat = (sensor_formatb_t *)pdata;
                pformat->format = 1;
                pformat->length = pinfo->sensors[i].sensor_raw_data_len - 1;
                pformat->property_id = pinfo->sensors[i].descriptor.property_id;
                pdata += sizeof(sensor_formatb_t);
            }

            get_data.property_id = pinfo->sensors[i].descriptor.property_id;
            get_data.raw_data = NULL;
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, SENSOR_SERVER_GET, &get_data);
            }

            if (NULL != get_data.raw_data)
            {
                for (uint16_t j = 0; j < pinfo->sensors[i].sensor_raw_data_len; ++j)
                {
                    *pdata ++ = ((uint8_t *)(get_data.raw_data))[j];
                }
            }
        }
    }
    else
    {
        /* get specified sensor data */
        sensor_db_t *psensor = NULL;
        for (uint16_t i = 0; i < pinfo->num_sensors; ++i)
        {
            if (pinfo->sensors[i].descriptor.property_id == property_id)
            {
                psensor = &pinfo->sensors[i];
            }
        }

        if (NULL == psensor)
        {
            if (property_id < 2048)
            {
                msg_len += 2;
            }
            else
            {
                msg_len += 3;
            }
        }
        else
        {
            if ((psensor->sensor_raw_data_len <= 16) &&
                (psensor->descriptor.property_id < 2048))
            {
                msg_len += 2;
            }
            else
            {
                msg_len += 3;
            }
            msg_len += psensor->sensor_raw_data_len;
        }
        pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
        if (NULL == pmsg)
        {
            return MESH_MSG_SEND_CAUSE_NO_MEMORY;
        }
        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_STATUS);
        uint8_t *pdata = pmsg->sensor_data;
        if (NULL == psensor)
        {
            if (property_id < 2048)
            {
                sensor_formata_t *pformat = (sensor_formata_t *)pdata;
                pformat->format = 0;
                pformat->length = 0x0f;
                pformat->property_id = property_id;
                pdata += sizeof(sensor_formata_t);
            }
            else
            {
                sensor_formatb_t *pformat = (sensor_formatb_t *)pdata;
                pformat->format = 1;
                pformat->length = 0x7f;
                pformat->property_id = property_id;
                pdata += sizeof(sensor_formatb_t);
            }
        }
        else
        {
            if ((psensor->sensor_raw_data_len <= 16) &&
                (psensor->descriptor.property_id < 2048))
            {
                sensor_formata_t *pformat = (sensor_formata_t *)pdata;
                pformat->format = 0;
                pformat->length = psensor->sensor_raw_data_len - 1;
                pformat->property_id = psensor->descriptor.property_id;
                pdata += sizeof(sensor_formata_t);
            }
            else
            {
                sensor_formatb_t *pformat = (sensor_formatb_t *)pdata;
                pformat->format = 1;
                pformat->length = psensor->sensor_raw_data_len - 1;
                pformat->property_id = psensor->descriptor.property_id;
                pdata += sizeof(sensor_formatb_t);
            }

            get_data.property_id = psensor->descriptor.property_id;
            get_data.raw_data = NULL;
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, SENSOR_SERVER_GET, &get_data);
            }

            if (NULL != get_data.raw_data)
            {
                for (uint16_t i = 0; i < psensor->sensor_raw_data_len; ++i)
                {
                    *pdata ++ = ((uint8_t *)get_data.raw_data)[i];
                }
            }
        }
    }
    ret = sensor_server_send(pmodel_info, dst, app_key_index, pmsg, msg_len, delay_time);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

static mesh_msg_send_cause_t sensor_status(const mesh_model_info_p pmodel_info, uint16_t dst,
                                           uint16_t app_key_index,
                                           const sensor_data_t *sensor_data, uint16_t num_sensor_data,
                                           uint32_t delay_time)
{
    sensor_status_t *pmsg;
    uint16_t msg_len = sizeof(sensor_status_t);
    for (uint16_t i = 0; i < num_sensor_data; ++i)
    {
        if ((sensor_data[i].raw_data_len <= 16) &&
            (sensor_data[i].property_id < 2048))
        {
            msg_len += 2;
        }
        else
        {
            msg_len += 3;
        }
        msg_len += sensor_data[i].raw_data_len;
    }
    pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_STATUS);
    uint8_t *pdata = pmsg->sensor_data;
    for (uint16_t i = 0; i < num_sensor_data; ++i)
    {
        if ((sensor_data[i].raw_data_len <= 16) &&
            (sensor_data[i].property_id < 2048))
        {
            sensor_formata_t *pformat = (sensor_formata_t *)pdata;
            pformat->format = 0;
            pformat->length = sensor_data[i].raw_data_len - 1;
            pformat->property_id = sensor_data[i].property_id;
            pdata += sizeof(sensor_formata_t);
        }
        else
        {
            sensor_formatb_t *pformat = (sensor_formatb_t *)pdata;
            pformat->format = 0;
            pformat->length = sensor_data[i].raw_data_len - 1;
            pformat->property_id = sensor_data[i].property_id;
            pdata += sizeof(sensor_formatb_t);
        }

        for (uint16_t j = 0; j < sensor_data[i].raw_data_len; ++j)
        {
            *pdata ++ = ((uint8_t *)sensor_data[i].raw_data)[j];
        }
    }

    mesh_msg_send_cause_t ret = sensor_server_send(pmodel_info, dst, app_key_index, pmsg, msg_len,
                                                   delay_time);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t sensor_delay_publish(const mesh_model_info_p pmodel_info,
                                           const sensor_data_t *sensor_data,
                                           uint16_t num_sensor_data, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = sensor_status(pmodel_info, 0, 0, sensor_data, num_sensor_data, delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t sensor_publish(const mesh_model_info_p pmodel_info,
                                     const sensor_data_t *sensor_data,
                                     uint16_t num_sensor_data)
{
    return sensor_delay_publish(pmodel_info, sensor_data, num_sensor_data, 0);
}

static int32_t sensor_server_publish(mesh_model_info_p pmodel_info, bool retrans)
{
	(void ) retrans;//avoid compile warning
    sensor_info_t *pinfo = pmodel_info->pargs;
    sensor_db_t *psensor = NULL;
    sensor_server_get_t get_data = {0, NULL};
    sensor_data_t sensor_data = {0, 0, NULL};

#if SENSOR_FAST_PERIOD_ENABLE
    uint32_t status_min_interval = 0;
    uint32_t pub_period = 0;
    for (uint16_t i = 0; i < pinfo->num_sensors; ++i)
    {
        psensor = &pinfo->sensors[i];
        if (psensor->pub_count <= 0)
        {
            status_min_interval = 0;
            /* get data */
            get_data.property_id = psensor->descriptor.property_id;
            get_data.raw_data = NULL;
            if (NULL != pmodel_info->model_data_cb)
            {
                pmodel_info->model_data_cb(pmodel_info, SENSOR_SERVER_GET, &get_data);
            }
            sensor_data.property_id = psensor->descriptor.property_id;
            sensor_data.raw_data_len = psensor->sensor_raw_data_len;
            sensor_data.raw_data = get_data.raw_data;
            /* publish */
            if (sensor_data.raw_data_len == 0 || sensor_data.raw_data != 0)
            {
                sensor_status(pmodel_info, 0, 0, &sensor_data, 1, 0);
            }
            /* compare data */
            sensor_server_compare_cadence_t get_data = {psensor->descriptor.property_id, FALSE};
            if (NULL != psensor->cadence)
            {
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, SENSOR_SERVER_COMPARE_CADENCE, &get_data);
                }
                status_min_interval = (1 << psensor->cadence->status_min_interval);
            }

            pub_period = mesh_model_pub_period_get(pmodel_info->pmodel);
            if (pub_period < status_min_interval)
            {
                pub_period = status_min_interval;
            }
            if (get_data.need_fast_divisor)
            {
                uint32_t divisor = (1 << psensor->cadence->fast_cadence_period_divisor);
                if (pub_period / divisor < status_min_interval)
                {
                    psensor->pub_count = status_min_interval / SENSOR_TIME_PERIOD;
                }
                else
                {
                    psensor->pub_count = pub_period / divisor / SENSOR_TIME_PERIOD;
                }
            }
            else
            {
                psensor->pub_count = pub_period / SENSOR_TIME_PERIOD;
            }
        }
        else
        {
            psensor->pub_count --;
        }
    }

    return SENSOR_TIME_PERIOD;
#else
    for (uint16_t i = 0; i < pinfo->num_sensors; ++i)
    {
        psensor = &pinfo->sensors[i];
        /* get data */
        get_data.property_id = psensor->descriptor.property_id;
        get_data.raw_data = NULL;
        if (NULL != pmodel_info->model_data_cb)
        {
            pmodel_info->model_data_cb(pmodel_info, SENSOR_SERVER_GET, &get_data);
        }
        sensor_data.property_id = psensor->descriptor.property_id;
        sensor_data.raw_data_len = psensor->sensor_raw_data_len;
        sensor_data.raw_data = get_data.raw_data;
        /* publish */
        sensor_status(pmodel_info, 0, 0, &sensor_data, 1, 0);
    }
    return 0;
#endif
}

static mesh_msg_send_cause_t sensor_descriptor_status(const mesh_model_info_p pmodel_info,
                                                      uint16_t dst, uint16_t app_key_index,
                                                      uint16_t property_id, uint32_t delay_time)
{
    sensor_descriptor_status_t *pmsg;
    sensor_info_t *pinfo = pmodel_info->pargs;
    mesh_msg_send_cause_t ret;
    uint16_t msg_len;
    if (0 == property_id)
    {
        /* get all descriptors */
        msg_len = sizeof(sensor_descriptor_status_t) + sizeof(sensor_descriptor_t) * pinfo->num_sensors;
        pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
        if (NULL == pmsg)
        {
            return MESH_MSG_SEND_CAUSE_NO_MEMORY;
        }

        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_DESCRIPTOR_STATUS);
        sensor_descriptor_t *pdescriptor = (sensor_descriptor_t *)pmsg->descriptors;
        for (uint16_t i = 0; i < pinfo->num_sensors; ++i)
        {
            *pdescriptor ++ = pinfo->sensors[i].descriptor;
        }
    }
    else
    {
        /* get specified descriptor */
        sensor_descriptor_t *pdescriptor = NULL;
        for (uint16_t i = 0; i < pinfo->num_sensors; ++i)
        {
            if (pinfo->sensors[i].descriptor.property_id == property_id)
            {
                pdescriptor = &(pinfo->sensors[i].descriptor);
                break;
            }
        }

        if (NULL == pdescriptor)
        {
            msg_len = sizeof(sensor_descriptor_status_t) + PROPERTY_ID_LEN;
        }
        else
        {
            msg_len = sizeof(sensor_descriptor_status_t) + sizeof(sensor_descriptor_t);
        }
        pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
        if (NULL == pmsg)
        {
            return MESH_MSG_SEND_CAUSE_NO_MEMORY;
        }

        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_DESCRIPTOR_STATUS);
        if (NULL == pdescriptor)
        {
            pmsg->descriptors[0] = (uint8_t)(property_id);
            pmsg->descriptors[1] = (uint8_t)(property_id >> 8);
        }
        else
        {
            *((sensor_descriptor_t *)(pmsg->descriptors)) = *pdescriptor;
        }
    }

    ret = sensor_server_send(pmodel_info, dst, app_key_index, pmsg, msg_len, delay_time);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

static mesh_msg_send_cause_t sensor_column_status(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                  uint16_t app_key_index, uint16_t property_id,
                                                  uint8_t raw_value_x_len, const void *raw_value_x,
                                                  uint16_t column_len, const void *column, uint32_t delay_time)
{
    sensor_column_status_t *pmsg;
    mesh_msg_send_cause_t ret;
    uint16_t msg_len;
    if ((0 == column_len) || (NULL == column))
    {
        msg_len = sizeof(sensor_column_status_t) + raw_value_x_len;
    }
    else
    {
        msg_len = sizeof(sensor_column_status_t) + column_len;
    }
    pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_COLUMN_STATUS);
    pmsg->property_id = property_id;
    uint8_t *pdata = pmsg->column_raw_value;
    if ((column_len > 0) && (NULL != column))
    {
        memcpy(pdata, column, column_len);
    }
    else
    {
        memcpy(pdata, raw_value_x, raw_value_x_len);
    }

    ret = sensor_server_send(pmodel_info, dst, app_key_index, pmsg, msg_len, delay_time);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

static mesh_msg_send_cause_t sensor_series_status(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                  uint16_t app_key_index, uint16_t property_id,
                                                  uint8_t series_len, void *series, uint32_t delay_time)
{
    sensor_series_status_t *pmsg;
    mesh_msg_send_cause_t ret;
    uint16_t msg_len = sizeof(sensor_column_status_t) + series_len;
    pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_SERIES_STATUS);
    pmsg->property_id = property_id;
    uint8_t *pdata = pmsg->series_raw_value;
    memcpy(pdata, series, series_len);

    ret = sensor_server_send(pmodel_info, dst, app_key_index, pmsg, msg_len, delay_time);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}


static bool sensor_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_SENSOR_DESCRIPTOR_GET:
        if (pmesh_msg->msg_len == sizeof(sensor_descriptor_get_t))
        {
            /* get specified descriptor */
            sensor_descriptor_get_t *pmsg = (sensor_descriptor_get_t *)pbuffer;
            if (IS_SENSOR_PROPERTY_ID_VALID(pmsg->property_id))
            {
                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                sensor_descriptor_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, pmsg->property_id,
                                         delay_rsp_time);
            }
        }
        else if (pmesh_msg->msg_len == MEMBER_OFFSET(sensor_descriptor_get_t, property_id))
        {
            /* get all descriptors */
            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            sensor_descriptor_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, 0, delay_rsp_time);
        }
        break;
    case MESH_MSG_SENSOR_GET:
        if (pmesh_msg->msg_len == sizeof(sensor_get_t))
        {
            /* get specified sensor data */
            sensor_get_t *pmsg = (sensor_get_t *)pbuffer;
            if (IS_SENSOR_PROPERTY_ID_VALID(pmsg->property_id))
            {
                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                sensor_status_internal(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, pmsg->property_id,
                                       delay_rsp_time);
            }
        }
        else if (pmesh_msg->msg_len == MEMBER_OFFSET(sensor_get_t, property_id))
        {
            /* get all sensor data */
            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
            sensor_status_internal(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, 0, delay_rsp_time);
        }
        break;
    case MESH_MSG_SENSOR_COLUMN_GET:
        {
            sensor_column_get_t *pmsg = (sensor_column_get_t *)pbuffer;
            if (IS_SENSOR_PROPERTY_ID_VALID(pmsg->property_id))
            {
                sensor_server_get_column_t get_data;
                memset(&get_data, 0, sizeof(sensor_server_get_column_t));
                get_data.property_id = pmsg->property_id;
                get_data.raw_value_x_len = pmesh_msg->msg_len - sizeof(sensor_column_get_t);
                get_data.raw_value_x = pmsg->raw_value_x;
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, SENSOR_SERVER_GET_COLUMN, &get_data);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                sensor_column_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                     pmsg->property_id, get_data.raw_value_x_len, get_data.raw_value_x,
                                     get_data.column_len, get_data.column, delay_rsp_time);
            }
        }
        break;
    case MESH_MSG_SENSOR_SERIES_GET:
        {
            sensor_series_get_t *pmsg = (sensor_series_get_t *)pbuffer;
            if (IS_SENSOR_PROPERTY_ID_VALID(pmsg->property_id))
            {
                sensor_server_get_series_t get_data;
                memset(&get_data, 0, sizeof(sensor_server_get_series_t));
                get_data.property_id = pmsg->property_id;
                if (pmesh_msg->msg_len > sizeof(sensor_series_get_t))
                {
                    get_data.raw_value_x_len = (pmesh_msg->msg_len - sizeof(sensor_series_get_t)) / 2;
                    get_data.raw_value_x1 = pmsg->raw_value_x;
                    get_data.raw_value_x2 = pmsg->raw_value_x + get_data.raw_value_x_len;
                }
                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, SENSOR_SERVER_GET_SERIES, &get_data);
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                sensor_series_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, pmsg->property_id,
                                     get_data.series_len, get_data.series, delay_rsp_time);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

#if MESH_MODEL_ENABLE_DEINIT
static void sensor_server_deinit(mesh_model_info_t *pmodel_info)
{
    if (pmodel_info->model_receive == sensor_server_receive)
    {
        if (NULL != pmodel_info->pargs)
        {
            plt_free(pmodel_info->pargs, RAM_TYPE_DATA_ON);
            pmodel_info->pargs = NULL;
        }
        pmodel_info->model_receive = NULL;
    }
}
#endif

bool sensor_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_SENSOR_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->pargs = plt_malloc(sizeof(sensor_info_t), RAM_TYPE_DATA_ON);
        if (NULL == pmodel_info->pargs)
        {
            printe("sensor_server_reg: fail to allocate memory for the new model extension data!");
            return FALSE;
        }
        memset(pmodel_info->pargs, 0, sizeof(sensor_info_t));

        pmodel_info->model_receive = sensor_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("sensor_server_reg: missing model data process callback!");
        }
#if MESH_MODEL_ENABLE_DEINIT
        pmodel_info->model_deinit = sensor_server_deinit;
#endif
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = sensor_server_publish;
    }
    return mesh_model_reg(element_index, pmodel_info);
}

