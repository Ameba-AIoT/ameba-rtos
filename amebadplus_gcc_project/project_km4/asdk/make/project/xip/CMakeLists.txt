#set your source code path here
set(
    CUSTOMER_DIR 
    ${PROJECTDIR}/asdk/make/project/xip
)

#add your include path here
set(
    MODULE_IFLAGS
    #${BASEDIR}/component/network/coap/include
)

set(
    CSRC 
    ${CUSTOMER_DIR}/xip_test.c
)

if(CONFIG_INTERNAL_LIB)
    ameba_internal_library(project_xip)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
else()
    set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
    target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})
endif()

