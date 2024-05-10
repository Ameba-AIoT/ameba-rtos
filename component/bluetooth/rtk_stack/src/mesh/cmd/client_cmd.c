/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     client_cmd.c
  * @brief    Source file for mesh client model cmd.
  * @details  User command interfaces.
  * @author   bill
  * @date     2017-3-31
  * @version  v1.0
  * *************************************************************************************
  */

/* Add Includes here */
#include <string.h>
#include "trace.h"
#include "app_msg.h"
#include "gap_wrapper.h"
#include "client_cmd.h"
#include "mesh_api.h"
#include "health.h"
#include "generic_client_app.h"
#include "light_client_app.h"


/**
 * @brief configuration model parameter get command
 * @param pparse_value - parsed parameters
 * @return command execute status
 */
user_cmd_parse_result_t user_cmd_cmg(user_cmd_parse_value_t *pparse_value)
{
    uint16_t dst;
    uint8_t model;
    if (pparse_value->para_count < 2)
    {
        return USER_CMD_RESULT_WRONG_NUM_OF_PARAMETERS;
    }
    dst = pparse_value->dw_parameter[0];
    model = pparse_value->dw_parameter[1];
    user_cmd_parse_result_t ret = USER_CMD_RESULT_OK;
    switch (model)
    {
    case 0:
        cfg_beacon_get(dst);
        break;
    case 1:
        cfg_compo_data_get(dst, pparse_value->dw_parameter[2]);
        break;
    case 2:
        cfg_default_ttl_get(dst);
        break;
    case 3:
        cfg_proxy_get(dst);
        break;
    case 4:
        cfg_frnd_get(dst);
        break;
    case 5:
        cfg_relay_get(dst);
        break;
    case 6:
        cfg_model_pub_get(dst, pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);
        break;
    case 7:
        cfg_model_sub_get(dst, pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);
        break;
    case 8:
        cfg_net_key_get(dst);
        break;
    case 9:
        cfg_app_key_get(dst, pparse_value->dw_parameter[2]);
        break;
    case 10:
        cfg_model_app_get(dst,  pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);
        break;
    case 11:
        cfg_node_identity_get(dst, pparse_value->dw_parameter[2]);
        break;
    case 13:
        cfg_hb_pub_get(dst);
        break;
    case 14:
        cfg_hb_sub_get(dst);
        break;
    case 15:
        cfg_net_transmit_get(dst);
        break;
    case 16:
        cfg_lpn_poll_timeout_get(dst, pparse_value->dw_parameter[2]);
        break;
    default:
        ret = USER_CMD_RESULT_WRONG_PARAMETER;
        break;
    }
    return ret;
}

/**
 * @brief configuration model parameter set command
 * @param pparse_value - parsed parameters
 * @return command execute status
 */
user_cmd_parse_result_t user_cmd_cms(user_cmd_parse_value_t *pparse_value)
{
    uint16_t dst;
    uint8_t model;
    if (pparse_value->para_count < 2)
    {
        return USER_CMD_RESULT_WRONG_NUM_OF_PARAMETERS;
    }
    dst = pparse_value->dw_parameter[0];
    model = pparse_value->dw_parameter[1];
    user_cmd_parse_result_t ret = USER_CMD_RESULT_OK;
    switch (model)
    {
    case 0:
        cfg_beacon_set(dst, pparse_value->dw_parameter[2]);
        break;
    case 2:
        cfg_default_ttl_set(dst, pparse_value->dw_parameter[2]);
        break;
    case 3:
        cfg_proxy_set(dst, pparse_value->dw_parameter[2]);
        break;
    case 4:
        cfg_frnd_set(dst, pparse_value->dw_parameter[2]);
        break;
    case 5:
        cfg_relay_set(dst, pparse_value->dw_parameter[2], pparse_value->dw_parameter[3],
                      pparse_value->dw_parameter[4]);
        break;
    case 6:
        {
            pub_key_info_t pub_key_info = {.app_key_index = pparse_value->dw_parameter[5], .frnd_flag = pparse_value->dw_parameter[6], 0};
            pub_period_t pub_period = {pparse_value->dw_parameter[8] & 0x3f, pparse_value->dw_parameter[8] >> 6};
            pub_retrans_info_t pub_retrans_info = {pparse_value->dw_parameter[9], pparse_value->dw_parameter[10]};
            uint8_t addr[16];
            if (0 == pparse_value->dw_parameter[3])
            {
                uint16_t element_addr = (uint16_t)(pparse_value->dw_parameter[4]);
                LE_WORD2EXTRN(addr, element_addr);
            }
            else
            {
                plt_hex_to_bin(addr, (uint8_t *)pparse_value->pparameter[4], 16);
            }

            cfg_model_pub_set(dst, pparse_value->dw_parameter[2], pparse_value->dw_parameter[3],
                              addr, pub_key_info, pparse_value->dw_parameter[7], pub_period,
                              pub_retrans_info, pparse_value->dw_parameter[11]);
        }
        break;
    case 7:
        {
            uint8_t mode = pparse_value->dw_parameter[2];
            uint8_t addr[16];
            if ((mode < 4) && (2 != mode))
            {
                if (0 == pparse_value->dw_parameter[3])
                {
                    uint16_t element_addr = (uint16_t)(pparse_value->dw_parameter[5]);
                    LE_WORD2EXTRN(addr, element_addr);
                }
                else
                {
                    plt_hex_to_bin(addr, (uint8_t *)pparse_value->pparameter[5], 16);
                }
            }
            switch (mode)
            {
            case 0:
                cfg_model_sub_add(dst, pparse_value->dw_parameter[3], pparse_value->dw_parameter[4],
                                  addr, pparse_value->dw_parameter[6]);
                break;
            case 1:
                cfg_model_sub_delete(dst, pparse_value->dw_parameter[3], pparse_value->dw_parameter[4],
                                     addr, pparse_value->dw_parameter[6]);
                break;
            case 2:
                cfg_model_sub_delete_all(dst, pparse_value->dw_parameter[3], pparse_value->dw_parameter[4]);
                break;
            case 3:
                cfg_model_sub_overwrite(dst, pparse_value->dw_parameter[3], pparse_value->dw_parameter[4],
                                        addr, pparse_value->dw_parameter[6]);
                break;
            default:
                break;
            }
        }
        break;
    case 8:
        {
            uint8_t mode = pparse_value->dw_parameter[2];
            uint8_t key[16];
            switch (mode)
            {
            case 0:
                plt_hex_to_bin(key, (uint8_t *)pparse_value->pparameter[4], 16);
                cfg_net_key_add(dst, pparse_value->dw_parameter[3], key);
                break;
            case 1:
                plt_hex_to_bin(key, (uint8_t *)pparse_value->pparameter[4], 16);
                cfg_net_key_update(dst, pparse_value->dw_parameter[3], key);
                break;
            case 2:
                cfg_net_key_delete(dst, pparse_value->dw_parameter[3]);
                break;
            default:
                break;
            }
        }
        break;
    case 9:
        {
            uint8_t mode = pparse_value->dw_parameter[2];
            uint8_t key[16];
            switch (mode)
            {
            case 0:
                plt_hex_to_bin(key, (uint8_t *)pparse_value->pparameter[5], 16);
                cfg_app_key_add(dst, pparse_value->dw_parameter[3], pparse_value->dw_parameter[4],
                                key);
                break;
            case 1:
                plt_hex_to_bin(key, (uint8_t *)pparse_value->pparameter[5], 16);
                cfg_app_key_update(dst, pparse_value->dw_parameter[3], pparse_value->dw_parameter[4],
                                   key);
                break;
            case 2:
                cfg_app_key_delete(dst, pparse_value->dw_parameter[3], pparse_value->dw_parameter[4]);
                break;
            default:
                break;
            }
        }
        break;
    case 10:
        {
            uint8_t mode = pparse_value->dw_parameter[2];
            switch (mode)
            {
            case 0:
                cfg_model_app_bind(dst, pparse_value->dw_parameter[3], pparse_value->dw_parameter[4],
                                   pparse_value->dw_parameter[5]);
                break;
            case 1:
                cfg_model_app_unbind(dst, pparse_value->dw_parameter[3], pparse_value->dw_parameter[4],
                                     pparse_value->dw_parameter[5]);
                break;
            default:
                break;
            }
        }
        break;
    case 11:
        cfg_node_identity_set(dst, pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);
        break;
    case 12:
        cfg_node_reset(dst);
        break;
    case 13:
        {
            hb_pub_features_t features = {pparse_value->dw_parameter[6]};
            cfg_hb_pub_set(dst, pparse_value->dw_parameter[2], pparse_value->dw_parameter[3],
                           pparse_value->dw_parameter[4], pparse_value->dw_parameter[5],
                           features, pparse_value->dw_parameter[7]);
        }
        break;
    case 14:
        cfg_hb_sub_set(dst, pparse_value->dw_parameter[2], pparse_value->dw_parameter[3],
                       pparse_value->dw_parameter[4]);
        break;
    case 15:
        cfg_net_transmit_set(dst, pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);
        break;
    default:
        ret = USER_CMD_RESULT_WRONG_PARAMETER;
        break;
    }
    return ret;
}

