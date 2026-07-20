ameba_list_append(
    CSRC
    ../crypto/crypto_internal-modexp.c
    ../crypto/dh_group5.c
    ../crypto/dh_groups.c
    ../crypto/random.c

    ../eap_common/eap_common.c
    ../eap_common/eap_wsc_common.c
    ../eap_server/eap_server_wsc.c
    ../eap_peer/eap_wsc.c

    ../utils/os_freertos.c
    ../utils/wpabuf.c
    ../utils/wpa_debug.c
    ../utils/wpa_base64.c

    wps.c
    wps_attr_build.c
    wps_attr_parse.c
    wps_attr_process.c
    wps_common.c
    wps_dev_attr.c
    wps_enrollee.c
    wps_validate.c
    wps_registrar.c

    ../common/ieee802_11_common.c

    ../../wpa_supplicant/wps_protocol_handler.c

    ../crypto/rom/rom_wps_aes-cbc.c
    ../crypto/rom/rom_wps_aes-internal.c
    ../crypto/rom/rom_wps_aes-internal-dec.c
    ../crypto/rom/rom_wps_aes-internal-enc.c
    ../crypto/rom/rom_wps_sha_256.c
    ../tls/rom/rom_wps_big_num.c
    ../utils/rom/rom_wps_os_freertos.c
)
