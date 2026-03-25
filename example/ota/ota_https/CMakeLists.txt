ameba_internal_library(example_ota_https)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_ota_https.c
    app_example.c
)
