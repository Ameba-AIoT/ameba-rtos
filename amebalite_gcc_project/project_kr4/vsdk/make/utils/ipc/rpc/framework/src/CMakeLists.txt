set(
    MODULE_IFLAGS
    ${BASEDIR}/component/utils/ipc/rpc/hal/interfaces
    ${BASEDIR}/component/utils/ipc/rpc/include
    ${BASEDIR}/component/utils/ipc/rpc/xdr/include
)

set(DIR${BASEDIR}/component/utils/ipc/rpc)

set(MODULE_C_DEFINES __RTOS__)
set(MODULE_C_OPTIONS -Wimplicit-function-declaration)

set(CSRC ${DIR}/src/rpc_common.c)

ameba_internal_library(utils_rpc_src)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
