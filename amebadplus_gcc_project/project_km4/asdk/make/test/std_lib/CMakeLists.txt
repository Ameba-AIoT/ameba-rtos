set(DIR ${TARGETDIR}/verification/swlib/std_lib)

set(
    CSRC
    ${DIR}/rtl_test_lib.c
    ${DIR}/rtl_test_string.c 
    ${DIR}/rtl_test_sscanf.c 
    ${DIR}/rtl_test_printf.c 
)

ameba_internal_library(test_std_lib)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
