ameba_target_add(project_src
    p_WRAP_TARGET_NAME
    p_PREFIX lib_
    p_SOURCES
        ${d_MCU_PROJECT_DIR}/src/main.c
    p_INCLUDES
        ${d_MCU_PROJECT_DIR}/src
    p_LINK_LIBRARY
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
    p_APPEND_TO_LIST
        g_${d_MCU_PROJECT_NAME}_IMAGE2_LIBS
    p_DEPENDENTED_BY_TARGETS
        dep_${d_MCU_PROJECT_NAME}_user_project
)
