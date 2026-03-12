ameba_internal_library(example_mqtt)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mqtt.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE
    ${BASEDIR}/component/network/mqtt/MQTTClient
)
