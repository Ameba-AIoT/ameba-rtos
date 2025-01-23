ameba_internal_library(example_raw_pwm)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_pwm.c
    app_example.c
)
