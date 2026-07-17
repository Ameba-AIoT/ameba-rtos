ameba_list_append(
    CSRC
    ../eap_common/eap_peap_common.c
    ../eap_common/eap_fast_common.c

    eap.c
    eap_methods.c
    eap_mschapv2.c
    eap_peap.c
    eap_tls.c
    eap_tls_common.c
    eap_ttls.c
    mschapv2.c
    eap_fast.c
    eap_fast_pac.c

    ../crypto/des-internal.c
    ../crypto/md4-internal.c
    ../crypto/ms_funcs.c
    ../crypto/rc4.c
    ../crypto/sha1-internal.c
    ../crypto/sha1-tprf.c
    ../utils/common.c

    ../../wpa_supplicant/eap_protocol_handler.c

)
