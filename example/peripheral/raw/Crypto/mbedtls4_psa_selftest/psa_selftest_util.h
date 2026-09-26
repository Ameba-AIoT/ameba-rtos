/**
 * @file  psa_selftest_util.h
 * @brief Test helper macros for mbedTLS 4.1.1 PSA selftest.
 */

#ifndef PSA_SELFTEST_UTIL_H
#define PSA_SELFTEST_UTIL_H

#include "ameba_soc.h"
#include "psa/crypto.h"
#include <string.h>

#define ST_TAG "mbedtls4"

/* Run a PSA call; jump to 'fail' label on error. */
#define ST_PSA_CHK(expr)                                            \
    do {                                                            \
        psa_status_t _s = (expr);                                   \
        if (_s != PSA_SUCCESS) {                                    \
            RTK_LOGS(ST_TAG, RTK_LOG_ERROR,                         \
                     "%s:%d psa err %d\n", __func__, __LINE__, _s); \
            goto fail;                                              \
        }                                                           \
    } while (0)

/* Compare two byte buffers; jump to 'fail' if they differ. */
#define ST_MEM_CHK(a, b, len)                                       \
    do {                                                            \
        if (memcmp((a), (b), (len)) != 0) {                         \
            RTK_LOGS(ST_TAG, RTK_LOG_ERROR,                         \
                     "%s:%d buffer mismatch\n", __func__, __LINE__);\
            goto fail;                                              \
        }                                                           \
    } while (0)

/* Log PASS / FAIL for one named case. */
#define ST_PASS(name) \
    RTK_LOGS(ST_TAG, RTK_LOG_INFO, "[PASS] %s\n", (name))
#define ST_FAIL(name) \
    RTK_LOGS(ST_TAG, RTK_LOG_ERROR, "[FAIL] %s\n", (name))

#endif /* PSA_SELFTEST_UTIL_H */
