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

if(CONFIG_WLAN)
   if (CONFIG_AS_INIC_AP OR CONFIG_SINGLE_CORE_WIFI)
       ameba_list_append(private_definitions SUPPORT_ATCMD=1)
   endif()
endif()

ameba_list_append(private_sources
   ${c_CMPT_AT_CMD_DIR}/monitor.c
   ${c_CMPT_SOC_DIR}/app/monitor/ram/shell_ram.c
)

if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${c_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_list_append(private_sources
            ${c_CMPT_SOC_DIR}/app/monitor/ram/low_level_io.c
        )
    endif()
endif()

if(CONFIG_LONGER_CMD)
    ameba_list_append(private_sources
        ${c_CMPT_SOC_DIR}/app/monitor/rom/shell_rom_patch.c
    )
else()
    if("${c_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_list_append(private_sources
            ${c_CMPT_SOC_DIR}/app/monitor/rom/shell_rom.c
        )
    endif()
endif()

if (CONFIG_SINGLE_CORE_WIFI OR CONFIG_AS_INIC_AP)
    ameba_list_append(private_sources
       ${c_CMPT_AT_CMD_DIR}/atcmd_service.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_sys.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_fs.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_bt_mp.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_otp.c
   )
   ameba_list_append_if(CONFIG_WLAN private_sources
       ${c_CMPT_AT_CMD_DIR}/atcmd_wifi.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_mqtt.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_sockets.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_websocket.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_network.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_coex.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_ota.c
   )
   ameba_list_append_if(CONFIG_BT private_sources
       ${c_CMPT_AT_CMD_DIR}/atcmd_bt_ext.c
   )
   ameba_list_append_if(CONFIG_ATCMD_MCU_CONTROL private_sources
       ${c_CMPT_AT_CMD_DIR}/at_intf_uart.c
       ${c_CMPT_AT_CMD_DIR}/at_intf_spi.c
   )
endif()
ameba_list_append(private_includes
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTClient
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTPacket
    ${c_CMPT_NETWORK_DIR}/cJSON
    ${c_CMPT_NETWORK_DIR}/websocket
    ${c_CMPT_FILE_SYSTEM_DIR}/kv
    ${c_COMPONENT_DIR}/utils/ringbuffer
)

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