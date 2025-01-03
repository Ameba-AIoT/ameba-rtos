if (__INCLUDED_APP_MONITOR_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_APP_MONITOR_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_app_monitor INTERFACE)

#--------------------------#
if(${d_PLATFORM_TYPE} STREQUAL "amebasmart")
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_app_monitor p_SCOPE interface
    ${c_CMPT_AT_CMD_DIR}/monitor.c
    ${d_SOC_PLATFORM_DIR}/app/monitor/ram/shell_ram.c
    ${d_SOC_PLATFORM_DIR}/app/monitor/ram/low_level_io.c
    )
    ameba_target_sources_if(CONFIG_LONGER_CMD src_${d_MCU_PROJECT_NAME}_app_monitor p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/app/monitor/rom/shell_rom_patch.c
    )
    if("${d_MCU_PROJECT_NAME}" STREQUAL "lp")
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_app_monitor p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/app/monitor/include
            ${c_FREERTOS_DIR}/include
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_LP
        )
    endif()
endif()
#--------------------------#