/**
 * *****************************************************************************
 * @file    rtk_psa_ecc_common.h
 * @brief   Helpers shared by the PKE-backed ECDSA and ECDH drivers.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef RTK_PSA_ECC_COMMON_H
#define RTK_PSA_ECC_COMMON_H

#include "psa/crypto.h"
#include "ameba_soc.h"

/* Widest curve the engine handles is 512-bit (Brainpool P512R1). */
#define RTK_ECC_MAX_BYTES 64
/* Uncompressed public key: 0x04 || X || Y. */
#define RTK_ECC_MAX_PUBKEY_BYTES (1 + 2 * RTK_ECC_MAX_BYTES)

/* PSA hands over scalars and coordinates big-endian, the PKE engine wants
 * little-endian, so every value crossing the boundary is byte-reversed. */
void rtk_ecc_reverse_copy(uint8_t *dst, const uint8_t *src, size_t len);

/* Map the PSA curve family/size onto a PKE curve. Returns 0 when the engine
 * has no such curve, which makes the caller fall through to software. */
int rtk_ecc_curve_id(psa_key_type_t type, size_t bits, pke_ecp_curve_id *curve_id);

#endif /* RTK_PSA_ECC_COMMON_H */
