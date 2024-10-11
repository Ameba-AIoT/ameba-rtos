set(DIR ${BASEDIR}/component/network/mqtt/MQTTPacket)
set(
    CSRC 
    ${DIR}/MQTTConnectClient.c
    ${DIR}/MQTTConnectServer.c
    ${DIR}/MQTTDeserializePublish.c
    ${DIR}/MQTTFormat.c
    ${DIR}/MQTTPacket.c
    ${DIR}/MQTTSerializePublish.c
    ${DIR}/MQTTSubscribeClient.c
    ${DIR}/MQTTSubscribeServer.c
    ${DIR}/MQTTUnsubscribeClient.c
    ${DIR}/MQTTUnsubscribeServer.c
)

set(MODULE_C_OPTIONS  -Wno-implicit-fallthrough)

ameba_internal_library(mqtt_packet)
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
