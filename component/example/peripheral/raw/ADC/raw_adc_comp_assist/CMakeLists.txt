ameba_internal_library(example_raw_adc_comp_assist)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_adc_comp_assist.c
    app_example.c
)
