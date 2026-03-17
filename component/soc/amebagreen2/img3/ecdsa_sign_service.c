/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ecdsa_sign_service.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/sha256.h"
#include <string.h>

static const char *const TAG = "SECURE";

/* Persistent key pair for signing */
static mbedtls_ecdsa_context g_ctx_sign;
static mbedtls_entropy_context g_entropy;
static mbedtls_ctr_drbg_context g_ctr_drbg;
static int g_initialized = 0;

/* Initialize the ECDSA context with a key pair */
static int ecdsa_secure_init(void)
{
	int ret = 0;
	const char *pers = "ecdsa_secure_service";

	if (g_initialized) {
		return 0;
	}

	/* Initialize contexts */
	mbedtls_ecdsa_init(&g_ctx_sign);
	mbedtls_ctr_drbg_init(&g_ctr_drbg);
	mbedtls_entropy_init(&g_entropy);

	/* Seed the random number generator */
	ret = mbedtls_ctr_drbg_seed(&g_ctr_drbg, mbedtls_entropy_func, &g_entropy,
								(const unsigned char *)pers, strlen(pers));
	if (ret != 0) {
		RTK_LOGE(TAG, "CTR_DRBG seed failed: -0x%04X\n", -ret);
		return ret;
	}

	/* Generate a key pair for signing */
	ret = mbedtls_ecdsa_genkey(&g_ctx_sign, MBEDTLS_ECP_DP_SECP256R1,
							   mbedtls_ctr_drbg_random, &g_ctr_drbg);
	if (ret != 0) {
		RTK_LOGE(TAG, "ECDSA key generation failed: -0x%04X\n", -ret);
		return ret;
	}

	RTK_LOGI(TAG, "ECDSA key pair generated successfully\n");
	g_initialized = 1;

	return 0;
}

/*
 * Internal implementation of ECDSA signing.
 * This function performs the actual signing operation.
 */
static int ecdsa_do_sign(const unsigned char *message, size_t message_len,
						 unsigned char *signature, size_t *sig_len,
						 unsigned char *public_key_raw, size_t *pub_key_len)
{
	int ret = 0;
	unsigned char message_hash[32];

	RTK_LOGI(TAG, "=== Secure ECDSA Sign Service ===\n");

	/* Initialize if not already done */
	ret = ecdsa_secure_init();
	if (ret != 0) {
		RTK_LOGE(TAG, "ECDSA init failed: -0x%04X\n", -ret);
		return ret;
	}

	RTK_LOGI(TAG, "Message to sign: %.*s\n", (int)message_len, message);

	/* Compute hash of the message */
	mbedtls_sha256(message, message_len, message_hash, 0);
	RTK_LOGI(TAG, "SHA256 hash computed\n");

	/* Sign the hash */
	ret = mbedtls_ecdsa_write_signature(&g_ctx_sign, MBEDTLS_MD_SHA256,
										message_hash, 32,
										signature, MBEDTLS_ECDSA_MAX_LEN, sig_len,
										mbedtls_ctr_drbg_random, &g_ctr_drbg);
	if (ret != 0) {
		RTK_LOGE(TAG, "ECDSA signature failed: -0x%04X\n", -ret);
		return ret;
	}

	RTK_LOGI(TAG, "Signature created (len=%d)\n", (int)*sig_len);

	/* Export the public key in raw uncompressed format (0x04 || X || Y) */
	ret = mbedtls_ecp_point_write_binary(&g_ctx_sign.grp, &g_ctx_sign.Q,
										 MBEDTLS_ECP_PF_UNCOMPRESSED, pub_key_len,
										 public_key_raw, 65);
	if (ret != 0) {
		RTK_LOGE(TAG, "Failed to export public key: -0x%04X\n", -ret);
		return ret;
	}

	RTK_LOGI(TAG, "Public key exported (len=%d)\n", (int)*pub_key_len);
	RTK_LOGI(TAG, "=== Secure Sign Complete ===\n");

	return 0;
}

/*
 * NSC entry function - wrapper for ECDSA signing service.
 * This function can be called from non-secure world.
 *
 * Parameters:
 *   req - Pointer to request structure containing:
 *         - message: Input message to sign
 *         - message_len: Length of the message
 *         - signature: Output buffer for the signature
 *         - sig_len: Output: actual signature length
 *         - public_key_raw: Output buffer for the raw public key (65 bytes)
 *         - pub_key_len: Output: actual public key length
 *
 * Returns: 0 on success, negative error code on failure
 */
IMAGE3_ENTRY_SECTION
NS_ENTRY int ecdsa_secure_sign(ecdsa_sign_req_t *req)
{
	/* Validate input parameters */
	if (req == NULL || req->message == NULL || req->signature == NULL ||
		req->sig_len == NULL || req->public_key_raw == NULL || req->pub_key_len == NULL) {
		RTK_LOGE(TAG, "Invalid request parameters\n");
		return -1;
	}

	/* Delegate to internal implementation */
	return ecdsa_do_sign(req->message, req->message_len,
						 req->signature, req->sig_len,
						 req->public_key_raw, req->pub_key_len);
}
