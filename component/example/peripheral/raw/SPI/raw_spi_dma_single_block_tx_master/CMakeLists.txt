ameba_internal_library(example_raw_spi_dma_single_block_tx_master)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_spi_dma_single_block_tx_master.c
    app_example.c
)
