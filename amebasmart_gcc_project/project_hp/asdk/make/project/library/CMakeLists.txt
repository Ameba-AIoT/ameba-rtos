ameba_target_add(user
    p_WRAP_TARGET_NAME
    p_STRIP_DEBUG
    p_ENABLE_DETERMINISTIC_ARCHIVES
    p_OUTPUT_PATH ${d_SDK_LIB_APPLICATION_DIR}
    p_OUTPUT_NAME user
    p_PREFIX lib_
    p_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/lib_user_test.c
    p_INCLUDES
        ${CMAKE_CURRENT_LIST_DIR}
    p_LINK_LIBRARY
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
    p_DEPENDENTED_BY_TARGETS
        dep_${d_MCU_PROJECT_NAME}_user_project
)