user_cmd_parse_result_t user_cmd_gdtt_get(user_cmd_parse_value_t *pparse_value)
{
    generic_default_transition_time_get(&model_gdtt_client,
                                        pparse_value->dw_parameter[0],
                                        pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_gdtt_set(user_cmd_parse_value_t *pparse_value)
{
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[1], pparse_value->dw_parameter[2]};
    generic_default_transition_time_set(&model_gdtt_client,
                                        pparse_value->dw_parameter[0],
                                        pparse_value->dw_parameter[4], trans_time,
                                        pparse_value->dw_parameter[3]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_health_fault_get(user_cmd_parse_value_t *pparse_value)
{
    if (pparse_value->para_count < 3)
    {
        return USER_CMD_RESULT_WRONG_NUM_OF_PARAMETERS;
    }

    health_fault_get(&model_health_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[1],
                     pparse_value->dw_parameter[2]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_health_fault_clear(user_cmd_parse_value_t *pparse_value)
{
    if (pparse_value->para_count < 4)
    {
        return USER_CMD_RESULT_WRONG_NUM_OF_PARAMETERS;
    }


    health_fault_clear(&model_health_client, pparse_value->dw_parameter[0],
                       pparse_value->dw_parameter[1],
                       pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_health_fault_test(user_cmd_parse_value_t *pparse_value)
{
    if (pparse_value->para_count < 5)
    {
        return USER_CMD_RESULT_WRONG_NUM_OF_PARAMETERS;
    }

    health_fault_test(&model_health_client, pparse_value->dw_parameter[0],
                      pparse_value->dw_parameter[1],
                      pparse_value->dw_parameter[2], pparse_value->dw_parameter[3],
                      pparse_value->dw_parameter[4]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_health_period_get(user_cmd_parse_value_t *pparse_value)
{
    if (pparse_value->para_count < 2)
    {
        return USER_CMD_RESULT_WRONG_NUM_OF_PARAMETERS;
    }

    health_period_get(&model_health_client, pparse_value->dw_parameter[0],
                      pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_health_period_set(user_cmd_parse_value_t *pparse_value)
{
    if (pparse_value->para_count < 4)
    {
        return USER_CMD_RESULT_WRONG_NUM_OF_PARAMETERS;
    }

    health_period_set(&model_health_client, pparse_value->dw_parameter[0],
                      pparse_value->dw_parameter[1],
                      pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_health_attn_get(user_cmd_parse_value_t *pparse_value)
{
    if (pparse_value->para_count < 2)
    {
        return USER_CMD_RESULT_WRONG_NUM_OF_PARAMETERS;
    }

    health_attn_get(&model_health_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_health_attn_set(user_cmd_parse_value_t *pparse_value)
{
    if (pparse_value->para_count < 4)
    {
        return USER_CMD_RESULT_WRONG_NUM_OF_PARAMETERS;
    }

    health_attn_set(&model_health_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[1],
                    pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_on_off_get(user_cmd_parse_value_t *pparse_value)
{
    generic_on_off_get(&model_goo_client, pparse_value->dw_parameter[0],
                       pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_on_off_set(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 4)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[4],
                                            pparse_value->dw_parameter[5]
                                           };
    generic_on_off_set(&model_goo_client, pparse_value->dw_parameter[0],
                       pparse_value->dw_parameter[3], (generic_on_off_t)pparse_value->dw_parameter[1],
                       tid, optional, trans_time, pparse_value->dw_parameter[6], pparse_value->dw_parameter[2]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_level_get(user_cmd_parse_value_t *pparse_value)
{
    generic_level_get(&model_gl_client, pparse_value->dw_parameter[0],
                      pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_level_set(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 4)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[4],
                                            pparse_value->dw_parameter[5]
                                           };
    generic_level_set(&model_gl_client, pparse_value->dw_parameter[0],
                      pparse_value->dw_parameter[3], pparse_value->dw_parameter[1],
                      tid, optional, trans_time, pparse_value->dw_parameter[6],
                      pparse_value->dw_parameter[2]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_delta_set(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 5)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[5],
                                            pparse_value->dw_parameter[6]
                                           };
    int32_t delta_level = pparse_value->dw_parameter[2];
    if (pparse_value->dw_parameter[1] == 0)
    {
        delta_level = -delta_level;
    }
    generic_delta_set(&model_gl_client, pparse_value->dw_parameter[0],
                      pparse_value->dw_parameter[4], delta_level,
                      tid, optional, trans_time, pparse_value->dw_parameter[7],
                      pparse_value->dw_parameter[3]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_move_set(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 5)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[5],
                                            pparse_value->dw_parameter[6]
                                           };
    int32_t delta_level = pparse_value->dw_parameter[2];
    if (pparse_value->dw_parameter[1] == 0)
    {
        delta_level = -delta_level;
    }
    generic_move_set(&model_gl_client, pparse_value->dw_parameter[0],
                     pparse_value->dw_parameter[4], delta_level,
                     tid, optional, trans_time, pparse_value->dw_parameter[7],
                     pparse_value->dw_parameter[3]);
    return USER_CMD_RESULT_OK;
}


user_cmd_parse_result_t user_cmd_generic_on_powerup_get(user_cmd_parse_value_t *pparse_value)
{
    generic_on_power_up_get(&model_gpoo_client, pparse_value->dw_parameter[0],
                            pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_on_powerup_set(user_cmd_parse_value_t *pparse_value)
{
    generic_on_power_up_set(&model_gpoo_client, pparse_value->dw_parameter[0],
                            pparse_value->dw_parameter[3], (generic_on_power_up_t)pparse_value->dw_parameter[1],
                            pparse_value->dw_parameter[2]);

    return USER_CMD_RESULT_OK;
}


user_cmd_parse_result_t user_cmd_generic_power_level_get(user_cmd_parse_value_t *pparse_value)
{
    generic_power_level_get(&model_gpl_client, pparse_value->dw_parameter[0],
                            pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_power_level_set(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 4)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[4],
                                            pparse_value->dw_parameter[5]
                                           };

    generic_power_level_set(&model_gpl_client, pparse_value->dw_parameter[0],
                            pparse_value->dw_parameter[3], pparse_value->dw_parameter[1],
                            tid, optional, trans_time, pparse_value->dw_parameter[6],
                            pparse_value->dw_parameter[2]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_power_last_get(user_cmd_parse_value_t *pparse_value)
{
    generic_power_last_get(&model_gpl_client, pparse_value->dw_parameter[0],
                           pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_power_default_get(user_cmd_parse_value_t *pparse_value)
{
    generic_power_default_get(&model_gpl_client, pparse_value->dw_parameter[0],
                              pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_power_default_set(user_cmd_parse_value_t *pparse_value)
{
    generic_power_default_set(&model_gpl_client, pparse_value->dw_parameter[0],
                              pparse_value->dw_parameter[3], pparse_value->dw_parameter[1],
                              pparse_value->dw_parameter[2]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_power_range_get(user_cmd_parse_value_t *pparse_value)
{
    generic_power_range_get(&model_gpl_client, pparse_value->dw_parameter[0],
                            pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_power_range_set(user_cmd_parse_value_t *pparse_value)
{
    generic_power_range_set(&model_gpl_client, pparse_value->dw_parameter[0],
                            pparse_value->dw_parameter[4], pparse_value->dw_parameter[1],
                            pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_battery_get(user_cmd_parse_value_t *pparse_value)
{
    generic_battery_get(&model_gb_client, pparse_value->dw_parameter[0],
                        pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lightness_get(user_cmd_parse_value_t *pparse_value)
{
    uint16_t dst;
    uint16_t app_key_index;
    dst = pparse_value->dw_parameter[0];
    app_key_index = pparse_value->dw_parameter[1];
    light_lightness_get(&light_lightness_client, dst, app_key_index);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lightness_set(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 4)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[4],
                                            pparse_value->dw_parameter[5]
                                           };

    light_lightness_set(&light_lightness_client, pparse_value->dw_parameter[0],
                        pparse_value->dw_parameter[3],
                        pparse_value->dw_parameter[1], tid, optional, trans_time,
                        pparse_value->dw_parameter[6], pparse_value->dw_parameter[2]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lightness_linear_get(user_cmd_parse_value_t
                                                            *pparse_value)
{
    uint16_t dst;
    uint16_t app_key_index;
    dst = pparse_value->dw_parameter[0];
    app_key_index = pparse_value->dw_parameter[1];
    light_lightness_linear_get(&light_lightness_client, dst, app_key_index);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lightness_linear_set(user_cmd_parse_value_t
                                                            *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 4)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[4],
                                            pparse_value->dw_parameter[5]
                                           };

    light_lightness_linear_set(&light_lightness_client, pparse_value->dw_parameter[0],
                               pparse_value->dw_parameter[3],
                               pparse_value->dw_parameter[1], tid, optional, trans_time,
                               pparse_value->dw_parameter[6], pparse_value->dw_parameter[2]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lightness_last_get(user_cmd_parse_value_t *pparse_value)
{
    light_lightness_last_get(&light_lightness_client, pparse_value->dw_parameter[0],
                             pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lightness_default_get(user_cmd_parse_value_t *pparse_value)
{
    light_lightness_default_get(&light_lightness_client, pparse_value->dw_parameter[0],
                                pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lightness_default_set(user_cmd_parse_value_t *pparse_value)
{
    light_lightness_default_set(&light_lightness_client, pparse_value->dw_parameter[0],
                                pparse_value->dw_parameter[3], pparse_value->dw_parameter[1],
                                pparse_value->dw_parameter[2]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lightness_range_get(user_cmd_parse_value_t *pparse_value)
{
    light_lightness_range_get(&light_lightness_client, pparse_value->dw_parameter[0],
                              pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lightness_range_set(user_cmd_parse_value_t *pparse_value)
{
    light_lightness_range_set(&light_lightness_client, pparse_value->dw_parameter[0],
                              pparse_value->dw_parameter[4], pparse_value->dw_parameter[1],
                              pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_ctl_get(user_cmd_parse_value_t *pparse_value)
{
    light_ctl_get(&light_ctl_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_ctl_set(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 6)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[6],
                                            pparse_value->dw_parameter[7]
                                           };

    light_ctl_set(&light_ctl_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[5],
                  pparse_value->dw_parameter[1], pparse_value->dw_parameter[2],
                  pparse_value->dw_parameter[3], tid, optional, trans_time,
                  pparse_value->dw_parameter[8], pparse_value->dw_parameter[4]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_ctl_temperature_get(user_cmd_parse_value_t *pparse_value)
{
    light_ctl_temperature_get(&light_ctl_client, pparse_value->dw_parameter[0],
                              pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_ctl_temperature_set(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 5)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[5],
                                            pparse_value->dw_parameter[6]
                                           };


    light_ctl_temperature_set(&light_ctl_client, pparse_value->dw_parameter[0],
                              pparse_value->dw_parameter[4],
                              pparse_value->dw_parameter[1], pparse_value->dw_parameter[2],
                              tid, optional, trans_time, pparse_value->dw_parameter[7], pparse_value->dw_parameter[3]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_ctl_temperature_range_get(user_cmd_parse_value_t
                                                                 *pparse_value)
{
    light_ctl_temperature_range_get(&light_ctl_client, pparse_value->dw_parameter[0],
                                    pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_ctl_temperature_range_set(user_cmd_parse_value_t
                                                                 *pparse_value)
{
    light_ctl_temperature_range_set(&light_ctl_client, pparse_value->dw_parameter[0],
                                    pparse_value->dw_parameter[4], pparse_value->dw_parameter[1],
                                    pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_ctl_default_get(user_cmd_parse_value_t *pparse_value)
{
    light_ctl_default_get(&light_ctl_client, pparse_value->dw_parameter[0],
                          pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_ctl_default_set(user_cmd_parse_value_t *pparse_value)
{
    light_ctl_default_set(&light_ctl_client, pparse_value->dw_parameter[0],
                          pparse_value->dw_parameter[5], pparse_value->dw_parameter[1],
                          pparse_value->dw_parameter[2], pparse_value->dw_parameter[3],
                          pparse_value->dw_parameter[4]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_hsl_get(user_cmd_parse_value_t *pparse_value)
{
    uint16_t dst;
    uint16_t app_key_index;
    dst = pparse_value->dw_parameter[0];
    app_key_index = pparse_value->dw_parameter[1];
    light_hsl_get(&light_hsl_client, dst, app_key_index);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_hsl_set(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 6)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[6],
                                            pparse_value->dw_parameter[7]
                                           };

    light_hsl_set(&light_hsl_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[5],
                  pparse_value->dw_parameter[3], pparse_value->dw_parameter[1],
                  pparse_value->dw_parameter[2], tid, optional, trans_time, pparse_value->dw_parameter[8],
                  pparse_value->dw_parameter[4]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_hsl_target_get(user_cmd_parse_value_t *pparse_value)
{
    light_hsl_target_get(&light_hsl_client, pparse_value->dw_parameter[0],
                         pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_hsl_hue_get(user_cmd_parse_value_t *pparse_value)
{
    uint16_t dst;
    uint16_t app_key_index;
    dst = pparse_value->dw_parameter[0];
    app_key_index = pparse_value->dw_parameter[1];
    light_hsl_hue_get(&light_hsl_client, dst, app_key_index);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_hsl_hue_set(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 4)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[4],
                                            pparse_value->dw_parameter[5]
                                           };

    light_hsl_hue_set(&light_hsl_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[3],
                      pparse_value->dw_parameter[1], tid, optional, trans_time,
                      pparse_value->dw_parameter[6], pparse_value->dw_parameter[2]);

    return USER_CMD_RESULT_OK;

}

user_cmd_parse_result_t user_cmd_light_hsl_saturation_get(user_cmd_parse_value_t *pparse_value)
{
    uint16_t dst;
    uint16_t app_key_index;
    dst = pparse_value->dw_parameter[0];
    app_key_index = pparse_value->dw_parameter[1];
    light_hsl_saturation_get(&light_hsl_client, dst, app_key_index);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_hsl_saturation_set(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 4)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[4],
                                            pparse_value->dw_parameter[5]
                                           };

    light_hsl_saturation_set(&light_hsl_client, pparse_value->dw_parameter[0],
                             pparse_value->dw_parameter[3],
                             pparse_value->dw_parameter[1], tid, optional, trans_time,
                             pparse_value->dw_parameter[6], pparse_value->dw_parameter[2]);

    return USER_CMD_RESULT_OK;
}


user_cmd_parse_result_t user_cmd_light_hsl_default_get(user_cmd_parse_value_t *pparse_value)
{
    light_hsl_default_get(&light_hsl_client, pparse_value->dw_parameter[0],
                          pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_hsl_default_set(user_cmd_parse_value_t *pparse_value)
{
    light_hsl_default_set(&light_hsl_client, pparse_value->dw_parameter[0],
                          pparse_value->dw_parameter[5],
                          pparse_value->dw_parameter[1], pparse_value->dw_parameter[2],
                          pparse_value->dw_parameter[3], pparse_value->dw_parameter[4]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_hsl_range_get(user_cmd_parse_value_t *pparse_value)
{
    light_hsl_range_get(&light_hsl_client, pparse_value->dw_parameter[0],
                        pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_hsl_range_set(user_cmd_parse_value_t *pparse_value)
{
    light_hsl_range_set(&light_hsl_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[6],
                        pparse_value->dw_parameter[1], pparse_value->dw_parameter[2],
                        pparse_value->dw_parameter[3], pparse_value->dw_parameter[4], pparse_value->dw_parameter[5]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_time_set(user_cmd_parse_value_t *pparse_value)
{
    tai_time_t time;
    time.tai_seconds[0] = pparse_value->dw_parameter[1];
    time.tai_seconds[1] = pparse_value->dw_parameter[1] >> 8;
    time.tai_seconds[2] = pparse_value->dw_parameter[1] >> 16;
    time.tai_seconds[3] = pparse_value->dw_parameter[1] >> 24;
    time.tai_seconds[4] = pparse_value->dw_parameter[2];
    time.subsecond = pparse_value->dw_parameter[3];
    time.uncertainty = pparse_value->dw_parameter[4];
    time.time_authority = pparse_value->dw_parameter[5];
    time.tai_utc_delta = pparse_value->dw_parameter[6];
    time.time_zone_offset = pparse_value->dw_parameter[7];
    time_set(&model_time_client, pparse_value->dw_parameter[0],
             pparse_value->dw_parameter[8], time);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_time_get(user_cmd_parse_value_t *pparse_value)
{
    time_get(&model_time_client, pparse_value->dw_parameter[0],
             pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_time_zone_set(user_cmd_parse_value_t *pparse_value)
{
    uint8_t change[5];
    change[0] = pparse_value->dw_parameter[2];
    change[1] = pparse_value->dw_parameter[2] >> 8;
    change[2] = pparse_value->dw_parameter[2] >> 16;
    change[3] = pparse_value->dw_parameter[2] >> 24;
    change[4] = pparse_value->dw_parameter[3];

    time_set_zone(&model_time_client, pparse_value->dw_parameter[0],
                  pparse_value->dw_parameter[4], pparse_value->dw_parameter[1],
                  change);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_time_zone_get(user_cmd_parse_value_t *pparse_value)
{
    time_get_zone(&model_time_client, pparse_value->dw_parameter[0],
                  pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_time_tai_utc_delta_set(user_cmd_parse_value_t *pparse_value)
{
    uint8_t delta[5];
    delta[0] = pparse_value->dw_parameter[2];
    delta[1] = pparse_value->dw_parameter[2] >> 8;
    delta[2] = pparse_value->dw_parameter[2] >> 16;
    delta[3] = pparse_value->dw_parameter[2] >> 24;
    delta[4] = pparse_value->dw_parameter[3];

    time_set_tai_utc_delta(&model_time_client, pparse_value->dw_parameter[0],
                           pparse_value->dw_parameter[4], pparse_value->dw_parameter[1],
                           delta);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_time_tai_utc_delta_get(user_cmd_parse_value_t *pparse_value)
{
    time_get_tai_utc_delta(&model_time_client, pparse_value->dw_parameter[0],
                           pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_time_role_set(user_cmd_parse_value_t *pparse_value)
{
    time_set_role(&model_time_client, pparse_value->dw_parameter[0],
                  pparse_value->dw_parameter[2], (time_role_t)(pparse_value->dw_parameter[1]));
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_time_role_get(user_cmd_parse_value_t *pparse_value)
{
    time_get_role(&model_time_client, pparse_value->dw_parameter[0],
                  pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}


user_cmd_parse_result_t user_cmd_scene_store(user_cmd_parse_value_t *pparse_value)
{
    scene_store(&model_scene_client, pparse_value->dw_parameter[0],
                pparse_value->dw_parameter[3],
                pparse_value->dw_parameter[1], pparse_value->dw_parameter[2]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_scene_recall(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 4)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[4],
                                            pparse_value->dw_parameter[5]
                                           };

    scene_recall(&model_scene_client, pparse_value->dw_parameter[0],
                 pparse_value->dw_parameter[3],
                 pparse_value->dw_parameter[1], tid, optional, trans_time,
                 pparse_value->dw_parameter[6], pparse_value->dw_parameter[2]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_scene_get(user_cmd_parse_value_t *pparse_value)
{
    scene_get(&model_scene_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_scene_register_get(user_cmd_parse_value_t *pparse_value)
{
    scene_register_get(&model_scene_client, pparse_value->dw_parameter[0],
                       pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_scene_delete(user_cmd_parse_value_t *pparse_value)
{
    scene_delete(&model_scene_client, pparse_value->dw_parameter[0],
                 pparse_value->dw_parameter[3],
                 pparse_value->dw_parameter[1], pparse_value->dw_parameter[2]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_sensor_descriptor_get(user_cmd_parse_value_t *pparse_value)
{
    sensor_descriptor_get(&model_sensor_client, pparse_value->dw_parameter[0],
                          pparse_value->dw_parameter[2], pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_sensor_cadence_get(user_cmd_parse_value_t *pparse_value)
{
    sensor_cadence_get(&model_sensor_client, pparse_value->dw_parameter[0],
                       pparse_value->dw_parameter[2], pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_sensor_cadence_set(user_cmd_parse_value_t *pparse_value)
{
    uint8_t trigger_down[10];
    uint8_t trigger_up[10];
    uint8_t cadence_low[10];
    uint8_t cadence_high[10];
    sensor_cadence_t cadence;
    cadence.fast_cadence_period_divisor = pparse_value->dw_parameter[2];
    cadence.status_trigger_type = pparse_value->dw_parameter[3];
    cadence.status_trigger_delta_down = trigger_down;
    cadence.status_trigger_delta_up = trigger_up;
    cadence.fast_cadence_low = cadence_low;
    cadence.fast_cadence_high = cadence_high;
    uint8_t data_len = pparse_value->para_count - 7;
    uint8_t trigger_len = 0;
    if (SENSOR_TRIGGER_TYPE_CHARACTERISTIC == cadence.status_trigger_type)
    {
        cadence.raw_value_len = data_len / 4;
        trigger_len = cadence.raw_value_len;
    }
    else
    {
        cadence.raw_value_len = (data_len - 4) / 2;
        trigger_len = 2;
    }

    uint32_t *pdata = pparse_value->dw_parameter + 4;
    for (uint8_t i = 0; i < trigger_len; ++i)
    {
        trigger_down[i] = *pdata ++;
    }

    for (uint8_t i = 0; i < trigger_len; ++i)
    {
        trigger_up[i] = *pdata ++;
    }

    cadence.status_min_interval = *pdata ++;

    for (uint8_t i = 0; i < cadence.raw_value_len; ++i)
    {
        cadence_low[i] = *pdata ++;
    }

    for (uint8_t i = 0; i < cadence.raw_value_len; ++i)
    {
        cadence_high[i] = *pdata ++;
    }

    bool ack = *pdata ++;
    uint16_t app_key_index = *pdata;

    sensor_cadence_set(&model_sensor_client, pparse_value->dw_parameter[0],
                       app_key_index, pparse_value->dw_parameter[1], &cadence, ack);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_sensor_settings_get(user_cmd_parse_value_t *pparse_value)
{
    sensor_settings_get(&model_sensor_client, pparse_value->dw_parameter[0],
                        pparse_value->dw_parameter[2], pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_sensor_setting_get(user_cmd_parse_value_t *pparse_value)
{
    sensor_setting_get(&model_sensor_client, pparse_value->dw_parameter[0],
                       pparse_value->dw_parameter[3],
                       pparse_value->dw_parameter[1], pparse_value->dw_parameter[2]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_sensor_setting_set(user_cmd_parse_value_t *pparse_value)
{
    uint8_t data_len = pparse_value->para_count - 5;
    uint8_t setting_raw[10];
    for (uint8_t i = 0; i < data_len; ++i)
    {
        setting_raw[i] = pparse_value->dw_parameter[3 + i];
    }
    sensor_setting_set(&model_sensor_client, pparse_value->dw_parameter[0],
                       pparse_value->dw_parameter[pparse_value->para_count - 1], pparse_value->dw_parameter[1],
                       pparse_value->dw_parameter[2],
                       data_len, setting_raw, pparse_value->dw_parameter[pparse_value->para_count - 2]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_sensor_get(user_cmd_parse_value_t *pparse_value)
{
    sensor_get(&model_sensor_client, pparse_value->dw_parameter[0],
               pparse_value->dw_parameter[2], pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_sensor_column_get(user_cmd_parse_value_t *pparse_value)
{
    uint8_t data_len = pparse_value->para_count - 3;
    uint8_t raw_value_x[10];
    for (uint8_t i = 0; i < data_len; ++i)
    {
        raw_value_x[i] = pparse_value->dw_parameter[2 + i];
    }

    sensor_column_get(&model_sensor_client, pparse_value->dw_parameter[0],
                      pparse_value->dw_parameter[pparse_value->para_count - 1], pparse_value->dw_parameter[1],
                      data_len, raw_value_x);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_sensor_series_get(user_cmd_parse_value_t *pparse_value)
{
    uint8_t data_len = (pparse_value->para_count - 3) / 2;
    uint8_t raw_value_x1[10];
    uint8_t raw_value_x2[10];
    uint32_t *pdata = pparse_value->dw_parameter + 2;
    for (uint8_t i = 0; i < data_len; ++i)
    {
        raw_value_x1[i] = *pdata ++;
    }

    for (uint8_t i = 0; i < data_len; ++i)
    {
        raw_value_x2[i] = *pdata ++;
    }

    sensor_series_get(&model_sensor_client, pparse_value->dw_parameter[0],
                      pparse_value->dw_parameter[pparse_value->para_count - 1], pparse_value->dw_parameter[1],
                      data_len, raw_value_x1, raw_value_x2);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_scheduler_get(user_cmd_parse_value_t *pparse_value)
{
    scheduler_get(&model_scheduler_client, pparse_value->dw_parameter[0],
                  pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_scheduler_action_get(user_cmd_parse_value_t *pparse_value)
{
    scheduler_action_get(&model_scheduler_client, pparse_value->dw_parameter[0],
                         pparse_value->dw_parameter[2], pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_scheduler_action_set(user_cmd_parse_value_t *pparse_value)
{
    scheduler_register_t scheduler = {pparse_value->dw_parameter[1], pparse_value->dw_parameter[2], pparse_value->dw_parameter[3],
                                      pparse_value->dw_parameter[4], pparse_value->dw_parameter[5], pparse_value->dw_parameter[6],
                                      pparse_value->dw_parameter[7], pparse_value->dw_parameter[8], pparse_value->dw_parameter[9],
                                      pparse_value->dw_parameter[10], pparse_value->dw_parameter[11], pparse_value->dw_parameter[12]
                                     };
    scheduler_action_set(&model_scheduler_client, pparse_value->dw_parameter[0],
                         pparse_value->dw_parameter[14], scheduler, pparse_value->dw_parameter[13]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_location_global_get(user_cmd_parse_value_t *pparse_value)
{
    generic_location_global_get(&model_generic_location_client, pparse_value->dw_parameter[0],
                                pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_location_global_set(user_cmd_parse_value_t *pparse_value)
{
    generic_location_global_t global = {pparse_value->dw_parameter[1], pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]};
    generic_location_global_set(&model_generic_location_client, pparse_value->dw_parameter[0],
                                pparse_value->dw_parameter[5], global, pparse_value->dw_parameter[4]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_location_local_get(user_cmd_parse_value_t *pparse_value)
{
    generic_location_local_get(&model_generic_location_client, pparse_value->dw_parameter[0],
                               pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_location_local_set(user_cmd_parse_value_t *pparse_value)
{
    generic_location_local_t local;
    local.local_north = pparse_value->dw_parameter[1];
    local.local_east = pparse_value->dw_parameter[2];
    local.local_altitude = pparse_value->dw_parameter[3];
    local.floor_num = pparse_value->dw_parameter[4];
    local.uncertainty.stationary = pparse_value->dw_parameter[5];
    local.uncertainty.update_time = pparse_value->dw_parameter[6];
    local.uncertainty.precision = pparse_value->dw_parameter[7];
    generic_location_local_set(&model_generic_location_client, pparse_value->dw_parameter[0],
                               pparse_value->dw_parameter[9], local, pparse_value->dw_parameter[8]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_user_properties_get(user_cmd_parse_value_t *pparse_value)
{
    generic_user_properties_get(&model_gp_client, pparse_value->dw_parameter[0],
                                pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_user_property_get(user_cmd_parse_value_t *pparse_value)
{
    generic_user_property_get(&model_gp_client, pparse_value->dw_parameter[0],
                              pparse_value->dw_parameter[2], pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_user_property_set(user_cmd_parse_value_t *pparse_value)
{
    if (pparse_value->para_count < 4)
    {
        return USER_CMD_RESULT_WRONG_NUM_OF_PARAMETERS;
    }
    uint8_t property_value[20];
    uint8_t value_len = pparse_value->para_count - 4;
    if (value_len > 20)
    {
        value_len = 20;
    }
    for (uint8_t i = 0; i < value_len; ++i)
    {
        property_value[i] = pparse_value->dw_parameter[4 + i];
    }

    generic_user_property_set(&model_gp_client, pparse_value->dw_parameter[0],
                              pparse_value->dw_parameter[1], pparse_value->dw_parameter[3], property_value, value_len,
                              pparse_value->dw_parameter[2]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_admin_properties_get(user_cmd_parse_value_t *pparse_value)
{
    generic_admin_properties_get(&model_gp_client, pparse_value->dw_parameter[0],
                                 pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_admin_property_get(user_cmd_parse_value_t *pparse_value)
{
    generic_admin_property_get(&model_gp_client, pparse_value->dw_parameter[0],
                               pparse_value->dw_parameter[2], pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_admin_property_set(user_cmd_parse_value_t *pparse_value)
{
    if (pparse_value->para_count < 5)
    {
        return USER_CMD_RESULT_WRONG_NUM_OF_PARAMETERS;
    }
    uint8_t property_value[20];
    uint8_t value_len = pparse_value->para_count - 5;
    if (value_len > 20)
    {
        value_len = 20;
    }
    for (uint8_t i = 0; i < value_len; ++i)
    {
        property_value[i] = pparse_value->dw_parameter[5 + i];
    }

    generic_admin_property_set(&model_gp_client, pparse_value->dw_parameter[0],
                               pparse_value->dw_parameter[1], pparse_value->dw_parameter[3], pparse_value->dw_parameter[4],
                               property_value, value_len, pparse_value->dw_parameter[2]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_manufacturer_properties_get(user_cmd_parse_value_t
                                                                     *pparse_value)
{
    generic_manufacturer_properties_get(&model_gp_client, pparse_value->dw_parameter[0],
                                        pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_manufacturer_property_get(user_cmd_parse_value_t
                                                                   *pparse_value)
{
    generic_manufacturer_property_get(&model_gp_client, pparse_value->dw_parameter[0],
                                      pparse_value->dw_parameter[2], pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_manufacturer_property_set(user_cmd_parse_value_t
                                                                   *pparse_value)
{
    generic_manufacturer_property_set(&model_gp_client, pparse_value->dw_parameter[0],
                                      pparse_value->dw_parameter[1], pparse_value->dw_parameter[3],
                                      pparse_value->dw_parameter[4], pparse_value->dw_parameter[2]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_generic_client_properties_get(user_cmd_parse_value_t *pparse_value)
{
    generic_client_properties_get(&model_gp_client, pparse_value->dw_parameter[0],
                                  pparse_value->dw_parameter[1], pparse_value->dw_parameter[2]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_xyl_get(user_cmd_parse_value_t *pparse_value)
{
    uint16_t dst;
    uint16_t app_key_index;
    dst = pparse_value->dw_parameter[0];
    app_key_index = pparse_value->dw_parameter[1];
    light_xyl_get(&light_xyl_client, dst, app_key_index);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_xyl_set(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 6)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[6],
                                            pparse_value->dw_parameter[7]
                                           };

    light_xyl_t xyl = {pparse_value->dw_parameter[1], pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]};
    light_xyl_set(&light_xyl_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[5], xyl,
                  tid, optional, trans_time, pparse_value->dw_parameter[8], pparse_value->dw_parameter[4]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_xyl_target_get(user_cmd_parse_value_t *pparse_value)
{
    light_xyl_target_get(&light_xyl_client, pparse_value->dw_parameter[0],
                         pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_xyl_default_get(user_cmd_parse_value_t *pparse_value)
{
    light_xyl_default_get(&light_xyl_client, pparse_value->dw_parameter[0],
                          pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_xyl_default_set(user_cmd_parse_value_t *pparse_value)
{
    light_xyl_t xyl = {pparse_value->dw_parameter[1], pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]};
    light_xyl_default_set(&light_xyl_client, pparse_value->dw_parameter[0],
                          pparse_value->dw_parameter[5], xyl,
                          pparse_value->dw_parameter[4]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_xyl_range_get(user_cmd_parse_value_t *pparse_value)
{
    light_xyl_range_get(&light_xyl_client, pparse_value->dw_parameter[0],
                        pparse_value->dw_parameter[1]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_xyl_range_set(user_cmd_parse_value_t *pparse_value)
{
    light_xyl_range_set(&light_xyl_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[6],
                        pparse_value->dw_parameter[1], pparse_value->dw_parameter[2],
                        pparse_value->dw_parameter[3], pparse_value->dw_parameter[4], pparse_value->dw_parameter[5]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lc_mode_get(user_cmd_parse_value_t *pparse_value)
{
    light_lc_mode_get(&light_lc_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lc_mode_set(user_cmd_parse_value_t *pparse_value)
{
    light_lc_mode_set(&light_lc_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[2],
                      pparse_value->dw_parameter[1], pparse_value->dw_parameter[3]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lc_om_get(user_cmd_parse_value_t *pparse_value)
{
    light_lc_om_get(&light_lc_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lc_om_set(user_cmd_parse_value_t *pparse_value)
{
    light_lc_om_set(&light_lc_client, pparse_value->dw_parameter[0], pparse_value->dw_parameter[2],
                    pparse_value->dw_parameter[1], pparse_value->dw_parameter[3]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lc_light_on_off_set(user_cmd_parse_value_t *pparse_value)
{
    static uint8_t tid = 0;
    tid++;
    bool optional = FALSE;
    if (pparse_value->para_count > 4)
    {
        optional = TRUE;
    }
    generic_transition_time_t trans_time = {pparse_value->dw_parameter[4],
                                            pparse_value->dw_parameter[5]
                                           };

    light_lc_light_on_off_set(&light_lc_client, pparse_value->dw_parameter[0],
                              pparse_value->dw_parameter[3],
                              pparse_value->dw_parameter[1], tid, optional, trans_time, pparse_value->dw_parameter[6],
                              pparse_value->dw_parameter[2]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lc_light_on_off_get(user_cmd_parse_value_t *pparse_value)
{
    light_lc_light_on_off_get(&light_lc_client, pparse_value->dw_parameter[0],
                              pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lc_property_get(user_cmd_parse_value_t *pparse_value)
{
    light_lc_property_get(&light_lc_client, pparse_value->dw_parameter[0],
                          pparse_value->dw_parameter[2], pparse_value->dw_parameter[1]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_light_lc_property_set(user_cmd_parse_value_t *pparse_value)
{
    if (pparse_value->para_count < 3)
    {
        return USER_CMD_RESULT_WRONG_NUM_OF_PARAMETERS;
    }
    uint8_t property_value[20];
    uint8_t value_len = pparse_value->para_count - 4;
    if (value_len > 20)
    {
        value_len = 20;
    }
    for (uint8_t i = 0; i < value_len; ++i)
    {
        property_value[i] = pparse_value->dw_parameter[4 + i];
    }

    light_lc_property_set(&light_lc_client, pparse_value->dw_parameter[0],
                          pparse_value->dw_parameter[1],
                          pparse_value->dw_parameter[3], property_value, value_len, pparse_value->dw_parameter[2]);

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_private_beacon_get(user_cmd_parse_value_t *pparse_value)
{
#if MESH_PRB
    private_beacon_get(pparse_value->dw_parameter[0], 0);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_private_beacon_set(user_cmd_parse_value_t *pparse_value)
{
#if MESH_PRB
    if (pparse_value->para_count > 2)
    {
        private_beacon_set(pparse_value->dw_parameter[0], 0, pparse_value->dw_parameter[1], true,
                           pparse_value->dw_parameter[2]);
    }
    else
    {
        private_beacon_set(pparse_value->dw_parameter[0], 0, pparse_value->dw_parameter[1], false, 0);
    }
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_private_gatt_proxy_get(user_cmd_parse_value_t *pparse_value)
{
#if MESH_PRB
    private_gatt_proxy_get(pparse_value->dw_parameter[0], 0);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_private_gatt_proxy_set(user_cmd_parse_value_t *pparse_value)
{
#if MESH_PRB
    private_gatt_proxy_set(pparse_value->dw_parameter[0], 0, pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_private_node_identity_get(user_cmd_parse_value_t *pparse_value)
{
#if MESH_PRB
    private_node_identity_get(pparse_value->dw_parameter[0], 0, pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_private_node_identity_set(user_cmd_parse_value_t *pparse_value)
{
#if MESH_PRB
    private_node_identity_set(pparse_value->dw_parameter[0], 0, pparse_value->dw_parameter[1],
                              pparse_value->dw_parameter[2]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_blob_transfer_get(user_cmd_parse_value_t *pparse_value)
{
#if MESH_BLOB
    blob_transfer_get(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_blob_transfer_start(user_cmd_parse_value_t *pparse_value)
{
#if MESH_BLOB
    uint8_t blob_id[8];
    plt_hex_to_bin(blob_id, (uint8_t *)pparse_value->pparameter[3], sizeof(blob_id));
    blob_transfer_start(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1],
                        pparse_value->dw_parameter[2], blob_id, pparse_value->dw_parameter[4],
                        pparse_value->dw_parameter[5], pparse_value->dw_parameter[6]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_blob_transfer_cancel(user_cmd_parse_value_t *pparse_value)
{
#if MESH_BLOB
    uint8_t blob_id[8];
    plt_hex_to_bin(blob_id, (uint8_t *)pparse_value->pparameter[2], sizeof(blob_id));
    blob_transfer_cancel(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1], blob_id);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_blob_block_get(user_cmd_parse_value_t *pparse_value)
{
#if MESH_BLOB
    blob_block_get(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_blob_block_start(user_cmd_parse_value_t *pparse_value)
{
#if MESH_BLOB
    blob_block_start(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1],
                     pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_blob_chunk_transfer(user_cmd_parse_value_t *pparse_value)
{
#if MESH_BLOB
    uint16_t chunk_size = pparse_value->dw_parameter[3];
    uint8_t *pdata = plt_zalloc(chunk_size, RAM_TYPE_DATA_ON);
    if (NULL == pdata)
    {
        return USER_CMD_RESULT_ERROR;
    }
    memset(pdata, 0x31, chunk_size);
    blob_chunk_transfer(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1],
                        pparse_value->dw_parameter[2], pdata, chunk_size);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_blob_info_get(user_cmd_parse_value_t *pparse_value)
{
#if MESH_BLOB
    blob_info_get(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_update_info_get(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_update_info_get(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1],
                       pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_update_metadata_check(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    uint8_t metadata[255];
    uint8_t metadata_len = pparse_value->dw_parameter[4];
    plt_hex_to_bin(metadata, (uint8_t *)pparse_value->pparameter[3], metadata_len);
    fw_update_fw_metadata_check(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1],
                                pparse_value->dw_parameter[2], metadata, metadata_len);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_update_get(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_update_get(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_update_start(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    uint8_t blob_id[8];
    plt_hex_to_bin(blob_id, (uint8_t *)pparse_value->pparameter[4], 8);
    uint8_t metadata[255];
    uint8_t metadata_len = pparse_value->dw_parameter[7];
    plt_hex_to_bin(metadata, (uint8_t *)pparse_value->pparameter[6], metadata_len);
    fw_update_start(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1],
                    pparse_value->dw_parameter[2], pparse_value->dw_parameter[3], blob_id,
                    pparse_value->dw_parameter[5], metadata, metadata_len);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_update_cancel(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_update_cancel(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_update_apply(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_update_apply(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_recvs_add(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_dist_receiver_t receiver;
    receiver.addr = pparse_value->dw_parameter[2];
    receiver.update_fw_image_idx = pparse_value->dw_parameter[3];
    fw_dist_recvs_add(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1], &receiver, 1);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_recvs_delete_all(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_dist_recvs_delete_all(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_recvs_get(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_dist_recvs_get(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1],
                      pparse_value->dw_parameter[2], pparse_value->dw_parameter[3]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_caps_get(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_dist_caps_get(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_get(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_dist_get(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_start(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_dist_start_data_t start;
    start.dist_app_key_index = pparse_value->dw_parameter[2];
    start.dist_ttl = pparse_value->dw_parameter[3];
    start.dist_timeout_base = pparse_value->dw_parameter[4];
    start.dist_transfer_mode = pparse_value->dw_parameter[5];
    start.update_policy = pparse_value->dw_parameter[6];
    start.rfu = 0;
    start.dist_fw_image_idx = pparse_value->dw_parameter[7];
    uint8_t dist_dst_len = pparse_value->dw_parameter[9];
    if (2 == dist_dst_len)
    {
        *(uint16_t *)start.dist_multicast_addr = pparse_value->dw_parameter[8];
    }
    else
    {
        plt_hex_to_bin(start.dist_multicast_addr, (uint8_t *)pparse_value->pparameter[8], 16);
    }

    fw_dist_start(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1], start, dist_dst_len);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_cancel(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_dist_cancel(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_apply(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_dist_apply(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_upload_get(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_dist_upload_get(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_user_cmd_fw_dist_upload_start(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    uint8_t blob_id[8];
    plt_hex_to_bin(blob_id, (uint8_t *)pparse_value->pparameter[4], 8);
    uint8_t metadata[64];
    plt_hex_to_bin(blob_id, (uint8_t *)pparse_value->pparameter[6], pparse_value->dw_parameter[7]);
    uint8_t fw_id[64];
    plt_hex_to_bin(blob_id, (uint8_t *)pparse_value->pparameter[8], pparse_value->dw_parameter[9]);
    fw_dist_upload_start(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1],
                         pparse_value->dw_parameter[2],
                         pparse_value->dw_parameter[3], blob_id, pparse_value->dw_parameter[5],
                         metadata, pparse_value->dw_parameter[7], fw_id, pparse_value->dw_parameter[9]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_upload_oob_start(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    uint8_t upload_uri[64];
    plt_hex_to_bin(upload_uri, (uint8_t *)pparse_value->pparameter[2], pparse_value->dw_parameter[3]);
    uint8_t fw_id[64];
    plt_hex_to_bin(fw_id, (uint8_t *)pparse_value->pparameter[4], pparse_value->dw_parameter[5]);
    fw_dist_upload_oob_start(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1], upload_uri,
                             pparse_value->dw_parameter[3], fw_id, pparse_value->dw_parameter[5]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_upload_cancel(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_dist_upload_cancel(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_fw_get(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    uint8_t fw_id[64];
    plt_hex_to_bin(fw_id, (uint8_t *)pparse_value->pparameter[2], pparse_value->dw_parameter[3]);
    fw_dist_fw_get(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1], fw_id,
                   pparse_value->dw_parameter[3]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_fw_get_by_index(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_dist_fw_get_by_index(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1],
                            pparse_value->dw_parameter[2]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_fw_delete(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    uint8_t fw_id[64];
    plt_hex_to_bin(fw_id, (uint8_t *)pparse_value->pparameter[2], pparse_value->dw_parameter[3]);
    fw_dist_fw_delete(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1], fw_id,
                      pparse_value->dw_parameter[3]);
#endif
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_fw_dist_fw_delete_all(user_cmd_parse_value_t *pparse_value)
{
#if MESH_DFU
    fw_dist_fw_delete_all(pparse_value->dw_parameter[0], pparse_value->dw_parameter[1]);
#endif
    return USER_CMD_RESULT_OK;
}
