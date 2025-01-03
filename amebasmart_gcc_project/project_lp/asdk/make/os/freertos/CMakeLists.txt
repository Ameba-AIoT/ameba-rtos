ameba_target_add(freertos
    p_WRAP_TARGET_NAME
    p_PREFIX lib_
    p_LINK_LIBRARY
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
        src_${d_MCU_PROJECT_NAME}_freertos
        src_${d_MCU_PROJECT_NAME}_freertos_wrapper
    p_APPEND_TO_LIST
        g_${d_MCU_PROJECT_NAME}_IMAGE2_LIBS
    p_DEPENDENTED_BY_TARGETS
        dep_${d_MCU_PROJECT_NAME}_freertos
)
add_custom_command(
    TARGET ${d_CURRENT_TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} --rename-section .text=.image2.net.ram.text $<TARGET_FILE:${d_CURRENT_TARGET_NAME}> $<TARGET_FILE:${d_CURRENT_TARGET_NAME}>
    COMMENT "NETWORK_RENAME_CODE2SRAM: freertos"
)
