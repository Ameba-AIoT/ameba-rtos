if (__INCLUDED_FREERTOS_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_FREERTOS_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_freertos INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_freertos_wrapper INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_freertos_secure INTERFACE)

#--------------------------#
if(CONFIG_AMEBASMART)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_freertos p_SCOPE interface
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/non_secure/port.c
        )
        ameba_target_sources_if(CONFIG_TRUSTZONE_EN src_${d_MCU_PROJECT_NAME}_freertos p_SCOPE interface
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/non_secure/portasm.c
            p_ELSE ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/non_secure/portasm_ntz.c
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "lp")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_freertos p_SCOPE interface
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_LP/port.c
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_LP/portasm.c
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_freertos p_SCOPE interface
            ${c_FREERTOS_DIR}/portable/GCC/ARM_CA7/port.c
            ${c_FREERTOS_DIR}/portable/GCC/ARM_CA7/portASM.S
        )
    else()
    endif()

    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_freertos p_SCOPE interface
        ${c_FREERTOS_DIR}/tasks.c
        ${c_FREERTOS_DIR}/list.c
        ${c_FREERTOS_DIR}/croutine.c
        ${c_FREERTOS_DIR}/queue.c
        ${c_FREERTOS_DIR}/timers.c
        ${c_FREERTOS_DIR}/event_groups.c
        ${c_FREERTOS_DIR}/stream_buffer.c
        ${c_FREERTOS_DIR}/portable/MemMang/heap_5.c
    )

    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_freertos_wrapper p_SCOPE interface
        ${c_CMPT_OS_DIR}/freertos/freertos_heap5_config.c
        ${c_CMPT_OS_DIR}/freertos/os_wrapper/os_wrapper_critical.c
        ${c_CMPT_OS_DIR}/freertos/os_wrapper/os_wrapper_memory.c
        ${c_CMPT_OS_DIR}/freertos/os_wrapper/os_wrapper_mutex.c
        ${c_CMPT_OS_DIR}/freertos/os_wrapper/os_wrapper_queue.c
        ${c_CMPT_OS_DIR}/freertos/os_wrapper/os_wrapper_semaphore.c
        ${c_CMPT_OS_DIR}/freertos/os_wrapper/os_wrapper_task.c
        ${c_CMPT_OS_DIR}/freertos/os_wrapper/os_wrapper_time.c
        ${c_CMPT_OS_DIR}/freertos/os_wrapper/os_wrapper_timer.c
        ${c_CMPT_OS_DIR}/freertos/os_wrapper/os_wrapper_static_functions.c
    )

    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_freertos_wrapper p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/swlib/gcc_wrap.c
        )
    endif()

    ameba_target_include(src_${d_MCU_PROJECT_NAME}_freertos_wrapper p_SCOPE interface
        $<TARGET_PROPERTY:src_${d_MCU_PROJECT_NAME}_freertos,INTERFACE_INCLUDE_DIRECTORIES>
    )
endif()
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_freertos_secure p_SCOPE interface
    ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/secure/secure_context.c
    ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/secure/secure_context_port.c
    ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/secure/secure_heap.c
    ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/secure/secure_init.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_freertos_secure p_SCOPE interface
    ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/secure
    ${c_FREERTOS_DIR}/include
    ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/non_secure
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_freertos_secure p_SCOPE interface
    -O0
)
#--------------------------#
