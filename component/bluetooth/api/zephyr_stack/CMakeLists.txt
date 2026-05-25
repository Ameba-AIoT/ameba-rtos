##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

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
    zephyr_stack_api.c
    zephyr_stack_resource.c
    zephyr_stack_gap.c
    zephyr_stack_le_gap.c
    zephyr_stack_gattc.c
    zephyr_stack_gatts.c
)

if (CONFIG_BT_MESH_PROVISIONER_SUPPORT OR CONFIG_BT_MESH_DEVICE_SUPPORT)
    ameba_list_append(private_sources
        zephyr_stack_mesh_common.c
        zephyr_stack_mesh_configuration_model.c
        zephyr_stack_mesh_datatrans_model.c
        zephyr_stack_mesh_device_firmware_update_model.c
        zephyr_stack_mesh_generic_onoff_model.c
        zephyr_stack_mesh_light_model.c
        zephyr_stack_mesh_scene_model.c
        zephyr_stack_mesh_models_handler.c
        zephyr_stack_mesh_sensor_model.c
        zephyr_stack_mesh_time_model.c
        zephyr_stack_mesh_scheduler_model.c
        zephyr_stack_mesh_generic_default_transition_time.c
        zephyr_stack_mesh_generic_model.c
        zephyr_stack_mesh_health_model.c
        zephyr_stack_mesh_remote_provision_model.c
        zephyr_stack_mesh_private_beacon_model.c
    )
endif()

ameba_list_append(private_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${c_CMPT_BLUETOOTH_DIR}/driver/hci
    ${c_CMPT_BLUETOOTH_DIR}/example/ble_mesh_ota
    ${c_CMPT_BLUETOOTH_DIR}/zephyr
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/hci_driver
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/include
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/host
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/stack/mesh
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/settings
    ${c_CMPT_BLUETOOTH_DIR}/zephyr/port
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(bt_api_zephyr_stack
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