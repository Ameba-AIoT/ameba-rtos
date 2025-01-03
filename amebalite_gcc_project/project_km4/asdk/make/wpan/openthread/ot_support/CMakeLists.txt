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

if(CONFIG_802154_THREAD_RADIO_RCP_EN)
    list(
        APPEND CPPSRC 
        #OpenThread radio
        ${RTK_OT_SUPPORT}/radio/rcp_radio/rtk_ot_radio.cpp
        ${RTK_OT_SUPPORT}/radio/rcp_radio/rtk_ot_hdlc_interface.cpp
    )
else()
    set(SW_RADIO ${BASEDIR}/component/wpan/platform/sw_radio)

    list(
        APPEND MODULE_IFLAGS
        ${SW_RADIO}/phy/common/radio_api/
        ${SW_RADIO}/mac/
        ${SW_RADIO}/mac/mac_features/
        ${SW_RADIO}/mac/mac_features/ack_generator/
        ${SW_RADIO}/mac/rsch/
        ${SW_RADIO}/mac/rsch/coex/
        ${SW_RADIO}/mac/rsch/raal/
        ${SW_RADIO}/mac/rsch/raal/softdevice/
    )

    list(
        APPEND CSRC 
        #OpenThread radio
        ${RTK_OT_SUPPORT}/radio/rtk_ot_sw_radio.c
    )
endif()

if(CONFIG_802154_THREAD_RCP_EN)
    list(
        APPEND CSRC 
        ${THREADDIR}/openthread/examples/apps/ncp/ncp.c
    )
endif()

ameba_internal_library(ot_support)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CPP_OPTIONS}> -Wundef)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC} ${CPPSRC})