ameba_internal_library(example_ssdp)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_ssdp.c
    app_example.c
)
