set(DIR ${BASEDIR}/component/application/benchmark/memtester)

set(MODULE_IFLAGS ${DIR}/../)
set(MODULE_C_OPTIONS -O3)

set(CSRC ${DIR}/memtester.c)

ameba_internal_library(test_memtester)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
