ameba_internal_library(example_raw_gpio_rw)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_gpio_rw.c
    app_example.c
)
