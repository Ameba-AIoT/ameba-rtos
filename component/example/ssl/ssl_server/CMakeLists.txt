ameba_internal_library(example_ssl_server)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_ssl_server.c
    app_example.c
)
