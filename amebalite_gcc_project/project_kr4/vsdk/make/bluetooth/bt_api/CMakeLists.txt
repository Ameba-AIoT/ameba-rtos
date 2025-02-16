set(DIR ${BASEDIR}/component/bluetooth)
set(BT_API_DIR ${DIR}/api)
set(BT_AUDIO_DIR ${DIR}/bt_audio)
set(BT_ZEPHYR_DIR ${BASEDIR}/component/bluetooth/zephyr)

set(
    MODULE_IFLAGS
    ${BT_AUDIO_DIR}/include
)
if(CONFIG_BT_ZEPHYR)
    list(APPEND MODULE_IFLAGS ${BT_ZEPHYR_DIR}/stack/mesh)
endif()

set(
    CSRC
	${BT_API_DIR}/rtk_bt_common.c
	${BT_API_DIR}/rtk_bt_device.c
	${BT_API_DIR}/rtk_bt_le_gap.c
	${BT_API_DIR}/rtk_bt_gap.c
	${BT_API_DIR}/rtk_bt_gatt.c
	${BT_API_DIR}/rtk_bt_vendor.c
	${BT_API_DIR}/rtk_bt_power_control.c
	${BT_API_DIR}/rtk_bt_br_gap.c
	${BT_API_DIR}/rtk_bt_avrcp.c
	${BT_API_DIR}/rtk_bt_a2dp.c
	${BT_API_DIR}/rtk_bt_sdp.c
	${BT_API_DIR}/rtk_bt_spp.c
    ${BT_API_DIR}/rtk_bt_rfc.c
	${BT_API_DIR}/rtk_bt_hid.c
	${BT_API_DIR}/rtk_bt_hfp.c
	${BT_API_DIR}/rtk_bt_pbap.c
	${BT_API_DIR}/rtk_bt_mesh_common.c
	${BT_API_DIR}/rtk_bt_mesh_config_client_model.c
	${BT_API_DIR}/rtk_bt_mesh_datatrans_model.c
	${BT_API_DIR}/rtk_bt_mesh_device_firmware_update_model.c
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
	${BT_API_DIR}/rtk_bt_mesh_subnet_bridge_model.c
	${BT_API_DIR}/rtk_bt_mesh_private_beacon_model.c
	${BT_API_DIR}/rtk_bt_le_iso.c
	${BT_API_DIR}/rtk_bt_bap.c
	${BT_API_DIR}/rtk_bt_cap.c
	${BT_API_DIR}/rtk_bt_tmap.c
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
    if(CONFIG_BT_MESH_PROVISIONER_SUPPORT OR CONFIG_BT_MESH_DEVICE_SUPPORT)
        list(
            APPEND CSRC
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_common.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_configuration_model.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_datatrans_model.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_generic_onoff_model.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_light_model.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_scene_model.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_models_handler.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_sensor_model.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_time_model.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_scheduler_model.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_generic_default_transition_time.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_generic_model.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_health_model.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_remote_provision_model.c
            ${BT_API_DIR}/zephyr_stack/zephyr_stack_mesh_private_beacon_model.c
        )
    endif()
else()
    list(
        APPEND CSRC
        ${BT_API_DIR}/rtk_stack/rtk_stack_api.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_le_gap.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_br_gap.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_gap.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_gattc.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_gatts.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_vendor.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_avrcp.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_a2dp.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_sdp.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_spp.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_rfc.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_hid.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_hfp.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_pbap.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_common.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_config_client_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_datatrans_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_device_firmware_update_model.c
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
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_subnet_bridge_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_mesh_private_beacon_model.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_iso.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_le_audio.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_bap.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_cap.c
        ${BT_API_DIR}/rtk_stack/rtk_stack_tmap.c
    )
endif()

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
