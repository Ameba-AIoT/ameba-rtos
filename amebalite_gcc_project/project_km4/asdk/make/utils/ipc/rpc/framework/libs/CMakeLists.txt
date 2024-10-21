set(
    MODULE_IFLAGS
    ${BASEDIR}/component/utils/ipc/rpc/hal/interfaces
    ${BASEDIR}/component/utils/ipc/rpc/include
    ${BASEDIR}/component/utils/ipc/rpc/xdr/include
)

set(DIR ${BASEDIR}/component/utils/ipc/rpc)

set(MODULE_C_DEFINES __RTOS__)
set(MODULE_C_OPTIONS -Wimplicit-function-declaration)

set(
    CSRC
    ${DIR}/src/rpc_client.c
	${DIR}/src/rpc_server.c
	${DIR}/src/rpc_common.c
	${DIR}/src/rpc_utils.c
	${DIR}/src/xdr/xdr_array.c
	${DIR}/src/xdr/xdr.c
	${DIR}/src/xdr/xdr_float.c
	${DIR}/src/xdr/xdr_mem.c
	${DIR}/src/xdr/xdr_process.c
	${DIR}/src/xdr/xdr_ref.c
	${DIR}/src/xdr/xdr_sizeof.c
)

ameba_app_library(rpc)
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
