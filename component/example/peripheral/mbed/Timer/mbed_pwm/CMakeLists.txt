ameba_internal_library(example_mbed_pwm)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_pwm.c
    app_example.c
)