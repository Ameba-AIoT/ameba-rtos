/**
 * *****************************************************************************
 * @file    rtk_psa_cipher_context.h
 * @brief   Opaque context type for the Realtek AES multipart cipher driver.
 *
 * This header is included by crypto_driver_contexts_primitives.h and therefore
 * by every translation unit that includes psa/crypto.h.  It must NOT include
 * any SoC header (ameba_aes.h, ameba_soc.h, ...) because those pull in
 * platform-specific types (__IO, u32) that break non-SoC TUs.
 *
 * The actual context struct lives in rtk_psa_cipher.c behind an opaque
 * aligned byte buffer.  A _Static_assert there verifies the size at
 * compile time.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef RTK_PSA_CIPHER_CONTEXT_H
#define RTK_PSA_CIPHER_CONTEXT_H

#include <stdint.h>
#include "psa/crypto_types.h"

/* Size of the opaque buffer.  Must be >= sizeof(rtk_psa_cipher_operation_s)
 * verified in rtk_psa_cipher.c via _Static_assert. */
#define RTK_PSA_CIPHER_CONTEXT_SIZE 96

typedef struct {
    uint8_t _opaque[RTK_PSA_CIPHER_CONTEXT_SIZE];
} rtk_psa_cipher_operation_t;

#endif /* RTK_PSA_CIPHER_CONTEXT_H */
