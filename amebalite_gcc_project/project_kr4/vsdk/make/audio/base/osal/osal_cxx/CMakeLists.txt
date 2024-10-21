set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
    ${BASEDIR}/component/audio/base/libs/src/include
)

set(DIR ${BASEDIR}/component/audio/base/osal/osal_cxx)

set(
    CPPSRC
    ${DIR}/src/refptr.cpp
    ${DIR}/adapter/freertos/mutex.cpp
    ${DIR}/adapter/freertos/condition.cpp
    ${DIR}/adapter/freertos/threads.cpp
)

set(MODULE_CPP_DEFINES __RTOS__)
set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_CPP_DEFINES}")
set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CPPSRC})