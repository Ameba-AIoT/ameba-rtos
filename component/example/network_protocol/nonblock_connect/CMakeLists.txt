ameba_internal_library(example_nonblock_connect)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_nonblock_connect.c
    app_example.c
)
