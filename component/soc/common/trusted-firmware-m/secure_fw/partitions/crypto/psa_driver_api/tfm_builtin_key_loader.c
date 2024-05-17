/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <string.h>
#if defined(TFM_BUILTIN_KEY_LOADER_DERIVE_KEY_USING_PSA)
#include "tfm_mbedcrypto_include.h"
#else
#include "mbedtls/hkdf.h"
#endif /* TFM_BUILTIN_KEY_LOADER_DERIVE_KEY_USING_PSA */
#include "tfm_builtin_key_loader.h"
#include "psa_manifest/pid.h"
#include "tfm_plat_crypto_keys.h"
#include "crypto_library.h"

#ifndef TFM_BUILTIN_MAX_KEY_LEN
#define TFM_BUILTIN_MAX_KEY_LEN (48)
#endif /* TFM_BUILTIN_MAX_KEY_LEN */

#ifndef TFM_BUILTIN_MAX_KEYS
#define TFM_BUILTIN_MAX_KEYS (TFM_BUILTIN_KEY_SLOT_MAX)
#endif /* TFM_BUILTIN_MAX_KEYS */

#define NUMBER_OF_ELEMENTS_OF(x) sizeof(x)/sizeof(*x)

/*!
 * \brief A structure which describes a builtin key slot
 */
struct tfm_builtin_key_t {
    uint8_t __attribute__((aligned(4))) key[TFM_BUILTIN_MAX_KEY_LEN]; /*!< Raw key material, 4-byte aligned */
    size_t key_len;                       /*!< Size of the key material held in the key buffer */
    psa_key_attributes_t attr;            /*!< Key attributes associated to the key */
    uint32_t is_loaded;                   /*!< Boolean indicating whether the slot is being used */
};

/*!
 * \brief The below array is used by the driver to store key material in secure memory, in order
 *        for the keys and relevant metadata to be accessible by the PSA Crypto core layer
 */
static struct tfm_builtin_key_t g_builtin_key_slots[TFM_BUILTIN_MAX_KEYS] = {0};

/*!
 * \brief This functions returns the slot associated to a key id interrogating the
 *        platform HAL table
 */
