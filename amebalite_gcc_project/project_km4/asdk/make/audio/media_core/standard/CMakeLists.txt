set(DIR ${BASEDIR}/component/audio/media_core/standard)
set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/libs/src/include
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
    ${BASEDIR}/component/audio/base/cutils/interfaces
    ${BASEDIR}/component/audio/base/audio_utils/include
    ${BASEDIR}/component/audio/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces/osal_c
    ${BASEDIR}/component/audio/media_utils/include
    ${BASEDIR}/component/audio/media_common/include
    ${BASEDIR}/component/audio/media_source/include
    ${BASEDIR}/component/audio/media_demux/include
    ${BASEDIR}/component/audio/media_codec/codec-omx/include
    ${DIR}/include
)

set(MODULE_CPP_DEFINES __RTOS__)
set(MODULE_CPP_OPTIONS -Wno-multichar -Wno-unused-parameter)

set(
    CPPSRC
    ${DIR}/player_driver.cpp
    ${DIR}/player_core.cpp
    ${DIR}/player_core_decoder_base.cpp
    ${DIR}/player_core_decoder.cpp
    ${DIR}/player_core_renderer.cpp
    ${DIR}/generic_source.cpp
    ${DIR}/audio_output.cpp
)

set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_CPP_DEFINES}")
set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES COMPILE_OPTIONS "${MODULE_CPP_OPTIONS}")
set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CPPSRC})
