/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/mesh.h>

#include <bt_api_config.h>
#include <rtk_bt_mesh_def.h>

#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT

extern const struct bt_mesh_cfg_cli_cb cfg_cb;
static struct bt_mesh_cfg_cli cfg_cli = {
	.cb = &cfg_cb,
};

extern struct bt_mesh_health_cli health_cli;

#endif  // RTK_BLE_MESH_PROVISIONER_SUPPORT

#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
extern struct bt_mesh_health_srv health_server;
BT_MESH_HEALTH_PUB_DEFINE(health_pub, BT_MESH_TX_SDU_MAX);

#endif // RTK_BLE_MESH_DEVICE_SUPPORT

#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL
extern int generic_onoff_server_pub_update(const struct bt_mesh_model *mod);
BT_MESH_MODEL_PUB_DEFINE(gen_onoff_pub_srv, generic_onoff_server_pub_update, 2 + 1);
extern const struct bt_mesh_model_op gen_onoff_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_gen_onoff_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL
extern const struct bt_mesh_model_op gen_onoff_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_gen_onoff_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL
extern const struct bt_mesh_model_op generic_level_srv_op;
extern int generic_level_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(generic_level_server_pub, generic_level_srv_update_publish, 2 + 2);
extern const struct bt_mesh_model_cb bt_mesh_generic_level_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
extern const struct bt_mesh_model_op generic_level_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_generic_level_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSTION_TIME_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSTION_TIME_SERVER_MODEL
extern const struct bt_mesh_model_op generic_default_transition_time_srv_op;
extern int generic_default_transition_time_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(generic_default_transition_time_server_pub, generic_default_transition_time_srv_update_publish, 2 + 1);
extern const struct bt_mesh_model_cb bt_mesh_generic_default_transition_time_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL
extern const struct bt_mesh_model_op generic_default_transition_time_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_generic_default_transition_time_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL
extern const struct bt_mesh_model_op generic_power_onoff_srv_op;
extern int generic_power_on_off_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(generic_power_on_off_server_pub, generic_power_on_off_srv_update_publish, 2 + 1);
extern const struct bt_mesh_model_cb bt_mesh_generic_power_on_off_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL
extern const struct bt_mesh_model_op generic_power_onoff_setup_srv_op;
extern const struct bt_mesh_model_cb bt_mesh_generic_power_on_off_setup_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL
extern const struct bt_mesh_model_op generic_power_onoff_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_generic_power_on_off_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL
extern const struct bt_mesh_model_op generic_power_level_srv_op;
extern int generic_power_level_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(generic_power_level_server_pub, generic_power_level_srv_update_publish, 2 + 2);
extern const struct bt_mesh_model_cb bt_mesh_generic_power_level_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL
extern const struct bt_mesh_model_op generic_power_level_setup_srv_op;
extern const struct bt_mesh_model_cb bt_mesh_generic_power_level_setup_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
extern const struct bt_mesh_model_op generic_power_level_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_generic_power_level_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_SERVER_MODEL
extern const struct bt_mesh_model_op generic_battery_srv_op;
extern int generic_battery_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(generic_battery_server_pub, generic_battery_srv_update_publish, 2 + 8);
extern const struct bt_mesh_model_cb bt_mesh_generic_battery_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL
extern const struct bt_mesh_model_op generic_battery_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_generic_battery_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL
extern const struct bt_mesh_model_op generic_location_srv_op;
extern int generic_location_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(generic_location_server_pub, generic_location_srv_update_publish, 2 + 10);
extern const struct bt_mesh_model_cb bt_mesh_generic_location_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL
extern const struct bt_mesh_model_op generic_location_setup_srv_op;
extern const struct bt_mesh_model_cb bt_mesh_generic_location_setup_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
extern const struct bt_mesh_model_op generic_location_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_generic_location_cli_cb;
#endif

