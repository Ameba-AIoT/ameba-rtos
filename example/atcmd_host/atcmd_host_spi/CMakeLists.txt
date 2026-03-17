ameba_internal_library(example_atcmd_host_spi)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_atcmd_host_spi.c
    app_example.c
)
