ameba_internal_library(example_mbed_uart_int_rx_timeout)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_uart_int_rx_timeout.c
    app_example.c
)
