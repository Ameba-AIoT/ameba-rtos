##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories
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
ameba_global_library(${public_libraries})

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

ameba_list_append(private_sources
    app_example.c
    speech_mind.c
    speech_tts.c
    test_cmd.c
    audio_capture.c
    pc_recorder.c
    audio_dump.c
    playlist.c
    music_player.c
    playlist_parser.c
    ${c_SOC_TYPE}/aivoice_manager.c
    ${c_SOC_TYPE}/speech_config.c
)


if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${c_SOC_TYPE}/aidl)
    file(GLOB_RECURSE aidl_src
        ${CMAKE_CURRENT_SOURCE_DIR}/${c_SOC_TYPE}/aidl/*.c
    )
    ameba_list_append(private_sources ${aidl_src})
endif()

ameba_list_append(private_includes
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_APP_DIR}/speechmind/libs
    ${c_CMPT_APP_DIR}/speechmind/libs/base/cutils/include
    ${c_CMPT_APP_DIR}/speechmind/libs/base/log/include
    ${c_CMPT_APP_DIR}/speechmind/libs/base/osal/osal_c/include
    ${c_CMPT_APP_DIR}/speechmind/libs/foundation/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${c_COMPONENT_DIR}/os/freertos/freertos_posix/FreeRTOS-Plus-POSIX/include
    ${c_COMPONENT_DIR}/os/freertos/freertos_posix/FreeRTOS-Plus-POSIX/include/portable/realtek
    ${c_COMPONENT_DIR}/os/freertos/freertos_posix/include
    ${CMAKE_CURRENT_SOURCE_DIR}/${c_SOC_TYPE}/
    ${c_COMPONENT_DIR}/aivoice/include
)

if(CONFIG_AMEBALITE)
    ameba_list_append(private_includes
        ${c_CMPT_UTILS_DIR}/ipc/rpc/include
        ${c_CMPT_UTILS_DIR}/ipc/rpc/src/aidl
        ${c_CMPT_APP_DIR}/speechmind/src/${c_SOC_TYPE}/aidl
    )
endif()

ameba_list_append(private_definitions
    __RTOS__ NDEBUG
)

if(CONFIG_AMEBALITE)
    ameba_list_append(private_compile_options
        -UDEBUG -Werror
    )
endif()

ameba_list_append(private_compile_options
    -Wno-multichar
    -Wno-implicit-function-declaration
    -Wno-int-conversion
    -Wno-unused-function
)


# Component private part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

###NOTE: For open-source component, always build from source
ameba_add_internal_library(speechmind
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