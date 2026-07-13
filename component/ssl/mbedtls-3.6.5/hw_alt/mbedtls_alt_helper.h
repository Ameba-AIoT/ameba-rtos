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
#include "mbedtls/aes.h"     /* MBEDTLS_ERR_AES_*                             */
#include "mbedtls/sha256.h"  /* MBEDTLS_ERR_SHA256_BAD_INPUT_DATA             */
#include "mbedtls/sha512.h"  /* MBEDTLS_ERR_SHA512_BAD_INPUT_DATA             */

#include "ameba_crypto.h"    /* _ERRNO_CRYPTO_* HAL error codes               */

/*
 * Compatibility fallbacks for error codes present in RTL8720F but absent in
 * amebagreen2 (which stops at _ERRNO_CRYPTO_SLAVE_ERROR = -22).
 * These values are never produced by amebagreen2 HAL calls; the corresponding
 * switch cases are therefore dead code on that platform and carry no runtime
 * overhead.  They exist solely to keep the header self-contained.
 */
#ifndef _ERRNO_CRYPTO_AAD_LENGTH_OutRange
#define _ERRNO_CRYPTO_AAD_LENGTH_OutRange   -23
#endif
#ifndef _ERRNO_CRYPTO_DMA_TIMEOUT
#define _ERRNO_CRYPTO_DMA_TIMEOUT           -24
#endif
#ifndef _ERRNO_CRYPTO_MODE_ERR
#define _ERRNO_CRYPTO_MODE_ERR              -25
#endif
#ifndef _ERRNO_CRYPTO_BUFFER_TOO_SMALL
#define _ERRNO_CRYPTO_BUFFER_TOO_SMALL      -26
#endif
#ifndef _ERRNO_CRYPTO_AUTH_FAILED
#define _ERRNO_CRYPTO_AUTH_FAILED           -27
#endif

/*
 * RTK_FAIL (-1) is used in Category 1 to cover FIH loop-integrity failures
 * returned by SHA2_Get_Final_Digest and crypto_km_set_sw_key.  It is defined
 * in component/soc/common/include/rtk_status.h which is included transitively
 * by ameba_soc.h before this header in all ALT translation units.  The
 * fallback below guards against any future include-order variation.
 */
