set(DIR ${BASEDIR}/component/application/benchmark/coremark)
set(MODULE_IFLAGS ${DIR}/../)
set(MODULE_C_OPTIONS -O3)

set(
    CSRC
    ${DIR}/core_list_join.c 
    ${DIR}/core_main.c 
    ${DIR}/core_matrix.c 
    ${DIR}/core_portme.c 
    ${DIR}/core_state.c 
    ${DIR}/core_util.c

)

ameba_internal_library(test_coremark)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})