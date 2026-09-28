##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

ameba_list_append(public_includes
	hw_alt
	thread_alt
	include
	tf-psa-crypto/include
	tf-psa-crypto/drivers/builtin/include
	tf-psa-crypto/dispatch
	../mbedtls_ram_map/rom
)

ameba_list_append(public_definitions
	MBEDTLS_CONFIG_FILE="mbedtls/config.h"
	TF_PSA_CRYPTO_CONFIG_FILE="psa/crypto_config_rtk.h"
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

# Upstream 4.1.1 tests several macros with #if without defining them first
# (MBEDTLS_AESNI_HAVE_CODE, LIBTESTDRIVER1_TF_PSA_CRYPTO_MARKER, ...).
# Relax -Wundef here instead of patching third-party sources.
ameba_list_append(private_compile_options -Wno-undef)

ameba_list_append(private_includes
	tf-psa-crypto/core
	tf-psa-crypto/drivers/builtin/src
	tf-psa-crypto/utilities
	tf-psa-crypto/extras
	tf-psa-crypto/platform
	library
)

# ========== TLS / X.509 layer (library/) ==========
ameba_list_append_if(CONFIG_WLAN private_sources library/net_sockets.c)

ameba_list_append(private_sources
	library/debug.c
	library/error.c
	library/mbedtls_config.c
	library/mps_reader.c
	library/mps_trace.c
	library/pkcs7.c
	library/ssl_cache.c
	library/ssl_ciphersuites.c
	library/ssl_client.c
	library/ssl_cookie.c
	library/ssl_debug_helpers_generated.c
	library/ssl_msg.c
	library/ssl_ticket.c
	library/ssl_tls.c
	library/ssl_tls12_client.c
	library/ssl_tls12_server.c
	library/ssl_tls13_client.c
	library/ssl_tls13_generic.c
	library/ssl_tls13_keys.c
	library/ssl_tls13_server.c
	library/version.c
	library/version_features.c
	library/x509.c
	library/x509_create.c
	library/x509_crl.c
	library/x509_crt.c
	library/x509_csr.c
	library/x509_oid.c
	library/x509write.c
	library/x509write_crt.c
	library/x509write_csr.c
)

# ========== PSA crypto core (tf-psa-crypto/core/) ==========
ameba_list_append(private_sources
	tf-psa-crypto/core/psa_crypto.c
	tf-psa-crypto/core/psa_crypto_client.c
	tf-psa-crypto/core/psa_crypto_driver_wrappers_no_static.c
	tf-psa-crypto/core/psa_crypto_random.c
	tf-psa-crypto/core/psa_crypto_slot_management.c
	tf-psa-crypto/core/psa_crypto_storage.c
	tf-psa-crypto/core/psa_its_file.c
	tf-psa-crypto/core/psa_util.c
	tf-psa-crypto/core/tf_psa_crypto_config.c
	tf-psa-crypto/core/tf_psa_crypto_version.c
)

# ========== Builtin crypto drivers (tf-psa-crypto/drivers/builtin/src/) ==========
# Only the algorithms required by RTK are listed. Unlisted sources stay on disk
# but are not compiled; keep this list in sync with PSA_WANT_* in crypto_config_rtk.h.
ameba_list_append(private_sources
	tf-psa-crypto/drivers/builtin/src/aes.c
	tf-psa-crypto/drivers/builtin/src/bignum.c
	tf-psa-crypto/drivers/builtin/src/bignum_core.c
	tf-psa-crypto/drivers/builtin/src/bignum_mod.c
	tf-psa-crypto/drivers/builtin/src/bignum_mod_raw.c
	tf-psa-crypto/drivers/builtin/src/block_cipher.c
	tf-psa-crypto/drivers/builtin/src/ccm.c
	tf-psa-crypto/drivers/builtin/src/cipher.c
	tf-psa-crypto/drivers/builtin/src/cipher_wrap.c
	tf-psa-crypto/drivers/builtin/src/cmac.c
	tf-psa-crypto/drivers/builtin/src/ctr_drbg.c
	tf-psa-crypto/drivers/builtin/src/ecdsa.c
	tf-psa-crypto/drivers/builtin/src/ecp.c
	tf-psa-crypto/drivers/builtin/src/ecp_curves.c
	tf-psa-crypto/drivers/builtin/src/entropy.c
	tf-psa-crypto/drivers/builtin/src/entropy_poll.c
	tf-psa-crypto/drivers/builtin/src/gcm.c
	tf-psa-crypto/drivers/builtin/src/hmac_drbg.c
	tf-psa-crypto/drivers/builtin/src/md5.c
	tf-psa-crypto/drivers/builtin/src/psa_crypto_aead.c
	tf-psa-crypto/drivers/builtin/src/psa_crypto_cipher.c
	tf-psa-crypto/drivers/builtin/src/psa_crypto_ecp.c
	tf-psa-crypto/drivers/builtin/src/psa_crypto_hash.c
	tf-psa-crypto/drivers/builtin/src/psa_crypto_mac.c
	tf-psa-crypto/drivers/builtin/src/psa_crypto_rsa.c
	tf-psa-crypto/drivers/builtin/src/psa_crypto_xof.c
	tf-psa-crypto/drivers/builtin/src/psa_util_internal.c
	tf-psa-crypto/drivers/builtin/src/rsa.c
	tf-psa-crypto/drivers/builtin/src/rsa_alt_helpers.c
	tf-psa-crypto/drivers/builtin/src/sha1.c
	tf-psa-crypto/drivers/builtin/src/sha256.c
	tf-psa-crypto/drivers/builtin/src/sha3.c
	tf-psa-crypto/drivers/builtin/src/sha512.c
)

# ========== PK / MD helpers (tf-psa-crypto/extras/) ==========
ameba_list_append(private_sources
	tf-psa-crypto/extras/md.c
	tf-psa-crypto/extras/nist_kw.c
	tf-psa-crypto/extras/pk.c
	tf-psa-crypto/extras/pk_ecc.c
	tf-psa-crypto/extras/pk_rsa.c
	tf-psa-crypto/extras/pk_wrap.c
	tf-psa-crypto/extras/pkparse.c
	tf-psa-crypto/extras/pkwrite.c
)

# ========== ASN.1 / encoding utilities (tf-psa-crypto/utilities/) ==========
ameba_list_append(private_sources
	tf-psa-crypto/utilities/asn1parse.c
	tf-psa-crypto/utilities/asn1write.c
	tf-psa-crypto/utilities/base64.c
	tf-psa-crypto/utilities/constant_time.c
	tf-psa-crypto/utilities/oid.c
	tf-psa-crypto/utilities/pem.c
	tf-psa-crypto/utilities/pkcs5.c
)

# ========== Platform layer (tf-psa-crypto/platform/) ==========
ameba_list_append(private_sources
	tf-psa-crypto/platform/memory_buffer_alloc.c
	tf-psa-crypto/platform/platform.c
	tf-psa-crypto/platform/platform_util.c
	tf-psa-crypto/platform/threading.c
)

# ========== RTK hardware acceleration (PSA transparent driver) ==========
# 4.x has no classic ALT macros: hardware acceleration is dispatched at runtime
# through the PSA driver interface, so hw_alt sources are always compiled here.
ameba_list_append(private_sources
	hw_alt/rtk_psa_entropy.c
	hw_alt/timing_alt.c
	hw_alt/ssl_rom_to_ram_map.c
	thread_alt/threading_alt.c
)

# SHA-2 engine. RTK_PSA_HASH_DRIVER is global: the driver context is embedded in
# psa_hash_operation_t, so every TU must agree on its size.
ameba_list_append(private_sources
	hw_alt/rtk_psa_hash.c
	hw_alt/rtk_psa_cipher.c
	hw_alt/rtk_psa_aead.c
	hw_alt/rtk_psa_ecc_common.c
	hw_alt/rtk_psa_ecdsa.c
	hw_alt/rtk_psa_ecdh.c
	hw_alt/rtk_psa_mac.c
	hw_alt/rtk_pke_bignum.c
)

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
