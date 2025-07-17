ameba_internal_library(example_mbed_audio_speaker)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_audio_speaker.c
    app_example.c
)
