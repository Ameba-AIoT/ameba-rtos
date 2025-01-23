ameba_internal_library(example_mbed_gpio_jtag)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_gpio_jtag.c
    app_example.c
)
