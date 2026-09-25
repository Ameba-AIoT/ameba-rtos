/**
 * @file  selftest_cipher.c
 * @brief AES ECB / CBC / CTR tests.
 *
 * Test strategy:
 *   KAT  — NIST SP 800-38A F.1/F.2/F.5 test vectors.
 *           These prove the hardware computes the correct algorithm.
 *   Round-trip — encrypt with PSA single-shot, decrypt with PSA single-shot.
 *   Multipart round-trip (CBC key path for P1-2):
 *     Encrypt via multipart update() in various chunk sizes,
 *     decrypt via single-shot, compare to original.
 *     This is the primary correctness test for the residual-buffer
 *     state machine introduced in P1-2.
 */

#include "psa_selftest_util.h"

/* -----------------------------------------------------------------------
 * NIST SP 800-38A  F.1.1  ECB-AES128 Encrypt
 * key = 2b 7e 15 16 28 ae d2 a6 ab f7 15 88 09 cf 4f 3c
 * pt  = 6b c1 be e2 2e 40 9f 96 e9 3d 7e 11 73 93 17 2a
 * ct  = 3a d7 7b b4 0d 7a 36 60 a8 9e ca f3 24 66 ef 97
 * --------------------------------------------------------------------- */
static const uint8_t ecb_key[] = {
	0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
	0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
};
static const uint8_t ecb_pt[] = {
	0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
};
static const uint8_t ecb_ct[] = {
	0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
	0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
};

/* -----------------------------------------------------------------------
 * NIST SP 800-38A  F.2.1  CBC-AES128 Encrypt  (first two blocks)
 * key = 2b 7e 15 16 28 ae d2 a6 ab f7 15 88 09 cf 4f 3c
 * IV  = 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
 * PT  = 6b c1...2a | ae 2d...3f | (2 blocks shown below)
 * CT  = 76 49...c4 | 50 04...71
 * --------------------------------------------------------------------- */
static const uint8_t cbc_key[] = {
	0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
	0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
};
static const uint8_t cbc_iv[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};
static const uint8_t cbc_pt[] = {
	0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, /* block 1 */
	0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
	0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, /* block 2 */
	0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
};
static const uint8_t cbc_ct[] = {
	0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, /* block 1 */
	0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
	0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee, /* block 2 */
	0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
};

/* helper: import a raw AES key, return key_id */
static psa_status_t import_aes_key(const uint8_t *key, size_t key_len,
								   psa_algorithm_t alg, psa_key_usage_t usage,
								   mbedtls_svc_key_id_t *kid)
{
	psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
	psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
	psa_set_key_algorithm(&attr, alg);
	psa_set_key_usage_flags(&attr, usage);
	return psa_import_key(&attr, key, key_len, kid);
}

/* --- ECB KAT ---------------------------------------------------------- */
static int test_ecb_kat(void)
{
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	uint8_t ct[16], rt[16];
	size_t out_len;

	ST_PSA_CHK(import_aes_key(ecb_key, sizeof(ecb_key),
							  PSA_ALG_ECB_NO_PADDING,
							  PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT, &kid));

	/* psa_cipher_encrypt: input = plaintext, output = ciphertext (ECB: no IV) */
	ST_PSA_CHK(psa_cipher_encrypt(kid, PSA_ALG_ECB_NO_PADDING,
								  ecb_pt, sizeof(ecb_pt),
								  ct, sizeof(ct), &out_len));
	ST_MEM_CHK(ct, ecb_ct, sizeof(ecb_ct));

	/* psa_cipher_decrypt: input = ciphertext, output = plaintext */
	ST_PSA_CHK(psa_cipher_decrypt(kid, PSA_ALG_ECB_NO_PADDING,
								  ct, sizeof(ct),
								  rt, sizeof(rt), &out_len));
	ST_MEM_CHK(rt, ecb_pt, sizeof(ecb_pt));

	psa_destroy_key(kid);
	ST_PASS("cipher_aes_ecb_kat");
	return 0;
fail:
	psa_destroy_key(kid);
	ST_FAIL("cipher_aes_ecb_kat");
	return 1;
}

