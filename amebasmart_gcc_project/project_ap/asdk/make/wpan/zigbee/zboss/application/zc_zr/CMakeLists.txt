ameba_target_add(wpan_zb_zboss_zc_zr
    p_WRAP_TARGET_NAME
    p_OUTPUT_NAME wpan_zb_zboss_zc_zr
    p_STRIP_DEBUG
    p_ENABLE_DETERMINISTIC_ARCHIVES
    p_OUTPUT_PATH ${d_SDK_LIB_APPLICATION_DIR}
    p_PREFIX lib_
    p_LINK_LIBRARY
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
        src_${d_MCU_PROJECT_NAME}_wpan_zb_zboss_zc_zr
    p_DEPENDENTED_BY_TARGETS
        dep_${d_MCU_PROJECT_NAME}_wpan
)