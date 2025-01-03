if (__INCLUDED_BLUETOOTH_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_BLUETOOTH_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_bsp INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_audio INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_audio_common INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_central INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_hogp_gamepad INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_iso INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_device INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_device_scatternet INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_device_test INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_ota_common INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_provisioner INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_provisioner_scatternet INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_provisioner_test INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_stack INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_peripheral INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_scatternet INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_throughput INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp_hfp INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp_pbp INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp_scatternet INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp_tmap INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_api INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_app_audio_data INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_atcmd INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_audio INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_audio_codec INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_audio_sync INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_audio_mp_test INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_coex INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_demo INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_hfp INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_hid INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_host INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_inic INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_rfc INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_spp INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_zephyr INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_bluetooth_example INTERFACE)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_bsp p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/driver/hci/hci_process/hci_process.c
    ${c_CMPT_BLUETOOTH_DIR}/driver/hci/hci_process/hci_download_v2.c
    ${c_CMPT_BLUETOOTH_DIR}/driver/hci/hci_process/hci_download_v3.c
    ${c_CMPT_BLUETOOTH_DIR}/driver/hci/hci_process/hci_common.c
    ${c_CMPT_BLUETOOTH_DIR}/driver/hci/hci_process/hci_standalone.c
    ${c_CMPT_BLUETOOTH_DIR}/driver/hci/hci_transport/hci_h4.c
    ${c_CMPT_BLUETOOTH_DIR}/driver/platform/${d_PLATFORM_TYPE}/hci/bt_normal_patch.c
    ${c_CMPT_BLUETOOTH_DIR}/driver/platform/${d_PLATFORM_TYPE}/hci/bt_mp_patch.c
    ${c_CMPT_BLUETOOTH_DIR}/driver/platform/${d_PLATFORM_TYPE}/hci/hci_platform.c
    ${c_CMPT_BLUETOOTH_DIR}/driver/platform/${d_PLATFORM_TYPE}/hci/hci_uart.c
    ${c_CMPT_BLUETOOTH_DIR}/osif/bt_debug.c
    ${c_CMPT_BLUETOOTH_DIR}/osif/osif.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_mp/bt_mp_api.c
)

if (NOT CONFIG_BT_INIC)
    if (CONFIG_BT_ZEPHYR)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_bsp p_SCOPE interface
            ${c_CMPT_BLUETOOTH_DIR}/driver/hci/hci_if_zephyr.c
        )
    else()
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_bsp p_SCOPE interface
            ${c_CMPT_BLUETOOTH_DIR}/driver/hci/hci_if_rtk.c
            ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/common/platform_utils.c
            ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/common/trace_uart.c
        )
    endif()
