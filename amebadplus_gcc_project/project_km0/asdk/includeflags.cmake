set(GLOBAL_IFLAGS)

list(
    APPEND GLOBAL_IFLAGS
    ${OSDIR}
    ${OSDIR}/freertos
    ${OSDIR}/os_wrapper/include
    ${OSDIR}/freertos/heap_trace

    ${TARGETDIR}/cmsis/
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
    ${BASEDIR}/component/soc/common/include

    ${MENUCONFIGDIR}/project_${PROJECT_NAME}
    ${INCDIR}
    ${INCDIR}/..

    ${BASEDIR}/component/stdlib
    ${BASEDIR}/component/example
)

#wifi
include(${WIFIMAKEDIR}/wifi_include.cmake)
list(
    APPEND GLOBAL_IFLAGS
    ${WIFI_IFLAGS}
)

list(
    APPEND GLOBAL_IFLAGS
    ${BASEDIR}/component/network
    ${BASEDIR}/component/lwip/lwip_${LWIP_VER}/src/include
    #${BASEDIR}/component/lwip/lwip_${LWIP_VER}/src/include/lwip
    ${BASEDIR}/component/lwip/lwip_${LWIP_VER}/src/include/netif
    #${BASEDIR}/component/lwip/lwip_${LWIP_VER}/src/include/ipv4
    ${BASEDIR}/component/lwip/lwip_${LWIP_VER}/port/realtek
    ${BASEDIR}/component/lwip/lwip_${LWIP_VER}/port/realtek/freertos
    ${BASEDIR}/component/lwip/api

    ${BASEDIR}/component/os_dep

    ${WIFIFWDIR}/amebadplus/include
    ${WIFIFWDIR}/common

    #SSL
    #${BASEDIR}/component/ssl/ssl_ram_map/rom

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

    #${BASEDIR}/component/soc/amebadplus/sboot/Curve25519
    ${BASEDIR}/component/soc/amebadplus/sboot/ed25519
    ${BASEDIR}/component/soc/amebadplus/sboot/sha512
)

#coex
list(
    APPEND GLOBAL_IFLAGS
    ${BASEDIR}/component/rtk_coex
)

#bluetooth
if(CONFIG_BT)
    include(bluetooth_include.cmake)
endif()