ameba_internal_library(example_mbed_flash_read_write)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_flash_read_write.c
    app_example.c
)
