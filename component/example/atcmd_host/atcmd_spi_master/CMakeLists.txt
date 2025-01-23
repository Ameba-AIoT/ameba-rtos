ameba_internal_library(example_atcmd_spi_master)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_atcmd_spi_master.c
    app_example.c
)
