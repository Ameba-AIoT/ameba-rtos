/*
 * Copyright (c) 2017-2023 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include "tfm_plat_crypto_keys.h"
#include "tfm_builtin_key_ids.h"
#include "tfm_plat_otp.h"
#include "psa_manifest/pid.h"
#include "tfm_builtin_key_loader.h"

#define NUMBER_OF_ELEMENTS_OF(x) sizeof(x)/sizeof(*x)
#define TFM_NS_PARTITION_ID -1

static enum tfm_plat_err_t tfm_plat_get_huk(uint8_t *buf, size_t buf_len,
                                            size_t *key_len,
                                            size_t *key_bits,
                                            psa_algorithm_t *algorithm,
                                            psa_key_type_t *type)
{
    enum tfm_plat_err_t err;

    err = tfm_plat_otp_read(PLAT_OTP_ID_HUK, buf_len, buf);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = tfm_plat_otp_get_size(PLAT_OTP_ID_HUK, key_len);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    *key_bits = *key_len * 8;
    *algorithm = PSA_ALG_HKDF(PSA_ALG_SHA_256);
    *type = PSA_KEY_TYPE_DERIVE;

    return TFM_PLAT_ERR_SUCCESS;
}

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
static enum tfm_plat_err_t tfm_plat_get_iak(uint8_t *buf, size_t buf_len,
                                            size_t *key_len,
                                            size_t *key_bits,
                                            psa_algorithm_t *algorithm,
                                            psa_key_type_t *type)
{
    enum tfm_plat_err_t err;
#ifndef SYMMETRIC_INITIAL_ATTESTATION
    psa_ecc_family_t curve_type;
#endif /* SYMMETRIC_INITIAL_ATTESTATION */

    err = tfm_plat_otp_read(PLAT_OTP_ID_IAK_LEN,
                            sizeof(size_t), (uint8_t*)key_len);
    if(err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    *key_bits = *key_len * 8;

    if (buf_len < *key_len) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

#ifdef SYMMETRIC_INITIAL_ATTESTATION
    err = tfm_plat_otp_read(PLAT_OTP_ID_IAK_TYPE,
                            sizeof(psa_algorithm_t), (uint8_t*)algorithm);
    if(err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    *type = PSA_KEY_TYPE_HMAC;
#else /* SYMMETRIC_INITIAL_ATTESTATION */
    err = tfm_plat_otp_read(PLAT_OTP_ID_IAK_TYPE, sizeof(psa_ecc_family_t),
                            &curve_type);
    if(err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    *algorithm = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
    *type = PSA_KEY_TYPE_ECC_KEY_PAIR(curve_type);
#endif /* SYMMETRIC_INITIAL_ATTESTATION */

    return tfm_plat_otp_read(PLAT_OTP_ID_IAK, *key_len, buf);
}
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
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
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

/**
 * @brief Table describing per-key user policies
 *
 */
static const tfm_plat_builtin_key_policy_t g_builtin_keys_policy[] = {
    {.key_id = TFM_BUILTIN_KEY_ID_HUK, .per_user_policy = 0, .usage = PSA_KEY_USAGE_DERIVE},
#ifdef TFM_PARTITION_INITIAL_ATTESTATION
    {.key_id = TFM_BUILTIN_KEY_ID_IAK,
     .per_user_policy = NUMBER_OF_ELEMENTS_OF(g_iak_per_user_policy),
     .policy_ptr = g_iak_per_user_policy},
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */
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
#ifdef TFM_PARTITION_INITIAL_ATTESTATION
    {.key_id = TFM_BUILTIN_KEY_ID_IAK,
     .slot_number = TFM_BUILTIN_KEY_SLOT_IAK,
     .lifetime = TFM_BUILTIN_KEY_LOADER_LIFETIME,
     .loader_key_func = tfm_plat_get_iak},
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */
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