#ifndef RTK_FAIL
#define RTK_FAIL    (-1)
#endif

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
	MBEDTLS_ALT_ALGO_GCM    = 0,
	MBEDTLS_ALT_ALGO_CCM    = 1,
	MBEDTLS_ALT_ALGO_AES    = 2,
	MBEDTLS_ALT_ALGO_SHA256 = 3,
	MBEDTLS_ALT_ALGO_SHA512 = 4,
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
 * an explicit, semantic mapping into five categories:
 *
 *   1. Hardware accelerator fault  -> MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED
 *      (algorithm-independent; the caller cannot fix it by changing inputs)
 *      Includes: KEY_LOAD_ERR, DMA_ERR, DMA_TIMEOUT, SLAVE_TIME_OUT,
 *                SLAVE_ERROR, and RTK_FAIL (-1, from FIH loop-integrity
 *                checks and internal state validation).
 *   2. Key length error            -> AES:    MBEDTLS_ERR_AES_INVALID_KEY_LENGTH
 *      (_ERRNO_CRYPTO_KEY_LENGTH_ERR)  GCM/CCM: MBEDTLS_ERR_GCM/CCM_BAD_INPUT
 *                                     SHA:    MBEDTLS_ERR_SHA256/512_BAD_INPUT_DATA
 *   3. Data/buffer length error    -> GCM:    MBEDTLS_ERR_GCM_BUFFER_TOO_SMALL
 *      (_ERRNO_CRYPTO_MSG_OutRange,    CCM:    MBEDTLS_ERR_CCM_BAD_INPUT
 *       MESSAGE_LEN_ERR,               AES:    MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH
 *       BUFFER_TOO_SMALL)              SHA256: MBEDTLS_ERR_SHA256_BAD_INPUT_DATA
 *                                     SHA512: MBEDTLS_ERR_SHA512_BAD_INPUT_DATA
 *   4. Authentication failure      -> GCM: MBEDTLS_ERR_GCM_AUTH_FAILED
 *                                     CCM: MBEDTLS_ERR_CCM_AUTH_FAILED
 *      (must stay in the GCM/CCM domain so cipher.c's auth-failure check fires;
 *       AES/SHA paths never produce these error codes)
 *   5. Caller parameter/sequencing -> GCM:    MBEDTLS_ERR_GCM_BAD_INPUT
 *      error, or unknown code         CCM:    MBEDTLS_ERR_CCM_BAD_INPUT
 *                                     AES:    MBEDTLS_ERR_AES_BAD_INPUT_DATA
 *                                     SHA256: MBEDTLS_ERR_SHA256_BAD_INPUT_DATA
 *                                     SHA512: MBEDTLS_ERR_SHA512_BAD_INPUT_DATA
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
	/* Category 1: hardware accelerator faults (algorithm-independent).
	 * RTK_FAIL (-1) is the generic failure sentinel returned by FIH
	 * loop-integrity checks (SHA2_Get_Final_Digest, crypto_km_set_sw_key),
	 * SHA2_Restore on an unknown sha_mode, and SHA2_ProcessDma on zero-length
	 * input.  All of these indicate a hardware/state problem the caller
	 * cannot recover from by adjusting parameters. */
	case RTK_FAIL:
	case _ERRNO_CRYPTO_KEY_LOAD_ERR:
	case _ERRNO_CRYPTO_DMA_ERR:
	case _ERRNO_CRYPTO_SLAVE_TIME_OUT:
	case _ERRNO_CRYPTO_SLAVE_ERROR:
	case _ERRNO_CRYPTO_DMA_TIMEOUT:
		return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;

	/* Category 2: key length invalid.
	 * AES has a dedicated mbedTLS code; for GCM/CCM the key is validated
	 * by setkey before the HAL is called, so this case is defensive only. */
	case _ERRNO_CRYPTO_KEY_LENGTH_ERR:
		switch (algo) {
		case MBEDTLS_ALT_ALGO_AES:
			return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
		case MBEDTLS_ALT_ALGO_GCM:
			return MBEDTLS_ERR_GCM_BAD_INPUT;
		case MBEDTLS_ALT_ALGO_CCM:
			return MBEDTLS_ERR_CCM_BAD_INPUT;
		case MBEDTLS_ALT_ALGO_SHA256:
			return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
		case MBEDTLS_ALT_ALGO_SHA512:
			return MBEDTLS_ERR_SHA512_BAD_INPUT_DATA;
		default:
			return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
		}

	/* Category 3: data / buffer length out of range.
	 * AES and SHA have no dedicated buffer-size code in mbedTLS. */
	case _ERRNO_CRYPTO_MSG_OutRange:
	case _ERRNO_CRYPTO_MESSAGE_LEN_ERR:
	case _ERRNO_CRYPTO_BUFFER_TOO_SMALL:
		switch (algo) {
		case MBEDTLS_ALT_ALGO_GCM:
			return MBEDTLS_ERR_GCM_BUFFER_TOO_SMALL;
		case MBEDTLS_ALT_ALGO_CCM:
			return MBEDTLS_ERR_CCM_BAD_INPUT;       /* CCM has no buffer-size code */
		case MBEDTLS_ALT_ALGO_AES:
			return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
		case MBEDTLS_ALT_ALGO_SHA256:
			return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
		case MBEDTLS_ALT_ALGO_SHA512:
			return MBEDTLS_ERR_SHA512_BAD_INPUT_DATA;
		default:
			return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
		}

	/* Category 4: authentication failure (GCM/CCM only; AES/SHA paths never
	 * produce these error codes). */
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

	/* Category 5: caller parameter or call-sequence errors, plus any
	 * unknown negative code (default). */
	case _ERRNO_CRYPTO_NULL_POINTER:
	case _ERRNO_CRYPTO_ENGINE_NOT_INIT:
	case _ERRNO_CRYPTO_KEY_OutRange:
	case _ERRNO_CRYPTO_IV_OutRange:
	case _ERRNO_CRYPTO_TAG_OutRange:
	case _ERRNO_CRYPTO_AAD_LENGTH_OutRange:
	case _ERRNO_CRYPTO_MODE_ERR:
	default:
		switch (algo) {
		case MBEDTLS_ALT_ALGO_GCM:
			return MBEDTLS_ERR_GCM_BAD_INPUT;
		case MBEDTLS_ALT_ALGO_CCM:
			return MBEDTLS_ERR_CCM_BAD_INPUT;
		case MBEDTLS_ALT_ALGO_AES:
			return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
		case MBEDTLS_ALT_ALGO_SHA256:
			return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
		case MBEDTLS_ALT_ALGO_SHA512:
			return MBEDTLS_ERR_SHA512_BAD_INPUT_DATA;
		default:
			return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
		}
	}
}

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_ALT_HELPER_H */