/* --- CBC KAT ---------------------------------------------------------- */
static int test_cbc_kat(void)
{
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	/* psa_cipher_encrypt prepends IV to output */
	uint8_t enc_buf[16 + sizeof(cbc_ct)];
	uint8_t dec_buf[sizeof(cbc_pt)];
	size_t out_len;

	ST_PSA_CHK(import_aes_key(cbc_key, sizeof(cbc_key),
							  PSA_ALG_CBC_NO_PADDING,
							  PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT, &kid));

	/* PSA encrypt: caller supplies IV via the multipart API; single-shot
	 * psa_cipher_encrypt uses a random IV and prepends it to output. */
	psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;
	ST_PSA_CHK(psa_cipher_encrypt_setup(&enc_op, kid, PSA_ALG_CBC_NO_PADDING));
	ST_PSA_CHK(psa_cipher_set_iv(&enc_op, cbc_iv, sizeof(cbc_iv)));
	ST_PSA_CHK(psa_cipher_update(&enc_op, cbc_pt, sizeof(cbc_pt),
								 enc_buf, sizeof(enc_buf), &out_len));
	size_t fin_len = 0;
	ST_PSA_CHK(psa_cipher_finish(&enc_op, enc_buf + out_len,
								 sizeof(enc_buf) - out_len, &fin_len));
	out_len += fin_len;

	if (out_len != sizeof(cbc_ct)) {
		goto fail;
	}
	ST_MEM_CHK(enc_buf, cbc_ct, sizeof(cbc_ct));

	/* Decrypt: PSA decrypt single-shot expects IV prepended */
	uint8_t dec_in[16 + sizeof(cbc_ct)];
	memcpy(dec_in, cbc_iv, 16);
	memcpy(dec_in + 16, cbc_ct, sizeof(cbc_ct));
	ST_PSA_CHK(psa_cipher_decrypt(kid, PSA_ALG_CBC_NO_PADDING,
								  dec_in, sizeof(dec_in),
								  dec_buf, sizeof(dec_buf), &out_len));
	if (out_len != sizeof(cbc_pt)) {
		goto fail;
	}
	ST_MEM_CHK(dec_buf, cbc_pt, sizeof(cbc_pt));

	psa_destroy_key(kid);
	ST_PASS("cipher_aes_cbc_kat");
	return 0;
fail:
	psa_cipher_abort(&enc_op);
	psa_destroy_key(kid);
	ST_FAIL("cipher_aes_cbc_kat");
	return 1;
}

/* --- CBC multipart: split encrypt, single-shot decrypt ---------------- */
/*
 * Critical test for P1-2 residual-buffer state machine.
 * Encrypt the 32-byte NIST plaintext via multipart in three chunks:
 *   update(5 bytes)   → residual=5, no output
 *   update(11 bytes)  → 16-byte block completes, 16 bytes output; residual=0
 *   update(16 bytes)  → another full block, 16 bytes output
 * Total output = 32 bytes, must equal cbc_ct.
 */
