##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

#FIXME: miss role for public_libraries
ameba_list_append(public_includes
    include
    library
    ../mbedtls_ram_map/rom
)

ameba_list_append(public_definitions
    MBEDTLS_CONFIG_FILE="mbedtls/config.h"
)
# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

ameba_list_append(private_definitions CONFIG_BUILD_NONSECURE=1)

ameba_list_append_if(CONFIG_WLAN private_sources library/net_sockets.c)

ameba_list_append(private_sources
    library/aesce.c
    library/aesni.c
    library/aria.c
    library/base64.c
    library/bignum_mod.c
    library/bignum_mod_raw.c
    library/camellia.c
    library/ccm.c
    library/chacha20.c
    library/chachapoly.c
    library/cipher.c
    library/cipher_wrap.c
    library/cmac.c
    library/debug.c
    library/des.c
    library/dhm.c
    library/ecjpake.c
    library/entropy_poll.c
    library/error.c
    library/gcm.c
    library/hkdf.c
    library/lmots.c
    library/lms.c
    library/memory_buffer_alloc.c
    library/mps_reader.c
    library/mps_trace.c
    library/nist_kw.c
    library/oid.c
    library/padlock.c
    library/pem.c
    library/pk.c
    library/pkcs12.c
    library/pkcs5.c
    library/pkcs7.c
    library/pkparse.c
    library/pk_wrap.c
    library/pkwrite.c
    library/platform.c
    library/poly1305.c
    library/psa_crypto_aead.c
    library/psa_crypto.c
    library/psa_crypto_cipher.c
    library/psa_crypto_client.c

    library/psa_crypto_ecp.c
    library/psa_crypto_hash.c
    library/psa_crypto_mac.c
    library/psa_crypto_pake.c
    library/psa_crypto_rsa.c
    library/psa_crypto_se.c
    library/psa_crypto_slot_management.c
    library/psa_crypto_storage.c
    library/psa_its_file.c
    library/psa_util.c
    library/ripemd160.c
    library/rsa_alt_helpers.c

    library/ssl_cache.c
    library/ssl_ciphersuites.c
    library/ssl_client.c
    library/ssl_cookie.c
    library/ssl_debug_helpers_generated.c
    library/ssl_msg.c
    library/ssl_ticket.c
    library/ssl_tls12_client.c
    library/ssl_tls12_server.c
    library/ssl_tls13_client.c
    library/ssl_tls13_generic.c
    library/ssl_tls13_keys.c
    library/ssl_tls13_server.c
    library/ssl_tls.c
    library/threading.c
    library/timing.c
    library/version.c
    library/version_features.c
    library/x509.c
    library/x509_create.c
    library/x509_crl.c
    library/x509_crt.c
    library/x509_csr.c
    library/x509write_crt.c
    library/x509write_csr.c
    library/rsa.c
    hw_alt/entropy_hardware_alt.c


    library/asn1parse.c
    library/asn1write.c
    library/ecp.c
    library/bignum_core.c
    library/bignum.c
    library/sha1.c
    library/md.c
    library/md5.c
    library/ecdh.c
    library/ecdsa.c
    library/ecp_curves.c
    library/platform_util.c
    library/constant_time.c
    library/ctr_drbg.c
    library/hmac_drbg.c
    library/entropy.c
    library/pk_ecc.c
)

if(CONFIG_AMEBAGREEN2 OR CONFIG_AMEBAL2)

    ameba_list_append (private_sources
        hw_alt/rsa_alt.c
        hw_alt/aes_alt.c
        hw_alt/sha256_alt.c
        hw_alt/sha512_alt.c
        hw_alt/ecc_alt.c
        hw_alt/ssl_rom_to_ram_map.c
    )


else()

    ameba_list_append(private_sources
        library/aes.c
        library/sha256.c
        library/sha512.c
        library/psa_crypto_driver_wrappers_no_static.c
        library/psa_crypto_ffdh.c
    )

endif()
# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(mbedtls
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################