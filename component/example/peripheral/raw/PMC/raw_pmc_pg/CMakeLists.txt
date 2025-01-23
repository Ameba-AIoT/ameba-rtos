ameba_internal_library(example_raw_pmc_pg)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_pmc_pg.c
    app_example.c
)
