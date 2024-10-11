set(TIMER ${BASEDIR}/component/wpan/platform/timer)
set(FLASH  ${BASEDIR}/component/wpan/platform/flash)
set(PLATFORM_CONFIG ${BASEDIR}/component/wpan/platform/config)

list(
	APPEND
    MODULE_IFLAGS
	${TIMER}/
    ${FLASH}/
	${PLATFORM_CONFIG}/
)

set(CSRC)
set(CPPSRC)

list(
    APPEND CSRC 
    #OpenThread platform
    ${RTK_OT_SUPPORT}/rtk_ot_alarm.c 
    ${RTK_OT_SUPPORT}/rtk_ot_entropy.c 
    ${RTK_OT_SUPPORT}/rtk_ot_flash.c 
    ${RTK_OT_SUPPORT}/rtk_ot_logging.c 
    ${RTK_OT_SUPPORT}/rtk_ot_misc.c 
    ${RTK_OT_SUPPORT}/rtk_ot_system.c 
    ${RTK_OT_SUPPORT}/rtk_ot_crypto.c 
    ${RTK_OT_SUPPORT}/rtk_ot_uart.c 
    ${RTK_OT_SUPPORT}/rtk_ot_settings.c 
    ${RTK_OT_SUPPORT}/rtk_ot_multipan.c 
    ${RTK_OT_SUPPORT}/rtk_ot_ble.c 
    ${RTK_OT_SUPPORT}/rtk_ot_test.c 
    ${RTK_OT_SUPPORT}/rtk_ot_cli_extension.c
    ${RTK_OT_SUPPORT}/rtk_ot_cli_uart.c
)

list(
    APPEND CPPSRC 
    #OpenThread radio
    ${RTK_OT_SUPPORT}/radio/rcp_radio/rtk_ot_radio.cpp
    ${RTK_OT_SUPPORT}/radio/rcp_radio/rtk_ot_hdlc_interface.cpp
)

ameba_internal_library(ot_support)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CPP_OPTIONS}> -Wundef)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC} ${CPPSRC})