static int test_cbc_multipart(void)
{
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	psa_cipher_operation_t op = PSA_CIPHER_OPERATION_INIT;
	uint8_t enc_out[sizeof(cbc_ct)];
	uint8_t dec_in[16 + sizeof(cbc_ct)];
	uint8_t dec_out[sizeof(cbc_pt)];
	size_t enc_total = 0, out_len, fin_len;

	ST_PSA_CHK(import_aes_key(cbc_key, sizeof(cbc_key),
							  PSA_ALG_CBC_NO_PADDING,
							  PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT, &kid));

	ST_PSA_CHK(psa_cipher_encrypt_setup(&op, kid, PSA_ALG_CBC_NO_PADDING));
	ST_PSA_CHK(psa_cipher_set_iv(&op, cbc_iv, sizeof(cbc_iv)));

	/* chunk 1: 5 bytes — fills residual buf, no output yet */
	ST_PSA_CHK(psa_cipher_update(&op, cbc_pt, 5,
								 enc_out + enc_total,
								 sizeof(enc_out) - enc_total, &out_len));
	enc_total += out_len;   /* expect 0 */

	/* chunk 2: 11 bytes — completes first 16-byte block */
	ST_PSA_CHK(psa_cipher_update(&op, cbc_pt + 5, 11,
								 enc_out + enc_total,
								 sizeof(enc_out) - enc_total, &out_len));
	enc_total += out_len;   /* expect 16 */

	/* chunk 3: 16 bytes — full second block */
	ST_PSA_CHK(psa_cipher_update(&op, cbc_pt + 16, 16,
								 enc_out + enc_total,
								 sizeof(enc_out) - enc_total, &out_len));
	enc_total += out_len;   /* expect 16 */

	ST_PSA_CHK(psa_cipher_finish(&op,
								 enc_out + enc_total,
								 sizeof(enc_out) - enc_total, &fin_len));
	enc_total += fin_len;

	if (enc_total != sizeof(cbc_ct)) {
		goto fail;
	}
	ST_MEM_CHK(enc_out, cbc_ct, sizeof(cbc_ct));

	/* decrypt single-shot to verify round-trip */
	memcpy(dec_in, cbc_iv, 16);
	memcpy(dec_in + 16, enc_out, enc_total);
	ST_PSA_CHK(psa_cipher_decrypt(kid, PSA_ALG_CBC_NO_PADDING,
								  dec_in, sizeof(dec_in),
								  dec_out, sizeof(dec_out), &out_len));
	ST_MEM_CHK(dec_out, cbc_pt, sizeof(cbc_pt));

	psa_destroy_key(kid);
	ST_PASS("cipher_aes_cbc_multipart_5_11_16");
	return 0;
fail:
	psa_cipher_abort(&op);
	psa_destroy_key(kid);
	ST_FAIL("cipher_aes_cbc_multipart_5_11_16");
	return 1;
}

/* --- CBC multipart: extreme fragmentation (1 byte per update) --------- */
static int test_cbc_multipart_1byte(void)
{
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	psa_cipher_operation_t op = PSA_CIPHER_OPERATION_INIT;
	uint8_t enc_out[sizeof(cbc_ct) + 16];
	uint8_t dec_in[16 + sizeof(cbc_ct)];
	uint8_t dec_out[sizeof(cbc_pt)];
	size_t enc_total = 0, out_len, fin_len;

	ST_PSA_CHK(import_aes_key(cbc_key, sizeof(cbc_key),
							  PSA_ALG_CBC_NO_PADDING,
							  PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT, &kid));

	ST_PSA_CHK(psa_cipher_encrypt_setup(&op, kid, PSA_ALG_CBC_NO_PADDING));
	ST_PSA_CHK(psa_cipher_set_iv(&op, cbc_iv, sizeof(cbc_iv)));

	for (size_t i = 0; i < sizeof(cbc_pt); i++) {
		ST_PSA_CHK(psa_cipher_update(&op, cbc_pt + i, 1,
									 enc_out + enc_total,
									 sizeof(enc_out) - enc_total, &out_len));
		enc_total += out_len;
	}
	ST_PSA_CHK(psa_cipher_finish(&op,
								 enc_out + enc_total,
								 sizeof(enc_out) - enc_total, &fin_len));
	enc_total += fin_len;

	if (enc_total != sizeof(cbc_ct)) {
		goto fail;
	}
	ST_MEM_CHK(enc_out, cbc_ct, sizeof(cbc_ct));

	memcpy(dec_in, cbc_iv, 16);
	memcpy(dec_in + 16, enc_out, enc_total);
	ST_PSA_CHK(psa_cipher_decrypt(kid, PSA_ALG_CBC_NO_PADDING,
								  dec_in, sizeof(dec_in),
								  dec_out, sizeof(dec_out), &out_len));
	ST_MEM_CHK(dec_out, cbc_pt, sizeof(cbc_pt));

	psa_destroy_key(kid);
	ST_PASS("cipher_aes_cbc_multipart_1byte");
	return 0;
fail:
	psa_cipher_abort(&op);
	psa_destroy_key(kid);
	ST_FAIL("cipher_aes_cbc_multipart_1byte");
	return 1;
}

/* -----------------------------------------------------------------------
 * NIST SP 800-38A F.1.5  ECB-AES256 Encrypt
 * key = 603deb10...f4  pt = 6bc1...2a  ct = f3eed1...f8
 * --------------------------------------------------------------------- */
