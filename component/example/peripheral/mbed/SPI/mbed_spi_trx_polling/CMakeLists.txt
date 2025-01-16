ameba_internal_library(example_mbed_spi_trx_polling)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_spi_trx_polling.c
    app_example.c
)
