ameba_internal_library(example_wifi_raw)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE

    example_wifi_raw.c
    app_example.c
)
