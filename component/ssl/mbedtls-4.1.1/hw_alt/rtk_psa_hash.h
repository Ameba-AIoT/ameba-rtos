/**
 * *****************************************************************************
 * @file    rtk_psa_hash.h
 * @brief   PSA transparent driver entry points for the Realtek SHA-2 engine.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef RTK_PSA_HASH_H
#define RTK_PSA_HASH_H

#include "psa/crypto.h"
#include "ameba_soc.h"

/* rtk_psa_hash_operation_t is defined in rtk_psa_hash_context.h, which
 * psa/crypto.h already pulled in via crypto_driver_contexts_primitives.h. */
#include "rtk_psa_hash_context.h"

psa_status_t rtk_transparent_hash_setup(rtk_psa_hash_operation_t *operation,
										psa_algorithm_t alg);

psa_status_t rtk_transparent_hash_clone(const rtk_psa_hash_operation_t *source_operation,
										rtk_psa_hash_operation_t *target_operation);

psa_status_t rtk_transparent_hash_update(rtk_psa_hash_operation_t *operation,
		const uint8_t *input, size_t input_length);

psa_status_t rtk_transparent_hash_finish(rtk_psa_hash_operation_t *operation,
		uint8_t *hash, size_t hash_size, size_t *hash_length);

psa_status_t rtk_transparent_hash_abort(rtk_psa_hash_operation_t *operation);

psa_status_t rtk_transparent_hash_compute(psa_algorithm_t alg,
		const uint8_t *input, size_t input_length,
		uint8_t *hash, size_t hash_size, size_t *hash_length);

#endif /* RTK_PSA_HASH_H */