endif()
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_audio p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_audio/bap.c
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_audio/cap.c
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_audio/pbp.c
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_audio/tmap.c
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_audio/gmap.c
)

ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_audio p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_audio_common
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_app_audio_data
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_audio_common p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_audio_common/app_bt_le_audio_common.c
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_central p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_central/central.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_gcs_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_bas_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_gaps_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_simple_ble_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_cte_client.c
)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_hogp_gamepad p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_hogp_gamepad
)

ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_hogp_gamepad p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_hogp_gamepad/hogp_gamepad_main.c
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_hogp_gamepad/hogp_dis_gp.c
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_hogp_gamepad/hogp_hids_gp.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_bas.c
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_iso p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_iso/le_iso.c
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_iso/app_le_iso_common.c
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_device p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_mesh_device/mesh_device.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_device_scatternet p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_mesh_device_scatternet/mesh_device_scatternet.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_bas_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_gaps_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_gcs_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_simple_ble_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_bas.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_dis.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_gls.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_hids_kb.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_hrs.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_ias.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_long_uuid_service.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_simple_ble_service.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_device_test p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_mesh_device/mesh_device_test.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_ota_common p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_mesh_ota_common/rtk_bt_device_firmware_update_interface.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_provisioner p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_mesh_provisioner/mesh_provisioner.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_provisioner_scatternet p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_mesh_provisioner_scatternet/mesh_provisioner_scatternet.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_bas_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_gaps_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_gcs_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_simple_ble_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_bas.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_dis.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_gls.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_hids_kb.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_hrs.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_ias.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_long_uuid_service.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_simple_ble_service.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_provisioner_test p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_mesh_provisioner_test/mesh_provisioner_test.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_performence_test.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_stack p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/cmd/mesh_cmd.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/common/blob_client_app.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/common/dfu_distributor_app.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/common/dfu_initiator_app.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/blob_transfer_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/delay_msg_rsp.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/firmware_distribution_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/firmware_update_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/realtek/datatrans_model.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/utility/mesh_data_dump.c
)
ameba_target_sources_if(CONFIG_BT_MESH_PROVISIONER_SUPPORT src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_stack p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/configuration_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_battery_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_default_transition_time_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_level_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_location_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_on_off_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_power_level_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_power_on_off_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_property_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_ctl_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_hsl_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_lc_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_lightness_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_xyl_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/scene_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/scheduler_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/time_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/sensor_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/health_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/directed_forwarding_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/private_beacon_client.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/subnet_bridge_client.c
)
ameba_target_sources_if(CONFIG_BT_MESH_DEVICE_SUPPORT src_${d_MCU_PROJECT_NAME}_bluetooth_ble_mesh_stack p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/delay_execution.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_battery_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_default_transition_time_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_level_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_location_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_location_setup_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_on_off_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_power_level_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_power_level_setup_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_power_on_off_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_power_on_off_setup_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_transition_time.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_user_property_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_admin_property_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_manufacturer_property_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/generic_client_property_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/health_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_ctl_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_ctl_setup_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_ctl_temperature_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_hsl_hue_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_hsl_saturation_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_hsl_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_hsl_setup_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_lc_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_lc_setup_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_lightness_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_lightness_setup_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_xyl_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/light_xyl_setup_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/scene_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/scene_setup_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/scheduler_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/scheduler_setup_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/time_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/time_setup_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/sensor_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/sensor_setup_server.c
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/utility/mesh_data_dump.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_peripheral p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_peripheral/peripheral.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_bas.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_simple_ble_service.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_hrs.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_dis.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_ias.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_hids_kb.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_gls.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_long_uuid_service.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_cte_service.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_scatternet p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_scatternet/scatternet.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_bas.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_simple_ble_service.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_hrs.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_dis.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_ias.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_hids_kb.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_gls.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_long_uuid_service.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_gcs_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_bas_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_gaps_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_simple_ble_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_cte_service.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_cte_client.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_throughput p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_throughput/throughput_main.c
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_throughput/throughput_server.c
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_throughput/throughput_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_throughput/throughput_common.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_throughput_test.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_gcs_client.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_ble_throughput p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_throughput
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_a2dp/a2dp.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_a2dp
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_app_audio_data
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/include
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp_hfp p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_a2dp_hfp/a2dp_hfp.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp_hfp p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_a2dp_hfp
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_app_audio_data
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/include
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp_pbp p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_a2dp_pbp/a2dp_pbp.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp_pbp p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_a2dp_hfp
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_app_audio_data
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/include
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_audio_common
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp_scatternet p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_a2dp_scatternet/a2dp_scatternet.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_bas.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_simple_ble_service.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_hrs.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_dis.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_ias.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_hids_kb.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_gls.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_long_uuid_service.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_gcs_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_bas_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_gaps_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_simple_ble_client.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/server/rtk_cte_service.c
    ${c_CMPT_BLUETOOTH_DIR}/example/gatt_service/client/rtk_cte_client.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp_scatternet p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_a2dp_scatternet
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_app_audio_data
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/include
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp_tmap p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_a2dp_tmap/a2dp_tmap.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_a2dp_tmap/tmap_a2dp_source.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_a2dp_tmap p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_app_audio_data
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/include
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_audio_common
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_api p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_common.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_device.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_le_gap.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_gap.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_gatt.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_vendor.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_power_control.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_br_gap.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_avrcp.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_a2dp.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_sdp.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_spp.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_rfc.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_hid.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_hfp.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_pbap.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_common.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_config_client_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_datatrans_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_device_firmware_update_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_generic_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_generic_onoff_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_generic_default_transition_time.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_light_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_time_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_scheduler_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_scene_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_directed_forwarding_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_remote_prov_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_health_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_sensor_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_subnet_bridge_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_mesh_private_beacon_model.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_le_iso.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_le_audio.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_bap.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_cap.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_pbp.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_tmap.c
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_bt_gmap.c
)

