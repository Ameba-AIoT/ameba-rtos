ameba_internal_library(example_vfs)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_vfs.c
    app_example.c
)
