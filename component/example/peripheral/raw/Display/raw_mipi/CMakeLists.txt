ameba_internal_library(example_raw_mipi)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_mipi.c
    ameba_mipi_show.c
    app_example.c
)