if (CONFIG_BT_ZEPHYR)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_api p_SCOPE interface
        ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_api.c
        ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_resource.c
        ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_le_gap.c
        ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_gattc.c
        ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_gatts.c
    )
    if (CONFIG_BT_MESH_PROVISIONER_SUPPORT OR CONFIG_BT_MESH_DEVICE_SUPPORT)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_api p_SCOPE interface
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_common.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_configuration_model.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_datatrans_model.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_generic_onoff_model.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_models_handler.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_sensor_model.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_light_model.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_scene_model.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_time_model.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_scheduler_model.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_generic_default_transition_time.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_generic_model.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_health_model.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_remote_provision_model.c
            ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack/zephyr_stack_mesh_private_beacon_model.c
        )
    endif()
else()
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_api p_SCOPE interface
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_api.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_le_gap.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_br_gap.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_gap.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_gattc.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_gatts.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_vendor.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_avrcp.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_a2dp.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_sdp.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_spp.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_rfc.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_hid.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_hfp.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_pbap.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_common.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_config_client_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_datatrans_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_device_firmware_update_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_remote_prov_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_generic_onoff_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_light_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_time_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_scheduler_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_scene_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_generic_default_transition_time.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_generic_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_directed_forwarding_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_health_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_sensor_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_private_beacon_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_mesh_subnet_bridge_model.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_iso.c
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack/rtk_stack_le_audio.c
    )