#if ((defined(BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL) || \
    (defined(BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL) || \
    (defined(BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL))
#include <rtk_bt_mesh_generic_model.h>
typedef struct {
	rtk_bt_mesh_generic_property_db_t *pproperties;
	uint16_t num_properties;
} generic_property_info_t;

#endif

#if defined(BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL
extern const struct bt_mesh_model_op generic_admin_property_srv_op;
extern generic_property_info_t admin_property_info;
extern const struct bt_mesh_model_cb bt_mesh_generic_admin_property_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL
extern const struct bt_mesh_model_op generic_manu_property_srv_op;
extern generic_property_info_t manu_property_info;
extern const struct bt_mesh_model_cb bt_mesh_generic_manufacturer_property_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL
extern const struct bt_mesh_model_op generic_user_property_srv_op;
extern generic_property_info_t user_property_info;
extern const struct bt_mesh_model_cb bt_mesh_generic_user_property_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_CLIENT_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_CLIENT_PROPERTY_SERVER_MODEL
extern const struct bt_mesh_model_op generic_client_property_srv_op;
extern const struct bt_mesh_model_cb bt_mesh_generic_client_property_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
extern const struct bt_mesh_model_op generic_property_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_generic_property_cli_cb;
#endif

#if ((defined(BT_MESH_ENABLE_SENSOR_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SERVER_MODEL) || \
    (defined(BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL))
#include <rtk_bt_mesh_sensor_model.h>
typedef struct {
	rtk_bt_mesh_sensor_db_t *sensors;
	uint16_t num_sensors;
} sensor_info_t;
extern sensor_info_t sensors_list;
#endif

#if defined(BT_MESH_ENABLE_SENSOR_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SERVER_MODEL
extern const struct bt_mesh_model_op sensor_srv_op;
extern int sensor_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(sensor_server_pub, sensor_srv_update_publish, BT_MESH_TX_SDU_MAX - 4);
extern const struct bt_mesh_model_cb bt_mesh_sensor_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL
extern const struct bt_mesh_model_op sensor_setup_srv_op;
extern int sensor_setup_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(sensor_server_setup_pub, sensor_setup_srv_update_publish, BT_MESH_TX_SDU_MAX - 4);
extern const struct bt_mesh_model_cb bt_mesh_sensor_setup_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
extern const struct bt_mesh_model_op sensor_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_sensor_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_TIME_SERVER_MODEL) && BT_MESH_ENABLE_TIME_SERVER_MODEL
extern const struct bt_mesh_model_op time_srv_op;
extern int time_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(time_server_pub, time_srv_update_publish, 2 + 10);
extern const struct bt_mesh_model_cb bt_mesh_time_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL
extern const struct bt_mesh_model_op time_setup_srv_op;
extern const struct bt_mesh_model_cb bt_mesh_time_setup_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
extern const struct bt_mesh_model_op time_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_time_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_SCENE_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SERVER_MODEL
extern const struct bt_mesh_model_op scene_srv_op;
extern int scene_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(scene_server_pub, scene_srv_update_publish, 2 + 3);
extern const struct bt_mesh_model_cb bt_mesh_scene_srv_cb;
#include <rtk_bt_mesh_scene_model.h>
typedef struct {
	rtk_bt_mesh_scene_storage_memory_t *scenes;
	uint16_t num_scenes;
} scene_info_t;

extern scene_info_t scene_model_info;
#endif

#if defined(BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL
extern const struct bt_mesh_model_op scene_setup_srv_op;
extern const struct bt_mesh_model_cb bt_mesh_scene_setup_srv_cb;
#include <rtk_bt_mesh_scene_model.h>
typedef struct {
	rtk_bt_mesh_scene_storage_memory_t *scenes;
	uint16_t num_scenes;
	rtk_bt_mesh_scene_status_code_t status_register;
} scene_setup_info_t;

extern scene_setup_info_t scene_setup_model_info;
#endif

