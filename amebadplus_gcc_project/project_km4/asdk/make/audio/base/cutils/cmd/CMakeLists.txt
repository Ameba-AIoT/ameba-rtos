set(
    MODULE_IFLAGS 
    ${BASEDIR}/component/audio/base/cutils/interfaces
    ${BASEDIR}/component/audio/base/cutils/src/include
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
)

set(DIR ${BASEDIR}/component/audio/base/cutils/cmd)
set(MODULE_C_OPTIONS -D__RTOS__ -Wno-multichar -Wno-pointer-sign)
set(CSRC ${DIR}/cmd_main.c)

ameba_internal_library(audio_cmd)
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
