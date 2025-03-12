##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above
ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
ameba_global_library(${public_libraries}) #default: whole-archived

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
	rtk_bt_common.c
	rtk_bt_device.c
	rtk_bt_le_gap.c
	rtk_bt_gap.c
	rtk_bt_gatt.c
	rtk_bt_vendor.c
	rtk_bt_power_control.c
	rtk_bt_br_gap.c
	rtk_bt_avrcp.c
	rtk_bt_a2dp.c
	rtk_bt_sdp.c
	rtk_bt_spp.c
    rtk_bt_rfc.c
	rtk_bt_hid.c
	rtk_bt_hfp.c
	rtk_bt_pbap.c
	rtk_bt_mesh_common.c
	rtk_bt_mesh_config_client_model.c
	rtk_bt_mesh_datatrans_model.c
	rtk_bt_mesh_device_firmware_update_model.c
	rtk_bt_mesh_generic_model.c
	rtk_bt_mesh_generic_onoff_model.c
	rtk_bt_mesh_generic_default_transition_time.c
	rtk_bt_mesh_light_model.c
	rtk_bt_mesh_time_model.c
	rtk_bt_mesh_scheduler_model.c
	rtk_bt_mesh_scene_model.c
	rtk_bt_mesh_remote_prov_model.c
	rtk_bt_mesh_health_model.c
	rtk_bt_mesh_sensor_model.c
	rtk_bt_mesh_directed_forwarding_model.c
	rtk_bt_mesh_subnet_bridge_model.c
	rtk_bt_mesh_private_beacon_model.c
	rtk_bt_le_iso.c
	rtk_bt_bap.c
	rtk_bt_cap.c
	rtk_bt_tmap.c
	rtk_bt_gmap.c
)

if(CONFIG_BT_ZEPHYR)
    ameba_add_subdirectory(zephyr_stack)
    ameba_list_append(private_includes
        ${c_CMPT_BLUETOOTH_DIR}/api/zephyr_stack
    )
else()
    ameba_add_subdirectory(rtk_stack)
    ameba_list_append(private_includes
        ${c_CMPT_BLUETOOTH_DIR}/api/rtk_stack
    )
endif()

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(bt_api
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