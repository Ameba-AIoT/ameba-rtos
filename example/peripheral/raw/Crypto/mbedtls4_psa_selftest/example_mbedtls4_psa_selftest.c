/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Entry stub — the real work is in selftest_main.c.
 */

#include "psa_selftest.h"
#include "psa_selftest_util.h"
#include "os_wrapper.h"

static void selftest_task(void *param)
{
	(void)param;

	int total = 0, fail = 0, n;

	RTK_LOGS(ST_TAG, RTK_LOG_INFO, "=== mbedTLS 4.1.1 PSA selftest ===\n");

#define RUN(fn, cnt) do { n = (fn)(); fail += n; total += (cnt); } while (0)
	RUN(selftest_hash,   5);   /* sha224/256/384/512 kat + sha256 multipart */
	RUN(selftest_cipher, 8);   /* ecb/ecb256/ecb-mp/cbc/cbc256/cbc-mp×2/ctr */
	RUN(selftest_aead,   4);   /* gcm kat + tamper + gcm multipart + ccm */
	RUN(selftest_mac,    8);   /* tc1×3 + tc2 + hw16B×2 + hw_vs_psa + manual */
	RUN(selftest_ecdsa,  3);
	RUN(selftest_ecdh,   2);
#undef RUN

	if (fail == 0) {
		RTK_LOGS(ST_TAG, RTK_LOG_INFO,
				 "=== RESULT: %d/%d PASS ===\n", total, total);
	} else {
		RTK_LOGS(ST_TAG, RTK_LOG_ERROR,
				 "=== RESULT: %d/%d PASS, %d FAIL ===\n",
				 total - fail, total, fail);
	}

	rtos_task_delete(NULL);
}

void mbedtls4_psa_selftest_example(void)
{
	rtos_task_create(NULL, "selftest", selftest_task, NULL, 8192, 5);
}
