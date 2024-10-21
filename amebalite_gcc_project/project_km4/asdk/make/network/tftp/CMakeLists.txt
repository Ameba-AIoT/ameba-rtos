set(DIR ${BASEDIR}/component/network/tftp)

set(MODULE_IFLAGS ${BASEDIR}/component/network/tftp)

set(
    CSRC 
    ${DIR}/tftp_client.c 
	${DIR}/tftp_server.c 
)

ameba_app_library(tftp)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})