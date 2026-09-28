/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __ZEPHYR_STACK_MESH_INTERNAL_H__
#define __ZEPHYR_STACK_MESH_INTERNAL_H__

#include <stdint.h>

uint16_t bt_stack_mesh_init(void *data);

uint16_t bt_stack_mesh_enable(void *data);

void bt_stack_mesh_deinit(void);

uint16_t bt_mesh_stack_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_datatrans_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_config_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_generic_level_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_generic_default_transition_time_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_generic_power_on_off_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_generic_power_level_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_generic_battery_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_generic_location_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_generic_property_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_sensor_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_time_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_scene_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_scheduler_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_light_lightness_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_light_ctl_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_light_hsl_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_light_xyl_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_light_lc_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_health_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_remote_prov_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_private_beacon_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_device_firmware_update_standlone_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_scene_server_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_scene_setup_server_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_sensor_server_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_sensor_setup_server_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_generic_user_property_server_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_generic_admin_property_server_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_generic_manu_property_server_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_health_server_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_directed_forwarding_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_directed_forwarding_common_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_subnet_bridge_client_model_act_handle(rtk_bt_cmd_t *p_cmd);

typedef enum {
	RTK_BT_MESH_PROVISIONING_METHOD_NONE = 0,
	RTK_BT_MESH_PROVISIONING_PB_ADV_METHOD,
	RTK_BT_MESH_PROVISIONING_PB_GATT_METHOD,
	RTK_BT_MESH_PROVISIONING_REMOTE_PROV_METHOD,
} rtk_bt_mesh_provisioning_method_t;

#endif  // __ZEPHYR_STACK_MESH_INTERNAL_H__
