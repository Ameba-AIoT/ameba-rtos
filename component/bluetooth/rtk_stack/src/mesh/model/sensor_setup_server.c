/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     sensor_setup_server.c
* @brief    Source file for sensor setup server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-8-22
* @version  v1.0
* *************************************************************************************
*/

#include "sensor.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif

#define SETTING_PROPERTY_ID_LEN               2
#define TRIGGER_DELTA_UNITLESS_LEN            2

typedef struct
{
    sensor_db_t *sensors;
    uint16_t num_sensors;
} sensor_setup_info_t;

void sensor_setup_server_set_db(mesh_model_info_p pmodel_info, sensor_db_t *sensors,
                                uint16_t num_sensors)
{
    sensor_setup_info_t *pinfo = pmodel_info->pargs;
    pinfo->sensors = sensors;
    pinfo->num_sensors = num_sensors;
}

static sensor_setting_t *sensor_get_setting(const mesh_model_info_p pmodel_info,
                                            uint16_t property_id, uint16_t setting_property_id)
{
    sensor_setup_info_t *pinfo = pmodel_info->pargs;
    for (uint16_t i = 0; i < pinfo->num_sensors; ++i)
    {
        if (pinfo->sensors[i].descriptor.property_id == property_id)
        {
            for (uint16_t j = 0; j < pinfo->sensors[i].num_settings; ++j)
            {
                if (pinfo->sensors[i].settings[j].setting_property_id == setting_property_id)
                {
                    return &pinfo->sensors[i].settings[j];
                }
            }
            break;
        }
    }

    return NULL;
}

static sensor_cadence_t *sensor_get_cadence(const mesh_model_info_p pmodel_info,
                                            uint16_t property_id)
{
    sensor_setup_info_t *pinfo = pmodel_info->pargs;
    for (uint16_t i = 0; i < pinfo->num_sensors; ++i)
    {
        if (pinfo->sensors[i].descriptor.property_id == property_id)
        {
            return pinfo->sensors[i].cadence;
        }
    }

    return NULL;
}

static mesh_msg_send_cause_t sensor_setup_server_send(const mesh_model_info_p pmodel_info,
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

static mesh_msg_send_cause_t sensor_cadence_status(const mesh_model_info_p pmodel_info,
                                                   uint16_t dst, uint16_t app_key_index,
                                                   uint16_t property_id, const sensor_cadence_t *cadence, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret;
    if (NULL == cadence)
    {
        sensor_cadence_status_t msg;
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SENSOR_CADENCE_STATUS);
        msg.property_id = property_id;
        ret = sensor_setup_server_send(pmodel_info, dst, app_key_index, &msg,
                                       sizeof(sensor_cadence_status_t), delay_time);
    }
    else
    {
        sensor_cadence_status_t *pmsg;
        uint16_t msg_len = 0;
        uint8_t trigger_len;
        if (SENSOR_TRIGGER_TYPE_CHARACTERISTIC == cadence->status_trigger_type)
        {
            msg_len = sizeof(sensor_cadence_status_t) + sizeof(sensor_cadence_t) - sizeof(
                          uint8_t *) * 4 - sizeof(uint8_t) + cadence->raw_value_len * 4;
            trigger_len = cadence->raw_value_len;
        }
        else
        {
            msg_len = sizeof(sensor_cadence_status_t) + sizeof(sensor_cadence_t) - sizeof(
                          uint8_t *) * 4  - sizeof(uint8_t) + TRIGGER_DELTA_UNITLESS_LEN * 2 + cadence->raw_value_len * 2;
            trigger_len = TRIGGER_DELTA_UNITLESS_LEN;
        }
        pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
        if (NULL == pmsg)
        {
            return MESH_MSG_SEND_CAUSE_NO_MEMORY;
        }

        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_CADENCE_STATUS);
        pmsg->property_id = property_id;
        uint8_t *pdata = pmsg->cadence;
        *pdata = cadence->fast_cadence_period_divisor;
        *pdata |= (cadence->status_trigger_type << 7);
        pdata ++;
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
        ret = sensor_setup_server_send(pmodel_info, dst, app_key_index, pmsg, msg_len, delay_time);
        plt_free(pmsg, RAM_TYPE_DATA_ON);
    }

    return ret;
}

