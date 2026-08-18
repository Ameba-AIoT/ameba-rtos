/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SECURE_CLIENT_SERVICE_RSA_H
#define SECURE_CLIENT_SERVICE_RSA_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum client certificate PEM size (including NUL terminator) */
#define SEC_CLI_CERT_MAX_LEN   2048
/* RSA-2048 signature output length in bytes */
#define SEC_CLI_SIG_LEN        256

/**
 * @brief Request for secure_client_init.
 */
typedef struct {
	unsigned char *cert_pem;
	size_t         cert_max;
	size_t        *cert_len;
} secure_client_init_req_t;

/**
 * @brief Request for secure_client_sign.
 */
typedef struct {
	unsigned int         md_alg;    /* mbedtls_md_type_t */
	unsigned int         hash_len;  /* Hash length (32 for SHA-256) */
	const unsigned char *hash;      /* Pre-computed transcript hash */
	unsigned char       *sig;       /* OUT: signature buffer */
	size_t               sig_size;  /* IN:  size of sig buffer (must be >= SEC_CLI_SIG_LEN) */
	size_t              *sig_len;   /* OUT: actual bytes written (always SEC_CLI_SIG_LEN = 256) */
} secure_client_sign_req_t;

/**
 * @brief NSC: Parse client private key and certificate into S-heap.
 */
NS_ENTRY int secure_client_init(const secure_client_init_req_t *req);

/**
 * @brief NSC: Sign a TLS handshake hash with the S-heap RSA private key.
 */
NS_ENTRY int secure_client_sign(const secure_client_sign_req_t *req);

#ifdef __cplusplus
}
#endif

#endif /* SECURE_CLIENT_SERVICE_RSA_H */
