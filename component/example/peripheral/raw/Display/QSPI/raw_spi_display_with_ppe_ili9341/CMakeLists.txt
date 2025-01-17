ameba_internal_library(example_raw_spi_dma_multi_block_rx_slave)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_spi_display_with_ppe_ili9341.c
    app_example.c
)
