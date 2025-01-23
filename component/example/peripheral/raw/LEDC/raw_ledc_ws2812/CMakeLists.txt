ameba_internal_library(example_raw_ledc_ws2812)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_ledc_ws2812.c
    app_example.c
)
