if(CONFIG_IMQ_EN)
    add_subdirectory(ipc_app)
endif()

set( 
    MODULE_IFLAGS 
    ${BASEDIR}/component/network/mqtt/MQTTClient
    ${BASEDIR}/component/network/mqtt/MQTTPacket
)

set(APP_DIR ${TARGETDIR}/app/monitor)
set(DIR ${BASEDIR}/component/at_cmd)


set(
    CSRC
    ${APP_DIR}/ram/monitor_km4.c
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
	${DIR}/atcmd_bt_mp.c
)

if(CONFIG_WLAN)
    list(
        APPEND CSRC 
        ${DIR}/atcmd_wifi.c 
		${DIR}/atcmd_mqtt.c 
		${DIR}/atcmd_lwip.c 
    )
endif()

if(CONFIG_BT)
    list(
        APPEND CSRC 
        ${DIR}/atcmd_bt_ext.c
    )
endif()

if(CONFIG_ATCMD_IO_UART)
    list(
        APPEND CSRC 
        ${DIR}/at_intf_uart.c
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

if(CONFIG_INTERNAL_LIB)

    ameba_internal_library(at_cmd)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})

else()

    set_source_files_properties(${CSRC} DIRECTORY .. PROPERTIES COMPILE_DEFINITIONS "${MODULE_C_DEFINES}")

    set_source_files_properties(${CSRC} DIRECTORY .. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")

    target_sources(
        target_img2_${PROJECT_NAME}
        PRIVATE
        ${CSRC}
    )
endif()