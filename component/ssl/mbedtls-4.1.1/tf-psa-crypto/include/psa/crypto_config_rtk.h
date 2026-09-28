/**
 * *****************************************************************************
 * @file    crypto_config_rtk.h
 * @brief   RTK crypto layer configuration for TF-PSA-Crypto 1.x.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef PSA_CRYPTO_CONFIG_RTK_H
#define PSA_CRYPTO_CONFIG_RTK_H

#include "platform_autoconf.h"
#include "ssl_rom_to_ram_map.h"

#define TF_PSA_CRYPTO_CONFIG_VERSION 0x01000000

/*
 * Cryptographic mechanisms. 4.x selects the crypto layer purely through
 * PSA_WANT_*: the legacy MBEDTLS_xxx_C / MBEDTLS_ECP_DP_xxx switches are
 * rejected by tf_psa_crypto_config_check_user.h. Keep this list in sync with
 * the builtin source list in ../../../CMakeLists.txt.
 */
#define PSA_WANT_ALG_CBC_NO_PADDING             1
#define PSA_WANT_ALG_CBC_PKCS7                  1
#define PSA_WANT_ALG_CCM                        1
#define PSA_WANT_ALG_CCM_STAR_NO_TAG            1
#define PSA_WANT_ALG_CFB                        1
#define PSA_WANT_ALG_CMAC                       1
#define PSA_WANT_ALG_CTR                        1
#define PSA_WANT_ALG_ECB_NO_PADDING             1
#define PSA_WANT_ALG_GCM                        1
#define PSA_WANT_ALG_OFB                        1

#define PSA_WANT_ALG_ECDH                       1
#define PSA_WANT_ALG_ECDSA                      1
#define PSA_WANT_ALG_DETERMINISTIC_ECDSA        1

#define PSA_WANT_ALG_RSA_OAEP                   1
#define PSA_WANT_ALG_RSA_PKCS1V15_CRYPT         1
#define PSA_WANT_ALG_RSA_PKCS1V15_SIGN          1
#define PSA_WANT_ALG_RSA_PSS                    1

#define PSA_WANT_ALG_HKDF                       1
#define PSA_WANT_ALG_HKDF_EXTRACT               1
#define PSA_WANT_ALG_HKDF_EXPAND                1
#define PSA_WANT_ALG_HMAC                       1
#define PSA_WANT_ALG_PBKDF2_HMAC                1
#define PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128    1
#define PSA_WANT_ALG_TLS12_PRF                  1
#define PSA_WANT_ALG_TLS12_PSK_TO_MS            1

#define PSA_WANT_ALG_MD5                        1
#define PSA_WANT_ALG_SHA_1                      1
#define PSA_WANT_ALG_SHA_224                    1
#define PSA_WANT_ALG_SHA_256                    1
#define PSA_WANT_ALG_SHA_384                    1
#define PSA_WANT_ALG_SHA_512                    1
#define PSA_WANT_ALG_SHA3_224                   1
#define PSA_WANT_ALG_SHA3_256                   1
#define PSA_WANT_ALG_SHA3_384                   1
#define PSA_WANT_ALG_SHA3_512                   1
#define PSA_WANT_ALG_SHAKE128                   1
#define PSA_WANT_ALG_SHAKE256                   1

/* Curves. 4.x dropped secp192k1/secp192r1/secp224k1/secp224r1, which the
 * 3.6.5 config enabled. */
#define PSA_WANT_ECC_BRAINPOOL_P_R1_256         1
#define PSA_WANT_ECC_BRAINPOOL_P_R1_384         1
#define PSA_WANT_ECC_BRAINPOOL_P_R1_512         1
#define PSA_WANT_ECC_MONTGOMERY_255             1
#define PSA_WANT_ECC_MONTGOMERY_448             1
#define PSA_WANT_ECC_SECP_K1_256                1
#define PSA_WANT_ECC_SECP_R1_256                1
#define PSA_WANT_ECC_SECP_R1_384                1

#define PSA_WANT_KEY_TYPE_AES                   1
#define PSA_WANT_KEY_TYPE_DERIVE                1
#define PSA_WANT_KEY_TYPE_HMAC                  1
#define PSA_WANT_KEY_TYPE_PASSWORD              1
#define PSA_WANT_KEY_TYPE_PASSWORD_HASH         1
#define PSA_WANT_KEY_TYPE_RAW_DATA              1

#define PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY        1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC    1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT   1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT   1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE   1

#define PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY        1
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC    1
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT   1
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT   1
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE 1

/*
 * Not enabled (upstream psa/crypto_config.h defaults them on): JPAKE,
 * FFDH/DH_RFC7919, CHACHA20_POLY1305, ARIA, CAMELLIA, RIPEMD160,
 * TLS12_ECJPAKE_TO_PMS, STREAM_CIPHER. The matching sources are on disk but
 * not listed in CMakeLists.txt.
 */

/* PSA core. */
#define MBEDTLS_PSA_CRYPTO_C
#define MBEDTLS_PSA_KEY_STORE_DYNAMIC
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_HMAC_DRBG_C

/* No /dev/urandom nor time() on the target: the TRNG is exposed as a PSA
 * entropy driver by hw_alt/rtk_psa_entropy.c. */
#define MBEDTLS_PSA_DRIVER_GET_ENTROPY

/* PK / MD / encoding helpers used by the TLS and X.509 layers. */
#define MBEDTLS_MD_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PK_WRITE_C
#define MBEDTLS_PK_PARSE_EC_EXTENDED
#define MBEDTLS_PK_PARSE_EC_COMPRESSED
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_PEM_WRITE_C
#define MBEDTLS_PKCS5_C
#define MBEDTLS_NIST_KW_C

/* RTK hardware acceleration driver flags — SoC-specific.
 * Separated into a dedicated file so that per-SoC guards can be applied
 * without touching the application-requirement macros (PSA_WANT_*) above.
 * See crypto_driver_config_rtk.h for the full rationale and per-SoC guards.
 * PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT is also set there. */
#include "psa/crypto_driver_config_rtk.h"

/* Build-time tuning. */
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_NO_UDBL_DIVISION
#define MBEDTLS_ECP_NIST_OPTIM
#define MBEDTLS_ECP_FIXED_POINT_OPTIM 0

/* Platform abstraction: no filesystem, no time, no libc defaults. */
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
#define MBEDTLS_PLATFORM_ZEROIZE_ALT
#define MBEDTLS_PLATFORM_CALLOC_MACRO   ssl_function_map.ssl_calloc
#define MBEDTLS_PLATFORM_FREE_MACRO     ssl_function_map.ssl_free
#define MBEDTLS_PLATFORM_PRINTF_MACRO   ssl_function_map.ssl_printf
#define MBEDTLS_PLATFORM_SNPRINTF_MACRO ssl_function_map.ssl_snprintf

#if defined(CONFIG_MBEDTLS_THREADING)
#define MBEDTLS_THREADING_C
#define MBEDTLS_THREADING_ALT
#endif

#endif /* PSA_CRYPTO_CONFIG_RTK_H */