mesh_msg_send_cause_t sensor_cadence_delay_publish(const mesh_model_info_p pmodel_info,
                                                   uint16_t property_id,
                                                   const sensor_cadence_t *cadence, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = sensor_cadence_status(pmodel_info, 0, 0, property_id, cadence, delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t sensor_cadence_publish(const mesh_model_info_p pmodel_info,
                                             uint16_t property_id,
                                             const sensor_cadence_t *cadence)
{
    return sensor_cadence_delay_publish(pmodel_info, property_id, cadence, 0);
}

static mesh_msg_send_cause_t sensor_settings_status(const mesh_model_info_p pmodel_info,
                                                    uint16_t dst, uint16_t app_key_index,
                                                    uint16_t property_id, const sensor_setting_t *settings, uint16_t num_settings,
                                                    uint32_t delay_time)
{
    mesh_msg_send_cause_t ret;
    if ((NULL == settings) || (0 == num_settings))
    {
        sensor_settings_status_t msg;
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SENSOR_SETTINGS_STATUS);
        msg.property_id = property_id;
        ret = sensor_setup_server_send(pmodel_info, dst, app_key_index, &msg,
                                       sizeof(sensor_settings_status_t), delay_time);
    }
    else
    {
        sensor_settings_status_t *pmsg;
        uint16_t msg_len = sizeof(sensor_settings_status_t) + num_settings * SETTING_PROPERTY_ID_LEN;
        pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
        if (NULL == pmsg)
        {
            return MESH_MSG_SEND_CAUSE_NO_MEMORY;
        }

        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_SETTINGS_STATUS);
        pmsg->property_id = property_id;
		// uint16_t *pid = pmsg->setting_ids;
        uint8_t *pid = (uint8_t *)pmsg->setting_ids;
		for (uint16_t i = 0; i < num_settings; ++i) {
			// *pid ++ = settings[i].setting_property_id; //avoid compile warning
			memcpy(pid,&settings[i].setting_property_id,2); //avoid crush on kr4
			// pid++; //avoid compile warning
			pid += 2;
        }
        ret = sensor_setup_server_send(pmodel_info, dst, app_key_index, pmsg, msg_len, delay_time);
        plt_free(pmsg, RAM_TYPE_DATA_ON);
    }

    return ret;
}

static mesh_msg_send_cause_t sensor_setting_status(const mesh_model_info_p pmodel_info,
                                                   uint16_t dst, uint16_t app_key_index,
                                                   uint16_t property_id, uint16_t setting_property_id,
                                                   bool set_read_only, const sensor_setting_t *setting, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret;
    if (NULL == setting)
    {
        sensor_setting_status_t msg;
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SENSOR_SETTING_STATUS);
        msg.property_id = property_id;
        msg.setting_property_id = setting_property_id;
        ret = sensor_setup_server_send(pmodel_info, dst, app_key_index, &msg,
                                       sizeof(sensor_setting_status_t), delay_time);
    }
    else
    {
        sensor_setting_status_t *pmsg;
        uint16_t msg_len = 0;
        if (!set_read_only)
        {
            msg_len = sizeof(sensor_setting_status_t) + 1 + setting->setting_raw_len;
        }
        else
        {
            msg_len = sizeof(sensor_setting_status_t) + 1;
        }
        pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
        if (NULL == pmsg)
        {
            return MESH_MSG_SEND_CAUSE_NO_MEMORY;
        }

        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_SENSOR_SETTING_STATUS);
        pmsg->property_id = property_id;
        pmsg->setting_property_id = setting_property_id;
        uint8_t *pdata = pmsg->setting;
        *pdata ++ = setting->setting_access;
        if (!set_read_only)
        {
            for (uint16_t i = 0; i < setting->setting_raw_len; ++i)
            {
                *pdata ++ = ((uint8_t *)setting->setting_raw)[i];
            }
        }
        ret = sensor_setup_server_send(pmodel_info, dst, app_key_index, pmsg, msg_len, delay_time);
        plt_free(pmsg, RAM_TYPE_DATA_ON);
    }

    return ret;

}

