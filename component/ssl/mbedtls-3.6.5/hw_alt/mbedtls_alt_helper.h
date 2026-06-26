/**
 * *****************************************************************************
 * @file    mbedtls_alt_helper.h
 * @author
 * @version V1.0.0
 * @date    2025-01-01
 * @brief   Shared helpers for the mbedTLS hardware alternative (ALT) layer.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef MBEDTLS_ALT_HELPER_H
#define MBEDTLS_ALT_HELPER_H

#include "mbedtls/error.h"   /* MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED          */
#include "mbedtls/gcm.h"     /* MBEDTLS_ERR_GCM_*                             */
#include "mbedtls/ccm.h"     /* MBEDTLS_ERR_CCM_*                             */

#include "ameba_crypto.h"    /* _ERRNO_CRYPTO_* HAL error codes               */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Algorithm selector for map_hw_to_mbedtls_error().
 *
 * The helper is deliberately algorithm-neutral so that future ALT modules
 * (for example signature/verification engines) can extend this enum and reuse
 * the same mapping function without renaming it.
 */
typedef enum {
	MBEDTLS_ALT_ALGO_GCM = 0,
	MBEDTLS_ALT_ALGO_CCM = 1,
} mbedtls_alt_algo_t;

/**
 * @brief Map a HAL crypto error code to the mbedTLS error domain.
 *
 * The HAL (ameba_crypto.h) and mbedTLS use independent error domains whose
 * numeric values collide, e.g.:
 *   _ERRNO_CRYPTO_KEY_LENGTH_ERR    (-15) == MBEDTLS_ERR_CCM_AUTH_FAILED (-15)
 *   _ERRNO_CRYPTO_MESSAGE_LEN_ERR   (-18) == MBEDTLS_ERR_GCM_AUTH_FAILED (-18)
 *   _ERRNO_CRYPTO_GCM_TAG_NOT_MATCH (-20) == MBEDTLS_ERR_GCM_BAD_INPUT   (-20)
 * so a raw cast between the two domains would be wrong. This function performs
 * an explicit, semantic mapping into four categories:
 *
 *   1. Hardware accelerator fault  -> MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED
 *      (algorithm-independent; the caller cannot fix it by changing inputs)
 *   2. Output buffer too small     -> GCM: MBEDTLS_ERR_GCM_BUFFER_TOO_SMALL
 *                                     CCM: MBEDTLS_ERR_CCM_BAD_INPUT (no CCM code)
 *   3. Authentication failure      -> GCM: MBEDTLS_ERR_GCM_AUTH_FAILED
 *                                     CCM: MBEDTLS_ERR_CCM_AUTH_FAILED
 *      (must stay in the GCM/CCM domain so cipher.c's auth-failure check fires)
 *   4. Caller parameter/sequencing -> GCM: MBEDTLS_ERR_GCM_BAD_INPUT
 *      error, or unknown code        CCM: MBEDTLS_ERR_CCM_BAD_INPUT
 *
 * @param hw_ret  HAL return code (_ERRNO_CRYPTO_* on error, 0 on success,
 *                or a positive raw hardware error-interrupt register value).
 * @param algo    Algorithm selector used to pick the algorithm-specific code.
 *
 * @return 0 on success, or the corresponding negative mbedTLS error code.
 */
static inline int map_hw_to_mbedtls_error(int hw_ret, mbedtls_alt_algo_t algo)
{
	if (hw_ret == 0) {
		return 0;
	}

	/* Positive values are raw hardware error-interrupt register bits. */
	if (hw_ret > 0) {
		return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
	}

	switch (hw_ret) {
	/* Category 1: hardware accelerator faults (algorithm-independent). */
	case _ERRNO_CRYPTO_KEY_LOAD_ERR:
	case _ERRNO_CRYPTO_DMA_ERR:
	case _ERRNO_CRYPTO_SLAVE_TIME_OUT:
	case _ERRNO_CRYPTO_SLAVE_ERROR:
	case _ERRNO_CRYPTO_DMA_TIMEOUT:
		return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;

	/* Category 2: output buffer too small. */
	case _ERRNO_CRYPTO_MSG_OutRange:
	case _ERRNO_CRYPTO_MESSAGE_LEN_ERR:
	case _ERRNO_CRYPTO_BUFFER_TOO_SMALL:
		switch (algo) {
		case MBEDTLS_ALT_ALGO_GCM:
			return MBEDTLS_ERR_GCM_BUFFER_TOO_SMALL;
		case MBEDTLS_ALT_ALGO_CCM:
			return MBEDTLS_ERR_CCM_BAD_INPUT;   /* CCM has no buffer-size code */
		default:
			return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
		}

	/* Category 3: authentication failure (defensive for GCM). */
	case _ERRNO_CRYPTO_GCM_TAG_NOT_MATCH:
	case _ERRNO_CRYPTO_AUTH_FAILED:
		switch (algo) {
		case MBEDTLS_ALT_ALGO_GCM:
			return MBEDTLS_ERR_GCM_AUTH_FAILED;
		case MBEDTLS_ALT_ALGO_CCM:
			return MBEDTLS_ERR_CCM_AUTH_FAILED;
		default:
			return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
		}

	/* Category 4: caller parameter or call-sequence errors, plus any
	 * unknown negative code (default). */
	case _ERRNO_CRYPTO_NULL_POINTER:
	case _ERRNO_CRYPTO_ENGINE_NOT_INIT:
	case _ERRNO_CRYPTO_KEY_OutRange:
	case _ERRNO_CRYPTO_IV_OutRange:
	case _ERRNO_CRYPTO_KEY_LENGTH_ERR:
	case _ERRNO_CRYPTO_TAG_OutRange:
	case _ERRNO_CRYPTO_AAD_LENGTH_OutRange:
	case _ERRNO_CRYPTO_MODE_ERR:
	default:
		switch (algo) {
		case MBEDTLS_ALT_ALGO_GCM:
			return MBEDTLS_ERR_GCM_BAD_INPUT;
		case MBEDTLS_ALT_ALGO_CCM:
			return MBEDTLS_ERR_CCM_BAD_INPUT;
		default:
			return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
		}
	}
}

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_ALT_HELPER_H */
