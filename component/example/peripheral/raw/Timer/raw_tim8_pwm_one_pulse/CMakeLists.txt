ameba_internal_library(example_raw_tim8_pwm_one_pulse)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_tim8_pwm_one_pulse.c
    app_example.c
)
