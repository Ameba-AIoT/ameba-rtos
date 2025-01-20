if (__INCLUDED_DYNAMIC_APP_LOAD_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_DYNAMIC_APP_LOAD_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_dynamic_app INTERFACE)

#--------------------------#
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_dynamic_app p_SCOPE interface
            ${c_DYNAMIC_APP_DIR}/app_loader/app_loader.c
            ${c_DYNAMIC_APP_DIR}/app_loader/app_ioctrl.c
            ${c_DYNAMIC_APP_DIR}/test/test_app_loader.c
            ${c_DYNAMIC_APP_DIR}/sys_module/syscall_posix.c
            ${c_DYNAMIC_APP_DIR}/sys_module/syscall_socket.c
            ${c_DYNAMIC_APP_DIR}/sys_module/syscall_vfs.c
            ${c_DYNAMIC_APP_DIR}/sys_module/syscall.c
        )

        ameba_target_include(src_${d_MCU_PROJECT_NAME}_dynamic_app p_SCOPE interface
            ${c_DYNAMIC_APP_DIR}/sys_module
            ${c_DYNAMIC_APP_DIR}/app_loader
            ${c_FREERTOS_DIR}/include
            ${c_FREERTOS_DIR}/portable/GCC/ARM_CA7
            ${c_FREERTOS_POSIX_DIR}/include
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/include
            ${c_FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/include/portable/realtek
        )

        ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_dynamic_app p_SCOPE interface
            KERNEL=1
        )

    endif()
endif()
