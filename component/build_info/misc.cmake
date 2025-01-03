if (__INCLUDED_MISC_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_MISC_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_misc INTERFACE)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_misc p_SCOPE interface ${d_SOC_PLATFORM_DIR}/misc/ameba_pmu.c)
if (CONFIG_AS_INIC_AP OR CONFIG_SINGLE_CORE_WIFI)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_misc p_SCOPE interface ${d_SOC_PLATFORM_DIR}/misc/ameba_ota.c)
endif()
#--------------------------#