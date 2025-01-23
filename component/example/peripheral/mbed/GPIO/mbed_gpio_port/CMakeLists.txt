ameba_internal_library(example_mbed_gpio_port)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_gpio_port.c
    app_example.c
)
