/**
 * *****************************************************************************
 * @file    rtk_psa_aead_context.h
 * @brief   Driver context for the Realtek AES-GCM/CCM PSA transparent driver.
 * *****************************************************************************
 * @attention
 *
 * This header is reached from psa/crypto.h via
 * crypto_driver_contexts_composites.h, so it is included by every translation
 * unit that uses PSA. It must therefore not include any SoC header: the
 * hardware contexts are held as opaque byte buffers whose size is checked
 * against the real types with a _Static_assert in rtk_psa_aead.c.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef RTK_PSA_AEAD_CONTEXT_H
#define RTK_PSA_AEAD_CONTEXT_H

#include <stdint.h>

/* Large enough for both crypto_aes_gcm_context and crypto_aes_ccm_context. */
#define RTK_PSA_AEAD_CONTEXT_SIZE 224

typedef struct {
	uint32_t hw_ctx[RTK_PSA_AEAD_CONTEXT_SIZE / 4];
	psa_algorithm_t alg;
	size_t tag_length;
	uint8_t is_encrypt;
	uint8_t lengths_set;
} rtk_psa_aead_operation_t;

#endif /* RTK_PSA_AEAD_CONTEXT_H */
