set(DIR ${BASEDIR}/component/audio/third_party/libgsm)
set(MODULE_IFLAGS ${DIR}/inc)

set(MODULE_C_DEFINES __RTOS__ SASR WAV49 WMOPS=1)
set(MODULE_C_OPTIONS -Wno-error)
set(
    CSRC
    ${DIR}/src/add.c
    ${DIR}/src/code.c
    ${DIR}/src/decode.c
    ${DIR}/src/gsm_create.c
    ${DIR}/src/gsm_decode.c
    ${DIR}/src/gsm_destroy.c
    ${DIR}/src/gsm_encode.c
    ${DIR}/src/gsm_option.c
    ${DIR}/src/long_term.c
    ${DIR}/src/lpc.c
    ${DIR}/src/preprocess.c
    ${DIR}/src/rpe.c
    ${DIR}/src/short_term.c
    ${DIR}/src/table.c
)

ameba_app_library(gsm)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
