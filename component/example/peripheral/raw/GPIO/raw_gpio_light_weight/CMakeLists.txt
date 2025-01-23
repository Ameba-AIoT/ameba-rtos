ameba_internal_library(example_raw_gpio_light_weight)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_gpio_light_weight.c
    app_example.c
)
