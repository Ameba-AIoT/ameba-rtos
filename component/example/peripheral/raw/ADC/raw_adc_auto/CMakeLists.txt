ameba_internal_library(example_raw_adc_auto)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_adc_auto.c
    app_example.c
)
