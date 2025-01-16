ameba_internal_library(example_wifi_user_reconnect)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE

    example_wifi_user_reconnect.c
    app_example.c
)
