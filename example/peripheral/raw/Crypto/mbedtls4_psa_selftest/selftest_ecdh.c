/**
 * @file  selftest_ecdh.c
 * @brief ECDH P-256 key agreement test.
 *
 * Test strategy:
 *   Alice and Bob each generate a key pair, then derive the shared secret
 *   using the other party's public key.  The two shared secrets must match.
 *
 *   This is the standard proof that ECDH is correct:
 *     Alice: SA = pke_ecp_mul(dA, QB)
 *     Bob:   SB = pke_ecp_mul(dB, QA)
 *     SA == SB  ←→  correct scalar multiplication
 *
 *   If the hardware point-multiplication is wrong, SA ≠ SB.
 */

#include "psa_selftest_util.h"

static psa_status_t gen_ecdh_key(psa_ecc_family_t family, size_t bits,
								 mbedtls_svc_key_id_t *kid)
{
	psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
	psa_set_key_type(&attr, PSA_KEY_TYPE_ECC_KEY_PAIR(family));
	psa_set_key_bits(&attr, bits);
	psa_set_key_algorithm(&attr, PSA_ALG_ECDH);
	psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_DERIVE);
	return psa_generate_key(&attr, kid);
}

static int test_ecdh_roundtrip(psa_ecc_family_t family, size_t bits,
							   const char *name)
{
	mbedtls_svc_key_id_t kidA = MBEDTLS_SVC_KEY_ID_INIT;
	mbedtls_svc_key_id_t kidB = MBEDTLS_SVC_KEY_ID_INIT;
	size_t pub_key_len = PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(bits);
	uint8_t *pubA = NULL, *pubB = NULL;
	uint8_t sharedA[66], sharedB[66];   /* 66 bytes covers P-521 */
	size_t pubA_len, pubB_len, sA_len, sB_len;
	int ret = 1;

	pubA = rtos_mem_malloc(pub_key_len);
	pubB = rtos_mem_malloc(pub_key_len);
	if (!pubA || !pubB) {
		goto fail;
	}

	ST_PSA_CHK(gen_ecdh_key(family, bits, &kidA));
	ST_PSA_CHK(gen_ecdh_key(family, bits, &kidB));

	ST_PSA_CHK(psa_export_public_key(kidA, pubA, pub_key_len, &pubA_len));
	ST_PSA_CHK(psa_export_public_key(kidB, pubB, pub_key_len, &pubB_len));

	/* Alice uses her private key + Bob's public key */
	ST_PSA_CHK(psa_raw_key_agreement(PSA_ALG_ECDH, kidA,
									 pubB, pubB_len,
									 sharedA, sizeof(sharedA), &sA_len));

	/* Bob uses his private key + Alice's public key */
	ST_PSA_CHK(psa_raw_key_agreement(PSA_ALG_ECDH, kidB,
									 pubA, pubA_len,
									 sharedB, sizeof(sharedB), &sB_len));

	if (sA_len != sB_len) {
		goto fail;
	}
	ST_MEM_CHK(sharedA, sharedB, sA_len);

	ret = 0;
	ST_PASS(name);
fail:
	psa_destroy_key(kidA);
	psa_destroy_key(kidB);
	rtos_mem_free(pubA);
	rtos_mem_free(pubB);
	if (ret) {
		ST_FAIL(name);
	}
	return ret;
}

int selftest_ecdh(void)
{
	int fail = 0;
	fail += test_ecdh_roundtrip(PSA_ECC_FAMILY_SECP_R1, 256,
								"ecdh_p256_shared_secret");
	fail += test_ecdh_roundtrip(PSA_ECC_FAMILY_SECP_R1, 384,
								"ecdh_p384_shared_secret");
	return fail;
}
