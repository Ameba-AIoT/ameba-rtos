ameba_internal_library(example_raw_uart_dma_rx_fc)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_uart_dma_rx_fc.c
    app_example.c
)
