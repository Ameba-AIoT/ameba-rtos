ameba_internal_library(example_mbed_uart_dma_rx_fc)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_uart_dma_rx_fc.c
    app_example.c
)
