/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SECURE_ECDSA_CLIENT_SERVICE_H
#define SECURE_ECDSA_CLIENT_SERVICE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Maximum client certificate PEM size (including NUL terminator).
 * ECDSA P-256 certs are significantly smaller than RSA-2048.
 */
#define SEC_ECC_CERT_MAX_LEN    1024

/*
 * Maximum DER-encoded ECDSA signature length for P-256.
 * P-256 DER signature: 2 + 2 + 32 + 2 + 32 = 70 bytes nominal, up to 72.
 * MBEDTLS_ECDSA_MAX_LEN is 139 (for P-521 worst case); use 72 for P-256.
 */
#define SEC_ECC_SIG_MAX_LEN     72

/**
 * @brief Request passed to secure_ecc_client_init().
 *        Tells S-side where to write the certificate PEM.
 */
typedef struct {
	unsigned char *cert_pem;    /* NS-side buffer to receive client cert PEM */
	size_t         cert_max;    /* Size of that buffer (>= SEC_ECC_CERT_MAX_LEN) */
	size_t        *cert_len;    /* OUT: actual bytes written (incl. NUL) */
} secure_ecc_init_req_t;

/**
 * @brief Request passed to secure_ecc_client_sign().
 *        S-side signs the pre-computed TLS handshake hash with the ECDSA key.
 *
 *  The hash is already computed by the TLS layer (SHA-256, 32 bytes for P-256).
 *  S-side produces a DER-encoded ECDSA signature.
 */
typedef struct {
	unsigned int         md_alg;    /* mbedtls_md_type_t (e.g. MBEDTLS_MD_SHA256 = 9) */
	const unsigned char *hash;      /* Pre-computed transcript hash */
	unsigned int         hash_len;  /* Hash length (32 for SHA-256) */
	unsigned char       *sig;       /* OUT: DER signature buffer */
	size_t               sig_size;  /* IN:  size of sig buffer (must be >= SEC_ECC_SIG_MAX_LEN) */
	size_t              *sig_len;   /* OUT: actual DER bytes written */
} secure_ecc_sign_req_t;

/**
 * @brief NSC: Load ECDSA P-256 private key into S-heap and return client cert PEM.
 *
 *  Idempotent: subsequent calls just re-copy the cert PEM without re-importing the key.
 *  Must be called before secure_ecc_client_sign().
 *
 * @return 0 on success, negative mbedtls error code on failure.
 */
NS_ENTRY int secure_ecc_client_init(const secure_ecc_init_req_t *req);

/**
 * @brief NSC: Sign a pre-computed hash with the S-heap ECDSA P-256 private key.
 *
 *  Produces a DER-encoded (r, s) signature as required by TLS CertificateVerify.
 *
 * @return 0 on success, negative mbedtls error code on failure.
 */
NS_ENTRY int secure_ecc_client_sign(const secure_ecc_sign_req_t *req);

#ifdef __cplusplus
}
#endif

#endif /* SECURE_ECDSA_CLIENT_SERVICE_H */
