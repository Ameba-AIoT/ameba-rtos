/**
 * *****************************************************************************
 * @file    rtk_psa_entropy.c
 * @brief   MBEDTLS_PSA_DRIVER_GET_ENTROPY implementation over the RTK TRNG.
 *          4.x removed MBEDTLS_ENTROPY_HARDWARE_ALT / mbedtls_hardware_poll();
 *          the platform now supplies mbedtls_platform_get_entropy() instead.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#include "mbedtls/build_info.h"

#if defined(MBEDTLS_PSA_DRIVER_GET_ENTROPY)

#include "mbedtls/platform.h"
#include "mbedtls/private/entropy.h"
#include "psa/crypto.h"
#include "ameba_soc.h"

int mbedtls_platform_get_entropy(psa_driver_get_entropy_flags_t flags,
								 size_t *estimate_bits,
								 unsigned char *output, size_t output_size)
{
	if (flags != PSA_DRIVER_GET_ENTROPY_FLAGS_NONE) {
		return PSA_ERROR_NOT_SUPPORTED;
	}

	if (TRNG_get_random_bytes(output, output_size) != RTK_SUCCESS) {
		return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
	}

	/* The TRNG is a full-entropy source. */
	*estimate_bits = output_size * 8;

	return 0;
}

#endif /* MBEDTLS_PSA_DRIVER_GET_ENTROPY */
