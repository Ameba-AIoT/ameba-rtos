ameba_internal_library(example_raw_spi_trx_dma)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_spi_trx_dma.c
    app_example.c
)
