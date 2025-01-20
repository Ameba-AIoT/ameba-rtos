ameba_internal_library(example_raw_uart_int_trx)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_uart_int_trx.c
    app_example.c
)
