set(BT_DIR ${BASEDIR}/component/bluetooth)

set(
    CSRC 
    ${BT_DIR}/rtk_stack/src/mesh/cmd/mesh_cmd.c 
	${BT_DIR}/rtk_stack/src/mesh/model/delay_msg_rsp.c 
	${BT_DIR}/rtk_stack/src/mesh/model/realtek/datatrans_model.c 
	${BT_DIR}/rtk_stack/src/mesh/utility/mesh_data_dump.c
)

if(CONFIG_BT_MESH_PROVISIONER_SUPPORT)
    list(
        APPEND CSRC 
        ${BT_DIR}/rtk_stack/src/mesh/model/configuration_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_battery_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_default_transition_time_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_level_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_location_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_on_off_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_power_level_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_power_on_off_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_property_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_ctl_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_hsl_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_lc_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_lightness_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_xyl_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/scene_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/scheduler_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/time_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/sensor_client.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/health_client.c
    )
endif()

if(CONFIG_BT_MESH_DEVICE_SUPPORT)
    list(
        APPEND CSRC 
        ${BT_DIR}/rtk_stack/src/mesh/model/delay_execution.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_battery_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_default_transition_time_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_level_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_location_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_location_setup_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_on_off_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_power_level_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_power_level_setup_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_power_on_off_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_power_on_off_setup_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_transition_time.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_user_property_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_admin_property_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_manufacturer_property_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/generic_client_property_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/health_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_ctl_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_ctl_setup_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_ctl_temperature_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_hsl_hue_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_hsl_saturation_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_hsl_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_hsl_setup_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_lc_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_lc_setup_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_lightness_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_lightness_setup_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_xyl_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/light_xyl_setup_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/scene_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/scene_setup_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/scheduler_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/scheduler_setup_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/time_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/time_setup_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/sensor_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/model/sensor_setup_server.c 
        ${BT_DIR}/rtk_stack/src/mesh/utility/mesh_data_dump.c
    )
endif()

target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})