ameba_internal_library(example_httpc)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_httpc.c
    app_example.c
)
