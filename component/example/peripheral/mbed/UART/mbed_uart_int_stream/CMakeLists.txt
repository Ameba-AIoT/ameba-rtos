ameba_internal_library(example_mbed_uart_int_stream)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_uart_int_stream.c
    app_example.c
)
