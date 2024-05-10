/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "tfm_plat_crypto_keys.h"
#include "tfm_builtin_key_ids.h"
#include "region_defs.h"
#include "cmsis_compiler.h"
#include "tfm_plat_otp.h"
#include "psa_manifest/pid.h"
#include "tfm_builtin_key_loader.h"
#include "kmu_drv.h"
#include "device_definition.h"
#include "tfm_plat_otp.h"

#define NUMBER_OF_ELEMENTS_OF(x) sizeof(x)/sizeof(*x)
#define TFM_NS_PARTITION_ID -1

static enum tfm_plat_err_t tfm_plat_get_huk(uint8_t *buf, size_t buf_len,
                                            size_t *key_len,
                                            size_t *key_bits,
                                            psa_algorithm_t *algorithm,
                                            psa_key_type_t *type)
{
    enum kmu_error_t kmu_err;

    if (buf_len < 32) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    *key_len = 32;
    *key_bits = 256;
    *algorithm = PSA_ALG_HKDF(PSA_ALG_SHA_256);
    *type = PSA_KEY_TYPE_DERIVE;

    kmu_err = kmu_get_key(&KMU_DEV_S, KMU_USER_SLOT_MIN + 0, buf, 32);
    if (kmu_err != KMU_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t tfm_plat_get_iak(uint8_t *buf, size_t buf_len,
                                     size_t *key_len,
                                     size_t *key_bits,
                                     psa_algorithm_t *algorithm,
                                     psa_key_type_t *type)
{
    psa_status_t status;
    enum kmu_error_t kmu_err;
    psa_key_attributes_t seed_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t transient_attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_handle_t seed_key = PSA_KEY_HANDLE_INIT;
    psa_key_handle_t transient_key = PSA_KEY_HANDLE_INIT;
    psa_key_derivation_operation_t op = PSA_KEY_DERIVATION_OPERATION_INIT;

    if (buf_len < PSA_KEY_EXPORT_ECC_KEY_PAIR_MAX_SIZE(ATTEST_KEY_BITS)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    kmu_err = kmu_get_key(&KMU_DEV_S, KMU_USER_SLOT_MIN + 1, buf, 32);
    if (kmu_err != KMU_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    psa_set_key_type(&seed_attributes, PSA_KEY_TYPE_DERIVE);
    psa_set_key_algorithm(&seed_attributes, PSA_ALG_HKDF(PSA_ALG_SHA_256));
    psa_set_key_bits(&seed_attributes, 256);
    psa_set_key_usage_flags(&seed_attributes, PSA_KEY_USAGE_DERIVE);

    status = psa_import_key(&seed_attributes, buf, 32, &seed_key);
    if (status != PSA_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    status = psa_key_derivation_setup(&op, PSA_ALG_HKDF(PSA_ALG_SHA_256));
    if (status != PSA_SUCCESS) {
        goto err_release_seed_key;
    }

    status = psa_key_derivation_input_key(&op, PSA_KEY_DERIVATION_INPUT_SECRET,
                                          seed_key);
    if (status != PSA_SUCCESS) {
        goto err_release_op;
    }

    status = psa_key_derivation_input_bytes(&op, PSA_KEY_DERIVATION_INPUT_INFO,
                                            NULL, 0);
    if (status != PSA_SUCCESS) {
        goto err_release_op;
    }

    *key_bits = ATTEST_KEY_BITS;
    *algorithm = PSA_ALG_ECDSA(PSA_ALG_SHA_384);
    *type = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);

    psa_set_key_type(&transient_attr, *type);
    psa_set_key_algorithm(&transient_attr, *algorithm);
    psa_set_key_bits(&transient_attr, *key_bits);
    psa_set_key_usage_flags(&transient_attr, PSA_KEY_USAGE_EXPORT);

    status = psa_key_derivation_output_key(&transient_attr, &op, &transient_key);
    if (status != PSA_SUCCESS) {
        goto err_release_op;
    }

    status = psa_export_key(transient_key, buf, buf_len, key_len);
    if (status != PSA_SUCCESS) {
        goto err_release_transient_key;
    }

    status = psa_destroy_key(transient_key);
    if (status != PSA_SUCCESS) {
        goto err_release_op;
    }

    status = psa_key_derivation_abort(&op);
    if (status != PSA_SUCCESS) {
        goto err_release_seed_key;
    }

    status = psa_destroy_key(seed_key);
    if (status != PSA_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return PSA_SUCCESS;

err_release_transient_key:
    (void)psa_destroy_key(transient_key);

err_release_op:
    (void)psa_key_derivation_abort(&op);

err_release_seed_key:
    (void)psa_destroy_key(seed_key);

    return TFM_PLAT_ERR_SYSTEM_ERR;
}

#ifdef TFM_PARTITION_DELEGATED_ATTESTATION
static enum tfm_plat_err_t tfm_plat_get_dak_seed(uint8_t *buf, size_t buf_len,
                                                 size_t *key_len,
                                                 size_t *key_bits,
                                                 psa_algorithm_t *algorithm,
                                                 psa_key_type_t *type)
{
    enum kmu_error_t kmu_err;

    if (buf_len < 32) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    *key_len = 32;
    *key_bits = 256;
    *algorithm = PSA_ALG_HKDF(PSA_ALG_SHA_256);
    *type = PSA_KEY_TYPE_DERIVE;

    kmu_err = kmu_get_key(&KMU_DEV_S, KMU_USER_SLOT_MIN + 2, buf, 32);
    if (kmu_err != KMU_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}
#endif /* TFM_PARTITION_DELEGATED_ATTESTATION */

static enum tfm_plat_err_t tfm_plat_get_host_s_rotpk(uint8_t *buf, size_t buf_len,
                                                     size_t *key_len,
                                                     size_t *key_bits,
                                                     psa_algorithm_t *algorithm,
                                                     psa_key_type_t *type)
{
    if (buf_len < 96) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* P384 public keys are 96 bytes in length */
    *key_len = 96;
    *key_bits = 384;
    *algorithm = PSA_ALG_ECDSA(PSA_ALG_SHA_384);
    *type = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1);

    return tfm_plat_otp_read(PLAT_OTP_ID_HOST_ROTPK_S, buf_len, buf);
}

static enum tfm_plat_err_t tfm_plat_get_host_ns_rotpk(uint8_t *buf, size_t buf_len,
                                                      size_t *key_len,
                                                      size_t *key_bits,
                                                      psa_algorithm_t *algorithm,
                                                      psa_key_type_t *type)
{
    if (buf_len < 96) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* P384 public keys are 96 bytes in length */
    *key_len = 96;
    *key_bits = 384;
    *algorithm = PSA_ALG_ECDSA(PSA_ALG_SHA_384);
    *type = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1);

    return tfm_plat_otp_read(PLAT_OTP_ID_HOST_ROTPK_NS, buf_len, buf);
}

static enum tfm_plat_err_t tfm_plat_get_host_cca_rotpk(uint8_t *buf, size_t buf_len,
                                                       size_t *key_len,
                                                       size_t *key_bits,
                                                       psa_algorithm_t *algorithm,
                                                       psa_key_type_t *type)
{
    if (buf_len < 96) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* P384 public keys are 96 bytes in length */
    *key_len = 96;
    *key_bits = 384;
    *algorithm = PSA_ALG_ECDSA(PSA_ALG_SHA_384);
    *type = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1);

    return tfm_plat_otp_read(PLAT_OTP_ID_HOST_ROTPK_CCA, buf_len, buf);
}

/**
 * @brief Table describing per-user key policy for the IAK
 *
 */
static const tfm_plat_builtin_key_per_user_policy_t g_iak_per_user_policy[] = {
    {.user = TFM_SP_INITIAL_ATTESTATION,
#ifdef SYMMETRIC_INITIAL_ATTESTATION
        .usage = PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_EXPORT,
#else
        .usage = PSA_KEY_USAGE_SIGN_HASH,
#endif /* SYMMETRIC_INITIAL_ATTESTATION */
    },
#ifdef TEST_S_ATTESTATION
    {.user = TFM_SP_SECURE_TEST_PARTITION, .usage = PSA_KEY_USAGE_VERIFY_HASH},
#endif /* TEST_S_ATTESTATION */
#ifdef TEST_NS_ATTESTATION
    {.user = TFM_NS_PARTITION_ID, .usage = PSA_KEY_USAGE_VERIFY_HASH},
#endif /* TEST_NS_ATTESTATION */
};

#ifdef TFM_PARTITION_DELEGATED_ATTESTATION
/**
 * @brief Table describing per-user key policy for the DAK seed
 *
 */
static const tfm_plat_builtin_key_per_user_policy_t g_dak_seed_per_user_policy[] = {
    {.user = TFM_SP_DELEGATED_ATTESTATION, .usage = PSA_KEY_USAGE_DERIVE},
};
#endif /* TFM_PARTITION_DELEGATED_ATTESTATION */

/**
 * @brief Table describing per-user key policy for all the HOST RoTPK (S, NS, CCA)
 *
 */
static const tfm_plat_builtin_key_per_user_policy_t g_host_rotpk_per_user_policy[] = {
    {.user = TFM_NS_PARTITION_ID, .usage = PSA_KEY_USAGE_VERIFY_HASH},
};

/**
 * @brief Table describing per-key user policies
 *
 */
static const tfm_plat_builtin_key_policy_t g_builtin_keys_policy[] = {
    {.key_id = TFM_BUILTIN_KEY_ID_HUK, .per_user_policy = 0, .usage = PSA_KEY_USAGE_DERIVE},
    {.key_id = TFM_BUILTIN_KEY_ID_IAK,
     .per_user_policy = NUMBER_OF_ELEMENTS_OF(g_iak_per_user_policy),
     .policy_ptr = g_iak_per_user_policy},
#ifdef TFM_PARTITION_DELEGATED_ATTESTATION
    {.key_id = TFM_BUILTIN_KEY_ID_DAK_SEED,
     .per_user_policy = NUMBER_OF_ELEMENTS_OF(g_dak_seed_per_user_policy),
     .policy_ptr = g_dak_seed_per_user_policy},
#endif /* TFM_PARTITION_DELEGATED_ATTESTATION */
    {.key_id = TFM_BUILTIN_KEY_ID_HOST_S_ROTPK,
     .per_user_policy = NUMBER_OF_ELEMENTS_OF(g_host_rotpk_per_user_policy),
     .policy_ptr = g_host_rotpk_per_user_policy},
    {.key_id = TFM_BUILTIN_KEY_ID_HOST_NS_ROTPK,
     .per_user_policy = NUMBER_OF_ELEMENTS_OF(g_host_rotpk_per_user_policy),
     .policy_ptr = g_host_rotpk_per_user_policy},
    {.key_id = TFM_BUILTIN_KEY_ID_HOST_CCA_ROTPK,
     .per_user_policy = NUMBER_OF_ELEMENTS_OF(g_host_rotpk_per_user_policy),
     .policy_ptr = g_host_rotpk_per_user_policy},
};

/**
 * @brief Table describing the builtin-in keys (plaform keys) available in the platform. Note
 *        that to bind the keys to the tfm_builtin_key_loader driver, the lifetime must be
 *        explicitly set to the one associated to the driver, i.e. TFM_BUILTIN_KEY_LOADER_LIFETIME
 */
static const tfm_plat_builtin_key_descriptor_t g_builtin_keys_desc[] = {
    {.key_id = TFM_BUILTIN_KEY_ID_HUK,
     .slot_number = TFM_BUILTIN_KEY_SLOT_HUK,
     .lifetime = TFM_BUILTIN_KEY_LOADER_LIFETIME,
     .loader_key_func = tfm_plat_get_huk},
    {.key_id = TFM_BUILTIN_KEY_ID_IAK,
     .slot_number = TFM_BUILTIN_KEY_SLOT_IAK,
     .lifetime = TFM_BUILTIN_KEY_LOADER_LIFETIME,
     .loader_key_func = tfm_plat_get_iak},
#ifdef TFM_PARTITION_DELEGATED_ATTESTATION
    {.key_id = TFM_BUILTIN_KEY_ID_DAK_SEED,
     .slot_number = TFM_BUILTIN_KEY_SLOT_DAK_SEED,
     .lifetime = TFM_BUILTIN_KEY_LOADER_LIFETIME,
     .loader_key_func = tfm_plat_get_dak_seed},
#endif /* TFM_PARTITION_DELEGATED_ATTESTATION */
    {.key_id = TFM_BUILTIN_KEY_ID_HOST_S_ROTPK,
     .slot_number = TFM_BUILTIN_KEY_SLOT_HOST_S_ROTPK,
     .lifetime = TFM_BUILTIN_KEY_LOADER_LIFETIME,
     .loader_key_func = tfm_plat_get_host_s_rotpk},
    {.key_id = TFM_BUILTIN_KEY_ID_HOST_NS_ROTPK,
     .slot_number = TFM_BUILTIN_KEY_SLOT_HOST_NS_ROTPK,
     .lifetime = TFM_BUILTIN_KEY_LOADER_LIFETIME,
     .loader_key_func = tfm_plat_get_host_ns_rotpk},
    {.key_id = TFM_BUILTIN_KEY_ID_HOST_CCA_ROTPK,
     .slot_number = TFM_BUILTIN_KEY_SLOT_HOST_CCA_ROTPK,
     .lifetime = TFM_BUILTIN_KEY_LOADER_LIFETIME,
     .loader_key_func = tfm_plat_get_host_cca_rotpk},
};

size_t tfm_plat_builtin_key_get_policy_table_ptr(const tfm_plat_builtin_key_policy_t *desc_ptr[])
{
    *desc_ptr = &g_builtin_keys_policy[0];
    return NUMBER_OF_ELEMENTS_OF(g_builtin_keys_policy);
}

size_t tfm_plat_builtin_key_get_desc_table_ptr(const tfm_plat_builtin_key_descriptor_t *desc_ptr[])
{
    *desc_ptr = &g_builtin_keys_desc[0];
    return NUMBER_OF_ELEMENTS_OF(g_builtin_keys_desc);
}