endif()

ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_api p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/include
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_audio_common
)
ameba_target_include_if(CONFIG_BT_ZEPHYR src_${d_MCU_PROJECT_NAME}_bluetooth_api p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_app_audio_data p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_app_audio_data/app_audio_data.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_app_audio_data p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_app_audio_data
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_audio_common
    ${c_CMPT_USB_DIR}/common
    ${c_CMPT_USB_DIR}/device/core
    ${c_CMPT_USB_DIR}/device/uac
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_atcmd p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/bt_utils.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_common.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_example.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_gap.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_le_gap.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_br_gap.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_gatts.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_gattc.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_vendor.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_sdp.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_a2dp.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_avrcp.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_spp.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_rfc.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_hid.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_hfp.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_iso.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_pbap.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_bap.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_cap.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_pbp.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_tmap.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_gmap.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_common.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_config_model.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_datatrans_model.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_device_firmware_update_model.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_goo_model.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_remote_prov_model.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_light_model.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_time_model.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_scheduler_model.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_scene_model.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_generic_default_transition_time.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_generic_model.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_sensor_model.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_health_model.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_directed_forwarding.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_private_beacon_model.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_atcmd/atcmd_bt_mesh_subnet_bridge_model.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_atcmd p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_iso
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_audio_common
    $<TARGET_PROPERTY:dep_${d_MCU_PROJECT_NAME}_at_cmd,INTERFACE_INCLUDE_DIRECTORIES>
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_audio p_SCOPE interface
	${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_audio_intf.c
	${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_audio_resample.c
	${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_audio_track_api.c
	${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_audio_record_api.c
	${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_audio_codec_wrapper.c
	${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc_codec_entity.c
	${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3_codec_entity.c
	${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/cvsd_codec_entity.c
)

ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_audio p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/example/ble_audio
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/leaudio
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/audio_sync
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libAACdec/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libAACenc/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libPCMutils/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libFDK/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libSYS/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libMpegTPDec/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libMpegTPEnc/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libSBRdec/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libSBRenc/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libArithCoding/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libDRCdec/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libSACdec/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libSACenc/include
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_audio_codec p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/alloc.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/bitalloc.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/bitalloc-sbc.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/bitstream-decode.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/decoder-oina.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/decoder-private.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/decoder-sbc.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/dequant.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/framing.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/framing-sbc.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/oi_codec_version.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/synthesis-8-generated.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/synthesis-dct8.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/srce/synthesis-sbc.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/srce/sbc_analysis.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/srce/sbc_dct.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/srce/sbc_dct_coeffs.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/srce/sbc_enc_bit_alloc_mono.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/srce/sbc_enc_bit_alloc_ste.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/srce/sbc_enc_coeffs.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/srce/sbc_encoder.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/srce/sbc_packing.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src/attdet.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src/bits.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src/bwdet.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src/energy.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src/lc3.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src/ltpf.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src/mdct.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src/plc.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src/sns.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src/spec.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src/tables.c
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src/tns.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_audio_codec p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/lc3/src

    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/example/ble_audio
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/leaudio
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/audio_sync
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libAACdec/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libAACenc/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libPCMutils/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libFDK/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libSYS/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libMpegTPDec/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libMpegTPEnc/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libSBRdec/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libSBRenc/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libArithCoding/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libDRCdec/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libSACdec/include
    # ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/aac/libSACenc/include
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_bluetooth_audio_codec p_SCOPE interface
    HAVE_ALLOCA_H
    DISABLE_FLOAT_API
    FIXED_POINT
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_audio_sync p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/audio_sync/bt_audio_sync.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_audio_sync p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/audio_sync
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_audio_mp_test p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_audio_mp_test/bt_audio_mp_test.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_audio_mp_test p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_audio_mp_test
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_app_audio_data
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/decoder/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec/sbc/encoder/include
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_coex p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/rtk_coex/rtk_coex.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_coex p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/rtk_coex
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_demo p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/bt_demo/bt_demo.c
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_hfp p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_hfp/hfp.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_hfp p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_hfp
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_app_audio_data
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/include
    ${c_CMPT_BLUETOOTH_DIR}/bt_audio/bt_codec
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_hid p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_hid/hid.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_hid/gamepad_des.c
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_hid/mouse_des.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_hid p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_hid
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_host p_SCOPE interface
	${c_CMPT_BLUETOOTH_DIR}/osif/osif.c
	${c_CMPT_BLUETOOTH_DIR}/osif/bt_debug.c
	${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/common/platform_utils.c
	${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/common/trace_uart.c
	${c_CMPT_BLUETOOTH_DIR}/example/bt_host/rtos_driver/rtk_bt_host_api.c
	${c_CMPT_BLUETOOTH_DIR}/example/bt_host/rtos_driver/rtk_stack_example.c
	${c_CMPT_BLUETOOTH_DIR}/example/bt_host/rtos_driver/h4.c
	${c_CMPT_BLUETOOTH_DIR}/example/bt_host/rtos_driver/uart_if.c
	${c_CMPT_BLUETOOTH_DIR}/example/bt_host/rtos_driver/os_if.c
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_inic p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/driver/hci/hci_if_inic.c
)
ameba_target_sources_if(CONFIG_BT_INIC_H4 src_${d_MCU_PROJECT_NAME}_bluetooth_inic p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/driver/bt_inic/bt_inic_h4.c
)
ameba_target_sources_if(CONFIG_BT_INIC_H5 src_${d_MCU_PROJECT_NAME}_bluetooth_inic p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/driver/bt_inic/bt_inic_h5.c
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_rfc p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_rfc/rfc.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_rfc p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_rfc
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_spp p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_spp/spp.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_spp p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_spp
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_zephyr p_SCOPE interface
	${c_CMPT_BLUETOOTH_DIR}/zephyr/hci_driver/buf.c

	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/common/addr.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/common/rpa.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/common/bt_str.c

	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/addr.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/buf.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/hci_core.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/id.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/crypto_tc.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/conn.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/scan.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/adv.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/l2cap.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/smp.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/att.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/gatt.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/ecc.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/keys.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/uuid.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/hci_ecc.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/long_wq.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/settings.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/aes_ccm.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/direction.c
)
ameba_target_sources_if(CONFIG_BT_DUAL_MODE src_${d_MCU_PROJECT_NAME}_bluetooth_zephyr p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/classic/br.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/classic/sco.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/classic/sdp.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/classic/ssp.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/classic/a2dp.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/classic/avdtp.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/classic/keys_br.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/classic/l2cap_br.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack_ext/avctp.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack_ext/avrcp.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack_ext/spp.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/classic/rfcomm.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/classic/hfp_hf.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/classic/hfp_ag.c
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host/classic/at.c
)

if (CONFIG_BT_MESH_PROVISIONER_SUPPORT OR CONFIG_BT_MESH_DEVICE_SUPPORT)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_zephyr p_SCOPE interface
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/access.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/adv.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/adv_ext.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/adv_legacy.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/app_keys.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/beacon.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/cdb.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/cfg.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/cfg_cli.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/cfg_srv.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/crypto.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/crypto_tc.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/friend.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/gatt_cli.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/health_cli.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/health_srv.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/heartbeat.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/main.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/msg.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/net.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/pb_adv.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/pb_gatt.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/pb_gatt_cli.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/pb_gatt_srv.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/proxy_cli.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/proxy_srv.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/proxy_msg.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/prov.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/provisioner.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/rpl.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/settings.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/subnet.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/transport.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/va.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/rpr_cli.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/rpr_srv.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/priv_beacon_cli.c
        ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/priv_beacon_srv.c
    )
    if(${d_PLATFORM_TYPE} STREQUAL "amebasmart" AND "${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_zephyr p_SCOPE interface
            ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/provisionee.c
        )
    endif()
endif()

ameba_target_sources_if(CONFIG_BT_MESH_DEVICE_SUPPORT src_${d_MCU_PROJECT_NAME}_bluetooth_zephyr p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh/lpn.c
)

ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bluetooth_zephyr p_SCOPE interface
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/crypto/bt_crypto.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/crypto/bt_crypto_tc.c

	${c_CMPT_BLUETOOTH_DIR}/zephyr/tinycrypt/cmac_mode.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/tinycrypt/aes_encrypt.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/tinycrypt/utils.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/tinycrypt/ecc.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/tinycrypt/ecc_dh.c

	${c_CMPT_BLUETOOTH_DIR}/zephyr/port/zephyr_log.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/port/zephyr_queue.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/port/zephyr_time.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/port/zephyr_thread.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/port/zephyr_work.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/port/zephyr_sema.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/port/zephyr_mutex.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/port/zephyr_poll.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/port/zephyr_mem.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/port/zephyr_flash.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/port/zephyr_sys.c

	${c_CMPT_BLUETOOTH_DIR}/zephyr/settings/settings.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/settings/settings_store.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/settings/settings_line.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/settings/settings_init.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/settings/settings_nvs.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/settings/nvs.c

#	${c_CMPT_BLUETOOTH_DIR}/zephyr/settings/settings_ftl.c

	${c_CMPT_BLUETOOTH_DIR}/zephyr/util/dec.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/util/hex.c
	${c_CMPT_BLUETOOTH_DIR}/zephyr/util/crc8_sw.c
)

ameba_target_include(src_${d_MCU_PROJECT_NAME}_bluetooth_zephyr p_SCOPE interface
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/include/syscalls
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/include/zephyr/sys
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/include/zephyr/tracing
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack
)

ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_bluetooth_zephyr p_SCOPE interface
    -Wno-error
)
#--------------------------#