#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
extern const struct bt_mesh_model_op scene_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_scene_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL) && BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL
extern const struct bt_mesh_model_op scheduler_srv_op;
extern int scheduler_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(scheduler_server_pub, scheduler_srv_update_publish, 2 + 2);
extern const struct bt_mesh_model_cb bt_mesh_scheduler_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL
extern const struct bt_mesh_model_op scheduler_setup_srv_op;
extern const struct bt_mesh_model_cb bt_mesh_scheduler_setup_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL) && BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL
extern const struct bt_mesh_model_op scheduler_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_scheduler_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_SERVER_MODEL
extern const struct bt_mesh_model_op light_lightness_srv_op;
extern int light_lightness_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(light_lightness_server_pub, light_lightness_srv_update_publish, 2 + 2);
extern const struct bt_mesh_model_cb bt_mesh_light_lightness_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL
extern const struct bt_mesh_model_op light_lightness_setup_srv_op;
extern const struct bt_mesh_model_cb bt_mesh_light_lightness_setup_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
extern const struct bt_mesh_model_op light_lightness_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_light_lightness_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_CTL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_SERVER_MODEL
extern const struct bt_mesh_model_op light_ctl_srv_op;
extern int light_ctl_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(light_ctl_server_pub, light_ctl_srv_update_publish, 2 + 4);
extern const struct bt_mesh_model_cb bt_mesh_light_ctl_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_CTL_TEMPERATURE_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_TEMPERATURE_SERVER_MODEL
extern const struct bt_mesh_model_op light_ctl_temp_srv_op;
extern int light_ctl_temp_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(light_ctl_temp_server_pub, light_ctl_temp_srv_update_publish, 2 + 4);
extern const struct bt_mesh_model_cb bt_mesh_light_ctl_temp_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_CTL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_SETUP_SERVER_MODEL
extern const struct bt_mesh_model_op light_ctl_setup_srv_op;
extern const struct bt_mesh_model_cb bt_mesh_light_ctl_setup_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
extern const struct bt_mesh_model_op light_ctl_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_light_ctl_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_HSL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SERVER_MODEL
extern const struct bt_mesh_model_op light_hsl_srv_op;
extern int light_hsl_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(light_hsl_server_pub, light_hsl_srv_update_publish, 2 + 6);
extern const struct bt_mesh_model_cb bt_mesh_light_hsl_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_HSL_HUE_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_HUE_SERVER_MODEL
extern const struct bt_mesh_model_op light_hsl_hue_srv_op;
extern int light_hsl_hue_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(light_hsl_hue_server_pub, light_hsl_hue_srv_update_publish, 2 + 2);
extern const struct bt_mesh_model_cb bt_mesh_light_hsl_hue_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_HSL_SATURATION_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SATURATION_SERVER_MODEL
extern const struct bt_mesh_model_op light_hsl_saturation_srv_op;
extern int light_hsl_saturation_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(light_hsl_saturation_server_pub, light_hsl_saturation_srv_update_publish, 2 + 2);
extern const struct bt_mesh_model_cb bt_mesh_light_hsl_saturation_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_HSL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SETUP_SERVER_MODEL
extern const struct bt_mesh_model_op light_hsl_setup_srv_op;
extern const struct bt_mesh_model_cb bt_mesh_light_hsl_setup_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
extern const struct bt_mesh_model_op light_hsl_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_light_hsl_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_XYL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_SERVER_MODEL
extern const struct bt_mesh_model_op light_xyl_srv_op;
extern int light_xyl_srv_update_publish(const struct bt_mesh_model *model);
BT_MESH_MODEL_PUB_DEFINE(light_xyl_server_pub, light_xyl_srv_update_publish, 2 + 6);
extern const struct bt_mesh_model_cb bt_mesh_light_xyl_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_XYL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_SETUP_SERVER_MODEL
extern const struct bt_mesh_model_op light_xyl_setup_srv_op;
extern const struct bt_mesh_model_cb bt_mesh_light_xyl_setup_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
extern const struct bt_mesh_model_op light_xyl_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_light_xyl_cli_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_LC_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LC_SERVER_MODEL
extern const struct bt_mesh_model_op light_lc_srv_op;
extern const struct bt_mesh_model_cb bt_mesh_light_lc_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_LC_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LC_SETUP_SERVER_MODEL
extern const struct bt_mesh_model_op light_lc_setup_srv_op;
extern const struct bt_mesh_model_cb bt_mesh_light_lc_setup_srv_cb;
#endif

#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
extern const struct bt_mesh_model_op light_lc_cli_op;
extern const struct bt_mesh_model_cb bt_mesh_light_lc_cli_cb;
#endif

#if defined(CONFIG_BT_MESH_RPR_CLI) && CONFIG_BT_MESH_RPR_CLI
extern struct bt_mesh_rpr_cli rpr_cli;
#endif

#if defined(CONFIG_BT_MESH_PRIV_BEACON_CLI) && CONFIG_BT_MESH_PRIV_BEACON_CLI
extern struct bt_mesh_priv_beacon_cli prb_cli;
#endif

#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
extern struct bt_mesh_dfu_cli dfu_client;
#endif

#if defined(BT_MESH_ENABLE_DFU_TARGET_ROLE) && BT_MESH_ENABLE_DFU_TARGET_ROLE
extern struct bt_mesh_dfu_srv dfu_server;
#endif

