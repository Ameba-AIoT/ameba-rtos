ameba_internal_library(example_vfs_sdcard)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_vfs_sdcard.c
    app_example.c
)
