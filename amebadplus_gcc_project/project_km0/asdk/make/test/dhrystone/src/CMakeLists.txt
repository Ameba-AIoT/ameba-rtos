set(DIR ${BASEDIR}/component/application/benchmark/dhrystone/src)

set(MODULE_IFLAGS ${DIR}/../include)

set(
    CSRC
    ${DIR}/dhry_1.c
    ${DIR}/dhry_2.c
)

ameba_internal_library(test_dhrystone)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})