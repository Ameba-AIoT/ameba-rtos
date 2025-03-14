set(GLOBAL_IFLAGS)

list(
    APPEND GLOBAL_IFLAGS
    ${OSDIR}/freertos
    ${OSDIR}/os_wrapper/include
    ${OSDIR}/freertos/heap_trace

    ${TARGETDIR}/cmsis
    ${TARGETDIR}/cmsis-dsp/Include
    ${TARGETDIR}/cmsis-dsp/PrivateInclude
    ${TARGETDIR}/fwlib/include
    ${TARGETDIR}/lib/include
    ${TARGETDIR}/app/monitor/include
    ${TARGETDIR}/app/xmodem
    ${TARGETDIR}/swlib
    ${TARGETDIR}/misc
    ${TARGETDIR}/hal/include
    ${TARGETDIR}/hal/src
    ${TARGETDIR}/usrcfg/include
    ${BASEDIR}/component/soc/common/include

    ${MENUCONFIGDIR}/project_${PROJECT_NAME}
    ${INCDIR}
    ${INCDIR}/..

    ${BASEDIR}/component/stdlib
)

#wifi
include(${WIFIMAKEDIR}/wifi_include.cmake)
list(
    APPEND GLOBAL_IFLAGS
    ${WIFI_IFLAGS}
)


list(
    APPEND GLOBAL_IFLAGS
    ${BASEDIR}/component/example


    ${BASEDIR}/component/network
    #${BASEDIR}/component/network/libcoap/include
    ${BASEDIR}/component/lwip/lwip_${LWIP_VER}/src/include
    #${BASEDIR}/component/lwip/lwip_${LWIP_VER}/src/include/lwip
    ${BASEDIR}/component/lwip/lwip_${LWIP_VER}/src/include/lwip/apps
    #${BASEDIR}/component/lwip/lwip_${LWIP_VER}/src/include/ipv4
    ${BASEDIR}/component/lwip/lwip_${LWIP_VER}/port/realtek
    ${BASEDIR}/component/lwip/lwip_${LWIP_VER}/port/realtek/freertos
    ${BASEDIR}/component/lwip/api

    ${BASEDIR}/component/os_dep

    ${BASEDIR}/component/wifi/wifi_fw/amebadplus/include
    ${BASEDIR}/component/wifi/wifi_fw/common


    #RTSP
    ${BASEDIR}/component/network/rtsp

    #SSL
    #${BASEDIR}/component/ssl/ssl_ram_map/rom

    #MBEDTLS
    ${BASEDIR}/component/ssl/mbedtls-${MBEDTLS_VER}/include
    ${BASEDIR}/component/ssl/mbedtls-${MBEDTLS_VER}/library
    ${BASEDIR}/component/ssl/mbedtls_ram_map/rom

    #apple
    ${BASEDIR}/component/application/apple/WACServer/External/Curve25519
    ${BASEDIR}/component/application/apple/WACServer/External/GladmanAES
    ${BASEDIR}/component/application/apple/homekit/crypto/chacha
    ${BASEDIR}/component/application/apple/homekit/crypto/poly1305
    ${BASEDIR}/component/application/apple/homekit/crypto/ed25519
    ${BASEDIR}/component/application/apple/homekit/crypto/ed25519/core
    ${BASEDIR}/component/application/apple/homekit/crypto/rom_ed25519
    ${BASEDIR}/component/application/apple/homekit/crypto/sha512

    #filesystem
    ${BASEDIR}/component/file_system/fatfs/${FATFS_VER}/include
    ${BASEDIR}/component/file_system/fatfs
    ${BASEDIR}/component/file_system/ftl
    ${BASEDIR}/component/file_system/ftl_common
    ${BASEDIR}/component/file_system/dct
    ${BASEDIR}/component/file_system/littlefs
    ${BASEDIR}/component/file_system/littlefs/${LITTLEFS_VER}
    ${BASEDIR}/component/file_system/kv
    ${BASEDIR}/component/file_system/vfs
    )

if(CONFIG_EMWIN_EN)
    list(
        APPEND GLOBAL_IFLAGS
        ${BASEDIR}/component/ui/emwin/emWinLibrary/include
    )
endif()

#coex
list(
    APPEND GLOBAL_IFLAGS
    ${BASEDIR}/component/rtk_coex
)

#bluetooth
if(CONFIG_BT)
    include(bluetooth_include.cmake)
endif()

#openthread
if(CONFIG_802154_THREAD_EN)
    list(
        APPEND GLOBAL_IFLAGS
        ${BASEDIR}/component/wpan/openthread/openthread/include
        ${BASEDIR}/component/wpan/openthread/openthread/src/core
        ${BASEDIR}/component/wpan/openthread/config
        ${BASEDIR}/component/wpan/openthread/openthread/third_party/mbedtls
    )
endif()

if(CONFIG_SPEEX_LIB)
    list(
        APPEND GLOBAL_IFLAGS
        ${BASEDIR}/component/example/audio/speexdsp
        ${BASEDIR}/component/audio/third_party/speexdsp/include
    )
endif()