if (__INCLUDED_SPEECHMIND_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_SPEECHMIND_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_speechmind INTERFACE)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_speechmind p_SCOPE interface
    ${c_COMPONENT_DIR}/utils/ringbuffer
    ${c_COMPONENT_DIR}/aivoice/include
    ${c_CMPT_APPLICATION_DIR}/speechmind/src
    ${c_CMPT_AUDIO_DIR}/interfaces
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_speechmind p_SCOPE interface __RTOS__ NDEBUG)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_speechmind p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/speechmind/src/speech_handler.c
    ${c_CMPT_APPLICATION_DIR}/speechmind/src/speech_tts.c
    ${c_CMPT_APPLICATION_DIR}/speechmind/src/app_example.c
    ${c_CMPT_APPLICATION_DIR}/speechmind/src/test_cmd.c
    ${c_CMPT_APPLICATION_DIR}/speechmind/src/speech_player.c
    ${c_CMPT_APPLICATION_DIR}/speechmind/src/speech_task.c
    ${c_CMPT_APPLICATION_DIR}/speechmind/src/amebasmart/speech_config.c
    ${c_CMPT_APPLICATION_DIR}/speechmind/src/amebasmart/speech_manager.c
)
#--------------------------#
