/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     light_client_app.c
  * @brief    Source file for light client application.
  * @details  User command interfaces.
  * @author   bill
  * @date     2018-1-4
  * @version  v1.0
  * *************************************************************************************
  */

#include "light_client_app.h"

mesh_model_info_t light_cwrgb_client;
mesh_model_info_t light_lightness_client;
mesh_model_info_t light_ctl_client;
mesh_model_info_t light_hsl_client;
mesh_model_info_t light_xyl_client;
mesh_model_info_t light_lc_client;

static bool light_cwrgb_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_LIGHT_CWRGB_STAT:
        if (pmesh_msg->msg_len == sizeof(light_cwrgb_stat_t))
        {
            light_cwrgb_stat_t *pmsg = (light_cwrgb_stat_t *)pbuffer;
            data_uart_debug("light 0x%04x cwrgb %d-%d-%d-%d-%d\r\n", pmesh_msg->src, pmsg->cwrgb[0],
                            pmsg->cwrgb[1], pmsg->cwrgb[2], pmsg->cwrgb[3], pmsg->cwrgb[4]);
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

static int32_t light_lightness_client_data(const mesh_model_info_p pmodel_info,
                                           uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case LIGHT_LIGHTNESS_CLIENT_STATUS:
        {
            light_lightness_client_status_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("light lightness client receive: src %d, present %d, target %d, remain time step(%d) resolution(%d)\r\n",
                                pdata->src,
                                pdata->present_lightness,
                                pdata->target_lightness,
                                pdata->remaining_time.num_steps,
                                pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("light lightness client receive: src %d, present %d\r\n", pdata->src,
                                pdata->present_lightness);
            }
        }
        break;
    case LIGHT_LIGHTNESS_CLIENT_STATUS_LINEAR:
        {
            light_lightness_client_status_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("light lightness client receive: src %d, present %d, target %d, remain time step(%d) resolution(%d)\r\n",
                                pdata->src,
                                pdata->present_lightness,
                                pdata->target_lightness,
                                pdata->remaining_time.num_steps,
                                pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("light lightness client receive: src %d, present %d\r\n", pdata->src,
                                pdata->present_lightness);
            }
        }
        break;
    case LIGHT_LIGHTNESS_CLIENT_STATUS_LAST:
        {
            light_lightness_client_status_last_t *pdata = pargs;
            data_uart_debug("light lightness client receive: src %d, lightness %d\r\n", pdata->src,
                            pdata->lightness);
        }
        break;
    case LIGHT_LIGHTNESS_CLIENT_STATUS_DEFAULT:
        {
            light_lightness_client_status_default_t *pdata = pargs;
            data_uart_debug("light lightness client receive: src %d, lightness %d\r\n", pdata->src,
                            pdata->lightness);
        }
        break;
    case LIGHT_LIGHTNESS_CLIENT_STATUS_RANGE:
        {
            light_lightness_client_status_range_t *pdata = pargs;
            data_uart_debug("light lightness client receive: src %d, status %d, min %d, max %d\r\n",
                            pdata->src, pdata->status, pdata->range_min, pdata->range_max);
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t light_ctl_client_data(const mesh_model_info_p pmodel_info,
                                     uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case LIGHT_CTL_CLIENT_STATUS:
        {
            light_ctl_client_status_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("light ctl client receive: src %d, present lightness %d, present temperature %d, target lightness %d, target temperature %d, remain time step(%d) resolution(%d)\r\n",
                                pdata->src,
                                pdata->present_lightness,
                                pdata->present_temperature,
                                pdata->target_lightness, pdata->target_temperature,
                                pdata->remaining_time.num_steps,
                                pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("light ctl client receive: src %d, present lightness %d, present temperature %d\r\n",
                                pdata->src, pdata->present_lightness, pdata->present_temperature);
            }
        }
        break;
    case LIGHT_CTL_CLIENT_STATUS_TEMPERATURE:
        {
            light_ctl_client_status_temperature_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("light ctl client receive: src %d, present temperature %d, present delta_uv %d, target temperature %d, target delta_uv %d, remain time step(%d), resolution(%d)\r\n",
                                pdata->src,
                                pdata->present_temperature,
                                pdata->present_delta_uv,
                                pdata->target_temperature, pdata->target_delta_uv,
                                pdata->remaining_time.num_steps,
                                pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("light ctl client receive: src %d, present temperature %d, present delta_uv %d\r\n",
                                pdata->src, pdata->present_temperature, pdata->present_delta_uv);
            }
        }
        break;
    case LIGHT_CTL_CLIENT_STATUS_DEFAULT:
        {
            light_ctl_client_status_default_t *pdata = pargs;
            data_uart_debug("light ctl client receive: src %d, lightness %d, temperature %d, delta_uv %d\r\n",
                            pdata->src, pdata->lightness, pdata->temperature, pdata->delta_uv);
        }
        break;
    case LIGHT_CTL_CLIENT_STATUS_TEMPERATURE_RANGE:
        {
            light_ctl_client_status_temperature_range_t *pdata = pargs;
            data_uart_debug("light ctl client receive: src %d, status %d, min %d, max %d\r\n",
                            pdata->src, pdata->status, pdata->range_min, pdata->range_max);
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t light_hsl_client_data(const mesh_model_info_p pmodel_info,
                                     uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case LIGHT_HSL_CLIENT_STATUS:
        {
            light_hsl_client_status_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("light hsl client receive: src %d, present lightness %d, present hue %d, present saturation %d, remain time step(%d) resolution(%d)\r\n",
                                pdata->src,
                                pdata->lightness,
                                pdata->hue, pdata->saturation,
                                pdata->remaining_time.num_steps,
                                pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("light hsl client receive: src %d, present lightness %d, present hue %d, present saturation %d\r\n",
                                pdata->src, pdata->lightness, pdata->hue, pdata->saturation);
            }
        }
        break;
    case LIGHT_HSL_CLIENT_STATUS_TARGET:
        {
            light_hsl_client_status_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("light hsl client receive: src %d, target lightness %d, target hue %d, target saturation %d, remain time step(%d) resolution(%d)\r\n",
                                pdata->src,
                                pdata->lightness,
                                pdata->hue, pdata->saturation,
                                pdata->remaining_time.num_steps,
                                pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("light hsl client receive: src %d, target lightness %d, target hue %d, target saturation %d\r\n",
                                pdata->src, pdata->lightness, pdata->hue, pdata->saturation);
            }
        }
        break;
    case LIGHT_HSL_CLIENT_STATUS_HUE:
        {
            light_hsl_client_status_hue_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("light hsl client receive: src %d, present hue %d, target hue %d, remain time step(%d) resolution(%d)\r\n",
                                pdata->src,
                                pdata->present_hue,
                                pdata->target_hue,
                                pdata->remaining_time.num_steps,
                                pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("light hsl client receive: src %d, present hue %d\r\n",
                                pdata->src, pdata->present_hue);
            }
        }
        break;
    case LIGHT_HSL_CLIENT_STATUS_SATURATION:
        {
            light_hsl_client_status_saturation_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("light hsl client receive: src %d, present saturation %d, target saturation %d, remain time step(%d) resolution(%d)\r\n",
                                pdata->src,
                                pdata->present_saturation,
                                pdata->target_saturation,
                                pdata->remaining_time.num_steps,
                                pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("light hsl client receive: src %d, present saturation %d\r\n",
                                pdata->src, pdata->present_saturation);
            }
        }
        break;
    case LIGHT_HSL_CLIENT_STATUS_DEFAULT:
        {
            light_hsl_client_status_default_t *pdata = pargs;
            data_uart_debug("light hsl client receive: src %d, lightness %d, hue %d, saturation %d\r\n",
                            pdata->src, pdata->lightness, pdata->hue, pdata->saturation);
        }
        break;
    case LIGHT_HSL_CLIENT_STATUS_RANGE:
        {
            light_hsl_client_status_range_t *pdata = pargs;
            data_uart_debug("light hsl client receive: src %d, status %d, hue min %d, hue max %d, saturation min %d, saturation max %d\r\n",
                            pdata->src, pdata->status, pdata->hue_range_min, pdata->hue_range_max,
                            pdata->saturation_range_min, pdata->saturation_range_max);
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t light_xyl_client_data(const mesh_model_info_p pmodel_info,
                                     uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case LIGHT_XYL_CLIENT_STATUS:
        {
            light_xyl_client_status_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("light xyl client receive: src %d, present lightness %d, present xyl_x %d, present xyl_y %d, remain time step(%d) resolution(%d)\r\n",
                                pdata->src,
                                pdata->xyl.xyl_lightness,
                                pdata->xyl.xyl_x, pdata->xyl.xyl_y,
                                pdata->remaining_time.num_steps,
                                pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("light xyl client receive: src %d, present lightness %d, present xyl_x %d, present xyl_y %d\r\n",
                                pdata->src, pdata->xyl.xyl_lightness, pdata->xyl.xyl_x, pdata->xyl.xyl_y);
            }
        }
        break;
    case LIGHT_XYL_CLIENT_STATUS_TARGET:
        {
            light_xyl_client_status_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("light xyl client receive: src %d, target lightness %d, target xyl_x %d, target xyl_y %d, remain time step(%d) resolution(%d)\r\n",
                                pdata->src,
                                pdata->xyl.xyl_lightness,
                                pdata->xyl.xyl_x, pdata->xyl.xyl_y,
                                pdata->remaining_time.num_steps,
                                pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("light xyl client receive: src %d, target lightness %d, target xyl_x %d, target xyl_y %d\r\n",
                                pdata->src, pdata->xyl.xyl_lightness, pdata->xyl.xyl_x, pdata->xyl.xyl_y);
            }
        }
        break;
    case LIGHT_XYL_CLIENT_STATUS_DEFAULT:
        {
            light_xyl_client_status_default_t *pdata = pargs;
            data_uart_debug("light xyl client receive: src %d, lightness %d, xyl_x %d, xyl_y %d\r\n",
                            pdata->src, pdata->xyl.xyl_lightness, pdata->xyl.xyl_x, pdata->xyl.xyl_y);
        }
        break;
    case LIGHT_XYL_CLIENT_STATUS_RANGE:
        {
            light_xyl_client_status_range_t *pdata = pargs;
            data_uart_debug("light xyl client receive: src %d, status %d, xyl_x min %d, xyl_x max %d, xyl_y min %d, xyl_y max %d\r\n",
                            pdata->src, pdata->status, pdata->xyl_x_range_min, pdata->xyl_x_range_max,
                            pdata->xyl_y_range_min, pdata->xyl_y_range_max);
        }
        break;
    default:
        break;
    }

    return 0;
}

static int32_t light_lc_client_data(const mesh_model_info_p pmodel_info,
                                    uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case LIGHT_LC_CLIENT_MODE_STATUS:
        {
            light_lc_client_mode_status_t *pdata = pargs;
            data_uart_debug("light lc client receive: src %d, mode %d\r\n", pdata->src, pdata->mode);
        }
        break;
    case LIGHT_LC_CLIENT_OM_STATUS:
        {
            light_lc_client_om_status_t *pdata = pargs;
            data_uart_debug("light lc client receive: src %d, occupancy mode %d\r\n", pdata->src, pdata->mode);
        }
        break;
    case LIGHT_LC_CLIENT_LIGHT_ON_OFF_STATUS:
        {
            light_lc_client_light_on_off_status_t *pdata = pargs;
            if (pdata->optional)
            {
                data_uart_debug("light lc client receive: src %d, present on off %d, target on off %d, remain time step(%d) resolution(%d)\r\n",
                                pdata->src,
                                pdata->present_on_off,
                                pdata->target_on_off,
                                pdata->remaining_time.num_steps,
                                pdata->remaining_time.step_resolution);
            }
            else
            {
                data_uart_debug("light lc client receive: src %d, present on off %d\r\n",
                                pdata->src, pdata->present_on_off);
            }
        }
        break;
    case LIGHT_LC_CLIENT_PROPERTY_STATUS:
        {
            light_lc_client_property_status_t *pdata = pargs;
            data_uart_debug("light lc client receive: src %d, property id 0x%x, property value %d\r\n",
                            pdata->src, pdata->property_id, pdata->property_value);
        }
        break;
    default:
        break;
    }

    return 0;
}
void light_client_models_init(void)
{
    light_cwrgb_client.model_receive = light_cwrgb_client_receive;
    light_cwrgb_client_reg(0, &light_cwrgb_client);

    light_lightness_client.model_data_cb = light_lightness_client_data;
    light_lightness_client_reg(0, &light_lightness_client);

    light_ctl_client.model_data_cb = light_ctl_client_data;
    light_ctl_client_reg(0, &light_ctl_client);

    light_hsl_client.model_data_cb = light_hsl_client_data;
    light_hsl_client_reg(0, &light_hsl_client);

    light_xyl_client.model_data_cb = light_xyl_client_data;
    light_xyl_client_reg(0, &light_xyl_client);

    light_lc_client.model_data_cb = light_lc_client_data;
    light_lc_client_reg(0, &light_lc_client);
}
