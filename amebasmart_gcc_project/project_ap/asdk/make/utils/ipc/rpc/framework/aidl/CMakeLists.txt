ameba_target_add(utils_rpc_framework_aidl
    p_WRAP_TARGET_NAME
    p_PREFIX lib_
    p_LINK_LIBRARY
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
        src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_aidl
    p_APPEND_TO_LIST
        g_${d_MCU_PROJECT_NAME}_IMAGE2_LIBS
    # p_LINKED_BY_LIBRARIES
    #     src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_xip
    # p_LINKED_BY_SCOPE interface
    p_DEPENDENTED_BY_TARGETS
        dep_${d_MCU_PROJECT_NAME}_utils
)
