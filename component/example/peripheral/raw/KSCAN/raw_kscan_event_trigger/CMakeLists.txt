ameba_internal_library(example_raw_kscan_event_trigger)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_kscan_event_trigger.c
    app_example.c
)