mesh_msg_send_cause_t sensor_setting_delay_publish(const mesh_model_info_p pmodel_info,
                                                   uint16_t property_id,
                                                   const sensor_setting_t *setting, uint32_t delay_time)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = sensor_setting_status(pmodel_info, 0, 0, property_id, setting->setting_property_id, FALSE,
                                    setting, delay_time);
    }

    return ret;
}

mesh_msg_send_cause_t sensor_setting_publish(const mesh_model_info_p pmodel_info,
                                             uint16_t property_id,
                                             const sensor_setting_t *setting)
{
    return sensor_setting_delay_publish(pmodel_info, property_id, setting, 0);
}

//RTK porting:avoid when candence length is variable
static bool sensor_check_msg(const mesh_model_info_p pmodel_info, const sensor_cadence_set_t *pmsg, uint16_t msg_len)
{
    if (!IS_SENSOR_PROPERTY_ID_VALID(pmsg->property_id))
    {
        return FALSE;
    }

    if (!IS_FAST_CADENCE_DIVISIOR_VALID(pmsg->fast_cadence_period_divisor))
    {
        return FALSE;
    }

    sensor_cadence_t *cadence = sensor_get_cadence(pmodel_info, pmsg->property_id);
    if (NULL == cadence)
    {
        return TRUE;
    }

    const uint8_t *pdata = pmsg->cadence;
    uint8_t trigger_len;
    //RTK porting:avoid when candence length is variable
	if (SENSOR_TRIGGER_TYPE_CHARACTERISTIC == pmsg->status_trigger_type) {
		trigger_len = (msg_len - sizeof(uint8_t)) / 4;
	} else {
        trigger_len = TRIGGER_DELTA_UNITLESS_LEN;
    }
    pdata += (trigger_len * 2);

    if (!IS_STATUS_MIN_INTERVAL_VALID(*pdata))
    {
        return FALSE;
    }

    return TRUE;
}

