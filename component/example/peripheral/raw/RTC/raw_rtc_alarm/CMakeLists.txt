ameba_internal_library(example_raw_rtc_alarm)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_rtc_alarm.c
    app_example.c
)
