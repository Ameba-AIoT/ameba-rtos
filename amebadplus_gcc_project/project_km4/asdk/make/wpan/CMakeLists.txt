if(CONFIG_802154_THREAD_EN)
    # add_subdirectory(openthread)
endif()


set(TIMER ${BASEDIR}/component/wpan/platform/timer)
set(FLASH ${BASEDIR}/component/wpan/platform/flash)
set(PLATFORM_CONFIG ${BASEDIR}/component/wpan/platform/config)

set(
    CSRC
    ${TIMER}/rtk_802154_timer.c 
	${TIMER}/rtk_802154_timer_sched.c

    ${FLASH}/rtk_802154_flash.c
)

set(
    MODULE_IFLAGS 
    ${TIMER}
    ${FLASH}
    ${PLATFORM_CONFIG}
    ${BASEDIR}/component/soc/amebadplus/fwlib/include
    ${BASEDIR}/component/os/os_dep
)

if(CONFIG_INTERNAL_LIB)

    ameba_internal_library(wpan)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})

else()
    
    set_source_files_properties(${CSRC} DIRECTORY .. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
    target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})

endif()


