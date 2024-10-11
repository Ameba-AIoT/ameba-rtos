set(DIR ${BASEDIR}/component/utils/profiler)
set(MODULE_IFLAGS ${DIR})

set(CSRC ${DIR}/profiler.c)


ameba_internal_library(utils_profiler)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
