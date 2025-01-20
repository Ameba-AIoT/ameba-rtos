ameba_internal_library(example_raw_adc_tim_trig_vbat)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_adc_tim_trig_vbat.c
    app_example.c
)
