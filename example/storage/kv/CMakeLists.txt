ameba_internal_library(example_kv)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_kv.c
    app_example.c
)