static const uint8_t ecb256_key[] = {
	0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
	0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4,
};
static const uint8_t ecb256_ct[] = {
	0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c, 0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8,
};

static int test_ecb256_kat(void)
{
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	uint8_t ct[16], rt[16];
	size_t out_len;

	ST_PSA_CHK(import_aes_key(ecb256_key, sizeof(ecb256_key),
							  PSA_ALG_ECB_NO_PADDING,
							  PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT, &kid));
	ST_PSA_CHK(psa_cipher_encrypt(kid, PSA_ALG_ECB_NO_PADDING,
								  ecb_pt, sizeof(ecb_pt), ct, sizeof(ct), &out_len));
	ST_MEM_CHK(ct, ecb256_ct, sizeof(ecb256_ct));
	ST_PSA_CHK(psa_cipher_decrypt(kid, PSA_ALG_ECB_NO_PADDING,
								  ct, sizeof(ct), rt, sizeof(rt), &out_len));
	ST_MEM_CHK(rt, ecb_pt, sizeof(ecb_pt));
	psa_destroy_key(kid);
	ST_PASS("cipher_aes256_ecb_kat");
	return 0;
fail:
	psa_destroy_key(kid);
	ST_FAIL("cipher_aes256_ecb_kat");
	return 1;
}

/* -----------------------------------------------------------------------
 * NIST SP 800-38A F.2.5  CBC-AES256 Encrypt (first two blocks)
 * --------------------------------------------------------------------- */
static const uint8_t cbc256_ct[] = {
	0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba, 0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6,
	0x9c, 0xfc, 0x4e, 0x96, 0x7e, 0xdb, 0x80, 0x8d, 0x67, 0x9f, 0x77, 0x7b, 0xc6, 0x70, 0x2c, 0x7d,
};

static int test_cbc256_kat(void)
{
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	uint8_t enc_buf[sizeof(cbc256_ct)];
	uint8_t dec_buf[sizeof(cbc_pt)];
	size_t out_len, fin_len;
	psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;

	ST_PSA_CHK(import_aes_key(ecb256_key, sizeof(ecb256_key),
							  PSA_ALG_CBC_NO_PADDING,
							  PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT, &kid));

	ST_PSA_CHK(psa_cipher_encrypt_setup(&enc_op, kid, PSA_ALG_CBC_NO_PADDING));
	ST_PSA_CHK(psa_cipher_set_iv(&enc_op, cbc_iv, sizeof(cbc_iv)));
	ST_PSA_CHK(psa_cipher_update(&enc_op, cbc_pt, sizeof(cbc_pt),
								 enc_buf, sizeof(enc_buf), &out_len));
	ST_PSA_CHK(psa_cipher_finish(&enc_op, enc_buf + out_len,
								 sizeof(enc_buf) - out_len, &fin_len));
	out_len += fin_len;
	if (out_len != sizeof(cbc256_ct)) {
		goto fail;
	}
	ST_MEM_CHK(enc_buf, cbc256_ct, sizeof(cbc256_ct));

	uint8_t dec_in[16 + sizeof(cbc256_ct)];
	memcpy(dec_in, cbc_iv, 16);
	memcpy(dec_in + 16, cbc256_ct, sizeof(cbc256_ct));
	ST_PSA_CHK(psa_cipher_decrypt(kid, PSA_ALG_CBC_NO_PADDING,
								  dec_in, sizeof(dec_in),
								  dec_buf, sizeof(dec_buf), &out_len));
	ST_MEM_CHK(dec_buf, cbc_pt, sizeof(cbc_pt));
	psa_destroy_key(kid);
	ST_PASS("cipher_aes256_cbc_kat");
	return 0;
fail:
	psa_cipher_abort(&enc_op);
	psa_destroy_key(kid);
	ST_FAIL("cipher_aes256_cbc_kat");
	return 1;
}

