##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)
# You may use if-else condition to set or update predefined variable above

# Component public part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
ameba_global_library(${public_libraries}) #default: whole-achived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public, it's unnecessary to add again below.


# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------------------------------------------#
# Component private part, user config begin(DO NOT remove this line)

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable

if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${c_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_list_append(private_sources
            FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_clock.c
            FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_cond.c
            FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_timer.c
            FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_mqueue.c
            FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_mutex.c
            FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_unistd.c
            FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_barrier.c
            FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.c
            FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_utils.c
            FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.c
            FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_semaphore.c
        )
        ameba_list_append(private_includes
            include
            FreeRTOS-Plus-POSIX/include
            FreeRTOS-Plus-POSIX/include/portable/realtek

            ${c_FREERTOS_DIR}/include
            ${c_FREERTOS_DIR}/portable/GCC/ARM_CA7
        )

        ameba_list_append(private_compile_options
            -Wno-cast-function-type
            -Wno-unused-function
        )
    endif()
endif()

# Component private part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

###NOTE: For open-source component, always build from source
ameba_add_internal_library(freertos_posix
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################