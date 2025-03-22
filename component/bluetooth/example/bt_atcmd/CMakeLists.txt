##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
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
	bt_utils.c
	atcmd_bt_common.c
	atcmd_bt_example.c
	atcmd_bt_gap.c
	atcmd_bt_le_gap.c
	atcmd_bt_br_gap.c
	atcmd_bt_gatts.c
	atcmd_bt_gattc.c
	atcmd_bt_vendor.c
	atcmd_bt_sdp.c
	atcmd_bt_a2dp.c
	atcmd_bt_avrcp.c
	atcmd_bt_spp.c
	atcmd_bt_rfc.c
	atcmd_bt_hid.c
	atcmd_bt_hfp.c
	atcmd_bt_iso.c
	atcmd_bt_pbap.c
	atcmd_bt_bap.c
	atcmd_bt_cap.c
	atcmd_bt_tmap.c
	atcmd_bt_gmap.c
	atcmd_bt_mesh_common.c
	atcmd_bt_mesh_config_model.c
	atcmd_bt_mesh_datatrans_model.c
	atcmd_bt_mesh_device_firmware_update_model.c
	atcmd_bt_mesh_goo_model.c
	atcmd_bt_mesh_remote_prov_model.c
	atcmd_bt_mesh_light_model.c
	atcmd_bt_mesh_time_model.c
	atcmd_bt_mesh_scheduler_model.c
	atcmd_bt_mesh_scene_model.c
	atcmd_bt_mesh_generic_default_transition_time.c
	atcmd_bt_mesh_generic_model.c
	atcmd_bt_mesh_sensor_model.c
	atcmd_bt_mesh_health_model.c
	atcmd_bt_mesh_directed_forwarding.c
	atcmd_bt_mesh_subnet_bridge_model.c
	atcmd_bt_mesh_private_beacon_model.c
)

ameba_list_append(private_includes
    ../gatt_service/client
    ../gatt_service/server
    ../ble_iso
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(bt_atcmd
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