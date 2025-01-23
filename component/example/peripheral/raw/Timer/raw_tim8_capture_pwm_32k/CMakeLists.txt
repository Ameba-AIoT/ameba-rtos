ameba_internal_library(example_raw_tim8_capture_pwm_32k)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_tim8_capture_pwm_32k.c
    app_example.c
)
