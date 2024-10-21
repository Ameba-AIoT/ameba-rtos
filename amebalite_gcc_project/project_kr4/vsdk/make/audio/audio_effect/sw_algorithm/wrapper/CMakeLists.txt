set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/audio_effect/include
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Eq/include
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/cutils/interfaces
    ${BASEDIR}/component/audio/base/audio_utils/include
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Bundle/include
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Common/include
)

set(DIR ${BASEDIR}/component/audio/audio_effect/sw_algorithm/wrapper/Bundle)

set(MODULE_CPP_DEFINES __RTOS__)
set(MODULE_CPP_OPTIONS -Wno-multichar -Wno-unused-parameter -Werror)

set(CPPSRC ${DIR}/EffectBundle.cpp)

set_source_files_properties(${CPPSRC} DIRECTORY ../../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_CPP_DEFINES}")
set_source_files_properties(${CPPSRC} DIRECTORY ../../.. PROPERTIES COMPILE_OPTIONS "${MODULE_CPP_OPTIONS}")
set_source_files_properties(${CPPSRC} DIRECTORY ../../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(audio_mixer_${PROJECT_NAME} PRIVATE ${CPPSRC})
target_sources(audio_passthrough_${PROJECT_NAME} PRIVATE ${CPPSRC})
