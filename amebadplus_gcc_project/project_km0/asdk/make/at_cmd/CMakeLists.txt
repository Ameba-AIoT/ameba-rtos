set(
    MODULE_IFLAGS
    ${BASEDIR}/component/network/mqtt/MQTTClient
    ${BASEDIR}/component/network/mqtt/MQTTPacket
    ${BASEDIR}/component/network/cJSON
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

if(CONFIG_WLAN)
# note: apply new atcmd, add your config here!!!
    if(CONFIG_MCU_CONTROL_UART)
        list(
            APPEND CSRC 
            ${DIR}/at_intf_uart.c
        )
    endif()
    if(CONFIG_MCU_CONTROL_SPI)
        list(
            APPEND CSRC 
            ${DIR}/at_intf_spi.c
        )
    endif()
endif()

list(
    APPEND MODULE_IFLAGS
    ${DIR}
	${APP_DIR}/include 
	${FREERTOSDIR}/include/
	${FREERTOSDIR}/portable/GCC/AmebaDplus_KM0
)

ameba_internal_library(at_cmd)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
