#eg: cmake .. –DEXAMPLE=ssl_client
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
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/non_secure
)

set(MODULE_C_DEFINES)

if(CONFIG_USB_DEVICE_EN)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/usb/common
        ${BASEDIR}/component/usb/device/core
        ${BASEDIR}/component/usb/device/cdc_acm
        ${BASEDIR}/component/usb/device/composite
        ${BASEDIR}/component/usb/device/hid
        ${BASEDIR}/component/usb/device/uac
        ${BASEDIR}/component/usb/device/vendor
    )
    list(
        APPEND MODULE_C_DEFINES
        CONFIG_USB_FS
    )
endif()

if(CONFIG_IMQ_EN)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/soc/amebalite/app/ipc_app
        ${BASEDIR}/component/soc/amebalite/app/ipc_app/include
        ${BASEDIR}/component/soc/amebalite/app/ipc_app/src
        ${BASEDIR}/component/example/rpc
        ${BASEDIR}/component/audio/base/rpc/interfaces
        ${BASEDIR}/component/audio/base/rpc/src
        ${BASEDIR}/component/audio/base/rpc/xdr/include
        ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
        ${BASEDIR}/component/audio/base/log/interfaces
    )
    list(
        APPEND MODULE_C_DEFINES
        __RTOS__
        IMQ_SERVER
    )

endif()

if(CONFIG_LVGL_EN)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/ui/LVGL/lvgl
        ${BASEDIR}/component/ui/LVGL/lvgl/demos/stress
        ${BASEDIR}/component/ui/LVGL/lvgl/demos/benchmark
        ${BASEDIR}/component/ui/LVGL/lvgl/demos/widgets
        ${BASEDIR}/component/ui/LVGL/lv_drivers/interfaces
        ${BASEDIR}/component/ui/LVGL/port/amebadplus
    )
endif()

if(CONFIG_AUDIO_FWK)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/audio/interfaces
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
