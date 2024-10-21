#eg: cmake .. –DEXAMPLE=ssl_client
# if(CONFIG_WLAN_KR4_AP_KM4_NP)
#     set(EXAMPLE)
# endif()
message("EXAMPLE: " ${EXAMPLE})
set(
    MODULE_IFLAGS
    ${BASEDIR}/component/network/cJSON
    ${BASEDIR}/component/network/coap/include
    ${BASEDIR}/component/network/mqtt/MQTTClient
    ${BASEDIR}/component/network/httplite
    ${BASEDIR}/component/network/xml
    ${BASEDIR}/component/bluetooth
    ${BASEDIR}/component/sdio/sd_host/inc
    ${BASEDIR}/component/wifi/realmesh/include
    ${BASEDIR}/component/bluetooth/example/bt_mesh_provisioner_rtk_demo
    ${BASEDIR}/component/bluetooth/example/bt_mesh_device_rtk_demo
    ${FREERTOSDIR}/include
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/non_secure
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/secure
)


if(CONFIG_MEDIA_PLAYER)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/example/audio/player
        ${BASEDIR}/component/example/audio/mydata_source_player
        ${BASEDIR}/component/example/audio/player_cache_test
        ${BASEDIR}/component/audio/interfaces
        ${BASEDIR}/component/audio/base/libs/src/include
        ${BASEDIR}/component/audio/base/log/interfaces
        ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
        ${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
        ${BASEDIR}/component/audio/base/cutils/interfaces
        ${BASEDIR}/component/audio/base/utils/interfaces
    )
endif()

if(CONFIG_SPEEX_LIB)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/example/audio/speexdsp
        ${BASEDIR}/component/audio/third_party/speexdsp/include
    )
endif()

if(CONFIG_AUDIO_FWK)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/example/audio/audio_track
        ${BASEDIR}/component/example/audio/audio_record
        ${BASEDIR}/component/audio/interfaces
        ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    )
endif()

if(CONFIG_RPC_EN)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/utils/ipc/rpc/include
        ${BASEDIR}/component/utils/ipc/rpc/xdr/include
        ${BASEDIR}/component/utils/ipc/rpc/src/aidl
        ${BASEDIR}/component/example/audio/voice
        ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    )
endif()


if (EXAMPLE)
    file(GLOB_RECURSE EXAMPLEDIR ${BASEDIR}/component/example/example_${EXAMPLE}.c)
    cmake_path(REMOVE_FILENAME EXAMPLEDIR)
    message("THE PATH of example_${EXAMPLE}.c is " "${EXAMPLEDIR}")
    file(GLOB EXAMPLE_SRC ${EXAMPLEDIR}*.c)
    ameba_internal_library(utilities_example)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${EXAMPLE_SRC})
endif()
