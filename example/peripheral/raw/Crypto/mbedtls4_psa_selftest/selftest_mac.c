/**
 * @file  selftest_mac.c
 * @brief HMAC-SHA-256/384/512 tests.
 *
 * Three test categories:
 * 1. RFC 4231 KAT (TC1/TC2) — both use small keys → PSA software HMAC path
 * 2. Hardware HMAC cross-check — 32-byte key → hardware path via both ROM API
 *    and PSA; compare results (no RFC vector needed)
 * 3. Manual HMAC via two psa_hash_compute calls — isolates whether the bug is
 *    in PSA HMAC state machine or in the underlying SHA hardware
 *
 * TC1: key=20 bytes (0x0b*20), data="Hi There"  → software path (20≠16/24/32)
 * TC2: key=4 bytes ("Jefe"),   data="what do ya want for nothing?"
 *      → software path (4≠16/24/32); TC1 passes/fails independently of TC2,
 *        helping isolate whether the failure is key-length-specific.
 * TC_HW: key=32 bytes (0x01*32), data="test"    → hardware path (32=32)
 */

#include "psa_selftest_util.h"
#include "ameba_sha.h"
#include "ameba_key_management.h"
#include "ameba_sema_rom.h"

/* --- RFC 4231 TC1 ------------------------------------------------------- */
static const uint8_t tc1_key[20] = {
	0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
	0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
};
/* "Hi There" */
static const uint8_t tc1_data[] = {
	0x48, 0x69, 0x20, 0x54, 0x68, 0x65, 0x72, 0x65,
};
static const uint8_t tc1_hmac256[] = {
	0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53, 0x5c, 0xa8, 0xaf, 0xce, 0xaf, 0x0b, 0xf1, 0x2b,
	0x88, 0x1d, 0xc2, 0x00, 0xc9, 0x83, 0x3d, 0xa7, 0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32, 0xcf, 0xf7,
};
static const uint8_t tc1_hmac384[] = {
	0xaf, 0xd0, 0x39, 0x44, 0xd8, 0x48, 0x95, 0x62, 0x6b, 0x08, 0x25, 0xf4, 0xab, 0x46, 0x90, 0x7f,
	0x15, 0xf9, 0xda, 0xdb, 0xe4, 0x10, 0x1e, 0xc6, 0x82, 0xaa, 0x03, 0x4c, 0x7c, 0xeb, 0xc5, 0x9c,
	0xfa, 0xea, 0x9e, 0xa9, 0x07, 0x6e, 0xde, 0x7f, 0x4a, 0xf1, 0x52, 0xe8, 0xb2, 0xfa, 0x9c, 0xb6,
};
static const uint8_t tc1_hmac512[] = {
	0x87, 0xaa, 0x7c, 0xde, 0xa5, 0xef, 0x61, 0x9d, 0x4f, 0xf0, 0xb4, 0x24, 0x1a, 0x1d, 0x6c, 0xb0,
	0x23, 0x79, 0xf4, 0xe2, 0xce, 0x4e, 0xc2, 0x78, 0x7a, 0xd0, 0xb3, 0x05, 0x45, 0xe1, 0x7c, 0xde,
	0xda, 0xa8, 0x33, 0xb7, 0xd6, 0xb8, 0xa7, 0x02, 0x03, 0x8b, 0x27, 0x4e, 0xae, 0xa3, 0xf4, 0xe4,
	0xbe, 0x9d, 0x91, 0x4e, 0xeb, 0x61, 0xf1, 0x70, 0x2e, 0x69, 0x6c, 0x20, 0x3a, 0x12, 0x68, 0x54,
};

/* --- 16-byte key KAT (hardware HMAC path: key satisfies 128-bit filter) -- */
/* Key = AES-128 test key; data = TC2 data; expected computed with Python hmac */
static const uint8_t hw_key16[16] = {
	0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
};
static const uint8_t hw_hmac256[] = {
	0xfc, 0x93, 0x0a, 0xc2, 0x7f, 0xf7, 0x2d, 0x47, 0xe7, 0x48, 0x5b, 0x1e, 0x12, 0x80, 0xcb, 0x24,
	0xfb, 0xa8, 0xcf, 0x2f, 0x21, 0x62, 0xff, 0x79, 0x0e, 0x52, 0xab, 0x66, 0x0c, 0x1c, 0xb9, 0x50,
};
static const uint8_t hw_hmac384[48] = {
	0x9a, 0x3f, 0xec, 0x85, 0xe9, 0x13, 0x82, 0x20, 0xfe, 0xda, 0x12, 0x13, 0x74, 0x5f, 0xe8, 0x0b,
	0x55, 0x80, 0x75, 0x8b, 0x20, 0xfb, 0x42, 0x70, 0x41, 0x36, 0x2c, 0x0e, 0x9d, 0x60, 0x4e, 0xb5,
	0xb1, 0xaf, 0xc2, 0x5e, 0x89, 0x48, 0xfc, 0xbe, 0x81, 0xeb, 0x90, 0xe9, 0x0a, 0x23, 0x2c, 0x64,
};

