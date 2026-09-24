/**
 * *****************************************************************************
 * @file    rtk_psa_hash_context.h
 * @brief   Multipart hash context for the Realtek PSA transparent driver.
 *
 *          Reached from psa/crypto.h via crypto_driver_contexts_primitives.h,
 *          i.e. included by every TU that uses PSA. It therefore must not pull
 *          in ameba_sha.h, which needs basic_types.h and the CMSIS __IO
 *          qualifier. The engine context is kept as an opaque, suitably aligned
 *          buffer instead; rtk_psa_hash.c asserts that it really fits.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef RTK_PSA_HASH_CONTEXT_H
#define RTK_PSA_HASH_CONTEXT_H

#include <stdint.h>

/* sizeof(SHA_context) is 256 bytes; its buffer member needs 32-byte (cache
 * line) alignment for the copy-mode DMA path. */
#define RTK_PSA_SHA_CONTEXT_SIZE 256

typedef struct {
	uint8_t sha_ctx[RTK_PSA_SHA_CONTEXT_SIZE] __attribute__((aligned(32)));
	psa_algorithm_t alg;
} rtk_psa_hash_operation_t;

#endif /* RTK_PSA_HASH_CONTEXT_H */
