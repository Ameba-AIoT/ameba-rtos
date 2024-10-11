set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/base/libs/src/include
    ${FREERTOSDIR}/include/
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/non_secure
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/secure
)

set(DIR ${BASEDIR}/component/audio/base/osal/osal_c)

set(
    CSRC
    ${DIR}/adapter/freertos/osal_mem.c
    ${DIR}/adapter/freertos/osal_mutex.c
    ${DIR}/adapter/freertos/osal_sem.c
    ${DIR}/adapter/freertos/osal_condition.c
    ${DIR}/adapter/freertos/osal_thread.c
    ${DIR}/adapter/freertos/osal_time.c
    ${DIR}/adapter/freertos/osal_timer.c
    ${DIR}/adapter/freertos/osal_strdup.c
    ${DIR}/adapter/freertos/osal_vasprintf.c
)

set(MODULE_C_DEFINES __RTOS__)
set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_C_DEFINES}")
set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})