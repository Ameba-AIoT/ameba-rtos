ameba_internal_library(example_socket_select)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_socket_select.c
    app_example.c
)
