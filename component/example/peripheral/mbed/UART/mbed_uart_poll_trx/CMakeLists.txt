ameba_internal_library(example_uart)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_uart_poll_trx.c
    app_example.c
)
