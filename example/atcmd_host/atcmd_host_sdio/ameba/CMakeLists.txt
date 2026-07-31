ameba_internal_library(example_atcmd_host_sdio)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_atcmd_host_sdio.c
    app_example.c
)
