set(DIR ${BASEDIR}/component/network/httpc)
set(MODULE_IFLAGS ${BASEDIR}/component/network/httpc)

set(
    CSRC 
    ${DIR}/httpc_core.c 
	${DIR}/httpc_msg.c 
	${DIR}/httpc_util.c 
)

ameba_app_library(httpc)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})