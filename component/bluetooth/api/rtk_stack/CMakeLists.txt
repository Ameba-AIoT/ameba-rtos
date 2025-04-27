##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

ameba_list_append(private_sources
    rtk_stack_api.c
    rtk_stack_le_gap.c
    rtk_stack_gap.c
    rtk_stack_gattc.c
    rtk_stack_gatts.c
    rtk_stack_vendor.c
)

ameba_list_append(private_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/example
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/app
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/os
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/platform
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/stack
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/gap
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/profile
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/profile/client
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/profile/server
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/ble_mgr
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/common
)

if(CONFIG_BT_HAS_BREDR)
    ameba_list_append(private_sources
        rtk_stack_br_gap.c
        rtk_stack_avrcp.c
        rtk_stack_a2dp.c
        rtk_stack_sdp.c
        rtk_stack_spp.c
        rtk_stack_rfc.c
        rtk_stack_hid.c
        rtk_stack_hfp.c
        rtk_stack_pbap.c
    )

    ameba_list_append(private_includes
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/framework/bt_mgr_inc
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/framework/sysm_inc
    )
endif()

if(CONFIG_BT_HAS_MESH)
    ameba_list_append(private_sources
        rtk_stack_mesh_common.c
        rtk_stack_mesh_config_client_model.c
        rtk_stack_mesh_datatrans_model.c
        rtk_stack_mesh_device_firmware_update_model.c
        rtk_stack_mesh_remote_prov_model.c
        rtk_stack_mesh_generic_onoff_model.c
        rtk_stack_mesh_light_model.c
        rtk_stack_mesh_time_model.c
        rtk_stack_mesh_scheduler_model.c
        rtk_stack_mesh_scene_model.c
        rtk_stack_mesh_generic_default_transition_time.c
        rtk_stack_mesh_generic_model.c
        rtk_stack_mesh_health_model.c
        rtk_stack_mesh_sensor_model.c
        rtk_stack_mesh_directed_forwarding_model.c
        rtk_stack_mesh_subnet_bridge_model.c
        rtk_stack_mesh_private_beacon_model.c
    )

    ameba_list_append(private_includes
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/cmd
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/common
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/gap
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/inc
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/inc/${c_SOC_TYPE}
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/ali
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/model/realtek
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/platform
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/profile
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/src/mesh/utility
        ${c_CMPT_BLUETOOTH_DIR}/example/ble_mesh_ota_common
    )
endif()

if(CONFIG_BT_HAS_LEAUDIO)
    ameba_list_append(private_sources
        rtk_stack_iso.c
        rtk_stack_le_audio.c
        rtk_stack_bap.c
        rtk_stack_cap.c
        rtk_stack_tmap.c
        rtk_stack_gmap.c
    )

    ameba_list_append(private_includes
        ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/leaudio
    )
endif()

if(CONFIG_AMEBASMART OR CONFIG_AMEBALITE)
    ameba_list_append_if(CONFIG_BT_BLE_ONLY private_includes ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${c_SOC_TYPE}/lib/${c_MCU_TYPE}/ble_only)
    ameba_list_append_if(CONFIG_BT_DUAL_MODE private_includes ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${c_SOC_TYPE}/lib/${c_MCU_TYPE}/dual_mode)
elseif(CONFIG_AMEBALITE2 OR CONFIG_AMEBASMARTPLUS)
	ameba_list_append(private_includes ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${c_SOC_TYPE}/lib/${c_MCU_TYPE})
elseif(CONFIG_AMEBADPLUS OR CONFIG_AMEBAGREEN2)
	ameba_list_append(private_includes ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${c_SOC_TYPE}/lib)
endif()

# Component private part, user config end
#------------------------------#

ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

###NOTE: For open-source component, always build from source
ameba_add_internal_library(bt_api_rtk_stack
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################