ameba_internal_library(example_raw_uart_polling_trx)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_uart_polling_trx.c
    app_example.c
)
