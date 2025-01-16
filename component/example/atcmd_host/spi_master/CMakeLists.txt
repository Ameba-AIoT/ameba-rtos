ameba_internal_library(example_spi_master)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_spi_master.c
    app_example.c
)
