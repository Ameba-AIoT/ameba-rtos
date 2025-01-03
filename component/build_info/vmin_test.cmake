if (__INCLUDED_VMIN_TEST_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_VMIN_TEST_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_vmin_test INTERFACE)

#--------------------------#
if(${d_PLATFORM_TYPE} STREQUAL "amebasmart")
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_vmin_test p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/lib/Vmin_Test/cpuburn.S
        ${d_SOC_PLATFORM_DIR}/lib/Vmin_Test/max_power_l2.S
    )
endif()
#--------------------------#