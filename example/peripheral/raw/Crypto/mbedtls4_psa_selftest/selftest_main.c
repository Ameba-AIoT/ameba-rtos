/**
 * @file  selftest_main.c
 * @brief mbedTLS 4.1.1 PSA driver selftest entry point.
 *
 * Runs all algorithm modules and prints a summary.
 * Expected serial output on all-pass:
 *   [mbedtls4] === mbedTLS 4.1.1 PSA selftest ===
 *   [mbedtls4] [PASS] hash_sha256_kat
 *   ...
 *   [mbedtls4] === RESULT: 13/13 PASS ===
 */

#include "psa_selftest.h"
#include "psa_selftest_util.h"
#include "os_wrapper.h"

static void selftest_task(void *param)
{
	(void)param;

	int total = 0, fail = 0;
	int n;

	RTK_LOGS(ST_TAG, RTK_LOG_INFO,
			 "=== mbedTLS 4.1.1 PSA selftest ===\n");

	/* Count total expected cases and accumulate failures.
	 * Each selftest_xxx() returns the number of failed sub-cases. */

#define RUN(fn, expected_count) \
    do { n = (fn)(); fail += n; total += (expected_count); } while (0)

	RUN(selftest_hash,   4);   /* sha256/384/512 kat + sha256 multipart */
	RUN(selftest_cipher, 5);   /* ecb kat, cbc kat, cbc mp 5-11-16,
                                  cbc mp 1byte, ctr roundtrip */
	RUN(selftest_aead,   3);   /* gcm kat, gcm tamper, ccm roundtrip */
	RUN(selftest_mac,    3);   /* hmac-256/384/512 kat */
	RUN(selftest_ecdsa,  3);   /* p256 rt, p384 rt, wrong-key */
	RUN(selftest_ecdh,   2);   /* p256, p384 shared secret */

#undef RUN

	int pass = total - fail;
	if (fail == 0) {
		RTK_LOGS(ST_TAG, RTK_LOG_INFO,
				 "=== RESULT: %d/%d PASS ===\n", pass, total);
	} else {
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR,
				 "=== RESULT: %d/%d PASS, %d FAIL ===\n",
				 pass, total, fail);
	}

	rtos_task_delete(NULL);
}

void mbedtls4_psa_selftest_example(void)
{
	rtos_task_create(NULL, "selftest", selftest_task,
					 NULL, 8192, 5);
}
