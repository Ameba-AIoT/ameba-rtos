set(
    MODULE_IFLAGS
    ${BASEDIR}/component/utils/ipc/rpc/hal/interfaces
    ${BASEDIR}/component/utils/ipc/ipc/amebalite/include
)

set(DIR ${BASEDIR}/component/utils/ipc/rpc)

set(MODULE_C_OPTIONS -D__RTOS__ -DNDEBUG -UDEBUG)

set(CSRC ${DIR}/hal/rpc_hw_manager.c)

ameba_internal_library(utils_rpc_hal)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
