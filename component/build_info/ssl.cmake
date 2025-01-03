if (__INCLUDED_SSL_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_SSL_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_ssl_mbedtls INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_ssl_mbedtls_secure INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_ssl_mbedtls_secure_s INTERFACE)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ssl_mbedtls p_SCOPE interface
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/aes.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/aesce.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/aesni.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/aria.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/base64.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/bignum_mod.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/bignum_mod_raw.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/camellia.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ccm.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/chacha20.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/chachapoly.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/cipher.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/cipher_wrap.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/cmac.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/debug.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/des.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/dhm.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ecjpake.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/entropy_poll.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/error.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/gcm.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/hkdf.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/lmots.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/lms.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/md.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/md5.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/memory_buffer_alloc.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/mps_reader.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/mps_trace.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/net_sockets.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/nist_kw.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/oid.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/padlock.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/pem.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/pk.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/pkcs12.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/pkcs5.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/pkcs7.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/pk_ecc.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/pkparse.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/pk_wrap.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/pkwrite.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/platform.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/poly1305.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto_aead.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto_cipher.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto_client.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto_driver_wrappers_no_static.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto_ffdh.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto_ecp.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto_hash.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto_mac.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto_pake.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto_rsa.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto_se.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto_slot_management.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_crypto_storage.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_its_file.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/psa_util.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ripemd160.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/rsa_alt_helpers.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/rsa.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/sha1.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/sha256.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/sha512.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_cache.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_ciphersuites.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_client.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_cookie.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_debug_helpers_generated.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_msg.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_ticket.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_tls12_client.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_tls12_server.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_tls13_client.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_tls13_generic.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_tls13_keys.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_tls13_server.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_tls.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/threading.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/timing.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/version.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/version_features.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/x509.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/x509_create.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/x509_crl.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/x509_crt.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/x509_csr.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/x509write_crt.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/x509write_csr.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/hw_alt/entropy_hardware_alt.c
)

if(NOT CONFIG_MP_INCLUDED AND CONFIG_WLAN)
    if(CONFIG_AS_INIC_AP OR CONFIG_SINGLE_CORE_WIFI)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ssl_mbedtls p_SCOPE interface
            ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/net_sockets.c
        )
    endif()
endif()
if(${d_PLATFORM_TYPE} STREQUAL "amebasmart")
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ssl_mbedtls p_SCOPE interface
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/asn1parse.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/asn1write.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ecp.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/bignum_core.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/bignum.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/hmac_drbg.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ecp_curves.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ecdh.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ecdsa.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/platform_util.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/constant_time.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ctr_drbg.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/entropy.c
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_sources_if(CONFIG_RLE1080_A_CUT src_${d_MCU_PROJECT_NAME}_ssl_mbedtls p_SCOPE interface
            ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/platform_util.c
        )
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ssl_mbedtls p_SCOPE interface
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/asn1parse.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/asn1write.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ecp.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/bignum_core.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/bignum.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/hmac_drbg.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ecp_curves.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ecdh.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ecdsa.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/platform_util.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/constant_time.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ctr_drbg.c
             ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/entropy.c
        )
    endif()
endif()


ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_ssl_mbedtls p_SCOPE interface CONFIG_BUILD_NONSECURE=1)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ssl_mbedtls_secure p_SCOPE interface
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/aes.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/base64.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/oid.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/pem.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/pk.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/pk_wrap.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/platform.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/rsa.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/rsa_internal.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/ssl_tls.c
    ${c_COMPONENT_DIR}/ssl/mbedtls_ram_map/rom/rom_ssl_ram_map.c
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/pkparse.c
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_ssl_mbedtls_secure p_SCOPE interface
    CONFIG_BUILD_SECURE=1
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ssl_mbedtls_secure_s p_SCOPE interface
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library/mbedtls_nsc.c
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_ssl_mbedtls_secure_s p_SCOPE interface
    CONFIG_BUILD_SECURE=1
)
#--------------------------#