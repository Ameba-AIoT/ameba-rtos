ameba_internal_library(example_mbed_gpio_rw)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_gpio_rw.c
    app_example.c
)
