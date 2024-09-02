set(DIR ${BASEDIR}/component/application/benchmark/coremark)

set(MODULE_IFLAGS -I${DIR}/../)
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

if(CONFIG_INTERNAL_LIB)
    ameba_internal_library(test_coremark)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
else()
    set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
    set_source_files_properties(${CSRC} DIRECTORY .. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")
    target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})
endif()