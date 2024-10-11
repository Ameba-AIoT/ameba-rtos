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


ameba_internal_library(project_xip)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})

