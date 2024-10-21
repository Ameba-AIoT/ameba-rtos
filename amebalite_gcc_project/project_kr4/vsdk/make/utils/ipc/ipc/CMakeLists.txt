set(MODULE_IFLAGS ${BASEDIR}/component/utils/ipc/ipc/amebalite/include)
set(DIR ${BASEDIR}/component/utils/ipc/ipc/amebalite)

set(CSRC ${DIR}/src/ipc.c)
if(CONFIG_IMQ2_TEST)
    list(APPEND CSRC ${DIR}/tests/test_ipc.c)
endif()

ameba_internal_library(utils_ipc)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
