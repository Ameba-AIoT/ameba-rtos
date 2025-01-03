if (__INCLUDED_IMG3_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_IMG3_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_img3 INTERFACE)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_img3 p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/img3/boot_img3.c
    ${d_SOC_PLATFORM_DIR}/img3/secure_src.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_img3 p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/img3
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_img3 p_SCOPE interface
    -O0
)
#--------------------------#