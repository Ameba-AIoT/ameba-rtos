set(GLOBAL_IFLAGS)

list(
    APPEND GLOBAL_IFLAGS
    ${OSDIR}
    ${OSDIR}/freertos
    ${OSDIR}/os_wrapper/include

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
    ${BASEDIR}/component/soc/common

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
    ${BASEDIR}/component/lwip/lwip_v2.1.2/src/include
    #${BASEDIR}/component/lwip/lwip_v2.1.2/src/include/lwip
    ${BASEDIR}/component/lwip/lwip_v2.1.2/src/include/netif
    #${BASEDIR}/component/lwip/lwip_v2.1.2/src/include/ipv4
    ${BASEDIR}/component/lwip/lwip_v2.1.2/port/realtek
    ${BASEDIR}/component/lwip/lwip_v2.1.2/port/realtek/freertos
    ${BASEDIR}/component/lwip/api
    
    ${BASEDIR}/component/os_dep
    
    ${WIFIFWDIR}/amebadplus/include
    ${WIFIFWDIR}/common
    
    #SSL
    #${BASEDIR}/component/ssl/ssl_ram_map/rom
    
    #MBEDTLS
    ${BASEDIR}/component/ssl/mbedtls-2.16.9/include
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

