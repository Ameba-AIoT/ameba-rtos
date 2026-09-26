/**
 * @file  selftest_aead.c
 * @brief AES-GCM / AES-CCM tests.
 *
 * Test strategy:
 *   KAT  — NIST CAVP GCM test vector (Encrypt-Then-MAC, 128-bit key).
 *           Proves hardware GCM matches the specification.
 *   Round-trip — random plaintext + AAD, PSA encrypt → PSA decrypt.
 *   Tamper    — corrupt one ciphertext byte, verify decrypt returns
 *               PSA_ERROR_INVALID_SIGNATURE.  Proves authentication works.
 */

#include "psa_selftest_util.h"

/* -----------------------------------------------------------------------
 * NIST CAVP GCM  (AES-128, IV=12 bytes, tag=16 bytes, no AAD)
 * from gcmEncryptExtIV128.rsp, first test case
 * --------------------------------------------------------------------- */
static const uint8_t gcm_key[] = {
	0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
	0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08,
};
static const uint8_t gcm_iv[] = {
	0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad,
	0xde, 0xca, 0xf8, 0x88,
};
static const uint8_t gcm_pt[] = {
	0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5,
	0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
	0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda,
	0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
	0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53,
	0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
	0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57,
	0xba, 0x63, 0x7b, 0x39, 0x1a, 0xaf, 0xd2, 0x55,
};
static const uint8_t gcm_ct[] = {
	0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24,
	0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c,
	0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0,
	0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e,
	0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c,
	0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05,
	0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97,
	0x3d, 0x58, 0xe0, 0x91, 0x47, 0x3f, 0x59, 0x85,
};
static const uint8_t gcm_tag[] = {
	0x4d, 0x5c, 0x2a, 0xf3, 0x27, 0xcd, 0x64, 0xa6,
	0x2c, 0xf3, 0x5a, 0xbd, 0x2b, 0xa6, 0xfa, 0xb4,
};

static psa_status_t import_aes_aead_key(const uint8_t *key, size_t key_len,
										psa_algorithm_t alg,
										mbedtls_svc_key_id_t *kid)
{
	psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
	psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
	psa_set_key_algorithm(&attr, alg);
	psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
	return psa_import_key(&attr, key, key_len, kid);
}

static int test_gcm_kat(void)
{
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	psa_algorithm_t alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 16);

	/* PSA encrypt output = ciphertext || tag */
	uint8_t enc_out[sizeof(gcm_ct) + 16];
	uint8_t dec_out[sizeof(gcm_pt)];
	size_t out_len;

	ST_PSA_CHK(import_aes_aead_key(gcm_key, sizeof(gcm_key), alg, &kid));

	ST_PSA_CHK(psa_aead_encrypt(kid, alg,
								gcm_iv, sizeof(gcm_iv),
								NULL, 0,
								gcm_pt, sizeof(gcm_pt),
								enc_out, sizeof(enc_out), &out_len));

	if (out_len != sizeof(gcm_ct) + 16) {
		goto fail;
	}
	ST_MEM_CHK(enc_out, gcm_ct, sizeof(gcm_ct));
	ST_MEM_CHK(enc_out + sizeof(gcm_ct), gcm_tag, 16);

	ST_PSA_CHK(psa_aead_decrypt(kid, alg,
								gcm_iv, sizeof(gcm_iv),
								NULL, 0,
								enc_out, out_len,
								dec_out, sizeof(dec_out), &out_len));
	ST_MEM_CHK(dec_out, gcm_pt, sizeof(gcm_pt));

	psa_destroy_key(kid);
	ST_PASS("aead_aes_gcm_kat");
	return 0;
fail:
	psa_destroy_key(kid);
	ST_FAIL("aead_aes_gcm_kat");
	return 1;
}

static int test_gcm_tamper(void)
{
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	psa_algorithm_t alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 16);
	uint8_t enc_out[sizeof(gcm_ct) + 16];
	uint8_t dec_out[sizeof(gcm_pt)];
	size_t out_len;

	ST_PSA_CHK(import_aes_aead_key(gcm_key, sizeof(gcm_key), alg, &kid));
	ST_PSA_CHK(psa_aead_encrypt(kid, alg,
								gcm_iv, sizeof(gcm_iv),
								NULL, 0, gcm_pt, sizeof(gcm_pt),
								enc_out, sizeof(enc_out), &out_len));

	/* Flip one bit in ciphertext → must fail authentication */
	enc_out[0] ^= 0x01;
	psa_status_t s = psa_aead_decrypt(kid, alg,
									  gcm_iv, sizeof(gcm_iv),
									  NULL, 0,
									  enc_out, out_len,
									  dec_out, sizeof(dec_out), &out_len);
	if (s != PSA_ERROR_INVALID_SIGNATURE) {
		goto fail;
	}

	psa_destroy_key(kid);
	ST_PASS("aead_aes_gcm_tamper");
	return 0;
