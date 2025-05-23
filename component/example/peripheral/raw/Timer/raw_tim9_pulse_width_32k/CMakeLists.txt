ameba_internal_library(example_tim)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_tim9_pulse_width_32k.c
    app_example.c
)
