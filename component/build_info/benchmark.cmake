if (__INCLUDED_BENCHMARK_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_BENCHMARK_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_benchmark_coremark INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_benchmark_cpuburn INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_benchmark_dhrystone INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_benchmark_memtester INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_benchmark_vmin INTERFACE)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_benchmark_coremark p_SCOPE interface ${c_CMPT_APPLICATION_DIR}/benchmark)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_benchmark_coremark p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/benchmark/coremark/core_list_join.c
    ${c_CMPT_APPLICATION_DIR}/benchmark/coremark/core_main.c
    ${c_CMPT_APPLICATION_DIR}/benchmark/coremark/core_matrix.c
    ${c_CMPT_APPLICATION_DIR}/benchmark/coremark/core_portme.c
    ${c_CMPT_APPLICATION_DIR}/benchmark/coremark/core_state.c
    ${c_CMPT_APPLICATION_DIR}/benchmark/coremark/core_util.c
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_benchmark_coremark p_SCOPE interface
    -O3
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_benchmark_cpuburn p_SCOPE interface ${c_CMPT_APPLICATION_DIR}/benchmark)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_benchmark_cpuburn p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/benchmark/cpuburn/cpuburn-a53.S
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_benchmark_cpuburn p_SCOPE interface
    -O3
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_benchmark_dhrystone p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/benchmark/dhrystone/include
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_benchmark_dhrystone p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/benchmark/dhrystone/src/dhry_1.c
    ${c_CMPT_APPLICATION_DIR}/benchmark/dhrystone/src/dhry_2.c
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_benchmark_memtester p_SCOPE interface ${c_CMPT_APPLICATION_DIR}/benchmark)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_benchmark_memtester p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/benchmark/memtester/memtester.c
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_benchmark_memtester p_SCOPE interface
    -O3
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_benchmark_vmin p_SCOPE interface
    ${c_FREERTOS_DIR}/include
    ${c_FREERTOS_DIR}/portable/GCC/ARM_CA7
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_benchmark_vmin p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/benchmark/memtester/memtester.c
)
#--------------------------#