set(DIR ${BASEDIR}/component/application/benchmark/memtester)
set(MODULE_IFLAGS ${DIR})

set(
    CSRC  
    ${DIR}/memtester.c
)


ameba_internal_library(test_memtester)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
