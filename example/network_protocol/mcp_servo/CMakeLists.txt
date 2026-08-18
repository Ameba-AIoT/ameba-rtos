ameba_internal_library(example_mcp_servo)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mcp_servo.c
    app_example.c
)
