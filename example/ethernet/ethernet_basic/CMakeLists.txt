ameba_internal_library(example_ethernet_basic)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_ethernet_basic.c
    app_example.c
)
