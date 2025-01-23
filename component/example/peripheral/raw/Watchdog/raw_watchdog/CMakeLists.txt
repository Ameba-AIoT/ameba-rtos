ameba_internal_library(example_raw_watchdog)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_watchdog.c
    app_example.c
)
