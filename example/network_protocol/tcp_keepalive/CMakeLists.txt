ameba_internal_library(example_tcp_keepalive)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_tcp_keepalive.c
    app_example.c
)
