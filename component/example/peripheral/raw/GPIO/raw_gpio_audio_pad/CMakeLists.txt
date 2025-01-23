ameba_internal_library(example_raw_gpio_audio_pad)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_gpio_audio_pad.c
    app_example.c
)
