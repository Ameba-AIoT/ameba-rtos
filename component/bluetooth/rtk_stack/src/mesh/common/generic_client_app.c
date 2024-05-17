/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     generic_client_app.c
  * @brief    Source file for generic models application.
  * @details  User command interfaces.
  * @author   hector_huang
  * @date     2018-7-24
  * @version  v1.0
  * *************************************************************************************
  */

#include "generic_client_app.h"

mesh_model_info_t model_gdtt_client;
mesh_model_info_t model_goo_client;
mesh_model_info_t model_gl_client;
mesh_model_info_t model_gpoo_client;
mesh_model_info_t model_gpl_client;
mesh_model_info_t model_gb_client;
mesh_model_info_t model_health_client;
mesh_model_info_t model_time_client;
mesh_model_info_t model_scene_client;
mesh_model_info_t model_sensor_client;
mesh_model_info_t model_scheduler_client;
mesh_model_info_t model_generic_location_client;
mesh_model_info_t model_gp_client;

static int32_t generic_default_transition_time_client_data(const mesh_model_info_p pmodel_info,
                                                           uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_STATUS:
        {
            generic_default_transition_time_client_status_t *pdata = pargs;
            data_uart_debug("gdtt client receive: src = %d, steps = %d, resolution = %d\r\n",
                            pdata->src, pdata->trans_time.num_steps, pdata->trans_time.step_resolution);
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t generic_on_off_client_data(const mesh_model_info_p pmodel_info,
                                          uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case GENERIC_ON_OFF_CLIENT_STATUS:
        {
            generic_on_off_client_status_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("goo client receive: src = %d, present = %d, target = %d, remain time = step(%d), \
resolution(%d)\r\n", pdata->src, pdata->present_on_off, pdata->target_on_off,
                                pdata->remaining_time.num_steps, pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("goo client receive: src = %d, present = %d\r\n", pdata->src,
                                pdata->present_on_off);
            }
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t generic_level_client_data(const mesh_model_info_p pmodel_info,
                                         uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case GENERIC_LEVEL_CLIENT_STATUS:
        {
            generic_level_client_status_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("genric level client receive: src = %d, present = %d, target = %d, remain time = step(%d), \
resolution(%d)\r\n", pdata->src, pdata->present_level, pdata->target_level,
                                pdata->remaining_time.num_steps,
                                pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("generic level client receive: src = %d, present = %d\r\n", pdata->src,
                                pdata->present_level);
            }
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t generic_battery_client_data(const mesh_model_info_p pmodel_info,
                                           uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case GENERIC_BATTERY_CLIENT_STATUS:
        {
            generic_battery_client_status_t *pdata = pargs;
            data_uart_debug("genric battery client receive: src = %d, battery level = %d, time to discharge = %d, \
time to charge = %d, presence = %d, indicator = %d, charging = %d, serviceability = %d\r\n",
                            pdata->src,
                            pdata->battery_level,
                            pdata->time_to_discharge, pdata->time_to_charge,
                            pdata->flags.presence, pdata->flags.indicator, pdata->flags.charging,
                            pdata->flags.serviceability);
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t generic_power_on_off_client_data(const mesh_model_info_p pmodel_info,
                                                uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case GENERIC_POWER_ON_OFF_CLIENT_STATUS:
        {
            generic_power_on_off_client_status_t *pdata = pargs;
            data_uart_debug("generic power on/off receive: src = %d, power = %d\r\n", pdata->src,
                            pdata->on_power_up);
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t generic_power_level_client_data(const mesh_model_info_p pmodel_info,
                                               uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case GENERIC_POWER_LEVEL_CLIENT_STATUS:
        {
            generic_power_level_client_status_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("genric power level client receive: src = %d, present = %d, target = %d, remain time = step(%d), \
resolution(%d)\r\n", pdata->src, pdata->present_power, pdata->target_power,
                                pdata->remaining_time.num_steps,
                                pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("generic power level client receive: src = %d, present = %d\r\n", pdata->src,
                                pdata->present_power);
            }
        }
        break;
    case GENERIC_POWER_LEVEL_CLIENT_STATUS_LAST:
        {
            generic_power_level_client_status_simple_t *pdata = pargs;
            data_uart_debug("generic power level client receive: power = %d\r\n", pdata->power);
        }
        break;
    case GENERIC_POWER_LEVEL_CLIENT_STATUS_DEFAULT:
        {
            generic_power_level_client_status_simple_t *pdata = pargs;
            data_uart_debug("generic power level client receive: power = %d\r\n", pdata->power);
        }
        break;
    case GENERIC_POWER_LEVEL_CLIENT_STATUS_RANGE:
        {
            generic_power_level_client_status_range_t *pdata = pargs;
            data_uart_debug("generic power level client receive: status = %d, min = %d, max = %d\r\n",
                            pdata->stat, pdata->range_min, pdata->range_max);
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t health_client_data(const mesh_model_info_p pmodel_info,
                                  uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case HEALTH_CLIENT_STATUS_CURRENT:
    case HEALTH_CLIENT_STATUS_REGISTERED:
        {
            health_client_status_t *pdata = pargs;
            data_uart_debug("health client receive: src = %d, test id = %d, company id = %d, fault = ",
                            pdata->src,
                            pdata->test_id, pdata->company_id);
            for (int i = 0; i < pdata->fault_array_len; ++i)
            {
                data_uart_debug("%d ", pdata->fault_array[i]);
            }
            data_uart_debug("\r\n");
        }
        break;
    case HEALTH_CLIENT_STATUS_PERIOD:
        {
            health_client_status_period_t *pdata = pargs;
            data_uart_debug("health client receive: fast period divisor = %d\r\n", pdata->fast_period_divisor);
        }
        break;
    case HEALTH_CLIENT_STATUS_ATTENTION:
        {
            health_client_status_attention_t *pdata = pargs;
            data_uart_debug("health client receive: attention = %d\r\n", pdata->attention);
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t time_client_data(const mesh_model_info_p pmodel_info,
                                uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case TIME_CLIENT_STATUS:
        {
            time_client_status_t *pdata = pargs;
            data_uart_debug("time client receive: src = %d, tai_seconds = 0x%02x%02x%02x%02x%02x, \
subsecond = %d, uncertainty = %d, time_authority = %d, tai_utc_delta = %d, \
time_zone_offset = %d\r\n", pdata->src, pdata->tai_time.tai_seconds[4],
                            pdata->tai_time.tai_seconds[3], pdata->tai_time.tai_seconds[2],
                            pdata->tai_time.tai_seconds[1], pdata->tai_time.tai_seconds[0], pdata->tai_time.subsecond,
                            pdata->tai_time.uncertainty,
                            pdata->tai_time.time_authority, pdata->tai_time.tai_utc_delta, pdata->tai_time.time_zone_offset);
        }
        break;
    case TIME_CLIENT_STATUS_ZONE:
        {
            time_client_status_zone_t *pdata = pargs;
            data_uart_debug("time client receive: time_zone_offset_current = %d, \
time_zone_offset_new = %d, tai_of_zone_change = 0x%02x%02x%02x%02x%02x\r\n",
                            pdata->time_zone_offset_current, pdata->time_zone_offset_new, pdata->tai_of_zone_change[4],
                            pdata->tai_of_zone_change[3], pdata->tai_of_zone_change[2], pdata->tai_of_zone_change[1],
                            pdata->tai_of_zone_change[0]);
        }
        break;
    case TIME_CLIENT_STATUS_TAI_UTC_DELTA:
        {
            time_client_status_tai_utc_delta_t *pdata = pargs;
            data_uart_debug("time client receive: tai_utc_delta_current = %d, tai_utc_delta_new = %d, \
tai_of_delta_change = 0x%02x%02x%02x%02x%02x\r\n", pdata->tai_utc_delta_current,
                            pdata->tai_utc_delta_new,
                            pdata->tai_of_delta_change[4], pdata->tai_of_delta_change[3], pdata->tai_of_delta_change[2],
                            pdata->tai_of_delta_change[1], pdata->tai_of_delta_change[0]);
        }
        break;
    case TIME_CLIENT_STATUS_ROLE:
        {
            time_client_status_role_t *pdata = pargs;
            data_uart_debug("time client receive: role = %d\r\n", pdata->role);
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t scene_client_data(const mesh_model_info_p pmodel_info,
                                 uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case SCENE_CLIENT_STATUS:
        {
            scene_client_status_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("scene client receive: src = %d, status = %d, current = %d, target = %d, \
remain time = step(%d), resolution(%d)\r\n", pdata->src, pdata->status, pdata->current_scene,
                                pdata->target_scene,
                                pdata->remaining_time.num_steps, pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("scene client receive: status = %d, current = %d\r\n", pdata->status,
                                pdata->current_scene);
            }
        }
        break;
    case SCENE_CLIENT_REGISTER_STATUS:
        {
            scene_client_register_status_t *pdata = pargs;
            data_uart_debug("scene client receive: status = %d, current scene = %d, scene = ", pdata->status,
                            pdata->current_scene);
            for (int i = 0; i < pdata->scene_array_len; ++i)
            {
                data_uart_debug("%d ", pdata->scene_array[i]);
            }
            data_uart_debug("\r\n");
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t sensor_client_data(const mesh_model_info_p pmodel_info,
                                  uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case SENSOR_CLIENT_STATUS_DESCRIPTOR:
        {
            sensor_client_status_descriptor_t *pdata = pargs;
            if (0 == pdata->num_descriptors)
            {
                data_uart_debug("sensor client receive: property id = %d\r\n", pdata->property_id);
            }
            else
            {
                for (uint16_t i = 0; i < pdata->num_descriptors; ++i)
                {
                    data_uart_debug("sensor client receive: property id = %d, positive rolerance = %d, negative tolerance = %d, sampling function = %d, measurement period = %d, updata_interval = %d\r\n",
                                    pdata->descriptors[i].property_id, pdata->descriptors[i].positive_tolerance,
                                    pdata->descriptors[i].negative_tolerance, pdata->descriptors[i].sampling_function,
                                    pdata->descriptors[i].measurement_period, pdata->descriptors[i].update_interval);
                }
            }
        }
        break;
    case SENSOR_CLIENT_STATUS_CADENCE:
        {
            sensor_client_status_cadence_t *pdata = pargs;
            data_uart_debug("sensor client receive: src = %d, property id = %d, divisor = %d, trigger type = %d, ",
                            pdata->src,
                            pdata->property_id,
                            pdata->cadence->fast_cadence_period_divisor, pdata->cadence->status_trigger_type);
            if (SENSOR_TRIGGER_TYPE_CHARACTERISTIC == pdata->cadence->status_trigger_type)
            {
                data_uart_debug("trigger delta down = ");
                data_uart_dump((uint8_t *)pdata->cadence->status_trigger_delta_down, pdata->cadence->raw_value_len);
                data_uart_debug(", ");
                data_uart_debug("trigger delta up = ");
                data_uart_dump((uint8_t *)pdata->cadence->status_trigger_delta_up, pdata->cadence->raw_value_len);
                data_uart_debug(", ");
            }
            else
            {
                data_uart_debug("trigger delta down = ");
                data_uart_dump((uint8_t *)pdata->cadence->status_trigger_delta_down, 2);
                data_uart_debug(", ");
                data_uart_debug("trigger delta up = ");
                data_uart_dump((uint8_t *)pdata->cadence->status_trigger_delta_up, 2);
                data_uart_debug(", ");
            }

            data_uart_debug("min interval = %d, ", pdata->cadence->status_min_interval);

            data_uart_debug("fast cadence low = ");
            data_uart_dump((uint8_t *)pdata->cadence->fast_cadence_low, pdata->cadence->raw_value_len);
            data_uart_debug(", ");
            data_uart_debug("fast cadence high = ");
            data_uart_dump((uint8_t *)pdata->cadence->fast_cadence_high, pdata->cadence->raw_value_len);
        }
        break;
    case SENSOR_CLIENT_STATUS_SETTINGS:
        {
            sensor_client_status_settings_t *pdata = pargs;
            data_uart_debug("sensor client receive: property id = %d, setting ids = ", pdata->property_id);
            for (uint16_t i = 0; i < pdata->num_ids; ++i)
            {
                data_uart_debug("0x%x ", pdata->setting_ids[i]);
            }
            data_uart_debug("\r\n");
        }
        break;
    case SENSOR_CLIENT_STATUS_SETTING:
        {
            sensor_client_status_setting_t *pdata = pargs;
            data_uart_debug("sensor client receive: src = %d, property id = %d, setting property id = %d",
                            pdata->src,
                            pdata->property_id,
                            pdata->setting_property_id);
            if (NULL != pdata->setting)
            {
                data_uart_debug(", access = %d", pdata->setting->setting_access);
                if (pdata->setting->setting_raw_len > 0)
                {
                    data_uart_debug(", setting raw = ");
                    data_uart_dump((uint8_t *)pdata->setting->setting_raw, pdata->setting->setting_raw_len);
                }
            }
            data_uart_debug("\r\n");
        }
        break;
    case SENSOR_CLIENT_STATUS:
        {
            sensor_client_status_t *pdata = pargs;
            data_uart_debug("sensor client receive: marshalled sensor data src = %d, ", pdata->src);
            data_uart_dump((uint8_t *)pdata->marshalled_sensor_data, pdata->marshalled_sensor_data_len);
            data_uart_debug("\r\n");
        }
        break;
    case SENSOR_CLIENT_STATUS_COLUMN:
        {
            sensor_client_status_column_t *pdata = pargs;
            data_uart_debug("sensor client receive: property id = %d, column raw data = ", pdata->property_id);
            data_uart_dump((uint8_t *)pdata->column_raw_value, pdata->column_raw_value_len);
        }
        break;
    case SENSOR_CLIENT_STATUS_SERIES:
        {
            sensor_client_status_series_t *pdata = pargs;
            data_uart_debug("sensor client receive: property id = %d, series raw data = ", pdata->property_id);
            data_uart_dump((uint8_t *)pdata->series_raw_value, pdata->series_raw_value_len);
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t scheduler_client_data(const mesh_model_info_p pmodel_info,
                                     uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case SCHEDULER_CLIENT_STATUS:
        {
            scheduler_client_status_t *pdata = pargs;
            data_uart_debug("scheduler client receive: schedulers = %d\r\n", pdata->schedulers);
        }
        break;
    case SCHEDULER_CLIENT_STATUS_ACTION:
        {
            scheduler_client_status_action_t *pdata = pargs;
            data_uart_debug("scheduler client receive: src = %d, index = %d, year = %d, month = %d, day = %d, hour = %d, minute = %d, second = %d, \
day_of_week = %d, action = %d, num_steps = %d, step_resolution = %d, scene_number = %d\r\n",
                            pdata->src,
                            pdata->scheduler.index, pdata->scheduler.year, pdata->scheduler.month, pdata->scheduler.day,
                            pdata->scheduler.hour, pdata->scheduler.minute, pdata->scheduler.second,
                            pdata->scheduler.day_of_week, pdata->scheduler.action, pdata->scheduler.num_steps,
                            pdata->scheduler.step_resolution, pdata->scheduler.scene_number);
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t generic_location_client_data(const mesh_model_info_p pmodel_info,
                                            uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case GENERIC_LOCATION_CLIENT_STATUS_GLOBAL:
        {
            generic_location_client_status_global_t *pdata = pargs;
            data_uart_debug("generic location client receive: src = %d, latitude = %d, longitude = %d, altitude = %d\r\n",
                            pdata->src,
                            pdata->global.global_latitude, pdata->global.global_longitude, pdata->global.global_altitude);
        }
        break;
    case SCHEDULER_CLIENT_STATUS_ACTION:
        {
            generic_location_client_status_local_t *pdata = pargs;
            data_uart_debug("generic location client receive: src = %d, north = %d, east = %d, altitude = %d, floor = %d, stationary = %d, updata time = %d, precision = %d\r\n",
                            pdata->src, pdata->local.local_north, pdata->local.local_east, pdata->local.local_altitude,
                            pdata->local.floor_num,
                            pdata->local.uncertainty.stationary, pdata->local.uncertainty.update_time,
                            pdata->local.uncertainty.precision);
        }
        break;
    default:
        break;
    }

    return 0;

}

static int32_t generic_property_client_data(const mesh_model_info_p pmodel_info,
                                            uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case GENERIC_USER_PROPERIES_CLIENT_STATUS:
        {
            generic_properties_client_status_t *pdata = pargs;
            data_uart_debug("generic property client receive: src = %d", pdata->src);
            if (pdata->num_ids > 0)
            {
                data_uart_debug(", user property ids = ");
                for (uint8_t i = 0; i < pdata->num_ids; ++i)
                {
                    data_uart_debug("%x ", pdata->pproperty_ids[i]);
                }
            }
            data_uart_debug("\r\n");
        }
        break;
    case GENERIC_USER_PROPERTY_CLIENT_STATUS:
        {
            generic_property_client_status_t *pdata = pargs;
            data_uart_debug("generic property client receive: src = %d,  property id = 0x%x, property access = %d",
                            pdata->src, pdata->property_id, pdata->property_access);
            if (pdata->value_len > 0)
            {
                data_uart_debug(", user property values = ");
                for (uint8_t i = 0; i < pdata->value_len; ++i)
                {
                    data_uart_debug("%d ", pdata->pproperty_value[i]);
                }
            }
            data_uart_debug("\r\n");
        }
        break;
    case GENERIC_ADMIN_PROPERTIES_CLIENT_STATUS:
        {
            generic_properties_client_status_t *pdata = pargs;
            data_uart_debug("generic property client receive: src = %d", pdata->src);
            if (pdata->num_ids > 0)
            {
                data_uart_debug(", admin property ids = ");
                for (uint8_t i = 0; i < pdata->num_ids; ++i)
                {
                    data_uart_debug("%x ", pdata->pproperty_ids[i]);
                }
            }
            data_uart_debug("\r\n");
        }
        break;
    case GENERIC_ADMIN_PROPERTY_CLIENT_STATUS:
        {
            generic_property_client_status_t *pdata = pargs;
            data_uart_debug("generic property client receive: src = %d,  property id = 0x%x, property access = %d",
                            pdata->src, pdata->property_id, pdata->property_access);
            if (pdata->value_len > 0)
            {
                data_uart_debug(", admin property values = ");
                for (uint8_t i = 0; i < pdata->value_len; ++i)
                {
                    data_uart_debug("%d ", pdata->pproperty_value[i]);
                }
            }
            data_uart_debug("\r\n");
        }
        break;
    case GENERIC_MANUFACTURER_PROPERTIES_CLIENT_STATUS:
        {
            generic_properties_client_status_t *pdata = pargs;
            data_uart_debug("generic property client receive: src = %d", pdata->src);
            if (pdata->num_ids > 0)
            {
                data_uart_debug(", manufacturer property ids = ");
                for (uint8_t i = 0; i < pdata->num_ids; ++i)
                {
                    data_uart_debug("%4x ", pdata->pproperty_ids[i]);
                }
            }
            data_uart_debug("\r\n");
        }
        break;
    case GENERIC_MANUFACTURER_PROPERTY_CLIENT_STATUS:
        {
            generic_property_client_status_t *pdata = pargs;
            data_uart_debug("generic property client receive: src = %d,  property id = 0x%x, property access = %d",
                            pdata->src, pdata->property_id, pdata->property_access);
            if (pdata->value_len > 0)
            {
                data_uart_debug(", manufacturer property values = ");
                for (uint8_t i = 0; i < pdata->value_len; ++i)
                {
                    data_uart_debug("%d ", pdata->pproperty_value[i]);
                }
            }
            data_uart_debug("\r\n");
        }
        break;
    case GENERIC_CLIENT_PROPERTIES_CLIENT_STATUS:
        {
            generic_properties_client_status_t *pdata = pargs;
            data_uart_debug("generic property client receive: src = %d", pdata->src);
            if (pdata->num_ids > 0)
            {
                data_uart_debug(", client property ids = ");
                for (uint8_t i = 0; i < pdata->num_ids; ++i)
                {
                    data_uart_debug("%x ", pdata->pproperty_ids[i]);
                }
            }
            data_uart_debug("\r\n");
        }
        break;
    default:
        break;
    }

    return 0;
}

void generic_client_models_init(void)
{
    model_gdtt_client.model_data_cb = generic_default_transition_time_client_data;
    generic_default_transition_time_client_reg(0, &model_gdtt_client);

    model_goo_client.model_data_cb = generic_on_off_client_data;
    generic_on_off_client_reg(0, &model_goo_client);

    model_gl_client.model_data_cb = generic_level_client_data;
    generic_level_client_reg(0, &model_gl_client);

    model_gpoo_client.model_data_cb = generic_power_on_off_client_data;
    generic_power_on_off_client_reg(0, &model_gpoo_client);

    model_gpl_client.model_data_cb = generic_power_level_client_data;
    generic_power_level_client_reg(0, &model_gpl_client);

    model_gb_client.model_data_cb = generic_battery_client_data;
    generic_battery_client_reg(0, &model_gb_client);

    model_health_client.model_data_cb = health_client_data;
    health_client_reg(0, &model_health_client);

    model_time_client.model_data_cb = time_client_data;
    time_client_reg(1, &model_time_client);

    model_scene_client.model_data_cb = scene_client_data;
    scene_client_reg(0, &model_scene_client);

    model_sensor_client.model_data_cb = sensor_client_data;
    sensor_client_reg(0, &model_sensor_client);

    model_scheduler_client.model_data_cb = scheduler_client_data;
    scheduler_client_reg(0, &model_scheduler_client);

    model_generic_location_client.model_data_cb = generic_location_client_data;
    generic_location_client_reg(0, &model_generic_location_client);

    model_gp_client.model_data_cb = generic_property_client_data;
    generic_property_client_reg(0, &model_gp_client);
}

