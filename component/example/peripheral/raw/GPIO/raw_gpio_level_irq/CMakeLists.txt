ameba_internal_library(example_raw_gpio_level_irq)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_gpio_level_irq.c
    app_example.c
)
