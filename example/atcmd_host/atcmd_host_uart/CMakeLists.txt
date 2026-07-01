ameba_internal_library(example_atcmd_host_uart)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_atcmd_host_uart.c
    app_example.c
)
