/**
 * @file  selftest_ecdsa.c
 * @brief ECDSA P-256 and P-384 sign→verify round-trip + keygen test.
 *
 * Test strategy:
 *   Sign → Verify round-trip using PSA APIs (no hardcoded vectors needed:
 *   if sign/verify are not mutual inverses, the verify step fails).
 *   This directly proves the hardware ECDSA engine and software P-256
 *   verifier are consistent.
 *
 *   keygen: generate a key, sign with it, verify — proves ECC keygen
 *   (P1-3) produces a usable key pair.
 *
 *   Wrong-key: sign with key A, verify with key B → must fail.
 *   Proves the verifier is actually checking the signature.
 */

#include "psa_selftest_util.h"

static psa_status_t gen_ecc_keypair(psa_ecc_family_t family, size_t bits,
									mbedtls_svc_key_id_t *kid)
{
	psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
	psa_set_key_type(&attr, PSA_KEY_TYPE_ECC_KEY_PAIR(family));
	psa_set_key_bits(&attr, bits);
	psa_set_key_algorithm(&attr, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
	psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH);
	return psa_generate_key(&attr, kid);
}

static int test_ecdsa_roundtrip(psa_ecc_family_t family, size_t bits,
								const char *name)
{
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	psa_algorithm_t alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
	static const uint8_t msg[] = "mbedtls 4.1.1 ECDSA selftest message";
	uint8_t hash[32], sig[128];
	size_t sig_len;

	ST_PSA_CHK(gen_ecc_keypair(family, bits, &kid));

	/* hash the message first */
	ST_PSA_CHK(psa_hash_compute(PSA_ALG_SHA_256,
								msg, sizeof(msg) - 1,
	hash, sizeof(hash), &(size_t) {
		0
	}));

	ST_PSA_CHK(psa_sign_hash(kid, alg, hash, sizeof(hash),
							 sig, sizeof(sig), &sig_len));
	ST_PSA_CHK(psa_verify_hash(kid, alg, hash, sizeof(hash),
							   sig, sig_len));

	psa_destroy_key(kid);
	ST_PASS(name);
	return 0;
fail:
	psa_destroy_key(kid);
	ST_FAIL(name);
	return 1;
}

static int test_ecdsa_wrong_key(void)
{
	mbedtls_svc_key_id_t kidA = MBEDTLS_SVC_KEY_ID_INIT;
	mbedtls_svc_key_id_t kidB = MBEDTLS_SVC_KEY_ID_INIT;
	psa_algorithm_t alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
	static const uint8_t msg[] = "wrong key test";
	uint8_t hash[32], sig[128];
	size_t sig_len;

	ST_PSA_CHK(gen_ecc_keypair(PSA_ECC_FAMILY_SECP_R1, 256, &kidA));
	ST_PSA_CHK(gen_ecc_keypair(PSA_ECC_FAMILY_SECP_R1, 256, &kidB));

	ST_PSA_CHK(psa_hash_compute(PSA_ALG_SHA_256,
								msg, sizeof(msg) - 1,
	hash, sizeof(hash), &(size_t) {
		0
	}));
	ST_PSA_CHK(psa_sign_hash(kidA, alg, hash, sizeof(hash),
							 sig, sizeof(sig), &sig_len));

	/* verify with different key → must fail */
	psa_status_t s = psa_verify_hash(kidB, alg, hash, sizeof(hash),
									 sig, sig_len);
	if (s != PSA_ERROR_INVALID_SIGNATURE) {
		goto fail;
	}

	psa_destroy_key(kidA);
	psa_destroy_key(kidB);
	ST_PASS("ecdsa_wrong_key");
	return 0;
fail:
	psa_destroy_key(kidA);
	psa_destroy_key(kidB);
	ST_FAIL("ecdsa_wrong_key");
	return 1;
}

int selftest_ecdsa(void)
{
	int fail = 0;
	fail += test_ecdsa_roundtrip(PSA_ECC_FAMILY_SECP_R1, 256,
								 "ecdsa_p256_roundtrip");
	fail += test_ecdsa_roundtrip(PSA_ECC_FAMILY_SECP_R1, 384,
								 "ecdsa_p384_roundtrip");
	fail += test_ecdsa_wrong_key();
	return fail;
}
