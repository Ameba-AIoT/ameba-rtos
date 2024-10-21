set(
    MODULE_IFLAGS
    ${BASEDIR}/component/utils/ipc/rpc/include
    ${BASEDIR}/component/utils/ipc/rpc/include/xdr
    ${BASEDIR}/component/utils/ipc/rpc/src/aidl
)

set(DIR ${BASEDIR}/component/utils/ipc/rpc/src/aidl)

set(MODULE_C_DEFINES __RTOS__)

set(CSRC ${DIR}/RPCBaseDS_xdr.c)

ameba_internal_library(utils_rpc_framwork)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
