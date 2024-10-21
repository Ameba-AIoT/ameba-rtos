set(DIR ${BASEDIR}/component/os/freertos)
set(DIR_OSDEP ${BASEDIR}/component/os_dep)
set(DIR_OSWRAPPER  ${BASEDIR}/component/os/freertos/os_wrapper)

set(
    MODULE_IFLAGS
    ${FREERTOSDIR}/include
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/non_secure
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/secure
    ${DIR_OSWRAPPER}/include
)

set(
    CSRC
    ${DIR}/freertos_heap5_config.c
    ${DIR_OSWRAPPER}/os_wrapper_critical.c
    ${DIR_OSWRAPPER}/os_wrapper_memory.c
    ${DIR_OSWRAPPER}/os_wrapper_mutex.c
    ${DIR_OSWRAPPER}/os_wrapper_queue.c
    ${DIR_OSWRAPPER}/os_wrapper_semaphore.c
    ${DIR_OSWRAPPER}/os_wrapper_task.c
    ${DIR_OSWRAPPER}/os_wrapper_time.c
    ${DIR_OSWRAPPER}/os_wrapper_timer.c
    ${DIR_OSWRAPPER}/os_wrapper_static_functions.c
)



ameba_internal_library(freertos)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
add_custom_command(
    TARGET ${CURRENT_LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} --rename-section .text=.image2.net.ram.text ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
    COMMENT "NETWORK_RENAME_CODE2SRAM: freertos"
)

add_subdirectory(freertos_${FREERTOS_VER})
