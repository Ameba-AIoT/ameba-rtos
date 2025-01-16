ameba_internal_library(example_websocket_server)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_websocket_server.c
    app_example.c
)
