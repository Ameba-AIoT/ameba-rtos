if (__INCLUDED_FREERTOS_POSIX_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_FREERTOS_POSIX_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_freertos_posix INTERFACE)

#--------------------------#
if(${d_PLATFORM_TYPE} STREQUAL "amebasmart")
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_freertos_posix p_SCOPE interface
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_clock.c
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_cond.c
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_timer.c
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_mqueue.c
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_mutex.c
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_unistd.c
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_barrier.c
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.c
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_utils.c
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.c
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_semaphore.c
        )
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_freertos_posix p_SCOPE interface
            ${c_FREERTOS_POSIX_DIR}/include
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/include
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/include/portable/realtek
        
            ${FREERTOSDIR}/include
            ${FREERTOSDIR}/portable/GCC/ARM_CA7
        )

        ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_freertos_posix p_SCOPE interface
            -Wno-cast-function-type
            -Wno-unused-function
        )
    endif()

endif()