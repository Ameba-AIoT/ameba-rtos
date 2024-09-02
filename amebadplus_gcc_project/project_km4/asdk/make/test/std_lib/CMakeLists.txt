set(DIR ${TARGETDIR}/verification/swlib/std_lib)

set(
    CSRC
    ${DIR}/rtl_test_lib.c
    ${DIR}/rtl_test_string.c 
    ${DIR}/rtl_test_sscanf.c 
    ${DIR}/rtl_test_printf.c 
)

target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})