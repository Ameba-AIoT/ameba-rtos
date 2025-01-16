ameba_internal_library(example_lwip_mdns)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_lwip_mdns.c
    app_example.c
)
