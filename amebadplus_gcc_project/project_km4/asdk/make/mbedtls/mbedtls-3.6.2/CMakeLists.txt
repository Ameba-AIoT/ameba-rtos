set(DIR ${BASEDIR}/component/ssl/mbedtls-${MBEDTLS_VER})
set(MODULE_C_DEFINES CONFIG_BUILD_NONSECURE=1)

set(CSRC)

if(CONFIG_WLAN)
    if(CONFIG_AS_INIC_AP)
        list(
            APPEND CSRC
            ${DIR}/library/net_sockets.c
        )
    endif()
endif()

list(
    APPEND CSRC
    ${DIR}/library/aes.c
    ${DIR}/library/aesce.c
    ${DIR}/library/aesni.c
    ${DIR}/library/aria.c
    ${DIR}/library/base64.c
    ${DIR}/library/bignum_mod.c
    ${DIR}/library/bignum_mod_raw.c
    ${DIR}/library/camellia.c
    ${DIR}/library/ccm.c
    ${DIR}/library/chacha20.c
    ${DIR}/library/chachapoly.c
    ${DIR}/library/cipher.c
    ${DIR}/library/cipher_wrap.c
    ${DIR}/library/cmac.c
    ${DIR}/library/debug.c
    ${DIR}/library/des.c
    ${DIR}/library/dhm.c
    ${DIR}/library/ecjpake.c
    ${DIR}/library/entropy_poll.c
    ${DIR}/library/error.c
    ${DIR}/library/gcm.c
    ${DIR}/library/hkdf.c
    ${DIR}/library/lmots.c
    ${DIR}/library/lms.c
    ${DIR}/library/md.c
    ${DIR}/library/md5.c
    ${DIR}/library/memory_buffer_alloc.c
    ${DIR}/library/mps_reader.c
    ${DIR}/library/mps_trace.c
    ${DIR}/library/nist_kw.c
    ${DIR}/library/oid.c
    ${DIR}/library/padlock.c
    ${DIR}/library/pem.c
    ${DIR}/library/pk.c
    ${DIR}/library/pkcs12.c
    ${DIR}/library/pkcs5.c
    ${DIR}/library/pkcs7.c
    ${DIR}/library/pk_ecc.c
    ${DIR}/library/pkparse.c
    ${DIR}/library/pk_wrap.c
    ${DIR}/library/pkwrite.c
    ${DIR}/library/platform.c
    ${DIR}/library/poly1305.c
    ${DIR}/library/psa_crypto_aead.c
    ${DIR}/library/psa_crypto.c
    ${DIR}/library/psa_crypto_cipher.c
    ${DIR}/library/psa_crypto_client.c
    ${DIR}/library/psa_crypto_driver_wrappers_no_static.c
    ${DIR}/library/psa_crypto_ffdh.c
    ${DIR}/library/psa_crypto_ecp.c
    ${DIR}/library/psa_crypto_hash.c
    ${DIR}/library/psa_crypto_mac.c
    ${DIR}/library/psa_crypto_pake.c
    ${DIR}/library/psa_crypto_rsa.c
    ${DIR}/library/psa_crypto_se.c
    ${DIR}/library/psa_crypto_slot_management.c
    ${DIR}/library/psa_crypto_storage.c
    ${DIR}/library/psa_its_file.c
    ${DIR}/library/psa_util.c
    ${DIR}/library/ripemd160.c
    ${DIR}/library/rsa_alt_helpers.c
    ${DIR}/library/rsa.c
    ${DIR}/library/sha1.c
    ${DIR}/library/sha256.c
    ${DIR}/library/sha512.c
    ${DIR}/library/ssl_cache.c
    ${DIR}/library/ssl_ciphersuites.c
    ${DIR}/library/ssl_client.c
    ${DIR}/library/ssl_cookie.c
    ${DIR}/library/ssl_debug_helpers_generated.c
    ${DIR}/library/ssl_msg.c
    ${DIR}/library/ssl_ticket.c
    ${DIR}/library/ssl_tls12_client.c
    ${DIR}/library/ssl_tls12_server.c
    ${DIR}/library/ssl_tls13_client.c
    ${DIR}/library/ssl_tls13_generic.c
    ${DIR}/library/ssl_tls13_keys.c
    ${DIR}/library/ssl_tls13_server.c
    ${DIR}/library/ssl_tls.c
    ${DIR}/library/threading.c
    ${DIR}/library/timing.c
    ${DIR}/library/version.c
    ${DIR}/library/version_features.c
    ${DIR}/library/x509.c
    ${DIR}/library/x509_create.c
    ${DIR}/library/x509_crl.c
    ${DIR}/library/x509_crt.c
    ${DIR}/library/x509_csr.c
    ${DIR}/library/x509write_crt.c
    ${DIR}/library/x509write_csr.c
    ${DIR}/hw_alt/entropy_hardware_alt.c
)

if(CONFIG_RLE1080_A_CUT)
    list(APPEND CSRC ${DIR}/library/platform_util.c)
endif()

list(
    APPEND CSRC
    ${DIR}/library/asn1parse.c
    ${DIR}/library/asn1write.c
    ${DIR}/library/ecp.c
    ${DIR}/library/bignum_core.c
    ${DIR}/library/bignum.c
    ${DIR}/library/hmac_drbg.c
    ${DIR}/library/ecp_curves.c
    ${DIR}/library/ecdh.c
    ${DIR}/library/ecdsa.c
    ${DIR}/library/platform_util.c
    ${DIR}/library/constant_time.c
    ${DIR}/library/ctr_drbg.c
    ${DIR}/library/entropy.c
)


ameba_internal_library(mbedtls)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
