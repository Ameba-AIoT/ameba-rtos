/**
 * @file  selftest_hash.c
 * @brief SHA-2 KAT using NIST FIPS 180-4 short-message test vectors.
 *
 * Test strategy:
 *   - KAT: known input → compare against NIST-published digest.
 *     If hardware produces wrong digest the test catches it.
 *   - Multipart: split same input over multiple update() calls,
 *     compare against single-shot result.  Proves hash_update
 *     accumulation is correct.
 */

#include "psa_selftest_util.h"

/* NIST FIPS 180-4, SHA-224 of the 3-byte message "abc" */
static const uint8_t sha224_expected[] = {
	0x23, 0x09, 0x7d, 0x22, 0x34, 0x05, 0xd8, 0x22, 0x86, 0x42, 0xa4, 0x77, 0xbd, 0xa2, 0x55, 0xb3,
	0x2a, 0xad, 0xbc, 0xe4, 0xbd, 0xa0, 0xb3, 0xf7, 0xe3, 0x6c, 0x9d, 0xa7,
};

/* NIST FIPS 180-4, SHA-256 of the 3-byte message "abc" */
static const uint8_t sha256_msg[] = { 'a', 'b', 'c' };
static const uint8_t sha256_expected[] = {
	0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
	0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
	0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
	0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad,
};

/* NIST FIPS 180-4, SHA-384 of "abc" */
static const uint8_t sha384_expected[] = {
	0xcb, 0x00, 0x75, 0x3f, 0x45, 0xa3, 0x5e, 0x8b,
	0xb5, 0xa0, 0x3d, 0x69, 0x9a, 0xc6, 0x50, 0x07,
	0x27, 0x2c, 0x32, 0xab, 0x0e, 0xde, 0xd1, 0x63,
	0x1a, 0x8b, 0x60, 0x5a, 0x43, 0xff, 0x5b, 0xed,
	0x80, 0x86, 0x07, 0x2b, 0xa1, 0xe7, 0xcc, 0x23,
	0x58, 0xba, 0xec, 0xa1, 0x34, 0xc8, 0x25, 0xa7,
};

/* NIST FIPS 180-4, SHA-512 of "abc" */
static const uint8_t sha512_expected[] = {
	0xdd, 0xaf, 0x35, 0xa1, 0x93, 0x61, 0x7a, 0xba,
	0xcc, 0x41, 0x73, 0x49, 0xae, 0x20, 0x41, 0x31,
	0x12, 0xe6, 0xfa, 0x4e, 0x89, 0xa9, 0x7e, 0xa2,
	0x0a, 0x9e, 0xee, 0xe6, 0x4b, 0x55, 0xd3, 0x9a,
	0x21, 0x92, 0x99, 0x2a, 0x27, 0x4f, 0xc1, 0xa8,
	0x36, 0xba, 0x3c, 0x23, 0xa3, 0xfe, 0xeb, 0xbd,
	0x45, 0x4d, 0x44, 0x23, 0x64, 0x3c, 0xe8, 0x0e,
	0x2a, 0x9a, 0xc9, 0x4f, 0xa5, 0x4c, 0xa4, 0x9f,
};

static int test_hash_kat(psa_algorithm_t alg, const uint8_t *expected,
						 size_t expected_len, const char *name)
{
	uint8_t digest[64];
	size_t digest_len = 0;

	ST_PSA_CHK(psa_hash_compute(alg,
								sha256_msg, sizeof(sha256_msg),
								digest, sizeof(digest), &digest_len));
	if (digest_len != expected_len) {
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR, "%s bad len %u\n", name, (unsigned)digest_len);
		goto fail;
	}
	ST_MEM_CHK(digest, expected, expected_len);
	ST_PASS(name);
	return 0;
fail:
	ST_FAIL(name);
	return 1;
}

static int test_hash_multipart(void)
{
	/* Split "abc" into "a" + "bc"; result must match single-shot. */
	uint8_t out_single[32], out_multi[32];
	size_t len_s = 0, len_m = 0;
	psa_hash_operation_t op = PSA_HASH_OPERATION_INIT;

	ST_PSA_CHK(psa_hash_compute(PSA_ALG_SHA_256,
								sha256_msg, sizeof(sha256_msg),
								out_single, sizeof(out_single), &len_s));

	ST_PSA_CHK(psa_hash_setup(&op, PSA_ALG_SHA_256));
	ST_PSA_CHK(psa_hash_update(&op, sha256_msg, 1));        /* "a" */
	ST_PSA_CHK(psa_hash_update(&op, sha256_msg + 1, 2));    /* "bc" */
	ST_PSA_CHK(psa_hash_finish(&op, out_multi, sizeof(out_multi), &len_m));

	if (len_s != len_m) {
		goto fail;
	}
	ST_MEM_CHK(out_single, out_multi, len_s);
	ST_PASS("hash_sha256_multipart");
	return 0;
fail:
	psa_hash_abort(&op);
	ST_FAIL("hash_sha256_multipart");
	return 1;
}

int selftest_hash(void)
{
	int fail = 0;
	fail += test_hash_kat(PSA_ALG_SHA_224, sha224_expected,
						  sizeof(sha224_expected), "hash_sha224_kat");
	fail += test_hash_kat(PSA_ALG_SHA_256, sha256_expected,
						  sizeof(sha256_expected), "hash_sha256_kat");
	fail += test_hash_kat(PSA_ALG_SHA_384, sha384_expected,
						  sizeof(sha384_expected), "hash_sha384_kat");
	fail += test_hash_kat(PSA_ALG_SHA_512, sha512_expected,
						  sizeof(sha512_expected), "hash_sha512_kat");
	fail += test_hash_multipart();
	return fail;
}
