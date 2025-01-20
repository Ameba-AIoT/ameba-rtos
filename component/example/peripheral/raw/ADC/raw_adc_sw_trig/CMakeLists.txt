ameba_internal_library(example_raw_adc_sw_trig)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_adc_sw_trig.c
    app_example.c
)
