ameba_internal_library(example_websocket_client)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_websocket_client.c
    app_example.c
)
