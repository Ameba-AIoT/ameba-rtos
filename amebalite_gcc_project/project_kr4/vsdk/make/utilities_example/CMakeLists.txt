#eg: cmake .. –DEXAMPLE=ssl_client
message("EXAMPLE: " ${EXAMPLE})
set(
    MODULE_IFLAGS
    ${BASEDIR}/component/network/cJSON
    ${BASEDIR}/component/network/coap/include
    ${BASEDIR}/component/network/libcoap/include
    ${BASEDIR}/component/network/mqtt/MQTTClient
    ${BASEDIR}/component/network/http2/nghttp2-1.31.0/includes
    ${BASEDIR}/component/network/httplite
    ${BASEDIR}/component/network/xml
    ${BASEDIR}/component/bluetooth
    ${BASEDIR}/component/sdio/sd_host/inc
    ${BASEDIR}/component/wifi/realmesh/include
    ${BASEDIR}/component/bluetooth/example/bt_mesh_provisioner_rtk_demo
    ${BASEDIR}/component/bluetooth/example/bt_mesh_device_rtk_demo
    ${FREERTOSDIR}/include
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KR4
)


if(CONFIG_MEDIA_PLAYER)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/example/player
        ${BASEDIR}/component/example/audio/speexdsp
        ${BASEDIR}/component/example/mydata_source_player
        ${BASEDIR}/component/example/player_cache_test
        ${BASEDIR}/component/audio/interfaces
        ${BASEDIR}/component/audio/base/libs/src/include
        ${BASEDIR}/component/audio/base/log/interfaces
        ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
        ${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
        ${BASEDIR}/component/audio/base/cutils/interfaces
        ${BASEDIR}/component/audio/base/utils/interfaces
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

#NOTICE: The source file name and folder name for 'example' should match. That means when you
#want to define a example named 'app', the directory of the source file must be component/example/.../app/example_app.c
#The build cmd is "cmake .. -DEXPAMLE=app -G Ninja && ninja" or "./build.py -a app".
if (EXAMPLE)
    file(GLOB_RECURSE EXAMPLEDIR ${BASEDIR}/component/example/example_${EXAMPLE}.c)
    cmake_path(REMOVE_FILENAME EXAMPLEDIR)
    if(EXAMPLEDIR)
        message("THE PATH of example_${EXAMPLE}.c is " "${EXAMPLEDIR}")
    else()
        message(SEND_ERROR "example_${EXAMPLE}.c not found. Please check example name!")
    endif()
    file(GLOB EXAMPLE_SRC ${EXAMPLEDIR}*.c)
    ameba_internal_library(utilities_example)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${EXAMPLE_SRC})
endif()
