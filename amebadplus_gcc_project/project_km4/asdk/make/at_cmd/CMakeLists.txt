if(CONFIG_IMQ_EN)
    add_subdirectory(ipc_app)
endif()

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

list(
    APPEND CSRC
    ${DIR}/atcmd_service.c
	${DIR}/atcmd_sys.c
	${DIR}/atcmd_fs.c
	${DIR}/atcmd_bt_mp.c
)

if(CONFIG_WLAN)
    list(
        APPEND CSRC
        ${DIR}/atcmd_wifi.c
		${DIR}/atcmd_mqtt.c
		${DIR}/atcmd_sockets.c
		${DIR}/atcmd_http.c
		${DIR}/atcmd_websocket.c
		${DIR}/atcmd_network.c
        ${DIR}/atcmd_ota.c
    )
endif()

if(CONFIG_CORE_AS_AP OR ((NOT CONFIG_CORE_AS_NP) AND CONFIG_FULLMAC_MENU))
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

if(CONFIG_ATCMD_MCU_CONTROL)
        list(
            APPEND CSRC
            ${DIR}/at_intf_uart.c
            ${DIR}/at_intf_spi.c
        )
endif()

list(
    APPEND
    MODULE_IFLAGS
    ${DIR}
    ${APP_DIR}/include
    ${FREERTOSDIR}/include
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/non_secure
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/secure
)



ameba_internal_library(at_cmd)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
