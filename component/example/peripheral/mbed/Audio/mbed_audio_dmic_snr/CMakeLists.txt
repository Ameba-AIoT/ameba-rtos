ameba_internal_library(example_mbed_audio_dmic_snr)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    snr_calculate.c
	audio_rx.c
    example_mbed_audio_dmic_snr.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE
    ${TARGETDIR}/cmsis_dsp/Include
)

