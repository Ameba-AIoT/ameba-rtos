ameba_internal_library(example_ota)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_ota.c
    app_example.c
)
