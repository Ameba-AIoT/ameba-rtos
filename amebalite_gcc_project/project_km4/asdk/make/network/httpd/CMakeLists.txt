set(DIR ${BASEDIR}/component/network/httpd)
set(MODULE_IFLAGS ${BASEDIR}/component/network/httpd)

set(
    CSRC 
    ${DIR}/httpd_core.c 
    ${DIR}/httpd_msg.c 
    ${DIR}/httpd_util.c 
)

ameba_app_library(httpd)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})