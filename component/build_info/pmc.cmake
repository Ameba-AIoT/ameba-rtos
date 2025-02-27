if (__INCLUDED_PMC_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_PMC_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_pmc_lib INTERFACE)

#--------------------------#
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_pmc_lib p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/lib/pmc/ameba_pmc_${d_MCU_PROJECT_NAME}_lib.c
    )
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_pmc_lib p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/lib/pmc/a32_pm.S
        )
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_pmc_lib p_SCOPE interface
            ${c_FREERTOS_DIR}/portable/GCC/ARM_CA7
        )
    endif()
endif()
#--------------------------#
