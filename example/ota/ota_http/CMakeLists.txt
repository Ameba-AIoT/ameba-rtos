ameba_internal_library(example_ota_http)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_ota_http.c
    app_example.c
)
