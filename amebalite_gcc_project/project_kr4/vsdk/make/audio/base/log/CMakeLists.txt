set(MODULE_IFLAGS ${BASEDIR}/component/audio/base/log/interfaces)
set(DIR ${BASEDIR}/component/audio/base/log)

set(
    CSRC
    ${DIR}/log.c
)
set(MODULE_C_DEFINES __RTOS__)
set_source_files_properties(${CSRC} DIRECTORY .. PROPERTIES COMPILE_DEFINITIONS "${MODULE_C_DEFINES}")
set_source_files_properties(${CSRC} DIRECTORY .. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
