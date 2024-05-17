/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 * Copyright (c) 2021 STMicroelectronics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef MBEDTLS_ACCELERATOR_CONF_H
#define MBEDTLS_ACCELERATOR_CONF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* RNG Config */
#undef MBEDTLS_ENTROPY_NV_SEED
#undef MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_ENTROPY_HARDWARE_ALT

#undef MBEDTLS_AES_SETKEY_DEC_ALT
#undef MBEDTLS_AES_DECRYPT_ALT

/* specific Define for platform hardware accelerator */
#define GENERATOR_HW_PKA_EXTENDED_API
#define GENERATOR_HW_CRYPTO_DPA_SUPPORTED
#define HW_CRYPTO_DPA_AES
#define HW_CRYPTO_DPA_GCM

/****************************************************************/
/* Require built-in implementations based on PSA requirements */
/****************************************************************/
#if defined(MBEDTLS_PSA_CRYPTO_CONFIG)
#ifdef PSA_USE_SE_ST
/* secure element define */
#define PSA_WANT_KEY_TYPE_AES                   1
#ifdef MBEDTLS_PSA_CRYPTO_C
#define MBEDTLS_PSA_CRYPTO_SE_C
#define MBEDTLS_CMAC_C
#define MBEDTLS_CIPHER_MODE_CBC
#endif
#ifdef PSA_WANT_ALG_SHA_1
#define MBEDTLS_SHA1_ALT
#endif /* PSA_WANT_ALG_SHA_1 */

#ifdef PSA_WANT_ALG_SHA_256
#define MBEDTLS_SHA256_ALT
#endif /* PSA_WANT_ALG_SHA_256 */

#if defined(PSA_WANT_ALG_RSA_OAEP)           ||     \
    defined(PSA_WANT_ALG_RSA_PKCS1V15_CRYPT) ||     \
    defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN)  ||     \
    defined(PSA_WANT_ALG_RSA_PSS)            ||     \
    defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR)  ||     \
    defined(PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY)
#define MBEDTLS_RSA_ALT
#endif

#if defined(PSA_WANT_ALG_ECDH)              ||  \
    defined(PSA_WANT_ALG_ECDSA)             ||  \
    defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR) ||  \
    defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY)
#define MBEDTLS_ECP_ALT
#undef MBEDTLS_ECP_NIST_OPTIM
#endif

#ifdef PSA_WANT_ALG_CCM
#define MBEDTLS_CCM_ALT
#endif /* PSA_WANT_ALG_CCM */

#ifdef PSA_WANT_KEY_TYPE_AES
#define MBEDTLS_AES_ALT
#endif /* PSA_WANT_KEY_TYPE_AES */

#ifdef PSA_WANT_ALG_GCM
#define MBEDTLS_GCM_ALT
#endif /* PSA_WANT_ALG_GCM */

#if defined(PSA_WANT_ALG_ECDSA)  ||  \
    defined(PSA_WANT_ALG_DETERMINISTIC_ECDSA)
#define MBEDTLS_ECDSA_VERIFY_ALT
#define MBEDTLS_ECDSA_SIGN_ALT
#endif

#endif
#else /* MBEDTLS_PSA_CRYPTO_CONFIG */
/****************************************************************/
/* Infer PSA requirements from Mbed TLS capabilities */
/****************************************************************/

#ifdef MBEDTLS_SHA1_C
#define MBEDTLS_SHA1_ALT
#endif /* MBEDTLS_SHA1_C */

#ifdef MBEDTLS_SHA256_C
#define MBEDTLS_SHA256_ALT
#endif /* MBEDTLS_SHA256_C */

#ifdef MBEDTLS_RSA_C
#define MBEDTLS_RSA_ALT
#endif /* MBEDTLS_RSA_C */

#if defined(MBEDTLS_ECP_C)
#define MBEDTLS_ECP_ALT
#undef MBEDTLS_ECP_NIST_OPTIM
/*#define MBEDTLS_MD5_ALT*/
#endif /* MBEDTLS_ECP_C && MBEDTLS_MD_C */

#ifdef MBEDTLS_CCM_C
#define MBEDTLS_CCM_ALT
#endif /* MBEDTLS_CCM_C */

#ifdef MBEDTLS_AES_C
#define MBEDTLS_AES_ALT
#endif /* MBEDTLS_AES_C */

#ifdef MBEDTLS_GCM_C
#define MBEDTLS_GCM_ALT
#endif /* MBEDTLS_GCM_C */

#ifdef MBEDTLS_ECDSA_C
#define MBEDTLS_ECDSA_VERIFY_ALT
#define MBEDTLS_ECDSA_SIGN_ALT
#endif /* MBEDTLS_ECDSA_C */

/* secure element define */
#ifdef MBEDTLS_PSA_CRYPTO_C
#ifdef PSA_USE_SE_ST
#define MBEDTLS_PSA_CRYPTO_SE_C
#define MBEDTLS_CMAC_C
#define MBEDTLS_CIPHER_MODE_CBC
#endif
#endif
#endif /* MBEDTLS_PSA_CRYPTO_CONFIG */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MBEDTLS_ACCELERATOR_CONF_H */
