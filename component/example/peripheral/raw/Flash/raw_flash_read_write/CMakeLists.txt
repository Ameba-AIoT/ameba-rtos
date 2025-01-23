ameba_internal_library(example_raw_flash_read_write)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_flash_read_write.c
    app_example.c
)
