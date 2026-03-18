/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>
#include <string.h>
#include "mbedtls/ecdsa.h"
#include "mbedtls/ecp.h"
#include "mbedtls/sha256.h"
#include "ecdsa_sign_service.h"

static const char *const TAG = "ECDSA_TZ";

/* ECDSA verification in non-secure world */
static int ecdsa_ns_verify(const unsigned char *message, size_t message_len,
						   const unsigned char *signature, size_t sig_len,
						   const unsigned char *public_key_raw, size_t pub_key_len)
{
	int ret = 0;
	mbedtls_ecdsa_context ctx_verify;
	mbedtls_ecp_group grp;
	mbedtls_ecp_point Q;
	unsigned char message_hash[32];

	mbedtls_ecdsa_init(&ctx_verify);
	mbedtls_ecp_group_init(&grp);
	mbedtls_ecp_point_init(&Q);

	/* Set up the group (SECP256R1) */
	ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
	if (ret != 0) {
		RTK_LOGE(TAG, "Failed to load group: -0x%04X\n", -ret);
		goto cleanup;
	}

	/* Parse raw public key (65 bytes: 0x04 || X (32) || Y (32)) */
	ret = mbedtls_ecp_point_read_binary(&grp, &Q, public_key_raw, pub_key_len);
	if (ret != 0) {
		RTK_LOGE(TAG, "Failed to read public key: -0x%04X\n", -ret);
		goto cleanup;
	}

	/* Copy group and public key to verification context */
	ret = mbedtls_ecp_group_copy(&ctx_verify.grp, &grp);
	if (ret != 0) {
		RTK_LOGE(TAG, "Failed to copy group: -0x%04X\n", -ret);
		goto cleanup;
	}

	ret = mbedtls_ecp_copy(&ctx_verify.Q, &Q);
	if (ret != 0) {
		RTK_LOGE(TAG, "Failed to copy public key: -0x%04X\n", -ret);
		goto cleanup;
	}

	/* Compute hash of the message */
	mbedtls_sha256(message, message_len, message_hash, 0);

	/* Verify the signature */
	ret = mbedtls_ecdsa_read_signature(&ctx_verify, message_hash, 32, signature, sig_len);
	if (ret != 0) {
		RTK_LOGE(TAG, "ECDSA verification failed: -0x%04X\n", -ret);
		goto cleanup;
	}

	RTK_LOGI(TAG, "Non-secure verification succeeded!\n");

cleanup:
	mbedtls_ecdsa_free(&ctx_verify);
	mbedtls_ecp_group_free(&grp);
	mbedtls_ecp_point_free(&Q);

	return ret;
}

void ecdsa_tz_demo(void)
{
	/* Wait for other CPU boot done */
	rtos_time_delay_ms(1000);

	RTK_LOGI(TAG, "=== ECDSA TrustZone Demo Start ===\n");

#ifdef CONFIG_TRUSTZONE_EN
	int ret;
	const char *message = "Hello from Non-Secure World!";
	unsigned char signature[MBEDTLS_ECDSA_MAX_LEN];
	size_t sig_len = 0;
	unsigned char public_key_raw[65];  /* Uncompressed point: 0x04 || X (32) || Y (32) */
	size_t pub_key_len = 0;
	ecdsa_sign_req_t req;

	/* Allocate secure context for this task before calling secure functions */
	RTK_LOGI(TAG, "Creating secure context for task...\n");
	rtos_create_secure_context(2048);

	RTK_LOGI(TAG, "Message to sign: %s\n", message);
	RTK_LOGI(TAG, "Calling secure world for signing...\n");

	/* Prepare the request structure */
	req.message = (const unsigned char *)message;
	req.message_len = strlen(message);
	req.signature = signature;
	req.sig_len = &sig_len;
	req.public_key_raw = public_key_raw;
	req.pub_key_len = &pub_key_len;

	/* Call secure world to sign the message and get the public key */
	ret = ecdsa_secure_sign(&req);

	if (ret != 0) {
		RTK_LOGE(TAG, "Secure signing failed: -0x%04X\n", -ret);
		goto end;
	}

	RTK_LOGI(TAG, "Signature received from secure world (len=%d)\n", sig_len);
	RTK_LOGI(TAG, "Public key length: %d\n", pub_key_len);

	/* Verify the signature in non-secure world */
	RTK_LOGI(TAG, "Verifying signature in non-secure world...\n");
	ret = ecdsa_ns_verify((const unsigned char *)message, strlen(message),
						  signature, sig_len, public_key_raw, pub_key_len);

	if (ret != 0) {
		RTK_LOGE(TAG, "Non-secure verification failed!\n");
		goto end;
	}

	RTK_LOGI(TAG, "=== ECDSA TrustZone Demo Complete ===\n");
	RTK_LOGI(TAG, "Sign in Secure World, Verify in Non-Secure World: SUCCESS!\n");
#else
	RTK_LOGE(TAG, "TrustZone is not enabled! Please enable CONFIG_TRUSTZONE_EN\n");
#endif

end:
	rtos_task_delete(NULL);
}

void example_rdp_ecdsa(void)
{
#ifdef CONFIG_TRUSTZONE_EN
	RTK_LOGI(TAG, "ECDSA TrustZone Demo main\n");

	/* Create demo task */
	if (rtos_task_create(NULL, "ECDSA_TZ_DEMO", (rtos_task_t)ecdsa_tz_demo, NULL, 8192, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Cannot create demo task\n");
		while (1);
	}
#else
	RTK_LOGE(TAG, "Please enable TrustZone in menuconfig\n");
#endif
}
