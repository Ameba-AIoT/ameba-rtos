ameba_internal_library(example_ssl_client_verify_required)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_ssl_client_verify_required.c
    app_example.c
)
