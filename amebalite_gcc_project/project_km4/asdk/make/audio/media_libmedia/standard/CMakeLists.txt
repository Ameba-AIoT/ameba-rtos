set(DIR ${BASEDIR}/component/audio/media_libmedia/standard)
set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/cutils/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/interfaces
    ${BASEDIR}/component/audio/media_utils/include
    ${BASEDIR}/component/audio/media_common/include
    ${BASEDIR}/component/audio/media_core/standard/include
)

set(MODULE_CPP_DEFINES __RTOS__)
set(MODULE_CPP_OPTIONS -Wno-multichar -Wno-unused-parameter)

set(CPPSRC ${DIR}/rtplayer.cpp)

set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_CPP_DEFINES}")
set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES COMPILE_OPTIONS "${MODULE_CPP_OPTIONS}")
set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CPPSRC})
