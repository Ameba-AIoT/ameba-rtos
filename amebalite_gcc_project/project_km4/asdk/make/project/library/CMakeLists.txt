#add your include path here
set(
    MODULE_IFLAGS
)

#set your source code path here
set (CUSTOMER_DIR  ${PROJECTDIR}/asdk/make/project/library)

set(CSRC ${CUSTOMER_DIR}/lib_user_test.c)

ameba_internal_library(user)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})

add_custom_command(TARGET ${CURRENT_LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy lib_${CURRENT_LIB_NAME}.a ${TARGET_LIBAPP_FOLDER}/lib_user.a
    COMMAND ${CMAKE_OBJCOPY} -g -D ${TARGET_LIBAPP_FOLDER}/lib_user.a
)
