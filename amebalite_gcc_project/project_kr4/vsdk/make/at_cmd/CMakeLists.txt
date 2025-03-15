set(
    MODULE_IFLAGS
    ${BASEDIR}/component/network/mqtt/MQTTClient
    ${BASEDIR}/component/network/mqtt/MQTTPacket
    ${BASEDIR}/component/network/cJSON
    ${BASEDIR}/component/network/websocket
    ${BASEDIR}/component/file_system/kv
    ${BASEDIR}/component/utils/ringbuffer
)

set(APP_DIR ${TARGETDIR}/app/monitor)
set(DIR ${BASEDIR}/component/at_cmd)
set(MODULE_C_DEFINES)

if(CONFIG_WLAN)
    if(CONFIG_AS_INIC_AP OR CONFIG_SINGLE_CORE_WIFI)
        list(APPEND MODULE_C_DEFINES SUPPORT_ATCMD=1)
    endif()
endif()

set(
    CSRC
    ${DIR}/monitor.c
    ${APP_DIR}/ram/shell_ram.c
    ${APP_DIR}/ram/low_level_io.c
)

if(CONFIG_LONGER_CMD)
    list(
        APPEND CSRC
        ${APP_DIR}/rom/shell_rom_patch.c
    )
endif()

if(CONFIG_AS_INIC_AP OR CONFIG_SINGLE_CORE_WIFI)
# note: apply new atcmd, add your config here!!!
    list(
        APPEND CSRC
        ${DIR}/atcmd_service.c
        ${DIR}/atcmd_sys.c
        ${DIR}/atcmd_fs.c
        ${DIR}/atcmd_bt_mp.c
        ${DIR}/atcmd_otp.c
    )

    if(CONFIG_WLAN)
        list(
            APPEND CSRC
            ${DIR}/atcmd_wifi.c
            ${DIR}/atcmd_mqtt.c
            ${DIR}/atcmd_sockets.c
            ${DIR}/atcmd_websocket.c
            ${DIR}/atcmd_network.c
            ${DIR}/atcmd_ota.c
        )
    endif()

    if(CONFIG_CORE_AS_AP)
        list(
            APPEND CSRC
            ${DIR}/atcmd_coex.c
        )
    endif()
    
    if(CONFIG_BT)
        list(
            APPEND CSRC
            ${DIR}/atcmd_bt_ext.c
        )
    endif()

    if(CONFIG_ATCMD_HOST_CONTROL)
        list(
            APPEND CSRC
            ${DIR}/at_intf_uart.c
            ${DIR}/at_intf_spi.c
        )
    endif()
endif()

list(
    APPEND
    MODULE_IFLAGS
    ${DIR}
    ${APP_DIR}/include
    ${FREERTOSDIR}/include
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KR4
)

ameba_internal_library(at_cmd)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
