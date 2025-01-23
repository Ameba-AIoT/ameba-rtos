ameba_internal_library(example_mbed_gpio_edge_irq)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_gpio_edge_irq.c
    app_example.c
)
