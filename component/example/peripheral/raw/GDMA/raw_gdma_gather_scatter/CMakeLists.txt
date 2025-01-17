ameba_internal_library(example_raw_gdma_gather_scatter)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_gdma_gather_scatter.c
    app_example.c
)
