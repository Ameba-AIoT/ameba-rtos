ameba_internal_library(example_raw_flash_otp)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_flash_otp.c
    app_example.c
)
