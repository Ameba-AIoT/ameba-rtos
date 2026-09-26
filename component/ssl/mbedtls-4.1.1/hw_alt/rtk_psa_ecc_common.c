/**
 * *****************************************************************************
 * @file    rtk_psa_ecc_common.c
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

#include "rtk_psa_ecc_common.h"

void rtk_ecc_reverse_copy(uint8_t *dst, const uint8_t *src, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++) {
		dst[i] = src[len - 1 - i];
	}
}

int rtk_ecc_curve_id(psa_key_type_t type, size_t bits, pke_ecp_curve_id *curve_id)
{
	psa_ecc_family_t family = PSA_KEY_TYPE_ECC_GET_FAMILY(type);

	if (family == PSA_ECC_FAMILY_SECP_R1) {
		switch (bits) {
		case 192: *curve_id = PKE_ECP_CURVE_SECP192R1; return 1;
		case 224: *curve_id = PKE_ECP_CURVE_SECP224R1; return 1;
		case 256: *curve_id = PKE_ECP_CURVE_SECP256R1; return 1;
		case 384: *curve_id = PKE_ECP_CURVE_SECP384R1; return 1;
		default: return 0;   /* 521-bit is not implemented by the engine. */
		}
	}

	if (family == PSA_ECC_FAMILY_SECP_K1) {
		switch (bits) {
		case 192: *curve_id = PKE_ECP_CURVE_SECP192K1; return 1;
		case 224: *curve_id = PKE_ECP_CURVE_SECP224K1; return 1;
		case 256: *curve_id = PKE_ECP_CURVE_SECP256K1; return 1;
		default: return 0;
		}
	}

	if (family == PSA_ECC_FAMILY_BRAINPOOL_P_R1) {
		switch (bits) {
		case 256: *curve_id = PKE_ECP_CURVE_BP256R1; return 1;
		case 384: *curve_id = PKE_ECP_CURVE_BP384R1; return 1;
		case 512: *curve_id = PKE_ECP_CURVE_BP512R1; return 1;
		default: return 0;
		}
	}

	return 0;
}
