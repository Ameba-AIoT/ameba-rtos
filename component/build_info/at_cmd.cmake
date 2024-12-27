if (__INCLUDED_AT_CMD_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_AT_CMD_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_at_cmd INTERFACE)

#--------------------------#
if(CONFIG_WLAN)
   if (CONFIG_AS_INIC_AP OR CONFIG_SINGLE_CORE_WIFI)
       ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_at_cmd p_SCOPE interface SUPPORT_ATCMD=1)
   endif()
endif()

ameba_target_sources(src_${d_MCU_PROJECT_NAME}_at_cmd p_SCOPE interface
   ${c_CMPT_AT_CMD_DIR}/monitor.c
   ${d_SOC_PLATFORM_DIR}/app/monitor/ram/shell_ram.c
)
if(${d_PLATFORM_TYPE} STREQUAL "amebasmart")
    if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_at_cmd p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/app/monitor/ram/low_level_io.c
        )
    endif()
endif()

if(CONFIG_LONGER_CMD)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_at_cmd p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/app/monitor/rom/shell_rom_patch.c
    )
else()
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_at_cmd p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/app/monitor/rom/shell_rom.c
        )
    endif()
endif()

if (CONFIG_SINGLE_CORE_WIFI OR CONFIG_AS_INIC_AP)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_at_cmd p_SCOPE interface
       ${c_CMPT_AT_CMD_DIR}/atcmd_service.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_sys.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_bt_mp.c
   )
   ameba_target_sources_if(CONFIG_WLAN src_${d_MCU_PROJECT_NAME}_at_cmd p_SCOPE interface
       ${c_CMPT_AT_CMD_DIR}/atcmd_wifi.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_mqtt.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_sockets.c
       ${c_CMPT_AT_CMD_DIR}/atcmd_websocket.c
   )
   ameba_target_sources_if(CONFIG_BT src_${d_MCU_PROJECT_NAME}_at_cmd p_SCOPE interface
       ${c_CMPT_AT_CMD_DIR}/atcmd_bt_ext.c
   )
   ameba_target_sources_if(CONFIG_MCU_CONTROL_UART src_${d_MCU_PROJECT_NAME}_at_cmd p_SCOPE interface
       ${c_CMPT_AT_CMD_DIR}/at_intf_uart.c
   )
   ameba_target_sources_if(CONFIG_MCU_CONTROL_SPI src_${d_MCU_PROJECT_NAME}_at_cmd p_SCOPE interface
       ${c_CMPT_AT_CMD_DIR}/at_intf_spi.c
   )
endif()
ameba_target_include(src_${d_MCU_PROJECT_NAME}_at_cmd p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTClient
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTPacket
    ${c_CMPT_NETWORK_DIR}/cJSON
    ${c_CMPT_NETWORK_DIR}/websocket
    ${c_CMPT_FILE_SYSTEM_DIR}/kv
    ${c_COMPONENT_DIR}/utils/ringbuffer
)
#--------------------------#