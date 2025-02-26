if (__INCLUDED_WIFI_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_WIFI_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_wifi_eap INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_wifi_realmesh INTERFACE)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_wifi_eap p_SCOPE interface
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/eap_common/eap_peap_common.c

    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/eap_peer/eap.c
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/eap_peer/eap_methods.c
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/eap_peer/eap_mschapv2.c
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/eap_peer/eap_peap.c
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/eap_peer/eap_tls.c
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/eap_peer/eap_tls_common.c
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/eap_peer/eap_ttls.c
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/eap_peer/mschapv2.c

    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/crypto/des-internal.c
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/crypto/md4-internal.c
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/crypto/ms_funcs.c
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/crypto/rc4.c
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/crypto/sha1-internal.c

    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/utils/common.c

    ${c_CMPT_WIFI_DIR}/wpa_supplicant/wpa_supplicant/eap_protocol_handler.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_wifi_realmesh p_SCOPE interface
    ${c_CMPT_WIFI_DIR}/realmesh/src/rmesh.c
    ${c_CMPT_WIFI_DIR}/realmesh/src/rmesh_frame.c
    ${c_CMPT_WIFI_DIR}/realmesh/src/rmesh_route.c
    ${c_CMPT_WIFI_DIR}/realmesh/src/rmesh_scan.c
    ${c_CMPT_WIFI_DIR}/realmesh/src/rmesh_connect.c
    ${c_CMPT_WIFI_DIR}/realmesh/src/rmesh_data_forward.c
    ${c_CMPT_WIFI_DIR}/realmesh/src/rmesh_event_cb.c
    ${c_CMPT_WIFI_DIR}/realmesh/src/rmesh_topology.c
    ${c_CMPT_WIFI_DIR}/realmesh/src/rmesh_keep_alive.c
    ${c_CMPT_WIFI_DIR}/realmesh/src/rmesh_light_application.c
    ${c_CMPT_WIFI_DIR}/realmesh/src/rmesh_root_socket.c
    ${c_CMPT_WIFI_DIR}/realmesh/test/rmesh_test.c
    ${c_CMPT_WIFI_DIR}/realmesh/src/rmesh_config.c
    ${c_CMPT_WIFI_DIR}/realmesh/rmesh_bt_config/rmesh_bt_config_app_main.c
    ${c_CMPT_WIFI_DIR}/realmesh/rmesh_bt_config/rmesh_bt_config_app_task.c
    ${c_CMPT_WIFI_DIR}/realmesh/rmesh_bt_config/rmesh_bt_config_peripheral_app.c
    ${c_CMPT_WIFI_DIR}/realmesh/rmesh_bt_config/rmesh_bt_config_service.c
    ${c_CMPT_WIFI_DIR}/realmesh/rmesh_bt_config/rmesh_bt_config_wifi.c
    ${c_CMPT_WIFI_DIR}/realmesh/rmesh_bt_config/rmesh_bt_config_wifi_lib.c
    ${c_CMPT_WIFI_DIR}/realmesh/src/rmesh_broadcast.c
)
#--------------------------#