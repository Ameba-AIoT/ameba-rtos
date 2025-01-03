ameba_target_add(test_std_lib
    p_WRAP_TARGET_NAME
    p_PREFIX lib_
    p_LINK_LIBRARY
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
        src_${d_MCU_PROJECT_NAME}_test_std_lib
    p_APPEND_TO_LIST
        g_${d_MCU_PROJECT_NAME}_IMAGE2_LIBS
    p_DEPENDENTED_BY_TARGETS
        dep_${d_MCU_PROJECT_NAME}_test
)
