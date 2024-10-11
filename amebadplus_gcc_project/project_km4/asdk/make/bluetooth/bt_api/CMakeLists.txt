set(DIR ${BASEDIR}/component/bluetooth)
set(BT_API_DIR ${DIR}/api)

set(
    CSRC 
	${BT_API_DIR}/rtk_bt_common.c
	${BT_API_DIR}/rtk_bt_device.c
	${BT_API_DIR}/rtk_bt_le_gap.c
	${BT_API_DIR}/rtk_bt_gap.c
	${BT_API_DIR}/rtk_bt_gatt.c
	${BT_API_DIR}/rtk_bt_vendor.c
	${BT_API_DIR}/rtk_bt_power_control.c
	${BT_API_DIR}/rtk_bt_mesh_common.c
	${BT_API_DIR}/rtk_bt_mesh_config_client_model.c
	${BT_API_DIR}/rtk_bt_mesh_datatrans_model.c
	${BT_API_DIR}/rtk_bt_mesh_generic_model.c
	${BT_API_DIR}/rtk_bt_mesh_generic_onoff_model.c
	${BT_API_DIR}/rtk_bt_mesh_generic_default_transition_time.c
	${BT_API_DIR}/rtk_bt_mesh_light_model.c
	${BT_API_DIR}/rtk_bt_mesh_time_model.c
	${BT_API_DIR}/rtk_bt_mesh_scheduler_model.c
	${BT_API_DIR}/rtk_bt_mesh_scene_model.c
	${BT_API_DIR}/rtk_bt_mesh_remote_prov_model.c
	${BT_API_DIR}/rtk_bt_mesh_health_model.c
	${BT_API_DIR}/rtk_bt_mesh_sensor_model.c
    ${BT_API_DIR}/rtk_bt_mesh_directed_forwarding_model.c
)

if(CONFIG_BT_ZEPHYR)
    list(
        APPEND CSRC 
        ${BT_API_DIR}/zephyr_stack/zephyr_stack_api.c
        ${BT_API_DIR}/zephyr_stack/zephyr_stack_resource.c
        ${BT_API_DIR}/zephyr_stack/zephyr_stack_le_gap.c
        ${BT_API_DIR}/zephyr_stack/zephyr_stack_gattc.c
        ${BT_API_DIR}/zephyr_stack/zephyr_stack_gatts.c
    )
else()
    list(
        APPEND CSRC 
        ${BT_API_DIR}/rtk_stack/rtk_stack_api.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_le_gap.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_gap.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_gattc.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_gatts.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_vendor.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_common.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_config_client_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_datatrans_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_remote_prov_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_generic_onoff_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_light_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_time_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_scheduler_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_scene_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_generic_default_transition_time.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_generic_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_health_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_sensor_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_directed_forwarding_model.c
    )
endif()

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
