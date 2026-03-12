ameba_internal_library(example_vfs_second_flash)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_vfs_second_flash.c
    app_example.c
)
