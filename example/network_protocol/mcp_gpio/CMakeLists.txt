ameba_internal_library(example_mcp_gpio)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mcp_gpio.c
    app_example.c
)
