/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CRYPTO_KEY_H__
#define __TFM_CRYPTO_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "psa/crypto.h"

/**
 * \brief The type which describes a key identifier to the Crypto
 *        service. The key identifiers must clearly provide a
 *        dedicated indication of the entity owner which owns the key
 */
struct tfm_crypto_key_id_s {
    psa_key_id_t key_id;  /*!< Key ID for the key itself */
    int32_t owner;        /*!< ID of the entity owning the key */
};

/**
 * \brief A macro to perform static initialisation of a \struct tfm_crypto_key_id_s
 *        structure
 */
#define TFM_CRYPTO_KEY_ID_S_INIT {0, 0}

#ifdef __cplusplus
}
#endif

#endif /* __TFM_CRYPTO_KEY_H__ */
