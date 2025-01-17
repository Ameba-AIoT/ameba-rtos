ameba_internal_library(example_raw_crypto_aes)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_crypto_aes.c
    app_example.c
)
