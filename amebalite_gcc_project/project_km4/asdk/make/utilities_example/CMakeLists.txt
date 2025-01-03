#eg: cmake .. –DEXAMPLE=ssl_client
message("EXAMPLE: " ${EXAMPLE})
set(
    MODULE_IFLAGS
    ${BASEDIR}/component/network/cJSON
    ${BASEDIR}/component/network/coap/include
    ${BASEDIR}/component/network/mqtt/MQTTClient
    ${BASEDIR}/component/network/httplite
    ${BASEDIR}/component/network/http2/nghttp2-1.31.0/includes
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

set(MODULE_C_OPTIOINS)

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

if(CONFIG_TFLITE_MICRO_EN)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/tflite_micro
        ${BASEDIR}/component/tflite_micro/third_party/gemmlowp
        ${BASEDIR}/component/tflite_micro/third_party/flatbuffers/include
    )
    list(
        APPEND MODULE_C_OPTIOINS
        -Wno-undef
        -Wno-unused-parameter
        -DTF_LITE_STATIC_MEMORY
    )
endif()

#NOTICE: The source file name and folder name for 'example' should match. That means when you
#want to define a example named 'app', the directory of the source file must be component/example/.../app/example_app.c
#The build cmd is "cmake .. -DEXPAMLE=app -G Ninja && ninja" or "./build.py -a app".
if (EXAMPLE)
    file(GLOB_RECURSE EXAMPLEDIR
        ${BASEDIR}/component/example/example_${EXAMPLE}.c
        ${BASEDIR}/component/example/example_${EXAMPLE}.cc)
    cmake_path(REMOVE_FILENAME EXAMPLEDIR)
    if(EXAMPLEDIR)
        message("THE PATH of example_${EXAMPLE}.c is " "${EXAMPLEDIR}")
    else()
        message(SEND_ERROR "example_${EXAMPLE}.c not found. Please check example name!")
    endif()
    file(GLOB_RECURSE EXAMPLE_SRC
        ${EXAMPLEDIR}*.c
        ${EXAMPLEDIR}*.cc)
    ameba_internal_library(utilities_example)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS} ${EXAMPLEDIR})
    target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
    target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIOINS})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${EXAMPLE_SRC})
endif()