static bool sensor_setup_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_SENSOR_CADENCE_GET:
        if (pmesh_msg->msg_len == sizeof(sensor_cadence_get_t))
        {
            sensor_cadence_get_t *pmsg = (sensor_cadence_get_t *)pbuffer;
            if (IS_SENSOR_PROPERTY_ID_VALID(pmsg->property_id))
            {
                sensor_cadence_t *cadence = sensor_get_cadence(pmodel_info, pmsg->property_id);
                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                sensor_cadence_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                      pmsg->property_id, cadence, delay_rsp_time);
            }
        }
        break;
    case MESH_MSG_SENSOR_CADENCE_SET:
    case MESH_MSG_SENSOR_CADENCE_SET_UNACK:
        {
            sensor_cadence_set_t *pmsg = (sensor_cadence_set_t *)pbuffer;
            //RTK porting:avoid when candence length is variable
			uint16_t msg_len = pmesh_msg->msg_len - sizeof(sensor_cadence_set_t);
			if (sensor_check_msg(pmodel_info, pmsg, msg_len)) {
                sensor_cadence_t *cadence = sensor_get_cadence(pmodel_info, pmsg->property_id);
                if (NULL != cadence)
                {
                    uint8_t *pdata = pmsg->cadence;
                    uint8_t trigger_len;
                    cadence->fast_cadence_period_divisor = pmsg->fast_cadence_period_divisor;
                    cadence->status_trigger_type = pmsg->status_trigger_type;
                    //RTK porting:avoid when candence length is variable
					uint8_t raw_value_len;
					if (SENSOR_TRIGGER_TYPE_CHARACTERISTIC == pmsg->status_trigger_type) {
						raw_value_len = (msg_len - sizeof(uint8_t)) / 4;
						cadence->raw_value_len = raw_value_len;
                        trigger_len = cadence->raw_value_len;
					} else {
						raw_value_len = (msg_len - sizeof(uint8_t) - 2 * TRIGGER_DELTA_UNITLESS_LEN) / 2;
						cadence->raw_value_len = raw_value_len;
                        trigger_len = TRIGGER_DELTA_UNITLESS_LEN;
                    }

                    for (uint8_t i = 0; i < trigger_len; ++i)
                    {
                        ((uint8_t *)cadence->status_trigger_delta_down)[i] = *pdata ++;
                    }

                    for (uint8_t i = 0; i < trigger_len; ++i)
                    {
                        ((uint8_t *)cadence->status_trigger_delta_up)[i] = *pdata ++;
                    }

                    cadence->status_min_interval = *pdata ++;

                    for (uint8_t i = 0; i < cadence->raw_value_len; ++i)
                    {
                        ((uint8_t *)cadence->fast_cadence_low)[i] = *pdata ++;
                    }

                    for (uint8_t i = 0; i < cadence->raw_value_len; ++i)
                    {
                        ((uint8_t *)cadence->fast_cadence_high)[i] = *pdata ++;
                    }
                    if (NULL != pmodel_info->model_data_cb)
                    {
                        sensor_server_set_cadence_t set_data = {pmsg->property_id, cadence};
                        pmodel_info->model_data_cb(pmodel_info, SENSOR_SERVER_SET_CADENCE, &set_data);
                    }
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (MESH_MSG_SENSOR_CADENCE_SET == pmesh_msg->access_opcode)
                {
                    sensor_cadence_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                          pmsg->property_id, cadence, delay_rsp_time);
                }

                if (NULL != cadence)
                {
#if MODEL_ENABLE_DELAY_MSG_RSP
                    bool ack = (pmesh_msg->access_opcode == MESH_MSG_SENSOR_CADENCE_SET_UNACK) ? FALSE : TRUE;
                    generic_transition_time_t trans_time = {0, 0};
                    uint32_t delay_pub_time = delay_msg_get_trans_delay(0, trans_time, delay_rsp_time, TRUE, ack);
                    sensor_cadence_delay_publish(pmodel_info, pmsg->property_id, cadence, delay_pub_time);
#else
                    /* TODO: check if cadence is the same */
                    sensor_cadence_publish(pmodel_info, pmsg->property_id, cadence);
#endif
                }
            }
        }
        break;
    case MESH_MSG_SENSOR_SETTINGS_GET:
        if (pmesh_msg->msg_len == sizeof(sensor_settings_get_t))
        {
            sensor_settings_get_t *pmsg = (sensor_settings_get_t *)pbuffer;
            if (IS_SENSOR_PROPERTY_ID_VALID(pmsg->property_id))
            {
                sensor_setup_info_t *pinfo = pmodel_info->pargs;
                sensor_setting_t *psettings = NULL;
                uint16_t num_settings = 0;
                for (uint16_t i = 0; i < pinfo->num_sensors; ++i)
                {
                    if (pinfo->sensors[i].descriptor.property_id == pmsg->property_id)
                    {
                        psettings = pinfo->sensors[i].settings;
                        num_settings = pinfo->sensors[i].num_settings;
                        break;
                    }
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                sensor_settings_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                       pmsg->property_id, psettings, num_settings, delay_rsp_time);
            }
        }
        break;
    case MESH_MSG_SENSOR_SETTING_GET:
        if (pmesh_msg->msg_len == sizeof(sensor_setting_get_t))
        {
            sensor_setting_get_t *pmsg = (sensor_setting_get_t *)pbuffer;
            if (IS_SENSOR_PROPERTY_ID_VALID(pmsg->property_id))
            {
                sensor_setting_get_t *pmsg = (sensor_setting_get_t *)pbuffer;
                sensor_setting_t *setting = sensor_get_setting(pmodel_info, pmsg->property_id,
                                                               pmsg->setting_property_id);
                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                sensor_setting_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                      pmsg->property_id, pmsg->setting_property_id, FALSE, setting, delay_rsp_time);
            }
        }
        break;
    case MESH_MSG_SENSOR_SETTING_SET:
    case MESH_MSG_SENSOR_SETTING_SET_UNACK:
        {
            sensor_setting_set_t *pmsg = (sensor_setting_set_t *)pbuffer;
            if (IS_SENSOR_PROPERTY_ID_VALID(pmsg->property_id) &&
                IS_SENSOR_SETTING_PROPERTY_ID_VALID(pmsg->setting_property_id))
            {
                sensor_setting_t *setting = sensor_get_setting(pmodel_info, pmsg->property_id,
                                                               pmsg->setting_property_id);
                bool set_read_only = FALSE;
                if (NULL != setting)
                {
                    if (SENSOR_SETTING_ACCESS_READ_WRITE == setting->setting_access)
                    {
                        uint16_t data_len = pmesh_msg->msg_len - sizeof(sensor_setting_set_t);
                        for (uint16_t i = 0; i < data_len; ++i)
                        {
                            setting->setting_raw_len = data_len;
                            ((uint8_t *)setting->setting_raw)[i] = pmsg->setting[i];
                        }
                        if (NULL != pmodel_info->model_data_cb)
                        {
                            sensor_server_set_setting_t set_data = {pmsg->property_id, setting};
                            pmodel_info->model_data_cb(pmodel_info, SENSOR_SERVER_SET_SETTING, &set_data);
                        }
                    }
                    else
                    {
                        set_read_only = TRUE;
                    }
                }

                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                if (MESH_MSG_SENSOR_SETTING_SET == pmesh_msg->access_opcode)
                {
                    sensor_setting_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                          pmsg->property_id, pmsg->setting_property_id, set_read_only,
                                          setting, delay_rsp_time);
                }

                if (NULL != setting)
                {
#if MODEL_ENABLE_DELAY_MSG_RSP
                    bool ack = (pmesh_msg->access_opcode == MESH_MSG_SENSOR_SETTING_SET_UNACK) ? FALSE : TRUE;
                    generic_transition_time_t trans_time = {0, 0};
                    uint32_t delay_pub_time = delay_msg_get_trans_delay(0, trans_time, delay_rsp_time, TRUE, ack);
                    sensor_setting_delay_publish(pmodel_info, pmsg->property_id, setting, delay_pub_time);
#else
                    /* TODO: check if setting is the same */
                    sensor_setting_publish(pmodel_info, pmsg->property_id, setting);
#endif
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

static int32_t sensor_setup_server_publish(mesh_model_info_p pmodel_info, bool retrans)
{
	(void) retrans; //avoid compile warning
    sensor_setup_info_t *pinfo = pmodel_info->pargs;
    uint16_t property_id = 0;
    for (uint16_t i = 0; i < pinfo->num_sensors; ++i)
    {
        property_id = pinfo->sensors[i].descriptor.property_id;
        /* publish cadence */
        sensor_cadence_status(pmodel_info, 0, 0, property_id, pinfo->sensors[i].cadence, 0);
        /* publish setting */
        for (uint16_t j = 0; j < pinfo->sensors[i].num_settings; ++j)
        {
            sensor_setting_status(pmodel_info, 0, 0, property_id,
                                  pinfo->sensors[i].settings[i].setting_property_id, FALSE,
                                  &(pinfo->sensors[i].settings[i]), 0);
        }
    }

    return 0;
}

#if MESH_MODEL_ENABLE_DEINIT
static void sensor_setup_server_deinit(mesh_model_info_t *pmodel_info)
{
    if (pmodel_info->model_receive == sensor_setup_server_receive)
    {
        plt_free(pmodel_info->pargs, RAM_TYPE_DATA_ON);
        pmodel_info->pargs = NULL;
        pmodel_info->model_receive = NULL;
    }
}
#endif

bool sensor_setup_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_SENSOR_SETUP_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->pargs = plt_malloc(sizeof(sensor_setup_info_t), RAM_TYPE_DATA_ON);
        if (NULL == pmodel_info->pargs)
        {
            printe("sensor_setup_server_reg: fail to allocate memory for the new model extension data!");
            return FALSE;
        }
        memset(pmodel_info->pargs, 0, sizeof(sensor_setup_info_t));

        pmodel_info->model_receive = sensor_setup_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("sensor_setup_server_reg: missing model data process callback!");
        }
#if MESH_MODEL_ENABLE_DEINIT
        pmodel_info->model_deinit = sensor_setup_server_deinit;
#endif
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = sensor_setup_server_publish;
    }

    return mesh_model_reg(element_index, pmodel_info);
}

