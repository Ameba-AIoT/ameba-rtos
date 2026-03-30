ameba_internal_library(example_vfs_sd_spi)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_vfs_sd_spi.c
    app_example.c
)
