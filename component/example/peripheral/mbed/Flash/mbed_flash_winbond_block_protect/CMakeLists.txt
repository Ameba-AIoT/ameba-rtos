ameba_internal_library(example_mbed_flash_winbond_block_protect)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_flash_winbond_block_protect.c
    app_example.c
)
