/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TFM_BUILTIN_KEY_LOADER_H
#define TFM_BUILTIN_KEY_LOADER_H

#include <psa/crypto.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PLATFORM_DEFAULT_CRYPTO_KEYS
enum psa_drv_slot_number_t {
    TFM_BUILTIN_KEY_SLOT_HUK = 0,
    TFM_BUILTIN_KEY_SLOT_IAK,
#ifdef TFM_PARTITION_DELEGATED_ATTESTATION
    TFM_BUILTIN_KEY_SLOT_DAK_SEED,
#endif /* TFM_PARTITION_DELEGATED_ATTESTATION */
    TFM_BUILTIN_KEY_SLOT_MAX,
};
#else
#include "platform_builtin_key_loader_ids.h"
#endif

#ifdef __DOXYGEN_ONLY__
/**
 *  \brief Enables the tfm_builtin_key_loader driver in the PSA Crypto
 *         core subsystem
 */
#define PSA_CRYPTO_DRIVER_TFM_BUILTIN_KEY_LOADER

/**
 * \brief Configures the tfm_builtin_key_loader driver to perform the key
 *        derivation required to produce platform keys using PSA Crypto APIs
 */
#define TFM_BUILTIN_KEY_LOADER_DERIVE_KEY_USING_PSA
#endif /* __DOXYGEN_ONLY__ */

/**
 * \brief The PSA driver location for TF-M builtin keys. Arbitrary within the
 *        ranges documented at
 *        https://armmbed.github.io/mbed-crypto/html/api/keys/lifetimes.html#c.psa_key_location_t
 */
#define TFM_BUILTIN_KEY_LOADER_KEY_LOCATION ((psa_key_location_t)0x800001)

/**
 * \brief This macro defines the lifetime associated to TF-M builtin keys as
 *        persistent and as an ad-hoc location associated to the TFM_BUILTIN_KEY_LOADER
 *        driver. To be handled by the tfm_builtin_ker_loader driver, the lifetime of
 *        the platform keys must be set equal to this particular lifetime value
 */
#define TFM_BUILTIN_KEY_LOADER_LIFETIME PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION( \
                                            PSA_KEY_LIFETIME_PERSISTENT, TFM_BUILTIN_KEY_LOADER_KEY_LOCATION)

/**
 * \brief This is the initialisation function for the tfm_builtin_key_laoder driver,
 *        to be called from the PSA core initialisation subsystem. It discovers the
 *        keys available in the underlying hardware platform and loads them in
 *        memory visible to the PSA Crypto subsystem to be used to the normal APIs
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t tfm_builtin_key_loader_init(void);

/**
 * \brief Returns the length of a key from the builtin driver.
 *
 * \note This function is called by the psa crypto driver wrapper.
 *
 * \param[in]  key_id  The ID of the key to return the length of. The type of this
 *                     must match the expected type of the underlying library that
 *                     provides the key management for the PSA Crypto core, and
 *                     must support encoding the owner in addition to the key_id.
 * \param[out] len     The length of the key.
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t tfm_builtin_key_loader_get_key_buffer_size(
        mbedtls_svc_key_id_t key_id, size_t *len);

/**
 * \brief Returns the attributes and key material of a key from the builtin
 *        driver to be used by the PSA Crypto core
 *
 * \note This function is called by the psa crypto driver wrapper.
 *
 * \param[in] slot_number        The slot of the key
 * \param[out] attributes        The attributes of the key.
 * \param[out] key_buffer        The buffer to output the key material into.
 * \param[in] key_buffer_size    The size of the key material buffer.
 * \param[out] key_buffer_length The length of the key material returned.
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t tfm_builtin_key_loader_get_builtin_key(
        psa_drv_slot_number_t slot_number, psa_key_attributes_t *attributes,
        uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length);

#ifdef __cplusplus
}
#endif

#endif /* TFM_BUILTIN_KEY_LOADER_H */
