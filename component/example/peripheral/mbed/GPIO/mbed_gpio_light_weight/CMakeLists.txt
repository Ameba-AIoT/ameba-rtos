ameba_internal_library(example_mbed_gpio_light_weight)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_gpio_light_weight.c
    app_example.c
)
