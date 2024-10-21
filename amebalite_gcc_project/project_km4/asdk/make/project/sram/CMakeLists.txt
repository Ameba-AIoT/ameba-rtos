#add your include path here
set(
    MODULE_IFLAGS
)

#set your source code path here
set (CUSTOMER_DIR  ${PROJECTDIR}/asdk/make/project/sram)

set(CSRC ${CUSTOMER_DIR}/ram_test.c)



ameba_internal_library(project_sram)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
add_custom_command(
    TARGET ${CURRENT_LIB_NAME} POST_BUILD 
    COMMAND ${CMAKE_OBJCOPY}  --rename-section .text=.image2.ram.text ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
    COMMENT "RENAME_CODE2SRAM: project sram"
)
