set(DIR ${BASEDIR}/component/network/dnrd/src)
set(MODULE_IFLAGS ${BASEDIR}/component/network/dnrd/src)

set(
    CSRC 
    ${DIR}/common.c 
    ${DIR}/dnrd.c 
    ${DIR}/dns.c 
    ${DIR}/lib.c 
    ${DIR}/master.c 
    ${DIR}/query.c 
    ${DIR}/relay.c 
    ${DIR}/udp.c 
)

ameba_app_library(ipdnrd)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})