ameba_internal_library(example_mbed_rtc)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_rtc.c
    app_example.c
)