set(DIR ${BASEDIR}/component/network/mqtt/MQTTClient)
set(
    MODULE_IFLAGS
    ${FREERTOSDIR}/include/
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/non_secure
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/secure
)

set(
    CSRC
    ${DIR}/MQTTClient.c
    ${DIR}/MQTTFreertos.c
)


ameba_internal_library(mqtt_client)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})

