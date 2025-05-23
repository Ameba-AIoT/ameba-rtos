ameba_internal_library(example_spi)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_spi_dma_sblk_tx_mstr.c
    app_example.c
)