/* --- RFC 4231 TC2 ------------------------------------------------------- */
static const uint8_t tc2_key[] = { 0x4a, 0x65, 0x66, 0x65 }; /* "Jefe" */
static const uint8_t tc2_data[] = {
	0x77, 0x68, 0x61, 0x74, 0x20, 0x64, 0x6f, 0x20,
	0x79, 0x61, 0x20, 0x77, 0x61, 0x6e, 0x74, 0x20,
	0x66, 0x6f, 0x72, 0x20, 0x6e, 0x6f, 0x74, 0x68,
	0x69, 0x6e, 0x67, 0x3f,
};
static const uint8_t tc2_hmac256[] = {
	0x5b, 0xdc, 0xc1, 0x46, 0xbf, 0x60, 0x75, 0x4e, 0x6a, 0x04, 0x24, 0x26, 0x08, 0x95, 0x75, 0xc7,
	0x5a, 0x00, 0x3f, 0x08, 0x9d, 0x27, 0x39, 0x83, 0x9d, 0xec, 0x58, 0xb9, 0x64, 0xec, 0x38, 0x43,
};

static int test_hmac(psa_algorithm_t alg,
					 const uint8_t *key, size_t key_len,
					 const uint8_t *data, size_t data_len,
					 const uint8_t *expected, size_t expected_len,
					 const char *name)
{
	psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	uint8_t mac[64];
	size_t mac_len;

	psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
	psa_set_key_algorithm(&attr, alg);
	psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_MESSAGE);
	ST_PSA_CHK(psa_import_key(&attr, key, key_len, &kid));

	ST_PSA_CHK(psa_mac_compute(kid, alg,
							   data, data_len,
							   mac, sizeof(mac), &mac_len));
	if (mac_len != expected_len) {
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR, "%s bad mac_len %u\n",
				 name, (unsigned)mac_len);
		goto fail;
	}
	if (memcmp(mac, expected, expected_len) != 0) {
		/* Print first 8 bytes of computed vs expected for diagnosis */
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR,
				 "%s got[0-7]:  %02x%02x%02x%02x%02x%02x%02x%02x\n", name,
				 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]);
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR,
				 "%s exp[0-7]:  %02x%02x%02x%02x%02x%02x%02x%02x\n", name,
				 expected[0], expected[1], expected[2], expected[3],
				 expected[4], expected[5], expected[6], expected[7]);
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR,
				 "%s got[8-15]: %02x%02x%02x%02x%02x%02x%02x%02x\n", name,
				 mac[8], mac[9], mac[10], mac[11], mac[12], mac[13], mac[14], mac[15]);
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR,
				 "%s exp[8-15]: %02x%02x%02x%02x%02x%02x%02x%02x\n", name,
				 expected[8], expected[9], expected[10], expected[11],
				 expected[12], expected[13], expected[14], expected[15]);
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR,
				 "%s got[16-23]:%02x%02x%02x%02x%02x%02x%02x%02x\n", name,
				 mac[16], mac[17], mac[18], mac[19], mac[20], mac[21], mac[22], mac[23]);
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR,
				 "%s exp[16-23]:%02x%02x%02x%02x%02x%02x%02x%02x\n", name,
				 expected[16], expected[17], expected[18], expected[19],
				 expected[20], expected[21], expected[22], expected[23]);
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR,
				 "%s got[24-31]:%02x%02x%02x%02x%02x%02x%02x%02x\n", name,
				 mac[24], mac[25], mac[26], mac[27], mac[28], mac[29], mac[30], mac[31]);
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR,
				 "%s exp[24-31]:%02x%02x%02x%02x%02x%02x%02x%02x\n", name,
				 expected[24], expected[25], expected[26], expected[27],
				 expected[28], expected[29], expected[30], expected[31]);
		goto fail;
	}

	psa_destroy_key(kid);
	ST_PASS(name);
	return 0;
fail:
	psa_destroy_key(kid);
	ST_FAIL(name);
	return 1;
}

