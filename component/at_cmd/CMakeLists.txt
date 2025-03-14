##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)
# You may use if-else condition to set or update predefined variable above

ameba_list_append(public_includes ${c_COMPONENT_DIR}/at_cmd)

# Component public part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Template section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
ameba_global_library(${public_libraries})

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public, it's unnecessary to add again below.

# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files
set(private_includes)                #private include directories
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------------------------------------------#
# Component private part, user config begin(DO NOT remove this line)

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
ameba_list_append(private_sources
    monitor.c
)
if(CONFIG_CORE_AS_AP)
    ameba_list_append(private_sources
        atcmd_service.c
        atcmd_sys.c
        atcmd_fs.c
        atcmd_bt_mp.c
        atcmd_otp.c
        atcmd_coex.c
    )
    ameba_list_append_if(CONFIG_WLAN private_sources
        atcmd_http.c
        atcmd_mqtt.c
        atcmd_network.c
        atcmd_ota.c
        atcmd_sockets.c
        atcmd_websocket.c
        atcmd_wifi.c
    )
    ameba_list_append_if(CONFIG_BT private_sources
        atcmd_bt_ext.c
    )
    ameba_list_append_if(CONFIG_ATCMD_MCU_CONTROL private_sources
        at_intf_uart.c
        at_intf_spi.c
    )
endif()
# Component private part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Template section, DO NOT change!
ameba_add_internal_library(at_cmd
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