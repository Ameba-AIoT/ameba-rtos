set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/audio_utils/include
    ${BASEDIR}/component/audio/base/cutils/interfaces
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/libs/src/include
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
    ${BASEDIR}/component/audio/audio_algorithm/sonic
    ${BASEDIR}/component/audio/audio_stream/core
    ${BASEDIR}/component/audio/audio_stream/services
    ${BASEDIR}/component/audio/interfaces
    ${BASEDIR}/component/audio/audio_effect/core
    ${BASEDIR}/component/audio/audio_effect/include
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/wrapper/Bundle
)

set(DIR ${BASEDIR}/component/audio/audio_effect/passthrough)

set(MODULE_DEFINES __RTOS__)
set(MODULE_OPTIONS -Wno-multichar -Wno-unused-parameter -Wno-narrowing)

set(CSRC ${DIR}/effect_module.c)
set(CPPSRC ${DIR}/audio_equalizer.cpp ${DIR}/effect_base.cpp ${DIR}/equalizer_client.cpp)
set(SRC ${CPPSRC} ${CSRC})

set_source_files_properties(${SRC} DIRECTORY ../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_DEFINES}")
set_source_files_properties(${SRC} DIRECTORY ../.. PROPERTIES COMPILE_OPTIONS "${MODULE_OPTIONS}")
set_source_files_properties(${SRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")

target_sources(audio_passthrough_${PROJECT_NAME} PRIVATE ${SRC})