/* --- ECB multipart (2 blocks, 8+8 split) ------------------------------ */
static int test_ecb_multipart(void)
{
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	psa_cipher_operation_t op = PSA_CIPHER_OPERATION_INIT;
	/* Two 16-byte blocks of plaintext (use cbc_pt = 32 bytes) */
	uint8_t enc_out[32];
	uint8_t dec_out[32];
	size_t enc_total = 0, out_len, fin_len;

	ST_PSA_CHK(import_aes_key(ecb_key, sizeof(ecb_key),
							  PSA_ALG_ECB_NO_PADDING,
							  PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT, &kid));

	/* Encrypt: feed in 8+8+8+8 byte chunks (none align to block until 16) */
	ST_PSA_CHK(psa_cipher_encrypt_setup(&op, kid, PSA_ALG_ECB_NO_PADDING));
	/* ECB has no IV */
	ST_PSA_CHK(psa_cipher_update(&op, cbc_pt,      8, enc_out, sizeof(enc_out), &out_len));
	enc_total += out_len;
	ST_PSA_CHK(psa_cipher_update(&op, cbc_pt + 8,  8, enc_out + enc_total, sizeof(enc_out) - enc_total, &out_len));
	enc_total += out_len;
	ST_PSA_CHK(psa_cipher_update(&op, cbc_pt + 16, 8, enc_out + enc_total, sizeof(enc_out) - enc_total, &out_len));
	enc_total += out_len;
	ST_PSA_CHK(psa_cipher_update(&op, cbc_pt + 24, 8, enc_out + enc_total, sizeof(enc_out) - enc_total, &out_len));
	enc_total += out_len;
	ST_PSA_CHK(psa_cipher_finish(&op, enc_out + enc_total, sizeof(enc_out) - enc_total, &fin_len));
	enc_total += fin_len;

	/* Decrypt single-shot, compare with original */
	ST_PSA_CHK(psa_cipher_decrypt(kid, PSA_ALG_ECB_NO_PADDING,
								  enc_out, enc_total, dec_out, sizeof(dec_out), &out_len));
	if (out_len != sizeof(cbc_pt)) {
		goto fail;
	}
	ST_MEM_CHK(dec_out, cbc_pt, sizeof(cbc_pt));

	psa_destroy_key(kid);
	ST_PASS("cipher_aes_ecb_multipart_8x4");
	return 0;
fail:
	psa_cipher_abort(&op);
	psa_destroy_key(kid);
	ST_FAIL("cipher_aes_ecb_multipart_8x4");
	return 1;
}

/* --- CTR round-trip --------------------------------------------------- */
static int test_ctr_roundtrip(void)
{
	static const uint8_t key[16] = {
		0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
		0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
	};
	static const uint8_t pt[32] = {
		0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
		0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
		0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
		0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
	};
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	/* psa_cipher_encrypt generates a random IV and prepends it */
	uint8_t enc[16 + 32], dec[32];
	size_t enc_len, dec_len;

	ST_PSA_CHK(import_aes_key(key, sizeof(key), PSA_ALG_CTR,
							  PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT, &kid));
	/* psa_cipher_encrypt for CTR: generates random IV, prepends to output */
	ST_PSA_CHK(psa_cipher_encrypt(kid, PSA_ALG_CTR,
								  pt, sizeof(pt),
								  enc, sizeof(enc), &enc_len));
	ST_PSA_CHK(psa_cipher_decrypt(kid, PSA_ALG_CTR,
								  enc, enc_len, dec, sizeof(dec), &dec_len));
	if (dec_len != sizeof(pt)) {
		goto fail;
	}
	ST_MEM_CHK(dec, pt, sizeof(pt));

	psa_destroy_key(kid);
	ST_PASS("cipher_aes_ctr_roundtrip");
	return 0;
fail:
	psa_destroy_key(kid);
	ST_FAIL("cipher_aes_ctr_roundtrip");
	return 1;
}

int selftest_cipher(void)
{
	int fail = 0;
	fail += test_ecb_kat();
	fail += test_ecb256_kat();
	fail += test_ecb_multipart();
	fail += test_cbc_kat();
	fail += test_cbc256_kat();
	fail += test_cbc_multipart();
	fail += test_cbc_multipart_1byte();
	fail += test_ctr_roundtrip();
	return fail;
}
