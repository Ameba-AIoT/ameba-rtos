set(DIR ${BASEDIR}/component/wifi/wpa_supplicant)
set(EAPDIR ${DIR}/src)
set(MODULE_C_OPTIONS  -Wno-implicit-fallthrough)

set(
    CSRC
    ${EAPDIR}/eap_common/eap_peap_common.c
	${EAPDIR}/eap_peer/eap.c
	${EAPDIR}/eap_peer/eap_methods.c
	${EAPDIR}/eap_peer/eap_mschapv2.c
	${EAPDIR}/eap_peer/eap_peap.c
	${EAPDIR}/eap_peer/eap_tls.c
	${EAPDIR}/eap_peer/eap_tls_common.c
	${EAPDIR}/eap_peer/eap_ttls.c
	${EAPDIR}/eap_peer/mschapv2.c
	${EAPDIR}/crypto/des-internal.c
	${EAPDIR}/crypto/md4-internal.c
	${EAPDIR}/crypto/ms_funcs.c
	${EAPDIR}/crypto/rc4.c
	${EAPDIR}/crypto/sha1-internal.c
	${EAPDIR}/utils/common.c
	${DIR}/wpa_supplicant/eap_protocol_handler.c
)


ameba_app_library(eap)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
