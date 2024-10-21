set(GLOBAL_IFLAGS)

list(
    APPEND GLOBAL_IFLAGS
    ${OSDIR}
    ${OSDIR}/freertos
    ${OSDIR}/os_wrapper/include

    ${TARGETDIR}/cmsis
    ${TARGETDIR}/cmsis/device
    ${TARGETDIR}/fwlib/include
    ${TARGETDIR}/lib/include
    ${TARGETDIR}/app/monitor/include
    ${TARGETDIR}/app/xmodem
    ${TARGETDIR}/swlib
    ${TARGETDIR}/misc
    ${TARGETDIR}/hal/include
    ${TARGETDIR}/hal/src
    ${TARGETDIR}/usrcfg/include
    ${BASEDIR}/component/soc/common

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
    ${BASEDIR}/component/lwip/lwip_${LWIP_VER}/src/include/netif
    #${BASEDIR}/component/lwip/lwip_${LWIP_VER}/src/include/ipv4
    ${BASEDIR}/component/lwip/lwip_${LWIP_VER}/port/realtek
    ${BASEDIR}/component/lwip/lwip_${LWIP_VER}/port/realtek/freertos
    ${BASEDIR}/component/lwip/api

    ${BASEDIR}/component/os_dep

    ${BASEDIR}/component/wifi/wifi_fw/amebalite/include
    ${BASEDIR}/component/wifi/wifi_fw/common

    #SSL
    ${BASEDIR}/component/ssl/ssl_ram_map/rom

    #MBEDTLS
    ${BASEDIR}/component/ssl/mbedtls-${MBEDTLS_VER}/include
    ${BASEDIR}/component/ssl/mbedtls_ram_map/rom

    #apple
    ${BASEDIR}/component/application/apple/WACServer/External/Curve25519
    ${BASEDIR}/component/application/apple/WACServer/External/GladmanAES
    ${BASEDIR}/component/application/apple/homekit/crypto/chacha
    ${BASEDIR}/component/application/apple/homekit/crypto/poly1305
    ${BASEDIR}/component/application/apple/homekit/crypto/ed25519
    ${BASEDIR}/component/application/apple/homekit/crypto/ed25519/core
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


#bluetooth
if(CONFIG_BT)
    include(bluetooth_include.cmake)
endif()
