ameba_internal_library(example_raw_spi_multislave)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_spi_multislave.c
    app_example.c
)
