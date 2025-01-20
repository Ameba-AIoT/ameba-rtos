ameba_internal_library(example_mbed_adc_norm_vol)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_adc_norm_vol.c
    app_example.c
)
