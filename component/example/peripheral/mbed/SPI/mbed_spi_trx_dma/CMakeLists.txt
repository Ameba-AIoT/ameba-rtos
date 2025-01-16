ameba_internal_library(example_mbed_spi_trx_dma)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_spi_trx_dma.c
    app_example.c
)
