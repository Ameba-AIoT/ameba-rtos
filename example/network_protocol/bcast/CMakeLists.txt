ameba_internal_library(example_bcast)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_bcast.c
    app_example.c
)
