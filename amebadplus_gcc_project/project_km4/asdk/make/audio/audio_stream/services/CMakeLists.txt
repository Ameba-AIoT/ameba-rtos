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
    ${BASEDIR}/component/audio/audio_effect/include
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/wrapper/Bundle
    ${BASEDIR}/component/audio/configs/include
)

set(DIR ${BASEDIR}/component/audio/audio_stream/services)

set(MODULE_CPP_DEFINES __RTOS__)
set(MODULE_CPP_OPTIONS -Wno-multichar -Wno-unused-parameter -Wno-unused-value -Wno-aligned-new -Wno-literal-suffix -Werror)
set(
    CPPSRC 
    ${DIR}/track_shared.cpp
    ${DIR}/buffer_agents.cpp
    ${DIR}/raw_resample_coef.cpp
    ${DIR}/linear_raw_resampler.cpp
    ${DIR}/sinc_raw_resampler.cpp
    ${DIR}/raw_resampler.cpp
    ${DIR}/raw_mixer.cpp
    ${DIR}/general_tracks.cpp
    ${DIR}/effects.cpp
    ${DIR}/streaming_threads.cpp
    ${DIR}/general_audio.cpp
    ${DIR}/audio_stream_render.cpp
)

set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_CPP_DEFINES}")
set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES COMPILE_OPTIONS "${MODULE_CPP_OPTIONS}")
set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")

target_sources(audio_mixer_${PROJECT_NAME} PRIVATE ${CPPSRC})
