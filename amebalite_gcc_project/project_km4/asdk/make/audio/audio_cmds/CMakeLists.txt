set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/audio_utils/include
    ${BASEDIR}/component/audio/base/cutils/interfaces
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/libs/src/include
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
    ${BASEDIR}/component/audio/audio_algorithm/sonic
    ${BASEDIR}/component/audio/interfaces
)

set(AUDIO_CMDS_DIR ${BASEDIR}/component/audio/audio_cmds)
set(MODULE_C_DEFINES __RTOS__)
set(MODULE_C_OPTIONS -Wno-multichar -Wno-unused-parameter -Wno-narrowing)
set(CSRC ${AUDIO_CMDS_DIR}/audio_uart_handler.c)

ameba_internal_library(audio_cmds)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