/* -----------------------------------------------------------------------
 * Cross-check: ROM hardware HMAC vs PSA hardware HMAC (32-byte key)
 *
 * Both paths should use the hardware HMAC engine.  If they agree, the
 * hardware HMAC is self-consistent.  This test needs NO RFC vector —
 * agreement between two independent code paths is the proof.
 * ---------------------------------------------------------------------- */
static int test_hmac_hw_vs_psa_crosscheck(void)
{
	static const uint8_t key32[32] = {
		0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
		0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
		0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
		0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4,
	};
	static const uint8_t data[] = {
		0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
		0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
	};

	/* --- ROM hardware HMAC path --- */
	SHA_context hw_ctx;
	uint8_t rom_mac[32];
	u8 key_id = (TrustZone_IsSecure() == 0) ? KM_KEY_NS_SW1 : KM_KEY_S_SW1;

	IPC_SEMTake(IPC_SEM_CRYPTO_AES_SW_KEY, 0xffffffff);
	crypto_km_set_sw_key(key_id, 256, key32);
	crypto_hmac_sha2_init(&hw_ctx, SHA_256, key_id, SHA_HMAC_KEY_BIT_256);
	crypto_hmac_sha2_update(&hw_ctx, data, NULL, sizeof(data));
	crypto_hmac_sha2_final(&hw_ctx, rom_mac);
	IPC_SEMFree(IPC_SEM_CRYPTO_AES_SW_KEY);

	/* --- PSA HMAC path (32-byte key → also uses RTK hardware HMAC) --- */
	psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
	mbedtls_svc_key_id_t kid = MBEDTLS_SVC_KEY_ID_INIT;
	uint8_t psa_mac[64];
	size_t mac_len;

	psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
	psa_set_key_algorithm(&attr, PSA_ALG_HMAC(PSA_ALG_SHA_256));
	psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_MESSAGE);
	ST_PSA_CHK(psa_import_key(&attr, key32, sizeof(key32), &kid));
	ST_PSA_CHK(psa_mac_compute(kid, PSA_ALG_HMAC(PSA_ALG_SHA_256),
							   data, sizeof(data),
							   psa_mac, sizeof(psa_mac), &mac_len));
	psa_destroy_key(kid);

	/* Both must produce identical output */
	if (mac_len != 32) {
		goto fail;
	}
	ST_MEM_CHK(rom_mac, psa_mac, 32);

	ST_PASS("mac_hw_vs_psa_32byte_key");
	return 0;
fail:
	psa_destroy_key(kid);
	ST_FAIL("mac_hw_vs_psa_32byte_key");
	return 1;
}

/* -----------------------------------------------------------------------
 * Manual HMAC: directly call psa_hash_compute twice (RFC 2104) for TC2.
 * Isolates whether the bug is in PSA HMAC state machine or SHA hardware.
 *
 * If manual_result == RFC_expected → SHA hardware is correct, bug is in
 *   PSA software HMAC (psa_crypto_mac.c).
 * If manual_result != RFC_expected → SHA hardware has an issue.
 * ---------------------------------------------------------------------- */