static psa_status_t builtin_key_get_slot(psa_key_id_t key_id, psa_drv_slot_number_t *slot_ptr)
{
    const tfm_plat_builtin_key_descriptor_t *desc_table = NULL;
    size_t number_of_keys = tfm_plat_builtin_key_get_desc_table_ptr(&desc_table);
    psa_drv_slot_number_t slot_number = TFM_BUILTIN_KEY_SLOT_MAX;

    for (size_t idx = 0; idx < number_of_keys; idx++) {
        if (desc_table[idx].key_id == key_id) {
            slot_number = desc_table[idx].slot_number;
            break;
        }
    }

    if (slot_number == TFM_BUILTIN_KEY_SLOT_MAX) {
        *slot_ptr = TFM_BUILTIN_KEY_SLOT_MAX;
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    *slot_ptr = slot_number;
    return PSA_SUCCESS;
}

/*!
 * \brief This functions returns the attributes of the key interrogating the
 *        platform HAL
 */
static psa_status_t builtin_key_get_attributes(
        struct tfm_builtin_key_t *key_slot, int32_t user, psa_key_id_t key_id, psa_key_attributes_t *attr)
{
    psa_key_usage_t usage = 0x0;

    /* Retrieve the usage policy based on the key_id and the user of the key */
    const tfm_plat_builtin_key_policy_t *policy_table = NULL;
    size_t number_of_keys = tfm_plat_builtin_key_get_policy_table_ptr(&policy_table);

    for (size_t idx = 0; idx < number_of_keys; idx++) {
        if (policy_table[idx].key_id == key_id) {
            if (policy_table[idx].per_user_policy == 0) {
                usage = policy_table[idx].usage;
            } else {
                /* The policy depedends also on the user of the key */
                size_t num_users = policy_table[idx].per_user_policy;
                const tfm_plat_builtin_key_per_user_policy_t *p_policy = policy_table[idx].policy_ptr;

                for (size_t j = 0; j < num_users; j++) {
                    if (p_policy[j].user == user) {
                        usage = p_policy[j].usage;
                        break;
                    }
                }
            }
            break;
        }
    }

    /* A normal copy is enough to copy all the fields as there no pointers */
    memcpy(attr, &(key_slot->attr), sizeof(psa_key_attributes_t));
    /* The stored attributes have an owner == 0, but we need to preserve the
     * user received from the caller, which is stored in user_id
     */
    psa_set_key_id(attr, tfm_crypto_library_key_id_init(user, key_id));
    /* Set the flags according to the policy for the user for this key_id */
    psa_set_key_usage_flags(attr, usage);

    return PSA_SUCCESS;
}

/*!
 * \brief This function derives a key into a provided buffer, to make sure that
 *        keys that are returned for usage to the PSA Crypto core, are differentiated
 *        based on the partition user. The derived keys are described as platform keys
 */
#if defined(TFM_BUILTIN_KEY_LOADER_DERIVE_KEY_USING_PSA)
static psa_status_t derive_subkey_into_buffer(
        struct tfm_builtin_key_t *key_slot, int32_t user,
        uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length)
{
#ifdef TFM_PARTITION_TEST_PS
    /* Hack to allow the PS tests to work, since they directly call
     * ps_system_prepare from the test partition which would otherwise derive a
     * different key.
     */
    if (user == TFM_SP_PS_TEST) {
        user = TFM_SP_PS;
    }
#endif /* TFM_PARTITION_TEST_PS */

    psa_status_t status;
    tfm_crypto_library_key_id_t output_key_id_local = tfm_crypto_library_key_id_init_default();
    tfm_crypto_library_key_id_t builtin_key = psa_get_key_id(&key_slot->attr);
    tfm_crypto_library_key_id_t input_key_id_local = tfm_crypto_library_key_id_init(
        TFM_SP_CRYPTO, CRYPTO_LIBRARY_GET_KEY_ID(builtin_key));
    psa_key_derivation_operation_t deriv_ops = psa_key_derivation_operation_init();
    psa_key_attributes_t output_key_attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t input_key_attr = PSA_KEY_ATTRIBUTES_INIT;

    /* Set properties for the output key */
    psa_set_key_type(&output_key_attr, PSA_KEY_TYPE_RAW_DATA);
    psa_set_key_bits(&output_key_attr, PSA_BYTES_TO_BITS(key_buffer_size));
    psa_set_key_usage_flags(&output_key_attr, PSA_KEY_USAGE_EXPORT);

    /* Import the key material as a volatiile key */
    psa_set_key_usage_flags(&input_key_attr, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&input_key_attr, PSA_ALG_HKDF(PSA_ALG_SHA_256));
    psa_set_key_type(&input_key_attr, PSA_KEY_TYPE_DERIVE);
    status = psa_import_key(&input_key_attr, key_slot->key, key_slot->key_len, &input_key_id_local);
    if (status != PSA_SUCCESS) {
        goto wrap_up;
    }

    status = psa_key_derivation_setup(&deriv_ops, PSA_ALG_HKDF(PSA_ALG_SHA_256));
    if (status != PSA_SUCCESS) {
        goto wrap_up;
    }

    /* No salt is being used for the derivation */

    status = psa_key_derivation_input_key(
                    &deriv_ops, PSA_KEY_DERIVATION_INPUT_SECRET, input_key_id_local);
    if (status != PSA_SUCCESS) {
        goto wrap_up;
    }

    status = psa_key_derivation_input_bytes(
                    &deriv_ops, PSA_KEY_DERIVATION_INPUT_INFO, (uint8_t *)&user, sizeof(user));
    if (status != PSA_SUCCESS) {
        goto wrap_up;
    }

    status = psa_key_derivation_output_key(&output_key_attr, &deriv_ops,
                                           &output_key_id_local);
    if (status != PSA_SUCCESS) {
        goto wrap_up;
    }

    status = psa_export_key(output_key_id_local, key_buffer, key_buffer_size, key_buffer_length);
    if (status != PSA_SUCCESS) {
        goto wrap_up;
    }

wrap_up:
    (void)psa_key_derivation_abort(&deriv_ops);
    (void)psa_destroy_key(input_key_id_local);
    (void)psa_destroy_key(output_key_id_local);

    return status;
}
#else
static psa_status_t derive_subkey_into_buffer(
        struct tfm_builtin_key_t *key_slot, int32_t user,
        uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length)
{
    int mbedtls_err;

#ifdef TFM_PARTITION_TEST_PS
    /* Hack to allow the PS tests to work, since they directly call
     * ps_system_prepare from the test partition which would otherwise derive a
     * different key.
     */
    if (user == TFM_SP_PS_TEST) {
        user = TFM_SP_PS;
    }
#endif /* TFM_PARTITION_TEST_PS */

    /* FIXME this should be moved to using the PSA APIs once key derivation is
     * implemented in the PSA driver wrapper. Using the external PSA apis
     * directly creates a keyslot and we'd need to read the data from it and
     * then destroy it, so isn't ideal. In order to avoid infinite recursion,
     * it'll be necessary to add a special case (probably if owner == 0) to make
     * sure the new PSA derivation request doesn't end up back here.
     */
    mbedtls_err = mbedtls_hkdf(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
                               NULL, 0, key_slot->key, key_slot->key_len,
                               (uint8_t *)&user, sizeof(user), key_buffer,
                               key_buffer_size);
    if (mbedtls_err) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    *key_buffer_length = key_buffer_size;

    return PSA_SUCCESS;
}
#endif /* TFM_BUILTIN_KEY_LOADER_DERIVE_KEY_USING_PSA */

static psa_status_t builtin_key_copy_to_buffer(
        struct tfm_builtin_key_t *key_slot, uint8_t *key_buffer,
        size_t key_buffer_size, size_t *key_buffer_length)
{
    memcpy(key_buffer, key_slot->key, key_slot->key_len);
    *key_buffer_length = key_slot->key_len;

    return PSA_SUCCESS;
}

/*!
 * \defgroup tfm_builtin_key_loader
 *
 */
/*!@{*/
psa_status_t tfm_builtin_key_loader_init(void)
{
    psa_status_t err = PSA_ERROR_CORRUPTION_DETECTED;
    const tfm_plat_builtin_key_descriptor_t *desc_table = NULL;
    size_t number_of_keys = tfm_plat_builtin_key_get_desc_table_ptr(&desc_table);

    /* These properties and key material are filled by the loaders */
    uint8_t buf[TFM_BUILTIN_MAX_KEY_LEN];
    size_t key_len;
    size_t key_bits;
    psa_algorithm_t algorithm;
    psa_key_type_t type;

    for (size_t key = 0; key < number_of_keys; key++) {
        if (desc_table[key].lifetime != TFM_BUILTIN_KEY_LOADER_LIFETIME) {
            /* If the key is not bound to this driver, just don't load it */
            continue;
        }
        psa_drv_slot_number_t slot_number = desc_table[key].slot_number;
        /* The owner of a builtin key is set to 0 */
        tfm_crypto_library_key_id_t key_id = tfm_crypto_library_key_id_init(0, desc_table[key].key_id);
        psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
        enum tfm_plat_err_t plat_err = desc_table[key].loader_key_func(
                                    &buf[0], TFM_BUILTIN_MAX_KEY_LEN, &key_len, &key_bits, &algorithm, &type);
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            err = PSA_ERROR_HARDWARE_FAILURE;
            goto wrap_up;
        }

        /* Build the attributes with the metadata retrieved from the platform and desc table */
        psa_set_key_id(&attr, key_id);
        psa_set_key_bits(&attr, key_bits);
        psa_set_key_algorithm(&attr, algorithm);
        psa_set_key_type(&attr, type);
        psa_set_key_lifetime(&attr, desc_table[key].lifetime);

        /* Populate the internal table of the tfm_builtin_key_loader driver with key and metadata */
        memcpy(&(g_builtin_key_slots[slot_number].attr), &attr, sizeof(psa_key_attributes_t));
        memcpy(&(g_builtin_key_slots[slot_number].key), buf, key_len);
        g_builtin_key_slots[slot_number].key_len = key_len;
        g_builtin_key_slots[slot_number].is_loaded = 1;
    }
    /* At this point the discovered keys have been loaded successfully into the driver */
    err = PSA_SUCCESS;

wrap_up:
    return err;
}

