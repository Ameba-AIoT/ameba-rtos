ameba_target_add(audio_xip_passthrough
    p_WRAP_TARGET_NAME
    p_OUTPUT_NAME audio_passthrough
    p_STRIP_DEBUG
    p_ENABLE_DETERMINISTIC_ARCHIVES
    p_ADD_EMPTY_C_FILE
    p_OUTPUT_PATH ${d_SDK_LIB_APPLICATION_DIR}
    p_PREFIX lib_
    p_LINK_LIBRARY
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
        src_${d_MCU_PROJECT_NAME}_audio_xip_passthrough
    p_DEPENDENTED_BY_TARGETS
        dep_${d_MCU_PROJECT_NAME}_audio
)