static struct bt_mesh_model root_models[] = {
	BT_MESH_MODEL_CFG_SRV,
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	BT_MESH_MODEL_CFG_CLI(&cfg_cli),
	BT_MESH_MODEL_HEALTH_CLI(&health_cli),
#if defined(CONFIG_BT_MESH_RPR_CLI) && CONFIG_BT_MESH_RPR_CLI
	BT_MESH_MODEL_RPR_CLI(&rpr_cli),
#endif
#if defined(CONFIG_BT_MESH_PRIV_BEACON_CLI) && CONFIG_BT_MESH_PRIV_BEACON_CLI
	BT_MESH_MODEL_PRIV_BEACON_CLI(&prb_cli),
#endif
#endif
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
	BT_MESH_MODEL_HEALTH_SRV(&health_server, &health_pub),
#if defined(CONFIG_BT_MESH_RPR_SRV) && CONFIG_BT_MESH_RPR_SRV
	BT_MESH_MODEL_RPR_SRV,
#endif
#if defined(CONFIG_BT_MESH_PRIV_BEACON_SRV) && CONFIG_BT_MESH_PRIV_BEACON_SRV
	BT_MESH_MODEL_PRIV_BEACON_SRV,
#endif
#endif
#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_srv_op, &gen_onoff_pub_srv, NULL, &bt_mesh_gen_onoff_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, gen_onoff_cli_op, NULL, NULL, &bt_mesh_gen_onoff_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_LEVEL_SRV, &generic_level_srv_op, &generic_level_server_pub, NULL, &bt_mesh_generic_level_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_LEVEL_CLI, &generic_level_cli_op, NULL, NULL, &bt_mesh_generic_level_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSTION_TIME_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSTION_TIME_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV, &generic_default_transition_time_srv_op, &generic_default_transition_time_server_pub, NULL, &bt_mesh_generic_default_transition_time_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI, &generic_default_transition_time_cli_op, NULL, NULL, &bt_mesh_generic_default_transition_time_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV, &generic_power_onoff_srv_op, &generic_power_on_off_server_pub, NULL, &bt_mesh_generic_power_on_off_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV, &generic_power_onoff_setup_srv_op, NULL, NULL, &bt_mesh_generic_power_on_off_setup_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI, &generic_power_onoff_cli_op, NULL, NULL, &bt_mesh_generic_power_on_off_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV, &generic_power_level_srv_op, &generic_power_level_server_pub, NULL, &bt_mesh_generic_power_level_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV, &generic_power_level_setup_srv_op, NULL, NULL, &bt_mesh_generic_power_level_setup_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI, &generic_power_level_cli_op, NULL, NULL, &bt_mesh_generic_power_level_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_BATTERY_SRV, &generic_battery_srv_op, &generic_battery_server_pub, NULL, &bt_mesh_generic_battery_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_BATTERY_CLI, &generic_battery_cli_op, NULL, NULL, &bt_mesh_generic_battery_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_LOCATION_SRV, &generic_location_srv_op, &generic_location_server_pub, NULL, &bt_mesh_generic_location_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_LOCATION_SETUPSRV, &generic_location_setup_srv_op, NULL, NULL, &bt_mesh_generic_location_setup_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_LOCATION_CLI, &generic_location_cli_op, NULL, NULL, &bt_mesh_generic_location_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV, &generic_admin_property_srv_op, NULL, &admin_property_info, &bt_mesh_generic_admin_property_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV, &generic_manu_property_srv_op, NULL, &manu_property_info, &bt_mesh_generic_manufacturer_property_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_USER_PROP_SRV, &generic_user_property_srv_op, NULL, &user_property_info, &bt_mesh_generic_user_property_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_CLIENT_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_CLIENT_PROPERTY_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV, &generic_client_property_srv_op, NULL, NULL, &bt_mesh_generic_client_property_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_GEN_PROP_CLI, &generic_property_cli_op, NULL, NULL, &bt_mesh_generic_property_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_SENSOR_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SENSOR_SRV, &sensor_srv_op, &sensor_server_pub, (void *) &sensors_list, &bt_mesh_sensor_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SENSOR_SETUP_SRV, &sensor_setup_srv_op, &sensor_server_setup_pub, (void *) &sensors_list, &bt_mesh_sensor_setup_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SENSOR_CLI, &sensor_cli_op, NULL, NULL, &bt_mesh_sensor_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_TIME_SERVER_MODEL) && BT_MESH_ENABLE_TIME_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_TIME_SRV, &time_srv_op, &time_server_pub, NULL, &bt_mesh_time_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_TIME_SETUP_SRV, &time_setup_srv_op, NULL, NULL, &bt_mesh_time_setup_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_TIME_CLI, &time_cli_op, NULL, NULL, &bt_mesh_time_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_SCENE_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SCENE_SRV, &scene_srv_op, &scene_server_pub, (void *) &scene_model_info, &bt_mesh_scene_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SCENE_SETUP_SRV, &scene_setup_srv_op, NULL, (void *) &scene_setup_model_info, &bt_mesh_scene_setup_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SCENE_CLI, &scene_cli_op, NULL, NULL, &bt_mesh_scene_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL) && BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SCHEDULER_SRV, &scheduler_srv_op, &scheduler_server_pub, NULL, &bt_mesh_scheduler_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SCHEDULER_SETUP_SRV, &scheduler_setup_srv_op, NULL, NULL, &bt_mesh_scheduler_setup_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL) && BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_SCHEDULER_CLI, &scheduler_cli_op, NULL, NULL, &bt_mesh_scheduler_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV, &light_lightness_srv_op, &light_lightness_server_pub, NULL, &bt_mesh_light_lightness_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV, &light_lightness_setup_srv_op, NULL, NULL, &bt_mesh_light_lightness_setup_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI, &light_lightness_cli_op, NULL, NULL, &bt_mesh_light_lightness_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_CTL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_CTL_SRV, &light_ctl_srv_op, &light_ctl_server_pub, NULL, &bt_mesh_light_ctl_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_CTL_TEMPERATURE_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_TEMPERATURE_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV, &light_ctl_temp_srv_op, &light_ctl_temp_server_pub, NULL, &bt_mesh_light_ctl_temp_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_CTL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_SETUP_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV, &light_ctl_setup_srv_op, NULL, NULL, &bt_mesh_light_ctl_setup_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_CTL_CLI, &light_ctl_cli_op, NULL, NULL, &bt_mesh_light_ctl_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_HSL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_HSL_SRV, &light_hsl_srv_op, &light_hsl_server_pub, NULL, &bt_mesh_light_hsl_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_HSL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SETUP_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV, &light_hsl_setup_srv_op, NULL, NULL, &bt_mesh_light_hsl_setup_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_HSL_HUE_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_HUE_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV, &light_hsl_hue_srv_op, &light_hsl_hue_server_pub, NULL, &bt_mesh_light_hsl_hue_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_HSL_SATURATION_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SATURATION_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV, &light_hsl_saturation_srv_op, &light_hsl_saturation_server_pub, NULL, &bt_mesh_light_hsl_saturation_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_HSL_CLI, &light_hsl_cli_op, NULL, NULL, &bt_mesh_light_hsl_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_XYL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_XYL_SRV, &light_xyl_srv_op, &light_xyl_server_pub, NULL, &bt_mesh_light_xyl_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_XYL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_SETUP_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV, &light_xyl_setup_srv_op, NULL, NULL, &bt_mesh_light_xyl_setup_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_XYL_CLI, &light_xyl_cli_op, NULL, NULL, &bt_mesh_light_xyl_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LC_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LC_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_LC_SRV, &light_lc_srv_op, NULL, NULL, &bt_mesh_light_lc_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LC_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LC_SETUP_SERVER_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_LC_SETUPSRV, &light_lc_setup_srv_op, NULL, NULL, &bt_mesh_light_lc_setup_srv_cb),
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_LIGHT_LC_CLI, &light_lc_cli_op, NULL, NULL, &bt_mesh_light_lc_cli_cb),
#endif
#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
	BT_MESH_MODEL_DFU_CLI(&dfu_client),
#endif
#if defined(BT_MESH_ENABLE_DFU_TARGET_ROLE) && BT_MESH_ENABLE_DFU_TARGET_ROLE
	BT_MESH_MODEL_DFU_SRV(&dfu_server),
#endif
};

#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
extern const struct bt_mesh_model_op datatrans_op;
extern const struct bt_mesh_model_cb bt_mesh_datatrans_cb;
#endif

#define BT_MESH_MODEL_ID_DATATRANS 0x0004

#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
static struct bt_mesh_model vnd_models[] = {
	BT_MESH_MODEL_VND_CB(COMPANY_ID, BT_MESH_MODEL_ID_DATATRANS, &datatrans_op,
						 NULL, NULL, &bt_mesh_datatrans_cb),
};
#endif

static struct bt_mesh_elem elements[] = {
#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
	BT_MESH_ELEM(0, root_models, vnd_models),
#else
	BT_MESH_ELEM(0, root_models, BT_MESH_MODEL_NONE),
#endif
};

static const struct bt_mesh_comp comp = {
	.cid = COMPANY_ID,
	.elem = elements,
	.elem_count = ARRAY_SIZE(elements),
};

const struct bt_mesh_comp *bt_mesh_get_composition_data(void)
{
	return &comp;
}
