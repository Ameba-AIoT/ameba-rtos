set(TIMER ${BASEDIR}/component/wpan/platform/timer)
set(FLASH  ${BASEDIR}/component/wpan/platform/flash)
set(PLATFORM_CONFIG ${BASEDIR}/component/wpan/platform/config)

set(
    MODULE_IFLAGS
    ${BASEDIR}/component/soc/amebalite/fwlib/include/
    ${BASEDIR}/component/component/os/os_dep/
    ${TIMER}/
    ${FLASH}/
    ${PLATFORM_CONFIG}/
)

set(MODULE_C_DEFINES)
set(CSRC)

list(
	APPEND CSRC 
	${TIMER}/rtk_802154_timer.c
	${TIMER}/rtk_802154_timer_sched.c
	${FLASH}/rtk_802154_flash.c
)

if(NOT CONFIG_802154_THREAD_RADIO_RCP_EN)
add_subdirectory(sw_radio)
endif()

ameba_internal_library(platform)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})