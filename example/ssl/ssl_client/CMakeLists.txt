ameba_internal_library(example_ssl_client)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_ssl_client.c
    app_example.c
)
