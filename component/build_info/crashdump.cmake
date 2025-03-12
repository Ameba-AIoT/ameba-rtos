if (__INCLUDED_CRASHDUMP_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_CRASHDUMP_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_crashdump_lib INTERFACE)

file(RELATIVE_PATH IMAGE_DIR ${FINAL_IMAGE_DIR} ${IMAGE_TARGET_FOLDER})
target_compile_definitions(src_${d_MCU_PROJECT_NAME}_crashdump_lib INTERFACE -DSDK_TOOLCHAIN="${SDK_TOOLCHAIN}")
target_compile_definitions(src_${d_MCU_PROJECT_NAME}_crashdump_lib INTERFACE -DIMAGE_DIR="${IMAGE_DIR}")

#--------------------------#
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_crashdump_lib p_SCOPE interface
        ${c_COMPONENT_DIR}/soc/common/crashdump/arm/ameba_v8m_crashdump.c
        ${c_COMPONENT_DIR}/soc/common/crashdump/arm/ameba_v8m_fault_diagnosis.c
        ${c_COMPONENT_DIR}/soc/common/crashdump/arm/ameba_v8m_backtrace.c
        ${c_COMPONENT_DIR}/soc/common/crashdump/ameba_freertos_backtrace.c
    )
    if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_crashdump_lib p_SCOPE interface
            ${c_FREERTOS_DIR}/include
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/non_secure
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "lp")
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_crashdump_lib p_SCOPE interface
            ${c_FREERTOS_DIR}/include
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_LP/non_secure
        )
    endif()
endif()

#--------------------------#