static int test_hmac_manual_two_hash(void)
{
	/* TC2: key="Jefe" (4 bytes), data="what do ya want for nothing?" */
	static const uint8_t key[] = { 0x4a, 0x65, 0x66, 0x65 };
	static const uint8_t data[] = {
		0x77, 0x68, 0x61, 0x74, 0x20, 0x64, 0x6f, 0x20,
		0x79, 0x61, 0x20, 0x77, 0x61, 0x6e, 0x74, 0x20,
		0x66, 0x6f, 0x72, 0x20, 0x6e, 0x6f, 0x74, 0x68,
		0x69, 0x6e, 0x67, 0x3f,
	};
	/* RFC 4231 TC2 expected result (verified with Python stdlib hmac) */
	static const uint8_t expected[] = {
		0x5b, 0xdc, 0xc1, 0x46, 0xbf, 0x60, 0x75, 0x4e,
		0x6a, 0x04, 0x24, 0x26, 0x08, 0x95, 0x75, 0xc7,
		0x5a, 0x00, 0x3f, 0x08, 0x9d, 0x27, 0x39, 0x83,
		0x9d, 0xec, 0x58, 0xb9, 0x64, 0xec, 0x38, 0x43,
	};

	/* HMAC-SHA-256 block size = 64 bytes */
	uint8_t ipad_key[64], opad_key[64];
	uint8_t inner_input[64 + sizeof(data)];
	uint8_t outer_input[64 + 32];
	uint8_t inner_digest[32], manual_mac[32];
	size_t digest_len;

	/* Build ipad_key and opad_key: zero-pad key to 64 bytes, then XOR */
	memset(ipad_key, 0, 64);
	memset(opad_key, 0, 64);
	memcpy(ipad_key, key, sizeof(key));
	memcpy(opad_key, key, sizeof(key));
	for (size_t i = 0; i < 64; i++) {
		ipad_key[i] ^= 0x36;
		opad_key[i] ^= 0x5c;
	}

	/* Inner hash: H(ipad_key || data) */
	memcpy(inner_input, ipad_key, 64);
	memcpy(inner_input + 64, data, sizeof(data));
	ST_PSA_CHK(psa_hash_compute(PSA_ALG_SHA_256,
								inner_input, sizeof(inner_input),
								inner_digest, sizeof(inner_digest), &digest_len));
	if (digest_len != 32) {
		goto fail;
	}

	/* Outer hash: H(opad_key || inner_digest) */
	memcpy(outer_input, opad_key, 64);
	memcpy(outer_input + 64, inner_digest, 32);
	ST_PSA_CHK(psa_hash_compute(PSA_ALG_SHA_256,
								outer_input, sizeof(outer_input),
								manual_mac, sizeof(manual_mac), &digest_len));
	if (digest_len != 32) {
		goto fail;
	}

	/* Compare with RFC 4231 expected */
	if (memcmp(manual_mac, expected, 32) != 0) {
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR,
				 "manual got[0-7]: %02x%02x%02x%02x%02x%02x%02x%02x\n",
				 manual_mac[0], manual_mac[1], manual_mac[2], manual_mac[3],
				 manual_mac[4], manual_mac[5], manual_mac[6], manual_mac[7]);
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR,
				 "manual got[24-31]: %02x%02x%02x%02x%02x%02x%02x%02x\n",
				 manual_mac[24], manual_mac[25], manual_mac[26], manual_mac[27],
				 manual_mac[28], manual_mac[29], manual_mac[30], manual_mac[31]);
		goto fail;
	}

	ST_PASS("mac_hmac_manual_two_hash_tc2");
	return 0;
fail:
	ST_FAIL("mac_hmac_manual_two_hash_tc2");
	return 1;
}

int selftest_mac(void)
{
	int fail = 0;

	/* TC1 (20-byte key) — software HMAC path for all SHA variants */
	fail += test_hmac(PSA_ALG_HMAC(PSA_ALG_SHA_256),
					  tc1_key, sizeof(tc1_key), tc1_data, sizeof(tc1_data),
					  tc1_hmac256, sizeof(tc1_hmac256),
					  "mac_hmac_sha256_tc1");
	fail += test_hmac(PSA_ALG_HMAC(PSA_ALG_SHA_384),
					  tc1_key, sizeof(tc1_key), tc1_data, sizeof(tc1_data),
					  tc1_hmac384, sizeof(tc1_hmac384),
					  "mac_hmac_sha384_tc1");
	fail += test_hmac(PSA_ALG_HMAC(PSA_ALG_SHA_512),
					  tc1_key, sizeof(tc1_key), tc1_data, sizeof(tc1_data),
					  tc1_hmac512, sizeof(tc1_hmac512),
					  "mac_hmac_sha512_tc1");

	/* TC2 short key — PSA software HMAC path */
	fail += test_hmac(PSA_ALG_HMAC(PSA_ALG_SHA_256),
					  tc2_key, sizeof(tc2_key), tc2_data, sizeof(tc2_data),
					  tc2_hmac256, sizeof(tc2_hmac256),
					  "mac_hmac_sha256_tc2_shortkey");

	/* 16-byte key KAT — hardware HMAC path (128-bit key satisfies filter) */
	fail += test_hmac(PSA_ALG_HMAC(PSA_ALG_SHA_256),
					  hw_key16, sizeof(hw_key16), tc2_data, sizeof(tc2_data),
					  hw_hmac256, sizeof(hw_hmac256),
					  "mac_hmac_sha256_16Bkey_hw");
	fail += test_hmac(PSA_ALG_HMAC(PSA_ALG_SHA_384),
					  hw_key16, sizeof(hw_key16), tc2_data, sizeof(tc2_data),
					  hw_hmac384, sizeof(hw_hmac384),
					  "mac_hmac_sha384_16Bkey_hw");

	/* Cross-check: ROM HMAC API vs PSA (32-byte key, hardware path) */
	fail += test_hmac_hw_vs_psa_crosscheck();

	/* Manual HMAC via two psa_hash_compute calls — isolates SHA vs HMAC bug */
	fail += test_hmac_manual_two_hash();

	return fail;
}
