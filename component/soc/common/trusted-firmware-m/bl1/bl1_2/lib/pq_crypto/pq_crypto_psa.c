/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "pq_crypto.h"
#include "crypto.h"
#include "mbedtls/lms.h"
#include "otp.h"
#include "psa/crypto.h"

psa_status_t psa_hash_setup(
    psa_hash_operation_t *operation,
    psa_algorithm_t alg)
{
    (void)operation;
    (void)alg;

    return fih_int_decode(bl1_sha256_init());
}

psa_status_t psa_hash_update(
    psa_hash_operation_t *operation,
    const uint8_t *input,
    size_t input_length)
{
    (void)operation;

    return fih_int_decode(bl1_sha256_update((unsigned char *)input, input_length));
}

psa_status_t psa_hash_finish(
    psa_hash_operation_t *operation,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length)
{
    (void)operation;
    (void)hash_size;

    *hash_length = 32;
    return fih_int_decode(bl1_sha256_finish(hash));
}

psa_status_t psa_hash_abort(
    psa_hash_operation_t *operation)
{
    (void)operation;

    return PSA_SUCCESS;
}

fih_int pq_crypto_verify(enum tfm_bl1_key_id_t key,
                         const uint8_t *data,
                         size_t data_length,
                         const uint8_t *signature,
                         size_t signature_length)
{
    int rc;
    fih_int fih_rc;
    mbedtls_lms_public_t ctx;
    uint8_t key_buf[MBEDTLS_LMS_PUBLIC_KEY_LEN(MBEDTLS_LMS_SHA256_M32_H10)];

    FIH_CALL(bl1_otp_read_key, fih_rc, key, key_buf);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        FIH_RET(FIH_FAILURE);
    }

    mbedtls_lms_public_init(&ctx);

    rc = mbedtls_lms_import_public_key(&ctx, key_buf, MBEDTLS_LMS_PUBLIC_KEY_LEN(MBEDTLS_LMS_SHA256_M32_H10));
    fih_rc = fih_int_encode_zero_equality(rc);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        fih_rc = FIH_FAILURE;
        goto out;
    }

    rc = mbedtls_lms_verify(&ctx, data, data_length, signature, signature_length);
    fih_rc = fih_int_encode_zero_equality(rc);

out:
    mbedtls_lms_public_free(&ctx);
    FIH_RET(fih_rc);
}