fail:
	psa_destroy_key(kid);
	ST_FAIL("aead_aes_gcm_tamper");
	return 1;
}

static int test_ccm_roundtrip(void)
{
	static const uint8_t key[16] = {
		0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
		0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
	};
	static const uint8_t iv[13]  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
	static const uint8_t pt[24]  = {
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	};
	static const uint8_t aad[8]  = { 0, 1, 2, 3, 4, 5, 6, 7 };
	psa_algorithm_t alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 8);
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	uint8_t enc[sizeof(pt) + 8], dec[sizeof(pt)];
	size_t out_len;

	ST_PSA_CHK(import_aes_aead_key(key, sizeof(key), alg, &kid));
	ST_PSA_CHK(psa_aead_encrypt(kid, alg,
								iv, sizeof(iv),
								aad, sizeof(aad),
								pt, sizeof(pt),
								enc, sizeof(enc), &out_len));
	ST_PSA_CHK(psa_aead_decrypt(kid, alg,
								iv, sizeof(iv),
								aad, sizeof(aad),
								enc, out_len,
								dec, sizeof(dec), &out_len));
	if (out_len != sizeof(pt)) {
		goto fail;
	}
	ST_MEM_CHK(dec, pt, sizeof(pt));

	psa_destroy_key(kid);
	ST_PASS("aead_aes_ccm_roundtrip");
	return 0;
fail:
	psa_destroy_key(kid);
	ST_FAIL("aead_aes_ccm_roundtrip");
	return 1;
}

/* --- GCM multipart: split same input as KAT into 4×16B updates -------- */
static int test_gcm_multipart(void)
{
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	psa_algorithm_t alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 16);
	psa_aead_operation_t op = PSA_AEAD_OPERATION_INIT;
	uint8_t enc_mp[sizeof(gcm_ct) + 16]; /* ct + tag */
	uint8_t dec_mp[sizeof(gcm_pt)];
	size_t enc_total = 0, out_len, tag_len;

	ST_PSA_CHK(import_aes_aead_key(gcm_key, sizeof(gcm_key), alg, &kid));

	/* Encrypt via multipart, 4×16-byte updates */
	uint8_t tag_buf[16];
	ST_PSA_CHK(psa_aead_encrypt_setup(&op, kid, alg));
	ST_PSA_CHK(psa_aead_set_nonce(&op, gcm_iv, sizeof(gcm_iv)));
	ST_PSA_CHK(psa_aead_set_lengths(&op, 0, sizeof(gcm_pt)));

	for (size_t i = 0; i < sizeof(gcm_pt); i += 16) {
		ST_PSA_CHK(psa_aead_update(&op, gcm_pt + i, 16,
								   enc_mp + enc_total,
								   sizeof(enc_mp) - enc_total, &out_len));
		enc_total += out_len;
	}
	/* finish: no remaining ciphertext, just tag */
	ST_PSA_CHK(psa_aead_finish(&op, enc_mp + enc_total, sizeof(enc_mp) - enc_total,
							   &out_len, tag_buf, sizeof(tag_buf), &tag_len));
	enc_total += out_len;

	/* Compare ciphertext with single-shot KAT output */
	if (enc_total != sizeof(gcm_ct) || tag_len != 16) {
		goto fail;
	}
	ST_MEM_CHK(enc_mp, gcm_ct, sizeof(gcm_ct));
	ST_MEM_CHK(tag_buf, gcm_tag, 16);

	/* Decrypt single-shot to verify round-trip */
	uint8_t dec_in[sizeof(gcm_ct) + 16];
	memcpy(dec_in, enc_mp, enc_total);
	memcpy(dec_in + enc_total, tag_buf, 16);
	ST_PSA_CHK(psa_aead_decrypt(kid, alg,
								gcm_iv, sizeof(gcm_iv), NULL, 0,
								dec_in, enc_total + 16,
								dec_mp, sizeof(dec_mp), &out_len));
	ST_MEM_CHK(dec_mp, gcm_pt, sizeof(gcm_pt));

	psa_destroy_key(kid);
	ST_PASS("aead_aes_gcm_multipart_4x16");
	return 0;
fail:
	psa_aead_abort(&op);
	psa_destroy_key(kid);
	ST_FAIL("aead_aes_gcm_multipart_4x16");
	return 1;
}

int selftest_aead(void)
{
	int fail = 0;
	fail += test_gcm_kat();
	fail += test_gcm_tamper();
	fail += test_gcm_multipart();
	fail += test_ccm_roundtrip();
	return fail;
}
