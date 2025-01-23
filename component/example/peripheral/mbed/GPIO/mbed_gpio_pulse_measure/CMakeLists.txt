ameba_internal_library(example_mbed_gpio_pulse_measure)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_gpio_pulse_measure.c
    app_example.c
)
