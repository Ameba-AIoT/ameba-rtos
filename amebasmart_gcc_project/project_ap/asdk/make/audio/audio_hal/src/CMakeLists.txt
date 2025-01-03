ameba_target_add(audio_hal
    p_WRAP_TARGET_NAME
    p_PREFIX lib_
    p_LINK_LIBRARY
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
        src_${d_MCU_PROJECT_NAME}_audio_hal
    p_APPEND_TO_LIST
        g_${d_MCU_PROJECT_NAME}_IMAGE2_LIBS
    p_DEPENDENTED_BY_TARGETS
        dep_${d_MCU_PROJECT_NAME}_audio
)

# make lib_audio_hal.a
# ameba_target_add(audio_hal_lib
#     p_WRAP_TARGET_NAME
#     p_STRIP_DEBUG
#     p_ENABLE_DETERMINISTIC_ARCHIVES
#     p_OUTPUT_PATH ${d_SDK_LIB_APPLICATION_DIR}
#     p_OUTPUT_NAME audio_hal
#     p_PREFIX lib_
#     p_LINK_LIBRARY
#         d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES
#         d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
#         src_${d_MCU_PROJECT_NAME}_audio_hal
#     p_DEPENDENTED_BY_TARGETS
#         dep_${d_MCU_PROJECT_NAME}_audio
# )
