ameba_internal_library(example_raw_tim9_capture_pulse_width_32k)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_tim9_capture_pulse_width_32k.c
    app_example.c
)
