ameba_internal_library(example_socket_tcp_multithread)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_socket_tcp_multithread.c
    app_example.c
)
