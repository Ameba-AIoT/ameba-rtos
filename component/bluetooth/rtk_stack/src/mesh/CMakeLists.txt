##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

if(CONFIG_AMEBASMART OR CONFIG_AMEBALITE)
	ameba_list_append(public_libraries ${CMAKE_CURRENT_SOURCE_DIR}/lib/${c_SOC_TYPE}/${c_MCU_TYPE}/btmesh.a)
elseif(CONFIG_AMEBADPLUS)
	if(CONFIG_BT_MESH_BASED_ON_CODED_PHY)
		ameba_list_append(public_libraries ${CMAKE_CURRENT_SOURCE_DIR}/lib/${c_SOC_TYPE}/btmesh_coded_phy.a)
	else()
		ameba_list_append(public_libraries ${CMAKE_CURRENT_SOURCE_DIR}/lib/${c_SOC_TYPE}/btmesh.a)
	endif()
endif()

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(p_NO_WHOLE_ARCHIVE ${public_libraries}) #default: whole-archived

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
	cmd/mesh_cmd.c
	common/blob_client_app.c
	common/dfu_distributor_app.c
	common/dfu_initiator_app.c
	model/blob_transfer_client.c
	model/delay_msg_rsp.c
	model/firmware_distribution_client.c
	model/firmware_update_client.c
	model/realtek/datatrans_model.c
	utility/mesh_data_dump.c
)

if(CONFIG_BT_MESH_PROVISIONER_SUPPORT)
	ameba_list_append(private_sources
        model/configuration_client.c
        model/generic_battery_client.c
        model/generic_default_transition_time_client.c
        model/generic_level_client.c
        model/generic_location_client.c
        model/generic_on_off_client.c
        model/generic_power_level_client.c
        model/generic_power_on_off_client.c
        model/generic_property_client.c
        model/light_ctl_client.c
        model/light_hsl_client.c
        model/light_lc_client.c
        model/light_lightness_client.c
        model/light_xyl_client.c
        model/scene_client.c
        model/scheduler_client.c
        model/time_client.c
        model/sensor_client.c
        model/health_client.c
        model/directed_forwarding_client.c
        model/private_beacon_client.c
        model/subnet_bridge_client.c
	)
endif()

if(CONFIG_BT_MESH_DEVICE_SUPPORT)
	ameba_list_append(private_sources
        model/delay_execution.c
        model/generic_battery_server.c
        model/generic_default_transition_time_server.c
        model/generic_level_server.c
        model/generic_location_server.c
        model/generic_location_setup_server.c
        model/generic_on_off_server.c
        model/generic_power_level_server.c
        model/generic_power_level_setup_server.c
        model/generic_power_on_off_server.c
        model/generic_power_on_off_setup_server.c
        model/generic_transition_time.c
        model/generic_user_property_server.c
        model/generic_admin_property_server.c
        model/generic_manufacturer_property_server.c
        model/generic_client_property_server.c
        model/health_server.c
        model/light_ctl_server.c
        model/light_ctl_setup_server.c
        model/light_ctl_temperature_server.c
        model/light_hsl_hue_server.c
        model/light_hsl_saturation_server.c
        model/light_hsl_server.c
        model/light_hsl_setup_server.c
        model/light_lc_server.c
        model/light_lc_setup_server.c
        model/light_lightness_server.c
        model/light_lightness_setup_server.c
        model/light_xyl_server.c
        model/light_xyl_setup_server.c
        model/scene_server.c
        model/scene_setup_server.c
        model/scheduler_server.c
        model/scheduler_setup_server.c
        model/time_server.c
        model/time_setup_server.c
        model/sensor_server.c
        model/sensor_setup_server.c
        utility/mesh_data_dump.c
	)
endif()

ameba_list_append(private_includes
    cmd
    common
    gap
    inc
    inc/${c_SOC_TYPE}
    model
    model/ali
    model/realtek
    platform
    profile
    utility
	${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/app
	${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/platform
	${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/os
	${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/gap
    ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/inc/bluetooth/profile
    ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack
)

if(CONFIG_AMEBASMART OR CONFIG_AMEBALITE)
    ameba_list_append_if(CONFIG_BT_BLE_ONLY private_includes ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${c_SOC_TYPE}/lib/${c_MCU_TYPE}/ble_only)
    ameba_list_append_if(CONFIG_BT_DUAL_MODE private_includes ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${c_SOC_TYPE}/lib/${c_MCU_TYPE}/dual_mode)
elseif(CONFIG_AMEBALITE2)
	ameba_list_append(private_includes ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/amebaL2/lib/${c_MCU_TYPE})
elseif(CONFIG_AMEBADPLUS OR CONFIG_AMEBAGREEN2)
	ameba_list_append(private_includes ${c_CMPT_BLUETOOTH_DIR}/rtk_stack/platform/${c_SOC_TYPE}/lib)
endif()

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(bt_rtk_stack_mesh
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