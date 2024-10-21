set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/interfaces
)

set(DIR ${BASEDIR}/component/audio/audio_hal/amebalite)

set(MODULE_C_DEFINES __RTOS__ NDEBUG)
set(MODULE_C_OPTIONS -UDEBUG -Werror)

set(
    CSRC
    ${DIR}/ameba_audio_stream_buffer.c
    ${DIR}/ameba_audio_stream.c
    ${DIR}/ameba_audio_stream_control.c
    ${DIR}/ameba_audio_stream_utils.c
    ${DIR}/ameba_audio_stream_render.c
    ${DIR}/ameba_audio_stream_capture.c
    ${DIR}/primary_audio_hw_stream_out.c
    ${DIR}/primary_audio_hw_card.c
    ${DIR}/primary_audio_hw_stream_in.c
    ${DIR}/audio_hw_params_handle.c
    ${DIR}/audio_hw_manager.c
    ${DIR}/audio_hw_control.c
    ${DIR}/audio_hw_mix.c
)

ameba_internal_library(audio_hal)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
