/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ECDSA_SIGN_SERVICE_H
#define ECDSA_SIGN_SERVICE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Uncompressed SECP256R1 public key: 0x04 || X(32) || Y(32). */
#define ECDSA_SIGN_SERVICE_PUBKEY_LEN   65

/** Signature buffer: PSA writes raw r||s (64 bytes), legacy API writes DER (<= 72 bytes). */
#define ECDSA_SIGN_SERVICE_SIG_MAX_LEN  72

/**
 * Structure for passing ECDSA sign request data between non-secure and secure worlds.
 * Used by the secure world ECDSA signing service.
 */
typedef struct {
	const unsigned char *message;      /* Input: Message to sign */
	size_t message_len;                /* Input: Length of the message */
	unsigned char *signature;          /* Output: Buffer for signature (ECDSA_SIGN_SERVICE_SIG_MAX_LEN bytes) */
	size_t *sig_len;                   /* Output: Actual signature length */
	unsigned char *public_key_raw;     /* Output: Buffer for raw public key (ECDSA_SIGN_SERVICE_PUBKEY_LEN bytes) */
	size_t *pub_key_len;               /* Output: Actual public key length */
} ecdsa_sign_req_t;

/**
 * NSC entry function for ECDSA signing service.
 * This function can be called from non-secure world to request signing.
 *
 * @param req Pointer to request structure containing message and output buffers
 * @return 0 on success, negative error code on failure
 */
NS_ENTRY int ecdsa_secure_sign(ecdsa_sign_req_t *req);

#ifdef __cplusplus
}
#endif

#endif /* ECDSA_SIGN_SERVICE_H */
