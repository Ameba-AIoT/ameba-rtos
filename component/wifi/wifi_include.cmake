set(
    WIFI_IFLAGS
    ${BASEDIR}/component/wifi/api
    ${BASEDIR}/component/wifi/driver
    ${BASEDIR}/component/wifi/driver/core
    ${BASEDIR}/component/wifi/driver/core_rom
    ${BASEDIR}/component/wifi/driver/halcommon_rom
    ${BASEDIR}/component/wifi/driver/include
    ${BASEDIR}/component/wifi/wifi_reg/common
    ${BASEDIR}/component/wifi/driver/intf
    ${BASEDIR}/component/wifi/driver/halmac
    ${BASEDIR}/component/wifi/driver/halcommon
    ${BASEDIR}/component/wifi/driver/halbtc
    ${BASEDIR}/component/wifi/driver/phl
    ${BASEDIR}/component/wifi/driver/p2p
    ${BASEDIR}/component/wifi/wpa_supplicant/src
    ${BASEDIR}/component/wifi/wpa_supplicant/src/crypto
    ${BASEDIR}/component/wifi/wpa_supplicant/wpa_supplicant
    ${BASEDIR}/component/wifi/wifi_fast_connect
    ${BASEDIR}/component/wifi/wifi_auto_reconnect
    ${BASEDIR}/component/wifi/simple_config
    ${BASEDIR}/component/wifi/common
    ${BASEDIR}/component/wifi/common/secure
    ${BASEDIR}/component/wifi/wpa_supplicant/wpa_lite
    ${BASEDIR}/component/wifi/wpa_supplicant/wpa_lite/rom
    ${BASEDIR}/component/wifi/driver/nan
    ${BASEDIR}/component/wifi/driver/mesh
    ${BASEDIR}/component/wifi/driver/wifi_tunnel
    ${BASEDIR}/component/at_cmd
)

if(CONFIG_WPA_STD)
    list(
        APPEND WIFI_IFLAGS
        ${BASEDIR}/component/wifi/wpa_supplicant_std/hostapd
        ${BASEDIR}/component/wifi/wpa_supplicant_std/wpa_supplicant
        ${BASEDIR}/component/wifi/wpa_supplicant_std/freertos
        ${BASEDIR}/component/wifi/wpa_supplicant_std/freertos/src
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/ap
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/common
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/crypto
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/drivers
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/eap_common
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/eap_peer
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/eap_server
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/eapol_auth
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/eapol_supp
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/fst
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/l2_packet
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/p2p
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/pae
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/radius
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/rsn_supp
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/tls
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/utils
        ${BASEDIR}/component/wifi/wpa_supplicant_std/src/wps
        ${BASEDIR}/component/wifi/wpa_supplicant_std/port/mbedtls
    )
endif()

if(CONFIG_AMEBAL2)
    list(
        APPEND WIFI_IFLAGS
        ${BASEDIR}/component/wifi/wifi_reg/rle1331
        ${BASEDIR}/component/wifi/driver/intf/g6_intf
        ${BASEDIR}/component/wifi/driver/intf/g6_intf/rle1331
        ${BASEDIR}/component/wifi/driver/phy_g6/bb
        ${BASEDIR}/component/wifi/driver/phy_g6/bb/halbb_6955
        ${BASEDIR}/component/wifi/driver/phy_g6/rf
        ${BASEDIR}/component/wifi/driver/phy_g6/rf/halrf_6955
        ${BASEDIR}/component/wifi/driver/rle1331
        ${BASEDIR}/component/wifi/driver/rle1331_rom

)
endif()

if(CONFIG_AMEBAGREEN2 OR CONFIG_AMEBAPRO3)
    list(
        APPEND WIFI_IFLAGS
        ${BASEDIR}/component/wifi/wifi_reg/rtl7005
        ${BASEDIR}/component/wifi/driver/intf/g6_intf
        ${BASEDIR}/component/wifi/driver/intf/g6_intf/rtl8720f
        ${BASEDIR}/component/wifi/driver/phy_g6/bb
        ${BASEDIR}/component/wifi/driver/phy_g6/bb/halbb_8720f
        ${BASEDIR}/component/wifi/driver/phy_g6/rf
        ${BASEDIR}/component/wifi/driver/rtl8720f
        ${BASEDIR}/component/wifi/driver/rtl8720f_rom

    )
endif()

if(CONFIG_AMEBAGREEN2)
    list(
        APPEND WIFI_IFLAGS
        ${BASEDIR}/component/wifi/driver/wifi_tunnel/rtl8720f
    )

endif()

if(CONFIG_AMEBADPLUS)
    list(
        APPEND WIFI_IFLAGS
        ${BASEDIR}/component/wifi/wifi_reg/rtl8721da
        ${BASEDIR}/component/wifi/driver/rtl8721da
        ${BASEDIR}/component/wifi/driver/rtl8721da_rom
        ${BASEDIR}/component/wifi/driver/phydm
        ${BASEDIR}/component/wifi/driver/wifi_tunnel/include
        ${BASEDIR}/component/wifi/driver/wifi_tunnel/rtl8721da
    )

endif()

if(CONFIG_AMEBAD)
    list(
        APPEND WIFI_IFLAGS
        ${BASEDIR}/component/wifi/wifi_reg/rtl8721d
        ${BASEDIR}/component/wifi/driver/rtl8721d
        ${BASEDIR}/component/wifi/driver/rtl8721d_rom
        ${BASEDIR}/component/wifi/driver/phydm
    )
endif()

if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    list(
        APPEND WIFI_IFLAGS
        ${BASEDIR}/component/wifi/wifi_reg/rtl8730e
        ${BASEDIR}/component/wifi/driver/intf/g6_intf
        ${BASEDIR}/component/wifi/driver/intf/g6_intf/rtl8730e
        ${BASEDIR}/component/wifi/driver/rtl8730e_rom
        ${BASEDIR}/component/wifi/driver/phy_g6/bb
        ${BASEDIR}/component/wifi/driver/phy_g6/bb/halbb_8730e
        ${BASEDIR}/component/wifi/driver/phy_g6/rf
    )
endif()

if(CONFIG_AMEBALITE)
    list(
        APPEND WIFI_IFLAGS
        ${BASEDIR}/component/wifi/wifi_reg/rtl8720e
        ${BASEDIR}/component/wifi/driver/intf/g6_intf
        ${BASEDIR}/component/wifi/driver/intf/g6_intf/rtl8720e
        ${BASEDIR}/component/wifi/driver/phy_g6/bb
        ${BASEDIR}/component/wifi/driver/phy_g6/rf
        ${BASEDIR}/component/wifi/driver/phy_g6/bb/halbb_8720e
        ${BASEDIR}/component/wifi/driver/phy_g6/rf/halrf_8720e
        ${BASEDIR}/component/wifi/driver/rtl8720e
        ${BASEDIR}/component/wifi/driver/rtl8720e_rom
    )
endif()
