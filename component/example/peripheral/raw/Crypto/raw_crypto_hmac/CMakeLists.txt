ameba_internal_library(example_raw_crypto_hmac)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_crypto_hmac.c
    app_example.c
)