psa_status_t tfm_builtin_key_loader_get_key_buffer_size(
        tfm_crypto_library_key_id_t key_id, size_t *len)
{
    psa_status_t err;
    psa_drv_slot_number_t slot_number;

    if (len == NULL) {
        err = PSA_ERROR_INVALID_ARGUMENT;
        goto wrap_up;
    }

    *len = 0;

    err = builtin_key_get_slot(CRYPTO_LIBRARY_GET_KEY_ID(key_id), &slot_number);
    if (err != PSA_SUCCESS) {
        goto wrap_up;
    }

    *len = g_builtin_key_slots[slot_number].key_len;
    err = PSA_SUCCESS;

wrap_up:
    return err;
}

psa_status_t tfm_builtin_key_loader_get_builtin_key(
        psa_drv_slot_number_t slot_number, psa_key_attributes_t *attributes,
        uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length)
{
    psa_status_t err;
    struct tfm_builtin_key_t *key_slot;
    tfm_crypto_library_key_id_t key_id;

    if ((uint32_t)slot_number >= NUMBER_OF_ELEMENTS_OF(g_builtin_key_slots)) {
        err = PSA_ERROR_INVALID_ARGUMENT;
        goto wrap_up;
    }

    key_slot = &g_builtin_key_slots[slot_number];

    /* The request is for a valid slot that has not been loaded*/
    if (!key_slot->is_loaded) {
        err = PSA_ERROR_DOES_NOT_EXIST;
        goto wrap_up;
    }

    if (attributes == NULL) {
        err = PSA_ERROR_INVALID_ARGUMENT;
        goto wrap_up;
    }

    key_id = psa_get_key_id(attributes);
    err = builtin_key_get_attributes(key_slot,
                                     CRYPTO_LIBRARY_GET_OWNER(key_id),
                                     CRYPTO_LIBRARY_GET_KEY_ID(key_id),
                                     attributes);
    if (err != PSA_SUCCESS) {
        goto wrap_up;
    }

    /* Note that the core layer might use this function just to probe, for a loaded
     * key, the attributes of the key. In this case, key_buffer and key_buffer_length
     * could be NULL hence the validation of those pointers must happen after this
     * check here. In fact, PSA_ERROR_BUFFER_TOO_SMALL is considered in the crypto
     * core as a safe failure value that lets the caller continue.
     */
    if (key_buffer_size < key_slot->key_len) {
        err = PSA_ERROR_BUFFER_TOO_SMALL;
        goto wrap_up;
    }

    if (key_buffer == NULL || key_buffer_length == NULL) {
        err = PSA_ERROR_INVALID_ARGUMENT;
        goto wrap_up;
    }

    /* If a key can be used for derivation, we derive a further subkey for each
     * users to prevent multiple users deriving the same keys as each other.
     * For keys for encryption and signing, it's assumed that if multiple
     * partitions have access to the key, there is a good reason and therefore
     * they all need access to the raw builtin key.
     */
    int32_t user = CRYPTO_LIBRARY_GET_OWNER(key_id);
    if (psa_get_key_usage_flags(attributes) & PSA_KEY_USAGE_DERIVE && user != TFM_SP_CRYPTO) {

        err = derive_subkey_into_buffer(key_slot, user,
                                        key_buffer, key_buffer_size,
                                        key_buffer_length);
    } else {
        err = builtin_key_copy_to_buffer(key_slot, key_buffer, key_buffer_size,
                                         key_buffer_length);
    }

wrap_up:
    return err;
}
/*!@}*/
