ameba_internal_library(example_mbed_uart_int_trx)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_uart_int_trx.c
    app_example.c
)
