/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file crypto_library.h
 *
 * \brief This file contains some abstractions required to interface the
 *        TF-M Crypto service to an underlying cryptographic library that
 *        implements the PSA Crypto API. The TF-M Crypto service uses this
 *        library to provide a PSA Crypto core layer implementation and
 *        a software or hardware based implementation of crypto algorithms.
 */

#ifndef CRYPTO_LIBRARY_H
#define CRYPTO_LIBRARY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"
#include "psa/crypto_client_struct.h"

/**
 * \brief This macro extracts the key ID from the library encoded key passed as parameter
 *
 */
#define CRYPTO_LIBRARY_GET_KEY_ID(encoded_key_library) MBEDTLS_SVC_KEY_ID_GET_KEY_ID(encoded_key_library)

/**
 * \brief This macro extracts the owner from the library encoded key passed as parameter
 *
 */
#define CRYPTO_LIBRARY_GET_OWNER(encoded_key_library) MBEDTLS_SVC_KEY_ID_GET_OWNER_ID(encoded_key_library)

/**
 * \brief The following typedef must be defined to the type associated to the key_id in the underlying library
 *
 */
typedef mbedtls_svc_key_id_t tfm_crypto_library_key_id_t;

/**
 * \brief Function used to initialise an object of \ref tfm_crypto_library_key_id_t to a (owner, key_id) pair
 *
 * \param[in] owner   Owner of the key
 * \param[in] key_id  key ID associated to the key of type \ref psa_key_id_t
 *
 * \return An object of type \ref tfm_crypto_library_key_id_t
 *
 */
tfm_crypto_library_key_id_t tfm_crypto_library_key_id_init(int32_t owner, psa_key_id_t key_id);

/**
 * \brief This function is used to retrieve a string describing the library used in the backend
 *        to provide information to the crypto service and the user
 *
 * \return A NULL terminated string describing the backend library
 */
char *tfm_crypto_library_get_info(void);

/**
 * \brief This function initialises a \ref tfm_crypto_library_key_id_t with default values
 *
 */
static inline tfm_crypto_library_key_id_t tfm_crypto_library_key_id_init_default(void)
{
    return tfm_crypto_library_key_id_init(0, 0);
}

/*!
 * \brief This function is used to perform the necessary steps to initialise the underlying
 *        library that provides the implementation of the PSA Crypto core to the TF-M Crypto
 *        service
 *
 * \return PSA_SUCCESS on successful initialisation
 */
psa_status_t tfm_crypto_core_library_init(void);

/**
 * \brief Gets key attributes for the underlying PSA crypto core implemented by
 *        the available crypto library, from client key attributes.
 *
 * \param[in]  client_key_attr  Client key attributes
 * \param[in]  client_id        Partition ID of the calling client
 * \param[out] key_attributes   Key attributes
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_core_library_key_attributes_from_client(
                    const struct psa_client_key_attributes_s *client_key_attr,
                    int32_t client_id,
                    psa_key_attributes_t *key_attributes);

/**
 * \brief Converts key attributes of the underlying cryptographic library,
 *        to the attributes exposed to the client (i.e. client key attributes).
 *
 * \param[in]  key_attributes   Key attributes
 * \param[out] client_key_attr  Client key attributes
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_core_library_key_attributes_to_client(
                        const psa_key_attributes_t *key_attributes,
                        struct psa_client_key_attributes_s *client_key_attr);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_LIBRARY_H */
