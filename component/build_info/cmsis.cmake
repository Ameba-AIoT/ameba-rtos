if (__INCLUDED_CMSIS_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_CMSIS_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_cmsis INTERFACE)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_cmsis p_SCOPE interface
    # ${d_SOC_PLATFORM_DIR}/cmsis/source/mpu_config.c
)
#--------------------------#