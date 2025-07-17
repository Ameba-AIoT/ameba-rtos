ameba_internal_library(example_mbed_audio_amic_snr)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    audio_rx.c
    snr_calculate.c
    example_mbed_audio_amic_snr.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE
    ${TARGETDIR}/cmsis_dsp/Include
)

