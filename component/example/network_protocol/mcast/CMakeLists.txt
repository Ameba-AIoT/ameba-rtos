ameba_internal_library(example_mcast)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mcast.c
    app_example.c
)
