ameba_internal_library(example_mbed_watchdog)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_watchdog.c
    app_example.c
)