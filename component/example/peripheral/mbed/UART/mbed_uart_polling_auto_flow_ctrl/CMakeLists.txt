ameba_internal_library(example_mbed_uart_polling_auto_flow_ctrl)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_uart_polling_auto_flow_ctrl.c
    app_example.c
)
