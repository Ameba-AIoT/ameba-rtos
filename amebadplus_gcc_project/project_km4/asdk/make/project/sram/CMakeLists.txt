#add your include path here
set(
    MODULE_IFLAGS
)

#set your source code path here
set (CUSTOMER_DIR  ${PROJECTDIR}/asdk/make/project/sram)

set(CSRC ${CUSTOMER_DIR}/ram_test.c)


if(CONFIG_INTERNAL_LIB)

    ameba_internal_library(project_sram)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
    add_custom_command(
        TARGET ${CURRENT_LIB_NAME} POST_BUILD 
        COMMAND ${CMAKE_OBJCOPY}  --rename-section .text=.image2.ram.text ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a ${CMAKE_CURRENT_BINARY_DIR}/lib_${CURRENT_LIB_NAME}.a
        COMMENT "RENAME_CODE2SRAM: project sram"
    )

else()
    foreach(src ${CSRC})
        cmake_path(GET src FILENAME filename)
        set_property(GLOBAL APPEND PROPERTY RENAME_CMD COMMAND ${CMAKE_OBJCOPY} --rename-section .text=.image2.ram.text ${CMAKE_CURRENT_BINARY_DIR}/../../CMakeFiles/target_img2_${PROJECT_NAME}.dir/project/sram/${filename}.o ${CMAKE_CURRENT_BINARY_DIR}/../../CMakeFiles/target_img2_${PROJECT_NAME}.dir/project/sram/${filename}.o)
    endforeach()
    set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
    target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})
endif()